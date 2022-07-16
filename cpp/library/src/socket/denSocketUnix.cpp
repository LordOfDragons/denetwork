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

#ifdef OS_BEOS
#include <sys/sockio.h>
#endif

#include "denSocketUnix.h"
#include "../denConnection.h"
#include "../denServer.h"

denSocketUnix::denSocketUnix() :
pSocket(-1)
{
	pSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if(pSocket == -1){
		throw std::invalid_argument("socket failed");
	}
}

denSocketUnix::~denSocketUnix() noexcept{
	if(pSocket != -1){
		close(pSocket);
	}
}

void denSocketUnix::Bind(){
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	SocketFromAddress(pAddress, sa);
	
	if(bind(pSocket, (struct sockaddr *)&sa, sizeof(sockaddr))){
		throw std::invalid_argument("bind failed");
	}
	
	socklen_t slen = sizeof(sockaddr);
	if(getsockname(pSocket, (struct sockaddr *)&sa, &slen)){
		throw std::invalid_argument("getsockname failed");
	}
	pAddress = AddressFromSocket(sa);
}

denMessage::Ref denSocketUnix::ReceiveDatagram(denSocketAddress &address){
	socklen_t slen = sizeof(sockaddr);
	struct sockaddr_in sa;
	struct pollfd ufd;
	
	ufd.fd = pSocket;
	ufd.events = POLLIN;
	if(poll(&ufd, 1, 0) > 0){
		const denMessage::Ref message(denMessage::Pool().Get());
		std::string &data = message->Item().GetData();
		size_t dataLen = 8192;
		if(data.size() < dataLen){
			data.assign(dataLen, 0);
		}
		
		const int result = recvfrom(pSocket, (char*)data.c_str(), dataLen, 0, (struct sockaddr *)&sa, &slen);
		
		if(result == -1){
			const int error = errno;
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

void denSocketUnix::SendDatagram(const denMessage &message, const denSocketAddress &address){
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
	SocketFromAddress(address, sa);
	
	sendto(pSocket, (char*)message.GetData().c_str(), message.GetLength(), 0, (struct sockaddr *)&sa, sizeof(sockaddr));
}

denSocketAddress denSocketUnix::ResolveAddress(const std::string &address){
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

denSocketAddress denSocketUnix::AddressFromSocket(const struct sockaddr_in &address) const{
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

void denSocketUnix::SocketFromAddress(const denSocketAddress &socketAddress, struct sockaddr_in &address){
	if(socketAddress.type != denSocketAddress::Type::ipv4){
		throw std::invalid_argument("type != ipv4");
	}
	address.sin_addr.s_addr = htonl((socketAddress.values[0] << 24)
		+ (socketAddress.values[1] << 16)
		+ (socketAddress.values[2] << 8)
		+ socketAddress.values[3]);
	address.sin_port = htons(socketAddress.port);
}

std::vector<std::string> denSocketUnix::FindPublicAddresses(){
	std::vector<std::string> list;
	
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
	
	return list;
}

#endif
