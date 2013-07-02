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

#ifndef SFPATH_H_
#define SFPATH_H_

#ifndef MOTIF

extern int SFchdir(const char *path);
extern void SFupdatePath(void);
extern void SFsetText(char *path);
extern void SFbuttonPressList(Widget w, int n, XButtonPressedEvent *event);
extern void SFbuttonReleaseList(Widget w, int n, XButtonReleasedEvent *event);
extern void SFdirModTimer(XtPointer cl, XtIntervalId *id);
extern char SFstatChar(struct stat *statBuf);

extern char SFcurrentPath[], SFstartDir[], SFcurrentDir[];
extern SFDir *SFdirs;
extern int SFdirEnd, SFdirPtr;

#endif /* MOTIF */

#endif /* SFPATH_H_ */
