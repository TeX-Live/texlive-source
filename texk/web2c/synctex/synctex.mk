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

# The C sources and headers
synctex_o = synctex.o
synctex.h: $(top_srcdir)/synctex/synctex.h
	cat $(top_srcdir)/synctex/synctex.h >$@
synctex.c: synctex.h $(top_srcdir)/synctex/synctex.c
	cat $(top_srcdir)/synctex/synctex.c >$@

# Cleaning up.
clean:: synctex-clean
synctex-clean:
	-rm synctex.o synctex.h synctex.c

# end of synctex.mk
