package ch.dragondreams.denetworkexample;

import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

import ch.dragondreams.denetwork.value.ValueString;

public class RemoteValueTime extends ValueString {
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
