/* synctex-luatex.h

   Copyright (c) 2010 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

License:
--------
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE

Except as contained in this notice, the name of the copyright holder  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from the copyright holder.

*/

/* $Id: synctex-luatex.h 3536 2010-03-22 13:07:39Z taco $ */

#undef synchronization_field_size
#define synchronization_field_size 1

#undef width_offset
#undef height_offset
#undef depth_offset
#undef box_node_size
#undef rule_node
#undef glue_node
#undef kern_node

#undef SYNCTEX_TYPE
#undef SYNCTEX_SUBTYPE
#undef SYNCTEX_WIDTH
#undef SYNCTEX_DEPTH
#undef SYNCTEX_HEIGHT
#undef SYNCTEX_TAG_MODEL
#undef SYNCTEX_LINE_MODEL
#undef SYNCTEX_VALUE

#define SYNCTEX_TAG_MODEL(NODE,SIZE)\
                    vinfo(NODE+SIZE-synchronization_field_size)
#define SYNCTEX_LINE_MODEL(NODE,SIZE)\
                    vlink(NODE+SIZE-synchronization_field_size)

#define SYNCTEX_TYPE(NODE) type(NODE)
#define SYNCTEX_SUBTYPE(NODE) subtype(NODE)
#define SYNCTEX_WIDTH(NODE) width(NODE)
#define SYNCTEX_DEPTH(NODE) depth(NODE)
#define SYNCTEX_HEIGHT(NODE) height(NODE)
#define SYNCTEX_VALUE int_par(synctex_code)


#define CURV (dimen_par(page_height_code)-static_pdf->posstruct->pos.v)
#define CURH static_pdf->posstruct->pos.h

#define GETJOBNAME() makecstring(job_name)
#define GETLOGNAME() (char *)xstrdup((const char*)texmf_log_name)
#define gettexstring(a) makecstring(a)

#define rulewd width(p)
#define ruleht height(p)
#define ruledp depth(p)

#define curinput cur_input
#define totalpages total_pages
#define synctextagfield synctex_tag_field
#define namefield name_field

#include "ptexlib.h"

/*   We observe pdfoutputvalue in order to determine whether output mode is
 *   pdf or dvi.
 *   We will assume that pdf_output_value equals pdf_output before entering
 *   the synctex_sheet function below.  */
#undef  SYNCTEX_OFFSET_IS_PDF
#define SYNCTEX_OFFSET_IS_PDF (pdf_output_value>0)
#undef  SYNCTEX_OUTPUT
#define SYNCTEX_OUTPUT ((pdf_output_value>0)?"pdf":"dvi")

#define __SyncTeX__ 1
