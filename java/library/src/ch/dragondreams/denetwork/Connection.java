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
import java.net.Inet6Address;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.ArrayDeque;
import java.util.Date;
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.Queue;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.endpoint.DatagramChannelEndpoint;
import ch.dragondreams.denetwork.endpoint.Endpoint;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.protocol.CommandCodes;
import ch.dragondreams.denetwork.protocol.ConnectionAck;
import ch.dragondreams.denetwork.protocol.LongLinkStateFlags;
import ch.dragondreams.denetwork.protocol.LongMessageFlags;
import ch.dragondreams.denetwork.protocol.Protocols;
import ch.dragondreams.denetwork.protocol.ReliableAck;
import ch.dragondreams.denetwork.state.State;
import ch.dragondreams.denetwork.state.StateLink;
import ch.dragondreams.denetwork.utils.CloseableReentrantLock;

/**
 * Network connection.
 *
 * Allows clients to connect to a server speaking Drag[en]gine Network Protocol.
 *
 * To use this class create a subclass overwriting one or more of the methods
 * below.
 *
 * To start connecting to the server call connectTo() with the IP address to
 * connect to in the format "hostnameOrIP" or "hostnameOrIP:port". You can use a
 * resolvable hostname or an IPv4. If the port is not specified the default port
 * 3413 is used. You can use any port you you like. Connecting attempt fails if
 * it takes longer than SetConnectTimeout seconds. The default timeout is 3
 * seconds.
 *
 * If connecting to the server succeedes connectionEstablished() is called.
 * Overwrite to request linking states and exchanging messages with the server.
 * If connection timed out or another error occured connectionFailed() is called
 * with the failure reason. Overwrite to handle connection failure.
 *
 * You can close the connection by calling disconnect(). This calls
 * connectionClosed() which you can overwrite. This method is also called if the
 * server closes the connection.
 *
 * Overwrite createState() to create states requested by the server. States
 * synchronize a fixed set of values between the server and the client. The
 * client can have read-write or read-only access to the state. Create an
 * instance of a subclass of State to handle individual states. It is not
 * necessary to create a subclass of State if you intent to subclass Value*
 * instead.
 *
 * Overwrite messageReceived() to process messages send by the server.
 */
public class Connection implements Endpoint.Listener {
	public static final String CLASS_NAME = Connection.class.getCanonicalName();
	public static final String LOGGER_NAME = Connection.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	/**
	 * State of the connection.
	 */
	public enum ConnectionState {
		DISCONNECTED, CONNECTING, CONNECTED
	}

	/**
	 * Connection failed reason.
	 */
	public enum ConnectionFailedReason {
		GENERIC, TIMEOUT, REJECTED, NO_COMMON_PROTOCOL, INVALID_MESSAGE
	};

	private String localAddress = null;
	private String remoteAddress = null;

	private Endpoint endpoint = null;
	private SocketAddress realRemoteAddress = null;
	private ConnectionState connectionState = ConnectionState.DISCONNECTED;

	private float connectResendInterval = 1.0f;
	private float connectTimeout = 5.0f;
	private float reliableResendInterval = 0.5f;
	private float reliableTimeout = 3.0f;

	private float elapsedConnectResend = 0.0f;
	private float elapsedConnectTimeout = 0.0f;

	private Protocols protocol = Protocols.DENETWORK_PROTOCOL;
	private LinkedList<StateLink> stateLinks = new LinkedList<>();
	private LinkedList<StateLink> modifiedStateLinks = new LinkedList<>();
	private int nextLinkIdentifier = 0;

	private Queue<RealMessage> reliableMessagesSend = new ArrayDeque<>();
	private Queue<RealMessage> reliableMessagesRecv = new ArrayDeque<>();
	private int reliableNumberSend = 0;
	private int reliableNumberRecv = 0;
	private int reliableWindowSize = 10;

	private Server parentServer = null;

	Message longMessage = null;
	int longMessagePartSize = 1357;
	Message longLinkStateMessage = null;
	Message longLinkStateValues = null;

	private final CloseableReentrantLock lock = new CloseableReentrantLock();

	final private ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
	private ScheduledFuture<?> futureUpdateTask = null;

	protected class UpdateStatesTask implements Runnable {
		private long lastTime = 0L;

		@Override
		public void run() {
			long curTime = System.nanoTime();
			float elapsed = 0.0f;
			if (lastTime != 0) {
				elapsed = (float) (1e-9 * (curTime - lastTime));
			}
			lastTime = curTime;

			try {
				updateTimeouts(elapsed);
				updateStates();

			} catch (Exception e) {
				logger.log(Level.SEVERE, "UpdateStatesTask", e);
			}
		}
	}

	public Connection() {
	}

	/**
	 * Dispose of connection.
	 */
	public void dispose() {
		stopUpdateTask();

		try {
			disconnect(false, false);
		} catch (IOException e) {
			e.printStackTrace();
		}

		try (CloseableReentrantLock locked = lock.open()) {
			reliableMessagesSend.clear();
			reliableMessagesRecv.clear();
			reliableNumberSend = 0;
			reliableNumberRecv = 0;
			modifiedStateLinks.clear();
			stateLinks.clear();
		}

		if (parentServer == null && endpoint != null) {
			endpoint.dispose();
		}
		endpoint = null;

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
	 * Connect resent interval in seconds.
	 */
	public float getConnectResendInterval() {
		return connectResendInterval;
	}

	/**
	 * Connect resent interval in seconds.
	 */
	public void setConnectResendInterval(float interval){
		connectResendInterval = Math.max(interval, 0.01f);
	}

	/**
	 * Connect timeout in seconds.
	 */
	public float getConnectTimeout(){
		return connectTimeout;
	}

	/**
	 * Connect resent interval in seconds.
	 */
	public void setConnectTimeout(float timeout){
		connectTimeout = Math.max(timeout, 0.01f);
	}

	/**
	 * Reliable message resend interval in seconds.
	 */
	public float getReliableResendInterval() {
		return reliableResendInterval;
	}

	/**
	 * Set reliable message resend interval in seconds.
	 */
	public void setReliableResendInterval(float interval){
		reliableResendInterval = Math.max(interval, 0.01f);
	}

	/**
	 * Reliable message timeout in seconds.
	 */
	public float getReliableTimeout() {
		return reliableTimeout;
	}

	/**
	 * Set reliable message timeout in seconds.
	 */
	public void setReliableTimeout(float timeout){
		reliableTimeout = Math.max(timeout, 0.01f);
	}

	/**
	 * Connection state.
	 */
	public ConnectionState getConnectionState() {
		return connectionState;
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

		try (CloseableReentrantLock locked = lock.open()) {
			InetSocketAddress resolved = (InetSocketAddress) resolveAddress(address);

			endpoint = createEndpoint();

			if (resolved.getAddress() instanceof Inet6Address) {
				endpoint.open(new InetSocketAddress("::", 0), this);

			}else {
				endpoint.open(new InetSocketAddress("0.0.0.0", 0), this);
			}

			localAddress = endpoint.getAddress().toString();

			Message connectRequest = new Message();
			try (MessageWriter writer = new MessageWriter(connectRequest)) {
				writer.writeByte((byte) CommandCodes.CONNECTION_REQUEST.value);
				writer.writeUShort(1); // version
				writer.writeUShort(Protocols.DENETWORK_PROTOCOL.value);
			}
			realRemoteAddress = resolved;
			remoteAddress = address;

			logger.info(String.format("Connection: Connecting to %s", realRemoteAddress.toString()));

			endpoint.sendDatagram(realRemoteAddress, connectRequest);

			connectionState = ConnectionState.CONNECTING;
			elapsedConnectResend = 0.0f;
			elapsedConnectTimeout = 0.0f;
			startUpdateTask();

		} catch (Exception e) {
			disconnect();
			throw e;
		}

		logger.exiting(CLASS_NAME, "connectTo");
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
	 * Disconnect from remote connection if connected.
	 */
	public void disconnect() throws IOException {
		disconnect(true, false);
	}

	/**
	 * Send message to remote connection if connected.
	 *
	 * The message can be queued and send at a later time to optimize throughput.
	 * The message will be not delayed longer than the given amount of milliseconds.
	 * The message is send unreliable and it is acceptable for the message to get
	 * lost due to transmission failure.
	 *
	 * Sending messages is not reliable. Messages can be potentially lost and you
	 * will not be notified if this occurs. Use this method for messages where
	 * loosing them is fine. This is typically the case for messages repeating in
	 * regular intervals so missing one of them is not a problem.
	 *
	 * @param[in] message Message to send. Message can contain any kind of byte
	 *            sequence. The most simply way to build messages is using
	 *            denMessageWriter.
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

		endpoint.sendDatagram(realRemoteAddress, unrealMessage);
	}

	/**
	 * Send reliable message to remote connection if connected.
	 *
	 * The message is append to already waiting reliable messages and send as soon
	 * as possible. Reliable messages always arrive in the same order they have been
	 * queued.
	 *
	 * This messages is guaranteed to be delivered in the order they have been send.
	 * Use this for messages which you can not afford to loose. This is typically
	 * the case for events happening once like a player activating an item or
	 * opening a door.
	 *
	 * @param[in] message Message to send. Message can contain any kind of byte
	 *            sequence. The most simply way to build messages is using
	 *            denMessageWriter.
	 */
	public void sendReliableMessage(Message message) throws IOException {
		try (CloseableReentrantLock locked = lock.open()) {
			if (message == null) {
				throw new IllegalArgumentException("message is null");
			}
			
			int length = message.getLength();
			if (length < 1) {
				throw new IllegalArgumentException("message has 0 length");
			}
			if (connectionState != ConnectionState.CONNECTED) {
				throw new IllegalArgumentException("not connected");
			}
			
			int partCount = (int)((length - 1) / longMessagePartSize + 1);
			if (partCount > 1) {
				byte[] data = message.getData();
				int offset = 0;
				int i;
				
				for (i=0; i<partCount; i++) {
					RealMessage realMessage = new RealMessage();
					realMessage.type = CommandCodes.RELIABLE_MESSAGE_LONG;
					realMessage.number = (reliableNumberSend + reliableMessagesSend.size()) % 65535;
					realMessage.state = RealMessage.State.PENDING;
					
					int flags = 0;
					if (i == 0) {
						flags |= LongMessageFlags.FIRST.value;
					}
					if (i == partCount - 1) {
						flags |= LongMessageFlags.LAST.value;
					}
					
					int partLength = Math.min(longMessagePartSize, length - offset);
					
					try (MessageWriter writer = new MessageWriter(realMessage.message)) {
						writer.writeByte((byte) CommandCodes.RELIABLE_MESSAGE_LONG.value);
						writer.writeUShort(realMessage.number);
						writer.writeByte((byte) flags);
						writer.write(data, offset, partLength);
					}
					
					reliableMessagesSend.add(realMessage);
					offset += partLength;
				}
				
				sendPendingReliables();
				
			}else{
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
					endpoint.sendDatagram(realRemoteAddress, realMessage.message);

					realMessage.state = RealMessage.State.SEND;
					realMessage.elapsedResend = 0.0f;
					realMessage.elapsedTimeout = 0.0f;
				}
			}
		}
	}

	/**
	 * Link network state to remote network state.
	 *
	 * The message contains information for the remote system to know what state to
	 * link to. The request is queued and carried out as soon as possible. The local
	 * state is considered the master state and the remote state the slave state. By
	 * default only the master state can apply changes.
	 *
	 * @param[in] message Message to send. Message can contain any kind of byte
	 *            sequence. The most simply way to build messages is using
	 *            denMessageWriter.
	 * @param[in] state State to link.
	 * @param[in] readOnly If true client receives a read-only link otherwise a
	 *            read-write link. Use true if the state to link is a server managed
	 *            state the client is only allowed to read. Use false if this is a
	 *            state the client has to change.
	 */
	public void linkState(Message message, State state, boolean readOnly) throws IOException {
		try (CloseableReentrantLock locked = lock.open()) {
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
				endpoint.sendDatagram(realRemoteAddress, realMessage.message);

				realMessage.state = RealMessage.State.SEND;
				realMessage.elapsedResend = 0.0f;
				realMessage.elapsedTimeout = 0.0f;
			}

			stateLink.setLinkState(StateLink.LinkState.LISTENING);
		}
	}

	/**
	 * Datagram received. Called asynchronous.
	 */
	public void receivedDatagram(SocketAddress address, Message message) {
		try (CloseableReentrantLock locked = lock.open()) {
			if (connectionState == ConnectionState.DISCONNECTED || parentServer != null) {
				return;
			}
			try {
				processDatagram(new MessageReader(message));

			} catch (Exception e) {
				logger.log(Level.SEVERE, "Connection.update()[1]", e);
			}
		}
	}

	/**
	 * Create endpoint.
	 *
	 * Default implementation creates an instance of DatagramChannelEndpoint which
	 * is a UDP socket. If you have to accept clients using a different
	 * transportation method overwrite method to create an instance of a class
	 * implementing Endpoint interface providing the required capabilities.
	 */
	public Endpoint createEndpoint() throws IOException {
		return new DatagramChannelEndpoint();
	}

	/**
	 * Connection established. Called asynchronously. Callback for subclass.
	 */
	public void connectionEstablished() throws IOException {
	}

	/**
	 * Connection failed or timeout out.
	 */
	public void connectionFailed(ConnectionFailedReason reason) {
	}

	/**
	 * Connection closed.
	 *
	 * This is called if Disconnect() is called or the server closes the connection.
	 */
	public void connectionClosed() throws IOException {
	}

	/**
	 * Long message is in progress of receiving. Called asynchronously. Callback for
	 * subclass.
	 */
	public void messageProgress(int bytesReceived) throws IOException {
	}

	/**
	 * Message received. Called asynchronously. Callback for subclass.
	 *
	 * @param message Received message. Object can be stored for later use.
	 */
	public void messageReceived(Message message) throws IOException {
	}

	/**
	 * Host send state to link.
	 *
	 * Overwrite to create states requested by the server. States synchronize a
	 * fixed set of values between the server and the client. The client can have
	 * read-write or read-only access to the state. Create an instance of a subclass
	 * of denState to handle individual states. It is not necessary to create a
	 * subclass of denState if you intent to subclass denValue* instead.
	 *
	 * If you do not support a state requested by the server you can return nullptr.
	 * In this case the state is not linked and state values are not synchronized.
	 * You can not re-link a state later on if you rejected it here. If you need
	 * re-linking a state make the server resend the link request. This will be a
	 * new state link.
	 *
	 * @returns State or nullptr to reject.
	 */
	public State createState(Message message, boolean readOnly) throws IOException {
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

		case RELIABLE_MESSAGE_LONG:
			processReliableMessageLong(reader);
			break;

		case RELIABLE_LINK_STATE_LONG:
			processReliableLinkStateLong(reader);
			break;

		default:
			// throw new IllegalArgumentException("Invalid command code");
			break;
		}
	}

	/**
	 * For internal use only.
	 */
	public void acceptConnection(Server server, Endpoint endpoint, SocketAddress address, Protocols protocol)
			throws IOException {
		this.endpoint = endpoint;
		realRemoteAddress = address;
		remoteAddress = address.toString();
		connectionState = ConnectionState.CONNECTED;
		elapsedConnectResend = 0.0f;
		elapsedConnectTimeout = 0.0f;
		this.protocol = protocol;
		parentServer = server;
		startUpdateTask();
		connectionEstablished();
	}

	private void disconnect(boolean notify, boolean remoteClosed) throws IOException {
		if (endpoint == null || connectionState == ConnectionState.DISCONNECTED) {
			return;
		}

		if (connectionState == ConnectionState.CONNECTED) {
			if (remoteClosed) {
				logger.info("Remove closed connection");
			} else {
				logger.info("Disconnecting");

				updateStates();
				sendPendingReliables();

				Message connectionClose = new Message();
				try (MessageWriter writer = new MessageWriter(connectionClose)) {
					writer.writeByte((byte) CommandCodes.CONNECTION_CLOSE.value);
				}
				endpoint.sendDatagram(realRemoteAddress, connectionClose);
			}
		}

		try (CloseableReentrantLock locked = lock.open()) {
			clearStates();
			reliableMessagesRecv.clear();
			reliableMessagesSend.clear();
			reliableNumberSend = 0;
			reliableNumberRecv = 0;
			longMessage = null;
			longLinkStateMessage = null;
			longLinkStateValues = null;
		}

		closeEndpoint();

		logger.info("Connection closed");

		if (notify) {
			connectionClosed();
		}

		removeConnectionFromParentServer();
	}

	/**
	 * Synchronized by caller.
	 */
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
		stopUpdateTask();
		connectionState = ConnectionState.DISCONNECTED;
		elapsedConnectResend = 0.0f;
		elapsedConnectTimeout = 0.0f;
		if (parentServer == null && endpoint != null) {
			endpoint.dispose();
		}
		endpoint = null;
	}

	private void removeConnectionFromParentServer() {
		if (parentServer == null) {
			return;
		}

		closeEndpoint();

		parentServer.getConnections().remove(this);
		parentServer = null;
	}

	/**
	 * Synchronized by caller.
	 */
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
					// throw new IllegalArgumentException("state link droppped");
					iter.remove();
					continue;
				}

				state.linkWriteValues(writer, link);

				iter.remove();

				changedCount--;
				if (changedCount == 0) {
					break;
				}
			}
		}

		endpoint.sendDatagram(realRemoteAddress, updateMessage);
	}

	/**
	 * Synchronized by caller.
	 */
	private void updateTimeouts(float elapsedTime) throws IOException {
		switch (connectionState) {
		case CONNECTED: {
			for (RealMessage each : reliableMessagesSend) {
				if (each.state != RealMessage.State.SEND) {
					continue;
				}

				each.elapsedTimeout += elapsedTime;
				if (each.elapsedTimeout > reliableTimeout) {
					logger.severe("Reliable message timeout");
					disconnect();
					return;
				}

				each.elapsedResend += elapsedTime;
				if (each.elapsedResend > reliableResendInterval) {
					each.elapsedResend = 0.0f;
					endpoint.sendDatagram(realRemoteAddress, each.message);
				}
			}
		}
			break;

		case CONNECTING:
			elapsedConnectTimeout += elapsedTime;
			if (elapsedConnectTimeout > connectTimeout) {
				closeEndpoint();
				logger.info("Connection failed (timeout)");
				connectionFailed(ConnectionFailedReason.TIMEOUT);
			}

			elapsedConnectResend += elapsedTime;
			if (elapsedConnectResend > connectResendInterval) {
				logger.finest("Resent connection request");
				elapsedConnectResend = 0.0f;
				Message connectRequest = new Message();
				try (MessageWriter writer = new MessageWriter(connectRequest)) {
					writer.writeByte((byte) CommandCodes.CONNECTION_REQUEST.value);
					writer.writeUShort(1); // version
					writer.writeUShort(Protocols.DENETWORK_PROTOCOL.value);
				}
				endpoint.sendDatagram(realRemoteAddress, connectRequest);
			}
			break;

		default:
			break;
		}
	}

	/*
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
	*/

	/**
	 * Internal use only.
	 */
	public void addModifiedStateLink(StateLink stateLink) {
		try (CloseableReentrantLock locked = lock.open()) {
			modifiedStateLinks.add(stateLink);
		}
	}

	/**
	 * Synchronized by caller.
	 */
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

			case RELIABLE_MESSAGE_LONG:
				processReliableMessageMessageLong(new MessageReader(message.message));
				break;

			case RELIABLE_LINK_STATE_LONG:
				processLinkStateLong(new MessageReader(message.message));
				break;

			default:
				break;
			}

			reliableMessagesRecv.remove(message);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processConnectionAck(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTING) {
			// throw new IllegalArgumentException("Not connecting");
			return;
		}

		switch (ConnectionAck.withValue(reader.readByte())) {
		case ACCEPTED:
			protocol = Protocols.withValue(reader.readUShort());
			connectionState = ConnectionState.CONNECTED;
			elapsedConnectResend = 0.0f;
			elapsedConnectTimeout = 0.0f;
			logger.info("Connection established");
			connectionEstablished();
			break;

		case REJECTED:
			closeEndpoint();
			logger.info("Connection failed (rejected)");
			connectionFailed(ConnectionFailedReason.REJECTED);
			break;

		case NO_COMMON_PROTOCOL:
			closeEndpoint();
			logger.info("Connection failed (no common protocol)");
			connectionFailed(ConnectionFailedReason.NO_COMMON_PROTOCOL);
			break;

		default:
			closeEndpoint();
			logger.info("Connection failed (invalid message)");
			connectionFailed(ConnectionFailedReason.INVALID_MESSAGE);
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processConnectionClose(MessageReader reader) throws IOException {
		disconnect(true, true);
	}

	/**
	 * Synchronized by caller.
	 */
	private void processMessage(MessageReader reader) throws IOException {
		Message message = new Message();
		message.setLength(reader.length() - reader.position());
		reader.read(message);

		messageReceived(message);
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableMessage(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			// throw new IllegalArgumentException("Reliable message received although not
			// connected.");
			return;
		}

		int number = reader.readUShort();
		boolean validNumber;

		if (number < reliableNumberRecv) {
			validNumber = number < (reliableNumberRecv + reliableWindowSize) % 65535;

		} else {
			validNumber = number < reliableNumberRecv + reliableWindowSize;
		}
		if (!validNumber) {
			// throw new IllegalArgumentException("Reliable message: invalid sequence
			// number.");
			return;
		}

		Message ackMessage = new Message();
		try (MessageWriter writer = new MessageWriter(ackMessage)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_ACK.value);
			writer.writeUShort(number);
			writer.writeByte((byte) ReliableAck.SUCCESS.value);
		}
		endpoint.sendDatagram(realRemoteAddress, ackMessage);

		if (number == reliableNumberRecv) {
			processReliableMessageMessage(reader);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
			processQueuedMessages();

		} else {
			addReliableReceive(CommandCodes.RELIABLE_MESSAGE, number, reader);
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableMessageMessage(MessageReader reader) throws IOException {
		Message message = new Message();
		message.setLength(reader.length() - reader.position());
		reader.read(message);

		messageReceived(message);
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableAck(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			// throw new IllegalArgumentException("Reliable ack: not connected.");
			return;
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
			// throw new IllegalArgumentException("Reliable ack: no reliable transmission
			// with this number waiting for an ack!");
			return;
		}

		switch (code) {
		case SUCCESS:
			message.state = RealMessage.State.DONE;
			removeSendReliablesDone();
			break;

		case FAILED:
			logger.finest("Reliable ACK failed, resend");
			message.elapsedResend = 0.0f;
			endpoint.sendDatagram(realRemoteAddress, message.message);
			break;
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableLinkState(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			// throw new IllegalArgumentException("Link state: not connected.");
			return;
		}

		int number = reader.readUShort();
		boolean validNumber;

		if (number < reliableNumberRecv) {
			validNumber = number < (reliableNumberRecv + reliableWindowSize) % 65535;

		} else {
			validNumber = number < reliableNumberRecv + reliableWindowSize;
		}
		if (!validNumber) {
			// throw new IllegalArgumentException("Link state: invalid sequence number.");
			return;
		}

		Message ackMessage = new Message();
		try (MessageWriter writer = new MessageWriter(ackMessage)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_ACK.value);
			writer.writeUShort(number);
			writer.writeByte((byte) ReliableAck.SUCCESS.value);
		}
		endpoint.sendDatagram(realRemoteAddress, ackMessage);

		if (number == reliableNumberRecv) {
			processLinkState(reader);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
			processQueuedMessages();

		} else {
			addReliableReceive(CommandCodes.RELIABLE_LINK_STATE, number, reader);
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processLinkUp(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTED) {
			// throw new IllegalArgumentException("Reliable ack: not connected.");
			return;
		}

		int identifier = reader.readUShort();

		StateLink link = null;
		for (StateLink each : stateLinks) {
			if (each.getIdentifier() == identifier) {
				link = each;
				break;
			}
		}
		if (link == null) {
			// throw new IllegalArgumentException("Link with identifier absent");
			return;
		}
		if (link.getLinkState() != StateLink.LinkState.LISTENING) {
			// throw new IllegalArgumentException("Link with identifier not listening");
			return;
		}

		link.setLinkState(StateLink.LinkState.UP);
	}

	/**
	 * Synchronized by caller.
	 */
	private void processLinkDown(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTED) {
			// throw new IllegalArgumentException("Reliable ack: not connected.");
			return;
		}

		int identifier = reader.readUShort();

		StateLink link = null;
		for (StateLink each : stateLinks) {
			if (each.getIdentifier() == identifier) {
				link = each;
				break;
			}
		}
		if (link == null) {
			// throw new IllegalArgumentException("Link with identifier absent");
			return;
		}
		if (link.getLinkState() != StateLink.LinkState.UP) {
			// throw new IllegalArgumentException("Link with identifier not up");
			return;
		}

		link.setLinkState(StateLink.LinkState.DOWN);
	}

	/**
	 * Synchronized by caller.
	 */
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
			// throw new IllegalArgumentException("Link with identifier exists already");
		}

		// create linked network state
		Message message = new Message();
		message.setLength(reader.readUShort());
		reader.read(message);

		State state = createState(message, readOnly);

		CommandCodes code = CommandCodes.LINK_DOWN;
		if (state != null) {
			if (!state.linkReadAndVerifyAllValues(reader)) {
				// throw new IllegalArgumentException("Link state does not match the state
				// provided.");
				return;
			}
			if (link != null) {
				// throw new IllegalArgumentException("Link state existing already.");
				return;
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
		endpoint.sendDatagram(realRemoteAddress, message);
	}

	/**
	 * Synchronized by caller.
	 */
	private void processLinkUpdate(MessageReader reader) {
		if (connectionState != ConnectionState.CONNECTED) {
			// throw new IllegalArgumentException("Reliable ack: not connected.");
			return;
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
			if (link == null) {
				// throw new IllegalArgumentException("Invalid link identifier");
				return;
			}
			if (link.getLinkState() != StateLink.LinkState.UP) {
				// throw new IllegalArgumentException("Invalid link identifier");
				return;
			}

			State state = link.getState();
			if (state == null) {
				// throw new IllegalArgumentException("State link droppped");
				return;
			}

			state.linkReadValues(reader, link);
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableMessageLong(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			return;
		}

		int number = reader.readUShort();
		boolean validNumber;

		if (number < reliableNumberRecv) {
			validNumber = number < (reliableNumberRecv + reliableWindowSize) % 65535;

		} else {
			validNumber = number < reliableNumberRecv + reliableWindowSize;
		}
		if (!validNumber) {
			return;
		}

		Message ackMessage = new Message();
		try (MessageWriter writer = new MessageWriter(ackMessage)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_ACK.value);
			writer.writeUShort(number);
			writer.writeByte((byte) ReliableAck.SUCCESS.value);
		}
		endpoint.sendDatagram(realRemoteAddress, ackMessage);

		if (number == reliableNumberRecv) {
			processReliableMessageMessageLong(reader);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
			processQueuedMessages();

		} else {
			addReliableReceive(CommandCodes.RELIABLE_MESSAGE_LONG, number, reader);
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableMessageMessageLong(MessageReader reader) throws IOException {
		int flags = reader.readByte();
		if ((flags & LongMessageFlags.FIRST.value) != 0) {
			longMessage = new Message();
		}
		if (longMessage == null) {
			return;
		}
		
		int length = reader.length() - reader.position();
		int offset = longMessage.getLength();
		longMessage.setLengthRetain(offset + length);
		reader.read(longMessage.getData(), offset, length);
		
		if ((flags & LongMessageFlags.LAST.value) != 0) {
			Message message = longMessage;
			longMessage = null;
			
			message.setTimestamp(new Date());
			messageReceived(message);
			
		}else{
			messageProgress(longMessage.getLength());
		}
	}

	/**
	 * Synchronized by caller.
	 */
	private void processReliableLinkStateLong(MessageReader reader) throws IOException {
		if (connectionState != ConnectionState.CONNECTED) {
			return;
		}

		int number = reader.readUShort();
		boolean validNumber;

		if (number < reliableNumberRecv) {
			validNumber = number < (reliableNumberRecv + reliableWindowSize) % 65535;

		} else {
			validNumber = number < reliableNumberRecv + reliableWindowSize;
		}
		if (!validNumber) {
			return;
		}

		Message ackMessage = new Message();
		try (MessageWriter writer = new MessageWriter(ackMessage)) {
			writer.writeByte((byte) CommandCodes.RELIABLE_ACK.value);
			writer.writeUShort(number);
			writer.writeByte((byte) ReliableAck.SUCCESS.value);
		}
		endpoint.sendDatagram(realRemoteAddress, ackMessage);

		if (number == reliableNumberRecv) {
			processLinkStateLong(reader);
			reliableNumberRecv = (reliableNumberRecv + 1) % 65535;
			processQueuedMessages();

		} else {
			addReliableReceive(CommandCodes.RELIABLE_LINK_STATE_LONG, number, reader);
		}
	}
	
	/**
	 * Synchronized by caller.
	 */
	private void processLinkStateLong(MessageReader reader) throws IOException {
		int identifier = reader.readUShort();
		int flags = reader.readByte();
		
		StateLink link = null;
		for (StateLink each : stateLinks) {
			if (each.getIdentifier() == identifier) {
				link = each;
				break;
			}
		}
		if (link != null && link.getLinkState() != StateLink.LinkState.DOWN) {
			return;
		}
		
		if ((flags & LongLinkStateFlags.FIRST.value) != 0) {
			longLinkStateMessage = new Message();
			longLinkStateValues = new Message();
		}
		if (longLinkStateMessage == null || longLinkStateValues == null) {
			return;
		}
		
		// message
		int length = reader.readUShort();
		int offset = longLinkStateMessage.getLength();
		longLinkStateMessage.setLengthRetain(offset + length);
		reader.read(longLinkStateMessage.getData(), offset, length);
		
		// state values
		length = reader.length() - reader.position();
		offset = longLinkStateValues.getLength();
		longLinkStateValues.setLengthRetain(offset + length);
		reader.read(longLinkStateValues.getData(), offset, length);
		
		if ((flags & LongMessageFlags.LAST.value) == 0) {
			return;
		}
		
		// create linked network state
		Message message = longLinkStateMessage;
		longLinkStateMessage = null;
		
		Message values = longLinkStateValues;
		longLinkStateValues = null;
		
		CommandCodes code = CommandCodes.LINK_DOWN;
		
		boolean readOnly = (flags & LongLinkStateFlags.READ_ONLY.value) != 0;
		State state = createState(message, readOnly);
		
		if (state != null) {
			if (!state.linkReadAndVerifyAllValues(new MessageReader(values))) {
				return;
			}
			if (link != null) {
				return;
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
		endpoint.sendDatagram(realRemoteAddress, message);
	}
	
	/**
	 * Synchronized by caller.
	 */
	private void addReliableReceive(CommandCodes type, int number, MessageReader reader) {
		RealMessage message = new RealMessage();
		message.message.setLength(reader.length() - reader.position());
		reader.read(message.message);

		message.type = type;
		message.number = number;
		message.state = RealMessage.State.DONE;

		reliableMessagesRecv.add(message);
	}

	/**
	 * Synchronized by caller.
	 */
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

	/**
	 * Synchronized by caller.
	 */
	private void sendPendingReliables() throws IOException {
		int freeSlots = reliableWindowSize;
		for (RealMessage each : reliableMessagesSend) {
			if (each.state == RealMessage.State.SEND) {
				freeSlots--;
			}
		}
		if (freeSlots < 1) {
			return;
		}
		
		for (RealMessage each : reliableMessagesSend) {
			if (each.state != RealMessage.State.PENDING) {
				continue;
			}
			
			endpoint.sendDatagram(realRemoteAddress, each.message);

			each.state = RealMessage.State.SEND;
			each.elapsedResend = 0.0f;
			each.elapsedTimeout = 0.0f;
			
			freeSlots--;
			if (freeSlots == 0) {
				break;
			}
		}
	}

	private void startUpdateTask() {
		if (futureUpdateTask == null) {
			futureUpdateTask = scheduler.scheduleAtFixedRate(new UpdateStatesTask(), 0L, 5L, TimeUnit.MILLISECONDS);
		}
	}

	private void stopUpdateTask() {
		if (futureUpdateTask != null) {
			futureUpdateTask.cancel(false);
			futureUpdateTask = null;
		}
	}
}
