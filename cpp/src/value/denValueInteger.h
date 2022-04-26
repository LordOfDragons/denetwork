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
	denValueInteger(Type type, denValueIntegerFormat format) :
	denValue(type),
	pFormat(format),
	pValue(0),
	pLastValue(0){
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
	
	/**
	 * \brief Update value.
	 * \returns true if value needs to by synchronized otherwise false if not changed enough.
	 */
	bool UpdateValue(bool force) override{
		if(!force && pValue == pLastValue ){
			return false;
			
		}else{
			pLastValue = pValue;
			return true;
		}
	}
	
protected:
	const denValueIntegerFormat pFormat;
	T pValue;
	T pLastValue;
};


/**
 * \brief Single component integer value.
 */
class denValueInt : public denValueInteger<uint64_t>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<uint64_t>> Ref;
	
	/** \brief Create network value. */
	denValueInt(denValueIntegerFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};

/**
 * \brief 2 component integer vector value.
 */
class denValuePoint2 : public denValueInteger<denPoint2>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<denPoint2>> Ref;
	
	/** \brief Create network value. */
	denValuePoint2(denValueIntegerFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};

/**
 * \brief 3 component integer vector value.
 */
class denValuePoint3 : public denValueInteger<denPoint3>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueInteger<denPoint3>> Ref;
	
	/** \brief Create network value. */
	denValuePoint3(denValueIntegerFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};
