/* Some definitions that get appended to the `coerce.h' file that web2c
   outputs.  */
/* $Id$ */

/* The C compiler ignores most unnecessary casts (i.e., casts of
   something to its own type).  However, for structures, it doesn't.
   Therefore, we have to redefine these macros so they don't cast
   their argument (of type memoryword or fourquarters, respectively).  */

#ifdef luaTeX

#ifdef	print_word
#undef	print_word
#define	print_word(x)	zprint_word (x)
#endif

/* luatex doesn't have tfm_qqqq */

#ifdef	eq_destroy
#undef	eq_destroy
#define	eq_destroy(x)	zeq_destroy(x)
#endif

#ifdef  synch_p_with_c
#undef  synch_p_with_c
#define synch_p_with_c(x) zsynch_p_with_c(x)
#endif

#else /* luaTeX */

#ifdef	printword
#undef	printword
#define	printword(x)	zprintword (x)
#endif

#ifdef	tfmqqqq
#undef	tfmqqqq
#define tfmqqqq(x)	ztfmqqqq (x)
#endif

#ifdef	eqdestroy
#undef	eqdestroy
#define	eqdestroy(x)	zeqdestroy(x)
#endif

#endif

/* And we use the opportunity to declare a few functions that could not be
   declared in texmfmp.h, because they need typedefs not yet known at that
   point.  */
extern strnumber getjobname (strnumber);

#ifdef XeTeX
/* XeTeX redefines "ASCII" types.... */
typedef packedUTF16code packedASCIIcode;
#endif
extern void calledit (packedASCIIcode *, poolpointer, integer, integer);

#ifdef MF
extern void blankrectangle (screencol, screencol, screenrow, screenrow);
extern void paintrow (screenrow, pixelcolor, transspec, screencol);
#endif

extern strnumber makefullnamestring(void);

#ifdef TeX
#ifndef luaTeX
extern string gettexstring (strnumber);
#endif
/* Prototypes for source-specials functions... */
extern boolean isnewsource (strnumber, int);
extern poolpointer makesrcspecial (strnumber, int);
extern void remembersourceinfo (strnumber, int);
#endif

#ifdef luaTeX
#include <luatexdir/luatex.h>
#endif /* luaTeX */

/* When compiling the lib in luatexdir, we -DpdfTeX so code can be more
   easily shared.  But we can't have both pdftexd.h and luatexd.h, etc.  */
#if defined (pdfTeX) && !defined (luaTeX)
#include <pdftexdir/pdftex.h>
#endif /* pdfTeX and not luaTeX */

#ifdef XeTeX
#include <xetexdir/xetex.h>
#endif /* XeTeX */

#if defined(TeX) && defined(__SyncTeX__)
#ifdef luaTeX
#include <luatexdir/utils/synctex.h>
#else
#include <synctexdir/synctex.h>
#endif
#endif
