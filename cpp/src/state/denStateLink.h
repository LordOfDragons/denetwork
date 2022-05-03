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

#include <memory>
#include <vector>

class denConnection;
class denState;


/**
 * \brief Network state link.
 */
class denStateLink{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denStateLink> Ref;
	
	/** \brief Changed values list. */
	typedef std::vector<bool> ChangedValues;
	
	/** \brief Link states. */
	enum class State{
		down,
		listening,
		up
	};
	
	/** \brief Create state link. */
	denStateLink(denConnection &connection, denState &state);
	
	/** \brief State. */
	inline denState &GetState() const{ return pState; }
	
	/** \brief Unique identifier. */
	inline int GetIdentifier() const{ return pIdentifier; }
	
	/** \brief Set unique identifier. */
	void SetIdentifier(int identifier);
	
	/** \brief Link state. */
	inline State GetLinkState() const{ return pLinkState; }
	
	/** \brief Set link state. */
	void SetLinkState(State linkState);
	
	/** \brief Connection. */
	inline denConnection &GetConnection() const{ return pConnection; }
	
	/** \brief State link changed. */
	inline bool GetChanged() const{ return pChanged; }
	
	/** \brief Set if state link changed. */
	void SetChanged(bool changed);
	
	/** \brief Value changed. */
	bool GetValueChangedAt(int index) const;
	
	/** \brief Set if value changed. */
	void SetValueChangedAt(int index, bool changed);
	
	/** \brief One or more values are marked changed. */
	bool HasChangedValues() const;
	
	/** \brief Reset changed of all values and state link. */
	void ResetChanged();
	
private:
	denConnection &pConnection;
	denState &pState;
	
	int pIdentifier;
	State pLinkState;
	
	bool pChanged;
	
	std::vector<bool> pValueChanged;
};
