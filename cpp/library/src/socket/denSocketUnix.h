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

#include "../config.h"

#if defined OS_UNIX || defined OS_BEOS

#include "denSocket.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * \brief Socket.
 */
class denSocketUnix : public denSocket{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denSocketUnix> Ref;
	
	/** \brief Create socket. */
	denSocketUnix();
	
	/** \brief Clean up socket. */
	virtual ~denSocketUnix() noexcept;
	
	/** \brief Bind socket to stored address. */
	virtual void Bind();
	
	/** \brief Receive datagram from socket. */
	virtual denMessage::Ref ReceiveDatagram(denSocketAddress &address);
	
	/** \brief Send datagram. */
	virtual void SendDatagram(const denMessage &message, const denSocketAddress &address);
	
	/** \brief Resolve address */
	static denSocketAddress ResolveAddress(const std::string &address);
	
	/** \brief Find public addresses. */
	static std::vector<std::string> FindPublicAddresses();
	
protected:
	denSocketAddress AddressFromSocket(const struct sockaddr_in &address) const;
	static void SocketFromAddress(const denSocketAddress &socketAddress, struct sockaddr_in &address);
	
private:
	int pSocket;
};

#endif
