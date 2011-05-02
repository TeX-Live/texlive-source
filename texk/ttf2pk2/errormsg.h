/*
 *   errormsg.h
 *
 *   This file is part of the ttf2pk package.
 *
 *   Copyright 1997-1999, 2000 by
 *     Frederic Loyer <loyer@ensta.fr>
 *     Werner Lemberg <wl@gnu.org>
 */

#ifndef ERRORMSG_H
#define ERRORMSG_H


/*
   Disable some annoying warnings when you compile with -W3.  Namely the
   warnings about missing __cdecl specifiers at some places where they are
   not really needed because the compiler can generate them.

   This is relevant only if you compile with -Gr option, i.e., use fastcall
   calling convention.

   This is needed for fpTeX.
 */

#ifdef WIN32
#pragma warning (disable : 4007 4096)
#define XCDECL __cdecl
#else
#define XCDECL
#endif


extern const char *progname;         /* the origin of the error/warning message */

void oops(const char *message,
          ...);
void boops(const char *buffer,
           size_t offset,
           const char *message,
           ...);
void warning(const char *message,
             ...);

#endif /* ERRORMSG_H */


/* end */
