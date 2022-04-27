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

#include "denValueData.h"
#include "../message/denMessageReader.h"
#include "../message/denMessageWriter.h"


denValueData::denValueData() :
denValue(Type::data)
{
	pDataType = denProtocol::ValueTypes::data;
}

void denValueData::Read(denMessageReader &reader){
	pValue.assign(reader.ReadUShort(), 0);
	reader.Read(pValue.data(), pValue.size());
	pLastValue = pValue;
}

void denValueData::Write(denMessageWriter &writer){
	writer.WriteUShort((uint16_t)pValue.size());
	writer.Write(pValue.data(), pValue.size());
}

bool denValueData::UpdateValue(bool force){
	if(!force && pValue == pLastValue){
		return false;
		
	}else{
		pLastValue = pValue;
		return true;
	}
}
