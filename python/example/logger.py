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

from typing import List

import logging


class Logger:

    """Logger."""

    def __init__(self: 'Logger') -> None:
        """Create logger."""
        self._buffer = []
        logging.basicConfig(stream=self, level=logging.DEBUG)

    @property
    def buffer(self: 'Logger') -> List[str]:
        """Buffer.

        Return:
        List[str] Buffer.

        """
        return self._buffer

    def log(self: 'Logger', message: str) -> None:
        """Add log message."""
        while len(self._buffer) > 9:
            self._buffer.pop()
        self._buffer.append(message)

    def write(self: 'Logger', data: str) -> None:
        """Write data."""
        if data and data[-1] == "\n":
            data = data[0:-1]
        if data:
            self.log(data)

    def flush(self: 'Logger') -> None:
        """Flush."""
        pass
