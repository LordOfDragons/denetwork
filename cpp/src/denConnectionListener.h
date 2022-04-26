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

#include "message/denMessage.h"

class denConnection;

/**
 * \brief Connection listener.
 */
class denConnectionListener{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denConnectionListener> Ref;
	
	/**
	 * \brief Create connection listener.
	 */
	denConnectionListener();
	
	/**
	 * \brief Clean up connection listener.
	 */
	virtual ~denConnectionListener();
	
	/**
	 * \brief Connection closed.
	 */
	virtual void ConnectionClosed(denConnection &connection) = 0;
	
	/**
	 * \brief Message received.
	 * \param[in] message Received message. Reference can be stored for later use.
	 */
	virtual void MessageReceived(const denMessage::Ref &message) = 0;
};
