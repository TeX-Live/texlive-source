/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
 written by Jonathan Kew

 This software is distributed under the terms of the Common Public License,
 version 1.0.
 For details, see <http://www.opensource.org/licenses/cpl1.0.php> or the file
 cpl1.0.txt included with the software.
\****************************************************************************/

/*
XeTeX_pic.c
   interface between xetex and graphics files
   (not used on OS X -- we use QuickTime graphics importers instead)
   only needs to get image dimensions, not actually load/process the file
*/

#define EXTERN extern
#include "xetexd.h"

#include "XeTeX_ext.h"

#include <kpathsea/c-ctype.h>
#include <kpathsea/line.h>
#include <kpathsea/readable.h>
#include <kpathsea/variable.h>
#include <kpathsea/absolute.h>

#include "pdfimage.h"
#include "pngimage.h"
#include "jpegimage.h"
#include "bmpimage.h"

/*
	locate picture file from /nameoffile+1/ using kpathsearch
	pdfBoxType indicates which pdf bounding box to use (0 for \XeTeXpicfile)
	page indicates which page is wanted (0-based)
	return 0 for success, or non-zero error code for failure
	return full path in *path
	return bounds (tex points) in *bounds
*/
int
find_pic_file(char** path, realrect* bounds, int pdfBoxType, int page)
{
	int		err = -1;
	FILE*	fp = NULL;
    char*	pic_path = kpse_find_file((char*)nameoffile + 1, kpse_pict_format, 1);

	*path = NULL;
	bounds->x = bounds->y = bounds->wd = bounds->ht = 0.0;

	if (pic_path == NULL)
		goto done;

	/* if cmd was \XeTeXpdffile, use xpdflib to read it */
	if (pdfBoxType != 0) {
		err = pdf_get_rect(pic_path, page, pdfBoxType, bounds);
		goto done;
	}

	/* otherwise try graphics formats that we know */
	fp = fopen(pic_path, FOPEN_RBIN_MODE);
	if (fp == NULL)
		goto done;

	if (check_for_jpeg(fp)) {
		struct JPEG_info	info;
		err = JPEG_scan_file(&info, fp);
		if (err == 0) {
			bounds->wd = (info.width * 72.27) / info.xdpi;
			bounds->ht = (info.height * 72.27) / info.ydpi;
		}
		goto done;
	}
	
	if (check_for_bmp(fp)) {
		struct bmp_info	info;
		err = bmp_scan_file(&info, fp);
		if (err == 0) {
			bounds->wd = (info.width * 72.27) / info.xdpi;
			bounds->ht = (info.height * 72.27) / info.ydpi;
		}
		goto done;
	}
	
	if (check_for_png(fp)) {
		struct png_info	info;
		err = png_scan_file(&info, fp);
		if (err == 0) {
			bounds->wd = (info.width * 72.27) / info.xdpi;
			bounds->ht = (info.height * 72.27) / info.ydpi;
		}
		goto done;
	}	

	/* could support other file types here (TIFF, WMF, etc?) */
	
done:
	if (fp != NULL)
		fclose(fp);

	if (err == 0)
		*path = pic_path;
	else {
		if (pic_path != NULL)
			free(pic_path);
	}
	
	return err;
}
