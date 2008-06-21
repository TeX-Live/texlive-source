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
# Acknowledgments:
# ----------------
# The author received useful remarks from the pdfTeX developers, especially Hahn The Thanh,
# and significant help from XeTeX developer Jonathan Kew
# 
# Nota Bene:
# ----------
# If you include or use a significant part of the synctex package into a software,
# I would appreciate to be listed as contributor and see "SyncTeX" highlighted.
# 
# Version 1
# Thu Jun 19 09:39:21 UTC 2008
# 
# Notice:
# -------
# Makefile fragment for synctex.
# It is included in web2c/Makefile.in

synctex_dir = $(srcdir)/synctexdir

Makefile: $(synctex_dir)/synctex.mk

# for all the engines, this trick allows to enable or disable SyncTeX support from here.
# One of the synctex functions is called before the main_control routine
# this is why we must include synctex-common.h header
synctex_common_texd = \
	echo "Enabling SyncTeX Support.";\
	echo "s|\#include \"texmfmp.h\"|&\\"> synctex_sed_command.txt;\
	echo "/* Start of SyncTeX Section */\\" >> synctex_sed_command.txt;\
	echo "\#define __SyncTeX__\\" >> synctex_sed_command.txt;\
	echo "\#include \"$(synctex_dir)/synctex-common.h\"\\" >> synctex_sed_command.txt;\
	echo "/* End  of SyncTeX Section */|g" >> synctex_sed_command.txt

### SyncTeX support in tex
# list of change files
tex_ch_srcs-with_synctex-mem_only = \
	$(synctex_dir)/synctex-mem.ch0 \
	$(synctex_dir)/synctex-mem.ch1 \
	$(synctex_dir)/synctex-mem.ch2
tex_ch_srcs-without_synctex = \
	$(tex_ch_srcs-with_synctex-mem_only) \
	$(synctex_dir)/synctex-rec.ch0 \
	$(synctex_dir)/synctex-rec.ch1 \
	$(synctex_dir)/synctex-rec.ch2
tex_ch_srcs-without_synctex =
synctex-tex.ch:tie tex.ch $(synctex-tex_ch_srcs)
	$(TIE) -c synctex-tex.ch $(srcdir)/tex.web $(synctex-tex_ch_srcs)
# The C sources and headers
synctex-tex_o = synctex.o
synctex.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-tex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex.c: texd.h $(synctex_dir)/synctex.c synctex.h synctex-tex.h
	sed s/TEX-OR-MF-OR-MP/tex/ $(synctex_dir)/synctex.c >$@
texd.h-with_synctex = if test -z "`grep __SyncTeX__ texd.h`";\
	then\
		$(synctex_common_texd);\
		sed -f synctex_sed_command.txt texd.h > synctex_texd.h;\
		mv synctex_texd.h texd.h;\
		if test -z "`grep __SyncTeX__ texd.h`";\
		then\
			echo "warning: SyncTeX activation FAILED";\
			exit 1;\
		fi;\
		echo "warning: SyncTeX is enabled";\
	fi
texd.h-without_synctex = echo "warning: SyncTeX is NOT enabled"

### SyncTeX support in etex
# list of change files
synctex-etex_ch_srcs_mem_only = \
	$(synctex_dir)/synctex-mem.ch0 \
	$(synctex_dir)/synctex-mem.ch1 \
	$(synctex_dir)/synctex-e-mem.ch0 \
	$(synctex_dir)/synctex-e-mem.ch1
synctex-etex_ch_srcs_on = \
	$(synctex-etex_ch_srcs_mem_only) \
	$(synctex_dir)/synctex-rec.ch0 \
	$(synctex_dir)/synctex-rec.ch1 \
	$(synctex_dir)/synctex-e-rec.ch0 \
	$(synctex_dir)/synctex-e-rec.ch1
synctex-etex_ch_srcs_off =
# The C sources and headers
synctex-etex_o = synctex-e.o
synctex-e.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-etex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex-e.c: etexd.h $(synctex_dir)/synctex.c synctex-etex.h
	sed s/TEX-OR-MF-OR-MP/etex/ $(synctex_dir)/synctex.c >$@
etexd.h-with_synctex = if test -z "`grep __SyncTeX__ etexd.h`";\
	then\
		$(synctex_common_texd);\
		sed -f synctex_sed_command.txt etexd.h > synctex_etexd.h;\
		mv synctex_etexd.h etexd.h;\
		if test -z "`grep __SyncTeX__ etexd.h`";\
		then\
			echo "warning: SyncTeX activation FAILED";\
			exit 1;\
		fi;\
		echo "warning: SyncTeX is enabled";\
	fi
etexd.h-without_synctex = echo "warning: SyncTeX is NOT enabled"

### SyncTeX support in pdftex
# list of change files
pdftex_ch_srcs-with_synctex-mem_only = \
	$(synctex_dir)/synctex-mem.ch0 \
	$(synctex_dir)/synctex-mem.ch1 \
	$(synctex_dir)/synctex-e-mem.ch0 \
	$(synctex_dir)/synctex-e-mem.ch1
pdftex_ch_srcs-with_synctex = \
	$(pdftex_ch_srcs-with_synctex-mem_only) \
	$(synctex_dir)/synctex-rec.ch0 \
	$(synctex_dir)/synctex-rec.ch1 \
	$(synctex_dir)/synctex-e-rec.ch0 \
	$(synctex_dir)/synctex-e-rec.ch1 \
	$(synctex_dir)/synctex-pdf-rec.ch2
pdftex_ch_srcs-without_synctex =
# The C sources and headers
pdftex_o-with_synctex = synctex-pdf.o
pdftex_o-without_synctex = 
synctex-pdf.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-pdftex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex-pdf.c: pdftexd.h $(synctex_dir)/synctex.c synctex-pdftex.h
	sed s/TEX-OR-MF-OR-MP/pdftex/ $(synctex_dir)/synctex.c >$@
pdftexd.h-with_synctex = if test -z "`grep __SyncTeX__ pdftexd.h`";\
	then\
		$(synctex_common_texd);\
		sed -f synctex_sed_command.txt pdftexd.h > synctex_pdftexd.h;\
		mv synctex_pdftexd.h pdftexd.h;\
		if test -z "`grep __SyncTeX__ pdftexd.h`";\
		then\
			echo "warning: SyncTeX activation FAILED";\
			exit 1;\
		fi;\
		echo "warning: SyncTeX is enabled";\
	fi
pdftexd.h-without_synctex = echo "warning: SyncTeX is NOT enabled"

### SyncTeX support in xetex
# Actually, xetex cannot build without SyncTeX because the tex.web
# code does not provide proper entry points
# list of change files
xetex_ch_srcs-with_synctex-mem_only = \
	$(synctex_dir)/synctex-mem.ch0 \
	$(synctex_dir)/synctex-e-mem.ch0 \
	$(synctex_dir)/synctex-e-mem.ch1 \
	$(synctex_dir)/synctex-xe-mem.ch2
xetex_ch_srcs-with_synctex = \
	$(xetex_ch_srcs-with_synctex-mem_only) \
	$(synctex_dir)/synctex-rec.ch0 \
	$(synctex_dir)/synctex-e-rec.ch0 \
	$(synctex_dir)/synctex-xe-rec.ch2
xetex_ch_srcs-without_synctex =
xetex_post_ch_srcs-with_synctex = \
	$(synctex_dir)/synctex-xe-rec.ch3
xetex_post_ch_srcs-without_synctex =
# The C sources and headers
xetex_o-with_synctex = synctex-xe.o
xetex_o-without_synctex = 
synctex-xe.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-xetex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex-xe.c: xetexd.h $(synctex_dir)/synctex.c synctex-xetex.h
	sed s/TEX-OR-MF-OR-MP/xetex/ $(synctex_dir)/synctex.c >$@
xetexd.h-with_synctex = if test -z "`grep __SyncTeX__ xetexd.h`";\
	then\
		$(synctex_common_texd);\
		sed -f synctex_sed_command.txt xetexd.h > synctex_xetexd.h;\
		mv synctex_xetexd.h xetexd.h;\
		if test -z "`grep __SyncTeX__ xetexd.h`";\
		then\
			echo "warning: SyncTeX activation FAILED";\
			exit 1;\
		fi;\
		echo "warning: SyncTeX is enabled";\
	fi
xetexd.h-without_synctex = echo "warning: SyncTeX is NOT enabled"

# the synctex tool
synctex_parser.o: $(synctex_dir)/synctex_parser.c $(synctex_dir)/synctex_parser.h
	$(compile) -c -DSYNCTEX_USE_LOCAL_HEADER -I$(synctex_dir) $(ZLIBCPPFLAGS) -o $@ $<

synctex_main.o: $(synctex_dir)/synctex_main.c $(synctex_dir)/synctex_parser.h
	$(compile) -c -I$(synctex_dir) $(ZLIBCPPFLAGS) -o $@ $<

synctex:synctex_main.o synctex_parser.o $(ZLIBDEPS)
	$(link_command) synctex_main.o synctex_parser.o $(LDZLIB) $(LIBS)

# Cleaning up.
clean:: synctex-clean
synctex-clean:
	-rm -f *synctex*

# end of synctex.mk
