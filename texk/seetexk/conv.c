/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/conv.c,v 3.1 89/08/22 21:48:01 chris Exp $";
#endif

/*
 * Conversions.
 */

#include "types.h"
#include "conv.h"

double	DMagFactor();

Conv	Conversion;		/* the global conversion */

/*
 * Set a conversion (possibly the global conversion).
 */
void
CSetConversion(c, dpi, usermag, num, denom, dvimag)
	register struct conversion *c;
	int dpi, usermag;
	i32 num, denom, dvimag;
{
	double ddpi = dpi;

	c->c_mag = DMagFactor((int) dvimag) * DMagFactor(usermag);
	c->c_dpi = ddpi;

	/*
	 * The conversion facture is figured as follows:  there are exactly
	 * num/denom DVI units per decimicron, and 254000 decimicrons per
	 * inch, and dpi pixels per inch.  Then we have to adjust this by
	 * the stated magnification. 
	 */
	c->c_fromsp = (num / 254000.0) * (ddpi / denom) * c->c_mag;

	/*
	 * c->c_tosp is 1/c->c_fromsp, but we will invert the expression
	 * above in the hopes of some extra accuracy.
	 *
	 * IS THIS ANY GOOD?  I NEED A NUMERICAL ANALYST!
	 */
	c->c_tosp = (254000.0 / num) * (denom / ddpi) * (1.0 / c->c_mag);
}
