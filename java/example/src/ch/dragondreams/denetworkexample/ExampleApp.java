package ch.dragondreams.denetworkexample;

import java.util.logging.ConsoleHandler;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

import ch.dragondreams.denetwork.message.Message;

public class ExampleApp {
	public static final String CLASS_NAME = ExampleApp.class.getCanonicalName();
	public static final String LOGGER_NAME = ExampleApp.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	public static void main(String[] args) {
		// setup logging to be easier to read
		LogManager.getLogManager().reset();

		ConsoleHandler logHandlerConsole = new ConsoleHandler();
		logHandlerConsole.setFormatter(new SimpleLineLogFormatter());
		logHandlerConsole.setLevel(Level.ALL);
		logger.addHandler(logHandlerConsole);

		// run example application
		logger.info("Running Example");
		new Message();
	}

}
