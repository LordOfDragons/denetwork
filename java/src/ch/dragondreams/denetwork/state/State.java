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

package ch.dragondreams.denetwork.state;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.ValueTypes;
import ch.dragondreams.denetwork.value.Value;

public class State {
	public static final String CLASS_NAME = State.class.getCanonicalName();
	public static final String LOGGER_NAME = State.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	private final boolean readOnly;
	private List<Value> values = new ArrayList<>();
	private LinkedList<StateLink> links = new LinkedList<>();

	/**
	 * Update state.
	 */
	private void update() {
	}

	/**
	 * Read values from message.
	 */
	public void linkReadValues(MessageReader reader, StateLink link) {
		int i, count = reader.readByte();

		for (i = 0; i < count; i++) {
			int index = reader.readUShort();
			if (index < 0 || index >= values.size()) {
				throw new IllegalArgumentException("index out of range");
			}

			Value value = values.get(index);
			value.read(reader);
			invalidateValueAtExcept(index, link);

			valueChanged(value);
		}

		link.setChanged(link.hasChangedValues());
	}

	/**
	 * Read all values from message.
	 */
	private void linkReadAllValues(MessageReader reader, StateLink link) {
		int i, count = values.size();

		for (i = 0; i < count; i++) {
			Value value = values.get(i);
			value.read(reader);
			invalidateValueAt(i);

			valueChanged(value);
		}

		if (!link.hasChangedValues()) {
			link.setChanged(false);
		}
	}

	/**
	 * Read all values from message including types.
	 *
	 * Verifies that the values in the state match in type and update their values.
	 * Returns true if the state matches and has been updated or false otherwise.
	 */
	public boolean linkReadAndVerifyAllValues(MessageReader reader) {
		int i, count = values.size();
		if (count != reader.readUShort()) {
			throw new IllegalArgumentException("count out of range");
		}

		for (i = 0; i < count; i++) {
			ValueTypes type = ValueTypes.withValue(reader.readByte());
			Value value = values.get(i);
			if (type != value.getDataType()) {
				throw new IllegalArgumentException("data type mismatch");
			}

			value.read(reader);
			invalidateValueAt(i);

			valueChanged(value);
		}

		return i == count;
	}

	/**
	 * Write all values to message.
	 *
	 * @param withTypes Include types.
	 */
	private void linkWriteValues(MessageWriter writer) {
		for (Value each : values) {
			each.write(writer);
		}
	}

	/**
	 * Write all values to message.
	 *
	 * @param withTypes Include types.
	 */
	public void linkWriteValuesWithVerify(MessageWriter writer) {
		writer.writeUShort(values.size());
		for (Value each : values) {
			writer.writeByte((byte) each.getDataType().value);
			each.write(writer);
		}
	}

	/**
	 * Write values to message if changed in link.
	 *
	 * @param withTypes Include types. Value only if allValues is true.
	 * @param allValues Ignore changed flags and send all values.
	 */
	public void linkWriteValues(MessageWriter writer, StateLink link) {
		int i, count = values.size();
		int changedCount = 0;
		for (i = 0; i < count; i++) {
			if (link.getValueChangedAt(i)) {
				changedCount++;
			}
		}
		changedCount = Math.min(changedCount, 255);

		writer.writeByte((byte) changedCount);

		for (i = 0; i < count; i++) {
			if (!link.getValueChangedAt(i)) {
				continue;
			}

			writer.writeUShort(i);
			values.get(i).write(writer);

			link.setValueChangedAt(i, false);

			changedCount--;
			if (changedCount == 0) {
				break;
			}
		}

		link.setChanged(link.hasChangedValues());
	}

	/**
	 * Invalid value in all state links.
	 */
	private void invalidateValueAt(int index) {
		for (StateLink each : links) {
			each.setValueChangedAt(index, true);
		}
	}

	/**
	 * Invalid value in all state links.
	 */
	private void invalidateValueAtExcept(int index, StateLink link) {
		for (StateLink each : links) {
			each.setValueChangedAt(index, each != link);
		}
	}

	/**
	 * For use by Value only.
	 */
	public void valueChanged(Value value) {
		if (value.updateValue(false)) {
			invalidateValueAt(value.getIndex());
		}
	}

	/**
	 * Create state.
	 */
	public State(boolean readOnly) {
		this.readOnly = readOnly;
	}

	/**
	 * Dispose.
	 */
	public void dispose() {
		for (StateLink each : links) {
			each.dropState();
		}
	}

	/**
	 * Values.
	 */
	public List<Value> getValues() {
		return values;
	}

	/**
	 * Add value.
	 */
	public void addValue(Value value) {
		if (value == null) {
			throw new IllegalArgumentException("value is null");
		}

		values.add(value);

		value.setState(this);
		value.setIndex(values.size() - 1);
	}

	/**
	 * Remove value.
	 */
	public void removeValue(Value value) {
		int index = values.indexOf(value);
		if (index == -1) {
			throw new IllegalArgumentException("value absent");
		}

		values.get(index).setState(null);
		values.remove(index);

		index = 0;
		for (Value each : values) {
			each.setIndex(index++);
		}
	}

	/**
	 * State links.
	 */
	public LinkedList<StateLink> getLinks() {
		return links;
	}

	/**
	 * Read only state.
	 */
	public boolean getReadOnly() {
		return readOnly;
	}
}
