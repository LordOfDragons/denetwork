package ch.dragondreams.denetworkexample;

public enum MessageCode {
	LINK_SERVER_STATE(1),
	LINK_CLIENT_STATE(2),
	LINK_OTHER_CLIENT_STATE(3),
	DROP_CLIENT(4);

	final public byte value;

	private MessageCode(int value) {
		this.value = (byte) value;
	}
}
