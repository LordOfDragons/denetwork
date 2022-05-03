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

#pragma once

#include <stdint.h>
#include <string>
#include "../config.h"

#ifdef OS_UNIX
	#include <arpa/inet.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#elif defined OS_W32
	#include "include_windows.h"
#endif

/**
 * \brief Address class.
 * 
 * Stores a computer address. The address can be currently only a IPv4
 * address but IPv6 might follow. Provides support to obtain an address
 * from an address string doing proper DNS lookup if required as well
 * as extracting the port from an address if specified. In addition
 * provides support to fill a socket address or to obtain an address
 * from a socket address..
 */
class denAddress{
public:
	enum class Type{
		ipv4,
		ipv6
	};
	
	denAddress();
	denAddress(const denAddress &address);
	
	/**
	 * \brief Set address to an IPv4 address.
	 * 
	 * The values represent the address values with 0 being the left most value.
	 */
	void SetIPv4(uint8_t values[4], uint16_t port);
	
	/** \brief Set address to an IPv4 any address. */
	void SetIPv4Any();
	
	/** \brief Set address to an IPv4 loopback address. */
	void SetIPv4Loopback();
	
	/**
	 * \brief Set address to an IPv4 address using the provided string.
	 * 
	 * The string can contain a valid IPv4 address or a valid domain name with
	 * an optional port. If specified the port follows the address separated
	 * by a semicolon.
	 */
	void SetIPv4FromString(const std::string &address);
	
	/** \brief Set address to an IPv4 address from a socket address. */
	void SetIPv4FromSocket(const struct sockaddr_in &address);
	
	/** \brief Set address in a socket address. */
	void SetSocketIPv4(struct sockaddr_in &address) const;
	
	std::string ToString() const;
	
	denAddress &operator=(const denAddress &address);
	bool operator==(const denAddress &address) const;
	
	Type type;
	uint8_t values[4];
	int valueCount;
	uint16_t port;
};
