/*
 * pts_lzw.h -- a compact LZW compressor (not uncompressor) interface
 * compiled by pts@fazekas.hu at Mon Mar  4 00:31:20 CET 2002
 */

#ifndef PTS_LZW_H
#define PTS_LZW_H 1

#ifdef __GNUC__
#pragma interface
#endif

/**** pts ****/
#ifndef ___
#if (defined(__STDC__) || defined(__cplusplus)) && !defined(NO_PROTO)
# define _(args) args
# define OF(args) args
# define ___(arg2s,arg1s,argafter) arg2s /* Dat: no direct comma allowed in args :-( */
#else
# define _(args) ()
# define OF(args) ()
# define ___(arg2s,arg1s,argafter) arg1s argafter /* Dat: no direct comma allowed in args :-( */
#endif
#endif
typedef char tbool_t;
typedef	unsigned char tidataval_t;	/* internal image data value type */
typedef	unsigned int tsize_t;		/* i/o size in bytes */
typedef int  (*tiwriter_t) OF((char *block, unsigned len, void *zfile));
struct pts_lzw_state;
struct pts_lzw_state {
	/*FILE*/void*	tif_sout;       /**** pts ****/ /* for encode */
	tiwriter_t     tif_writer;
        int (*tif_feeder)(char *readbuf, unsigned readlen, struct pts_lzw_state *tif);
#if 0
	tbool_t		tif_revbits_p;  /**** pts ****/
#endif
	tbool_t		tif_reading_p;  /**** pts ****/
	char*		tif_name;	/* name of open file, used for debugging */
/* compression scheme hooks */
	tidataval_t*	tif_data;	/* compression scheme private data */
/* input/output buffering */
        tidataval_t*    tif_rawend;     /**** pts ****/
	tidataval_t*	tif_rawdata;	/* raw data buffer */
	tsize_t		tif_rawdatasize;/* # of bytes in raw data buffer */
	tidataval_t*	tif_rawcp;	/* current spot in raw buffer */
	tsize_t		tif_rawcc;	/* bytes unread from raw buffer */
};

extern
#ifdef __cplusplus
"C"
#endif
int pts_lzw_init OF((struct pts_lzw_state*));

#endif
