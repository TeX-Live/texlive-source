#ifndef __decs_h   /* prevent multiple inclusions */
#define __decs_h

#include <windows.h>
#ifndef __unzip_h
#  include "unzip.h"
#endif
#ifndef __structs_h
#  include "structs.h"
#endif

#define WizMatch match

void    WINAPI Wiz_NoPrinting(int f);
int     WINAPI Wiz_Validate(LPSTR archive, int AllCodes);
BOOL    WINAPI Wiz_Init(zvoid *, LPUSERFUNCTIONS);
BOOL    WINAPI Wiz_SetOpts(zvoid *, LPDCL);
int     WINAPI Wiz_Unzip(zvoid *, int, char **, int, char **);
int     WINAPI Wiz_SingleEntryUnzip(int, char **, int, char **,
                                    LPDCL, LPUSERFUNCTIONS);

int     WINAPI Wiz_UnzipToMemory(LPSTR zip, LPSTR file,
                                 LPUSERFUNCTIONS lpUserFunctions,
                                 UzpBuffer *retstr);
int     WINAPI Wiz_Grep(LPSTR archive, LPSTR file, LPSTR pattern,
                        int cmd, int SkipBin,
                        LPUSERFUNCTIONS lpUserFunctions);

#endif /* __decs_h */
