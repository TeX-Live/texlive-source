/* access -- test for access permissions of a file.

   Copyright 2008, 2009 Karl Berry.
   Copyright 1997-2001, 2005 Olaf Weber.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this software; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/c-unistd.h>
#include <kpathsea/c-memstr.h>

/*
 *      access mode x
 *      Returns 0 is x exists and can be accessed in accordance with mode.
 *      We use this rather than test because test looks at the permissions
 *      only, which doesn't take read-only file systems into account.
 */

int
main (int argc, char **argv)
{
    int mode;
    int status;
    char * i;

    if (argc > 1 && strcmp (argv[1], "--help") == 0) {
        printf("Usage: %s -MODE FILE\n\
  MODE is one or more of rwx.  Exit successfully if FILE exists and is\n\
  readable (r), writable (w), or executable (x).\n\
\n\
--help      display this help and exit\n\
--version   output version information and exit\n\n", argv[0]);
        fputs ("Email bug reports to tex-k@tug.org.\n", stdout);
        exit(0);
    } else if (argc > 1 && strcmp (argv[1], "--version") == 0) {
        printf ("%s (%s)\n\
Copyright (C) 2009 Olaf Weber & Karl Berry.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License\n\
For more information about these matters, see the file named COPYING.\n\
Primary author of %s: Olaf Weber.\n",
argv[0], KPSEVERSION, argv[0]);
        exit (0);
    }

    /* insist on exactly two args */
    if (argc != 3) {
        fprintf(stderr, "%s: Need exactly two arguments.\n\
Try `%s --help' for more information.\n", argv[0], argv[0]);
        exit(1);
    }

    /* The option parsing is somewhat primitive.  The mode must be
     * specified as a single parameter: an optional '-' followed by
     * zero or more letters 'r', 'w', or 'x'.  This may change.
     */
    mode = 0;
    i = argv[1];
    for (i = argv[1]; *i; ++i)
        switch (*i) {
        case 'r': mode |= R_OK; break;
        case 'w': mode |= W_OK; break;
        case 'x': mode |= X_OK; break;
        case '-': if (i == argv[1]) break;
        default:
            fprintf(stderr, "%s: Invalid MODE.\n", argv[0]);
            exit(1);
        }

    status = access(argv[2], mode);

    /* fail if the access call failed */
    if (status != 0) {
        return 1;
    }

    /* otherwise, succeed */
    return 0;
}
