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

#include "value/denValueInteger.h"
#include "value/denValueFloating.h"
#include "value/denValueString.h"
#include "value/denValueData.h"
#include "state/denState.h"

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
}
