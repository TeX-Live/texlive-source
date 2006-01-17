#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/bcopy.c,v 3.1 89/08/22 21:42:07 chris Exp $";
#endif

/*
 * Sample bcopy() routine.
 * This should be rewritten to be as fast as possible for your
 * machine.
 */
void bcopy(from, to, count)
	register char *from, *to;
	register int count;
{

	if (from == to)
		return;
	if (from > to) {
		while (--count >= 0)
			*to++ = *from++;
	} else {
		from += count;
		to += count;
		while (--count >= 0)
			*--to = *--from;
	}
}
