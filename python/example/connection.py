# -*- coding: utf-8 -*-

# MIT License
#
# Copyright (c) 2022 DragonDreams (info@dragondreams.ch)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""@package Drag[en]gine Network Library Example."""

# Import Drag[en]gine Network Library python module. It is recommended
# to use an alias name for the imported module to simplify usage
import DENetworkLibrary as dnl

from protocol import MessageCodes
from typing import List
import logging


class ExampleConnection(dnl.Connection):

    """Example connection."""

    _next_id = 1

    class OtherClientState(dnl.state.State):

        """Other client state."""

        def __init__(self: 'ExampleConnection.OtherClientState',
                     identifier: int) -> None:
            """Create other client state."""
            dnl.state.State.__init__(self,  True)
            self._id = identifier
            self._value_bar = dnl.value.ValueInt(
                dnl.value.ValueInt.Format.SINT16)
            self.add_value(self._value_bar)

        @property
        def id(self: 'ExampleConnection.OtherClientState') -> int:
            """Id.

            Return:
            int: Id.

            """
            return self._id

        @property
        def value_bar(self: 'ExampleConnection.OtherClientState') -> (
                dnl.value.ValueInt):
            """Value bar.

            Return:
            dnl.value.ValueInt: Value.

            """
            return self._value_bar

    def __init__(self: 'ExampleConnection', app: 'ExampleApp',
                 read_only: bool) -> None:
        """Create example connection."""
        dnl.Connection.__init__(self)
        self._app = app
        self._id = ExampleConnection._next_id
        ExampleConnection._next_id = ExampleConnection._next_id + 1
        self._state = dnl.state.State(read_only)
        self._value_bar = dnl.value.ValueInt(dnl.value.ValueInt.Format.SINT16)
        self._value_bar.value = 30
        self._state.add_value(self._value_bar)
        self._server_state = None
        self._server_value_time = None
        self._other_client_states = []

    @property
    def id(self: 'ExampleConnection') -> int:
        """Id.

        Return:
        int: Id.

        """
        return self._id

    @property
    def bar(self: 'ExampleConnection') -> int:
        """Get bar.

        Return:
        int: Bar.

        """
        return self._value_bar.value

    @bar.setter
    def bar(self: 'ExampleConnection', value: int) -> None:
        """Set bar.

        Parameter:
        value (int): Bar.

        """
        self._value_bar.value = max(min(value, 60), 0)

    def increment_bar(self: 'ExampleConnection', amount: int) -> None:
        """Increment bar.

        Parameter:
        amount (int): Amount.

        """
        self.bar = self.bar + amount

    @property
    def ready(self: 'ExampleConnection') -> bool:
        """Connection ready.

        Return:
        bool: Ready.

        """
        return self._value_bar is not None

    def connection_established(self: 'ExampleConnection') -> None:
        """Connection established. Callback for subclass."""
        logging.info("Connection: Connection established")

    def connection_failed(self: 'ExampleConnection',
                          reason: 'dnl.connection.Connection.ConnectionFailedReason') -> None:
        """Connection failed or timeout out. Callback for subclass."""
        logging.info("Connection: Connection failed")
        self.connection_closed()

    def connection_closed(self: 'ExampleConnection') -> None:
        """Connection closed."""
        logging.info("Connection: Connection closed")
        if self.parent_server is not None:
            for c in self.parent_server.connections:
                if c == self:
                    message = dnl.message.Message()
                    with dnl.message.MessageWriter(message) as w:
                        w.write_byte(MessageCodes.DROP_CLIENT.value)
                        w.write_ushort(self._id)
                    """This can fail if the client closes connection not us.
                    We could track this properly but this is fair enough."""
                    try:
                        c.send_reliable_message(message)
                    except Exception:
                        pass
        else:
            self._app.quit = True

    def message_received(self: 'ExampleConnection',
                         message: dnl.message.Message) -> None:
        """Message received. Called asynchronously. Callback for subclass."""
        logging.info("Connection: Message received")
        with dnl.message.MessageReader(message) as r:
            code = MessageCodes(r.read_byte())
            if code == MessageCodes.DROP_CLIENT:
                identifier = r.read_ushort()
                found = [c for c in self._other_client_states
                         if c._id == identifier]
                if found:
                    del self._other_client_states[
                        self._other_client_states.index(found[0])]

    def create_state(self: 'ExampleConnection', message: dnl.message.Message,
                     read_only: bool) -> dnl.state.State:
        """Host send state to link.

        Return:
        State State or None to reject.

        """
        logging.info("Connection: Create state")
        with dnl.message.MessageReader(message) as r:
            code = MessageCodes(r.read_byte())
            if code == MessageCodes.LINK_SERVER_STATE:
                """create local state for server state. read-only to us"""
                self._server_state = dnl.state.State(read_only)
                self._server_value_time = dnl.value.ValueString()
                self._server_value_bar = dnl.value.ValueInt(
                    dnl.value.ValueInt.Format.SINT16)
                self._server_state.add_value(self._server_value_time)
                self._server_state.add_value(self._server_value_bar)
                return self._server_state
            elif code == MessageCodes.LINK_CLIENT_STATE:
                """use local state as client state. read-write to us"""
                return self._state
            elif code == MessageCodes.LINK_OTHER_CLIENT_STATE:
                """create other client state. read-only to us"""
                identifier = r.read_ushort()
                other_state = ExampleConnection.OtherClientState(identifier)
                self._other_client_states.append(other_state)
                return other_state
        return None

    @property
    def has_server_state(self: 'ExampleConnection') -> bool:
        """Server state is present.

        Return:
        bool: Has server state.

        """
        return self._server_state is not None

    @property
    def server_time(self: 'ExampleConnection') -> str:
        """Get server time.

        Return:
        str: Server time.

        """
        return self._server_value_time.value

    @property
    def server_bar(self: 'ExampleConnection') -> int:
        """Get server bar.

        Return:
        int: Server bar.

        """
        return self._server_value_bar.value

    @property
    def other_client_states(self: 'ExampleConnection') -> (
            List['ExampleConnection.OtherClientState']):
        """Other client states.

        Return:
        List[OtherClientState]: Other client states.

        """
        return self._other_client_states
