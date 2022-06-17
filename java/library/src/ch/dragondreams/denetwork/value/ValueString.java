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

import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.ValueTypes;

/**
 * String network state value.
 */
public class ValueString extends Value {
	protected String value;
	protected String lastValue;

	/**
	 * Create value.
	 */
	public ValueString() {
		super(Type.STRING);
		dataType = ValueTypes.STRING;
	}

	/**
	 * Value.
	 */
	public synchronized String getValue() {
		return value;
	}

	/**
	 * Set value.
	 */
	public synchronized void setValue(String value) {
		if (value == null) {
			throw new IllegalArgumentException("value is null");
		}

		this.value = value;
		valueChanged();
	}

	/* (non-Javadoc)
	 * @see ch.dragondreams.denetwork.value.Value#read(ch.dragondreams.denetwork.message.MessageReader)
	 */
	@Override
	public synchronized void read(MessageReader reader) {
		value = reader.readString16();
		lastValue = value;
	}

	/* (non-Javadoc)
	 * @see ch.dragondreams.denetwork.value.Value#write(ch.dragondreams.denetwork.message.MessageWriter)
	 */
	@Override
	public synchronized void write(MessageWriter writer) {
		writer.writeString16(value);
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
