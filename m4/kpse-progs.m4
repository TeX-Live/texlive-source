# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2011 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_CHECK_LATEX
# ----------------
AC_DEFUN([KPSE_CHECK_LATEX],
[AC_ARG_VAR([LATEX], [LaTeX command])
AC_CHECK_PROGS([LATEX], [latex elatex lambda], [no])])

# KPSE_CHECK_PDFLATEX
# -------------------
AC_DEFUN([KPSE_CHECK_PDFLATEX],
[AC_ARG_VAR([PDFLATEX], [pdfLaTeX command])
AC_CHECK_PROGS([PDFLATEX], [pdflatex], [no])])

# KPSE_CHECK_PERL
# ---------------
AC_DEFUN([KPSE_CHECK_PERL],
[AC_ARG_VAR([PERL], [Perl interpreter command])
AC_CHECK_PROGS([PERL], [perl perl5], [no])])

