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

#include <memory.h>
#include <stdexcept>
#include "denMessage.h"
#include "denMessageWriter.h"

denMessageWriter::denMessageWriter(denMessage &message) :
pMessage(message),
pStream(message.GetData()){
}

denMessageWriter::~denMessageWriter() noexcept{
	pMessage.SetData(pStream.str());
	pMessage.SetLength(pStream.tellp());
}

denMessageWriter &denMessageWriter::WriteChar(int8_t value){
	return Write(&value, 1);
}

denMessageWriter &denMessageWriter::WriteByte(uint8_t value){
	return Write(&value, 1);
}

denMessageWriter &denMessageWriter::WriteShort(int16_t value){
	uint16_t realValue;
	memcpy(&realValue, &value, 2);
	return WriteUShort(realValue);
}

denMessageWriter &denMessageWriter::WriteUShort(uint16_t value){
	uint8_t buffer[ 2 ];
	buffer[ 0 ] = (uint8_t)value;
	buffer[ 1 ] = (uint8_t)(value >> 8);
	return Write(buffer, 2);
}

denMessageWriter &denMessageWriter::WriteInt(int32_t value){
	uint32_t realValue;
	memcpy(&realValue, &value, 4);
	return WriteUInt(realValue);
}

denMessageWriter &denMessageWriter::WriteUInt(uint32_t value){
	uint8_t buffer[ 4 ];
	buffer[ 0 ] = (uint8_t)value;
	buffer[ 1 ] = (uint8_t)(value >> 8);
	buffer[ 2 ] = (uint8_t)(value >> 16);
	buffer[ 3 ] = (uint8_t)(value >> 24);
	return Write(buffer, 4);
}

denMessageWriter &denMessageWriter::WriteLong(int64_t value){
	uint64_t realValue;
	memcpy(&realValue, &value, 8);
	return WriteULong(realValue);
}

denMessageWriter &denMessageWriter::WriteULong(uint64_t value){
	uint8_t buffer[ 8 ];
	buffer[ 0 ] = (uint8_t)value;
	buffer[ 1 ] = (uint8_t)(value >> 8);
	buffer[ 2 ] = (uint8_t)(value >> 16);
	buffer[ 3 ] = (uint8_t)(value >> 24);
	buffer[ 4 ] = (uint8_t)(value >> 32);
	buffer[ 5 ] = (uint8_t)(value >> 40);
	buffer[ 6 ] = (uint8_t)(value >> 48);
	buffer[ 7 ] = (uint8_t)(value >> 56);
	return Write(buffer, 8);
}

denMessageWriter &denMessageWriter::WriteFloat(float value){
	uint32_t realValue;
	memcpy(&realValue, &value, 4);
	return WriteUInt(realValue);
}

denMessageWriter &denMessageWriter::WriteDouble(double value){
	uint64_t realValue;
	memcpy(&realValue, &value, 8);
	return WriteULong(realValue);
}

denMessageWriter &denMessageWriter::WriteString8(const std::string &string){
	const size_t len = string.size();
	if (len > 255) {
		throw std::invalid_argument("string length > 255");
	}
	WriteByte((uint8_t)len);
	return Write(string.c_str(), len);
}

denMessageWriter &denMessageWriter::WriteString16(const std::string &string){
	const size_t len = string.size();
	if (len > 65535) {
		throw std::invalid_argument("string length > 65535");
	}
	WriteUShort((uint16_t)len);
	return Write(string.c_str(), len);
}

denMessageWriter &denMessageWriter::WriteVector2(const denVector2 &vector){
	WriteFloat((float)vector.x);
	return WriteFloat((float)vector.y);
}

denMessageWriter &denMessageWriter::WriteVector3(const denVector3 &vector){
	WriteFloat((float)vector.x);
	WriteFloat((float)vector.y);
	return WriteFloat((float)vector.z);
}

denMessageWriter &denMessageWriter::WriteQuaternion(const denQuaternion &quaternion){
	WriteFloat((float)quaternion.x);
	WriteFloat((float)quaternion.y);
	WriteFloat((float)quaternion.z);
	return WriteFloat((float)quaternion.w);
}

denMessageWriter &denMessageWriter::WritePoint2(const denPoint2 &point){
	WriteInt((uint32_t)point.x);
	return WriteInt((uint32_t)point.y);
}

denMessageWriter &denMessageWriter::WritePoint3(const denPoint3 &point){
	WriteInt((uint32_t)point.x);
	WriteInt((uint32_t)point.y);
	return WriteInt((uint32_t)point.z);
}

denMessageWriter &denMessageWriter::WriteDVector(const denVector3 &vector){
	WriteDouble(vector.x);
	WriteDouble(vector.y);
	return WriteDouble(vector.z);
}

denMessageWriter &denMessageWriter::Write(const void *buffer, size_t length){
	pStream.write((const char*)buffer, length);
	return *this;
}

denMessageWriter & denMessageWriter::Write(const denMessage &message){
	return Write(message.GetData().c_str(), message.GetLength());
}
