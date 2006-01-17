#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/bzero.c,v 3.1 89/08/22 21:42:12 chris Exp $";
#endif

/*
 * Sample bzero() routine.
 * This should be rewritten to be as fast as possible for your
 * machine.
 */
bzero(addr, count)
	register char *addr;
	register int count;
{

	while (--count >= 0)
		*addr++ = 0;
}
