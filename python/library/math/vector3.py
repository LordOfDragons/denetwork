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


class Vector3:

    """Immutable integer 2 component vector."""

    def __init__(self: 'Vector3',
                 x: Optional[float] = 0.0,
                 y: Optional[float] = 0.0,
                 z: Optional[float] = 0.0) -> None:
        """Create vector."""

        self.x = x
        """X component."""

        self.y = y
        """Y component."""

        self.z = z
        """Z component."""

    def equals(self: 'Vector3',
               other: 'Vector3',
               threshold: float = 1e-15) -> bool:
        """Equals.

        Parameters:
        other (Vector3): Object to compare against.
        threshold (float): Equality threshold

        Return:
        bool: Result

        """
        return (fabs(self.x - other.x) <= threshold
                and fabs(self.y - other.y) <= threshold
                and fabs(self.z - other.z) <= threshold)

    def differs(self: 'Vector3',
                other: 'Vector3',
                threshold: float = 1e-15) -> bool:
        """Equals.

        Parameters:
        other (Vector3): Object to compare against.
        threshold (float): Equality threshold

        Return:
        bool: Result

        """
        return (fabs(self.x - other.x) > threshold
                or fabs(self.y - other.y) > threshold
                or fabs(self.z - other.z) > threshold)

    def __eq__(self: 'Vector3', other: 'Vector3') -> bool:
        """Equals.

        Parameters:
        other (Vector3): Object to compare against.

        Return:
        bool: Result

        """
        if isinstance(other, Vector3):
            return self.equals(other,  1e-15)
        return NotImplemented

    def __ne__(self: 'Vector3',  other: 'Vector3') -> bool:
        """Not equal.

        Parameters:
        other (Vector3): vector to compare against.

        Return:
        bool: Result.

        """
        return self.differs(other,  1e-15)

    def __lt__(self: 'Vector3',  other: 'Vector3') -> bool:
        """Less than.

        Parameters:
        other (Vector3): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x < other.x and self.y < other.y and self.z < other.z

    def __le__(self: 'Vector3', other: 'Vector3') -> bool:
        """Less than or equal.

        Parameters:
        other (Vector3): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x <= other.x and self.y <= other.y and self.z <= other.z

    def __gt__(self: 'Vector3', other: 'Vector3') -> bool:
        """Greater than.

        Parameters:
        other (Vector3): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x > other.x and self.y > other.y and self.z > other.z

    def __ge__(self: 'Vector3', other: 'Vector3') -> bool:
        """Greater than or equal.

        Parameters:
        other (Vector3): vector to compare against.

        Return:
        bool: Result.

        """
        return self.x >= other.x and self.y >= other.y and self.z >= other.z

    def __abs__(self: 'Vector3') -> 'Vector3':
        """Absolute.

        Return:
        Vector3: Result.

        """
        return Vector3(fabs(self.x), fabs(self.y), fabs(self.z))

    def __add__(self: 'Vector3', other: 'Vector3') -> 'Vector3':
        """Add.

        Parameters:
        other (Vector3): vector to add.

        Return:
        Vector3: Result.

        """
        return Vector3(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self: 'Vector3', other: 'Vector3') -> 'Vector3':
        """Subtract.

        Parameters:
        other (Vector3): vector to subtract.

        Return:
        Vector3: Result.

        """
        return Vector3(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self: 'Vector3', scale: float) -> 'Vector3':
        """Multiply.

        Parameters:
        scale (float): Scale factor.

        Return:
        Vector3: Result.

        """
        return Vector3(self.x * scale, self.y * scale, self.z * scale)

    def __div__(self: 'Vector3', divisor: float) -> 'Vector3':
        """Multiply.

        Parameters:
        divisor (float): Division factor.

        Return:
        Vector3: Result.

        """
        return Vector3(self.x / divisor, self.y / divisor, self.z / divisor)

    def __neg__(self: 'Vector3') -> 'Vector3':
        """Negate.

        Return:
        Vector3: Result.

        """
        return Vector3(-self.x, -self.y, -self.z)

    def __hash__(self: 'Vector3') -> int:
        """Hash.

        Return:
        int: Hash

        """
        return hash((self.x, self.y, self.z))

    def __repr__(self: 'Vector3') -> str:
        """Representing string (object information).

        Return:
        str: String

        """
        return 'Vector3({0},{1},{2})'.format(self.x, self.y, self.z)

    def __str__(self: 'Vector3') -> str:
        """Readable string.

        Return:
        str: String

        """
        return 'Vector3({0:.3g},{1:.3g},{2:.3g})'.format(
               self.x, self.y, self.z)

    def __getitem__(self: 'Vector3', key: int) -> float:
        """Get component.

        Parameters:
        key (int): Index of component to return (0=x, 1=y, 2=z).

        Return:
        float: Component value

        """
        if key == 0:
            return self.x
        elif key == 1:
            return self.y
        elif key == 2:
            return self.z
        else:
            raise KeyError()

    def __setitem__(self: 'Vector3', key: int,  value: float) -> None:
        """Set component.

        Parameters:
        key (int): Index of component to modify (0=x, 1=y, 2=z).
        value (float): Value to set.

        """
        if key == 0:
            self.x = value
        elif key == 1:
            self.y = value
        elif key == 2:
            self.z = value
        else:
            raise KeyError()
