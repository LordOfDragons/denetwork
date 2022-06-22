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
	address.values[0] = values[0];
	address.values[1] = values[1];
	address.values[2] = values[2];
	address.values[3] = values[3];
	address.valueCount = 4;
	address.port = port;
	return address;
}

denSocketAddress denSocketAddress::IPv4Any(){
	denSocketAddress address;
	address.type = Type::ipv4;
	memset(&address.values, 0, sizeof(address.values));
	address.valueCount = 4;
	address.port = 0;
	return address;
}

denSocketAddress denSocketAddress::IPv4Loopback(uint16_t port){
	denSocketAddress address;
	address.type = Type::ipv4;
	memset(&address.values, 0, sizeof(address.values));
	address.values[0] = 127;
	address.values[3] = 1;
	address.valueCount = 4;
	address.port = port;
	return address;
}

std::string denSocketAddress::ToString() const{
	if(type == Type::ipv4){
		std::stringstream s;
		s << (int)values[0] << "." << (int)values[1] << "." << (int)values[2] << "." << (int)values[3] << ":" << (int)port;
		return s.str();
		
	}else{
		return "?";
	}
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
