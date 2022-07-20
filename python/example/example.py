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

# Import Drag[en]gine Network Library python module. It is recommended
# to use an alias name for the imported module to simplify usage
import DENetworkLibrary as dnl

print('Example Project')

c = dnl.Connection()
v = dnl.math.HalfFloat.float_to_half(8.5)
print('0x{0:x}'.format(v))
print('{0}'.format(dnl.math.HalfFloat.half_to_float(v)))

print(dnl.math.Point2(8, 5))
print(dnl.math.Point2())
print(dnl.math.Point2(5, 2) < dnl.math.Point2(1, 1) + dnl.math.Point2(6, 2))

print(dnl.math.Point3(8, 5,  7))
print(dnl.math.Point3())
print(dnl.math.Point3(5, 2, 3) < dnl.math.Point3(1, 1, 2) + dnl.math.Point3(6, 2, 2))

print(dnl.math.Vector2(8, 5))
print(dnl.math.Vector2())
print(dnl.math.Vector2(5, 2) < dnl.math.Vector2(1, 1) + dnl.math.Vector2(6, 2))

print(dnl.math.Vector3(8, 5, 7))
print(dnl.math.Vector3())
print(dnl.math.Vector3(5, 2, 3) < dnl.math.Vector3(1, 1, 2) + dnl.math.Vector3(6, 2, 2))

print(dnl.math.Quaternion(8, 5, 7))
print(dnl.math.Quaternion())
print(dnl.math.Quaternion(5, 2, 3) < dnl.math.Quaternion(1, 1, 2) + dnl.math.Quaternion(6, 2, 2))

message = dnl.message.Message()
with dnl.message.MessageWriter(message) as w:
    w.write_byte(8)
    w.write_float(2.5)
    w.write_vector3(dnl.math.Vector3(1.5,  8,  6.8))
    w.write_string8('hello world')

with dnl.message.MessageReader(message) as r:
    print(r.read_byte())
    print(r.read_float())
    print(r.read_vector3())
    print(r.read_string8())

"""
from DENetworkLibrary.endpoint.ifaddr import get_adapters
for x in get_adapters():
    print(x)
"""

class TestListener(dnl.endpoint.Endpoint.Listener):
    def __init__(self):
        dnl.endpoint.Endpoint.Listener.__init__(self)
    
    def received_datagram(self, address, message):
        print("Received: '{0}'".format(message.data))

ep = dnl.endpoint.SocketEndpoint()
ep.open(dnl.endpoint.Address.ipv4_loopback(), TestListener())
ep.dispose()
