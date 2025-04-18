/* c-ctype.h: ASCII-safe versions of the <ctype.h> macros.

   Copyright 1992, 1994, 2008, 2010, 2011, 2015, 2025 Karl Berry.
   Copyright 1998, 2000, 2005 Olaf Weber.

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

#ifndef KPATHSEA_C_CTYPE_H
#define KPATHSEA_C_CTYPE_H

#include <ctype.h>

/* Be sure we have `isascii'.  */
#ifndef WIN32
#if !(defined(HAVE_DECL_ISASCII) && HAVE_DECL_ISASCII)
#define isascii(c) (((c) & ~0x7f) == 0)
#endif
#endif

/* However, we don't use isascii, since POSIX + compilers have made it
   impossible to do so portably (first failure reports for gcc 15;
   underlying reason is unclear). Leave the definition above just
   because we've always defined it, but for our use here, apparently we
   now have to define and use our own macro.  Discussion:
   https://tug.org/pipermail/tlbuild/2025q2/005743.html */

/* We want to possibly accept integers as an argument, because of the
   old problems described in the comment below. So cast to unsigned
   since otherwise checking for (i)>0 runs the risk of more useless
   compiler warnings/errors about redundant comparisons.  */
#define kpse_isascii(i) ((unsigned) (i) <= 127)

#define ISALNUM(c) (kpse_isascii (c) && isalnum((unsigned char)c))
#define ISALPHA(c) (kpse_isascii (c) && isalpha((unsigned char)c))
#define KPSE_ISASCII kpse_isascii
#define ISCNTRL(c) (kpse_isascii (c) && iscntrl((unsigned char)c))
#define ISDIGIT(c) (kpse_isascii (c) && isdigit ((unsigned char)c))
#define ISGRAPH(c) (kpse_isascii (c) && isgraph((unsigned char)c))
#define ISLOWER(c) (kpse_isascii (c) && islower((unsigned char)c))
#define ISPRINT(c) (kpse_isascii (c) && isprint((unsigned char)c))
#define ISPUNCT(c) (kpse_isascii (c) && ispunct((unsigned char)c))
#define ISSPACE(c) (kpse_isascii (c) && isspace((unsigned char)c))
#define ISUPPER(c) (kpse_isascii (c) && isupper((unsigned char)c))
#define ISXDIGIT(c) (kpse_isascii (c) && isxdigit((unsigned char)c))
#define TOASCII toascii
#define TOLOWER(c) (ISUPPER (c) ? tolower ((unsigned char)c) : (c))
#define TOUPPER(c) (ISLOWER (c) ? toupper ((unsigned char)c) : (c))

/* This isn't part of the usual <ctype.h>, but it's useful sometimes.
   We don't consider vertical tab blank, since in normal use, it isn't.  */
#ifndef isblank
#define isblank(c) ((c) == ' ' || (c) == '\t')
#endif

#define ISBLANK(c) (kpse_isascii (c) && isblank ((unsigned char)c))


/* Here's why this mess is necessary:

From: meyering@cs.utexas.edu (Jim Meyering)
Date: Wed, 25 Nov 1992 09:52:33 -0600
Subject: ss-921123: using isascii with <ctype.h> macros

  Yesterday some cursory regression testing found that GNU od
  (in an upcoming release of textutils) generated incorrect output
  when run on an SGI indigo because isprint ('\377') returned true.
  Of course, '\377' is not a printing character;  the problem lay
  in using isprint without first making sure its integer argument
  corresponded to an ascii code.

  MORAL: always guard uses of ctype macros with isascii if it's available.
  An obvious alternative is to avoid <ctype.h> and define and use your
  own versions of the ctype macros.

  A pretty clean approach to using <ctype.h> and isascii was
  suggested by David MacKenzie: [but as of 2025, no longer works; see above]

  #ifndef isascii
  #define isascii(c) 1
  #endif

  #define ISDIGIT(c) (isascii (c) && isdigit (c))
  #define ISPRINT(c) (isascii (c) && isprint (c))
  ...

  then, use ISDIGIT, etc. instead of isdigit, etc.  */

#endif /* not KPATHSEA_C_CTYPE_H */
