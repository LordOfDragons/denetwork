package ch.dragondreams.denetworkexample;

import javax.swing.DefaultBoundedRangeModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import ch.dragondreams.denetwork.value.ValueInt;

/**
 * Integer state value updated if model changes.
 */
class LocalValueBar extends ValueInt implements ChangeListener {
	final public DefaultBoundedRangeModel model = new DefaultBoundedRangeModel(30, 0, 0, 60);

	public LocalValueBar() {
		super(Format.SINT16);
		model.addChangeListener(this);
	}

	@Override
	public void stateChanged(ChangeEvent event) {
		setValue((long) model.getValue());
	}
}