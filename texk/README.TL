Copyright 2006, 2007 Karl Berry.
You may freely use, modify and/or distribute this file.

This TeX Live directory contains the programs that use the kpathsea
library for configuration and path searching, and the library itself.

Some programs can be built on their own, but others, including kpathsea
itself, cannot.  Instead, try configure --enable-tetex-build at the top
level.  In general, for TeX Live we only ever test configuring the whole
tree.

Upstream sources for the programs here:

afm2pl 0.7.03 - ok 5dec06
  http://tex.aanhet.net/afm2pl/

devnag - from devnag package installed in texmf-dist.

dvipdfmx 20061211 - ok 14dec06
  http://project.ktug.or.kr/dvipdfmx/snapshot/current/
  -> http://www.ctan.org/tex-archive/dviware/dvipdfmx/

dvipng 1.9 - ok 5dec06
  http://www.ctan.org/tex-archive/dviware/dvipng/

dvi2tty 5.3.1 - needs update 18may07
  http://www.mesa.nl/pub/dvi2tty/

xdvik 22.84.10 - ok 4dec06
  http://xdvi.sourceforge.net/
