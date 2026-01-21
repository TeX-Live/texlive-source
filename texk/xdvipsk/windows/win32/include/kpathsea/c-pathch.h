/* c-pathch.h: define the characters which separate components of
   filenames and environment variable paths.

   Copyright 1992, 1993, 1995, 1997, 2008, 2018 Karl Berry.
   Copyright 1997, 1999, 2001, 2005 Olaf Weber.

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

#ifndef C_PATHCH_H
#define C_PATHCH_H

#include <kpathsea/c-ctype.h>

/* What separates filename components?  */
#ifndef DIR_SEP
# if defined(VMS)
#  define DIR_SEP ':'
#  define DIR_SEP_STRING ":"
# elif defined(DOSISH) /* not VMS */
/* Either \'s or 's work.  Wayne Sullivan's web2pc prefers /, so we'll
   go with that.  */
#  define DIR_SEP '/'
#  define DIR_SEP_STRING "/"
#  define IS_DEVICE_SEP(ch) ((ch) == ':')
#  define NAME_BEGINS_WITH_DEVICE(name) (*(name) && IS_DEVICE_SEP((name)[1]))
/* On DOS, it's good to allow both \ and / between directories.  */
#  define IS_DIR_SEP(ch) ((ch) == '/' || (ch) == '\\')
#  ifdef WIN32
/*  On win32, UNC names are authorized */
#   define IS_UNC_NAME(name) (strlen(name)>=3 && IS_DIR_SEP(*name)  \
                               && IS_DIR_SEP(*(name+1)) && isalnum(*(name+2)))
/*  Used after converting '\\' into '/' */
#   define IS_DIR_SEP_CH(ch) ((ch) == '/')
#  endif
# elif defined(AMIGA) /* not DOSISH */
#  define DIR_SEP '/'
#  define DIR_SEP_STRING "/"
#  define IS_DIR_SEP(ch) ((ch) == '/' || (ch) == ':')
#  define IS_DEVICE_SEP(ch) ((ch) == ':')
# elif defined(VMCMS) /* not AMIGA */
#  define DIR_SEP ' '
#  define DIR_SEP_STRING " "
# else /* not VMCMS */
#  define DIR_SEP '/'
#  define DIR_SEP_STRING "/"
# endif /* not VMCMS */
#endif /* not DIR_SEP */

#ifndef IS_DIR_SEP
#define IS_DIR_SEP(ch) ((ch) == DIR_SEP)
#endif
#ifndef IS_DIR_SEP_CH
#define IS_DIR_SEP_CH(ch) IS_DIR_SEP(ch)
#endif
#ifndef IS_DEVICE_SEP /* No `devices' on, e.g., Unix.  */
#define IS_DEVICE_SEP(ch) 0
#endif
#ifndef NAME_BEGINS_WITH_DEVICE
#define NAME_BEGINS_WITH_DEVICE(name) 0
#endif
#ifndef IS_UNC_NAME /* Unc names are in practice found on Win32 only. */
#define IS_UNC_NAME(name) 0
#endif

/* What separates elements in environment variable path lists?  */
#ifndef ENV_SEP
#ifdef VMS
# define ENV_SEP ','
# define ENV_SEP_STRING ","
#elif defined (DOSISH)
# define ENV_SEP ';'
# define ENV_SEP_STRING ";"
#elif defined (AMIGA)
# define ENV_SEP ';'
# define ENV_SEP_STRING ";"
#elif defined (VMCMS)
# define ENV_SEP ' '
# define ENV_SEP_STRING " "
#else
# define ENV_SEP ':'
# define ENV_SEP_STRING ":"
/* Because paths in Kpathsea cnf files allow use of either ; or : separators
   regardless of the current system, on Unix we sometimes need to check
   for either .  */
# define IS_KPSE_SEP(ch) ((ch) == ':' || (ch) == ';')
#endif
#endif /* not ENV_SEP */

#ifndef IS_ENV_SEP
#define IS_ENV_SEP(ch) ((ch) == ENV_SEP)
#endif

#ifndef IS_KPSE_SEP
/* But for Windows, we do not want to consider : as a path separator,
   ever, because it is the drive separator (as in c:\tex).  So just
   check for the regular separator (;).  */
#define IS_KPSE_SEP(ch) (IS_ENV_SEP (ch))
#endif

#endif /* not C_PATHCH_H */
