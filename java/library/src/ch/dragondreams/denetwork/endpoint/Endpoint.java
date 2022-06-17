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
	 * Listener.
	 */
	public interface Listener {
		/**
		 * Datagram received.
		 */
		void receivedDatagram(SocketAddress address, Message message);
	}

	/**
	 * Get socket address.
	 */
	abstract public SocketAddress getAddress();

	/**
	 * Dispose of endpoint.
	 */
	abstract public void dispose();

	/**
	 * Open endpoint delivering events to the provided listener. If address is null
	 * listen on random port.
	 */
	abstract public void open(SocketAddress address, Listener listener) throws IOException;

	/**
	 * Close endpoint if open and stop delivering events to listener provided in the
	 * previous open call.
	 */
	abstract public void close();

	/**
	 * Send datagram.
	 */
	abstract public void sendDatagram(SocketAddress address, Message message) throws IOException;
}
