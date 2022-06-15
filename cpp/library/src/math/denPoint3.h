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
 * \brief Floating point 3 component point.
 */
class denPoint3{
public:
	uint64_t x;
	uint64_t y;
	uint64_t z;
	
	/** \brief Create point. */
	inline denPoint3() : x(0), y(0), z(0){
	}
	
	/** \brief Create point. */
	inline denPoint3(uint64_t value) : x(value), y(value), z(value){
	}
	
	/** \brief Create point. */
	inline denPoint3(uint64_t x, uint64_t y, uint64_t z) : x(x), y(y), z(z){
	}
	
	/** \brief Create copy of point. */
	inline denPoint3(const denPoint3 &point) : x(point.x), y(point.y), z(point.z){
	}
	
	/** \brief Assign. */
	inline denPoint3 &operator=(const denPoint3 &point){
		x = point.x;
		y = point.y;
		z = point.z;
		return *this;
	}
	
	/** \brief Equals. */
	inline bool operator==(const denPoint3 &point) const{
		return x == point.x && y == point.y && z == point.z;
	}
	
	inline bool operator!=(const denPoint3 &point) const{
		return x != point.x || y != point.y || z != point.z;
	}
	
	/** \brief Compare. */
	inline bool operator<(const denPoint3 &point) const{
		return x < point.x && y < point.y && z < point.z;
	}
	
	inline bool operator<=(const denPoint3 &point) const{
		return x <= point.x && y <= point.y && z <= point.z;
	}
	
	inline bool operator>(const denPoint3 &point) const{
		return x > point.x && y > point.y && z > point.z;
	}
	
	inline bool operator>=(const denPoint3 &point) const{
		return x >= point.x && y >= point.y && z >= point.z;
	}
	
	/** \brief Add. */
	inline denPoint3 operator+(const denPoint3 &point) const{
		return denPoint3(x + point.x, y + point.y, z + point.z);
	}
	
	inline denPoint3 &operator+=(const denPoint3 &point){
		x += point.x;
		y += point.y;
		z += point.z;
		return *this;
	}
	
	/** \brief Subtract. */
	inline denPoint3 operator-(const denPoint3 &point) const{
		return denPoint3(x - point.x, y - point.y, z - point.z);
	}
	
	inline denPoint3 &operator-=(const denPoint3 &point){
		x -= point.x;
		y -= point.y;
		z -= point.z;
		return *this;
	}
	
	/** \brief Scale. */
	inline denPoint3 operator*(uint64_t scale) const{
		return denPoint3(x * scale, y * scale, z * scale);
	}
	
	inline denPoint3 operator/(uint64_t scale) const{
		return denPoint3(x / scale, y / scale, z / scale);
	}
	
	inline denPoint3 &operator*=(uint64_t scale){
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}
	
	inline denPoint3 &operator/=(uint64_t scale){
		x /= scale;
		y /= scale;
		z /= scale;
		return *this;
	}
};
