package ch.dragondreams.denetworkexample;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;

import javax.swing.DefaultBoundedRangeModel;
import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

import ch.dragondreams.denetwork.Connection;
import ch.dragondreams.denetwork.Server;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageReader;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.state.State;
import ch.dragondreams.denetwork.value.ValueInt;
import ch.dragondreams.denetwork.value.ValueInteger.Format;
import ch.dragondreams.denetwork.value.ValueString;

public class ExampleConnection extends Connection {
	public static final String CLASS_NAME = ExampleConnection.class.getCanonicalName();
	public static final String LOGGER_NAME = ExampleConnection.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	static class OtherClientState extends State {
		final private int identifier;
		final private LocalValueBar valueBar = new LocalValueBar();

		public OtherClientState(int identifier) {
			super(true);
			this.identifier = identifier;
		}

		public int getIdentifier() {
			return identifier;
		}

		public LocalValueBar getValueBar() {
			return valueBar;
		}
	}

	class ServerValueTime extends ValueString {
		final public PlainDocument model = new PlainDocument();

		@Override
		public void remoteValueChanged() {
			try {
				model.replace(0, model.getLength(), getValue(), null);
			} catch (BadLocationException e) {
				e.printStackTrace();
			}
		}
	}

	class ServerValueBar extends ValueInt {
		final public DefaultBoundedRangeModel model = new DefaultBoundedRangeModel(30, 0, 0, 60);

		public ServerValueBar() {
			super(Format.SINT16);
		}

		@Override
		public void remoteValueChanged() {
			model.setValue(getValue().intValue());
		}
	}



	final private WindowMain windowMain;

	private static int nextIdentifier = 1;

	final private int identifier;
	final private State state;
	final private ValueInt valueBar = new ValueInt(Format.SINT16);

	private State serverState;
	private ServerValueTime serverValueTime;
	private ServerValueBar serverValueBar;

	private List<OtherClientState> otherClientStates = new ArrayList<>();

	public ExampleConnection(WindowMain windowMain, boolean readOnly) {
		this.windowMain = windowMain;
		identifier = nextIdentifier++;
		state = new State(readOnly);
		valueBar.setValue(30L);
		state.addValue(valueBar);
	}

	public ValueInt getValueBar() {
		return valueBar;
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

	public ValueString getServerValueTime() {
		return serverValueTime;
	}

	public ValueInt getServerValueBar() {
		return serverValueBar;
	}

	public List<OtherClientState> getOtherClientStates() {
		return otherClientStates;
	}

	@Override
	public void connectionEstablished() {
		logger.info("Connection established: Client " + identifier);
	}

	@Override
	public void connectionClosed() {
		logger.info("Connection closed: Client " + identifier);

		Server server = getParentServer();
		if (server == null) {
			windowMain.closeWindow();
			return;
		}

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
			serverValueTime = new ServerValueTime();
			serverValueBar = new ServerValueBar();
			serverState.addValue(serverValueTime);
			serverState.addValue(serverValueBar);
			return serverState;

		case LINK_CLIENT_STATE:
			// use local state as client state. read-write to us
			return state;

		case LINK_OTHER_CLIENT_STATE: {
			// create other client state. read-only to us
			OtherClientState otherState = new OtherClientState(reader.readUShort());
			otherClientStates.add(otherState);
			return otherState;
		}

		default:
			return null;
		}
	}
}
