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
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.PriorityQueue;
import java.util.Queue;
import java.util.logging.Level;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.endpoint.DatagramChannelEndpoint;
import ch.dragondreams.denetwork.endpoint.Endpoint;
import ch.dragondreams.denetwork.endpoint.Endpoint.Datagram;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.CommandCodes;
import ch.dragondreams.denetwork.protocol.ConnectionAck;
import ch.dragondreams.denetwork.protocol.Protocols;
import ch.dragondreams.denetwork.protocol.ReliableAck;
import ch.dragondreams.denetwork.state.State;
import ch.dragondreams.denetwork.state.StateLink;

/**
 * Connection.
 */
public class Connection {
	public static final String CLASS_NAME = Connection.class.getCanonicalName();
	public static final String LOGGER_NAME = Connection.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	/**
	 * State of the connection.
	 */
	enum ConnectionState {
		DISCONNECTED, CONNECTING, CONNECTED
	}

	private String localAddress = null;
	private String remoteAddress = null;

	private Endpoint endpoint = null;
	private SocketAddress realRemoteAddress = null;
	private ConnectionState connectionState = ConnectionState.DISCONNECTED;

	private Protocols protocol = Protocols.DENETWORK_PROTOCOL;
	private LinkedList<StateLink> stateLinks = new LinkedList<>();
	private LinkedList<StateLink> modifiedStateLinks = new LinkedList<>();
	private int nextLinkIdentifier = 0;

	private Queue<RealMessage> reliableMessagesSend = new PriorityQueue<>();
	private Queue<RealMessage> reliableMessagesRecv = new PriorityQueue<>();
	private int reliableNumberSend = 0;
	private int reliableNumberRecv = 0;
	private int reliableWindowSize = 10;

	private Server parentServer = null;

	public Connection() {
	}

	/**
	 * Dispose of connection.
	 */
	public void dispose() {
		reliableMessagesSend.clear();
		reliableMessagesRecv.clear();
		modifiedStateLinks.clear();
		stateLinks.clear();

		if (endpoint != null) {
			endpoint.dispose();
			endpoint = null;
		}

		parentServer = null;
	}

	/**
	 * Local address.
	 */
	public String getLocalAddress() {
		return localAddress;
	}

	/**
	 * Remote address.
	 */
	public String getRemoteAddress() {
		return remoteAddress;
	}

	/**
	 * Connection to a remote host is established.
	 */
	public boolean getConnected() {
		return connectionState == ConnectionState.CONNECTED;
	}

	/**
	 * Connect to connection object on host at address.
	 */
	public void connectTo(String address) throws IOException {
		logger.entering(CLASS_NAME, "connectTo", address);
		if (endpoint != null || connectionState != ConnectionState.DISCONNECTED) {
			throw new IllegalArgumentException("already connected");
		}

		endpoint = createSocket();
		endpoint.setAddress(new InetSocketAddress(0));
		endpoint.bind();

		localAddress = endpoint.getAddress().toString();

		Message connectRequest = new Message();
		try (MessageWriter writer = new MessageWriter(connectRequest)) {
			writer.writeByte((byte) CommandCodes.CONNECTION_REQUEST.value);
			writer.writeUShort(1); // version
			writer.writeUShort(Protocols.DENETWORK_PROTOCOL.value);
		}
		realRemoteAddress = resolveAddress(address);
		remoteAddress = address;

		logger.info(String.format("Connection: Connecting to %s", realRemoteAddress.toString()));

		endpoint.sendDatagram(new Datagram(connectRequest, realRemoteAddress));

		connectionState = ConnectionState.CONNECTING;
		logger.exiting(CLASS_NAME, "connectTo");
	}

	/**
	 * Resolve internet address in the format "hostname", "hostname:port", "ip" or
	 * "ip:port". If port is not set 3413 is used.
	 */
	public SocketAddress resolveAddress(String address) {
		int delimiter = address.indexOf(':');
		int port = 3413;
		if (delimiter != -1) {
			port = Integer.parseInt(address.substring(delimiter + 1));
			address = address.substring(0, delimiter);
		}
		return new InetSocketAddress(address, port);
	}

	/**
	 * Disconnect from remote connection if connected.
	 */
	public void disconnect() throws IOException {
		disconnect(true);
	}

	/**
	 * Send message to remote connection if connected.
	 *
	 * The message can be queued and send at a later time to optimize throughput.
	 * The message will be not delayed longer than the given amount of milliseconds.
	 * The message is send unreliable and it is acceptable for the message to get
	 * lost due to transmission failure.
	 */
	public void sendMessage(Message message, int maxDelay) throws IOException {
		if (message == null) {
			throw new IllegalArgumentException("message is null");
		}
		if (message.getLength() < 1) {
			throw new IllegalArgumentException("message has 0 length");
		}
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("not connected");
		}

		// send message
		Message unrealMessage = new Message();
		try (MessageWriter writer = new MessageWriter(unrealMessage)) {
			writer.writeByte((byte) CommandCodes.MESSAGE.value);
			writer.write(message);
		}

		endpoint.sendDatagram(new Datagram(unrealMessage, realRemoteAddress));
	}

	/**
	 * Send reliable message to remote connection if connected.
	 *
	 * The message is append to already waiting reliable messages and send as soon
	 * as possible. Reliable messages always arrive in the same order they have been
	 * queued.
	 */
	public void sendReliableMessage(Message message) throws IOException {
		if (message == null) {
			throw new IllegalArgumentException("message is null");
		}
		if (message.getLength() < 1) {
			throw new IllegalArgumentException("message has 0 length");
		}
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("not connected");
		}

		RealMessage realMessage = new RealMessage();
		realMessage.type = CommandCodes.RELIABLE_MESSAGE;
		realMessage.number = (reliableNumberSend + reliableMessagesSend.size()) % 65535;
		realMessage.state = RealMessage.State.PENDING;

		try (MessageWriter writer = new MessageWriter(realMessage.message)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_MESSAGE.value);
			writer.writeUShort(realMessage.number);
			writer.write(message);
		}

		reliableMessagesSend.add(realMessage);

		// if the message fits into the window send it right now
		if (reliableMessagesSend.size() <= reliableWindowSize) {
			endpoint.sendDatagram(new Datagram(realMessage.message, realRemoteAddress));

			realMessage.state = RealMessage.State.SEND;
			realMessage.secondsSinceSend = 0.0f;
		}
	}

	/**
	 * Link network state to remote network state.
	 *
	 * The message contains information for the remote system to know what state to
	 * link to. The request is queued and carried out as soon as possible. The local
	 * state is considered the master state and the remote state the slave state. By
	 * default only the master state can apply changes. s
	 */
	public void linkState(Message message, State state, boolean readOnly) throws IOException {
		if (message == null) {
			throw new IllegalArgumentException("message is null");
		}
		if (message.getLength() < 1) {
			throw new IllegalArgumentException("message has 0 length");
		}
		if (message.getLength() > 0xffff) {
			throw new IllegalArgumentException("message too long");
		}
		if (state == null) {
			throw new IllegalArgumentException("state is null");
		}
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("not connected");
		}

		// check if a link exists with this state already that is not broken
		StateLink stateLink = null;
		for (StateLink each : stateLinks) {
			if (each.getState() == state) {
				stateLink = each;
				break;
			}
		}

		if (stateLink != null && stateLink.getLinkState() != StateLink.LinkState.DOWN) {
			throw new IllegalArgumentException("link with state present");
		}

		// create the link if not existing, assign it a new identifier and add it
		if (stateLink == null) {
			int lastNextLinkIdentifier = nextLinkIdentifier;

			boolean found = false;
			for (StateLink each : stateLinks) {
				if (each.getIdentifier() == nextLinkIdentifier) {
					found = true;
					break;
				}
			}
			if (found) {
				nextLinkIdentifier = (nextLinkIdentifier + 1) % 65535;
				if (nextLinkIdentifier == lastNextLinkIdentifier) {
					throw new IllegalArgumentException("too many state links");
				}
			}

			stateLink = new StateLink(this, state);
			stateLink.setIdentifier(nextLinkIdentifier);
			stateLinks.add(stateLink);

			state.getLinks().add(stateLink);
		}

		// add message
		RealMessage realMessage = new RealMessage();
		realMessage.type = CommandCodes.RELIABLE_LINK_STATE;
		realMessage.number = (reliableNumberSend + reliableMessagesSend.size()) % 65535;
		realMessage.state = RealMessage.State.PENDING;

		try (MessageWriter writer = new MessageWriter(realMessage.message)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_LINK_STATE.value);
			writer.writeUShort(realMessage.number);
			writer.writeUShort(stateLink.getIdentifier());
			writer.writeByte((byte) (readOnly ? 1 : 0)); // flags: readOnly=0x1
			writer.writeUShort(message.getLength());
			writer.write(message);
			state.linkWriteValuesWithVerify(writer);
		}

		reliableMessagesSend.add(realMessage);

		// if the message fits into the window send it right now
		if (reliableMessagesSend.size() <= reliableWindowSize) {
			endpoint.sendDatagram(new Datagram(realMessage.message, realRemoteAddress));

			realMessage.state = RealMessage.State.SEND;
			realMessage.secondsSinceSend = 0.0f;
		}

		stateLink.setLinkState(StateLink.LinkState.LISTENING);
	}

	/**
	 * Update connection.
	 *
	 * Send and received queued messages. Call this on each frame update or in a
	 * loop from inside a thread. If using a thread use a mutex to ensure thread
	 * safety.
	 *
	 * @param elapsedTime
	 *            Elapsed time in seconds since the last call to Update();
	 */
	public void update(float elapsedTime) {
		if (connectionState == ConnectionState.DISCONNECTED) {
			return;
		}

		if (parentServer == null) {
			while (true) {
				if (connectionState == ConnectionState.DISCONNECTED) {
					return;
				}

				try {
					Datagram datagram = endpoint.receiveDatagram();
					if (datagram.message == null) {
						break;
					}

					processDatagram(new MessageReader(datagram.message));

				} catch (Exception e) {
					logger.log(Level.SEVERE, "Connection.update()[1]", e);
				}
			}
		}

		try {
			updateTimeouts(elapsedTime);
			updateStates();

		} catch (Exception e) {
			logger.log(Level.SEVERE, "Connection.update()[2]", e);
		}
	}

	/**
	 * Create endpoint. Default implementation creates instance of
	 * DatagramChannelEndpoint.
	 */
	public Endpoint createSocket() throws IOException {
		return new DatagramChannelEndpoint();
	}

	/**
	 * Connection established. Callback for subclass.
	 */
	public void connectionEstablished() {
	}

	/**
	 * Connection closed. Callback for subclass.
	 */
	public void connectionClosed() {
	}

	/**
	 * Long message is in progress of receiving. Callback for subclass.
	 */
	public void messageProgress(int bytesReceived) {
	}

	/**
	 * Message received. Callback for subclass.
	 *
	 * @param message
	 *            Received message. Object can be stored for later use.
	 */
	public void messageReceived(Message message) {
	}

	/**
	 * Host send state to link. Callback for subclass.
	 *
	 * @returns State or null to reject.
	 */
	public State createState(Message message, boolean readOnly) {
		return null;
	}

	/**
	 * Server owning this connection or null if this is a client side connection.
	 */
	public Server getParentServer() {
		return parentServer;
	}

	/**
	 * Endpoint or null if not connected.
	 */
	public Endpoint getEndpoint() {
		return endpoint;
	}

	/**
	 * Connection protocol.
	 */
	public Protocols getProtocol() {
		return protocol;
	}

	/**
	 * Connection matches endpoint and address.
	 */
	public boolean matches(Endpoint endpoint, SocketAddress address) {
		return this.endpoint == endpoint && address.equals(realRemoteAddress);
	}

	/**
	 * Process datagram.
	 */
	public void processDatagram(MessageReader reader) throws IOException {
		switch (CommandCodes.withValue(reader.readByte())) {
		case CONNECTION_ACK:
			processConnectionAck(reader);
			break;

		case CONNECTION_CLOSE:
			processConnectionClose(reader);
			break;

		case MESSAGE:
			processMessage(reader);
			break;

		case RELIABLE_MESSAGE:
			processReliableMessage(reader);
			break;

		case RELIABLE_LINK_STATE:
			processReliableLinkState(reader);
			break;

		case RELIABLE_ACK:
			processReliableAck(reader);
			break;

		case LINK_UP:
			processLinkUp(reader);
			break;

		case LINK_DOWN:
			processLinkDown(reader);
			break;

		case LINK_UPDATE:
			processLinkUpdate(reader);
			break;

		default:
			throw new IllegalArgumentException("Invalid command code");
		}
	}

	/**
	 * For internal use only.
	 */
	public void acceptConnection(Server server, Endpoint endpoint, SocketAddress address, Protocols protocol) {
		this.endpoint = endpoint;
		realRemoteAddress = address;
		remoteAddress = address.toString();
		connectionState = ConnectionState.CONNECTED;
		this.protocol = protocol;
		parentServer = server;

		connectionEstablished();
	}

	private void disconnect(boolean notify) throws IOException {
		if (endpoint == null || connectionState == ConnectionState.DISCONNECTED) {
			return;
		}

		if (connectionState == ConnectionState.CONNECTED) {
			logger.info("Disconnecting");

			Message connectionClose = new Message();
			try (MessageWriter writer = new MessageWriter(connectionClose)) {
				writer.writeByte((byte) CommandCodes.CONNECTION_CLOSE.value);
			}
			endpoint.sendDatagram(new Datagram(connectionClose, realRemoteAddress));
		}

		clearStates();

		reliableMessagesRecv.clear();
		reliableMessagesSend.clear();

		closeEndpoint();

		logger.info("Connection closed");

		if (notify) {
			connectionClosed();
		}

		removeConnectionFromParentServer();
	}

	private void clearStates() {
		modifiedStateLinks.clear();

		for (StateLink each : modifiedStateLinks) {
			State state = each.getState();
			if (state != null) {
				int index = state.getLinks().indexOf(each);
				if (index != -1) {
					each.dropState();
					state.getLinks().remove(index);
				}
			}
		}
		stateLinks.clear();
	}

	private void closeEndpoint() {
		connectionState = ConnectionState.DISCONNECTED;
		if (endpoint != null) {
			endpoint.dispose();
			endpoint = null;
		}
	}

	private void removeConnectionFromParentServer() {
		if (parentServer == null) {
			return;
		}

		parentServer.getConnections().remove(this);
		parentServer = null;
	}

	private void updateStates() throws IOException {
		int linkCount = modifiedStateLinks.size();
		if (linkCount == 0) {
			return;
		}

		int changedCount = 0;
		for (StateLink each : modifiedStateLinks) {
			if (each.getLinkState() == StateLink.LinkState.UP && each.getChanged()) {
				changedCount++;
			}
		}
		if (changedCount == 0) {
			return;
		}
		changedCount = Math.min(changedCount, 255);

		Message updateMessage = new Message();
		try (MessageWriter writer = new MessageWriter(updateMessage)) {
			writer.writeByte((byte) CommandCodes.LINK_UPDATE.value);
			writer.writeByte((byte) changedCount);

			ListIterator<StateLink> iter = modifiedStateLinks.listIterator();
			while (iter.hasNext()) {
				StateLink link = iter.next();
				if (link.getLinkState() != StateLink.LinkState.UP || !link.getChanged()) {
					continue;
				}

				writer.writeUShort(link.getIdentifier());
				State state = link.getState();
				if (state == null) {
					throw new IllegalArgumentException("state link droppped");
				}

				state.linkWriteValues(writer, link);

				iter.remove();

				changedCount--;
				if (changedCount == 0) {
					break;
				}
			}
		}

		endpoint.sendDatagram(new Datagram(updateMessage, realRemoteAddress));
	}

	private void updateTimeouts(float elapsedTime) throws IOException {
		// increase the timeouts on all send packages
		float timeout = 3.0f;

		for (RealMessage each : reliableMessagesSend) {
			if (each.state != RealMessage.State.SEND) {
				continue;
			}

			each.secondsSinceSend += elapsedTime;

			if (each.secondsSinceSend > timeout) {
				// resend message
				endpoint.sendDatagram(new Datagram(each.message, realRemoteAddress));
				each.secondsSinceSend = 0.0f;
			}
		}
	}

	private void invalidateState(State state) {
		ListIterator<StateLink> iter = stateLinks.listIterator();
		while (iter.hasNext()) {
			StateLink link = iter.next();
			if (link.getState() == state) {
				int index = state.getLinks().indexOf(link);
				if (index != -1) {
					state.getLinks().get(index).dropState();
					state.getLinks().remove(index);
				}
				iter.remove();
			}
		}
	}

	/**
	 * Internal use only.
	 */
	public void addModifiedStateLink(StateLink stateLink) {
		modifiedStateLinks.add(stateLink);
	}

	private void processQueuedMessages() throws IOException {
		while (true) {
			RealMessage message = null;
			for (RealMessage each : reliableMessagesRecv) {
				if (each.number == reliableNumberRecv) {
					message = each;
					break;
				}
			}
			if (message == null) {
				break;
			}

			switch (message.type) {
			case RELIABLE_MESSAGE:
				processReliableMessageMessage(new MessageReader(message.message));
				break;

			case RELIABLE_LINK_STATE:
				processLinkState(new MessageReader(message.message));
				break;

			default:
				break;
			}

			reliableMessagesRecv.remove(message);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
		}
	}

	private void processConnectionAck(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTING) {
			throw new IllegalArgumentException("Not connecting");
		}

		switch (ConnectionAck.withValue(reader.readByte())) {
		case ACCEPTED:
			protocol = Protocols.withValue(reader.readUShort());
			connectionState = ConnectionState.CONNECTED;
			logger.info("Connection established");
			connectionEstablished();
			break;

		case REJECTED:
		case NO_COMMON_PROTOCOL:
		default:
			connectionState = ConnectionState.DISCONNECTED;
			logger.info("Connection rejected");
			connectionClosed();
		}
	}

	private void processConnectionClose(MessageReader reader) throws IOException {
		disconnect(true);
	}

	private void processMessage(MessageReader reader) {
		/* const int flags = */ reader.readByte();

		Message message = new Message();
		message.setLength(reader.length() - reader.position());
		reader.read(message);

		messageReceived(message);
	}

	private void processReliableMessage(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("Reliable message received although not connected.");
		}

		int number = reader.readUShort();
		boolean validNumber;

		if (number < reliableNumberRecv) {
			validNumber = number < (reliableNumberRecv + reliableWindowSize) % 65535;

		} else {
			validNumber = number < reliableNumberRecv + reliableWindowSize;
		}
		if (!validNumber) {
			throw new IllegalArgumentException("Reliable message: invalid sequence number.");
		}

		Message ackMessage = new Message();
		try (MessageWriter writer = new MessageWriter(ackMessage)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_ACK.value);
			writer.writeUShort(number);
			writer.writeByte((byte) ReliableAck.SUCCESS.value);
		}
		endpoint.sendDatagram(new Datagram(ackMessage, realRemoteAddress));

		if (number == reliableNumberRecv) {
			processReliableMessageMessage(reader);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
			processQueuedMessages();

		} else {
			addReliableReceive(CommandCodes.RELIABLE_MESSAGE, number, reader);
		}
	}

	private void processReliableMessageMessage(MessageReader reader) {
		Message message = new Message();
		message.setLength(reader.length() - reader.position());
		reader.read(message);

		messageReceived(message);
	}

	private void processReliableAck(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("Reliable ack: not connected.");
		}

		int number = reader.readUShort();
		ReliableAck code = ReliableAck.withValue(reader.readByte());

		RealMessage message = null;
		for (RealMessage each : reliableMessagesSend) {
			if (each.number == number) {
				message = each;
				break;
			}
		}
		if (message == null) {
			throw new IllegalArgumentException(
					"Reliable ack: no reliable transmission with this number waiting for an ack!");
		}

		switch (code) {
		case SUCCESS:
			message.state = RealMessage.State.DONE;
			removeSendReliablesDone();
			break;

		case FAILED:
			message.secondsSinceSend = 0.0f;
			endpoint.sendDatagram(new Datagram(message.message, realRemoteAddress));
			break;
		}
	}

	private void processReliableLinkState(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("Link state: not connected.");
		}

		int number = reader.readUShort();
		boolean validNumber;

		if (number < reliableNumberRecv) {
			validNumber = number < (reliableNumberRecv + reliableWindowSize) % 65535;

		} else {
			validNumber = number < reliableNumberRecv + reliableWindowSize;
		}
		if (!validNumber) {
			throw new IllegalArgumentException("Link state: invalid sequence number.");
		}

		Message ackMessage = new Message();
		try (MessageWriter writer = new MessageWriter(ackMessage)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_ACK.value);
			writer.writeUShort(number);
			writer.writeByte((byte) ReliableAck.SUCCESS.value);
		}
		endpoint.sendDatagram(new Datagram(ackMessage, realRemoteAddress));

		if (number == reliableNumberRecv) {
			processLinkState(reader);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
			processQueuedMessages();

		} else {
			addReliableReceive(CommandCodes.RELIABLE_LINK_STATE, number, reader);
		}
	}

	private void processLinkUp(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("Reliable ack: not connected.");
		}

		int identifier = reader.readUShort();

		StateLink link = null;
		for (StateLink each : stateLinks) {
			if (each.getIdentifier() == identifier) {
				link = each;
				break;
			}
		}
		if (link == null || link.getLinkState() != StateLink.LinkState.LISTENING) {
			throw new IllegalArgumentException("Link with identifier absent or not listening");
		}

		link.setLinkState(StateLink.LinkState.UP);
	}

	private void processLinkDown(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("Reliable ack: not connected.");
		}

		int identifier = reader.readUShort();

		StateLink link = null;
		for (StateLink each : stateLinks) {
			if (each.getIdentifier() == identifier) {
				link = each;
				break;
			}
		}
		if (link == null || link.getLinkState() != StateLink.LinkState.UP) {
			throw new IllegalArgumentException("Link with identifier absent or not up");
		}

		link.setLinkState(StateLink.LinkState.DOWN);
	}

	private void processLinkState(MessageReader reader) throws IOException {
		int identifier = reader.readUShort();
		boolean readOnly = reader.readByte() == 1; // flags: 0x1=readOnly

		StateLink link = null;
		for (StateLink each : stateLinks) {
			if (each.getIdentifier() == identifier) {
				link = each;
				break;
			}
		}
		if (link != null && link.getLinkState() != StateLink.LinkState.DOWN) {
			throw new IllegalArgumentException("Link with identifier exists already");
		}

		// create linked network state
		Message message = new Message();
		message.setLength(reader.readUShort());
		reader.read(message);

		State state = createState(message, readOnly);

		CommandCodes code = CommandCodes.LINK_DOWN;
		if (state != null) {
			if (!state.linkReadAndVerifyAllValues(reader)) {
				throw new IllegalArgumentException("Link state does not match the state provided.");
			}

			if (link != null) {
				throw new IllegalArgumentException("Link state existing already.");
			}

			link = new StateLink(this, state);
			link.setIdentifier(identifier);
			stateLinks.add(link);
			state.getLinks().add(link);

			link.setLinkState(StateLink.LinkState.UP);
			code = CommandCodes.LINK_UP;
		}

		message = new Message();
		try (MessageWriter writer = new MessageWriter(message)) {
			writer.writeByte((byte) code.value);
			writer.writeUShort(identifier);
		}
		endpoint.sendDatagram(new Datagram(message, realRemoteAddress));
	}

	private void processLinkUpdate(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTED) {
			throw new IllegalArgumentException("Reliable ack: not connected.");
		}

		int i, count = reader.readByte();
		for (i = 0; i < count; i++) {
			int identifier = reader.readUShort();

			StateLink link = null;
			for (StateLink each : stateLinks) {
				if (each.getIdentifier() == identifier) {
					link = each;
					break;
				}
			}
			if (link == null || link.getLinkState() != StateLink.LinkState.UP) {
				throw new IllegalArgumentException("Invalid link identifier");
			}

			State state = link.getState();
			if (state == null) {
				throw new IllegalArgumentException("State link droppped");
			}

			state.linkReadValues(reader, link);
		}
	}

	private void addReliableReceive(CommandCodes type, int number, MessageReader reader) {
		RealMessage message = new RealMessage();
		message.message.setLength(reader.length() - reader.position());
		reader.read(message.message);

		message.type = type;
		message.number = number;
		message.state = RealMessage.State.DONE;

		reliableMessagesRecv.add(message);
	}

	private void removeSendReliablesDone() throws IOException {
		boolean anyRemoved = false;

		while (!reliableMessagesSend.isEmpty()) {
			if (reliableMessagesSend.peek().state != RealMessage.State.DONE) {
				break;
			}

			reliableMessagesSend.remove();
			reliableNumberSend = (reliableNumberSend + 1) % 65535;
			anyRemoved = true;
		}

		if (anyRemoved) {
			sendPendingReliables();
		}
	}

	private void sendPendingReliables() throws IOException {
		int sendCount = 0;

		for (RealMessage each : reliableMessagesSend) {
			if (each.state != RealMessage.State.PENDING) {
				continue;
			}

			endpoint.sendDatagram(new Datagram(each.message, realRemoteAddress));

			each.state = RealMessage.State.SEND;
			each.secondsSinceSend = 0.0f;

			if (++sendCount == reliableWindowSize) {
				break;
			}
		}
	}
}