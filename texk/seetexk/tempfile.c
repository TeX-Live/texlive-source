/* Copyright (c) 1987, 1989, 2012, 2025 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _AMIGA
#define getpid() 42
#endif /* _AMIGA */

#if defined( WIN32 ) || defined( _AMIGA )
#include "types.h"
#else
#include <sys/file.h>
#endif

#include "tempfile.h"

#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/c-fopen.h>
#else
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

#if defined(WIN32) || defined(MSDOS)
#define TMP_OPEN_MODE  (O_RDWR | O_BINARY)
#else
#define TMP_OPEN_MODE  2
#endif

#ifdef DOSISH
#ifndef WIN32
#define __cdecl
#endif
static char *TmpFile;
static void __cdecl
RemoveTempFile(void)
{
  if (TmpFile) {
    unlink(TmpFile);
    TmpFile = (char *)0;
  }
}
#endif

/*
 * Create an open but unlinked (i.e., remove-on-exit-or-file-close)
 * file.  Return the file descriptor, or -1 if unable to create such
 * a file.  The buffer `name' is used to hold the file name (so that
 * if this returns -1, it is useful for error messages).
 *
 * We use the despicable trick of unlinking an open temporary file.
 * The alternatives are too painful.  If it becomes necessary to
 * do this another way, however, here is a method suggested by Fred
 * Blonder: fork, and have the parent wait for the child to exit.
 * (The parent must avoid being killed during this phase.)  When
 * the child exits, the parent should then remove the temporary file,
 * then exit with the same status, or send itself the same signal.
 */
int
MakeRWTempFile(char *name)
{
	register int tf, n;
	int mypid, tries;
	const char *tmpdir;

	if ((tmpdir = getenv("TMP")) == NULL
	    && (tmpdir = getenv("TEMP")) == NULL
	    && (tmpdir = getenv("TMPDIR")) == NULL)
#if defined(WIN32) || defined(MSDOS)
	  tmpdir = ".";
#else
	  tmpdir = "/tmp";
#endif
	/* compose a suitable temporary file name, and get an r/w descriptor */
	mypid = getpid();
	n = 0;
	tries = 0;
	do {
		(void) sprintf(name, "%s/#%d.%d", tmpdir, mypid, n++);
#ifndef DOSISH
		(void) unlink(name);
#endif
#ifdef O_CREAT			/* have three-argument open syscall */
		tries++;
		tf = open(name, O_RDWR | O_CREAT | O_EXCL | O_BINARY, 0666);
#else
		if (access(name, 0) == 0) {
			/*
			 * Skip existing files.  Note that tf might
			 * not be set yet.
			 */
			tf = -1;
			continue;
		}
		tries++;
		tf = creat(name, 0666);
		if (tf >= 0) {
			(void) close(tf);
			tf = open(name, TMP_OPEN_MODE);
			if (tf < 0)
				(void) unlink(name);
		}
#endif
	} while (tf < 0 && tries < 20);
	if (tf < 0)		/* probably unrecoverable user error */
		return (-1);

#ifdef DOSISH
	/* Can't use the despicable trick of unlinking an open file,
	   since the filesystem won't wait for it to be closed.  Let's
	   be somewhat less despicable.  */
	TmpFile = (char *)malloc(strlen(name) + 1);
	if (TmpFile) {
	  strcpy(TmpFile, name);
	  atexit(RemoveTempFile);
	}
#else
	(void) unlink(name);
#endif
	return (tf);
}
