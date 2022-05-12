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
#include <sstream>
#include "denServer.h"
#include "denConnection.h"
#include "denProtocolEnums.h"
#include "message/denMessageReader.h"
#include "message/denMessageWriter.h"
#include "socket/denSocketShared.h"

denServer::denServer() :
pListening(false){
}

denServer::~denServer() noexcept{
	StopListening();
}

void denServer::ListenOn(const std::string &address){
	if(pListening){
		throw std::invalid_argument("Already listening");
	}
	
	std::string useAddress(address);
	
	if(useAddress == "*"){
		const std::vector<std::string> publicAddresses(FindPublicAddresses());
		
		if(!useAddress.empty()){
			if(pLogger){
				std::vector<std::string>::const_iterator iter;
				const std::string text("Server: Found public address: ");
				for(iter = publicAddresses.cbegin(); iter != publicAddresses.cend(); iter++){
					pLogger->Log(denLogger::LogSeverity::info, text + *iter);
				}
			}
			
			useAddress = publicAddresses.front();
			
		}else{
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::info, "Server: No public address found. Using localhost");
			}
			useAddress = "127.0.0.1";
		}
	}
	
	pSocket = CreateSocket();
	pSocket->SetAddress(ResolveAddress(useAddress));
	pSocket->Bind();
	
	if(pLogger){
		std::stringstream s;
		s << "Server: Listening on " << pSocket->GetAddress().ToString();
		pLogger->Log(denLogger::LogSeverity::info, s.str());
	}
	
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
			denSocketAddress addressReceive;
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
	// 				if(denLogger){
	// 					denLogger->Log(denLogger::LogSeverity::warning, "Server: Invalid datagram: Sender does not match any connection!" );
	// 				}
				}
			}
			
		}catch(const std::exception &e){
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::error, std::string("Server: Update[1]: ") + e.what());
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
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::error, std::string("Server: Update[2]: ") + e.what());
			}
		}
	}
}

denConnection::Ref denServer::CreateConnection(){
	return std::make_shared<denConnection>();
}

denSocket::Ref denServer::CreateSocket(){
	return denSocketShared::CreateSocket();
}

denSocketAddress denServer::ResolveAddress(const std::string &address){
	return denSocketShared::ResolveAddress(address);
}

std::vector<std::string> denServer::FindPublicAddresses(){
	return denSocketShared::FindPublicAddresses();
}

void denServer::SetLogger(const denLogger::Ref &logger){
	pLogger = logger;
}

void denServer::ClientConnected(const denConnection::Ref &){
}

void denServer::ProcessConnectionRequest(const denSocketAddress &address, denMessageReader &reader){
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
	connection->AcceptConnection(*this, pSocket, address, protocol);
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
	
	if(pLogger){
		std::stringstream s;
		s << "Server: Client connected from " << address.ToString();
		pLogger->Log(denLogger::LogSeverity::info, s.str());
	}
	ClientConnected(connection);
}
