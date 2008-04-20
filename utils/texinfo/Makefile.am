# Makefile.am for texinfo.
# $Id: Makefile.am,v 1.24 2008/03/05 09:05:04 gray Exp $
# Process this file with automake to produce Makefile.in in all directories.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Find our headers and gnulib headers.
ACLOCAL_AMFLAGS = -I gnulib/m4

# Additional files to distribute.
EXTRA_DIST = ChangeLog.46 INSTALL.generic INTRODUCTION README.dev system.h \
  djgpp/README djgpp/config.bat djgpp/config.sed djgpp/config.site

# This is to prevent texinfo.tex from being included in the top-level
# distribution directory.
TEXINFO_TEX = doc/texinfo.tex

if TOOLS_ONLY
  # Build native tools only.
  SUBDIRS = lib info install-info makeinfo util
else
  # All subdirectories.
  # Do libs first since the C programs depend on it.
  # Do doc last so makeinfo will be built when we get there.
  # Others are alphabetical.
  SUBDIRS = $(native_tools) gnulib/lib lib \
            install-info info makeinfo po util doc 
endif

# One special target for installers to use by hand if desired.
install-tex:
	cd doc && $(MAKE) TEXMF=$(TEXMF) install-tex

distclean-local:
	-test -d "$(native_tools)" && rm -rf "$(native_tools)"

# Don't install native tools for cross compile.
if TOOLS_ONLY
install:

endif

# From coreutils.
# Verify that all source files using _() are listed in po/POTFILES.in.
# Run this before making pretests, as well as official releases, so that
# translators will see changed string.
po-check:
	if test -f po/POTFILES.in; then					\
	  grep -E -v '^(#|$$)' po/POTFILES.in				\
	    | grep -v '^src/false\.c$$' | sort > $@-1;			\
	  files=;							\
	  for file in $$($(CVS_LIST_EXCEPT)) `find [a-z]* -name '*.[ch]'`; do \
	    case $$file in						\
	    djgpp/* | man/*) continue;;					\
	    esac;							\
	    case $$file in						\
	    *.[ch])							\
	      base=`expr " $$file" : ' \(.*\)\..'`;			\
	      { test -f $$base.l || test -f $$base.y; } && continue;;	\
	    esac;							\
	    files="$$files $$file";					\
	  done;								\
	  grep -E -l '\b(N?_|gettext *)\([^)"]*("|$$)' $$files		\
	    | sort -u > $@-2;						\
	  diff -u $@-1 $@-2 || exit 1;					\
	  rm -f $@-1 $@-2;						\
	fi
