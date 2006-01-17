/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /homes/grunwald/Src/SeeTeX/libtex/RCS/font.c,v 1.2 1992/07/22 16:26:02 grunwald Exp grunwald $";
#endif

/*
 * Routines for working with fonts.  In particular, the configuration
 * dependent code is here.
 *
 * Specific fonts (GF, PXL, etc.) have functions in separate files.
 */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include "types.h"
#include "conv.h"
#include "error.h"
#include "font.h"

/*
 * Define the default configuration file.
 * Also define the maximum path name length.
 */
#ifndef FONTDESC
/* #define FONTDESC "/usr/local/lib/tex/fontdesc" */
	you must define FONTDESC in the Makefile
#endif

#define	PATHLEN	1024

/*
 * A font configuration.  The font list is ordered.
 *
 * A specifier is typically a particular print engine, since
 * different engines need slightly different fonts.
 */
struct fontconf {
	struct	fontconf *fc_next;
	struct	fontops *fc_ops;
	char	*fc_path;	/* path, with metacharacters */
	char	*fc_spec;	/* specifier */
	int	fc_slop;	/* slop value */
};

/*
 * EQ is a fast way to check for string equivalence.
 */
#define	EQ(a, b) (*(a) == *(b) && strcmp(a, b) == 0)

/*
 * Private variables and functions.
 */
static	int didinit;		/* true => initialised already */
static	char *cfname;		/* config file name, for errors */
static	int cfline;		/* config file line, likewise */
static	struct fontops *fontops;/* font operations code: list head */
static	struct fontconf *fonts;	/* font list */
static	struct fontconf **nextfc;/* used during initialisation */
static	char spec_any[] = "*";	/* the `anything' specifier */

static void readconf(), badcf(), setfenv(), setfont();
static struct font *getafont();

/*
 * Imports.
 */
extern	int errno;
char	*getenv(), *malloc(), *strncpy(), *strsave();

/*
 * You may get warnings from lint about sprintf(), which is of type
 * `char *' in early 4BSD releases.  Ignore the warning.
 */

/*
 * Here, alas, we know about all the kinds of fonts.
 * This also means that every DVI interpreter pulls in
 * the full set of font manipulation routines.
 *
 * PERHAPS THIS SHOULD BE CONFIGURABLE.
 */
#define	ADDFONT(x) { \
	extern struct fontops x; \
	x.fo_next = fontops; \
	fontops = &x; \
}

void
fontinit(file)
	char *file;
{

	if (didinit) {
		/*
		 * Could free the old configuration and fire up
		 * a new one, but for now . . .
		 */
		error(1, 0, "attempt to reinit fonts");
		/* NOTREACHED */
	}
	didinit++;
	ADDFONT(invisops);
	ADDFONT(tfmops);
	ADDFONT(blankops);
	ADDFONT(boxops);
	ADDFONT(pxlops);
	ADDFONT(gfops);
	ADDFONT(pkops);
	nextfc = &fonts;
	if (file == NULL)
		if ((file = getenv(CONFENV)) == NULL)
			file = FONTDESC;
	readconf(file);
}

/*
 * Find a font's operations, given its name.
 */
static struct fontops *
findops(name)
	register char *name;
{
	register struct fontops *fo;

	for (fo = fontops; fo != NULL; fo = fo->fo_next)
		if (EQ(fo->fo_name, name))
			return (fo);
	return (NULL);
}

/*
 * Read the named configuration file.  The file is split into
 * lines, and lines are split into words; if the first word is
 * "font", this is a fontconf, and we read the remainder of the
 * words and make a fontconf entry.
 */
static void
readconf(name)
	char *name;
{
	register FILE *f;	/* config file */
	register int c;		/* char and word counter */
	int isenv;		/* true => doing `fontenv', not `font' */
	char *env;		/* value from getenv() */
	struct fontconf proto;	/* prototype fontconf */
	char *v[20];		/* word vector */
	char line[BUFSIZ];	/* input line */

	if ((f = fopen(name, "r")) == NULL)
		error(1, -1, "cannot read font configuration file \"%s\"",
		    name);
	cfname = name;
	cfline = 0;
	while (fgets(line, sizeof (line), f) != NULL) {
		cfline++;
		if ((c = strlen(line)) > 0) {
			if (line[--c] != '\n')
				badcf("line too long");
			line[c] = 0;
		}
		if ((c = split(line, v, sizeof v / sizeof *v)) < 0)
			badcf("too many words");
		/* skip things that are not fonts */
		if (c == 0)
			continue;
		if (EQ(v[0], "font"))
			isenv = 0;
		else if (EQ(v[0], "fontenv"))
			isenv = 1;
		else
			continue;
		switch (c) {
		case 1:
			badcf("missing font typename");
			/* NOTREACHED */
		case 2:
			badcf("missing font spec (engine)");
			/* NOTREACHED */
		case 3:
			badcf("missing slop value");
			/* NOTREACHED */
		case 4:
			badcf(isenv ? "need environment variable" :
			    "need pathname");
			/* NOTREACHED */
		case 5:
			if (isenv)
				badcf("need suffix");
			break;
		case 6:
			if (isenv)
				break;
		default:
			error(0, 0,
			    "%s, line %d: warning: %d extra word(s) ignored",
			    cfname, cfline, c - 5);
			break;
		}
		if ((proto.fc_ops = findops(v[1])) == NULL) {
			error(0, 0,
			    "\"%s\", line %d: unknown font type \"%s\" ignored",
			    cfname, cfline, v[1]);
			continue;
		}

		/*
		 * For fontenv, if the environment variable is not actually
		 * set in the environment, just skip over this line.
		 */
		
		/* Debugging (LOG) */
		printf(" texsunLOG: ");
		printf(v[0]);
		printf(" .");
		printf(v[1]);
		printf(" ");
		if (isenv)
			printf("'");
		printf(v[4]);
		if (isenv)
			printf("'");
		printf(" does:\n");

		if (isenv && (env = getenv(v[4])) == NULL)
			continue;
		proto.fc_next = NULL;
		proto.fc_spec = EQ(v[2], spec_any) ? NULL : strsave(v[2]);
		proto.fc_slop = atoi(v[3]);
		if (proto.fc_slop < 0)	/* quietly enforce proper slops */
			proto.fc_slop = 0;
		if (isenv) {
			printf("  = ");
			printf(env);
			printf("\n");
			setfenv(&proto, env, v[5]);
			}
		else
			setfont(&proto, strsave(v[4]));
	}
}

/*
 * Handle fontenv lines by appending as many configuration entries
 * as there are components in the given environment variable.
 */
static void
setfenv(pfc, env, suf)
	struct fontconf *pfc;
	char *env, *suf;
{
	register char *s, *t;
	register int len, suflen = strlen(suf);
	register char *mem, *slash;
	char *munch = strsave(env);	/* a copy we can write on */
	struct fontops *fo;

	/*
	 * Loop over the path.  Turn `foo' into `foo/<suf1><suf2>';
	 * turn an empty string into just <suf1><suf2>.  Put these
	 * names in fresh memory and set them up as places to search.
	 */

	for (s = munch; (t = s) != NULL; s = t) {
		for (;;) {
			if (*t == ':') {
				*t++ = 0;
				len = t - s;	/* includes trailing NUL */
				break;
			}
			if (*t++ == 0) {
				len = t - s;	/* includes trailing NUL */
				t = NULL;
				break;
			}
		}
		if (len != 0) {
			slash = "/";
			len++;
		} else
			slash = "";
		if ((mem = malloc(len + suflen)) == NULL)
			error(1, -1, "out of memory while saving $%s", env);
		(void) sprintf(mem, "%s%s%s", s, slash, suf);
		setfont(pfc, mem);
	}
	free(munch);
}

/*
 * Turn a prototype fontconf into a real one.
 */
static void
setfont(pfc, path)
	struct fontconf *pfc;
	char *path;
{
	register struct fontconf *fc;

	if ((fc = (struct fontconf *)malloc(sizeof(*fc))) == NULL)
		error(1, -1, "out of memory for font configuration (sorry)");
	*fc = *pfc;
	fc->fc_path = path;
	*nextfc = fc;
	nextfc = &fc->fc_next;

	printf(" texsunLOG: scanning ");
	printf(path);
	printf("\n");
		/* message for found path (logging) */
}

/*
 * Complain about a problem in the configuration file.
 */
static void
badcf(why)
	char *why;
{

	error(1, 0, "\"%s\", line %d: %s", cfname, cfline, why);
	/* NOTREACHED */
}

/*
 * Copy string s into buffer buf, dropping off any numerical suffix.
 * E.g., cmr7 and cmr10 become cmr.  (This is the `%b' operator.)
 */
static void
basify(s, buf, size)
	char *s, *buf;
	int size;
{
	register char *p, *endp;
	register int n;

	for (p = s; *p; p++)
		if (!isdigit(*p))
			endp = p;
	if ((n = endp + 1 - s) >= size)
		error(1, 0, "font name `%s' too long for %%b (sorry)", s);
	(void) strncpy(buf, s, n);
	buf[n] = 0;
}

/*
 * Turn a prototype path, name, and magnification into a full
 * path (expand %f, %m, %b).
 */
static void
pave(result, proto, name, mag)
	char *result, *proto, *name;
	int mag;
{
	register int c;
	register char *s, *d, *p;
	char num[30], fontbase[256];

	d = result;
	p = proto;
	s = NULL;
	num[0] = 0;		/* will need changing for other bases */
	fontbase[0] = 0;

	while (p != NULL) {
		/*
		 * If sourcing from s, take its next character, and
		 * insert it directly.  Otherwise take the next path
		 * character and interpret it.
		 */
		if (s != NULL) {
			if ((c = *s++) == 0) {
				s = NULL;
				continue;
			}
			goto put;
		}
		if ((c = *p++) == 0)
			p = NULL;
		if (c != '%')
			goto put;

		switch (c = *p++) {

		case 'b':
			if (fontbase[0] == 0)
				basify(name, fontbase, sizeof(fontbase));
			s = fontbase;
			continue;

		case 'f':
		case 'n':
		case 's':
			s = name;
			continue;

		case 'd':
		case 'm':
			if (num[0] == 0)
				(void) sprintf(num, "%d", mag);
			s = num;
			continue;

		case 0:
			c = '%';
			p--;
			/* FALLTHROUGH */
		}
put:
		if (d - result >= PATHLEN)
			error(1, 0, "font path `%s' too long (sorry)", proto);
		*d++ = c;
	}
}

/*
 * Given a font name and size, return the first font that fits, along
 * with its name (via fname).  If we cannot find such a font, we set
 * *fname to point to a `canonical' example font name, unless there are
 * are no fonts for the device, in which case we set *fname to NULL.
 */
struct font *
GetFont(nm, dvimag, dvidsz, dev, fname)
	char *nm;
	i32 dvimag, dvidsz;
	char *dev, **fname;
{

	return (getafont(nm, dvimag, dvidsz, dev, fname, 1));
}

/*
 * Same as GetFont, but caller promises never to ask for rasters.
 */
struct font *
GetRasterlessFont(nm, dvimag, dvidsz, dev, fname)
	char *nm;
	i32 dvimag, dvidsz;
	char *dev, **fname;
{

	return (getafont(nm, dvimag, dvidsz, dev, fname, 0));
}

/*
 * NEED TO THINK ABOUT gf NAMING CONVENTIONS HERE: ARE THEY LIKE pxl?
 * WHAT ABOUT OTHERS?
 */
static struct font *
getafont(nm, dvimag, dvidsz, dev, fname, wantrast)
	char *nm;
	i32 dvimag, dvidsz;
	char *dev, **fname;
	int wantrast;
{
	register int slop, fmag;
	register struct font *f;
	register struct fontconf *fc;
	register struct fontops *fo;
	register char *path;
	int fd, scaled;
	double mag;
	static char firstpath[PATHLEN], laterpath[PATHLEN];

	if (!didinit)
		fontinit((char *)NULL);

	/*
	 * The equation below means, approximately, `the font is
	 * magnified by the ratio of the actual size dvimag to the
	 * design size dvidsz, and then further scaled by the
	 * global magnification.'  We multiply this by the printer's
	 * resolution in dots per inch, then use the per-font
	 * conversion factor to convert a dots-per-inch value to
	 * a font name `%m' magnification (extension).
	 */
	mag = (double) dvimag / (double) dvidsz;
	scaled = mag * 1000.0 + 0.5;
	mag *= Conversion.c_mag * Conversion.c_dpi;

	path = firstpath;	/* no canonical font name yet */
	for (fc = fonts; fc != NULL; fc = fc->fc_next) {
		if (dev != NULL && fc->fc_spec != NULL &&
		    !EQ(dev, fc->fc_spec))
			continue;
		/*
		 * Fake fonts are ignored unless there is already
		 * `canonical' real font.  The first non-fake font
		 * is the `canonical' one.
		 */
		fo = fc->fc_ops;
		if (path == firstpath && fo->fo_fakefont)
			continue;
		fmag = mag * fo->fo_dpitomag + 0.5;
		for (slop = 0; slop <= fc->fc_slop; slop++) {
			pave(path, fc->fc_path, nm, fmag + slop);
			if ((fd = open(path, 0)) >= 0)
				goto found;
			path = laterpath;	/* tried at least one now */
			if (slop) {
				pave(path, fc->fc_path, nm, fmag - slop);
				if ((fd = open(path, 0)) >= 0)
					goto found;
			}
		}
	}

	/* not found */
	if (path == firstpath) {
		*fname = NULL;
		errno = ENXIO;
	} else {
		*fname = firstpath;
		errno = ENOENT;
	}
	return (NULL);

found:
	*fname = path;

	/* allocate space for the per-font info, and read it in */
	f = (struct font *)malloc(sizeof *f);
	if (f == NULL) {
		errno = ENOMEM;
		return (NULL);
	}
	f->f_flags = wantrast ? FF_RASTERS : 0;
	f->f_ops = fc->fc_ops;
	f->f_path = strsave(path);
	f->f_font = strsave(nm);
	f->f_dvimag = dvimag;
	f->f_dvidsz = dvidsz;
	f->f_scaled = scaled;
	f->f_pspace = f->f_dvimag / 6;	/* a three-unit "thin space" */
	f->f_nspace = -4 * f->f_pspace;
	f->f_vspace = 5 * f->f_pspace;
	errno = 0;
	if ((*f->f_ops->fo_read)(f, fd)) {
		int e = errno;	/* paranoid */

		free(f->f_path);
		free(f->f_font);
		free((char *)f);
		errno = e;
		return (NULL);
	}
	if (f->f_ops->fo_fakefont) {
		error(0, 0, "Warning: no font for %s", Font_TeXName(f));
		if (firstpath[0])
			error(0, 0, "(wanted, e.g., \"%s\")", firstpath);
		error(0, 0, "I will substitute %s for the missing glyphs",
		    f->f_ops->fo_fakefont == 1 ? "boxes" : "white space");
	}
	return (f);
}

