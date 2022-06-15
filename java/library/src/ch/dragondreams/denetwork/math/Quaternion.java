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
 * Immutable floating point 4 component quaternion.
 */
public class Quaternion {
	private double x;
	private double y;
	private double z;
	private double w;

	/**
	 * Create vector.
	 */
	public Quaternion() {
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}

	public Quaternion(double value) {
		x = value;
		y = value;
		z = value;
		w = value;
	}

	public Quaternion(double x, double y, double z, double w) {
		this.x = x;
		this.y = y;
		this.z = z;
		this.w = w;
	}

	/**
	 * X component.
	 */
	public double x() {
		return this.x;
	}

	/**
	 * Y component.
	 */
	public double y() {
		return this.y;
	}

	/**
	 * Z component.
	 */
	public double z() {
		return this.z;
	}

	/**
	 * W component.
	 */
	public double w() {
		return this.w;
	}

	/**
	 * Equals.
	 */
	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Quaternion) {
			return equals((Quaternion) obj, 1e-15);
		}
		return false;
	}

	public boolean equals(Quaternion vector, double threshold) {
		return Math.abs(vector.x - x) < threshold
				&& Math.abs(vector.y - y) < threshold
				&& Math.abs(vector.z - z) < threshold
				&& Math.abs(vector.w - w) < threshold;
	}

	/**
	 * Hash code.
	 */
	@Override
	public int hashCode() {
		return (int) (1e4f * (x + y + z + w));
	}

	/**
	 * Differs.
	 */
	public boolean differs(Quaternion vector) {
		return !equals(vector);
	}

	public boolean differs(Quaternion vector, float threshold) {
		return !equals(vector, threshold);
	}

	/**
	 * Less than.
	 */
	public boolean lessThan(Quaternion vector) {
		return x < vector.x && y < vector.y && z < vector.z && w < vector.w;
	}

	/**
	 * Less than or equal.
	 */
	public boolean lessThanEqual(Quaternion vector) {
		return x <= vector.x && y <= vector.y && z <= vector.z && w <= vector.w;
	}

	/**
	 * Greater than.
	 */
	public boolean greaterThan(Quaternion vector) {
		return x > vector.x && y > vector.y && z > vector.z && w > vector.w;
	}

	/**
	 * Greater than or equal.
	 */
	public boolean greateThanEqual(Quaternion vector) {
		return x >= vector.x && y >= vector.y && z >= vector.z && w >= vector.w;
	}

	/**
	 * Absolute value.
	 */
	public Quaternion absolute() {
		return new Quaternion(Math.abs(x), Math.abs(y), Math.abs(z), Math.abs(w));
	}

	/**
	 * Add.
	 */
	public Quaternion add(Quaternion vector) {
		return new Quaternion(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
	}

	/**
	 * Subtract.
	 */
	public Quaternion subtract(Quaternion vector) {
		return new Quaternion(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
	}

	/**
	 * Scale.
	 */
	public Quaternion scale(long scale) {
		return new Quaternion(x * scale, y * scale, z * scale, w * scale);
	}

	/**
	 * Divide.
	 */
	public Quaternion divide(long scale) {
		return new Quaternion(x / scale, y / scale, z / scale, w / scale);
	}

	/**
	 * String representation.
	 */
	@Override
	public String toString() {
		return String.format("(%f, %f, %f, %f)", x, y, z, w);
	}
}
