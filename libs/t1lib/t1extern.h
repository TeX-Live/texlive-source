/*--------------------------------------------------------------------------
  ----- File:        t1extern.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-10-03
  ----- Description: This file is part of the t1-library. It contains
                     external declarations used by the t1-library.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2001. 
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  

#include "t1misc.h"


/* Following struct will allow all accesses to font data! */
extern struct FONTBASE
{
  int t1lib_flags;    /* Global library flags */
  int no_fonts_ini;   /* The number of fonts initially declared in
			 Font database file. */
  int no_fonts;       /* The number of currently allocated fonts including
			 logical fonts produced by T1_CopyFont() */
  int no_fonts_limit; /* The maximum number of font for which is memory
			 currently available. */
  int bitmap_pad;     /* The value to which bitmap-scanlines are padded */
  int endian;         /* 1 if little endian representation and 0 otherwise */
  char **default_enc; /* The default encoding vector */
  FONTPRIVATE *pFontArray; /* Points to first element of font pointer array */
} FontBase;
extern struct FONTBASE *pFontBase;


/* Further we need a struct where to save device dependent data such as
   resolution. The values in this struct may be altered using
   T1_SetDeviceRes(x_res,y_res). Generally, the fontsize is to be specified
   in BigPoints ("bp")---this is the default PostScript unit. It is
   1in=72bp.
   */
extern struct 
{
  float x_resolution;       /* Value must be specified in DPI */
  float y_resolution;       /* Value must be specified in DPI */
  float scale_x;            /* horizontal scale-value to get a
			       matrix scaled to 1b */
  float scale_y;             /* vertical scale-value to get a
			       matrix scaled to 1b */
} DeviceSpecifics;


extern struct stat filestats; /* A structure where fileinfo is stored */
extern char linebuf[BUF_SIZE];   /* A buffer for reading files line
					     by line */
extern int T1_Up;     /* This one is for initialization-checking */

/* The width of AA-pixels */
extern int T1aa_bpp;

/* The follwoing variable allows the primary rastering functions
   to check for the caller */
extern int SetFuncRequestID;

/* The following variable allows to suppress rastering at 1000 bp
   for getting a correct bounding box for slanted characters -->
   should only internally be used */
extern int ForceAFMBBox;

extern struct XYspace *IDENTITY; 

extern int errornumber;  /* for debugging purposes */
 
extern char **T1_PFAB_ptr;
extern char **T1_AFM_ptr;
extern char **T1_ENC_ptr;
extern char **T1_FDB_ptr;

/* We use a uchar buffer for error and warning messages: */
extern char err_warn_msg_buf[1024];

/* file pointer for log-file */
extern FILE *t1lib_log_file;
extern int  t1lib_log_level;

/* The errno for t1lib */
extern int T1_errno;

/* A variable for saving stack info */
extern jmp_buf stck_state;

