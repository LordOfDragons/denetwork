package ch.dragondreams.denetworkexample;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.util.logging.Logger;

import javax.swing.JFrame;

import ch.dragondreams.denetwork.Connection;
import ch.dragondreams.denetwork.Server;

public class WindowMain extends JFrame {
	public static final String CLASS_NAME = WindowMain.class.getCanonicalName();
	public static final String LOGGER_NAME = WindowMain.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	private String paramListen = "";
	private String paramConnect = "";
	private Server server = null;
	private Connection connection = null;


	public WindowMain(String[] args) throws IOException {
		super("DENetwork Example");
		logger.entering(CLASS_NAME, "<constructor>");

		parseCommandLine(args);
		createContent();

		setSize(800, 400);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		setDefaultLookAndFeelDecorated(true);
		setVisible(true);
		setLocationRelativeTo(null);

		if(!paramListen.isEmpty()){
			serverListen();

		}else if(!paramConnect.isEmpty()){
			clientConnect();

		}else {
			parseCommandLine(new String[] {"--help"});
		}

		logger.exiting(CLASS_NAME, "<constructor>");
	}

	@Override
	public void dispose() {
		if (server != null) {
			server.dispose();
			server = null;
		}
		if (connection != null) {
			connection.dispose();
			connection = null;
		}
		super.dispose();
	}

	public void closeWindow() {
		dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
	}

	private void serverListen() throws IOException {
		server = new ExampleServer(this);
		server.listenOn(paramListen);
	}

	private void clientConnect() {

	}

	private void parseCommandLine(String[] args) {
		int i;
		for(i=0; i<args.length; i++){
			if(args[i].startsWith("--listen=")){
				paramListen = args[i].substring(9);

			}else if(args[i].startsWith("--connect=")){
				paramConnect = args[i].substring(10);

			}else if(args[i].equals("--help")){
				System.out.println("... [parameters]");
				System.out.println();
				System.out.println("parameters:");
				System.out.println();
				System.out.println("--listen=address:port");
				System.out.println("    listen for clients on address:port");
				System.out.println();
				System.out.println("--connect=address:port");
				System.out.println("    connect to host on address:port");
				System.out.println();
				System.out.println("--help");
				System.out.println("    show help screen");
				closeWindow();
				return;

			}else{
				throw new IllegalArgumentException("Invalid argument '" + args[i]+ "'");
			}
		}
	}

	private void createContent() {
	}
}
