/* This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002-2014 by Jin-Hwan Cho and Matthias Franz,
    the dvipdfmx project team.

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * JP2 & JPX baseline SUPPORT:
 */

#include <math.h>
#include "system.h"
#include "error.h"
#include "mem.h"

#include "dvipdfmx.h"

#include "pdfobj.h"

#include "jp2image.h"

/* Box types. */
#define JP2_BOX_JP__  0x6a502020  /* Signature */
#define JP2_BOX_FTYP  0x66747970  /* File Type */

#define JP2_BOX_JP2H  0x6a703268  /* JP2 Header */
#define JP2_BOX_IHDR  0x69686472  /* Image Header */
#define JP2_BOX_BPCC  0x62706363  /* Bits Per Component */
#define JP2_BOX_COLR  0x636f6c72  /* Color Specification */
#define JP2_BOX_PCLR  0x70636c72  /* Palette */
#define JP2_BOX_CMAP  0x636d6170  /* Component Mapping */
#define JP2_BOX_CDEF  0x63646566  /* Channel Definition */
#define JP2_BOX_RES_  0x72657320  /* Resolution */
#define JP2_BOX_RESC  0x72657363  /* Capture Resolution */
#define JP2_BOX_RESD  0x72657364  /* Default Display Resolution */

#define JP2_BOX_JP2C  0x6a703263  /* Contiguous Code Stream */

#define JPX_BOX_LBL_  0x6c626c20  /* Label */


static unsigned long
read_box_hdr (FILE *fp, unsigned long *lbox, unsigned long *tbox)
{
  unsigned long bytesread = 0;

  *lbox = get_unsigned_quad(fp);
  *tbox = get_unsigned_quad(fp);
  bytesread += 8;
  if (*lbox == 1) {
    *lbox = get_unsigned_quad(fp);
    bytesread += 4;
  } else if (*lbox > 1 && *lbox < 8) {
    WARN("Unknown LBox value %lu in JP2 file!", lbox);
  }

  return bytesread;
}

#define FTYP_BR_JP2_  0x6a703220
#define FTYP_BR_JPX_  0x6a707820
#define FTYP_CL_JPXB  0x6a707862

static int
check_jp___box (FILE *fp)
{
  if (get_unsigned_quad(fp) != 0x0c)
    return 0;
  if (get_unsigned_quad(fp) != JP2_BOX_JP__)
    return 0;
   /* Next 4 bytes shall be 0D 0A 87 0A */
  if (get_unsigned_quad(fp) != 0x0D0A870A)
    return 0;
  return 1;
}

static int
check_ftyp_data (FILE *fp, unsigned long size)
{
  int supported = 0;
  unsigned long BR, CLi;

  BR = get_unsigned_quad(fp);
  size -= 4;
  /* MinV = */ get_unsigned_quad(fp);
  size -= 4;
  switch (BR) {
  case FTYP_BR_JP2_: /* "jp2 " ... supported */
    seek_relative(fp, size);
    size = 0;
    supported = 1;
    break;
  case FTYP_BR_JPX_: /* "jpx " ... baseline subset supported */
    while (size > 0) {
      CLi = get_unsigned_quad(fp);
      if (CLi == FTYP_CL_JPXB)
        supported = 1;
      size -= 4;
    }
    break;
  default:
    WARN("Unknown JPEG 2000 File Type box Brand field value.");
    seek_relative(fp, size);
    size = 0;
    supported = 0;
  }

  return supported;
}


static unsigned long
read_res__data (ximage_info *info, FILE *fp, unsigned long size)
{
  unsigned int  VR_N, VR_D, HR_N, HR_D;
  unsigned char VR_E, HR_E;

  VR_N = get_unsigned_pair(fp);
  VR_D = get_unsigned_pair(fp);
  HR_N = get_unsigned_pair(fp);
  HR_D = get_unsigned_pair(fp);
  VR_E = get_unsigned_byte(fp);
  HR_E = get_unsigned_byte(fp);
  if (compat_mode)
    info->xdensity = info->ydensity = 72.0 / 100.0;
  else {
    info->xdensity = 72.0/(((double) HR_N / HR_D) * pow(10.0, HR_E) * 0.0254);
    info->ydensity = 72.0/(((double) VR_N / VR_D) * pow(10.0, VR_E) * 0.0254);
  }

  return 10;
}

static int
scan_res_ (ximage_info *info, FILE *fp, unsigned long size)
{
  unsigned long len, lbox, tbox;
  int have_resd = 0;

    while (size > 0) {
    len = read_box_hdr(fp, &lbox, &tbox);
    if (lbox == 0) {
      WARN("Unexpected lbox value 0 in JP2 Resolution box.");
      break;
    }
        switch (tbox) {
        case JP2_BOX_RESC:
            if (!have_resd) {
                read_res__data(info, fp, lbox - len);
            } else {
        seek_relative(fp, lbox - len);
      }
            break;
        case JP2_BOX_RESD:
            read_res__data(info, fp, lbox - len);
      have_resd = 1;
            break;
        default:
      WARN("Unknown JPEG 2000 box type in Resolution box.");
      seek_relative(fp, lbox - len);
        }
    size -= lbox;
    }

  return size == 0 ? 0 : -1;
}

static int
scan_jp2h (ximage_info *info, FILE *fp, unsigned long size)
{
  int error = 0, have_ihdr = 0;
  unsigned long len, lbox, tbox;

  while (size > 0 && !error) {
    len = read_box_hdr(fp, &lbox, &tbox);
    if (lbox == 0) {
      WARN("Unexpected lbox value 0 in JP2 Header box...");
      error = -1;
      break;
    }
    switch (tbox) {
    case JP2_BOX_IHDR:
      info->height = get_unsigned_quad(fp);
      info->width  = get_unsigned_quad(fp);
      info->num_components = get_unsigned_pair(fp);
      /* c = */ get_unsigned_byte(fp); /* BPC - 1 */
      /* c = */ get_unsigned_byte(fp); /* C: Compression type */
      /* c = */ get_unsigned_byte(fp); /* UnkC */
      /* c = */ get_unsigned_byte(fp); /* IPR */
      have_ihdr = 1;
      break;
    case JP2_BOX_RES_:
      error = scan_res_(info, fp, lbox - len);
      break;
    case JP2_BOX_BPCC: case JP2_BOX_COLR: case JP2_BOX_PCLR:
    case JP2_BOX_CMAP: case JP2_BOX_CDEF:
    case JPX_BOX_LBL_:
      seek_relative(fp, lbox - len);
      break;
    default:
      WARN("Unknown JPEG 2000 box in JP2 Header box.");
      seek_relative(fp, lbox - len);
      error = -1;
    }
    size -= lbox;
  }

  if (!have_ihdr)
    WARN("Expecting JPEG 2000 Image Header box but could not find.");
  return (!error && have_ihdr && size == 0) ? 0 : -1;
}

static int
scan_file (ximage_info *info, FILE *fp)
{
  int  error = 0, have_jp2h = 0;
  long size;
  unsigned long len, lbox, tbox;


  size = file_size(fp);
  rewind(fp);

  /* Should have already been checked before. */
  /* JPEG 2000 Singature box */
  if (!check_jp___box(fp))
    return -1;
  size -= 12;
  /* File Type box shall immediately follow */
  len = read_box_hdr(fp, &lbox, &tbox);
  if (tbox != JP2_BOX_FTYP)
    return -1;
  if (!check_ftyp_data(fp, lbox - len))
    return -1;
  size -= lbox;

  /* Search for JP2 Header box */
  while (size > 0 && !error) {
    len = read_box_hdr(fp, &lbox, &tbox);
    if (lbox == 0)
      lbox = size;
    switch (tbox) {
    case JP2_BOX_JP2H:
      error = scan_jp2h(info, fp, lbox - len);
      have_jp2h = 1;
      break;
    case JP2_BOX_JP2C:
      /* JP2 requires JP2H appears before JP2C. */
      if (!have_jp2h)
        WARN("JPEG 2000 Codestream box found before JP2 Header box.");
      seek_relative(fp, lbox - len);
      break;
    default:
      seek_relative(fp, lbox - len);
    }
    size -= lbox;
  }

  /* From ISO/IEC 15444-2 M.9.2.7
     The JP2 Header box shall be found in the file before the first
     Contiguous Codestream box, Fragment Table box, Media Data box,
     Codestream Header box, and Compositing Layer Header box. ...
  */
  if (!have_jp2h && !error) {
    WARN("No JP2 Header box found. Not a JP2/JPX baseline file?");
    error = -1;
  }
  return error;
}

int
check_for_jp2 (FILE *fp)
{
  unsigned long len, lbox, tbox;

  if (!fp)
    return 0;

  rewind(fp);

  /* JPEG 2000 Singature box */
  if (!check_jp___box(fp))
    return 0;

  /* File Type box shall immediately follow */
  len = read_box_hdr(fp, &lbox, &tbox);
  if (tbox != JP2_BOX_FTYP)
    return 0;
  if (!check_ftyp_data(fp, lbox - len))
    return 0;

  return 1;
}

int
jp2_include_image (pdf_ximage *ximage, FILE *fp)
{
  unsigned pdf_version;
  pdf_obj *stream, *stream_dict;
  ximage_info info;

  pdf_version = pdf_get_version();
  if (pdf_version < 5) {
    WARN("JPEG 2000 support requires PDF version >= 1.5 (Current setting 1.%d)\n", pdf_version);
    return -1;
  }

  pdf_ximage_init_image_info(&info);
  stream = stream_dict = NULL;

  rewind(fp);
  if (scan_file(&info, fp) < 0) {
    WARN("Reading JPEG 2000 file failed.");
    return -1;
  }

  stream      = pdf_new_stream(0);
  stream_dict = pdf_stream_dict(stream);
  pdf_add_dict(stream_dict,
        pdf_new_name("Filter"), pdf_new_name("JPXDecode"));
  /* Read whole file */
  {
    long nb_read;
    rewind(fp);
    while ((nb_read =
        fread(work_buffer, sizeof(char), WORK_BUFFER_SIZE, fp)) > 0)
      pdf_add_stream(stream, work_buffer, nb_read);
  }

  pdf_ximage_set_image(ximage, &info, stream);

  return 0;
}

int
jp2_get_bbox (FILE *fp, long *width, long *height,
         double *xdensity, double *ydensity)
{
  int r;
  ximage_info info;

  pdf_ximage_init_image_info(&info);

  rewind(fp);
  r = scan_file(&info, fp);

  *width  = info.width;
  *height = info.height;
  *xdensity = info.xdensity;
  *ydensity = info.ydensity;

  return r;
}