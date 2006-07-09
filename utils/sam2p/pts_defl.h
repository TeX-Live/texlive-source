/*
 * pts_defl.h -- C header file ZIP compression ripped from linux-2.6.8.1
 * by pts@fazekas.hu at Tue Jan 18 15:19:06 CET 2005
 *
 * This ZIP compression (ZIP == PostScript /FlateEncode compression filter
 * (ZLIB RFC 1950)) routine has been ripped from the Linux kernel 2.6.8.1
 * (directory lib/zlib_deflate), which has been ripped from ZLIB 1.1.3
 *
 * This is a minimal .h file, to avoid namespace pollution.
 * See more in the beginning of pts_defl.c
 * For documentation comments, see pts_defl.c
 *
 */

#ifndef PTS_DEFL_H
#define PTS_DEFL_H

#define PTS_DEFL_RIPPED_ZLIB 1

/** Must be at least zlib_deflate_workspacesize(): 267946 on Linux i386 */
#define ZLIB_DEFLATE_WORKSPACESIZE_MIN 270000

#define ZLIB_VERSION "1.1.3"

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_PACKET_FLUSH  2
#define Z_SYNC_FLUSH    3
#define Z_FULL_FLUSH    4
#define Z_FINISH        5
/* Allowed flush values; see deflate() below for details */

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field */

struct zlib_internal_state; /**** pts ****/ /* Dat: was: internal_state */

typedef struct z_stream_s {
    unsigned char *next_in;   /* next input byte */
    unsigned int     avail_in;  /* number of bytes available at next_in */
    unsigned long    total_in;  /* total nb of input bytes read so far */

    unsigned char    *next_out;  /* next output byte should be put there */
    unsigned int     avail_out; /* remaining free space at next_out */
    unsigned long    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULLP if no error */
    struct zlib_internal_state *state; /* not visible by applications */

    void     *workspace; /* memory allocated for this stream */

    int     data_type;  /* best guess about the data type: ascii or binary */
    unsigned long   adler;      /* adler32 value of the uncompressed data */
    unsigned long   reserved;   /* reserved for future use */
} z_stream;

extern int zlib_deflate_workspacesize (void);
extern int zlib_deflate (z_stream* strm, int flush);
extern int zlib_deflateEnd (z_stream* strm);
extern int zlib_deflateParams (z_stream* strm, int level, int strategy);

extern int zlib_deflateInit_ (z_stream* strm, int level,
                                     const char *version, int stream_size);
extern int zlib_deflateInit2_ (z_stream* strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size);
#define zlib_deflateInit(strm, level) \
        zlib_deflateInit_((strm), (level), ZLIB_VERSION, sizeof(z_stream))
#define zlib_deflateInit2(strm, level, method, windowBits, memLevel, strategy) \
        zlib_deflateInit2_((strm),(level),(method),(windowBits),(memLevel),\
                      (strategy), ZLIB_VERSION, sizeof(z_stream))

#endif
