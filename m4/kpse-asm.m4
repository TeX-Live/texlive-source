# Private macro for the TeX Live (TL) tree.
# Copyright (C) 1996 - 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 3

# KPSE_ASM_UNDERSCORE
# -------------------
# Check if gcc asm for i386 needs external symbols with an underscore.
AC_DEFUN([KPSE_ASM_UNDERSCORE],
[AC_CACHE_CHECK([whether gcc asm needs underscore],
                [pb_cv_asm_underscore],
                [
# Some versions of GCC asm for i386 need an underscore prepended to
# external symbols. Figure out if this is so.
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
extern char val ;
extern void sub () ;
#if defined (__i386__) && defined (__GNUC__) 
asm("        .align 4\n"
".globl sub\n"
"sub:\n"
"        movb \$][1,val\n"
"        ret\n");
#else
void sub () { val = 1; }
#endif /* assembler */
char val ;]],
                                [[sub (); return 0;]])],
               [pb_cv_asm_underscore=no],
               [pb_cv_asm_underscore=yes])])
if test "x$pb_cv_asm_underscore" = xyes; then
  AC_DEFINE([ASM_NEEDS_UNDERSCORE], 1,
            [web2c: Define if gcc asm needs _ on external symbols.])
fi
]) # KPSE_ASM_UNDERSCORE

