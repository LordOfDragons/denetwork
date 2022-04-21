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
#include <vector>
#include <string>
#include "config.h"
#include "denState.h"
#include "message/denMessage.h"

/**
 * \brief Network connection.
 */
class denConnection{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denConnection> Ref;
	
	/** \brief Create server. */
	denConnection();
	
	/** \brief Clean up server. */
	~denConnection();
	
	/** \brief Local address. */
	inline const std::string &GetLocalAddress() const{ return pLocalAddress; }
	
	/** \brief Remote address. */
	inline const std::string &GetRemoteAddress() const{ return pRemoteAddress; }
	
	/** \brief Connection to a remote host is established. */
	inline bool GetConnected() const{ return pConnected; }
	
	/** \brief Connect to connection object on host at address. */
	void ConnectTo(const std::string &address);
	
	/** \brief Disconnect from remote connection if connected. */
	void Disconnect();
	
	/**
	 * \brief Send message to remote connection if connected.
	 * 
	 * The message can be queued and send at a later time to optimize throughput.
	 * The message will be not delayed longer than the given amount of milliseconds.
	 * The message is send unreliable and it is acceptable for the message to get
	 * lost due to transmission failure.
	 */
	void SendMessage(const denMessage::Ref &message, int maxDelay);
	
	/**
	 * \brief Send reliable message to remote connection if connected.
	 * 
	 * The  message is append to already waiting reliable messages and send as
	 * soon as possible. Reliable messages always arrive in the same order
	 * they have been queued.
	 */
	void SendReliableMessage(const denMessage::Ref &message);
	
	/**
	 * \brief Link network state to remote network state.
	 * 
	 * The message contains information for the remote system to know what state to link to.
	 * The request is queued and carried out as soon as possible. The local state
	 * is considered the master state and the remote state the slave state.
	 * By default only the master state can apply changes.
	 */
	void LinkState(const denMessage::Ref &message, const denState::Ref &state, bool readOnly);
	
private:
	std::string pLocalAddress;
	std::string pRemoteAddress;
	bool pConnected;
};
