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

package ch.dragondreams.denetwork.endpoint;

import java.io.IOException;
import java.net.SocketAddress;

import ch.dragondreams.denetwork.message.Message;

/**
 * Endpoint supporting sending and receiving datagrams.
 */
abstract public interface Endpoint {
	/**
	 * Received datagram.
	 */
	public static class Datagram {
		public final Message message;
		public final SocketAddress address;

		/**
		 * Create received datagram.
		 */
		public Datagram(Message message, SocketAddress address) {
			if (message == null) {
				throw new IllegalArgumentException("message is null");
			}
			if (address == null) {
				throw new IllegalArgumentException("address is null");
			}
			this.message = message;
			this.address = address;
		}
	}

	/**
	 * Get socket address.
	 */
	abstract public SocketAddress getAddress();

	/**
	 * Set socket address.
	 */
	abstract public void setAddress(SocketAddress address);

	/**
	 * Dispose of endpoint.
	 */
	abstract public void dispose();

	/**
	 * Bind endpoint.
	 */
	abstract public void bind() throws IOException;

	/**
	 * Receive datagram from socket.
	 */
	abstract public Datagram receiveDatagram() throws IOException;

	/**
	 * Send datagram.
	 */
	abstract public void sendDatagram(Datagram datagram) throws IOException;
}
