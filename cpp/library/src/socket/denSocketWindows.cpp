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

#include "denSocketWindows.h"

#ifdef OS_W32

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <memory.h>
#include <iomanip>

#include <Iphlpapi.h>

#include "../denConnection.h"
#include "../denServer.h"

denSocketWindows::denSocketWindows() :
pSocket(-1)
{
	pSocket = (int)socket(PF_INET, SOCK_DGRAM, 0);
	if(pSocket == -1){
		throw std::invalid_argument("socket failed");
	}
}

denSocketWindows::~denSocketWindows() noexcept{
	if(pSocket != -1){
		#ifdef OS_W32_VS
		closesocket(pSocket);
		#else
		close(pSocket);
		#endif
	}
}

void denSocketWindows::Bind(){
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	SocketFromAddress(pAddress, sa);
	
	if(bind(pSocket, (SOCKADDR*)&sa, sizeof(sa))){
		throw std::invalid_argument("bind failed");
	}
	
	int slen = sizeof(sa);
	if(getsockname(pSocket, (SOCKADDR*)&sa, &slen)){
		throw std::invalid_argument("getsockname failed");
	}
	pAddress = AddressFromSocket(sa);
}

denMessage::Ref denSocketWindows::ReceiveDatagram(denSocketAddress &address){
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(pSocket, &fd);
	
	TIMEVAL tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if(select(0, &fd, nullptr, nullptr, &tv) == 1){
		const denMessage::Ref message(denMessage::Pool().Get());
		std::string &data = message->Item().GetData();
		size_t dataLen = 8192;
		if(data.size() < dataLen){
			data.assign(dataLen, 0);
		}
		
		struct sockaddr_in sa;
		int slen = sizeof(sa);
		const int result = recvfrom(pSocket, (char*)data.c_str(), (int)dataLen, 0, (SOCKADDR*)&sa, &slen);
		
		if(result == SOCKET_ERROR){
			const int error = WSAGetLastError();
			std::stringstream s;
			s << "recvfrom failed: error " << error;
			throw std::runtime_error(s.str());
		}

		if(result > 0){
			address = AddressFromSocket(sa);
			message->Item().SetLength(result);
			return message;
		} // connection closed returns 0 length
	}
	
	return nullptr;
}

void denSocketWindows::SendDatagram(const denMessage &message, const denSocketAddress &address){
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
	SocketFromAddress(address, sa);
	
	sendto(pSocket, (char*)message.GetData().c_str(), (int)message.GetLength(), 0, (SOCKADDR*)&sa, sizeof(sa));
}

denSocketAddress denSocketWindows::ResolveAddress(const std::string &address){
	const std::string::size_type delimiter = address.find(':');
	struct hostent *he = nullptr;
	uint16_t port = 3413;
	
	// get address and port if present
	if(delimiter != std::string::npos){
		port = (uint16_t)strtol(&address[delimiter + 1], nullptr, 10);
		he = gethostbyname(address.substr(0, delimiter).c_str());
		
	}else{
		he = gethostbyname(address.c_str());
	}
	
	if(! he){
		throw std::invalid_argument("address");
	}
	
	// set address and port
	denSocketAddress socketAddress;
	socketAddress.type = denSocketAddress::Type::ipv4;
	
	const uint32_t sockAddr = ntohl(((struct in_addr *)he->h_addr)->s_addr);
	socketAddress.values[0] = (uint8_t)((sockAddr >> 24) & 0xff);
	socketAddress.values[1] = (uint8_t)((sockAddr >> 16) & 0xff);
	socketAddress.values[2] = (uint8_t)((sockAddr >> 8) & 0xff);
	socketAddress.values[3] = (uint8_t)(sockAddr & 0xff);
	socketAddress.valueCount = 4;
	socketAddress.port = port;
	return socketAddress;
}

denSocketAddress denSocketWindows::AddressFromSocket(const struct sockaddr_in &address) const{
	const uint32_t sockAddr = ntohl(address.sin_addr.s_addr);
	denSocketAddress socketAddress;
	socketAddress.type = denSocketAddress::Type::ipv4;
	socketAddress.values[0] = (uint8_t)((sockAddr >> 24) & 0xff);
	socketAddress.values[1] = (uint8_t)((sockAddr >> 16) & 0xff);
	socketAddress.values[2] = (uint8_t)((sockAddr >> 8) & 0xff);
	socketAddress.values[3] = (uint8_t)(sockAddr & 0xff);
	socketAddress.valueCount = 4;
	socketAddress.port = ntohs(address.sin_port);
	return socketAddress;
}

void denSocketWindows::SocketFromAddress(const denSocketAddress &socketAddress, struct sockaddr_in &address){
	if(socketAddress.type != denSocketAddress::Type::ipv4){
		throw std::invalid_argument("type != ipv4");
	}
	address.sin_addr.s_addr = htonl((socketAddress.values[0] << 24)
		+ (socketAddress.values[1] << 16)
		+ (socketAddress.values[2] << 8)
		+ socketAddress.values[3]);
	address.sin_port = htons(socketAddress.port);
}

std::vector<std::string> denSocketWindows::FindPublicAddresses(){
	std::vector<std::string> list;
	
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
	
	return list;
}

#endif
