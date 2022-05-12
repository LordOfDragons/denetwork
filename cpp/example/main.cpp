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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <sstream>
#include <list>

#include <denetwork/denPool.h>
#include <denetwork/denRealMessage.h>
#include <denetwork/denConnection.h>
#include <denetwork/denLogger.h>
#include <denetwork/denServer.h>
#include <denetwork/value/denValueInteger.h>
#include <denetwork/value/denValueFloating.h>
#include <denetwork/value/denValueString.h>
#include <denetwork/value/denValueData.h>
#include <denetwork/message/denMessage.h>
#include <denetwork/message/denMessageWriter.h>
#include <denetwork/message/denMessageReader.h>
#include <denetwork/state/denState.h>

void clearScreen(){
	std::cout << "\x1B[2J";
}

void screenTopLeft(std::ostream &s){
	s << "\x1B[H";
}

enum class Color{black, red, green, yellow, blue, magenta, cyan, white};
enum class Style{regular, bold, low, italic, underline, blinking, reverse, background, invisible};

void resetColors(std::ostream &s){
	s << "\033[0m";
}

void foregroundColor(std::ostream &s, Color color, Style style){
	s << "\033[" << (int)style << ";" << 30 + (int)color << "m";
}

void backgroundColor(std::ostream &s, Color color, Style style){
	s << "\033[" << (int)style << ";" << 40 + (int)color << "m";
}

void printBar(std::ostream &s, int lineWidth, const std::string &title, int length, Color color){
	const int tlen = title.length();
	int i;
	
	resetColors(s);
	foregroundColor(s, Color::white, Style::bold);
	for(i=0; i<14-tlen; i++){
		s << ' ';
	}
	s << title << " [";
	
	resetColors(s);
	foregroundColor(s, color, Style::bold);
	for(i=0; i<length; i++){
		s << "\u2593";
	}
	
	resetColors(s);
	foregroundColor(s, color, Style::low);
	for(i=length; i<60; i++){
		s << "\u2591";
	}
	
	resetColors(s);
	foregroundColor(s, Color::white, Style::bold);
	s << "]" << std::string(lineWidth - 77, ' ') << std::endl;
}

void printString(std::ostream &s, int lineWidth, const std::string &title, const std::string &value, Color color){
	const int tlen = title.length();
	int i;
	
	resetColors(s);
	foregroundColor(s, Color::white, Style::bold);
	for(i=0; i<14-tlen; i++){
		s << ' ';
	}
	s << title << " '";
	
	resetColors(s);
	foregroundColor(s, color, Style::bold);
	s << value;
	
	resetColors(s);
	foregroundColor(s, Color::white, Style::bold);
	s << "'" << std::string(lineWidth - 17 - value.length(), ' ') << std::endl;
}

void nonblock(bool enable){
	struct termios ttystate;
	tcgetattr(STDIN_FILENO, &ttystate);
	
	if(enable){
		ttystate.c_lflag &= ~ICANON;
		ttystate.c_lflag &= ~ECHO;
		ttystate.c_cc[VMIN] = 1;
		std::cout << "\033[?25l";
		
	}else{
		ttystate.c_lflag |= ICANON;
		ttystate.c_lflag |= ECHO;
		std::cout << "\033[?25h";
	}
	
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

denPoint2 terminalSize(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return denPoint2(w.ws_col, w.ws_row);
}

bool kbhit(int timeoutms = 0){
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = (__suseconds_t)timeoutms * 1000;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

static std::list<std::string> logbuffer;

void addLog(const std::string &message){
	while(logbuffer.size() > 9) logbuffer.pop_back();
	logbuffer.push_front(message);
}

class ExampleLogger : public denLogger{
public:
	void Log(LogSeverity severity, const std::string &message) override{
		std::stringstream s;
		s << "[" << severityText(severity) << "] " << message;
		addLog(s.str());
	}
	
	const char *severityText(LogSeverity severity){
		switch(severity){
			case LogSeverity::error: return "EE";
			case LogSeverity::warning: return "WW";
			case LogSeverity::info: return "II";
			case LogSeverity::debug: return "DD";
		}
		return "";
	}
	
	static const std::shared_ptr<ExampleLogger> logger;
};
const std::shared_ptr<ExampleLogger> ExampleLogger::logger = std::make_shared<ExampleLogger>();

class ExampleConnection : public denConnection{
	static int nextid;
	
public:
	class OtherClientState : public denState{
	public:
		const int id;
		const denValueInt::Ref valueBar;
		
		OtherClientState(int id) :
		denState(true),
		id(id),
		valueBar(std::make_shared<denValueInt>(denValueIntegerFormat::sint16))
		{
			SetLogger(ExampleLogger::logger);
			AddValue(valueBar);
		}
	};
	
	int id;
	denState::Ref state;
	denValueInt::Ref valueBar;
	
	denState::Ref serverState;
	denValueString::Ref serverValueTime;
	denValueInt::Ref serverValueBar;
	
	std::list<denState::Ref> otherClientStates;
	
	ExampleConnection(bool readonly) : id(nextid++)
	{
		SetLogger(ExampleLogger::logger);
		state = std::make_shared<denState>(readonly);
		state->SetLogger(ExampleLogger::logger);
		valueBar = std::make_shared<denValueInt>(denValueIntegerFormat::sint16);
		valueBar->SetValue(30);
		state->AddValue(valueBar);
	}
	
	~ExampleConnection() override{
	}
	
	inline int getBar(){
		return valueBar->GetValue();
	}
	
	void setBar(int value){
		valueBar->SetValue((int16_t)std::max(std::min(value, 60), 0));
	}
	
	void incrementBar(int value){
		setBar(getBar() + value);
	}
	
	inline bool ready() const{
		return (bool)valueBar;
	}
	
	void ConnectionEstablished() override{
	}
	
	void ConnectionClosed() override{
		if(GetParentServer()){
			const denServer::Connections &connections = GetParentServer()->GetConnections();
			denServer::Connections::const_iterator iter;
			for(iter = connections.cbegin(); iter != connections.cend(); iter++){
				if(iter->get() != this){
					denMessage::Ref lm(denMessage::Pool().Get());
					denMessageWriter(lm->Item())
						.WriteUInt(0x12340004)
						.WriteUShort((uint16_t)id);
					(*iter)->SendReliableMessage(lm);
				}
			}
		}
	}
	
	void MessageProgress(size_t bytesReceived) override{
	}
	
	void MessageReceived(const denMessage::Ref &message) override{
		denMessageReader reader(message->Item());
		const int ident = reader.ReadUInt();
		
		switch(ident){
		case 0x12340004:{
			const int id = reader.ReadUShort();
			otherClientStates.remove_if([&](const denState::Ref &each){
				return static_cast<OtherClientState&>(*each).id == id;
			});
			}break;
		}
	}
	
	denState::Ref CreateState(const denMessage::Ref &message, bool readOnly) override{
		denMessageReader reader(message->Item());
		const int ident = reader.ReadUInt();
		
		std::stringstream s;
		addLog(s.str());
		
		switch(ident){
		case 0x12340001:
			// create local state for server state. read-only to us
			serverState = std::make_shared<denState>(readOnly);
			serverState->SetLogger(ExampleLogger::logger);
			serverValueTime = std::make_shared<denValueString>();
			serverValueBar = std::make_shared<denValueInt>(denValueIntegerFormat::sint16);
			serverState->AddValue(serverValueTime);
			serverState->AddValue(serverValueBar);
			return serverState;
			
		case 0x12340002:
			// use local state as client state. read-write to us
			return state;
			
		case 0x12340003:{
			// create other client state. read-only to us
			const int id = reader.ReadUShort();
			const denState::Ref otherState = std::make_shared<OtherClientState>(id);
			otherClientStates.push_back(otherState);
			return otherState;
			}
			
		default:
			return nullptr;
		}
	}
	
	inline bool hasServerState() const{
		return (bool)serverState;
	}
	
	inline const std::string &getServerTime(){
		return serverValueTime->GetValue();
	}
	
	inline int getServerBar(){
		return serverValueBar->GetValue();
	}
};
int ExampleConnection::nextid = 1;

class ExampleServer : public denServer{
public:
	denState::Ref state;
	denValueString::Ref valueTime;
	denValueInt::Ref valueBar;
	
	ExampleServer(){
		SetLogger(ExampleLogger::logger);
		state = std::make_shared<denState>(false);
		state->SetLogger(ExampleLogger::logger);
		valueTime = std::make_shared<denValueString>();
		valueBar = std::make_shared<denValueInt>(denValueIntegerFormat::sint16);
		valueBar->SetValue(30);
		state->AddValue(valueTime);
		state->AddValue(valueBar);
	}
	
	denConnection::Ref CreateConnection() override{
		return std::make_shared<ExampleConnection>(true);
	}
	
	inline const std::string &getTime() const{
		return valueTime->GetValue();
	}
	
	void setTime(const std::string &time){
		valueTime->SetValue(time);
	}
	
	void updateTime(){
		std::stringstream s;
		std::time_t t = std::time(nullptr);
		std::tm tm = *std::localtime(&t);
		s << std::put_time(&tm, "%c");
		setTime(s.str());
	}
	
	inline int getBar() const{
		return valueBar->GetValue();
	}
	
	void setBar(int value){
		valueBar->SetValue((int16_t)std::max(std::min(value, 60), 0));
	}
	
	void incrementBar(int value){
		setBar(getBar() + value);
	}
	
	void ClientConnected(const denConnection::Ref &connection) override{
		linkClient(*connection);
	}
	
	void linkClient(denConnection &connection){
		ExampleConnection &excon = static_cast<ExampleConnection&>(connection);
		
		// link server state. read-only on client side
		denMessage::Ref lm(denMessage::Pool().Get());
		denMessageWriter(lm->Item()).WriteUInt(0x12340001);
		connection.LinkState(lm, state, true);
		
		// link all client states. this includes connecting client itself
		Connections::const_iterator iter;
		for(iter = GetConnections().cbegin(); iter != GetConnections().cend(); iter++){
			if(iter->get() == &connection){
				// connecting client gets read-write state of its own state
				lm = denMessage::Pool().Get();
				denMessageWriter(lm->Item()).WriteUInt(0x12340002);
				connection.LinkState(lm, excon.state, false);
				
			}else{
				ExampleConnection &othercon = static_cast<ExampleConnection&>(**iter);
				
				// all other client states are read-only to the connecting client
				lm = denMessage::Pool().Get();
				denMessageWriter(lm->Item())
					.WriteUInt(0x12340003)
					.WriteUShort((uint16_t)othercon.id);
				connection.LinkState(lm, othercon.state, true);
				
				// and the connecting client state is also read-only to the others
				lm = denMessage::Pool().Get();
				denMessageWriter(lm->Item())
					.WriteUInt(0x12340003)
					.WriteUShort((uint16_t)excon.id);
				othercon.LinkState(lm, excon.state, true);
			}
		}
	}
};

class App{
	std::string paramListen, paramConnect;
	bool quit = false;
	denServer::Ref server;
	denConnection::Ref connection;
	
public:
	bool init(int argc, char *argv[]){
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
	
	void run(){
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
	
	void initScreen(){
		clearScreen();
		nonblock(true);
	}
	
	void exitScreen(){
		nonblock(false);
	}
	
	void serverListen(){
		server = std::make_shared<ExampleServer>();
		server->ListenOn(paramListen);
	}
	
	void clientConnect(){
		connection = std::make_shared<ExampleConnection>(false);
		connection->ConnectTo(paramConnect);
	}
	
	void drawScreen(){
		const denPoint2 size(terminalSize());
		std::stringstream s;
		screenTopLeft(s);
		
		if(server){
			const ExampleServer &exsrv = static_cast<ExampleServer&>(*server);
			printString(s, size.x, "Server Time", exsrv.getTime(), Color::red);
			printBar(s, size.x, "Server Bar", exsrv.getBar(), Color::red);
			
			denServer::Connections::const_iterator iterCon;
			for(iterCon = server->GetConnections().cbegin(); iterCon != server->GetConnections().cend(); iterCon++){
				ExampleConnection &excon = static_cast<ExampleConnection&>(**iterCon);
				if(excon.ready()){
					std::stringstream s2;
					s2 << "Client#" << excon.id << " Bar";
					printBar(s, size.x, s2.str(), excon.getBar(), Color::green);
				}
			}
		}
		
		if(connection){
			ExampleConnection &excon = static_cast<ExampleConnection&>(*connection);
			printBar(s, size.x, "Client Bar", excon.getBar(), Color::red);
			
			if(excon.hasServerState()){
				printString(s, size.x, "Server Time", excon.getServerTime(), Color::green);
				printBar(s, size.x, "Server Bar", excon.getServerBar(), Color::green);
			}
			
			std::list<denState::Ref>::const_iterator iter;
			for(iter = excon.otherClientStates.cbegin(); iter != excon.otherClientStates.cend(); iter++){
				ExampleConnection::OtherClientState &otsta = static_cast<ExampleConnection::OtherClientState&>(**iter);
				std::stringstream s2;
				s2 << "Client#" << otsta.id << " Bar";
				printBar(s, size.x, s2.str(), otsta.valueBar->GetValue(), Color::cyan);
			}
		}
		
		foregroundColor(s, Color::white, Style::bold);
		s << "Logs:" << std::string(size.x - 5, ' ') << std::endl;
		resetColors(s);
		std::list<std::string>::const_iterator iterLog;
		for(iterLog = logbuffer.cbegin(); iterLog != logbuffer.cend(); iterLog++){
			s << *iterLog << std::string(size.x - iterLog->length(), ' ') << std::endl;
		}
		
		std::cout << s.str();
	}
	
	void handleInput(){
		if(!kbhit()) return;
		
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
				if(server) static_cast<ExampleServer&>(*server).incrementBar(1);
				if(connection) static_cast<ExampleConnection&>(*connection).incrementBar(1);
				break;
				
			case 'D': // left arrow
				if(server) static_cast<ExampleServer&>(*server).incrementBar(-1);
				if(connection) static_cast<ExampleConnection&>(*connection).incrementBar(-1);
				break;
			}
		}
	}
	
	void updateServer(float elapsed){
		if(!server) return;
		static_cast<ExampleServer&>(*server).updateTime();
		server->Update(elapsed);
	}
	
	void updateClient(float elapsed){
		if(!connection) return;
		connection->Update(elapsed);
	}
	
	void appLoop(){
		auto start = std::chrono::steady_clock::now();
		
		while(!quit){
			const auto end = std::chrono::steady_clock::now();
			start = end;
			const float elapsed = 0.001f * std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			
			handleInput();
			updateServer(elapsed);
			updateClient(elapsed);
			drawScreen();
		}
	}
	
	void specialTest(){
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
};

void run(){
}

int main(int argc, char *argv[]){
	App app;
	if(app.init(argc, argv)){
		app.run();
	}
	return 0;
}
