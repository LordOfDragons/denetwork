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
#include "state/denState.h"

class denConnection;

/**
 * \brief Connection listener.
 */
class denConnectionListener{
public:
	/** \brief Log severity. */
	enum class LogSeverity{
		error,
		warning,
		info,
		debug
	};
	
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denConnectionListener> Ref;
	
	/** \brief Create connection listener. */
	denConnectionListener();
	
	/** \brief Clean up connection listener. */
	virtual ~denConnectionListener();
	
	/** \brief Connection established. */
	virtual void ConnectionEstablished(denConnection &connection);
	
	/** \brief Connection closed. */
	virtual void ConnectionClosed(denConnection &connection);
	
	/** \brief A long message is in progress of receiving. */
	virtual void MessageProgress(denConnection &connection, size_t bytesReceived);
	
	/**
	 * \brief Message received.
	 * \param[in] message Received message. Reference can be stored for later use.
	 */
	virtual void MessageReceived(denConnection &connection, const denMessage::Ref &message);
	
	/**
	 * \brief Host send state to link.
	 * \param networkState Network state to use for this side of the link.
	 * \param message Additional information.
	 * \returns true to accept the link or false to deny it.
	 */
	virtual bool LinkState(denConnection &connection, const denState::Ref &state, const denMessage::Ref &message );
	
	/** \brief Logging. */
	virtual void Log(denConnection &connection, LogSeverity severity, const std::string &message);
};
