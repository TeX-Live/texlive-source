/*
 * File:    types.h
 * Purpose: define basic types so that program is portable to non 32 bits
 *          platforms
 * Version: 1.0 (Dec. 1993)
 * Author:  Piet Tutelaers
 */

#ifndef __PS2PK_TYPES__
#define __PS2PK_TYPES__

#ifdef KPATHSEA
#include <kpathsea/kpathsea.h>
#include <c-auto.h>
#endif /* KPATHSEA */

typedef unsigned short USHORT; /* 16 bits */
typedef short SHORT;           /* 16 bits */
#if defined(WORD16)
typedef long LONG;             /* 32 bits */
typedef unsigned long ULONG;   /* 32 bits */
#elif defined(WORD64)
typedef int LONG;              /* 32 bits */
typedef unsigned long ULONG;   /* 32 bits */
#else /* default: WORD32 */
#  ifndef WIN32
/* These are already defined under WIN32 */
typedef int LONG;              /* 32 bits */
typedef unsigned int ULONG;    /* 32 bits */
#  endif
#endif
typedef double DOUBLE;         /* 64 bits */

typedef char *encoding[256];
extern void getenc(char **, char **, encoding, int [256]);

#endif /* __PS2PK_TYPES__ */
