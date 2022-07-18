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

#include <algorithm>
#include <stdexcept>
#include "denConnection.h"
#include "denServer.h"
#include "denRealMessage.h"
#include "message/denMessage.h"
#include "message/denMessageReader.h"
#include "message/denMessageWriter.h"
#include "socket/denSocketShared.h"

denConnection::denConnection() :
pConnectionState(ConnectionState::disconnected),
pConnectResendInterval( 1.0f ),
pConnectTimeout( 5.0f ),
pReliableResendInterval( 0.5f ),
pReliableTimeout( 3.0f ),
pElapsedConnectResend( 0.0f ),
pElapsedConnectTimeout( 0.0f ),
pProtocol(denProtocol::Protocols::DENetworkProtocol),
pNextLinkIdentifier(0),
pReliableNumberSend(0),
pReliableNumberRecv(0),
pReliableWindowSize(10),
pParentServer(nullptr){
}

denConnection::~denConnection() noexcept{
	pParentServer = nullptr; // required to avoid potential segfault in denServer
	pDisconnect(false, false);
}

void denConnection::SetConnectResendInterval(float interval){
	pConnectResendInterval = std::max(interval, 0.01f);
}

void denConnection::SetConnectTimeout(float timeout){
	pConnectTimeout = std::max(timeout, 0.01f);
}

void denConnection::SetReliableResendInterval(float interval){
	pReliableResendInterval = std::max(interval, 0.01f);
}

void denConnection::SetReliableTimeout(float timeout){
	pReliableTimeout = std::max(timeout, 0.01f);
}

void denConnection::SetLogger(const denLogger::Ref &logger){
	pLogger = logger;
}

void denConnection::ConnectTo(const std::string &address){
	if(pSocket || pConnectionState != ConnectionState::disconnected){
		throw std::invalid_argument("already connected");
	}
	
	const denSocketAddress realRemoteAddress(ResolveAddress(address));
	
	pSocket = CreateSocket();
	
	if(realRemoteAddress.type == denSocketAddress::Type::ipv6){
		pSocket->SetAddress(denSocketAddress::IPv6Any());
		
	}else{
		pSocket->SetAddress(denSocketAddress::IPv4Any());
	}
	
	pSocket->Bind();
	
	pLocalAddress = pSocket->GetAddress().ToString();
	
	const denMessage::Ref connectRequest(denMessage::Pool().Get());
	{
	denMessageWriter writer(connectRequest->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionRequest);
	writer.WriteUShort( 1 ); // version
	writer.WriteUShort((uint8_t)denProtocol::Protocols::DENetworkProtocol);
	}
	pRealRemoteAddress = realRemoteAddress;
	pRemoteAddress = address;
	
	if(pLogger){
		std::stringstream s;
		s << "Connection: Connecting to " << pRealRemoteAddress.ToString();
		pLogger->Log(denLogger::LogSeverity::info, s.str());
	}
	
	pSocket->SendDatagram(connectRequest->Item(), pRealRemoteAddress);
	
	pConnectionState = ConnectionState::connecting;
	pElapsedConnectResend = 0.0f;
	pElapsedConnectTimeout = 0.0f;
}

void denConnection::Disconnect(){
	pDisconnect(true, false);
}

void denConnection::SendMessage(const denMessage::Ref &message){
	if(!message){
		throw std::invalid_argument("message is nullptr");
	}
	if(message->Item().GetLength() < 1){
		throw std::invalid_argument("message has 0 length");
	}
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("not connected");
	}
	
	// send message
	const denMessage::Ref unrealMessage(denMessage::Pool().Get());
	{
	denMessageWriter writer(unrealMessage->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::message);
	writer.Write(message->Item());
	}
	
	pSocket->SendDatagram(unrealMessage->Item(), pRealRemoteAddress);
}

void denConnection::SendReliableMessage(const denMessage::Ref &message){
	if(!message){
		throw std::invalid_argument("message is nullptr");
	}
	if(message->Item().GetLength() < 1){
		throw std::invalid_argument("message has 0 length");
	}
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("not connected");
	}
	
	const denRealMessage::Ref realMessage(denRealMessage::Pool().Get());
	realMessage->Item().type = denProtocol::CommandCodes::reliableMessage;
	realMessage->Item().number = (pReliableNumberSend + pReliableMessagesSend.size()) % 65535;
	realMessage->Item().state = denRealMessage::State::pending;
	
	{
	denMessageWriter writer(realMessage->Item().message->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::reliableMessage);
	writer.WriteUShort((uint16_t)realMessage->Item().number);
	writer.Write(message->Item());
	}
	
	pReliableMessagesSend.push_back(realMessage);
	
	// if the message fits into the window send it right now
	if(pReliableMessagesSend.size() <= (size_t)pReliableWindowSize){
		pSocket->SendDatagram(realMessage->Item().message->Item(), pRealRemoteAddress);
		
		realMessage->Item().state = denRealMessage::State::send;
		realMessage->Item().elapsedResend = 0.0f;
		realMessage->Item().elapsedTimeout = 0.0f;
	}
}

void denConnection::LinkState(const denMessage::Ref &message, const denState::Ref &state, bool readOnly){
	if(!message){
		throw std::invalid_argument("message is nullptr");
	}
	if(message->Item().GetLength() < 1){
		throw std::invalid_argument("message has 0 length");
	}
	if(message->Item().GetLength() > 0xffff){
		throw std::invalid_argument("message too long");
	}
	if(!state){
		throw std::invalid_argument("state is nullptr");
	}
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("not connected");
	}
	
	// check if a link exists with this state already that is not broken
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(),
	pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return state.get() == each->GetState();
	}));
	
	if(iterLink != pStateLinks.cend() && (*iterLink)->GetLinkState() != denStateLink::State::down){
		throw std::invalid_argument("link with state present");
	}
	
	// create the link if not existing, assign it a new identifier and add it
	if(iterLink == pStateLinks.cend()){
		const int lastNextLinkIdentifier = pNextLinkIdentifier;
		
		while(std::find_if(pStateLinks.cbegin(), pStateLinks.cend(), [&](const denStateLink::Ref &each){
			return each->GetIdentifier() == pNextLinkIdentifier;
		}) != pStateLinks.cend()){
			pNextLinkIdentifier = (pNextLinkIdentifier + 1) % 65535;
			if(pNextLinkIdentifier == lastNextLinkIdentifier){
				throw std::invalid_argument("too many state links");
			}
		}
		
		const denStateLink::Ref link(std::make_shared<denStateLink>(*this, *state));
		link->SetIdentifier(pNextLinkIdentifier);
		pStateLinks.push_back(link);
		iterLink = std::prev(pStateLinks.cend());
		
		state->pLinks.push_back(link);
	}
	
	// add message
	const denRealMessage::Ref realMessage(denRealMessage::Pool().Get());
	realMessage->Item().type = denProtocol::CommandCodes::reliableLinkState;
	realMessage->Item().number = (pReliableNumberSend + pReliableMessagesSend.size()) % 65535;
	realMessage->Item().state = denRealMessage::State::pending;
	
	{
	denMessageWriter writer(realMessage->Item().message->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::reliableLinkState);
	writer.WriteUShort((uint16_t)realMessage->Item().number);
	writer.WriteUShort((uint16_t)(*iterLink)->GetIdentifier());
	writer.WriteByte(readOnly ? 1 : 0); // flags: readOnly=0x1
	writer.WriteUShort((uint16_t)message->Item().GetLength());
	writer.Write(message->Item());
	state->LinkWriteValuesWithVerify(writer);
	}
	
	pReliableMessagesSend.push_back(realMessage);
	
	// if the message fits into the window send it right now
	if(pReliableMessagesSend.size() <= (size_t)pReliableWindowSize){
		pSocket->SendDatagram(realMessage->Item().message->Item(), pRealRemoteAddress);
		
		realMessage->Item().state = denRealMessage::State::send;
		realMessage->Item().elapsedResend = 0.0f;
		realMessage->Item().elapsedTimeout = 0.0f;
	}
	
	(*iterLink)->SetLinkState(denStateLink::State::listening);
}

void denConnection::Update(float elapsedTime){
	if(pConnectionState == ConnectionState::disconnected){
		return;
	}
	
	if(!pParentServer){
		while(true){
			if(pConnectionState == ConnectionState::disconnected){
				return;
			}
			
			try{
				denSocketAddress addressReceive;
				const denMessage::Ref message(pSocket->ReceiveDatagram(addressReceive));
				if(!message){
					break;
				}
				
				denMessageReader reader(message->Item());
				ProcessDatagram(reader);
				
			}catch(const std::exception &e){
				if(pLogger){
					pLogger->Log(denLogger::LogSeverity::error,
						std::string("Connection: Update[1]: ") + e.what());
				}
			}
		}
	}
	
	try{
		if( pUpdateTimeouts(elapsedTime) ){
			pUpdateStates();
		}
		
	}catch(const std::exception &e){
		if(pLogger){
			pLogger->Log(denLogger::LogSeverity::error,
				std::string("Connection: Update[2]: ") + e.what());
		}
	}
}

denSocket::Ref denConnection::CreateSocket(){
	return denSocketShared::CreateSocket();
}

denSocketAddress denConnection::ResolveAddress(const std::string &address){
	return denSocketShared::ResolveAddress(address);
}

void denConnection::ConnectionEstablished(){
}

void denConnection::ConnectionFailed(ConnectionFailedReason){
}

void denConnection::ConnectionClosed(){
}

void denConnection::MessageProgress(size_t){
}

void denConnection::MessageReceived(const denMessage::Ref &){
}

denState::Ref denConnection::CreateState(const denMessage::Ref &, bool){
	return nullptr;
}

bool denConnection::Matches(denSocket *bnSocket, const denSocketAddress &address) const{
	return pSocket.get() == bnSocket && address == pRealRemoteAddress;
}

void denConnection::ProcessDatagram(denMessageReader& reader){
	switch((denProtocol::CommandCodes)reader.ReadByte()){
	case denProtocol::CommandCodes::connectionAck:
		pProcessConnectionAck(reader);
		break;
		
	case denProtocol::CommandCodes::connectionClose:
		pProcessConnectionClose(reader);
		break;
		
	case denProtocol::CommandCodes::message:
		pProcessMessage(reader);
		break;
		
	case denProtocol::CommandCodes::reliableMessage:
		pProcessReliableMessage(reader);
		break;
		
	case denProtocol::CommandCodes::reliableLinkState:
		pProcessReliableLinkState(reader);
		break;
		
	case denProtocol::CommandCodes::reliableAck:
		pProcessReliableAck(reader);
		break;
		
	case denProtocol::CommandCodes::linkUp:
		pProcessLinkUp(reader);
		break;
		
	case denProtocol::CommandCodes::linkDown:
		pProcessLinkDown(reader);
		break;
		
	case denProtocol::CommandCodes::linkUpdate:
		pProcessLinkUpdate(reader);
		break;
		
	default:
		// throw std::invalid_argument("Invalid command code");
		break;
	}
}



void denConnection::AcceptConnection(denServer &server, const denSocket::Ref &bnSocket,
const denSocketAddress &address, denProtocol::Protocols protocol){
	pSocket = bnSocket;
	pRealRemoteAddress = address;
	pRemoteAddress = address.ToString();
	pConnectionState = ConnectionState::connected;
	pElapsedConnectResend = 0.0f;
	pElapsedConnectTimeout = 0.0f;
	pProtocol = protocol;
	pParentServer = &server;
	
	ConnectionEstablished();
}

void denConnection::pDisconnect(bool notify, bool remoteClosed){
	if(!pSocket || pConnectionState == ConnectionState::disconnected){
		return;
	}
	
	if(pConnectionState == ConnectionState::connected){
		if(remoteClosed){
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::info, "Connection: Remote closed connection");
			}
			
		}else{
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::info, "Connection: Disconnecting");
			}
			
			const denMessage::Ref connectionClose(denMessage::Pool().Get());
			{
			denMessageWriter writer(connectionClose->Item());
			writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionClose);
			}
			pSocket->SendDatagram(connectionClose->Item(), pRealRemoteAddress);
		}
	}
	
	pClearStates();
	
	pReliableMessagesRecv.clear();
	pReliableMessagesSend.clear();
	pReliableNumberSend = 0;
	pReliableNumberRecv = 0;
	
	pCloseSocket();
	
	if(pLogger){
		pLogger->Log(denLogger::LogSeverity::info, "Connection: Connection closed");
	}
	
	if(notify){
		ConnectionClosed();
	}
	
	pRemoveConnectionFromParentServer();
}

void denConnection::pClearStates(){
	pModifiedStateLinks.clear();
	
	StateLinks::const_iterator iterLink;
	for(iterLink = pStateLinks.cbegin(); iterLink != pStateLinks.cend(); iterLink++){
		denState * const state = (*iterLink)->GetState();
		if(state){
			state->pLinks.remove_if([&](const denStateLink::Ref &each){
				if(each == *iterLink){
					each->pState = nullptr;
					return true;
				}
				return false;
			});
		}
	}
	pStateLinks.clear();
}

void denConnection::pCloseSocket(){
	pConnectionState = ConnectionState::disconnected;
	pElapsedConnectResend = 0.0f;
	pElapsedConnectTimeout = 0.0f;
	pSocket.reset();
}

void denConnection::pRemoveConnectionFromParentServer(){
	if(!pParentServer){
		return;
	}
	
	denServer::Connections &connections = pParentServer->pConnections;
	pParentServer = nullptr;
	// below this point has to be save against this-pointer being potentially deleted
	
	denServer::Connections::iterator iter(std::find_if(connections.begin(),
		connections.end(), [&](const denConnection::Ref &each){
			return each.get() == this;
		}));
	if(iter != connections.end()){
		connections.erase(iter);
	}
}

void denConnection::pUpdateStates(){
	int linkCount = (int)pModifiedStateLinks.size();
	if(linkCount == 0){
		return;
	}
	
	ModifiedStateLinks::iterator iter;
	int changedCount = 0;
	for(iter = pModifiedStateLinks.begin(); iter != pModifiedStateLinks.end(); iter++){
		if((*iter)->GetLinkState() == denStateLink::State::up && (*iter)->GetChanged()){
			changedCount++;
		}
	}
	if(changedCount == 0){
		return;
	}
	changedCount = std::min(changedCount, 255);
	
	const denMessage::Ref updateMessage(denMessage::Pool().Get());
	{
	denMessageWriter writer(updateMessage->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::linkUpdate);
	writer.WriteByte((uint8_t)changedCount);
	
	for(iter = pModifiedStateLinks.begin(); iter != pModifiedStateLinks.end(); ){
		if((*iter)->GetLinkState() != denStateLink::State::up || !(*iter)->GetChanged()){
			iter++;
			continue;
		}
		
		writer.WriteUShort((uint16_t)((*iter)->GetIdentifier()));
		denState * const state = (*iter)->GetState();
		if(!state){
			//throw std::invalid_argument("state link droppped");
			pModifiedStateLinks.erase(ModifiedStateLinks::iterator(iter++));
			continue;
		}
		
		state->LinkWriteValues(writer, **iter);
		
		pModifiedStateLinks.erase(ModifiedStateLinks::iterator(iter++));
		
		changedCount--;
		if(changedCount == 0){
			break;
		}
	}
	}
	
	pSocket->SendDatagram(updateMessage->Item(), pRealRemoteAddress);
}

bool denConnection::pUpdateTimeouts(float elapsedTime){
	switch(pConnectionState){
	case ConnectionState::connected:{
		// increase the timeouts on all send packages
		Messages::const_iterator iter;
		
		for(iter = pReliableMessagesSend.cbegin(); iter != pReliableMessagesSend.cend(); iter++){
			if((*iter)->Item().state != denRealMessage::State::send){
				continue;
			}
			
			(*iter)->Item().elapsedTimeout += elapsedTime;
			if((*iter)->Item().elapsedTimeout > pReliableTimeout){
				if(pLogger){
					pLogger->Log(denLogger::LogSeverity::error, "Connection: Reliable message timeout");
				}
				Disconnect();
				return false;
			}
			
			(*iter)->Item().elapsedResend += elapsedTime;
			if((*iter)->Item().elapsedResend > pReliableResendInterval){
				(*iter)->Item().elapsedResend = 0.0f;
				pSocket->SendDatagram((*iter)->Item().message->Item(), pRealRemoteAddress);
			}
		}
		}
		return true;
		
	case ConnectionState::connecting:
		// increase connecting timeout
		pElapsedConnectTimeout += elapsedTime;
		if(pElapsedConnectTimeout > pConnectTimeout){
			pCloseSocket();
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::info, "Connection: Connection failed (timeout)");
			}
			ConnectionFailed(ConnectionFailedReason::timeout);
			return false;
		}
		
		pElapsedConnectResend += elapsedTime;
		if(pElapsedConnectResend > pConnectResendInterval){
			if(pLogger){
				pLogger->Log(denLogger::LogSeverity::debug, "Connection: Resend connect request");
			}
			pElapsedConnectResend = 0.0f;
			
			const denMessage::Ref connectRequest(denMessage::Pool().Get());
			{
			denMessageWriter writer(connectRequest->Item());
			writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionRequest);
			writer.WriteUShort( 1 ); // version
			writer.WriteUShort((uint8_t)denProtocol::Protocols::DENetworkProtocol);
			}
			pSocket->SendDatagram(connectRequest->Item(), pRealRemoteAddress);
		}
		return true;
		
	default:
		return true;
	}
}


void denConnection::pInvalidateState(const denState::Ref &state){
	StateLinks::iterator iter;
	for(iter = pStateLinks.begin(); iter != pStateLinks.end(); ){
		denStateLink * const link = iter->get();
		if(state.get() == link->GetState()){
			denState::StateLinks::iterator iter2(state->FindLink(link));
			if(iter2 != state->pLinks.end()){
				(*iter2)->pState = nullptr;
				state->pLinks.erase(iter2);
			}
			
			pStateLinks.erase(StateLinks::iterator(iter++));
			
		}else{
			iter++;
		}
	}
}

void denConnection::pAddModifiedStateLink(denStateLink *link){
	pModifiedStateLinks.push_back(link);
}

void denConnection::pProcessQueuedMessages(){
	Messages::iterator iter(std::find_if(pReliableMessagesRecv.begin(),
	pReliableMessagesRecv.end(), [&](const denRealMessage::Ref &each){
		return each->Item().number == pReliableNumberRecv;
	}));
	
	while(iter != pReliableMessagesRecv.cend()){
		switch((*iter)->Item().type){
		case denProtocol::CommandCodes::reliableMessage:{
			denMessageReader reader((*iter)->Item().message->Item());
			pProcessReliableMessageMessage(reader);
			}break;
			
		case denProtocol::CommandCodes::reliableLinkState:{
			denMessageReader reader((*iter)->Item().message->Item());
			pProcessLinkState(reader);
			}break;
			
		default:
			break;
		}
		
		pReliableMessagesRecv.erase(iter);
		pReliableNumberRecv = (pReliableNumberRecv + 1) % 65535;
		
		iter = std::find_if(pReliableMessagesRecv.begin(),
			pReliableMessagesRecv.end(), [&](const denRealMessage::Ref &each){
				return each->Item().number == pReliableNumberRecv;
			});
	}
}

void denConnection::pProcessConnectionAck(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connecting){
		//throw std::invalid_argument("Not connecting");
		return;
	}
	
	switch((denProtocol::ConnectionAck)reader.ReadByte()){
	case denProtocol::ConnectionAck::accepted:
		pProtocol = (denProtocol::Protocols)reader.ReadUShort();
		pConnectionState = ConnectionState::connected;
		pElapsedConnectResend = 0.0f;
		pElapsedConnectTimeout = 0.0f;
		if(pLogger){
			pLogger->Log(denLogger::LogSeverity::info, "Connection: Connection established");
		}
		ConnectionEstablished();
		break;
		
	case denProtocol::ConnectionAck::rejected:
		pCloseSocket();
		if(pLogger){
			pLogger->Log(denLogger::LogSeverity::info, "Connection: Connection failed (rejected)");
		}
		ConnectionFailed(ConnectionFailedReason::rejected);
		break;
		
	case denProtocol::ConnectionAck::noCommonProtocol:
		pCloseSocket();
		if(pLogger){
			pLogger->Log(denLogger::LogSeverity::info, "Connection: Connection failed (no common protocol)");
		}
		ConnectionFailed(ConnectionFailedReason::noCommonProtocol);
		break;
		
	default:
		pCloseSocket();
		if(pLogger){
			pLogger->Log(denLogger::LogSeverity::info, "Connection: Connection failed (invalid message)");
		}
		ConnectionFailed(ConnectionFailedReason::invalidMessage);
	}
}

void denConnection::pProcessConnectionClose(denMessageReader&){
	pDisconnect(true, true);
}

void denConnection::pProcessMessage(denMessageReader &reader){
	const denMessage::Ref message(denMessage::Pool().Get());
	message->Item().SetLength(reader.GetLength() - reader.GetPosition());
	reader.Read(message->Item());
	
	MessageReceived(message);
}

void denConnection::pProcessReliableMessage(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		//throw std::invalid_argument("Reliable message received although not connected.");
		return;
	}
	
	const int number = reader.ReadUShort();
	bool validNumber;
	
	if(number < pReliableNumberRecv){
		validNumber = number < (pReliableNumberRecv + pReliableWindowSize) % 65535;
		
	}else{
		validNumber = number < pReliableNumberRecv + pReliableWindowSize;
	}
	if(!validNumber){
		//throw std::invalid_argument("Reliable message: invalid sequence number.");
		return;
	}
	
	const denMessage::Ref ackMessage(denMessage::Pool().Get());
	{
	denMessageWriter ackWriter(ackMessage->Item());
	ackWriter.WriteByte((uint8_t)denProtocol::CommandCodes::reliableAck);
	ackWriter.WriteUShort((uint16_t)number);
	ackWriter.WriteByte((uint8_t)denProtocol::ReliableAck::success);
	}
	pSocket->SendDatagram(ackMessage->Item(), pRealRemoteAddress);
	
	if(number == pReliableNumberRecv){
		pProcessReliableMessageMessage(reader);
		pReliableNumberRecv = (pReliableNumberRecv + 1) % 65535;
		pProcessQueuedMessages();
		
	}else{
		pAddReliableReceive(denProtocol::CommandCodes::reliableMessage, number, reader);
	}
}

void denConnection::pProcessReliableMessageMessage(denMessageReader &reader){
	denMessage::Ref message(denMessage::Pool().Get());
	message->Item().SetLength(reader.GetLength() - reader.GetPosition());
	reader.Read(message->Item());
	
	MessageReceived(message);
}

void denConnection::pProcessReliableAck(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		//throw std::invalid_argument("Reliable ack: not connected.");
		return;
	}
	
	const int number = reader.ReadUShort();
	const denProtocol::ReliableAck code = (denProtocol::ReliableAck)reader.ReadByte();
	
	Messages::const_iterator iter(std::find_if(pReliableMessagesSend.begin(),
		pReliableMessagesSend.end(), [&](const denRealMessage::Ref &each){
			return (*each).Item().number == number;
		}));
	if(iter == pReliableMessagesSend.cend()){
		//throw std::invalid_argument("Reliable ack: no reliable transmission with this number waiting for an ack!");
		return;
	}
	
	const denRealMessage::Ref message(*iter);
	
	switch(code){
	case denProtocol::ReliableAck::success:
		message->Item().state = denRealMessage::State::done;
		pRemoveSendReliablesDone();
		break;
		
	case denProtocol::ReliableAck::failed:
		if(pLogger){
			pLogger->Log(denLogger::LogSeverity::debug, "Connection: Reliable ACK failed, resend");
		}
		message->Item().elapsedResend = 0.0f;
		pSocket->SendDatagram(message->Item().message->Item(), pRealRemoteAddress);
		break;
	}
}

void denConnection::pProcessReliableLinkState(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		//throw std::invalid_argument("Link state: not connected.");
		return;
	}
	
	const int number = reader.ReadUShort();
	bool validNumber;
	
	if( number < pReliableNumberRecv ){
		validNumber = number < (pReliableNumberRecv + pReliableWindowSize) % 65535;
		
	}else{
		validNumber = number < pReliableNumberRecv + pReliableWindowSize;
	}
	if( ! validNumber ){
		//throw std::invalid_argument("Link state: invalid sequence number.");
		return;
	}
	
	const denMessage::Ref ackMessage(denMessage::Pool().Get());
	{
	denMessageWriter ackWriter(ackMessage->Item());
	ackWriter.WriteByte((uint8_t)denProtocol::CommandCodes::reliableAck);
	ackWriter.WriteUShort((uint16_t)number);
	ackWriter.WriteByte((uint8_t)denProtocol::ReliableAck::success);
	}
	pSocket->SendDatagram(ackMessage->Item(), pRealRemoteAddress);
	
	if(number == pReliableNumberRecv){
		pProcessLinkState(reader);
		pReliableNumberRecv = (pReliableNumberRecv + 1) % 65535;
		pProcessQueuedMessages();
		
	}else{
		pAddReliableReceive(denProtocol::CommandCodes::reliableLinkState, number, reader);
	}
}

void denConnection::pProcessLinkUp(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		//throw std::invalid_argument("Reliable ack: not connected.");
		return;
	}
	
	const int identifier = reader.ReadUShort();
	
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(),
	pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return each->GetIdentifier() == identifier;
	}));
	
	if(iterLink == pStateLinks.cend()){
		//throw std::invalid_argument("up link with identifier absent");
		return;
	}
	if((*iterLink)->GetLinkState() != denStateLink::State::listening){
		//throw std::invalid_argument("up link with identifier absent or not listening");
		return;
	}
	
	(*iterLink)->SetLinkState(denStateLink::State::up);
}

void denConnection::pProcessLinkDown(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		//throw std::invalid_argument("Reliable ack: not connected.");
		return;
	}
	
	const int identifier = reader.ReadUShort();
	
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(),
	pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return each->GetIdentifier() == identifier;
	}));
	
	if(iterLink == pStateLinks.cend()){
		//throw std::invalid_argument("down link with identifier absent");
		return;
	}
	if((*iterLink)->GetLinkState() != denStateLink::State::up){
		//throw std::invalid_argument("down link with identifier not up");
		return;
	}
	
	(*iterLink)->SetLinkState(denStateLink::State::down);
}

void denConnection::pProcessLinkState(denMessageReader &reader){
	const int identifier = reader.ReadUShort();
	const bool readOnly = reader.ReadByte() == 1; // flags: 0x1=readOnly
	
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(),
	pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return each->GetIdentifier() == identifier;
	}));
	
	if(iterLink != pStateLinks.cend() && (*iterLink)->GetLinkState() != denStateLink::State::down){
		//throw std::invalid_argument("Link state: link with this identifier already exists.");
		return;
	}
	
	// create linked network state
	denMessage::Ref message(denMessage::Pool().Get());
	message->Item().SetLength(reader.ReadUShort());
	reader.Read(message->Item());
	
	const denState::Ref state(CreateState(message, readOnly));
	
	denProtocol::CommandCodes code = denProtocol::CommandCodes::linkDown;
	if(state){
		if(!state->LinkReadAndVerifyAllValues(reader)){
			//throw std::invalid_argument("Link state does not match the state provided.");
			return;
		}
		if(iterLink != pStateLinks.cend()){
			//throw std::invalid_argument("Link state existing already.");
			return;
		}
		
		const denStateLink::Ref link(std::make_shared<denStateLink>(*this, *state));
		link->SetIdentifier(identifier);
		pStateLinks.push_back(link);
		state->pLinks.push_back(link);
		
		link->SetLinkState(denStateLink::State::up);
		code = denProtocol::CommandCodes::linkUp;
	}
	
	message = denMessage::Pool().Get();
	{
	denMessageWriter writer(message->Item());
	writer.WriteByte((uint8_t)code);
	writer.WriteUShort((uint16_t)identifier);
	}
	pSocket->SendDatagram(message->Item(), pRealRemoteAddress);
}

void denConnection::pProcessLinkUpdate(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		//throw std::invalid_argument("Reliable ack: not connected.");
		return;
	}
	
	const int count = reader.ReadByte();
	int i;
	for(i=0; i<count; i++){
		const int identifier = reader.ReadUShort();
		
		StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(),
		pStateLinks.cend(), [&](const denStateLink::Ref &each){
			return each->GetIdentifier() == identifier;
		}));
		
		if(iterLink == pStateLinks.cend()){
			//throw std::invalid_argument("invalid link identifier");
			return;
		}
		if((*iterLink)->GetLinkState() != denStateLink::State::up){
			//throw std::invalid_argument("invalid link identifier");
			return;
		}
		
		denState * const state = (*iterLink)->GetState();
		if(!state){
			//throw std::invalid_argument("state link droppped");
			return;
		}
		
		state->LinkReadValues(reader, *iterLink->get());
	}
}

void denConnection::pAddReliableReceive(denProtocol::CommandCodes type, int number, denMessageReader &reader){
	denRealMessage::Ref message(denRealMessage::Pool().Get());
	message->Item().message->Item().SetLength(reader.GetLength() - reader.GetPosition());
	reader.Read(message->Item().message->Item());
	
	message->Item().type = type;
	message->Item().number = number;
	message->Item().state = denRealMessage::State::done;
	
	pReliableMessagesRecv.push_back(message);
}

void denConnection::pRemoveSendReliablesDone(){
	bool anyRemoved = false;
	
	while(!pReliableMessagesSend.empty()){
		if(pReliableMessagesSend.front()->Item().state != denRealMessage::State::done){
			break;
		}
		
		pReliableMessagesSend.pop_front();
		pReliableNumberSend = (pReliableNumberSend + 1) % 65535;
		anyRemoved = true;
	}
	
	if(anyRemoved){
		pSendPendingReliables();
	}
}

void denConnection::pSendPendingReliables(){
	Messages::const_iterator iter;
	int sendCount = 0;
	
	for(iter = pReliableMessagesSend.cbegin(); iter != pReliableMessagesSend.cend(); iter++){
		if((*iter)->Item().state != denRealMessage::State::pending){
			continue;
		}
		
		pSocket->SendDatagram((*iter)->Item().message->Item(), pRealRemoteAddress);
		
		(*iter)->Item().state = denRealMessage::State::send;
		(*iter)->Item().elapsedResend = 0.0f;
		(*iter)->Item().elapsedTimeout = 0.0f;
		
		if(++sendCount == pReliableWindowSize){
			break;
		}
	}
}
