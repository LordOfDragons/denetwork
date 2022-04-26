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

denConnection::denConnection() :
pConnected(false),
pConnectionState(ConnectionState::disconnected),
pIdentifier(-1),
pProtocol(denProtocol::Protocols::DENetworkProtocol),
pReliableNumberSend(0),
pReliableNumberRecv(0),
pReliableWindowSize(10){
}

denConnection::~denConnection(){
}

void denConnection::SetListener(const denConnectionListener::Ref &listener){
	pListener = listener;
}

void denConnection::ConnectTo(const std::string &address){
}

void denConnection::Disconnect(){
}

void denConnection::SendMessage(const denMessage::Ref &message, int maxDelay){
}

void denConnection::SendReliableMessage(const denMessage::Ref &message){
}

void denConnection::LinkState(const denMessage::Ref &message, const denState::Ref &state, bool readOnly){
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
		denStateLink * const link = *iter;
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
		pReliableMessagesSend.end(), [&](const denMessage::Ref &each){
			return (*each).Item().pNumber == number;
		}));
	if(iter == pReliableMessagesSend.cend()){
		throw std::invalid_argument("Reliable ack: no reliable transmission with this number waiting for an ack!");
	}
	
	const denMessage::Ref message(*iter);
	
	switch(code){
	case denProtocol::ReliableAck::success:
		message->Item().pState = denMessage::State::done;
		pRemoveSendReliablesDone();
		break;
		
	case denProtocol::ReliableAck::failed:
		message->Item().pSecondsSinceSend = 0.0f;
		pSocket->SendDatagram(message->Item(), pRealRemoteAddress);
		break;
	}
}

void denConnection::ProcessLinkUp(denMessageReader &reader){
}

void denConnection::ProcessLinkDown(denMessageReader &reader){
}

void denConnection::ProcessLinkUpdate(denMessageReader &reader){
}
