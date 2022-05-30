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
 * Immutable integer 3 component point.
 */
public class Point3 {
	private long x;
	private long y;
	private long z;

	/**
	 * Create point.
	 */
	public Point3() {
		x = 0;
		y = 0;
		z = 0;
	}

	public Point3(long value) {
		x = value;
		y = value;
		z = value;
	}

	public Point3(long x, long y, long z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}

	/**
	 * X component.
	 */
	public long x() {
		return this.x;
	}

	/**
	 * Y component.
	 */
	public long y() {
		return this.y;
	}

	/**
	 * Z component.
	 */
	public long z() {
		return this.z;
	}

	/**
	 * Equals.
	 */
	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Point3) {
			Point3 point = (Point3) obj;
			return point.x == x && point.y == y && point.z == z;
		}
		return false;
	}

	/**
	 * Hash code.
	 */
	@Override
	public int hashCode() {
		return (int) (x + y + z);
	}

	/**
	 * Differs.
	 */
	public boolean differs(Point3 point) {
		return point.x != x || point.y != y || point.z != z;
	}

	/**
	 * Less than.
	 */
	public boolean lessThan(Point3 point) {
		return x < point.x && y < point.y && z < point.z;
	}

	/**
	 * Less than or equal.
	 */
	public boolean lessThanEqual(Point3 point) {
		return x <= point.x && y <= point.y && z <= point.z;
	}

	/**
	 * Greater than.
	 */
	public boolean greaterThan(Point3 point) {
		return x > point.x && y > point.y && z > point.z;
	}

	/**
	 * Greater than or equal.
	 */
	public boolean greateThanEqual(Point3 point) {
		return x >= point.x && y >= point.y && z >= point.z;
	}

	/**
	 * Add.
	 */
	public Point3 add(Point3 point) {
		return new Point3(x + point.x, y + point.y, z + point.z);
	}

	/**
	 * Subtract.
	 */
	public Point3 subtract(Point3 point) {
		return new Point3(x - point.x, y - point.y, z - point.z);
	}

	/**
	 * Scale.
	 */
	public Point3 scale(long scale) {
		return new Point3(x * scale, y * scale, z * scale);
	}

	/**
	 * Divide.
	 */
	public Point3 divide(long scale) {
		return new Point3(x / scale, y / scale, z / scale);
	}

	/**
	 * String representation.
	 */
	@Override
	public String toString() {
		return String.format("(%d, %d, %d)", x, y, z);
	}
}
