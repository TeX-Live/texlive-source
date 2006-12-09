/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 2006 by SIL International
 written by Jonathan Kew

 This software is distributed under the terms of the Common Public License,
 version 1.0.
 For details, see <http://www.opensource.org/licenses/cpl1.0.php> or the file
 cpl1.0.txt included with the software.
\****************************************************************************/

#include "pdfimage.h"

#include "PDFDoc.h"
#include "Catalog.h"
#include "Page.h"
#include "GString.h"

#include "XeTeX_ext.h"

/* use our own fmin function because it seems to be missing on certain platforms */
inline double
my_fmin(double x, double y)
{
	return (x < y) ? x : y;
}

int
pdf_get_rect(char* filename, int page_num, int pdf_box, realrect* box)
{
	GString*	name = new GString(filename);
	PDFDoc*		doc = new PDFDoc(name);
	
	if (!doc) {
		delete name;
		return -1;
	}
	
	/* if the doc got created, it now owns name, so we mustn't delete it! */
	
	if (!doc->isOk()) {
		delete doc;
		return -1;
	}
	
	int			pages = doc->getNumPages();
	if (page_num > pages)
		page_num = pages;
	if (page_num < 0)
		page_num = pages + 1 + page_num;
	if (page_num < 1)
		page_num = 1;

	Page*		page = doc->getCatalog()->getPage(page_num);

	PDFRectangle*	r;
	switch (pdf_box) {
		default:
		case pdfbox_crop:
			r = page->getCropBox();
			break;
		case pdfbox_media:
			r = page->getMediaBox();
			break;
		case pdfbox_bleed:
			r = page->getBleedBox();
			break;
		case pdfbox_trim:
			r = page->getTrimBox();
			break;
		case pdfbox_art:
			r = page->getArtBox();
			break;
	}

	box->x  = 72 / 72.27 * my_fmin(r->x1, r->x2);
	box->y  = 72 / 72.27 * my_fmin(r->y1, r->y2);
	box->wd = 72 / 72.27 * fabs(r->x2 - r->x1);
	box->ht = 72 / 72.27 * fabs(r->y2 - r->y1);

	delete doc;

	return 0;
}
