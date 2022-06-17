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
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.ValueTypes;

/**
 * Single component floating value.
 */
public class ValueFloat extends ValueFloating<Double> {
	/**
	 * Create value.
	 */
	public ValueFloat(Format format) {
		super(Type.FLOAT, format, 1e-15);

		switch (format) {
		case FLOAT16:
			dataType = ValueTypes.FLOAT16;
			break;

		case FLOAT32:
			dataType = ValueTypes.FLOAT32;
			break;

		case FLOAT64:
			dataType = ValueTypes.FLOAT64;
			break;

		default:
			throw new IllegalArgumentException("format");
		}

		value = 0.0;
		lastValue = 0.0;
	}

	@Override
	public synchronized void read(MessageReader reader) {
		switch (format) {
		case FLOAT16:
			lastValue = (double) HalfFloat.halfToFloat(reader.readUShort());
			break;

		case FLOAT32:
			lastValue = (double) reader.readFloat();
			break;

		case FLOAT64:
			lastValue = reader.readDouble();
			break;
		}

		value = lastValue;
	}

	@Override
	public synchronized void write(MessageWriter writer) {
		switch (format) {
		case FLOAT16:
			writer.writeUShort(HalfFloat.floatToHalf(value.floatValue()));
			break;

		case FLOAT32:
			writer.writeFloat(value.floatValue());
			break;

		case FLOAT64:
			writer.writeDouble(value.doubleValue());
			break;
		}
	}

	@Override
	protected Double maximum(Double a, Double b) {
		return Math.max(a, b);
	}

	@Override
	protected boolean absDiffLessThanOrEqual(Double a, Double b, Double p) {
		return Math.abs(value - lastValue) <= precision;
	}
}
