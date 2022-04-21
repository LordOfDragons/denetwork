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

#include <sstream>
#include "../math/denPoint2.h"
#include "../math/denPoint3.h"
#include "../math/denVector2.h"
#include "../math/denVector3.h"
#include "../math/denQuaternion.h"
#include "../message/denMessage.h"

/**
 * \brief Network message writer.
 */
class denMessageReader{
public:
	denMessageReader(denMessage &message);
	
	int8_t ReadChar();
	uint8_t ReadByte();
	int16_t ReadShort();
	uint16_t ReadUShort();
	int32_t ReadInt();
	uint32_t ReadUInt();
	int64_t ReadLong();
	uint64_t ReadULong();
	float ReadFloat();
	double ReadDouble();
	std::string ReadString();
	std::string ReadString8();
	std::string ReadString16();
	denVector2 ReadVector2();
	denVector3 ReadVector3();
	denQuaternion ReadQuaternion();
	denPoint2 ReadPoint2();
	denPoint3 ReadPoint3();
	denVector3 ReadDVector();
	
	void Read(void *buffer, size_t length);
	
private:
	std::istringstream pStream;
};
