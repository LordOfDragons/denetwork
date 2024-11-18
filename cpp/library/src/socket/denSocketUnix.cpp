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

#include "denSocketUnix.h"

#if defined OS_UNIX || defined OS_BEOS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <memory.h>
#include <errno.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>

#ifdef OS_BEOS
#include <sys/sockio.h>
#endif

#include "denSocketUnix.h"
#include "../denConnection.h"
#include "../denServer.h"

denSocketUnix::denSocketUnix() :
pSocket(-1){
}

denSocketUnix::~denSocketUnix() noexcept{
	if(pSocket != -1){
		close(pSocket);
	}
}

void denSocketUnix::Bind(){
	if(pSocket != -1){
		throw std::runtime_error("socket already bound");
	}
	
	if(pAddress.type == denSocketAddress::Type::ipv6){
		pSocket = socket(PF_INET6, SOCK_DGRAM, 0);
		if(pSocket == -1){
			const int error = errno;
			std::stringstream s;
			s << "socket failed: " << strerror(error) << " (" << error << ")";
			throw std::runtime_error(s.str());
		}
		
		struct sockaddr_in6 sa;
		memset(&sa, 0, sizeof(sa));
		SocketFromAddress(pAddress, sa);
		
		sa.sin6_scope_id = pScopeIdFor(sa); // required for local links or it fails to bind
		
		if(bind(pSocket, (struct sockaddr *)&sa, sizeof(sa))){
			const int error = errno;
			std::stringstream s;
			s << "bind failed: " << strerror(error) << " (" << error << ")";
			throw std::runtime_error(s.str());
		}
		
		socklen_t slen = sizeof(sa);
		if(getsockname(pSocket, (struct sockaddr *)&sa, &slen)){
			const int error = errno;
			std::stringstream s;
			s << "getsockname failed: " << strerror(error) << " (" << error << ")";
			throw std::runtime_error(s.str());
		}
		pAddress = AddressFromSocket(sa);
		
	}else{
		pSocket = socket(PF_INET, SOCK_DGRAM, 0);
		if(pSocket == -1){
			const int error = errno;
			std::stringstream s;
			s << "socket failed: " << strerror(error) << " (" << error << ")";
			throw std::runtime_error(s.str());
		}
		
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		SocketFromAddress(pAddress, sa);
		
		if(bind(pSocket, (struct sockaddr *)&sa, sizeof(sa))){
			const int error = errno;
			std::stringstream s;
			s << "bind failed: " << strerror(error) << " (" << error << ")";
			throw std::runtime_error(s.str());
		}
		
		socklen_t slen = sizeof(sa);
		if(getsockname(pSocket, (struct sockaddr *)&sa, &slen)){
			const int error = errno;
			std::stringstream s;
			s << "getsockname failed: " << strerror(error) << " (" << error << ")";
			throw std::runtime_error(s.str());
		}
		pAddress = AddressFromSocket(sa);
	}
}

denMessage::Ref denSocketUnix::ReceiveDatagram(denSocketAddress &address){
	struct pollfd ufd;
	
	ufd.fd = pSocket;
	ufd.events = POLLIN;
	if(poll(&ufd, 1, 0) > 0){
		const denMessage::Ref message(denMessage::Pool().Get());
		std::string &data = message->Item().GetData();
		size_t dataLen = 65535;
		if(data.size() < dataLen){
			data.assign(dataLen, 0);
		}
		
		if(pAddress.type == denSocketAddress::Type::ipv6){
			struct sockaddr_in6 sa;
			socklen_t slen = sizeof(sa);
			const int result = recvfrom(pSocket, (char*)data.c_str(), dataLen, 0, (struct sockaddr *)&sa, &slen);
			
			if(result == -1){
				const int error = errno;
				std::stringstream s;
				s << "recvfrom failed: " << strerror(error) << " (" << error << ")";
				throw std::runtime_error(s.str());
			}
			
			if(result > 0){
				address = AddressFromSocket(sa);
				message->Item().SetLength(result);
				return message;
			} // connection closed returns 0 length
			
		}else{
			struct sockaddr_in sa;
			socklen_t slen = sizeof(sa);
			const int result = recvfrom(pSocket, (char*)data.c_str(), dataLen, 0, (struct sockaddr *)&sa, &slen);
			
			if(result == -1){
				const int error = errno;
				std::stringstream s;
				s << "recvfrom failed: " << strerror(error) << " (" << error << ")";
				throw std::runtime_error(s.str());
			}
			
			if(result > 0){
				address = AddressFromSocket(sa);
				message->Item().SetLength(result);
				return message;
			} // connection closed returns 0 length
		}
	}
	
	return nullptr;
}

void denSocketUnix::SendDatagram(const denMessage &message, const denSocketAddress &address){
	if(message.GetLength() > 65500){
		std::stringstream s;
		s << "SendDatagram: message size too long: " << message.GetLength() << " (max 65500)";
		throw std::runtime_error(s.str());
	}
	
	if(pAddress.type == denSocketAddress::Type::ipv6){
		struct sockaddr_in6 sa;
		memset(&sa, 0, sizeof(sa));
		SocketFromAddress(address, sa);
		
		sendto(pSocket, (char*)message.GetData().c_str(),
			message.GetLength(), 0, (struct sockaddr *)&sa, sizeof(sa));
		
	}else{
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		SocketFromAddress(address, sa);
		
		sendto(pSocket, (char*)message.GetData().c_str(),
			message.GetLength(), 0, (struct sockaddr *)&sa, sizeof(sa));
	}
}

denSocketAddress denSocketUnix::ResolveAddress(const std::string &address){
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
				throw std::invalid_argument("address invalid: missing ']'");
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
		node = address;
		
		hints.ai_family = AF_UNSPEC;
	}
	
	uint16_t port = 3413;
	
	if(portBegin != std::string::npos){
		char *end;
		port = (uint16_t)strtol(&address[portBegin], &end, 10);
		if(*end){
			throw std::invalid_argument("address invalid: port not numeric");
		}
	}
	
	addrinfo *result;
	int retcode = getaddrinfo(node.c_str(), nullptr, &hints, &result);
	if(retcode != 0){
		std::stringstream s;
		s << "address invalid: getaddrinfo: " << gai_strerror(retcode) << " (" << retcode << ")";
		throw std::runtime_error(s.str());
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
			
			const uint32_t * const sain = ((sockaddr_in6 *)rai.ai_addr)->sin6_addr.s6_addr32;
			const uint32_t sa32[4] = {ntohl(sain[0]), ntohl(sain[1]), ntohl(sain[2]), ntohl(sain[3])};
			
			int i;
			for(i=0; i<16; i+=4){
				const uint32_t &in = sa32[i/4];
				socketAddress.values[i] = (uint8_t)((in >> 24) & 0xff);
				socketAddress.values[i+1] = (uint8_t)((in >> 16) & 0xff);
				socketAddress.values[i+2] = (uint8_t)((in >> 8) & 0xff);
				socketAddress.values[i+3] = (uint8_t)(in & 0xff);
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
			throw std::invalid_argument("address invalid: scope resolve wrong family");
		}
		
		freeaddrinfo(result);
		
		return socketAddress;
		
	}catch(...){
		freeaddrinfo(result);
		throw;
	}
}

std::vector<std::string> denSocketUnix::FindPublicAddresses(){
	return pFindAddresses(true);
}

std::vector<std::string> denSocketUnix::FindAllAddresses(){
	return pFindAddresses(false);
}

denSocketAddress denSocketUnix::AddressFromSocket(const struct sockaddr_in &address) const{
	denSocketAddress socketAddress;
	socketAddress.type = denSocketAddress::Type::ipv4;
	
	const uint32_t sockAddr = ntohl(address.sin_addr.s_addr);
	socketAddress.values[0] = (uint8_t)((sockAddr >> 24) & 0xff);
	socketAddress.values[1] = (uint8_t)((sockAddr >> 16) & 0xff);
	socketAddress.values[2] = (uint8_t)((sockAddr >> 8) & 0xff);
	socketAddress.values[3] = (uint8_t)(sockAddr & 0xff);
	
	socketAddress.valueCount = 4;
	socketAddress.port = ntohs(address.sin_port);
	return socketAddress;
}

denSocketAddress denSocketUnix::AddressFromSocket(const struct sockaddr_in6 &address) const{
	denSocketAddress socketAddress;
	socketAddress.type = denSocketAddress::Type::ipv6;
	
	const uint32_t * const sain = address.sin6_addr.s6_addr32;
	const uint32_t sa32[4] = {ntohl(sain[0]), ntohl(sain[1]), ntohl(sain[2]), ntohl(sain[3])};
	
	int i;
	for(i=0; i<16; i+=4){
		const uint32_t &in = sa32[i/4];
		socketAddress.values[i] = (uint8_t)((in >> 24) & 0xff);
		socketAddress.values[i+1] = (uint8_t)((in >> 16) & 0xff);
		socketAddress.values[i+2] = (uint8_t)((in >> 8) & 0xff);
		socketAddress.values[i+3] = (uint8_t)(in & 0xff);
	}
	
	socketAddress.valueCount = 16;
	socketAddress.port = ntohs(address.sin6_port);
	return socketAddress;
}

void denSocketUnix::SocketFromAddress(const denSocketAddress &socketAddress, struct sockaddr_in &address){
	if(socketAddress.type != denSocketAddress::Type::ipv4){
		throw std::invalid_argument("type != ipv4");
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl((socketAddress.values[0] << 24)
		+ (socketAddress.values[1] << 16)
		+ (socketAddress.values[2] << 8)
		+ socketAddress.values[3]);
	address.sin_port = htons(socketAddress.port);
}

void denSocketUnix::SocketFromAddress( const denSocketAddress &socketAddress, struct sockaddr_in6 &address ){
	if(socketAddress.type != denSocketAddress::Type::ipv6){
		throw std::invalid_argument("type != ipv6");
	}
	
	int i;
	for(i=0; i<16; i+=4){
		address.sin6_addr.s6_addr32[i/4] = htonl((socketAddress.values[i] << 24)
			+ (socketAddress.values[i+1] << 16)
			+ (socketAddress.values[i+2] << 8)
			+ socketAddress.values[i+3]);
	}
	
	address.sin6_family = AF_INET6;
	address.sin6_port = htons(socketAddress.port);
}

std::vector<std::string> denSocketUnix::pFindAddresses(bool onlyPublic){
	std::vector<std::string> list;
	
	ifaddrs *ifaddr, *ifiter;
	char bufferIPv6[INET6_ADDRSTRLEN];
	char bufferIPv4[INET_ADDRSTRLEN];
	
	if(getifaddrs(&ifaddr) == -1){
		const int error = errno;
		std::stringstream s;
		s << "getifaddrs failed: " << strerror(error) << " (" << error << ")";
		throw std::runtime_error(s.str());
	}
	
	try{
		// find first IPv6 address
		for(ifiter=ifaddr; ifiter; ifiter=ifiter->ifa_next){
			if(!ifiter->ifa_addr || ifiter->ifa_addr->sa_family != AF_INET6){
				continue;
			}
			
			const in6_addr &saddr = ((sockaddr_in6 *)ifiter->ifa_addr)->sin6_addr;
			if(!inet_ntop(AF_INET6, &saddr, bufferIPv6, INET6_ADDRSTRLEN)){
				continue;
			}
			
			if(onlyPublic && (ifiter->ifa_flags & IFF_LOOPBACK) != 0){
				continue;
			}
			
			list.push_back(bufferIPv6);
		}
		
		// then find IPv4 address
		for(ifiter=ifaddr; ifiter; ifiter=ifiter->ifa_next){
			if(!ifiter->ifa_addr || ifiter->ifa_addr->sa_family != AF_INET){
				continue;
			}
			
			const in_addr &saddr = ((sockaddr_in *)ifiter->ifa_addr)->sin_addr;
			if(!inet_ntop(AF_INET, &saddr, bufferIPv4, INET_ADDRSTRLEN)){
				continue;
			}
			
			if(onlyPublic && (ifiter->ifa_flags & IFF_LOOPBACK) != 0){
				continue;
			}
			
			list.push_back(bufferIPv4);
		}
		
		freeifaddrs(ifaddr);
		
	}catch(...){
		freeifaddrs(ifaddr);
		throw;
	}
	
	return list;
}

uint32_t denSocketUnix::pScopeIdFor(const sockaddr_in6 &address){
	ifaddrs *ifaddr, *ifiter;
	
	if(getifaddrs(&ifaddr) == -1){
		const int error = errno;
		std::stringstream s;
		s << "getifaddrs failed: " << strerror(error) << " (" << error << ")";
		throw std::runtime_error(s.str());
	}
	
	uint32_t scope = 0;
	
	try{
		for(ifiter=ifaddr; ifiter; ifiter=ifiter->ifa_next){
			if(!ifiter->ifa_addr || ifiter->ifa_addr->sa_family != AF_INET6){
				continue;
			}
			
			const sockaddr_in6 &ifa = *(const sockaddr_in6 *)ifiter->ifa_addr;
			
			if(memcmp(&ifa.sin6_addr, &address.sin6_addr, sizeof(address.sin6_addr)) == 0){
				scope = ((sockaddr_in6 *)ifiter->ifa_addr)->sin6_scope_id;
				break;
			}
		}
		
		freeifaddrs(ifaddr);
		
	}catch(...){
		freeifaddrs(ifaddr);
		throw;
	}
	
	return scope;
}


#endif
