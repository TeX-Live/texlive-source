# ps2eps
ps2eps - calculate correct bounding boxes for PostScript and PDF files

## Description

ps2eps is a tool (written in Perl) to produce Encapsulated PostScript
Files (EPS/EPSF) from usual one-paged Postscript documents.  It
calculates correct Bounding Boxes for those EPS files and filters some
special postscript command sequences that can produce erroneous
results on printers. EPS files are often needed for including
(scalable) graphics of high quality into TeX/LaTeX (or even Word)
documents. Included graphics can be clipped to their bounding box.
	
Nowadays PDF is more widely used, but the original problems
described below still exist: pdfcrop sometimes does not 
produce the correct bounding box. This program can be used 
together with pdfcrop in order to produce reliable bounding 
boxes for PDF files, too (see shell script scripts/rbpdfcrop
as an example how to use ps2eps for cropping PDFs – requires
pdfcrop).

Other programs like ps2epsi do not calculate the bounding box 
always correctly (because the values are put on the postscript
stack which may get corrupted by bad postscript code) or rounded 
it off so that clipping the EPS cut off some part of the image.
ps2eps uses a double precision resolution of 144 dpi and appropriate 
rounding to get a proper bounding box. The internal bbox device of 
ghostscript generates different values (sometimes even incorrect), 
so using the provided bbox should be more robust.
However, because normal clipping has only a resolution of 1/72dpi 
(postscript point), the clipping process may still erase parts
of your EPS image. In this case please use the -l option to add 
an additional point of white space around the tight bounding box.

## Documentation

Please see documentation in the manpage or doc/ sub directory.
Note that the manpage may have an older version number in it,
which is nothing to worry about since there may have been minor
changes incorporated into ps2eps not requiring any update of the
documentation.

## Installation

see doc/INSTALL.md
	
## Updates

See the git repository:	https://github.com/roland-bless/ps2eps
   
## License (GNU GPL 2.0)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
