/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/split.c,v 3.1 89/08/22 21:59:36 chris Exp $";
#endif

#include <ctype.h>

/*
 * Split a line into an array of words.  This is destructive of
 * the original line; the word pointers point to places within
 * that line.
 *
 * The pound-sign character `#', at the beginning of the line or
 * after white space, marks the end of the line.
 *
 * Return the number of words made, or -1 for overflow.
 */

/*
 * The lexical states are much like `sh's, except that we also do
 * C-style backslash-escapes.
 */
enum lexstate {
	S_BLANK,		/* outside a word */
	S_WORD,			/* inside a word, no quoting */
	S_SQUOTE,		/* inside a single quote */
	S_DQUOTE,		/* inside a double quote */
	S_BKSL0,		/* last char was \ */
	S_BKSL1,		/* last chars were \, [0-7] */
	S_BKSL2			/* last chars were \, [0-7][0-7] */
};

int
split(s, w, nw)
	register char *s, **w;
	int nw;
{
	register int c;
	register char *canon = s;
	register int wleft = nw;
	enum lexstate state, prebkstate;

	/*
	 * Start out in the `blank' state (outside a word).  Handle
	 * quotes and things.  Backslashes are handled by saving the
	 * `pre-backslash' state, doing the backslash, and restoring
	 * that state at the end of the backslash sequence.
	 */
	state = S_BLANK;
	while ((c = *s++) != 0) {
reswitch:
		switch (state) {

		/*
		 * Blanks: spaces stay in blank state; '#' ends the line;
		 * anything else starts a word.  However, quotes may put
		 * us into quote states, rather than word states.
		 */
		case S_BLANK:
			if (isspace(c))
				continue;
			if (c == '#')
				goto stopped_by_comment;
			if (--wleft < 0)
				return (-1);
			*w++ = canon;
			state = S_WORD;
			/* FALLTHROUGH */

		/*
		 * In a word.  Spaces take us out (and end the
		 * current word).  Quotes, however, put us into
		 * quote states.
		 */
		case S_WORD:
			if (isspace(c)) {
				*canon++ = 0;
				state = S_BLANK;
				break;
			}
			if (c == '\'') {
				state = S_SQUOTE;
				break;
			}
			if (c == '"') {
				state = S_DQUOTE;
				break;
			}
			if (c == '\\') {
				prebkstate = S_WORD;
				state = S_BKSL0;
				break;
			}
			*canon++ = c;
			break;

		/*
		 * Inside a single quote, the only special character
		 * is another single quote.  This matches the Bourne
		 * shell quoting convention exactly.
		 */
		case S_SQUOTE:
			if (c == '\'')
				state = S_WORD;
			else
				*canon++ = c;
			break;

		/*
		 * Inside a double quote, double quotes get us out,
		 * but backslashes must be interpreted.
		 */
		case S_DQUOTE:
			if (c == '\\') {
				prebkstate = S_DQUOTE;
				state = S_BKSL0;
			} else if (c == '"')
				state = S_WORD;
			else
				*canon++ = c;
			break;

		/*
		 * If we are handling a backslash, we will either
		 * restore the state, or go to BKSL1 state.  In
		 * the latter case, do not advance the canonicalisation
		 * pointer, since we might have more octal digits
		 * to insert.
		 */
		case S_BKSL0:
			state = prebkstate;	/* probably */
			switch (c) {

			case 'b':
				*canon++ = '\b';
				break;

			case 'f':
				*canon++ = '\f';
				break;

			case 'n':
				*canon++ = '\n';
				break;

			case 'r':
				*canon++ = '\r';
				break;

			case 't':
				*canon++ = '\t';
				break;

			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				*canon = c - '0';
				state = S_BKSL1;
				break;

			default:
				*canon++ = c;
				break;
			}
			break;


		/*
		 * In BKSL1, we have seen backslash and one octal
		 * digit.  There may be more (in which case just
		 * count them on in), or there might be something
		 * that requires we restore the state and try again.
		 */
		case S_BKSL1:
			switch (c) {

			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				*canon <<= 3;
				*canon |= c - '0';
				state = S_BKSL2;
				break;

			default:
				canon++;
				state = prebkstate;
				goto reswitch;
			}
			break;

		/*
		 * BKSL2 is like BKSL1, except that it cannot
		 * help but restore the original state, since
		 * there are no four-character octal sequences.
		 */
		case S_BKSL2:
			state = prebkstate;	/* assuredly */
			switch (c) {

			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				*canon <<= 3;
				*canon++ |= c - '0';
				break;

			default:
				canon++;
				goto reswitch;
			}
			break;
		}
	}
stopped_by_comment:
#ifdef notdef
	if (state != S_WORD && state != S_BLANK)
		error(0, 0, "warning: unclosed quote");
#endif
	if (state != S_BLANK)
		*canon = 0;
	return (nw - wleft);
}
