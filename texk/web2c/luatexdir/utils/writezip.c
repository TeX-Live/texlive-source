/* writezip.c
   
   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

#include "ptexlib.h"
#include "zlib.h"
#include <assert.h>

static const char __svn_version[] =
    "$Id: writezip.c 2073 2009-03-21 10:06:50Z hhenkel $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/utils/writezip.c $";

#define ZIP_BUF_SIZE  32768

#define check_err(f, fn)                        \
  if (f != Z_OK)                                \
    pdftex_fail("zlib: %s() failed (error code %d)", fn, f)

static char *zipbuf = NULL;
static z_stream c_stream;       /* compression stream */

void write_zip(boolean finish)
{
    int err;
    static int level_old = 0;
    int level = get_pdf_compress_level();
    assert(level > 0);
    cur_file_name = NULL;
    if (pdf_stream_length == 0) {
        if (zipbuf == NULL) {
            zipbuf = xtalloc(ZIP_BUF_SIZE, char);
            c_stream.zalloc = (alloc_func) 0;
            c_stream.zfree = (free_func) 0;
            c_stream.opaque = (voidpf) 0;
            check_err(deflateInit(&c_stream, level), "deflateInit");
        } else {
            if (level != level_old) {   /* \pdfcompresslevel change in mid document */
                check_err(deflateEnd(&c_stream), "deflateEnd");
                c_stream.zalloc = (alloc_func) 0;       /* these 3 lines no need, just to be safe */
                c_stream.zfree = (free_func) 0;
                c_stream.opaque = (voidpf) 0;
                check_err(deflateInit(&c_stream, level), "deflateInit");
            } else
                check_err(deflateReset(&c_stream), "deflateReset");
        }
        level_old = level;
        c_stream.next_out = (Bytef *) zipbuf;
        c_stream.avail_out = ZIP_BUF_SIZE;
    }
    assert(zipbuf != NULL);
    c_stream.next_in = pdf_buf;
    c_stream.avail_in = pdf_ptr;
    for (;;) {
        if (c_stream.avail_out == 0) {
            pdf_gone += xfwrite(zipbuf, 1, ZIP_BUF_SIZE, pdf_file);
            pdf_last_byte = zipbuf[ZIP_BUF_SIZE - 1];   /* not needed */
            c_stream.next_out = (Bytef *) zipbuf;
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
        if (c_stream.avail_out < ZIP_BUF_SIZE) {        /* at least one byte has been output */
            pdf_gone +=
                xfwrite(zipbuf, 1, ZIP_BUF_SIZE - c_stream.avail_out, pdf_file);
            pdf_last_byte = zipbuf[ZIP_BUF_SIZE - c_stream.avail_out - 1];
        }
        xfflush(pdf_file);
    }
    pdf_stream_length = c_stream.total_out;
}

void zip_free(void)
{
    if (zipbuf != NULL) {
        check_err(deflateEnd(&c_stream), "deflateEnd");
        free(zipbuf);
    }
}
