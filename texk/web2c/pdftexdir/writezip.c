/*
Copyright (c) 1996-2002 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/writezip.c#7 $
*/

#include "ptexlib.h"
#include "zlib.h"

static const char perforce_id[] = 
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/writezip.c#7 $";

#define ZIP_BUF_SIZE  32768

#define check_err(f, fn)                                   \
    if (f != Z_OK)                                         \
        pdftex_fail("zlib: %s() failed", fn)


static char zipbuf[ZIP_BUF_SIZE];
static z_stream c_stream; /* compression stream */

void writezip(boolean finish)
{
    int err;

    if (!getpdfcompresslevel()) {
        if (pdfptr) {
            pdfgone += xfwrite(pdfbuf, 1, pdfptr, pdffile);
            pdfstreamlength += pdfptr;
            }
        return;
    }

    cur_file_name = NULL;
    if (pdfstreamlength == 0) {
        c_stream.zalloc = (alloc_func)0;
        c_stream.zfree = (free_func)0;
        c_stream.opaque = (voidpf)0;
        check_err(deflateInit(&c_stream, getpdfcompresslevel()), "deflateInit");
        c_stream.next_out = (Bytef*)zipbuf;
        c_stream.avail_out = ZIP_BUF_SIZE;
    }
    c_stream.next_in = pdfbuf;
    c_stream.avail_in = pdfptr;
    for(;;) {
        if (c_stream.avail_out == 0) {
            pdfgone += xfwrite(zipbuf, 1, ZIP_BUF_SIZE, pdffile);
            c_stream.next_out = (Bytef*)zipbuf;
            c_stream.avail_out = ZIP_BUF_SIZE;
        }
        err = deflate(&c_stream, finish ? Z_FINISH : Z_NO_FLUSH);
        if (finish && err == Z_STREAM_END)
            break;
        check_err(err, "deflate");
        if (!finish && c_stream.avail_in == 0)
            break;
    }
    if (finish) {
        if (c_stream.avail_out < ZIP_BUF_SIZE) /* at least one byte has been output */
            pdfgone += xfwrite(zipbuf, 1, ZIP_BUF_SIZE - c_stream.avail_out, pdffile);
        check_err(deflateEnd(&c_stream), "deflateEnd");
        xfflush(pdffile);
    }
    pdfstreamlength = c_stream.total_out;
}
