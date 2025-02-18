# This is a fontforge python script; use fontforge -lang=py -script to run it
# coding=utf-8
# pylint: disable=import-error


"""
    Python fontforge script to convert from fontforge's native sfd
    to a TrueType font (ttf).

    Copyright (C) 2015-2025 The Gregorio Project (see CONTRIBUTORS.md)

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

    This script takes a .sfd file and converts it to a .ttf file.

    Basic use :
        fontforge -script convertsfdtottf.py fontfile
"""


import sys
import getopt
import fontforge


def usage():
    "Prints help message."
    print("""
Python script to convert a fontforge native file (.sfd) to a TrueType font
(.ttf).

Usage:
    fontforge -script convertsfdtottf.py fontfile
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
    if args[0][-3:] == "sfd":
        outputfile = f'{args[0][:-4]}.ttf'
        inputfile = args[0]
    else:
        usage()
        sys.exit(2)
    font = fontforge.open(inputfile)
    font.generate(outputfile)
    font.close()

if __name__ == "__main__":
    main()
