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
	
	/** \brief Create quaternion. */
	inline denQuaternion() : x(0.0), y(0.0), z(0.0), w(1.0){
	}
	
	/** \brief Create quaternion. */
	inline denQuaternion(double value) : x(value), y(value), z(value), w(value){
	}
	
	/** \brief Create quaternion. */
	inline denQuaternion(double ax, double ay, double az, double aw) : x(ax), y(ay), z(az), w(aw){
	}
	
	/** \brief Create copy of quaternion. */
	inline denQuaternion(const denQuaternion &quaternion) :
	x(quaternion.x), y(quaternion.y), z(quaternion.z), w(quaternion.w){
	}
	
	/** \brief Equal. */
	inline bool Equals(const denQuaternion &quaternion, double threshold = DENM_THRESHOLD_DOUBLE) const{
		return std::fabs(quaternion.x - x) < threshold
			&& std::fabs(quaternion.y - y) < threshold
			&& std::fabs(quaternion.z - z) < threshold
			&& std::fabs(quaternion.w - w) < threshold;
	}
	
	/** \brief Absolute value. */
	inline denQuaternion Absolute() const{
		return denQuaternion(fabs(x), fabs(y), fabs(z), fabs(w));
	}
	
	/** \brief Largest. */
	inline denQuaternion Largest(const denQuaternion &quaternion) const{
		return denQuaternion(fmax(x, quaternion.x), fmax(y, quaternion.y),
			fmax(z, quaternion.z), fmax(w, quaternion.w));
	}
	
	/** \brief Assign. */
	inline denQuaternion &operator=(const denQuaternion &quaternion){
		x = quaternion.x;
		y = quaternion.y;
		z = quaternion.z;
		w = quaternion.w;
		return *this;
	}
	
	/** \brief Equals. */
	inline bool operator==(const denQuaternion &quaternion) const{
		return Equals(quaternion);
	}
	
	/** \brief Compare. */
	inline bool operator<(const denQuaternion &quaternion) const{
		return x < quaternion.x && y < quaternion.y && z < quaternion.z && w < quaternion.w;
	}
	
	inline bool operator<=(const denQuaternion &quaternion) const{
		return x <= quaternion.x && y <= quaternion.y && z <= quaternion.z && w <= quaternion.w;
	}
	
	inline bool operator>(const denQuaternion &quaternion) const{
		return x > quaternion.x && y > quaternion.y && z > quaternion.z && w > quaternion.w;
	}
	
	inline bool operator>=(const denQuaternion &quaternion) const{
		return x >= quaternion.x && y >= quaternion.y && z >= quaternion.z && w >= quaternion.w;
	}
	
	/** \brief Add. */
	inline denQuaternion operator+(const denQuaternion &quaternion) const{
		return denQuaternion(x + quaternion.x, y + quaternion.y, z + quaternion.z, w + quaternion.w);
	}
	
	inline denQuaternion &operator+=(const denQuaternion &quaternion){
		x += quaternion.x;
		y += quaternion.y;
		z += quaternion.z;
		w += quaternion.w;
		return *this;
	}
	
	/** \brief Subtract. */
	inline denQuaternion operator-(const denQuaternion &quaternion) const{
		return denQuaternion(x - quaternion.x, y - quaternion.y, z - quaternion.z, w - quaternion.w);
	}
	
	inline denQuaternion &operator-=(const denQuaternion &quaternion){
		x -= quaternion.x;
		y -= quaternion.y;
		z -= quaternion.z;
		w -= quaternion.w;
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

namespace std{
	inline denQuaternion abs(const denQuaternion &q){
		return q.Absolute();
	}
	
	inline denQuaternion fmax(const denQuaternion &a, const denQuaternion &b){
		return a.Largest(b);
	}
}
