/* -------------------------- psimage.h ------------------------- */
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifdef min
#undef min
#endif
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef int BOOL;

/* Simple prototype macros for K&R and ANSI-C */

#ifdef KNR
#define P0(v)			()
#define P1(t1, p1)		(p1) t1 p1
#define P2(t1, p1, t2, p2)	(p1, p2) t1 p1; t2 p2
#define P3(t1, p1, t2, p2, t3, p3)	(p1, p2, p3) t1 p1; t2 p2; t3 p3
#else
#define P0(v)			(void)
#define P1(t1, p1)		(t1 p1)
#define P2(t1, p1, t2, p2)	(t1 p1, t2 p2)
#define P3(t1, p1, t2, p2, t3, p3)	(t1 p1, t2 p2, t3 p3)
#endif

/* data output mode: binary, ascii85, hex-ascii */
typedef enum { BINARY, ASCII85, ASCIIHEX } DATAMODE;

typedef struct {
  FILE     *fp;                   /* file pointer for jpeg file		 */
  char     *filename;             /* name of image file			 */
  int      width;                 /* pixels per line			 */
  int      height;                /* rows				 */
  int      components;            /* number of color components		 */
  int      bits_per_component;    /* bits per color component		 */
  float    dpi;                   /* image resolution in dots per inch   */
  DATAMODE mode;                  /* output mode: 8bit, ascii, ascii85	 */
  long     startpos;              /* offset to jpeg data		 */
  BOOL     landscape;             /* rotate image to landscape mode?	 */
  BOOL     adobe;                 /* image includes Adobe comment marker */
} imagedata;

#define	DPI_IGNORE	-1.0      /* dummy value for imagedata.dpi       */
#define DPI_USE_FILE     0.0      /* dummy value for imagedata.dpi       */
