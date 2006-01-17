/*  $Header: /home/cvsroot/dvipdfmx/src/pdfencoding.h,v 1.3 2005/06/08 11:18:37 hirata Exp $
    
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

#ifndef _PDFENCODING_H_
#define _PDFENCODING_H_

#include "pdfobj.h"

extern void      pdf_encoding_set_verbose    (void);

extern void      pdf_init_encodings          (void);
extern void      pdf_close_encodings         (void);

/* enc_name here is .enc file name or the name of predefined
 * encodings.
 */
extern int       pdf_encoding_findresource   (const char *enc_name);

/* Returns indirect reference to Encoding resource.
 */
extern pdf_obj  *pdf_get_encoding_reference  (int enc_id);

extern int       pdf_encoding_is_ASL_charset (int enc_id);
extern int       pdf_encoding_is_predefined  (int enc_id);

/* WARNING:
 * Pointer(s) may change after another encoding is loaded.
 */
extern char     *pdf_encoding_get_name       (int enc_id);
extern char    **pdf_encoding_get_encoding   (int enc_id);

/* 
 * The last argument "is_used" is 256 char array, value 0
 * indicates glyph in that slot is not used in the document
 * (at least for that font). We need this to handle the
 * situation that the encoding contains, e.g., 30 rarely
 * used glyphs without Unicode mapping and 200+ glyphs with
 * Unicode mapping.
 * pdf_create_ToUnicode_CMap() returns stream object but not
 * reference. This need to be renamed to other name like
 * pdf_create_ToUnicode_stream().
 */
extern pdf_obj  *pdf_create_ToUnicode_CMap   (const char *enc_name,
					      char **enc_vec, char *is_used);

/* Add /ToUnicode entry with indirect reference to ToUnicode stream
 * to font dictionary "fontdict". This creates an empty stream object
 * if it has not already created. Generation of actual ToUnicode CMap
 * data is delayed until pdf_close_encodings(). This also check if
 * glyphs can be converted to (a sequence of) Unicode and returns -1
 * if more than 10% of glyphs can't be converted. ToUnicode CMap is
 * not attached in that case.
 */
extern int       pdf_attach_ToUnicode_CMap   (pdf_obj *fontdict,
					      int encoding_id, char *is_used);

/* Just load CMap identified with 'ident'. (parsed)
 * PDF stream object (not reference) returned.
 */ 
extern pdf_obj  *pdf_load_ToUnicode_stream   (const char *ident);

#endif /* _PDFENCODINGS_H_ */
