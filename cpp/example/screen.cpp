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

#include "screen.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

void Screen::clearScreen(){
	std::cout << "\x1B[2J";
}

void Screen::screenTopLeft(std::ostream &s){
	s << "\x1B[H";
}

void Screen::resetColors(std::ostream &s){
	s << "\033[0m";
}

void Screen::foregroundColor(std::ostream &s, Color color, Style style){
	s << "\033[" << (int)style << ";" << 30 + (int)color << "m";
}

void Screen::backgroundColor(std::ostream &s, Color color, Style style){
	s << "\033[" << (int)style << ";" << 40 + (int)color << "m";
}

void Screen::printBar(std::ostream &s, int lineWidth, const std::string &title, int length, Color color){
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

void Screen::printString(std::ostream &s, int lineWidth, const std::string &title, const std::string &value, Color color){
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

denPoint2 Screen::terminalSize(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return denPoint2(w.ws_col, w.ws_row);
}
