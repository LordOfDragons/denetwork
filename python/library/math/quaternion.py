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


class Quaternion:

    """Immutable integer 4 component quaternion."""

    def __init__(self: 'Quaternion',
                 x: Optional[float] = 0.0,
                 y: Optional[float] = 0.0,
                 z: Optional[float] = 0.0,
                 w: Optional[float] = 1.0) -> None:
        """Create quaternion."""

        self.x = x
        """X component."""

        self.y = y
        """Y component."""

        self.z = z
        """Z component."""

        self.w = w
        """W component."""

    def equals(self: 'Quaternion',
               other: 'Quaternion',
               threshold: float = 1e-15) -> bool:
        """Equals.

        Parameters:
        other (Quaternion): Object to compare against.
        threshold (float): Equality threshold

        Return:
        bool: Result

        """
        return (fabs(self.x - other.x) <= threshold
                and fabs(self.y - other.y) <= threshold
                and fabs(self.z - other.z) <= threshold
                and fabs(self.w - other.w) <= threshold)

    def differs(self: 'Quaternion',
                other: 'Quaternion',
                threshold: float = 1e-15) -> bool:
        """Equals.

        Parameters:
        other (Quaternion): Object to compare against.
        threshold (float): Equality threshold

        Return:
        bool: Result

        """
        return (fabs(self.x - other.x) > threshold
                or fabs(self.y - other.y) > threshold
                or fabs(self.z - other.z) > threshold
                or fabs(self.w - other.w) > threshold)

    def __eq__(self: 'Quaternion', other: 'Quaternion') -> bool:
        """Equals.

        Parameters:
        other (Quaternion): Object to compare against.

        Return:
        bool: Result

        """
        if isinstance(other, Quaternion):
            return self.equals(other,  1e-15)
        return NotImplemented

    def __ne__(self: 'Quaternion',  other: 'Quaternion') -> bool:
        """Not equal.

        Parameters:
        other (Quaternion): quaternion to compare against.

        Return:
        bool: Result.

        """
        return self.differs(other,  1e-15)

    def __lt__(self: 'Quaternion',  other: 'Quaternion') -> bool:
        """Less than.

        Parameters:
        other (Quaternion): quaternion to compare against.

        Return:
        bool: Result.

        """
        return (self.x < other.x and self.y < other.y
                and self.z < other.z and self.w < other.w)

    def __le__(self: 'Quaternion', other: 'Quaternion') -> bool:
        """Less than or equal.

        Parameters:
        other (Quaternion): quaternion to compare against.

        Return:
        bool: Result.

        """
        return (self.x <= other.x and self.y <= other.y
                and self.z <= other.z and self.w <= other.w)

    def __gt__(self: 'Quaternion', other: 'Quaternion') -> bool:
        """Greater than.

        Parameters:
        other (Quaternion): quaternion to compare against.

        Return:
        bool: Result.

        """
        return (self.x > other.x and self.y > other.y
                and self.z > other.z and self.w > other.w)

    def __ge__(self: 'Quaternion', other: 'Quaternion') -> bool:
        """Greater than or equal.

        Parameters:
        other (Quaternion): quaternion to compare against.

        Return:
        bool: Result.

        """
        return (self.x >= other.x and self.y >= other.y
                and self.z >= other.z and self.w >= other.w)

    def __abs__(self: 'Quaternion') -> 'Quaternion':
        """Absolute.

        Return:
        Quaternion: Result.

        """
        return Quaternion(fabs(self.x), fabs(self.y),
                          fabs(self.z), fabs(self.w))

    def __add__(self: 'Quaternion', other: 'Quaternion') -> 'Quaternion':
        """Add.

        Parameters:
        other (Quaternion): quaternion to add.

        Return:
        Quaternion: Result.

        """
        return Quaternion(self.x + other.x, self.y + other.y,
                          self.z + other.z, self.w + other.w)

    def __sub__(self: 'Quaternion', other: 'Quaternion') -> 'Quaternion':
        """Subtract.

        Parameters:
        other (Quaternion): quaternion to subtract.

        Return:
        Quaternion: Result.

        """
        return Quaternion(self.x - other.x, self.y - other.y,
                          self.z - other.z, self.w - other.w)

    def __mul__(self: 'Quaternion', scale: float) -> 'Quaternion':
        """Multiply.

        Parameters:
        scale (float): Scale factor.

        Return:
        Quaternion: Result.

        """
        return Quaternion(self.x * scale, self.y * scale,
                          self.z * scale, self.w * scale)

    def __div__(self: 'Quaternion', divisor: float) -> 'Quaternion':
        """Multiply.

        Parameters:
        divisor (float): Division factor.

        Return:
        Quaternion: Result.

        """
        return Quaternion(self.x / divisor, self.y / divisor,
                          self.z / divisor, self.w / divisor)

    def __neg__(self: 'Quaternion') -> 'Quaternion':
        """Negate.

        Return:
        Quaternion: Result.

        """
        return Quaternion(-self.x, -self.y, -self.z, -self.w)

    def __hash__(self: 'Quaternion') -> int:
        """Hash.

        Return:
        int: Hash

        """
        return hash((self.x, self.y, self.z, self.w))

    def __repr__(self: 'Quaternion') -> str:
        """Representing string (object information).

        Return:
        str: String

        """
        return "Quaternion({0},{1},{2},{3})".format(
               self.x, self.y, self.z, self.w)

    def __str__(self: 'Quaternion') -> str:
        """Readable string.

        Return:
        str: String

        """
        return "Quaternion({0:.3g},{1:.3g},{2:.3g},{3:.3g})".format(
               self.x, self.y, self.z, self.w)

    def __getitem__(self: 'Quaternion', key: int) -> float:
        """Get component.

        Parameters:
        key (int): Index of component to return (0=x, 1=y, 2=z, 3=w).

        Return:
        float: Component value

        """
        if key == 0:
            return self.x
        elif key == 1:
            return self.y
        elif key == 2:
            return self.z
        elif key == 3:
            return self.w
        else:
            raise KeyError()

    def __setitem__(self: 'Quaternion', key: int,  value: float) -> None:
        """Set component.

        Parameters:
        key (int): Index of component to modify (0=x, 1=y, 2=z, 3=w).
        value (float): Value to set.

        """
        if key == 0:
            self.x = value
        elif key == 1:
            self.y = value
        elif key == 2:
            self.z = value
        elif key == 3:
            self.w = value
        else:
            raise KeyError()
