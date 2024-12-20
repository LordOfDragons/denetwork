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

package ch.dragondreams.denetwork.protocol;

/**
 * Protocol command codes.
 */
public enum CommandCodes {
	/**
	 *  Connection Request.
	 */
	CONNECTION_REQUEST(0),

	/**
	 * Connection Acknowledge.
	 */
	CONNECTION_ACK(1),

	/**
	 * Close Connection.
	 */
	CONNECTION_CLOSE(2),

	/**
	 * Unreliable message.
	 */
	MESSAGE(3),

	/**
	 * Reliable message.
	 */
	RELIABLE_MESSAGE(4),

	/**
	 * Link state.
	 */
	RELIABLE_LINK_STATE(5),

	/**
	 * Reliable acknowledge.
	 */
	RELIABLE_ACK(6),

	/**
	 * Link up.
	 */
	LINK_UP(7),

	/**
	 * Link down.
	 */
	LINK_DOWN(8),

	/**
	 * Link update.
	 */
	LINK_UPDATE(9),

	/**
	 * Long reliable message.
	 */
	RELIABLE_MESSAGE_LONG(10),

	/**
	 * Long link state.
	 */
	RELIABLE_LINK_STATE_LONG(11);

	public final int value;

	private CommandCodes(int value) {
		this.value = value;
	}

	public static CommandCodes withValue(int value) {
		for (CommandCodes each : values()) {
			if (each.value == value) {
				return each;
			}
		}
		return null;
	}
}
