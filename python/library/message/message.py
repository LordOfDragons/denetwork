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

from datetime import datetime, timezone


class Message:

    """Network message."""

    def __init__(self: 'Message') -> None:
        """Create message."""

        self.data = bytearray(0)
        """Message data. Can be longer than actual message length."""

        self.length = 0
        """Length of message. Can be less than data size."""

        self.timestamp = datetime.now(timezone.utc)
        """Message timestamp."""

    def resize(self: 'Message',  size: int) -> None:
        """Set length of message.

        Can be less than data size. If length is larger than current data
        size the data size is increased. The data content is not retained
        if the size is increased.

        """

        if size < 0:
            raise Exception('size < 0')
        self.length = size
        if len(self.data) < size:
            self.data = bytearray(size)
