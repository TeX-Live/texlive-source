/* xgetcwd.c: a from-scratch version of getwd, returning result in new
   memory.  Never uses a system getwd.  Ideas from tcsh 5.20 source.

   Copyright 1992, 1994, 1996, 2008, 2011, 2016, 2026 Karl Berry.
   Copyright 2005 Olaf Weber.

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

#if defined (HAVE_GETCWD) && !defined (GETCWD_FORKS)
#include <kpathsea/c-pathmx.h>

#else /* not HAVE_GETCWD || GETCWD_FORKS */
#include <kpathsea/c-dir.h>
#include <kpathsea/xopendir.h>
#include <kpathsea/xstat.h>

static void
xchdir (string dirname)
{
    if (chdir (dirname) != 0) {
        FATAL_PERROR (dirname);
    }
}
#endif /* not HAVE_GETCWD || GETCWD_FORKS */


/* Return the pathname of the current directory in new memory, or give a
   fatal error.  */

string
xgetcwd (void)
{
    /* If the system provides getcwd, and it doesn't fork, use it.
       This should be the case on just about all systems nowadays.
       But just in case, if getcwd does fork, provide a fallback
       implemented from scratch. Don't ever use getwd, since some
       versions of GNU ld (as of 2025 or so) give an unconditional
       warning that it's dangerous.  */
#if defined (HAVE_GETCWD) && !defined (GETCWD_FORKS)
    char path[PATH_MAX + 1];
#if defined(WIN32)
    string pp;
#endif
    if (getcwd (path, PATH_MAX + 1) == NULL) {
        FATAL_PERROR ("getcwd");
    }

#if defined(WIN32)
    for (pp = path; *pp; pp++) {
        if (*pp == '\\')
            *pp = '/';
#if defined (KPSE_COMPAT_API)
        else if (IS_KANJI(pp))
            pp++;
#endif
    }
#endif

    return xstrdup (path);

#else /* not HAVE_GETCWD || GETCWD_FORKS */
    /* Our own implementation, written by Olaf.  */
    struct stat root_stat, cwd_stat;
    string cwd_path = (string)xmalloc(2); /* In case we assign "/" below.  */

    *cwd_path = 0;

    /* Find the inodes of the root and current directories.  */
    root_stat = xstat("/");
    cwd_stat  = xstat(".");

    /* Go up the directory hierarchy until we get to root, prepending each
       directory we pass through to `cwd_path'.  */
    while (!SAME_FILE_P(root_stat, cwd_stat)) {
        struct dirent *e;
        DIR *parent_dir;
        boolean found = false;

        xchdir("..");
        parent_dir = xopendir(".");

        /* Look through the parent directory for the entry with the same
           inode, so we can get its name.  */
        while ((e = readdir (parent_dir)) != NULL && !found) {
            struct stat test_stat;
            test_stat = xlstat(e->d_name);

            if (SAME_FILE_P(test_stat, cwd_stat)) {
                /* We've found it.  Prepend the pathname.  */
                string temp = cwd_path;
                cwd_path = concat3("/", e->d_name, cwd_path);
                free(temp);

                /* Set up to test the next parent.  */
                cwd_stat = xstat(".");

                /* Stop reading this directory.  */
                found = true;
            }
        }
        if (!found)
            FATAL2("No inode %d/device %d in parent directory",
                   cwd_stat.st_ino, cwd_stat.st_dev);

        xclosedir(parent_dir);
    }

    /* If the current directory is the root, cwd_path will be the empty
       string, and we will have not gone through the loop.  */
    if (*cwd_path == 0)
        strcpy(cwd_path, "/");
    else
        /* Go back to where we were.  */
        xchdir(cwd_path);

#ifdef DOSISH
    /* Prepend the drive letter to CWD_PATH, since this technique
       never tells us what the drive is.

       Note that on MS-DOS/MS-Windows, the branch that works around
       missing `getwd' will probably only work for DJGPP (which does
       have `getwd'), because only DJGPP reports meaningful
       st_ino numbers.  But someday, somebody might need this...  */
    {
        char drive[3];
        string temp = cwd_path;

        /* Make the drive letter lower-case, unless it is beyond Z: (yes,
           there ARE such drives, in case of Novell Netware on MS-DOS).  */
        drive[0] = root_stat.st_dev + (root_stat.st_dev < 26 ? 'a' : 'A');
        drive[1] = ':';
        drive[2] = '\0';

        cwd_path = concat(drive, cwd_path);
        free(temp);
    }
#endif

    return cwd_path;
#endif /* not HAVE_GETCWD || GETCWD_FORKS */
}
