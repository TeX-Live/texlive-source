Documentation for gregorio and gregoriotex
==========================================

This folder contains the source files of a manual is primarily intended for
developers. Users should not expect to find an indepth guide on using gregorio.

You can find a compiled PDF version in the files of each [Gregorio release](https://github.com/gregorio-project/gregorio/releases).

Developers can use this manual as a reference for information on the
internal workings of gregorio.

## Building

To compile the pdf you must have
 * an up-to-date [TeX Live](https://www.tug.org/texlive/) system, with at least the `luatex` bundle and `latexmk`
 * the [Linux Libertine](http://www.linuxlibertine.org/index.php?id=1&L=1) and [Inconsolata](http://www.levien.com/type/myfonts/inconsolata.html) fonts (distributed by TeX Live too)
 * the [`pygments` library](http://pygments.org/)

Once you have run `./configure` in the main directory, run `make doc` in this directory.


Developer notes on extending the gregall and grelaon fonts
==========================================================

The glyphs are drawn with cropped pictures of the neumes
from the manuscripts.  The gregall glyphs are coming from
multiple manuscripts with different neume sizes and furthermore
in some manuscripts like Einsiedeln codex 121 even the sizes of
neumes varies between different folios and often even on the same
folio.  Therefore, when loading the images as background images
in fontforge (version used so far is 20120731), they need to be
scaled accordingly.  The easiest way is to measure the height in
pixels of a clivis with episema (cl-) neume from the highest to
the lowest extreme and divide by 42.0 pixels which is corresponding
height of the original cl- background picture.  The result is the
scale factor listed in GregiorioNabcRef.tex.  After loading the
background image, the background layer in fontforge should be scaled
(same in both axis) by the height of the glyph image in pixels
divided by the scale factor computed above and divided by 52.0 (the
pixel height of the cl- background picture).  The cl- background
picture in the gregall font had the height of 1000 fontforge units.
The grelaon font has as a source only a single manuscript with
approximately comparable neume sizes everywhere, so if using as the
source images with heights around 3500-3600 pixels, the scale factor
is just 1 and height 52 pixels should correspond to 1000 fontforge
units.  Each neume should be named in fontforge with the corresponding
nabc string, with - replaced by N, > replaced by B, ! replaced by E
and ~ replaced by T.  After adding any glyph, GregorioNabcRef.tex
should be updated to list the source of the neume and if the corresponding
gregorian piece is not listed yet, also update list of them, if possible
with a reference to Graduale Triplex, Antiphonale Monasticum,
Nocturnale Romanum etc. book pages where the gregorian piece can be
found.
