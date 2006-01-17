/*
 * Copyright (c) 2004 Stefan Ulrich
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

#ifndef FILEHIST_H_
#define FILEHIST_H_

typedef	void (*filehistCallbackT) (int idx, const char *filename, int pageno, void *data);

extern void file_history_enumerate(filehistCallbackT callback, void *data);
extern void file_history_init(void);
extern Boolean file_history_push(const char *filename);
extern void file_history_set_page(int pageno);
extern void file_history_set_page_at(int idx, int pageno);
extern int file_history_get_page(void);
extern char *file_history_get_elem(int idx, int *ret_page);
extern char *file_history_get_list(void);
extern void file_history_open(const char *filename);
extern size_t file_history_size(void);

#endif /* FILEHIST_H_ */

