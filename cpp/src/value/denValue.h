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
#include <stdexcept>
#include "../config.h"

/**
 * \brief Network state value.
 */
class denValue{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denValue> Ref;
	
	/** \brief Value type. */
	enum class Type{
		integer,
		floating,
		string,
		data,
		point2,
		point3,
		vector2,
		vector3,
		quaternion
	};
	
	/** \brief Value formats. */
	enum class Format{
		sint8, /** \brief 8-Bit signed integer. */
		uint8, /** \brief 8-Bit unsigned integer. */
		sint16, /** \brief 16-Bit signed integer. */
		uint16, /** \brief 16-Bit unsigned integer. */
		sint32, /** \brief 32-Bit signed integer. */
		uint32, /** \brief 32-Bit unsigned integer. */
		sint64, /** \brief 64-Bit signed integer. */
		uint64, /** \brief 64-Bit unsigned integer. */
		float16, /** \brief 16-Bit float. */
		float32, /** \brief 32-Bit float. */
		float64 /** \brief 64-Bit float. */
	};
	
	/** \brief Create network value. */
	denValue(Type type);
	
	/** \brief Clean up value. */
	virtual ~denValue();
	
	/** \brief Type. */
	inline Type GetType() const{ return pType; }
	
private:
	const Type pType;
};
