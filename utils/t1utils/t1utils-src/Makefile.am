## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = t1ascii t1binary t1asm t1disasm t1unmac t1mac
man_MANS = t1ascii.1 t1binary.1 t1asm.1 t1disasm.1 t1unmac.1 t1mac.1

t1ascii_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1ascii.c
t1binary_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1binary.c
t1asm_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1asmhelp.h t1lib.c t1asm.c
t1disasm_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1asmhelp.h t1lib.c t1disasm.c
t1unmac_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1unmac.c
t1mac_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1mac.c

AM_CPPFLAGS = -I$(top_srcdir)/include
LDADD = @LIBOBJS@

EXTRA_DIST = $(man_MANS) INSTALL LICENSE README.md NEWS.md

versionize:
	perl -pi -e "s/^\\.ds V.*/.ds V $(VERSION)/;" t1ascii.1 t1binary.1 t1disasm.1 t1asm.1 t1unmac.1 t1mac.1

.PHONY: srclinks versionize
