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

import java.util.Date;

/**
 * Network message.
 */
public class Message {
	byte[] data = new byte[1];
	int length = 0;
	Date timestamp = new Date();



	/**
	 * Message data. Can be longer than actual message length.
	 */
	public byte[] getData() {
		return data;
	}

	/**
	 * Set message data. Can be longer than actual message length.
	 */
	public void setData(byte[] data) {
		if (data == null) {
			throw new IllegalArgumentException("data is null");
		}
		this.data = data;
	}

	/**
	 * Length of message. Can be less than data size.
	 */
	public int getLength() {
		return length;
	}

	/**
	 * Set length of message. Can be less than data size. If length is larger than
	 * current data size the data size is increased. The data content is not retained
	 * if the size is increased.
	 */
	public void setLength(int length) {
		if (length < 0) {
			throw new IllegalArgumentException("length < 0");
		}
		this.length = length;
		if (data.length < length) {
			data = new byte[length];
		}
	}

	/**
	 * Same as setLength() but keeps content up to previous length intact.
	 */
	public void setLengthRetain(int length) {
		if (length < 0) {
			throw new IllegalArgumentException("length < 0");
		}
		
		int prevLength = length;
		
		this.length = length;
		if(data.length >= length){
			return;
		}
		
		byte[] temp = data;
		data = new byte[length];
		if (prevLength > 0){
			System.arraycopy(temp, 0, data, prevLength, length);
		}
	}

	/**
	 * Message timestamp.
	 */
	public Date getTimestamp() {
		return timestamp;
	}

	/**
	 * Set message timestamp.
	 */
	public void setTimestamp(Date timestamp) {
		if (timestamp != null) {
			throw new IllegalArgumentException("timestamp is null");
		}
		this.timestamp = timestamp;
	}
}
