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

package ch.dragondreams.denetwork.math;

/**
 * Half float implementation based on
 * https://stackoverflow.com/questions/6162651/half-precision-floating-point-in-java
 * .
 */
public class HalfFloat {
	/**
	 * half to float.
	 */
	public static float halfToFloat(int hbits) {
		int mant = hbits & 0x03ff;
		int exp = hbits & 0x7c00;
		if (exp == 0x7c00) {
			exp = 0x3fc00;
		} else if (exp != 0) {
			exp += 0x1c000;
			if (mant == 0 && exp > 0x1c400) {
				return Float.intBitsToFloat((hbits & 0x8000) << 16 | exp << 13 | 0x3ff);
			}
		} else if (mant != 0) {
			exp = 0x1c400;
			do {
				mant <<= 1;
				exp -= 0x400;
			} while ((mant & 0x400) == 0);
			mant &= 0x3ff;
		}
		return Float.intBitsToFloat((hbits & 0x8000) << 16 | (exp | mant) << 13);
	}

	/**
	 * Float to half.
	 */
	public static int floatToHalf(float fval) {
		int fbits = Float.floatToIntBits(fval);
		int sign = fbits >>> 16 & 0x8000;
		int val = (fbits & 0x7fffffff) + 0x1000;

		if (val >= 0x47800000) {
			if ((fbits & 0x7fffffff) >= 0x47800000) {
				if (val < 0x7f800000) {
					return sign | 0x7c00;
				}
				return sign | 0x7c00 | (fbits & 0x007fffff) >>> 13;
			}
			return sign | 0x7bff;
		}
		if (val >= 0x38800000) {
			return sign | val - 0x38000000 >>> 13;
		}
		if (val < 0x33000000) {
			return sign;
		}
		val = (fbits & 0x7fffffff) >>> 23;
		return sign | ((fbits & 0x7fffff | 0x800000) + (0x800000 >>> val - 102) >>> 126 - val);
	}
}
