package ch.dragondreams.denetworkexample;

import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

import ch.dragondreams.denetwork.value.ValueString;

/**
 * String state value updated if model changes.
 */
class LocalValueTime extends ValueString implements DocumentListener {
	final public PlainDocument model = new PlainDocument();

	public LocalValueTime() {
		super();
		model.addDocumentListener(this);
	}

	@Override
	public void remoteValueChanged() {
		try {
			model.replace(0, model.getLength(), getValue(), null);
		} catch (BadLocationException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void changedUpdate(DocumentEvent event) {
		doUpdateValue();
	}

	@Override
	public void insertUpdate(DocumentEvent event) {
		doUpdateValue();
	}

	@Override
	public void removeUpdate(DocumentEvent event) {
		doUpdateValue();
	}

	private void doUpdateValue() {
		try {
			setValue(model.getText(0, model.getLength()));
		} catch (BadLocationException e) {
			e.printStackTrace();
		}
	}
}