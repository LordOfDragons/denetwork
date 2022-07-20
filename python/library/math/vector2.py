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
from math import fabs


class Vector2:

    """Immutable integer 2 component vector."""

    def __init__(self: 'Vector2',
                 x: Optional[float] = 0.0,
                 y: Optional[float] = 0.0) -> None:
        """Create vector."""

        self.x = x
        """X component."""

        self.y = y
        """Y component."""

    def equals(self: 'Vector2',
               other: 'Vector2',
               threshold: float = 1e-15) -> bool:
        """Equals.

        Parameters:
        other (Vector2): Object to compare against.
        threshold (float): Equality threshold

        Return:
        bool: Result

        """
        return (fabs(self.x - other.x) <= threshold
                and fabs(self.y - other.y) <= threshold)

    def differs(self: 'Vector2',
                other: 'Vector2',
                threshold: float = 1e-15) -> bool:
        """Equals.

        Parameters:
        other (Vector2): Object to compare against.
        threshold (float): Equality threshold

        Return:
        bool: Result

        """
        return (fabs(self.x - other.x) > threshold
                or fabs(self.y - other.y) > threshold)

    def __eq__(self: 'Vector2', other: 'Vector2') -> bool:
        """Equals.

        Parameters:
        other (Vector2): Object to compare against.

        Return:
        bool: Result

        """
        if isinstance(other, Vector2):
            return self.equals(other,  1e-15)
        return NotImplemented

    def __ne__(self: 'Vector2',  other: 'Vector2') -> bool:
        """Not equal.

        Parameters:
        other (Vector2): vector to compare against.

        Return:
        bool: Result.

        """
        return self.differs(other,  1e-15)

    def __lt__(self: 'Vector2',  other: 'Vector2') -> bool:
        """Less than.

        Parameters:
        other (Vector2): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x < other.x and self.y < other.y

    def __le__(self: 'Vector2', other: 'Vector2') -> bool:
        """Less than or equal.

        Parameters:
        other (Vector2): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x <= other.x and self.y <= other.y

    def __gt__(self: 'Vector2', other: 'Vector2') -> bool:
        """Greater than.

        Parameters:
        other (Vector2): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x > other.x and self.y > other.y

    def __ge__(self: 'Vector2', other: 'Vector2') -> bool:
        """Greater than or equal.

        Parameters:
        other (Vector2): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x >= other.x and self.y >= other.y

    def __abs__(self: 'Vector2') -> 'Vector2':
        """Absolute.

        Return:
        Vector2: Result.

        """
        return Vector2(fabs(self.x), fabs(self.y))

    def __add__(self: 'Vector2', other: 'Vector2') -> 'Vector2':
        """Add.

        Parameters:
        other (Vector2): vector to add.

        Return:
        Vector2: Result.

        """
        return Vector2(self.x + other.x, self.y + other.y)

    def __sub__(self: 'Vector2', other: 'Vector2') -> 'Vector2':
        """Subtract.

        Parameters:
        other (Vector2): vector to subtract.

        Return:
        Vector2: Result.

        """
        return Vector2(self.x - other.x, self.y - other.y)

    def __mul__(self: 'Vector2', scale: float) -> 'Vector2':
        """Multiply.

        Parameters:
        scale (float): Scale factor.

        Return:
        Vector2: Result.

        """
        return Vector2(self.x * scale, self.y * scale)

    def __div__(self: 'Vector2', divisor: float) -> 'Vector2':
        """Multiply.

        Parameters:
        divisor (float): Division factor.

        Return:
        Vector2: Result.

        """
        return Vector2(self.x / divisor, self.y / divisor)

    def __neg__(self: 'Vector2') -> 'Vector2':
        """Negate.

        Return:
        Vector2: Result.

        """
        return Vector2(-self.x, -self.y)

    def __hash__(self: 'Vector2') -> int:
        """Hash.

        Return:
        int: Hash

        """
        return hash((self.x, self.y))

    def __repr__(self: 'Vector2') -> str:
        """Representing string (object information).

        Return:
        str: String

        """
        return "Vector2({0},{1})".format(self.x, self.y)

    def __str__(self: 'Vector2') -> str:
        """Readable string.

        Return:
        str: String

        """
        return "Vector2({0:.3g},{1:.3g})".format(self.x, self.y)

    def __getitem__(self: 'Vector2', key: int) -> float:
        """Get component.

        Parameters:
        key (int): Index of component to return (0=x, 1=y).

        Return:
        float: Component value

        """
        if key == 0:
            return self.x
        elif key == 1:
            return self.y
        else:
            raise KeyError()

    def __setitem__(self: 'Vector2', key: int,  value: float) -> None:
        """Set component.

        Parameters:
        key (int): Index of component to modify (0=x, 1=y).
        value (float): Value to set.

        """
        if key == 0:
            self.x = value
        elif key == 1:
            self.y = value
        else:
            raise KeyError()
