/*
 *  ChkTeX v1.5, utility functions -- header file.
 *  Copyright (C) 1995-96 Jens T. Berger Thielemann
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Contact the author at:
 *              Jens Berger
 *              Spektrumvn. 4
 *              N-0666 Oslo
 *              Norway
 *              E-mail: <jensthi@ifi.uio.no>
 *
 *
 */

#ifndef UTILITY_H
#define UTILITY_H 1

#ifndef CHKTEX_H
# include "ChkTeX.h"
#endif /* CHKTEX_H */
#ifndef OPSYS_H
# include "OpSys.h"
#endif /* OPSYS_H */


/*
 * How many indexes we'll allocate first time
 */
#define MINPUDDLE       256

/*
 * How many bytes we want in front/end of each TEXT buffer. > 2
 */
#define WALLBYTES       4

#ifndef HASH_SIZE
#  define HASH_SIZE    1009   /* A reasonably large prime */
#endif

#define FORWL(ind, list)  for(ind = 0; ind < (list).Stack.Used; ind++)


enum Strip {
    STRP_LFT = 0x01,
    STRP_RGT = 0x02,
    STRP_BTH = 0x03
};

struct HashEntry {
    struct HashEntry *Next;
    STRPTR Str;
};

struct Hash {
    struct HashEntry **Index;
};

struct Stack {
  APTR  *Data;
  ULONG Size, Used;
};

struct WordList {
    ULONG         MaxLen;
    BOOL          NonEmpty;
    struct Stack  Stack;
    struct Hash   Hash;
};
#define WORDLIST_DEFINED

struct FileNode {
  STRPTR        Name;
  FILE  *fh;
  ULONG Line;
};

/* Rotates x n bits left (should be an int, long, etc.) */
#define ROTATEL(x,n) ((x<<n) | (x>>((CHAR_BIT*sizeof(x)) - n)))

/* Rotates x n bits right (should be an int, long, etc.) */
#define ROTATER(x,n) ((x>>n) | (x<<((CHAR_BIT*sizeof(x)) - n)))

BOOL    fexists         __PROTO((const STRPTR Filename));

APTR    sfmemset        __PROTO((APTR to, int c, LONG n));
void *  saferealloc     __PROTO((void *old, size_t newsize));

int     strafter        __PROTO((const STRPTR Str, const STRPTR Cmp));
void    strrep          __PROTO((STRPTR String, const TEXT From,
				 const TEXT To));
void    strxrep         __PROTO((STRPTR Buf, const STRPTR Prot,
				 const TEXT To));
STRPTR  strip           __PROTO((STRPTR String, const enum Strip What));
void    strwrite        __PROTO((STRPTR To, const STRPTR From, ULONG Len));
int     strinfront      __PROTO((STRPTR Str, STRPTR Cmp));
STRPTR  strdupx         __PROTO((const STRPTR String, int Extra));
void    strmove         __PROTO((char *a, const char *b));

inline void
        ClearHash       __PROTO((struct Hash *h));
inline void
        InsertHash      __PROTO((const STRPTR a, struct Hash *h));
inline STRPTR
        HasHash         __PROTO((const STRPTR a, const struct Hash *h));

BOOL    InsertWord      __PROTO((const STRPTR Word, struct WordList *WL));
STRPTR  HasWord         __PROTO((const STRPTR Word, struct WordList *WL));
void    MakeLower       __PROTO((struct WordList *wl));
void    ListRep         __PROTO((struct WordList *wl, const TEXT From,
                                 const TEXT To));
void    ClearWord       __PROTO((struct WordList *WL));

BOOL    StkPush         __PROTO((const APTR Data, struct Stack *Stack));
inline APTR
        StkPop          __PROTO((struct Stack *Stack));
APTR
        StkTop          __PROTO((struct Stack *Stack));

FILE *  CurStkFile      __PROTO((struct Stack *stack));
STRPTR  CurStkName      __PROTO((struct Stack *stack));
ULONG   CurStkLine      __PROTO((struct Stack *stack));
STRPTR  FGetsStk        __PROTO((STRPTR Dest, ULONG len, struct Stack *stack));
BOOL    PushFileName    __PROTO((const STRPTR Name, struct Stack *stack));
BOOL    PushFile        __PROTO((STRPTR, FILE *, struct Stack *));


void    FreeErrInfo     __PROTO((struct ErrInfo* ei));
struct ErrInfo *
        PushChar        __PROTO((const TEXT c, const ULONG Line,
				 const ULONG Column, struct Stack *Stk,
				 const STRPTR LineCpy));
struct ErrInfo *
        PushErr         __PROTO((const STRPTR Data, const ULONG Line,
                                const ULONG Column, const ULONG ErrLen,
                                const STRPTR LineCpy, struct Stack *Stk));
struct ErrInfo *TopChar __PROTO((struct Stack *Stack));
struct ErrInfo *TopErr  __PROTO((struct Stack *Stack));
struct ErrInfo *PopErr  __PROTO((struct Stack *Stack));
struct ErrInfo *TopMatch __PROTO((struct Stack *Stack, STRPTR String));

LONG    BrackIndex      __PROTO((TEXT const c));
void    AddBracket      __PROTO((TEXT const c));
TEXT   MatchBracket    __PROTO((TEXT const));



WORD    substring       __PROTO((const STRPTR source, STRPTR dest,
				 ULONG pos, LONG len));

#ifndef  HAVE_STRLWR
#  define  strlwr  mystrlwr
STRPTR  mystrlwr          __PROTO((STRPTR String));
#endif

#ifndef  HAVE_STRDUP
#  define  strdup  mystrdup
STRPTR  mystrdup          __PROTO((const STRPTR String));
#endif


#ifndef HAVE_STRCASECMP
int strcasecmp(char* a, char* b);
#endif



#endif /* UTILITY_H */
