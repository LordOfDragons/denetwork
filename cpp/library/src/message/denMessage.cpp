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

#include "denMessage.h"

denMessage::denMessage() :
pLength(0),
pTimestamp(std::chrono::system_clock::now()){
}

void denMessage::SetTimestamp(const Timestamp &timestamp){
	pTimestamp = timestamp;
}

void denMessage::SetData(const std::string &data){
	pData = data;
}

void denMessage::SetLength(size_t length){
	pLength = length;
	if(pData.size() < length){
		pData.assign(length, 0);
	}
}

void denMessage::SetLengthRetain(size_t length){
	const size_t prevLength = pLength;
	
	pLength = length;
	if(pData.size() >= length){
		return;
	}
	
	const std::string temp(pData);
	pData.assign(length, 0);
	if(prevLength > 0){
		temp.copy((char*)pData.c_str(), prevLength);
	}
}
