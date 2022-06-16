package ch.dragondreams.denetworkexample;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.Connection;
import ch.dragondreams.denetwork.Server;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.state.State;
import ch.dragondreams.denetwork.value.ValueInt;
import ch.dragondreams.denetwork.value.ValueString;
import ch.dragondreams.denetwork.value.ValueInteger.Format;

public class ExampleConnection extends Connection {
	public static final String CLASS_NAME = ExampleConnection.class.getCanonicalName();
	public static final String LOGGER_NAME = ExampleConnection.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	static class OtherClientState extends State{
		final private int identifier;
		final private ValueInt valueBar = new ValueInt(Format.SINT16);

		public OtherClientState(int identifier) {
			super(true);
			this.identifier = identifier;
		}

		public int getIdentifier() {
			return identifier;
		}

		public ValueInt getValueBar() {
			return valueBar;
		}
	};

	final private WindowMain windowMain;

	private static int nextIdentifier = 1;

	final private int identifier;
	final private State state;
	final private ValueInt valueBar = new ValueInt(Format.SINT16);

	private State serverState;
	private ValueString serverValueTime;
	private ValueInt serverValueBar;

	private List<State> otherClientStates = new ArrayList<>();

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

	@Override
	public void connectionEstablished() {
		logger.info("Connection established: Client " + identifier);
	}

	@Override
	public void connectionClosed() {
		logger.info("Connection closed: Client " + identifier);

		Server server = getParentServer();
		if(server == null){
			windowMain.closeWindow();
			return;
		}

		for(Connection each : server.getConnections()) {
			if(each == this){
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
}
