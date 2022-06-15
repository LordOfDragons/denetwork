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
 * Immutable integer 2 component point.
 */
public class Point2 {
	private long x;
	private long y;

	/**
	 * Create point.
	 */
	public Point2() {
		x = 0;
		y = 0;
	}

	public Point2(long value) {
		x = value;
		y = value;
	}

	public Point2(long x, long y) {
		this.x = x;
		this.y = y;
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
	 * Equals.
	 */
	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Point2) {
			Point2 point = (Point2) obj;
			return point.x == x && point.y == y;
		}
		return false;
	}

	/**
	 * Hash code.
	 */
	@Override
	public int hashCode() {
		return (int) (x + y);
	}

	/**
	 * Differs.
	 */
	public boolean differs(Point2 point) {
		return point.x != x || point.y != y;
	}

	/**
	 * Less than.
	 */
	public boolean lessThan(Point2 point) {
		return x < point.x && y < point.y;
	}

	/**
	 * Less than or equal.
	 */
	public boolean lessThanEqual(Point2 point) {
		return x <= point.x && y <= point.y;
	}

	/**
	 * Greater than.
	 */
	public boolean greaterThan(Point2 point) {
		return x > point.x && y > point.y;
	}

	/**
	 * Greater than or equal.
	 */
	public boolean greateThanEqual(Point2 point) {
		return x >= point.x && y >= point.y;
	}

	/**
	 * Add.
	 */
	public Point2 add(Point2 point) {
		return new Point2(x + point.x, y + point.y);
	}

	/**
	 * Subtract.
	 */
	public Point2 subtract(Point2 point) {
		return new Point2(x - point.x, y - point.y);
	}

	/**
	 * Scale.
	 */
	public Point2 scale(long scale) {
		return new Point2(x * scale, y * scale);
	}

	/**
	 * Divide.
	 */
	public Point2 divide(long scale) {
		return new Point2(x / scale, y / scale);
	}

	/**
	 * String representation.
	 */
	@Override
	public String toString() {
		return String.format("(%d, %d)", x, y);
	}
}
