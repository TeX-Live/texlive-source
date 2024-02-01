## $Id$
## Proxy Makefile.am to build MPFI for TeX Live.
##
##   Copyright 2022-2024 Luigi Scarso <luigi.scarso@gmail.com>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole MPFI source tree.
##
EXTRA_DIST = $(MPFI_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += TLpatches

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include

AM_CPPFLAGS = -I$(srcdir)/$(MPFI_TREE)/src $(MPFR_INCLUDES) $(GMP_INCLUDES)
AM_CFLAGS = $(VISIBILITY_CFLAGS) # $(WARNING_CFLAGS)

noinst_LIBRARIES = libmpfi.a
$(libmpfi_a_OBJECTS): $(GMP_DEPEND)
nodist_libmpfi_a_SOURCES = \
		@MPFI_TREE@/src/abs.c \
		@MPFI_TREE@/src/acos.c \
		@MPFI_TREE@/src/acosh.c \
		@MPFI_TREE@/src/add.c \
		@MPFI_TREE@/src/add_d.c \
		@MPFI_TREE@/src/add_fr.c \
		@MPFI_TREE@/src/add_q.c \
		@MPFI_TREE@/src/add_si.c \
		@MPFI_TREE@/src/add_ui.c \
		@MPFI_TREE@/src/add_z.c \
		@MPFI_TREE@/src/alea.c \
		@MPFI_TREE@/src/asin.c \
		@MPFI_TREE@/src/asinh.c \
		@MPFI_TREE@/src/atan.c \
		@MPFI_TREE@/src/atan2.c \
		@MPFI_TREE@/src/atanh.c \
		@MPFI_TREE@/src/bisect.c \
		@MPFI_TREE@/src/blow.c \
		@MPFI_TREE@/src/cbrt.c \
		@MPFI_TREE@/src/clear.c \
		@MPFI_TREE@/src/clears.c \
		@MPFI_TREE@/src/cmp.c \
		@MPFI_TREE@/src/cmp_sym_pi.c \
		@MPFI_TREE@/src/constants.c \
		@MPFI_TREE@/src/cos.c \
		@MPFI_TREE@/src/cosh.c \
		@MPFI_TREE@/src/cot.c \
		@MPFI_TREE@/src/coth.c \
		@MPFI_TREE@/src/csc.c \
		@MPFI_TREE@/src/csch.c \
		@MPFI_TREE@/src/d_div.c \
		@MPFI_TREE@/src/d_sub.c \
		@MPFI_TREE@/src/diam.c \
		@MPFI_TREE@/src/div.c \
		@MPFI_TREE@/src/div_2exp.c \
		@MPFI_TREE@/src/div_2si.c \
		@MPFI_TREE@/src/div_2ui.c \
		@MPFI_TREE@/src/div_d.c \
		@MPFI_TREE@/src/div_ext.c \
		@MPFI_TREE@/src/div_fr.c \
		@MPFI_TREE@/src/div_q.c \
		@MPFI_TREE@/src/div_si.c \
		@MPFI_TREE@/src/div_ui.c \
		@MPFI_TREE@/src/div_z.c \
		@MPFI_TREE@/src/erandom.c \
		@MPFI_TREE@/src/error.c \
		@MPFI_TREE@/src/exp.c \
		@MPFI_TREE@/src/exp10.c \
		@MPFI_TREE@/src/exp10m1.c \
		@MPFI_TREE@/src/exp2.c \
		@MPFI_TREE@/src/exp2m1.c \
		@MPFI_TREE@/src/expm1.c \
		@MPFI_TREE@/src/fr_div.c \
		@MPFI_TREE@/src/fr_sub.c \
		@MPFI_TREE@/src/get_d.c \
		@MPFI_TREE@/src/get_endpoints.c \
		@MPFI_TREE@/src/get_fr.c \
		@MPFI_TREE@/src/get_prec.c \
		@MPFI_TREE@/src/get_version.c \
		@MPFI_TREE@/src/has_zero.c \
		@MPFI_TREE@/src/hypot.c \
		@MPFI_TREE@/src/increase.c \
		@MPFI_TREE@/src/init.c \
		@MPFI_TREE@/src/init2.c \
		@MPFI_TREE@/src/inits.c \
		@MPFI_TREE@/src/inits2.c \
		@MPFI_TREE@/src/inp_str.c \
		@MPFI_TREE@/src/intersect.c \
		@MPFI_TREE@/src/interv_d.c \
		@MPFI_TREE@/src/interv_fr.c \
		@MPFI_TREE@/src/interv_q.c \
		@MPFI_TREE@/src/interv_si.c \
		@MPFI_TREE@/src/interv_ui.c \
		@MPFI_TREE@/src/interv_z.c \
		@MPFI_TREE@/src/inv.c \
		@MPFI_TREE@/src/is_empty.c \
		@MPFI_TREE@/src/is_inside.c \
		@MPFI_TREE@/src/log.c \
		@MPFI_TREE@/src/log10.c \
		@MPFI_TREE@/src/log10p1.c \
		@MPFI_TREE@/src/log1p.c \
		@MPFI_TREE@/src/log2.c \
		@MPFI_TREE@/src/log2p1.c \
		@MPFI_TREE@/src/mag.c \
		@MPFI_TREE@/src/mid.c \
		@MPFI_TREE@/src/mig.c \
		@MPFI_TREE@/src/mpfi-impl.h \
		@MPFI_TREE@/src/mpfi.h \
		@MPFI_TREE@/src/mpfi_io.h \
		@MPFI_TREE@/src/mul.c \
		@MPFI_TREE@/src/mul_2exp.c \
		@MPFI_TREE@/src/mul_2si.c \
		@MPFI_TREE@/src/mul_2ui.c \
		@MPFI_TREE@/src/mul_d.c \
		@MPFI_TREE@/src/mul_fr.c \
		@MPFI_TREE@/src/mul_q.c \
		@MPFI_TREE@/src/mul_si.c \
		@MPFI_TREE@/src/mul_ui.c \
		@MPFI_TREE@/src/mul_z.c \
		@MPFI_TREE@/src/neg.c \
		@MPFI_TREE@/src/nrandom.c \
		@MPFI_TREE@/src/out_str.c \
		@MPFI_TREE@/src/predicates.c \
		@MPFI_TREE@/src/print_binary.c \
		@MPFI_TREE@/src/put.c \
		@MPFI_TREE@/src/put_d.c \
		@MPFI_TREE@/src/put_fr.c \
		@MPFI_TREE@/src/put_q.c \
		@MPFI_TREE@/src/put_si.c \
		@MPFI_TREE@/src/put_ui.c \
		@MPFI_TREE@/src/put_z.c \
		@MPFI_TREE@/src/q_div.c \
		@MPFI_TREE@/src/q_sub.c \
		@MPFI_TREE@/src/quadrant.c \
		@MPFI_TREE@/src/rec_sqrt.c \
		@MPFI_TREE@/src/revert_if_needed.c \
		@MPFI_TREE@/src/round_prec.c \
		@MPFI_TREE@/src/sec.c \
		@MPFI_TREE@/src/sech.c \
		@MPFI_TREE@/src/set.c \
		@MPFI_TREE@/src/set_d.c \
		@MPFI_TREE@/src/set_flt.c \
		@MPFI_TREE@/src/set_fr.c \
		@MPFI_TREE@/src/set_ld.c \
		@MPFI_TREE@/src/set_prec.c \
		@MPFI_TREE@/src/set_q.c \
		@MPFI_TREE@/src/set_si.c \
		@MPFI_TREE@/src/set_str.c \
		@MPFI_TREE@/src/set_ui.c \
		@MPFI_TREE@/src/set_z.c \
		@MPFI_TREE@/src/si_div.c \
		@MPFI_TREE@/src/si_sub.c \
		@MPFI_TREE@/src/sign.c \
		@MPFI_TREE@/src/sin.c \
		@MPFI_TREE@/src/sinh.c \
		@MPFI_TREE@/src/sqr.c \
		@MPFI_TREE@/src/sqrt.c \
		@MPFI_TREE@/src/sub.c \
		@MPFI_TREE@/src/sub_d.c \
		@MPFI_TREE@/src/sub_fr.c \
		@MPFI_TREE@/src/sub_q.c \
		@MPFI_TREE@/src/sub_si.c \
		@MPFI_TREE@/src/sub_ui.c \
		@MPFI_TREE@/src/sub_z.c \
		@MPFI_TREE@/src/swap.c \
		@MPFI_TREE@/src/tan.c \
		@MPFI_TREE@/src/tanh.c \
		@MPFI_TREE@/src/ui_div.c \
		@MPFI_TREE@/src/ui_sub.c \
		@MPFI_TREE@/src/union.c \
		@MPFI_TREE@/src/urandom.c \
		@MPFI_TREE@/src/z_div.c \
		@MPFI_TREE@/src/z_sub.c 

##unused_sources = \

if build
check_PROGRAMS = mpfitest
dist_check_SCRIPTS = mpfi.test
TESTS = mpfi.test
endif build
mpfi.log: mpfitest$(EXEEXT)

mpfitest_SOURCES = mpfitest.c

mpfitest_CPPFLAGS = -Iinclude $(MPFR_INCLUDES) $(GMP_INCLUDES)

LDADD = libmpfi.a $(MPFR_LIBS) $(GMP_LIBS)

## Rebuild libgmp
@GMP_RULE@

## Rebuild libmpfr
@MPFR_RULE@



# Reconfig
reconfig_prereq = $(MPFR_DEPEND) $(GMP_DEPEND)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

