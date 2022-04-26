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

#include "denSocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <memory.h>

#ifdef OS_UNIX
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/poll.h>
#elif defined OS_W32
	#include "include_windows.h"
	typedef int socklen_t;
#endif

#include "denSocket.h"

denSocket::denSocket() :
pSocket(-1)
{
	pSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if(pSocket == -1){
		throw std::invalid_argument("socket failed");
	}
}

denSocket::~denSocket(){
	if(pSocket != -1){
		close(pSocket);
	}
}

void denSocket::Bind(){
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	pAddress.SetSocketIPv4(sa);
	
	if(bind(pSocket, (struct sockaddr *)&sa, sizeof(sockaddr))){
		throw std::invalid_argument("bind failed");
	}
	
	socklen_t slen = sizeof(sockaddr);
	if(getsockname(pSocket, (struct sockaddr *)&sa, &slen)){
		throw std::invalid_argument("getsockname failed");
	}
	pAddress.SetIPv4FromSocket(sa);
}

bool denSocket::ReceiveDatagram(denMessage &message, denAddress &address){
	socklen_t slen = sizeof(sockaddr);
	struct sockaddr_in sa;
#ifdef OS_UNIX
	struct pollfd ufd;
#elif defined OS_W32
	fd_set fd;
#endif
	size_t dataLen = 0;
	
#ifdef OS_UNIX
	ufd.fd = pSocket;
	ufd.events = POLLIN;
	if(poll(&ufd, 1, 0) > 0)
#elif defined OS_W32
	FD_ZERO(&fd);
	FD_SET(pSocket, &fd);
	
	TIMEVAL tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if(select(0, &fd, nullptr, nullptr, &tv) == 1)
#endif
	{
		std::string &data = message.GetData();
		dataLen = 8192;
		if(data.size() < dataLen){
			data.assign(dataLen, 0);
		}
		
		dataLen = recvfrom(pSocket, (char*)data.c_str(), dataLen, 0, (struct sockaddr *)&sa, &slen);
		
		if(dataLen > 0){
			address.SetIPv4FromSocket(sa);
		} // connection closed returns 0 length
	}
	message.SetLength(dataLen);
	return dataLen > 0;
}

void denSocket::SendDatagram(const denMessage &message, const denAddress &address){
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
	address.SetSocketIPv4(sa);
	
	sendto(pSocket, (char*)message.GetData().c_str(), message.GetLength(), 0, (struct sockaddr *)&sa, sizeof(sockaddr));
}
