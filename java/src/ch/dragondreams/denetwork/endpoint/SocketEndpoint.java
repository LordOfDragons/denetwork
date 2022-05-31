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
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.List;

import ch.dragondreams.denetwork.message.Message;

/**
 * Endpoint using java.net.DatagramChannel.
 */
public class SocketEndpoint implements Endpoint {
	private final DatagramChannel channel;
	private final ByteBuffer buffer = ByteBuffer.allocate(8192);
	protected SocketAddress address;

	/**
	 * Create socket endpoint.
	 *
	 * @throws IOException
	 */
	public SocketEndpoint() throws IOException {
		channel = DatagramChannel.open();
		channel.configureBlocking(false);
		channel.bind(null);
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#getAddress()
	 */
	@Override
	public SocketAddress getAddress() {
		return address;
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#setAddress(java.net.
	 * SocketAddress)
	 */
	@Override
	public void setAddress(SocketAddress address) {
		this.address = address;
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#dispose()
	 */
	@Override
	public void dispose() {
		try {
			channel.close();
		} catch (IOException ignore) {
		}
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#bind()
	 */
	@Override
	public void bind() throws IOException {
		channel.close(); // to allow re-binding
		channel.bind(address);
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#receiveDatagram()
	 */
	@Override
	public Datagram receiveDatagram() throws IOException {
		buffer.reset();

		SocketAddress senderAddress = channel.receive(buffer);
		if (senderAddress == null) {
			return null;
		}

		buffer.flip();

		Message message = new Message();
		message.setData(Arrays.copyOfRange(buffer.array(), buffer.position(), buffer.position() + buffer.limit()));
		message.setLength(buffer.limit());
		return new Datagram(message, senderAddress);
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see
	 * ch.dragondreams.denetwork.endpoint.Endpoint#sendDatagram(ch.dragondreams.
	 * denetwork.SocketEndpoint#Datagram)
	 */
	@Override
	public void sendDatagram(Datagram datagram) throws IOException {
		channel.send(ByteBuffer.wrap(datagram.message.getData(), 0, datagram.message.getLength()), address);
	}

	/**
	 * Get list of public IP address.
	 */
	public static List<String> getPublicAddresses() throws IOException {
		List<String> addresses = new ArrayList<>();

		Enumeration<NetworkInterface> networkInterfaceEnumeration = NetworkInterface.getNetworkInterfaces();
		while (networkInterfaceEnumeration.hasMoreElements()) {
			for (InterfaceAddress interfaceAddress : networkInterfaceEnumeration.nextElement()
					.getInterfaceAddresses()) {
				if (interfaceAddress.getAddress().isSiteLocalAddress()) {
					addresses.add(interfaceAddress.getAddress().getHostAddress());
				}
			}
		}

		return addresses;
	}
}
