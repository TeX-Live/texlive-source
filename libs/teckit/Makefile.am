## Makefile.am for the TeX Live subdirectory libs/teckit/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = -I$(top_srcdir)/source/Public-headers $(ZLIB_INCLUDES)
AM_CPPFLAGS = -DNDEBUG
AM_CFLAGS = $(WARNING_CFLAGS)
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

noinst_LIBRARIES = libTECkit.a
EXTRA_LIBRARIES = libTECkit_Compiler.a

libTECkit_Compiler_a_SOURCES = \
	source/Compiler.cpp \
	source/UnicodeNames.cpp

libTECkit_Compiler_a_DEPENDENCIES = $(ZLIB_DEPEND)

libTECkit_a_SOURCES = \
	source/Engine.cpp

## included from source/Engine.cpp
EXTRA_DIST = source/NormalizationData.c

libTECkit_a_DEPENDENCIES = $(ZLIB_DEPEND)

noinst_HEADERS = \
	source/Compiler.h \
	source/Engine.h \
	source/Public-headers/TECkit_Common.h \
	source/Public-headers/TECkit_Compiler.h \
	source/Public-headers/TECkit_Engine.h \
	source/TECkit_Format.h \
	source/ulong_chartraits.h

## Rebuild zlib
@ZLIB_RULE@

## Not used
##
EXTRA_DIST += \
	SFconv \
	autogen.sh \
	ax_misc.m4 \
	ax_prog_rc.m4 \
	bin \
	docs/Calling_TECkit_from_VB.doc \
	docs/Makefile.am \
	docs/Makefile.in \
	docs/TECkit_Binary_Format.doc \
	docs/TECkit_Binary_Format.doc.pdf \
	docs/TECkit_Language_2.1.doc \
	docs/TECkit_Language_2.1.doc.pdf \
	docs/TECkit_version_2.1.doc \
	docs/TECkit_version_2.1.doc.pdf \
	lib \
	license/LICENSING.txt \
	license/License_CPLv05.txt \
	license/License_LGPLv21.txt \
	source/Carbon.r \
	source/Compiler_ver.rc \
	source/Engine_ver.rc \
	source/MakeNormData.pl \
	source/MakeUnicodeNames.pl \
	source/Makefile.am \
	source/Perl \
	source/Prefix_Mac.h \
	source/Prefix_Win32.h \
	source/Public-headers/Makefile.am \
	source/REALplugin.cp \
	source/Sample-tools \
	source/TECkit_Compiler.cmd \
	source/TECkit_Compiler.exp \
	source/TECkit_Engine.cmd \
	source/TECkit_Engine.exp \
	source/test.cp \
	source/version_defs.h \
	test

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

## Original files
##
EXTRA_DIST += Makefile.am.orig configure.ac.orig

