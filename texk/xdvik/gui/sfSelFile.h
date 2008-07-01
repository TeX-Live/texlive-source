/*
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef SFSELFILE_H_
#define SFSELFILE_H_

struct filesel_callback {
    Widget shell;
    char *browse_fname;
    const char *title;
    const char *prompt;
    const char *ok;
    const char *cancel;
    const char *init_path;
    const char *filemask;
    Boolean must_exist;
    Boolean exit_on_cancel;
    void (*func_ptr)(const char *filename, void *data);
    void *data;
};

#define Xdvi_NEW_WINDOW_RADIO_NAME "new_window_radio"

extern void SFpositionWidget(Widget w);
extern FILE *SFopenFile(const char *name, const char *mode, const char *prompt, const char *failed);
extern void SFtextChanged(void);
extern Widget XsraSelFile(Widget parent, struct filesel_callback *callback);
extern void XsraSelFilePopup(struct filesel_callback *callback);
void raise_file_selector(void);

#endif /* SF_SEL_FILE_H_ */
