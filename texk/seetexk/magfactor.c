/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Convert a magnification factor to floating point.  This is used in
 * conjunction with the FONT_SLOP stuff to try to get the file names
 * right, and may also be used by DVI reading programs to get slightly
 * more accurate values for (mag/1000.0).
 */
double
DMagFactor(mag)
	int mag;
{

	switch (mag) {

	case 1095:		/* stephalf */
		return (1.095445);

	case 1315:		/* stepihalf */
		return (1.314534);

	case 2074:		/* stepiv */
		return (2.0736);

	case 2488:		/* stepv */
		return (2.48832);

	case 2986:		/* stepiv */
		return (2.985984);

	default:		/* remaining mags have been ok */
		return ((double) mag / 1000.);
	}
	/* NOTREACHED */
}
