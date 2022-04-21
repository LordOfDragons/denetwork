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
#include <sstream>
#include <stdexcept>
#include "denMessageReader.h"

denMessageReader::denMessageReader(denMessage &message) :
pStream(message.GetData()){
}

int8_t denMessageReader::ReadChar(){
	int8_t value;
	Read(&value, 1);
	return value;
}

uint8_t denMessageReader::ReadByte(){
	uint8_t value;
	Read(&value, 1);
	return value;
}

int16_t denMessageReader::ReadShort(){
	const uint16_t value = ReadUShort();
	int16_t realValue;
	memcpy(&realValue, &value, 2);
	return realValue;
}

uint16_t denMessageReader::ReadUShort(){
	uint8_t value[ 2 ];
	Read(&value, 2);
	return (uint16_t)value[ 0 ]
		| ((uint16_t)value[ 1 ] << 8);
}

int32_t denMessageReader::ReadInt(){
	const uint32_t value = ReadUInt();
	int32_t realValue;
	memcpy(&realValue, &value, 4);
	return realValue;
}

uint32_t denMessageReader::ReadUInt(){
	uint8_t value[ 4 ];
	Read(&value, 4);
	return (uint32_t)value[ 0 ]
		| ((uint32_t)value[ 1 ] << 8)
		| ((uint32_t)value[ 2 ] << 16)
		| ((uint32_t)value[ 3 ] << 24);
}

int64_t denMessageReader::ReadLong(){
	const uint64_t value = ReadULong();
	int64_t realValue;
	memcpy(&realValue, &value, 8);
	return realValue;
}

uint64_t denMessageReader::ReadULong(){
	uint8_t value[ 8 ];
	Read(&value, 8);
	return (uint64_t)value[ 0 ]
		| ((uint64_t)value[ 1 ] << 8)
		| ((uint64_t)value[ 2 ] << 16)
		| ((uint64_t)value[ 3 ] << 24)
		| ((uint64_t)value[ 4 ] << 32)
		| ((uint64_t)value[ 5 ] << 40)
		| ((uint64_t)value[ 6 ] << 48)
		| ((uint64_t)value[ 7 ] << 56);
}

float denMessageReader::ReadFloat(){
	const uint32_t value = ReadUInt();
	float realValue;
	memcpy(&realValue, &value, 4);
	return realValue;
}

double denMessageReader::ReadDouble(){
	const uint64_t value = ReadULong();
	double realValue;
	memcpy(&realValue, &value, 8);
	return realValue;
}

std::string denMessageReader::ReadString8(){
	std::string string;
	const size_t len = ReadByte();
	string.assign(' ', len);
	Read((char*)string.c_str(), len);
	return string;
}

std::string denMessageReader::ReadString16(){
	std::string string;
	const size_t len = ReadUShort();
	string.assign(' ', len);
	Read((char*)string.c_str(), len);
	return string;
}

denVector3 denMessageReader::ReadVector3(){
	denVector3 vector;
	vector.x = ReadFloat();
	vector.y = ReadFloat();
	vector.z = ReadFloat();
	return vector;
}

denVector2 denMessageReader::ReadVector2(){
	denVector2 vector;
	vector.x = ReadFloat();
	vector.y = ReadFloat();
	return vector;
}

denQuaternion denMessageReader::ReadQuaternion(){
	denQuaternion quaternion;
	quaternion.x = ReadFloat();
	quaternion.y = ReadFloat();
	quaternion.z = ReadFloat();
	quaternion.w = ReadFloat();
	return quaternion;
}

denPoint2 denMessageReader::ReadPoint2(){
	denPoint2 point;
	point.x = ReadInt();
	point.y = ReadInt();
	return point;
}

denPoint3 denMessageReader::ReadPoint3(){
	denPoint3 point;
	point.x = ReadInt();
	point.y = ReadInt();
	point.z = ReadInt();
	return point;
}

denVector3 denMessageReader::ReadDVector(){
	denVector3 vector;
	vector.x = ReadDouble();
	vector.y = ReadDouble();
	vector.z = ReadDouble();
	return vector;
}

void denMessageReader::Read(void *buffer, size_t length){
	pStream.read((char*)buffer, length);
}
