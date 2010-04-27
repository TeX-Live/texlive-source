## Define configure options for lcdf-typetools.  Extracted from configure.ac
## for ease of building TeX Live.
m4_define_default([kpse_indent_26], [26])[]dnl
m4_define([kpse_lcdf_typetools_progs], [cfftot1 mmafm mmpfb otfinfo otftotfm t1dotlessj t1lint t1rawafm t1reencode t1testpage ttftotype42])[]dnl
AC_FOREACH([Kpse_Prog], kpse_lcdf_typetools_progs,
           [AC_ARG_ENABLE(Kpse_Prog,
                          AS_HELP_STRING([--disable-]Kpse_Prog,
                                         [do not build the ]Kpse_Prog[ program],
                                         kpse_indent_26))])
m4_define([kpse_otftotfm_auto_opts], [cfftot1 t1dotlessj ttftotype42 updmap])[]dnl
AC_FOREACH([Kpse_Opt], kpse_otftotfm_auto_opts,
           [AC_ARG_ENABLE(Kpse_Opt,
                          AS_HELP_STRING([--disable-auto-]Kpse_Opt,
                                         [disable running ]Kpse_Opt[ from otftotfm],
                                         kpse_indent_26))])
