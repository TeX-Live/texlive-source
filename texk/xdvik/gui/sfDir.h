/*
 * Copyright (c) 2002-2004 the xdvik development team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef SFDIR_H_
#define SFDIR_H_

#include "kpathsea/c-auto.h"
#include "kpathsea/config.h"
#include "kpathsea/c-dir.h"
#include "kpathsea/c-stat.h"

#ifndef MOTIF

typedef struct {
	int	statDone;
	char	*real;
	char	*shown;
} SFEntry;

typedef struct {
	char	*dir;
	char	*path;
	SFEntry	*entries;
	int	nEntries;
	int	vOrigin;
	int	nChars;
	int	hOrigin;
	int	changed;
	int	beginSelection;
	int	endSelection;
	time_t	mtime;
} SFDir;

extern int SFcompareEntries(const void *vp, const void *vq);
extern int SFgetDir(SFDir *dir);

extern SFDir *SFdirs;

#endif /* MOTIF */

#endif /* SFDIR_H_ */
