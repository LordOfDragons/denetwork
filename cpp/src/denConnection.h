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
#include <queue>
#include <string>
#include <list>
#include "config.h"
#include "denConnectionListener.h"
#include "message/denMessage.h"
#include "state/denState.h"
#include "state/denStateLink.h"
#include "internal/denAddress.h"
#include "internal/denSocket.h"
#include "internal/denProtocolEnums.h"
#include "internal/denRealMessage.h"

class denMessageReader;
class denServer;


/**
 * \brief Network connection.
 */
class denConnection{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denConnection> Ref;
	
	/** \brief State link list. */
	typedef std::list<denStateLink::Ref> StateLinks;
	
	/** \brief Modified state link list. */
	typedef std::list<denStateLink*> ModifiedStateLinks;
	
	/** \brief Link list. */
	typedef std::deque<denRealMessage::Ref> Messages;
	
	/** \brief State of the connection. */
	enum class ConnectionState{
		disconnected,
		connecting,
		connected
	};
	
	/** \brief Create connection. */
	denConnection();
	
	/** \brief Local address. */
	inline const std::string &GetLocalAddress() const{ return pLocalAddress; }
	
	/** \brief Remote address. */
	inline const std::string &GetRemoteAddress() const{ return pRemoteAddress; }
	
	/** \brief User data. */
	inline void *GetUserData() const{ return pUserData; }
	
	/** \brief Set user data. */
	void SetUserData(void *userData);
	
	/** \brief Connection to a remote host is established. */
	bool GetConnected() const;
	
	/** \brief Set connection listener or nullptr to clear. */
	void SetListener(const denConnectionListener::Ref &listener);
	
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
	
	/**
	 * \brief Update connection.
	 * 
	 * Send and received queued messages. Call this on each frame update or in a loop
	 * from inside a thread. If using a thread use a mutex to ensure thread safety.
	 * 
	 * \param[in] elapsedTime Elapsed time in seconds since the last call to Update();
	 */
	void Update(float elapsedTime);
	
	
	
	/** \warning Internal use. Do not call directly. */
	inline const denSocket::Ref &GetSocket() const{ return pSocket; }
	inline int GetIdentifier() const{ return pIdentifier; }
	void SetIdentifier(int identifier);
	inline denProtocol::Protocols GetProtocol() const{ return pProtocol; }
	
	void AddModifiedStateLink(denStateLink *link);
	void InvalidateState(denState &state);
	bool Matches(denSocket *bnSocket, const denAddress &address) const;
	void AcceptConnection(const denSocket::Ref &bnSocket,
		const denAddress &address, denProtocol::Protocols protocol);
	void ProcessDatagram(denMessageReader &reader);
	void ProcessConnectionAck(denMessageReader &reader);
	void ProcessConnectionClose(denMessageReader &reader);
	void ProcessMessage(denMessageReader &reader);
	void ProcessReliableMessage(denMessageReader &reader);
	void ProcessReliableLinkState(denMessageReader &reader);
	void ProcessReliableAck(denMessageReader &reader);
	void ProcessLinkUp(denMessageReader &reader);
	void ProcessLinkDown(denMessageReader &reader);
	void ProcessLinkUpdate(denMessageReader &reader);
	
private:
	std::string pLocalAddress;
	std::string pRemoteAddress;
	void *pUserData;
	
	denSocket::Ref pSocket;
	denAddress pRealRemoteAddress;
	ConnectionState pConnectionState;
	int pIdentifier;
	
	denProtocol::Protocols pProtocol;
	StateLinks pStateLinks;
	ModifiedStateLinks pModifiedStateLinks;
	int pNextLinkIdentifier;
	
	Messages pReliableMessagesSend;
	Messages pReliableMessagesRecv;
	int pReliableNumberSend;
	int pReliableNumberRecv;
	int pReliableWindowSize;
	
	denConnectionListener::Ref pListener;
	
	void pDisconnect();
	void pUpdateStates();
	void pUpdateTimeouts(float elapsedTime);
	void pProcessQueuedMessages();
	void pProcessReliableMessage(denMessageReader &reader);
	void pProcessLinkState(denMessageReader &reader);
	void pAddReliableReceive(denProtocol::CommandCodes type, int number, denMessageReader &reader);
	void pRemoveSendReliablesDone();
	void pSendPendingReliables();
	
	friend denServer;
	denServer *pParentServer;
};
