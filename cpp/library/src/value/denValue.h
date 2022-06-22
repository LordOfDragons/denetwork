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
#include <config.h>
#include "../denProtocolEnums.h"

class denMessageReader;
class denMessageWriter;
class denState;

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
	
	/** \brief Create network value. */
	denValue(Type type);
	
	/** \brief Clean up value. */
	virtual ~denValue() noexcept = default;
	
	/** \brief Type. */
	inline Type GetType() const{ return pType; }
	
	/** \brief Data type. */
	inline denProtocol::ValueTypes GetDataType() const{ return pDataType; }
	
	/** \brief Read value from message. */
	virtual void Read(denMessageReader &reader) = 0;
	
	/** \brief Write value to message. */
	virtual void Write(denMessageWriter &writer) = 0;
	
	/**
	 * \brief Update value.
	 * \returns true if value needs to by synchronized otherwise false if not changed enough.
	 */
	virtual bool UpdateValue(bool force) = 0;
	
	/**
	 * \brief Remote value changed.
	 * 
	 * For use by subclass to react to remote value changes.
	 */
	virtual void RemoteValueChanged();
	
	
	
protected:
	const Type pType;
	denProtocol::ValueTypes pDataType;
	
	void pValueChanged();
	
	friend denState;
	denState *pState;
	size_t pIndex;
};
