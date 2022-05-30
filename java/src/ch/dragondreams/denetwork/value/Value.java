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
import ch.dragondreams.denetwork.state.State;

/**
 * Network state value.
 */
abstract public class Value {
	/** \brief Value type. */
	public enum Type{
		INTEGER,
		FLOAT,
		STRING,
		DATA,
		POINT2,
		POINT3,
		VECTOR2,
		VECTOR3,
		QUATERNION
	}


	protected final Type type;
	protected ValueTypes dataType;

	protected void valueChanged() {
		if (state != null) {
			state.valueChanged(this);
		}
	}

	protected State state;
	protected int index;

	/**
	 * Create network value.
	 */
	public Value(Type type) {
		if (type == null) {
			throw new IllegalArgumentException("type is null");
		}

		this.type = type;
		dataType = ValueTypes.DATA;
		state = null;
		index = 0;
	}

	/**
	 * Type.
	 */
	public Type getType() {
		return type;
	}

	/**
	 * Data type.
	 */
	public ValueTypes getDataType() {
		return dataType;
	}

	/**
	 * Read value from message.
	 */
	abstract public void read(MessageReader reader);

	/**
	 * Write value to message.
	 */
	abstract public void write(MessageWriter writer);

	/**
	 * Update value. Returns true if value needs to by synchronized
	 * otherwise false if not changed enough.
	 */
	abstract public boolean updateValue(boolean force);
}
