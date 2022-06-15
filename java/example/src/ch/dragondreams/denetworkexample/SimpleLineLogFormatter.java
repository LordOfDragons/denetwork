package ch.dragondreams.denetworkexample;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.logging.Formatter;
import java.util.logging.Handler;
import java.util.logging.LogRecord;

public class SimpleLineLogFormatter extends Formatter {
	private static final SimpleDateFormat timeFormatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS");

	public String format(LogRecord record) {
		final StringBuilder buffer = new StringBuilder();

		buffer.append('[');
		buffer.append(record.getLevel().getLocalizedName());
		buffer.append("] (");
		buffer.append(timeFormatter.format(new Date(record.getMillis())));
		buffer.append(") ");
		buffer.append(record.getSourceClassName());
		buffer.append('.');
		buffer.append(record.getSourceMethodName());
		buffer.append("(): ");
		buffer.append(formatMessage(record));
		buffer.append('\n');
		if (record.getThrown() != null) {
			try (StringWriter sw = new StringWriter(); PrintWriter pw = new PrintWriter(sw)) {
				record.getThrown().printStackTrace(pw);
				buffer.append(sw.toString());
			} catch (IOException ignore) {
			}
		}
		return buffer.toString();
	}

	@Override
	public String getHead(Handler h) {
		return "";
	}

	@Override
	public String getTail(Handler h) {
		return "";
	}
}
