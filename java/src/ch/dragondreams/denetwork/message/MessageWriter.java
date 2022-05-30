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

package ch.dragondreams.denetwork.message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;

import ch.dragondreams.denetwork.math.Point2;
import ch.dragondreams.denetwork.math.Point3;
import ch.dragondreams.denetwork.math.Quaternion;
import ch.dragondreams.denetwork.math.Vector2;
import ch.dragondreams.denetwork.math.Vector3;

public class MessageWriter {
	private byte[] data;
	private int position = 0;

	static private ByteBuffer byteBuffer = createByteBuffer();

	private static ByteBuffer createByteBuffer() {
		ByteBuffer buffer = ByteBuffer.allocate(24);
		buffer.order(ByteOrder.BIG_ENDIAN);
		return buffer;
	}

	/**
	 * Create message writer.
	 */
	public MessageWriter(Message message) {
		data = message.getData();
	}

	/**
	 * Write value.
	 */
	public MessageWriter writeChar(char value) {
		data[position++] = (byte) value;
		return this;
	}

	public MessageWriter writeByte(byte value) {
		data[position++] = value;
		return this;
	}

	public MessageWriter writeShort(short value) {
		byteBuffer.clear();
		byteBuffer.putShort(value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 2);
		position += 2;
		return this;
	}

	public MessageWriter writeUShort(int value) {
		byteBuffer.clear();
		byteBuffer.putShort((short) value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 2);
		position += 2;
		return this;
	}

	public MessageWriter writeInt(int value) {
		byteBuffer.clear();
		byteBuffer.putInt(value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 4);
		position += 4;
		return this;
	}

	public MessageWriter writeUInt(long value) {
		byteBuffer.clear();
		byteBuffer.putInt((int) value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 4);
		position += 4;
		return this;
	}

	public MessageWriter writeLong(long value) {
		byteBuffer.clear();
		byteBuffer.putLong(value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 8);
		position += 8;
		return this;
	}

	public MessageWriter writeULong(long value) {
		return writeLong(value);
	}

	public MessageWriter writeFloat(float value) {
		byteBuffer.clear();
		byteBuffer.putFloat(value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 4);
		position += 4;
		return this;
	}

	public MessageWriter writeDouble(double value) {
		byteBuffer.clear();
		byteBuffer.putDouble(value);
		byteBuffer.rewind();
		byteBuffer.get(data, position, 8);
		position += 8;
		return this;
	}

	public MessageWriter writeString8(String string) {
		byte[] utf8 = string.getBytes(StandardCharsets.UTF_8);
		if (utf8.length > 255) {
			throw new IllegalArgumentException("string.utf8.length > 255");
		}
		writeByte((byte) utf8.length);
		return write(utf8, 0, utf8.length);
	}

	public MessageWriter writeString16(String string) {
		byte[] utf8 = string.getBytes(StandardCharsets.UTF_8);
		if (utf8.length > 65535) {
			throw new IllegalArgumentException("string.utf8.length > 65535");
		}
		writeUShort(utf8.length);
		return write(utf8, 0, utf8.length);
	}

	public MessageWriter writeVector2(Vector2 vector) {
		byteBuffer.clear();
		byteBuffer.putFloat(0, (float) vector.x());
		byteBuffer.putFloat(1, (float) vector.y());
		byteBuffer.rewind();
		byteBuffer.get(data, position, 8);
		position += 8;
		return this;
	}

	public MessageWriter writeVector3(Vector3 vector) {
		byteBuffer.clear();
		byteBuffer.putFloat(0, (float) vector.x());
		byteBuffer.putFloat(1, (float) vector.y());
		byteBuffer.putFloat(2, (float) vector.z());
		byteBuffer.rewind();
		byteBuffer.get(data, position, 12);
		position += 12;
		return this;
	}

	public MessageWriter writeQuaternion(Quaternion quaternion) {
		byteBuffer.clear();
		byteBuffer.putFloat(0, (float) quaternion.x());
		byteBuffer.putFloat(1, (float) quaternion.y());
		byteBuffer.putFloat(2, (float) quaternion.z());
		byteBuffer.putFloat(3, (float) quaternion.w());
		byteBuffer.rewind();
		byteBuffer.get(data, position, 16);
		position += 16;
		return this;
	}

	public MessageWriter writePoint2(Point2 point) {
		byteBuffer.clear();
		byteBuffer.putInt(0, (int) point.x());
		byteBuffer.putInt(1, (int) point.y());
		byteBuffer.rewind();
		byteBuffer.get(data, position, 8);
		position += 8;
		return this;
	}

	public MessageWriter writePoint3(Point3 point) {
		byteBuffer.clear();
		byteBuffer.putInt(0, (int) point.x());
		byteBuffer.putInt(1, (int) point.y());
		byteBuffer.putInt(2, (int) point.z());
		byteBuffer.rewind();
		byteBuffer.get(data, position, 12);
		position += 12;
		return this;
	}

	public MessageWriter writeDVector(Vector3 vector) {
		byteBuffer.clear();
		byteBuffer.putDouble(0, vector.x());
		byteBuffer.putDouble(1, vector.y());
		byteBuffer.putDouble(2, vector.z());
		byteBuffer.rewind();
		byteBuffer.get(data, position, 24);
		position += 24;
		return this;
	}

	public MessageWriter write(byte[] buffer, int offset, int length) {
		if (offset < 0) {
			throw new IllegalArgumentException("offset < 0");
		}
		if (length < 0) {
			throw new IllegalArgumentException("length < 0");
		}

		System.arraycopy(buffer, offset, data, position, length);
		position += length;
		return this;
	}

	public MessageWriter write(Message message) {
		System.arraycopy(message.getData(), 0, data, position, message.getLength());
		position += message.getLength();
		return this;
	}
}
