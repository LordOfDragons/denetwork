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

#include <memory>
#include "denSocketAddress.h"
#include "../message/denMessage.h"

/**
 * \brief Socket.
 */
class denSocket{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denSocket> Ref;
	
protected:
	/** \brief Create socket. */
	denSocket();
	
public:
	/** \brief Clean up socket. */
	virtual ~denSocket() noexcept;
	
	/** \brief Get socket address. */
	inline const denSocketAddress &GetAddress() const{ return pAddress; }
	
	/** \brief Set socket address. */
	void SetAddress(const denSocketAddress &address);
	
	/** \brief Bind socket to stored address. */
	virtual void Bind() = 0;
	
	/** \brief Receive datagram from socket. */
	virtual denMessage::Ref ReceiveDatagram(denSocketAddress &address) = 0;
	
	/** \brief Send datagram. */
	virtual void SendDatagram(const denMessage &message, const denSocketAddress &address) = 0;
	
protected:
	denSocketAddress pAddress;
};
