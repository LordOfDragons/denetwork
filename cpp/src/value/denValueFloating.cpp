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

#include "denValueFloating.h"
#include "../message/denMessageReader.h"
#include "../message/denMessageWriter.h"
#include "../half/half.h"


denValueFloat::denValueFloat(denValueFloatingFormat format) :
denValueFloating<double>(Type::floating, format, DENM_THRESHOLD_DOUBLE){
}

void denValueFloat::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		pLastValue = halfToFloat(reader.ReadUShort());
		break;
		
	case denValueFloatingFormat::float32:
		pLastValue = reader.ReadFloat();
		break;
		
	case denValueFloatingFormat::float64:
		pLastValue = reader.ReadDouble();
		break;
	};
	
	pValue = pLastValue;
}

void denValueFloat::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		writer.WriteUShort(floatToHalf((float)pValue));
		break;
		
	case denValueFloatingFormat::float32:
		writer.WriteFloat((float)pValue);
		break;
		
	case denValueFloatingFormat::float64:
		writer.WriteDouble(pValue);
		break;
	}
}


denValueVector2::denValueVector2(denValueFloatingFormat format) :
denValueFloating<denVector2>(Type::vector2, format, DENM_THRESHOLD_DOUBLE){
}

void denValueVector2::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		pLastValue.x = halfToFloat(reader.ReadUShort());
		pLastValue.y = halfToFloat(reader.ReadUShort());
		break;
		
	case denValueFloatingFormat::float32:
		pLastValue.x = reader.ReadFloat();
		pLastValue.y = reader.ReadFloat();
		break;
		
	case denValueFloatingFormat::float64:
		pLastValue.x = reader.ReadDouble();
		pLastValue.y = reader.ReadDouble();
		break;
	};
	
	pValue = pLastValue;
}

void denValueVector2::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		writer.WriteUShort(floatToHalf((float)pValue.x));
		writer.WriteUShort(floatToHalf((float)pValue.y));
		break;
		
	case denValueFloatingFormat::float32:
		writer.WriteFloat((float)pValue.x);
		writer.WriteFloat((float)pValue.y);
		break;
		
	case denValueFloatingFormat::float64:
		writer.WriteDouble(pValue.x);
		writer.WriteDouble(pValue.y);
		break;
	}
}


denValueVector3::denValueVector3(denValueFloatingFormat format) :
denValueFloating<denVector3>(Type::vector3, format, DENM_THRESHOLD_DOUBLE){
}

void denValueVector3::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		pLastValue.x = halfToFloat(reader.ReadUShort());
		pLastValue.y = halfToFloat(reader.ReadUShort());
		pLastValue.z = halfToFloat(reader.ReadUShort());
		break;
		
	case denValueFloatingFormat::float32:
		pLastValue.x = reader.ReadFloat();
		pLastValue.y = reader.ReadFloat();
		pLastValue.z = reader.ReadFloat();
		break;
		
	case denValueFloatingFormat::float64:
		pLastValue.x = reader.ReadDouble();
		pLastValue.y = reader.ReadDouble();
		pLastValue.z = reader.ReadDouble();
		break;
	};
	
	pValue = pLastValue;
}

void denValueVector3::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		writer.WriteUShort(floatToHalf((float)pValue.x));
		writer.WriteUShort(floatToHalf((float)pValue.y));
		writer.WriteUShort(floatToHalf((float)pValue.z));
		break;
		
	case denValueFloatingFormat::float32:
		writer.WriteFloat((float)pValue.x);
		writer.WriteFloat((float)pValue.y);
		writer.WriteFloat((float)pValue.z);
		break;
		
	case denValueFloatingFormat::float64:
		writer.WriteDouble(pValue.x);
		writer.WriteDouble(pValue.y);
		writer.WriteDouble(pValue.z);
		break;
	}
}


denValueQuaternion::denValueQuaternion(denValueFloatingFormat format) :
denValueFloating<denQuaternion>(Type::quaternion, format, DENM_THRESHOLD_DOUBLE){
}

void denValueQuaternion::Read(denMessageReader &reader){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		pLastValue.x = halfToFloat(reader.ReadUShort());
		pLastValue.y = halfToFloat(reader.ReadUShort());
		pLastValue.z = halfToFloat(reader.ReadUShort());
		pLastValue.w = halfToFloat(reader.ReadUShort());
		break;
		
	case denValueFloatingFormat::float32:
		pLastValue.x = reader.ReadFloat();
		pLastValue.y = reader.ReadFloat();
		pLastValue.z = reader.ReadFloat();
		pLastValue.w = reader.ReadFloat();
		break;
		
	case denValueFloatingFormat::float64:
		pLastValue.x = reader.ReadDouble();
		pLastValue.y = reader.ReadDouble();
		pLastValue.z = reader.ReadDouble();
		pLastValue.w = reader.ReadDouble();
		break;
	};
	
	pValue = pLastValue;
}

void denValueQuaternion::Write(denMessageWriter &writer){
	switch(pFormat){
	case denValueFloatingFormat::float16:
		writer.WriteUShort(floatToHalf((float)pValue.x));
		writer.WriteUShort(floatToHalf((float)pValue.y));
		writer.WriteUShort(floatToHalf((float)pValue.z));
		writer.WriteUShort(floatToHalf((float)pValue.w));
		break;
		
	case denValueFloatingFormat::float32:
		writer.WriteFloat((float)pValue.x);
		writer.WriteFloat((float)pValue.y);
		writer.WriteFloat((float)pValue.z);
		writer.WriteFloat((float)pValue.w);
		break;
		
	case denValueFloatingFormat::float64:
		writer.WriteDouble(pValue.x);
		writer.WriteDouble(pValue.y);
		writer.WriteDouble(pValue.z);
		writer.WriteDouble(pValue.w);
		break;
	}
}
