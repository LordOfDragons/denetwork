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

#include <stdint.h>
#include <vector>
#include "denValue.h"

/**
 * \brief Data network state value.
 */
class denValueData : public denValue{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueData> Ref;
	
	/** \brief Data type. */
	typedef std::vector<uint8_t> Data;
	
	/** \brief Create network value. */
	denValueData();
	
public:
	/** \brief Value. */
	inline const Data &GetValue() const{
		return pValue;
	}
	
	/** \brief Set value. */
	void SetValue(const Data &value);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
	
	/**
	 * \brief Update value.
	 * \returns true if value needs to by synchronized otherwise false if not changed enough.
	 */
	bool UpdateValue(bool force) override;
	
private:
	Data pValue;
	Data pLastValue;
};
