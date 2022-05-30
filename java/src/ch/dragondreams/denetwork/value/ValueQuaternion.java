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
import ch.dragondreams.denetwork.math.Quaternion;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.ValueTypes;

/**
 * Single component floating value.
 */
public class ValueQuaternion extends ValueFloating<Quaternion> {
	/**
	 * Create value.
	 */
	public ValueQuaternion(Format format) {
		super(Type.QUATERNION, format, new Quaternion(1e-15));

		switch (format) {
		case FLOAT16:
			dataType = ValueTypes.QUATERNIONF16;
			break;

		case FLOAT32:
			dataType = ValueTypes.QUATERNIONF32;
			break;

		case FLOAT64:
			dataType = ValueTypes.QUATERNIONF64;
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = new Quaternion();
		lastValue = new Quaternion();
	}

	@Override
	public void read(MessageReader reader) {
		double x, y, z, w;

		switch (format) {
		case FLOAT16:
			x = (double) HalfFloat.halfToFloat(reader.readUShort());
			y = (double) HalfFloat.halfToFloat(reader.readUShort());
			z = (double) HalfFloat.halfToFloat(reader.readUShort());
			w = (double) HalfFloat.halfToFloat(reader.readUShort());
			break;

		case FLOAT32:
			x = (double) reader.readFloat();
			y = (double) reader.readFloat();
			z = (double) reader.readFloat();
			w = (double) reader.readFloat();
			break;

		case FLOAT64:
			x = reader.readDouble();
			y = reader.readDouble();
			z = reader.readDouble();
			w = reader.readDouble();
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = lastValue = new Quaternion(x, y, z, w);
	}

	@Override
	public void write(MessageWriter writer) {
		switch (format) {
		case FLOAT16:
			writer.writeUShort(HalfFloat.floatToHalf((float) value.x()));
			writer.writeUShort(HalfFloat.floatToHalf((float) value.y()));
			writer.writeUShort(HalfFloat.floatToHalf((float) value.z()));
			writer.writeUShort(HalfFloat.floatToHalf((float) value.w()));
			break;

		case FLOAT32:
			writer.writeFloat((float) value.x());
			writer.writeFloat((float) value.y());
			writer.writeFloat((float) value.z());
			writer.writeFloat((float) value.w());
			break;

		case FLOAT64:
			writer.writeDouble(value.x());
			writer.writeDouble(value.y());
			writer.writeDouble(value.z());
			writer.writeDouble(value.w());
			break;
		}
	}

	@Override
	protected Quaternion maximum(Quaternion a, Quaternion b) {
		return a.greaterThan(b) ? a : b;
	}

	@Override
	protected boolean absDiffLessThanOrEqual(Quaternion a, Quaternion b, Quaternion p) {
		return value.subtract(lastValue).absolute().lessThanEqual(precision);
	}
}
