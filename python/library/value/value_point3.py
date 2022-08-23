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

from ..protocol import ValueTypes
from ..message.reader import MessageReader
from ..message.writer import MessageWriter
from ..math.point3 import Point3
from .value import Value
from .value_int import ValueInt


class ValuePoint3(Value):

    """3 component integer vector value."""

    def __init__(self: 'ValuePoint3',
                 value_format: 'ValueInt.Format') -> None:
        """Create integer value.

        Parameters:
        format (ValueInt.Format): Format of value.

        """
        value_types = [ValueTypes.Point3S8,
                       ValueTypes.Point3U8,
                       ValueTypes.Point3S16,
                       ValueTypes.Point3U16,
                       ValueTypes.Point3S32,
                       ValueTypes.Point3U32,
                       ValueTypes.Point3S64,
                       ValueTypes.Point3U64]
        Value.__init__(self, type, value_types[value_format.value])

        self._format = value_format
        self._value = Point3()
        self._last_value = self._value

    @property
    def value(self: 'ValuePoint3') -> Point3:
        """Value.

        Return:
        Point3: Value.

        """
        return self._value

    @value.setter
    def value(self: 'ValuePoint3', value: Point3) -> None:
        """Set value.

        Parameters:
        value (Point3): Value.

        """
        self._value = value
        self._value_changed()

    def read(self: 'ValuePoint3', reader: MessageReader) -> None:
        """Read value from message.

        Parameters:
        reader (MessageReader): Message reader.

        """
        if self._format == ValueInt.Format.SINT8:
            x = reader.read_char()
            y = reader.read_char()
            z = reader.read_char()
        elif self._format == ValueInt.Format.UINT8:
            x = reader.read_byte()
            y = reader.read_byte()
            z = reader.read_byte()
        elif self._format == ValueInt.Format.SINT16:
            x = reader.read_short()
            y = reader.read_short()
            z = reader.read_short()
        elif self._format == ValueInt.Format.UINT16:
            x = reader.read_ushort()
            y = reader.read_ushort()
            z = reader.read_ushort()
        elif self._format == ValueInt.Format.SINT32:
            x = reader.read_int()
            y = reader.read_int()
            z = reader.read_int()
        elif self._format == ValueInt.Format.UINT32:
            x = reader.read_uint()
            y = reader.read_uint()
            z = reader.read_uint()
        elif self._format == ValueInt.Format.SINT64:
            x = reader.read_long()
            y = reader.read_long()
            z = reader.read_long()
        elif self._format == ValueInt.Format.UINT64:
            x = reader.read_ulong()
            y = reader.read_ulong()
            z = reader.read_ulong()
        self._last_value = Point3(x, y,  z)
        self._value = self._last_value

    def write(self: 'ValuePoint3', writer: MessageWriter) -> None:
        """Write value to message.

        Parameters:
        writer (MessageWriter): Message writer.

        """
        if self._format == ValueInt.Format.SINT8:
            writer.write_char(self._value.x)
            writer.write_char(self._value.y)
            writer.write_char(self._value.z)
        elif self._format == ValueInt.Format.UINT8:
            writer.write_byte(self._value.x)
            writer.write_byte(self._value.y)
            writer.write_byte(self._value.z)
        elif self._format == ValueInt.Format.SINT16:
            writer.write_short(self._value.x)
            writer.write_short(self._value.y)
            writer.write_short(self._value.z)
        elif self._format == ValueInt.Format.UINT16:
            writer.write_ushort(self._value.x)
            writer.write_ushort(self._value.y)
            writer.write_ushort(self._value.z)
        elif self._format == ValueInt.Format.SINT32:
            writer.write_int(self._value.x)
            writer.write_int(self._value.y)
            writer.write_int(self._value.z)
        elif self._format == ValueInt.Format.UINT32:
            writer.write_uint(self._value.x)
            writer.write_uint(self._value.y)
            writer.write_uint(self._value.z)
        elif self._format == ValueInt.Format.SINT64:
            writer.write_long(self._value.x)
            writer.write_long(self._value.y)
            writer.write_long(self._value.z)
        elif self._format == ValueInt.Format.UINT64:
            writer.write_ulong(self._value.x)
            writer.write_ulong(self._value.y)
            writer.write_ulong(self._value.z)

    def update_value(self: 'ValuePoint3', force: bool) -> bool:
        """Update value.

        Returns true if value needs to by synchronized otherwise false if
        not changed enough.

        Parameters:
        force (bool): Force updating value even if not changed.

        Return:
        bool: True if value change or force is True.

        """
        if not force and self._value == self._last_value:
            return False
        else:
            self._last_value = self._value
            return True
