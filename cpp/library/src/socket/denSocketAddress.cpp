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

#include "denSocketAddress.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

denSocketAddress::denSocketAddress() :
type(Type::ipv4),
valueCount(0),
port(0)
{
	memset(&values, 0, sizeof(values));
}

denSocketAddress::denSocketAddress(const denSocketAddress &address) :
type(address.type),
valueCount(address.valueCount),
port(address.port)
{
	memcpy(&values, &address.values, sizeof(values));
}

denSocketAddress denSocketAddress::IPv4(uint8_t values[4], uint16_t port){
	denSocketAddress address;
	address.type = Type::ipv4;
	memcpy(&address.values, values, 4);
	address.valueCount = 4;
	address.port = port;
	return address;
}

denSocketAddress denSocketAddress::IPv6(uint8_t values[16], uint16_t port){
	denSocketAddress address;
	address.type = Type::ipv6;
	memcpy(&address.values, values, 15);
	address.valueCount = 16;
	address.port = port;
	return address;
}

denSocketAddress denSocketAddress::IPv4Any(){
	denSocketAddress address;
	address.type = Type::ipv4;
	address.valueCount = 4;
	address.port = 0;
	return address;
}

denSocketAddress denSocketAddress::IPv6Any(){
	denSocketAddress address;
	address.type = Type::ipv6;
	address.valueCount = 16;
	address.port = 0;
	return address;
}

denSocketAddress denSocketAddress::IPv4Loopback(uint16_t port){
	denSocketAddress address;
	address.type = Type::ipv4;
	address.values[0] = 127;
	address.values[1] = 0;
	address.values[2] = 0;
	address.values[3] = 1;
	address.valueCount = 4;
	address.port = port;
	return address;
}

denSocketAddress denSocketAddress::IPv6Loopback(uint16_t port){
	denSocketAddress address;
	address.type = Type::ipv6;
	address.values[15] = 1;
	address.valueCount = 16;
	address.port = port;
	return address;
}

std::string denSocketAddress::ToString() const{
	bool groupingZeros = false;
	bool canGroupZeros = true;
	std::stringstream s;
	int i;
	
	switch(type){
	case Type::ipv4:
		s << (int)values[0] << "." << (int)values[1] << "."
			<< (int)values[2] << "." << (int)values[3] << ":" << (int)port;
		break;
		
	case Type::ipv6:
		s << "[";
		
		for(i=0; i<8; i++){
			const int a = values[i * 2];
			const int b = values[i * 2 + 1];
			
			// groups of 0 can be truncated but only once
			if(!a && !b){
				if(groupingZeros){
					continue;
					
				}else if(canGroupZeros){
					s << ":";
					groupingZeros = true;
					continue;
				}
				
			}else if(groupingZeros){
				groupingZeros = false;
				canGroupZeros = false;
			}
			
			// leading zeros can be truncated
			if(i > 0){
				s << ":";
			}
			
			s << std::hex << ( int )( ( a << 8 ) | b );
		}
		
		if(groupingZeros){
			s << ":";
		}
		
		s << "]:" << std::dec << std::setw(0) << (int)port;
		break;
		
	default:
		s << "?";
	}
	
	return s.str();
}

denSocketAddress &denSocketAddress::operator=(const denSocketAddress &address){
	type = address.type;
	memcpy(&values, &address.values, sizeof(values));
	valueCount = address.valueCount;
	port = address.port;
	return *this;
}

bool denSocketAddress::operator==(const denSocketAddress &address) const{
	return type == address.type && port == address.port
		&& memcmp(&values, &address.values, sizeof(values)) == 0;
}
