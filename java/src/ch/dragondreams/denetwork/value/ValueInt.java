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
 * Single component integer value.
 */
public class ValueInt extends ValueInteger<Long> {
	/**
	 * Create value.
	 */
	public ValueInt(Format format) {
		super(Type.INTEGER, format);

		switch (format) {
		case SINT8:
			dataType = ValueTypes.SINT8;
			break;

		case UINT8:
			dataType = ValueTypes.UINT8;
			break;

		case SINT16:
			dataType = ValueTypes.SINT16;
			break;

		case UINT16:
			dataType = ValueTypes.UINT16;
			break;

		case SINT32:
			dataType = ValueTypes.SINT32;
			break;

		case UINT32:
			dataType = ValueTypes.UINT32;
			break;

		case SINT64:
			dataType = ValueTypes.SINT64;
			break;

		case UINT64:
			dataType = ValueTypes.UINT64;
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = 0L;
		lastValue = 0L;
	}

	@Override
	public void read(MessageReader reader) {
		switch (format) {
		case SINT8:
			lastValue = (long) reader.readChar();
			break;

		case UINT8:
			lastValue = (long) reader.readByte();
			break;

		case SINT16:
			lastValue = (long) reader.readShort();
			break;

		case UINT16:
			lastValue = (long) reader.readUShort();
			break;

		case SINT32:
			lastValue = (long) reader.readInt();
			break;

		case UINT32:
			lastValue = reader.readUInt();
			break;

		case SINT64:
			lastValue = reader.readLong();
			break;

		case UINT64:
			lastValue = reader.readULong();
			break;
		}

		value = lastValue;
	}

	@Override
	public void write(MessageWriter writer) {
		switch (format) {
		case SINT8:
			writer.writeChar((char) value.intValue());
			break;

		case UINT8:
			writer.writeByte(value.byteValue());
			break;

		case SINT16:
			writer.writeShort(value.shortValue());
			break;

		case UINT16:
			writer.writeUShort(value.intValue());
			break;

		case SINT32:
			writer.writeInt(value.intValue());
			break;

		case UINT32:
			writer.writeUInt(value.longValue());
			break;

		case SINT64:
			writer.writeLong(value.longValue());
			break;

		case UINT64:
			writer.writeULong(value.longValue());
			break;
		}
	}
}
