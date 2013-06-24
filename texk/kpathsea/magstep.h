/* magstep.h: declaration for magstep fixing.

   Copyright 1994, 2008, 2009 Karl Berry.
   Copyright 1999, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef KPATHSEA_MAGSTEP_H
#define KPATHSEA_MAGSTEP_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* If DPI is close enough to some magstep of BDPI, return the true dpi
   value, and the magstep found (or zero) in M_RET (if
   non-null). ``Close enough'' means within one pixel.

   M_RET is slightly encoded: the least significant bit is on for a
   half-magstep, off otherwise.  Thus, a returned M_RET of 1 means
   \magstephalf, i.e., sqrt(1.2), i.e., 1.09544.  Put another way,
   return twice the number of magsteps.

   In practice, this matters for magstephalf.  Floating-point computation
   with the fixed-point DVI representation leads to 328 (for BDPI ==
   300); specifying `at 11pt' yields 330; the true \magstephalf is 329
   (that's what you get if you run Metafont with mag:=magstep(.5)).

   The time to call this is after you read the font spec from the DVI
   file, but before you look up any files -- do the usual floating-point
   computations, and then fix up the result.  */

extern KPSEDLL unsigned kpathsea_magstep_fix (kpathsea kpse, unsigned dpi,
                                              unsigned bdpi, int *m_ret);

#if defined (KPSE_COMPAT_API)
extern KPSEDLL unsigned kpse_magstep_fix (unsigned dpi, unsigned bdpi,
                                          int *m_ret);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_MAGSTEP_H */
