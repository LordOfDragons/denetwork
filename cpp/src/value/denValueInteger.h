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
#include "denValue.h"

/**
 * \brief Integer network state value.
 */
template<class T> class denValueInteger : public denValue{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<T>> Ref;
	
	/** \brief Create network value. */
	denValueInteger(Format format) : denValue(Type::integer), pFormat(format){
		switch(format){
		case Format::sint8:
		case Format::sint16:
		case Format::sint32:
		case Format::sint64:
		case Format::uint8:
		case Format::uint16:
		case Format::uint32:
		case Format::uint64:
			break;
			
		default:
			throw std::invalid_argument("format");
		}
	}
	
public:
	/** \brief Format. */
	inline Format GetFormat() const{
		return pFormat;
	}
	
	/** \brief Value. */
	inline T GetValue() const{
		return pValue;
	}
	
	/** \brief Set value. */
	inline void SetValue(T value){
		pValue = value;
	}
	
private:
	const Format pFormat;
	T pValue;
};

/**
 * \brief Single component integer value.
 */
typedef denValueInteger<uint64_t> denValueInteger1;
