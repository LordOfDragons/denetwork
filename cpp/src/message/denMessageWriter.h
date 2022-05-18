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
class denMessageWriter{
public:
	denMessageWriter(denMessage &message);
	~denMessageWriter() noexcept;
	
	denMessageWriter &WriteChar(int8_t value);
	denMessageWriter &WriteByte(uint8_t value);
	denMessageWriter &WriteShort(int16_t value);
	denMessageWriter &WriteUShort(uint16_t value);
	denMessageWriter &WriteInt(int32_t value);
	denMessageWriter &WriteUInt(uint32_t value);
	denMessageWriter &WriteLong(int64_t value);
	denMessageWriter &WriteULong(uint64_t value);
	denMessageWriter &WriteFloat(float value);
	denMessageWriter &WriteDouble(double value);
	denMessageWriter &WriteString8(const std::string &string);
	denMessageWriter &WriteString16(const std::string &string);
	denMessageWriter &WriteVector2(const denVector2 &vector);
	denMessageWriter &WriteVector3(const denVector3 &vector);
	denMessageWriter &WriteQuaternion(const denQuaternion &quaternion);
	denMessageWriter &WritePoint2(const denPoint2 &point);
	denMessageWriter &WritePoint3(const denPoint3 &point);
	denMessageWriter &WriteDVector(const denVector3 &vector);
	
	denMessageWriter &Write(const void *buffer, size_t length);
	denMessageWriter &Write(const denMessage &message);
	
private:
	denMessage &pMessage;
	std::ostringstream pStream;
};
