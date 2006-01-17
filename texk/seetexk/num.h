/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Pointer I/O: numbers.
 *
 * We deal in fixed format numbers and pointers here.
 * For file I/O, see fio.h.
 */

/*
 * Get one unsigned byte.  Note that this is a proper expression.
 * The rest have more limited contexts, and are therefore OddLy
 * CapItaliseD.
 */
#define	pgetbyte(p)	UnSign8(*(p)++)

/*
 * Get a two-byte unsigned integer, a three-byte unsigned integer,
 * or a four-byte signed integer.
 */
#define	pGetWord(p, r) ((r)  = UnSign8(*(p)++) << 8, \
			(r) |= UnSign8(*(p)++))
#define	pGet3Byte(p,r) ((r)  = UnSign8(*(p)++) << 16, \
			(r) |= UnSign8(*(p)++) << 8, \
			(r) |= UnSign8(*(p)++))
#define	pGetLong(p, r) ((r)  = UnSign8(*(p)++) << 24, \
			(r) |= UnSign8(*(p)++) << 16, \
			(r) |= UnSign8(*(p)++) << 8, \
			(r) |= UnSign8(*(p)++))

/*
 * ADD pputbyte, pPutWord, pPut3Byte, pPutLong HERE IF THEY PROVE
 * USEFUL.  (But then must consider changing PutWord &c in fio.h.)
 */
