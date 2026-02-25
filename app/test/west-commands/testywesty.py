"""Simple west command to print hello world."""

from west.commands import WestCommand


class Testywesty(WestCommand):
    def __init__(self):
        super().__init__(
            name="testywesty",
            help="Print hello world to console",
            description="Print hello world to console",
        )

    def do_run(self, args, unknown_args):
        print("hello world")
