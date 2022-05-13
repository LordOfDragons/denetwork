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
#include <ostream>

#include <denetwork/math/denPoint2.h>

class Screen{
public:
	enum class Color{black, red, green, yellow, blue, magenta, cyan, white};
	enum class Style{regular, bold, low, italic, underline, blinking, reverse, background, invisible};
	
	void clearScreen();
	void screenTopLeft(std::ostream &s);
	void resetColors(std::ostream &s);
	void foregroundColor(std::ostream &s, Color color, Style style);
	void backgroundColor(std::ostream &s, Color color, Style style);
	void printBar(std::ostream &s, int lineWidth, const std::string &title, int length, Color color);
	void printString(std::ostream &s, int lineWidth, const std::string &title, const std::string &value, Color color);
	denPoint2 terminalSize();
};
