package ch.dragondreams.denetwork.math;

/**
 * Immutable floating point 3 component vector.
 */
public class Vector3 {
	private double x;
	private double y;
	private double z;

	/**
	 * Create vector.
	 */
	public Vector3() {
		x = 0;
		y = 0;
		z = 0;
	}

	public Vector3(double value) {
		x = value;
		y = value;
		z = value;
	}

	public Vector3(double x, double y, double z) {
		this.x = x;
		this.y = y;
		this.z = z;
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
	 * Equals.
	 */
	@Override
	public boolean equals(Object obj) {
		if (obj instanceof Vector3) {
			return equals((Vector3) obj, 1e-15);
		}
		return false;
	}

	public boolean equals(Vector3 vector, double threshold) {
		return Math.abs(vector.x - x) < threshold
				&& Math.abs(vector.y - y) < threshold
				&& Math.abs(vector.z - z) < threshold;
	}

	/**
	 * Hash code.
	 */
	@Override
	public int hashCode() {
		return (int) (1e4f * (x + y + z));
	}

	/**
	 * Differs.
	 */
	@Override
	public boolean differs(Vector3 vector) {
		return !equals(vector);
	}

	public boolean differs(Vector3 vector, float threshold) {
		return !equals(vector, threshold);
	}

	/**
	 * Less than.
	 */
	public boolean lessThan(Vector3 vector) {
		return x < vector.x && y < vector.y && z < vector.z;
	}

	/**
	 * Less than or equal.
	 */
	public boolean lessThanEqual(Vector3 vector) {
		return x <= vector.x && y <= vector.y && z <= vector.z;
	}

	/**
	 * Greater than.
	 */
	public boolean greaterThan(Vector3 vector) {
		return x > vector.x && y > vector.y && z > vector.z;
	}

	/**
	 * Greater than or equal.
	 */
	public boolean greateThanEqual(Vector3 vector) {
		return x >= vector.x && y >= vector.y && z >= vector.z;
	}

	/**
	 * Absolute value.
	 */
	public Vector3 absolute() {
		return new Vector3(Math.abs(x), Math.abs(y), Math.abs(z));
	}

	/**
	 * Add.
	 */
	public Vector3 add(Vector3 vector) {
		return new Vector3(x + vector.x, y + vector.y, z + vector.z);
	}

	/**
	 * Subtract.
	 */
	public Vector3 subtract(Vector3 vector) {
		return new Vector3(x - vector.x, y - vector.y, z - vector.z);
	}

	/**
	 * Scale.
	 */
	public Vector3 scale(long scale) {
		return new Vector3(x * scale, y * scale, z * scale);
	}

	/**
	 * Divide.
	 */
	public Vector3 divide(long scale) {
		return new Vector3(x / scale, y / scale, z / scale);
	}

	/**
	 * String representation.
	 */
	@Override
	public String toString() {
		return String.format("(%f, %f, %f)", x, y, z);
	}
}
