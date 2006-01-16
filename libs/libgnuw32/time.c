/* Utility and Unix shadow routines for XEmacs on Windows NT.
   Copyright (C) 1994, 1995 Free Software Foundation, Inc.

This file is part of XEmacs.

XEmacs is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

XEmacs is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with XEmacs; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.


   Geoff Voelker (voelker@cs.washington.edu) 7-29-94 */

/* Adapted for XEmacs by David Hobley <david@spook-le0.cia.com.au> */
/* Sync'ed with Emacs 19.34.6 by Marc Paquette <marcpa@cam.org> */
/* Adapted to fpTeX 0.4 by Fabrice Popineau <Fabrice.Popineau@supelec.fr> */

#include <win32lib.h>

FILETIME utc_base_ft;
static long double utc_base;
static int init = 0;

time_t
convert_time (FILETIME ft)
{
  long double ret;

  if (!init)
    {
      /* Determine the delta between 1-Jan-1601 and 1-Jan-1970. */
      SYSTEMTIME st;

      st.wYear = 1970;
      st.wMonth = 1;
      st.wDay = 1;
      st.wHour = 0;
      st.wMinute = 0;
      st.wSecond = 0;
      st.wMilliseconds = 0;

      SystemTimeToFileTime (&st, &utc_base_ft);
      utc_base = (long double) utc_base_ft.dwHighDateTime
	* 4096 * 1024 * 1024 + utc_base_ft.dwLowDateTime;
      init = 1;
    }

  if (CompareFileTime (&ft, &utc_base_ft) < 0)
    return 0;

  ret = (long double) ft.dwHighDateTime * 4096 * 1024 * 1024 + ft.dwLowDateTime;
  ret -= utc_base;
  return (time_t) (ret * 1e-7);
}
