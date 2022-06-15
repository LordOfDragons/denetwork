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

#include "denValueInteger.h"
#include "../message/denMessageReader.h"
#include "../message/denMessageWriter.h"


denValueInt::denValueInt(denValueIntegerFormat format) :
denValueInteger<uint64_t>(Type::integer, format)
{
	switch(format){
	case denValueIntegerFormat::sint8:
		pDataType = denProtocol::ValueTypes::sint8;
		break;
		
	case denValueIntegerFormat::uint8:
		pDataType = denProtocol::ValueTypes::uint8;
		break;
		
	case denValueIntegerFormat::sint16:
		pDataType = denProtocol::ValueTypes::sint16;
		break;
		
	case denValueIntegerFormat::uint16:
		pDataType = denProtocol::ValueTypes::uint16;
		break;
		
	case denValueIntegerFormat::sint32:
		pDataType = denProtocol::ValueTypes::sint32;
		break;
		
	case denValueIntegerFormat::uint32:
		pDataType = denProtocol::ValueTypes::uint32;
		break;
		
	case denValueIntegerFormat::sint64:
		pDataType = denProtocol::ValueTypes::sint64;
		break;
		
	case denValueIntegerFormat::uint64:
		pDataType = denProtocol::ValueTypes::uint64;
		break;
	};
}

void denValueInt::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueIntegerFormat::sint8:
		pLastValue = reader.ReadChar();
		break;
		
	case denValueIntegerFormat::uint8:
		pLastValue = reader.ReadByte();
		break;
		
	case denValueIntegerFormat::sint16:
		pLastValue = reader.ReadShort();
		break;
		
	case denValueIntegerFormat::uint16:
		pLastValue = reader.ReadUShort();
		break;
		
	case denValueIntegerFormat::sint32:
		pLastValue = reader.ReadInt();
		break;
		
	case denValueIntegerFormat::uint32:
		pLastValue = reader.ReadUInt();
		break;
		
	case denValueIntegerFormat::sint64:
		pLastValue = reader.ReadLong();
		break;
		
	case denValueIntegerFormat::uint64:
		pLastValue = reader.ReadULong();
		break;
	};
	
	pValue = pLastValue;
}

void denValueInt::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueIntegerFormat::sint8:
		writer.WriteChar((int8_t)pValue);
		break;
		
	case denValueIntegerFormat::uint8:
		writer.WriteByte((uint8_t)pValue);
		break;
		
	case denValueIntegerFormat::sint16:
		writer.WriteShort((int16_t)pValue);
		break;
		
	case denValueIntegerFormat::uint16:
		writer.WriteUShort((uint16_t)pValue);
		break;
		
	case denValueIntegerFormat::sint32:
		writer.WriteInt((int32_t)pValue);
		break;
		
	case denValueIntegerFormat::uint32:
		writer.WriteUInt((uint32_t)pValue);
		break;
		
	case denValueIntegerFormat::sint64:
		writer.WriteLong((int64_t)pValue);
		break;
		
	case denValueIntegerFormat::uint64:
		writer.WriteULong((uint64_t)pValue);
		break;
	}
}


denValuePoint2::denValuePoint2(denValueIntegerFormat format) :
denValueInteger<denPoint2>(Type::vector2, format)
{
	switch(format){
	case denValueIntegerFormat::sint8:
		pDataType = denProtocol::ValueTypes::point2S8;
		break;
		
	case denValueIntegerFormat::uint8:
		pDataType = denProtocol::ValueTypes::point2U8;
		break;
		
	case denValueIntegerFormat::sint16:
		pDataType = denProtocol::ValueTypes::point2S16;
		break;
		
	case denValueIntegerFormat::uint16:
		pDataType = denProtocol::ValueTypes::point2U16;
		break;
		
	case denValueIntegerFormat::sint32:
		pDataType = denProtocol::ValueTypes::point2S32;
		break;
		
	case denValueIntegerFormat::uint32:
		pDataType = denProtocol::ValueTypes::point2U32;
		break;
		
	case denValueIntegerFormat::sint64:
		pDataType = denProtocol::ValueTypes::point2S64;
		break;
		
	case denValueIntegerFormat::uint64:
		pDataType = denProtocol::ValueTypes::point2U64;
		break;
	};
}

void denValuePoint2::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueIntegerFormat::sint8:
		pLastValue.x = reader.ReadChar();
		pLastValue.y = reader.ReadChar();
		break;
		
	case denValueIntegerFormat::uint8:
		pLastValue.x = reader.ReadByte();
		pLastValue.y = reader.ReadByte();
		break;
		
	case denValueIntegerFormat::sint16:
		pLastValue.x = reader.ReadShort();
		pLastValue.y = reader.ReadShort();
		break;
		
	case denValueIntegerFormat::uint16:
		pLastValue.x = reader.ReadUShort();
		pLastValue.y = reader.ReadUShort();
		break;
		
	case denValueIntegerFormat::sint32:
		pLastValue.x = reader.ReadInt();
		pLastValue.y = reader.ReadInt();
		break;
		
	case denValueIntegerFormat::uint32:
		pLastValue.x = reader.ReadUInt();
		pLastValue.y = reader.ReadUInt();
		break;
		
	case denValueIntegerFormat::sint64:
		pLastValue.x = reader.ReadLong();
		pLastValue.y = reader.ReadLong();
		break;
		
	case denValueIntegerFormat::uint64:
		pLastValue.x = reader.ReadULong();
		pLastValue.y = reader.ReadULong();
		break;
	};
	
	pValue = pLastValue;
}

void denValuePoint2::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueIntegerFormat::sint8:
		writer.WriteChar((int8_t)pValue.x);
		writer.WriteChar((int8_t)pValue.y);
		break;
		
	case denValueIntegerFormat::uint8:
		writer.WriteByte((uint8_t)pValue.x);
		writer.WriteByte((uint8_t)pValue.y);
		break;
		
	case denValueIntegerFormat::sint16:
		writer.WriteShort((int16_t)pValue.x);
		writer.WriteShort((int16_t)pValue.y);
		break;
		
	case denValueIntegerFormat::uint16:
		writer.WriteUShort((uint16_t)pValue.x);
		writer.WriteUShort((uint16_t)pValue.y);
		break;
		
	case denValueIntegerFormat::sint32:
		writer.WriteInt((int32_t)pValue.x);
		writer.WriteInt((int32_t)pValue.y);
		break;
		
	case denValueIntegerFormat::uint32:
		writer.WriteUInt((uint32_t)pValue.x);
		writer.WriteUInt((uint32_t)pValue.y);
		break;
		
	case denValueIntegerFormat::sint64:
		writer.WriteLong((int64_t)pValue.x);
		writer.WriteLong((int64_t)pValue.y);
		break;
		
	case denValueIntegerFormat::uint64:
		writer.WriteULong((uint64_t)pValue.x);
		writer.WriteULong((uint64_t)pValue.y);
		break;
	}
}


denValuePoint3::denValuePoint3(denValueIntegerFormat format) :
denValueInteger<denPoint3>(Type::vector3, format)
{
	switch(format){
	case denValueIntegerFormat::sint8:
		pDataType = denProtocol::ValueTypes::point3S8;
		break;
		
	case denValueIntegerFormat::uint8:
		pDataType = denProtocol::ValueTypes::point3U8;
		break;
		
	case denValueIntegerFormat::sint16:
		pDataType = denProtocol::ValueTypes::point3S16;
		break;
		
	case denValueIntegerFormat::uint16:
		pDataType = denProtocol::ValueTypes::point3U16;
		break;
		
	case denValueIntegerFormat::sint32:
		pDataType = denProtocol::ValueTypes::point3S32;
		break;
		
	case denValueIntegerFormat::uint32:
		pDataType = denProtocol::ValueTypes::point3U32;
		break;
		
	case denValueIntegerFormat::sint64:
		pDataType = denProtocol::ValueTypes::point3S64;
		break;
		
	case denValueIntegerFormat::uint64:
		pDataType = denProtocol::ValueTypes::point3U64;
		break;
	};
}

void denValuePoint3::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueIntegerFormat::sint8:
		pLastValue.x = reader.ReadChar();
		pLastValue.y = reader.ReadChar();
		pLastValue.z = reader.ReadChar();
		break;
		
	case denValueIntegerFormat::uint8:
		pLastValue.x = reader.ReadByte();
		pLastValue.y = reader.ReadByte();
		pLastValue.z = reader.ReadByte();
		break;
		
	case denValueIntegerFormat::sint16:
		pLastValue.x = reader.ReadShort();
		pLastValue.y = reader.ReadShort();
		pLastValue.z = reader.ReadShort();
		break;
		
	case denValueIntegerFormat::uint16:
		pLastValue.x = reader.ReadUShort();
		pLastValue.y = reader.ReadUShort();
		pLastValue.z = reader.ReadUShort();
		break;
		
	case denValueIntegerFormat::sint32:
		pLastValue.x = reader.ReadInt();
		pLastValue.y = reader.ReadInt();
		pLastValue.z = reader.ReadInt();
		break;
		
	case denValueIntegerFormat::uint32:
		pLastValue.x = reader.ReadUInt();
		pLastValue.y = reader.ReadUInt();
		pLastValue.z = reader.ReadUInt();
		break;
		
	case denValueIntegerFormat::sint64:
		pLastValue.x = reader.ReadLong();
		pLastValue.y = reader.ReadLong();
		pLastValue.z = reader.ReadLong();
		break;
		
	case denValueIntegerFormat::uint64:
		pLastValue.x = reader.ReadULong();
		pLastValue.y = reader.ReadULong();
		pLastValue.z = reader.ReadULong();
		break;
	};
	
	pValue = pLastValue;
}

void denValuePoint3::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueIntegerFormat::sint8:
		writer.WriteChar((int8_t)pValue.x);
		writer.WriteChar((int8_t)pValue.y);
		writer.WriteChar((int8_t)pValue.z);
		break;
		
	case denValueIntegerFormat::uint8:
		writer.WriteByte((uint8_t)pValue.x);
		writer.WriteByte((uint8_t)pValue.y);
		writer.WriteByte((uint8_t)pValue.z);
		break;
		
	case denValueIntegerFormat::sint16:
		writer.WriteShort((int16_t)pValue.x);
		writer.WriteShort((int16_t)pValue.y);
		writer.WriteShort((int16_t)pValue.z);
		break;
		
	case denValueIntegerFormat::uint16:
		writer.WriteUShort((uint16_t)pValue.x);
		writer.WriteUShort((uint16_t)pValue.y);
		writer.WriteUShort((uint16_t)pValue.z);
		break;
		
	case denValueIntegerFormat::sint32:
		writer.WriteInt((int32_t)pValue.x);
		writer.WriteInt((int32_t)pValue.y);
		writer.WriteInt((int32_t)pValue.z);
		break;
		
	case denValueIntegerFormat::uint32:
		writer.WriteUInt((uint32_t)pValue.x);
		writer.WriteUInt((uint32_t)pValue.y);
		writer.WriteUInt((uint32_t)pValue.z);
		break;
		
	case denValueIntegerFormat::sint64:
		writer.WriteLong((int64_t)pValue.x);
		writer.WriteLong((int64_t)pValue.y);
		writer.WriteLong((int64_t)pValue.z);
		break;
		
	case denValueIntegerFormat::uint64:
		writer.WriteULong((uint64_t)pValue.x);
		writer.WriteULong((uint64_t)pValue.y);
		writer.WriteULong((uint64_t)pValue.z);
		break;
	}
}
