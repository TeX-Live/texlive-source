## Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##

INCLUDES = -I$(srcdir)/libf2c

bin_PROGRAMS = pmxab scor2prt

pmxab_SOURCES = pmxab.c

scor2prt_SOURCES = scor2prt.c

LDADD = libf2c.a

noinst_LIBRARIES = libf2c.a

## libf2c headers
libf2c_a_SOURCES = \
	arith.h \
	libf2c/f2c.h \
	libf2c/fio.h \
	libf2c/fmt.h \
	libf2c/fp.h \
	libf2c/lio.h \
	libf2c/rawio.h \
	libf2c/signal1.h \
	libf2c/sysdep1.h
## MISC
libf2c_a_SOURCES += \
	libf2c/abort_.c \
	libf2c/cabs.c \
	libf2c/derf_.c \
	libf2c/derfc_.c \
	libf2c/erf_.c \
	libf2c/erfc_.c \
	libf2c/exit_.c \
	libf2c/f77vers.c \
	libf2c/getarg_.c \
	libf2c/getenv_.c \
	libf2c/i77vers.c \
	libf2c/iargc_.c \
	libf2c/main.c \
	libf2c/s_paus.c \
	libf2c/s_rnge.c \
	libf2c/s_stop.c \
	libf2c/sig_die.c \
	libf2c/signal_.c \
	libf2c/system_.c
## MISC (not used for pmx)
EXTRA_libf2c_a_SOURCES = \
	libf2c/uninit.c
## POW
libf2c_a_SOURCES += \
	libf2c/pow_ci.c \
	libf2c/pow_dd.c \
	libf2c/pow_di.c \
	libf2c/pow_hh.c \
	libf2c/pow_ii.c \
	libf2c/pow_ri.c \
	libf2c/pow_zi.c \
	libf2c/pow_zz.c
## CX
libf2c_a_SOURCES += \
	libf2c/c_abs.c \
	libf2c/c_cos.c \
	libf2c/c_div.c \
	libf2c/c_exp.c \
	libf2c/c_log.c \
	libf2c/c_sin.c \
	libf2c/c_sqrt.c
## DCX
libf2c_a_SOURCES += \
	libf2c/z_abs.c \
	libf2c/z_cos.c \
	libf2c/z_div.c \
	libf2c/z_exp.c \
	libf2c/z_log.c \
	libf2c/z_sin.c \
	libf2c/z_sqrt.c
## REAL
libf2c_a_SOURCES += \
	libf2c/r_abs.c \
	libf2c/r_acos.c \
	libf2c/r_asin.c \
	libf2c/r_atan.c \
	libf2c/r_atn2.c \
	libf2c/r_cnjg.c \
	libf2c/r_cos.c \
	libf2c/r_cosh.c \
	libf2c/r_dim.c \
	libf2c/r_exp.c \
	libf2c/r_imag.c \
	libf2c/r_int.c \
	libf2c/r_lg10.c \
	libf2c/r_log.c \
	libf2c/r_mod.c \
	libf2c/r_nint.c \
	libf2c/r_sign.c \
	libf2c/r_sin.c \
	libf2c/r_sinh.c \
	libf2c/r_sqrt.c \
	libf2c/r_tan.c \
	libf2c/r_tanh.c
## DBL
libf2c_a_SOURCES += \
	libf2c/d_abs.c \
	libf2c/d_acos.c \
	libf2c/d_asin.c \
	libf2c/d_atan.c \
	libf2c/d_atn2.c \
	libf2c/d_cnjg.c \
	libf2c/d_cos.c \
	libf2c/d_cosh.c \
	libf2c/d_dim.c \
	libf2c/d_exp.c \
	libf2c/d_imag.c \
	libf2c/d_int.c \
	libf2c/d_lg10.c \
	libf2c/d_log.c \
	libf2c/d_mod.c \
	libf2c/d_nint.c \
	libf2c/d_prod.c \
	libf2c/d_sign.c \
	libf2c/d_sin.c \
	libf2c/d_sinh.c \
	libf2c/d_sqrt.c \
	libf2c/d_tan.c \
	libf2c/d_tanh.c
## INT
libf2c_a_SOURCES += \
	libf2c/i_abs.c \
	libf2c/i_dim.c \
	libf2c/i_dnnt.c \
	libf2c/i_indx.c \
	libf2c/i_len.c \
	libf2c/i_mod.c \
	libf2c/i_nint.c \
	libf2c/i_sign.c \
	libf2c/lbitbits.c \
	libf2c/lbitshft.c
## HALF
libf2c_a_SOURCES += \
	libf2c/h_abs.c \
	libf2c/h_dim.c \
	libf2c/h_dnnt.c \
	libf2c/h_indx.c \
	libf2c/h_len.c \
	libf2c/h_mod.c \
	libf2c/h_nint.c \
	libf2c/h_sign.c
## CMP
libf2c_a_SOURCES += \
	libf2c/hl_ge.c \
	libf2c/hl_gt.c \
	libf2c/hl_le.c \
	libf2c/hl_lt.c \
	libf2c/l_ge.c \
	libf2c/l_gt.c \
	libf2c/l_le.c \
	libf2c/l_lt.c
## EFL
libf2c_a_SOURCES += \
	libf2c/ef1asc_.c \
	libf2c/ef1cmc_.c
## CHAR
libf2c_a_SOURCES += \
	libf2c/f77_aloc.c \
	libf2c/s_cat.c \
	libf2c/s_cmp.c \
	libf2c/s_copy.c
## I77
libf2c_a_SOURCES += \
	libf2c/backspac.c \
	libf2c/close.c \
	libf2c/dfe.c \
	libf2c/dolio.c \
	libf2c/due.c \
	libf2c/endfile.c \
	libf2c/err.c \
	libf2c/fmt.c \
	libf2c/fmtlib.c \
	libf2c/ftell_.c \
	libf2c/iio.c \
	libf2c/ilnw.c \
	libf2c/inquire.c \
	libf2c/lread.c \
	libf2c/lwrite.c \
	libf2c/open.c \
	libf2c/rdfmt.c \
	libf2c/rewind.c \
	libf2c/rsfe.c \
	libf2c/rsli.c \
	libf2c/rsne.c \
	libf2c/sfe.c \
	libf2c/sue.c \
	libf2c/typesize.c \
	libf2c/uio.c \
	libf2c/util.c \
	libf2c/wref.c \
	libf2c/wrtfmt.c \
	libf2c/wsfe.c \
	libf2c/wsle.c \
	libf2c/wsne.c \
	libf2c/xwsne.c
## TIME
libf2c_a_SOURCES += \
	libf2c/dtime_.c \
	libf2c/etime_.c

## Handle DOS line endings
if WIN32
libf2c_a_SOURCES += w32getc.c
endif WIN32

## QINT (unused)
EXTRA_libf2c_a_SOURCES += \
	libf2c/ftell64_.c \
	libf2c/pow_qq.c \
	libf2c/qbitbits.c \
	libf2c/qbitshft.c
## unused
EXTRA_libf2c_a_SOURCES += \
	libf2c/signbit.c

## Tests
##
TESTS = pmx.test
TESTS_ENVIRONMENT = test_src=$(srcdir)/tests
EXTRA_DIST = $(TESTS)

PATCHES = libf2c/patch-01-binary libf2c/patch-03-DOS-line-endings \
	libf2c/patch-04-MSVC-complex libf2c/patch-05-FreeBSD-off_t libf2c/patch-06-WIN32 \
	libf2c/patch-07-MuslLibc \
        libf2c/patch-08-WIN64 \
        libf2c/patch-09-tmpfile 
## pmx.test
EXTRA_DIST += tests/barsant.mid tests/barsant.pml tests/barsant.pmx \
	tests/barsant.tex tests/barsant1.pmx tests/barsant2.pmx
EXTRA_DIST += scor2prt.for pmx294.for 
EXTRA_DIST += $(PATCHES)
EXTRA_DIST += version.ac configure.ac Makefile.am Makefile.f2c
EXTRA_DIST += pmx.tex
DISTCLEANFILES = barsant* pmxaerr.dat

