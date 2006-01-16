/*******************************************************************
 *
 *  gmain.h   graphics utility main interface                   1.1
 *
 *  This file defines a common interface, implemented in the body
 *  file 'gmain.c'.  It relies on system dependent driver files,
 *  like 'gfs_os.c', whose interface is described in 'gdriver.h'.
 *
 *  Copyright 1996-2001 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#ifndef GMAIN_H
#define GMAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Graphics mode definitions */

#define Graphics_Mode_Mono  1    /* monochrome graphics mode    */
#define Graphics_Mode_Gray  2    /* 8-bit palette graphics mode */

  extern char*  Vio;       /* Pointer to VRAM or display buffer */

  extern int    vio_ScanLineWidth;  /* Scan Line width in bytes */

  extern int    vio_Width;
  extern int    vio_Height;

  extern unsigned char  gray_palette[5];    /* standard gray_palette */


  /* set a graphics mode, chosen from the Graphics_Mode_xxx list */
  int  SetGraphScreen( int  mode );

  /* restore previous (or text) video mode */
  int   RestoreScreen( void );

  /* display a bitmap of 'line' lines, and 'col' columns (each */
  /* column made of 8 bits)                                    */
  void  Display_Bitmap_On_Screen( char*  buffer, int  line, int  col );

  void  Goto_XY( int  x, int  y );


  extern int  gcursor_x;
  extern int  gcursor_y;

  extern int  gwindow_width;
  extern int  gwindow_height;

  void  Print_Str( char*  string );
  void  Print_XY ( int  x, int  y, char*  string );

#ifdef __cplusplus
}
#endif

#endif /* GMAIN_H */


/* End */
