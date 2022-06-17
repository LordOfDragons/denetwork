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

import ch.dragondreams.denetwork.math.HalfFloat;
import ch.dragondreams.denetwork.math.Vector2;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.ValueTypes;

/**
 * Single component floating value.
 */
public class ValueVector2 extends ValueFloating<Vector2> {
	/**
	 * Create value.
	 */
	public ValueVector2(Format format) {
		super(Type.VECTOR2, format, new Vector2(1e-15));

		switch (format) {
		case FLOAT16:
			dataType = ValueTypes.VECTOR2F16;
			break;

		case FLOAT32:
			dataType = ValueTypes.VECTOR2F32;
			break;

		case FLOAT64:
			dataType = ValueTypes.VECTOR2F64;
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = new Vector2();
		lastValue = new Vector2();
	}

	@Override
	public synchronized void read(MessageReader reader) {
		double x, y;

		switch (format) {
		case FLOAT16:
			x = (double) HalfFloat.halfToFloat(reader.readUShort());
			y = (double) HalfFloat.halfToFloat(reader.readUShort());
			break;

		case FLOAT32:
			x = (double) reader.readFloat();
			y = (double) reader.readFloat();
			break;

		case FLOAT64:
			x = reader.readDouble();
			y = reader.readDouble();
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = lastValue = new Vector2(x, y);
	}

	@Override
	public synchronized void write(MessageWriter writer) {
		switch (format) {
		case FLOAT16:
			writer.writeUShort(HalfFloat.floatToHalf((float) value.x()));
			writer.writeUShort(HalfFloat.floatToHalf((float) value.y()));
			break;

		case FLOAT32:
			writer.writeFloat((float) value.x());
			writer.writeFloat((float) value.y());
			break;

		case FLOAT64:
			writer.writeDouble(value.x());
			writer.writeDouble(value.y());
			break;
		}
	}

	@Override
	protected Vector2 maximum(Vector2 a, Vector2 b) {
		return a.greaterThan(b) ? a : b;
	}

	@Override
	protected boolean absDiffLessThanOrEqual(Vector2 a, Vector2 b, Vector2 p) {
		return value.subtract(lastValue).absolute().lessThanEqual(precision);
	}
}
