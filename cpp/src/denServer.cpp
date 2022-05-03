/**
 * MIT License
 * 
 * Copyright (c) 2022 DragonDreams (info@dragondreams.ch)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdexcept>
#include <vector>
#include <algorithm>
#include "denServer.h"
#include "denConnection.h"
#include "message/denMessageReader.h"
#include "message/denMessageWriter.h"
#include "internal/denProtocolEnums.h"

#ifdef OS_UNIX
#include <string.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifdef OS_BEOS
#include <sys/sockio.h>
#endif

#ifdef OS_W32
#	include <dragengine/app/include_windows.h>
#	include <iphlpapi.h>
#endif

denServer::denServer() :
pUserData(nullptr),
pListening(false){
}

denServer::~denServer(){
	StopListening();
}

void denServer::SetUserData(void *userData){
	pUserData = userData;
}

void denServer::ListenOn(const std::string &address){
	if(pListening){
		throw std::invalid_argument("Already listening");
	}
	
	std::string useAddress(address);
	
	if(useAddress == "*"){
		const std::vector<std::string> publicAddresses(FindPublicAddresses());
		
		if(!useAddress.empty()){
			if(pListener){
				std::vector<std::string>::const_iterator iter;
				const std::string text("Found public address: ");
				for(iter = publicAddresses.cbegin(); iter != publicAddresses.cend(); iter++){
					pListener->Log(*this, denServerListener::LogSeverity::info, text + *iter);
				}
			}
			
			useAddress = publicAddresses.front();
			
		}else{
			if(pListener){
				pListener->Log(*this, denServerListener::LogSeverity::info, "No public address found. Using localhost");
			}
			useAddress = "127.0.0.1";
		}
	}
	
	if(pListener){
		pListener->Log(*this, denServerListener::LogSeverity::info, std::string("Listening on ") + useAddress);
	}
	
	pSocket = std::make_shared<denSocket>();
	pSocket->GetAddress().SetIPv4FromString(useAddress);
	pSocket->Bind();
	
	pListening = true;
}

void denServer::StopListening(){
	if(!pListening){
		throw std::invalid_argument("Not listening");
	}
	
	pSocket.reset();
	pListening = false;
}

void denServer::Update(float elapsedTime){
	if(!pSocket){
		return;
	}
	
	// receive messages
	while(true){
		denConnection *connection = nullptr;
		
		try{
			denAddress addressReceive;
			const denMessage::Ref message(pSocket->ReceiveDatagram(addressReceive));
			if(!message){
				break;
			}
			
			denMessageReader reader(message->Item());
			
			const Connections::const_iterator iter(std::find_if(pConnections.begin(),
				pConnections.end(), [&](const denConnection::Ref &each){
					return each->Matches(pSocket.get(), addressReceive);
				}));
			
			if(iter != pConnections.cend()){
				connection = iter->get();
				connection->ProcessDatagram(reader);
				
			}else{
				const denProtocol::CommandCodes command = (denProtocol::CommandCodes)reader.ReadByte();
				if(command == denProtocol::CommandCodes::connectionRequest){
					ProcessConnectionRequest(addressReceive, reader);
					
				}else{
					// ignore invalid package
	// 				if(pListener){
	// 					pListener->Log(denServerListener::LogSeverity::warning, "Invalid datagram: Sender does not match any connection!\n" );
	// 				}
				}
			}
			
		}catch(const std::exception &e){
			if(pListener){
				pListener->Log(*this, denServerListener::LogSeverity::error,
					std::string("denServer::Update[1]: ") + e.what());
			}
		}
	}
	
	// update connections
	Connections::const_iterator iter(pConnections.cbegin());
	while(iter != pConnections.cend()){
		denConnection &connection = **(iter++);
		try{
			connection.Update(elapsedTime);
			
		}catch(const std::exception &e){
			if(pListener){
				pListener->Log(*this, denServerListener::LogSeverity::error,
					std::string("denServer::Update[2]: ") + e.what());
			}
		}
	}
}

denConnection::Ref denServer::CreateConnection(){
	return std::make_shared<denConnection>();
}

void denServer::SetListener(const denServerListener::Ref &listener){
	pListener = listener;
}

std::vector<std::string> denServer::FindPublicAddresses(){
	std::vector<std::string> list;
	
	// unix version
	#ifdef OS_UNIX
	const int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1){
		throw std::invalid_argument("Failed creating socket");
	}
	
	try{
		struct ifreq ifr;
		int ifindex = 1;
		memset(&ifr, 0, sizeof(ifr));
		char bufferIP[17];
		
		while(true){
			#ifdef OS_BEOS
			ifr.ifr_index = ifindex++;
			#else
			ifr.ifr_ifindex = ifindex++;
			#endif
			if(ioctl(sock, SIOCGIFNAME, &ifr)){
				break;
			}
			
			if(ioctl(sock, SIOCGIFADDR, &ifr)){
				continue; // something failed, ignore the interface
			}
			const struct in_addr saddr = ((struct sockaddr_in &)ifr.ifr_addr).sin_addr;
			
			if(!inet_ntop(AF_INET, &saddr, bufferIP, 16)){
				continue;
			}
			
			if(strcmp(bufferIP, "127.0.0.1") == 0){
				continue; // ignore localhost
			}
			
			list.push_back(bufferIP);
			// ifr.ifr_name  => device name
		}
		close(sock);
		
	}catch(...){
		close(sock);
		throw;
	}
	#endif
	
	// beos version
	#ifdef OS_BEOS
	(void)list;
	throw std::invalid_argument("TODO Implement this using BeOS means");
	#endif
	
	// windows version
	#ifdef OS_W32
	// get size and allocate buffer
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)HeapAlloc(GetProcessHeap(), 0, sizeof(IP_ADAPTER_INFO));
	if(!pAdapterInfo){
		throw std::invalid_argument("HeapAlloc returned nullptr");
	}
	
	try{
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW){
			HeapFree(GetProcessHeap(), 0, pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO*)HeapAlloc(GetProcessHeap(), 0, ulOutBufLen);
			if(!pAdapterInfo){
				throw std::invalid_argument("HeapAlloc returned nullptr");
			}
		}
		
		if(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != NO_ERROR){
			throw std::invalid_argument("GetAdaptersInfo failed");
		}
		
		// evaluate
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		while(pAdapter){
			// NOTE: IP_ADDR_STRING is a linked list and can potentially contain more than one address
			
			const unsigned long ulAddr = inet_addr(pAdapter->IpAddressList.IpAddress.String);
			if(ulAddr == INADDR_NONE || ulAddr == 0 /*0.0.0.0*/){
				pAdapter = pAdapter->Next;
				continue;
			}
			
			const unsigned long ulNetMask = inet_addr(pAdapter->IpAddressList.IpMask.String);
			if(ulNetMask == INADDR_NONE || ulNetMask == 0 /*0.0.0.0*/){
				pAdapter = pAdapter->Next;
				continue;
			}
			
			if(strcmp(pAdapter->IpAddressList.IpAddress.String, "127.0.0.1") == 0){
				pAdapter = pAdapter->Next;
				continue; // ignore localhost
			}
			
			list.push_back(pAdapter->IpAddressList.IpAddress.String);
			// pAdapter->AdapterName  => device name
			// (uint32_t)ulAddr   => address in in_addr format
			// (uint32_t)ulNetMask   => netmask in in_addr format
			
			pAdapter = pAdapter->Next;
		}
		
		HeapFree(GetProcessHeap(), 0, pAdapterInfo);
		
	}catch(...){
		if(pAdapterInfo){
			HeapFree(GetProcessHeap(), 0, pAdapterInfo);
		}
		throw;
	}
	#endif
	
	return list;
}

void denServer::ProcessConnectionRequest(const denAddress &address, denMessageReader &reader){
	if(! pListening){
		const denMessage::Ref message(denMessage::Pool().Get());
		{
		denMessageWriter writer(message->Item());
		writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionAck);
		writer.WriteByte((uint8_t)denProtocol::ConnectionAck::rejected);
		}
		pSocket->SendDatagram(message->Item(), address);
		return;
	}
	
	// find best protocol to speak
	std::vector<int> clientProtocols;
	const int clientProtocolCount = reader.ReadUShort();
	int i;
	for(i=0; i<clientProtocolCount; i++){
		clientProtocols.push_back(reader.ReadUShort());
	}
	
	if(std::find(clientProtocols.cbegin(), clientProtocols.cend(),
	(int)denProtocol::Protocols::DENetworkProtocol) == clientProtocols.cend()){
		const denMessage::Ref message(denMessage::Pool().Get());
		{
		denMessageWriter writer(message->Item());
		writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionAck);
		writer.WriteByte((uint8_t)denProtocol::ConnectionAck::noCommonProtocol);
		}
		pSocket->SendDatagram(message->Item(), address);
		return;
	}
	
	denProtocol::Protocols protocol = denProtocol::Protocols::DENetworkProtocol;
	
	// create connection 
	const denConnection::Ref connection(CreateConnection());
	connection->AcceptConnection(pSocket, address, protocol);
	pConnections.push_back(connection);
	
	// send back result
	const denMessage::Ref message(denMessage::Pool().Get());
	{
	denMessageWriter writer(message->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionAck);
	writer.WriteByte((uint8_t)denProtocol::ConnectionAck::accepted);
	writer.WriteUShort((uint16_t)protocol);
	}
	pSocket->SendDatagram(message->Item(), address);
	
	if(pListener){
		pListener->ClientConnected(*this, connection);
	}
}
