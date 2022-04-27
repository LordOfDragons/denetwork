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
#include "message/denMessage.h"
#include "message/denMessageReader.h"
#include "message/denMessageWriter.h"
#include "internal/denRealMessage.h"

denConnection::denConnection() :
pConnectionState(ConnectionState::disconnected),
pIdentifier(-1),
pProtocol(denProtocol::Protocols::DENetworkProtocol),
pNextLinkIdentifier(0),
pReliableNumberSend(0),
pReliableNumberRecv(0),
pReliableWindowSize(10){
}

denConnection::~denConnection(){
}

bool denConnection::GetConnected() const{
	return pConnectionState == ConnectionState::connected;
}

void denConnection::SetListener(const denConnectionListener::Ref &listener){
	pListener = listener;
}

void denConnection::ConnectTo(const std::string &address){
	if(pSocket || pConnectionState != ConnectionState::disconnected){
		throw std::invalid_argument("already connected");
	}
	
	pSocket = std::make_shared<denSocket>();
	pSocket->GetAddress().SetIPv4Any();
	pSocket->Bind();
	
	pLocalAddress = pSocket->GetAddress().ToString();
	
	const denMessage::Ref connectRequest(denMessage::Pool().Get());
	{
	denMessageWriter writer(connectRequest->Item());
	writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionRequest);
	writer.WriteUShort( 1 ); // version
	writer.WriteUShort((uint8_t)denProtocol::Protocols::DENetworkProtocol);
	}
	pRealRemoteAddress.SetIPv4FromString(address);
	pRemoteAddress = address;
	
	pSocket->SendDatagram(connectRequest->Item(), pRealRemoteAddress);
	
	pConnectionState = ConnectionState::connecting;
}

void denConnection::Disconnect(){
	if(!pSocket){
		return;
	}
	
	if(pConnectionState == ConnectionState::connected){
		const denMessage::Ref connectionClose(denMessage::Pool().Get());
		{
		denMessageWriter writer(connectionClose->Item());
		writer.WriteByte((uint8_t)denProtocol::CommandCodes::connectionClose);
		}
		pSocket->SendDatagram(connectionClose->Item(), pRealRemoteAddress);
	}
	
	pDisconnect();
}

void denConnection::SendMessage(const denMessage::Ref &message, int maxDelay){
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
		realMessage->Item().secondsSinceSend = 0.0f;
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
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(), pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return &each->GetState() == state.get();
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
		iterLink = pStateLinks.cend();
		
		state->GetLinks().push_back(link);
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
		realMessage->Item().secondsSinceSend = 0.0f;
	}
	
	(*iterLink)->SetLinkState(denStateLink::State::listening);
}

void denConnection::SetIdentifier(int identifier){
	pIdentifier = identifier;
}

void denConnection::AddModifiedStateLink(denStateLink *link){
	pModifiedStateLinks.push_back(link);
}

void denConnection::Process(float elapsedTime){
	if(pConnectionState == ConnectionState::connected){
		pUpdateTimeouts(elapsedTime);
		pUpdateStates();
	}
}

void denConnection::InvalidateState(denState &state){
	StateLinks::iterator iter;
	for(iter = pStateLinks.begin(); iter != pStateLinks.end(); ){
		denStateLink * const link = iter->get();
		if(&link->GetState() == &state){
			denState::StateLinks::iterator iter2(state.FindLink(link));
			if(iter2 != state.GetLinks().end()){
				state.GetLinks().erase(iter2);
			}
			
			pStateLinks.erase(StateLinks::iterator(iter++));
			
		}else{
			iter++;
		}
	}
}

bool denConnection::Matches(denSocket *bnSocket, const denAddress &address) const{
	return pSocket.get() == bnSocket && address == pRealRemoteAddress;
}

void denConnection::AcceptConnection(const denSocket::Ref &bnSocket, denAddress &address, denProtocol::Protocols protocol){
	pSocket = bnSocket;
	pRealRemoteAddress = address;
	pRemoteAddress = address.ToString();
	pConnectionState = ConnectionState::connected;
	pProtocol = protocol;
}

void denConnection::ProcessConnectionAck(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connecting){
		throw std::invalid_argument("Not connecting");
	}
	
	switch((denProtocol::ConnectionAck)reader.ReadByte()){
	case denProtocol::ConnectionAck::accepted:
		pProtocol = (denProtocol::Protocols)reader.ReadUShort();
		pConnectionState = ConnectionState::connected;
		break;
		
	case denProtocol::ConnectionAck::rejected:
	case denProtocol::ConnectionAck::noCommonProtocol:
	default:
		pConnectionState = ConnectionState::disconnected;
		if(pListener){
			pListener->ConnectionClosed(*this);
		}
	}
}

void denConnection::ProcessConnectionClose(denMessageReader&){
	if(pConnectionState != ConnectionState::connected){
		return;
	}
	
	pDisconnect();
	
	if(pListener){
		pListener->ConnectionClosed(*this);
	}
}

void denConnection::ProcessMessage(denMessageReader &reader){
	/*const int flags = */ reader.ReadByte();
	
	const denMessage::Ref message(denMessage::Pool().Get());
	message->Item().SetLength(reader.GetLength() - reader.GetPosition());
	reader.Read(message->Item());
	
	if(pListener){
		pListener->MessageReceived(message);
	}
}

void denConnection::ProcessReliableMessage(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("Reliable message received although not connected.");
	}
	
	const int number = reader.ReadUShort();
	bool validNumber;
	
	if(number < pReliableNumberRecv){
		validNumber = number < (pReliableNumberRecv + pReliableWindowSize) % 65535;
		
	}else{
		validNumber = number < pReliableNumberRecv + pReliableWindowSize;
	}
	if(!validNumber){
		throw std::invalid_argument("Reliable message: invalid sequence number.");
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
		pProcessReliableMessage(number, reader);
		pReliableNumberRecv = (pReliableNumberRecv + 1) % 65535;
		pProcessQueuedMessages();
		
	}else{
		pAddReliableReceive(denProtocol::CommandCodes::reliableMessage, number, reader);
	}
}

void denConnection::ProcessReliableLinkState(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("Link state: not connected.");
	}
	
	const int number = reader.ReadUShort();
	bool validNumber;
	
	if( number < pReliableNumberRecv ){
		validNumber = number < (pReliableNumberRecv + pReliableWindowSize) % 65535;
		
	}else{
		validNumber = number < pReliableNumberRecv + pReliableWindowSize;
	}
	if( ! validNumber ){
		throw std::invalid_argument("Link state: invalid sequence number.");
	}
	
	const denMessage::Ref ackMessage(denMessage::Pool().Get());
	{
	denMessageWriter ackWriter(ackMessage->Item());
	ackWriter.WriteByte((uint8_t)denProtocol::CommandCodes::connectionAck);
	ackWriter.WriteUShort((uint16_t)number);
	ackWriter.WriteByte((uint8_t)denProtocol::ReliableAck::success);
	}
	pSocket->SendDatagram(ackMessage->Item(), pRealRemoteAddress);
	
	if(number == pReliableNumberRecv){
		pProcessLinkState(number, reader);
		pReliableNumberRecv = (pReliableNumberRecv + 1) % 65535;
		pProcessQueuedMessages();
		
	}else{
		pAddReliableReceive(denProtocol::CommandCodes::reliableLinkState, number, reader);
	}
}

void denConnection::ProcessReliableAck(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("Reliable ack: not connected.");
	}
	
	const int number = reader.ReadUShort();
	const denProtocol::ReliableAck code = (denProtocol::ReliableAck)reader.ReadByte();
	
	Messages::const_iterator iter(std::find_if(pReliableMessagesSend.begin(),
		pReliableMessagesSend.end(), [&](const denRealMessage::Ref &each){
			return (*each).Item().number == number;
		}));
	if(iter == pReliableMessagesSend.cend()){
		throw std::invalid_argument("Reliable ack: no reliable transmission with this number waiting for an ack!");
	}
	
	const denRealMessage::Ref message(*iter);
	
	switch(code){
	case denProtocol::ReliableAck::success:
		message->Item().state = denRealMessage::State::done;
		pRemoveSendReliablesDone();
		break;
		
	case denProtocol::ReliableAck::failed:
		message->Item().secondsSinceSend = 0.0f;
		pSocket->SendDatagram(message->Item().message->Item(), pRealRemoteAddress);
		break;
	}
}

void denConnection::ProcessLinkUp(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("Reliable ack: not connected.");
	}
	
	const int identifier = reader.ReadUShort();
	
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(), pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return each->GetIdentifier() == identifier;
	}));
	
	if(iterLink == pStateLinks.cend() || (*iterLink)->GetLinkState() != denStateLink::State::listening){
		throw std::invalid_argument("up link with identifier absent or not listening");
	}
	
	(*iterLink)->SetLinkState(denStateLink::State::up);
}

void denConnection::ProcessLinkDown(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("Reliable ack: not connected.");
	}
	
	const int identifier = reader.ReadUShort();
	
	StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(), pStateLinks.cend(), [&](const denStateLink::Ref &each){
		return each->GetIdentifier() == identifier;
	}));
	
	if(iterLink == pStateLinks.cend() || (*iterLink)->GetLinkState() != denStateLink::State::listening){
		throw std::invalid_argument("down link with identifier absent or not listening");
	}
	
	(*iterLink)->SetLinkState(denStateLink::State::down);
}

void denConnection::ProcessLinkUpdate(denMessageReader &reader){
	if(pConnectionState != ConnectionState::connected){
		throw std::invalid_argument("Reliable ack: not connected.");
	}
	
	const int count = reader.ReadByte();
	int i;
	for(i=0; i<count; i++){
		const int identifier = reader.ReadUShort();
		
		StateLinks::const_iterator iterLink(std::find_if(pStateLinks.cbegin(), pStateLinks.cend(), [&](const denStateLink::Ref &each){
			return each->GetIdentifier() == identifier;
		}));
		
		if(iterLink == pStateLinks.cend() || (*iterLink)->GetLinkState() != denStateLink::State::up){
			throw std::invalid_argument("invalid link identifier");
		}
		
		(*iterLink)->GetState().LinkReadValues(reader, *iterLink->get());
	}
}



void denConnection::pDisconnect(){
	pModifiedStateLinks.clear();
	
	StateLinks::const_iterator iterLink;
	for(iterLink = pStateLinks.cbegin(); iterLink != pStateLinks.cend(); iterLink++){
		(*iterLink)->GetState().GetLinks().remove_if([&](const denStateLink::Ref &each){
			return each == *iterLink;
		});
	}
	pStateLinks.clear();
	
	pReliableMessagesRecv.clear();
	pReliableMessagesSend.clear();
	
	pConnectionState = ConnectionState::disconnected;
	pSocket.reset();
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
		(*iter)->GetState().LinkWriteValues(writer, **iter);
		
		pModifiedStateLinks.erase(ModifiedStateLinks::iterator(iter++));
		linkCount--;
		
		changedCount--;
		if(changedCount == 0){
			break;
		}
	}
	}
	
	pSocket->SendDatagram(updateMessage->Item(), pRealRemoteAddress);
}

void denConnection::pUpdateTimeouts(float elapsedTime){
	// increase the timeouts on all send packages
	Messages::const_iterator iter;
	float timeout = 3.0f;
	
	for(iter = pReliableMessagesSend.cbegin(); iter != pReliableMessagesSend.cend(); iter++){
		if((*iter)->Item().state != denRealMessage::State::send){
			continue;
		}
		
		(*iter)->Item().secondsSinceSend += elapsedTime;
		
		if((*iter)->Item().secondsSinceSend > timeout){
			// resend message
			pSocket->SendDatagram((*iter)->Item().message->Item(), pRealRemoteAddress);
			(*iter)->Item().secondsSinceSend = 0.0f;
		}
	}
}

void denConnection::pProcessQueuedMessages(){
}
