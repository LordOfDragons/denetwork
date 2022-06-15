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

#include <string>

#include <denetwork/denConnection.h>
#include <denetwork/denServer.h>

#include "screen.h"
#include "input.h"
#include "logger.h"

class App{
	std::string paramListen, paramConnect;
	denServer::Ref server;
	denConnection::Ref connection;
	Screen screen;
	Input input;
	std::shared_ptr<Logger> logger = std::make_shared<Logger>();
	
public:
	bool quit = false;
	
	bool init(int argc, char *argv[]);
	void run();
	void initScreen();
	void exitScreen();
	void serverListen();
	void clientConnect();
	void drawScreen();
	void handleInput();
	void updateServer(float elapsed);
	void updateClient(float elapsed);
	void appLoop();
	void specialTest();
};
