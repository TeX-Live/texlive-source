# Copyright (C) 2013 Kevin W. Hamlen
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301, USA.
#
# The latest version of this program can be obtained from
# http://songs.sourceforge.net.

AC_PREREQ(2.69)
AC_INIT(songs,2.17)
AM_INIT_AUTOMAKE([-Wno-portability])
AC_CONFIG_SRCDIR([src/songs/songs.dtx])
AC_CONFIG_HEADER([src/config.h])
AC_ARG_VAR([texmfdir], [Root texmf tree where new packages are installed.
                        [DATADIR/texmf]])
AS_IF([test "x$texmfdir" = x], [texmfdir="${datadir}/texmf"])
AC_ARG_VAR([candir], [Where .can data files (used by songidx) are installed.
                      [DATADIR/songs]])
AS_IF([test "x$candir" = x], [candir="${datadir}/songs"])
AC_ARG_VAR([vimfilesdir], [Prefix for installing Vim support scripts.
	                   [DATADIR/vimfiles]])
AS_IF([test "x$vimfilesdir" = x], [vimfilesdir="${datadir}/vimfiles"])
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_PROG_LATEX
AC_PROG_PDFLATEX
AC_PROG_MAKEINDEX
AC_PROG_DVIPS
AC_PROG_TEXHASH
AC_PROG_KPSEWHICH
AC_CHECK_HEADERS([locale.h stdlib.h stdio.h string.h wchar.h wctype.h])
AC_C_CONST
AC_TYPE_SIZE_T
AC_FUNC_REALLOC
AC_CHECK_FUNCS([setlocale strrchr])
AC_CHECK_TYPES([wchar_t], , ,[
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_WCHAR_H
#include <wchar.h>
#endif
#if HAVE_WCTYPE_H
#include <wctype.h>
#endif
])
AH_TOP([
#ifndef CONFIG_H
#define CONFIG_H
])
AH_BOTTOM([#endif])
AC_CONFIG_FILES([
  Makefile
  src/songs/GNUmakefile
  src/songidx/Makefile
  sample/GNUmakefile
  src/sbdchk/Makefile
  src/vim/Makefile
])
define([COPY1],[AC_CONFIG_COMMANDS([$1],AS_IF([test "$srcdir/$1" -ef "$1"],,[cp "$srcdir/$1" "$1"]))])
define([COPY],[m4_foreach_w([f],$1,[COPY1(f)])])
COPY([
  src/songs/songs.dtx
  sample/chordbook.tex
  sample/lyricbook.tex
  sample/slidebook.tex
  sample/transparencies.tex
  sample/songs.sbd
])
AC_OUTPUT

