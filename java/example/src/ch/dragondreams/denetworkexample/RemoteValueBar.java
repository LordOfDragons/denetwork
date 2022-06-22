package ch.dragondreams.denetworkexample;

import javax.swing.DefaultBoundedRangeModel;

import ch.dragondreams.denetwork.value.ValueInt;

public class RemoteValueBar extends ValueInt {
	final public DefaultBoundedRangeModel model = new DefaultBoundedRangeModel(30, 0, 0, 60);

	public RemoteValueBar() {
		super(Format.SINT16);
	}

	@Override
	public void remoteValueChanged() {
		model.setValue(getValue().intValue());
	}
}
