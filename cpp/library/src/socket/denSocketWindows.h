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

#include "../config.h"

#ifdef OS_W32

#include <WinSock2.h>
#include <ws2ipdef.h>

#include <Windows.h>

#include "denSocket.h"

/**
 * \brief Socket on windows platform.
 */
class denSocketWindows : public denSocket{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denSocketWindows> Ref;
	
	/** \brief Create socket. */
	denSocketWindows();
	
	/** \brief Clean up socket. */
	virtual ~denSocketWindows() noexcept;
	
	/** \brief Bind socket to stored address. */
	virtual void Bind();
	
	/** \brief Receive datagram from socket. */
	virtual denMessage::Ref ReceiveDatagram(denSocketAddress &address);
	
	/** \brief Send datagram. */
	virtual void SendDatagram(const denMessage &message, const denSocketAddress &address);
	
	/** \brief Resolve IPv4 address */
	static denSocketAddress ResolveAddress(const std::string &address);
	
	/** \brief Find public addresses. */
	static std::vector<std::string> FindPublicAddresses();
	
protected:
	denSocketAddress AddressFromSocket(const sockaddr_in &address) const;
	denSocketAddress AddressFromSocket(const sockaddr_in6 &address) const;
	static void SocketFromAddress(const denSocketAddress &socketAddress, sockaddr_in &address);
	static void SocketFromAddress(const denSocketAddress &socketAddress, sockaddr_in6 &address);
	
private:
	static uint32_t pScopeIdFor(const sockaddr_in6 &address);
	static void pThrowErrno(const char *message);
	static void pThrowWSAError(const char *message);
	static void pWSAStartup();
	static void pWSACleanup();
	
private:
	SOCKET pSocket;
	bool pWSAStarted;

	static int pWSAStartedCount;
};

#endif
