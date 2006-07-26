/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 2006 by SIL International
 written by Jonathan Kew

 This software is distributed under the terms of the Common Public License,
 version 1.0.
 For details, see <http://www.opensource.org/licenses/cpl1.0.php> or the file
 cpl1.0.txt included with the software.
\****************************************************************************/

#ifndef __PDF_IMAGE_H__
#define __PDF_IMAGE_H__

#include "trans.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int pdf_get_rect(char* filename, int page_num, int pdf_box, realrect* box);

#ifdef __cplusplus
};
#endif

#endif
