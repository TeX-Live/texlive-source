/*
 * select pages from a DVI file.
 *
 * The original version is:
 * Copyright (c) 1999
 *      WATANABE Takeshi        watanabe@komadori.planet.sci.kobe-u.ac.jp
 *
 * Heavily changed for xdvik:
 * Copyright (c) 2002-2004 the xdvik development team
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL ANY AUTHO OF THIS SOFTWARE BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef DVISEL_H_
#define DVISEL_H_

#include "dvi-init.h"
#include "print-internal.h"

/* callbacks */
Boolean check_pagerange(struct save_or_print_info *info, int pageno);
Boolean check_marked(struct save_or_print_info *info, int pageno);
void select_pages(struct save_or_print_info *info);

#endif /* DVISEL_H_ */
