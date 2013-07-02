/*------------------------------------------------------------

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

#include "xdvi-config.h"

#include <string.h>
#include <stdio.h>

#include "xdvi.h"
#include "my-vsnprintf.h"
#include "my-snprintf.h"

#ifndef HAVE_SNPRINTF
/*------------------------------------------------------------
 *  my_snprintf - emulation of glibc2.1 snprintf, use if
 *		  snprintf is not available	
 *
 *  Arguments:
 *	char *str	- buffer to print <format> string into
 *	int size	- print only len characters of <format>
 *	char *format	- variable format list
 *
 *  Returns:
 *	 int size - number of characters that would have been
 *		    written if enough space had been available
 *
 *  Purpose:
 *	Implementation of snprintf function, using my_vsnprintf
 *	(which see).
 *------------------------------------------------------------*/
int my_snprintf(char *str, size_t size, const  char  *format, ...)
{
    va_list args;
    int retVal;

    va_start(args, format);
    retVal = VSNPRINTF(str, size, format, args);
    va_end(args);

    return retVal;
}
#endif
