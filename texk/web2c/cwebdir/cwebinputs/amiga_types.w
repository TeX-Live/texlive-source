%
% CWEB/INCLUDE $VER: AMIGA_TYPES.W 1.3 (13.8.1998)
%
% SYNOPSIS:
%    This file tells CWEAVE to treat AMIGA specific keywords as
%    reserved words.  Some of these are introduced by Commodore's
%    operating system, others by the SAS/C compiler.
%
% CREATION DATE: unknown
%
% AUTHOR: Andreas Scherer
%
% COPYRIGHT: There is no restriction to the usage of this file.

@q Keywords specific to SAS/C@>

@s __aligned int
@s __asm int
@s __chip int
@s __far int
@s __inline int
@s __interrupt int
@s __near int
@s __regargs int
@s __saveds int
@s __stackext int
@s __stdargs int

@q Registers of the AMIGA@>

@s __d0 int
@s __d1 int
@s __d2 int
@s __d3 int
@s __d4 int
@s __d5 int
@s __d6 int
@s __d7 int
@s __a0 int
@s __a1 int
@s __a2 int
@s __a3 int
@s __a4 int
@s __a5 int
@s __a6 int
@s __a7 int
@s __fp0 int
@s __fp1 int
@s __fp2 int
@s __fp3 int
@s __fp4 int
@s __fp5 int
@s __fp6 int
@s __fp7 int

@q Keywords by Commodore@>

@s GLOBAL int
@s IMPORT int
@s STATIC int
@s REGISTER int
@s VOID int
@s APTR int
@s LONG int
@s ULONG int
@s LONGBITS int
@s WORD int
@s UWORD int
@s WORDBITS int
@s BYTE int
@s UBYTE int
@s BYTEBITS int
@s RPTR int
@s STRPTR int
@s SHORT int
@s USHORT int
@s COUNT int
@s UCOUNT int
@s CPTR int
@s FLOAT int
@s DOUBLE int
@s BOOL int
@s TEXT int
@s BPTR int
@s BSTR int

@s byte int
@s Class int
@s ClassID int
@s CxMsg int
@s CxObj int
@s dev_t int
@s DIR int
@s DisplayInfoHandle int
@s ino_t int
@s IX int
@s Msg int
@s Object int
@s off_t int
@s PFL int
@s PLANEPTR int
@s Tag int
@s tPoint int
@s ushort int
@s u_char int
@s u_int int
@s u_long int
@s u_short int
@s WINDOW int
