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
import java.net.InetSocketAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.StandardProtocolFamily;
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
public class DatagramChannelEndpoint implements Endpoint {
	private class ReceiveDatagramTask implements Runnable {
		@Override
		public void run() {
			try {
				while (receiveDatagram()) {
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	private Listener listener = null;
	private SocketAddress address = null;
	private DatagramChannel channel = null;
	private final ByteBuffer buffer = ByteBuffer.allocate(8192);
	private boolean keepThreadRunning = true;
	private Thread threadReceive = null;

	/**
	 * Create datagram channel endpoint.
	 */
	public DatagramChannelEndpoint() {
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
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#dispose()
	 */
	@Override
	public void dispose() {
		close();
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#open(SocketAddress,Message)
	 */
	@Override
	public void open(SocketAddress address, Listener listener) throws IOException {
		if (channel != null) {
			throw new IllegalArgumentException("already open");
		}
		if (listener == null) {
			throw new IllegalArgumentException("listener is null");
		}

		if (address == null) {
			address = new InetSocketAddress(0);
		}

		channel = DatagramChannel.open(StandardProtocolFamily.INET);
		channel.bind(address);

		this.listener = listener;
		this.address = address;

		threadReceive = new Thread(new ReceiveDatagramTask());
		threadReceive.start();
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#close()
	 */
	@Override
	public void close() {
		if (channel == null) {
			return;
		}

		try {
			synchronized (this) {
				keepThreadRunning = false;
			}
			channel.close();
		} catch (IOException ignore) {
		}

		try {
			threadReceive.join();
		} catch (InterruptedException ignore) {
		}
		threadReceive = null;
	}

	/**
	 * Called asynchronously by receiver thread.
	 */
	protected boolean receiveDatagram() throws IOException {
		DatagramChannel safeChannel;
		synchronized (this) {
			if (!keepThreadRunning || channel == null) {
				return false;
			}
			safeChannel = channel;
		}

		buffer.clear();
		SocketAddress senderAddress = safeChannel.receive(buffer);
		if (senderAddress == null) {
			return false;
		}
		buffer.flip();

		Message message = new Message();
		message.setData(Arrays.copyOfRange(buffer.array(), buffer.position(), buffer.position() + buffer.limit()));
		message.setLength(buffer.limit());

		Listener safeListener;
		synchronized (this) {
			safeListener = listener;
		}
		if (safeListener != null) {
			safeListener.receivedDatagram(senderAddress, message);
		}

		return true;
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see
	 * ch.dragondreams.denetwork.endpoint.Endpoint#sendDatagram(ch.dragondreams.
	 * denetwork.SocketEndpoint#Datagram)
	 */
	@Override
	public void sendDatagram(SocketAddress address, Message message) throws IOException {
		channel.send(ByteBuffer.wrap(message.getData(), 0, message.getLength()), address);
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
