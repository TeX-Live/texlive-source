LCDF Typetools
==============

LCDF Typetools comprises several programs for manipulating PostScript
Type 1, Type 1 Multiple Master, OpenType, and TrueType fonts.

**cfftot1** translates a Compact Font Format (CFF) font, or a
PostScript-flavored OpenType font, into PostScript Type 1 format. It
correctly handles subroutines and hints.

**mmafm** creates an AFM file (font metrics) corresponding to an instance of
a Type 1 Multiple Master font. It reads the AMFM and AFM files distributed
with the font.

**mmpfb** creates a normal, single-master font program which looks like an
instance of a Type 1 Multiple Master font. It reads the multiple master font
program in PFA or PFB format.

**otfinfo** reports information about OpenType and TrueType fonts, such as
the OpenType features and Unicode code points they support, or the contents
of their `size` optical size features.

**otftotfm** creates TeX font metrics and encodings that correspond to an
OpenType or TrueType font. It interprets glyph positionings, substitutions,
and ligatures as far as it is able. You can say which OpenType features
should be activated.

**t1dotlessj** reads a Type 1 font, then creates a new Type 1 font whose
only character is a dotless lower-case j matching the input font’s design.

**t1lint** checks Type 1 fonts for correctness. It tests most of the
requirements listed in Adobe Systems’ Black Book (“Adobe Type 1 Font
Format”), and some others.

**t1rawafm** creates an AFM font metrics file corresponding to a raw Type 1
font file (in PFA or PFB format).

**t1reencode** reencodes a Type 1 font, replacing its internal encoding with
one you specify.

**t1testpage** creates PostScript test pages for a given Type 1 font. These
pages show every character defined in the font.

**ttftotype42** creates a Type 42 wrapper for a TrueType or
TrueType-flavored OpenType font. This allows the font to be embedded in a
PostScript file.

Each of these programs has a manual page; `man PROGRAMNAME/PROGRAMNAME.1`
for more information.

See `NEWS` in this directory for changes in recent versions. The LCDF
Typetools home page is:

http://www.lcdf.org/type/


Installation
------------

Type `./configure`, then `make`.

If `./configure` does not exist (you downloaded from Github), run
`./bootstrap.sh` first.

`./configure` accepts the usual options; see `INSTALL` for details.
Some of the typetools programs can link with additional libraries. Otftotfm
can use the Kpathsea library for integration with TeX directories; if your
version of this library is in a nonstandard place, supply `./configure` with
the `--with-kpathsea=PREFIX` option to find it.

You can also disable individual programs by supplying `./configure` with
`--disable-PROGNAME` options.  See `./configure --help` for more
information.


Mmafm and mmpfb
---------------

Run `mmafm --help` and `mmpfb --help` for a full option summary. Here are
two example runs:

    % mmafm MyriadMM.amfm --weight=300 --width=585 > MyriadMM_300_585_.afm
    % mmpfb MyriadMM.pfb --weight=300 --width=585 > MyriadMM_300_585_.pfb

Mmafm expects the name of an AMFM file on the command line. It also needs
an AFM file for each master (these should have been distributed with the
AMFM file). You can give the AFM files’ names on the command line, along
with the AMFM file, or you let mmafm find the AFM files automatically. For
the automatic method, you must follow one of these 2 conventions:

1. The AFM files are in the same directory as the AMFM file. They are
named `FONTNAME.afm` -- `MyriadMM-LightCn.afm`, for example.

2. There is a `PSres.upr` file that lists the AFMs by font name, and the
`PSRESOURCEPATH` environment variable contains the directory with that
`PSres.upr` file. (`ps2pk` comes with a sample `PSres.upr` file.)


Copyright and license
---------------------

All source code is Copyright (c) 1997-2019 Eddie Kohler.

This code is distributed under the GNU General Public License, Version 2
(and only Version 2). The GNU General Public License is available via the
Web at <http://www.gnu.org/licenses/gpl.html>, or in the COPYING file in
this directory.


Author
------

Eddie Kohler <ekohler@gmail.com>, http://www.lcdf.org/

The current version of the lcdf-typetools package is available on the Web at
http://www.lcdf.org/type/

LCDF stands for Little Cambridgeport Design Factory.
