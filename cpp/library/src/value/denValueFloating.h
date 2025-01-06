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
#include "../math/denVector2.h"
#include "../math/denVector3.h"
#include "../math/denQuaternion.h"

/** \brief Value formats. */
enum class denValueFloatingFormat{
	float16, /** \brief 16-Bit float. */
	float32, /** \brief 32-Bit float. */
	float64 /** \brief 64-Bit float. */
};

/**
 * \brief Floating network state value.
 */
template<class T> class denValueFloating : public denValue{
public:
	/** \brief Create network value. */
	denValueFloating(Type type, denValueFloatingFormat format, T minPrecision) :
	denValue(type),
	pFormat(format),
	pValue(0),
	pLastValue(0),
	pPrecision(minPrecision),
	pMinPrecision(minPrecision){
	}
	
	/** \brief Format. */
	denValueFloatingFormat GetFormat() const{
		return pFormat;
	}
	
	/** \brief Value. */
	T GetValue() const{
		return pValue;
	}
	
	/** \brief Set value. */
	void SetValue(T value){
		pValue = value;
		pValueChanged();
	}
	
	/** \brief Precision. */
	T GetPrecision() const{
		return pPrecision;
	}
	
	/** \brief Set precision. */
	void SetPrecision(T precision){
		pPrecision = std::fmax(precision, pMinPrecision);
		pValueChanged();
	}
	
	/**
	 * \brief Update value.
	 * \returns true if value needs to by synchronized otherwise false if not changed enough.
	 */
	bool UpdateValue(bool force) override{
		if(!force && std::abs(pValue - pLastValue) <= pPrecision){
			return false;
			
		}else{
			pLastValue = pValue;
			return true;
		}
	}
	
protected:
	const denValueFloatingFormat pFormat;
	T pValue;
	T pLastValue;
	T pPrecision;
	const T pMinPrecision;
};

/**
 * \brief Single component floating value.
 */
class denValueFloat : public denValueFloating<double>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueFloating<double>> Ref;
	
	/** \brief Create network value. */
	denValueFloat(denValueFloatingFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};

/**
 * \brief 2 component floating vector value.
 */
class denValueVector2 : public denValueFloating<denVector2>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueFloating<denVector2>> Ref;
	
	/** \brief Create network value. */
	denValueVector2(denValueFloatingFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};

/**
 * \brief 3 component floating vector value.
 */
class denValueVector3 : public denValueFloating<denVector3>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueFloating<denVector3>> Ref;
	
	/** \brief Create network value. */
	denValueVector3(denValueFloatingFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};

/**
 * \brief 4 component floating quaternion value.
 */
class denValueQuaternion : public denValueFloating<denQuaternion>{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValueFloating<denQuaternion>> Ref;
	
	/** \brief Create network value. */
	denValueQuaternion(denValueFloatingFormat format);
	
	/** \brief Read value from message. */
	void Read(denMessageReader &reader) override;
	
	/** \brief Write value to message. */
	void Write(denMessageWriter &writer) override;
};
