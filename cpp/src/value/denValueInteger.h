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
#include "../math/denPoint2.h"
#include "../math/denPoint3.h"

/**
 * \brief Value formats.
 */
enum class denValueIntegerFormat{
	sint8, /** \brief 8-Bit signed integer. */
	uint8, /** \brief 8-Bit unsigned integer. */
	sint16, /** \brief 16-Bit signed integer. */
	uint16, /** \brief 16-Bit unsigned integer. */
	sint32, /** \brief 32-Bit signed integer. */
	uint32, /** \brief 32-Bit unsigned integer. */
	sint64, /** \brief 64-Bit signed integer. */
	uint64 /** \brief 64-Bit unsigned integer. */
};

/**
 * \brief Integer network state value.
 */
template<class T> class denValueInteger : public denValue{
public:
	/** \brief Create network value. */
	denValueInteger(Type type, denValueIntegerFormat format) : denValue(type), pFormat(format){
	}
	
public:
	/** \brief Format. */
	denValueIntegerFormat GetFormat() const{
		return pFormat;
	}
	
	/** \brief Value. */
	T GetValue() const{
		return pValue;
	}
	
	/** \brief Set value. */
	void SetValue(T value){
		pValue = value;
	}
	
private:
	const denValueIntegerFormat pFormat;
	T pValue;
};


/**
 * \brief Single component integer value.
 */
class denValueInt : public denValueInteger<uint64_t>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<uint64_t>> Ref;
	
	/** \brief Create network value. */
	inline denValueInt(denValueIntegerFormat format) :
	denValueInteger<uint64_t>(Type::integer, format){}
};

/**
 * \brief 2 component integer vector value.
 */
class denValuePoint2 : public denValueInteger<denPoint2>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<denPoint2>> Ref;
	
	/** \brief Create network value. */
	inline denValuePoint2(denValueIntegerFormat format) :
	denValueInteger<denPoint2>(Type::point2, format){}
};

/**
 * \brief 3 component integer vector value.
 */
class denValuePoint3 : public denValueInteger<denPoint3>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<denPoint3>> Ref;
	
	/** \brief Create network value. */
	inline denValuePoint3(denValueIntegerFormat format) :
	denValueInteger<denPoint3>(Type::vector3, format){}
};
