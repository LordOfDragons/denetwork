/**
 * MIT License
 *
 * Copyright (c) 2022 DragonDreams (info@dragondreams.ch)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package ch.dragondreams.denetwork.state;

import java.util.logging.Logger;

import ch.dragondreams.denetwork.Connection;

public class StateLink {
	public static final String CLASS_NAME = StateLink.class.getCanonicalName();
	public static final String LOGGER_NAME = StateLink.class.getPackage().getName();

	protected static final Logger logger = Logger.getLogger(LOGGER_NAME);

	/**
	 * Link states.
	 */
	public enum LinkState {
		DOWN,
		LISTENING,
		UP
	}

	private final Connection connection;
	private final boolean[] valueChanged;

	private State state;
	private int identifier = -1;
	private LinkState linkState = LinkState.DOWN;
	private boolean changed = false;

	/**
	 * Create state link.
	 */
	public StateLink(Connection connection, State state) {
		if (connection == null) {
			throw new IllegalArgumentException("connection is null");
		}
		if (state == null) {
			throw new IllegalArgumentException("state");
		}
		this.connection = connection;
		this.state = state;
		valueChanged = new boolean[state.getValues().size()];
	}

	/**
	 * State or null if dropped.
	 */
	public State getState() {
		return state;
	}

	/**
	 * Unique identifier.
	 */
	public int getIdentifier() {
		return identifier;
	}

	/**
	 * Set unique identifier.
	 */
	public void setIdentifier(int identifier) {
		if (identifier < -1) {
			throw new IllegalArgumentException("identifier < -1");
		}
		this.identifier = identifier;
	}

	/**
	 * Link state.
	 */
	public LinkState getLinkState() {
		return linkState;
	}

	/**
	 * Set link state.
	 */
	public void setLinkState(LinkState linkState) {
		if (linkState == null) {
			throw new IllegalArgumentException("linkState is null");
		}
		this.linkState = linkState;
	}

	/**
	 * Connection.
	 */
	public Connection getConnection() {
		return connection;
	}

	/**
	 * State link changed.
	 */
	public boolean getChanged() {
		return changed;
	}

	/**
	 * Set if state link changed.
	 */
	public void setChanged(boolean changed) {
		if (changed != this.changed) {
			this.changed = changed;
			if (changed) {
				connection.addModifiedStateLink(this);
			}
		}
	}

	/**
	 * Value changed.
	 */
	public boolean getValueChangedAt(int index) {
		return valueChanged[index];
	}

	/**
	 * Set if value changed.
	 */
	public void setValueChangedAt(int index, boolean changed) {
		if (changed != valueChanged[index]) {
			valueChanged[index] = changed;
			if (changed) {
				setChanged(true);
			}
		}
	}

	/**
	 * One or more values are marked changed.
	 */
	public boolean hasChangedValues() {
		for (boolean each : valueChanged) {
			if (each) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Reset changed of all values and state link.
	 */
	public void resetChanged() {
		changed = false;
		int i;
		for (i = 0; i < valueChanged.length; i++) {
			valueChanged[i] = false;
		}
	}

	/**
	 * For use by State only.
	 */
	public void dropState() {
		state = null;
	}
}
