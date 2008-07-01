/*
 * Copyright 1989 Software Research Associates, Inc., Tokyo, Japan
 * Copyright 2004 the Xdvik development team
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Software Research Associates
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * SOFTWARE RESEARCH ASSOCIATES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL SOFTWARE RESEARCH ASSOCIATES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Erik M. van der Poel
 *         Software Research Associates, Inc., Tokyo, Japan
 *         erik@sra.co.jp
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#undef wchar_t

#define SEL_FILE_CANCEL		-1
#define SEL_FILE_OK		0
#define SEL_FILE_NULL		1
#define SEL_FILE_TEXT		2

#define SF_DO_SCROLL		1
#define SF_DO_NOT_SCROLL	0

/* Widget names for get_widget_by_name() */
#define SfSelFile_FIELD_NAME "selFileField"
#define SfSelFile_FORM_NAME "selFileForm"
#define SfSelFile_HSCROLL_BIG_NAME "selFileHScrollBig"
#define SfSelFile_VSCROLL_BASE_NAME "selFileVScroll"
#define SfSelFile_HSCROLL_BASE_NAME "selFileHScroll"
#define SfSelFile_LIST_BASE_NAME "selFileList"
#define SfSelFile_LIST1_NAME "selFileList1"
#define SfSelFile_LIST2_NAME "selFileList2"
#define SfSelFile_LIST3_NAME "selFileList3"

extern void
	SFenterList(),
	SFleaveList(),
	SFmotionList(),
	SFbuttonPressList(),
	SFbuttonReleaseList();

extern void
	SFvSliderMovedCallback(),
	SFvFloatSliderMovedCallback(),
	SFhSliderMovedCallback(),
	SFpathSliderMovedCallback(),
	SFvAreaSelectedCallback(),
	SFhAreaSelectedCallback(),
	SFpathAreaSelectedCallback();


