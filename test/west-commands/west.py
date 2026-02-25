"""Simple west command to print hello world."""

from west.commands import WestCommand


class West(WestCommand):
    def __init__(self):
        super().__init__(
            name="west",
            help="Print hello world to console",
            description="Print hello world to console",
        )

    def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(
            self.name,
            help=self.help,
            description=self.description,
        )
        return parser

    def do_run(self, args, unknown_args):
        print("hello world")
