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

import ch.dragondreams.denetwork.math.Point2;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.ValueTypes;

/**
 * 2 component integer vector value.
 */
public class ValuePoint2 extends ValueInteger<Point2> {
	/**
	 * Create value.
	 */
	public ValuePoint2(Format format) {
		super(Type.VECTOR2, format);

		switch (format) {
		case SINT8:
			dataType = ValueTypes.POINT2S8;
			break;

		case UINT8:
			dataType = ValueTypes.POINT2U8;
			break;

		case SINT16:
			dataType = ValueTypes.POINT2S16;
			break;

		case UINT16:
			dataType = ValueTypes.POINT2U16;
			break;

		case SINT32:
			dataType = ValueTypes.POINT2S32;
			break;

		case UINT32:
			dataType = ValueTypes.POINT2U32;
			break;

		case SINT64:
			dataType = ValueTypes.POINT2S64;
			break;

		case UINT64:
			dataType = ValueTypes.POINT2U64;
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		lastValue = value = new Point2();
	}

	@Override
	public void read(MessageReader reader) {
		long x, y;

		switch (format) {
		case SINT8:
			x = (long) reader.readChar();
			y = (long) reader.readChar();
			break;

		case UINT8:
			x = (long) reader.readByte();
			y = (long) reader.readByte();
			break;

		case SINT16:
			x = (long) reader.readShort();
			y = (long) reader.readShort();
			break;

		case UINT16:
			x = (long) reader.readUShort();
			y = (long) reader.readUShort();
			break;

		case SINT32:
			x = (long) reader.readInt();
			y = (long) reader.readInt();
			break;

		case UINT32:
			x = reader.readUInt();
			y = reader.readUInt();
			break;

		case SINT64:
			x = reader.readLong();
			y = reader.readLong();
			break;

		case UINT64:
			x = reader.readULong();
			y = reader.readULong();
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = lastValue = new Point2(x, y);
	}

	@Override
	public void write(MessageWriter writer) {
		switch (format) {
		case SINT8:
			writer.writeChar((char) value.x());
			writer.writeChar((char) value.y());
			break;

		case UINT8:
			writer.writeByte((byte) value.x());
			writer.writeByte((byte) value.y());
			break;

		case SINT16:
			writer.writeShort((short) value.x());
			writer.writeShort((short) value.y());
			break;

		case UINT16:
			writer.writeUShort((int) value.x());
			writer.writeUShort((int) value.y());
			break;

		case SINT32:
			writer.writeInt((int) value.x());
			writer.writeInt((int) value.y());
			break;

		case UINT32:
			writer.writeUInt(value.x());
			writer.writeUInt(value.y());
			break;

		case SINT64:
			writer.writeLong(value.x());
			writer.writeLong(value.y());
			break;

		case UINT64:
			writer.writeULong(value.x());
			writer.writeULong(value.y());
			break;
		}
	}
}
