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

#include "../math/denMath.h"

/**
 * \brief Integer point 2 component point.
 */
class denPoint2{
public:
	uint64_t x;
	uint64_t y;
	
	/** \brief Create point. */
	inline denPoint2() : x(0), y(0){
	}
	
	/** \brief Create point. */
	inline denPoint2(uint64_t value) : x(value), y(value){
	}
	
	/** \brief Create point. */
	inline denPoint2(uint64_t ax, uint64_t ay) : x(ax), y(ay){
	}
	
	/** \brief Create copy of point. */
	inline denPoint2(const denPoint2 &point) : x(point.x), y(point.y){
	}
	
	/** \brief Assign. */
	inline denPoint2 &operator=(const denPoint2 &point){
		x = point.x;
		y = point.y;
		return *this;
	}
	
	/** \brief Equals. */
	inline bool operator==(const denPoint2 &point) const{
		return point.x == x && point.y == y;
	}
	
	inline bool operator!=(const denPoint2 &point) const{
		return point.x != x || point.y != y;
	}
	
	/** \brief Compare. */
	inline bool operator<(const denPoint2 &point) const{
		return x < point.x && y < point.y;
	}
	
	inline bool operator<=(const denPoint2 &point) const{
		return x <= point.x && y <= point.y;
	}
	
	inline bool operator>(const denPoint2 &point) const{
		return x > point.x && y > point.y;
	}
	
	inline bool operator>=(const denPoint2 &point) const{
		return x >= point.x && y >= point.y;
	}
	
	/** \brief Add. */
	inline denPoint2 operator+(const denPoint2 &point) const{
		return denPoint2(x + point.x, y + point.y);
	}
	
	inline denPoint2 &operator+=(const denPoint2 &point){
		x += point.x;
		y += point.y;
		return *this;
	}
	
	/** \brief Subtract. */
	inline denPoint2 operator-(const denPoint2 &point) const{
		return denPoint2(x - point.x, y - point.y);
	}
	
	inline denPoint2 &operator-=(const denPoint2 &point){
		x -= point.x;
		y -= point.y;
		return *this;
	}
	
	/** \brief Scale. */
	inline denPoint2 operator*(uint64_t scale) const{
		return denPoint2(x * scale, y * scale);
	}
	
	inline denPoint2 operator/(uint64_t scale) const{
		return denPoint2(x / scale, y / scale);
	}
	
	inline denPoint2 &operator*=(uint64_t scale){
		x *= scale;
		y *= scale;
		return *this;
	}
	
	inline denPoint2 &operator/=(uint64_t scale){
		x /= scale;
		y /= scale;
		return *this;
	}
};
