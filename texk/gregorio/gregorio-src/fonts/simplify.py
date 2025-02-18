# This is a fontforge python script; use fontforge -lang=py -script to run it
# coding=utf-8
# pylint: disable=import-error


"""
    Python fontforge script to simplify a font. This only exists
    because simplification doesn't work in squarize.py due to bugs
    in fontforge.

    Copyright (C) 2016-2025 The Gregorio Project (see CONTRIBUTORS.md)

    This file is part of Gregorio.

    Gregorio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Gregorio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

    This script takes a font file, simplifies it and overwrites it.

    Basic use :
         fontforge -script simplify.py fontfile
"""


import sys
import getopt
import fontforge


def usage():
    "Prints help message."
    print("""
Python fontforge script to simplify a font.

Usage:
    fontforge -script simplify.py fontfile
""")

def main():
    "Main function."
    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], "h", ["help"])
    except getopt.GetoptError:
        # print help information and exit:
        usage()
        sys.exit(2)
    outputfile = None
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
    if len(args) == 0:
        usage()
        sys.exit(2)
    fontfile = args[0]
    font = fontforge.open(fontfile)
    font.selection.all()
    font.simplify(0.1, ('mergelines','ignoreslopes','setstarttoextremum'))
    font.generate(fontfile)
    font.close()

if __name__ == "__main__":
    main()
