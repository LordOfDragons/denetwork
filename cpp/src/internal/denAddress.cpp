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

#include "denAddress.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <stdexcept>

#ifdef OS_UNIX
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#elif defined OS_W32
	typedef unsigned int uint32_t;
#endif

denAddress::denAddress() :
type(Type::ipv4),
valueCount(0),
port(0)
{
	memset(&values, 0, sizeof(values));
}

denAddress::denAddress(const denAddress &address) :
type(address.type),
valueCount(address.valueCount),
port(address.port)
{
	memcpy(&values, &address.values, sizeof(values));
}

denAddress::~denAddress(){
}

void denAddress::SetIPv4(uint8_t nvalues[4], uint16_t nport){
	int i;
	type = Type::ipv4;
	for(i=0; i<4; i++){
		values[i] = nvalues[i];
	}
	valueCount = 4;
	port = nport;
}

void denAddress::SetIPv4Any(){
	type = Type::ipv4;
	memset(&values, 0, sizeof(values));
	valueCount = 4;
	port = 0;
}

void denAddress::SetIPv4Loopback(){
	type = Type::ipv4;
	memset(&values, 0, sizeof(values));
	values[0] = 127;
	values[3] = 1;
	valueCount = 4;
	port = 3413;
}

void denAddress::SetIPv4FromString(const std::string &address){
	const std::string::size_type delimiter = address.find(':');
	struct hostent *he = nullptr;
	uint16_t nport = 3413;
	
	// get address and port if present
	if(delimiter != std::string::npos){
		nport = (uint16_t)strtol(&address[delimiter + 1], nullptr, 10);
		he = gethostbyname(address.substr(0, delimiter).c_str());
		
	}else{
		he = gethostbyname(address.c_str());
	}
	
	if(! he){
		throw std::invalid_argument("address");
	}
	
	// set address and port
	type = Type::ipv4;
	
	const uint32_t sockAddr = ntohl(((struct in_addr *)he->h_addr)->s_addr);
	memset(&values, 0, sizeof(values));
	values[0] = (uint8_t)((sockAddr >> 24) & 0xff);
	values[1] = (uint8_t)((sockAddr >> 16) & 0xff);
	values[2] = (uint8_t)((sockAddr >> 8) & 0xff);
	values[3] = (uint8_t)(sockAddr & 0xff);
	valueCount = 4;
	port = nport;
}

void denAddress::SetIPv4FromSocket(const struct sockaddr_in &address){
	const uint32_t sockAddr = ntohl(address.sin_addr.s_addr);
	type = Type::ipv4;
	memset(&values, 0, sizeof(values));
	values[0] = (uint8_t)((sockAddr >> 24) & 0xff);
	values[1] = (uint8_t)((sockAddr >> 16) & 0xff);
	values[2] = (uint8_t)((sockAddr >> 8) & 0xff);
	values[3] = (uint8_t)(sockAddr & 0xff);
	valueCount = 4;
	port = ntohs(address.sin_port);
}

void denAddress::SetSocketIPv4(struct sockaddr_in &address) const{
	if(type != Type::ipv4){
		throw std::invalid_argument("type != ipv4");
	}
	address.sin_addr.s_addr = htonl((values[0] << 24) + (values[1] << 16) + (values[2] << 8) + values[3]);
	address.sin_port = htons(port);
}

std::string denAddress::ToString() const{
	if(type == Type::ipv4){
		std::stringstream s;
		s << values[0] << "." << values[1] << "." << values[2] << "." << values[3] << ":" << port;
		return s.str();
		
	}else{
		return "?";
	}
}

denAddress &denAddress::operator=(const denAddress &address){
	type = address.type;
	memcpy(&values, &address.values, sizeof(values));
	valueCount = address.valueCount;
	port = address.port;
	return *this;
}

bool denAddress::operator==(const denAddress &address) const{
	return type == address.type && port == address.port && memcmp(&values, &address.values, sizeof(values)) == 0;
}
