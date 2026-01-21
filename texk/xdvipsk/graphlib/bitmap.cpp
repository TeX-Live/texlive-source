/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright by Floris van den Berg,
	Hervé Drolon and Karl-Heinz Bussian, the FreeImage 3 project team.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

// ==========================================================
// Bitmap implementation
//
// ==========================================================

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <string.h>
#include <stdarg.h>
#ifdef _APPLE_
#include <wctype.h>
#else
#include <ctype.h>
#endif
#endif

#include "bitmap.h"
#include "bitmapIO.h"


//----------------------------------------------------------------------

BOOL
Bitmap_IsLittleEndian() {
	union {
		DWORD i;
		BYTE c[4];
	} u;
	u.i = 1;
	return (u.c[0] != 0);
}

//----------------------------------------------------------------------

static Bitmap_OutputMessageFunction bitmap_outputmessage_proc = NULL;
static Bitmap_OutputMessageFunctionStdCall bitmap_outputmessagestdcall_proc = NULL; 

void
Bitmap_SetOutputMessage(Bitmap_OutputMessageFunction omf) {
	bitmap_outputmessage_proc = omf;
}

void
Bitmap_SetOutputMessageStdCall(Bitmap_OutputMessageFunctionStdCall omf) {
	bitmap_outputmessagestdcall_proc = omf;
}

void
Bitmap_OutputMessageProc(int fif, const char *fmt, ...) {
	const int MSG_SIZE = 512; // 512 bytes should be more than enough for a short message

	if ((fmt != NULL) && ((bitmap_outputmessage_proc != NULL) || (bitmap_outputmessagestdcall_proc != NULL))) {
		char message[512];
		memset(message, 0, MSG_SIZE);

		// initialize the optional parameter list

		va_list arg;
		va_start(arg, fmt);

		// check the length of the format string

		int str_length = (int)( ((int)strlen(fmt) > MSG_SIZE) ? MSG_SIZE : strlen(fmt) );

		// parse the format string and put the result in 'message'

		for (int i = 0, j = 0; i < str_length; ++i) {
			if (fmt[i] == '%') {
				if (i + 1 < str_length) {
					switch(tolower(fmt[i + 1])) {
						case '%' :
							message[j++] = '%';
							break;

						case 'o' : // octal numbers
						{
							char tmp[16];

                                                        sprintf(tmp,"%o",va_arg(arg, int));
							//_itoa(va_arg(arg, int), tmp, 8);

							strcat(message, tmp);

							j += (int)strlen(tmp);

							++i;

							break;
						}

						case 'i' : // decimal numbers
						case 'd' :
						{
							char tmp[16];

                                                        sprintf(tmp,"%d",va_arg(arg, int)); 
							//_itoa(va_arg(arg, int), tmp, 10);

							strcat(message, tmp);

							j += (int)strlen(tmp);

							++i;

							break;
						}

						case 'x' : // hexadecimal numbers
						{
							char tmp[16];

                                                        sprintf(tmp,"%x",va_arg(arg, int));
							//_itoa(va_arg(arg, int), tmp, 16);

							strcat(message, tmp);

							j += (int)strlen(tmp);

							++i;

							break;
						}

						case 's' : // strings
						{
							char *tmp = va_arg(arg, char*);

							strcat(message, tmp);

							j += (int)strlen(tmp);

							++i;

							break;
						}
					};
				} else {
					message[j++] = fmt[i];
				}
			} else {
				message[j++] = fmt[i];
			};
		}

		// deinitialize the optional parameter list

		va_end(arg);

		// output the message to the user program

		if (bitmap_outputmessage_proc != NULL)
			bitmap_outputmessage_proc((BITMAP_FORMAT)fif, message);

		if (bitmap_outputmessagestdcall_proc != NULL)
			bitmap_outputmessagestdcall_proc((BITMAP_FORMAT)fif, message);
	}
}
