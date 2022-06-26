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
#include "denLogger.h"
#include "denProtocolEnums.h"
#include "denRealMessage.h"
#include "message/denMessage.h"
#include "state/denState.h"
#include "state/denStateLink.h"
#include "socket/denSocketAddress.h"
#include "socket/denSocket.h"

class denMessageReader;
class denServer;


/**
 * \brief Network connection.
 * 
 * Allows clients to connect to a server speaking Drag[en]gine Network Protocol.
 * 
 * To use this class create a subclass overwriting one or more of the methods below.
 * 
 * To start connecting to the server call ConnectTo() with the IP address to connect to
 * in the format "hostnameOrIP" or "hostnameOrIP:port". You can use a resolvable hostname
 * or an IPv4. If the port is not specified the default port 3413 is used. You can use any
 * port you you like. Connecting attempt fails if it takes longer than SetConnectTimeout
 * seconds. The default timeout is 3 seconds.
 * 
 * If connecting to the server succeedes ConnectionEstablished() is called. Overwrite to
 * request linking states and exchanging messages with the server. If connection timed
 * out or another error occured ConnectionFailed() is called with the failure reason.
 * Overwrite to handle connection failure.
 * 
 * You can close the connection by calling Disconnect(). This calls ConnectionClosed()
 * which you can overwrite. This method is also called if the server closes the connection.
 * 
 * Overwrite CreateState() to create states requested by the server. States synchronize
 * a fixed set of values between the server and the client. The client can have read-write
 * or read-only access to the state. Create an instance of a subclass of denState to
 * handle individual states. It is not necessary to create a subclass of denState if
 * you intent to subclass denValue* instead.
 * 
 * Overwrite MessageReceived() to process messages send by the server.
 * 
 * Call Update() in regular intervals to receive and process incoming messages as well as
 * updating states. DENetwork does not use internal threading giving you full control
 * over threading.
 * 
 * To get logging implemnent a subclass of denLogger and set the logger instance using
 * SetLogger(). You can share the logger instance across multiple servers and connections.
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
	
	/** \brief Connection failed reason. */
	enum class ConnectionFailedReason{
		generic,
		timeout,
		rejected,
		noCommonProtocol,
		invalidMessage
	};
	
	/** \brief Create connection. */
	denConnection();
	
	/** \brief Destroy connection. */
	virtual ~denConnection() noexcept;
	
	/** \brief Local address. */
	inline const std::string &GetLocalAddress() const{ return pLocalAddress; }
	
	/** \brief Remote address. */
	inline const std::string &GetRemoteAddress() const{ return pRemoteAddress; }
	
	/** \brief Timeout in seconds for ConnectTo call. */
	inline float GetConnectTimeout() const{ return pConnectTimeout; }
	
	/** \brief Set timeout in seconds for ConnectTo call. */
	void SetConnectTimeout( float timeout );
	
	/** \brief Connection state. */
	inline ConnectionState GetConnectionState() const{ return pConnectionState; }
	
	/** \brief Connection to a remote host is established. */
	inline bool GetConnected() const{ return pConnectionState == ConnectionState::connected; }
	
	/**
	 * \brief Seconds since ConnectTo() call.
	 * 
	 * Only valid while connection state is connecting. Connection attempts fails
	 * if seconds since connect to exceeds connect timeout.
	 */
	inline float GetSecondsSinceConnectTo() const{ return pSecondsSinceConnectTo; }
	
	/** \brief Logger or null. */
	inline const denLogger::Ref &GetLogger() const{ return pLogger; }
	
	/** \brief Set logger or nullptr to clear. */
	void SetLogger(const denLogger::Ref &logger);
	
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
	 * 
	 * Sending messages is not reliable. Messages can be potentially lost and you
	 * will not be notified if this occurs. Use this method for messages where loosing
	 * them is fine. This is typically the case for messages repeating in regular
	 * intervals so missing one of them is not a problem.
	 * 
	 * \param[in] message Message to send. Message can contain any kind of byte sequence.
	 *                    The most simply way to build messages is using denMessageWriter.
	 */
	void SendMessage(const denMessage::Ref &message);
	
	/**
	 * \brief Send reliable message to remote connection if connected.
	 * 
	 * The  message is append to already waiting reliable messages and send as
	 * soon as possible. Reliable messages always arrive in the same order
	 * they have been queued.
	 * 
	 * This messages is guaranteed to be delivered in the order they have been send.
	 * Use this for messages which you can not afford to loose. This is typically the
	 * case for events happening once like a player activating an item or opening
	 * a door.
	 * 
	 * \param[in] message Message to send. Message can contain any kind of byte sequence.
	 *                    The most simply way to build messages is using denMessageWriter.
	 */
	void SendReliableMessage(const denMessage::Ref &message);
	
	/**
	 * \brief Link network state to remote network state.
	 * 
	 * The message contains information for the remote system to know what state to link to.
	 * The request is queued and carried out as soon as possible. The local state
	 * is considered the master state and the remote state the slave state.
	 * By default only the master state can apply changes.
	 * 
	 * \param[in] message Message to send. Message can contain any kind of byte sequence.
	 *                    The most simply way to build messages is using denMessageWriter.
	 * \param[in] state State to link.
	 * \param[in] readOnly If true client receives a read-only link otherwise a read-write
	 *                     link. Use true if the state to link is a server managed state
	 *                     the client is only allowed to read. Use false if this is a state
	 *                     the client has to change.
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
	
	/**
	 * \brief Create socket.
	 * 
	 * Default implementation creates platform specific socket implementation which is a
	 * UDP socket. If you have to accept clients using a different transportation method
	 * overwrite method to create an instance of a subclass of denSocket providing the
	 * required capabilities.
	 */
	virtual denSocket::Ref CreateSocket();
	
	/**
	 * \brief Resolve address.
	 * 
	 * Address is in the format "hostnameOrIP" or "hostnameOrIP:port". You can use a
	 * resolvable hostname or an IPv4. If the port is not specified the default port
	 * 3413 is used.
	 * 
	 * If you overwrite CreateSocket() you have to also overwrite this method to resolve
	 * address using the appropriate method.
	 */
	virtual denSocketAddress ResolveAddress(const std::string &address);
	
	/** \brief Connection established. */
	virtual void ConnectionEstablished();
	
	/** \brief Connection failed or timeout out. */
	virtual void ConnectionFailed(ConnectionFailedReason reason);
	
	/**
	 * \brief Connection closed.
	 * 
	 * This is called if Disconnect() is called or the server closes the connection.
	 */
	virtual void ConnectionClosed();
	
	/** \brief Long message is in progress of receiving. */
	virtual void MessageProgress(size_t bytesReceived);
	
	/**
	 * \brief Message received.
	 * \param[in] message Received message. Reference can be stored for later use.
	 */
	virtual void MessageReceived(const denMessage::Ref &message);
	
	/**
	 * \brief Host send state to link.
	 * 
	 * Overwrite to create states requested by the server. States synchronize a fixed set
	 * of values between the server and the client. The client can have read-write or
	 * read-only access to the state. Create an instance of a subclass of denState to handle
	 * individual states. It is not necessary to create a subclass of denState if you intent
	 * to subclass denValue* instead.
	 * 
	 * If you do not support a state requested by the server you can return nullptr.
	 * In this case the state is not linked and state values are not synchronized.
	 * You can not re-link a state later on if you rejected it here. If you need re-linking
	 * a state make the server resend the link request. This will be a new state link.
	 * 
	 * \returns State or nullptr to reject.
	 */
	virtual denState::Ref CreateState(const denMessage::Ref &message, bool readOnly);
	
	/**
	 * \brief Server owning this connection or nullptr if this is a client side connection.
	 */
	inline denServer *GetParentServer() const{ return pParentServer; }
	
	/**
	 * \brief Socket or nullptr if not connected.
	 */
	inline const denSocket::Ref &GetSocket() const{ return pSocket; }
	
	/**
	 * \brief Connection protocol.
	 */
	inline denProtocol::Protocols GetProtocol() const{ return pProtocol; }
	
	/**
	 * \brief Connection matches socket and address.
	 */
	bool Matches(denSocket *asocket, const denSocketAddress &address) const;
	
	/**
	 * \brief Process datagram.
	 */
	void ProcessDatagram(denMessageReader &reader);
	
	
	
private:
	std::string pLocalAddress;
	std::string pRemoteAddress;
	
	denSocket::Ref pSocket;
	denSocketAddress pRealRemoteAddress;
	ConnectionState pConnectionState;
	float pConnectTimeout;
	float pSecondsSinceConnectTo;
	
	denProtocol::Protocols pProtocol;
	StateLinks pStateLinks;
	ModifiedStateLinks pModifiedStateLinks;
	int pNextLinkIdentifier;
	
	Messages pReliableMessagesSend;
	Messages pReliableMessagesRecv;
	int pReliableNumberSend;
	int pReliableNumberRecv;
	int pReliableWindowSize;
	
	denLogger::Ref pLogger;
	
	friend class denStateLink;
	
	void AcceptConnection(denServer &server, const denSocket::Ref &asocket,
		const denSocketAddress &address, denProtocol::Protocols protocol);
	
	void pDisconnect(bool notify);
	void pClearStates();
	void pCloseSocket();
	void pRemoveConnectionFromParentServer();
	void pUpdateStates();
	void pUpdateTimeouts(float elapsedTime);
	void pInvalidateState(const denState::Ref &state);
	void pAddModifiedStateLink(denStateLink *link);
	void pProcessQueuedMessages();
	void pProcessConnectionAck(denMessageReader &reader);
	void pProcessConnectionClose(denMessageReader &reader);
	void pProcessMessage(denMessageReader &reader);
	void pProcessReliableMessage(denMessageReader &reader);
	void pProcessReliableMessageMessage(denMessageReader &reader);
	void pProcessReliableAck(denMessageReader &reader);
	void pProcessReliableLinkState(denMessageReader &reader);
	void pProcessLinkUp(denMessageReader &reader);
	void pProcessLinkDown(denMessageReader &reader);
	void pProcessLinkState(denMessageReader &reader);
	void pProcessLinkUpdate(denMessageReader &reader);
	void pAddReliableReceive(denProtocol::CommandCodes type, int number, denMessageReader &reader);
	void pRemoveSendReliablesDone();
	void pSendPendingReliables();
	
	friend denServer;
	denServer *pParentServer;
};
