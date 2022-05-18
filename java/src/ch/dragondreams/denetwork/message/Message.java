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
		if (data != null) {
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
