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

from connection import ExampleConnection
from protocol import MessageCodes

from datetime import datetime, timezone
import logging


class ExampleServer(dnl.Server):

    """Example server."""

    def __init__(self: 'ExampleServer', app: 'ExampleApp') -> None:
        """Create example server."""
        dnl.Server.__init__(self)
        self._app = app
        self._state = dnl.state.State(False)
        self._value_time = dnl.value.ValueString()
        self._value_bar = dnl.value.ValueInt(dnl.value.ValueInt.Format.SINT16)
        self._value_bar.value = 30
        self._state.add_value(self._value_time)
        self._state.add_value(self._value_bar)

    def create_connection(self: 'ExampleServer') -> dnl.connection.Connection:
        """Create connection for each connecting client.

        Return:
        Connection: Connection.

        """
        logging.info("Server: Create connection")
        return ExampleConnection(self._app, True)

    @property
    def time(self: 'ExampleServer') -> str:
        """Get time.

        Return:
        str: Time.

        """
        return self._value_time.value

    @time.setter
    def time(self: 'ExampleServer', value: str) -> None:
        """Set time.

        Parameter:
        value (str): Time.

        """
        self._value_time.value = value

    def update_time(self: 'ExampleServer') -> None:
        """Update time."""
        self.time = datetime.now(timezone.utc).ctime()

    @property
    def bar(self: 'ExampleServer') -> int:
        """Get bar.

        Return:
        int: Bar.

        """
        return self._value_bar.value

    @bar.setter
    def bar(self: 'ExampleServer', value: int) -> None:
        """Set bar.

        Parameter:
        value (int): Bar.

        """
        self._value_bar.value = max(min(value, 60), 0)

    def increment_bar(self: 'ExampleServer', amount: int) -> None:
        """Increment bar.

        Parameter:
        amount (int): Amount.

        """
        self.bar = self.bar + amount

    def client_connected(self: 'ExampleServer',
                         connection: dnl.connection.Connection) -> None:
        """Client connected.

        Parameters:
        connection (Connection): Connection of connecting client.

        """
        logging.info("Server: Client connected")
        self._link_client(connection)

    def _link_client(self: 'ExampleServer',
                     connection: dnl.connection.Connection) -> None:
        """Link Client.

        Parameter:
        connection (Connection): Connection.

        """

        """link server state. read-only on client side"""
        message = dnl.message.Message()
        with dnl.message.MessageWriter(message) as w:
            w.write_byte(MessageCodes.LINK_CLIENT_STATE.value)
        connection.link_state(message, self._state, True)

        """link all client states. this includes connecting client itself"""
        for c in self.connections:
            if c == connection:
                """connecting client gets read-write state of its own state"""
                message = dnl.message.Message()
                with dnl.message.MessageWriter(message) as w:
                    w.write_byte(MessageCodes.LINK_CLIENT_STATE.value)
                connection.link_state(message, self._state, False)
            else:
                """all other client states are read-only to the
                connecting client"""
                message = dnl.message.Message()
                with dnl.message.MessageWriter(message) as w:
                    w.write_byte(MessageCodes.LINK_OTHER_CLIENT_STATE.value)
                    w.write_ushort(c._id)
                connection.link_state(message, c._state, True)

                """and the connecting client state is also read-only
                to the others"""
                message = dnl.message.Message()
                with dnl.message.MessageWriter(message) as w:
                    w.write_byte(MessageCodes.LINK_OTHER_CLIENT_STATE.value)
                    w.write_ushort(self._id)
                connection.link_state(message, self._state, True)
