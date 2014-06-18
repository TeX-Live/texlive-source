## Makefile.am for the TeX Live subdirectory libs/gmp/native/
##
## Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
AM_CPPFLAGS = -I$(srcdir)/$(GMP_TREE)
AM_CFLAGS = -Wall # $(WARNING_CFLAGS)

EXTRA_PROGRAMS =
built_sources =
extra_data =

dist-hook:
	rm -rf `find $(distdir) -name '*.c'`

EXTRA_PROGRAMS += gen-bases
gen_bases_LDADD = $(MATH_LIB)
stamp-bases: gen-bases$(EXEEXT)
	echo timestamp >$@
built_sources += gen-bases.c
extra_data += stamp-bases

EXTRA_PROGRAMS += gen-fac
stamp-fac: gen-fac$(EXEEXT)
	echo timestamp >$@
built_sources += gen-fac.c
extra_data += stamp-fac

EXTRA_PROGRAMS += gen-fib
stamp-fib: gen-fib$(EXEEXT)
	echo timestamp >$@
built_sources += gen-fib.c
extra_data += stamp-fib

EXTRA_PROGRAMS += gen-jacobitab
stamp-jacobitab: gen-jacobitab$(EXEEXT)
	echo timestamp >$@
built_sources += gen-jacobitab.c
extra_data += stamp-jacobitab

EXTRA_PROGRAMS += gen-psqr
gen_psqr_LDADD = $(MATH_LIB)
stamp-psqr: gen-psqr$(EXEEXT)
	echo timestamp >$@
built_sources += gen-psqr.c
extra_data += stamp-psqr

EXTRA_PROGRAMS += gen-trialdivtab
gen_trialdivtab_LDADD = $(MATH_LIB)
stamp-trialdivtab: gen-trialdivtab$(EXEEXT)
	echo timestamp >$@
built_sources += gen-trialdivtab.c
extra_data += stamp-trialdivtab

$(built_sources):
	@rm -f $@
	$(AM_V_LINK)$(LN_S) $(srcdir)/$(GMP_TREE)/$@ $@

AM_V_LINK = $(am__v_LINK_@AM_V@)
am__v_LINK_ = $(am__v_LINK_@AM_DEFAULT_V@)
am__v_LINK_0 = @echo "  LINK    " $@;
am__v_LINK_1 = 

CLEANFILES = $(extra_data)

DISTCLEANFILES = $(built_sources)

