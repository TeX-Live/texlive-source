## Proxy Makefile.am to build GNU MP for TeX Live.
##
##   Copyright (C) 2014 Taco Hoekwater <taco@metatex.org>
##   Copyright (C) 2014, 2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole GNU MP source tree.
##
EXTRA_DIST = $(GMP_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(GMP_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = native . include

AM_CPPFLAGS = -I$(srcdir)/$(GMP_TREE) -I$(srcdir)/$(GMP_TREE)/mpn/generic -DNO_ASM
AM_CFLAGS = $(VISIBILITY_CFLAGS) # $(WARNING_CFLAGS)

noinst_LIBRARIES = libgmp.a
$(libgmp_a_OBJECTS): mp_bases.h fac_table.h fib_table.h
nodist_libgmp_a_SOURCES = \
	$(basic_sources) \
	$(mpn_sources) \
	$(mpz_sources)

basic_sources = \
	@GMP_TREE@/assert.c \
	@GMP_TREE@/compat.c \
	@GMP_TREE@/errno.c \
	@GMP_TREE@/extract-dbl.c \
	@GMP_TREE@/invalid.c \
	@GMP_TREE@/memory.c \
	@GMP_TREE@/mp_bpl.c \
	@GMP_TREE@/mp_clz_tab.c \
	@GMP_TREE@/mp_dv_tab.c \
	@GMP_TREE@/mp_get_fns.c \
	@GMP_TREE@/mp_minv_tab.c \
	@GMP_TREE@/mp_set_fns.c \
	@GMP_TREE@/nextprime.c \
	@GMP_TREE@/primesieve.c \
	@GMP_TREE@/tal-reent.c \
	@GMP_TREE@/version.c

basic_unused = \
	@GMP_TREE@/tal-debug.c \
	@GMP_TREE@/tal-notreent.c

logops_n_sources = and_n.c andn_n.c ior_n.c iorn_n.c nand_n.c nior_n.c xnor_n.c xor_n.c
popham_sources = hamdist.c popcount.c
sec_aors_1_sources = sec_add_1.c sec_sub_1.c
sec_div_sources = sec_div_qr.c sec_div_r.c
sec_pi1_div_sources = sec_pi1_div_qr.c sec_pi1_div_r.c

$(logops_n_sources):
	@echo "#define OPERATION_`echo $@ | sed 's/\.c//'` 1" >$@
	@echo "#include <logops_n.c>" >>$@
$(popham_sources):
	@echo "#define OPERATION_`echo $@ | sed 's/\.c//'` 1" >$@
	@echo "#include <popham.c>" >>$@
$(sec_aors_1_sources):
	@echo "#define OPERATION_`echo $@ | sed 's/\.c//'` 1" >$@
	@echo "#include <sec_aors_1.c>" >>$@
$(sec_div_sources):
	@echo "#define OPERATION_`echo $@ | sed 's/\.c//'` 1" >$@
	@echo "#include <sec_div.c>" >>$@
$(sec_pi1_div_sources):
	@echo "#define OPERATION_`echo $@ | sed 's/\.c//'` 1" >$@
	@echo "#include <sec_pi1_div.c>" >>$@

mpn_sources = \
	mp_bases.c \
	popcount.c \
	@GMP_TREE@/mpn/generic/add.c \
	@GMP_TREE@/mpn/generic/add_1.c \
	@GMP_TREE@/mpn/generic/add_n.c \
	@GMP_TREE@/mpn/generic/addmul_1.c \
	@GMP_TREE@/mpn/generic/bdiv_dbm1c.c \
	@GMP_TREE@/mpn/generic/bdiv_q.c \
	@GMP_TREE@/mpn/generic/binvert.c \
	@GMP_TREE@/mpn/generic/cmp.c \
	@GMP_TREE@/mpn/generic/com.c \
	@GMP_TREE@/mpn/generic/dcpi1_bdiv_q.c \
	@GMP_TREE@/mpn/generic/dcpi1_bdiv_qr.c \
	@GMP_TREE@/mpn/generic/dcpi1_div_q.c \
	@GMP_TREE@/mpn/generic/dcpi1_div_qr.c \
	@GMP_TREE@/mpn/generic/dcpi1_divappr_q.c \
	@GMP_TREE@/mpn/generic/div_q.c \
	@GMP_TREE@/mpn/generic/div_qr_2n_pi1.c \
	@GMP_TREE@/mpn/generic/dive_1.c \
	@GMP_TREE@/mpn/generic/divexact.c \
	@GMP_TREE@/mpn/generic/divrem.c \
	@GMP_TREE@/mpn/generic/divrem_1.c \
	@GMP_TREE@/mpn/generic/divrem_2.c \
	@GMP_TREE@/mpn/generic/gcd_subdiv_step.c \
	@GMP_TREE@/mpn/generic/gcdext.c \
	@GMP_TREE@/mpn/generic/gcdext_1.c \
	@GMP_TREE@/mpn/generic/gcdext_lehmer.c \
	@GMP_TREE@/mpn/generic/get_str.c \
	@GMP_TREE@/mpn/generic/hgcd.c \
	@GMP_TREE@/mpn/generic/hgcd2.c \
	@GMP_TREE@/mpn/generic/hgcd_appr.c \
	@GMP_TREE@/mpn/generic/hgcd_matrix.c \
	@GMP_TREE@/mpn/generic/hgcd_reduce.c \
	@GMP_TREE@/mpn/generic/hgcd_step.c \
	@GMP_TREE@/mpn/generic/invertappr.c \
	@GMP_TREE@/mpn/generic/lshift.c \
	@GMP_TREE@/mpn/generic/lshiftc.c \
	@GMP_TREE@/mpn/generic/matrix22_mul.c \
	@GMP_TREE@/mpn/generic/matrix22_mul1_inverse_vector.c \
	@GMP_TREE@/mpn/generic/mu_bdiv_q.c \
	@GMP_TREE@/mpn/generic/mu_div_q.c \
	@GMP_TREE@/mpn/generic/mu_div_qr.c \
	@GMP_TREE@/mpn/generic/mu_divappr_q.c \
	@GMP_TREE@/mpn/generic/mul.c \
	@GMP_TREE@/mpn/generic/mul_1.c \
	@GMP_TREE@/mpn/generic/mul_basecase.c \
	@GMP_TREE@/mpn/generic/mul_fft.c \
	@GMP_TREE@/mpn/generic/mul_n.c \
	@GMP_TREE@/mpn/generic/mullo_basecase.c \
	@GMP_TREE@/mpn/generic/mullo_n.c \
	@GMP_TREE@/mpn/generic/mulmod_bnm1.c \
	@GMP_TREE@/mpn/generic/neg.c \
	@GMP_TREE@/mpn/generic/nussbaumer_mul.c \
	@GMP_TREE@/mpn/generic/powlo.c \
	@GMP_TREE@/mpn/generic/powm.c \
	@GMP_TREE@/mpn/generic/pre_divrem_1.c \
	@GMP_TREE@/mpn/generic/redc_1.c \
	@GMP_TREE@/mpn/generic/redc_n.c \
	@GMP_TREE@/mpn/generic/rshift.c \
	@GMP_TREE@/mpn/generic/sbpi1_bdiv_q.c \
	@GMP_TREE@/mpn/generic/sbpi1_bdiv_qr.c \
	@GMP_TREE@/mpn/generic/sbpi1_div_q.c \
	@GMP_TREE@/mpn/generic/sbpi1_div_qr.c \
	@GMP_TREE@/mpn/generic/sbpi1_divappr_q.c \
	@GMP_TREE@/mpn/generic/scan1.c \
	@GMP_TREE@/mpn/generic/set_str.c \
	@GMP_TREE@/mpn/generic/sqr.c \
	@GMP_TREE@/mpn/generic/sqr_basecase.c \
	@GMP_TREE@/mpn/generic/sqrlo.c \
	@GMP_TREE@/mpn/generic/sqrlo_basecase.c \
	@GMP_TREE@/mpn/generic/sqrmod_bnm1.c \
	@GMP_TREE@/mpn/generic/sqrtrem.c \
	@GMP_TREE@/mpn/generic/sub.c \
	@GMP_TREE@/mpn/generic/sub_1.c \
	@GMP_TREE@/mpn/generic/sub_n.c \
	@GMP_TREE@/mpn/generic/submul_1.c \
	@GMP_TREE@/mpn/generic/tdiv_qr.c \
	@GMP_TREE@/mpn/generic/toom22_mul.c \
	@GMP_TREE@/mpn/generic/toom2_sqr.c \
	@GMP_TREE@/mpn/generic/toom32_mul.c \
	@GMP_TREE@/mpn/generic/toom33_mul.c \
	@GMP_TREE@/mpn/generic/toom3_sqr.c \
	@GMP_TREE@/mpn/generic/toom42_mul.c \
	@GMP_TREE@/mpn/generic/toom42_mulmid.c \
	@GMP_TREE@/mpn/generic/toom43_mul.c \
	@GMP_TREE@/mpn/generic/toom44_mul.c \
	@GMP_TREE@/mpn/generic/toom4_sqr.c \
	@GMP_TREE@/mpn/generic/toom53_mul.c \
	@GMP_TREE@/mpn/generic/toom63_mul.c \
	@GMP_TREE@/mpn/generic/toom6_sqr.c \
	@GMP_TREE@/mpn/generic/toom6h_mul.c \
	@GMP_TREE@/mpn/generic/toom8_sqr.c \
	@GMP_TREE@/mpn/generic/toom8h_mul.c \
	@GMP_TREE@/mpn/generic/toom_couple_handling.c \
	@GMP_TREE@/mpn/generic/toom_eval_dgr3_pm1.c \
	@GMP_TREE@/mpn/generic/toom_eval_dgr3_pm2.c \
	@GMP_TREE@/mpn/generic/toom_eval_pm1.c \
	@GMP_TREE@/mpn/generic/toom_eval_pm2.c \
	@GMP_TREE@/mpn/generic/toom_eval_pm2exp.c \
	@GMP_TREE@/mpn/generic/toom_eval_pm2rexp.c \
	@GMP_TREE@/mpn/generic/toom_interpolate_12pts.c \
	@GMP_TREE@/mpn/generic/toom_interpolate_16pts.c \
	@GMP_TREE@/mpn/generic/toom_interpolate_5pts.c \
	@GMP_TREE@/mpn/generic/toom_interpolate_6pts.c \
	@GMP_TREE@/mpn/generic/toom_interpolate_7pts.c \
	@GMP_TREE@/mpn/generic/toom_interpolate_8pts.c \
	@GMP_TREE@/mpn/generic/zero_p.c

mpn_unused = \
	$(logops_n_sources) \
	$(sec_aors_1_sources) \
	$(sec_div_sources) \
	$(sec_pi1_div_sources) \
	fib_table.c \
	hamdist.c \
	@GMP_TREE@/mpn/generic/add_err1_n.c \
	@GMP_TREE@/mpn/generic/add_err2_n.c \
	@GMP_TREE@/mpn/generic/add_err3_n.c \
	@GMP_TREE@/mpn/generic/add_n_sub_n.c \
	@GMP_TREE@/mpn/generic/bdiv_q_1.c \
	@GMP_TREE@/mpn/generic/bdiv_qr.c \
	@GMP_TREE@/mpn/generic/broot.c \
	@GMP_TREE@/mpn/generic/brootinv.c \
	@GMP_TREE@/mpn/generic/bsqrt.c \
	@GMP_TREE@/mpn/generic/bsqrtinv.c \
	@GMP_TREE@/mpn/generic/cnd_add_n.c \
	@GMP_TREE@/mpn/generic/cnd_sub_n.c \
	@GMP_TREE@/mpn/generic/comb_tables.c \
	@GMP_TREE@/mpn/generic/copyd.c \
	@GMP_TREE@/mpn/generic/copyi.c \
	@GMP_TREE@/mpn/generic/div_qr_1.c \
	@GMP_TREE@/mpn/generic/div_qr_1n_pi1.c \
	@GMP_TREE@/mpn/generic/div_qr_2.c \
	@GMP_TREE@/mpn/generic/div_qr_2u_pi1.c \
	@GMP_TREE@/mpn/generic/diveby3.c \
	@GMP_TREE@/mpn/generic/divis.c \
	@GMP_TREE@/mpn/generic/dump.c \
	@GMP_TREE@/mpn/generic/fib2_ui.c \
	@GMP_TREE@/mpn/generic/gcd.c \
	@GMP_TREE@/mpn/generic/gcd_1.c \
	@GMP_TREE@/mpn/generic/get_d.c \
	@GMP_TREE@/mpn/generic/hgcd2_jacobi.c \
	@GMP_TREE@/mpn/generic/hgcd_jacobi.c \
	@GMP_TREE@/mpn/generic/invert.c \
	@GMP_TREE@/mpn/generic/jacbase.c \
	@GMP_TREE@/mpn/generic/jacobi.c \
	@GMP_TREE@/mpn/generic/jacobi_2.c \
	@GMP_TREE@/mpn/generic/mod_1.c \
	@GMP_TREE@/mpn/generic/mod_1_1.c \
	@GMP_TREE@/mpn/generic/mod_1_2.c \
	@GMP_TREE@/mpn/generic/mod_1_3.c \
	@GMP_TREE@/mpn/generic/mod_1_4.c \
	@GMP_TREE@/mpn/generic/mod_34lsub1.c \
	@GMP_TREE@/mpn/generic/mode1o.c \
	@GMP_TREE@/mpn/generic/mu_bdiv_qr.c \
	@GMP_TREE@/mpn/generic/mulmid.c \
	@GMP_TREE@/mpn/generic/mulmid_basecase.c \
	@GMP_TREE@/mpn/generic/mulmid_n.c \
	@GMP_TREE@/mpn/generic/perfpow.c \
	@GMP_TREE@/mpn/generic/perfsqr.c \
	@GMP_TREE@/mpn/generic/pow_1.c \
	@GMP_TREE@/mpn/generic/pre_mod_1.c \
	@GMP_TREE@/mpn/generic/random.c \
	@GMP_TREE@/mpn/generic/random2.c \
	@GMP_TREE@/mpn/generic/redc_2.c \
	@GMP_TREE@/mpn/generic/remove.c \
	@GMP_TREE@/mpn/generic/rootrem.c \
	@GMP_TREE@/mpn/generic/scan0.c \
	@GMP_TREE@/mpn/generic/sec_invert.c \
	@GMP_TREE@/mpn/generic/sec_mul.c \
	@GMP_TREE@/mpn/generic/sec_powm.c \
	@GMP_TREE@/mpn/generic/sec_sqr.c \
	@GMP_TREE@/mpn/generic/sec_tabselect.c \
	@GMP_TREE@/mpn/generic/sizeinbase.c \
	@GMP_TREE@/mpn/generic/sub_err1_n.c \
	@GMP_TREE@/mpn/generic/sub_err2_n.c \
	@GMP_TREE@/mpn/generic/sub_err3_n.c \
	@GMP_TREE@/mpn/generic/toom52_mul.c \
	@GMP_TREE@/mpn/generic/toom54_mul.c \
	@GMP_TREE@/mpn/generic/toom62_mul.c \
	@GMP_TREE@/mpn/generic/trialdiv.c \
	@GMP_TREE@/mpn/generic/zero.c

mpz_sources = \
	@GMP_TREE@/mpz/abs.c \
	@GMP_TREE@/mpz/add.c \
	@GMP_TREE@/mpz/add_ui.c \
	@GMP_TREE@/mpz/aorsmul.c \
	@GMP_TREE@/mpz/aorsmul_i.c \
	@GMP_TREE@/mpz/cfdiv_q_2exp.c \
	@GMP_TREE@/mpz/clear.c \
	@GMP_TREE@/mpz/cmp_ui.c \
	@GMP_TREE@/mpz/cmpabs.c \
	@GMP_TREE@/mpz/divexact.c \
	@GMP_TREE@/mpz/fdiv_q.c \
	@GMP_TREE@/mpz/fdiv_q_ui.c \
	@GMP_TREE@/mpz/fdiv_qr.c \
	@GMP_TREE@/mpz/gcdext.c \
	@GMP_TREE@/mpz/get_si.c \
	@GMP_TREE@/mpz/init.c \
	@GMP_TREE@/mpz/invert.c \
	@GMP_TREE@/mpz/iset.c \
	@GMP_TREE@/mpz/iset_ui.c \
	@GMP_TREE@/mpz/mod.c \
	@GMP_TREE@/mpz/mul.c \
	@GMP_TREE@/mpz/mul_2exp.c \
	@GMP_TREE@/mpz/mul_si.c \
	@GMP_TREE@/mpz/mul_ui.c \
	@GMP_TREE@/mpz/n_pow_ui.c \
	@GMP_TREE@/mpz/neg.c \
	@GMP_TREE@/mpz/powm.c \
	@GMP_TREE@/mpz/powm_ui.c \
	@GMP_TREE@/mpz/realloc.c \
	@GMP_TREE@/mpz/scan1.c \
	@GMP_TREE@/mpz/set.c \
	@GMP_TREE@/mpz/size.c \
	@GMP_TREE@/mpz/set_ui.c \
	@GMP_TREE@/mpz/sizeinbase.c \
	@GMP_TREE@/mpz/sqrt.c \
	@GMP_TREE@/mpz/sub.c \
	@GMP_TREE@/mpz/sub_ui.c \
	@GMP_TREE@/mpz/swap.c \
	@GMP_TREE@/mpz/tdiv_q.c \
	@GMP_TREE@/mpz/tdiv_q_2exp.c \
	@GMP_TREE@/mpz/tdiv_qr.c \
	@GMP_TREE@/mpz/tdiv_r.c \
	@GMP_TREE@/mpz/tdiv_r_2exp.c \
	@GMP_TREE@/mpz/tstbit.c \
	@GMP_TREE@/mpz/ui_pow_ui.c

mpz_unused = \
	@GMP_TREE@/mpz/2fac_ui.c \
	@GMP_TREE@/mpz/and.c \
	@GMP_TREE@/mpz/array_init.c \
	@GMP_TREE@/mpz/bin_ui.c \
	@GMP_TREE@/mpz/bin_uiui.c \
	@GMP_TREE@/mpz/cdiv_q.c \
	@GMP_TREE@/mpz/cdiv_q_ui.c \
	@GMP_TREE@/mpz/cdiv_qr.c \
	@GMP_TREE@/mpz/cdiv_qr_ui.c \
	@GMP_TREE@/mpz/cdiv_r.c \
	@GMP_TREE@/mpz/cdiv_r_ui.c \
	@GMP_TREE@/mpz/cdiv_ui.c \
	@GMP_TREE@/mpz/cfdiv_r_2exp.c \
	@GMP_TREE@/mpz/clears.c \
	@GMP_TREE@/mpz/clrbit.c \
	@GMP_TREE@/mpz/cmp.c \
	@GMP_TREE@/mpz/cmp_d.c \
	@GMP_TREE@/mpz/cmp_si.c \
	@GMP_TREE@/mpz/cmpabs_d.c \
	@GMP_TREE@/mpz/cmpabs_ui.c \
	@GMP_TREE@/mpz/com.c \
	@GMP_TREE@/mpz/combit.c \
	@GMP_TREE@/mpz/cong.c \
	@GMP_TREE@/mpz/cong_2exp.c \
	@GMP_TREE@/mpz/cong_ui.c \
	@GMP_TREE@/mpz/dive_ui.c \
	@GMP_TREE@/mpz/divegcd.c \
	@GMP_TREE@/mpz/divis.c \
	@GMP_TREE@/mpz/divis_2exp.c \
	@GMP_TREE@/mpz/divis_ui.c \
	@GMP_TREE@/mpz/dump.c \
	@GMP_TREE@/mpz/export.c \
	@GMP_TREE@/mpz/fac_ui.c \
	@GMP_TREE@/mpz/fdiv_qr_ui.c \
	@GMP_TREE@/mpz/fdiv_r.c \
	@GMP_TREE@/mpz/fdiv_r_ui.c \
	@GMP_TREE@/mpz/fdiv_ui.c \
	@GMP_TREE@/mpz/fib2_ui.c \
	@GMP_TREE@/mpz/fib_ui.c \
	@GMP_TREE@/mpz/fits_sint.c \
	@GMP_TREE@/mpz/fits_slong.c \
	@GMP_TREE@/mpz/fits_sshort.c \
	@GMP_TREE@/mpz/fits_uint.c \
	@GMP_TREE@/mpz/fits_ulong.c \
	@GMP_TREE@/mpz/fits_ushort.c \
	@GMP_TREE@/mpz/gcd.c \
	@GMP_TREE@/mpz/gcd_ui.c \
	@GMP_TREE@/mpz/get_d.c \
	@GMP_TREE@/mpz/get_d_2exp.c \
	@GMP_TREE@/mpz/get_str.c \
	@GMP_TREE@/mpz/get_ui.c \
	@GMP_TREE@/mpz/getlimbn.c \
	@GMP_TREE@/mpz/hamdist.c \
	@GMP_TREE@/mpz/import.c \
	@GMP_TREE@/mpz/init2.c \
	@GMP_TREE@/mpz/inits.c \
	@GMP_TREE@/mpz/inp_raw.c \
	@GMP_TREE@/mpz/inp_str.c \
	@GMP_TREE@/mpz/ior.c \
	@GMP_TREE@/mpz/iset_d.c \
	@GMP_TREE@/mpz/iset_si.c \
	@GMP_TREE@/mpz/iset_str.c \
	@GMP_TREE@/mpz/jacobi.c \
	@GMP_TREE@/mpz/kronsz.c \
	@GMP_TREE@/mpz/kronuz.c \
	@GMP_TREE@/mpz/kronzs.c \
	@GMP_TREE@/mpz/kronzu.c \
	@GMP_TREE@/mpz/lcm.c \
	@GMP_TREE@/mpz/lcm_ui.c \
	@GMP_TREE@/mpz/limbs_finish.c \
	@GMP_TREE@/mpz/limbs_modify.c \
	@GMP_TREE@/mpz/limbs_read.c \
	@GMP_TREE@/mpz/limbs_write.c \
	@GMP_TREE@/mpz/lucnum2_ui.c \
	@GMP_TREE@/mpz/lucnum_ui.c \
	@GMP_TREE@/mpz/mfac_uiui.c \
	@GMP_TREE@/mpz/millerrabin.c \
	@GMP_TREE@/mpz/nextprime.c \
	@GMP_TREE@/mpz/oddfac_1.c \
	@GMP_TREE@/mpz/out_raw.c \
	@GMP_TREE@/mpz/out_str.c \
	@GMP_TREE@/mpz/perfpow.c \
	@GMP_TREE@/mpz/perfsqr.c \
	@GMP_TREE@/mpz/popcount.c \
	@GMP_TREE@/mpz/pow_ui.c \
	@GMP_TREE@/mpz/powm_sec.c \
	@GMP_TREE@/mpz/pprime_p.c \
	@GMP_TREE@/mpz/primorial_ui.c \
	@GMP_TREE@/mpz/prodlimbs.c \
	@GMP_TREE@/mpz/random.c \
	@GMP_TREE@/mpz/random2.c \
	@GMP_TREE@/mpz/realloc2.c \
	@GMP_TREE@/mpz/remove.c \
	@GMP_TREE@/mpz/roinit_n.c \
	@GMP_TREE@/mpz/root.c \
	@GMP_TREE@/mpz/rootrem.c \
	@GMP_TREE@/mpz/rrandomb.c \
	@GMP_TREE@/mpz/scan0.c \
	@GMP_TREE@/mpz/set_d.c \
	@GMP_TREE@/mpz/set_f.c \
	@GMP_TREE@/mpz/set_q.c \
	@GMP_TREE@/mpz/set_si.c \
	@GMP_TREE@/mpz/set_str.c \
	@GMP_TREE@/mpz/setbit.c \
	@GMP_TREE@/mpz/sqrtrem.c \
	@GMP_TREE@/mpz/tdiv_q_ui.c \
	@GMP_TREE@/mpz/tdiv_qr_ui.c \
	@GMP_TREE@/mpz/tdiv_r_ui.c \
	@GMP_TREE@/mpz/tdiv_ui.c \
	@GMP_TREE@/mpz/ui_sub.c \
	@GMP_TREE@/mpz/urandomb.c \
	@GMP_TREE@/mpz/urandomm.c \
	@GMP_TREE@/mpz/xor.c

DISTCLEANFILES =

native/stamp-bases:
	cd native && $(MAKE) $(AM_MAKEFLAGS) stamp-bases
mp_bases.h: native/stamp-bases
	$(AM_V_GEN)native/gen-bases header $(GMP_LIMB_BITS) $(GMP_NAIL_BITS) >$@ || (rm -f $@; exit 1)
mp_bases.c: mp_bases.h
	$(AM_V_GEN)native/gen-bases table $(GMP_LIMB_BITS) $(GMP_NAIL_BITS) >$@ || (rm -f $@; exit 1)
DISTCLEANFILES += mp_bases.h mp_bases.c

native/stamp-fac:
	cd native && $(MAKE) $(AM_MAKEFLAGS) stamp-fac
fac_table.h: native/stamp-fac
	$(AM_V_GEN)native/gen-fac $(GMP_LIMB_BITS) $(GMP_NAIL_BITS) >$@ || (rm -f $@; exit 1)
DISTCLEANFILES += fac_table.h

native/stamp-fib:
	cd native && $(MAKE) $(AM_MAKEFLAGS) stamp-fib
fib_table.h: native/stamp-fib
	$(AM_V_GEN)native/gen-fib header $(GMP_LIMB_BITS) $(GMP_NAIL_BITS) >$@ || (rm -f $@; exit 1)
fib_table.c: fib_table.h
	$(AM_V_GEN)native/gen-fib table $(GMP_LIMB_BITS) $(GMP_NAIL_BITS) >$@ || (rm -f $@; exit 1)
DISTCLEANFILES += fib_table.h fib_table.c

native/stamp-jacobitab:
	cd native && $(MAKE) $(AM_MAKEFLAGS) stamp-jacobitab
jacobitab.h: native/stamp-jacobitab
	$(AM_V_GEN)native/gen-jacobitab >$@ || (rm -f $@; exit 1)
DISTCLEANFILES += jacobitab.h

native/stamp-psqr:
	cd native && $(MAKE) $(AM_MAKEFLAGS) stamp-psqr
perfsqr.h: native/stamp-psqr
	$(AM_V_GEN)native/gen-psqr $(GMP_LIMB_BITS) $(GMP_NAIL_BITS) >$@ || (rm -f $@; exit 1)
DISTCLEANFILES += perfsqr.h

native/stamp-trialdivtab:
	cd native && $(MAKE) $(AM_MAKEFLAGS) stamp-trialdivtab
trialdivtab.h: native/stamp-trialdivtab
	$(AM_V_GEN)native/gen-trialdivtab $(GMP_LIMB_BITS) 8000 >$@ || (rm -f $@; exit 1)
DISTCLEANFILES += trialdivtab.h

if build
check_PROGRAMS = gmptest
dist_check_SCRIPTS = gmp.test
TESTS = gmp.test
endif build
gmp.log: gmptest$(EXEEXT)

gmptest_SOURCES = gmptest.c

gmptest_CPPFLAGS = -Iinclude

LDADD = libgmp.a

# Reconfig
reconfig_prereq =

include $(srcdir)/../../am/reconfig.am

# Rebuild
rebuild_prereq =
rebuild_target = all
CLEANFILES =

include $(srcdir)/../../am/rebuild.am

