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

package ch.dragondreams.denetwork.value;

/**
 * Floating network state value.
 */
abstract public class ValueFloating<T> extends Value {
	/**
	 * Value formats.
	 */
	public enum Format {
		/**
		 * 16-Bit float.
		 */
		FLOAT16,

		/**
		 * 32-Bit float.
		 */
		FLOAT32,

		/**
		 * 64-Bit float.
		 */
		FLOAT64
	}

	protected Format format;
	protected T value;
	protected T lastValue;
	protected T precision;
	protected T minPrecision;

	/**
	 * Create value.
	 */
	public ValueFloating(Type type, Format format, T minPrecision) {
		super(type);
		this.format = format;
		precision = minPrecision;
		this.minPrecision = minPrecision;
	}

	/**
	 * Value.
	 */
	public synchronized T getValue() {
		return value;
	}

	/**
	 * Set value.
	 */
	public synchronized void setValue(T value) {
		if (value == null) {
			throw new IllegalArgumentException("value is null");
		}

		this.value = value;
		valueChanged();
	}

	/**
	 * Precision.
	 */
	public T getPrecision() {
		return precision;
	}

	abstract protected T maximum(T a, T b);
	abstract protected boolean absDiffLessThanOrEqual(T a, T b, T p);

	/**
	 * Set precision.
	 */
	public void setPrecision(T precision){
		this.precision = maximum(precision, minPrecision);
		valueChanged();
	}

	/* (non-Javadoc)
	 * @see ch.dragondreams.denetwork.value.Value#updateValue(boolean)
	 */
	@Override
	public synchronized boolean updateValue(boolean force) {
		if (!force && absDiffLessThanOrEqual(value, lastValue, precision)) {
			return false;

		} else {
			lastValue = value;
			return true;
		}
	}
}
