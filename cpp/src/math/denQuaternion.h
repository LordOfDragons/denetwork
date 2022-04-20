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
 * \brief Floating point 4 component quaternion.
 */
class denQuaternion{
public:
	double x;
	double y;
	double z;
	double w;
	
	/** \brief Create vector. */
	inline denQuaternion() : x(0.0), y(0.0), z(0.0), w(1.0){
	}
	
	/** \brief Create vector. */
	inline denQuaternion(double value) : x(value), y(value), z(value), w(value){
	}
	
	/** \brief Create vector. */
	inline denQuaternion(double x, double y, double z, double w) : x(x), y(y), z(z), w(w){
	}
	
	/** \brief Create copy of vector. */
	inline denQuaternion(const denQuaternion &vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w){
	}
	
	/** \brief Equal. */
	inline bool Equals(const denQuaternion &vector, double threshold = DENM_THRESHOLD_DOUBLE) const{
		return std::fabs(vector.x - x) < threshold
			&& std::fabs(vector.y - y) < threshold
			&& std::fabs(vector.z - z) < threshold
			&& std::fabs(vector.w - w) < threshold;
	}
	
	/** \brief Assign. */
	inline denQuaternion &operator=(const denQuaternion &vector){
		x = vector.x;
		y = vector.y;
		z = vector.z;
		w = vector.w;
		return *this;
	}
	
	/** \brief Equals. */
	inline bool operator==(const denQuaternion &vector) const{
		return Equals(vector);
	}
	
	/** \brief Compare. */
	inline bool operator<(const denQuaternion &vector) const{
		return x < vector.x && y < vector.y && z < vector.z && w < vector.w;
	}
	
	inline bool operator<=(const denQuaternion &vector) const{
		return x <= vector.x && y <= vector.y && z <= vector.z && w <= vector.w;
	}
	
	inline bool operator>(const denQuaternion &vector) const{
		return x > vector.x && y > vector.y && z > vector.z && w > vector.w;
	}
	
	inline bool operator>=(const denQuaternion &vector) const{
		return x >= vector.x && y >= vector.y && z >= vector.z && w >= vector.w;
	}
	
	/** \brief Add. */
	inline denQuaternion operator+(const denQuaternion &vector) const{
		return denQuaternion(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
	}
	
	inline denQuaternion &operator+=(const denQuaternion &vector){
		x += vector.x;
		y += vector.y;
		z += vector.z;
		w += vector.w;
		return *this;
	}
	
	/** \brief Subtract. */
	inline denQuaternion operator-(const denQuaternion &vector) const{
		return denQuaternion(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
	}
	
	inline denQuaternion &operator-=(const denQuaternion &vector){
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		w -= vector.w;
		return *this;
	}
	
	/** \brief Scale. */
	inline denQuaternion operator*(double scale) const{
		return denQuaternion(x * scale, y * scale, z * scale, w * scale);
	}
	
	inline denQuaternion operator/(double scale) const{
		return denQuaternion(x / scale, y / scale, z / scale, w / scale);
	}
	
	inline denQuaternion &operator*=(double scale){
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}
	
	inline denQuaternion &operator/=(double scale){
		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;
		return *this;
	}
};
