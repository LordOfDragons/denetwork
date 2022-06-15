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

#include <denetwork/denConnection.h>
#include <denetwork/state/denState.h>
#include <denetwork/value/denValueInteger.h>
#include <denetwork/value/denValueString.h>

class App;

class Connection : public denConnection{
	static int nextid;
	
public:
	class OtherClientState : public denState{
	public:
		const int id;
		const denValueInt::Ref valueBar;
		
		OtherClientState(const denLogger::Ref &logger, int id);
	};
	
	App &app;
	int id;
	denState::Ref state;
	denValueInt::Ref valueBar;
	
	denState::Ref serverState;
	denValueString::Ref serverValueTime;
	denValueInt::Ref serverValueBar;
	
	std::list<denState::Ref> otherClientStates;
	
	Connection(App &app, const denLogger::Ref &logger, bool readonly);
	
	int getBar();
	void setBar(int value);
	void incrementBar(int value);
	
	bool ready() const;
	
	void ConnectionEstablished() override;
	void ConnectionClosed() override;
	void MessageProgress(size_t bytesReceived) override;
	void MessageReceived(const denMessage::Ref &message) override;
	denState::Ref CreateState(const denMessage::Ref &message, bool readOnly) override;
	
	bool hasServerState() const;
	const std::string &getServerTime();
	int getServerBar();
};
