T1utils
=======

T1utils is a collection of simple programs for manipulating PostScript Type 1
fonts. Together, they allow you to convert between PFA (ASCII) and PFB
(binary) formats, disassemble PFA or PFB files into human-readable form, and
reassemble them into PFA or PFB format. Additionally you can extract font
resources from a Macintosh font file or create a Macintosh Type 1 font file
from a PFA or PFB font.

There are currently six programs:

* **t1ascii**: Converts PFB files to PFA format.

* **t1binary**: Converts PFA files to PFB format.

* **t1disasm**: Disassembles a Type 1 font (PFA or PFB format) into a
  raw, human-readable text form for subsequent hand editing, tweaking, hint
  fixing, and so forth.

* **t1asm**: Assembles the human-readable t1disasm text form into a Type 1
  font in PFA or PFB format.

* **t1unmac**: Extracts POST resources from a Macintosh Type 1 font file into
  PFA or PFB format for use outside the Macintosh environment. The Macintosh
  file should be stored in MacBinary, AppleSingle, AppleDouble, or BinHex
  format, or as a raw resource fork. Note that t1unmac does not have to run on
  a Macintosh.

* **t1mac**: Creates a Macintosh Type 1 file from a PFA or PFB-format Type 1
  font. Writes the Macintosh file in MacBinary, AppleSingle, AppleDouble, or
  BinHex format, or as a raw resource fork. WARNING: This will not suffice to
  use the new font on a Macintosh, as Macintoshes cannot read raw Type 1
  fonts. You will need to create a font suitcase containing bitmap fonts if
  you do not have such a suitcase for the font already. T1utils cannot help
  you do this.

Installation
------------

You need an ANSI C compiler, such as gcc.

Just type `./configure`, then `make`. `make install` will build and install
the utilities and their manual pages.

`./configure` accepts the usual options. See `INSTALL` for more details. The
most commonly used option is `--prefix`, which can be used to install the
utilities in a place other than /usr/local.

Copyright and license
---------------------

The original t1utils were (c) 1992 I. Lee Hetherington, <ilh@lcs.mit.edu>.
Changes since version 1.2 are (c) 1998-2017 Eddie Kohler. Distribution is
under the Click LICENSE, a BSD-like license described in the LICENSE file in
this directory.

Note that these tools should not be used to illegally copy Type 1 font
programs. Typeface design is an intricate art that should be rewarded.

Eddie Kohler, ekohler@gmail.com
