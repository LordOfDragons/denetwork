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

import DENetworkLibrary as dnl
from enum import IntEnum
import sys
from typing import TextIO
import termios
import fcntl
import os
import struct


class Screen:

    """Screen."""

    class Color(IntEnum):

        """Color."""

        BLACK = 0
        """Black."""

        RED = 1
        """Red."""

        GREEN = 2
        """Green."""

        YELLOW = 3
        """Yellow."""

        BLUE = 4
        """Blue."""

        MAGENTA = 5
        """Magenta."""

        CYAN = 6
        """Cyan."""

        WHITE = 7
        """White."""

    class Style(IntEnum):

        """Style."""

        REGULAR = 0
        """Regular."""

        BOLD = 1
        """Bold."""

        LOW = 2
        """Low."""

        ITALIC = 3
        """Italic."""

        UNDERLINE = 4
        """Underline."""

        BLINKING = 5
        """Blinking."""

        REVERSE = 6
        """Reverse."""

        BACKGROUND = 7
        """Background."""

        INVISIBLE = 8
        """Invisible."""

    def __init__(self: 'Screen') -> None:
        """Create screen."""

    def clear_screen(self: 'Screen') -> None:
        """Clear screen."""
        sys.stdout.write("\x1B[2J")

    def screen_top_left(self: 'Screen', stream: TextIO) -> None:
        """Move to screen top left position."""
        stream.write("\x1B[H")

    def reset_colors(self: 'Screen', stream: TextIO) -> None:
        """Reset colors."""
        stream.write("\033[0m")

    def foreground_color(self: 'Screen', stream: TextIO,
                         color: 'Screen.Color', style: 'Screen.Style') -> None:
        """Set foreground color."""
        stream.write("\033[{0:d};{1:d}m".format(style, 30 + color))

    def background_color(self: 'Screen', stream: TextIO,
                         color: 'Screen.Color', style: 'Screen.Style') -> None:
        """Set background color."""
        stream.write("\033[{0:d};{1:d}m".format(style, 40 + color))

    def print_bar(self: 'Screen', stream: TextIO, line_width: int,
                  title: str, length: int, color: 'Screen.Color') -> None:
        """Print bar."""
        tlen = len(title)

        self.reset_colors(stream)
        self.foreground_color(stream, Screen.Color.WHITE, Screen.Style.BOLD)
        stream.write(" " * (14 - tlen))
        stream.write(title)
        stream.write(" [")

        self.reset_colors(stream)
        self.foreground_color(stream, color, Screen.Style.BOLD)
        stream.write("\u2593" * length)

        self.reset_colors(stream)
        self.foreground_color(stream, color, Screen.Style.LOW)
        stream.write("\u2591" * (60 - length))

        self.reset_colors(stream)
        self.foreground_color(stream, Screen.Color.WHITE, Screen.Style.BOLD)
        stream.write("]")
        stream.write(" " * (line_width - 77))
        stream.write("\n")

    def print_string(self: 'Screen', stream: TextIO, line_width: int,
                     title: str, value: str, color: 'Screen.Color') -> None:
        """Print value."""
        tlen = len(title)

        self.reset_colors(stream)
        self.foreground_color(stream, Screen.Color.WHITE, Screen.Style.BOLD)
        stream.write(" " * (14 - tlen))
        stream.write(title)
        stream.write(" '")

        self.reset_colors(stream)
        self.foreground_color(stream, color, Screen.Style.BOLD)
        stream.write(value)

        self.reset_colors(stream)
        self.foreground_color(stream, Screen.Color.WHITE, Screen.Style.BOLD)
        stream.write("'")
        stream.write(" " * (line_width - 17 - len(value)))
        stream.write("\n")

    def terminal_size(self: 'Screen') -> dnl.math.Point2:
        """Get terminal size.

        Return:
        Point2 Terminal size.

        """
        with open(os.ctermid(), 'r') as fd:
            packed = fcntl.ioctl(fd, termios.TIOCGWINSZ,
                                 struct.pack('HHHH', 0, 0, 0, 0))
            rows, cols, h_pixels, v_pixels = struct.unpack('HHHH', packed)
            return dnl.math.Point2(cols,  rows)
