## Proxy Makefile.am to build MPFR for TeX Live.
##
##   Copyright (C) 2014 Taco Hoekwater <taco@metatex.org>
##   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole MPFR source tree.
##
EXTRA_DIST = $(MPFR_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(MPFR_TREE)-PATCHES

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn -o -name .deps -o -name .dirstamp -o -name '*.$(OBJEXT)'`

SUBDIRS = . include

AM_CPPFLAGS = -I$(srcdir)/$(MPFR_TREE)/src $(GMP_INCLUDES)
AM_CFLAGS = $(VISIBILITY_CFLAGS) # $(WARNING_CFLAGS)

noinst_LIBRARIES = libmpfr.a
$(libmpfr_a_OBJECTS): $(GMP_DEPEND)
nodist_libmpfr_a_SOURCES = \
	@MPFR_TREE@/src/abort_prec_max.c \
	@MPFR_TREE@/src/add.c \
	@MPFR_TREE@/src/add1.c \
	@MPFR_TREE@/src/add1sp.c \
	@MPFR_TREE@/src/add_d.c \
	@MPFR_TREE@/src/add_ui.c \
	@MPFR_TREE@/src/agm.c \
	@MPFR_TREE@/src/atan.c \
	@MPFR_TREE@/src/atan2.c \
	@MPFR_TREE@/src/cache.c \
	@MPFR_TREE@/src/clear.c \
	@MPFR_TREE@/src/clears.c \
	@MPFR_TREE@/src/cmp.c \
	@MPFR_TREE@/src/cmp2.c \
	@MPFR_TREE@/src/cmp_abs.c \
	@MPFR_TREE@/src/cmp_si.c \
	@MPFR_TREE@/src/cmp_ui.c \
	@MPFR_TREE@/src/comparisons.c \
	@MPFR_TREE@/src/const_catalan.c \
	@MPFR_TREE@/src/const_euler.c \
	@MPFR_TREE@/src/const_log2.c \
	@MPFR_TREE@/src/const_pi.c \
	@MPFR_TREE@/src/constant.c \
	@MPFR_TREE@/src/cos.c \
	@MPFR_TREE@/src/div.c \
	@MPFR_TREE@/src/div_2si.c \
	@MPFR_TREE@/src/div_2ui.c \
	@MPFR_TREE@/src/div_ui.c \
	@MPFR_TREE@/src/exceptions.c \
	@MPFR_TREE@/src/exp.c \
	@MPFR_TREE@/src/exp2.c \
	@MPFR_TREE@/src/exp3.c \
	@MPFR_TREE@/src/exp_2.c \
	@MPFR_TREE@/src/extract.c \
	@MPFR_TREE@/src/fits_sint.c \
	@MPFR_TREE@/src/fits_slong.c \
	@MPFR_TREE@/src/free_cache.c \
	@MPFR_TREE@/src/get_d.c \
	@MPFR_TREE@/src/get_si.c \
	@MPFR_TREE@/src/get_str.c \
	@MPFR_TREE@/src/get_z.c \
	@MPFR_TREE@/src/get_z_exp.c \
	@MPFR_TREE@/src/gmp_op.c \
	@MPFR_TREE@/src/init.c \
	@MPFR_TREE@/src/init2.c \
	@MPFR_TREE@/src/inits2.c \
	@MPFR_TREE@/src/isinteger.c \
	@MPFR_TREE@/src/isnum.c \
	@MPFR_TREE@/src/isqrt.c \
	@MPFR_TREE@/src/log.c \
	@MPFR_TREE@/src/mpfr-gmp.c \
	@MPFR_TREE@/src/mpn_exp.c \
	@MPFR_TREE@/src/mp_clz_tab.c \
	@MPFR_TREE@/src/mul.c \
	@MPFR_TREE@/src/mul_2si.c \
	@MPFR_TREE@/src/mul_2ui.c \
	@MPFR_TREE@/src/mul_ui.c \
	@MPFR_TREE@/src/mulders.c \
	@MPFR_TREE@/src/neg.c \
	@MPFR_TREE@/src/next.c \
	@MPFR_TREE@/src/powerof2.c \
	@MPFR_TREE@/src/rem1.c \
	@MPFR_TREE@/src/rint.c \
	@MPFR_TREE@/src/round_near_x.c \
	@MPFR_TREE@/src/round_p.c \
	@MPFR_TREE@/src/round_prec.c \
	@MPFR_TREE@/src/scale2.c \
	@MPFR_TREE@/src/set.c \
	@MPFR_TREE@/src/set_d.c \
	@MPFR_TREE@/src/set_dfl_prec.c \
	@MPFR_TREE@/src/set_f.c \
	@MPFR_TREE@/src/set_prec.c \
	@MPFR_TREE@/src/set_q.c \
	@MPFR_TREE@/src/set_rnd.c \
	@MPFR_TREE@/src/set_si_2exp.c \
	@MPFR_TREE@/src/set_str.c \
	@MPFR_TREE@/src/set_ui_2exp.c \
	@MPFR_TREE@/src/set_z.c \
	@MPFR_TREE@/src/set_z_exp.c \
	@MPFR_TREE@/src/set_zero.c \
	@MPFR_TREE@/src/setmax.c \
	@MPFR_TREE@/src/setmin.c \
	@MPFR_TREE@/src/sgn.c \
	@MPFR_TREE@/src/sin.c \
	@MPFR_TREE@/src/sin_cos.c \
	@MPFR_TREE@/src/sqr.c \
	@MPFR_TREE@/src/sqrt.c \
	@MPFR_TREE@/src/sqrt_ui.c \
	@MPFR_TREE@/src/strtofr.c \
	@MPFR_TREE@/src/sub.c \
	@MPFR_TREE@/src/sub1.c \
	@MPFR_TREE@/src/sub1sp.c \
	@MPFR_TREE@/src/sub_ui.c \
	@MPFR_TREE@/src/swap.c \
	@MPFR_TREE@/src/ui_div.c \
	@MPFR_TREE@/src/ui_sub.c \
	@MPFR_TREE@/src/version.c

unused_sources = \
	@MPFR_TREE@/src/acos.c \
	@MPFR_TREE@/src/acosh.c \
	@MPFR_TREE@/src/ai.c \
	@MPFR_TREE@/src/asin.c \
	@MPFR_TREE@/src/asinh.c \
	@MPFR_TREE@/src/atanh.c \
	@MPFR_TREE@/src/bernoulli.c \
	@MPFR_TREE@/src/buildopt.c \
	@MPFR_TREE@/src/cbrt.c \
	@MPFR_TREE@/src/check.c \
	@MPFR_TREE@/src/cmp_d.c \
	@MPFR_TREE@/src/cmp_ld.c \
	@MPFR_TREE@/src/copysign.c \
	@MPFR_TREE@/src/cosh.c \
	@MPFR_TREE@/src/cot.c \
	@MPFR_TREE@/src/coth.c \
	@MPFR_TREE@/src/csc.c \
	@MPFR_TREE@/src/csch.c \
	@MPFR_TREE@/src/d_div.c \
	@MPFR_TREE@/src/d_sub.c \
	@MPFR_TREE@/src/digamma.c \
	@MPFR_TREE@/src/dim.c \
	@MPFR_TREE@/src/div_2exp.c \
	@MPFR_TREE@/src/div_d.c \
	@MPFR_TREE@/src/dump.c \
	@MPFR_TREE@/src/eint.c \
	@MPFR_TREE@/src/eq.c \
	@MPFR_TREE@/src/erf.c \
	@MPFR_TREE@/src/erfc.c \
	@MPFR_TREE@/src/exp10.c \
	@MPFR_TREE@/src/expm1.c \
	@MPFR_TREE@/src/factorial.c \
	@MPFR_TREE@/src/fits_intmax.c \
	@MPFR_TREE@/src/fits_sshort.c \
	@MPFR_TREE@/src/fits_uint.c \
	@MPFR_TREE@/src/fits_uintmax.c \
	@MPFR_TREE@/src/fits_ulong.c \
	@MPFR_TREE@/src/fits_ushort.c \
	@MPFR_TREE@/src/fma.c \
	@MPFR_TREE@/src/fms.c \
	@MPFR_TREE@/src/frac.c \
	@MPFR_TREE@/src/frexp.c \
	@MPFR_TREE@/src/gamma.c \
	@MPFR_TREE@/src/gammaonethird.c \
	@MPFR_TREE@/src/get_d64.c \
	@MPFR_TREE@/src/get_exp.c \
	@MPFR_TREE@/src/get_f.c \
	@MPFR_TREE@/src/get_flt.c \
	@MPFR_TREE@/src/get_ld.c \
	@MPFR_TREE@/src/get_patches.c \
	@MPFR_TREE@/src/get_sj.c \
	@MPFR_TREE@/src/get_ui.c \
	@MPFR_TREE@/src/get_uj.c \
	@MPFR_TREE@/src/grandom.c \
	@MPFR_TREE@/src/hypot.c \
	@MPFR_TREE@/src/inits.c \
	@MPFR_TREE@/src/inp_str.c \
	@MPFR_TREE@/src/int_ceil_log2.c \
	@MPFR_TREE@/src/isinf.c \
	@MPFR_TREE@/src/isnan.c \
	@MPFR_TREE@/src/isregular.c \
	@MPFR_TREE@/src/iszero.c \
	@MPFR_TREE@/src/jn.c \
	@MPFR_TREE@/src/li2.c \
	@MPFR_TREE@/src/lngamma.c \
	@MPFR_TREE@/src/log10.c \
	@MPFR_TREE@/src/log1p.c \
	@MPFR_TREE@/src/log2.c \
	@MPFR_TREE@/src/logging.c \
	@MPFR_TREE@/src/min_prec.c \
	@MPFR_TREE@/src/minmax.c \
	@MPFR_TREE@/src/modf.c \
	@MPFR_TREE@/src/mul_2exp.c \
	@MPFR_TREE@/src/mul_d.c \
	@MPFR_TREE@/src/out_str.c \
	@MPFR_TREE@/src/pow.c \
	@MPFR_TREE@/src/pow_si.c \
	@MPFR_TREE@/src/pow_ui.c \
	@MPFR_TREE@/src/pow_z.c \
	@MPFR_TREE@/src/print_raw.c \
	@MPFR_TREE@/src/print_rnd_mode.c \
	@MPFR_TREE@/src/printf.c \
	@MPFR_TREE@/src/rec_sqrt.c \
	@MPFR_TREE@/src/reldiff.c \
	@MPFR_TREE@/src/root.c \
	@MPFR_TREE@/src/sec.c \
	@MPFR_TREE@/src/sech.c \
	@MPFR_TREE@/src/set_d64.c \
	@MPFR_TREE@/src/set_exp.c \
	@MPFR_TREE@/src/set_flt.c \
	@MPFR_TREE@/src/set_inf.c \
	@MPFR_TREE@/src/set_ld.c \
	@MPFR_TREE@/src/set_nan.c \
	@MPFR_TREE@/src/set_prc_raw.c \
	@MPFR_TREE@/src/set_si.c \
	@MPFR_TREE@/src/set_sj.c \
	@MPFR_TREE@/src/set_str_raw.c \
	@MPFR_TREE@/src/set_ui.c \
	@MPFR_TREE@/src/set_uj.c \
	@MPFR_TREE@/src/setsign.c \
	@MPFR_TREE@/src/si_op.c \
	@MPFR_TREE@/src/signbit.c \
	@MPFR_TREE@/src/sinh.c \
	@MPFR_TREE@/src/sinh_cosh.c \
	@MPFR_TREE@/src/stack_interface.c \
	@MPFR_TREE@/src/sub_d.c \
	@MPFR_TREE@/src/subnormal.c \
	@MPFR_TREE@/src/sum.c \
	@MPFR_TREE@/src/tan.c \
	@MPFR_TREE@/src/tanh.c \
	@MPFR_TREE@/src/uceil_exp2.c \
	@MPFR_TREE@/src/uceil_log2.c \
	@MPFR_TREE@/src/ufloor_log2.c \
	@MPFR_TREE@/src/ui_pow.c \
	@MPFR_TREE@/src/ui_pow_ui.c \
	@MPFR_TREE@/src/urandom.c \
	@MPFR_TREE@/src/urandomb.c \
	@MPFR_TREE@/src/vasprintf.c \
	@MPFR_TREE@/src/volatile.c \
	@MPFR_TREE@/src/yn.c \
	@MPFR_TREE@/src/zeta.c \
	@MPFR_TREE@/src/zeta_ui.c

if build
check_PROGRAMS = mpfrtest
dist_check_SCRIPTS = mpfr.test
TESTS = mpfr.test
endif build

mpfrtest_SOURCES = mpfrtest.c

mpfrtest_CPPFLAGS = -Iinclude $(GMP_INCLUDES)

LDADD = libmpfr.a $(GMP_LIBS)

## Rebuild libgmp
@GMP_RULE@

# Reconfig
reconfig_prereq = $(GMP_DEPEND)
DISTCLEANFILES =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

