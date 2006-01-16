/*
 * File:    types.h
 * Purpose: define basic types so that program is portable to non 32 bits
 *          platforms
 * Version: 1.0 (Dec. 1993)
 * Author:  Piet Tutelaers
 */

#ifndef __TYPES__
#define __TYPES__

#if defined(WORD16)
typedef unsigned short USHORT; /* 16 bits */
typedef short SHORT;           /* 16 bits */
typedef long LONG;             /* 32 bits */
typedef unsigned long ULONG;   /* 32 bits */
typedef double DOUBLE;         /* 64 bits */
#elif defined(WORD64)
typedef unsigned short USHORT; /* 16 bits */
typedef short SHORT;           /* 16 bits */
typedef int LONG;              /* 32 bits */
typedef unsigned long ULONG;   /* 32 bits */
typedef double DOUBLE;         /* 64 bits */
#else /* default: WORD32 */
typedef unsigned short USHORT; /* 16 bits */
typedef short SHORT;           /* 16 bits */
typedef int LONG;              /* 32 bits */
typedef unsigned int ULONG;    /* 32 bits */
typedef double DOUBLE;         /* 64 bits */
#endif

#endif /* __TYPES__ */
