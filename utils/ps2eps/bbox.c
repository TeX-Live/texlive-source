/********************************************************************/
/** bbox -- calculates Bounding Box of a pbmraw/ppmraw-picture     **/
/** Created:   Nov. 1997, revised Feb. 1998, Dec. 1999             **/
/** Author:    Roland Bless <roland@bless.de>                      **/
/** Copyright (C) 1998-2000 Roland Bless                           **/
/** To compile simply use:                                         **/
/** "cc bbox.c -o bbox" or "make bbox"                             **/
/********************************************************************/

/*
 * $Id: bbox.c,v 1.13 2004/01/25 10:15:48 bless Exp $
 *
 * $Log: bbox.c,v $
 * Revision 1.13  2004/01/25 10:15:48  bless
 * - added %%HiResBoundingBox: output (now compatible with bbox device output of ghostscript)
 *
 * Revision 1.12  2003/11/09 18:16:53  bless
 * - print help, license and version to stdout instead of stderr (incorporated patch from Rafael Laboissiere <rafael@debian.org>)
 *
 * Revision 1.11  2003/07/08 11:14:12  bless
 * - added include <string.h> for strcmp()
 * - removed unused argument in fprintf() call
 *
 * Revision 1.10  2000/08/04 08:46:47  bless
 * Incorporated bugfix by Dan Blake <dblake@rose.hp.com>: read
 * unsigned int into unsigned char variable via sscanf caused crash
 * under HP-UX. Thanks, Dan!
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <io.h>	  /* needed for _setmode() */
#include <fcntl.h>
#endif

/**********************
*  global variables   *
**********************/
const char *const version= "$Revision: 1.13 $ $Date: 2004/01/25 10:15:48 $";
const char *const prgname= "bbox";

unsigned char bitval[8]=
{
  1 << 7,
  1 << 6,
  1 << 5,
  1 << 4,
  1 << 3,
  1 << 2,
  1 << 1,
  1
};

static unsigned int
minus_one(const unsigned x) 
{
  return (x == 0) ? x : x-1;
}

static unsigned int
plus_one(const unsigned x) 
{
  return (x == (unsigned int) ~0U) ? x : x+1;
}

/***************************** readppm_and_calcbb ***********************
*	input: 	name, resolution, tight                         	*
*	output: 	- (stdout)           				*
*                                                                       *
*     	Reads a RAWPPM or RAWPBM picture file (name or STDIN) and       *
*       calculates its Bounding Box on-the-fly (line-by-line).          *
*                                                                       *
*       Parameters:                                                     *
*       name: name of the PBMRAW file or NULL (input from stdin)        *
*       resolution: Pixel per Inch (DPI)                                *
*       tight: boolean value, if false 1 Postscript Point is added to   *
               each Bounding Box parameter, otherwise the BB is tight   *
*       The Bounding Box is given in Postscript Points  (72 dots/inch)  *
*       and printed to stdout                                           *
************************************************************************/
/* calculate the bounding box in postscript points, given a resolution in dpi */
static void
readppm_and_calcbb(const char *name, 
                   const unsigned int resolution, 
                   const unsigned char tight)
{
	FILE 	*inputfile;
        char    inputline[1024];
        unsigned char magic_found= 0;
        int x,y,byte_x,i;
        unsigned int x_min, x_max, y_min, y_max;
        unsigned int llx, lly, urx, ury; /* bounding box */
        double       hllx, hlly, hurx, hury; /* hires bounding box */
        unsigned char   *image_row,	 /* ImageRow */
                        *tmprowp;
        unsigned int 	xmax,ymax;	 /* Image Size	*/ 
        unsigned int    byte_xmax;
        unsigned char   stepsize;
        unsigned char   colmax= 0;       /* max color value */
        unsigned int    ui_colmax= 0;    /* max color value */
        
 	if ( name == NULL ) 
        {
          inputfile = stdin;
          name = "- STDIN -";
	}
	else 	
        {
          inputfile = fopen(name,"r");
          if ( inputfile == NULL )
          {
            fprintf(stderr,"%s: ERROR -- could not open file %s\n", 
                    prgname, name);
            return;
          }
        }
        /** check for magic number **/
        do 
        {
          fgets(inputline, 1024, inputfile);
#ifdef DEBUG
          fprintf(stderr,"read:[%s]\n",inputline);
#endif
          if ( strcmp(inputline,"P4\n") == 0 )
          {
            stepsize= 1;
            magic_found= 4;
          }
          else
          if ( strcmp(inputline,"P6\n") == 0 )
          {
            stepsize= 3;
            magic_found= 6;
          }
        }
        while ( !feof(inputfile) && !magic_found );
        
        if ( !magic_found )
        {
          fprintf(stderr,"%s: ERROR -- %s is not in ppmraw or pbmraw format\n",
                  prgname, name);
          return;
        }
        /** skip comments **/
        do 
        {
          fgets(inputline, 1024, inputfile);
#ifdef DEBUG
          fprintf(stderr,"read:[%s]\n",inputline);
#endif
          if (*inputline == '#')
            continue;
          else
            break;
        }
        while ( !feof(inputfile) );
        /** read picture size: width, height **/
        sscanf(inputline,"%u %u",&xmax,&ymax);
        if ( magic_found == 6 ) /* PPM file has maximum color-component value */
        {
          fgets(inputline, 1024, inputfile);
          sscanf(inputline,"%u",&ui_colmax);  
	  colmax = (unsigned char) ui_colmax; /* this is safer */
        }
#ifdef DEBUG
	fprintf(stderr,"\nreading picture: %s   X: %u   Y: %u\n",name,xmax,ymax);
#endif
        x = 0;		/* avoid uninitialized warning */
        x_min= xmax; 
        x_max= 0; 
        y_min= ymax; 
        y_max= 0;
        if ( magic_found == 4 ) /* PBMRAW = Bitmap */
        { /** read raw pbmfile **/
          byte_xmax= xmax / 8;
          if (xmax % 8 != 0)
            byte_xmax++;
        }
        else /** assume ppm raw **/
        { 
          byte_xmax= xmax * 3; /* we have RGB, i.e. three bytes for each pixel */
        }

        image_row= malloc(byte_xmax);
        if ( image_row )
        {
#if defined(_WIN32) && !defined(__CYGWIN__)  /* this is really braindead stuff for MSVC */
	  i= _setmode( _fileno(stdin), _O_BINARY);
	  if (i == -1)
	    fprintf(stderr,"%s: ERROR - Cannot set binary mode for STDIN\n", prgname);
#endif
          for (y= 0; y<ymax-1; y++) /* for every image row */
          {
            if (fread(image_row, byte_xmax, 1, inputfile) != 1)
            {
              fprintf(stderr,"%s: WARNING -- fread incomplete - file %s seems to be corrupt\n", prgname, name);
              break;
            }
            tmprowp= image_row;
            /* inspect this line from the left */
            for (byte_x= 0; byte_x<byte_xmax; byte_x++,tmprowp++)
            {
              if (*tmprowp != colmax) /* there are pixels not white */
              {
                if (magic_found == 4)
                {
                  for (i= 0; i<8 ; i++)
                  {
                    if (*tmprowp & bitval[i])
                    {
                      x= byte_x*8+i;
                      if ( x >= xmax ) break;
#ifdef DEBUG
                      printf("(%04d,%04d): <not white>\n",y,x);
#endif
                    }
                  } /* end for */
                } /* end if magic_found 4 */
                else
                { /* assume PPM */
                  x= byte_x/3; /* we have 3 bytes per pixel */
#ifdef DEBUG
                      printf("(%04d,%04d)%04d: <not %d>\n",y,x,byte_x,colmax);
#endif
                }
                /* update bounding box */
                if ( x < x_min ) x_min= x;
                if ( x > x_max ) x_max= x;
                if ( y < y_min ) y_min= y;
                if ( y > y_max ) y_max= y;
#ifdef DEBUG
                printf("ymin,ymax:(%04d,%04d) xmin,xmax:(%04d,%04d)",
                       y_min,y_max,x_min,x_max);
#endif
                break; /* stop here */
              } /* if there are pixels not white */ 
            } /* end for byte_x */
            if ( byte_x != byte_xmax )
            { /* there was a pixel with no background color */
              tmprowp= image_row+byte_xmax-1;
              /* inspect this line from the right */
              for (byte_x= byte_xmax-1; 
                   byte_x >= 0; 
                   byte_x--,tmprowp--)
              {
                if ( *tmprowp != colmax ) /* there are pixels not white */
                {
                  if ( magic_found == 4 )
                  {
                    for (i= 0; i<8 ; i++)
                    {
                      if ( *tmprowp & bitval[i] )
                      {
                        x= byte_x*8+i;
                        if (x >= xmax) break;
#ifdef DEBUG
                        printf("(%04d,%04d): <not white>\n",y,x);
#endif
                      }
                    } /* end for */
                  } /* end if magic_found 4 */
                  else
                  { /* assume PPM */
                    x= byte_x/3; /* we have 3 bytes per pixel */
                  }
                  /* update bounding box */
                  if ( x < x_min ) x_min= x;
                  if ( x > x_max ) x_max= x;
                  if ( y < y_min ) y_min= y;
                  if ( y > y_max ) y_max= y;
                  break;
                } /* if there are pixels not white */ 
              } /* end for byte_x */
            } /* if line contained not only background color */
          } /* end for y */
#ifdef DEBUG_BOX
          fprintf(stderr,"(%04d,%04d), (%04d,%04d)\n", x_min,ymax-y_max,x_max,ymax-y_min);
#endif
            /* distance from the left edge to the leftmost point */
            hllx= (x_min*72.0)/resolution;
            /* distance from the bottom edge to the bottommost point */
            hlly= ((ymax-y_max)*72.0)/resolution;
            /* distance from the left edge to the righmost point  */ 
            hurx= (x_max*72.0)/resolution;
            /* distance from the bottom edge to the uppermost point */
            hury= ((ymax-y_min)*72.0)/resolution;


          if ( !tight )
          {
            /* distance from the left edge to the leftmost point */
            llx= minus_one((unsigned int) ((unsigned long) x_min*72UL)/resolution);
            /* distance from the bottom edge to the bottommost point */
            lly= minus_one((unsigned int) ((unsigned long) (ymax-y_max)*72UL)/resolution);
            /* distance from the left edge to the righmost point  */ 
            urx= plus_one((unsigned int) ((unsigned long) x_max*72UL)/resolution);
            /* distance from the bottom edge to the uppermost point */
            ury= plus_one((unsigned int) ((unsigned long) (ymax-y_min)*72UL)/resolution);
          }
          else /* tight bounding box */
          {
            /* distance from the left edge to the leftmost point */
            llx= (unsigned int) ((unsigned long) x_min*72UL)/resolution;
            /* distance from the bottom edge to the bottommost point */
            lly= (unsigned int) ((unsigned long) (ymax-y_max)*72UL)/resolution;
            /* distance from the left edge to the righmost point  */ 
            urx= (unsigned int) ((unsigned long) x_max*72UL)/resolution;
            if ( (((unsigned long) x_max*72UL) % resolution) != 0 )
              urx= plus_one(urx);
            /* distance from the bottom edge to the uppermost point */
            ury= (unsigned int) ((unsigned long) (ymax-y_min)*72UL)/resolution;
            if ( (((unsigned long) (y_max-y_min)*72UL) % resolution) != 0 )
              ury= plus_one(ury);
          }
          /* skip the rest of the file if any data is still present */
          while ( !feof(inputfile) )
          {
            fgets(inputline, 1024, inputfile);
          }

          /* give out Bounding Box */
          printf("%%%%BoundingBox: %d %d %d %d\n", llx, lly, urx, ury);
          printf("%%%%HiResBoundingBox: %f %f %f %f\n", hllx, hlly, hurx, hury);
        }
        else
          fprintf(stderr,"%s: ERROR -- not enough memory to read in one row of the picture\n",prgname);
        
	fclose(inputfile);
        free(image_row);
}


int	
main(int argc, char **argv)
{ 
  int i;
  char *filename= NULL;
  unsigned int resolution= 72; /* use 72 dpi as default resolution */
  unsigned char tight= 1;

  for (i= 1; i<argc; i++)
  {
    if ( strcmp(argv[i],"-r") == 0 )
    {
      if (++i<argc)
        resolution= atol(argv[i]);
      else
        fprintf(stderr,"%s: ERROR -- Missing resolution after -r\n",prgname);
    }
    else
    if ( strcmp(argv[i],"-l") == 0 )
    {
      tight= 0;
    }
    else
    if ( strcmp(argv[i],"-V") == 0 || strcmp(argv[i],"--version")==0 )
    {
      printf("%s: bbox Version %s\n",prgname,version);
      return 0;
    }
    else
    if ( strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0 )
    {
      printf("%s: Version %s\n",prgname,version);
      printf(" usage: %s [-l] [-r resolution] [-V] [filename]\n",prgname);
      printf(" -l: loose bounding box (bbox is expanded by 1 point)\n");
      printf(" -r: resolution of picture in dpi\n");
      printf(" -V: version information\n");
      printf(" -h: this help\n");
      printf(" bbox reads a rawppm or rawpbm file and prints out the\n");
      printf(" bounding box of the image. If no filename is specified\n");
      printf(" input is read from standard input.\n");
      return 0;
    }
    else
    if ( argv[i][0] == '-' ) /* unkown option */
    {
      fprintf(stderr,"%s: ERROR -- unknown option %s\n call %s -h for help on usage\n",
             prgname, argv[i], prgname);
      return 1;
    }
    else /* filename argument */
      filename= argv[i];
  }

  readppm_and_calcbb(filename, resolution, tight);

  return 0;
}
