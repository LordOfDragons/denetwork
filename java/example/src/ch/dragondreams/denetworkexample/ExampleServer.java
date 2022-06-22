package ch.dragondreams.denetworkexample;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

import javax.swing.text.BadLocationException;

import ch.dragondreams.denetwork.Connection;
import ch.dragondreams.denetwork.Server;
import ch.dragondreams.denetwork.message.Message;
import ch.dragondreams.denetwork.message.MessageWriter;
import ch.dragondreams.denetwork.state.State;

public class ExampleServer extends Server {
	public static final String CLASS_NAME = ExampleServer.class.getCanonicalName();
	public static final String LOGGER_NAME = ExampleServer.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	final private WindowMain windowMain;
	final private State state = new State(false);
	final private LocalValueTime valueTime = new LocalValueTime();
	final private LocalValueBar valueBar = new LocalValueBar();

	final static public SimpleDateFormat timeFormatter = new SimpleDateFormat("EEE MMM d HH:mm:ss yyyy");

	final private ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
	final private ScheduledFuture<?> futureUpdateTask;

	public ExampleServer(WindowMain windowMain) {
		this.windowMain = windowMain;
		valueBar.setValue(30L);
		state.addValue(valueTime);
		state.addValue(valueBar);

		futureUpdateTask = scheduler.scheduleAtFixedRate(new Runnable() {
			@Override
			public void run() {
				updateTime();
			}
		}, 1L, 1L, TimeUnit.SECONDS);
	}

	@Override
	public void dispose() {
		futureUpdateTask.cancel(false);
		super.dispose();
	}

	public WindowMain getWindowMain() {
		return windowMain;
	}

	public State getState() {
		return state;
	}

	public LocalValueTime getValueTime() {
		return valueTime;
	}

	public LocalValueBar getValueBar() {
		return valueBar;
	}

	@Override
	public Connection createConnection() {
		return new ExampleConnection(windowMain, true);
	}

	@Override
	public void onClientConnected(Connection connection) throws IOException {
		linkClient(connection);
	}

	protected void linkClient(Connection connection) throws IOException {
		ExampleConnection excon = (ExampleConnection) connection;

		// link server state. read-only on client side
		Message message = new Message();
		try (MessageWriter writer = new MessageWriter(message)) {
			writer.writeByte(MessageCode.LINK_SERVER_STATE.value);
		}
		connection.linkState(message, state, true);

		// link all client states. this includes connecting client itself
		for (Connection each : getConnections()) {
			if (each == excon) {
				// connecting client gets read-write state of its own state
				message = new Message();
				try (MessageWriter writer = new MessageWriter(message)) {
					writer.writeByte(MessageCode.LINK_CLIENT_STATE.value);
				}
				connection.linkState(message, excon.getState(), false);

			} else {
				ExampleConnection othercon = (ExampleConnection) each;

				// all other client states are read-only to the connecting client
				message = new Message();
				try (MessageWriter writer = new MessageWriter(message)) {
					writer.writeByte(MessageCode.LINK_OTHER_CLIENT_STATE.value);
					writer.writeUShort(othercon.getIdentifier());
				}
				connection.linkState(message, othercon.getState(), true);

				// and the connecting client state is also read-only to the others
				message = new Message();
				try (MessageWriter writer = new MessageWriter(message)) {
					writer.writeByte(MessageCode.LINK_OTHER_CLIENT_STATE.value);
					writer.writeUShort(excon.getIdentifier());
				}
				othercon.linkState(message, excon.getState(), true);
			}
		}
	}

	public void updateTime() {
		try {
			valueTime.model.replace(0, valueTime.model.getLength(), timeFormatter.format(new Date()), null);
		} catch (BadLocationException e) {
			e.printStackTrace();
		}
	}
}
