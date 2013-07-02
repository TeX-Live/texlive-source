/*------------------------------------------------------------

written by Stefan Ulrich <stefanulrich@users.sourceforge.net> 2001/02/25

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

#if !HAVE_VSNPRINTF || !HAVE_GOOD_VSNPRINTF
/*------------------------------------------------------------
 *  my_vsnprintf - emulation of glibc2.1 vsnprintf, use if
 *		   vsnprintf is not available	
 *
 *  Arguments:
 *	char *buf	- buffer to print <format> string into
 *	int len		- print only len characters of <format>
 *	char *format	- format string
 *	va_list argp	- variable argument list
 *
 *  Returns:
 *	 int size - number of characters that would have been
 *		    written if enough space had been available
 *
 *  Purpose:
 *	Implementation of GNU's vsnprintf using POSIX pipes (less
 *      portable and much slower than the original, but also much
 *	easier to implement ;-):
 *	Print the formatted string to a pipe and read back
 *	at most len-1 characters, appending a '\0' at the end.
 *------------------------------------------------------------*/

int
my_vsnprintf(char *buf, size_t len, const char *format, va_list argp)
{
    int pipe_fd[2];
    FILE *fd;
    int size;
#ifdef DEBUG
    printf("=============my_vsnprintf called!\n");
#endif
    if (len > 0) {
	len--;	/* for the trailing '\0' */
    }
    
    /* create a pipe for reading/writing */
    if (xpipe(pipe_fd) == -1) {
	perror("my_vsnprintf: pipe");
	xdvi_exit(EXIT_FAILURE);
    }
    if ((fd = try_fdopen(pipe_fd[1], "w")) == NULL) {
	perror("my_vsnprintf: fdopen");
	xdvi_exit(EXIT_FAILURE);
    }
    /* instead of setting fd to non-buffering:
       setvbuf(fd, NULL, _IONBF, BUFSIZ);
       flush it explicitly: */
    size = vfprintf(fd, format, argp);
    fflush(fd);

    /* according to C99, buf may be NULL, in which case
       just return correct number of chars. Also, don't
       write anything if there had been an output error. */
    if (buf != NULL && size >= 0) {
	/* wrote less characters than len -> adjust len for the subsequent read */
	if ((size_t)size < len) {
	    len = size;
	}
	read(pipe_fd[0], buf, len);
	buf[len] = '\0';     /* always null-terminate */
    }

    fclose(fd);
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return size;
}

#endif /* !HAVE_VSNPRINTF || !HAVE_GOOD_VSNPRINTF */
