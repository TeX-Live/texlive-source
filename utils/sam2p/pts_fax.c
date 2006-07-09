/*
 * pts_fax.c -- a compact CCITTFax compressor and uncompressor) implementation
 * compiled by pts@fazekas.hu at Sun Jul  7 19:51:42 CEST 2002
 *
 * algorithm ripped from GNU Ghostscript, implementation and (C):
 *
Copyright (C) 1993, 1995, 1996, 1997, 1998, 1999 Aladdin Enterprises.  All
rights reserved.

GNU Ghostscript is free software; you can redistribute it and/or
modify it under the terms of version 2 of the GNU General Public
License as published by the Free Software Foundation.

GNU Ghostscript is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program so you can know your rights and responsibilities.
It should be in a file named doc/COPYING. If not, write to the
Free Software Foundation, Inc., 59 Temple Place Suite 330, Boston, MA
02111-1307, USA.
 */

#include "pts_fax.h"
#if 0 /* doesn't work in C++ */
#  define intern static
#  define impl static
#  define intern_const static const
#  define impl_const static const
#else
#  define intern static
#  define impl static
#  define intern_const extern const
#  define impl_const const
#endif
/* #define cxxintern extern */

#ifdef __GNUC__
#pragma implementation
#endif

/* misc_types.h by pts@fazekas.hu at Sat Jul  6 19:20:17 CEST 2002 */
#ifndef MISC_TYPES_H
#define MISC_TYPES_H 1

/* #include "config2.h" */

#define BEGIN	do {
#define END	} while (0)
#ifndef DO_NOTHING
#  define DO_NOTHING BEGIN END
#endif

#  define if_debug0(c,s) DO_NOTHING
#  define if_debug1(c,s,a1) DO_NOTHING
#  define if_debug2(c,s,a1,a2) DO_NOTHING
#  define if_debug3(c,s,a1,a2,a3) DO_NOTHING
#  define if_debug4(c,s,a1,a2,a3,a4) DO_NOTHING
#  define if_debug5(c,s,a1,a2,a3,a4,a5) DO_NOTHING
#  define if_debug6(c,s,a1,a2,a3,a4,a5,a6) DO_NOTHING
#  define if_debug7(c,s,a1,a2,a3,a4,a5,a6,a7) DO_NOTHING
#  define if_debug8(c,s,a1,a2,a3,a4,a5,a6,a7,a8) DO_NOTHING
#  define if_debug9(c,s,a1,a2,a3,a4,a5,a6,a7,a8,a9) DO_NOTHING
#  define if_debug10(c,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) DO_NOTHING
#  define if_debug11(c,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) DO_NOTHING
#  define if_debug12(c,s,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12) DO_NOTHING

/* to the one that all the compilers seem to have.... */
#ifndef min
#  define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#  define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

/* Define a standard way to round values to a (constant) modulus. */
#define ROUND_DOWN(value, modulus)\
  ( (modulus) & ((modulus) - 1) ?	/* not a power of 2 */\
    (value) - (value) % (modulus) :\
    (value) & -(modulus) )
#define ROUND_UP(value, modulus)\
  ( (modulus) & ((modulus) - 1) ?	/* not a power of 2 */\
    ((value) + ((modulus) - 1)) / (modulus) * (modulus) :\
    ((value) + ((modulus) - 1)) & -(modulus) )

#define countof(a) (sizeof(a) / sizeof((a)[0]))

#define gs_alloc_bytes(a, n, c) ss->xalloc_(n)
#define gs_free_object(a, ptr, c) ss->free_(ptr)

#endif /* gstypes.h */
/* end of former misc_types.h */

/* gsbittab.h */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* Interface to tables for bit operations */

#ifndef gsbittab_INCLUDED
#  define gsbittab_INCLUDED

/*
 * Generate tables for transforming 2, 4, 6, or 8 bits.
 */
#define btab2_(v0,v2,v1)\
  v0,v1+v0,v2+v0,v2+v1+v0
#define bit_table_2(v0,v2,v1) btab2_(v0,v2,v1)
#define btab4_(v0,v8,v4,v2,v1)\
  btab2_(v0,v2,v1), btab2_(v4+v0,v2,v1),\
  btab2_(v8+v0,v2,v1), btab2_(v8+v4+v0,v2,v1)
#define bit_table_4(v0,v8,v4,v2,v1) btab4_(v0,v8,v4,v2,v1)
#define btab6_(v0,v20,v10,v8,v4,v2,v1)\
  btab4_(v0,v8,v4,v2,v1), btab4_(v10+v0,v8,v4,v2,v1),\
  btab4_(v20+v0,v8,v4,v2,v1), btab4_(v20+v10+v0,v8,v4,v2,v1)
#define bit_table_6(v0,v20,v10,v8,v4,v2,v1) btab6_(v0,v20,v10,v8,v4,v2,v1)
#define bit_table_8(v0,v80,v40,v20,v10,v8,v4,v2,v1)\
  btab6_(v0,v20,v10,v8,v4,v2,v1), btab6_(v40+v0,v20,v10,v8,v4,v2,v1),\
  btab6_(v80+v0,v20,v10,v8,v4,v2,v1), btab6_(v80+v40+v0,v20,v10,v8,v4,v2,v1)

/*
 * byte_reverse_bits[B] = the unsigned char B with the order of bits reversed.
 */
intern_const unsigned char byte_reverse_bits[256];

/*
 * byte_right_mask[N] = a unsigned char with N trailing 1s, 0 <= N <= 8.
 */
intern_const unsigned char byte_right_mask[9];

/*
 * byte_count_bits[B] = the number of 1-bits in a unsigned char with value B.
 */
intern_const unsigned char byte_count_bits[256];

/*
 * byte_bit_run_length_N[B], for 0 <= N <= 7, gives the length of the
 * run of 1-bits starting at bit N in a unsigned char with value B,
 * numbering the bits in the unsigned char as 01234567.  If the run includes
 * the low-order bit (i.e., might be continued into a following unsigned char),
 * the run length is increased by 8.
 */
intern_const unsigned char
    byte_bit_run_length_0[256], byte_bit_run_length_1[256],
    byte_bit_run_length_2[256], byte_bit_run_length_3[256],
    byte_bit_run_length_4[256], byte_bit_run_length_5[256],
    byte_bit_run_length_6[256], byte_bit_run_length_7[256];

/*
 * byte_bit_run_length[N] points to byte_bit_run_length_N.
 * byte_bit_run_length_neg[N] = byte_bit_run_length[-N & 7].
 */
intern_const unsigned char *const byte_bit_run_length[8];
intern_const unsigned char *const byte_bit_run_length_neg[8];

/*
 * byte_acegbdfh_to_abcdefgh[acegbdfh] = abcdefgh, where the letters
 * denote the individual bits of the unsigned char.
 */
intern_const unsigned char byte_acegbdfh_to_abcdefgh[256];

#endif /* gsbittab_INCLUDED */

/* end of former gsbittab.h */

/* shc.h */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* Common definitions for filters using Huffman coding */

#ifndef shc_INCLUDED
#  define shc_INCLUDED

/* #include "gsbittab.h" */

#define hc_bits_size (SIZEOF_INT * 8)
#define s_hce_init_inline(ss)\
  ((ss)->bits = 0, (ss)->bits_left = hc_bits_size)
#define s_hcd_init_inline(ss)\
  ((ss)->bits = 0, (ss)->bits_left = 0)

/*
 * These definitions are valid for code lengths up to 16 bits
 * and non-negative decoded values up to 15 bits.
 *
 * We define 3 different representations of the code: encoding tables,
 * decoding tables, and a definition table which can be generated easily
 * from frequency information and which in turn can easily generate
 * the encoding and decoding tables.
 *
 * The definition table has two parts: a list of the number of i-bit
 * codes for each i >= 1, and the decoded values corresponding to
 * the code values in increasing lexicographic order (which will also
 * normally be decreasing code frequency).  Calling these two lists
 * L[1..M] and V[0..N-1] respectively, we have the following invariants:
 *      - 1 <= M <= max_hc_length, N >= 2.
 *      - L[0] = 0.
 *      - for i=1..M, L[i] >= 0.
 *      - sum(i=1..M: L[i]) = N.
 *      - sum(i=1..M: L[i] * 2^-i) = 1.
 *      - V[0..N-1] are a permutation of the integers 0..N-1.
 */
#define max_hc_length 16
typedef struct hc_definition_s {
    unsigned short *counts;		/* [0..M] */
    unsigned int num_counts;		/* M */
    unsigned short *values;		/* [0..N-1] */
    unsigned int num_values;		/* N */
} hc_definition;

/**** pts ****/
struct stream_hc_state_s;
/* definition moved to pts_fax.h */

/* ------ Encoding tables ------ */

/* Define the structure for the encoding tables. */
typedef struct hce_code_s {
    unsigned short code;
    unsigned short code_length;
} hce_code;

#define hce_entry(c, len) { c, len }

typedef struct hce_table_s {
    unsigned int count;
    hce_code *codes;
} hce_table;

#define hce_bits_available(n)\
  (ss->bits_left >= (n) || wlimit - q > ((n) - ss->bits_left - 1) >> 3)

/* ------ Encoding utilities ------ */

/*
 * Put a code on the output.  The client is responsible for ensuring
 * that q does not exceed pw->limit.
 */

#ifdef DEBUG
#  define hc_print_value(code, clen)\
    (gs_debug_c('W') ?\
     (dlprintf2("[W]0x%x,%d\n", code, clen), 0) : 0)
#  define hc_print_value_then(code, clen) hc_print_value(code, clen),
#else
#  define hc_print_value(code, clen) 0
#  define hc_print_value_then(code, clen)	/* */
#endif
#define hc_print_code(rp) hc_print_value((rp)->code, (rp)->code_length)

/* Declare variables that hold the encoder state. */
#define hce_declare_state\
	register unsigned int bits;\
	register int bits_left

/* Load the state from the stream. */
/* Free variables: ss, bits, bits_left. */
#define hce_load_state()\
	bits = ss->bits, bits_left = ss->bits_left

/* Store the state back in the stream. */
/* Free variables: ss, bits, bits_left. */
#define hce_store_state()\
	ss->bits = bits, ss->bits_left = bits_left

/* Put a code on the stream. */
intern void hc_put_code_proc _((bool, unsigned char *, unsigned int));

#define hc_put_value(ss, q, code, clen)\
  (hc_print_value_then(code, clen)\
   ((bits_left -= (clen)) >= 0 ?\
    (bits += (code) << bits_left) :\
    (hc_put_code_proc((ss)->FirstBitLowOrder,\
		      q += hc_bits_size >> 3,\
		      (bits + ((code) >> -bits_left))),\
     bits = (code) << (bits_left += hc_bits_size))))
#define hc_put_code(ss, q, cp)\
  hc_put_value(ss, q, (cp)->code, (cp)->code_length)

/*
 * Force out the final bits to the output.
 * Note that this does a store_state, but not a load_state.
 */
intern unsigned char *hc_put_last_bits_proc _((struct stream_hc_state_s *, unsigned char *, unsigned int, int));

#define hc_put_last_bits(ss, q)\
  hc_put_last_bits_proc(ss, q, bits, bits_left)

/* ------ Decoding tables ------ */

/*
 * Define the structure for the decoding tables.
 * First-level nodes are either leaves, which have
 *      value = decoded value
 *      code_length <= initial_bits
 * or non-leaves, which have
 *      value = the index of a sub-table
 *      code_length = initial_bits + the number of additional dispatch bits
 * Second-level nodes are always leaves, with
 *      code_length = the actual number of bits in the code - initial_bits.
 */

typedef struct hcd_code_s {
    short value;
    unsigned short code_length;
} hcd_code;

typedef struct hcd_table_s {
    unsigned int count;
    unsigned int initial_bits;
    hcd_code *codes;
} hcd_table;

/* Declare variables that hold the decoder state. */
#define hcd_declare_state\
	register const unsigned char *p;\
	const unsigned char *rlimit;\
	unsigned int bits;\
	int bits_left

/* Load the state from the stream. */
/* Free variables: pr, ss, p, rlimit, bits, bits_left. */
#define hcd_load_state()\
	p = pr->ptr,\
	rlimit = pr->limit,\
	bits = ss->bits,\
	bits_left = ss->bits_left

/* Store the state back in the stream. */
/* Put back any complete bytes into the input buffer. */
/* Free variables: pr, ss, p, bits, bits_left. */
#define hcd_store_state()\
	pr->ptr = p -= (bits_left >> 3),\
	ss->bits = bits >>= (bits_left & ~7),\
	ss->bits_left = bits_left &= 7

/* Macros to get blocks of bits from the input stream. */
/* Invariants: 0 <= bits_left <= bits_size; */
/* bits [bits_left-1..0] contain valid data. */

#define hcd_bits_available(n)\
  (bits_left >= (n) || rlimit - p > ((n) - bits_left - 1) >> 3)
/* For hcd_ensure_bits, n must not be greater than 8. */
#define HCD_ENSURE_BITS_ELSE(n)\
  if (bits_left >= n)\
    DO_NOTHING;\
  else HCD_MORE_BITS_ELSE
#define hcd_ensure_bits(n, outl)\
  BEGIN HCD_ENSURE_BITS_ELSE(n) goto outl; END

/* Load more bits into the buffer. */
#define HCD_MORE_BITS_1_ELSE\
  if (p < rlimit) {\
    int c = *++p;\
\
    if (ss->FirstBitLowOrder)\
      c = byte_reverse_bits[c];\
    bits = (bits << 8) + c, bits_left += 8;\
  } else
#if hc_bits_size == 16
#  define HCD_MORE_BITS_ELSE HCD_MORE_BITS_1_ELSE
#else /* hc_bits_size >= 32 */
#  define HCD_MORE_BITS_ELSE\
  if (rlimit - p >= 3) {\
    if (ss->FirstBitLowOrder)\
      bits = (bits << 24) + ((unsigned int)byte_reverse_bits[p[1]] << 16) + ((unsigned int)byte_reverse_bits[p[2]] << 8) + byte_reverse_bits[p[3]];\
    else\
      bits = (bits << 24) + ((unsigned int)p[1] << 16) + ((unsigned int)p[2] << 8) + p[3];\
    bits_left += 24, p += 3;\
  } else HCD_MORE_BITS_1_ELSE
#endif
#define hcd_more_bits(outl)\
  BEGIN HCD_MORE_BITS_ELSE goto outl; END

#define hcd_peek_bits(n) ((bits >> (bits_left - (n))) & ((1 << (n)) - 1))

/* hcd_peek_var_bits requires bits_left <= 8. */
#define hcd_peek_var_bits(n)\
  ((bits >> (bits_left - (n))) & byte_right_mask[n])

/* hcd_peek_bits_left requires bits_left <= 8. */
#define hcd_peek_bits_left()\
  (bits & byte_right_mask[bits_left])

#define hcd_skip_bits(n) (bits_left -= (n))

#endif /* shc_INCLUDED */

/* end of former shc.h */

/* scf.h */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* Common definitions for CCITTFax encoding and decoding filters */

#ifndef scf_INCLUDED
#  define scf_INCLUDED

/* #include "shc.h" */

/*
 * The CCITT Group 3 (T.4) and Group 4 (T.6) fax specifications map
 * run lengths to Huffman codes.  White and black have different mappings.
 * If the run length is 64 or greater, two or more codes are needed:
 *      - One or more 'make-up' codes for 2560 pixels;
 *      - A 'make-up' code that encodes the multiple of 64;
 *      - A 'termination' code for the remainder.
 * For runs of 63 or less, only the 'termination' code is needed.
 */

/* ------ Encoding tables ------ */

/*
 * The maximum possible length of a scan line is determined by the
 * requirement that 3 runs have to fit into the stream buffer.
 * A run of length N requires approximately ceil(N / 2560) makeup codes,
 * hence 1.5 * ceil(N / 2560) bytes.  Taking the largest safe stream
 * buffer size as 32K, we arrive at the following maximum width:
 */
#if SIZEOF_INT > 2
#  define cfe_max_width (2560 * 32000 * 2 / 3)
#else
#  define cfe_max_width (max_int - 40)	/* avoid overflows */
#endif
/* The +5 in cfe_max_code_bytes is a little conservative. */
#define cfe_max_code_bytes(width) ((width) / 2560 * 3 / 2 + 5)

typedef hce_code cfe_run;

/* Codes common to 1-D and 2-D encoding. */
/* The decoding algorithms know that EOL is 0....01. */
#define run_eol_code_length 12
#define run_eol_code_value 1
intern_const cfe_run cf_run_eol;
typedef struct cf_runs_s {
    cfe_run termination[64];
    cfe_run make_up[41];
} cf_runs;
intern_const cf_runs
      cf_white_runs, cf_black_runs;
intern_const cfe_run cf_uncompressed[6];
intern_const cfe_run cf_uncompressed_exit[10];	/* indexed by 2 x length of */

			/* white run + (1 if next run black, 0 if white) */
/* 1-D encoding. */
intern_const cfe_run cf1_run_uncompressed;

/* 2-D encoding. */
intern_const cfe_run cf2_run_pass;

#define cf2_run_pass_length 4
#define cf2_run_pass_value 0x1
#define cf2_run_vertical_offset 3
intern_const cfe_run cf2_run_vertical[7];	/* indexed by b1 - a1 + offset */
intern_const cfe_run cf2_run_horizontal;

#define cf2_run_horizontal_value 1
#define cf2_run_horizontal_length 3
intern_const cfe_run cf2_run_uncompressed;

/* 2-D Group 3 encoding. */
intern_const cfe_run cf2_run_eol_1d;
intern_const cfe_run cf2_run_eol_2d;

/* ------ Decoding tables ------ */

typedef hcd_code cfd_node;

#define run_length value

/*
 * The value in the decoding tables is either a white or black run length,
 * or a (negative) exceptional value.
 */
#define run_error (-1)
#define run_zeros (-2)	/* EOL follows, possibly with more padding first */
#define run_uncompressed (-3)
/* 2-D codes */
#define run2_pass (-4)
#define run2_horizontal (-5)

#define cfd_white_initial_bits 8
#define cfd_white_min_bits 4	/* shortest white run */
/* intern_const cfd_node cf_white_decode[]; */

#define cfd_black_initial_bits 7
#define cfd_black_min_bits 2	/* shortest black run */
/* intern_const cfd_node cf_black_decode[]; */

#define cfd_2d_initial_bits 7
#define cfd_2d_min_bits 4	/* shortest non-H/V 2-D run */
/* intern_const cfd_node cf_2d_decode[]; */

#define cfd_uncompressed_initial_bits 6		/* must be 6 */
/* intern_const cfd_node cf_uncompressed_decode[]; */

/* ------ Run detection macros ------ */

/*
 * For the run detection macros:
 *   white_byte is 0 or 0xff for BlackIs1 or !BlackIs1 respectively;
 *   data holds p[-1], inverted if !BlackIs1;
 *   count is the number of valid bits remaining in the scan line.
 */

/* Aliases for bit processing tables. */
#define cf_byte_run_length byte_bit_run_length_neg
#define cf_byte_run_length_0 byte_bit_run_length_0

/* Skip over white pixels to find the next black pixel in the input. */
/* Store the run length in rlen, and update data, p, and count. */
/* There are many more white pixels in typical input than black pixels, */
/* and the runs of white pixels tend to be much longer, so we use */
/* substantially different loops for the two cases. */

#define skip_white_pixels(data, p, count, white_byte, rlen)\
BEGIN\
    rlen = cf_byte_run_length[count & 7][data ^ 0xff];\
    if ( rlen >= 8 ) {		/* run extends past unsigned char boundary */\
	if ( white_byte == 0 ) {\
	    if ( p[0] ) { data = p[0]; p += 1; rlen -= 8; }\
	    else if ( p[1] ) { data = p[1]; p += 2; }\
	    else {\
		while ( !(p[2] | p[3] | p[4] | p[5]) )\
		    p += 4, rlen += 32;\
		if ( p[2] ) {\
		    data = p[2]; p += 3; rlen += 8;\
		} else if ( p[3] ) {\
		    data = p[3]; p += 4; rlen += 16;\
		} else if ( p[4] ) {\
		    data = p[4]; p += 5; rlen += 24;\
		} else /* p[5] */ {\
		    data = p[5]; p += 6; rlen += 32;\
		}\
	    }\
	} else {\
	    if ( p[0] != 0xff ) { data = (unsigned char)~p[0]; p += 1; rlen -= 8; }\
	    else if ( p[1] != 0xff ) { data = (unsigned char)~p[1]; p += 2; }\
	    else {\
		while ( (p[2] & p[3] & p[4] & p[5]) == 0xff )\
		    p += 4, rlen += 32;\
		if ( p[2] != 0xff ) {\
		    data = (unsigned char)~p[2]; p += 3; rlen += 8;\
		} else if ( p[3] != 0xff ) {\
		    data = (unsigned char)~p[3]; p += 4; rlen += 16;\
		} else if ( p[4] != 0xff ) {\
		    data = (unsigned char)~p[4]; p += 5; rlen += 24;\
		} else /* p[5] != 0xff */ {\
		    data = (unsigned char)~p[5]; p += 6; rlen += 32;\
		}\
	    }\
	}\
	rlen += cf_byte_run_length_0[data ^ 0xff];\
    }\
    count -= rlen;\
END

/* Skip over black pixels to find the next white pixel in the input. */
/* Store the run length in rlen, and update data, p, and count. */

#define skip_black_pixels(data, p, count, white_byte, rlen)\
BEGIN\
    rlen = cf_byte_run_length[count & 7][data];\
    if ( rlen >= 8 ) {\
	if ( white_byte == 0 )\
	    for ( ; ; p += 4, rlen += 32 ) {\
		if ( p[0] != 0xff ) { data = p[0]; p += 1; rlen -= 8; break; }\
		if ( p[1] != 0xff ) { data = p[1]; p += 2; break; }\
		if ( p[2] != 0xff ) { data = p[2]; p += 3; rlen += 8; break; }\
		if ( p[3] != 0xff ) { data = p[3]; p += 4; rlen += 16; break; }\
	    }\
	else\
	    for ( ; ; p += 4, rlen += 32 ) {\
		if ( p[0] ) { data = (unsigned char)~p[0]; p += 1; rlen -= 8; break; }\
		if ( p[1] ) { data = (unsigned char)~p[1]; p += 2; break; }\
		if ( p[2] ) { data = (unsigned char)~p[2]; p += 3; rlen += 8; break; }\
		if ( p[3] ) { data = (unsigned char)~p[3]; p += 4; rlen += 16; break; }\
	    }\
	rlen += cf_byte_run_length_0[data];\
    }\
    count -= rlen;\
END

#endif /* scf_INCLUDED */

/* end of former scf.h */

#if USE_BUILTIN_FAXE
#if OBJDEP
#  warning PROVIDES: pts_faxe
#endif
/* scfe.c */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* CCITTFax encoding filter */
/* #include "config2.h" */
/* #include "scf.h" */
/* #include "scfx.h" */

/* ------ Macros and support routines ------ */

/* Statistics */

#ifdef DEBUG

typedef struct stats_runs_s {
    unsigned long termination[64];
    unsigned long make_up[41];
} stats_runs_t;
static stats_runs_t stats_white_runs, stats_black_runs;

#define COUNT_RUN(tab, i) (tab)[i]++;

static void
print_run_stats(const stats_runs_t * stats)
{
    int i;
    unsigned long total;

    for (i = 0, total = 0; i < 41; i++)
	dprintf1(" %lu", stats->make_up[i]),
	    total += stats->make_up[i];
    dprintf1(" total=%lu\n\t", total);
    for (i = 0, total = 0; i < 64; i++)
	dprintf1(" %lu", stats->termination[i]),
	    total += stats->termination[i];
    dprintf1(" total=%lu\n", total);
}

#else /* !DEBUG */

#define COUNT_RUN(cnt, i) DO_NOTHING

#endif /* DEBUG */

/* Put a run onto the output stream. */
/* Free variables: q, bits, bits_left. */

#define CF_PUT_RUN(ss, lenv, rt, stats)\
BEGIN\
    cfe_run rr;\
\
    if ( lenv >= 64 ) {\
	hce_store_state();\
	q = cf_put_long_run(ss, q, lenv, &rt);\
	hce_load_state();\
	lenv &= 63;\
    }\
    rr = rt.termination[lenv];\
    COUNT_RUN(stats.termination, lenv);\
    hc_put_value(ss, q, rr.code, rr.code_length);\
END

static unsigned char *
cf_put_long_run(stream_CFE_state * ss, unsigned char * q, int lenv, const cf_runs * prt)
{
    hce_declare_state;
    cfe_run rr;

#ifdef DEBUG
    stats_runs_t *pstats =
    (prt == &cf_white_runs ? &stats_white_runs : &stats_black_runs);

#endif

    hce_load_state();
    while (lenv >= 2560 + 64) {
	rr = prt->make_up[40];
	COUNT_RUN(pstats->make_up, 40);
	hc_put_value(ss, q, rr.code, rr.code_length);
	lenv -= 2560;
    }
    rr = prt->make_up[lenv >> 6];
    COUNT_RUN(pstats->make_up, lenv >> 6);
    hc_put_value(ss, q, rr.code, rr.code_length);
    hce_store_state();
    return q;
}

#define CF_PUT_WHITE_RUN(ss, lenv)\
  CF_PUT_RUN(ss, lenv, cf_white_runs, stats_white_runs)

#define CF_PUT_BLACK_RUN(ss, lenv)\
  CF_PUT_RUN(ss, lenv, cf_black_runs, stats_black_runs)

/* ------ CCITTFaxEncode ------ */

/* private_st_CFE_state(); */

static void s_CFE_release _((stream_state *));

/* Set default parameter values. */
static void
s_CFE_set_defaults(register stream_state * st)
{
    stream_CFE_state *const ss = (stream_CFE_state *) st;

    s_CFE_set_defaults_inline(ss);
}

/* Initialize CCITTFaxEncode filter */
static int
s_CFE_init(register stream_state * st)
{
    stream_CFE_state *const ss = (stream_CFE_state *) st;
    int columns = ss->Columns;

    /*
     * The worst case for encoding is alternating white and black pixels.
     * For 1-D encoding, the worst case is 9 bits per 2 pixels; for 2-D
     * (horizontal), 12 bits per 2 pixels.  To fill out a scan line,
     * we may add up to 6 12-bit EOL codes.
     */
    /**** pts: added UL ****/
    int code_bytes =
    ((columns * (ss->K == 0 ? 9UL : 12UL)) >> 4) + 20;	/* add slop */
    int raster = ss->raster =
	ROUND_UP((columns + 7) >> 3, ss->DecodedByteAlign);

    s_hce_init_inline(ss);
    ss->lbuf = ss->lprev = ss->lcode = 0;	/* in case we have to release */
    if (columns > cfe_max_width)
	return PTSFAX_ERRC;
/****** WRONG ******/
    /* Because skip_white_pixels can look as many as 4 bytes ahead, */
    /* we need to allow 4 extra bytes at the end of the row buffers. */
    ss->lbuf = (unsigned char*)gs_alloc_bytes(st->memory, raster + 4, "CFE lbuf");
    ss->lcode = (unsigned char*)gs_alloc_bytes(st->memory, code_bytes, "CFE lcode");
    if (ss->lbuf == 0 || ss->lcode == 0) {
	s_CFE_release(st);
	return PTSFAX_ERRC;
/****** WRONG ******/
    }
    if (ss->K != 0) {
	ss->lprev = (unsigned char*)gs_alloc_bytes(st->memory, raster + 4, "CFE lprev");
	if (ss->lprev == 0) {
	    s_CFE_release(st);
	    return PTSFAX_ERRC;
/****** WRONG ******/
	}
	/* Clear the initial reference line for 2-D encoding. */
	/* Make sure it is terminated properly. */
	ss->memset_(ss->lprev, (ss->BlackIs1 ? 0 : 0xff), raster);
	if (columns & 7)
	    ss->lprev[raster - 1] ^= 0x80 >> (columns & 7);
	else
	    ss->lprev[raster] = ~ss->lprev[0];
    }
    ss->read_count = raster;
    ss->write_count = 0;
    ss->k_left = (ss->K > 0 ? 1 : ss->K);
    ss->max_code_bytes = code_bytes;
    return 0;
}

/* Release the filter. */
static void
s_CFE_release(stream_state * st)
{
    stream_CFE_state *const ss = (stream_CFE_state *) st;

    gs_free_object(st->memory, ss->lprev, "CFE lprev(close)");
    gs_free_object(st->memory, ss->lcode, "CFE lcode(close)");
    gs_free_object(st->memory, ss->lbuf, "CFE lbuf(close)");
}

/* Flush the buffer */
static void cf_encode_1d _((stream_CFE_state *, const unsigned char *,
			     stream_cursor_write *));
static void cf_encode_2d _((stream_CFE_state *, const unsigned char *,
			     stream_cursor_write *, const unsigned char *));
static int
s_CFE_process(stream_state * st, stream_cursor_read * pr,
	      stream_cursor_write * pw, bool last)
{
    stream_CFE_state *const ss = (stream_CFE_state *) st;
    const unsigned char *rlimit = pr->limit;
    unsigned char *wlimit = pw->limit;
    int raster = ss->raster;
    unsigned char end_mask = 1 << (-ss->Columns & 7);
    int status = 0;

    for (;;) {
	stream_cursor_write w;

	if_debug2('w', "[w]CFE: read_count = %d, write_count=%d,\n",
		  ss->read_count, ss->write_count);
	if_debug6('w', "    pr = 0x%lx(%d)0x%lx, pw = 0x%lx(%d)0x%lx\n",
		  (unsigned long) pr->ptr, (int)(rlimit - pr->ptr), (unsigned long) rlimit,
		  (unsigned long) pw->ptr, (int)(wlimit - pw->ptr), (unsigned long) wlimit);
	if (ss->write_count) {
	    /* Copy more of an encoded line to the caller. */
	    int wcount = wlimit - pw->ptr;
	    int ccount = min(wcount, ss->write_count);

	    ss->memcpy_(pw->ptr + 1, ss->lcode + ss->code_bytes - ss->write_count,
		   ccount);
	    pw->ptr += ccount;
	    if ((ss->write_count -= ccount) > 0) {
		status = 1;
		break;
	    }
	}
	if (ss->read_count) {
	    /* Copy more of an unencoded line from the caller. */
	    int rcount = rlimit - pr->ptr;
	    int ccount = min(rcount, ss->read_count);

	    if (rcount == 0 && last)
		break;
	    ss->memcpy_(ss->lbuf + raster - ss->read_count,
		   pr->ptr + 1, ccount);
	    pr->ptr += ccount;
	    if ((ss->read_count -= ccount) != 0)
		break;
	}
	/*
	 * We have a full scan line in lbuf.  Ensure that it ends with
	 * two polarity changes.
	 */
	{
	    unsigned char *end = ss->lbuf + raster - 1;
	    unsigned char end_bit = *end & end_mask;
	    unsigned char not_bit = end_bit ^ end_mask;

	    *end &= -end_mask;
	    if (end_mask == 1)
		end[1] = (end_bit ? 0x40 : 0x80);
	    else if (end_mask == 2)
		*end |= not_bit >> 1, end[1] = end_bit << 7;
	    else
		*end |= (not_bit >> 1) | (end_bit >> 2);
	}
	/*
	 * Write the output directly to the caller's buffer if it's large
	 * enough, otherwise to our own buffer.
	 */
	if (wlimit - pw->ptr >= ss->max_code_bytes) {
	    w = *pw;
	} else {
	    w.ptr = ss->lcode - 1;
	    w.limit = w.ptr + ss->max_code_bytes;
	}
#ifdef DEBUG
	if (ss->K > 0) {
	    if_debug1('w', "[w]new row, k_left=%d\n",
		      ss->k_left);
	} else {
	    if_debug0('w', "[w]new row\n");
	}
#endif
	/*
	 * Write an EOL (actually a "beginning of line") if requested.
	 */
	if (ss->EndOfLine) {
	    const cfe_run *rp =
	    (ss->K <= 0 ? &cf_run_eol :
	     ss->k_left > 1 ? &cf2_run_eol_2d :
	     &cf2_run_eol_1d);
	    cfe_run run;

	    hce_declare_state;

	    hce_load_state();
	    if (ss->EncodedByteAlign) {
		run = *rp;
		/* Pad the run on the left */
		/* so it winds up unsigned char-aligned. */
		run.code_length +=
		    (bits_left - run_eol_code_length) & 7;
		if (run.code_length > 16)	/* <= 23 */
		    bits_left -= run.code_length & 7,
			run.code_length = 16;
		rp = &run;
	    }
	    hc_put_code(ss, w.ptr, rp);
	    hce_store_state();
	} else if (ss->EncodedByteAlign)
	    ss->bits_left &= ~7;
	/* Encode the line. */
	if (ss->K == 0)
	    cf_encode_1d(ss, ss->lbuf, &w);	/* pure 1-D */
	else if (ss->K < 0)
	    cf_encode_2d(ss, ss->lbuf, &w, ss->lprev);	/* pure 2-D */
	else if (--(ss->k_left))	/* mixed, use 2-D */
	    cf_encode_2d(ss, ss->lbuf, &w, ss->lprev);
	else {			/* mixed, use 1-D */
	    cf_encode_1d(ss, ss->lbuf, &w);
	    ss->k_left = ss->K;
	}
	/*
	 * If we didn't write directly to the client's buffer, schedule
	 * the output data to be written.
	 */
	if (w.limit == wlimit)
	    pw->ptr = w.ptr;
	else
	    ss->write_count = ss->code_bytes = w.ptr - (ss->lcode - 1);
	if (ss->K != 0) {
	    /* In 2-D modes, swap the current and previous scan lines. */
	    unsigned char *temp = ss->lbuf;

	    ss->lbuf = ss->lprev;
	    ss->lprev = temp;
	}
	/* Note that the input buffer needs refilling. */
	ss->read_count = raster;
    }
    /*
     * When we exit from the loop, we know that write_count = 0, and
     * there is no line waiting to be processed in the input buffer.
     */
    if (last && status == 0) {
	const cfe_run *rp =
	(ss->K > 0 ? &cf2_run_eol_1d : &cf_run_eol);
	int i = (!ss->EndOfBlock ? 0 : ss->K < 0 ? 2 : 6);
	unsigned int bits_to_write =
	hc_bits_size - ss->bits_left + i * rp->code_length;
	unsigned char *q = pw->ptr;

	hce_declare_state;

	if (0U+(wlimit - q) < (bits_to_write + 7) >> 3) { /* PTS_UNSIGNED */
	    status = 1;
	    goto out;
	}
	hce_load_state();
	if (ss->EncodedByteAlign)
	    bits_left &= ~7;
	while (--i >= 0)
	    hc_put_code(ss, q, rp);
	/* Force out the last unsigned char or bytes. */
	pw->ptr = hc_put_last_bits((stream_hc_state *) ss, q);
    }
  out:
    if_debug9('w', "[w]CFE exit %d: read_count = %d, write_count = %d,\n     pr = 0x%lx(%d)0x%lx; pw = 0x%lx(%d)0x%lx\n",
	      status, ss->read_count, ss->write_count,
	      (unsigned long) pr->ptr, (int)(rlimit - pr->ptr), (unsigned long) rlimit,
	      (unsigned long) pw->ptr, (int)(wlimit - pw->ptr), (unsigned long) wlimit);
#ifdef DEBUG
    if (pr->ptr > rlimit || pw->ptr > wlimit) {
	lprintf("Pointer overrun!\n");
	status = PTSFAX_ERRC;
    }
    if (gs_debug_c('w') && status == 1) {
	dlputs("[w]white runs:");
	print_run_stats(&stats_white_runs);
	dlputs("[w]black runs:");
	print_run_stats(&stats_black_runs);
    }
#endif
    return status;
}

/* Encode a 1-D scan line. */
static void
cf_encode_1d(stream_CFE_state * ss, const unsigned char * lbuf, stream_cursor_write * pw)
{
    unsigned int count = ss->raster << 3;
    unsigned char *q = pw->ptr;
    int end_count = -ss->Columns & 7;
    int rlen;

    hce_declare_state;
    const unsigned char *p = lbuf;
    unsigned char invert = (ss->BlackIs1 ? 0 : 0xff);

    /* Invariant: data = p[-1] ^ invert. */
    unsigned int data = *p++ ^ invert;

    hce_load_state();
    while (count != 0U+end_count) { /* PTS_UNSIGNED */
	/* Parse a white run. */
	skip_white_pixels(data, p, count, invert, rlen);
	CF_PUT_WHITE_RUN(ss, rlen);
	if (count == 0U+end_count) /* PTS_UNSIGNED */
	    break;
	/* Parse a black run. */
	skip_black_pixels(data, p, count, invert, rlen);
	CF_PUT_BLACK_RUN(ss, rlen);
    }
    hce_store_state();
    pw->ptr = q;
}

/* Encode a 2-D scan line. */
static void
cf_encode_2d(stream_CFE_state * ss, const unsigned char * lbuf, stream_cursor_write * pw,
	     const unsigned char * lprev)
{
    unsigned char invert_white = (ss->BlackIs1 ? 0 : 0xff);
    unsigned char invert = invert_white;
    unsigned int count = ss->raster << 3;
    int end_count = -ss->Columns & 7;
    const unsigned char *p = lbuf;
    unsigned char *q = pw->ptr;
    unsigned int data = *p++ ^ invert;

    hce_declare_state;
    /*
     * In order to handle the nominal 'changing white' at the beginning of
     * each scan line, we need to suppress the test for an initial black bit
     * in the reference line when we are at the very beginning of the scan
     * line.  To avoid an extra test, we use two different mask tables.
     */
    static const unsigned char initial_count_bit[8] =
    {
	0, 1, 2, 4, 8, 0x10, 0x20, 0x40
    };
    static const unsigned char further_count_bit[8] =
    {
	0x80, 1, 2, 4, 8, 0x10, 0x20, 0x40
    };
    const unsigned char *count_bit = initial_count_bit;

    hce_load_state();
    while (count != 0U+end_count) { /* PTS_UNSIGNED */
	/*
	 * If invert == invert_white, white and black have their
	 * correct meanings; if invert == ~invert_white,
	 * black and white are interchanged.
	 */
	unsigned int a0 = count;
	unsigned int a1;

#define b1 (a1 - diff)		/* only for printing */
	int diff;
	unsigned int prev_count = count;
	const unsigned char *prev_p = p - lbuf + lprev;
	unsigned char prev_data = prev_p[-1] ^ invert;
	int rlen;

	/* Find the a1 and b1 transitions. */
	skip_white_pixels(data, p, count, invert, rlen);
	a1 = count;
	if ((prev_data & count_bit[prev_count & 7])) {
	    /* Look for changing white first. */
	    skip_black_pixels(prev_data, prev_p, prev_count, invert, rlen);
	}
	count_bit = further_count_bit;	/* no longer at beginning */
      pass:
	if (prev_count != 0U+end_count) /* PTS_UNSIGNED */
	    skip_white_pixels(prev_data, prev_p, prev_count, invert, rlen);
	diff = a1 - prev_count;	/* i.e., logical b1 - a1 */
	/* In all the comparisons below, remember that count */
	/* runs downward, not upward, so the comparisons are */
	/* reversed. */
	if (diff <= -2) {
	    /* Could be a pass mode.  Find b2. */
	    if (prev_count != 0U+ end_count) /* PTS_UNSIGNED */
		skip_black_pixels(prev_data, prev_p,
				  prev_count, invert, rlen);
	    if (prev_count > a1) {
		/* Use pass mode. */
		if_debug4('W', "[W]pass: count = %d, a1 = %d, b1 = %d, new count = %d\n",
			  a0, a1, b1, prev_count);
		hc_put_value(ss, q, cf2_run_pass_value, cf2_run_pass_length);
		a0 = prev_count;
		goto pass;
	    }
	}
	/* Check for vertical coding. */
	if (diff <= 3 && diff >= -3) {
	    /* Use vertical coding. */
	    const cfe_run *cp = &cf2_run_vertical[diff + 3];

	    if_debug5('W', "[W]vertical %d: count = %d, a1 = %d, b1 = %d, new count = %d\n",
		      diff, a0, a1, b1, count);
	    hc_put_code(ss, q, cp);
	    invert = ~invert;	/* a1 polarity changes */
	    data ^= 0xff;
	    continue;
	}
	/* No luck, use horizontal coding. */
	if (count != 0U+end_count) /* PTS_UNSIGNED */
	    skip_black_pixels(data, p, count, invert, rlen);	/* find a2 */
	hc_put_value(ss, q, cf2_run_horizontal_value,
		     cf2_run_horizontal_length);
	a0 -= a1;
	a1 -= count;
	if (invert == invert_white) {
	    if_debug3('W', "[W]horizontal: white = %d, black = %d, new count = %d\n",
		      a0, a1, count);
	    CF_PUT_WHITE_RUN(ss, a0);
	    CF_PUT_BLACK_RUN(ss, a1);
	} else {
	    if_debug3('W', "[W]horizontal: black = %d, white = %d, new count = %d\n",
		      a0, a1, count);
	    CF_PUT_BLACK_RUN(ss, a0);
	    CF_PUT_WHITE_RUN(ss, a1);
#undef b1
	}
    }
    hce_store_state();
    pw->ptr = q;
}

/* Stream template */
const stream_template s_CFE_template = {
    /*0, &st_CFE_state*/ s_CFE_init, s_CFE_process, 1, 1,
    s_CFE_release, s_CFE_set_defaults, 0
};
/* end of former scfe.c */

/* scfetab.c */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* Tables for CCITTFaxEncode filter */
/* #include "scommon.h" */		/* for scf.h */
/* #include "scf.h" */

/* We make this a separate file so that it can be used by */
/* the program that generates the tables for the CCITTFaxDecode filter. */

/* ------ Run encoding tables ------ */

/* Abbreviate hce_entry to make the file smaller. */
#define RUN(c,len) hce_entry(c,len)

/* Define the end-of-line code. */
/* Code in scfd.c and scfdgen.c knows that the run value is 1. */
impl_const cfe_run cf_run_eol =
RUN(run_eol_code_value, run_eol_code_length);

/* Define the 1-D code that signals uncompressed data. */
impl_const cfe_run cf1_run_uncompressed =
RUN(0xf, 12);

/* Define the 2-D run codes. */
impl_const cfe_run cf2_run_pass =
RUN(cf2_run_pass_value, cf2_run_pass_length);
impl_const cfe_run cf2_run_vertical[7] =
{
    RUN(0x3, 7),
    RUN(0x3, 6),
    RUN(0x3, 3),
    RUN(0x1, 1),
    RUN(0x2, 3),
    RUN(0x2, 6),
    RUN(0x2, 7)
};
impl_const cfe_run cf2_run_horizontal =
RUN(cf2_run_horizontal_value, cf2_run_horizontal_length);
impl_const cfe_run cf2_run_uncompressed =
RUN(0xf, 10);

/* EOL codes for Group 3 2-D. */
/* Code in scfd.c knows that these are 0...01x. */
impl_const cfe_run cf2_run_eol_1d =
RUN((run_eol_code_value << 1) + 1, run_eol_code_length + 1);
impl_const cfe_run cf2_run_eol_2d =
RUN((run_eol_code_value << 1) + 0, run_eol_code_length + 1);

/* White run codes. */
impl_const cf_runs cf_white_runs =
{
    {				/* Termination codes */
	RUN(0x35, 8), RUN(0x7, 6), RUN(0x7, 4), RUN(0x8, 4),
	RUN(0xb, 4), RUN(0xc, 4), RUN(0xe, 4), RUN(0xf, 4),
	RUN(0x13, 5), RUN(0x14, 5), RUN(0x7, 5), RUN(0x8, 5),
	RUN(0x8, 6), RUN(0x3, 6), RUN(0x34, 6), RUN(0x35, 6),
	RUN(0x2a, 6), RUN(0x2b, 6), RUN(0x27, 7), RUN(0xc, 7),
	RUN(0x8, 7), RUN(0x17, 7), RUN(0x3, 7), RUN(0x4, 7),
	RUN(0x28, 7), RUN(0x2b, 7), RUN(0x13, 7), RUN(0x24, 7),
	RUN(0x18, 7), RUN(0x2, 8), RUN(0x3, 8), RUN(0x1a, 8),
	RUN(0x1b, 8), RUN(0x12, 8), RUN(0x13, 8), RUN(0x14, 8),
	RUN(0x15, 8), RUN(0x16, 8), RUN(0x17, 8), RUN(0x28, 8),
	RUN(0x29, 8), RUN(0x2a, 8), RUN(0x2b, 8), RUN(0x2c, 8),
	RUN(0x2d, 8), RUN(0x4, 8), RUN(0x5, 8), RUN(0xa, 8),
	RUN(0xb, 8), RUN(0x52, 8), RUN(0x53, 8), RUN(0x54, 8),
	RUN(0x55, 8), RUN(0x24, 8), RUN(0x25, 8), RUN(0x58, 8),
	RUN(0x59, 8), RUN(0x5a, 8), RUN(0x5b, 8), RUN(0x4a, 8),
	RUN(0x4b, 8), RUN(0x32, 8), RUN(0x33, 8), RUN(0x34, 8)
    },
    {				/* Make-up codes */
	RUN(0, 0) /* dummy */ , RUN(0x1b, 5), RUN(0x12, 5), RUN(0x17, 6),
	RUN(0x37, 7), RUN(0x36, 8), RUN(0x37, 8), RUN(0x64, 8),
	RUN(0x65, 8), RUN(0x68, 8), RUN(0x67, 8), RUN(0xcc, 9),
	RUN(0xcd, 9), RUN(0xd2, 9), RUN(0xd3, 9), RUN(0xd4, 9),
	RUN(0xd5, 9), RUN(0xd6, 9), RUN(0xd7, 9), RUN(0xd8, 9),
	RUN(0xd9, 9), RUN(0xda, 9), RUN(0xdb, 9), RUN(0x98, 9),
	RUN(0x99, 9), RUN(0x9a, 9), RUN(0x18, 6), RUN(0x9b, 9),
	RUN(0x8, 11), RUN(0xc, 11), RUN(0xd, 11), RUN(0x12, 12),
	RUN(0x13, 12), RUN(0x14, 12), RUN(0x15, 12), RUN(0x16, 12),
	RUN(0x17, 12), RUN(0x1c, 12), RUN(0x1d, 12), RUN(0x1e, 12),
	RUN(0x1f, 12)
    }
};

/* Black run codes. */
impl_const cf_runs cf_black_runs =
{
    {				/* Termination codes */
	RUN(0x37, 10), RUN(0x2, 3), RUN(0x3, 2), RUN(0x2, 2),
	RUN(0x3, 3), RUN(0x3, 4), RUN(0x2, 4), RUN(0x3, 5),
	RUN(0x5, 6), RUN(0x4, 6), RUN(0x4, 7), RUN(0x5, 7),
	RUN(0x7, 7), RUN(0x4, 8), RUN(0x7, 8), RUN(0x18, 9),
	RUN(0x17, 10), RUN(0x18, 10), RUN(0x8, 10), RUN(0x67, 11),
	RUN(0x68, 11), RUN(0x6c, 11), RUN(0x37, 11), RUN(0x28, 11),
	RUN(0x17, 11), RUN(0x18, 11), RUN(0xca, 12), RUN(0xcb, 12),
	RUN(0xcc, 12), RUN(0xcd, 12), RUN(0x68, 12), RUN(0x69, 12),
	RUN(0x6a, 12), RUN(0x6b, 12), RUN(0xd2, 12), RUN(0xd3, 12),
	RUN(0xd4, 12), RUN(0xd5, 12), RUN(0xd6, 12), RUN(0xd7, 12),
	RUN(0x6c, 12), RUN(0x6d, 12), RUN(0xda, 12), RUN(0xdb, 12),
	RUN(0x54, 12), RUN(0x55, 12), RUN(0x56, 12), RUN(0x57, 12),
	RUN(0x64, 12), RUN(0x65, 12), RUN(0x52, 12), RUN(0x53, 12),
	RUN(0x24, 12), RUN(0x37, 12), RUN(0x38, 12), RUN(0x27, 12),
	RUN(0x28, 12), RUN(0x58, 12), RUN(0x59, 12), RUN(0x2b, 12),
	RUN(0x2c, 12), RUN(0x5a, 12), RUN(0x66, 12), RUN(0x67, 12)
    },
    {				/* Make-up codes. */
	RUN(0, 0) /* dummy */ , RUN(0xf, 10), RUN(0xc8, 12), RUN(0xc9, 12),
	RUN(0x5b, 12), RUN(0x33, 12), RUN(0x34, 12), RUN(0x35, 12),
	RUN(0x6c, 13), RUN(0x6d, 13), RUN(0x4a, 13), RUN(0x4b, 13),
	RUN(0x4c, 13), RUN(0x4d, 13), RUN(0x72, 13), RUN(0x73, 13),
	RUN(0x74, 13), RUN(0x75, 13), RUN(0x76, 13), RUN(0x77, 13),
	RUN(0x52, 13), RUN(0x53, 13), RUN(0x54, 13), RUN(0x55, 13),
	RUN(0x5a, 13), RUN(0x5b, 13), RUN(0x64, 13), RUN(0x65, 13),
	RUN(0x8, 11), RUN(0xc, 11), RUN(0xd, 11), RUN(0x12, 12),
	RUN(0x13, 12), RUN(0x14, 12), RUN(0x15, 12), RUN(0x16, 12),
	RUN(0x17, 12), RUN(0x1c, 12), RUN(0x1d, 12), RUN(0x1e, 12),
	RUN(0x1f, 12)
    }
};

/* Uncompressed codes. */
impl_const cfe_run cf_uncompressed[6] =
{
    RUN(1, 1),
    RUN(1, 2),
    RUN(1, 3),
    RUN(1, 4),
    RUN(1, 5),
    RUN(1, 6)
};

/* Uncompressed exit codes. */
impl_const cfe_run cf_uncompressed_exit[10] =
{
    RUN(2, 8), RUN(3, 8),
    RUN(2, 9), RUN(3, 9),
    RUN(2, 10), RUN(3, 10),
    RUN(2, 11), RUN(3, 11),
    RUN(2, 12), RUN(3, 12)
};

#if 0
/* Some C compilers insist on having executable code in every file.... */
void scfetab_dummy _((void));	/* for picky compilers */
void
scfetab_dummy(void)
{
}
#endif

/* end of former scfetab.c */

#endif /* USE_BUILTIN_FAXE */

#if USE_BUILTIN_FAXD
#if OBJDEP
#  warning PROVIDES: pts_faxd
#endif
/* scfdtab.c; must be before scfd.c */
/* #include "scommon.h" */		/* for scf.h */
/* #include "scf.h" */

/* White decoding table. */
impl_const cfd_node cf_white_decode[] = {
	{ 256, 12 },
	{ 272, 12 },
	{ 29, 8 },
	{ 30, 8 },
	{ 45, 8 },
	{ 46, 8 },
	{ 22, 7 },
	{ 22, 7 },
	{ 23, 7 },
	{ 23, 7 },
	{ 47, 8 },
	{ 48, 8 },
	{ 13, 6 },
	{ 13, 6 },
	{ 13, 6 },
	{ 13, 6 },
	{ 20, 7 },
	{ 20, 7 },
	{ 33, 8 },
	{ 34, 8 },
	{ 35, 8 },
	{ 36, 8 },
	{ 37, 8 },
	{ 38, 8 },
	{ 19, 7 },
	{ 19, 7 },
	{ 31, 8 },
	{ 32, 8 },
	{ 1, 6 },
	{ 1, 6 },
	{ 1, 6 },
	{ 1, 6 },
	{ 12, 6 },
	{ 12, 6 },
	{ 12, 6 },
	{ 12, 6 },
	{ 53, 8 },
	{ 54, 8 },
	{ 26, 7 },
	{ 26, 7 },
	{ 39, 8 },
	{ 40, 8 },
	{ 41, 8 },
	{ 42, 8 },
	{ 43, 8 },
	{ 44, 8 },
	{ 21, 7 },
	{ 21, 7 },
	{ 28, 7 },
	{ 28, 7 },
	{ 61, 8 },
	{ 62, 8 },
	{ 63, 8 },
	{ 0, 8 },
	{ 320, 8 },
	{ 384, 8 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 10, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 11, 5 },
	{ 27, 7 },
	{ 27, 7 },
	{ 59, 8 },
	{ 60, 8 },
	{ 288, 9 },
	{ 290, 9 },
	{ 18, 7 },
	{ 18, 7 },
	{ 24, 7 },
	{ 24, 7 },
	{ 49, 8 },
	{ 50, 8 },
	{ 51, 8 },
	{ 52, 8 },
	{ 25, 7 },
	{ 25, 7 },
	{ 55, 8 },
	{ 56, 8 },
	{ 57, 8 },
	{ 58, 8 },
	{ 192, 6 },
	{ 192, 6 },
	{ 192, 6 },
	{ 192, 6 },
	{ 1664, 6 },
	{ 1664, 6 },
	{ 1664, 6 },
	{ 1664, 6 },
	{ 448, 8 },
	{ 512, 8 },
	{ 292, 9 },
	{ 640, 8 },
	{ 576, 8 },
	{ 294, 9 },
	{ 296, 9 },
	{ 298, 9 },
	{ 300, 9 },
	{ 302, 9 },
	{ 256, 7 },
	{ 256, 7 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 2, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 128, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 8, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 9, 5 },
	{ 16, 6 },
	{ 16, 6 },
	{ 16, 6 },
	{ 16, 6 },
	{ 17, 6 },
	{ 17, 6 },
	{ 17, 6 },
	{ 17, 6 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 14, 6 },
	{ 14, 6 },
	{ 14, 6 },
	{ 14, 6 },
	{ 15, 6 },
	{ 15, 6 },
	{ 15, 6 },
	{ 15, 6 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 64, 5 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ 7, 4 },
	{ -2, 3 },
	{ -2, 3 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -3, 4 },
	{ 1792, 3 },
	{ 1792, 3 },
	{ 1984, 4 },
	{ 2048, 4 },
	{ 2112, 4 },
	{ 2176, 4 },
	{ 2240, 4 },
	{ 2304, 4 },
	{ 1856, 3 },
	{ 1856, 3 },
	{ 1920, 3 },
	{ 1920, 3 },
	{ 2368, 4 },
	{ 2432, 4 },
	{ 2496, 4 },
	{ 2560, 4 },
	{ 1472, 1 },
	{ 1536, 1 },
	{ 1600, 1 },
	{ 1728, 1 },
	{ 704, 1 },
	{ 768, 1 },
	{ 832, 1 },
	{ 896, 1 },
	{ 960, 1 },
	{ 1024, 1 },
	{ 1088, 1 },
	{ 1152, 1 },
	{ 1216, 1 },
	{ 1280, 1 },
	{ 1344, 1 },
	{ 1408, 1 }
};

/* Black decoding table. */
impl_const cfd_node cf_black_decode[] = {
	{ 128, 12 },
	{ 160, 13 },
	{ 224, 12 },
	{ 256, 12 },
	{ 10, 7 },
	{ 11, 7 },
	{ 288, 12 },
	{ 12, 7 },
	{ 9, 6 },
	{ 9, 6 },
	{ 8, 6 },
	{ 8, 6 },
	{ 7, 5 },
	{ 7, 5 },
	{ 7, 5 },
	{ 7, 5 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 6, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 1, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 3, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ 2, 2 },
	{ -2, 4 },
	{ -2, 4 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -3, 5 },
	{ 1792, 4 },
	{ 1792, 4 },
	{ 1984, 5 },
	{ 2048, 5 },
	{ 2112, 5 },
	{ 2176, 5 },
	{ 2240, 5 },
	{ 2304, 5 },
	{ 1856, 4 },
	{ 1856, 4 },
	{ 1920, 4 },
	{ 1920, 4 },
	{ 2368, 5 },
	{ 2432, 5 },
	{ 2496, 5 },
	{ 2560, 5 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 18, 3 },
	{ 52, 5 },
	{ 52, 5 },
	{ 640, 6 },
	{ 704, 6 },
	{ 768, 6 },
	{ 832, 6 },
	{ 55, 5 },
	{ 55, 5 },
	{ 56, 5 },
	{ 56, 5 },
	{ 1280, 6 },
	{ 1344, 6 },
	{ 1408, 6 },
	{ 1472, 6 },
	{ 59, 5 },
	{ 59, 5 },
	{ 60, 5 },
	{ 60, 5 },
	{ 1536, 6 },
	{ 1600, 6 },
	{ 24, 4 },
	{ 24, 4 },
	{ 24, 4 },
	{ 24, 4 },
	{ 25, 4 },
	{ 25, 4 },
	{ 25, 4 },
	{ 25, 4 },
	{ 1664, 6 },
	{ 1728, 6 },
	{ 320, 5 },
	{ 320, 5 },
	{ 384, 5 },
	{ 384, 5 },
	{ 448, 5 },
	{ 448, 5 },
	{ 512, 6 },
	{ 576, 6 },
	{ 53, 5 },
	{ 53, 5 },
	{ 54, 5 },
	{ 54, 5 },
	{ 896, 6 },
	{ 960, 6 },
	{ 1024, 6 },
	{ 1088, 6 },
	{ 1152, 6 },
	{ 1216, 6 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 64, 3 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 13, 1 },
	{ 23, 4 },
	{ 23, 4 },
	{ 50, 5 },
	{ 51, 5 },
	{ 44, 5 },
	{ 45, 5 },
	{ 46, 5 },
	{ 47, 5 },
	{ 57, 5 },
	{ 58, 5 },
	{ 61, 5 },
	{ 256, 5 },
	{ 16, 3 },
	{ 16, 3 },
	{ 16, 3 },
	{ 16, 3 },
	{ 17, 3 },
	{ 17, 3 },
	{ 17, 3 },
	{ 17, 3 },
	{ 48, 5 },
	{ 49, 5 },
	{ 62, 5 },
	{ 63, 5 },
	{ 30, 5 },
	{ 31, 5 },
	{ 32, 5 },
	{ 33, 5 },
	{ 40, 5 },
	{ 41, 5 },
	{ 22, 4 },
	{ 22, 4 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 14, 1 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 15, 2 },
	{ 128, 5 },
	{ 192, 5 },
	{ 26, 5 },
	{ 27, 5 },
	{ 28, 5 },
	{ 29, 5 },
	{ 19, 4 },
	{ 19, 4 },
	{ 20, 4 },
	{ 20, 4 },
	{ 34, 5 },
	{ 35, 5 },
	{ 36, 5 },
	{ 37, 5 },
	{ 38, 5 },
	{ 39, 5 },
	{ 21, 4 },
	{ 21, 4 },
	{ 42, 5 },
	{ 43, 5 },
	{ 0, 3 },
	{ 0, 3 },
	{ 0, 3 },
	{ 0, 3 }
};

/* 2-D decoding table. */
impl_const cfd_node cf_2d_decode[] = {
	{ 128, 11 },
	{ 144, 10 },
	{ 6, 7 },
	{ 0, 7 },
	{ 5, 6 },
	{ 5, 6 },
	{ 1, 6 },
	{ 1, 6 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -4, 4 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ -5, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 4, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ 3, 1 },
	{ -2, 4 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -1, 0 },
	{ -3, 3 }
};

/* Uncompresssed decoding table. */
impl_const cfd_node cf_uncompressed_decode[] = {
	{ 64, 12 },
	{ 5, 6 },
	{ 4, 5 },
	{ 4, 5 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 3, 4 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ 0, 1 },
	{ -1, 0 },
	{ -1, 0 },
	{ 8, 6 },
	{ 9, 6 },
	{ 6, 5 },
	{ 6, 5 },
	{ 7, 5 },
	{ 7, 5 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 4, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 5, 4 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 2, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 3, 3 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 0, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 },
	{ 1, 2 }
};

#if 0
/* Dummy executable code to pacify compilers. */
void scfdtab_dummy _((void));
void
scfdtab_dummy()
{
}
#endif
/* end of former scfdtab.c */
/* scfd.c */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* CCITTFax decoding filter */
/* #include "gstypes.h" */
/* #include "scf.h" */
/* #include "scfx.h" */

/* Utility procedures */
int stream_move _((stream_cursor_read *, stream_cursor_write *));	/* in stream.c */

/* ------ CCITTFaxDecode ------ */

/* private_st_CFD_state(); */

/* Set default parameter values. */
static void
s_CFD_set_defaults(register stream_state * st)
{
    stream_CFD_state *const ss = (stream_CFD_state *) st;

    s_CFD_set_defaults_inline(ss);
}

/* Initialize CCITTFaxDecode filter */
static int
s_CFD_init(stream_state * st)
{
    stream_CFD_state *const ss = (stream_CFD_state *) st;
    int raster = ss->raster =
	ROUND_UP((ss->Columns + 7) >> 3, ss->DecodedByteAlign);
    unsigned char white = (ss->BlackIs1 ? 0 : 0xff);

    s_hcd_init_inline(ss);
    /* Because skip_white_pixels can look as many as 4 bytes ahead, */
    /* we need to allow 4 extra bytes at the end of the row buffers. */
    ss->lbuf = (unsigned char*)gs_alloc_bytes(st->memory, raster + 4, "CFD lbuf");
    ss->lprev = 0;
    if (ss->lbuf == 0)
	return PTSFAX_ERRC;
/****** WRONG ******/
    if (ss->K != 0) {
	ss->lprev = (unsigned char*)gs_alloc_bytes(st->memory, raster + 4, "CFD lprev");
	if (ss->lprev == 0)
	    return PTSFAX_ERRC;
/****** WRONG ******/
	/* Clear the initial reference line for 2-D encoding. */
	ss->memset_(ss->lbuf, white, raster);
	/* Ensure that the scan of the reference line will stop. */
	ss->lbuf[raster] = 0xa0;
    }
    ss->k_left = min(ss->K, 0);
    ss->run_color = 0;
    ss->damaged_rows = 0;
    ss->skipping_damage = false;
    ss->cbit = 0;
    ss->uncomp_run = 0;
    ss->rows_left = (ss->Rows <= 0 || ss->EndOfBlock ? -1 : ss->Rows + 1);
    ss->rpos = ss->wpos = raster - 1;
    ss->eol_count = 0;
    ss->invert = white;
    ss->min_left = 1;
    return 0;
}

/* Release the filter. */
static void
s_CFD_release(stream_state * st)
{
    stream_CFD_state *const ss = (stream_CFD_state *) st;

    gs_free_object(st->memory, ss->lprev, "CFD lprev(close)");
    gs_free_object(st->memory, ss->lbuf, "CFD lbuf(close)");
}

/* Declare the variables that hold the state. */
#define cfd_declare_state\
	hcd_declare_state;\
	register unsigned char *q;\
	int qbit
/* Load the state from the stream. */
#define cfd_load_state()\
	hcd_load_state(),\
	q = ss->lbuf + ss->wpos, qbit = ss->cbit
/* Store the state back in the stream. */
#define cfd_store_state()\
	hcd_store_state(),\
	ss->wpos = q - ss->lbuf, ss->cbit = qbit

/* Macros to get blocks of bits from the input stream. */
/* Invariants: 0 <= bits_left <= bits_size; */
/* bits [bits_left-1..0] contain valid data. */

#define avail_bits(n) hcd_bits_available(n)
#define ensure_bits(n, outl) hcd_ensure_bits(n, outl)
#define peek_bits(n) hcd_peek_bits(n)
#define peek_var_bits(n) hcd_peek_var_bits(n)
#define skip_bits(n) hcd_skip_bits(n)

/* Get a run from the stream. */
#ifdef DEBUG
#  define IF_DEBUG(expr) expr
#else
#  define IF_DEBUG(expr) DO_NOTHING
#endif
#define get_run(decode, initial_bits, min_bits, runlen, str, locl, outl)\
    BEGIN\
	const cfd_node *np;\
	int clen;\
\
	HCD_ENSURE_BITS_ELSE(initial_bits) {\
	    /* We might still have enough bits for the specific code. */\
	    if (bits_left < min_bits) goto outl;\
	    np = &decode[hcd_peek_bits_left() << (initial_bits - bits_left)];\
	    if ((clen = np->code_length) > bits_left) goto outl;\
	    goto locl;\
	}\
	np = &decode[peek_bits(initial_bits)];\
	if ((clen = np->code_length) > initial_bits) {\
		IF_DEBUG(unsigned int init_bits = peek_bits(initial_bits));\
		if (!avail_bits(clen)) goto outl;\
		clen -= initial_bits;\
		skip_bits(initial_bits);\
		ensure_bits(clen, outl);		/* can't goto outl */\
		np = &decode[np->run_length + peek_var_bits(clen)];\
		if_debug4('W', "%s xcode=0x%x,%d rlen=%d\n", str,\
			  (init_bits << np->code_length) +\
			    peek_var_bits(np->code_length),\
			  initial_bits + np->code_length,\
			  np->run_length);\
		skip_bits(np->code_length);\
	} else {\
    locl:	if_debug4('W', "%s code=0x%x,%d rlen=%d\n", str,\
			  peek_var_bits(clen), clen, np->run_length);\
		skip_bits(clen);\
	}\
	runlen = np->run_length;\
    END

/* Skip data bits for a white run. */
/* rlen is either less than 64, or a multiple of 64. */
#define skip_data(rlen, makeup_label)\
	if ( (qbit -= rlen) < 0 )\
	{	q -= qbit >> 3, qbit &= 7;\
		if ( rlen >= 64 ) goto makeup_label;\
	}

/* Invert data bits for a black run. */
/* If rlen >= 64, execute makeup_action: this is to handle */
/* makeup codes efficiently, since these are always a multiple of 64. */
#define invert_data(rlen, black_byte, makeup_action, d)\
	if ( rlen > qbit )\
	{	*q++ ^= (1 << qbit) - 1;\
		rlen -= qbit;\
		switch ( rlen >> 3 )\
		{\
		case 7:		/* original rlen possibly >= 64 */\
			if ( rlen + qbit >= 64 ) goto d;\
			*q++ = black_byte;\
		case 6: *q++ = black_byte;\
		case 5: *q++ = black_byte;\
		case 4: *q++ = black_byte;\
		case 3: *q++ = black_byte;\
		case 2: *q++ = black_byte;\
		case 1: *q = black_byte;\
			rlen &= 7;\
			if ( !rlen ) { qbit = 0; break; }\
			q++;\
		case 0:			/* know rlen != 0 */\
			qbit = 8 - rlen;\
			*q ^= 0xff << qbit;\
			break;\
		default:	/* original rlen >= 64 */\
d:			ss->memset_(q, black_byte, rlen >> 3);\
			q += rlen >> 3;\
			rlen &= 7;\
			if ( !rlen ) qbit = 0, q--;\
			else qbit = 8 - rlen, *q ^= 0xff << qbit;\
			makeup_action;\
		}\
	}\
	else\
		qbit -= rlen,\
		*q ^= ((1 << rlen) - 1) << qbit

/* Buffer refill for CCITTFaxDecode filter */
static int cf_decode_eol _((stream_CFD_state *, stream_cursor_read *));
static int cf_decode_1d _((stream_CFD_state *, stream_cursor_read *));
static int cf_decode_2d _((stream_CFD_state *, stream_cursor_read *));
static int cf_decode_uncompressed _((stream_CFD_state *, stream_cursor_read *));
static int
s_CFD_process(stream_state * st, stream_cursor_read * pr,
	      stream_cursor_write * pw, bool last)
{
    stream_CFD_state *const ss = (stream_CFD_state *) st;
    int wstop = ss->raster - 1;
    int eol_count = ss->eol_count;
    int k_left = ss->k_left;
    int rows_left = ss->rows_left;
    int status = 0;
    
    (void)last; /**** pts ****/

#ifdef DEBUG
    const unsigned char *rstart = pr->ptr;
    const unsigned char *wstart = pw->ptr;

#endif

  top:
#ifdef DEBUG
    {
	hcd_declare_state;
	hcd_load_state();
	if_debug8('w', "\
[w]CFD_process top: eol_count=%d, k_left=%d, rows_left=%d\n\
    bits=0x%lx, bits_left=%d, read %u, wrote %u%s\n",
		  eol_count, k_left, rows_left,
		  (unsigned long) bits, bits_left,
		  (unsigned int) (p - rstart), (unsigned int) (pw->ptr - wstart),
		  (ss->skipping_damage ? ", skipping damage" : ""));
    }
#endif
    if (ss->skipping_damage) {	/* Skip until we reach an EOL. */
	hcd_declare_state;
	int skip;

	status = 0;
	do {
	    switch ((skip = cf_decode_eol(ss, pr))) {
		default:	/* not EOL */
		    hcd_load_state();
		    skip_bits(-skip);
		    hcd_store_state();
		    continue;
		case 0:	/* need more input */
		    goto out;
		case 1:	/* EOL */
		    {		/* Back up over the EOL. */
			hcd_load_state();
			bits_left += run_eol_code_length;
			hcd_store_state();
		    }
		    ss->skipping_damage = false;
	    }
	}
	while (ss->skipping_damage);
	ss->damaged_rows++;
    }
    /*
     * Check for a completed input scan line.  This isn't quite as
     * simple as it seems, because we could have run out of input data
     * between a makeup code and a 0-length termination code, or in a
     * 2-D line before a final horizontal code with a 0-length second
     * run.  There's probably a way to think about this situation that
     * doesn't require a special check, but I haven't found it yet.
     */
    if (ss->wpos == wstop && ss->cbit <= (-ss->Columns & 7) &&
	(k_left == 0 ? !(ss->run_color & ~1) : ss->run_color == 0)
	) {			/* Check for completed data to be copied to the client. */
	/* (We could avoid the extra copy step for 1-D, but */
	/* it's simpler not to, and it doesn't cost much.) */
	if (ss->rpos < ss->wpos) {
	    stream_cursor_read cr;

	    cr.ptr = ss->lbuf + ss->rpos;
	    cr.limit = ss->lbuf + ss->wpos;
	    status = stream_move(&cr, pw);
	    ss->rpos = cr.ptr - ss->lbuf;
	    if (status)
		goto out;
	}
	if (rows_left > 0 && --rows_left == 0) {
	    status = EOFC;
	    goto out;
	}
	if (ss->K != 0) {
	    unsigned char *prev_bits = ss->lprev;

	    ss->lprev = ss->lbuf;
	    ss->lbuf = prev_bits;
	    if (ss->K > 0)
		k_left = (k_left == 0 ? ss->K : k_left) - 1;
	}
	ss->rpos = ss->wpos = -1;
	ss->eol_count = eol_count = 0;
	ss->cbit = 0;
	ss->invert = (ss->BlackIs1 ? 0 : 0xff);
	ss->memset_(ss->lbuf, ss->invert, wstop + 1);
	ss->run_color = 0;
	/*
	 * If EndOfLine is true, we want to include the unsigned char padding
	 * in the string of initial zeros in the EOL.  If EndOfLine
	 * is false, we aren't sure what we should do....
	 */
	if (ss->EncodedByteAlign & !ss->EndOfLine)
	    ss->bits_left &= ~7;
    }
    /* If we're between scan lines, scan for EOLs. */
    if (ss->wpos < 0) {
	while ((status = cf_decode_eol(ss, pr)) > 0) {
	    if_debug0('w', "[w]EOL\n");
	    /* If we are in a Group 3 mixed regime, */
	    /* check the next bit for 1- vs. 2-D. */
	    if (ss->K > 0) {
		hcd_declare_state;
		hcd_load_state();
		ensure_bits(1, out);	/* can't fail */
		k_left = (peek_bits(1) ? 0 : 1);
		skip_bits(1);
		hcd_store_state();
	    }
	    ++eol_count;
	    /*
	     * According to Adobe, the decoder should always check for
	     * the EOD sequence, regardless of EndOfBlock: the Red Book's
	     * documentation of EndOfBlock is wrong.
	     */
	    if (eol_count == (ss->K < 0 ? 2 : 6)) {
		status = EOFC;
		goto out;
	    }
	}
	if (status == 0)	/* input empty while scanning EOLs */
	    goto out;
	switch (eol_count) {
	    case 0:
		if (ss->EndOfLine) {	/* EOL is required, but none is present. */
		    status = PTSFAX_ERRC;
		    goto check;
		}
	    case 1:
		break;
	    default:
		status = PTSFAX_ERRC;
		goto check;
	}
    }
    /* Now decode actual data. */
    if (k_left < 0) {
	if_debug0('w', "[w2]new row\n");
	status = cf_decode_2d(ss, pr);
    } else if (k_left == 0) {
	if_debug0('w', "[w1]new row\n");
	status = cf_decode_1d(ss, pr);
    } else {
	if_debug1('w', "[w1]new 2-D row, %d left\n", k_left);
	status = cf_decode_2d(ss, pr);
    }
    if_debug3('w', "[w]CFD status = %d, wpos = %d, cbit = %d\n",
	      status, ss->wpos, ss->cbit);
  check:switch (status) {
	case 1:		/* output full */
	    goto top;
	case PTSFAX_ERRC:
	    /* Check for special handling of damaged rows. */
	    if (ss->damaged_rows >= ss->DamagedRowsBeforeError ||
		!(ss->EndOfLine && ss->K >= 0)
		)
		break;
	    /* Substitute undamaged data if appropriate. */
/****** NOT IMPLEMENTED YET ******/
	    {
		ss->wpos = wstop;
		ss->cbit = -ss->Columns & 7;
		ss->run_color = 0;
	    }
	    ss->skipping_damage = true;
	    goto top;
	default:
	    ss->damaged_rows = 0;	/* finished a good row */
    }
  out:ss->k_left = k_left;
    ss->rows_left = rows_left;
    ss->eol_count = eol_count;
    return status;
}

/*
 * Decode a leading EOL, if any.
 * If an EOL is present, skip over it and return 1;
 * if no EOL is present, read no input and return -N, where N is the
 * number of initial bits that can be skipped in the search for an EOL;
 * if more input is needed, return 0.
 * Note that if we detected an EOL, we know that we can back up over it;
 * if we detected an N-bit non-EOL, we know that at least N bits of data
 * are available in the buffer.
 */
static int
cf_decode_eol(stream_CFD_state * ss, stream_cursor_read * pr)
{
    hcd_declare_state;
    int zeros;
    int look_ahead;

    hcd_load_state();
    for (zeros = 0; zeros < run_eol_code_length - 1; zeros++) {
	ensure_bits(1, out);
	if (peek_bits(1))
	    return -(zeros + 1);
	skip_bits(1);
    }
    /* We definitely have an EOL.  Skip further zero bits. */
    look_ahead = (ss->K > 0 ? 2 : 1);
    for (;;) {
	ensure_bits(look_ahead, back);
	if (peek_bits(1))
	    break;
	skip_bits(1);
    }
    skip_bits(1);
    hcd_store_state();
    return 1;
  back:			/*
				 * We ran out of data while skipping zeros.
				 * We know we are at a unsigned char boundary, and have just skipped
				 * at least run_eol_code_length - 1 zeros.  However,
				 * bits_left may be 1 if look_ahead == 2.
				 */
    bits &= (1 << bits_left) - 1;
    bits_left += run_eol_code_length - 1;
    hcd_store_state();
  out:return 0;
}

/* Decode a 1-D scan line. */
static int
cf_decode_1d(stream_CFD_state * ss, stream_cursor_read * pr)
{
    cfd_declare_state;
    unsigned char black_byte = (ss->BlackIs1 ? 0xff : 0);
    int end_bit = -ss->Columns & 7;
    unsigned char *stop = ss->lbuf - 1 + ss->raster;
    int run_color = ss->run_color;
    int status;
    int bcnt;

    cfd_load_state();
    if_debug1('w', "[w1]entry run_color = %d\n", ss->run_color);
    if (ss->run_color > 0)
	goto db;
    else
	goto dw;
#define q_at_stop() (q >= stop && (qbit <= end_bit || q > stop))
  top:run_color = 0;
    if (q_at_stop())
	goto done;
  dw:				/* Decode a white run. */
    get_run(cf_white_decode, cfd_white_initial_bits, cfd_white_min_bits,
	    bcnt, "[w1]white", dwl, out0);
    if (bcnt < 0) {		/* exceptional situation */
	switch (bcnt) {
	    case run_uncompressed:	/* Uncompressed data. */
		cfd_store_state();
		bcnt = cf_decode_uncompressed(ss, pr);
		if (bcnt < 0)
		    return bcnt;
		cfd_load_state();
		if (bcnt)
		    goto db;
		else
		    goto dw;
		/*case run_error: */
		/*case run_zeros: *//* Premature end-of-line. */
	    default:
		status = PTSFAX_ERRC;
		goto out;
	}
    }
    skip_data(bcnt, dwx);
    if (q_at_stop()) {
	run_color = 0;		/* not inside a run */
	goto done;
    }
    run_color = 1;
  db:				/* Decode a black run. */
    get_run(cf_black_decode, cfd_black_initial_bits, cfd_black_min_bits,
	    bcnt, "[w1]black", dbl, out1);
    if (bcnt < 0) {		/* All exceptional codes are invalid here. */
/****** WRONG, uncompressed IS ALLOWED ******/
	status = PTSFAX_ERRC;
	goto out;
    }
    /* Invert bits designated by black run. */
    invert_data(bcnt, black_byte, goto dbx, idb);
    goto top;
  dwx:				/* If we run out of data after a makeup code, */
    /* note that we are still processing a white run. */
    run_color = -1;
    goto dw;
  dbx:				/* If we run out of data after a makeup code, */
    /* note that we are still processing a black run. */
    run_color = 2;
    goto db;
  done:if (q > stop || qbit < end_bit)
	status = PTSFAX_ERRC;
    else
	status = 1;
  out:cfd_store_state();
    ss->run_color = run_color;
    if_debug1('w', "[w1]exit run_color = %d\n", run_color);
    return status;
  out0:			/* We already set run_color to 0 or -1. */
    status = 0;
    goto out;
  out1:			/* We already set run_color to 1 or 2. */
    status = 0;
    goto out;
}

/* Decode a 2-D scan line. */
static int
cf_decode_2d(stream_CFD_state * ss, stream_cursor_read * pr)
{
    cfd_declare_state;
    unsigned char invert_white = (ss->BlackIs1 ? 0 : 0xff);
    unsigned char black_byte = ~invert_white;
    unsigned char invert = ss->invert;
    int end_count = -ss->Columns & 7;
    unsigned int raster = ss->raster;
    unsigned char *q0 = ss->lbuf;
    unsigned char *prev_q01 = ss->lprev + 1;
    unsigned char *endptr = q0 - 1 + raster;
    int init_count = raster << 3;
    register int count;
    int rlen;
    int status;

    cfd_load_state();
    count = ((endptr - q) << 3) + qbit;
    endptr[1] = 0xa0;		/* a unsigned char with some 0s and some 1s, */
    /* to ensure run scan will stop */
    if_debug1('W', "[w2]raster=%d\n", raster);
    switch (ss->run_color) {
	case -2:
	    ss->run_color = 0;
	    goto hww;
	case -1:
	    ss->run_color = 0;
	    goto hbw;
	case 1:
	    ss->run_color = 0;
	    goto hwb;
	case 2:
	    ss->run_color = 0;
	    goto hbb;
	    /*case 0: */
    }
  top:if (count <= end_count) {
	status = (count < end_count ? PTSFAX_ERRC : 1);
	goto out;
    }
    /* If invert == invert_white, white and black have their */
    /* correct meanings; if invert == ~invert_white, */
    /* black and white are interchanged. */
    if_debug1('W', "[w2]%4d:\n", count);
#ifdef DEBUG
    /* Check the invariant between q, qbit, and count. */
    {
	int pcount = (endptr - q) * 8 + qbit;

	if (pcount != count)
	    dlprintf2("[w2]Error: count=%d pcount=%d\n",
		      count, pcount);
    }
#endif
    /*
     * We could just use get_run here, but we can do better.  However,
     * we must be careful to handle the case where the very last codes
     * in the input stream are 1-bit "vertical 0" codes: we can't just
     * use ensure_bits(3, ...) and go to get more data if it fails.
     */
    ensure_bits(3, out3);
#define vertical_0 (countof(cf2_run_vertical) / 2)
    switch (peek_bits(3)) {
	default /*4..7*/ :	/* vertical(0) */
v0:	    skip_bits(1);
	    rlen = vertical_0;
	    break;
	case 2:		/* vertical(+1) */
	    skip_bits(3);
	    rlen = vertical_0 + 1;
	    break;
	case 3:		/* vertical(-1) */
	    skip_bits(3);
	    rlen = vertical_0 - 1;
	    break;
	case 1:		/* horizontal */
	    skip_bits(3);
	    if (invert == invert_white)
		goto hww;
	    else
		goto hbb;
	case 0:		/* everything else */
	    get_run(cf_2d_decode, cfd_2d_initial_bits, cfd_2d_min_bits,
		    rlen, "[w2]", d2l, out0);
	    /* rlen may be run2_pass, run_uncompressed, or */
	    /* 0..countof(cf2_run_vertical)-1. */
	    if (rlen < 0)
		switch (rlen) {
		    case run2_pass:
			break;
		    case run_uncompressed:
			{
			    int which;

			    cfd_store_state();
			    which = cf_decode_uncompressed(ss, pr);
			    if (which < 0) {
				status = which;
				goto out;
			    }
			    cfd_load_state();
/****** ADJUST count ******/
			    invert = (which ? ~invert_white : invert_white);
			}
			goto top;
		    default:	/* run_error, run_zeros */
			status = PTSFAX_ERRC;
			goto out;
		}
    }
    /* Interpreting the run requires scanning the */
    /* previous ('reference') line. */
    {
	int prev_count = count;
	unsigned char prev_data;
	int dlen;
	static const unsigned char count_bit[8] =
	{0x80, 1, 2, 4, 8, 0x10, 0x20, 0x40};
	unsigned char *prev_q = prev_q01 + (q - q0);
	int plen;

	if (!(count & 7))
	    prev_q++;		/* because of skip macros */
	prev_data = prev_q[-1] ^ invert;
	/* Find the b1 transition. */
	if ((prev_data & count_bit[prev_count & 7]) &&
	    (prev_count < init_count || invert != invert_white)
	    ) {			/* Look for changing white first. */
	    if_debug1('W', " data=0x%x", prev_data);
	    skip_black_pixels(prev_data, prev_q,
			      prev_count, invert, plen);
	    if (prev_count < end_count)		/* overshot */
		prev_count = end_count;
	    if_debug1('W', " b1 other=%d", prev_count);
	}
	if (prev_count != end_count) {
	    if_debug1('W', " data=0x%x", prev_data);
	    skip_white_pixels(prev_data, prev_q,
			      prev_count, invert, plen);
	    if (prev_count < end_count)		/* overshot */
		prev_count = end_count;
	    if_debug1('W', " b1 same=%d", prev_count);
	}
	/* b1 = prev_count; */
	if (rlen == run2_pass) {	/* Pass mode.  Find b2. */
	    if (prev_count != end_count) {
		if_debug1('W', " data=0x%x", prev_data);
		skip_black_pixels(prev_data, prev_q,
				  prev_count, invert, plen);
		if (prev_count < end_count)	/* overshot */
		    prev_count = end_count;
	    }
	    /* b2 = prev_count; */
	    if_debug2('W', " b2=%d, pass %d\n",
		      prev_count, count - prev_count);
	} else {		/* Vertical coding. */
	    /* Remember that count counts *down*. */
	    prev_count += rlen - vertical_0;	/* a1 */
	    if_debug2('W', " vertical %d -> %d\n",
		      rlen - vertical_0, prev_count);
	}
	/* Now either invert or skip from count */
	/* to prev_count, and reset count. */
	if (invert == invert_white) {	/* Skip data bits. */
	    q = endptr - (prev_count >> 3);
	    qbit = prev_count & 7;
	} else {		/* Invert data bits. */
	    dlen = count - prev_count;
	    invert_data(dlen, black_byte, DO_NOTHING, idd);
	}
	count = prev_count;
	if (rlen >= 0)		/* vertical mode */
	    invert = ~invert;	/* polarity changes */
    }
    goto top;
 out3:
    if (bits_left > 0 && peek_bits(1)) {
	/* This is a 1-bit "vertical 0" code, which we can still process. */
	goto v0;
    }
    /* falls through */
  out0:status = 0;
    /* falls through */
  out:cfd_store_state();
    ss->invert = invert;
    return status;
    /*
     * We handle horizontal decoding here, so that we can
     * branch back into it if we run out of input data.
     */
    /* White, then black. */
  hww:get_run(cf_white_decode, cfd_white_initial_bits, cfd_white_min_bits,
	      rlen, " white", wwl, outww);
    if ((count -= rlen) < end_count) {
	status = PTSFAX_ERRC;
	goto out;
    }
    skip_data(rlen, hww);
    /* Handle the second half of a white-black horizontal code. */
  hwb:get_run(cf_black_decode, cfd_black_initial_bits, cfd_black_min_bits,
	      rlen, " black", wbl, outwb);
    if ((count -= rlen) < end_count) {
	status = PTSFAX_ERRC;
	goto out;
    }
    invert_data(rlen, black_byte, goto hwb, ihwb);
    goto top;
  outww:ss->run_color = -2;
    goto out0;
  outwb:ss->run_color = 1;
    goto out0;
    /* Black, then white. */
  hbb:get_run(cf_black_decode, cfd_black_initial_bits, cfd_black_min_bits,
	      rlen, " black", bbl, outbb);
    if ((count -= rlen) < end_count) {
	status = PTSFAX_ERRC;
	goto out;
    }
    invert_data(rlen, black_byte, goto hbb, ihbb);
    /* Handle the second half of a black-white horizontal code. */
  hbw:get_run(cf_white_decode, cfd_white_initial_bits, cfd_white_min_bits,
	      rlen, " white", bwl, outbw);
    if ((count -= rlen) < end_count) {
	status = PTSFAX_ERRC;
	goto out;
    }
    skip_data(rlen, hbw);
    goto top;
  outbb:ss->run_color = 2;
    goto out0;
  outbw:ss->run_color = -1;
    goto out0;
}

#if 1				/*************** */
static int
cf_decode_uncompressed(stream_CFD_state * ss, stream_cursor_read * pr)
{
    (void)ss;
    (void)pr;
    /**** pts ****/
    return PTSFAX_ERRC;
}
#else /*************** */

/* Decode uncompressed data. */
/* (Not tested: no sample data available!) */
/****** DOESN'T CHECK FOR OVERFLOWING SCAN LINE ******/
static int
cf_decode_uncompressed(stream * s)
{
    cfd_declare_state;
    const cfd_node *np;
    int clen, rlen;

    cfd_load_state();
    while (1) {
	ensure_bits(cfd_uncompressed_initial_bits, NOOUT);
	np = &cf_uncompressed_decode[peek_bits(cfd_uncompressed_initial_bits)];
	clen = np->code_length;
	rlen = np->run_length;
	if (clen > cfd_uncompressed_initial_bits) {	/* Must be an exit code. */
	    break;
	}
	if (rlen == cfd_uncompressed_initial_bits) {	/* Longest representable white run */
	    if_debug1('W', "[wu]%d\n", rlen);
	    if ((qbit -= cfd_uncompressed_initial_bits) < 0)
		qbit += 8, q++;
	} else {
	    if_debug1('W', "[wu]%d+1\n", rlen);
	    if (qbit -= rlen < 0)
		qbit += 8, q++;
	    *q ^= 1 << qbit;
	}
	skip_bits(clen);
    }
    clen -= cfd_uncompressed_initial_bits;
    skip_bits(cfd_uncompressed_initial_bits);
    ensure_bits(clen, NOOUT);
    np = &cf_uncompressed_decode[rlen + peek_var_bits(clen)];
    rlen = np->run_length;
    skip_bits(np->code_length);
    if_debug1('w', "[wu]exit %d\n", rlen);
    if (rlen >= 0) {		/* Valid exit code, rlen = 2 * run length + next polarity */
	if ((qbit -= rlen >> 1) < 0)
	    qbit += 8, q++;
	rlen &= 1;
    }
  out:
/******* WRONG ******/
    cfd_store_state();
    return rlen;
}

#endif /*************** */

/* Move as much data as possible from one buffer to another. */
/* Return 0 if the input became empty, 1 if the output became full. */
int
stream_move(stream_cursor_read * pr, stream_cursor_write * pw)
{
    unsigned int rcount = pr->limit - pr->ptr;
    unsigned int wcount = pw->limit - pw->ptr;
    unsigned int count;
    int status;

    if (rcount <= wcount)
        count = rcount, status = 0;
    else
        count = wcount, status = 1;

    /**** pts ****/ /* Dat: memcpy should be enough instead of memmove */
    #if 0
      memmove(pw->ptr + 1, pr->ptr + 1, count);
      pr->ptr += count;
      pw->ptr += count;
    #else
      while (count--!=0) *++pw->ptr=*++pr->ptr;
    #endif
    return status;
}
                                                        

#if 1 /**** pts ****/
/* Stream template */
const stream_template s_CFD_template =
{/*0, &st_CFD_state*/ s_CFD_init, s_CFD_process, 1, 1, s_CFD_release,
 s_CFD_set_defaults, 0
};
#endif

/* end of former scfd.c */


#endif /* USE_BUILTIN_FAXD */

#if USE_BUILTIN_FAXE || USE_BUILTIN_FAXD
/* shc.c */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* Support code for shc.h */
/* #include "scommon.h" */
/* #include "shc.h" */
/* #include "scfx.h" */ /**** pts ****/ /* struct stream_hc_state_s */

/* ------ Encoding ------ */

/* Empty the 1-word buffer onto the output stream. */
/* q has already been incremented. */
static void
hc_put_code_proc(bool reverse_bits, unsigned char * q, unsigned int cw)
{
#define cb(n) ((unsigned char)(cw >> (n * 8)))
    if (reverse_bits) {
#if hc_bits_size > 16
	q[-3] = byte_reverse_bits[cb(3)];
	q[-2] = byte_reverse_bits[cb(2)];
#endif
	q[-1] = byte_reverse_bits[cb(1)];
	q[0] = byte_reverse_bits[cb(0)];
    } else {
#if hc_bits_size > 16
	q[-3] = cb(3);
	q[-2] = cb(2);
#endif
	q[-1] = cb(1);
	q[0] = cb(0);
    }
#undef cb
}

/* Put out any final bytes. */
/* Note that this does a store_state, but not a load_state. */
static unsigned char *
hc_put_last_bits_proc(stream_hc_state * ss, unsigned char * q, unsigned int bits, int bits_left)
{
    while (bits_left < hc_bits_size) {
	unsigned char c = (unsigned char) (bits >> (hc_bits_size - 8));

	if (ss->FirstBitLowOrder)
	    c = byte_reverse_bits[c];
	*++q = c;
	bits <<= 8;
	bits_left += 8;
    }
    ss->bits = bits;
    ss->bits_left = bits_left;
    return q;
}

/* end of former shc.c */


/* gsbittab.c */
/*$Id: pts_fax.c,v 1.3 2005/02/21 13:09:56 pts Exp $ */
/* Tables for bit operations */
/* #include "gstypes.h" */
/* #include "gsbittab.h" */

/* ---------------- Byte processing tables ---------------- */

/*
 * byte_reverse_bits[B] = the unsigned char B with the order of bits reversed.
 */
impl_const unsigned char byte_reverse_bits[256] = {
    bit_table_8(0, 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80)
};

/*
 * byte_right_mask[N] = a unsigned char with N trailing 1s, 0 <= N <= 8.
 */
impl_const unsigned char byte_right_mask[9] = {
    0, 1, 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 0xff
};

/*
 * byte_count_bits[B] = the number of 1-bits in a unsigned char with value B.
 */
impl_const unsigned char byte_count_bits[256] = {
    bit_table_8(0, 1, 1, 1, 1, 1, 1, 1, 1)
};

/* ---------------- Scanning tables ---------------- */

/*
 * byte_bit_run_length_N[B], for 0 <= N <= 7, gives the length of the
 * run of 1-bits starting at bit N in a unsigned char with value B,
 * numbering the bits in the unsigned char as 01234567.  If the run includes
 * the low-order bit (i.e., might be continued into a following unsigned char),
 * the run length is increased by 8.
 */

#define t8(n) n,n,n,n,n+1,n+1,n+2,n+11
#define r8(n) n,n,n,n,n,n,n,n
#define r16(n) r8(n),r8(n)
#define r32(n) r16(n),r16(n)
#define r64(n) r32(n),r32(n)
#define r128(n) r64(n),r64(n)
impl_const unsigned char byte_bit_run_length_0[256] = {
    r128(0), r64(1), r32(2), r16(3), r8(4), t8(5)
};
impl_const unsigned char byte_bit_run_length_1[256] = {
    r64(0), r32(1), r16(2), r8(3), t8(4),
    r64(0), r32(1), r16(2), r8(3), t8(4)
};
impl_const unsigned char byte_bit_run_length_2[256] = {
    r32(0), r16(1), r8(2), t8(3),
    r32(0), r16(1), r8(2), t8(3),
    r32(0), r16(1), r8(2), t8(3),
    r32(0), r16(1), r8(2), t8(3)
};
impl_const unsigned char byte_bit_run_length_3[256] = {
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2),
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2),
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2),
    r16(0), r8(1), t8(2), r16(0), r8(1), t8(2)
};
impl_const unsigned char byte_bit_run_length_4[256] = {
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
    r8(0), t8(1), r8(0), t8(1), r8(0), t8(1), r8(0), t8(1),
};

#define rr8(a,b,c,d,e,f,g,h)\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h,\
  a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h, a,b,c,d,e,f,g,h
impl_const unsigned char byte_bit_run_length_5[256] = {
    rr8(0, 0, 0, 0, 1, 1, 2, 11)
};
impl_const unsigned char byte_bit_run_length_6[256] = {
    rr8(0, 0, 1, 10, 0, 0, 1, 10)
};
impl_const unsigned char byte_bit_run_length_7[256] = {
    rr8(0, 9, 0, 9, 0, 9, 0, 9)
};

/* Pointer tables indexed by bit number. */

impl_const unsigned char *const byte_bit_run_length[8] = {
    byte_bit_run_length_0, byte_bit_run_length_1,
    byte_bit_run_length_2, byte_bit_run_length_3,
    byte_bit_run_length_4, byte_bit_run_length_5,
    byte_bit_run_length_6, byte_bit_run_length_7
};
impl_const unsigned char *const byte_bit_run_length_neg[8] = {
    byte_bit_run_length_0, byte_bit_run_length_7,
    byte_bit_run_length_6, byte_bit_run_length_5,
    byte_bit_run_length_4, byte_bit_run_length_3,
    byte_bit_run_length_2, byte_bit_run_length_1
};

/*
 * byte_acegbdfh_to_abcdefgh[acegbdfh] = abcdefgh, where the letters
 * denote the individual bits of the unsigned char.
 */
impl_const unsigned char byte_acegbdfh_to_abcdefgh[256] = {
    bit_table_8(0, 0x80, 0x20, 0x08, 0x02, 0x40, 0x10, 0x04, 0x01)
};

#if 0
/* Some C compilers insist on having executable code in every file.... */
void gsbittab_dummy _((void));	/* for picky compilers */
void
gsbittab_dummy(void)
{
}
#endif

/* end of former gsbittab.c */
#endif /* USE_BUILTIN_FAXE || USE_BUILTIN_FAXD */

/* end of pts_fax.c */
