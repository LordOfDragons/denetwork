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
#include "denConnection.h"
#include "denLogger.h"
#include "socket/denSocket.h"

class denMessageReader;

/**
 * \brief Network server.
 * 
 * Allows clients speaking Drag[en]gine Network Protocol to connect.
 * 
 * To use this class create a subclass and overwrite CreateConnection() and ClientConnected().
 * The method CreateConnection() method creates a denConnection instance for each connecting
 * client. By overwriting this method you can create a subclass of denConnection handling the
 * client. Overwriting ClientConnected() allows to communicate with a connecting client to
 * link states and exchanging messages.
 * 
 * The default implementation connects by creating a UDP socket. If you have to accept clients
 * using a different transportation method overwrite CreateSocket() to create a denSocket
 * subclass providing the required capabilities. Usually this is not required.
 * 
 * To start listening call ListenOn with the IP address to listen on in the format
 * "hostnameOrIP" or "hostnameOrIP:port". You can use a resolvable hostname or an IPv4.
 * If the port is not specified the default port 3413 is used. You can use any port you you like.
 * 
 * Call Update() in regular intervals to receive and process incoming messages as well as
 * updating connected clients. DENetwork does not use internal threading giving you full
 * control over threading.
 * 
 * To get logging implemnent a subclass of denLogger and set the logger instance using
 * SetLogger(). You can share the logger instance across multiple servers and connections.
 */
class denServer{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denServer> Ref;
	
	/** \brief Connection list. */
	typedef std::list<denConnection::Ref> Connections;
	
	/** \brief Create server. */
	denServer();
	
	/** \brief Clean up server. */
	virtual ~denServer() noexcept;
	
	/** \brief Address. */
	inline const std::string &GetAddress() const{ return pAddress; }
	
	/** \brief Server is listening for connections. */
	inline bool IsListening() const{ return pListening; }
	
	/**
	 * \brief Start listening on address for incoming connections.
	 * 
	 * \param[in] address Address is in the format "hostnameOrIP" or "hostnameOrIP:port".
	 *                    You can use a resolvable hostname or an IPv4. If the port is not
	 *                    specified the default port 3413 is used. You can use any port
	 *                    you you like.
	 */
	void ListenOn(const std::string &address);
	
	/** \brief Stop listening. */
	void StopListening();
	
	/** \brier Connections. */
	inline const Connections &GetConnections() const{ return pConnections; }
	
	/**
	 * \brief Update server.
	 * 
	 * Send and received queued messages. Call this on each frame update or in a loop
	 * from inside a thread. If using a thread use a mutex to ensure thread safety.
	 * 
	 * \param[in] elapsedTime Elapsed time in seconds since the last call to Update().
	 */
	void Update(float elapsedTime);
	
	/**
	 * \brief Create connection for each connecting client.
	 * 
	 * Overwrite this method to create an instance of a custom subclass of denConnection
	 * handling the client.
	 *  
	 * Default implementation creates instance of denConnection.
	 */
	virtual denConnection::Ref CreateConnection();
	
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
	
	/** \brief Find public addresses. */
	virtual std::vector<std::string> FindPublicAddresses();
	
	/** \brief Find all addresses. */
	virtual std::vector<std::string> FindAllAddresses();
	
	/** \brief Logger or nullptr. */
	inline const denLogger::Ref &GetLogger() const{ return pLogger; }
	
	/** \brief Set logger or nullptr to clear. */
	void SetLogger(const denLogger::Ref &logger);
	
	/**
	 * \brief Client connected.
	 * 
	 * Overwrite to communicate with a connecting client to link states and exchange messages.
	 */
	virtual void ClientConnected(const denConnection::Ref &connection);
	
	
private:
	std::string pAddress;
	
	denSocket::Ref pSocket;
	bool pListening;
	
	Connections pConnections;
	
	denLogger::Ref pLogger;
	
	friend denConnection;
	void ProcessConnectionRequest(const denSocketAddress &address, denMessageReader &reader);
};
