/*  $Header: /home/cvsroot/dvipdfmx/src/epdf.c,v 1.14 2004/09/12 11:57:49 hirata Exp $

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/


/*
 * Concatinating content streams are only supported for streams that only uses
 * single FlateDecode filter, i.e.,
 *
 *   /Filter /FlateDecode or /Filter [/FlateDecode]
 *
 * TrimBox, BleedBox, ArtBox, Rotate ...
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "mem.h"
#include "mfileio.h"
#include "error.h"

#if HAVE_ZLIB
#include <zlib.h>
#endif

#include "pdfobj.h"
#include "pdfdev.h"

#include "pdfximage.h"

#include "epdf.h"

#if HAVE_ZLIB
static int  add_stream_flate (pdf_obj *dst, const void *data, long len);
#endif
static int  concat_stream    (pdf_obj *dst, pdf_obj *src);
static void print_bbox_info  (pdf_obj *rect, const char *type, pdf_obj *crop_box);
static int  rect_equal       (pdf_obj *rect1, pdf_obj *rect2);

/*
 * From PDF Reference 4th ed.
 *
 *  MediaBox rectangle (Required; inheritable)
 *
 *   A rectangle, expressed in default user space units, defining the boundaries of the
 *   physical medium on which the page is intended to be displayed or printed.
 *
 *  CropBox rectangle (Optional; inheritable)
 *
 *    A rectangle, expressed in default user space units, defining the visible region of
 *    default user space. When the page is displayed or printed, its contents are to be
 *    clipped (cropped) to this rectangle and then imposed on the output medium in some
 *    implementation-defined manner.
 *    Default value: the value of MediaBox.
 *
 *  BleedBox rectangle (Optional; PDF 1.3)
 *
 *    A rectangle, expressed in default user space units, defining the region to which the
 *    contents of the page should be clipped when output in a production environment.
 *    Default value: the value of CropBox.
 *
 *  TrimBox rectangle (Optional; PDF 1.3)
 *
 *    A rectangle, expressed in default user space units, defining the intended dimensions
 *    of the finished page after trimming.
 *    Default value: the value of CropBox.
 *
 *  ArtBox rectangle (Optional; PDF 1.3)
 *
 *    A rectangle, expressed in default user space units, defining the extent of the page's
 *    meaningful content (including potential white space) as intended by the page's creator
 *    Default value: the value of CropBox.
 *
 *  Rotate integer (Optional; inheritable)
 *
 *    The number of degrees by which the page should be rotated clockwise when displayed or
 *    printed. The value must be a multiple of 90.
 *    Default value: 0.
 */

static int
rect_equal (pdf_obj *rect1, pdf_obj *rect2)
{
  int i;

  if (!rect1 || !rect2)
    return 0;
  for (i = 0; i < 4; i++) {
    if (pdf_number_value(pdf_get_array(rect1, i)) !=
	pdf_number_value(pdf_get_array(rect2, i)))
      return 0;
  }

  return 1;
}

static void
print_bbox_info (pdf_obj *rect, const char *type, pdf_obj *crop_box)
{
  WARN("\"%s\" different from current CropBox found.", type);
  WARN("%s (PDF): [ %g %g %g %g ]", type,
       pdf_number_value(pdf_get_array(rect, 0)),
       pdf_number_value(pdf_get_array(rect, 1)),
       pdf_number_value(pdf_get_array(rect, 2)),
       pdf_number_value(pdf_get_array(rect, 3)));
  WARN("CropBox/MediaBox (PDF)   : [ %g %g %g %g ]",
       pdf_number_value(pdf_get_array(crop_box, 0)),
       pdf_number_value(pdf_get_array(crop_box, 1)),
       pdf_number_value(pdf_get_array(crop_box, 2)),
       pdf_number_value(pdf_get_array(crop_box, 3)));
}

int
pdf_include_page (pdf_ximage *ximage, FILE *image_file, int page_index, int pdf_box)
{
  xform_info info;
  pdf_obj *contents,  *contents_ref, *contents_dict;
  pdf_obj *page_tree;
  pdf_obj *bbox, *resources, *rotate, *matrix;
  int page_count;

  pdf_ximage_init_form_info(&info);
  /*
   * Get Page Tree.
   */
  page_tree = NULL;
  {
    pdf_obj *trailer, *catalog;
    pdf_obj *markinfo, *tmp;

    trailer = pdf_open(image_file);
    if (!trailer) {
      WARN("Trailer not found! Corrupt PDF file?");
      pdf_close();
      return -1;
    }

    if (pdf_lookup_dict(trailer, "Encrypt")) {
      WARN("This PDF document is encrypted.");
      pdf_release_obj(trailer);
      pdf_close();
      return -1;
    }

    catalog = pdf_deref_obj(pdf_lookup_dict(trailer, "Root"));
    if (!catalog) {
      WARN("Catalog isn't where I expect it.");
      pdf_close();
      return -1;
    }
    pdf_release_obj(trailer);

    markinfo = pdf_deref_obj(pdf_lookup_dict(catalog, "MarkInfo"));
    if (markinfo) {
      tmp = pdf_lookup_dict(markinfo, "Marked");
      pdf_release_obj(markinfo);
      if (tmp && pdf_boolean_value(tmp)) {
	WARN("Tagged PDF not supported.");
	pdf_release_obj(catalog);
	pdf_close();
	return -1;
      }
    }

    page_tree = pdf_deref_obj(pdf_lookup_dict(catalog, "Pages"));
    pdf_release_obj(catalog);
  }
  if (!page_tree) {
    WARN("Page tree not found.");
    pdf_close();
    return -1;
  }

  page_count = (int)pdf_number_value(pdf_lookup_dict(page_tree, "Count"));
  if (page_index >= page_count)
    page_index = page_count - 1;
  else if (page_index < 0) {
    page_index = page_count + page_index;
    if (page_index < 0)
      page_index = 0;
  }
  
  /*
   * Seek required page. Get Media/Crop Box.
   * Media box and resources can be inherited.
   */
  {
    pdf_obj *kids_ref, *kids;
    pdf_obj *crop_box;
    pdf_obj *tmp;

    tmp  = pdf_lookup_dict(page_tree, "MediaBox");
    bbox = tmp ? pdf_deref_obj(tmp) : NULL;
    tmp  = pdf_lookup_dict(page_tree, "CropBox");
    crop_box = tmp ? pdf_deref_obj(tmp) : NULL;
    tmp  = pdf_lookup_dict(page_tree, "Rotate");
    rotate   = tmp ? pdf_deref_obj(tmp) : NULL;

    tmp = pdf_lookup_dict(page_tree, "Resources");
    resources = tmp ? pdf_deref_obj(tmp) : pdf_new_dict();

    while ((kids_ref = pdf_lookup_dict(page_tree, "Kids")) != NULL) {
      /* page_tree is a Pages object: figure out which of its Kids we need */
      int kid_index;
      kids = pdf_deref_obj(kids_ref);
      for (kid_index = 0; ; ++kid_index) {
        pdf_release_obj(page_tree);
        page_tree = pdf_deref_obj(pdf_get_array(kids, kid_index));
        /* page_tree might be a Page or another Pages node */
        tmp = pdf_lookup_dict(page_tree, "Count");
        if (tmp) {
          /* it's a subtree: check if the required page is within this tree */
          page_count = (int)pdf_number_value(tmp);
          if (page_index < page_count)
            break;
          else
            page_index -= page_count;
        }
        else {
          /* it's a page: break if it's the one we want */
          if (page_index == 0)
            break;
          else
            --page_index;
        }
      }
      pdf_release_obj(kids);

      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "MediaBox")))) {
	if (bbox)
	  pdf_release_obj(bbox);
	bbox = tmp;
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "CropBox")))) {
	if (crop_box)
	  pdf_release_obj(crop_box);
	crop_box = tmp;
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "Rotate")))) {
	if (rotate)
	  pdf_release_obj(rotate);
	rotate = tmp;
      }
      if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "Resources")))) {
#if 0
	pdf_merge_dict(tmp, resources);
#endif
	if (resources)
	  pdf_release_obj(resources);
	resources = tmp;
      }
    }
    if (crop_box) {
      pdf_release_obj(bbox);
      bbox = crop_box;
    }
  }
  if (!bbox) {
    WARN("No BoundingBox information available.");
    pdf_release_obj(page_tree);
    pdf_release_obj(resources);
    if (rotate)
      pdf_release_obj(rotate);
    pdf_close();
    return -1;
  }

  /*
   * BleedBox, TrimBox, and ArtBox are not inheritable.
   */
  {
    pdf_obj *tmp;

    if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "TrimBox")))) {
      if (!rect_equal(tmp, bbox)) {
	print_bbox_info(tmp, "TrimBox", bbox);
	if (bbox)
	  pdf_release_obj(bbox);
	bbox = tmp;
      }
    }
    if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "BleedBox")))) {
      if (!rect_equal(tmp, bbox)) {
	print_bbox_info(tmp, "BleedBox", bbox);
	if (bbox)
	  pdf_release_obj(bbox);
	bbox = tmp;
      }
    }
    if ((tmp = pdf_deref_obj(pdf_lookup_dict(page_tree, "ArtBox")))) {
      if (!rect_equal(tmp, bbox)) {
	print_bbox_info(tmp, "ArtBox", bbox);
	if (bbox)
	  pdf_release_obj(bbox);
	bbox = tmp;
      }
    }
    if (rotate) {
      if (pdf_number_value(rotate) != 0.0)
	WARN("<< /Rotate %d >> found. (Not supported yet)",  (int)pdf_number_value(rotate));
      pdf_release_obj(rotate);
      rotate = NULL;
    }

    info.bbox.llx = pdf_number_value(pdf_get_array(bbox, 0));
    info.bbox.lly = pdf_number_value(pdf_get_array(bbox, 1));
    info.bbox.urx = pdf_number_value(pdf_get_array(bbox, 2));
    info.bbox.ury = pdf_number_value(pdf_get_array(bbox, 3));
  }

  /*
   * Handle page content stream.
   * page_tree is now set to the first page.
   */
  contents_ref = contents = NULL;
  {
    contents = pdf_deref_obj(pdf_lookup_dict(page_tree, "Contents"));
    pdf_release_obj(page_tree);
    if (!contents) {
      pdf_release_obj(bbox);
      pdf_close();
      return -1;
    }

    /*
     * Concatinate all content streams.
     */
    if (PDF_OBJ_ARRAYTYPE(contents)) {
      pdf_obj *content_seg, *content_new;
      int      idx = 0;
      content_new = pdf_new_stream(STREAM_COMPRESS);
      for (;;) {
	content_seg = pdf_deref_obj(pdf_get_array(contents, idx));
	if (!content_seg)
	  break;
	else if (PDF_OBJ_NULLTYPE(content_seg)) {
	  /* Silently ignore. */
	}  else if (!PDF_OBJ_STREAMTYPE(content_seg)) {
	  WARN("Page content not a stream object. Broken PDF file?");
	  pdf_release_obj(content_new);
	  pdf_release_obj(bbox);
	  pdf_close();
	  return -1;
	} else if (concat_stream(content_new, content_seg) < 0) {
	  WARN("Could not handle content stream with multiple segment.");
	  pdf_release_obj(content_new);
	  pdf_release_obj(bbox);
	  pdf_close();
	  return -1;
	}
	pdf_release_obj(content_seg);
	idx++;
      }
      pdf_release_obj(contents);
      contents = content_new;
    }
    if (!contents) {
      pdf_release_obj(bbox);
      pdf_close();
      return -1;
    }
  }

  {
    pdf_obj *tmp;

    tmp = pdf_import_object(resources);
    pdf_release_obj(resources);
    resources = tmp;
  }

  pdf_close();

  contents_dict = pdf_stream_dict(contents);
  pdf_add_dict(contents_dict,
	       pdf_new_name("Type"), 
	       pdf_new_name("XObject"));
  pdf_add_dict(contents_dict,
	       pdf_new_name("Subtype"),
	       pdf_new_name("Form"));
  pdf_add_dict(contents_dict,
	       pdf_new_name("FormType"),
	       pdf_new_number(1.0));

  pdf_add_dict(contents_dict, pdf_new_name("BBox"), bbox);

  matrix = pdf_new_array();
  pdf_add_array(matrix, pdf_new_number(1.0));
  pdf_add_array(matrix, pdf_new_number(0.0));
  pdf_add_array(matrix, pdf_new_number(0.0));
  pdf_add_array(matrix, pdf_new_number(1.0));
  pdf_add_array(matrix, pdf_new_number(0.0));
  pdf_add_array(matrix, pdf_new_number(0.0));

  pdf_add_dict(contents_dict, pdf_new_name("Matrix"), matrix);

  pdf_add_dict(contents_dict, pdf_new_name("Resources"), resources);

  pdf_ximage_set_form(ximage, &info, contents);

  return 0;
}

#define WBUF_SIZE 4096
#if HAVE_ZLIB
static int
add_stream_flate (pdf_obj *dst, const void *data, long len)
{
  z_stream z;
  Bytef wbuf[WBUF_SIZE];

  z.zalloc = Z_NULL; z.zfree = Z_NULL; z.opaque = Z_NULL;

  z.next_in  = (Bytef *) data; z.avail_in  = len;
  z.next_out = (Bytef *) wbuf; z.avail_out = WBUF_SIZE;

  if (inflateInit(&z) != Z_OK) {
    WARN("inflateInit() failed.");
    return -1;
  }

  for (;;) {
    int status;
    status = inflate(&z, Z_NO_FLUSH);
    if (status == Z_STREAM_END)
      break;
    else if (status != Z_OK) {
      WARN("inflate() failed. Broken PDF file?");
      inflateEnd(&z);
      return -1;
    }

    if (z.avail_out == 0) {
      pdf_add_stream(dst, wbuf, WBUF_SIZE);
      z.next_out  = wbuf;
      z.avail_out = WBUF_SIZE;
    }
  }

  if (WBUF_SIZE - z.avail_out > 0)
    pdf_add_stream(dst, wbuf, WBUF_SIZE - z.avail_out);

  return (inflateEnd(&z) == Z_OK ? 0 : -1);
}
#endif

static int
concat_stream (pdf_obj *dst, pdf_obj *src)
{
  const char *stream_data;
  long        stream_length;
  pdf_obj    *stream_dict;
  pdf_obj    *filter;

  if (!PDF_OBJ_STREAMTYPE(dst) || !PDF_OBJ_STREAMTYPE(src))
    ERROR("Invalid type.");

  stream_data   = pdf_stream_dataptr(src);
  stream_length = pdf_stream_length (src);
  stream_dict   = pdf_stream_dict   (src);

  if (pdf_lookup_dict(stream_dict, "DecodeParms")) {
    WARN("DecodeParams not supported.");
    return -1;
  }

  filter = pdf_lookup_dict(stream_dict, "Filter");
  if (!filter) {
    pdf_add_stream(dst, stream_data, stream_length);
    return 0;
#if HAVE_ZLIB
  } else {
    char *filter_name;
    if (PDF_OBJ_NAMETYPE(filter)) {
      filter_name = pdf_name_value(filter);
      if (filter_name && !strcmp(filter_name, "FlateDecode"))
	return add_stream_flate(dst, stream_data, stream_length);
      else {
	WARN("DecodeFilter \"%s\" not supported.", filter_name);
	return -1;
      }
    } else if (PDF_OBJ_ARRAYTYPE(filter)) {
      if (pdf_array_length(filter) > 1) {
	WARN("Multiple DecodeFilter not supported.");
	return -1;
      } else {
	filter_name = pdf_name_value(pdf_get_array(filter, 0));
	if (filter_name && !strcmp(filter_name, "FlateDecode"))
	  return add_stream_flate(dst, stream_data, stream_length);
	else {
	  WARN("DecodeFilter \"%s\" not supported.", filter_name);
	  return -1;
	}
      }
    } else
      ERROR("Broken PDF file?");
#endif /* HAVE_ZLIB */
  }

  return -1;
}
