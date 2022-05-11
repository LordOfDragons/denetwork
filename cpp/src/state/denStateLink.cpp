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
#include "denStateLink.h"
#include "../denConnection.h"


denStateLink::denStateLink(denConnection &connection, denState *state) :
pConnection(connection),
pState(state),
pIdentifier(-1),
pLinkState(State::down),
pChanged(false),
pValueChanged(state->GetValues().size(), false){
}

void denStateLink::SetIdentifier(int identifier){
	if(identifier < -1){
		throw std::invalid_argument("identifier < -1");
	}
	pIdentifier = identifier;
}

void denStateLink::SetLinkState(State linkState){
	pLinkState = linkState;
}

void denStateLink::SetChanged(bool changed){
	if(changed != pChanged){
		pChanged = changed;
		if( changed ){
			pConnection.AddModifiedStateLink(this);
		}
	}
}

bool denStateLink::GetValueChangedAt(int index) const{
	return pValueChanged[index];
}

void denStateLink::SetValueChangedAt(int index, bool changed){
	if(changed != pValueChanged[index]){
		pValueChanged[index] = changed;
		if(changed){
			SetChanged(true);
		}
	}
}

bool denStateLink::HasChangedValues() const{
	return std::find(pValueChanged.cbegin(), pValueChanged.cend(), true) != pValueChanged.cend();
}

void denStateLink::ResetChanged(){
	pChanged = false;
	std::fill(pValueChanged.begin(), pValueChanged.end(), false);
}
