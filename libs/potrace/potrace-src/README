
	   POTRACE - transform bitmaps into vector graphics
			
				* * *
	   
DESCRIPTION

 Potrace is a tool for tracing a bitmap, which means, transforming a
 bitmap into a smooth, scalable image.  The input is a bitmap (PBM,
 PGM, PPM, or BMP), and the output is one of several vector file
 formats.  A typical use is to create SVG or PDF files from scanned
 data, such as company or university logos, handwritten notes, etc.
 The resulting image is not "jaggy" like a bitmap, but smooth. It can
 then be rendered at any resolution.

 Potrace can currently produce the following output formats: SVG, PDF,
 EPS, PostScript, DXF, GeoJSON, PGM (for easy antialiasing of
 pixel-based images), Gimppath, and XFig. Additional backends might be
 added in the future.

 A separate program, mkbitmap, is also provided. This program can act
 as a pre-processor for Potrace, applying scaling and various filters
 to an image before converting it to a bitmap. This is useful for
 potracing greyscale and color images.

DEPENDENCIES

 Potrace relies on the zlib library for PostScript level 3
 compression. This library is pre-installed on most modern systems. 
 If not present, it can be obtained from http://www.gzip.org/zlib/.

COMPILATION

 For generic compilation instructions, see the file INSTALL.  In
 addition, the following compile time options (options to ./configure)
 are supported. 

  --with-libpotrace       install Potrace library and headers (default: no)
  --disable-zlib          disable PostScript level 3 compression
  --enable-metric         use metric units (centimeters) as default
  --enable-a4             use a4 as the default papersize
  --enable-dumb-tty       use simplified ASCII-only progress bar
  --with-included-getopt  avoid using the system-wide getopt library

 See also ./configure --help.

INSTALLING

 If you are installing from sources, just do "make install" as root.
 If you are installing from a binary distribution, just copy the
 "potrace" and "mkbitmap" binaries to a place where the operating
 system expects them, such as /usr/local/bin. Also copy the files
 "potrace.1" and "mkbitmap.1" to a directory where the operating
 system looks for man pages, such as /usr/local/man/man1. For
 instructions on installing Potrace under Windows, see the file
 README-WIN

USAGE

 For more detailed usage information, see the man page.

 Usage: potrace [options] [filename...]
 General options:
  -h, --help                 - print this help message and exit
  -v, --version              - print version info and exit
  -l, --license              - print license info and exit
 File selection:
  <filename>                 - an input file
  -o, --output <filename>    - write all output to this file
  --                         - end of options; 0 or more input filenames follow
 Backend selection:
  -b, --backend <name>       - select backend by name
  -b svg, -s, --svg          - SVG backend (scalable vector graphics)
  -b pdf                     - PDF backend (portable document format)
  -b pdfpage                 - fixed page-size PDF backend
  -b eps, -e, --eps          - EPS backend (encapsulated PostScript) (default)
  -b ps, -p, --postscript    - PostScript backend
  -b pgm, -g, --pgm          - PGM backend (portable greymap)
  -b dxf                     - DXF backend (drawing interchange format)
  -b geojson                 - GeoJSON backend
  -b gimppath                - Gimppath backend (GNU Gimp)
  -b xfig                    - XFig backend
 Algorithm options:
  -z, --turnpolicy <policy>  - how to resolve ambiguities in path decomposition
  -t, --turdsize <n>         - suppress speckles of up to this size (default 2)
  -a, --alphamax <n>         - corner threshold parameter (default 1)
  -n, --longcurve            - turn off curve optimization
  -O, --opttolerance <n>     - curve optimization tolerance (default 0.2)
  -u, --unit <n>             - quantize output to 1/unit pixels (default 10)
  -d, --debug <n>            - produce debugging output of type n (n=1,2,3)
 Scaling and placement options:
  -P, --pagesize <format>    - page size (default is letter)
  -W, --width <dim>          - width of output image
  -H, --height <dim>         - height of output image
  -r, --resolution <n>[x<n>] - resolution (in dpi) (dimension-based backends)
  -x, --scale <n>[x<n>]      - scaling factor (pixel-based backends)
  -S, --stretch <n>          - yresolution/xresolution
  -A, --rotate <angle>       - rotate counterclockwise by angle
  -M, --margin <dim>         - margin
  -L, --leftmargin <dim>     - left margin
  -R, --rightmargin <dim>    - right margin
  -T, --topmargin <dim>      - top margin
  -B, --bottommargin <dim>   - bottom margin
  --tight                    - remove whitespace around the input image
 Color options, supported by some backends:
  -C, --color #rrggbb        - set foreground color (default black)
  --fillcolor #rrggbb        - set fill color (default transparent)
  --opaque                   - make white shapes opaque
 SVG options:
  --group                    - group related paths together
  --flat                     - whole image as a single path
 Postscript/EPS/PDF options:
  -c, --cleartext            - do not compress the output
  -2, --level2               - use postscript level 2 compression (default)
  -3, --level3               - use postscript level 3 compression
  -q, --longcoding           - do not optimize for file size
 PGM options:
  -G, --gamma <n>            - gamma value for anti-aliasing (default 2.2)
 Frontend options:
  -k, --blacklevel <n>       - black/white cutoff in input file (default 0.5)
  -i, --invert               - invert bitmap
 Progress bar options:
  --progress                 - show progress bar
  --tty <mode>               - progress bar rendering: vt100 or dumb
 
 Dimensions can have optional units, e.g. 6.5in, 15cm, 100pt.
 Default is inches (or pixels for pgm, dxf, and gimppath backends).
 Possible input file formats are: pnm (pbm, pgm, ppm), bmp.
 Backends are: svg, pdf, pdfpage, eps, postscript, ps, dxf, geojson, pgm,
 gimppath, xfig.

POTRACE LIBRARY (FOR DEVELOPERS)

 As of Potrace release 1.6, the Potrace core functionality has been
 separated into a library libpotrace.a. This makes it easy to integrate
 tracing functionality into other GPL programs. The API of the library
 is defined in the header file potracelib.h, and is documented in
 doc/potracelib.pdf. It is not by default installed, but can be
 installed with the --with-libpotrace configuration option.

DOCUMENTATION

 Usage information is contained in the man page:
 http://potrace.sourceforge.net/potrace.html

 Many common questions are answered in the FAQ:
 http://potrace.sourceforge.net/faq.html

 Some examples are given on the following web pages:
 http://potrace.sourceforge.net/samples.html
 http://potrace.sourceforge.net/mkbitmap.html

 The inner workings of the potrace algorithm are explained here:
 http://potrace.sourceforge.net/potrace.pdf

 The potrace library is documented here:
 http://potrace.sourceforge.net/potracelib.pdf

COPYRIGHT

 Copyright (C) 2001-2019 Peter Selinger

 The use of the name "Potrace" is governed by a separate policy, see
 TRADEMARKS below.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or (at
 your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation, 
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. 
 See also http://www.gnu.org/.

 See the file COPYING for details.

TRADEMARKS

 "Potrace" is a trademark of Peter Selinger.

 Policy on using the name "Potrace". If you are planning to modify
 Potrace, including making a port of Potrace to another programming
 language, please be aware of the following. While the source code of
 Potrace is licensed under the GNU General Public License, the name
 "Potrace" is my trademark, and may only be used with my permission.

 In practice, I will permit the use of the name "Potrace" in
 connection with a port of Potrace to another programming language,
 provided that the port complies with the term of the GNU General
 Public License and produces the same high quality vectorized output
 as the original Potrace. Ports that are buggy or do not produce high
 quality output are not permitted to use the name "Potrace". To ensure
 compliance with this policy, please run your port by me before making
 it public. Also, if you use the "Potrace" name (or any derivative,
 such as "MyPotrace"), you must provide a name and email address where
 I can reach you. Derivatives of Potrace that are posted anonymously
 or where the authors cannot be reached or do not reply to email or
 bug reports are not permitted to use the name "Potrace".

 The purpose of this policy is to allow people to port Potrace to as
 many programming languages as possible, while also ensuring that all
 such ports are of the same high quality that has come to be
 associated with the name "Potrace".

GPG KEY

 The key fingerprint of Peter Selinger's GPG key is:
 12A2 4B3C 3790 B688 E484  7A98 A68B CC37 CA31 696A
 This may be useful for authenticating future versions of Potrace.
