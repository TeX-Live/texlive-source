/* Some definitions that get appended to the `coerce.h' file that web2c
   outputs.  */

/* The C compiler ignores most unnecessary casts (i.e., casts of
   something to its own type).  However, for structures, it doesn't.
   Therefore, we have to redefine these macros so they don't cast
   cast their argument (of type memoryword or fourquarters,
   respectively).  */
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

/* And we use the opportunity to declare a few functions that could not be
   declared in texmfmp.h, because they need typedefs not yet known at that
   point.  */
extern strnumber getjobname P1H(strnumber);

#ifdef MP
/* MP defined poolASCIIcode instead of packedASCIIcode, sigh. */
typedef poolASCIIcode packedASCIIcode;
#endif
#ifdef XeTeX
/* XeTeX redefines "ASCII" types.... */
typedef packedUTF16code packedASCIIcode;
#endif
extern void calledit P4H(packedASCIIcode *, poolpointer, integer, integer);

#ifdef MF
extern void blankrectangle P4H(screencol, screencol, screenrow, screenrow);
extern void paintrow P4H(screenrow, pixelcolor, transspec, screencol);
#endif

#ifdef TeX
/* Prototypes for source-specials functions... */
extern strnumber makefullnamestring();
extern boolean isnewsource P2H(strnumber, int);
extern poolpointer makesrcspecial P2H(strnumber, int);
extern void remembersourceinfo P2H(strnumber, int);
#endif

#ifdef pdfTeX
#include <pdftexdir/pdftex.h>
#endif /* pdfTeX */

#ifdef XeTeX
#include <xetexdir/xetex.h>
#endif /* XeTeX */
