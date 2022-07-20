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

from typing import Optional


class Point2:

    """Immutable integer 2 component point."""

    def __init__(self: 'Point2',
                 x: Optional[int] = 0,
                 y: Optional[int] = 0) -> None:
        """Create point."""

        self.x = x
        """X component."""

        self.y = y
        """Y component."""

    def __eq__(self: 'Point2', other: 'Point2') -> bool:
        """Equals.

        Parameters:
        other (Point2): Object to compare against.

        Return:
        bool: Result

        """
        if isinstance(other, Point2):
            return (self.x, self.y) == (other.x, other.y)
        return NotImplemented

    def __ne__(self: 'Point2',  other: 'Point2') -> bool:
        """Not equal.

        Parameters:
        other (Point2): Point to compare against.

        Return:
        bool: Result.

        """
        return (self.x, self.y) != (other.x, other.y)

    def __lt__(self: 'Point2',  other: 'Point2') -> bool:
        """Less than.

        Parameters:
        other (Point2): Point to compare against.

        Return:
        bool: Result.

        """
        return self.x < other.x and self.y < other.y

    def __le__(self: 'Point2', other: 'Point2') -> bool:
        """Less than or equal.

        Parameters:
        other (Point2): Point to compare against.

        Return:
        bool: Result.

        """
        return self.x <= other.x and self.y <= other.y

    def __gt__(self: 'Point2', other: 'Point2') -> bool:
        """Greater than.

        Parameters:
        other (Point2): Point to compare against.

        Return:
        bool: Result.

        """
        return self.x > other.x and self.y > other.y

    def __ge__(self: 'Point2', other: 'Point2') -> bool:
        """Greater than or equal.

        Parameters:
        other (Point2): Point to compare against.

        Return:
        bool: Result.

        """
        return self.x >= other.x and self.y >= other.y

    def __add__(self: 'Point2', other: 'Point2') -> 'Point2':
        """Add.

        Parameters:
        other (Point2): Point to add.

        Return:
        Point2: Result.

        """
        return Point2(self.x + other.x, self.y + other.y)

    def __sub__(self: 'Point2', other: 'Point2') -> 'Point2':
        """Subtract.

        Parameters:
        other (Point2): Point to subtract.

        Return:
        Point2: Result.

        """
        return Point2(self.x - other.x, self.y - other.y)

    def __mul__(self: 'Point2', scale: float) -> 'Point2':
        """Multiply.

        Parameters:
        scale (float): Scale factor.

        Return:
        Point2: Result.

        """
        return Point2(self.x * scale, self.y * scale)

    def __div__(self: 'Point2', divisor: float) -> 'Point2':
        """Multiply.

        Parameters:
        divisor (float): Division factor.

        Return:
        Point2: Result.

        """
        return Point2(self.x / divisor, self.y / divisor)

    def __neg__(self: 'Point2') -> 'Point2':
        """Negate.

        Return:
        Point2: Result.

        """
        return Point2(-self.x, -self.y)

    def __hash__(self: 'Point2') -> int:
        """Hash.

        Return:
        int: Hash

        """
        return hash((self.x, self.y))

    def __repr__(self: 'Point2') -> str:
        """Representing string (object information).

        Return:
        str: String

        """
        return "Point2({0},{1})".format(self.x, self.y)

    def __str__(self: 'Point2') -> str:
        """Readable string.

        Return:
        str: String

        """
        return "Point2({0},{1})".format(self.x, self.y)

    def __getitem__(self: 'Point2', key: int) -> int:
        """Get component.

        Parameters:
        key (int): Index of component to return (0=x, 1=y).

        Return:
        int: Component value

        """
        if key == 0:
            return self.x
        elif key == 1:
            return self.y
        else:
            raise KeyError()

    def __setitem__(self: 'Point2', key: int,  value: int) -> None:
        """Set component.

        Parameters:
        key (int): Index of component to modify (0=x, 1=y).
        value (int): Value to set.

        """
        if key == 0:
            self.x = value
        elif key == 1:
            self.y = value
        else:
            raise KeyError()
