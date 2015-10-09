Documentation for gregorio and gregoriotex
==========================================

This folder contains the source files of a manual is primarily intended for
developers. Users should not expect to find an indepth guide on using gregorio.

You can find a compiled PDF version in the files of each [Gregorio release](https://github.com/gregorio-project/gregorio/releases).

Developers can use this manual as a reference for information on the
internal workings of gregorio.

## Building

To compile the pdf you must have
 * an up-to-date [TeXLive](https://www.tug.org/texlive/) system, with at least the `luatex` bundle and `latexmk`
 * the [Linux Libertine](http://www.linuxlibertine.org/index.php?id=1&L=1) and [Inconsolata](http://www.levien.com/type/myfonts/inconsolata.html) fonts (distributed by TeXLive too)
 * the [`pygments` library](http://pygments.org/)

Once you have run `./configure` in the main directory, run `make doc` in this directory.
