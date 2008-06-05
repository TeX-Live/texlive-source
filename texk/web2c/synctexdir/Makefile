# Copyright (c) 2008 jerome DOT laurens AT u-bourgogne DOT fr
#
# This file is part of the SyncTeX package.
#
# License:
# --------
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE
#
# Except as contained in this notice, the name of the copyright holder	
# shall not be used in advertising or otherwise to promote the sale,	
# use or other dealings in this Software without prior written	
# authorization from the copyright holder.
#
# Notice:
# -------
# make some tests

srcdir = ..

prepare:
	if ! test -d tests;\
	then\
		mkdir -p tests;\
	fi

tests: prepare
	cd tests;\
	make all
force:

clean:
	rm -Rf tests

tests_dir:
	-mkdir -p tests

texch1:prepare
	tie -m tests/tex1.web ../tex.web ../tex.ch

texch2:texch1
	tie -m tests/tex2.web tests/tex1.web ../synctex/synctex.ch0

texch3:texch2
	tie -m tests/tex3.web tests/tex2.web ../synctex/synctex.ch1

texch: texch3

etexch01:tests_dir
	tie -m tests/etex01.web ../tex.web ../etexdir/tex.ch0

etexch02:etexch01
	tie -m tests/etex02.web tests/etex01.web ../tex.ch

etexch03:etexch02
	tie -m tests/etex03.web tests/etex02.web ../etexdir/tex.ch1

etexch04:etexch03
	tie -m tests/etex04.web tests/etex03.web ../etexdir/tex.ech

etexch05:etexch04
	tie -m tests/etex05.web tests/etex04.web ../synctex/synctex.ch0

etexch06:etexch05
	tie -m tests/etex06.web tests/etex05.web ../synctex/synctex.ch1

etexch0:etexch06

etexch1:tests_dir
	tie -m tests/etex1.web ../tex.web ../etexdir/etex.ch

etexch2:etexch1
	tie -m tests/etex2.web tests/etex1.web ../synctex/synctex.ch0

etexch3:etexch2
	tie -m tests/etex3.web tests/etex2.web ../synctex/synctex.ch1

etexch4:etexch3
	tie -m tests/etex4.web tests/etex3.web ../synctex/synctex-e.ch0

etexch5:etexch4
	tie -m tests/etex5.web tests/etex4.web ../synctex/synctex-e.ch1

etexch:etexch5
	echo "DONE"

etexch-11:tests_dir etex.web
	tie -m tests/etex-11.web tests/etex.web ../synctex/synctex.ch0

etexch-12:etexch-11
	tie -m tests/etex-12.web tests/etex-11.web ../synctex/synctex.ch1

etexch-13:etexch-12
	tie -m tests/etex-13.web tests/etex-12.web ../synctex/synctex-e.ch0

etexch-14:etexch-13
	tie -m tests/etex-14.web tests/etex-13.web ../synctex/synctex-e.ch1

etexch-1:etexch-14
	echo "DONE"

# Generation of the web and ch file.
# 	Sources for etex.web:
etex_web_srcs = $(srcdir)/tex.web \
	$(srcdir)/etexdir/etex.ch \
	$(srcdir)/etexdir/etex.fix
# 	Sources for etex.ch:
etex_ch_srcs = etex.web \
	$(srcdir)/etexdir/tex.ch0 \
	$(srcdir)/tex.ch \
	$(srcdir)/etexdir/tex.ch1 \
	$(srcdir)/etexdir/tex.ech
# 	Rules:
etex.web: prepare $(etex_web_srcs)
	tie -m tests/etex.web $(etex_web_srcs)
etex.ch: prepare $(etex_ch_srcs)
	tie -c tests/etex.ch $(etex_ch_srcs)

xetex_web_srcs = $(srcdir)/tex.web \
  $(srcdir)/etexdir/etex.ch \
  $(srcdir)/etexdir/tex.ch0 \
  $(srcdir)/tex.ch \
  $(srcdir)/etexdir/tex.ch1 \
  $(srcdir)/etexdir/tex.ech \
  ../synctex/synctex.ch0 \
  ../synctex/synctex-e.ch0 \
  ../synctex/synctex-xetex.ch1 \
  $(srcdir)/xetexdir/xetex.ch

xetex.web: prepare $(xetex_web_srcs)
	tie -m tests/xetex.web $(xetex_web_srcs)

xetex1: xetex.web
	tie -m tests/xetex1.web tests/xetex.web ../synctex/synctex.ch0

xetex2:xetex1
	tie -m tests/xetex2.web tests/xetex1.web ../synctex/synctex-e.ch0

xetex3:xetex2
	tie -m tests/xetex3.web tests/xetex2.web ../synctex/synctex-xetex.ch1

xetex:xetex3
	echo "DONE"

pdftex1: prepare
	pwd
	ls
	tie -m tests/pdftex-1.web ../pdftexdir/pdftex.web ../pdftexdir/tex.ch0

pdftex2: prepare
	tie -m tests/pdftex-2.web tests/pdftex-1.web ../tex.ch

pdftex3: prepare
	tie -m tests/pdftex-3.web tests/pdftex-2.web ../pdftexdir/pdftex.ch

pdftex:pdftex3
