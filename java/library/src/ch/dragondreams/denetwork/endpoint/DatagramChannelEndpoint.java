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
import java.net.Inet6Address;
import java.net.InetSocketAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.StandardProtocolFamily;
import java.nio.ByteBuffer;
import java.nio.channels.ClosedChannelException;
import java.nio.channels.DatagramChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.utils.CloseableReentrantLock;

/**
 * Endpoint using java.net.DatagramChannel.
 */
public class DatagramChannelEndpoint implements Endpoint {
	public static final String CLASS_NAME = DatagramChannelEndpoint.class.getCanonicalName();
	public static final String LOGGER_NAME = DatagramChannelEndpoint.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	/**
	 * Task receiving datagrams.
	 */
	protected class ReceiveDatagramTask implements Runnable {
		protected final String className = ReceiveDatagramTask.class.getCanonicalName();

		protected final ByteBuffer buffer = ByteBuffer.allocate(8192);
		protected final DatagramChannel channel;
		protected final Listener listener;

		protected boolean keepRunning = true;

		public ReceiveDatagramTask(DatagramChannel channel, Listener listener) {
			this.channel = channel;
			this.listener = listener;
		}

		public void cancel() {
			keepRunning = false;
		}

		@Override
		public void run() {
			logger.entering(className, "run");
			try {
				while (keepRunning) {
					receiveDatagram();
				}
			} catch (ClosedChannelException e) {
				logger.info("channel closed");
			} catch (IOException e) {
				logger.log(Level.SEVERE, "receiveDiagram failed", e);
			}
			logger.exiting(className, "run");
		}

		protected void receiveDatagram() throws IOException {
			buffer.clear();
			SocketAddress senderAddress = channel.receive(buffer);
			if (senderAddress == null) {
				logger.info("senderAddress is null");
				keepRunning = false;
				return;
			}
			buffer.flip();

			Message message = new Message();
			message.setData(Arrays.copyOfRange(buffer.array(), buffer.position(), buffer.position() + buffer.limit()));
			message.setLength(buffer.limit());

			if (keepRunning && listener != null) {
				listener.receivedDatagram(senderAddress, message);
			}
		}
	}

	private SocketAddress address = null;
	private DatagramChannel channel = null;
	private ReceiveDatagramTask taskReceive = null;
	private Thread threadReceive = null;

	private final CloseableReentrantLock lock = new CloseableReentrantLock();

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
		logger.entering(CLASS_NAME, "dispose");
		close();
		logger.exiting(CLASS_NAME, "dispose");
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#open(SocketAddress,Message)
	 */
	@Override
	public void open(SocketAddress address, Listener listener) throws IOException {
		logger.entering(CLASS_NAME, "open", address);

		try (CloseableReentrantLock locked = lock.open()) {
			if (channel != null) {
				throw new IllegalArgumentException("already open");
			}
			if (listener == null) {
				throw new IllegalArgumentException("listener is null");
			}

			if (address == null) {
				address = new InetSocketAddress(0);
			}

			InetSocketAddress isaddr = (InetSocketAddress) address;
			if (isaddr.getAddress() instanceof Inet6Address) {
				channel = DatagramChannel.open(StandardProtocolFamily.INET6);
			} else {
				channel = DatagramChannel.open(StandardProtocolFamily.INET);
			}
			channel.bind(address);

			this.address = address;

			taskReceive = new ReceiveDatagramTask(channel, listener);

			threadReceive = new Thread(taskReceive);
			threadReceive.start();
		}

		logger.exiting(CLASS_NAME, "open");
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see ch.dragondreams.denetwork.endpoint.Endpoint#close()
	 */
	@Override
	public void close() {
		logger.entering(CLASS_NAME, "close");

		try (CloseableReentrantLock locked = lock.open()) {
			if (taskReceive != null) {
				taskReceive.cancel();
				taskReceive = null;
			}

			threadReceive = null;

			if (channel != null) {
				try {
					channel.close();
				} catch (IOException ignore) {
				}
				channel = null;
			}
		}

		logger.exiting(CLASS_NAME, "close");
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
		try (CloseableReentrantLock locked = lock.open()) {
			channel.send(ByteBuffer.wrap(message.getData(), 0, message.getLength()), address);
		}
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
				if (interfaceAddress.getAddress().isSiteLocalAddress()
						|| interfaceAddress.getAddress().isLinkLocalAddress()) {
					addresses.add(interfaceAddress.getAddress().getHostAddress());
				}
			}
		}

		return addresses;
	}

	/**
	 * Get list of all IP address.
	 */
	public static List<String> getAllAddresses() throws IOException {
		List<String> addresses = new ArrayList<>();

		Enumeration<NetworkInterface> networkInterfaceEnumeration = NetworkInterface.getNetworkInterfaces();
		while (networkInterfaceEnumeration.hasMoreElements()) {
			for (InterfaceAddress interfaceAddress : networkInterfaceEnumeration.nextElement()
					.getInterfaceAddresses()) {
				addresses.add(interfaceAddress.getAddress().getHostAddress());
			}
		}

		return addresses;
	}

	/**
	 * Resolve address.
	 * 
	 * Address is in the format "hostnameOrIP" or "hostnameOrIP:port". You can use a
	 * resolvable hostname or an IPv4. If the port is not specified the default port
	 * 3413 is used.
	 * 
	 * If you overwrite CreateSocket() you have to also overwrite this method to
	 * resolve address using the appropriate method.
	 */
	public static SocketAddress resolveAddress(String address) {
		if (address.isEmpty()) {
			throw new IllegalArgumentException("address is empty");
		}

		int delimiter = address.lastIndexOf(':');
		int portBegin = -1;
		String host;

		if (delimiter != -1) {
			if (address.charAt(0) == '[') {
				// "[IPv6]:port"
				if (address.charAt(delimiter - 1) != ']') {
					throw new IllegalArgumentException("address invalid");
				}

				host = address.substring(1, delimiter - 1);
				portBegin = delimiter + 1;

			} else if (address.indexOf(':') != delimiter) {
				// "IPv6"
				host = address;

			} else {
				// "IPv4:port" or "hostname:port"
				host = address.substring(0, delimiter);
				portBegin = delimiter + 1;
			}

		} else {
			// "IPv4" or "hostname"
			host = address;
		}

		int port = 3413;

		if (portBegin != -1) {
			port = Integer.parseInt(address.substring(portBegin));
		}

		return new InetSocketAddress(host, port);
	}
}
