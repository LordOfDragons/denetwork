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

#include "denValue.h"

/**
 * \brief Floating network state value.
 */
template<class T> class denValueFloating : public denValue{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueFloating<T>> Ref;
	
	/** \brief Create network value. */
	denValueFloating(Format format) : denValue(Type::integer), pFormat(format){
		switch(format){
		case Format::float16:
		case Format::float32:
		case Format::float64:
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
	
	/** \brief Precision. */
	inline T GetPrecision() const{
		return pPrecision;
	}
	
	/** \brief Set precision. */
	inline void SetPrecision(T precision){
		pPrecision = std::max(precision, (T)0);
	}
	
private:
	const Format pFormat;
	T pValue;
	T pPrecision;
};

/**
 * \brief Single component floating value.
 */
typedef denValueFloating<double> denValueFloating1;
