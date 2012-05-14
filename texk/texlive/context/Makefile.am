## Makefile.am for the TeX Live subdirectory texk/texlive/context/
##
## Copyright (C) 2010 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
## This directory contains a copy of
## texmf-dist/scripts/context/stubs/unix.  They should be kept in sync.
## The scripts here are installed as-is into each Unixish bin directory.
##
## Because mptopdf.pl is not part of the context/stubs directory, it is
## handled through a symlink, see ../linked_scripts.

if !WIN32
dist_bin_SCRIPTS = \
	context \
	ctxtools \
	luatools \
	mtxrun \
	pstopdf \
	texexec \
	texmfstart
endif !WIN32

dist_man1_MANS = \
	context.man \
	ctxtools.man \
	mptopdf.man \
	pstopdf.man \
	texexec.man \
	texmfstart.man

