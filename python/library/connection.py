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

"""@package Drag[en]gine Network Library Python Module."""

from typing import List, Deque
from .endpoint.socket import SocketEndpoint
from .endpoint.address import Address
from .message.message import Message
from .message.reader import MessageReader
from .message.writer import MessageWriter
from .endpoint.endpoint import Endpoint
from .protocol import *
from collections import deque
import logging
from enum import IntEnum, auto
import asyncio
from time import time_ns


class Connection:

    """Network connection.

    Allows clients to connect to a server speaking Drag[en]gine Network
    Protocol.

    To use this class create a subclass overwriting one or more of the
    methods below.

    To start connecting to the server call connectTo() with the IP address
    to connect to in the format "hostnameOrIP" or "hostnameOrIP:port". You
    can use a resolvable hostname or an IPv4. If the port is not specified
    the default port 3413 is used. You can use any port you you like.
    Connecting attempt fails if it takes longer than SetConnectTimeout
    seconds. The default timeout is 3 seconds.

    If connecting to the server succeedes connectionEstablished() is called.
    Overwrite to request linking states and exchanging messages with the
    server. If connection timed out or another error occured
    connectionFailed() is called with the failure reason. Overwrite to
    handle connection failure.

    You can close the connection by calling disconnect(). This calls
    connectionClosed() which you can overwrite. This method is also called
    if the server closes the connection.

    Overwrite createState() to create states requested by the server.
    States synchronize a fixed set of values between the server and the
    client. The client can have read-write or read-only access to the state.
    Create an instance of a subclass of denState to handle individual states.
    It is not necessary to create a subclass of denState if you intent to
    subclass denValue* instead.

    Overwrite messageReceived() to process messages send by the server.

    Call update() in regular intervals to receive and process incoming
    messages as well as updating states. DENetwork does not use internal
    threading giving you full control over threading.

    To get logging implemnent a subclass of denLogger and set the logger
    instance using setLogger(). You can share the logger instance across
    multiple servers and connections.

    """

    class ConnectionState(IntEnum):

        """Connection state."""

        DISCONNECTED = auto()
        """Disconnected."""

        CONNECTING = auto()
        """Connecting."""

        CONNECTED = auto()
        """Connected."""

    class ConnectionFailedReason(IntEnum):

        """Connection failed reason."""

        GENERIC = auto()
        """Generic."""

        TIMEOUT = auto()
        """Timeout."""

        REJECTED = auto()
        """Rejected."""

        NO_COMMON_PROTOCOL = auto()
        """No common protocol."""

        INVALID_MESSAGE = auto()
        """Invalid message."""

    def __init__(self: 'Connection') -> None:
        """Create connection."""

        self._local_address = None
        self._remote_address = None
        self._endpoint = None
        self._real_remote_address = None
        self._connection_state = Connection.ConnectionState.DISCONNECTED
        self._connect_resend_interval = 1.0
        self._connect_timeout = 5.0
        self.reliable_resend_interval = 0.5
        self._reliable_timeout = 3.0
        self._elapsed_connect_resend = 0.0
        self._elapsed_connect_timeout = 0.0
        self._protocol = Protocols.DENETWORK_PROTOCOL
        self._state_links = deque()
        self._modified_state_links = deque()
        self._next_link_identifier = 0
        self._reliable_messages_send = deque()
        self._reliable_messages_recv = deque()
        self._reliable_number_send = 0
        self._reliable_number_recv = 0
        self._reliable_window_size = 10
        self._parentServer = None
        self._update_task = None

    def dispose(self: 'Connection') -> None:
        """Dispose of connection."""
        self._stop_update_task()
        try:
            self.disconnect(False, False)
        except Exception:
            logging.exception("Connection dispose")

        del self._reliable_messages_send[:]
        del self._reliable_messages_recv[:]
        self._reliable_number_send = 0
        self._reliable_number_recv = 0
        del self._modified_state_links[:]
        del self._state_links[:]

        if self._parent_server is None and self._endpoint is not None:
            self._endpoint.dispose()
        self._endpoint = None
        self._parent_server = None

    @property
    def local_address(self: 'Connection') -> Address:
        """Local address.

        Return:
        Address: Address.

        """
        return self._local_address

    @property
    def remote_address(self: 'Connection') -> Address:
        """Remote address.

        Return:
        Address: Address.

        """
        return self._remote_address

    @property
    def connect_resend_interval(self: 'Connection') -> float:
        """Connect resent interval in seconds.

        Return:
        float: Interval in seconds.

        """
        return self._connect_resend_interval

    @connect_resend_interval.setter
    def connect_resend_interval(self: 'Connection', value: float) -> None:
        """Connect resent interval in seconds.

        Parameters:
        value (float): Interval in seconds.

        """
        self._connect_resend_interval = max(interval, 0.01)

    @property
    def connect_timeout(self: 'Connection') -> float:
        """Connect timeout in seconds.

        Return:
        float: Timeout in seconds.

        """
        return self._connect_timeout

    @connect_timeout.setter
    def connect_timeout(self: 'Connection', value: float) -> None:
        """Connect resent interval in seconds.

        Parameters:
        value (float): Interval in seconds.

        """
        self._connect_timeout = max(timeout, 0.01)

    @property
    def reliable_resend_interval(self: 'Connection') -> float:
        """Reliable message resend interval in seconds.

        Return:
        float: Interval in seconds.

        """
        return self._reliable_resend_interval

    @reliable_resend_interval.setter
    def reliable_resend_interval(self: 'Connection', value: float) -> None:
        """Set reliable message resend interval in seconds.

        Parameters:
        value (float): Interval in seconds.

        """
        self._reliable_resend_interval = max(value, 0.01)

    @property
    def reliable_timeout(self: 'Connection') -> float:
        """Reliable message timeout in seconds.

        Return:
        float: Timeout in seconds.

        """
        return self._reliable_timeout

    @reliable_timeout.setter
    def reliable_timeout(self: 'Connection', value: float) -> None:
        """Set reliable message timeout in seconds.

        Parameters:
        value (float): Timeout in seconds.

        """
        self._reliable_timeout = max(timeout, 0.01)

    @property
    def connection_state(self: 'Connection') -> 'Connection.ConnectionState':
        """Connection state.

        Return:
        Connection.ConnectionState: Connection state.

        """
        return self._connection_state

    @property
    def connected(self: 'Connection') -> bool:
        """Connection to a remote host is established.

        Return:
        bool: Connected.

        """
        return self._connection_state == Connection.ConnectionState.CONNECTED

    async def _task_update(self: 'Connection') -> None:
        """Update task."""
        last_time = time_ns()
        while True:
            await asyncio.sleep(0.005)
            if self._update_task is None:
                break
            cur_time = time_ns()
            elapsed = 1e-9 * (cur_time - last_time)
            last_time = cur_time
            try:
                self._update_timeouts(elapsed)
                self._updateStates()
            except Exception:
                logging.exception("Connection timer update")

    def _start_update_task(self: 'Connection') -> None:
        """Start update task."""
        if self._update_task is None:
            self._update_task = asyncio.get_event_loop().create_task(
                self._task_update)

    def _stop_update_task(self: 'Connection') -> None:
        """Stop update task."""
        if self._update_task is not None:
            self._update_task.cancel()
            self._update_task = None
