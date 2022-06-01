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
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.endpoint.Endpoint;
import ch.dragondreams.denetwork.endpoint.Endpoint.Datagram;
import ch.dragondreams.denetwork.endpoint.SocketEndpoint;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.CommandCodes;
import ch.dragondreams.denetwork.protocol.ConnectionAck;
import ch.dragondreams.denetwork.protocol.Protocols;

/**
 * Network server.
 */
public class Server {
	public static final String CLASS_NAME = Server.class.getCanonicalName();
	public static final String LOGGER_NAME = Server.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	private String address = null;
	private Endpoint endpoint = null;
	private boolean listening = false;
	private LinkedList<Connection> connections = new LinkedList<>();

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
	 */
	public void listenOn(String address) throws IOException {
		if (listening) {
			throw new IllegalArgumentException("Already listening");
		}

		String useAddress = address;

		if (useAddress.equals("*")) {
			List<String> publicAddresses = findPublicAddresses();

			if (!useAddress.isEmpty()) {
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
		endpoint.setAddress(resolveAddress(useAddress));
		endpoint.bind();

		logger.info("Server: Listening on " + endpoint.getAddress().toString());
		listening = true;
	}

	/**
	 * Stop listening.
	 */
	public void stopListening() {
		if (!listening) {
			throw new IllegalArgumentException("Not listening");
		}

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
	 * Update server.
	 *
	 * Send and received queued messages. Call this on each frame update or in a
	 * loop from inside a thread. If using a thread use a mutex to ensure thread
	 * safety.
	 *
	 * \param[in] elapsedTime Elapsed time in seconds since the last call to
	 * Update();
	 */
	public void update(float elapsedTime) throws IOException {
		if (endpoint == null) {
			return;
		}

		// receive messages
		while (true) {
			Connection connection = null;

			try {
				SocketAddress addressReceive;
				Datagram message = endpoint.receiveDatagram();
				if (message == null) {
					break;
				}

				MessageReader reader = new MessageReader(message.message);
				addressReceive = message.address;

				for (Connection each : connections) {
					if (each.matches(endpoint, addressReceive)) {
						connection = each;
						break;
					}
				}

				if (connection != null) {
					connection.processDatagram(reader);

				} else {
					CommandCodes command = CommandCodes.withValue(reader.readByte());
					if (command == CommandCodes.CONNECTION_REQUEST) {
						processConnectionRequest(addressReceive, reader);

					} else {
						// ignore invalid package
						// logger.warning("Invalid datagram: Sender does not match any connection!");
					}
				}

			} catch (Exception e) {
				logger.log(Level.SEVERE, "Update[1]", e);
				throw e;
			}
		}

		// update connections
		for (Connection each : connections) {
			try {
				each.update(elapsedTime);

			} catch (Exception e) {
				logger.log(Level.SEVERE, "Update[2]", e);
				throw e;
			}
		}
	}

	/**
	 * Create connection.
	 *
	 * Default implementation creates instance of Connection.
	 */
	public Connection createConnection() {
		return new Connection();
	}

	/**
	 * Create endpoint.
	 *
	 * Default implementation creates instance of SocketEndpoint.
	 */
	public Endpoint createEndpoint() throws IOException {
		return new SocketEndpoint();
	}

	/**
	 * Find public addresses.
	 */
	public List<String> findPublicAddresses() throws IOException {
		return SocketEndpoint.getPublicAddresses();
	}

	/**
	 * Resolve address.
	 */
	public SocketAddress resolveAddress(String address) {
		int delimiter = address.indexOf(':');
		int port = 3413;

		// get address and port if present
		if (delimiter != -1) {
			port = Integer.parseInt(address.substring(delimiter + 1));
			address = address.substring(0, delimiter);
		}

		return new InetSocketAddress(address, port);
	}

	/**
	 * Client connected event. For overwriting by subclass.
	 */
	public void onClientConnected(Connection connection) {
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
			} catch (Exception e) {
				throw new IOException(e);
			}
			endpoint.sendDatagram(new Datagram(message, address));
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
			} catch (Exception e) {
				throw new IOException(e);
			}
			endpoint.sendDatagram(new Datagram(message, address));
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
		} catch (Exception e) {
			throw new IOException(e);
		}
		endpoint.sendDatagram(new Datagram(message, address));

		logger.info("Server: Client connected from " + address.toString());
		onClientConnected(connection);
	}
}
