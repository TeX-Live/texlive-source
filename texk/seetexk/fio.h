/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * File I/O: numbers.
 *
 * We deal in fixed format numbers and (FILE *)s here.
 * For pointer I/O, see pio.h.
 *
 * N.B.: These do the `wrong thing' at EOF.  It is imperative
 * that the caller add appropriate `if (feof(fp))' statements.
 */

/*
 * Get one unsigned byte.  Note that this is a proper expression.
 * The reset have more limited contexts, and are therefore OddLy
 * CapItaliseD.
 */
#define	fgetbyte(fp)	getc(fp)

/*
 * Get a two-byte unsigned integer, a three-byte unsigned integer,
 * or a four-byte signed integer.
 */
#define fGetWord(fp, r)	((r)  = getc(fp) << 8,  (r) |= getc(fp))
#define fGet3Byte(fp,r) ((r)  = getc(fp) << 16, (r) |= getc(fp) << 8, \
			 (r) |= getc(fp))
#define fGetLong(fp, r)	((r)  = getc(fp) << 24, (r) |= getc(fp) << 16, \
			 (r) |= getc(fp) << 8,  (r) |= getc(fp))

/*
 * Fast I/O write (and regular write) macros.
 */
#define	putbyte(fp, r)	((void) putc((r), fp))

#define PutWord(fp, r)	((void) putc((r) >> 8,  fp), \
			 (void) putc((r), fp))
#define Put3Byte(fp, r)	((void) putc((r) >> 16, fp), \
			 (void) putc((r) >> 8, fp), \
			 (void) putc((r), fp))
#define PutLong(fp, r)	((void) putc((r) >> 24, fp), \
			 (void) putc((r) >> 16, fp), \
			 (void) putc((r) >> 8, fp), \
			 (void) putc((r), fp))

/*
 * Function types
 */
i32	GetByte(), GetWord(), Get3Byte(), GetLong();
