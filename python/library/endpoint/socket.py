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

from .endpoint import Endpoint
from ..message.message import Message
from .address import Address
from . import ifaddr
from ipaddress import ip_address
import asyncio
import socket
import warnings


class SocketEndpoint(Endpoint, asyncio.DatagramProtocol):

    """Endpoint using UDP socket."""

    def __init__(self: 'SocketEndpoint') -> None:
        """Create SocketEndpoint."""

        Endpoint.__init__(self)

        self.transport = None

    def dispose(self: 'SocketEndpoint') -> None:
        """Dispose of SocketEndpoint."""
        self.close()

    def open(self: 'SocketEndpoint',
             address: Address,
             listener: 'SocketEndpoint.Listener') -> None:
        """Open SocketEndpoint delivering events to the provided listener.

        Parameters:
        address (Address): Address to listen on.
        listener (Listener): Listener to invoke.

        """
        if self.transport:
            raise Exception("already open")
        if not address:
            raise Exception("address is None")
        if not listener:
            raise Exception("listener is None")

        if address.type == Address.Type.IPV6:
            protocol = socket.AF_INET6
            saddr = (address.host(), address.port, 0,
                     self.scope_id_for(address))
        else:
            protocol = socket.AF_INET
            saddr = (address.host(), address.port)

        loop = asyncio.get_event_loop()
        (t, p) = loop.run_until_complete(loop.create_datagram_endpoint(
            lambda: self, local_addr=saddr, family=protocol))

        self.transport = t
        if address.type == Address.Type.IPV6:
            self.address = self.address_from_socket_ipv6(
                t.get_extra_info('sockname'))
        else:
            self.address = self.address_from_socket_ipv4(
                t.get_extra_info('sockname'))

    def close(self: 'SocketEndpoint') -> None:
        """Close SocketEndpoint if open.

        Stop delivering events to listener provided in the previous open call.

        """
        if self.transport:
            self.transport.close()
            self.transport = None

    def send_datagram(self: 'SocketEndpoint',
                      address: Address,
                      message: Message) -> None:
        """Send datagram.

        Parameters:
        address (Address): Address to send datagram to.
        message (Message): Message to send.

        """
        if self.transport:
            if self.address.type == Address.Type.IPV6:
                saddr = (address.host(), address.port, 0, 0)
            else:
                saddr = (address.host(), address.port)
            self.transport.sendto(message.data, saddr)

    def scope_id_for(self: 'SocketEndpoint', address: Address) -> int:
        """Find scope id for address or 0 if not found.

        Return:
        str: Scope id or None.

        """
        host = address.host()
        for adapter in ifaddr.get_adapters():
            for ip in adapter.ips:
                if ip.is_ipv6() and ip.ip[0] == host:
                    return ip.ip[2]
        return 0

    def address_from_socket_ipv4(self: 'SocketEndpoint',
                                 address: tuple) -> Address:
        """Get address from socket address.

        Parameters:
        address (tuple): Socket address.

        Return:
        Address: Converted address.

        """
        sa = int(ip_address(address[0]))
        values = []
        for _i in range(4):
            values.append(sa & 0xff)
            sa = sa >> 8
        return Address.ipv6(reversed(values), socket.ntohs(address[1]))

    def address_from_socket_ipv6(self: 'SocketEndpoint',
                                 address: tuple) -> Address:
        """Get address from socket address.

        Parameters:
        address (tuple): Socket address.

        Return:
        Address: Converted address.

        """
        sa = int(ip_address(address[0]))
        values = []
        for _i in range(32):
            values.append(sa & 0xff)
            sa = sa >> 8
        return Address.ipv6(reversed(values), socket.ntohs(address[1]))

    def datagram_received(self: 'SocketEndpoint',
                          data: str,
                          address: tuple) -> None:
        """DatagramProtocol.datagram_received.

        Parameters:
        data (str): Data.
        address (tuple): Socket address.

        """
        asyncio.get_event_loop().create_task(
            self.process_datagram(data, address))

    async def process_datagram(self: 'SocketEndpoint',
                               data: str, address: tuple) -> None:
        """Process received datagram."""
        if self.address.type == Address.Type.IPV6:
            a = self.address_from_socket_ipv6(address)
        else:
            a = self.address_from_socket_ipv4(address)
        self.listener.received_datagram(a, data)

    def error_received(self: 'SocketEndpoint',  exception: Exception) -> None:
        """DatagramProtocol.error_received.

        Parameters:
        exception (Exception): Exception.

        """
        warnings.warn("Endpoint received an error: {0!r}".format(exception))
