/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/sdecode.c,v 3.4 89/11/06 15:01:07 chris Exp $";
#endif

#include <stdio.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include "types.h"
#include "error.h"
#include "sdecode.h"

static char cclass[256];	/* XXX assumes 8-bit char */
#define	CCL_SPACE	0x01	/* white space */
#define	CCL_SEMI	0x02	/* `semicolon' (statement separator) */
#define	isstopc(c) (cclass[c] != 0)	/* both space and semi char */
static int sdset;		/* flag says whether cclass[] set up */

static void args(), badarg();
static int scan(struct decode_info *di, char **fmt, ...), scan_i(), scan_d();

extern char *strsave();

extern char *DVIFileName;

#ifndef BLOCK_COPY
/*
 * Copy the text at `src' `downward' in memory for `len' bytes.
 * This is like strcpy or memcpy, but handles one direction of
 * overlap (when dst < src).
 */
static void
movedown(src, dst, len)
	register char *src, *dst;
	register int len;
{

	while (--len >= 0)
		*dst++ = *src++;
}
#endif

/*
 * We have a number of states to do word interpretation.  Outside words,
 * we skip blanks and `semicolons'.  The first non-blank, non-`semi' enters
 * WORD state; in WORD state, double quote characters switch to
 * QUOTE state, after which another double quote switches back to
 * WORD state, and blanks and `semi' end WORD state, switching to DONE.
 * While in WORD or QUOTE states, `c produces c for all characters c;
 * to do this, we use two more states.
 */
#define	S_DULL	0		/* not in a word */
#define	S_WORD	1		/* in a word */
#define	S_QUOTE	2		/* inside "" in a word */
#define	S_BKQT1	3		/* not in "", but just read one ` */
#define	S_BKQT2	4		/* in "", but just read one ` */
#define	S_DONE	5		/* found end of word */

/*
 * Structure used to communicate between the various functions.
 */
struct decode_info {
	/* data used to read from input file */
	int	di_nch;		/* number of characters remaining in buffer */
	char	*di_cp;		/* next character to be read from buffer */
	long	di_nfilech;	/* number of characters remaining in file */
	FILE	*di_file;	/* the file itself */

	/* information used and/or returned by the canon routine */
	int	di_lexstate;	/* lexical state */
	int	di_call;	/* set when we reach terminating `semicolon' */

	/* information used more globally */
	char	*di_word;	/* last word from word() */
	char	*di_kw;		/* the keyword */
	int	di_bkw;		/* true iff keyword is in static buffer */
};

/*
 * `Edit' the text in the buffer in-place.
 * Return the count of characters resulting from the edit (e.g.,
 * from `foo`"bar' the count would be 7).
 * Set di_call if we reach state S_DONE by encountering a `semicolon'.
 * The edited text begins wherever di_cp points.
 * We begin the edit in the state given by di_lexstate.
 */
static int
canon(di)
	register struct decode_info *di;
{
	register int state, c, n;
	register char *cp, *outp, *s;

	state = di->di_lexstate;
	n = di->di_nch;
	cp = outp = di->di_cp;
	while (--n >= 0) {
		c = *cp++;
		switch (state) {

		case S_DULL:
			if (isstopc(c))
				continue;
			state = S_WORD;
			/* FALLTHROUGH */

		case S_WORD:
			if (c == '"') {
				state = S_QUOTE;
				continue;
			}
			if (c == '`') {
				state = S_BKQT1;
				continue;
			}
			/* look for `semicolon' (call) characters */
			if (cclass[c] & CCL_SEMI)
				di->di_call = 1;
			if (isstopc(c)) {
				state = S_DONE;
				goto done;
			}
			break;

		case S_QUOTE:
			if (c == '`') {
				state = S_BKQT2;
				continue;
			}
			if (c == '"') {
				state = S_WORD;
				continue;
			}
			break;

		case S_BKQT1:
			state = S_WORD;
			break;

		case S_BKQT2:
			state = S_QUOTE;
			break;

		default:
			panic("sdecode canon");
			/* NOTREACHED */
		}
		*outp++ = c;
	}
	n = 0;
done:
	c = outp - di->di_cp;	/* wrote this many chars */
	di->di_lexstate = state;/* save new state */
	di->di_nch = n;		/* and new counts and pointers */
	di->di_cp = cp;
	return (c);
}

#define	WDMAX	BUFSIZ		/* when testing, make this small */

/*
 * Read a word from the input file.
 * Returns NULL (and sets di_call) at end of \special.
 *
 * We gather up to WDMAX + k characters at a time into an input
 * buffer, and lex them using `canon' above.  The result is at most
 * as long as the original text.  Since a single legal word might
 * be up to WDMAX bytes long, k must be at least 1 (for the trailing
 * NUL).  If the input word is longer, we flush the trailing part,
 * using the k `extra' bytes (and again k must be at least 1).
 *
 * If there is a current keyword and it is in the current buffer
 * (di_bkw != 0), and we have to move existing buffer text around
 * or write over it with new file text, we save the keyword and
 * clear di_bkw.
 */
static char *
word(di, quietly)
	register struct decode_info *di;
	int quietly;
{
	register int n;
	register char *p;
	register int len, trunc = 0;
	static char inbuf[WDMAX + 20];	/* k=20 */

	len = 0;		/* no word bytes yet */
	p = di->di_cp;		/* where canon() writes (nil if first time) */
	di->di_lexstate = S_DULL;
	/*
	 * Loop invariants:
	 *	len is in [0..WDMAX)
	 *	di_lexstate != S_DONE
	 */
	do {
		/* if the buffer is empty, refill it */
		if (di->di_nch == 0) {
			/* compute remaining buffer space */
			n = sizeof inbuf - len;
			/* read n bytes or rest of string, whichever smaller */
			if (di->di_nfilech < n && (n = di->di_nfilech) == 0) {
				/* `eof': fake up a call-terminator */
				di->di_call = 1;
				if (di->di_lexstate == S_DULL)
					return (di->di_word = NULL);
				break;	/* take partial word */
			}
			/* save keyword if this will clobber it */
			if (di->di_bkw) {
				di->di_kw = strsave(di->di_kw);
				di->di_bkw = 0;
			}
			/* if there is some canon text, move it to front */
			if (len && p > inbuf) {
#ifdef BLOCK_COPY
				bcopy(p, inbuf, len);
#else
				movedown(p, inbuf, len);
#endif
			}
			/* canon text is (or will be) at front now */
			p = inbuf;
			di->di_cp = p + len;
			if (fread(di->di_cp, 1, n, di->di_file) != n) {
				if (ferror(di->di_file))
					error(1, -1, "error reading %s",
					    DVIFileName);
				GripeUnexpectedDVIEOF();
			}
			di->di_nfilech -= n;
			di->di_nch = n;
		}
		n = canon(di);
		if ((len += n) > WDMAX) {
			len = WDMAX;
			trunc = 1;
		}
	} while (di->di_lexstate != S_DONE);
	p[len] = 0;

	/*
	 * If the word was too long (and hence truncated), complain
	 * (unless we are quietly skipping over the remains of a
	 * broken \special).
	 */
	if (trunc && !quietly) {
		/* word was too long; complain */
		error(0, 0,
#if WDMAX > 20
		    "over-long word in \\special: `%.20s...'",
#else
		    "over-long word in \\special: `%s...'",
#endif
		    p);
		error(0, 0, "(truncated to %d characters)", WDMAX);
	}
	return (di->di_word = p);
}

/*
 * Initialise the character class table.
 */
void
SDsetclass(spaces, semis)
	register char *spaces;
	register char *semis;
{
	register int c;

	if (spaces == NULL)
		spaces = "\b\t\n\f\r ";
	if (semis == NULL)
		semis = ";\n";

	for (c = 0; c < sizeof cclass / sizeof *cclass; c++)
		cclass[c] = 0;
	while ((c = *spaces++) != 0)
		cclass[c] |= CCL_SPACE;
	while ((c = *semis++) != 0)
		cclass[c] |= CCL_SEMI;
	sdset = 1;
}

/*
 * Perform a binary search for the given string in the given table.
 */
static struct sdecode *
lookup(str, p, max)
	register char *str;
	register struct sdecode *p;
	register int max;
{
	register struct sdecode *m;
	register int dir;

	while (max > 0) {
		/*
		 * Look at the median; pick the right hand one if
		 * there are two such entries (when `max' is even).
		 * Set dir positive if str is `greater than' m->sd_name,
		 * negative if it is `less'.  If not equal, move
		 * left or right accordingly.  When max is even and
		 * we move right, we have to use (max-1)/2 since we
		 * broke the tie towards the right.
		 */
		m = p + (max >> 1);
		if ((dir = *str - *m->sd_name) == 0)
			dir = strcmp(str, m->sd_name);
		if (dir == 0)	/* found */
			return (m);
		if (dir > 0) {	/* str > mid: move right */
			p = m + 1;
			max = (max - 1) >> 1;
		} else		/* str < mid: move left */
			max >>= 1;
	}
	return (NULL);
}

/*
 * Decode a series of \special keywords.
 *
 * The table points to a sorted list of keywords to match.
 * On a match, the appropriate function is called with appropriate
 * arguments.
 *
 * Characters in `semi' (if not NULL) act like `statement separators'.
 * For instance, decoding with semi=>";\n" means that
 *
 *	\special{foo 1; bar 2 4^^Jbaz 3}
 *
 * tries to call foo(1), bar(2,4), and baz(3) (in that order).
 */
void
SDecode(fp, len, table, tsize)
	FILE *fp;
	register i32 len;
	struct sdecode *table;
	int tsize;
{
	register char *cp;
	register struct sdecode *tp;
	register int h;
	struct decode_info di;

	/* set defaults, if necessary */
	if (!sdset)
		SDsetclass((char *)NULL, (char *)NULL);

	di.di_nch = 0;
	di.di_cp = NULL;
	di.di_nfilech = len;
	di.di_file = fp;
	di.di_bkw = 0;
	for (;;) {
		di.di_call = 0;
		if ((cp = word(&di, 0)) == NULL)
			return;	/* (all done) */
		tp = lookup(cp, table, tsize);
		if (tp == NULL) {
			error(0, 0,
	"Warning: unrecognised \\special keyword `%s' ignored", cp);
			/* di_call is not set, so fall through */
		} else {
			di.di_kw = cp;
			di.di_bkw = 1;
			args(tp, &di);
			if (!di.di_call)
				error(0, 0,
	"Warning: extra arguments to \\special `%s' ignored", di.di_kw);
			if (!di.di_bkw)
				free(di.di_kw);
			di.di_bkw = 0;
		}
		/* eat extra arguments, or arguments to an unknown function */
		while (!di.di_call)
			(void) word(&di, 1);
	}
}

/*
 * Gather up the arguments to the given function, then (if all goes well)
 * call that function with those arguments.  On return, if di_call is not
 * set, there were extra arguments left over.
 */
static void
args(tp, di)
	register struct sdecode *tp;
	register struct decode_info *di;
{
	char *fmt;
	int n, room, f1;
	i32 *ip, i[4];
	double d[2];

	switch (tp->sd_args) {

	case sda_none:
		(*tp->sd_fn)(tp->sd_name);
		return;

	case sda_s:
		fmt = "s";
		if (scan(di, &fmt))
			break;
		(*tp->sd_fn)(tp->sd_name, di->di_word);
		return;

	case sda_d:
		fmt = "d";
		if (scan(di, &fmt, &i[0]))
			break;
		(*tp->sd_fn)(tp->sd_name, i[0]);
		return;

	case sda_f:
		fmt = "f";
		if (scan(di, &fmt, &d[0]))
			break;
		(*tp->sd_fn)(tp->sd_name, d[0]);
		return;

	case sda_dd:
		fmt = "dd";
		if (scan(di, &fmt, &i[0], &i[1]))
			break;
		(*tp->sd_fn)(tp->sd_name, i[0], i[1]);
		return;

	case sda_ff:
		fmt = "ff";
		if (scan(di, &fmt, &d[0], &d[1]))
			break;
		(*tp->sd_fn)(tp->sd_name, d[0], d[1]);
		return;

	case sda_ddddff:
		fmt = "ddddff";
		if (scan(di, &fmt, &i[0], &i[1], &i[2], &i[3], &d[0], &d[1]))
			break;
		(*tp->sd_fn)(tp->sd_name, i[0], i[1], i[2], i[3], d[0], d[1]);
		return;

	case sda_nd:
		f1 = 'd';
		goto array;

	case sda_nx:
		f1 = 'x';
		goto array;

	case sda_rest:
		(*tp->sd_fn)(tp->sd_name,
		    (i32)di->di_nch, di->di_cp, (i32)di->di_nfilech);
		di->di_call = 1;
		di->di_nch = 0;
		di->di_nfilech = 0;
		return;

	default:
		panic("sdecode args(): sd_argtype");
		/* NOTREACHED */
	}

	/*
	 * If we get here, there is a missing or incorrect argument.
	 */
	badarg(*fmt, di);
	return;

array:
	ip = (i32 *)malloc((unsigned)(room = 10) * sizeof(i32));
	if (ip == NULL)
		goto nomem;
	n = 0;
	while (!di->di_call && word(di, 0) != NULL) {
		if (scan_i(di->di_word, f1, &i[0])) {
			badarg(f1, di);
			free((char *)ip);
			return;
		}
		if (n == room) {
			room += 10;
			ip = (i32 *)realloc((char *)ip,
			    (unsigned)room * sizeof(i32));
			if (ip == NULL)
				goto nomem;
		}
		ip[n++] = i[0];
	}
	(*tp->sd_fn)(tp->sd_name, n, ip);
	free((char *)ip);
	return;
nomem:
	/*
	 * might as well stop: if malloc() failed, we will probably get
	 * nowhere fast anyway.
	 */
	error(1, -1,
	    "ran out of memory allocating %d bytes for \\special `%s'",
	    n * sizeof(i32), di->di_kw);
	/* NOTREACHED */

}

/*
 * Complain about an argument.  Scan() has set di_word to NULL if
 * it was missing; otherwise it is incorrect.
 */
static void
badarg(c, di)
	int c;
	struct decode_info *di;
{
	char *s;

	switch (c) {

	case 'd':
		s = "decimal";
		break;

	case 'f':
		s = "floating";
		break;

	case 's':
		s = "string";	/* must be `missing', not `bad' */
		break;

	case 'x':
		s = "hexadecimal";
		break;

	default:
		panic("sdecode badarg(%c)", c);
		/* NOTREACHED */
	}
	if (di->di_word == NULL)
		error(0, 0, "missing %s argument for \\special `%s'",
		    s, di->di_kw);
	else {
		error(0, 0, "bad %s argument to \\special `%s': `%s'",
		    s, di->di_kw, di->di_word);
		error(0, 0, "(this part of the \\special will be ignored)");
		while (!di->di_call)
			(void) word(di, 1);
	}
}

#ifdef WIN32
static int
scan(struct decode_info *di, char **fmtp, va_list ap)
{
	register int c;
	char *fmt = *fmtp;

	va_start(ap, fmt);
	while ((c = *fmt++) != 0) {
		if (di->di_call || word(di, 0) == NULL) {
			di->di_word = NULL;
			goto out;
		}
		switch (c) {

		/* BEWARE, can only handle one string */
		case 's':	/* always accepted */
			break;

		case 'd':
		case 'x':
			if (scan_i(di->di_word, c, va_arg(ap, i32 *)))
				goto out;
			break;

		case 'f':
			if (scan_d(di->di_word, va_arg(ap, double *)))
				goto out;
			break;

		default:
			panic("sdecode scan() *fmt=%c", c);
			/* NOTREACHED */
		}
	}
	c = 0;
out:
	va_end(ap);
	return (c);
}

#else
static int
scan(va_alist)
	va_dcl
{
	register struct decode_info *di;
	register char *fmt;
	register int c;
	char **fmtp;
	va_list ap;

	va_start(ap);
	di = va_arg(ap, struct decode_info *);
	fmtp = va_arg(ap, char **);
	fmt = *fmtp;
	while ((c = *fmt++) != 0) {
		if (di->di_call || word(di, 0) == NULL) {
			di->di_word = NULL;
			goto out;
		}
		switch (c) {

		/* BEWARE, can only handle one string */
		case 's':	/* always accepted */
			break;

		case 'd':
		case 'x':
			if (scan_i(di->di_word, c, va_arg(ap, i32 *)))
				goto out;
			break;

		case 'f':
			if (scan_d(di->di_word, va_arg(ap, double *)))
				goto out;
			break;

		default:
			panic("sdecode scan() *fmt=%c", c);
			/* NOTREACHED */
		}
	}
	c = 0;
out:
	va_end(ap);
	*fmtp = fmt - 1;
	return (c);
}
#endif
static int
scan_i(p, c, result)
	char *p;
	int c;
	i32 *result;
{
#ifdef HAVE_STRTOL
	char *end;
	long strtol();

	*result = strtol(p, &end, c == 'x' ? 16 : 10);
	return (*end);
#else
	long l;
	char junk;

	if (sscanf(p, c == 'x' ? "%lx%c" : "%ld%c", &l, &junk) != 1)
		return (1);
	*result = l;
	return (0);
#endif
}

static int
scan_d(p, result)
	char *p;
	double *result;
{
#ifdef HAVE_STRTOD
	char *end;
	double strtod();

	*result = strtod(p, &end);
	return (*end);
#else
	char junk;

	return (sscanf(p, "%lf%c", result, &junk) != 1);
#endif
}
