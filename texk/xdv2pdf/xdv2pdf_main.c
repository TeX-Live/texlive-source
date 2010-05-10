/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
 written by Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining  
a copy of this software and associated documentation files (the  
"Software"), to deal in the Software without restriction, including  
without limitation the rights to use, copy, modify, merge, publish,  
distribute, sublicense, and/or sell copies of the Software, and to  
permit persons to whom the Software is furnished to do so, subject to  
the following conditions:

The above copyright notice and this permission notice shall be  
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,  
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF  
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND  
NONINFRINGEMENT. IN NO EVENT SHALL SIL INTERNATIONAL BE LIABLE FOR  
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of SIL International  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from SIL International.
\****************************************************************************/

/*
	xdv2pdf_main
	
	Main file for xdv2pdf
	Written in plain C for easier linkage to kpathsea library
*/

#define MAC_OS_X_VERSION_MIN_REQUIRED	1020

#include <kpathsea/progname.h>
#include <kpathsea/tex-file.h>

#include "xdv_kpse_formats.h"

/* The program name, for `.PROG' construct in texmf.cnf.  (-program) */
string progname = NULL;

int
main(int argc, char** argv)
{
    kpse_set_program_name (argv[0], progname);
    
    /* Initialize type1 search path and change the suffix list.  */
    kpse_init_format (kpse_type1_format);
    kpse_set_suffixes (kpse_type1_format, false, ".pfb", NULL);

    return xdv2pdf(argc, argv);
}
