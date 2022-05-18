package ch.dragondreams.denetwork.math;

/**
 * Immutable floating point 2 component vector.
 */
public class Vector2 {
	private double x;
	private double y;

	/**
	 * Create vector.
	 */
	public Vector2() {
		x = 0;
		y = 0;
	}

	public Vector2(double value) {
		x = value;
		y = value;
	}

	public Vector2(double x, double y) {
		this.x = x;
		this.y = y;
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
	 * Equals.
	 */
	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Vector2) {
			return equals((Vector2) obj, 1e-15);
		}
		return false;
	}

	public boolean equals(Vector2 vector, double threshold) {
		return Math.abs(vector.x - x) < threshold && Math.abs(vector.y - y) < threshold;
	}

	/**
	 * Hash code.
	 */
	@Override
	public int hashCode() {
		return (int) (1e4f * (x + y));
	}

	/**
	 * Differs.
	 */
	@Override
	public boolean differs(Vector2 vector) {
		return !equals(vector);
	}

	public boolean differs(Vector2 vector, float threshold) {
		return !equals(vector, threshold);
	}

	/**
	 * Less than.
	 */
	public boolean lessThan(Vector2 vector) {
		return x < vector.x && y < vector.y;
	}

	/**
	 * Less than or equal.
	 */
	public boolean lessThanEqual(Vector2 vector) {
		return x <= vector.x && y <= vector.y;
	}

	/**
	 * Greater than.
	 */
	public boolean greaterThan(Vector2 vector) {
		return x > vector.x && y > vector.y;
	}

	/**
	 * Greater than or equal.
	 */
	public boolean greateThanEqual(Vector2 vector) {
		return x >= vector.x && y >= vector.y;
	}

	/**
	 * Absolute value.
	 */
	public Vector2 absolute() {
		return new Vector2(Math.abs(x), Math.abs(y));
	}

	/**
	 * Add.
	 */
	public Vector2 add(Vector2 vector) {
		return new Vector2(x + vector.x, y + vector.y);
	}

	/**
	 * Subtract.
	 */
	public Vector2 subtract(Vector2 vector) {
		return new Vector2(x - vector.x, y - vector.y);
	}

	/**
	 * Scale.
	 */
	public Vector2 scale(long scale) {
		return new Vector2(x * scale, y * scale);
	}

	/**
	 * Divide.
	 */
	public Vector2 divide(long scale) {
		return new Vector2(x / scale, y / scale);
	}

	/**
	 * String representation.
	 */
	@Override
	public String toString() {
		return String.format("(%f, %f)", x, y);
	}
}
