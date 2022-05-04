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

/**
 * \brief Socket address class.
 * 
 * Stores a computer address. The address can be currently only a IPv4
 * address but IPv6 might follow. Provides support to obtain an address
 * from an address string doing proper DNS lookup if required as well
 * as extracting the port from an address if specified. In addition
 * provides support to fill a socket address or to obtain an address
 * from a socket address..
 */
class denSocketAddress{
public:
	enum class Type{
		ipv4,
		ipv6
	};
	
	/** \brief Create empty address. */
	denSocketAddress();
	
	/** \brief Create copy of address. */
	denSocketAddress(const denSocketAddress &address);
	
	/**
	 * \brief Create IPv4 address.
	 * 
	 * The values represent the address values with 0 being the left most value.
	 */
	static denSocketAddress IPv4(uint8_t values[4], uint16_t port);
	
	/** \brief Create IPv4 any address. */
	static denSocketAddress IPv4Any();
	
	/** \brief Create IPv4 loopback address. */
	static denSocketAddress IPv4Loopback(uint16_t port = 3413);
	
	/** \brief String representation of address. */
	std::string ToString() const;
	
	/** \brief Assign address. */
	denSocketAddress &operator=(const denSocketAddress &address);
	
	/** \brief Addresses are equal. */
	bool operator==(const denSocketAddress &address) const;
	
	/** \brief Address type. */
	Type type;
	
	/** \brief Address components. */
	uint8_t values[4];
	
	/** \brief Count of components. */
	int valueCount;
	
	/** \brief Port. */
	uint16_t port;
};
