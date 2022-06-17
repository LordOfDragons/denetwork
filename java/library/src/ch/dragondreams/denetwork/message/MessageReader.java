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

public class MessageReader {
	private byte[] data;
	private int length;
	private int position = 0;
	static private ByteBuffer byteBuffer = createByteBuffer();

	private static ByteBuffer createByteBuffer() {
		ByteBuffer buffer = ByteBuffer.allocate(24);
		buffer.order(ByteOrder.LITTLE_ENDIAN);
		return buffer;
	}

	/**
	 * Create message reader.
	 */
	public MessageReader(Message message) {
		data = message.getData();
		length = message.getLength();
	}


	/**
	 * Length of message data.
	 */
	public int length() {
		return this.length;
	}

	/**
	 * Read position in bytes from the start of the message.
	 */
	public int position() {
		return this.position;
	}

	/**
	 * Read value.
	 */
	public char readChar() {
		return (char) data[position++];
	}

	public byte readByte() {
		return data[position++];
	}

	public short readShort() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 2);
		position += 2;
		byteBuffer.rewind();
        return byteBuffer.getShort();
	}

	public int readUShort() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 2);
		position += 2;
		byteBuffer.rewind();
        return byteBuffer.getShort() & 0xffff;
	}

	public int readInt() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 4);
		position += 4;
		byteBuffer.rewind();
        return byteBuffer.getInt();
	}

	public long readUInt() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 4);
		position += 4;
		byteBuffer.rewind();
        return byteBuffer.getInt() & 0xffffffff;
	}

	public long readLong() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 8);
		position += 8;
		byteBuffer.rewind();
        return byteBuffer.getLong();
	}

	public long readULong() {
		return readLong();
	}

	public float readFloat() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 4);
		position += 4;
		byteBuffer.rewind();
        return byteBuffer.getFloat();
	}

	public double readDouble() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 8);
		position += 8;
		byteBuffer.rewind();
        return byteBuffer.getDouble();
	}

	public String readString8() {
		int len = readByte();
		String string = new String(data, position, len, StandardCharsets.UTF_8);
		position += len;
		return string;
	}

	public String readString16() {
		int len = readUShort();
		String string = new String(data, position, len, StandardCharsets.UTF_8);
		position += len;
		return string;
	}

	public Vector2 readVector2() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 8);
		position += 8;
		byteBuffer.rewind();
		return new Vector2(byteBuffer.getFloat(0), byteBuffer.getFloat(1));
	}

	public Vector3 readVector3() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 12);
		position += 12;
		byteBuffer.rewind();
		return new Vector3(byteBuffer.getFloat(0), byteBuffer.getFloat(1), byteBuffer.getFloat(2));
	}

	public Quaternion readQuaternion() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 16);
		position += 16;
		byteBuffer.rewind();
		return new Quaternion(byteBuffer.getFloat(0), byteBuffer.getFloat(1),
				byteBuffer.getFloat(2), byteBuffer.getFloat(3));
	}

	public Point2 readPoint2() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 8);
		position += 8;
		byteBuffer.rewind();
		return new Point2(byteBuffer.getInt(0), byteBuffer.getInt(1));
	}

	public Point3 readPoint3() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 12);
		position += 12;
		byteBuffer.rewind();
		return new Point3(byteBuffer.getInt(0), byteBuffer.getInt(1), byteBuffer.getInt(2));
	}

	public Vector3 readDVector() {
		byteBuffer.clear();
		byteBuffer.put(data, position, 24);
		position += 24;
		byteBuffer.rewind();
		return new Vector3(byteBuffer.getDouble(0), byteBuffer.getDouble(1), byteBuffer.getDouble(2));
	}

	/**
	 * Read data.
	 */
	public void read(byte[] buffer, int offset, int length) {
		if (offset < 0) {
			throw new IllegalArgumentException("offset < 0");
		}
		if (length < 0) {
			throw new IllegalArgumentException("length < 0");
		}

		System.arraycopy(data, position, buffer, offset, length);
		position += length;
	}

	/**
	 * Read message.
	 */
	public void read(Message message) {
		System.arraycopy(data, position, message.getData(), 0, message.getLength());
		position += message.getLength();
	}
}
