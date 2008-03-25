/* Copyright 2008 Akira Kakuto
   You may freely use, modify and/or distribute this file.  */

#ifndef TMF_POOL_H
#define TMF_POOL_H
#ifndef META_FONT
typedef int integer;
typedef integer strnumber;
typedef unsigned char packedASCIIcode;
typedef integer poolpointer;
extern  packedASCIIcode *strpool;
extern  poolpointer poolptr;
#else
#define EXTERN extern
#include "mfd.h"
#endif
#endif
