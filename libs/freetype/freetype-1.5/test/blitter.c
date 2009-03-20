/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2000 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  blitter.c: Support for blitting of bitmaps with various depth.          */
/*                                                                          */
/****************************************************************************/

#include "blitter.h"


  typedef struct TBlitter_
  {
    int  width;   /* width in pixels of the written area  */
    int  height;  /* height in pixels of the written area */

    int  xread;   /* x position of start point in read area */
    int  yread;   /* y position of start point in read area */

    int  xwrite;  /* x position of start point in write area */
    int  ywrite;  /* y position of start point in write area */

    int  right_clip;   /* amount of right clip               */

    char*  read;       /* top left corner of source map */
    char*  write;      /* top left corner of target map */

    int    read_line;  /* byte increment to go down one row in read area  */
    int    write_line; /* byte increment to go down one row in write area */

    TT_Raster_Map  source;
    TT_Raster_Map  target;

    int  source_depth;
    int  target_depth;

  } TBlitter;


  static
  int  compute_clips( TBlitter*  blit,
                      int        x_offset,
                      int        y_offset )
  {
    int  xmin, ymin, xmax, ymax, width, height, target_width;


    /* perform clipping and setup variables */
    width  = blit->source.width;
    height = blit->source.rows;

    switch ( blit->source_depth )
    {
    case 1:
      width = (width + 7) & -8;
      break;

    case 4:
      width = (width + 1) & -2;
      break;
    }

    xmin = x_offset;
    ymin = y_offset;
    xmax = xmin + width-1;
    ymax = ymin + height-1;

    /* clip if necessary */
    if ( width == 0 || height == 0                ||
         xmax < 0   || xmin >= blit->target.width ||
         ymax < 0   || ymin >= blit->target.rows  )
      return 1;

    /* set up clipping and cursors */
    blit->yread = 0;
    if ( ymin < 0 )
    {
      blit->yread  -= ymin;
      height       += ymin;
      blit->ywrite  = 0;
    }
    else
      blit->ywrite  = ymin;

    if ( ymax >= blit->target.rows )
      height -= ymax - blit->target.rows + 1;

    blit->xread = 0;
    if ( xmin < 0 )
    {
      blit->xread  -= xmin;
      width        += xmin;
      blit->xwrite  = 0;
    }
    else
      blit->xwrite  = xmin;

    target_width = blit->target.width;

    switch ( blit->target_depth )
    {
    case 1:
      target_width = (target_width + 7) & -8;
      break;
    case 4:
      target_width = (target_width + 1) & -2;
      break;
    }

    blit->right_clip = xmax - target_width + 1;
    if ( blit->right_clip > 0 )
      width -= blit->right_clip;
    else
      blit->right_clip = 0;

    blit->width  = width;
    blit->height = height;

    /* set read and write to the top-left corner of the the read */
    /* and write area before clipping.                           */

    blit->read  = (char*)blit->source.bitmap;
    blit->write = (char*)blit->target.bitmap;

    if ( blit->source.flow == TT_Flow_Up )
    {
      blit->read_line = -blit->source.cols;
      blit->read     += (blit->source.rows-1) * blit->source.cols;
    }
    else
      blit->read_line =  blit->source.cols;

    if ( blit->target.flow == TT_Flow_Up )
    {
      blit->write_line = -blit->target.cols;
      blit->write     += (blit->target.rows-1) * blit->target.cols;
    }
    else
      blit->write_line =  blit->target.cols;

    /* now go to the start line. Note that we do not move the   */
    /* x position yet, as this is dependent on the pixel format */
    blit->read  += blit->yread * blit->read_line;
    blit->write += blit->ywrite * blit->write_line;

    return 0;
  }


/**************************************************************************/
/*                                                                        */
/* <Function> blit_bitmap_to_bitmap                                       */
/*                                                                        */
/**************************************************************************/

  static
  void  blit_bitmap_to_bitmap( TBlitter*  blit )
  {
    int             shift, left_clip, x, y;
    unsigned char*  read;
    unsigned char*  write;


    left_clip = ( blit->xread > 0 );
    shift     = ( blit->xwrite - blit->xread ) & 7;

    read  = (unsigned char*)blit->read  + (blit->xread >> 3);
    write = (unsigned char*)blit->write + (blit->xwrite >> 3);

    if ( shift == 0 )
    {
      y = blit->height;
      do
      {
        unsigned char*  _read  = read;
        unsigned char*  _write = write;


        x = blit->width;

        do
        {
          *_write++ |= *_read++;
          x -= 8;
        } while ( x > 0 );

        read  += blit->read_line;
        write += blit->write_line;
        y--;
      } while ( y > 0 );
    }
    else
    {
      int  first, last, count;


      first = blit->xwrite >> 3;
      last  = (blit->xwrite + blit->width-1) >> 3;

      count = last - first;

      if ( blit->right_clip )
        count++;

      y = blit->height;

      do
      {
        unsigned char*  _read  = read;
        unsigned char*  _write = write;
        unsigned char   old;
        int             shift2 = (8-shift);


        if ( left_clip )
          old = (*_read++) << shift2;
        else
          old = 0;

        x = count;
        while ( x > 0 )
        {
          unsigned char val;


          val = *_read++;
          *_write++ |= ( (val >> shift) | old );
          old = val << shift2;
          x--;
        }

        if ( !blit->right_clip )
          *_write |= old;

        read  += blit->read_line;
        write += blit->write_line;
        y--;

      } while ( y > 0 );
    }
  }


/**************************************************************************/
/*                                                                        */
/* <Function> blit_bitmap_to_pixmap8                                      */
/*                                                                        */
/**************************************************************************/

  static
  void  blit_bitmap_to_pixmap8( TBlitter*      blit,
                                unsigned char  color )
  {
    int             x, y;
    unsigned int    left_mask;
    unsigned char*  read;
    unsigned char*  write;


    read  = (unsigned char*)blit->read  + (blit->xread >> 3);
    write = (unsigned char*)blit->write +  blit->xwrite;

    left_mask = 0x80 >> (blit->xread & 7);

    y = blit->height;
    do
    {
      unsigned char*  _read  = read;
      unsigned char*  _write = write;
      unsigned int    mask   = left_mask;
      unsigned int    val    = 0;


      x = blit->width;
      do
      {
        if ( mask == 0x80 )
          val = *_read++;

        if ( val & mask )
          *_write = (unsigned char)color;

        mask >>= 1;
        if ( mask == 0 )
          mask = 0x80;

        _write++;
        x--;
      } while ( x > 0 );

      read  += blit->read_line;
      write += blit->write_line;
      y--;
    } while ( y > 0 );
  }


/**************************************************************************/
/*                                                                        */
/* <Function> blit_bitmap_to_pixmap4                                      */
/*                                                                        */
/**************************************************************************/

  static
  void  blit_bitmap_to_pixmap4( TBlitter*      blit,
                                unsigned char  color )
  {
    int             x, y, phase;
    unsigned int    left_mask;
    unsigned char*  read;
    unsigned char*  write;


    color = color & 15;

    read  = (unsigned char*)blit->read  + (blit->xread >> 3);
    write = (unsigned char*)blit->write + (blit->xwrite >> 1);

    /* now begin blit */
    left_mask = 0x80 >> (blit->xread & 7);
    phase     = blit->xwrite & 1;

    y = blit->height;
    do
    {
      unsigned char*  _read  = read;
      unsigned char*  _write = write;
      unsigned int    mask   = left_mask;
      int             _phase = phase;
      unsigned int    val    = 0;

      x = blit->width;
      do
      {
        if ( mask == 0x80 )
          val = *_read++;

        if ( val & mask )
        {
          if ( _phase )
            *_write = (*_write & 0xF0) | color;
          else
            *_write = (*_write & 0x0F) | (color << 4);
        }

        mask >>= 1;
        if ( mask == 0 )
          mask = 0x80;

        _write += _phase;
        _phase ^= 1;
        x--;
      } while ( x > 0 );

      read  += blit->read_line;
      write += blit->write_line;
      y--;
    } while ( y > 0 );
  }


/**************************************************************************/
/*                                                                        */
/* <Function> blit_bitmap_to_pixmap16                                     */
/*                                                                        */
/**************************************************************************/

  static
  void  blit_bitmap_to_pixmap16( TBlitter*       blit,
                                 unsigned short  color )
  {
    int              x, y;
    unsigned int     left_mask;
    unsigned char*   read;
    unsigned short*  write;


    read  = (unsigned char*)blit->read + (blit->xread >> 3);
    write = (unsigned short*)(blit->write + blit->xwrite*2);

    left_mask = 0x80 >> (blit->xread & 7);

    y = blit->height;
    do
    {
      unsigned char*   _read  = read;
      unsigned short*  _write = write;
      unsigned int     mask   = left_mask;
      unsigned int     val    = 0;

      x = blit->width;
      do
      {
        if ( mask == 0x80 )
          val = *_read++;

        if ( val & mask )
          *_write = color;

        mask >>= 1;
        if ( mask == 0 )
          mask = 0x80;

        _write++;
        x--;
      } while ( x > 0 );

      read  += blit->read_line;
      write += blit->write_line;
      y--;
    } while ( y > 0 );
  }


  /* <Function>                                                          */
  /*    Blit_Bitmap                                                      */
  /*                                                                     */
  /* <Description>                                                       */
  /*    blit a source bitmap to a target bitmap or pixmap                */
  /*                                                                     */
  /* <Input>                                                             */
  /*    target       :: target bitmap or pixmap                          */
  /*    source       :: source bitmap (depth must be 1)                  */
  /*    target_depth :: pixel bit depth of target map                    */
  /*    x_offset     :: horizontal offset of source in target            */
  /*    y_offset     :: vertical offset of source in target              */
  /*    color        :: color to use when blitting to color pixmap       */
  /*                                                                     */
  /* <Return>                                                            */
  /*    error code. 0 means success                                      */
  /*                                                                     */
  /* <Note>                                                              */
  /*    an error occurs when the target bit depth isn't supported, or    */
  /*    if the source's bit depth isn't 1.                               */
  /*                                                                     */
  /*    the offsets are relative to the top-left corner of the target    */
  /*    map. Positive y are downwards.                                   */
  /*                                                                     */
  extern
  int  Blit_Bitmap( TT_Raster_Map*  target,
                    TT_Raster_Map*  source,
                    int             target_depth,
                    int             x_offset,
                    int             y_offset,
                    int             color )
  {
    TBlitter  blit;


    if ( !target || !source )
      return -1;

    blit.source       = *source;
    blit.target       = *target;
    blit.source_depth = 1;
    blit.target_depth = target_depth;

    /* set up blitter and compute clipping. Return immediately if needed */
    if ( compute_clips( &blit, x_offset, y_offset ) )
      return 0;

    /* now perform the blit */
    switch ( target_depth )
    {
    case 1:
      blit_bitmap_to_bitmap( &blit );
      break;

    case 4:
      blit_bitmap_to_pixmap4( &blit, (unsigned char)color );
      break;

    case 8:
      blit_bitmap_to_pixmap8( &blit, (unsigned char)color );
      break;

    case 16:
      blit_bitmap_to_pixmap16( &blit, (unsigned short)color );
      break;

    default:
      return -2;
    }

    return 0;
  }


/* End */
