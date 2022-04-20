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
 * \brief Floating point 3 component vector.
 */
class denVector3{
public:
	double x;
	double y;
	double z;
	
	/** \brief Create vector. */
	inline denVector3() : x(0.0), y(0.0), z(0.0){
	}
	
	/** \brief Create vector. */
	inline denVector3(double value) : x(value), y(value), z(value){
	}
	
	/** \brief Create vector. */
	inline denVector3(double x, double y, double z) : x(x), y(y), z(z){
	}
	
	/** \brief Create copy of vector. */
	inline denVector3(const denVector3 &vector) : x(vector.x), y(vector.y), z(vector.z){
	}
	
	/** \brief Equal. */
	inline bool Equals(const denVector3 &vector, double threshold = DENM_THRESHOLD_DOUBLE) const{
		return std::fabs(vector.x - x) < threshold
			&& std::fabs(vector.y - y) < threshold
			&& std::fabs(vector.z - z) < threshold;
	}
	
	/** \brief Assign. */
	inline denVector3 &operator=(const denVector3 &vector){
		x = vector.x;
		y = vector.y;
		z = vector.z;
		return *this;
	}
	
	/** \brief Equals. */
	inline bool operator==(const denVector3 &vector) const{
		return Equals(vector);
	}
	
	inline bool operator!=(const denVector3 &vector) const{
		return ! Equals(vector);
	}
	
	/** \brief Compare. */
	inline bool operator<(const denVector3 &vector) const{
		return x < vector.x && y < vector.y && z < vector.z;
	}
	
	inline bool operator<=(const denVector3 &vector) const{
		return x <= vector.x && y <= vector.y && z <= vector.z;
	}
	
	inline bool operator>(const denVector3 &vector) const{
		return x > vector.x && y > vector.y && z > vector.z;
	}
	
	inline bool operator>=(const denVector3 &vector) const{
		return x >= vector.x && y >= vector.y && z >= vector.z;
	}
	
	/** \brief Add. */
	inline denVector3 operator+(const denVector3 &vector) const{
		return denVector3(x + vector.x, y + vector.y, z + vector.z);
	}
	
	inline denVector3 &operator+=(const denVector3 &vector){
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}
	
	/** \brief Subtract. */
	inline denVector3 operator-(const denVector3 &vector) const{
		return denVector3(x - vector.x, y - vector.y, z - vector.z);
	}
	
	inline denVector3 &operator-=(const denVector3 &vector){
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}
	
	/** \brief Scale. */
	inline denVector3 operator*(double scale) const{
		return denVector3(x * scale, y * scale, z * scale);
	}
	
	inline denVector3 operator/(double scale) const{
		return denVector3(x / scale, y / scale, z / scale);
	}
	
	inline denVector3 &operator*=(double scale){
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}
	
	inline denVector3 &operator/=(double scale){
		x /= scale;
		y /= scale;
		z /= scale;
		return *this;
	}
};
