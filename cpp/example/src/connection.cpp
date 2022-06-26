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

#include "connection.h"
#include "messages.h"
#include "app.h"

#include <denetwork/denServer.h>
#include <denetwork/message/denMessageReader.h>
#include <denetwork/message/denMessageWriter.h>

int Connection::nextid = 1;

Connection::OtherClientState::OtherClientState(const denLogger::Ref &logger, int id) :
denState(true),
id(id),
valueBar(std::make_shared<denValueInt>(denValueIntegerFormat::sint16))
{
	SetLogger(logger);
	AddValue(valueBar);
}

Connection::Connection(App &app, const denLogger::Ref &logger, bool readonly) : app(app), id(nextid++){
	SetLogger(logger);
	state = std::make_shared<denState>(readonly);
	state->SetLogger(logger);
	valueBar = std::make_shared<denValueInt>(denValueIntegerFormat::sint16);
	valueBar->SetValue(30);
	state->AddValue(valueBar);
}

int Connection::getBar(){
	return valueBar->GetValue();
}

void Connection::setBar(int value){
	valueBar->SetValue((int16_t)std::max(std::min(value, 60), 0));
}

void Connection::incrementBar(int value){
	setBar(getBar() + value);
}

bool Connection::ready() const{
	return (bool)valueBar;
}

void Connection::ConnectionEstablished(){
}

void Connection::ConnectionFailed(ConnectionFailedReason reason){
	ConnectionClosed();
}

void Connection::ConnectionClosed(){
	if(GetParentServer()){
		const denServer::Connections &connections = GetParentServer()->GetConnections();
		denServer::Connections::const_iterator iter;
		for(iter = connections.cbegin(); iter != connections.cend(); iter++){
			if(iter->get() != this){
				denMessage::Ref lm(denMessage::Pool().Get());
				denMessageWriter(lm->Item())
					.WriteByte((uint8_t)MessageCode::dropClient)
					.WriteUShort((uint16_t)id);
				(*iter)->SendReliableMessage(lm);
			}
		}
		
	}else{
		app.quit = true;
	}
}

void Connection::MessageProgress(size_t bytesReceived){
}

void Connection::MessageReceived(const denMessage::Ref &message){
	denMessageReader reader(message->Item());
	
	switch((MessageCode)reader.ReadByte()){
	case MessageCode::dropClient:{
		const int id = reader.ReadUShort();
		otherClientStates.remove_if([&](const denState::Ref &each){
			return static_cast<OtherClientState&>(*each).id == id;
		});
		}break;
		
	default:
		break;
	}
}

denState::Ref Connection::CreateState(const denMessage::Ref &message, bool readOnly){
	denMessageReader reader(message->Item());
	
	switch((MessageCode)reader.ReadByte()){
	case MessageCode::linkServerState:
		// create local state for server state. read-only to us
		serverState = std::make_shared<denState>(readOnly);
		serverState->SetLogger(GetLogger());
		serverValueTime = std::make_shared<denValueString>();
		serverValueBar = std::make_shared<denValueInt>(denValueIntegerFormat::sint16);
		serverState->AddValue(serverValueTime);
		serverState->AddValue(serverValueBar);
		return serverState;
		
	case MessageCode::linkClientState:
		// use local state as client state. read-write to us
		return state;
		
	case MessageCode::linkOtherClientState:{
		// create other client state. read-only to us
		const int id = reader.ReadUShort();
		const denState::Ref otherState = std::make_shared<OtherClientState>(GetLogger(), id);
		otherClientStates.push_back(otherState);
		return otherState;
		}
		
	default:
		return nullptr;
	}
}

bool Connection::hasServerState() const{
	return (bool)serverState;
}

const std::string &Connection::getServerTime(){
	return serverValueTime->GetValue();
}

int Connection::getServerBar(){
	return serverValueBar->GetValue();
}
