/*
 * Copyright (c) 2003-2004 the xdvik development team
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ENCODINGS_H_
#define ENCODINGS_H_

#include "xdvi.h"

struct adobe2unicode {
    const char *adobe_name;
    uint32_t unicode;
};

struct unicode2adobe {
    uint32_t unicode;
    const char *adobe_name;
};

extern uint32_t get_accented_glyph(uint32_t accent, uint32_t base_glyph);
extern const char *unicode2adobe_name(uint32_t unicode);
extern uint32_t adobe2unicode_name(const char *adobe_name);
extern uint32_t guess_encoding(wide_ubyte ch, const char *fontname, char *retbuf);
extern const char *expand_ligature(uint32_t unicode);
extern const char *search_normalize_chars(uint32_t unicode);

extern Boolean utf8_lowercase(char *utf8);
extern int utf8_to_ucs4(const char *utf8, uint32_t *ucs4, size_t len);
extern void ucs4_to_utf8(uint32_t ucs4, char *utf8, size_t *len, Boolean do_lowercase);
extern unsigned char utf8_to_iso_8859_1(const char *utf8, size_t *len);
extern char *str_utf8_to_iso_8859_1(const char *utf8);
extern void iso_8859_1_to_utf8(unsigned char iso_8859_1, char *utf8, size_t *len);
extern int str_iso_8859_1_to_utf8(const char *latin1, char *utf8, size_t len);
extern Boolean is_hyphenchar(uint32_t unicode);
extern Boolean is_ideograph(uint32_t unicode);

#endif /* ENCODINGS_H_ */

