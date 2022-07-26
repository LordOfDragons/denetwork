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

from typing import Optional, List
from .endpoint.socket import SocketEndpoint


class Server:

    """Network server.

    Allows clients speaking Drag[en]gine Network Protocol to connect.

    To use this class create a subclass and overwrite createConnection()
    and clientConnected(). The method createConnection() method creates
    a Connection instance for each connecting client. By overwriting this
    method you can create a subclass of Connection handling the client.
    Overwriting clientConnected() allows to communicate with a connecting
    client to link states and exchanging messages.

    The default implementation connects by creating a UDP socket. If you
    have to accept clients using a different transportation method overwrite
    createSocket() to create a class instance implementing Endpoint interface
    providing the required capabilities. Usually this is not required.

    To start listening call ListenOn with the IP address to listen on in the
    format "hostnameOrIP" or "hostnameOrIP:port". You can use a resolvable
    hostname or an IPv4. If the port is not specified the default port 3413
    is used. You can use any port you you like.

    """

    def __init__(self: 'Server') -> None:
        """Create server."""
        print('Create Server')

    def find_public_address(self: 'Server') -> List[str]:
        """Find public addresses.

        Return:
        List[str]: List of address as string.

        """
        return SocketEndpoint.find_public_address()

    def resolve_address(self: 'Server', address: str) -> str:
        """Resolve address.

        Address is in the format "hostnameOrIP" or "hostnameOrIP:port".
        You can use a resolvable hostname or an IPv4. If the port is not
        specified the default port 3413 is used.

        If you overwrite create_socket() you have to also overwrite this
        method to resolve address using the appropriate method.

        Parameters:
        address (str): Address to resolve.

        Return:
        str: Resolved address.

        """
        return SocketEndpoint.resolve_address(address)
