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

bool denConnection::Matches(const denSocket &bnSocket, const denAddress &address) const{
}

void denConnection::AcceptConnection(denSocket &bnSocket, denAddress &address, denProtocol::Protocols protocol){
}

void denConnection::ProcessConnectionAck(denMessageReader &reader){
}

void denConnection::ProcessConnectionClose(denMessageReader &reader){
}

void denConnection::ProcessMessage(denMessageReader &reader){
}

void denConnection::ProcessReliableMessage(denMessageReader &reader){
}

void denConnection::ProcessReliableLinkState(denMessageReader &reader){
}

void denConnection::ProcessReliableAck(denMessageReader &reader){
}

void denConnection::ProcessLinkUp(denMessageReader &reader){
}

void denConnection::ProcessLinkDown(denMessageReader &reader){
}

void denConnection::ProcessLinkUpdate(denMessageReader &reader){
}
