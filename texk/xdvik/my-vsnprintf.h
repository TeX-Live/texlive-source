/*------------------------------------------------------------

written by S. Ulrich (ulrich@cis.uni-muenchen.de)  2001/02/25

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------*/

#ifndef MY_VSNPRINTF_H__
#define MY_VSNPRINTF_H__

#include "xdvi.h"

#if HAVE_VSNPRINTF && HAVE_GOOD_VSNPRINTF

#include <stdio.h>
#include <stdarg.h>
#define VSNPRINTF(buf, len, format, args) vsnprintf(buf, len, format, args)

#else /* HAVE_VSNPRINTF && HAVE_GOOD_VSNPRINTF */

#include <unistd.h> /* for pipe() */
#include "util.h" /* for xpipe() etc. */
extern int my_vsnprintf(char *, size_t, const char *, va_list);
#define VSNPRINTF(buf, len, format, args) my_vsnprintf(buf, len, format, args)

#endif /* HAVE_VSNPRINTF && HAVE_GOOD_VSNPRINTF */

#endif /* MY_VSNPRINTF_H__ */
