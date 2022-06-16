package ch.dragondreams.denetworkexample;

import java.util.logging.Logger;

import ch.dragondreams.denetwork.Server;
import ch.dragondreams.denetwork.state.State;
import ch.dragondreams.denetwork.value.ValueInt;
import ch.dragondreams.denetwork.value.ValueInteger.Format;
import ch.dragondreams.denetwork.value.ValueString;

public class ExampleServer extends Server {
	public static final String CLASS_NAME = ExampleServer.class.getCanonicalName();
	public static final String LOGGER_NAME = ExampleServer.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	final private WindowMain windowMain;
	final private State state = new State(false);
	final private ValueString valueTime = new ValueString();
	final private ValueInt valueBar = new ValueInt(Format.SINT16);

	public ExampleServer(WindowMain windowMain) {
		this.windowMain = windowMain;
		valueBar.setValue(30L);
		state.addValue(valueTime);
		state.addValue(valueBar);
	}

	public WindowMain getWindowMain() {
		return windowMain;
	}

	public State getState() {
		return state;
	}

	public ValueString getValueTime() {
		return valueTime;
	}

	public ValueInt getValueBar() {
		return valueBar;
	}
}
