
/*
 * Copyright (C) Yves Arrouye <Yves.Arrouye@marin.fdn.fr>, 1996.
 *
 * Use under the GPL version 2. You are not allowed to remove this
 * copyright notice.
 *
 */

#ifndef DIMEN_H
#define DIMEN_H

/*
 * psdimension() converts a string to a number of points, accepting a
 *   string consisting of a number (eventually decimal but without any
 *   exponent) followed by a unit (one of `m', `dm', `cm', `mm', `ft',
 *   `in' or `pt', `in' being the default); on success it returs 0,
 *   otherwise it returns -1 if no number was found or 1 if the unit is
 *   incorrect.
 *
 */

#if __STDC__ - 0 == 0
#define const
#endif

#if __STDC__ - 0 == 0
extern float unitfactor();
extern int psdimension();
#else
extern float unitfactor(const char* unit);
extern int psdimension(const char* dimspec, int* dim);
#endif

#endif

