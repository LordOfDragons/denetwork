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

from server import ExampleServer
from connection import ExampleConnection
from screen import Screen
from input import Input
from logger import Logger
from io import StringIO
import sys
import time
import logging


class ExampleApp:

    """Example Application."""

    def __init__(self: 'ExampleApp') -> None:
        """Create example application."""
        self._requestExit = False
        self._param_listen = None
        self._param_connect = None
        self._connection = None
        self._server = None
        self._screen = Screen()
        self._input = Input()
        self._logger = Logger()
        self.quit = False

    def run(self: 'ExampleApp') -> None:
        """Run example application."""
        if not self._parse_arguments():
            self._print_help()
            return

        self._init_screen()

        try:
            if self._param_listen:
                self._server_listen()
                self._app_loop()
            elif self._param_connect:
                self._client_connect()
                self._app_loop()
            else:
                self._print_help()
        finally:
            if self._connection is not None:
                self._connection.dispose()
            if self._server is not None:
                self._server.dispose()
            self._exit_screen()

    def _parse_arguments(self: 'ExampleApp') -> bool:
        """Parse command line arguments.

        Return:
        bool True to continue application or False to exit.

        """

        args = iter(sys.argv)
        next(args)
        while True:
            arg = next(args, None)
            if not arg:
                break
            if arg.startswith("--listen="):
                self._param_listen = arg[9:]
            elif arg.startswith("--connect="):
                self._param_connect = arg[10:]
            elif arg == "--help":
                self._print_help()
                return False
            else:
                sys.stdout.write("Invalid argument '{0}'".format(arg))
                return False
        return True

    def _print_help(self: 'ExampleApp') -> None:
        """Print help."""
        sys.stdout.write("denetworkpython_example.py [parameters]\n")
        sys.stdout.write("\n")
        sys.stdout.write("parameters:\n")
        sys.stdout.write("\n")
        sys.stdout.write("--listen=address:port\n")
        sys.stdout.write("    listen for clients on address:port\n")
        sys.stdout.write("\n")
        sys.stdout.write("--connect=address:port\n")
        sys.stdout.write("    connect to host on address:port\n")
        sys.stdout.write("\n")
        sys.stdout.write("--help\n")
        sys.stdout.write("    show help screen\n")

    def _init_screen(self: 'ExampleApp') -> None:
        """Init screen."""
        self._screen.clear_screen()
        self._input.nonblock(True)

    def _exit_screen(self: 'ExampleApp') -> None:
        """Exit screen."""
        self._input.nonblock(False)

    def _server_listen(self: 'ExampleApp') -> None:
        """Start server listening."""
        logging.info("Start listening")
        self._server = ExampleServer(self)
        self._server.listen_on(self._param_listen)

    def _client_connect(self: 'ExampleApp') -> None:
        """Connect to server."""
        logging.info("Connect to server")
        self._connection = ExampleConnection(self, False)
        self._connection.connect_to(self._param_connect)

    def _app_loop(self: 'ExampleApp') -> None:
        """Application loop."""
        while not self.quit:
            self._handle_input()
            if self._server is not None:
                self._server.update_time()
            self._draw_screen()
            time.sleep(0.001)

    def _draw_screen(self: 'ExampleApp') -> None:
        """Draw screen."""
        size = self._screen.terminal_size()
        s = StringIO()
        self._screen.screen_top_left(s)

        if self._server is not None:
            self._screen.print_string(s, size.x, "Server Time",
                                      self._server.time, Screen.Color.RED)
            self._screen.print_bar(s, size.x, "Server Bar",
                                   self._server.bar, Screen.Color.RED)

            for c in self._server.connections:
                if c.ready():
                    self._screen.printBar(s, size.x, "Client#{0} Bar".format(
                        c.id), c.bar, Screen.Color.GREEN)

        if self._connection is not None:
            self._screen.print_bar(s, size.x, "Client Bar",
                                   self._connection.bar, Screen.Color.RED)

            if self._connection.has_server_state:
                self._screen.print_string(s, size.x, "Server Time",
                                          self._connection.server_time,
                                          Screen.Color.GREEN)
                self._screen.print_bar(s, size.x, "Server Bar",
                                       self._connection.server_bar,
                                       Screen.Color.GREEN)

            for c in self._connection.other_client_states:
                self._screen.print_bar(s, size.x, "Client#{0} Bar".format(
                    c.id), c.value_bar.value, Screen.Color.CYAN)

        self._screen.foreground_color(s, Screen.Color.WHITE, Screen.Style.BOLD)
        s.write("Logs:{0}\n".format(" " * (size.x - 5)))
        self._screen.reset_colors(s)

        for l in self._logger.buffer:
            s.write("{0}{1}\n".format(l, " " * (size.x - len(l))))

        sys.stdout.write(s.getvalue())

    def _handle_input(self: 'ExampleApp') -> None:
        """Handle input."""
        if not self._input.kbhit():
            return

        c = sys.stdin.read(1)
        if c == 'q':
            self.quit = True
        elif c == '\033':
            sys.stdin.read(1)  # '['
            c = sys.stdin.read(1)
            if c == 'C':  # right arrow
                if self._server is not None:
                    self._server.increment_bar(1)
                elif self._connection is not None:
                    self._connection.increment_bar(1)
            elif c == 'D':  # left arrow
                if self._server is not None:
                    self._server.increment_bar(-1)
                elif self._connection is not None:
                    self._connection.increment_bar(-1)


ExampleApp().run()
sys.exit(0)

'''
import DENetworkLibrary as dnl

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
for x in dnl.math.Vector3(5, 2, 3):
    print(x)

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

"""
class TestListener(dnl.endpoint.Endpoint.Listener):
    def __init__(self, ep):
        dnl.endpoint.Endpoint.Listener.__init__(self)
        self.ep = ep

    def received_datagram(self, address, message):
        with dnl.message.MessageReader(message) as r:
            print("Received: '{0}'".format(r.read_byte()))
        asyncio.get_event_loop().create_task(self.disconnect())

    async def disconnect(self):
        message = dnl.message.Message()
        with dnl.message.MessageWriter(message) as w:
            from DENetworkLibrary.protocol import CommandCodes
            w.write_byte(CommandCodes.CONNECTION_CLOSE.value)
        self.ep.send_datagram(dnl.endpoint.Address.ipv4_loopback(), message)
        self.ep.close()
"""

print("public_address:")
for a in dnl.endpoint.SocketEndpoint.find_public_address():
    print("- {}".format(a))

print("all address:")
for a in dnl.endpoint.SocketEndpoint.find_all_address():
    print("- {}".format(a))

"""
ep = dnl.endpoint.SocketEndpoint()

print("resolve:", ep.resolve_address("google.ch"))

ep.open(dnl.endpoint.Address.ipv4_any(), TestListener(ep))

message = dnl.message.Message()
with dnl.message.MessageWriter(message) as w:
    from DENetworkLibrary.protocol import CommandCodes, Protocols
    w.write_byte(CommandCodes.CONNECTION_REQUEST.value)
    w.write_ushort(1)
    w.write_ushort(Protocols.DENETWORK_PROTOCOL.value)
ep.send_datagram(dnl.endpoint.Address.ipv4_loopback(), message)

import asyncio
while ep._transport:
    asyncio.get_event_loop().run_until_complete(asyncio.sleep(0.01))

print("dispose socket")
ep.dispose()
"""
'''
