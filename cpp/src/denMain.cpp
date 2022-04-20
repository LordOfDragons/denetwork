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

#include "denState.h"
#include "denPool.h"
#include "value/denValueInteger.h"
#include "value/denValueFloating.h"
#include "value/denValueString.h"
#include "value/denValueData.h"
#include "message/denMessage.h"
#include "message/denMessageWriter.h"
#include "message/denMessageReader.h"
#include "internal/denRealMessage.h"

void dummy(){
	denState::Ref state(std::make_shared<denState>(false));
	
	denValueInt::Ref test(std::make_shared<denValueInt>(denValueIntegerFormat::uint32));
	test->SetValue(8);
	state->GetValues().push_back(test);
	
	denValueFloat::Ref test2(std::make_shared<denValueFloat>(denValueFloatingFormat::float32));
	test2->SetPrecision(0.01);
	test2->SetValue(8);
	state->GetValues().push_back(test2);
	
	denValueVector3::Ref test3(std::make_shared<denValueVector3>(denValueFloatingFormat::float32));
	test3->SetPrecision(denVector3(0.01));
	test3->SetValue(denVector3(1, 2, 3));
	state->GetValues().push_back(test3);
	
	denValuePoint2::Ref test4(std::make_shared<denValuePoint2>(denValueIntegerFormat::sint32));
	test4->SetValue(denPoint2(1, 2));
	state->GetValues().push_back(test4);
	
	denValueString::Ref test5(std::make_shared<denValueString>());
	test5->SetValue("this is a test");
	state->GetValues().push_back(test5);
	
	denValueData::Ref test6(std::make_shared<denValueData>());
	test6->SetValue(denValueData::Data{5, 2, 80, 45, 60, 30});
	state->GetValues().push_back(test6);
	
	{
	denMessage::Ref message(denMessage::Pool().Get());
	
	{
	denMessageWriter writer(message);
	writer.WriteUShort(80).WriteInt(210).WriteUInt(99443).WriteFloat(1.34f);
	}
	
	{
	message->Item().GetData().seekg(0);
	denMessageReader reader(message);
	const uint16_t mv1 = reader.ReadUShort();
	const int32_t mv2 = reader.ReadInt();
	const uint32_t mv3 = reader.ReadUInt();
	const float mv4 = reader.ReadFloat();
	printf("message: %d %d %d %g\n", mv1, mv2, mv3, mv4);
	}
	
	{
	denRealMessage::Ref realMessage(denRealMessage::Pool().Get());
	realMessage->Item().message = message;
	}
	}
	
	// just if somebody uses leak detection. pools are static class members and as such
	// they are cleaned up during application destruction. leak detection can notice
	// them as "still reachable" which is only true because the leak detection has to
	// run before such memory is cleaned up. doing an explicit clear helps to locate
	// real memory leaks
	denMessage::Pool().Clear();
	denRealMessage::Pool().Clear();
}

int main(int, char*[]){
	dummy();
}
