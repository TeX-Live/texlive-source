## Makefile.am for the TeX Live subdirectory utils/pmx/
##
## Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

## We want to re-distribute the whole original pmx source tree.
EXTRA_DIST = $(PMX_TREE)
## Patches applied to the original source tree
EXTRA_DIST += $(PMX_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

INCLUDES = -I$(srcdir)/$(PMX_TREE)/libf2c
AM_CFLAGS = #$(WARNING_CFLAGS)

bin_PROGRAMS = pmxab scor2prt

pmxab_SOURCES = $(PMX_TREE)/pmxab.c

scor2prt_SOURCES = $(PMX_TREE)/scor2prt.c

LDADD = libf2c.a

noinst_LIBRARIES = libf2c.a

## libf2c headers
libf2c_a_SOURCES = \
	arith.h \
	$(PMX_TREE)/libf2c/f2c.h \
	$(PMX_TREE)/libf2c/fio.h \
	$(PMX_TREE)/libf2c/fmt.h \
	$(PMX_TREE)/libf2c/fp.h \
	$(PMX_TREE)/libf2c/lio.h \
	$(PMX_TREE)/libf2c/rawio.h \
	$(PMX_TREE)/libf2c/signal1.h \
	$(PMX_TREE)/libf2c/sysdep1.h
## MISC
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/abort_.c \
	$(PMX_TREE)/libf2c/cabs.c \
	$(PMX_TREE)/libf2c/derf_.c \
	$(PMX_TREE)/libf2c/derfc_.c \
	$(PMX_TREE)/libf2c/erf_.c \
	$(PMX_TREE)/libf2c/erfc_.c \
	$(PMX_TREE)/libf2c/exit_.c \
	$(PMX_TREE)/libf2c/f77vers.c \
	$(PMX_TREE)/libf2c/getarg_.c \
	$(PMX_TREE)/libf2c/getenv_.c \
	$(PMX_TREE)/libf2c/i77vers.c \
	$(PMX_TREE)/libf2c/iargc_.c \
	$(PMX_TREE)/libf2c/main.c \
	$(PMX_TREE)/libf2c/s_paus.c \
	$(PMX_TREE)/libf2c/s_rnge.c \
	$(PMX_TREE)/libf2c/s_stop.c \
	$(PMX_TREE)/libf2c/sig_die.c \
	$(PMX_TREE)/libf2c/signal_.c \
	$(PMX_TREE)/libf2c/system_.c
## MISC (not used for pmx)
EXTRA_libf2c_a_SOURCES = \
	$(PMX_TREE)/libf2c/uninit.c
## POW
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/pow_ci.c \
	$(PMX_TREE)/libf2c/pow_dd.c \
	$(PMX_TREE)/libf2c/pow_di.c \
	$(PMX_TREE)/libf2c/pow_hh.c \
	$(PMX_TREE)/libf2c/pow_ii.c \
	$(PMX_TREE)/libf2c/pow_ri.c \
	$(PMX_TREE)/libf2c/pow_zi.c \
	$(PMX_TREE)/libf2c/pow_zz.c
## CX
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/c_abs.c \
	$(PMX_TREE)/libf2c/c_cos.c \
	$(PMX_TREE)/libf2c/c_div.c \
	$(PMX_TREE)/libf2c/c_exp.c \
	$(PMX_TREE)/libf2c/c_log.c \
	$(PMX_TREE)/libf2c/c_sin.c \
	$(PMX_TREE)/libf2c/c_sqrt.c
## DCX
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/z_abs.c \
	$(PMX_TREE)/libf2c/z_cos.c \
	$(PMX_TREE)/libf2c/z_div.c \
	$(PMX_TREE)/libf2c/z_exp.c \
	$(PMX_TREE)/libf2c/z_log.c \
	$(PMX_TREE)/libf2c/z_sin.c \
	$(PMX_TREE)/libf2c/z_sqrt.c
## REAL
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/r_abs.c \
	$(PMX_TREE)/libf2c/r_acos.c \
	$(PMX_TREE)/libf2c/r_asin.c \
	$(PMX_TREE)/libf2c/r_atan.c \
	$(PMX_TREE)/libf2c/r_atn2.c \
	$(PMX_TREE)/libf2c/r_cnjg.c \
	$(PMX_TREE)/libf2c/r_cos.c \
	$(PMX_TREE)/libf2c/r_cosh.c \
	$(PMX_TREE)/libf2c/r_dim.c \
	$(PMX_TREE)/libf2c/r_exp.c \
	$(PMX_TREE)/libf2c/r_imag.c \
	$(PMX_TREE)/libf2c/r_int.c \
	$(PMX_TREE)/libf2c/r_lg10.c \
	$(PMX_TREE)/libf2c/r_log.c \
	$(PMX_TREE)/libf2c/r_mod.c \
	$(PMX_TREE)/libf2c/r_nint.c \
	$(PMX_TREE)/libf2c/r_sign.c \
	$(PMX_TREE)/libf2c/r_sin.c \
	$(PMX_TREE)/libf2c/r_sinh.c \
	$(PMX_TREE)/libf2c/r_sqrt.c \
	$(PMX_TREE)/libf2c/r_tan.c \
	$(PMX_TREE)/libf2c/r_tanh.c
## DBL
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/d_abs.c \
	$(PMX_TREE)/libf2c/d_acos.c \
	$(PMX_TREE)/libf2c/d_asin.c \
	$(PMX_TREE)/libf2c/d_atan.c \
	$(PMX_TREE)/libf2c/d_atn2.c \
	$(PMX_TREE)/libf2c/d_cnjg.c \
	$(PMX_TREE)/libf2c/d_cos.c \
	$(PMX_TREE)/libf2c/d_cosh.c \
	$(PMX_TREE)/libf2c/d_dim.c \
	$(PMX_TREE)/libf2c/d_exp.c \
	$(PMX_TREE)/libf2c/d_imag.c \
	$(PMX_TREE)/libf2c/d_int.c \
	$(PMX_TREE)/libf2c/d_lg10.c \
	$(PMX_TREE)/libf2c/d_log.c \
	$(PMX_TREE)/libf2c/d_mod.c \
	$(PMX_TREE)/libf2c/d_nint.c \
	$(PMX_TREE)/libf2c/d_prod.c \
	$(PMX_TREE)/libf2c/d_sign.c \
	$(PMX_TREE)/libf2c/d_sin.c \
	$(PMX_TREE)/libf2c/d_sinh.c \
	$(PMX_TREE)/libf2c/d_sqrt.c \
	$(PMX_TREE)/libf2c/d_tan.c \
	$(PMX_TREE)/libf2c/d_tanh.c
## INT
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/i_abs.c \
	$(PMX_TREE)/libf2c/i_dim.c \
	$(PMX_TREE)/libf2c/i_dnnt.c \
	$(PMX_TREE)/libf2c/i_indx.c \
	$(PMX_TREE)/libf2c/i_len.c \
	$(PMX_TREE)/libf2c/i_mod.c \
	$(PMX_TREE)/libf2c/i_nint.c \
	$(PMX_TREE)/libf2c/i_sign.c \
	$(PMX_TREE)/libf2c/lbitbits.c \
	$(PMX_TREE)/libf2c/lbitshft.c
## HALF
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/h_abs.c \
	$(PMX_TREE)/libf2c/h_dim.c \
	$(PMX_TREE)/libf2c/h_dnnt.c \
	$(PMX_TREE)/libf2c/h_indx.c \
	$(PMX_TREE)/libf2c/h_len.c \
	$(PMX_TREE)/libf2c/h_mod.c \
	$(PMX_TREE)/libf2c/h_nint.c \
	$(PMX_TREE)/libf2c/h_sign.c
## CMP
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/hl_ge.c \
	$(PMX_TREE)/libf2c/hl_gt.c \
	$(PMX_TREE)/libf2c/hl_le.c \
	$(PMX_TREE)/libf2c/hl_lt.c \
	$(PMX_TREE)/libf2c/l_ge.c \
	$(PMX_TREE)/libf2c/l_gt.c \
	$(PMX_TREE)/libf2c/l_le.c \
	$(PMX_TREE)/libf2c/l_lt.c
## EFL
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/ef1asc_.c \
	$(PMX_TREE)/libf2c/ef1cmc_.c
## CHAR
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/f77_aloc.c \
	$(PMX_TREE)/libf2c/s_cat.c \
	$(PMX_TREE)/libf2c/s_cmp.c \
	$(PMX_TREE)/libf2c/s_copy.c
## I77
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/backspac.c \
	$(PMX_TREE)/libf2c/close.c \
	$(PMX_TREE)/libf2c/dfe.c \
	$(PMX_TREE)/libf2c/dolio.c \
	$(PMX_TREE)/libf2c/due.c \
	$(PMX_TREE)/libf2c/endfile.c \
	$(PMX_TREE)/libf2c/err.c \
	$(PMX_TREE)/libf2c/fmt.c \
	$(PMX_TREE)/libf2c/fmtlib.c \
	$(PMX_TREE)/libf2c/ftell_.c \
	$(PMX_TREE)/libf2c/iio.c \
	$(PMX_TREE)/libf2c/ilnw.c \
	$(PMX_TREE)/libf2c/inquire.c \
	$(PMX_TREE)/libf2c/lread.c \
	$(PMX_TREE)/libf2c/lwrite.c \
	$(PMX_TREE)/libf2c/open.c \
	$(PMX_TREE)/libf2c/rdfmt.c \
	$(PMX_TREE)/libf2c/rewind.c \
	$(PMX_TREE)/libf2c/rsfe.c \
	$(PMX_TREE)/libf2c/rsli.c \
	$(PMX_TREE)/libf2c/rsne.c \
	$(PMX_TREE)/libf2c/sfe.c \
	$(PMX_TREE)/libf2c/sue.c \
	$(PMX_TREE)/libf2c/typesize.c \
	$(PMX_TREE)/libf2c/uio.c \
	$(PMX_TREE)/libf2c/util.c \
	$(PMX_TREE)/libf2c/wref.c \
	$(PMX_TREE)/libf2c/wrtfmt.c \
	$(PMX_TREE)/libf2c/wsfe.c \
	$(PMX_TREE)/libf2c/wsle.c \
	$(PMX_TREE)/libf2c/wsne.c \
	$(PMX_TREE)/libf2c/xwsne.c
## TIME
libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/dtime_.c \
	$(PMX_TREE)/libf2c/etime_.c

## QINT (unused)
EXTRA_libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/ftell64_.c \
	$(PMX_TREE)/libf2c/pow_qq.c \
	$(PMX_TREE)/libf2c/qbitbits.c \
	$(PMX_TREE)/libf2c/qbitshft.c
## unused
EXTRA_libf2c_a_SOURCES += \
	$(PMX_TREE)/libf2c/signbit.c

lua_scripts = pmx

scriptsdir = texmf/scripts/pmx
nodist_bin_SCRIPTS =

include $(srcdir)/../../texk/am/script_links.am

texmfdir = $(datarootdir)/$(scriptsdir)
dist_texmf_SCRIPTS = $(lua_scripts:=.lua)

install-data-hook: install-lua-links

uninstall-hook: uninstall-links

dist_man1_MANS = pmx.1 pmxab.1 scor2prt.1

## Tests
##
TESTS = pmx.test
EXTRA_DIST += $(TESTS)

## pmx.test
EXTRA_DIST += tests/barsant.mid tests/barsant.pml tests/barsant.pmx \
	tests/barsant.tex tests/barsant1.pmx tests/barsant2.pmx
DISTCLEANFILES = barsant* pmxaerr.dat

