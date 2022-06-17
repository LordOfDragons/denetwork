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
 * Integer network state value.
 */
abstract public class ValueInteger<T> extends Value {
	/**
	 * Value formats.
	 */
	public enum Format {
		/**
		 * 8-Bit signed integer.
		 */
		SINT8,

		/**
		 * 8-Bit unsigned integer.
		 */
		UINT8,

		/**
		 * 16-Bit signed integer.
		 */
		SINT16,

		/**
		 * 16-Bit unsigned integer.
		 */
		UINT16,

		/**
		 * 32-Bit signed integer.
		 */
		SINT32,

		/**
		 * 32-Bit unsigned integer.
		 */
		UINT32,

		/**
		 * 64-Bit signed integer.
		 */
		SINT64,

		/**
		 * 64-Bit unsigned integer.
		 */
		UINT64
	}

	protected Format format;
	protected T value;
	protected T lastValue;

	/**
	 * Create value.
	 */
	public ValueInteger(Type type, Format format) {
		super(type);
		this.format = format;
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

	/* (non-Javadoc)
	 * @see ch.dragondreams.denetwork.value.Value#updateValue(boolean)
	 */
	@Override
	public synchronized boolean updateValue(boolean force) {
		if (!force && value.equals(lastValue)) {
			return false;

		} else {
			lastValue = value;
			return true;
		}
	}
}
