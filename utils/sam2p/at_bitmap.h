/* This code is ripped from Autotrace-0.29, but modified by pts. */

/* bitmap.h: definition for a bitmap type.  No packing is done by
   default; each pixel is represented by an entire byte.  Among other
   things, this means the type can be used for both grayscale and binary
   images. */

#ifndef AT_BITMAP_H
#define AT_BITMAP_H

#ifdef __GNUC__
#pragma interface
#endif

#define PTS_SAM2P 1
#define FATALP(m) Error::sev(Error::EERROR) << m << (Error*)0;
#define WARNINGP(m) Error::sev(Error::WARNING) << m << (Error*)0;
#define FATALP1(m,a) Error::sev(Error::EERROR) << m << a << (Error*)0;
#define WARNINGP1(m,a) Error::sev(Error::WARNING) << m << a << (Error*)0;
#define FATALP3(m,a,n,b,o,c,p) Error::sev(Error::EERROR) << m << a << n << b << o << c << p << (Error*)0;
#define WARNINGP3(m,a,n,b,o,c,p) Error::sev(Error::WARNING) << m << n << b << o << c << p << (Error*)0;

#define XMALLOCT(var,typep,size) var=(typep)new char[size]
#define XMALLOCTU(var,type) var=(type*)new type
#define XFREE(p) delete p
/* ^^^ Imp: arrays?? */

/* #include "autotrace.h" */
typedef char *at_string;
typedef struct _at_bitmap_type {
  unsigned short height;
  unsigned short width;
  unsigned char *bitmap;
  unsigned int np;
} at_bitmap_type;

#include <stdio.h>

#define xfclose(fd,dummy) fclose(fd)

/* The basic structure and macros to access it.  */
typedef at_bitmap_type bitmap_type;

/* The number of color planes of each pixel */
#define BITMAP_PLANES(b)  ((b).np)

/* The pixels, represented as an array of bytes (in contiguous storage).
   Each pixel is represented by np bytes.  */
#define BITMAP_BITS(b)  ((b).bitmap)

/* These are convenient abbreviations for geting inside the members.  */
#define BITMAP_WIDTH(b)  ((b).width)
#define BITMAP_HEIGHT(b)  ((b).height)

/* This is the pixel at [ROW,COL].  */
#define BITMAP_PIXEL(b, row, col)					\
  ((BITMAP_BITS (b) + (row) * BITMAP_PLANES (b) * BITMAP_WIDTH (b)	\
        + (col) * BITMAP_PLANES(b)))

#define BITMAP_VALID_PIXEL(b, row, col)					\
   	((row) < BITMAP_HEIGHT (b) && (col) < BITMAP_WIDTH (b))

/* Allocate storage for the bits, set them all to white, and return an
   initialized structure.  */
extern bitmap_type new_bitmap (unsigned short, unsigned short);

/* Free that storage.  */
extern void free_bitmap (bitmap_type *);

#endif /* not AT_BITMAP_H */
