	/*525:*/
	#line 10706 "format.w"

#ifndef __BASETYPES_H__
#define __BASETYPES_H__
#include <stdlib.h>
#include <stdio.h>
#ifndef _STDLIB_H
#define _STDLIB_H
#endif
#ifdef  _MSC_VER
#include <windows.h>
#define uint8_t UINT8
#define uint16_t UINT16
#define uint32_t UINT32
#define uint64_t UINT64
#define int8_t INT8
#define int16_t INT16
#define int32_t INT32
#define bool BOOL
#define true (0==0)
#define false (!true)
#define __SIZEOF_FLOAT__ 4
#define __SIZEOF_DOUBLE__ 8
#define PRIx64 "I64x"
#pragma  warning( disable : 4244 4996 4127)
#else
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#ifdef WIN32
#include <io.h>
#endif
#endif
typedef float float32_t;
typedef double float64_t;
#if __SIZEOF_FLOAT__!=4
#error  float32 type must have size 4
#endif
#if __SIZEOF_DOUBLE__!=8
#error  float64 type must have size 8
#endif
#define HINT_VERSION 2
#define HINT_SUB_VERSION 0
#define AS_STR(X) #X
#define VERSION_AS_STR(X,Y) AS_STR(X) "." AS_STR(Y)
#define HINT_VERSION_STRING VERSION_AS_STR(HINT_VERSION, HINT_SUB_VERSION)
#endif
	/*:525*/
