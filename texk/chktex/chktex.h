/*
 *  ChkTeX v1.5, header file for main program.
 *  Copyright (C) 1995-96 Jens T. Berger Thielemann
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Contact the author at:
 *              Jens Berger
 *              Spektrumvn. 4
 *              N-0666 Oslo
 *              Norway
 *              E-mail: <jensthi@ifi.uio.no>
 *
 *
 */

#ifndef CHKTEX_H
#define CHKTEX_H 1

#ifdef WIN32
#  include <win32lib.h>
#  include "config.h"
#else

#if defined(HAVE_CONFIG_H) || defined(CONFIG_H_NAME)
#  ifndef CONFIG_H_NAME
#    define  CONFIG_H_NAME   "config.h"
#  endif
#  ifndef HAVE_CONFIG_H
#    define HAVE_CONFIG_H 1
#  endif
#  include CONFIG_H_NAME
#endif

#endif

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#ifdef HAVE_STDARG_H
#  include <stdarg.h>
#else
#error Sorry, ChkTeX needs an _ANSI_ compiler w/stdarg.h installed to compile.
#endif
#ifndef HAVE_VPRINTF
#error Sorry, ChkTeX needs an _ANSI_ compiler w/vprintf() to compile.
#endif
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef HAVE_LIMITS_H
#  include <limits.h>
#endif
#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif
#include "chktypes.h"
#ifndef __NOPROTO
#  ifndef __PROTO
#    define __PROTO(a) a
#  endif
#else
#  ifndef __PROTO
#    define __PROTO(a) ()
#  endif
#endif
#ifndef LONG_MAX
#  define LONG_MAX (0x7fffffff)
#endif
#ifndef CHAR_BIT
#  define CHAR_BIT (8)
#endif
#ifndef FINDERRS_H
# include "FindErrs.h"
#endif /* FINDERRS_H */

#ifndef CDECL
#define CDECL
#endif

/*
 * Define aliases for preprocessor statements
 */

#if defined(__unix__) || defined(__unix) || defined(unix)
#undef __unix__
#define __unix__ 1
#elif defined(AMIGA) || defined(__AMIGA) || defined(__amigados__)
#undef AMIGA
#define AMIGA 1
#elif defined(MSDOS) || defined(__MSDOS__) || defined(__msdos__)
#undef __MSDOS__
#define __MSDOS__ 1
#endif

/*
 * The next macro should contain a list of characters LaTeX (and you?)
 * considers as an end-of-sentence characters, which should be
 * detected when whether sentence spacing is correct.
 *
 */

#define LATEX_EOSPUNC        CHAR('.') CHAR(':') CHAR('?') CHAR('!')

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */

/*
 * The next macro should contain general punctuation characters
 * used on your system.
 */

#define LATEX_GENPUNC         LATEX_EOSPUNC CHAR(',') CHAR(';')


/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */

/*
 * The next macro should contain a list of characters LaTeX (and you?)
 * considers as an small punctuation characters, which should not be
 * preceded by a \/.
 */

#define LATEX_SMALLPUNC       CHAR('.') CHAR(',')

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */

/*
 * NOTE: All macros below should return FALSE (0) if the input
 * character is 0.
 *
 * The next macro should return TRUE if LaTeX (and you?) considers
 * the character `c' as a space, which should be detected when
 * we're checking whether commands are terminated by spaces.
 *
 * Note: If you wish to change this macro, you'll have to update
 * the main loop in FindErrs.c as well.
 */

#define LATEX_SPACE(c)  (c > 0 && c <= ' ')

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */






#define betw(a,b,c)             ((a < b) && (b < c))
#define within(a,b,c)           ((a <= b) && (b <= c))
#define elif                    else if

#ifndef NOT
#  define NOT(a)                   (!(a))
#endif
#define ifn(a)                  if(NOT(a))

#ifndef min
#  define min(a,b) ((a)<=(b)?(a):(b))
#endif

#ifndef max
#  define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef abs
#  define abs(a) (a>=0?a:(-a))
#endif

#define LOOP(name, body)  {{body} _end__##name:;}
#define LAST(name)        goto _end__##name

#define NUMBRACKETS          6

#define NEWBUF(name,len) \
static TEXT _ ## name [len  + (WALLBYTES<<1)] = {0};  \
const STRPTR name = &_ ## name[WALLBYTES]

#define INTFAULTMSG "INTERNAL FAULT OCCURED! PLEASE SUBMIT A BUG REPORT!\n"

#define INTERNFAULT   INTFAULTMSG

#define BITDEF1(a)    BIT_ ## a
#define BITDEF2(a)    FLG_ ## a = (1<<BIT_ ## a)

/*
 * Debugging info
 */

#define DEBUGBITS(a) enum a { \
    BIT(DbgMsgs),        /* Dump messages */ \
    BIT(DbgListInfo),    /* Dump list info */ \
    BIT(DbgListCont),    /* Dump list contents */ \
    BIT(DbgOtherInfo),   /* Dump misc. other info */ \
    BIT(DbgRunTime)      /* Dump runtime info */ \
};

#undef BIT
#define BIT BITDEF1
DEBUGBITS(Debug_BIT)
#undef BIT
#define BIT BITDEF2
DEBUGBITS(Debug_FLG)

#ifdef STRIP_DEBUG
#  define DEBUG(a)
#else
#  define DEBUG(a)    if(DebugLevel & FLG_DbgRunTime) ErrPrintf a
#endif

#define PRGMSGS \
 MSG(pmMinFault, etWarn, TRUE, 0, INTERNFAULT)\
 MSG(pmNoFileMatch, etWarn, TRUE, 0,\
     "No files matched the pattern `%s'.")\
 MSG(pmNoTeXOpen,   etWarn,  TRUE, 0,\
     "Unable to open the TeX file `%s'.")\
 MSG(pmRename,      etMsg,  TRUE, 0,\
     "Renaming `%s' as `%s'.")\
 MSG(pmRenameErr,   etErr,  TRUE, 0,\
     "Could not rename `%s' to `%s'.")\
 MSG(pmOutOpen,     etErr,  TRUE, 0,\
     "Unable to open output file.")\
 MSG(pmOutTwice,    etErr,  TRUE, 0,\
     "You can specify output file only once.")\
 MSG(pmStrDupErr,   etErr,  TRUE, 0,\
     "Unable to duplicate strings - no memory?")\
 MSG(pmWordListErr, etErr,  TRUE, 0,\
     "Unable to create wordlist - no memory?")\
 MSG(pmNoStackMem,  etErr,  TRUE, 0,\
     "Unable to create stack - no memory?\n")\
 MSG(pmWarnNumErr,  etErr,  TRUE, 0,\
     "Illegal warning number used.")\
 MSG(pmVerbLevErr,  etErr,  TRUE, 0,\
     "Illegal verbosity level.")\
 MSG(pmNotPSDigit,  etWarn, TRUE, 0,\
     "`%c' is not a %s digit - ignored!")\
 MSG(pmEscCode,     etWarn, TRUE, 0,\
     "Unknown escape code `%c%c' - ignored!")\
 MSG(pmKeyWord,     etErr,  TRUE, 0,\
     "Unsupported control word (`%s') encountered in file `%s'.")\
 MSG(pmFaultFmt,    etErr,  TRUE, 0,\
     "\"%s\", line %d: Faulty format - unexpected %s found.")\
 MSG(pmRsrcOpen,    etWarn, TRUE, 0,\
     "Could not open `%s', may cause unwanted behaviour.")\
 MSG(pmSlowAbbr,    etMsg,  FALSE, 0,\
     "The abbreviation `%s' requires slow abbreviation searching.")\
 MSG(pmEmptyToken,  etWarn,  TRUE,  0,\
     "Empty token isolated in `%s' - probably faulty format")\
 MSG(pmAssert,      etErr,  TRUE,  0,\
     "Assertion failed. Please report bug.")\
 MSG(pmNoRsrc,      etWarn, TRUE, 0,\
     "Could not find global resource file.")\
 MSG(pmQuoteStyle,    etWarn, TRUE, 0,\
     "Illegal quotestyle `%s'.")\
 MSG(pmWrongWipeTemp, etWarn, TRUE, 0,\
     "Wrong template for wiping arguments, \"%s\"")\
 MSG(pmSpecifyTerm, etErr, TRUE, 0,\
     "Specify a terminal type with `setenv TERM <yourtype>'.")\
 MSG(pmNoTermData, etErr, TRUE, 0,\
     "Could not access the termcap data base.")\
 MSG(pmNoTermDefd, etErr, TRUE, 0,\
     "Terminal type `%s' is not defined.")\
 MSG(pmNoRegExp,   etWarn, TRUE, 0,\
     "Regular expressions not compiled in. Please recompile.")\
 MSG(pmNoCommon,   etErr, TRUE, 0,\
     "`%s' can't be a member of both `%s' and `%s'.") \
 MSG(pmNoOpenDir,  etWarn, TRUE, 0, \
     "Could not open the directory `%s'.") \
 MSG(pmNoDebugFlag, etWarn, TRUE, 0, \
     "This version of doesn't support the -d flag. Please recompile.")


#undef MSG
#define MSG(num, type, inuse, ctxt, text) num,

enum PrgErrNum {
  PRGMSGS
  pmMaxFault
};
#undef MSG

extern struct ErrMsg  PrgMsgs [pmMaxFault + 1];

struct ErrInfo {
    STRPTR Data, LineBuf, File;
    ULONG  Line, Column, ErrLen;
    enum
    {
        efNone        = 0x00,
        efNoItal      = 0x01,
        efItal        = 0x02
    } Flags;
};

extern const STRPTR
    ReadBuffer,
    CmdBuffer,
    TmpBuffer;

extern const
    TEXT BrOrder[NUMBRACKETS + 1];

extern
    ULONG Brackets  [NUMBRACKETS];


extern FILE *OutputFile, *InputFile;

#define OPTION_DEFAULTS \
  DEF(BOOL, GlobalRC,    TRUE) \
  DEF(BOOL, WipeVerb,    TRUE) \
  DEF(BOOL, BackupOut,   TRUE) \
  DEF(BOOL, Quiet,       FALSE) \
  DEF(BOOL, LicenseOnly, FALSE) \
  DEF(BOOL, UsingStdIn,  FALSE) \
  DEF(BOOL, InputFiles,  TRUE) \
  DEF(BOOL, HeadErrOut,  TRUE) \
  DEF(STRPTR, OutputName, "") \
  DEF(STRPTR, PseudoInName, NULL) \
  DEF(STRPTR, OutputFormat, VerbNormal) \
  DEF(STRPTR, PipeOutputFormat, NULL) \
  DEF(STRPTR, Delimit, ":") \
  DEF(LONG,  DebugLevel, 0)

#define STATE_VARS \
  DEF(enum ItState, ItState, itOff) /* Are we doing italics? */ \
  DEF(BOOL, AtLetter, FALSE) /* Whether `@' is a letter or not. */ \
  DEF(BOOL, InHeader, TRUE)  /* Whether we're in the header */ \
  DEF(BOOL, VerbMode, FALSE) /* Whether we're in complete ignore-mode */ \
  DEF(LONG, MathMode, 0)     /* Whether we're in math mode or not */ \
  DEF(STRPTR, VerbStr, "")   /* String we'll terminate verbmode upon */ \
  DEF(ULONG, ErrPrint, 0)    /* # errors printed */ \
  DEF(ULONG, WarnPrint, 0)   /* # warnings printed */ \
  DEF(ULONG, UserSupp, 0)    /* # user suppressed warnings */

#define DEF(type, name, value) extern type name;
OPTION_DEFAULTS
STATE_VARS
#undef DEF

extern struct Stack
     CharStack,
     InputStack,
     EnvStack;

enum Quote {
    quLogic,
    quTrad
};

extern enum Quote Quote;
extern BOOL StdInTTY, StdOutTTY, UsingStdIn;

int     main            __PROTO((int argc, char **argv));
void    PrintPrgErr     __PROTO((enum PrgErrNum , ...));
void    ErrPrintf       __PROTO((const char *fmt, ...));

extern TEXT LTX_EosPunc[], LTX_GenPunc[], LTX_SmallPunc[];
extern STRPTR PrgName;

#endif /* CHKTEX_H */
