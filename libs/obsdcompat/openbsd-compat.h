/* $Id: openbsd-compat.h,v 1.33 2005/12/31 05:33:37 djm Exp $ */

/*
 * Copyright (c) 1999-2003 Damien Miller.  All rights reserved.
 * Copyright (c) 2003 Ben Lindstrom. All rights reserved.
 * Copyright (c) 2002 Tim Rice.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------------
 *
 * Modified (butchered) 2006 by Martin Schröder for pdfTeX
 * Modified 2009-2012 by Peter Breitenlohner for TeX Live
 */

#ifndef _OPENBSD_COMPAT_H
#define _OPENBSD_COMPAT_H

#include "includes.h"

#define OBSDLIB_VERSION "4.3.1.1"
#define OBSDLIB_VERNUM 0x4310

/* OpenBSD function replacements */
#ifndef HAVE_STRLCAT
/* #include <sys/types.h> XXX Still needed? */
/* see http://www.openbsd.org/cgi-bin/man.cgi?query=strlcat */
size_t strlcat(char *dst, const char *src, size_t siz);
#endif 

#ifndef HAVE_STRLCPY
/* #include <sys/types.h> XXX Still needed? */
/* see http://www.openbsd.org/cgi-bin/man.cgi?query=strlcpy */
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRSEP
/* see http://www.openbsd.org/cgi-bin/man.cgi?query=strsep */
char *strsep(char **stringp, const char *delim);
#endif

#endif /* _OPENBSD_COMPAT_H */
