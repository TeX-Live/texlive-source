/*
 * pts_fax.h -- a compact CCITTFax compressor and uncompressor) interface
 * compiled by pts@fazekas.hu at Sun Jul  7 19:51:42 CEST 2002
 *
 * For usage example, see fax_test.c.
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

/*$Id: pts_fax.h,v 1.4 2005/07/20 21:15:08 pts Exp $ */
/* CCITTFax filter state definition */
/* Requires strimpl.h */

#ifndef PTS_FAX_H
#  define PTS_FAX_H 1

#ifdef __GNUC__
#pragma interface
#endif

/* #include "scommon.h" */
/*$Id: pts_fax.h,v 1.4 2005/07/20 21:15:08 pts Exp $ */
/* Definitions common to stream clients and implementors */

/* #ifndef scommon_DEFINED */
/* #  define scommon_DEFINED */

/**** pts ****/
#if 0
#include "gsmemory.h"
#include "gsstype.h"		/* for extern_st */
#endif

/**** pts ****/
#include "config2.h"
/* ^^^ should #define SIZEOF_INT
 * ^^^ should #define _, and ___
 * ^^^ should #define? USE_BULITIN_FAXD
 * ^^^ should #define? USE_BULITIN_FAXE
 * ^^^ should define const
 */
#if 0 /* example for Linux i386 gcc 2.95: */
  #define SIZEOF_INT 4
  #if ((defined(__STDC__) || defined(__PROTOTYPES__)) && !defined(NO_PROTO)) || defined(__cplusplus)
  # define _(args) args
  # define OF(args) args
  # define ___(arg2s,arg1s,argafter) arg2s /* Dat: no direct comma allowed in args :-( */
  #else
  # define _(args) ()
  # define OF(args) ()
  # define ___(arg2s,arg1s,argafter) arg1s argafter /* Dat: no direct comma allowed in args :-( */
  #endif
  #define USE_BUILTIN_FAXE 1
  #define USE_BUILITN_FAXD 1
#endif

typedef void (*gssss_memset_t)(void *s, int c, unsigned n); /* Imp: not `unsigned' len */
/** The function must not return out of memory. */
typedef void* (*gssss_xalloc_t)(unsigned len); /* Imp: not `unsigned' len */
typedef void (*gssss_free_t)(void *ptr);
typedef void (*gssss_memcpy_t)(void *dest, const void *src, unsigned len); /* Imp: not `unsigned' len */

/*
 * There are three major structures involved in the stream package.
 *
 * A stream is an "object" that owns a buffer, which it uses to implement
 * unsigned char-oriented sequential access in a standard way, and a set of
 * procedures that handle things like buffer refilling.  See stream.h
 * for more information about streams.
 */
#ifndef stream_DEFINED
#  define stream_DEFINED
typedef struct stream_s stream;

#endif
/*
 * A stream_state records the state specific to a given variety of stream.
 * The buffer processing function of a stream maintains this state.
 */
typedef struct stream_state_s stream_state;

/*
 * A stream_template provides the information needed to create a stream.
 * The client must fill in any needed setup parameters in the appropriate
 * variety of stream_state, and then call the initialization function
 * provided by the template.  See strimpl.h for more information about
 * stream_templates.
 */
typedef struct stream_template_s stream_template;

/*
 * The stream package works with bytes, not chars.
 * This is to ensure unsigned representation on all systems.
 * A stream currently can only be read or written, not both.
 * Note also that the read procedure returns an int, not a char or a unsigned char;
 * we use negative values to indicate exceptional conditions.
 * (We cast these values to int explicitly, because some compilers
 * don't do this if the other arm of a conditional is a unsigned char.)
 */
/* End of data */
#define PTSFAX_EOFC ((int)(-1))
/* Error */
#define PTSFAX_ERRC ((int)(-2))
/* Interrupt */
/* #define INTC ((int)(-3)) */
/****** INTC IS NOT USED YET ******/
/* Callout */
/* #define CALLC ((int)(-4)) */
#define max_stream_exception 4
/* The following hack is needed for initializing scan_char_array in iscan.c. */
#define stream_exception_repeat(x) x, x, x, x

/*
 * Define cursors for reading from or writing into a buffer.
 * We lay them out this way so that we can alias
 * the write pointer and the read limit.
 */
typedef struct stream_cursor_read_s {
    const unsigned char *ptr;
    const unsigned char *limit;
    unsigned char *_skip;
} stream_cursor_read;
typedef struct stream_cursor_write_s {
    const unsigned char *_skip;
    unsigned char *ptr;
    unsigned char *limit;
} stream_cursor_write;
typedef union stream_cursor_s {
    stream_cursor_read r;
    stream_cursor_write w;
} stream_cursor;

/*
 * Define the prototype for the procedures known to both the generic
 * stream code and the stream implementations.
 */

/* Initialize the stream state (after the client parameters are set). */
#define stream_proc_init(proc)\
  int proc _((stream_state *))

/* Process a buffer.  See strimpl.h for details. */
#define stream_proc_process(proc)\
  int proc _((stream_state *, stream_cursor_read *,\
    stream_cursor_write *, bool))

/* Release the stream state when closing. */
#define stream_proc_release(proc)\
  void proc _((stream_state *))

/* Initialize the client parameters to default values. */
#define stream_proc_set_defaults(proc)\
  void proc _((stream_state *))

/* Reinitialize any internal stream state.  Note that this does not */
/* affect buffered data.  We declare this as returning an int so that */
/* it can be the same as the init procedure; however, reinit cannot fail. */
#define stream_proc_reinit(proc)\
  int proc _((stream_state *))

/* Report an error.  Note that this procedure is stored in the state, */
/* not in the main stream structure. */
#define stream_proc_report_error(proc)\
  int proc _((stream_state *, const char *))
stream_proc_report_error(s_no_report_error);

/*
 * Some types of streams have the ability to read their parameters from
 * a parameter list, and to write all (or only the non-default)
 * parameters to a parameter list.  Since these are not virtual
 * procedures for the stream (they operate on stream_state structures
 * even if no actual stream has been created), we name them differently.
 */
#define stream_state_proc_get_params(proc, state_type)\
  int proc _((gs_param_list *plist, const state_type *ss, bool all))
#define stream_state_proc_put_params(proc, state_type)\
  int proc _((gs_param_list *plist, state_type *ss))

/*
 * Define a generic stream state.  If a processing procedure has no
 * state of its own, it can use stream_state; otherwise, it must
 * create a "subclass".  There is a hack in stream.h to allow the stream
 * itself to serve as the "state" of a couple of heavily used stream types.
 *
 * In order to simplify the structure descriptors for concrete streams,
 * we require that the generic stream state not contain any pointers
 * to garbage-collectable storage.
 */
#define STREAM_MAX_ERROR_STRING 79
#define stream_state_common\
	const stream_template *template_; /**** pts C++ */\
	gssss_memset_t memset_;\
	gssss_xalloc_t xalloc_;\
	gssss_free_t   free_;\
	gssss_memcpy_t memcpy_;\
	/* gs_memory_t *memory; */ /**** pts ****/ \
	stream_proc_report_error((*report_error));\
        int min_left; /* required bytes for lookahead */ \
	char error_string[STREAM_MAX_ERROR_STRING + 1]
struct stream_state_s {
    stream_state_common;
};

/* extern_st(st_stream_state); */ /**** pts ****/
#define public_st_stream_state() /* in stream.c */\
  gs_public_st_simple(st_stream_state, stream_state, "stream_state")

/**** pts ****/
/* original strimpl.h coming */
/*$Id: pts_fax.h,v 1.4 2005/07/20 21:15:08 pts Exp $ */
/* Definitions for stream implementors */
/* Requires stdio.h */

/*
 * The 'process' procedure does the real work of the stream.
 * It must process as much input information (from pr->ptr + 1 through
 * pr->limit) as it can, subject to space available for output
 * (pw->ptr + 1 through pw->limit), updating pr->ptr and pw->ptr.
 *
 * The procedure return value must be one of:
 *      PTSFAX_EOFC - an end-of-data pattern was detected in the input,
 *        or no more input can be processed for some other reason (e.g.,
 *        the stream was told only to read a certain amount of data).
 *      PTSFAX_ERRC - a syntactic error was detected in the input.
 *      0 - more input data is needed.
 *      1 - more output space is needed.
 * If the procedure returns PTSFAX_EOFC, it can assume it will never be called
 * again for that stream.
 *
 * If the procedure is called with last = 1, this is an indication that
 * no more input will ever be supplied (after the input in the current
 * buffer defined by *pr); the procedure should produce as much output
 * as possible, including an end-of-data marker if applicable.  In this
 * case:
 *      - If the procedure returns 1, it may be called again (also with
 *        last = 1).
 *      - If the procedure returns any other value other than 1, the
 *        procedure will never be called again for that stream.
 *      - If the procedure returns 0, this is taken as equivalent to
 *        returning PTSFAX_EOFC.
 *      - If the procedure returns PTSFAX_EOFC (or 0), the stream's end_status
 *        is set to PTSFAX_EOFC, meaning no more writing is allowed.
 *
 * Note that these specifications do not distinguish input from output
 * streams.  This is deliberate: The processing procedures should work
 * regardless of which way they are oriented in a stream pipeline.
 * (The PostScript language does take a position as whether any given
 * filter may be used for input or output, but this occurs at a higher level.)
 *
 * The value returned by the process procedure of a stream whose data source
 * or sink is external (i.e., not another stream) is interpreted slightly
 * differently.  For an external data source, a return value of 0 means
 * "no more input data are available now, but more might become available
 * later."  For an external data sink, a return value of 1 means "there is
 * no more room for output data now, but there might be room later."
 *
 * It appears that the Adobe specifications, read correctly, require that when
 * the process procedure of a decoding filter has filled up the output
 * buffer, it must still peek ahead in the input to determine whether or not
 * the next thing in the input stream is EOD.  If the next thing is an EOD (or
 * end-of-data, indicated by running out of input data with last = true), the
 * process procedure must return PTSFAX_EOFC; if the next thing is definitely not
 * an EOD, the process procedure must return 1 (output full) (without, of
 * course, consuming the non-EOD datum); if the procedure cannot determine
 * whether or not the next thing is an EOD, it must return 0 (need more input).
 * Decoding filters that don't have EOD (for example, NullDecode) can use
 * a simpler algorithm: if the output buffer is full, then if there is more
 * input, return 1, otherwise return 0 (which is taken as PTSFAX_EOFC if last
 * is true).  All this may seem a little awkward, but it is needed in order
 * to have consistent behavior regardless of where buffer boundaries fall --
 * in particular, if a buffer boundary falls just before an EOD.  It is
 * actually quite easy to implement if the main loop of the process
 * procedure tests for running out of input rather than for filling the
 * output: with this structure, exhausting the input always returns 0,
 * and discovering that the output buffer is full when attempting to store
 * more output always returns 1.
 *
 * Even this algorithm for handling end-of-buffer is not sufficient if an
 * EOD falls just after a buffer boundary, but the generic stream code
 * handles this case: the process procedures need only do what was just
 * described.
 */

/*
 * The set_defaults procedure in the template has a dual purpose: it sets
 * default values for all parameters that the client can set before calling
 * the init procedure, and it also must initialize all pointers in the
 * stream state to a value that will be valid for the garbage collector
 * (normally 0).  The latter implies that:
 *
 *	Any stream whose state includes additional pointers (beyond those
 *	in stream_state_common) must have a set_defaults procedure.
 */

/*
 * Note that all decoding filters that require an explicit EOD in the
 * source data must have an init procedure that sets min_left = 1.
 * This effectively provides a 1-unsigned char lookahead in the source data,
 * which is required so that the stream can close itself "after reading
 * the last unsigned char of data" (per Adobe specification), as noted above.
 */

/*
 * Define a template for creating a stream.
 *
 * The meaning of min_in_size and min_out_size is the following:
 * If the amount of input information is at least min_in_size,
 * and the available output space is at least min_out_size,
 * the process procedure guarantees that it will make some progress.
 * (It may make progress even if this condition is not met, but this is
 * not guaranteed.)
 */
struct stream_template_s {

    /* Define the structure type for the stream state. */
    /* gs_memory_type_ptr_t stype; */ /**** pts ****/

    /* Define an optional initialization procedure. */
    stream_proc_init((*init));

    /* Define the processing procedure. */
    /* (The init procedure can reset other procs if it wants.) */
    stream_proc_process((*process));

    /* Define the minimum buffer sizes. */
    unsigned int min_in_size;		/* minimum size for process input */
    unsigned int min_out_size;		/* minimum size for process output */

    /* Define an optional releasing procedure. */
    stream_proc_release((*release));

    /* Define an optional parameter defaulting and pointer initialization */
    /* procedure. */
    stream_proc_set_defaults((*set_defaults));

    /* Define an optional reinitialization procedure. */
    stream_proc_reinit((*reinit));

};


#if 0
/* Hex decoding utility procedure */
typedef enum {
    hex_ignore_garbage = 0,
    hex_ignore_whitespace = 1,
    hex_ignore_leading_whitespace = 2
} hex_syntax;
int s_hex_process _((stream_cursor_read *, stream_cursor_write *, int *, hex_syntax));	/* in sstring.c */
#endif
/* end of former strimpl.h */
/* #endif */ /* scommon_INCLUDED */
/* end of former scommon.h */



/* ------ Common state ------ */

/*
 * Define the common stream state for Huffman-coded filters.
 * Invariants when writing:
 *      0 <= bits_left <= hc_bits_size;
 *      Only the leftmost (hc_bits_size - bits_left) bits of bits
 *        contain valid data.
 */
#define stream_hc_state_common\
	stream_state_common;\
		/* The client sets the following before initialization. */\
	bool FirstBitLowOrder;\
		/* The following are updated dynamically. */\
	unsigned int bits;		/* most recent bits of input or */\
				/* current bits of output */\
	int bits_left		/* # of valid low bits (input) or */\
				/* unused low bits (output) in above, */\
				/* 0 <= bits_left <= 7 */
typedef struct stream_hc_state_s {
    stream_hc_state_common;
} stream_hc_state;

/* Common state */
#define stream_CF_state_common\
	stream_hc_state_common;\
		/* The client sets the following before initialization. */\
	bool Uncompressed;\
	int K;\
	bool EndOfLine;\
	bool EncodedByteAlign;\
	int Columns;\
	int Rows;\
	bool EndOfBlock;\
	bool BlackIs1;\
	int DamagedRowsBeforeError;	/* (Decode only) */\
	/*bool FirstBitLowOrder;*/	/* in stream_hc_state_common */\
	int DecodedByteAlign;\
		/* The init procedure sets the following. */\
	unsigned int raster;\
	unsigned char *lbuf;		/* current scan line buffer */\
				/* (only if decoding or 2-D encoding) */\
	unsigned char *lprev;		/* previous scan line buffer (only if 2-D) */\
		/* The following are updated dynamically. */\
	int k_left		/* number of next rows to encode in 2-D */\
				/* (only if K > 0) */
typedef struct stream_CF_state_s {
    stream_CF_state_common;
} stream_CF_state;

/* Define common default parameter setting. */
#define s_CF_set_defaults_inline(ss)\
  ((ss)->Uncompressed = false,\
   (ss)->K = 0,\
   (ss)->EndOfLine = false,\
   (ss)->EncodedByteAlign = false,\
   (ss)->Columns = 1728,\
   (ss)->Rows = 0,\
   (ss)->EndOfBlock = true,\
   (ss)->BlackIs1 = false,\
		/* Added by Adobe since the Red Book */\
   (ss)->DamagedRowsBeforeError = 0, /* always set, for s_CF_get_params */\
   (ss)->FirstBitLowOrder = false,\
		/* Added by us */\
   (ss)->DecodedByteAlign = 1,\
	/* Clear pointers */\
   (ss)->lbuf = 0, (ss)->lprev = 0)

/* CCITTFaxEncode */
typedef struct stream_CFE_state_s {
    stream_CF_state_common;
    /* The init procedure sets the following. */
    int max_code_bytes;		/* max # of bytes for an encoded line */
    unsigned char *lcode;		/* buffer for encoded output line */
    /* The following change dynamically. */
    int read_count;		/* # of bytes to copy into lbuf */
    int write_count;		/* # of bytes to copy out of lcode */
    int code_bytes;		/* # of occupied bytes in lcode */
} stream_CFE_state;

#define private_st_CFE_state()	/* in scfe.c */\
  gs_private_st_ptrs3(st_CFE_state, stream_CFE_state, "CCITTFaxEncode state",\
    cfe_enum_ptrs, cfe_reloc_ptrs, lbuf, lprev, lcode)
#define s_CFE_set_defaults_inline(ss)\
  (s_CF_set_defaults_inline(ss), (ss)->lcode = 0)
#if USE_BUILTIN_FAXE
/**** pts ****/
#if SIZEOF_INT > 2
#  define cfe_max_width (2560 * 32000 * 2 / 3)
#else
#  define cfe_max_width ((int)((unsigned)-1/2 - 40))	/* avoid overflows */
#endif
#ifdef __cplusplus
extern "C"
#else
extern
#endif
const stream_template s_CFE_template;
#endif

/* CCITTFaxDecode */
typedef struct stream_CFD_state_s {
    stream_CF_state_common;
    int cbit;			/* bits left to fill in current decoded */
    /* unsigned char at lbuf[wpos] (0..7) */
    int rows_left;		/* number of rows left */
    int rpos;			/* rptr for copying lbuf to client */
    int wpos;			/* rlimit/wptr for filling lbuf or */
    /* copying to client */
    int eol_count;		/* number of EOLs seen so far */
    unsigned char invert;		/* current value of 'white' */
    /* for 2-D decoding */
    int run_color;		/* -1 if processing white run, */
    /* 0 if between runs but white is next, */
    /* 1 if between runs and black is next, */
    /* 2 if processing black run */
    int damaged_rows;		/* # of consecutive damaged rows preceding */
    /* the current row */
    bool skipping_damage;	/* true if skipping a damaged row looking */
    /* for EOL */
    /* The following are not used yet. */
    int uncomp_run;		/* non-0 iff we are in an uncompressed */
    /* run straddling a scan line (-1 if white, */
    /* 1 if black) */
    int uncomp_left;		/* # of bits left in the run */
    int uncomp_exit;		/* non-0 iff this is an exit run */
    /* (-1 if next run white, 1 if black) */
} stream_CFD_state;

#define private_st_CFD_state()	/* in scfd.c */\
  gs_private_st_ptrs2(st_CFD_state, stream_CFD_state, "CCITTFaxDecode state",\
    cfd_enum_ptrs, cfd_reloc_ptrs, lbuf, lprev)
#define s_CFD_set_defaults_inline(ss)\
  s_CF_set_defaults_inline(ss)
#if USE_BUILTIN_FAXD
extern const stream_template s_CFD_template;
#endif

#endif /* pts_fax.h */
