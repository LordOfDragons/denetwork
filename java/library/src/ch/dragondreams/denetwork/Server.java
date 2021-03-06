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

package ch.dragondreams.denetwork;

import java.io.IOException;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.endpoint.DatagramChannelEndpoint;
import ch.dragondreams.denetwork.endpoint.Endpoint;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.CommandCodes;
import ch.dragondreams.denetwork.protocol.ConnectionAck;
import ch.dragondreams.denetwork.protocol.Protocols;
import ch.dragondreams.denetwork.utils.CloseableReentrantLock;

/**
 * Network server.
 *
 * Allows clients speaking Drag[en]gine Network Protocol to connect.
 *
 * To use this class create a subclass and overwrite createConnection() and
 * clientConnected(). The method createConnection() method creates a Connection
 * instance for each connecting client. By overwriting this method you can
 * create a subclass of Connection handling the client. Overwriting
 * clientConnected() allows to communicate with a connecting client to link
 * states and exchanging messages.
 *
 * The default implementation connects by creating a UDP socket. If you have to
 * accept clients using a different transportation method overwrite
 * createSocket() to create a class instance implementing Endpoint interface
 * providing the required capabilities. Usually this is not required.
 *
 * To start listening call ListenOn with the IP address to listen on in the
 * format "hostnameOrIP" or "hostnameOrIP:port". You can use a resolvable
 * hostname or an IPv4. If the port is not specified the default port 3413 is
 * used. You can use any port you you like.
 */
public class Server implements Endpoint.Listener {
	public static final String CLASS_NAME = Server.class.getCanonicalName();
	public static final String LOGGER_NAME = Server.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	private String address = null;
	private Endpoint endpoint = null;
	private boolean listening = false;
	private LinkedList<Connection> connections = new LinkedList<>();

	private final CloseableReentrantLock lock = new CloseableReentrantLock();

	/**
	 * Create server.
	 */
	public Server() {
	}

	/**
	 * Dispose of server.
	 */
	public void dispose() {
		stopListening();
	}

	/**
	 * Address.
	 */
	public String getAddress() {
		return address;
	}

	/**
	 * Server is listening for connections.
	 */
	public boolean isListening() {
		return listening;
	}

	/**
	 * Start listening on address for incoming connections.
	 *
	 * @param[in] address Address is in the format "hostnameOrIP" or
	 *            "hostnameOrIP:port". You can use a resolvable hostname or an IPv4.
	 *            If the port is not specified the default port 3413 is used. You
	 *            can use any port you you like.
	 */
	public void listenOn(String address) throws IOException {
		if (listening) {
			throw new IllegalArgumentException("Already listening");
		}

		String useAddress = address;

		if (useAddress.equals("*")) {
			List<String> publicAddresses = findPublicAddresses();

			if (!publicAddresses.isEmpty()) {
				StringBuilder sb = new StringBuilder();
				sb.append("Found public address: ");
				boolean notFirst = false;
				for (String each : publicAddresses) {
					if (notFirst) {
						sb.append(", ");
					}
					notFirst = true;
					sb.append(each);
				}
				logger.info(sb.toString());

				useAddress = publicAddresses.get(0);

			} else {
				logger.info("No public address found. Using localhost");
				useAddress = "127.0.0.1";
			}
		}

		endpoint = createEndpoint();
		endpoint.open(resolveAddress(useAddress), this);

		logger.info("Server: Listening on " + endpoint.getAddress().toString());
		listening = true;
	}

	/**
	 * Stop listening.
	 */
	public void stopListening() {
		if (!listening) {
			return;
		}

		for (Connection each : new LinkedList<>(connections)) {
			each.dispose();
		}
		connections.clear();

		if (endpoint != null) {
			endpoint.dispose();
			endpoint = null;
		}

		listening = false;
	}

	/**
	 * Connections.
	 */
	public LinkedList<Connection> getConnections() {
		return connections;
	}

	/**
	 * Datagram received. Called asynchronous.
	 */
	public void receivedDatagram(SocketAddress address, Message message) {
		Connection connection = null;

		try (CloseableReentrantLock locked = lock.open()) {
			MessageReader reader = new MessageReader(message);

			for (Connection each : connections) {
				if (each.matches(endpoint, address)) {
					connection = each;
					break;
				}
			}

			if (connection != null) {
				connection.processDatagram(reader);

			} else {
				CommandCodes command = CommandCodes.withValue(reader.readByte());
				if (command == CommandCodes.CONNECTION_REQUEST) {
					processConnectionRequest(address, reader);

				} else {
					// ignore invalid package
					// logger.warning("Invalid datagram: Sender does not match any connection!");
				}
			}

		} catch (Exception e) {
			logger.log(Level.SEVERE, "receivedDatagram", e);
		}
	}

	/**
	 * Create connection for each connecting client.
	 *
	 * Overwrite this method to create an instance of a custom subclass of
	 * Connection handling the client.
	 *
	 * Default implementation creates instance of Connection.
	 */
	public Connection createConnection() {
		return new Connection();
	}

	/**
	 * Create endpoint.
	 *
	 * Default implementation creates an instance of DatagramChannelEndpoint which
	 * is a UDP socket. If you have to accept clients using a different
	 * transportation method overwrite method to create an instance of a class
	 * implementing Endpoint interface providing the required capabilities.
	 */
	public Endpoint createEndpoint() {
		return new DatagramChannelEndpoint();
	}

	/**
	 * Find public addresses.
	 */
	public List<String> findPublicAddresses() throws IOException {
		return DatagramChannelEndpoint.getPublicAddresses();
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
	public SocketAddress resolveAddress(String address) {
		return DatagramChannelEndpoint.resolveAddress(address);
	}

	/**
	 * Client connected.
	 *
	 * Overwrite to communicate with a connecting client to link states and exchange
	 * messages.
	 */
	public void onClientConnected(Connection connection) throws IOException {
	}

	/**
	 * Internal use only.
	 */
	public void processConnectionRequest(SocketAddress address, MessageReader reader) throws IOException {
		if (!listening) {
			Message message = new Message();
			try (MessageWriter writer = new MessageWriter(message)) {
				writer.writeByte((byte) CommandCodes.CONNECTION_ACK.value);
				writer.writeByte((byte) ConnectionAck.REJECTED.value);
			}
			endpoint.sendDatagram(address, message);
			return;
		}

		// find best protocol to speak
		List<Integer> clientProtocols = new ArrayList<>();
		int i, clientProtocolCount = reader.readUShort();
		for (i = 0; i < clientProtocolCount; i++) {
			clientProtocols.add(reader.readUShort());
		}

		if (!clientProtocols.contains(Protocols.DENETWORK_PROTOCOL.value)) {
			Message message = new Message();
			try (MessageWriter writer = new MessageWriter(message)) {
				writer.writeByte((byte) CommandCodes.CONNECTION_ACK.value);
				writer.writeByte((byte) ConnectionAck.NO_COMMON_PROTOCOL.value);
			}
			endpoint.sendDatagram(address, message);
			return;
		}

		Protocols protocol = Protocols.DENETWORK_PROTOCOL;

		// create connection
		Connection connection = createConnection();
		connection.acceptConnection(this, endpoint, address, protocol);
		connections.add(connection);

		// send back result
		Message message = new Message();
		try (MessageWriter writer = new MessageWriter(message)) {
			writer.writeByte((byte) CommandCodes.CONNECTION_ACK.value);
			writer.writeByte((byte) ConnectionAck.ACCEPTED.value);
			writer.writeUShort(protocol.value);
		}
		endpoint.sendDatagram(address, message);

		logger.info("Server: Client connected from " + address.toString());
		onClientConnected(connection);
	}
}
