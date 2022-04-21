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

#include "denAddress.h"
#include "../message/denMessage.h"

/**
 * \brief Socket.
 */
class denSocket{
public:
	denSocket();
	
	~denSocket();
	
	inline const denAddress &GetAddress() const{ return pAddress; }
	
	/** \brief Bind socket to stored address. */
	void Bind();
	
	/**
	 * \brief Receive datagram from socket.
	 * \returns true if a message has been receives or false otherwise.
	 */
	bool ReceiveDatagram(denMessage &message, denAddress &address);
	
	/**
	 * \brief Send datagram.
	 */
	void SendDatagram(const denMessage &message, const denAddress &address);
	
private:
	denAddress pAddress;
	int pSocket;
};
