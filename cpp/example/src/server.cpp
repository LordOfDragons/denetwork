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

#include <iomanip>

#include "connection.h"
#include "server.h"
#include "messages.h"

#include <denetwork/message/denMessageWriter.h>

Server::Server(App &app, const denLogger::Ref &logger) : app(app){
	SetLogger(logger);
	state = std::make_shared<denState>(false);
	state->SetLogger(logger);
	valueTime = std::make_shared<denValueString>();
	valueBar = std::make_shared<denValueInt>(denValueIntegerFormat::sint16);
	valueBar->SetValue(30);
	state->AddValue(valueTime);
	state->AddValue(valueBar);
}

denConnection::Ref Server::CreateConnection(){
	return std::make_shared<Connection>(app, GetLogger(), true);
}

const std::string &Server::getTime() const{
	return valueTime->GetValue();
}

void Server::setTime(const std::string &time){
	valueTime->SetValue(time);
}

void Server::updateTime(){
	std::stringstream s;
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	s << std::put_time(&tm, "%c");
	setTime(s.str());
}

int Server::getBar() const{
	return valueBar->GetValue();
}

void Server::setBar(int value){
	valueBar->SetValue((int16_t)std::max(std::min(value, 60), 0));
}

void Server::incrementBar(int value){
	setBar(getBar() + value);
}

void Server::ClientConnected(const denConnection::Ref &connection){
	linkClient(*connection);
}

void Server::linkClient(denConnection &connection){
	Connection &excon = static_cast<Connection&>(connection);
	
	// link server state. read-only on client side
	denMessage::Ref lm(denMessage::Pool().Get());
	denMessageWriter(lm->Item()).WriteByte((uint8_t)MessageCode::linkServerState);
	connection.LinkState(lm, state, true);
	
	// link all client states. this includes connecting client itself
	Connections::const_iterator iter;
	for(iter = GetConnections().cbegin(); iter != GetConnections().cend(); iter++){
		if(iter->get() == &connection){
			// connecting client gets read-write state of its own state
			lm = denMessage::Pool().Get();
			denMessageWriter(lm->Item()).WriteByte((uint8_t)MessageCode::linkClientState);
			connection.LinkState(lm, excon.state, false);
			
		}else{
			Connection &othercon = static_cast<Connection&>(**iter);
			
			// all other client states are read-only to the connecting client
			lm = denMessage::Pool().Get();
			denMessageWriter(lm->Item())
				.WriteByte((uint8_t)MessageCode::linkOtherClientState)
				.WriteUShort((uint16_t)othercon.id);
			connection.LinkState(lm, othercon.state, true);
			
			// and the connecting client state is also read-only to the others
			lm = denMessage::Pool().Get();
			denMessageWriter(lm->Item())
				.WriteByte((uint8_t)MessageCode::linkOtherClientState)
				.WriteUShort((uint16_t)excon.id);
			othercon.LinkState(lm, excon.state, true);
		}
	}
}
