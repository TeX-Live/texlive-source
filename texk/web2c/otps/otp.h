/* otp.h: Instructions found in OCP files.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
Copyright (c) 2002 Behdad Esfahbod
Copyright (c) 2002 Roozbeh Pournader

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

enum {
    OTP_START_TOKENS = 256,
    OTP_RIGHT_OUTPUT,
    OTP_RIGHT_NUM,
    OTP_RIGHT_CHAR,
    OTP_RIGHT_LCHAR,
    OTP_RIGHT_SOME,

    OTP_PBACK_OUTPUT,
    OTP_PBACK_NUM,
    OTP_PBACK_CHAR,
    OTP_PBACK_LCHAR,
    OTP_PBACK_SOME,

    OTP_ADD,
    OTP_SUB,
    OTP_MULT,
    OTP_DIV,
    OTP_MOD,
    OTP_LOOKUP,
    OTP_PUSH_NUM,
    OTP_PUSH_CHAR,
    OTP_PUSH_LCHAR,

    OTP_STATE_CHANGE,
    OTP_STATE_PUSH,
    OTP_STATE_POP,

    OTP_LEFT_START,
    OTP_LEFT_RETURN,
    OTP_LEFT_BACKUP,

    OTP_GOTO,
    OTP_GOTO_NE,
    OTP_GOTO_EQ,
    OTP_GOTO_LT,
    OTP_GOTO_LE,
    OTP_GOTO_GT,
    OTP_GOTO_GE,
    OTP_GOTO_NO_ADVANCE,
    OTP_GOTO_BEG,
    OTP_GOTO_END,

    OTP_STOP,
};

#define OTP_PBACK_OFFSET (OTP_PBACK_OUTPUT - OTP_RIGHT_OUTPUT)

#define OTP_MAXCODE 0xFFFF

#define OTP_MAXSTATES 100
#define OTP_MAXTABLES 100
#define OTP_MAXALIASES 1000
