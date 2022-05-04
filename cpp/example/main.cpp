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
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <sstream>

#include <denetwork/denPool.h>
#include <denetwork/denRealMessage.h>
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

enum class Color{
	black = 0,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	white
};

enum class Style{
	regular,
	bold,
	low,
	italic,
	underline,
	blinking,
	reverse,
	background,
	invisible
};

void resetColors(std::ostream &s){
	s << "\033[0m";
}

void foregroundColor(std::ostream &s, Color color, Style style){
	s << "\033[" << (int)style << ";" << 30 + (int)color << "m";
}

void backgroundColor(std::ostream &s, Color color, Style style){
	s << "\033[" << (int)style << ";" << 40 + (int)color << "m";
}

void printBar(std::ostream &s, const std::string &title, int length, Color color){
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
	s << "]" << std::endl;
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

void dummy(){
	int bar1 = 12;
	int bar2 = 56;
	
	clearScreen();
	
	nonblock(true);
	
	bool quit = false;
	while(!quit){
		if(kbhit()){
			const int c = fgetc(stdin);
			switch(c){
			case 'q':
				quit = true;
				break;
				
			case 'w':
				if(bar1 > 0) bar1--;
				break;
				
			case 'e':
				if(bar1 < 60) bar1++;
				break;
				
			case '\033':
				fgetc(stdin); // '['
				switch(fgetc(stdin)){
				case 'A': //up arrow
					if(bar2 < 60) bar2++;
					break;
					
				case 'B': // down arrow
					if(bar2 > 0) bar2--;
					break;
					
				case 'C': // right arrow
					break;
					
				case 'D': // left arrow
					break;
				}
			}
		}
		
		std::stringstream s;
		screenTopLeft(s);
		printBar(s, "Value 1", bar1, Color::red);
		printBar(s, "Value 2", bar2, Color::green);
		printBar(s, "Value 3", 10, Color::blue);
		resetColors(s);
		std::cout << s.str();
	}
	nonblock(false);
	
	denState::Ref state(std::make_shared<denState>(false));
	
	denValueInt::Ref test(std::make_shared<denValueInt>(denValueIntegerFormat::uint32));
	test->SetValue(8);
	state->GetValues().push_back(test);
	
	denValueFloat::Ref test2(std::make_shared<denValueFloat>(denValueFloatingFormat::float32));
	test2->SetPrecision(0.01);
	test2->SetValue(8);
	state->GetValues().push_back(test2);
	
	denValueVector3::Ref test3(std::make_shared<denValueVector3>(denValueFloatingFormat::float32));
	test3->SetPrecision(denVector3(0.01));
	test3->SetValue(denVector3(1, 2, 3));
	state->GetValues().push_back(test3);
	
	denValuePoint2::Ref test4(std::make_shared<denValuePoint2>(denValueIntegerFormat::sint32));
	test4->SetValue(denPoint2(1, 2));
	state->GetValues().push_back(test4);
	
	denValueString::Ref test5(std::make_shared<denValueString>());
	test5->SetValue("this is a test");
	state->GetValues().push_back(test5);
	
	denValueData::Ref test6(std::make_shared<denValueData>());
	test6->SetValue(denValueData::Data{5, 2, 80, 45, 60, 30});
	state->GetValues().push_back(test6);
	
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

int main(int, char*[]){
	dummy();
}
