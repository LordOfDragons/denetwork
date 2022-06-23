package ch.dragondreams.denetworkexample;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.Connection;
import ch.dragondreams.denetwork.Server;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.state.State;

public class ExampleConnection extends Connection {
	public static final String CLASS_NAME = ExampleConnection.class.getCanonicalName();
	public static final String LOGGER_NAME = ExampleConnection.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	static class OtherClientState extends State {
		final private int identifier;
		final private RemoteValueBar valueBar = new RemoteValueBar();

		public OtherClientState(int identifier) {
			super(true);
			this.identifier = identifier;
			addValue(valueBar);
		}

		public int getIdentifier() {
			return identifier;
		}

		public RemoteValueBar getValueBar() {
			return valueBar;
		}
	}

	final private WindowMain windowMain;

	private static int nextIdentifier = 1;

	final private int identifier;
	final private State state;
	final private LocalValueBar localValueBar;
	final private RemoteValueBar remoteValueBar;

	private State serverState;
	private RemoteValueTime serverValueTime;
	private RemoteValueBar serverValueBar;

	private List<OtherClientState> otherClientStates = new ArrayList<>();

	public ExampleConnection(WindowMain windowMain, boolean readOnly) {
		this.windowMain = windowMain;
		identifier = nextIdentifier++;
		state = new State(readOnly);
		
		if (readOnly) {
			localValueBar = null;
			remoteValueBar = new RemoteValueBar();
			remoteValueBar.setValue(30L);
			state.addValue(remoteValueBar);
		} else {
			remoteValueBar = null;
			localValueBar = new LocalValueBar();
			localValueBar.setValue(30L);
			state.addValue(localValueBar);
		}
	}

	@Override
	public void dispose() {
		if (getParentServer() == null) {
			windowMain.disconnectClientServerState();
		}
		super.dispose();
	}

	public LocalValueBar getLocalValueBar() {
		return localValueBar;
	}

	public RemoteValueBar getRemoteValueBar() {
		return remoteValueBar;
	}

	public int getIdentifier() {
		return identifier;
	}

	public State getState() {
		return state;
	}

	public State getServerState() {
		return serverState;
	}

	public RemoteValueTime getServerValueTime() {
		return serverValueTime;
	}

	public RemoteValueBar getServerValueBar() {
		return serverValueBar;
	}

	public List<OtherClientState> getOtherClientStates() {
		return otherClientStates;
	}

	@Override
	public void connectionEstablished() {
	}

	@Override
	public void connectionClosed() {
		Server server = getParentServer();
		if (server == null) {
			windowMain.closeWindow();
			return;
		}

		windowMain.disconnectClientState(this);
		
		for (Connection each : server.getConnections()) {
			if (each == this) {
				continue;
			}

			Message message = new Message();
			try (MessageWriter writer = new MessageWriter(message)) {
				writer.writeByte(MessageCode.DROP_CLIENT.value);
				writer.writeUShort(identifier);
			}

			try {
				each.sendReliableMessage(message);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	@Override
	public void messageReceived(Message message) {
		MessageReader reader = new MessageReader(message);

		switch (MessageCode.withValue(reader.readByte())) {
		case DROP_CLIENT: {
			int otherIdentifier = reader.readUShort();
			List<OtherClientState> remove = new ArrayList<>();
			for (OtherClientState each : otherClientStates) {
				if (each.getIdentifier() == otherIdentifier) {
					remove.add(each);
					windowMain.disconnectClientState(each);
				}
			}
			otherClientStates.removeAll(remove);
		}
			break;

		default:
			break;
		}
	}

	@Override
	public State createState(Message message, boolean readOnly) {
		MessageReader reader = new MessageReader(message);

		switch (MessageCode.withValue(reader.readByte())) {
		case LINK_SERVER_STATE:
			// create local state for server state. read-only to us
			serverState = new State(readOnly);
			serverValueTime = new RemoteValueTime();
			serverValueBar = new RemoteValueBar();
			serverState.addValue(serverValueTime);
			serverState.addValue(serverValueBar);
			windowMain.connectClientServerState();
			return serverState;

		case LINK_CLIENT_STATE:
			// use local state as client state. read-write to us
			return state;

		case LINK_OTHER_CLIENT_STATE: {
			// create other client state. read-only to us
			OtherClientState otherState = new OtherClientState(reader.readUShort());
			otherClientStates.add(otherState);
			windowMain.connectClientState(otherState);
			return otherState;
		}

		default:
			return null;
		}
	}
}
