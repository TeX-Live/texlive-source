/*
 * in_pcx.cpp: loads PCX images
 * modified by pts@fazekas.hu at Fri Apr 12 22:16:08 CEST 2002
 * -- Fri Apr 12 23:54:57 CEST 2002
 *
 * xvpcx.c - load routine for PCX format pictures
 *
 * LoadPCX(fname, pinfo)  -  loads a PCX file
 */

/**** pts ****/
#include "config2.h"
#include "image.hpp"

#if USE_IN_PCX
#include "error.hpp"
#include "gensio.hpp"
#include <string.h>


/* Imp: palette handling etc. according to PCX_VER, see decode.c */
#define dimen Image::Sampled::dimen_t
#define pcxError(bname,conststr) Error::sev(Error::WARNING) << "PCX: " conststr << (Error*)0
#define WaitCursor()
#define xvbzero(p,len) memset(p, '\0', len)
#define FatalError(conststr) Error::sev(Error::EERROR) << "PCX: " conststr << (Error*)0
#define return_pcxError(bname, conststr) Error::sev(Error::EERROR) << "PCX: " conststr << (Error*)0
#define byte unsigned char
#define size_t slen_t
#define malloc_byte(n) new byte[n]
#define free(p) delete p
#define DEBUG 1
/* the following list give indicies into saveColors[] array in xvdir.c */
#define F_FULLCOLOR 0
#define F_GREYSCALE 1
#define F_BWDITHER  2
/* values 'picType' can take */
#define PIC8  8
#define PIC24 24
#define xv_fopen(filename,read_mode) fopen(filename,"rb")
#define BaseName(x) ((char*)0)
#define PARM(parm) parm
/* info structure filled in by the LoadXXX() image reading routines */

typedef struct { byte *pic;                  /* image data */
		 dimen w, h;                 /* pic size */
#if 0 /**** pts ****/
		 byte  r[256],g[256],b[256];
#else
                 /* byte pal[3*256]; */
                 byte *pal;
#  define PAL_R(pinfo,idx) (pinfo)->pal[3*(idx)]
#  define PAL_G(pinfo,idx) (pinfo)->pal[3*(idx)+1]
#  define PAL_B(pinfo,idx) (pinfo)->pal[3*(idx)+2]
#endif		                             /* colormap, if PIC8 */
#if 0 /**** pts ****/
		 int   colType;              /* def. Color type to save in */
		 int   type;                 /* PIC8 or PIC24 */
		 int   normw, normh;         /* 'normal size' of image file
					        (normally eq. w,h, except when
						doing 'quick' load for icons */
		 int   frmType;              /* def. Format type to save in */
		 char  fullInfo[128];        /* Format: field in info box */
		 char  shrtInfo[128];        /* short format info */
		 char *comment;              /* comment text */
		 int   numpages;             /* # of page files, if >1 */
		 char  pagebname[64];        /* basename of page files */
#endif
	       } PICINFO;


/* #include "copyright.h" */

/*
 * the following code has been derived from code written by
 *  Eckhard Rueggeberg  (Eckhard.Rueggeberg@ts.go.dlr.de)
 */


/* #include "xv.h" */

/* offsets into PCX header */
#define PCX_ID      0
#define PCX_VER     1
#define PCX_ENC     2
#define PCX_BPP     3
#define PCX_XMINL   4
#define PCX_XMINH   5
#define PCX_YMINL   6
#define PCX_YMINH   7
#define PCX_XMAXL   8
#define PCX_XMAXH   9
#define PCX_YMAXL   10
#define PCX_YMAXH   11
                          /* hres (12,13) and vres (14,15) not used */
#define PCX_CMAP    16    /* start of 16*3 colormap data */
#define PCX_PLANES  65 
#define PCX_BPRL    66
#define PCX_BPRH    67

#define PCX_MAPSTART 0x0c	/* Start of appended colormap	*/


static int  pcxLoadImage8  PARM((char *, FILE *, PICINFO *, byte *));
static int  pcxLoadImage24 PARM((char *, FILE *, PICINFO *, byte *));
static void pcxLoadRaster  PARM((FILE *, byte *, int, byte *, dimen, dimen));
#if 0 /**** pts ****/
static int  pcxError       PARM((char *, char *));
#endif


/*******************************************/
static Image::Sampled *LoadPCX
#if 0 /**** pts ****/
  ___((char *fname, PICINFO *pinfo), (fname, pinfo), (char    *fname; PICINFO *pinfo;))
#else
  ___((FILE *fp, PICINFO *pinfo), (fname, pinfo), (char    *fname; PICINFO *pinfo;))
#endif
/*******************************************/
{
  Image::Sampled *ret=(Image::Sampled*)NULLP;
  byte   hdr[128];
#if 0 /**** pts ****/
  long   filesize;
  char  *bname;
  FILE  *fp;
  char *errstr; byte *image;
  int gray;
#endif
  int    i, colors, fullcolor;

  pinfo->pic     = (byte *) NULL;
  pinfo->pal     = (byte *) NULL;
#if 0 /**** pts ****/
  pinfo->type = PIC8;
  pinfo->comment = (char *) NULL;
  bname = BaseName(fname);

  /* open the stream */
  fp = xv_fopen(fname,"r");
  if (!fp) return_pcxError(bname, "unable to open file");
#endif

#if 0 /**** pts ****/
  /* figure out the file size */
  fseek(fp, 0L, 2);
  filesize = ftell(fp);
  fseek(fp, 0L, 0);
#endif

  /* read the PCX header */
  fread(hdr, (size_t) 128, (size_t) 1, fp);
  if (ferror(fp) || feof(fp)) {
    /* fclose(fp); */
    return_pcxError(bname, "EOF reached in PCX header.\n");
  }

  if (hdr[PCX_ID] != 0x0a || hdr[PCX_VER] > 5) {
    /* fclose(fp); */
    return_pcxError(bname,"unrecognized magic number");
  }

  pinfo->w = (hdr[PCX_XMAXL] + ((int) hdr[PCX_XMAXH]<<8)) 
           - (hdr[PCX_XMINL] + ((int) hdr[PCX_XMINH]<<8));

  pinfo->h = (hdr[PCX_YMAXL] + ((int) hdr[PCX_YMAXH]<<8)) 
           - (hdr[PCX_YMINL] + ((int) hdr[PCX_YMINH]<<8));

  pinfo->w++;  pinfo->h++;

  colors = 1 << (hdr[PCX_BPP] * hdr[PCX_PLANES]);
  fullcolor = (hdr[PCX_BPP] == 8 && hdr[PCX_PLANES] == 3);

#if USE_DEBUG_MESSAGES
  if (DEBUG) {
    fprintf(stderr,"PCX: %dx%d image, version=%d, encoding=%d\n", 
	    pinfo->w, pinfo->h, hdr[PCX_VER], hdr[PCX_ENC]);
    fprintf(stderr,"   BitsPerPixel=%d, planes=%d, BytePerRow=%d, colors=%d\n",
	    hdr[PCX_BPP], hdr[PCX_PLANES], 
	    hdr[PCX_BPRL] + ((int) hdr[PCX_BPRH]<<8),
	    colors);
  }
#endif

  if (colors>256 && !fullcolor) {
    /* fclose(fp); */
    return_pcxError(bname,"No more than 256 colors allowed in PCX file.");
  }

  if (hdr[PCX_ENC] != 1) {
    /* fclose(fp); */
    return_pcxError(bname,"Unsupported PCX encoding format.");
  }

  /* load the image, the image function fills in pinfo->pic */
  if (!fullcolor) {
    Image::Indexed *img=new Image::Indexed(pinfo->w, pinfo->h, colors, 8);
    pinfo->pal=(byte*)img->getHeadp();
    ASSERT_SIDE(pcxLoadImage8((char*)NULLP/*bname*/, fp, pinfo, hdr));
    memcpy(img->getRowbeg(), pinfo->pic, pinfo->w*pinfo->h);
    ret=img;
  } else {
    Image::RGB *img=new Image::RGB(pinfo->w, pinfo->h, 8);
    ASSERT_SIDE(pcxLoadImage24((char*)NULLP/*bname*/, fp, pinfo, hdr));
    memcpy(img->getRowbeg(), pinfo->pic, pinfo->w*pinfo->h*3);
    ret=img;
  }
  free(pinfo->pic);
  pinfo->pic=(byte*)NULLP;


  if (ferror(fp) | feof(fp))    /* just a warning */
    pcxError(bname, "PCX file appears to be truncated.");

  if (colors>16 && !fullcolor) {       /* handle trailing colormap */
    while (1) {
      i=MACRO_GETC(fp);
      if (i==PCX_MAPSTART || i==EOF) break;
    }

#if 0 /**** pts ****/
    for (i=0; i<colors; i++) {
      PAL_R(pinfo,i) = MACRO_GETC(fp);
      PAL_G(pinfo,i) = MACRO_GETC(fp);
      PAL_B(pinfo,i) = MACRO_GETC(fp);
    }
#else
    fread(pinfo->pal, 1, colors*3, fp);
#endif

    if (ferror(fp) || feof(fp)) {
      pcxError(bname,"Error reading PCX colormap.  Using grayscale.");
      for (i=0; i<256; i++) PAL_R(pinfo,i) = PAL_G(pinfo,i) = PAL_B(pinfo,i) = i;
    }
  }
  else if (colors<=16) {   /* internal colormap */
#if 0 /**** pts ****/
    for (i=0; i<colors; i++) {
      PAL_R(pinfo,i) = hdr[PCX_CMAP + i*3];
      PAL_G(pinfo,i) = hdr[PCX_CMAP + i*3 + 1];
      PAL_B(pinfo,i) = hdr[PCX_CMAP + i*3 + 2];
    }
#else
    memcpy(pinfo->pal, hdr+PCX_CMAP, colors*3);
#endif
  }

  if (colors == 2) {    /* b&w */
#if 0 /**** pts ****/
    if (MONO(PAL_R(pinfo,0), PAL_G(pinfo,0), PAL_B(pinfo,0)) ==
	MONO(PAL_R(pinfo,1), PAL_G(pinfo,1), PAL_B(pinfo,1))) {
#else
    if (PAL_R(pinfo,0)==PAL_R(pinfo,1) && PAL_G(pinfo,0)==PAL_G(pinfo,1) && PAL_B(pinfo,0)==PAL_B(pinfo,1)) {
#endif
      /* create cmap */
      PAL_R(pinfo,0) = PAL_G(pinfo,0) = PAL_B(pinfo,0) = 255;
      PAL_R(pinfo,1) = PAL_G(pinfo,1) = PAL_B(pinfo,1) = 0;
#if USE_DEBUG_MESSAGES
      if (DEBUG) fprintf(stderr,"PCX: no cmap:  using 0=white,1=black\n");
#endif
    }
  }
  /* fclose(fp); */

  /* finally, convert into XV internal format */
#if 0 /**** pts ****/
  pinfo->type    = fullcolor ? PIC24 : PIC8;
  pinfo->frmType = -1;    /* no default format to save in */
#endif

#if 0 /**** pts ****/
  /* check for grayscaleitude */
  gray = 0;
  if (!fullcolor) {
    for (i=0; i<colors; i++) {
      if ((PAL_R(pinfo,i) != PAL_G(pinfo,i)) || (PAL_R(pinfo,i) != PAL_B(pinfo,i))) break;
    }
    gray = (i==colors) ? 1 : 0;
  }


  if (colors > 2 || (colors==2 && !gray)) {  /* grayscale or PseudoColor */
    pinfo->colType = (gray) ? F_GREYSCALE : F_FULLCOLOR;
#if 0 /**** pts ****/
    sprintf(pinfo->fullInfo, 
	    "%s PCX, %d plane%s, %d bit%s per pixel.  (%ld bytes)", 
	    (gray) ? "Greyscale" : "Color", 
	    hdr[PCX_PLANES], (hdr[PCX_PLANES]==1) ? "" : "s",
	    hdr[PCX_BPP],    (hdr[PCX_BPP]==1) ? "" : "s",
	    filesize);
#endif
  }
  else {
    pinfo->colType = F_BWDITHER;
#if 0 /**** pts ****/
    sprintf(pinfo->fullInfo, "B&W PCX.  (%ld bytes)", filesize);
#endif
  }

#if 0 /**** pts ****/
  sprintf(pinfo->shrtInfo, "%dx%d PCX.", pinfo->w, pinfo->h);
  pinfo->normw = pinfo->w;   pinfo->normh = pinfo->h;
#endif
#endif

  return ret;
}



/*****************************/
static int pcxLoadImage8 ___((char *fname, FILE *fp, PICINFO *pinfo, byte *hdr), (fname, fp, pinfo, hdr),
    (char    *fname;
     FILE    *fp;
     PICINFO *pinfo;
     byte    *hdr;))
{
  /* load an image with at most 8 bits per pixel */
  (void)fname; /**** pts ****/
  
  byte *image;
  
  /* note:  overallocation to make life easier... */
  image = (byte *) malloc_byte((size_t) (pinfo->h + 1) * pinfo->w + 16);
  if (!image) FatalError("Can't alloc 'image' in pcxLoadImage8()");
  
  xvbzero((char *) image, (size_t) ((pinfo->h+1) * pinfo->w + 16));
  
  switch (hdr[PCX_BPP]) {
  case 1: case 2: case 4: case 8: pcxLoadRaster(fp, image, hdr[PCX_BPP], hdr, pinfo->w, pinfo->h);   break;
  default:
    free(image);
    return_pcxError(fname, "Unsupported # of bits per plane.");
  }

  pinfo->pic = image;
  return 1;
}


/*****************************/
static int pcxLoadImage24 ___((char *fname, FILE *fp, PICINFO *pinfo, byte *hdr), (fname, fp, pinfo, hdr),
    (char *fname;
     FILE *fp;
     PICINFO *pinfo;
     byte *hdr;))
{
  byte *pix, *pic24;
  int   c;
  unsigned i, j, w, h, cnt, planes, bperlin, nbytes;
#if 0 /***** pts ****/
  int maxv; /* ImageMagick does not have one */
  byte scale[256];
#endif

  (void)fname; /**** pts ****/
  
  w = pinfo->w;  h = pinfo->h;
  
  planes = (int) hdr[PCX_PLANES];
  bperlin = hdr[PCX_BPRL] + ((int) hdr[PCX_BPRH]<<8);
  
  /* allocate 24-bit image */
  pic24 = (byte *) malloc_byte((size_t) w*h*planes);
  if (!pic24) FatalError("couldn't malloc 'pic24'");
  
  xvbzero((char *) pic24, (size_t) w*h*planes);
  
#if 0 /**** pts ****/
  maxv = 0;
#endif  
  pix = pinfo->pic = pic24;
  i = 0;      /* planes, in this while loop */
  j = 0;      /* bytes per line, in this while loop */
  nbytes = bperlin*h*planes;
 
  while (nbytes > 0 && (c = MACRO_GETC(fp)) != EOF) {
    if (c>=0xC0) {   /* have a rep. count */
      cnt = c & 0x3F;
      c = MACRO_GETC(fp);
      if (c == EOF) { MACRO_GETC(fp); break; }
    }
    else cnt = 1;
    
#if 0 /**** pts ****/
    if (c > maxv)  maxv = c;
#endif
    
    while (cnt-- > 0) {
      if (j < w) {
	*pix = c;
	pix += planes;
      }
      j++;
      nbytes--;
      if (j == bperlin) {
	j = 0;
	if (++i < planes) {
	  pix -= (w*planes)-1;  /* next plane on this line */
	}
	else {
	  pix -= (planes-1);    /* start of next line, first plane */
	  i = 0;
	}
      }
    }
  }
  
  
#if 0 /**** pts ****/  
  /* scale all RGB to range 0-255, if they aren't */

  if (maxv<255) {
    for (i=0; i<=maxv; i++) scale[i] = (i * 255) / maxv;
    
    for (i=0, pix=pic24; i<h; i++) {
      if ((i&0x3f)==0) WaitCursor();
      for (j=0; j<w*planes; j++, pix++) *pix = scale[*pix];
    }
  }
#endif
  
  return 1;
}



/*****************************/
static void pcxLoadRaster ___((FILE *fp, byte *image, int depth, byte *hdr, dimen w, dimen h), (fp, image, depth, hdr, w, h),
    (FILE    *fp;
     byte    *image, *hdr;
     int      depth;
     dimen w,h;))
{
  /* was supported:  8 bits per pixel, 1 plane, or 1 bit per pixel, 1-8 planes */

  unsigned row, bcnt, bperlin, pad, cnt, pmask, i, pleft;
  int b;
  byte *oldimage;

  bperlin = hdr[PCX_BPRL] + ((int) hdr[PCX_BPRH]<<8);
  if (depth == 1) pad = (bperlin * 8) - w;
             else pad = bperlin - w;

  row = bcnt = 0;

  pmask = 1;  oldimage = image;
  pleft=hdr[PCX_PLANES];

  while ( (b=MACRO_GETC(fp)) != EOF) {
    if (b>=0xC0) {   /* have a rep. count */
      cnt = b & 0x3F;
      b = MACRO_GETC(fp);
      if (b == EOF) { MACRO_GETC(fp); return; }
    }
    else cnt = 1;
    
    for (i=0; i<cnt; i++) {
      switch (depth) {
       case 1:
        *image++|=(b&0x80)?pmask:0;
        *image++|=(b&0x40)?pmask:0;
        *image++|=(b&0x20)?pmask:0;
        *image++|=(b&0x10)?pmask:0;
        *image++|=(b&0x8)?pmask:0;
        *image++|=(b&0x4)?pmask:0;
        *image++|=(b&0x2)?pmask:0;
        *image++|=(b&0x1)?pmask:0;
        break;
       case 2: /**** pts ****/
        *image++|=((b>>6)&3)*pmask;
        *image++|=((b>>4)&3)*pmask;
        *image++|=((b>>2)&3)*pmask;
        *image++|=((b   )&3)*pmask;
        break;
       case 4: /**** pts ****/
        *image++|=((b>>4)&15)*pmask;
        *image++|=((b   )&15)*pmask;
        break;
       default:
        *image++=(byte)b;
      }
      
      bcnt++;
	
      if (bcnt == bperlin) {     /* end of a line reached */
	bcnt = 0;

	if (--pleft==0) {   /* moved to next row */
          pleft=hdr[PCX_PLANES];
	  pmask=1;
	  image -= pad;
	  oldimage = image;
	  row++;
	  if (row >= h) return;   /* done */
	}
	else {   /* next plane, same row */
	  image = oldimage;
	  pmask<<=depth;
	}	
      }
    }
  }
}    

#if 0 /**** pts ****/
/*******************************************/
static int pcxError(fname,st)
     char *fname, *st;
{
  SetISTR(ISTR_WARNING,"%s:  %s", fname, st);
  return 0;
}
#endif

static Image::Sampled *in_pcx_reader(Image::Loader::UFD *ufd, SimBuffer::Flat const&) {
  PICINFO pinfo_;
  return LoadPCX(((Filter::UngetFILED*)ufd)->getFILE(/*seekable:*/false), &pinfo_);
}
static Image::Loader::reader_t in_pcx_checker(char buf[Image::Loader::MAGIC_LEN], char [Image::Loader::MAGIC_LEN], SimBuffer::Flat const&, Image::Loader::UFD*) {
  return buf[PCX_ID]==0x0a
      && (unsigned char)buf[PCX_VER]<=5
      && buf[PCX_ENC]==1
      && buf[PCX_BPP]<=8
       ? in_pcx_reader : 0;
}

#else
#define in_pcx_checker (Image::Loader::checker_t)NULLP
#endif /* USE_IN_PCX */

Image::Loader in_pcx_loader = { "PCX", in_pcx_checker, 0 };

/* __END__ */
