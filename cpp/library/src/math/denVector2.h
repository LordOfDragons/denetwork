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
 * \brief Floating point 2 component vector.
 */
class denVector2{
public:
	double x;
	double y;
	
	/** \brief Create vector. */
	inline denVector2() : x(0.0), y(0.0){
	}
	
	/** \brief Create vector. */
	inline denVector2(double value) : x(value), y(value){
	}
	
	/** \brief Create vector. */
	inline denVector2(double ax, double ay) : x(ax), y(ay){
	}
	
	/** \brief Create copy of vector. */
	inline denVector2(const denVector2 &vector) : x(vector.x), y(vector.y){
	}
	
	/** \brief Equal. */
	inline bool Equals(const denVector2 &vector, double threshold = DENM_THRESHOLD_DOUBLE) const{
		return std::fabs(vector.x - x) < threshold
			&& std::fabs(vector.y - y) < threshold;
	}
	
	/** \brief Absolute value. */
	inline denVector2 Absolute() const{
		return denVector2(fabs(x), fabs(y));
	}
	
	/** \brief Largest. */
	inline denVector2 Largest(const denVector2 &vector) const{
		return denVector2(fmax(x, vector.x), fmax(y, vector.y));
	}
	
	/** \brief Assign. */
	inline denVector2 &operator=(const denVector2 &vector){
		x = vector.x;
		y = vector.y;
		return *this;
	}
	
	/** \brief Equals. */
	inline bool operator==(const denVector2 &vector) const{
		return Equals(vector);
	}
	
	inline bool operator!=(const denVector2 &vector) const{
		return ! Equals(vector);
	}
	
	/** \brief Compare. */
	inline bool operator<(const denVector2 &vector) const{
		return x < vector.x && y < vector.y;
	}
	
	inline bool operator<=(const denVector2 &vector) const{
		return x <= vector.x && y <= vector.y;
	}
	
	inline bool operator>(const denVector2 &vector) const{
		return x > vector.x && y > vector.y;
	}
	
	inline bool operator>=(const denVector2 &vector) const{
		return x >= vector.x && y >= vector.y;
	}
	
	/** \brief Add. */
	inline denVector2 operator+(const denVector2 &vector) const{
		return denVector2(x + vector.x, y + vector.y);
	}
	
	inline denVector2 &operator+=(const denVector2 &vector){
		x += vector.x;
		y += vector.y;
		return *this;
	}
	
	/** \brief Subtract. */
	inline denVector2 operator-(const denVector2 &vector) const{
		return denVector2(x - vector.x, y - vector.y);
	}
	
	inline denVector2 &operator-=(const denVector2 &vector){
		x -= vector.x;
		y -= vector.y;
		return *this;
	}
	
	/** \brief Scale. */
	inline denVector2 operator*(double scale) const{
		return denVector2(x * scale, y * scale);
	}
	
	inline denVector2 operator/(double scale) const{
		return denVector2(x / scale, y / scale);
	}
	
	inline denVector2 &operator*=(double scale){
		x *= scale;
		y *= scale;
		return *this;
	}
	
	inline denVector2 &operator/=(double scale){
		x /= scale;
		y /= scale;
		return *this;
	}
};

namespace std{
	inline denVector2 abs(const denVector2 &v){
		return v.Absolute();
	}
	
	inline denVector2 fmax(const denVector2 &a, const denVector2 &b){
		return a.Largest(b);
	}
}
