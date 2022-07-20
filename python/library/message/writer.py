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

from .message import Message
from struct import pack
from ..math.vector2 import Vector2
from ..math.vector3 import Vector3
from ..math.quaternion import Quaternion
from ..math.point2 import Point2
from ..math.point3 import Point3


class MessageWriter:

    """Message writer.

    Use this class with context managing like this:

    import DENetworkLibrary as dnl
    message = dnl.message.Message()
    with dnl.message.MessageWriter(message) as w:
        w.write_byte(8)
        w.write_float(2.5)
        w.write_vector3(dnl.math.Vector3(1.5,  8,  6.8))
        w.write_string8('hello world')

    After the context manager exits the message is properly written to
    and can be send. You can use this class also without context managing.
    In this case call close() after you are finished writing to the message.

    """

    def __init__(self: 'MessageWriter',  message: Message) -> None:
        """Create message writer."""

        self.message = message
        self.data = bytearray()

    def write_char(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<b", value)

    def write_byte(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<B", value)

    def write_short(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<h", value)

    def write_ushort(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<H", value)

    def write_int(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<i", value)

    def write_uint(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<I", value)

    def write_long(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<q", value)

    def write_ulong(self: 'MessageWriter', value: int) -> None:
        """Write value.

        Parameters:
        value (int): Value

        """
        self.data += pack("<Q", value)

    def write_float(self: 'MessageWriter', value: float) -> None:
        """Write value.

        Parameters:
        value (float): Value

        """
        self.data += pack("<f", value)

    def write_double(self: 'MessageWriter', value: float) -> None:
        """Write value.

        Parameters:
        value (float): Value

        """
        self.data += pack("<d", value)

    def write_string8(self: 'MessageWriter', value: str) -> None:
        """Write value.

        Parameters:
        value (str): String object to write (utf-8 encoded)

        """
        data = value.encode('utf-8')
        length = len(data)
        if length > 255:
            raise Exception("encoded string too long")
        self.data += pack("<B", length)
        self.data += data

    def write_string16(self: 'MessageWriter', value: str) -> None:
        """Write value.

        Parameters:
        value (str): String object to write (utf-8 encoded)

        """
        data = value.encode('utf-8')
        length = len(data)
        if length > 65535:
            raise Exception("encoded string too long")
        self.data += pack("<H", length)
        self.data += data

    def write_vector2(self: 'MessageWriter', value: Vector2) -> None:
        """Write value.

        Parameters:
        value (Vector2): Value

        """
        self.data += pack("<ff", value.x,  value.y)

    def write_vector3(self: 'MessageWriter', value: Vector3) -> None:
        """Write value.

        Parameters:
        value (Vector3): Value

        """
        self.data += pack("<fff", value.x,  value.y,  value.z)

    def write_quaternion(self: 'MessageWriter', value: Quaternion) -> None:
        """Write value.

        Parameters:
        value (Quaternion): Value

        """
        self.data += pack("<ffff", value.x,  value.y,  value.z,  value.w)

    def write_point2(self: 'MessageWriter', value: Point2) -> None:
        """Write value.

        Parameters:
        value (Point2): Value

        """
        self.data += pack("<ii", value.x,  value.y)

    def write_point3(self: 'MessageWriter', value: Point3) -> None:
        """Write value.

        Parameters:
        value (Point3): Value

        """
        self.data += pack("<iii", value.x,  value.y, value.z)

    def write_dvector(self: 'MessageWriter', value: Vector3) -> None:
        """Write value.

        Parameters:
        value (Vector3): Value

        """
        self.data += pack("<ddd", value.x,  value.y,  value.z)

    def write(self: 'MessageWriter',
              buffer: str,
              offset: int,
              length: int) -> None:
        """Write data.

        Parameters:
        buffer (str): Bytes object to write.
        offset (int): Offset in bytes object to start reading from.
        length (int): Count of bytes to write.

        """
        if offset < 0:
            raise Exception("offset < 0")
        if length < 0:
            raise Exception("length < 0")
        data = buffer[offset:offset + length]
        if len(data) != length:
            raise Exception("length mismatch")
        self.data += data

    def write_message(self: 'MessageWriter', message: Message) -> None:
        """Write message.

        Parameters:
        message (Message): Message to write.

        """
        self.data += message.data

    def close(self: 'MessageWriter') -> None:
        """Close writer updating message."""

        self.message.data = self.data

    def __enter__(self: 'MessageWriter') -> 'MessageWriter':
        """Enter method for context manager support.

        Return:
        MessageWriter: Self.

        """
        return self

    def __exit__(self: 'MessageWriter',
                 exception_type: None,
                 exception_value: None,
                 traceback: None) -> None:
        """Exit method for context manager support."""
        self.close()
