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

#include <iostream>

#include "app.h"
#include "connection.h"
#include "server.h"

#include <denetwork/message/denMessageReader.h>
#include <denetwork/message/denMessageWriter.h>
#include <denetwork/value/denValueFloating.h>
#include <denetwork/value/denValueData.h>

bool App::init(int argc, char *argv[]){
	int i;
	for(i=1; i<argc; i++){
		const std::string arg(argv[i]);
		
		if(arg.rfind("--listen=", 0) == 0){
			paramListen = arg.substr(9);
			
		}else if(arg.rfind("--connect=", 0) == 0){
			paramConnect = arg.substr(10);
			
		}else if(arg == "--help"){
			std::cout << "denetworkcpp_example [parameters]" << std::endl;
			std::cout << std::endl;
			std::cout << "parameters:" << std::endl;
			std::cout << std::endl;
			std::cout << "--listen=address:port" << std::endl;
			std::cout << "    listen for clients on address:port" << std::endl;
			std::cout << std::endl;
			std::cout << "--connect=address:port" << std::endl;
			std::cout << "    connect to host on address:port" << std::endl;
			std::cout << std::endl;
			std::cout << "--help" << std::endl;
			std::cout << "    show help screen" << std::endl;
			return false;
			
		}else{
			std::cerr << "Invalid argument '" << arg << "'" << std::endl;
			return false;
		}
	}
	return true;
}

void App::run(){
	/*
	server = std::make_shared<Server>(*this, logger);
	const std::vector<std::string> list(server->FindPublicAddresses());
	std::vector<std::string>::const_iterator iter;
	for(iter=list.cbegin(); iter!=list.cend(); iter++){
		std::cout << *iter << std::endl;
	}
	std::cout << server->ResolveAddress(list.front()).ToString() << std::endl;
	std::cout << server->ResolveAddress("0123:4567:89ab:cdef:0123:4567:89ab:cdef").ToString() << std::endl;
	std::cout << server->ResolveAddress("0123:4567:0:0:0123:4567:0:0").ToString() << std::endl;
	std::cout << server->ResolveAddress("0:0:0:cdef:0123:4567:0:0").ToString() << std::endl;
	std::cout << server->ResolveAddress("0123:4567:89ab:cdef:0123:0:0:0").ToString() << std::endl;
	std::cout << server->ResolveAddress("0:0:0:0:0:0:0:1").ToString() << std::endl;
	std::cout << server->ResolveAddress("2:3:4:5:6:7:8:1").ToString() << std::endl;
	std::cout << server->ResolveAddress("dragondreams.ch").ToString() << std::endl;
	std::cout << server->ResolveAddress("localhost").ToString() << std::endl;
	return;
	*/
	
	initScreen();
	
	if(!paramListen.empty()){
		serverListen();
		appLoop();
		
	}else if(!paramConnect.empty()){
		clientConnect();
		appLoop();
		
	}else{
	}
	
	connection.reset();
	server.reset();
	
	exitScreen();
	
// 		specialTest();
}

void App::initScreen(){
	screen.clearScreen();
	input.nonblock(true);
}

void App::exitScreen(){
	input.nonblock(false);
}

void App::serverListen(){
	server = std::make_shared<Server>(*this, logger);
	server->ListenOn(paramListen);
}

void App::clientConnect(){
	connection = std::make_shared<Connection>(*this, logger, false);
	connection->ConnectTo(paramConnect);
}

void App::drawScreen(){
	const denPoint2 size(screen.terminalSize());
	std::stringstream s;
	screen.screenTopLeft(s);
	
	if(server){
		const Server &exsrv = static_cast<Server&>(*server);
		screen.printString(s, size.x, "Server Time", exsrv.getTime(), Screen::Color::red);
		screen.printBar(s, size.x, "Server Bar", exsrv.getBar(), Screen::Color::red);
		
		denServer::Connections::const_iterator iterCon;
		for(iterCon = server->GetConnections().cbegin(); iterCon != server->GetConnections().cend(); iterCon++){
			Connection &excon = static_cast<Connection&>(**iterCon);
			if(excon.ready()){
				std::stringstream s2;
				s2 << "Client#" << excon.id << " Bar";
				screen.printBar(s, size.x, s2.str(), excon.getBar(), Screen::Color::green);
			}
		}
	}
	
	if(connection){
		Connection &excon = static_cast<Connection&>(*connection);
		screen.printBar(s, size.x, "Client Bar", excon.getBar(), Screen::Color::red);
		
		if(excon.hasServerState()){
			screen.printString(s, size.x, "Server Time", excon.getServerTime(), Screen::Color::green);
			screen.printBar(s, size.x, "Server Bar", excon.getServerBar(), Screen::Color::green);
		}
		
		std::list<denState::Ref>::const_iterator iter;
		for(iter = excon.otherClientStates.cbegin(); iter != excon.otherClientStates.cend(); iter++){
			Connection::OtherClientState &otsta = static_cast<Connection::OtherClientState&>(**iter);
			std::stringstream s2;
			s2 << "Client#" << otsta.id << " Bar";
			screen.printBar(s, size.x, s2.str(), otsta.valueBar->GetValue(), Screen::Color::cyan);
		}
	}
	
	screen.foregroundColor(s, Screen::Color::white, Screen::Style::bold);
	s << "Logs:" << std::string(size.x - 5, ' ') << std::endl;
	screen.resetColors(s);
	Logger::LogBuffer::const_iterator iterLog;
	for(iterLog = logger->buffer.cbegin(); iterLog != logger->buffer.cend(); iterLog++){
		s << *iterLog << std::string(size.x - iterLog->length(), ' ') << std::endl;
	}
	
	std::cout << s.str();
}

void App::handleInput(){
	if(!input.kbhit()) return;
	
	const int c = fgetc(stdin);
	switch(c){
	case 'q':
		quit = true;
		break;
		
	case 'w':
		break;
		
	case 'e':
		break;
		
	case '\033':
		fgetc(stdin); // '['
		switch(fgetc(stdin)){
		case 'A': //up arrow
			break;
			
		case 'B': // down arrow
			break;
			
		case 'C': // right arrow
			if(server) static_cast<Server&>(*server).incrementBar(1);
			if(connection) static_cast<Connection&>(*connection).incrementBar(1);
			break;
			
		case 'D': // left arrow
			if(server) static_cast<Server&>(*server).incrementBar(-1);
			if(connection) static_cast<Connection&>(*connection).incrementBar(-1);
			break;
		}
	}
}

void App::updateServer(float elapsed){
	if(!server) return;
	static_cast<Server&>(*server).updateTime();
	server->Update(elapsed);
}

void App::updateClient(float elapsed){
	if(!connection) return;
	connection->Update(elapsed);
}

void App::appLoop(){
	auto start = std::chrono::steady_clock::now();
	
	timespec sleeper;
	sleeper.tv_sec = 0;
	sleeper.tv_nsec = 1000000;
	
	while(!quit){
		const auto end = std::chrono::steady_clock::now();
		const float elapsed = 1e-9f * std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		start = end;
		
		handleInput();
		updateServer(elapsed);
		updateClient(elapsed);
		drawScreen();
		
		nanosleep(&sleeper, nullptr); // 1ms sleep
	}
}

void App::specialTest(){
	denState::Ref state(std::make_shared<denState>(false));
	
	denValueInt::Ref test(std::make_shared<denValueInt>(denValueIntegerFormat::uint32));
	test->SetValue(8);
	state->AddValue(test);
	
	denValueFloat::Ref test2(std::make_shared<denValueFloat>(denValueFloatingFormat::float32));
	test2->SetPrecision(0.01);
	test2->SetValue(8);
	state->AddValue(test2);
	
	denValueVector3::Ref test3(std::make_shared<denValueVector3>(denValueFloatingFormat::float32));
	test3->SetPrecision(denVector3(0.01));
	test3->SetValue(denVector3(1, 2, 3));
	state->AddValue(test3);
	
	denValuePoint2::Ref test4(std::make_shared<denValuePoint2>(denValueIntegerFormat::sint32));
	test4->SetValue(denPoint2(1, 2));
	state->AddValue(test4);
	
	denValueString::Ref test5(std::make_shared<denValueString>());
	test5->SetValue("this is a test");
	state->AddValue(test5);
	
	denValueData::Ref test6(std::make_shared<denValueData>());
	test6->SetValue(denValueData::Data{5, 2, 80, 45, 60, 30});
	state->AddValue(test6);
	
	{
	denMessage::Ref message(denMessage::Pool().Get());
	
	{
	denMessageWriter writer(message->Item());
	writer.WriteUShort(80).WriteInt(210).WriteUInt(99443).WriteFloat(1.34f);
	}
	
	{
	denMessageReader reader(message->Item());
	const uint16_t mv1 = reader.ReadUShort();
	const int32_t mv2 = reader.ReadInt();
	const uint32_t mv3 = reader.ReadUInt();
	const float mv4 = reader.ReadFloat();
	printf("message: %d %d %d %g\n", mv1, mv2, mv3, mv4);
	}
	
	{
	denRealMessage::Ref realMessage(denRealMessage::Pool().Get());
	realMessage->Item().message = message;
	}
	}
	
	// just if somebody uses leak detection. pools are static class members and as such
	// they are cleaned up during application destruction. leak detection can notice
	// them as "still reachable" which is only true because the leak detection has to
	// run before such memory is cleaned up. doing an explicit clear helps to locate
	// real memory leaks
	denMessage::Pool().Clear();
	denRealMessage::Pool().Clear();
}
