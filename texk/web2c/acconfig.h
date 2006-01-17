/* acconfig.h -- used by autoheader when generating c-auto.in.

   If you're thinking of editing acconfig.h to fix a configuration
   problem, don't. Edit the c-auto.h file created by configure,
   instead.  Even better, fix configure to give the right answer.  */

/* Guard against double inclusion. */
#ifndef WEB2C_C_AUTO_H
#define WEB2C_C_AUTO_H

/* web2c: the version string. */
#define WEB2CVERSION "REPLACE-WITH-WEB2CVERSION"

@TOP@

/* Define if you get clashes concerning wchar_t, between X's include
   files and system includes.  */
#undef FOIL_X_WCHAR_T

/* tex: Define to enable --ipc.  */
#undef IPC

/* web2c: Define if gcc asm needs _ on external symbols.  */
#undef ASM_NEEDS_UNDERSCORE

/* web2c: Define when using system-specific files for arithmetic.  */
#undef ASM_SCALED_FRACTION

/* web2c: Define to enable HackyInputFileNameForCoreDump.tex.  */
#undef FUNNY_CORE_DUMP

/* web2c: Define to disable architecture-independent dump files.
   Faster on LittleEndian architectures.  */
#undef NO_DUMP_SHARE

/* web2c: Default editor for interactive `e' option. */
#define EDITOR "vi +%d %s"

/* web2c: Window system support for Metafont. */
#undef EPSFWIN
#undef HP2627WIN
#undef MFTALKWIN
#undef NEXTWIN
#undef REGISWIN
#undef SUNWIN
#undef TEKTRONIXWIN
#undef UNITERMWIN
#undef X11WIN

@BOTTOM@
#endif /* !WEB2C_C_AUTO_H */
