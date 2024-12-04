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
#include <WS2tcpip.h>

#include "../denConnection.h"
#include "../denServer.h"

denSocketWindows::denSocketWindows() :
pSocket(-1),
pWSAStarted(false),
pBufferLen(65535)
{
	pWSAStartup();
	pWSAStarted = true;
	pBuffer.assign(pBufferLen, 0);
}

denSocketWindows::~denSocketWindows() noexcept{
	if(pSocket != -1){
		#ifdef OS_W32_VS
		closesocket(pSocket);
		#else
		close(pSocket);
		#endif
	}

	if(pWSAStarted){
		pWSACleanup();
	}
}

void denSocketWindows::Bind(){
	if(pSocket != -1){
		throw std::runtime_error("socket already bound");
	}
	
	if(pAddress.type == denSocketAddress::Type::ipv6){
		pSocket = socket(PF_INET6, SOCK_DGRAM, 0);
		if(pSocket == -1){
			pThrowWSAError("socket failed");
		}
		
		sockaddr_in6 sa;
		memset(&sa, 0, sizeof(sa));
		SocketFromAddress(pAddress, sa);
		
		sa.sin6_scope_id = pScopeIdFor(sa); // required for local links or it fails to bind
		
		if(bind(pSocket, (SOCKADDR*)&sa, sizeof(sa))){
			pThrowWSAError("bind failed");
		}
		
		int slen = sizeof(sa);
		if(getsockname(pSocket, (SOCKADDR*)&sa, &slen)){
			pThrowWSAError("getsockname failed");
		}
		pAddress = AddressFromSocket(sa);

	}else{
		pSocket = (int)socket(PF_INET, SOCK_DGRAM, 0);
		if(pSocket == -1){
			pThrowWSAError("socket failed");
		}

		sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		SocketFromAddress(pAddress, sa);
	
		if(bind(pSocket, (SOCKADDR*)&sa, sizeof(sa))){
			pThrowWSAError("bind failed");
		}
	
		int slen = sizeof(sa);
		if(getsockname(pSocket, (SOCKADDR*)&sa, &slen)){
			pThrowWSAError("getsockname failed");
		}
		pAddress = AddressFromSocket(sa);
	}
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
		
		if(pAddress.type == denSocketAddress::Type::ipv6){
			sockaddr_in6 sa;
			int slen = sizeof(sa);
			const int result = recvfrom(pSocket, (char*)pBuffer.c_str(), pBufferLen, 0, (SOCKADDR*)&sa, &slen);
		
			if(result == SOCKET_ERROR){
				pThrowWSAError("recvfrom failed");
			}
			
			if(result > 0){
				address = AddressFromSocket(sa);
				if(data.size() < (std::size_t)result){
					data.assign(result, 0);
				}
				pBuffer.copy((char*)data.c_str(), result, 0);
				message->Item().SetLength(result);
				return message;
			} // connection closed returns 0 length
			
		}else{
			sockaddr_in sa;
			int slen = sizeof(sa);
			const int result = recvfrom(pSocket, (char*)pBuffer.c_str(), pBufferLen, 0, (SOCKADDR*)&sa, &slen);
		
			if(result == SOCKET_ERROR){
				pThrowWSAError("recvfrom failed");
			}
			
			if(result > 0){
				address = AddressFromSocket(sa);
				if(data.size() < (std::size_t)result){
					data.assign(result, 0);
				}
				pBuffer.copy((char*)data.c_str(), result, 0);
				message->Item().SetLength(result);
				return message;
			} // connection closed returns 0 length
		}
	}
	
	return nullptr;
}

void denSocketWindows::SendDatagram(const denMessage &message, const denSocketAddress &address){
	if(message.GetLength() > 65500){
		const int error = errno;
		std::stringstream s;
		s << "SendDatagram: message size too long: " << message.GetLength() << " (max 65500)";
		throw std::runtime_error(s.str());
	}
	
	if(pAddress.type == denSocketAddress::Type::ipv6){
		sockaddr_in6 sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin6_family = AF_INET6;
		SocketFromAddress(address, sa);
		
		sendto(pSocket, (char*)message.GetData().c_str(), (int)message.GetLength(), 0, (SOCKADDR*)&sa, sizeof(sa));

	}else{
		sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		SocketFromAddress(address, sa);
	
		sendto(pSocket, (char*)message.GetData().c_str(), (int)message.GetLength(), 0, (SOCKADDR*)&sa, sizeof(sa));
	}
}

denSocketAddress denSocketWindows::ResolveAddress(const std::string &address){
	if(address.empty()){
		throw std::invalid_argument("address is empty");
	}
	
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
	
	const std::string::size_type delimiter = address.rfind(':');
	std::string::size_type portBegin = std::string::npos;
	std::string node;
	
	if(delimiter != std::string::npos){
		if(address[0] == '['){
			// "[IPv6]:port"
			if(address[delimiter - 1] != ']'){
				throw std::invalid_argument("address invalid");
			}
			
			node = address.substr(1, delimiter - 2);
			portBegin = delimiter + 1;
			
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
			
		}else if(address.find(':') != delimiter){
			// "IPv6"
			node = address;
			
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
			
		}else{
			// "IPv4:port" or "hostname:port"
			node = address.substr(0, delimiter);
			portBegin = delimiter + 1;
			
			hints.ai_family = AF_UNSPEC;
		}
		
	}else{
		// "IPv4" or "hostname"
		node = address.substr(0, delimiter);
		
		hints.ai_family = AF_UNSPEC;
	}
	
	uint16_t port = 3413;
	
	if(portBegin != std::string::npos){
		char *end;
		port = (uint16_t)strtol(&address[portBegin], &end, 10);
		if(*end){
			throw std::invalid_argument("address invalid");
		}
	}
	
	pWSAStartup();

	addrinfo *result = NULL;
	if(getaddrinfo(node.c_str(), NULL, &hints, &result)){
		pThrowWSAError("getaddrinfo");
	}

	try{
		// there can be more than one address but we use the first one. using AI_ADDRCONFIG
		// should give us IPv6 if the host system has an IPv6 address otherwise IPv4.
		// should this be a problem we have to do this differently.
		// 
		// according to documentation the first returned address should be used since the
		// lookup function has internal sorting logic returning the preferred address first
		const addrinfo &rai = result[0];
		
		// set address and port
		denSocketAddress socketAddress;
		socketAddress.port = port;
		
		if(rai.ai_family == AF_INET6){
			socketAddress.type = denSocketAddress::Type::ipv6;
			
			const USHORT * const sain = ((sockaddr_in6*)rai.ai_addr)->sin6_addr.u.Word;
			const uint16_t sa16[8] = {ntohs(sain[0]), ntohs(sain[1]), ntohs(sain[2]), ntohs(sain[3]),
				ntohs(sain[4]), ntohs(sain[5]), ntohs(sain[6]), ntohs(sain[7])};
			
			int i;
			for(i=0; i<16; i+=2){
				const uint16_t &in = sa16[i/2];
				socketAddress.values[i] = (uint8_t)((in >> 8) & 0xff);
				socketAddress.values[i+1] = (uint8_t)(in & 0xff);
			}
			
			socketAddress.valueCount = 16;
			
		}else if(rai.ai_family == AF_INET){
			socketAddress.type = denSocketAddress::Type::ipv4;
			
			const uint32_t sockAddr = ntohl(((sockaddr_in *)rai.ai_addr)->sin_addr.s_addr);
			socketAddress.values[0] = (uint8_t)((sockAddr >> 24) & 0xff);
			socketAddress.values[1] = (uint8_t)((sockAddr >> 16) & 0xff);
			socketAddress.values[2] = (uint8_t)((sockAddr >> 8) & 0xff);
			socketAddress.values[3] = (uint8_t)(sockAddr & 0xff);
			socketAddress.valueCount = 4;
			
		}else{
			throw std::invalid_argument("address invalid");
		}
		
		freeaddrinfo(result);
		
		return socketAddress;
		
	}catch(...){
		freeaddrinfo(result);
		throw;
	}
}

std::vector<std::string> denSocketWindows::FindPublicAddresses(){
	return pFindAddresses(true);
}

std::vector<std::string> denSocketWindows::FindAllAddresses(){
	return pFindAddresses(false);
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

denSocketAddress denSocketWindows::AddressFromSocket(const sockaddr_in6 &address) const{
	denSocketAddress socketAddress;
	socketAddress.type = denSocketAddress::Type::ipv6;
	
	const USHORT * const sain = address.sin6_addr.u.Word;
	const uint16_t sa16[8] = {ntohs(sain[0]), ntohs(sain[1]), ntohs(sain[2]), ntohs(sain[3]),
		ntohs(sain[4]), ntohs(sain[5]), ntohs(sain[6]), ntohs(sain[7])};
			
	int i;
	for(i=0; i<16; i+=2){
		const uint16_t &in = sa16[i/2];
		socketAddress.values[i] = (uint8_t)((in >> 8) & 0xff);
		socketAddress.values[i+1] = (uint8_t)(in & 0xff);
	}
	
	socketAddress.valueCount = 16;
	socketAddress.port = ntohs(address.sin6_port);
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

void denSocketWindows::SocketFromAddress(const denSocketAddress &socketAddress, sockaddr_in6 &address){
	if(socketAddress.type != denSocketAddress::Type::ipv6){
		throw std::invalid_argument("type != ipv6");
	}
	
	int i;
	for(i=0; i<16; i+=2){
		address.sin6_addr.u.Word[i/2] = htons((socketAddress.values[i] << 8) + socketAddress.values[i+1]);
	}
	
	address.sin6_family = AF_INET6;
	address.sin6_port = htons(socketAddress.port);
}

std::vector<std::string> denSocketWindows::pFindAddresses(bool onlyPublic){
	pWSAStartup();
	try{
		std::vector<std::string> list;
	
		// get size and allocate buffer
		PIP_ADAPTER_ADDRESSES addresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, 15000);
		if (addresses == NULL) {
			throw std::invalid_argument("HeapAlloc returned nullptr");
		}
	
		ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST
			| GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_PREFIX;

		// get the data. this stupid way to do it is actually recommended by microsoft
		ULONG outBufLen = 15000;
		ULONG iterations = 0;
		DWORD dwRetVal = NO_ERROR;

		do{
			addresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, outBufLen);
			if(addresses == NULL){
				throw std::invalid_argument("HeapAlloc failed");
			}

			dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, addresses, &outBufLen);
			if(dwRetVal == ERROR_BUFFER_OVERFLOW){
				HeapFree(GetProcessHeap(), 0, addresses);
				addresses = NULL;

			}else{
				break;
			}

			iterations++;
		}while(dwRetVal == ERROR_BUFFER_OVERFLOW && iterations < 3);

		if(dwRetVal != NO_ERROR){
			HeapFree(GetProcessHeap(), 0, addresses);
			throw std::invalid_argument("GetAdaptersAddresses failed");
		}

		// evaluate the result
		try{
			// find IPv6 address
			PIP_ADAPTER_ADDRESSES curAddress = addresses;
			while(curAddress){
				PIP_ADAPTER_UNICAST_ADDRESS ua = curAddress->FirstUnicastAddress;
				while(ua){
					if(ua->Address.lpSockaddr->sa_family != AF_INET6){
						ua = ua->Next;
						continue;
					}
					
					sockaddr_in6 * const si = (sockaddr_in6*)(ua->Address.lpSockaddr);
					char ip[INET6_ADDRSTRLEN];
					memset(ip, 0, sizeof(ip));
					
					if(!inet_ntop(AF_INET6, &si->sin6_addr, ip, sizeof(ip))){
						ua = ua->Next;
						continue;
					}
					
					if(onlyPublic && strcmp(ip, "::1") == 0){
						ua = ua->Next;
						continue;
					}
					
					list.push_back(ip);
					ua = ua->Next;
				}
			
				curAddress = curAddress->Next;
			}

			// find IPv8 address
			curAddress = addresses;
			while(curAddress){
				PIP_ADAPTER_UNICAST_ADDRESS ua = curAddress->FirstUnicastAddress;
				while(ua){
					if(ua->Address.lpSockaddr->sa_family != AF_INET){
						ua = ua->Next;
						continue;
					}
					
					sockaddr_in * const si = (sockaddr_in*)(ua->Address.lpSockaddr);
					char ip[INET_ADDRSTRLEN];
					memset(ip, 0, sizeof(ip));
					
					if(!inet_ntop(AF_INET, &si->sin_addr, ip, sizeof(ip))){
						ua = ua->Next;
						continue;
					}
					
					if(onlyPublic && strcmp(ip, "127.0.0.1") == 0){
						ua = ua->Next;
						continue;
					}
					
					list.push_back(ip);
					ua = ua->Next;
				}
			
				curAddress = curAddress->Next;
			}
		
			HeapFree(GetProcessHeap(), 0, addresses);
		
		}catch(...){
			HeapFree(GetProcessHeap(), 0, addresses);
			throw;
		}
	
		return list;

	}catch(...){
		pWSACleanup();
		throw;
	}
}

uint32_t denSocketWindows::pScopeIdFor(const sockaddr_in6& address){
	// get size and allocate buffer
	PIP_ADAPTER_ADDRESSES addresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, 15000);
	if (addresses == NULL) {
		throw std::invalid_argument("HeapAlloc returned nullptr");
	}
	
	ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST
		| GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_INCLUDE_PREFIX;
	
	// get the data. this stupid way to do it is actually recommended by microsoft
	ULONG outBufLen = 15000;
	ULONG iterations = 0;
	DWORD dwRetVal = NO_ERROR;
	
	do{
		addresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, outBufLen);
		if(addresses == NULL){
			throw std::invalid_argument("HeapAlloc failed");
		}
		
		dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, addresses, &outBufLen);
		if(dwRetVal == ERROR_BUFFER_OVERFLOW){
			HeapFree(GetProcessHeap(), 0, addresses);
			addresses = NULL;
			
		}else{
			break;
		}
		
		iterations++;
	}while(dwRetVal == ERROR_BUFFER_OVERFLOW && iterations < 3);
	
	if(dwRetVal != NO_ERROR){
		HeapFree(GetProcessHeap(), 0, addresses);
		throw std::invalid_argument("GetAdaptersAddresses failed");
	}
	
	// evaluate the result
	uint32_t scope = 0;
	bool found = false;
	
	try{
		PIP_ADAPTER_ADDRESSES curAddress = addresses;
		while(curAddress && !found){
			PIP_ADAPTER_UNICAST_ADDRESS ua = curAddress->FirstUnicastAddress;
			while(ua && !found){
				if(ua->Address.lpSockaddr->sa_family == AF_INET6){
					const sockaddr_in6 * const si = (sockaddr_in6*)(ua->Address.lpSockaddr);
					
					if(memcmp(&si->sin6_addr, &address.sin6_addr, sizeof(address.sin6_addr)) == 0){
						scope = si->sin6_scope_id;
						found = true;
						break;
					}
				}
				ua = ua->Next;
			}
			
			curAddress = curAddress->Next;
		}
		
		HeapFree(GetProcessHeap(), 0, addresses);
		
	}catch(...){
		HeapFree(GetProcessHeap(), 0, addresses);
		throw;
	}
	
	return scope;
}

void denSocketWindows::pThrowErrno(const char *message){
	const int error = errno;
	std::stringstream s;
	char errbuf[100];
	memset(errbuf, 0, sizeof(errbuf));
	strerror_s(errbuf, sizeof(errbuf), errno);
	s << message << ": " << errbuf << " (" << error << ")";
	throw std::runtime_error(s.str());
}

void denSocketWindows::pThrowWSAError(const char *message){
	const int error = WSAGetLastError();
	std::stringstream s;
	char *errbuf = NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errbuf, 0, NULL);
	s << message << ": " << errbuf << " (" << error << ")";
	LocalFree(errbuf);
	throw std::runtime_error(s.str());
}

void denSocketWindows::pWSAStartup(){
	pWSAStartedCount++;

	if(pWSAStartedCount++ != 1){
		return;
	}

	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)){
		throw std::invalid_argument("WSAStartup failed");
	}

	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){
		pWSACleanup();
		throw std::invalid_argument("WSAStartup succeeded but returned unsupported version");
	}
}

void denSocketWindows::pWSACleanup(){
	if(pWSAStartedCount == 0){
		return;
	}

	pWSAStartedCount--;

	if(pWSAStartedCount == 0){
		WSACleanup();
	}
}

int denSocketWindows::pWSAStartedCount = 0;

#endif
