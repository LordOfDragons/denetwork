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
#include <string>
#include "../config.h"
#include "../denPool.h"

/**
 * \brief Network message.
 */
class denMessage{
public:
	/** \brief Shared pointer. */
	typedef denPoolItem<denMessage>::Ref Ref;
	
	/** \brief Timestamp. */
	typedef std::chrono::time_point<std::chrono::system_clock> Timestamp;
	
	/** \brief Create message. */
	denMessage();
	
	/** \brief Timestamp. */
	inline const Timestamp &GetTimestamp() const{ return pTimestamp; }
	
	/** \brief Set timestamp. */
	void SetTimestamp(const Timestamp &timestamp);
	
	/** \brief Data. */
	inline std::string &GetData(){ return pData; }
	inline const std::string &GetData() const{ return pData; }
	
	/** \brief Set data. */
	void SetData(const std::string &data);
	
	/** \brief Data length which can be less than the data size. */
	inline size_t GetLength() const{ return pLength; }
	
	/** \brief Set data length which can be less than the data size. */
	void SetLength(size_t length);
	
	/** \brief Same as SetLength() but keeps content up to previous length intact. */
	void SetLengthRetain(size_t length);
	
	/** \brief Pool. */
	inline static denPool<denMessage> &Pool(){ return pPool; }
	
private:
	std::string pData;
	size_t pLength;
	Timestamp pTimestamp;
	
	static denPool<denMessage> pPool;
};
