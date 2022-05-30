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
	 *  Connection Request:
	 *  [ 0 ] [ protocols ]
	 *
	 *  protocols:  // list of protocols supported by client
	 *     [ count:uint16 ] [ protocol:uint16 ]{ 1..n }
	 */
	CONNECTION_REQUEST(0),

	/**
	 * Connection Ack:
	 * [ 1 ] [ resultCode:uint8 ]
	 *
	 * resultCode:
	 *    0: Connection Accepted
	 *    1: Connection Rejected
	 *    2: Connection Rejected because no common protocols
	 *
	 * if connection is accepted the message also contains:
	 *    [ protocol:uint16 ]
	 *
	 * protocol:
	 *    The chosen protocol
	 */
	CONNECTION_ACK(1),

	/**
	 * Close Connection:
	 * [ 2 ]
	 */
	CONNECTION_CLOSE(2),

	/**
	 * Message:
	 * [ 3 ] [ data ]
	 */
	MESSAGE(3),

	/**
	 * Reliable message:
	 * [ 4 ] [ number:uint16 ] [ data ]
	 */
	RELIABLE_MESSAGE(4),

	/**
	 * Link state:
	 * [ 5 ] [ number:uint16 ] [ link_id:uint16 ] [ flags ] [ message ] [ values ]
	 *
	 * flags:
	 *    0x1: create read only state
	 *
	 * message:
	 *    [ length:uint16 ] [ message_bytes:uint8 ]{ 1..n }
	 *
	 * values:
	 *    [ value_count:uint16 ] ( [ value_type:uint8 ] [ value_data:* ] ){ 1..n }
	 */
	RELIABLE_LINK_STATE(5),

	/**
	 * Reliable ack:
	 * [ 6 ] [ number:uint16 ] [ code:uint8 ]
	 *
	 * code:
	 *    [ 0 ] received successfully
	 *    [ 1 ] failed
	 */
	RELIABLE_ACK(6),

	/**
	 * Link up:
	 * [ 7 ] [ link_id:uint16 ]
	 */
	LINK_UP(7),

	/**
	 * Link down:
	 * [ 8 ] [ link_id:uint16 ]
	 */
	LINK_DOWN(8),

	/**
	 * Link update:
	 * [ 9 ] [ link_count:uint8 ] [ link ]{ 1..link_count }
	 *
	 * link:
	 *    [ link_id:uint16 ] [ value_count:uint8 ] [ value ]{ 1..value_count }
	 *
	 * value:
	 *    [ value_index:uint16 ] [ value_data:* ]
	 */
	LINK_UPDATE(9);

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
