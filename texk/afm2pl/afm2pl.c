/* afm2pl
 * Copyright (C) 2002, 2005, 2009 Siep Kroonenberg
 * ntg-afm2pl@ntg.nl
 * based on afm2tfm.
 */

/* Here the opening comments from afm2tfm:
 *********************************************************************
 * (Modified by Don Knuth from Tom Rokicki's pre-VPL version.)
 *
 * VM/CMS port by J. Hafner (hafner@almaden.ibm.com), based on
 * the port by Alessio Guglielmi (guglielmi@ipisnsib.bitnet)
 * and Marco Prevedelli (prevedelli@ipisnsva.bitnet).
 * This port is still in test state.  No guarantees.
 * 11/3/92: more corrections to VM/CMS port. Now it looks correct
 * and will be supported by J. Hafner.
 *
 * More changes, primarily from Karl Berry, enough for a new version
 * number to 8.0; 1 December 1996.  Note that this version computes
 * checksums differently (more intelligently).
 *
 *********************************************************************/

/* Changed features:
 * writes pl with lig- and kern info instead of tfm
 * never adds characters not explicitly specified in the encoding
 * extra ligkern info, previously hardwired in the source, is now
 * read from an external file. This may include accented char ligatures.
 * Default behavior still matches afm2tfm, given the right ligkern
 * files.
 * letterspacing option, implemented as kerning pairs between any
 * two characters.
 *
 * For VM/CMS we no longer need asciified names for the font or the
 * encoding since we write an ascii pl file instead of a binary tfm.
 *
 * This version doesn't try as hard to always give sensible output.
 * On the other hand, since it uses simpler logic, it should be
 * easier for the user to figure out the cause of failures.
 *
 * changes in structure:
 * writes character metrics directly to an array
 * metric information is transformed in stages
 * does not create smallcaps
 */

/* differences between afm and tfm:
 * afm usually has a space char
 * afm lists chars by name and also includes unencoded chars
 * afm lists ligatures together with their first `component',
 *   and lists kerns separately.
 * afm implies 3-way ligs by recursive application of 2-way glyphs
 * afm allows negative heights and depths
 *
 * tfm usually doesn't have a space char
 * tfm may have boundarychar ligkerns instead of space ligkerns
 * tfm is lists chars by code and is glyphname-agnostic
 * tfm contains a ligkern table, where entries are sorted by left
 *   `component'. 3-way ligs are written out in one lig `program'.
 * tfm may contain some special-purpose ligs such as
 *   question quoteleft =: questiondown
 */

/*******************************************************************
 * includes, defines, system */

#ifdef HAVE_CONFIG_H
#define KPATHSEA 1
#include <kpathsea/config.h>
#include <kpathsea/debug.h>
#include <kpathsea/tex-file.h>
#include "c-auto.h"
#include <kpathsea/c-ctype.h>
#include <kpathsea/progname.h>
#include <kpathsea/version.h>
#else /* HAVE_CONFIG_H */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#ifdef ATARIST
#include <float.h>
#endif
#endif /* HAVE_CONFIG_H */

#ifdef VMCMS
#define interesting lookstr     /* for 8 character truncation conflicts */
#include "dvipscms.h"
extern FILE *cmsfopen ();
extern char ascii2ebcdic[];
#ifdef fopen
#undef fopen
#endif
#define fopen cmsfopen
#endif

/* from dvips.h: */

#ifdef VMS
#include "[.vms]vms.h"
#endif /* VMS */

/*
 *   Is your malloc big?
 */
#if defined(MSDOS) && !defined(__EMX__) && !defined(DJGPP)
#define SMALLMALLOC
#endif
#if defined(OS2) && defined(_MSC_VER)
#define SMALLMALLOC
#endif

#if (defined(MSDOS) && !defined(DJGPP)) || (defined(OS2) && defined(_MSC_VER)) || defined(ATARIST)
typedef long integer;
#else
typedef int integer;
#endif

#ifndef KPATHSEA
typedef char boolean;
#endif

#ifdef WIN32
#define Boolean boolean
#else
#ifndef __THINK__
typedef short Boolean ;
#endif
#endif

/**************************************************************
 * data structures */

/* Adobe char metrics; one lig- and kern linked list for each
 * adobechar
 */

struct lig {
  struct lig *next;
  const char *succ, *sub;
  short op, boundright;         /* boundright: succ is rboundarychar */
};

struct kern {
  struct kern *next;
  const char *succ;
  int delta;
};

/* metric info for one char */
struct adobeinfo {
  int adobenum, tfmnum, width;
  const char *adobename;
  int llx, lly, urx, ury;
  int nonstd_lk; /* non-standard ligs and/or kerns */
  struct lig *ligs;
  struct kern *kerns;
};
/* The nonstd_lk field is for efficiency in case of letterspacing.
 * With letterspacing, all glyphs originally without ligs or
 * kerns do show up in the ligkern table, but with identical
 * information. We write them in one go to the ligkern table.
 */

/* encodings */
struct encoding {
  const char *name;
  const char *vec[256];
};

/* struct to store the names of files with extra ligkern info */
struct nstrings {
  int n;
  const char **names;
};

/*************************************************************
 * global vars */

#define buflen 256
char buffer[buflen];            /* input buffer (modified while parsing) */
char obuffer[buflen];           /* unmodified copy of input buffer */
char *param;                    /* current position in input buffer */

const char *fontname = "Unknown";

struct encoding *afmencoding = NULL;
struct encoding *outencoding = NULL;
const char *ligoption = "1";          /* which extra ligkerns? */
const char *Ligoption = "1";          /* extra ligkern info after letterspacing */
int based_on;                   /* name output file based on name afm file? */
int no_prefix = 0;              /* don't look for afm2pl- prefixed filenames */
FILE *infile, *outfile;
const char *afmname, *outname;  /* names of input and output files */
const char *encfilename;        /* encoding file */
struct nstrings *ligfilenames;  /* for files with extra ligkern info */
struct nstrings *Ligfilenames;  /* same; for after letterspacing */
const char *ligfilename;

struct adobeinfo
**adobechars,                   /* array of adobeinfo structs, from afm and in afm order */
 *tfmptrs[256];                 /* same, but indexed by tfm order */

int tfmnext[256];               /* for characters encoded multiple times in output */

/* global font info */
int iglyph, nglyphs, nglyphsb;  /* glyph no., n. of glyphs in afm */
float italicangle = 0.0;
float newslant = 0.0; /* computed from slant and italicangle */
int fixedpitch;
int afm2tfm_compat = 0; /* create afm2tfm-compatible font dimensions */
int xheight = 0;
int fontspace = -1;
int fstretch = -1;
int fshrink = -1;
int fextra = -1;
int fquad = -1;
int charwidth = 0;

float efactor = 1.0, slant = 0.0;       /* requested transformation */
int lspace = 0;                 /* for letterspacing */
char *efactorparam, *slantparam, *lspaceparam;
int forceoctal = 0;
int keepligs = 0;               /* retain afm ligs in spite of lspace>0 */

int rboundarychar = -1;         /* the (right) boundary character */
const char *boundaryname = "||";      /* name of boundarychars for liginfo specs */
struct adobeinfo *rbound, *lbound;
char *boundglyph = NULL;
  /* glyph name of rboundarychar; non-null only if encoded */
  /* track presence of r- and l boundarychar ligkerns.
   * we don't check whether the real chars in those ligkerns
   * are encoded, so we may get false positives
   */

int missingchars = 0;
int verbose = 0;

/**************************************************************
 * general utility functions */

/* little hack: error message is warning unless it starts with '!' */
static void
error(const char *s)
{
  char *parasave;
  parasave = param;
  (void) fprintf (stderr, "%s\n", s);
  if (obuffer[0]) {
    (void) fprintf (stderr, "%s\n", obuffer);
    while (param > buffer) {
      (void) fprintf (stderr, " ");
      param--;
    }
    (void) fprintf (stderr, "^\n");
  }
  if (*s == '!')
    exit (1);
  param = parasave;
}

static char *
mymalloc(unsigned long len)
{
  register char *p;
  unsigned long i;

#ifdef SMALLMALLOC
  if (len > 65500L)
    error ("! can't allocate more than 64K!");
#endif
  p = malloc ((unsigned) len);
  if (p == NULL)
    error ("! out of memory");
  for (i = 0; i < len; i++)
    p[i] = 0;
  return (p);
}

static char *
newstring(const char *s)
{
  char *q = mymalloc ((unsigned long) (strlen (s) + 1));
  (void) strcpy (q, s);
  return q;
}

/* find a glyph name in an adobeinfo array */
static struct adobeinfo *
findname(struct adobeinfo **achars, int n, const char *p)
{
  register int i;
  for (i = 0; i < n; i++)
    if (achars[i] && strcmp (p, achars[i]->adobename) == 0)
      return achars[i];
  return NULL;
}

/* Find glyph info by name in the original adobechars array
 * We don't include the boundarychars in the search but always test
 * separately for those.
 */
static struct adobeinfo *
findadobe(const char *p)
{
  return findname (adobechars, nglyphs, p);
}

/* Find glyph info by name in target encoding */
static struct adobeinfo *
findtfm(const char *p)
{
  return findname (tfmptrs, 256, p);
}

/* just for adobechars, we sometimes want the index instead */
static int
findindex(const char *p)
{
  int i;
  for (i = 0; i < nglyphs; i++)
    if (adobechars[i] && strcmp (p, adobechars[i]->adobename) == 0)
      return i;
  return -1;
}

/* geometrically transformed boundingbox */
static int
transform(int x, int y)
{
  register double acc;
  acc = efactor * x + slant * y;
  return (int) (acc >= 0 ? floor (acc + 0.5) : ceil (acc - 0.5));
}

/* read a line from infile into buffer and obuffer */
static int
texlive_getline(void)
{
  register char *p;
  register int c;
  int maxline;

  param = buffer;
  p = buffer;
  maxline = buflen;
  c = 0; /* to suppress phony compiler warning */
  while (((c = getc (infile)) != EOF) && (c != '\n') && (c != '\r'))
    /* added: test for \r. The way we do it, the DOS cr/lf convention
     * leads to alternating empty lines, but that is ok.
     */
    if (--maxline > 0) *p++ = c;
  *p = 0;
  (void) strcpy (obuffer, buffer);
  if (p == buffer && c == EOF)
    return (0);
  else
    return (1);
}

/*******************************************************
 * file handling
 */

#ifndef KPATHSEA
/* simple versions of concat (concatenate with allocation),
 * kpse_find_suffix, xbasename and an enum for kpse_file_format_type.
 * This is to reduce the number of #ifdef KPATHSEA sections.
 */
typedef enum {
  kpse_afm_format, kpse_lig_format, kpse_enc_format
} kpse_file_format_type;

/* string concatenation with allocation */
static char *
concat(const char *s1, const char *s2)
{
  char *answer = (char *) malloc (strlen (s1) + strlen (s2) + 1);
  strcpy (answer, s1);
  strcat (answer, s2);

  return answer;
}

/* Return pointer to first character after `.' in last directory element
 * of NAME.  If the name is `foo' or `/foo.bar/baz', we have no extension.
 */
static const char *
find_suffix(const char *name)
{
  const char *slash_pos;
  const char *dot_pos = (char *) strrchr (name, '.');

  if (dot_pos == NULL)
    return NULL;

  /* find last directory separator */
  for (slash_pos = name + strlen (name);
       slash_pos > dot_pos &&
       (*slash_pos != ':' && *slash_pos != '\\' && *slash_pos != '/');
       slash_pos--);

  /* accept suffix only if after last path separator */
  return slash_pos > dot_pos ? NULL : dot_pos + 1;
}

/* Return pointer to filename with leading path stripped */
static const char *
xbasename(const char *name)
{
  const char *base = NULL;
  unsigned len = strlen (name);

  for (len = strlen (name); len > 0; len--) {
    if (name[len - 1] == ':' || name[len - 1] == '\\'
        || name[len - 1] == '/') {
      base = name + len;
      break;
    }
  }
  if (!base)
    base = name;
  return base;
}

#define ISALNUM(c) (isascii (c) && isalnum(c))

#endif
/* end of KPATHSEA replacements */

/* file open functions; abort in case of failure; return filename */

/* open file for reading
 * parameters: (pointer to) requested filename
 * return (pointer to) actual filename without dir
 * Since we only have one input file open at any time
 * we use a global file variable infile.
 * Note. The kpse lib fu concat itself takes care of
 * allocating the result string
 * kpse handling isn't so helpful in the enc- and lig cases,
 * so we add a suffix explicitly.
 */
static const char *
openin(const char *fname, kpse_file_format_type format, const char *ext)
{
#ifdef KPATHSEA
  char *realfname;

  realfname = NULL;
  if (!no_prefix && (!strcmp(ext, ".enc") || !strcmp(ext, ".lig"))) {
    realfname = kpse_find_file (concat("afm2pl-", fname), format, false);
    if (!realfname) {
      realfname = concat (concat("afm2pl-", fname), ext);
      realfname = kpse_find_file (realfname, format, false);
    }
  }
  if (!realfname) {
    realfname = kpse_find_file (fname, format, false);
    if (!realfname) {
      realfname = concat (fname, ext);
      realfname = kpse_find_file (realfname, format, false);
    }
  }
  if (!realfname)
    FATAL1 ("%s not found", fname);
  infile = kpse_open_file (realfname, format);
  if (infile)
    return xbasename (realfname);
  else
    FATAL1 ("couldn't open %s", realfname);
#else
  const char *realfname;

  realfname = fname;
  infile = fopen (realfname, "r");
  if (!infile && !find_suffix (realfname)) {
    realfname = concat (fname, ext);
    infile = fopen (realfname, "r");
  }
  if (infile)
    return realfname;
  else
    error (concat ("!couldn't open ", realfname));
#endif
  return NULL;
}

/* open file for writing
 * Parameter based_on: if non-zero, then fname was the name
 * of the afm file, and we have to construct a new name
 * based on fname:
 * - replace an extension ".afm" with ".pl"
 * - strip leading path+
 * In this function, kpathsea functions don't provide better
 * functionality, but they do improve portability.
 */
static const char *
openout(const char *fname, int based_on, const char *outext)
{
  const char *inext = ".afm";
  /* use inext+1 and outext+1 when the leading dot is not desired */
  const char *realfname;
  const char *suf;

  if (based_on) {  /* compute output filename */
    suf = find_suffix (fname);
    if (suf && !strcmp ((inext + 1), suf)) {    /* replace afm suffix */
      char * q;
      q = newstring (fname);
      q[suf - fname] = 0;
      strcat (q, (outext + 1));
      realfname = q;
      /* no allocation required: new suffix not longer than old one */
    } else         /* append new suffix */
      realfname = concat (fname, outext);
    realfname = xbasename (realfname);
  } else {
    suf = find_suffix (fname);
    if (suf)
      realfname = fname;
    else
      realfname = concat (fname, outext);
  }
  outfile = fopen (realfname, "w");
  /* afm2tfm uses WRITEBIN instead of "w" for some OS-es */
  if (outfile)
    return realfname;
  else
    error (concat ("!", concat (realfname, " not found")));
  return NULL;
}

/********************************************************
 * reading the afm
 */

const char *interesting[] = { "FontName", "ItalicAngle", "IsFixedPitch",
  "XHeight", "C", "KPX", "EncodingScheme",
  "StartCharMetrics", "CharWidth", NULL
};
#define FontName (0)
#define ItalicAngle (1)
#define IsFixedPitch (2)
#define XHeight (3)
#define C (4)
#define KPX (5)
#define EncodingScheme (6)
#define StartCharMetrics (7)
#define CharWidth (8)
#define NONE (-1)
static int
interest(char *s)
{
  register const char **p;
  register int n;

  for (p = interesting, n = 0; *p; p++, n++)
    if (strcmp (s, *p) == 0)
      return (n);
  return (NONE);
}

/* At function call, param points to the first char of the next
 * string, or to eol.
 * In the input buffer, a terminating space is replaced with a null char.
 * Before termination, param is moved to the start of the next
 * string, or to eol.
 */

/* next (parameter) string (param is input pointer),
 * with allocation */

static char *
paramnewstring(void)
{
  register char *p, *q;

  p = param;
  while (*p > ' ')
    p++;
  if (*p != 0)
    *p++ = 0;
  q = newstring (param);
  while (*p && *p <= ' ')
    p++;
  param = p;
  return (q);
}

/* next (parameter) string from afm file (param is input pointer),
 * pre-allocated */

static char *
paramstring(void)
{
  register char *p, *q;

  p = param;
  while (*p > ' ')
    p++;
  q = param;
  if (*p != 0)
    *p++ = 0;
  while (*p && *p <= ' ')
    p++;
  param = p;
  return (q);
}

static int
paramnum(void)
{
  register char *p;
  int i;

  p = paramstring ();
  if (sscanf (p, "%d", &i) != 1)
    error ("! integer expected");
  return (i);
}

static float
paramfloat(void)
{
  register char *p;
  float i;

  p = paramstring ();
  if (sscanf (p, "%f", &i) != 1)
    error ("! number expected");
  return (i);
}

static void
expect(const char *s)
{
  if (strcmp (paramstring (), s) != 0) {
    (void) fprintf (stderr, "%s expected: ", s);
    error ("! syntax error");
  }
}

/* allocating and initializing structs */

/* initialize afm info for one char */
static struct adobeinfo *
newchar(void)
{
  register struct adobeinfo *ai;

  ai = (struct adobeinfo *) mymalloc ((unsigned long)
                                      sizeof (struct adobeinfo));
  ai->adobenum = -1;
  ai->tfmnum = -1;
  /* ai->kern_as = NULL; */
  ai->width = -1;
  ai->adobename = NULL;
  ai->llx = -1;
  ai->lly = -1;
  ai->urx = -1;
  ai->ury = -1;
  ai->ligs = NULL;
  ai->kerns = NULL;
  /* ai->kern_equivs = NULL ; */
  return (ai);
}

static struct kern *
newkern(void)
{
  register struct kern *nk;

  nk = (struct kern *) mymalloc ((unsigned long) sizeof (struct kern));
  nk->next = NULL;
  nk->succ = NULL;
  nk->delta = 0;
  return (nk);
}

static struct lig *
newlig(void)
{
  register struct lig *nl;

  nl = (struct lig *) mymalloc ((unsigned long) sizeof (struct lig));
  nl->next = NULL;
  nl->succ = NULL;
  nl->sub = NULL;
  nl->op = 0;      /* the default =: op */
  nl->boundright = 0;
  return (nl);
}

/* read and interpret afm info for 1 char
 * adobeptrs is an array of pointers to adobeinfo structs
 * indexed by adobenum.
 * */
static void
handlechar(void)
{                  /* an input line beginning with C */
  register struct adobeinfo *ai;
  register struct lig *nl;

  if ((++iglyph) >= nglyphs)
    error ("!Too many glyphs in afm");
  adobechars[iglyph] = newchar ();
  ai = adobechars[iglyph];
  ai->adobenum = paramnum ();
  expect (";");
  expect ("WX");
  ai->width = paramnum ();
  if (fixedpitch)
    charwidth = ai->width;
  expect (";");
  expect ("N");
  ai->adobename = paramnewstring ();
  if ((ai->adobenum) >= 0)
    afmencoding->vec[ai->adobenum] = newstring (ai->adobename);
  expect (";");
  expect ("B");
  ai->llx = paramnum ();
  ai->lly = paramnum ();
  ai->urx = paramnum ();
  ai->ury = paramnum ();
  expect (";");
/* Now look for ligatures (which aren't present in fixedpitch fonts) */
  while (*param == 'L' && !fixedpitch) {
    expect ("L");
    nl = newlig ();
    nl->succ = paramnewstring ();
    nl->sub = paramnewstring ();
    nl->next = ai->ligs;
    ai->ligs = nl;
    expect (";");
  }
}

static void
handlekern(void)
{                  /* an input line beginning with KPX */
  register struct adobeinfo *ai;
  register char *p, *sc;
  register struct kern *nk;
  int dlt;

  p = paramstring ();
  ai = findadobe (p);
  if (!ai)
    error ("kern char not found");
  else {
    sc = paramstring ();
    dlt = paramnum ();
    if (dlt) {
      nk = newkern ();
      nk->succ = newstring (sc);
      nk->delta = dlt;
      nk->next = ai->kerns;
      ai->kerns = nk;
    }
  }
}

/* read afm file */
static void
readadobe(void)
{
  const char *inname;
  int i;

  /* open afm file */
  inname = newstring (afmname);
  inname = openin (inname, kpse_afm_format, ".afm");

  nglyphs = -1;    /* allocate adobechars at StartCharMetrics */
  afmencoding = (struct encoding *) mymalloc ((unsigned long)
                                              sizeof (struct encoding));
  for (i = 0; i < 256; i++)
    afmencoding->vec[i] = ".notdef";
  afmencoding->name = "Unspecified";

  while (texlive_getline ()) {
    switch (interest (paramstring ())) {
    case FontName:
      fontname = paramnewstring ();
      break;
    case EncodingScheme:
      afmencoding->name = paramnewstring ();
      break;
    case ItalicAngle:
      italicangle = paramfloat ();
      break;
    case IsFixedPitch:
      if (*param == 't' || *param == 'T')
        fixedpitch = 1;
      else
        fixedpitch = 0;
      break;
    case XHeight:
      xheight = paramnum ();
      break;
    case StartCharMetrics:
      nglyphs = paramnum ();
      nglyphsb = nglyphs + 1;
      iglyph = -1;
      /* Allocate adobechars array (just the array of pointers).
       * Reserve extra slot for rboundarychar
       */
      adobechars =
        (struct adobeinfo **) mymalloc ((unsigned long) (nglyphs+1) *
                                        (unsigned long) sizeof (struct
                                                                adobeinfo
                                                                *));
      break;
    case C:
      handlechar ();
      break;
    case KPX:
      handlekern ();
      break;
    case CharWidth:
      charwidth = paramnum ();
      fixedpitch = 1;
      break;
    default:
      break;
    }
  }
  fclose (infile);
  infile = NULL;
}

/* now some changes to the afm info */
static void
changeadobe(void)
{
  struct adobeinfo *ai;
  int i;
  /* float newslant; local declaration masks global one */

  /* A fix: avoiding negative heights or depths.
   * They break accents in math mode, among other things.
   */
  for (i = 0; i < nglyphs; i++) {
    ai = adobechars[i];
    if (ai->lly > 0)
      ai->lly = 0;
    if (ai->ury < 0)
      ai->ury = 0;
  }

  /* apply any requested geometric transforms */
  if (efactor != 1.0 || slant != 0.0) {
    for (i = 0; i < nglyphs; i++) {
      ai = adobechars[i];
      ai->width = transform (ai->width, 0);
      ai->llx = transform (ai->llx, ai->lly);
      ai->urx = transform (ai->urx, ai->ury);
    }
  }
  newslant = (double) slant -
    efactor * tan (italicangle * (3.1415926535 / 180.0));

  /* Avoid output of '(SLANT R -0.000000)' due to floating point
   * rounding; neither round() nor fabs() need be defined.  */
  if ((newslant < 0.0) && (newslant*2000000.0 >= -1.0))
    newslant = 0.0;
   
  /* if !keepligs, remove native ligatures */
  if (!keepligs && lspace>0) {
    for (i = 0; i < nglyphs; i++)
      adobechars[i]->ligs = NULL;
  }

  /* create a left and right boundarychar entries - just in case,
   * but don't add them (yet) to adobechars */
  for (i = 0; i < 2; i++) {
    ai = newchar ();
    ai->adobenum = -1;
    ai->width = 0;
    ai->llx = 0;
    ai->lly = 0;
    ai->urx = 0;
    ai->ury = 0;
    ai->adobename = boundaryname;
    if (i == 0)
      lbound = ai;
    else
      rbound = ai;
  }

  /* global font dimensions */

  /* The following may depend on the -f parameter */

  if (xheight == 0) {
    ai = findadobe ("x");
    if (ai)
      xheight = ai->ury;
    else
      xheight = 400;
  }

  if (afm2tfm_compat) { /* -f 'afm2tfm'*/

    ai = findadobe ("space");
    if (ai)
      fontspace = ai->width; /* transform already applied */
    else {
      if (fixedpitch)
        fontspace = charwidth;
      else
        transform(500, 0);
    }
    fstretch = (fixedpitch) ? 0 : transform (300, 0);
    fshrink = (fixedpitch) ? 0 : transform (100, 0);
    /* fextra = transform (111, 0); */
    fquad = transform (1000, 0);

  } else if (!fixedpitch) {

    /* some may be set by a numerical -f parameter) */

    if (fontspace<0) {
      ai = findadobe ("space");
      if (ai)
        fontspace = ai->width; /* transform already applied */
      else {
        fontspace = transform(500, 0);
      }
    }
    if (fquad<0) {
      ai = findadobe ("zero");
      if (ai) fquad = 2 * ai->width;
      else if (fixedpitch) fquad = 2 * charwidth;
      else fquad = transform (1000, 0);
    }
    if (fstretch<0) fstretch = fontspace / 2;
    if (fshrink<0) fshrink = fontspace / 3;
    if (fextra<0) fextra = fontspace / 3;
  } else { /* fixedpitch && !afm2tfm_compat */
    if (fontspace<0) fontspace = charwidth;
    if (fstretch<0) fstretch = 0;
    if (fshrink<0) fshrink = 0;
    if (fquad<0) fquad = 2 * charwidth;
    if (fextra<0) fextra = charwidth;
  }
}

/**************************************************
 * reading encodings
 */

char smbuffer[100];             /* for tokens */

/*   Here we get a token from the encoding file.  We parse just as
 *   much PostScript as we expect to find in an encoding file.  We
 *   allow commented lines and names like 0, .notdef, _foo_.  We do
 *   not allow //abc.
 */
static char *
gettoken(void)
{
  char *p, *q;

  while (1) {
    while (param == 0 || *param == 0) {
      if (texlive_getline () == 0)
        error ("! premature end in encoding file");
    }
    if (param[0] == '%') {
      param[0] = 0;     /* be done with this line */
      continue;
    }
    while (*param && *param <= ' ')
      param++;
    if (*param) {
      if (*param == '[' || *param == ']' ||
          *param == '{' || *param == '}') {
        smbuffer[0] = *param++;
        smbuffer[1] = 0;
        return smbuffer;
      } else if (*param == '/' || *param == '-' || *param == '_' ||
                 *param == '.' ||
                 ('0' <= *param && *param <= '9') ||
                 ('a' <= *param && *param <= 'z') ||
                 ('A' <= *param && *param <= 'Z')) {
        smbuffer[0] = *param;
        for (p = param + 1, q = smbuffer + 1;
             *p == '-' || *p == '_' || *p == '.' ||
             ('0' <= *p && *p <= '9') ||
             ('a' <= *p && *p <= 'z') ||
             ('A' <= *p && *p <= 'Z'); p++, q++)
          *q = *p;
        *q = 0;
        param = p;
        return smbuffer;
      }
    }
  }
}

/* This routine reads the encoding file.
 * Return value: pointer to encoding.
 *
 * There is just one reason why we read the encoding vector
 * before the extra ligkern information: if a (r)boundarychar is
 * specified by number then we want to translate it into
 * a glyph name.
 *
 * We also create tfmptrs as a reencoded version of adobechars, and
 * tfmnext, which tracks multiple encodings of a single glyph and is
 * best explained by the code creating it.
 */
static void
readencoding(void)
{
  char *p;
  int i, inx;
  struct adobeinfo *ai;

  if (infile)
    error ("! oops; internal infile error");
  if (encfilename) {
    encfilename =
      openin (encfilename, kpse_enc_format, ".enc");
    param = 0;
    if (infile == 0)
      error ("! couldn't open that encoding file");
    outencoding = (struct encoding *) mymalloc
      ((unsigned long) sizeof (struct encoding));
    for (i = 0; i < 256; i++)
      outencoding->vec[i] = ".notdef";
    outencoding->name = "Unspecified";
    p = gettoken ();
    if (*p != '/' || p[1] == 0)
      error ("! first token in encoding must be literal encoding name");
    outencoding->name = newstring (p + 1);
    p = gettoken ();
    if (strcmp (p, "["))
      error ("! second token in encoding must be mark ([) token");
    for (i = 0; i < 256; i++) {
      p = gettoken ();
      if (*p != '/' || p[1] == 0)
        error ("! tokens 3 to 257 in encoding must be literal names");
      outencoding->vec[i] = newstring (p + 1);
    }
    p = gettoken ();
    if (strcmp (p, "]"))
      error ("! token 258 in encoding must be make-array (])");
    fclose (infile);
    infile = 0;
  } else
    outencoding = afmencoding;

  /* find adobeinfo for each encoded glyph name
   * track multiple encodings (tfmnext).
   * For missing glyphs, this is not necessary.
   */
  for (i = 0; i < 256; i++)
    tfmnext[i] = -1;
  for (i = 0; i < 256; i++) {
    if (strcmp (outencoding->vec[i], ".notdef")) {
      ai = findadobe (outencoding->vec[i]);
      tfmptrs[i] = ai;
      if (ai) {
        inx = ai->tfmnum;
        if (inx == -1)
          ai->tfmnum = i;
        else {
          while (tfmnext[inx] != -1)
            inx = tfmnext[inx];
          tfmnext[inx] = i;
        }
      } else { /* missing glyph */
        if (!missingchars && verbose)
          fputs ("Missing glyphs\n", stderr);
        missingchars += 1;
        if (verbose) printf ("%s\n", outencoding->vec[i]);
      }
    } else /* empty slot */
      tfmptrs[i] = NULL;
  }
}

/****************************************************************
 * Processing extra ligkern info
 *
 * When these functions are called, we have available the afm data
 * and the raw encoding vector.
 * changeadobe has created adobeinfos for left and right boundarychar.
 * We use the encoding vector ONLY to translate an rboundarychar spec
 * into an rboundarychar name. However, normally an rboundarychar will
 * occupy an otherwise unused slot in the encoding, in which case
 * the name is irrelevant.
 * I don't see the point of a numeric rboundarychar specification:
 * either one wants to use a specific character or the numeric value
 * doesn't matter. But I allow it for backward compatibility.
 *
 * extra ligkern info can come from:
 * - comments in the encoding file
 * - lig files. These replace the hard-coded set in afm2tfm.
 * afm2tfm applies its own set only if the enc file contains no
 * ligkern instructions. This is also the default behavior of
 * afm2pl.
 *
 * Typically, extra ligkern info may contain:
 * - tex-specific hacks such as `less less =: guillemotleft'
 * - deletion of space- and number kerns
 * - kern accented chars as their unaccented versions
 * - the f/l/i ligatures, in case they were absent from the afm.
 *
 * note. afm2pl allows first copying space kerns
 * to boundarychar kerns and then deleting the space kerns, leaving
 * the boundarychar kerns intact.
 *
 * There was a function for creating accented char ligatures.
 * This doesn't prevent TeX from compositing accented chars
 * so it is not very useful.
 * In afm2tfm this function was never called and I removed it.
 * I created a file accents.lig which may be loaded for these
 * accent ligs but default isn't.
 */

/* Notation of ligkern specs:
 * char1 char2 =: char3    => replace char1 char2 with char3
 *   and more generally:
 * char1 char2 ligop char3 => char1 char2 ligop char3
 * char1 {} char2          => delete char1 char2 kern
 * char1 <> char2          => kern char1 the same as char2
 * || = char               => char is right boundary char
 *
 * Notes.
 * - cork.enc contains specs `char @{@} char' instead of `char {} char'
 * - The ligkern specs use `||' for both lboundarychar and
 *   rboundarychar. The context must make clear which one is intended.
 *   I store them in separate adobechar structs.
 */

/* These are the eight ligature ops, in pl terms and in METAFONT terms.
 */
const char *plligops[] = {
  "LIG", "/LIG", "/LIG>", "LIG/", "LIG/>", "/LIG/", "/LIG/>",
  "/LIG/>>", 0
};
const char *encligops[] = {
  "=:", "|=:", "|=:>", "=:|", "=:|>", "|=:|", "|=:|>", "|=:|>>", 0
};

/* Make the kerning for character s1 equivalent to that for s2
 * but don't replace existing kerns.
 * It doesn't matter whether or not s2 is encoded.
 * In the pl file, the kerning info of both s1 and s2 will be written
 * out in full.
 */
static void
copykerns(char *s1, char *s2)
{
  int i, found, delta2, found1, found2;
  struct adobeinfo *ai, *ai1, *ai2;
  struct kern *nk, *nk1, *nk2;

  if (!strcmp (s1, s2))
    return;

  /* kerns with s1 and s2 on the left */
  if (!strcmp (boundaryname, s1))
    ai1 = lbound;
  else
    ai1 = findadobe (s1);
  if (!strcmp (boundaryname, s2))
    ai2 = lbound;
  else
    ai2 = findadobe (s2);
  if (ai1 && ai2 && ai2->kerns) {
    nk2 = ai2->kerns;
    while (nk2) {
      found = 0;
      nk1 = ai1->kerns;
      while (nk1) {
        if (!strcmp (nk1->succ, nk2->succ)) {
          found = 1;
          break;
        }
        nk1 = nk1->next;
      }
      if (!found) {
        nk1 = newkern ();
        nk1->succ = nk2->succ;
        nk1->delta = nk2->delta;
        nk1->next = ai1->kerns;
        ai1->kerns = nk1;
      }
      nk2 = nk2->next;
    }
  }

  /* kerns with s1 and s2 on the right. */
  if (!strcmp (boundaryname, s1) && boundglyph)
    s1 = boundglyph;
  if (!strcmp (boundaryname, s2) && boundglyph)
    s2 = boundglyph;
  /* a name '||' for s1/s2 is untouched */
  if (!strcmp (s1, s2))
    return;
  delta2 = 0; /* silence 'might be used uninitialized' warning */
  for (i = 0; i < nglyphs; i++) {
    ai = adobechars[i];
    found1 = 0;
    found2 = 0;
    nk = ai->kerns;
    while (nk) {
      if (!strcmp (nk->succ, s2)) {
        found2 = 1;
        delta2 = nk->delta;
      }
      if (!strcmp (nk->succ, s1))
        found1 = 1;
      nk = nk->next;
    }
    if (found2 && !found1) {
      nk1 = newkern ();
      nk1->next = ai->kerns;
      ai->kerns = nk1;
      nk1->succ = newstring (s1);
      /* can't copy s1, which is just a stack variable */
      nk1->delta = delta2;
    }
  }
}

/* remove kern between ai->adobechar and s2, where s2 != "*"
 */
static void
adobermkern(struct adobeinfo *ai, char *s2)
{
  struct kern *k, *kprev;
  char *s2a;
  int first;
  if (!ai->kerns)
    return;
  k = ai->kerns;
  if (!strcmp (s2, boundaryname) && boundglyph)
    s2a = boundglyph;
  else
    s2a = s2;
  kprev = k; /* silence 'might be used uninitialized' warning */
  first = 1;
  while (k) {
    if (!strcmp (s2a, k->succ) || !strcmp (s2, k->succ)) {
      if (first)
        ai->kerns = k->next;
      else
        kprev->next = k->next;
      return;
    }
    first = 0;
    kprev = k;
    k = k->next;
  }
}

/* remove kerns between s1 and s2 (wildcards allowed) */
static void
rmkern(char *s1, char *s2)
{
  int i;
  struct adobeinfo *ai;

  if (!strcmp (s1, "*")) {
    for (i = 0; i < nglyphs; i++) {
      ai = adobechars[i];
      adobermkern (ai, s2);
    }
    adobermkern (lbound, s2);
  } else if (!strcmp (s2, "*")) {
    if (!strcmp (s1, boundaryname))
      ai = lbound;
    else
      ai = findadobe (s1);
    if (ai)
      ai->kerns = NULL;
  } else {         /* remove single pair */
    if (!strcmp (s1, boundaryname))
      ai = lbound;
    else
      ai = findadobe (s1);
    if (!ai)
      return;
    adobermkern (ai, s2);
  }
}

/* With default ligkern option, need to know whether
 * the enc file contains ligkern specs
 */
int sawligkern;

/* zero-th iteration: look out for rboundarychar spec.
 * first iteration: look out for everything else.
 */
int lig_it;

/* set_rboundarychar is to be called after the ligkern specs have been
 * scanned for a rboundarychar spec but before we know whether it
 * will actually be used.
 */
static void
set_rboundarychar(int bch)
{
  int i;
  if (bch == -1) {
    /* find empty slot; skip 0, just to be safe */
    for (i = 1; i < 256; i++)
      if (!strcmp (outencoding->vec[i], ".notdef")) {
        rboundarychar = i;
        tfmptrs[i] = rbound;
        rbound->tfmnum = i;
        adobechars[nglyphs] = rbound;
        return;
      }
    error ("No tfm slot available for boundarychar");
  } else {
    rboundarychar = bch;
    if (tfmptrs[rboundarychar]) {
      rbound = tfmptrs[rboundarychar];
      boundglyph = newstring (rbound->adobename);
      nglyphsb = nglyphs; /* no extra slot in adobechars needed */
    } else {
      tfmptrs[bch] = rbound;
      rbound->tfmnum = bch;
      adobechars[nglyphs] = rbound;
    }
    return;
  }
}

/* Reads a ligkern line, which may contain several ligkern specs.
 * lig_it = 0: only process rboundarychar spec
 * lig_it != 0: process all other ligkern specs
 * lig_it = 2: post-letterspacing; only `{}' specs are allowed.
 */
static void
checkligkern(char *s, int isencfile)
{
  char *mlist[5];               /* tokens constituting a ligkern spec */
  int n, i, bch;
  struct adobeinfo *ai;

  if (s[0] == '%')
    s++;           /* skip optional leading `%' */
  /* Note. '%' is necessary for ligkern comments in an encoding file
   * and optional otherwise.
   */
  while (*s && *s <= ' ')
    s++;           /* skip to first token */
  if (!s[0])
    return;        /* empty line; done */
  if (isencfile && strncmp (s, "LIGKERN", 7)) /* not a ligkern line */
    return;
  if (!strncmp (s, "LIGKERN", 7))
    s += 7;        /* skip beyond `LIGKERN' */
  sawligkern = 1;
  while (*s && *s <= ' ')
    s++;
  param = s;
  while (*param) {
    /* collect next ligkern spec */
    for (n = 0; n < 5;) {
      /* collect next token for ligkern spec */
      if (*param == 0)
        break;     /* ligkern spec complete */
      mlist[n] = paramstring ();
      if (strcmp (mlist[n], ";") == 0)
        break;     /* ligkern spec complete */
      n++;
    }

    /* handle ligkern spec */
    if (n > 4)
      error ("! too many parameters in lig kern data");
    if (n < 3)
      error ("! not enough parameters in lig kern data");
    if (n == 3 && strcmp (mlist[1], "{}") == 0) {
      if (!lig_it)
        continue;
      if (lig_it == 2 && strcmp (mlist[0], "*") != 0) {
        ai = findadobe (mlist[0]);
        if (ai) ai->nonstd_lk = 1;
      }
      rmkern (mlist[0], mlist[2]);
    } else if (n == 3 && strcmp (mlist[1], "<>") == 0) {
      if (!lig_it)
        continue;
      if (lig_it == 2)
        error ("!Post-letterspacing <> kern spec not allowed");
      copykerns (mlist[0], mlist[2]);
    } else if (n == 3 && strcmp (mlist[0], boundaryname) == 0 &&
               strcmp (mlist[1], "=") == 0) {
      /* setting of rboundarychar; only during 0-th iteration */
      if (lig_it)
        continue;
      if (lig_it == 2)
        error ("!Post-letterspacing boundarychar spec not allowed");
      if (rboundarychar != -1)
        error ("! multiple boundary character commands?");
      if (sscanf (mlist[2], "%d", &bch) == 1) {
        /* number for rboundarychar */
        if (bch < 0 || bch > 255)
          error ("! boundary character number must be 0..255");
        else
          set_rboundarychar (bch);
      } else {     /* named rboundarychar */
        for (bch = 0; bch < 256; bch++)
          if (!strcmp (outencoding->vec[bch], mlist[2]))
            break;
        if (bch<256) boundglyph = newstring(mlist[2]);
        else {
          if (strcmp ("cwm", mlist[2])) error
           ("unencoded boundarychar; will use empty slot instead");
          bch = -1;
          boundglyph = newstring(boundaryname);
        }
        set_rboundarychar (bch);
      }
    } else if (n == 4) {        /* ligature: char succ lig_op result */
      int op = -1;

      if (!lig_it)
        continue;
      if (lig_it == 2)
        error ("!Post-letterspacing lig spec not allowed");
      for (i = 0; encligops[i]; i++)
        if (strcmp (mlist[2], encligops[i]) == 0) {
          op = i;
          break;
        }
      if (op < 0)
        error ("! bad ligature op specified");
      if (!strcmp (mlist[0], boundaryname))
        ai = lbound;
      else
        ai = findadobe (mlist[0]);
      if (ai) {
        struct lig *lig;

        if (findadobe (mlist[1]))       /* remove coincident kerns */
          rmkern (mlist[0], mlist[1]);
        if (strcmp (mlist[3], boundaryname) == 0)
          error ("! you can't lig to the boundary character!");
        for (lig = ai->ligs; lig; lig = lig->next)
          if (strcmp (lig->succ, mlist[1]) == 0)
            break; /* we'll re-use this structure */
        if (lig == 0) {
          lig = newlig ();
          lig->succ = newstring (mlist[1]);
          lig->next = ai->ligs;
          ai->ligs = lig;
        }
        lig->sub = newstring (mlist[3]);
        lig->op = op;
        if (strcmp (mlist[1], boundaryname) == 0) {
          lig->boundright = 1;
          if (strcmp (mlist[0], boundaryname) == 0)
            error ("! you can't lig boundarychar boundarychar!");
        } else
          lig->boundright = 0;
      }
    } else
      error ("! bad form in LIGKERN command");
  }
}

static void
letterspace(void)
{
  int i,j;
  struct kern *k;
  struct adobeinfo *ai;
  int *haskern; /* which glyphs are rhs of a kerning pair? */

  haskern = (int *) malloc (nglyphs * sizeof (int));
  for (i=0;i<nglyphs;i++) {
    for (j=0;j<nglyphs;j++) haskern[j] = 0;
    ai = adobechars[i];
    for (k=ai->kerns;k;k=k->next) {
      j = findindex (k->succ);
      if (j>0 && j<nglyphs) {
        /* kern with rbound is NOT modified */
        k->delta += lspace;
        haskern[j] = 1;
      }
    }
    for (j=0;j<nglyphs;j++) if (!haskern[j]) {
      k = newkern();
      k->succ = adobechars[j]->adobename;
      k->delta = lspace;
      k->next = ai->kerns;
      ai->kerns = k;
    }
  }
  free (haskern);
  fontspace += 2 * lspace;
}

static void
extraligkerninfo(void)
{
  int i;
  char *p;
  struct adobeinfo *ai;

  /* with positive letterspacing, remove native ligs unless keepligs
   */
  if (lspace>0 && !keepligs)
    for (i=0;i<nglyphs;i++) adobechars[i]->ligs = NULL;

  /* Boundarychars make things a lot messier than they would be
   * otherwise. A rather long summary:
   * When typesetting, TeX sandwiches words between a left
   * boundarychar and a right boundarychar.
   * It seems sensible to translate space kerns into boundarychar
   * kerns although afm2tfm doesn't do this.
   * Below, we read ligkern info twice: the first time to read
   * a right boundarychar spec, if any, the second time to read everything
   * else. The global loop variable lig_it is consulted by checkligkern.
   * more about boundarychar/boundaryname:
   * rboundarychar is the index of right boundarychar in the tfm;
   * initialized as -1
   * The left boundarychar doesn't need a proper tfm index
   */
  if (lspace>0 && !strcmp (ligoption, "0")) ligoption = "1";
  if (!strcmp (ligoption, "0"))
    return;
  if (!strcmp (ligoption, "1") && encfilename && lspace<=0) {
    sawligkern = 0;
    /* ligkern specs in comments in encoding file? */
    encfilename =
      openin (encfilename, kpse_enc_format, ".enc");
    for (lig_it = 0; lig_it < 2; lig_it++) {
      while (texlive_getline ()) {
        /* search for (ligkern) comment in line */
        for (p = buffer; *p; p++)
          if (*p == '%') {
            checkligkern (p, 1);        /* 2nd param 1: reading .enc file */
            break;
          }
      }
      if (!sawligkern)
        break;     /* read default lig file instead */
      if (!lig_it) {
        if (rboundarychar == -1)
          set_rboundarychar (-1);
        rewind (infile);
      }
    }
    fclose (infile);
    if (sawligkern)
      return;
    else {
      ligfilenames->n = 1;
      ligfilenames->names = malloc (sizeof (char *));
      ligfilenames->names[0] = "default.lig";
    }
  } else if (!strcmp (ligoption, "1")) { /* lspace>0 or no encfile */
    ligfilenames->n = 1;
    ligfilenames->names = malloc (sizeof (char *));
    if (lspace<=0) ligfilenames->names[0] = "default.lig";
    else ligfilenames->names[0] = "defpre.lig";
  }

  /* process ligfilenames struct */
  for (lig_it = 0; lig_it < 2; lig_it++) {
    for (i = 0; i < ligfilenames->n; i++) {
      ligfilename = openin (ligfilenames->names[i],
                            kpse_lig_format, ".lig");
      while (texlive_getline ())
        checkligkern (buffer, 0);       /* 2nd param 0: lig file */
      fclose (infile);
    }
    if (!lig_it && (rboundarychar == -1))
      set_rboundarychar (-1);
  }

  /* optimization: identify glyphs which now have ligs or kerns.
   * Boundarychars: rboundarychar won't get a label in the ligtable,
   * and lboundarychar will be done separately anyhow.
   */
  for (i=0;i<nglyphs;i++) {
    ai = adobechars[i];
    ai->nonstd_lk = (ai->ligs!=NULL || ai->kerns!=NULL);
  }

  if (!lspace) return;

  letterspace();

  /* process post-letterspace ligfiles */
  if (!strcmp (Ligoption, "1")) {
    Ligfilenames->n = 1;
    Ligfilenames->names = malloc (sizeof (char *));
    Ligfilenames->names[0] = "defpost.lig";
  }
  lig_it = 2;
  for (i = 0; i < Ligfilenames->n; i++) {
    ligfilename = openin (Ligfilenames->names[i],
                          kpse_lig_format, ".lig");
    while (texlive_getline ())
      checkligkern (buffer, 0);       /* 2nd param 0: lig file */
    fclose (infile);
  }
}

static void
spaceparms(void)
{
  /* afm2tfm values for fstretch, fshrink, fextra: 200, 100, 111
   * fontinst values: 0.6 * space, 0.24 * space, ?
   * cmr10 values: 0.5 * space, .33 * space, .33 * space
   */
  if (!fixedpitch) {
    if (fstretch==0) fstretch = fontspace / 2;
    if (fshrink==0) fshrink = fontspace / 3;
    if (fextra==0) fextra = fontspace / 3;
  }
}

/*********************************************************
 * debug output of adobeinfo structs */

#ifdef AFMDEBUG
FILE *dmp;
char *dmpname = "afm2pl.dmp";

static void
dumpai(struct adobeinfo *ai)
{
  struct lig *lg;
  struct kern *kr;
  putc ('\n', dmp);
  fprintf (dmp, "adobename %s\n",
           ai->adobename ? ai->adobename : "undefined");
  fprintf (dmp, "adobenum %d\n", ai->adobenum);
  fprintf (dmp, "tfmnum %d\n", ai->tfmnum);
  fprintf (dmp, "width %d\n", ai->width);
  fprintf (dmp, "bbox %d %d %d %d\n", ai->llx, ai->lly, ai->urx,
           ai->ury);
  fprintf (dmp, "nonstd_lk %d\n", ai->nonstd_lk);
  if (!ai->ligs)
    fputs ("no ligs\n", dmp);
  else
    for (lg = ai->ligs; lg; lg = lg->next)
      fprintf (dmp, "lig %s %d %s%s\n",
               lg->succ, lg->op, lg->sub,
               lg->boundright ? " boundarychar" : "");
  if (!ai->kerns)
    fputs ("no kerns\n", dmp);
  else
    for (kr = ai->kerns; kr; kr = kr->next)
      fprintf (dmp, "kern %s %d\n", kr->succ, kr->delta);
}

static void
writedump(void)
{
  int i;
  dmp = fopen (dmpname, "w");
  if (!dmp)
    error ("Cant open dump file");
  if (lbound) {
    fputs ("\nleft boundarychar\n", dmp);
    dumpai (lbound);
  }
  if (rbound) {
    fputs ("\nright boundarychar\n", dmp);
    dumpai (rbound);
  }
  for (i = 0; i < nglyphs; i++)
    if (adobechars[i]) dumpai (adobechars[i]);
    else fprintf (dmp, "\nNo char at slot %d\n", i);

  fputs ("\nBy tfmptrs:\n", dmp);
  for (i = 0; i < 256; i++)
    if (tfmptrs[i]) dumpai (tfmptrs[i]);
    else fprintf (dmp, "\nNo char at slot %d\n", i);
  fclose (dmp);
}
#endif

/*******************************************************************
 * the PL file. */

#define plout(s)  fprintf(outfile, s)
int level;                      /* depth of parenthesis nesting in PL output file */

/* indent */
static void
pllevout(void)
{
  register int l = level;
  while (l--)
    plout ("   ");
}

/* newline plus indent */
static void
pllevnlout(void)
{
  plout ("\n");
  pllevout ();
}

#define ploutln(str) {fprintf(outfile,"%s\n",str);pllevout();}
#define ploutln2(f,s) {fprintf(outfile,f,s);pllevnlout();}
#define ploutln3(f,a,b) {fprintf(outfile,f,a,b);pllevnlout();}
#define ploutln4(f,a,b,c) {fprintf(outfile,f,a,b,c);pllevnlout();}

/* left bracket */
static void
plleft(void)
{
  level++;
  plout ("(");
}

/* right bracket */
static void
plright(void)
{
  level--;
  ploutln (")");
}

/* return string representation for .pl file  of tfmptrs[c]
 * don't forget that TeX doesn't use glyph names.
 * only, if c happens to be a printable ascii char
 * then we gratefully use that fact if we are allowed to.
 * If the encoding moves the printable ascii range around then
 * forceoctal would be a good idea.
 */
char plcharbuf[6];
static char *
plchar(int c)
{
  if (forceoctal == 0 && ISALNUM (c))
    (void) sprintf (plcharbuf, "C %c",
#ifndef VMCMS
                    c);
#else
                    ascii2ebcdic[c]);
#endif
  else
    (void) sprintf (plcharbuf, "O %o", (unsigned) c);
  return (char *) plcharbuf;
}

/* comment string with official glyph name if useful,
 * null string otherwise
 */
char plnamebuf[100];
static char *
plname(int c)
{
  if (!forceoctal && ISALNUM (c)) {
    plnamebuf[0] = 0;
  } else if (c >= 0 && c < 256) {
    sprintf (plnamebuf, " (comment %s)", tfmptrs[c]->adobename);
  }
  return (char *) plnamebuf;
}


/* obuffer: originally unmodified copy of input buffer,
 * now recycled as output buffer
 */
static void
writepl(void)
{
  register int i, j, k;
  int bc, ec;
  register struct adobeinfo *ai;
  register struct lig *nlig;
  register struct kern *nkern;
  struct adobeinfo *asucc, *asub;
  int ht, dt;
  char labeled;
  const char *pp;

  outname = openout (outname, based_on, ".pl");

  /* header */
  {
    char *outbase = newstring (xbasename (outname));
    pp = find_suffix (outbase);
    if (pp)
      outbase[pp - outbase - 1] = 0;
    (void) sprintf (obuffer, "%s%s%s", outbase,
                    (efactor == 1.0 ? "" : "-E"),
                    (slant == 0.0 ? "" : "-S"));
    free (outbase);
  }
  if (strlen (obuffer) > 19) {  /* too long, will retain first 9 and last 10 */
    register char *p, *q;
    for (p = &obuffer[9], q = &obuffer[strlen (obuffer) - 10];
         p < &obuffer[19]; p++, q++)
      *p = *q;
    obuffer[19] = 0;
  }

  /* global parameters */
  ploutln2 ("(FAMILY %s)", obuffer);
  {
    char tbuf[41];
    char *tbp;

    strncpy (tbuf, outencoding->name, 40);
    tbuf[40] = 0;

    if (strlen (tbuf) > 39) {
      error ("Coding scheme too long; shortening to 39 characters.");
      tbuf[39] = 0;
    }
    tbp = tbuf;
    while (*tbp) { *tbp = toupper ((unsigned char)*tbp); tbp++; }
    ploutln2 ("(CODINGSCHEME %s)", tbuf);
  }
  ploutln ("(DESIGNSIZE R 10.0)");
  ploutln ("(DESIGNUNITS R 1000)");
  ploutln ("(COMMENT DESIGNSIZE (1 em) IS IN POINTS)");
  ploutln
    ("(COMMENT OTHER DIMENSIONS ARE MULTIPLES OF DESIGNSIZE/1000)");
  /* Let pltotf compute the checksum. */
  /* ploutln2("(CHECKSUM O %lo)",cksum ^ 0xffffffff) ; */
  if (rboundarychar >= 0)
    ploutln2 ("(BOUNDARYCHAR O %lo)", (unsigned long) rboundarychar);
  plleft ();
  ploutln ("FONTDIMEN");
  ploutln2 ("(SLANT R %f)", newslant);
  ploutln2 ("(SPACE D %d)", fontspace);
  ploutln2 ("(STRETCH D %d)", fstretch);
  ploutln2 ("(SHRINK D %d)", fshrink);
  ploutln2 ("(XHEIGHT D %d)", xheight);
  ploutln2 ("(QUAD D %d)", fquad);
  if (!afm2tfm_compat) ploutln2 ("(EXTRASPACE D %d)", fextra);
  plright ();

  /* beginning and end of char array */
  for (i = 0; i < 256 && tfmptrs[i] == NULL; i++);
  bc = i;
  for (i = 255; i >= 0 && tfmptrs[i] == NULL; i--);
  ec = i;

  /* ligkern table */
  plleft ();
  ploutln ("LIGTABLE");

  /* (left) boundarychar ligskerns */
  ai = lbound;
  labeled = 0;
  for (nlig = ai->ligs; nlig; nlig = nlig->next)
    if (0 != (asucc = findtfm (nlig->succ)))
      if (0 != (asub = findtfm (nlig->sub))) {
        if (!labeled) {
          ploutln ("(LABEL BOUNDARYCHAR)");
          labeled = 1;
        }
        /* handle all encodings of asucc */
        for (k = asucc->tfmnum; k >= 0; k = tfmnext[k])
          ploutln4 ("(%s %s O %o)", plligops[nlig->op],
                    plchar (k), (unsigned) asub->tfmnum);
      }
  for (nkern = ai->kerns; nkern; nkern = nkern->next)
    if (0 != (asucc = findtfm (nkern->succ))) {
      if (!labeled) {
        ploutln ("(LABEL BOUNDARYCHAR)");
        labeled = 1;
      }
      /* handle all encodings of asucc */
      for (k = asucc->tfmnum; k >= 0; k = tfmnext[k]) {
        ploutln4 ("(KRN %s R %d)%s", plchar (k),
                  nkern->delta, plname (k));
      }
    }
  if (labeled)
    ploutln ("(STOP)");

  /* other ligs and kerns */


  for (i = bc; i <= ec; i++)
    if ((ai = tfmptrs[i]) && ai->tfmnum == i && ai->nonstd_lk) {
      /* slot i is filled, points to a not-previously-encoded
       * character, and has ligs and kerns other than
       * standard letterspacing kerns. Do right boundarychar
       * only if it is a real glyph.
       */
      if (!strcmp (ai->adobename, "||"))
        continue;
      labeled = 0;
      /* do ligatures for ai = tfmptrs[i] */
      for (nlig = ai->ligs; nlig; nlig = nlig->next)
        if (0 != (asucc = findtfm (nlig->succ)))
          if (0 != (asub = findtfm (nlig->sub))) {
            /* we found a lig which really belongs in the tfm */
            if (!labeled) {
              /* also take care of all other slots for this char */
              for (j = i; j >= 0; j = tfmnext[j]) {
                ploutln3 ("(LABEL %s)%s", plchar (j), plname (j));
              }
              labeled = 1;
            }
            for (k = asucc->tfmnum; k >= 0; k = tfmnext[k]) {
              ploutln4 ("(%s %s O %o)", plligops[nlig->op],
                        plchar (k), (unsigned) asub->tfmnum);
            }
          }
      /* do kerns for ai = tfmptrs[i] */
      for (nkern = ai->kerns; nkern; nkern = nkern->next)
        if (0 != (asucc = findtfm (nkern->succ))) {
          if (!labeled) {
            for (j = i; j >= 0; j = tfmnext[j]) {
              ploutln3 ("(LABEL %s)%s", plchar (j), plname (j));
            }
            labeled = 1;
          }
          dt = nkern->delta;
          for (k = asucc->tfmnum; k >= 0; k = tfmnext[k]) {
            ploutln4 ("(KRN %s R %d)%s", plchar (k), dt, plname (k));
          }
        }
      if (labeled)
        ploutln ("(STOP)");
    }

  if (lspace) {
    for (i = bc; i <= ec; i++)
      if ((ai = tfmptrs[i]) && !ai->nonstd_lk) {
        /* do all 'standard' glyphs together.
         * no need to check for ligatures.
         * we do check for kerns, though:
         * if there are none, then apparently all space kerns
         * have been tossed out, and there is nothing to do.
         * Either way, we break out of the loop after doing the
         * first glyph with `standard' ligkerns.
         */
      if (!strcmp(ai->adobename,"||")) continue;
      if (!ai->kerns) break;
      for (j = i; j <= ec; j++) if (tfmptrs[j] && !tfmptrs[j]->nonstd_lk)
        ploutln3 ("(LABEL %s)%s", plchar (j), plname (j));
      for (nkern = ai->kerns; nkern; nkern = nkern->next)
        if (0 != (asucc = findtfm (nkern->succ))) {
          dt = nkern->delta;
          for (k = asucc->tfmnum; k >= 0; k = tfmnext[k])
            ploutln4 ("(KRN %s R %d)%s", plchar (k), dt, plname (k));
        }
      ploutln ("(STOP)");
      break;
    }
  }

  plright ();

  /* done with ligkerns; now char metrics */
  for (i = bc; i <= ec; i++)
    if (0 != (ai = tfmptrs[i])) {
      if (!strcmp (ai->adobename, "||"))
        continue;
      plleft ();
      fprintf (outfile, "CHARACTER %s", plchar (i));
      if (*plcharbuf == 'C') {
        ploutln ("");
      } else
        ploutln2 (" (comment %s)", ai->adobename);
      ploutln2 ("(CHARWD R %d)", ai->width);
      if (0 != (ht = ai->ury))
        ploutln2 ("(CHARHT R %d)", ht);
      if (ai->lly)
        ploutln2 ("(CHARDP R %d)", -ai->lly);
      if (ai->urx > ai->width)
        ploutln2 ("(CHARIC R %d)", ai->urx - ai->width);
      plright ();
    }
  if (level)
    error ("! I forgot to match the parentheses");
  fclose (outfile);
}

/*******************************************************************
 * (version and) usage
 */
static void
version(FILE *f)
{
  fputs ("afm2pl(k) 0.7.1\n", f);
#ifdef KPATHSEA
  fprintf (f, "%s\n", kpathsea_version_string);
#endif
  fputs ("Copyright (C) 2002, 2005, 2009 Siep Kroonenberg.\n\
This program is derived from afm2tfm, (C) 2002 Radical Eye Software.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the files\n\
named COPYING and afm2pl.c.\n", f);
}

#define USAGE "\
Convert an Adobe font metric file to a TeX font property list.\n\
\n\
-p ENCFILE          Read/download ENCFILE for the PostScript encoding\n\
-o                  Use octal for all character codes in the pl file\n\
-e REAL             Widen (extend) characters by a factor of REAL\n\
-s REAL             Oblique (slant) characters by REAL, generally <<1\n\
-m INTEGER          Letterspace by INTEGER/1000 em\n\
-V                  Verbose output; i.e. report on missing glyphs\n\
--help              Print this message and exit.\n\
--version           Print version number and exit.\n\n\
See the man page for full documentation.\n\n\
"

static void
afm2pl_usage(FILE *f)
{
  fputs ("Usage: afm2pl [OPTIONS]... FILE[.afm] [FILE[.pl]]\n", f);
  fputs (USAGE, f);
  fputc ('\n', f);
  fputs ("Email bug reports to ntg-afm2pl@ntg.nl\n", f);
}

/**************************************************************
 * parse commandline
 */

/* decode comma-separated list of non-negative integers.
 * initialize everything to -1 i.e. undefined
*/
static int
getnums(char *st, int *nums, int num)
{
  char *p; /* pointer into st */
  int curnum, curindex, ndigits;
  /* curnum: number to be parsed
     curindex: index for nums array
     ndigits: n. of digits found for curnum
  */
  for (curindex=0;curindex<num;curindex++) {
    nums[curindex] = -1;
  }
  for (curnum=0,curindex=0,ndigits=0,p=st;;p++) {
    if (*p==',' || *p==0) {
      /* done with this number */
      if (ndigits>0) /* we found some digits */
        nums[curindex] = curnum;
      curnum = 0;
      curindex++;
      ndigits = 0;
      if (curindex>=num || !*p) break;
    } else if (*p>='0' && *p<='9') {
      curnum = 10*curnum + (*p-'0');
      ndigits++;
    } else {
      error ("! Illegal -f parameter");
    }
  }
  if (!*p) return 1; /* end of string reached: no problems */
  else return 0;
}

/* split string on commas into strings; disregard empty strings */
static struct nstrings *
getoutnames(const char *st, struct nstrings *onames)
{
  char *argcopy;
  unsigned i;
  int j, inpart;
  argcopy = (char *) malloc (strlen (st) + 1);
  strcpy (argcopy, st);
  inpart = 0;      /* cursor not inside a constituing substring */
  for (i = 0, onames->n = 0; i < strlen (st); i++) {
    if (argcopy[i] == ',') {
      argcopy[i] = 0;
      inpart = 0;
    } else if (!inpart) {
      inpart = 1;
      onames->n++;
    }
    /* !=',', inpart: do nothing */
  }
  onames->names = (const char **) malloc (onames->n * sizeof (char *));
  inpart = 0;
  for (i = 0, j = 0; i < strlen (st); i++) {
    if (argcopy[i] == 0)
      inpart = 0;
    else if (!inpart) {
      onames->names[j++] = argcopy + i;
      inpart = 1;
    }
  }
  return onames;
}

/* call this when an option requires an argument */
#define CHECKARG3 if (argc < 3) { afm2pl_usage(stderr); exit(1); }

static void
readargs(int argc, char **argv)
{
  register int i;
  int fdims[5];

  /* skip argv[0] and look at the rest. */
  argv++;
  argc--;

  if (argc <= 0 || !strcmp (argv[0], "--help") ||
    !strcmp (argv[0], "-help") || !strcmp (argv[0], "-h")) {
    afm2pl_usage (stdout);
    exit (0);
  }

  if (!strcmp (argv[0], "--version") ||
    !strcmp (argv[0], "-version") || !strcmp (argv[0], "-v")) {
    version (stdout);
    exit (0);
  }

  /* allocate structs for ligkern filenames */
  ligfilenames = (struct nstrings *) mymalloc
    ((unsigned long) sizeof (struct nstrings));
  Ligfilenames = (struct nstrings *) mymalloc
    ((unsigned long) sizeof (struct nstrings));

  /* looping: advance argv and decrement argc to match unprocessed
   * command-line arguments.
   * After the options, we need at least an afm filename,
   * which will be handled AFTER this loop.
   */

  while (argc > 0 && *argv[0] == '-') {
    i = argv[0][1];
    /* I don't understand what this is about.
     * The comment below suggests that it is VM/CMS-specific
     * so I comment it out for other OS-es. [SK]
     */
#ifdef VMCMS
    if (i == '/')
      i = argv[0][2] - 32;      /* /a ==> A for VMS */
#endif
    switch (i) {
    case 'e':
      efactor = 1.0;
      CHECKARG3
        if (sscanf (argv[1], "%f", &efactor) == 0 || efactor < 0.01)
        error ("! Bad extension factor");
      efactorparam = argv[1];
      argv += 2;
      argc -= 2;
      break;
    case 's':
      CHECKARG3 if (sscanf (argv[1], "%f", &slant) == 0)
        error ("! Bad slant parameter");
      slantparam = argv[1];
      argv += 2;
      argc -= 2;
      break;
    case 'm':
      CHECKARG3 if (sscanf (argv[1], "%d", &lspace) == 0)
        error ("! Bad letterspacing parameter");
      lspaceparam = argv[1];
      if (lspace > 0)
        keepligs = 0;
      argv += 2;
      argc -= 2;
      break;
    case 'p':
      CHECKARG3 encfilename = argv[1];
      argv += 2;
      argc -= 2;
      break;
    case 'f':
      CHECKARG3
      if (!strcmp (argv[1], "afm2tfm")) {
        afm2tfm_compat = 1;
      } else if (!strcmp (argv[1], "afm2tfm")) {
        afm2tfm_compat = 1;
      } else {
        if (!getnums (argv[1], fdims, 5))
          error ("!Parsing error in argument of -f");
        fstretch = fdims[0]; fshrink = fdims[1]; fextra = fdims[2];
        fquad = fdims[3]; fontspace = fdims[4];
      }
      argv += 2;
      argc -= 2;
      break;
    case 'l':
      CHECKARG3 ligoption = argv[1];
      if (strcmp (ligoption, "0") && strcmp (ligoption, "1")) {
        getoutnames (ligoption, ligfilenames);
      }
      argv += 2;
      argc -= 2;
      break;
    case 'L':
      CHECKARG3 Ligoption = argv[1];
      if (strcmp (Ligoption, "0") && strcmp (Ligoption, "1")) {
        getoutnames (Ligoption, Ligfilenames);
      }
      argv += 2;
      argc -= 2;
      break;
    case 'o':
      forceoctal = 1;
      argv += 1;
      argc -= 1;
      break;
    case 'k':
      keepligs = 1;
      argv += 1;
      argc -= 1;
      break;
    case 'n':
      no_prefix = 1;
      argv += 1;
      argc -= 1;
      break;
    case 'V':
      verbose = 1;
      argv += 1;
      argc -= 1;
      break;
    default:
      (void) fprintf (stderr,
                      "Unknown option %s %s will be ignored.\n",
                      argv[0], argv[1]);
      argv += 2;
      argc -= 2;
    }
  }

  /* end of loop. Remainder: name of afm file and possibly pl file */
  afmname = argv[0];

  if ((argc < 1) || (argc > 2)) {
    error ("! need one or two non-option arguments");
    afm2pl_usage (stderr);
  }

  if (argc == 1) {
    outname = afmname;
    based_on = 1;
  } else {
    outname = argv[1];
    based_on = 0;
  }
}

/* This routine prints out the line that needs to be added to psfonts.map.
 */
static void
conspsfonts(void)
{
  char *p;
  const char *q;

  /* TeX fontname is file basename without path or extension */
  p = newstring (xbasename (outname));
  q = find_suffix (p);
  if (q)
    p[q - p - 1] = 0;
  openout (p, 0, ".map");
  (void) fprintf (outfile, "%s %s", p, fontname);
  free (p);
  if (slantparam || efactorparam || encfilename) {
    (void) fprintf (outfile, " \"");
    if (slantparam)
      (void) fprintf (outfile, " %s SlantFont", slantparam);
    if (efactorparam)
      (void) fprintf (outfile, " %s ExtendFont", efactorparam);
    if (encfilename)
      (void) fprintf (outfile, " %s ReEncodeFont", outencoding->name);
    (void) fprintf (outfile, " \"");
    if (encfilename) {
      const char *base = xbasename (encfilename);
      (void) fprintf (outfile, " <%s", base);
    }
  }
  p = newstring (xbasename (afmname));
  q = find_suffix (p);
  if (q)
    p[q - p - 1] = 0;
  (void) fprintf (outfile, " <%s.pfb", p);
  free (p);
  (void) fprintf (outfile, "\n");
  fclose (outfile);
}

/********************************************************
 * main program
 */

#ifndef VMS
int
#endif
main(int argc, char **argv)
{

#ifdef KPATHSEA
  kpse_set_program_name (argv[0], "afm2pl");

  if (argc == 1) {
    fputs ("afm2pl: Need at least one file argument.\n", stderr);
    fputs ("Try `afm2pl --help' for more information.\n", stderr);
    exit (1);
  }
  if (argc == 2) {
    if (strcmp (argv[1], "--help") == 0) {
      afm2pl_usage (stdout);
      exit (0);
    } else if (strcmp (argv[1], "--version") == 0) {
      version (stdout);
      exit (0);
    }
  }
#endif /* KPATHSEA */
  readargs (argc, argv);
  readadobe ();
  changeadobe ();
  readencoding ();
  extraligkerninfo ();  /* loop over lig files and lines in lig files */
  spaceparms ();
#ifdef AFMDEBUG
  writedump ();
#endif
  writepl ();
  conspsfonts ();
  return -missingchars;
}
