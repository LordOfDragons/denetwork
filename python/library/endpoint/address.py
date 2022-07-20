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
from enum import IntEnum
from io import StringIO


class Address:

    """Endpoint address.

    Can be IPv4 or IPv6 address.

    """

    class Type(IntEnum):

        """Address type."""

        IPV4 = 0
        """IPv4."""

        IPV6 = 1
        """IPv6."""

    def __init__(self: 'Address') -> None:
        """Create address."""

        self.type = Address.Type.IPV4
        """Address type."""

        self.values = ()
        """Address components."""

        self.port = 3413
        """Port."""

    @classmethod
    def ipv4(cls: 'Address', values: List[int], port: int) -> 'Address':
        """Create IPv4 address.

        The values represent the address values with 0 being the left
        most value.

        Return:
        Address: Address.

        """

        address = Address()
        address.type = Address.Type.IPV4
        address.values = tuple(values)
        address.port = port
        return address

    @classmethod
    def ipv6(cls: 'Address', values: List[int], port: int) -> 'Address':
        """Create IPv6 address.

        The values represent the address values with 0 being the left
        most value.

        Return:
        Address: Address.

        """

        address = Address()
        address.type = Address.Type.IPV6
        address.values = tuple(values)
        address.port = port
        return address

    @classmethod
    def ipv4_any(cls: 'Address') -> 'Address':
        """Create IPv4 any address.

        Return:
        Address: Address.

        """

        address = Address()
        address.type = Address.Type.IPV4
        address.values = tuple([0] * 4)
        address.port = 0
        return address

    @classmethod
    def ipv6_any(cls: 'Address') -> 'Address':
        """Create IPv6 any address.

        Return:
        Address: Address.

        """

        address = Address()
        address.type = Address.Type.IPV6
        address.values = tuple([0] * 16)
        address.port = 0
        return address

    @classmethod
    def ipv4_loopback(cls: 'Address', port: Optional[int] = 3413) -> 'Address':
        """Create IPv4 loopback address.

        Parameters:
        port (int): Port. Default 3413.

        Return:
        Address: Address.

        """

        address = Address()
        address.type = Address.Type.IPV4
        address.values = (127, 0, 0, 1)
        address.port = port
        return address

    @classmethod
    def ipv6_loopback(cls: 'Address', port: Optional[int] = 3413) -> 'Address':
        """Create IPv6 loopback address.

        Parameters:
        port (int): Port. Default 3413.

        Return:
        Address: Address.

        """

        address = Address()
        address.type = Address.Type.IPV6
        address.values = tuple([0] * 15 + [1])
        address.port = port
        return address

    def host(self: 'Address') -> str:
        """Host part of address in string form.

        Return:
        str: String

        """
        grouping_zeros = False
        can_group_zeros = True
        s = StringIO()

        if self.type == Address.Type.IPV4:
            s.write("{0}.{1}.{2}.{3}".format(self.values[0],
                    self.values[1], self.values[2], self.values[3]))
        elif self.type == Address.Type.IPV6:
            for i in range(8):
                a = self.values[i * 2]
                b = self.values[i * 2 + 1]

                # groups of 0 can be truncated but only once
                if not a and not b:
                    if grouping_zeros:
                        continue
                    elif can_group_zeros:
                        s.write(":")
                        grouping_zeros = True
                        continue
                elif grouping_zeros:
                    grouping_zeros = False
                    can_group_zeros = False

                # leading zeros can be truncated
                if i > 0:
                    s.write(":")
                s.write("{0:x}".format((a << 8) | b))

            if grouping_zeros:
                s.write(":")
        else:
            raise Exception("invald type")
        s.seek(0)
        return s.read()

    def __repr__(self: 'Address') -> str:
        """Representing string (object information).

        Return:
        str: String

        """
        return "Address({0}, {1}, {2})".format(
            self.type, self.host(), self.port)

    def __str__(self: 'Address') -> str:
        """Readable string.

        Return:
        str: String

        """
        if self.type == Address.Type.IPV4:
            return "{0}:{1}".format(self.host(), self.port)
        elif self.type == Address.Type.IPV6:
            return "[{0}]:{1}".format(self.host(), self.port)
        else:
            raise Exception("invald type")

    def __eq__(self: 'Address', other: 'Address') -> bool:
        """Equals.

        Parameters:
        other (Address): Object to compare against.

        Return:
        bool: Result

        """
        if isinstance(other, Address):
            return (self.type == other.type and self.values == other.values
                    and self.port == other.port)
        return NotImplemented

    def __ne__(self: 'Address',  other: 'Address') -> bool:
        """Not equal.

        Parameters:
        other (Vector3): vector to compare against.

        Return:
        bool: Result.

        """
        return (self.type != other.type or self.values != other.values
                or self.port != other.port)
