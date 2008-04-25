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
# Makefile fragment for synctex.
# It is included in web2c/Makefile.in

synctex_dir = $(srcdir)/synctex

synctex_all_texd = \
		echo "Enabling SyncTeX Support.";\
		echo "s/\#include \"texmfmp.h\"/\#define __SyncTeX__\\" > synctex_sed_command.txt;\
		echo "\#include \"texmfmp.h\"/" >> synctex_sed_command.txt

### SyncTeX support in tex
# list of change files
synctex-tex_ch_srcs_mem_only = \
  $(synctex_dir)/synctex-mem.ch0 \
  $(synctex_dir)/synctex-mem.ch1 \
  $(synctex_dir)/synctex-mem.ch2
synctex-tex_ch_srcs_on = \
  $(synctex-tex_ch_srcs_mem_only) \
  $(synctex_dir)/synctex-rec.ch0 \
  $(synctex_dir)/synctex-rec.ch1 \
  $(synctex_dir)/synctex-rec.ch2
synctex-tex_ch_srcs_off =
synctex-tex.ch:tie tex.ch $(synctex-tex_ch_srcs)
	$(TIE) -c synctex-tex.ch $(srcdir)/tex.web $(synctex-tex_ch_srcs)
# The C sources and headers
synctex-tex_o = synctex.o
synctex.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-tex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex.c: texd.h $(synctex_dir)/synctex.c synctex-tex.h
	sed s/TEX-OR-MF-OR-MP/tex/ $(synctex_dir)/synctex.c >$@
synctex_texd_on = \
	if test -z "`grep __SyncTeX__ texd.h`";\
	then\
		$(synctex_all_texd);\
		sed -f synctex_sed_command.txt texd.h > synctex_texd.h;\
		mv synctex_texd.h texd.h;\
		rm -f synctex_sed_command.txt;\
	fi
synctex_texd_off =
# switchers: _on to enable full synctex support, _off to completely disable synctex, _mem_only to only implement memory management.
synctex-tex_ch_srcs = \
  $(srcdir)/tex.ch \
  $(synctex-tex_ch_srcs_on)
# _on to enable -synctex command line option, _off to disable
synctex_texd = $(synctex_texd_on)

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
synctex_etexd_on = \
	if test -z "`grep __SyncTeX__ etexd.h`";\
	then\
		$(synctex_all_texd);\
		sed -f synctex_sed_command.txt etexd.h > synctex_etexd.h;\
		mv synctex_etexd.h etexd.h;\
		rm -f synctex_sed_command.txt;\
	fi
synctex_etexd_off =
# switchers: _on to enable full synctex support, _off to completely disable synctex, _mem_only to only implement memory management.
synctex-etex_ch_srcs = $(synctex-etex_ch_srcs_on)
# _on to enable -synctex command line option, _off to disable
synctex_etexd = $(synctex_etexd_on)

### SyncTeX support in pdftex
# list of change files
synctex-pdftex_ch_srcs_mem_only = \
  $(synctex_dir)/synctex-mem.ch0 \
  $(synctex_dir)/synctex-mem.ch1 \
  $(synctex_dir)/synctex-e-mem.ch0 \
  $(synctex_dir)/synctex-e-mem.ch1
synctex-pdftex_ch_srcs_on = \
  $(synctex-pdftex_ch_srcs_mem_only) \
  $(synctex_dir)/synctex-rec.ch0 \
  $(synctex_dir)/synctex-rec.ch1 \
  $(synctex_dir)/synctex-e-rec.ch0 \
  $(synctex_dir)/synctex-e-rec.ch1 \
  $(synctex_dir)/synctex-pdf-rec.ch2
synctex-pdftex_ch_srcs_off =
# The C sources and headers
synctex-pdftex_o = synctex-pdf.o
synctex-pdf.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-pdftex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex-pdf.c: pdftexd.h $(synctex_dir)/synctex.c synctex-pdftex.h
	sed s/TEX-OR-MF-OR-MP/pdftex/ $(synctex_dir)/synctex.c >$@
synctex_pdftexd_on = \
	if test -z "`grep __SyncTeX__ pdftexd.h`";\
	then\
		$(synctex_all_texd);\
		sed -f synctex_sed_command.txt pdftexd.h > synctex_pdftexd.h;\
		mv synctex_pdftexd.h pdftexd.h;\
		rm -f synctex_sed_command.txt;\
	fi
synctex_pdftexd_off =
# switchers: _on to enable full synctex support, _off to completely disable synctex, _mem_only to only implement memory management.
synctex-pdftex_ch_srcs = $(synctex-pdftex_ch_srcs_on)
# _on to enable -synctex command line option, _off to disable
synctex_pdftexd = $(synctex_pdftexd_on)

### SyncTeX support in xetex
# list of change files
synctex-xetex_ch_srcs_mem_only = \
  $(synctex_dir)/synctex-mem.ch0 \
  $(synctex_dir)/synctex-e-mem.ch0 \
  $(synctex_dir)/synctex-e-mem.ch1 \
  $(synctex_dir)/synctex-xe-mem.ch2
synctex-xetex_ch_srcs_on = \
  $(synctex-xetex_ch_srcs_mem_only) \
  $(synctex_dir)/synctex-rec.ch0 \
  $(synctex_dir)/synctex-e-rec.ch0 \
  $(synctex_dir)/synctex-xe-rec.ch2
synctex-xetex_ch_srcs_off =
# The C sources and headers
synctex-xetex_o = synctex-xe.o
synctex-xe.h: $(synctex_dir)/synctex.h
	cat $(synctex_dir)/synctex.h >$@
synctex-xetex.h: $(synctex_dir)/$@
	cat $(synctex_dir)/$@ >$@
synctex-xe.c: xetexd.h $(synctex_dir)/synctex.c synctex-xetex.h
	sed s/TEX-OR-MF-OR-MP/xetex/ $(synctex_dir)/synctex.c >$@
synctex_xetexd_on = \
	if test -z "`grep __SyncTeX__ xetexd.h`";\
	then\
		$(synctex_all_texd);\
		sed -f synctex_sed_command.txt xetexd.h > synctex_xetexd.h;\
		mv synctex_xetexd.h xetexd.h;\
		rm -f synctex_sed_command.txt;\
	fi
synctex_xetexd_off =
# switchers: _on to enable full synctex support, _off to completely disable synctex, _mem_only to only implement memory management.
synctex-xetex_ch_srcs = $(synctex-xetex_ch_srcs_on)
# _on to enable -synctex command line option, _off to disable
synctex_xetexd = $(synctex_xetexd_on)

# the synctex tool, this is not yet implemented
synctex_parser.c synctex_parser.h synctex_main.c:
	cat $(synctex_dir)/$@ >$@
synctex:synctex_main.o synctex_parser.o
	$(link_command) synctex_main.o synctex_parser.o

# Cleaning up.
clean:: synctex-clean
synctex-clean:
	-rm -f synctex-*

# end of synctex.mk
