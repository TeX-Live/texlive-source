/*  $Header: /home/cvsroot/dvipdfmx/src/pdfximage.c,v 1.14 2005/07/30 11:44:18 hirata Exp $
    
    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team <dvipdfmx@project.ktug.or.kr>
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "error.h"
#include "mem.h"

#include "dpxfile.h"

#include "pdfobj.h"

#include "pdfdoc.h"
#include "pdfdev.h"
#include "pdfdraw.h"

#include "epdf.h"
#include "mpost.h"
#include "pngimage.h"
#include "jpegimage.h"
#include "bmpimage.h"

#include "pdfximage.h"

/* From psimage.h */
static int  check_for_ps    (FILE *fp);
static int  ps_include_page (pdf_ximage *ximage, const char *file_name);


#define IMAGE_TYPE_UNKNOWN -1
#define IMAGE_TYPE_PDF      0
#define IMAGE_TYPE_JPEG     1
#define IMAGE_TYPE_PNG      2
#define IMAGE_TYPE_MPS      4
#define IMAGE_TYPE_EPS      5
#define IMAGE_TYPE_BMP      6


struct attr_
{
  long     width, height;
  pdf_rect bbox;
};

struct pdf_ximage_
{
  char        *ident;
  char         res_name[16];

  int          subtype;

  struct attr_ attr;

  char        *filename;
  pdf_obj     *reference;
  pdf_obj     *resource;
};


/* verbose, verbose, verbose... */
struct opt_
{
  int    verbose;
  char  *cmdtmpl;
};

static struct opt_ _opts = {
  0, NULL
};

void pdf_ximage_set_verbose (void) { _opts.verbose++; }


struct ic_
{
  int         count, capacity;
  pdf_ximage *ximages;
};

static struct ic_  _ic = {
  0, 0, NULL
};

static void
pdf_init_ximage_struct (pdf_ximage *I)
{
  I->ident    = NULL;
  I->filename = NULL;
  I->subtype  = -1;
  memset(I->res_name, 0, 16);
  I->reference = NULL;
  I->resource  = NULL;

  I->attr.width = I->attr.height = 0;
  I->attr.bbox.llx = I->attr.bbox.lly = 0;
  I->attr.bbox.urx = I->attr.bbox.ury = 0;
}

static void
pdf_clean_ximage_struct (pdf_ximage *I)
{
  if (I->ident)
    RELEASE(I->ident);
  if (I->filename)
    RELEASE(I->filename);
  if (I->reference)
    pdf_release_obj(I->reference);
  if (I->resource)
    pdf_release_obj(I->resource);
  pdf_init_ximage_struct(I);
}


void
pdf_init_images (void)
{
  struct ic_ *ic = &_ic;
  ic->count    = 0;
  ic->capacity = 0;
  ic->ximages  = NULL;
}

void
pdf_close_images (void)
{
  struct ic_ *ic = &_ic;
  if (ic->ximages) {
    int  i;
    for (i = 0; i < ic->count; i++)
      pdf_clean_ximage_struct(&ic->ximages[i]);
    RELEASE(ic->ximages);
  }
  ic->ximages = NULL;
  ic->count = ic->capacity = 0;

  if (_opts.cmdtmpl)
    RELEASE(_opts.cmdtmpl);
  _opts.cmdtmpl = NULL;
}


static int
source_image_type (FILE *fp)
{
  int  format = IMAGE_TYPE_UNKNOWN;

  rewind(fp);
  /*
   * Make sure we check for PS *after* checking for MP since
   * MP is a special case of PS.
   */
  if (check_for_jpeg(fp))
  {
    format = IMAGE_TYPE_JPEG;
  }
#ifdef  HAVE_LIBPNG
  else if (check_for_png(fp))
  {
    format = IMAGE_TYPE_PNG;
  }
#endif
  else if (check_for_bmp(fp))
  {
    format = IMAGE_TYPE_BMP;
  } else if (check_for_pdf(fp)) {
    format = IMAGE_TYPE_PDF;
  } else if (check_for_mp(fp)) {
    format = IMAGE_TYPE_MPS;
  } else if (check_for_ps(fp)) {
    format = IMAGE_TYPE_EPS;
  } else {
    format = IMAGE_TYPE_UNKNOWN;
  }
  rewind(fp);

  return  format;
}

#if  0
#define KEYCMP(k,s) (strcmp(pdf_name_value((k)),(s)))
static int
filter_put_image_attr (pdf_obj *kp, pdf_obj *vp, void *dp)
{
  struct attr_ *attr = dp;

  if (KEYCMP(kp, "Width")) {
    if (pdf_obj_typeof(vp) != PDF_NUMBER)
      return  -1;
    attr->width  = (long) pdf_number_value(vp);
  } else if (KEYCMP(kp, "Height")) {
    if (pdf_obj_typeof(vp) != PDF_NUMBER)
      return  -1;
    attr->height = (long) pdf_number_value(vp);
  }

  return  0;
}

static int
filter_put_form_attr (pdf_obj *kp, pdf_obj *vp, void *dp)
{
  struct attr_ *attr = dp;

  if (KEYCMP(kp, "BBox")) {
    if (pdf_array_length(vp) != 4)
      return  -1;
    else {
      int     i;
      double  v[4];
      for (i = 0; i < 4; i++) {
        pdf_obj *obj = pdf_get_array(vp, i);
        if (pdf_obj_typeof(vp) != PDF_NUMBER)
          return  -1;
        else {
          v = pdf_number_value(obj);
        }
      }
      attr->bbox.llx = v[0]; attr->bbox.lly = v[1];
      attr->bbox.urx = v[2]; attr->bbox.ury = v[3];
    }
  }

  return  0;
}
#endif


static int
load_image (const char *ident,
            const char *fullname, int format, FILE  *fp)
{
  struct ic_ *ic = &_ic;
  int         id = -1; /* ret */
  pdf_ximage *I;

  id = ic->count;
  if (ic->count >= ic->capacity) {
    ic->capacity += 16;
    ic->ximages   = RENEW(ic->ximages, ic->capacity, pdf_ximage);
  }

  I  = &ic->ximages[id];
  pdf_init_ximage_struct(I);

  switch (format) {
  case  IMAGE_TYPE_JPEG:
    if (_opts.verbose)
      MESG("[JPEG]");
    if (jpeg_include_image(I, fp) < 0)
      return  -1;
    I->subtype  = PDF_XOBJECT_TYPE_IMAGE;
    break;
#ifdef HAVE_LIBPNG
  case  IMAGE_TYPE_PNG:
    if (_opts.verbose)
      MESG("[PNG]");
    if (png_include_image(I, fp) < 0)
      return  -1;
    I->subtype  = PDF_XOBJECT_TYPE_IMAGE;
    break;
#endif
  case  IMAGE_TYPE_BMP:
    if (_opts.verbose)
      MESG("[BMP]");
    if (bmp_include_image(I, fp) < 0)
      return  -1;
    I->subtype  = PDF_XOBJECT_TYPE_IMAGE;
    break;
  case  IMAGE_TYPE_PDF:
    if (_opts.verbose)
      MESG("[PDF]");
    if (pdf_include_page(I, fp) < 0)
      return  -1;
    I->subtype  = PDF_XOBJECT_TYPE_FORM;
    break;
  case  IMAGE_TYPE_EPS:
    if (_opts.verbose)
      MESG("[PS]");
    if (ps_include_page(I, fullname) < 0)
      return  -1;
    I->subtype  = PDF_XOBJECT_TYPE_FORM;
    break;
  default:
    if (_opts.verbose)
      MESG("[UNKNOWN]");
    if (ps_include_page(I, fullname) < 0)
      return  -1;
    I->subtype  = PDF_XOBJECT_TYPE_FORM;
  }

  I->filename = NEW(strlen(ident)+1, char);
  I->ident    = NEW(strlen(ident)+1, char);
  strcpy(I->filename, ident);
  strcpy(I->ident,    ident);

  switch (I->subtype) {
  case PDF_XOBJECT_TYPE_IMAGE:
    sprintf(I->res_name, "Im%d", id);
    break;
  case PDF_XOBJECT_TYPE_FORM:
    sprintf(I->res_name, "Fm%d", id);
    break;
  default:
    ERROR("Unknown XObject subtype: %d", I->subtype);
    return -1;
  }

  ic->count++;

  return  id;
}


#define dpx_find_file(n,d,s) (kpse_find_pict((n)))
#define dpx_fopen(n,m) (MFOPEN((n),(m)))
#define dpx_fclose(f)  (MFCLOSE((f)))

int
pdf_ximage_findresource (const char *ident)
{
  struct ic_ *ic = &_ic;
  int         id = -1;
  pdf_ximage *I;
  char       *fullname;
  int         format;
  FILE       *fp;

  for (id = 0; id < ic->count; id++) {
    I = &ic->ximages[id];
    if (I->ident && !strcmp(ident, I->ident)) {
      return  id;
    }
  }

  /* try loading image */
  fullname = dpx_find_file(ident, "_pic_", "");
  if (!fullname) {
    WARN("Error locating image file \"%s\"", ident);
    return  -1;
  }

  fp = dpx_fopen(fullname, FOPEN_RBIN_MODE);
  if (!fp) {
    WARN("Error opening image file \"%s\"", fullname);
    RELEASE(fullname);
    return  -1;
  }
  if (_opts.verbose) {
    MESG("(Image:%s", ident);
    if (_opts.verbose > 1)
      MESG("[%s]", fullname);
  }

  format = source_image_type(fp);
  switch (format) {
  case IMAGE_TYPE_MPS:
    if (_opts.verbose)
      MESG("[MPS]");
    id = mps_include_page(ident, fp);
    break;
  default:
    id = load_image(ident, fullname, format, fp);
    break;
  }
  dpx_fclose(fp);

  RELEASE(fullname);

  if (_opts.verbose)
    MESG(")");

  if (id < 0)
    WARN("pdf: image inclusion failed for \"%s\".", ident);

  return  id;
}

void
pdf_ximage_init_form_info (xform_info *info)
{
  info->flags    = 0;
  info->bbox.llx = 0;
  info->bbox.lly = 0;
  info->bbox.urx = 0;
  info->bbox.ury = 0;
  info->matrix.a = 1.0;
  info->matrix.b = 0.0;
  info->matrix.c = 0.0;
  info->matrix.d = 1.0;
  info->matrix.e = 0.0;
  info->matrix.f = 0.0;
}

void
pdf_ximage_init_image_info (ximage_info *info)
{
  info->flags  = 0;
  info->width  = 0;
  info->height = 0;
  info->bits_per_component = 0;
  info->num_components = 0;
  info->min_dpi = 0;
}

void
pdf_ximage_set_image (pdf_ximage *I, void *image_info, pdf_obj *resource)
{
  pdf_obj     *dict;
  ximage_info *info = image_info;

  if (!PDF_OBJ_STREAMTYPE(resource))
    ERROR("Image XObject must be stream type.");

  I->subtype = PDF_XOBJECT_TYPE_IMAGE;

  I->attr.width  = info->width;
  I->attr.height = info->height;

  I->reference = pdf_ref_obj(resource);

  dict = pdf_stream_dict(resource);
  pdf_add_dict(dict, pdf_new_name("Type"),    pdf_new_name("XObject"));
  pdf_add_dict(dict, pdf_new_name("Subtype"), pdf_new_name("Image"));
  pdf_add_dict(dict, pdf_new_name("Width"),   pdf_new_number(info->width));
  pdf_add_dict(dict, pdf_new_name("Height"),  pdf_new_number(info->height));
  pdf_add_dict(dict, pdf_new_name("BitsPerComponent"),
               pdf_new_number(info->bits_per_component));

  pdf_release_obj(resource); /* Caller don't know we are using reference. */
  I->resource  = NULL;
}

void
pdf_ximage_set_form (pdf_ximage *I,  void *form_info, pdf_obj *resource)
{
  xform_info  *info = form_info;

  I->subtype   = PDF_XOBJECT_TYPE_FORM;
  I->attr.bbox.llx = info->bbox.llx;
  I->attr.bbox.lly = info->bbox.lly;
  I->attr.bbox.urx = info->bbox.urx;
  I->attr.bbox.ury = info->bbox.ury;

  I->reference = pdf_ref_obj(resource);
  pdf_release_obj(resource); /* Caller don't know we are using reference. */
  I->resource  = NULL;
}


#define CHECK_ID(c,n) do {\
  if ((n) < 0 || (n) >= (c)->count) {\
    ERROR("Invalid XObject ID: %d", (n));\
  }\
} while (0)
#define GET_IMAGE(c,n) (&((c)->ximages[(n)]))

pdf_obj *
pdf_ximage_get_reference (int id)
{
  struct ic_  *ic = &_ic;
  pdf_ximage  *I;

  CHECK_ID(ic, id);

  I = GET_IMAGE(ic, id);
  if (!I->reference)
    I->reference = pdf_ref_obj(I->resource);

  return  pdf_link_obj(I->reference);
}

int
pdf_ximage_defineresource (const char *ident,
			   int subtype, void *info, pdf_obj *resource)
{
  struct ic_  *ic = &_ic;
  int          id;
  pdf_ximage  *I;

  id = ic->count;
  if (ic->count >= ic->capacity) {
    ic->capacity += 16;
    ic->ximages   = RENEW(ic->ximages, ic->capacity, pdf_ximage);
  }

  I = &ic->ximages[id];

  pdf_init_ximage_struct(I);
  if (ident) {
    I->ident = NEW(strlen(ident) + 1, char);
    strcpy(I->ident, ident);
  }

  switch (subtype) {
  case PDF_XOBJECT_TYPE_IMAGE:
    pdf_ximage_set_image(I, info, resource);
    I->subtype = PDF_XOBJECT_TYPE_IMAGE;
    sprintf(I->res_name, "Im%d", id);
    break;
  case PDF_XOBJECT_TYPE_FORM:
    pdf_ximage_set_form (I, info, resource);
    I->subtype = PDF_XOBJECT_TYPE_FORM;
    sprintf(I->res_name, "Fm%d", id);
    break;
  default:
    ERROR("Unknown XObject subtype: %d", subtype);
  }
  ic->count++;

  return  id;
}


char *
pdf_ximage_get_resname (int id)
{
  struct ic_  *ic = &_ic;
  pdf_ximage  *I;

  CHECK_ID(ic, id);

  I = GET_IMAGE(ic, id);

  return  I->res_name;
}


/* depth...
 * Dvipdfm treat "depth" as "yoffset" for pdf:image and pdf:uxobj
 * not as vertical dimension of scaled image. (And there are bugs.)
 * This part contains incompatibile behaviour than dvipdfm!
 */
#define EBB_DPI 100
static void
scale_to_fit_I (pdf_tmatrix    *T,
                transform_info *p,
                pdf_ximage     *I)
{
  double  s_x, s_y;
  long    wdx = I->attr.width;
  long    htx = I->attr.height;
  double  ar, dp;

  if (htx == 0) {
    WARN("Image height=0!");
    htx = 1;
  }

  ar = (double) wdx / htx;

  /* only width/height --> uniform (keep aspect ratio)
   * no width-height   --> uniform with 72dpi implied (?)
   * both width-height --> non-uniform
   */
  if ( (p->flags & INFO_HAS_WIDTH ) &&
       (p->flags & INFO_HAS_HEIGHT) ) {
    s_x = p->width; s_y = p->height + p->depth;
    dp  = p->depth;
  } else if ( p->flags & INFO_HAS_WIDTH  ) {
    s_x = p->width; s_y = s_x / ar;
    dp  = 0.0;
  } else if ( p->flags & INFO_HAS_HEIGHT ) {
    s_y = p->height + p->depth; s_x = s_y * ar;
    dp  = p->depth;
  } else {
    s_x = wdx * 72.0 / EBB_DPI;
    s_y = htx * 72.0 / EBB_DPI;
    dp  = 0.0;
  }

  T->a   = s_x;  T->c  = 0.0;
  T->b   = 0.0;  T->d  = s_y;
  T->e   = 0.0;  T->f  = -dp;

  return;
}


static void
scale_to_fit_F (pdf_tmatrix    *T,
                transform_info *p,
                pdf_ximage     *I)
{
  double  s_x, s_y, d_x, d_y;
  double  wd0, ht0, dp;

  if (p->flags & INFO_HAS_USER_BBOX) {
    wd0 =  p->bbox.urx - p->bbox.llx;
    ht0 =  p->bbox.ury - p->bbox.lly;
    d_x = -p->bbox.llx;
    d_y = -p->bbox.lly;
  } else {
    wd0 = I->attr.bbox.urx - I->attr.bbox.llx;
    ht0 = I->attr.bbox.ury - I->attr.bbox.lly;
    d_x = 0.0;
    d_y = 0.0; 
  }

  if (wd0 == 0.0) {
    WARN("Image width=0.0!");
    wd0 = 1.0;
  }
  if (ht0 == 0.0) {
    WARN("Image height=0.0!");
    ht0 = 1.0;
  }

  if ( (p->flags & INFO_HAS_WIDTH ) &&
       (p->flags & INFO_HAS_HEIGHT) ) {
    s_x = p->width  / wd0;
    s_y = (p->height + p->depth) / ht0;
    dp  = p->depth;
  } else if ( p->flags & INFO_HAS_WIDTH ) {
    s_x = p->width  / wd0;
    s_y = s_x;
    dp  = 0.0;
  } else if ( p->flags & INFO_HAS_HEIGHT) {
    s_y = (p->height + p->depth) / ht0;
    s_x = s_y;
    dp  = p->depth;
  } else {
    s_x = s_y = 1.0;
    dp  = 0.0;
  }

  T->a = s_x; T->c = 0.0;
  T->b = 0.0; T->d = s_y;
  T->e = s_x * d_x; T->f = s_y * d_y - dp;

  return;
}


int
pdf_ximage_scale_image (int            id,
                        pdf_tmatrix    *M, /* ret */
                        pdf_rect       *r, /* ret */
                        transform_info *p  /* arg */
                       )
{
  struct ic_  *ic = &_ic;
  pdf_ximage  *I;

  CHECK_ID(ic, id);

  I = GET_IMAGE(ic, id);

  pdf_setmatrix(M, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);

  switch (I->subtype) {
  case PDF_XOBJECT_TYPE_IMAGE:
    scale_to_fit_I(M, p, I);
    r->llx = 0.0;
    r->lly = 0.0;
    r->urx = M->a;
    r->ury = M->d;
    break;
  case PDF_XOBJECT_TYPE_FORM:
    scale_to_fit_F(M, p, I);
    if (p->flags & INFO_HAS_USER_BBOX) {
      r->llx = p->bbox.llx;
      r->lly = p->bbox.lly;
      r->urx = p->bbox.urx;
      r->ury = p->bbox.ury;
    } else {
      r->llx = I->attr.bbox.llx;
      r->lly = I->attr.bbox.lly;
      r->urx = I->attr.bbox.urx;
      r->ury = I->attr.bbox.ury;
    }
    break;
  }

  return  0;
}


/* Migrated from psimage.c */

void set_distiller_template (char *s) 
{
  if (_opts.cmdtmpl)
    RELEASE(_opts.cmdtmpl);
  if (!s || *s == '\0')
    _opts.cmdtmpl = NULL;
  else {
    _opts.cmdtmpl = NEW(strlen(s) + 1, char);
    strcpy(_opts.cmdtmpl, s);
  }
  return;
}

static int
ps_include_page (pdf_ximage *ximage, const char *filename)
{
  char  *distiller_template = _opts.cmdtmpl;
  char  *temp;
  FILE  *fp;
  int    error = 0;

  if (!distiller_template) {
    WARN("No image converter available for converting file \"%s\" to PDF format.", filename);
    WARN(">> Please check if you have 'D' option in config file.");
    return  -1;
  }

  temp = dpx_create_temp_file();
  if (!temp) {
    WARN("Failed to create temporary file for image conversion: %s", filename);
    return  -1;
  }

  if (_opts.verbose > 1) {
    MESG("\n");
    MESG("pdf_image>> Converting file \"%s\" --> \"%s\" via:\n", filename, temp);
    MESG("pdf_image>>   %s\n", distiller_template);
    MESG("pdf_image>> ...");
  }

  error = dpx_file_apply_filter(distiller_template, filename, temp);
  if (error) {
    WARN("Image format conversion for \"%s\" failed...", filename);
    dpx_delete_temp_file(temp);
    return  error;
  }

  fp = MFOPEN(temp, FOPEN_RBIN_MODE);
  if (!fp) {
    WARN("Could not open conversion result \"%s\" for image \"%s\". Why?", temp, filename);
    dpx_delete_temp_file(temp);
    return  -1;
  }
  error = pdf_include_page(ximage, fp);
  MFCLOSE(fp);

  if (_opts.verbose > 1) {
    MESG("pdf_image>> deleting file \"%s\"", temp);
  }
  dpx_delete_temp_file(temp); /* temp freed here */

  if (error) {
    WARN("Failed to include image file \"%s\"", filename);
    WARN(">> Please check if");
    WARN(">>   %s", distiller_template);
    WARN(">>   %%o = output filename, %%i = input filename, %%b = input filename without suffix");
    WARN(">> can really convert \"%s\" to PDF format image.", filename);
  }

  return  error;
}

static int check_for_ps (FILE *image_file) 
{
  rewind (image_file);
  mfgets (work_buffer, WORK_BUFFER_SIZE, image_file);
  if (!strncmp (work_buffer, "%!", 2))
    return 1;
  return 0;
}


