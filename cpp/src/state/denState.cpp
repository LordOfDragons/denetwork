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

#include <algorithm>
#include "denState.h"
#include "../denProtocolEnums.h"
#include "../message/denMessageReader.h"
#include "../message/denMessageWriter.h"

denState::denState(bool readOnly) :
pReadOnly(readOnly){
}

denState::StateLinks::iterator denState::FindLink(denStateLink *link){
	return std::find_if(pLinks.begin(), pLinks.end(), [&](const denStateLink::Ref &each){
		return each.get() == link;
	});
}

denState::StateLinks::const_iterator denState::FindLink(denStateLink *link) const{
	return std::find_if(pLinks.cbegin(), pLinks.cend(), [&](const denStateLink::Ref &each){
		return each.get() == link;
	});
}

void denState::AddValue(const denValue::Ref &value){
	if(!value){
		throw std::invalid_argument("value is nullptr");
	}
	
	pValues.push_back(value);
	
	value->pState = this;
	value->pIndex = pValues.size() - 1;
}

void denState::RemoveValue(const denValue::Ref &value){
	Values::iterator iter(std::find(pValues.begin(), pValues.end(), value));
	if(iter == pValues.end()){
		throw std::invalid_argument("value absent");
	}
	
	(*iter)->pState = nullptr;
	pValues.erase(iter);
	
	size_t index = 0;
	for(iter = pValues.begin(); iter != pValues.end(); iter++){
		(*iter)->pIndex = index++;
	}
}

void denState::SetLogger(const denLogger::Ref &logger){
	pLogger = logger;
}

void denState::Update(){
}

void denState::LinkReadValues(denMessageReader &reader, denStateLink &link){
	const size_t count = (size_t)reader.ReadByte();
	size_t i;
	
	for(i=0; i<count; i++){
		const int index = reader.ReadUShort();
		if(index < 0 || index >= (int)pValues.size()){
			throw std::invalid_argument("index out of range");
		}
		
		pValues[index]->Read(reader);
		InvalidateValueAtExcept(index, link);
		
		ValueChanged(*pValues[index]);
	}
	
	link.SetChanged(link.HasChangedValues());
}

void denState::LinkReadAllValues(denMessageReader &reader, denStateLink &link){
	const size_t count = (size_t)pValues.size();
	size_t i;
	
	for(i=0; i<count; i++){
		pValues[ i ]->Read(reader);
		InvalidateValueAt(i);
		
		ValueChanged(*pValues[ i ]);
	}
	
	if(!link.HasChangedValues()){
		link.SetChanged(false);
	}
}

bool denState::LinkReadAndVerifyAllValues(denMessageReader &reader){
	const size_t count = (size_t)pValues.size();
	if(count != (size_t)reader.ReadUShort()){
		throw std::invalid_argument("count out of range");
	}
	
	size_t i;
	for(i=0; i<count; i++){
		const denProtocol::ValueTypes type = (denProtocol::ValueTypes)reader.ReadByte();
		if(type != pValues[i]->GetDataType()){
			throw std::invalid_argument("data type mismatch");
		}
		
		pValues[i]->Read(reader);
		InvalidateValueAt(i);
		
		ValueChanged(*pValues[i]);
	}
	
	return i == count;
}

void denState::LinkWriteValues(denMessageWriter &writer){
	Values::const_iterator iter;
	for(iter = pValues.cbegin(); iter != pValues.cend(); iter++){
		(*iter)->Write(writer);
	}
}

void denState::LinkWriteValuesWithVerify(denMessageWriter &writer){
	writer.WriteUShort((uint16_t)pValues.size());
	Values::const_iterator iter;
	for(iter = pValues.cbegin(); iter != pValues.cend(); iter++){
		writer.WriteByte((uint8_t)(*iter)->GetDataType());
		(*iter)->Write(writer);
	}
}

void denState::LinkWriteValues(denMessageWriter &writer, denStateLink &link){
	const int count = (int)pValues.size();
	int i, changedCount = 0;
	for(i=0; i<count; i++ ){
		if(link.GetValueChangedAt(i)){
			changedCount++;
		}
	}
	changedCount = std::min(changedCount, 255);
	
	writer.WriteByte((uint8_t)changedCount);
	
	for(i=0; i<count; i++){
		if(!link.GetValueChangedAt(i)){
			continue;
		}
		
		writer.WriteUShort((uint16_t)i);
		pValues[i]->Write(writer);
		
		link.SetValueChangedAt(i, false);
		
		changedCount--;
		if(changedCount == 0){
			break;
		}
	}
	
	link.SetChanged(link.HasChangedValues());
}

void denState::InvalidateValueAt(size_t index){
	StateLinks::const_iterator iter;
	for(iter = pLinks.cbegin(); iter != pLinks.cend(); iter++){
		(*iter)->SetValueChangedAt(index, true);
	}
}

void denState::InvalidateValueAtExcept(size_t index, denStateLink &link){
	StateLinks::const_iterator iter;
	for(iter = pLinks.cbegin(); iter != pLinks.cend(); iter++){
		(*iter)->SetValueChangedAt(index, iter->get() != &link);
	}
}

void denState::ValueChanged(denValue &value){
	if(value.UpdateValue(false)){
		InvalidateValueAt(value.pIndex);
	}
}
