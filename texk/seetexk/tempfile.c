/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/tempfile.c,v 3.1 89/08/22 21:45:20 chris Exp $";
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>

#ifdef _AMIGA
#define getpid() 42
#endif /* _AMIGA */

#if defined( WIN32 ) || defined( _AMIGA )
#include "types.h"
#else
#include <sys/file.h>

char	*getenv();

#endif

#ifdef KPATHSEA
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

#ifndef WIN32
#define __cdecl
#endif

#ifdef DOSISH
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
MakeRWTempFile(name)
	register char *name;
{
	register int tf, n;
	int mypid, tries;
	char *tmpdir;

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
