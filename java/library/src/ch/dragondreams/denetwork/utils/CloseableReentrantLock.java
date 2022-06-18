package ch.dragondreams.denetwork.utils;

import java.util.concurrent.locks.ReentrantLock;

/**
 * ReentrantLock modified to support AutoClosing.
 */
public class CloseableReentrantLock extends ReentrantLock implements AutoCloseable {
	private static final long serialVersionUID = 5065016248643488794L;

	public CloseableReentrantLock open() {
		lock();
		return this;
	}

	@Override
	public void close() {
		unlock();
	}
}