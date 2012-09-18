/*
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 * OS specific configuration for PSUtils
 */

#if defined(MSDOS) || defined(WINNT) || defined(WIN32)
#define TMPDIR "."
#define OPEN_READ "rb"
#define OPEN_WRITE "wb"
#include <fcntl.h>
#include <io.h>
#else /* ! MSDOS && ! WINNT && ! WIN32 */
#if defined(OS2)
#define TMPDIR "."
#define OPEN_READ "rb"
#define OPEN_WRITE "wb"
#else /* ! OS2 */
#define OPEN_READ "r"
#define OPEN_WRITE "w"
#ifdef VMS
#define unlink delete
#define TMPDIR "/tmp"
#else /* assumes UNIX-like OS */
#include <unistd.h>
#define TMPDIR "/tmp"
#endif /* ! VMS */
#endif /* ! OS2 */
#endif /* ! MSDOS && ! WINNT && !WIN32 */

