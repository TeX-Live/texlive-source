#!/usr/bin/env python3

# Copyright 2020 Louis Paternault
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""Compile a `.tex` file, executing commands that are set inside the file itself."""

import argparse
import logging
import os
import pathlib
import re
import subprocess
import sys

NAME = "SpiX"
VERSION = "1.1.0"

RE_EMPTY = re.compile("^ *$")
RE_COMMENT = re.compile("^ *%")
RE_COMMAND = re.compile(r"^%\$ ?(.*)$")


class SpixError(Exception):
    """Exception that should be catched and nicely displayed to user."""


def parse_lines(lines):
    """Parse line to find code snippets.

    :param iterable lines: Lines to parte (typically ``open("foo.tex").readlines()``.
    :return: Iterator over snippets (as strings).
    """
    snippet = None
    for line in lines:
        line = line.rstrip("\n")
        if RE_COMMAND.match(line):
            match = RE_COMMAND.match(line)
            if snippet is None:
                snippet = ""
            else:
                snippet += "\n"
            snippet += match.groups()[0]
        elif RE_EMPTY.match(line) or RE_COMMENT.match(line):
            if snippet is not None:
                yield snippet
                snippet = None
        else:
            break
    if snippet is not None:
        yield snippet


def compiletex(filename, *, dryrun=False):
    """Read commands from file, and execute them.

    :param str filename: File to process.
    :param bool dryrun: If ``True``, print commands to run, but do not execute them.
    """
    env = os.environ
    filename = pathlib.Path(filename)
    env["texname"] = filename.name
    env["basename"] = filename.stem

    try:
        with open(filename, errors="ignore") as file:
            for snippet in parse_lines(file.readlines()):
                print(snippet)
                if dryrun:
                    continue

                subprocess.check_call(
                    ["sh", "-c", snippet, NAME, filename.name],
                    cwd=(pathlib.Path.cwd() / filename).parent,
                    env=env,
                )
    except subprocess.CalledProcessError:
        raise SpixError()
    except IsADirectoryError as error:
        raise SpixError(str(error))


def commandline_parser():
    """Return a command line parser.

    :rtype: argparse.ArgumentParser
    """
    parser = argparse.ArgumentParser(
        prog="spix",
        description=(
            "Compile a `.tex` file, "
            "executing commands that are set inside the file itself."
        ),
    )
    parser.add_argument(
        "-n",
        "--dry-run",
        action="store_true",
        help="Print the commands that would be executed, but do not execute them.",
    )
    parser.add_argument(
        "--version",
        help="Show version and exit.",
        action="version",
        version=f"{NAME} {VERSION}",
    )
    parser.add_argument("FILE", nargs=1, help="File to process.")

    return parser


def main():
    """Main function."""
    arguments = commandline_parser().parse_args()

    if os.path.exists(arguments.FILE[0]):
        arguments.FILE = arguments.FILE[0]
    elif os.path.exists(f"{arguments.FILE[0]}.tex"):
        arguments.FILE = f"{arguments.FILE[0]}.tex"
    else:
        logging.error("""File not found: "%s".""", arguments.FILE[0])
        sys.exit(1)

    try:
        compiletex(arguments.FILE, dryrun=arguments.dry_run)
    except SpixError as error:
        if str(error):
            logging.error(error)
        sys.exit(1)


if __name__ == "__main__":
    main()
