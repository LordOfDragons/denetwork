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

    def __init__(self: 'Connection') -> None:
        """Create connection."""
        print('Create Connection')
