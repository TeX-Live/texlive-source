lib_LTLIBRARIES = libmpfi.la

# apparently the type "intmax_t" s not always known
# and consequently set_sj, set_uj cause trouble
#libmpfi_la_SOURCES = mpfi-impl.h abs.c acos.c acosh.c add.c	\
#   add_d.c add_fr.c add_q.c add_si.c add_ui.c add_z.c alea.c	\
#   asin.c asinh.c atan2.c atan.c atanh.c bisect.c blow.c	\
#   cbrt.c clear.c clears.c cmp.c cmp_sym_pi.c constants.c cos.c	\
#   cosh.c csc.c csch.c cot.c coth.c d_div.c d_sub.c diam.c	\
#   div.c div_2exp.c div_2si.c div_2ui.c div_d.c div_ext.c div_fr.c	\
#   div_q.c div_si.c div_ui.c div_z.c erandom.c error.c exp.c 	\
#   exp2.c exp10.c expm1.c exp2m1.c exp10m1.c fr_div.c fr_sub.c get_endpoints.c get_fr.c	\
#   get_d.c get_prec.c get_version.c has_zero.c hypot.c		\
#   increase.c init.c init2.c inits.c inits2.c intersect.c       \
#   interv_d.c interv_fr.c					\
#   interv_q.c interv_si.c interv_ui.c interv_z.c inp_str.c	\
#   inv.c is_empty.c is_inside.c log.c log10.c log1p.c log2.c log2p1.c log10p1.c	\
#   mag.c mid.c mig.c mul.c mul_2exp.c mul_2si.c mul_2ui.c	\
#   mul_d.c mul_fr.c mul_q.c mul_si.c mul_ui.c mul_z.c neg.c	\
#   nrandom.c out_str.c predicates.c print_binary.c put.c 	\
#   put_d.c put_fr.c put_q.c put_si.c put_ui.c put_z.c q_div.c	\
#   q_sub.c quadrant.c rec_sqrt.c revert_if_needed.c round_prec.c sec.c	\
#   sech.c set.c set_d.c set_flt.c set_fr.c set_ld.c set_prec.c set_q.c	\
#   set_si.c set_sj.c set_str.c set_ui.c set_uj.c set_z.c	\
#   si_div.c si_sub.c sign.c sin.c				\
#   sinh.c sqr.c sqrt.c sub.c sub_d.c sub_fr.c sub_q.c		\
#   sub_si.c sub_ui.c sub_z.c swap.c tan.c tanh.c ui_div.c	\
#   ui_sub.c union.c urandom.c z_div.c z_sub.c

libmpfi_la_SOURCES = mpfi-impl.h abs.c acos.c acosh.c add.c	\
   add_d.c add_fr.c add_q.c add_si.c add_ui.c add_z.c alea.c	\
   asin.c asinh.c atan2.c atan.c atanh.c bisect.c blow.c	\
   cbrt.c clear.c clears.c cmp.c cmp_sym_pi.c constants.c cos.c	\
   cosh.c csc.c csch.c cot.c coth.c d_div.c d_sub.c diam.c	\
   div.c div_2exp.c div_2si.c div_2ui.c div_d.c div_ext.c div_fr.c	\
   div_q.c div_si.c div_ui.c div_z.c erandom.c error.c exp.c 	\
   exp2.c exp10.c expm1.c exp2m1.c exp10m1.c fr_div.c fr_sub.c get_endpoints.c get_fr.c	\
   get_d.c get_prec.c get_version.c has_zero.c hypot.c		\
   increase.c init.c init2.c inits.c inits2.c intersect.c       \
   interv_d.c interv_fr.c					\
   interv_q.c interv_si.c interv_ui.c interv_z.c inp_str.c	\
   inv.c is_empty.c is_inside.c log.c log10.c log1p.c log2.c log2p1.c log10p1.c	\
   mag.c mid.c mig.c mul.c mul_2exp.c mul_2si.c mul_2ui.c	\
   mul_d.c mul_fr.c mul_q.c mul_si.c mul_ui.c mul_z.c neg.c	\
   nrandom.c out_str.c predicates.c print_binary.c put.c 	\
   put_d.c put_fr.c put_q.c put_si.c put_ui.c put_z.c q_div.c	\
   q_sub.c quadrant.c rec_sqrt.c revert_if_needed.c round_prec.c sec.c	\
   sech.c set.c set_d.c set_flt.c set_fr.c set_ld.c set_prec.c set_q.c	\
   set_si.c set_str.c set_ui.c set_z.c				\
   si_div.c si_sub.c sign.c sin.c				\
   sinh.c sqr.c sqrt.c sub.c sub_d.c sub_fr.c sub_q.c		\
   sub_si.c sub_ui.c sub_z.c swap.c tan.c tanh.c ui_div.c	\
   ui_sub.c union.c urandom.c z_div.c z_sub.c
libmpfi_la_LDFLAGS = -no-undefined

AM_CPPFLAGS =-I$(top_srcdir)
