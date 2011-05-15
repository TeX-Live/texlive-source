/* absolute.c: test if a filename is absolute or explicitly relative.

   Copyright 1993, 1994, 1995, 2008, 2009, 2010, 2011 Karl Berry.
   Copyright 1997, 2002, 2005 Olaf Weber.

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

#include <kpathsea/config.h>

#include <kpathsea/absolute.h>
#include <kpathsea/c-pathch.h>

/* Sorry this is such a system-dependent mess, but I can't see any way
   to usefully generalize.  */

boolean
kpathsea_absolute_p (kpathsea kpse, const_string filename, boolean relative_ok)
{
#ifdef VMS
#include <string.h>
  (void)kpse; /* currenty not used */
  return strcspn (filename, "]>:") != strlen (filename);
#else /* not VMS */
  boolean absolute;
  boolean explicit_relative;

  absolute = IS_DIR_SEP (*filename)
#ifdef DOSISH
                     /* Novell allows non-alphanumeric drive letters. */
                     || (*filename && IS_DEVICE_SEP (filename[1]))
#endif /* DOSISH */
#ifdef WIN32
                     /* UNC names */
                     || (*filename == '\\' && filename[1] == '\\')
                     || (*filename == '/' && filename[1] == '/')
#endif /* WIN32 */
#ifdef AMIGA
                     /* Colon anywhere means a device.  */
                     || strchr (filename, ':')
#endif /* AMIGA */
                      ;
  explicit_relative
    = relative_ok
#ifdef AMIGA
      /* Leading / is like `../' on Unix and DOS.  Allow Unix syntax,
         too, though, because of possible patch programs like
         `UnixDirsII' by Martin Scott.  */
      && IS_DIR_SEP (*filename) || 0
#endif /* AMIGA */
      && (*filename == '.' && (IS_DIR_SEP (filename[1])
                         || (filename[1] == '.' && IS_DIR_SEP (filename[2]))));

  (void)kpse; /* currenty not used */
  /* FIXME: On UNIX an IS_DIR_SEP of any but the last character in the name
     implies relative.  */
  return absolute || explicit_relative;
#endif /* not VMS */
}

#if defined (KPSE_COMPAT_API)
boolean
kpse_absolute_p (const_string filename, boolean relative_ok)
{
    return kpathsea_absolute_p (kpse_def, filename, relative_ok);
}
#endif

#ifdef TEST
int main()
{
  char **name;
  char *t[] = { "./foo", "\\\\server\\foo\\bar", "ftp://localhost/foo" };

  for (name = t; name - t < sizeof(t)/sizeof(char*); name++) {
    printf ("Path `%s' %s absolute.\n", *name,
            kpse_absolute_p(*name, true) ? "is" : "is not");
  }
}
#endif /* TEST */

/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST absolute.c kpathsea.a"
End:
*/
