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
#include <ctime>
#include <chrono>
#include <sstream>
#include "../config.h"

/**
 * \brief Network message.
 */
class denMessage{
public:
	/** \brief Shared pointer. */
	typedef std::shared_ptr<denMessage> Ref;
	
	/** \brief Buffer. */
	typedef std::stringstream Buffer;
	
	/** \brief Timestamp. */
	typedef std::chrono::time_point<std::chrono::system_clock> Timestamp;
	
	/** \brief Create message. */
	denMessage();
	
	/** \brief Clean up message. */
	virtual ~denMessage();
	
	/** \brief Timestamp. */
	inline const Timestamp &GetTimestamp() const{ return pTimestamp; }
	
	/** \brief Set timestamp. */
	void SetTimestamp(const Timestamp &timestamp);
	
	/** \brief Buffer. */
	inline Buffer &GetBuffer(){ return pBuffer; }
	inline const Buffer &GetBuffer() const{ return pBuffer; }
	
private:
	Buffer pBuffer;
	Timestamp pTimestamp;
};
