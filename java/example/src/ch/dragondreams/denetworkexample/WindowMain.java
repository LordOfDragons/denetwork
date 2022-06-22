package ch.dragondreams.denetworkexample;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.Iterator;
import java.util.Queue;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JViewport;
import javax.swing.ScrollPaneConstants;
import javax.swing.SwingConstants;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultCaret;
import javax.swing.text.PlainDocument;

import ch.dragondreams.denetwork.Connection;
import ch.dragondreams.denetwork.Server;

public class WindowMain extends JFrame {
	private static final long serialVersionUID = 3936704916883629102L;

	public static final String CLASS_NAME = WindowMain.class.getCanonicalName();
	public static final String LOGGER_NAME = WindowMain.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	private JTextField textAddress;
	private JButton buttonListen;
	private JButton buttonConnect;
	private JButton buttonDisconnect;
	private JTextArea textLogs;

	private JTextField textServerTime;
	private JSlider sliderServerBar;


	private ExampleServer server = null;
	private ExampleConnection connection = null;

	private class LogToTextArea extends Handler {
		private Deque<String> entries = new ArrayDeque<>();

		public LogToTextArea() {
			setLevel(Level.ALL);
			setFormatter(new SimpleLineLogFormatter());
		}

		@Override
		public void publish(LogRecord record) {
			entries.add(getFormatter().format(record));
			while (entries.size() > 50) {
				entries.remove();
			}

			StringBuilder text = new StringBuilder();
			Iterator<String> iter = entries.iterator();
			while (iter.hasNext()) {
				text.append(iter.next());
			}

			textLogs.setText(text.toString());
		}

		@Override
		public void close() {
		}

		@Override
		public void flush() {
		}
	}

	public WindowMain() throws IOException {
		super("DENetwork Example");
		logger.entering(CLASS_NAME, "<constructor>");

		createContent(getContentPane());

		setSize(800, 600);
		setDefaultCloseOperation(DISPOSE_ON_CLOSE);

		setDefaultLookAndFeelDecorated(true);
		setVisible(true);
		setLocationRelativeTo(null);

		LogToTextArea logHandler = new LogToTextArea();
		Logger.getLogger("ch.dragondreams").addHandler(logHandler);

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

		System.exit(0);
	}

	public void closeWindow() {
		dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
	}

	private void serverListen() throws IOException {
		server = new ExampleServer(this);
		server.listenOn(textAddress.getText());

		textServerTime.setDocument(server.getValueTime().model);

		sliderServerBar.setModel(server.getValueBar().model);
		sliderServerBar.setEnabled(true);

		buttonConnect.setEnabled(false);
		buttonListen.setEnabled(false);
		buttonDisconnect.setEnabled(true);
	}

	private void clientConnect() {

	}

	private void disconnect() {
		if (connection != null) {
			connection.dispose();
			connection = null;
		}
		if (server != null) {
			server.dispose();
			server = null;
		}

		textServerTime.setDocument(new PlainDocument());

		sliderServerBar.setEnabled(false);
		sliderServerBar.setModel(new DefaultBoundedRangeModel(30, 0, 0, 60));

		buttonConnect.setEnabled(true);
		buttonListen.setEnabled(true);
		buttonDisconnect.setEnabled(false);
	}

	private void createContent(Container content) {
		content.setLayout(new BorderLayout(5, 5));
		content.add(createTopPanel(), BorderLayout.NORTH);
		content.add(createBottomPanel(), BorderLayout.SOUTH);
		content.add(createContentPanel(), BorderLayout.CENTER);
	}

	private JPanel createTopPanel() {
		JPanel panel = new JPanel();
		panel.setLayout(new FlowLayout(FlowLayout.LEFT, 5, 5));

		panel.add(new JLabel("Address:", SwingConstants.LEFT));

		textAddress = new JTextField("localhost");
		textAddress.setColumns(20);
		panel.add(textAddress);

		buttonConnect = new JButton(new AbstractAction("Connect") {
			private static final long serialVersionUID = -6567362516427880981L;

			@Override
			public void actionPerformed(ActionEvent e) {
				onConnect();
			}
		});
		panel.add(buttonConnect);

		buttonListen = new JButton(new AbstractAction("Listen") {
			private static final long serialVersionUID = 7423318570854341162L;

			@Override
			public void actionPerformed(ActionEvent e) {
				onListen();
			}
		});
		panel.add(buttonListen);

		buttonDisconnect = new JButton(new AbstractAction("Disconnect") {
			private static final long serialVersionUID = 2370171511983014101L;

			@Override
			public void actionPerformed(ActionEvent e) {
				onDisconnect();
			}
		});
		buttonDisconnect.setEnabled(false);
		panel.add(buttonDisconnect);

		return panel;
	}

	private JPanel createBottomPanel() {
		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

		textLogs = new JTextArea(10, 30);
		textLogs.setEditable(false);
		textLogs.setWrapStyleWord(true);
		textLogs.setLineWrap(true);
		DefaultCaret caret = (DefaultCaret) textLogs.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);

		JScrollPane scroll = new JScrollPane(textLogs);
		scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		scroll.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
		panel.add(scroll);

		return panel;
	}

	private JPanel createContentPanel() {
		JPanel panel = new JPanel();
		panel.setLayout(new BorderLayout());

		JPanel panel2 = new JPanel();
		panel2.setLayout(new GridLayout(0, 1));

		panel2.add(createContentPanelServer());

		panel.add(panel2, BorderLayout.NORTH);

		JScrollPane scrollPane = new JScrollPane(panel);
        scrollPane.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);

		JPanel cpane = new JPanel();
		cpane.setLayout(new BoxLayout(cpane, BoxLayout.Y_AXIS));
		cpane.add(scrollPane);
		return cpane;
	}

	private JPanel createContentPanelServer() {
		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		panel.setBorder(BorderFactory.createTitledBorder("Server:"));

		JPanel panel2 = new JPanel();
		panel2.setLayout(new BorderLayout(20, 0));
		panel2.add(new JLabel("Time:"), BorderLayout.WEST);
		textServerTime = new JTextField(20);
		textServerTime.setEditable(false);
		panel2.add(textServerTime, BorderLayout.CENTER);
		panel.add(panel2);

		panel2 = new JPanel();
		panel2.setLayout(new BorderLayout(20, 0));
		panel2.add(new JLabel("Bar:"), BorderLayout.WEST);
		sliderServerBar = new JSlider(SwingConstants.HORIZONTAL, 0, 60, 30);
		sliderServerBar.setMinorTickSpacing(5);
		sliderServerBar.setMajorTickSpacing(30);
		sliderServerBar.setPaintTicks(true);
		sliderServerBar.setEnabled(false);
		panel2.add(sliderServerBar, BorderLayout.CENTER);
		panel.add(panel2);

		return panel;
	}

	private void onConnect() {
		if (server != null || connection != null) {
			JOptionPane.showMessageDialog(this, "Already connected", "Connect", JOptionPane.INFORMATION_MESSAGE);
		} else {
			clientConnect();
		}
	}

	private void onListen() {
		if (server != null || connection != null) {
			JOptionPane.showMessageDialog(this, "Already connected", "Listen", JOptionPane.INFORMATION_MESSAGE);

		} else {
			try {
				serverListen();
			} catch (IOException e) {
				e.printStackTrace();
				JOptionPane.showMessageDialog(this, e.toString(), "Listen", JOptionPane.ERROR_MESSAGE);
			}
		}
	}

	private void onDisconnect() {
		disconnect();
	}
}
