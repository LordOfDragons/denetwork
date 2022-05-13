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

#include <denetwork/denServer.h>
#include <denetwork/value/denValueInteger.h>
#include <denetwork/value/denValueString.h>

class App;

class Server : public denServer{
public:
	App &app;
	denState::Ref state;
	denValueString::Ref valueTime;
	denValueInt::Ref valueBar;
	
	Server(App &app, const denLogger::Ref &logger);
	
	denConnection::Ref CreateConnection() override;
	
	const std::string &getTime() const;
	void setTime(const std::string &time);
	void updateTime();
	
	int getBar() const;
	void setBar(int value);
	void incrementBar(int value);
	
	void ClientConnected(const denConnection::Ref &connection) override;
	
	void linkClient(denConnection &connection);
};
