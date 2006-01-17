/* set.c */

/************************************************************************

  Part of the dvipng distribution

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA.

  Copyright (C) 2002-2005 Jan-Åke Larsson

************************************************************************/

#include "dvipng.h"
#if HAVE_ALLOCA_H
# include <alloca.h>
#endif
#include <math.h>

void CreateImage(pixels x_width,pixels y_width)
{
  int Background;

  if (page_imagep) 
    gdImageDestroy(page_imagep);
  if (x_width <= 0) x_width=1;
  if (y_width <= 0) y_width=1;
#ifdef HAVE_GDIMAGECREATETRUECOLOR
  if (flags & RENDER_TRUECOLOR) 
    page_imagep=gdImageCreateTrueColor(x_width,y_width);
  else
#endif
    page_imagep=gdImageCreate(x_width,y_width);
  /* Set bg color */
#ifdef HAVE_GDIMAGECOLORRESOLVEALPHA
  if (flags & BG_TRANSPARENT_ALPHA
#ifdef HAVE_GDIMAGEGIF
    /* GIFs cannot handle an alpha channel, resort to transparent
       color index */
      && ~flags & GIF_OUTPUT
#endif /* HAVE_GDIMAGEGIF */
      ) {
    Background = gdImageColorAllocateAlpha(page_imagep,
					   cstack[0].red,
					   cstack[0].green,
					   cstack[0].blue,127);
#ifdef HAVE_GDIMAGEALPHABLENDING
    /* Draw without blending to allow the background of the _viewer_
       to shine through. Color blending is calculated where needed. */
    gdImageAlphaBlending(page_imagep, 0);
    gdImageSaveAlpha(page_imagep, 1);
#endif
  }
  else 
#endif /* HAVE_GDIMAGECOLORRESOLVEALPHA */
    {
      Background = gdImageColorAllocate(page_imagep,
					cstack[0].red,
					cstack[0].green,
					cstack[0].blue);
      /* maybe alpha is not available or perhaps we are producing
	 GIFs, so test for BG_TRANSPARENT_ALPHA too */
      if (flags & (BG_TRANSPARENT|BG_TRANSPARENT_ALPHA))
	gdImageColorTransparent(page_imagep,Background); 
    }
#ifdef HAVE_GDIMAGECREATETRUECOLOR
  if (flags & RENDER_TRUECOLOR) 
    /* Truecolor: there is no background color index, fill image instead. */
    gdImageFilledRectangle(page_imagep, 0, 0, 
			   x_width-1, y_width-1, Background);
#endif
}


void ChangeColor(gdImagePtr imagep,int x1,int y1,int x2,int y2,int color1,int color2)
/* In the given rectangle, change color1 to color2 */
{
  int x,y;
  for( y=y1; y<=y2; y++) {
    for( x=x1; x<=x2; x++) {
      if (gdImageGetPixel(imagep, x, y)==color1) 
	gdImageSetPixel(imagep, x, y, color2);
    }
  }
}

void WriteImage(char *pngname, int pagenum)
{
  char* pos;
  FILE* outfp=NULL;

  /* Transparent border */
  if (borderwidth>0) {
    int Transparent,bgcolor;
    pixels x_width,y_width;

    x_width=gdImageSX(page_imagep);
    y_width=gdImageSY(page_imagep);
    
    bgcolor = gdImageColorResolve(page_imagep,
				  cstack[0].red,cstack[0].green,cstack[0].blue);
    /* Set ANOTHER bg color, transparent this time */
    /* No semi-transparency here, given the motivation for this code
       * (box cursor visibility in Emacs) */
    if (userbordercolor)
      Transparent = gdImageColorAllocate(page_imagep,
					 bordercolor.red,bordercolor.green,bordercolor.blue); 
    else
      Transparent = gdImageColorAllocate(page_imagep,
					 cstack[0].red,cstack[0].green,cstack[0].blue); 
    gdImageColorTransparent(page_imagep,Transparent); 
    ChangeColor(page_imagep,0,0,x_width-1,borderwidth-1,bgcolor,Transparent);
    ChangeColor(page_imagep,0,0,borderwidth-1,y_width-1,bgcolor,Transparent);
    ChangeColor(page_imagep,x_width-borderwidth,0,x_width-1,y_width-1,bgcolor,Transparent);
    ChangeColor(page_imagep,0,y_width-borderwidth,x_width-1,y_width-1,bgcolor,Transparent);
  }

  if ((pos=strchr(pngname,'%')) != NULL) {
    if (strchr(++pos,'%'))
      Fatal("too many %%s in output file name");
    if (*pos == 'd' 
	|| (*pos=='0' && pos[1]>='1' && pos[1]<='9' && pos[2]=='d')) {
      /* %d -> pagenumber, so add 9 string positions 
	 since pagenumber max +-2^31 or +-2*10^9 */
      char* tempname = alloca(strlen(pngname)+9);
      sprintf(tempname,pngname,pagenum);
      pngname = tempname;
    } else {
      Fatal("unacceptible format spec in output file name");
    }
  }
#ifdef HAVE_GDIMAGEGIF
  if (flags & GIF_OUTPUT && (pos=strrchr(pngname,'.')) != NULL 
      && strcmp(pos,".png")==0) {
    *(pos+1)='g';
    *(pos+2)='i';
    *(pos+3)='f';
  }
#endif
  if ((outfp = fopen(pngname,"wb")) == NULL)
      Fatal("cannot open output file %s",pngname);
#ifdef HAVE_GDIMAGEGIF
  if (flags & GIF_OUTPUT) 
    gdImageGif(page_imagep,outfp);
  else
#endif
#ifdef HAVE_GDIMAGEPNGEX
  gdImagePngEx(page_imagep,outfp,compression);
#else
  gdImagePng(page_imagep,outfp);
#endif
  fclose(outfp);
  DEBUG_PRINT(DEBUG_DVI,("\n  WROTE:   \t%s\n",pngname));
  DestroyImage();
}

void DestroyImage(void)
{
  gdImageDestroy(page_imagep);
  page_imagep=NULL;
}

static int gammatable[]=
  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
   20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
   40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,
   60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
   80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,
   100,101,102,103,104,105,106,107,108,109,
   110,111,112,113,114,115,116,117,118,119,
   120,121,122,123,124,125,126,127,128,129,
   130,131,132,133,134,135,136,137,138,139,
   140,141,142,143,144,145,146,147,148,149,
   150,151,152,153,154,155,156,157,158,159,
   160,161,162,163,164,165,166,167,168,169,
   170,171,172,173,174,175,176,177,178,179,
   180,181,182,183,184,185,186,187,188,189,
   190,191,192,193,194,195,196,197,198,199,
   200,201,202,203,204,205,206,207,208,209,
   210,211,212,213,214,215,216,217,218,219,
   220,221,222,223,224,225,226,227,228,229,
   230,231,232,233,234,235,236,237,238,239,
   240,241,242,243,244,245,246,247,248,249,
   250,251,252,253,254,255};

void Gamma(double gamma)
{
  int i=0;
  
  while (i<=255) {
    gammatable[i]=255-(int)(pow((double)(255-i)/255.0,gamma)*255);
    DEBUG_PRINT(DEBUG_GLYPH,("\n  GAMMA GREYSCALE: %d -> %d ",i,gammatable[i]));
    i++;
  }
}


#define GREYS 255
dviunits SetGlyph(int32_t c, int32_t hh,int32_t vv)
{
  register struct char_entry *ptr = currentfont->chr[c];
                                      /* temporary char_entry pointer */
  int red,green,blue;
  int *Color=alloca(sizeof(int)*(GREYS+1));
  int x,y;
  int pos=0;
  int bgColor,pixelgrey,pixelcolor;
  hh -= ptr->xOffset/shrinkfactor;
  vv -= ptr->yOffset/shrinkfactor;

  for( x=1; x<=GREYS ; x++) 
    Color[x] = -1;
#ifdef HAVE_GDIMAGECOLORRESOLVEALPHA
  if (flags & BG_TRANSPARENT_ALPHA
#ifdef HAVE_GDIMAGEGIF
    /* GIFs cannot handle an alpha channel, resort to transparent
       color index */
      && ~flags & GIF_OUTPUT
#endif /* HAVE_GDIMAGEGIF */
      ) {
    int alpha;
    Color[0] = gdImageColorResolveAlpha(page_imagep,
					cstack[0].red,
					cstack[0].green,
					cstack[0].blue,127);
    for( y=0; y<ptr->h; y++) {
      for( x=0; x<ptr->w; x++) {
	if (ptr->data[pos]>0) {
	  pixelgrey=gammatable[(int)ptr->data[pos]];
	  bgColor = gdImageGetPixel(page_imagep, hh + x, vv + y);
	  if (bgColor == Color[0]) {
	    if  (Color[pixelgrey]==-1) {
	      DEBUG_PRINT(DEBUG_GLYPH,("\n  GAMMA GREYSCALE: %d -> %d ",ptr->data[pos],pixelgrey));
	      /* Standard background: use cached value if present */
	      /* 127-alpha/2: 0=opaque,127=full_trans */
	      Color[pixelgrey]=gdImageColorResolveAlpha(page_imagep,
							cstack[csp].red,
							cstack[csp].green,
							cstack[csp].blue,
							127-pixelgrey/2);
	    }
	    pixelcolor=Color[pixelgrey];
	  } else {
	    /* Overstrike: No cache */
	    red  =gdImageRed(page_imagep, bgColor);
	    green=gdImageGreen(page_imagep, bgColor);
	    blue =gdImageBlue(page_imagep, bgColor);
	    alpha=255-2*gdImageAlpha(page_imagep, bgColor);
	    red  =red-(red-cstack[csp].red)*pixelgrey/GREYS;
	    green=green-(green-cstack[csp].green)*pixelgrey/GREYS;
	    blue =blue-(blue-cstack[csp].blue)*pixelgrey/GREYS;
	    /* 255=opaque */
	    alpha=alpha-(alpha-255)*pixelgrey/GREYS;
	    /* 127-alpha/2: 0=opaque,127=full_trans */
	    pixelcolor = gdImageColorResolveAlpha(page_imagep,
						  red,green,blue,127-alpha/2);
	  }
	  gdImageSetPixel(page_imagep, hh + x, vv + y, pixelcolor);
	}
	pos++;
      }
    }
  } else
#endif /* HAVE_GDIMAGECOLORRESOLVEALPHA */
    {
      Color[0] = gdImageColorResolve(page_imagep,
				     cstack[0].red,
				     cstack[0].green,
				     cstack[0].blue);
      for( y=0; y<ptr->h; y++) {
	for( x=0; x<ptr->w; x++) {
	  if (ptr->data[pos]>0) {
	    pixelgrey=gammatable[(int)ptr->data[pos]];
	    bgColor = gdImageGetPixel(page_imagep, hh + x, vv + y);
	    if (bgColor == Color[0]) {
	      /* Standard background: use cached value if present */
	      if (Color[pixelgrey]==-1) {
		DEBUG_PRINT(DEBUG_GLYPH,("\n  GAMMA GREYSCALE: %d -> %d ",ptr->data[pos],pixelgrey));
		red = cstack[0].red 
		  - (cstack[0].red-cstack[csp].red)*pixelgrey/GREYS;
		green = cstack[0].green
		  - (cstack[0].green-cstack[csp].green)*pixelgrey/GREYS;
		blue = cstack[0].blue
		  - (cstack[0].blue-cstack[csp].blue)*pixelgrey/GREYS;
		Color[pixelgrey] = 
		  gdImageColorResolve(page_imagep,red,green,blue);
	      }
	      pixelcolor=Color[pixelgrey];
	    } else {
	      /* Overstrike: No cache */
	      red=gdImageRed(page_imagep, bgColor);
	      green=gdImageGreen(page_imagep, bgColor);
	      blue=gdImageBlue(page_imagep, bgColor);
	      red = red-(red-cstack[csp].red)*pixelgrey/GREYS;
	      green = green-(green-cstack[csp].green)*pixelgrey/GREYS;
	      blue = blue-(blue-cstack[csp].blue)*pixelgrey/GREYS;
	      pixelcolor = gdImageColorResolve(page_imagep, red, green, blue);
	    }
	    gdImageSetPixel(page_imagep, hh + x, vv + y, pixelcolor);
	  }
	  pos++;
	}
      }
    }
  /* This code saved _no_ execution time, strangely.
   * Also, it cannot gamma correct; needs that in loaded glyphs
   *
   * #ifdef HAVE_GDIMAGECREATETRUECOLOR 
   *   if (truecolor) 
   *     for( i=1; i<=ptr->glyph.nchars ; i++) {
   *       Color = gdImageColorResolveAlpha(page_imagep,Red,Green,Blue,
   *                                        128-128*i/ptr->glyph.nchars);
   *       gdImageChar(page_imagep, &(ptr->glyph),
   *	               hh - ptr->xOffset/shrinkfactor,
   *		       vv - ptr->yOffset/shrinkfactor,
   *	  	       i,Color);
   *       }
   *    else {
   *  #endif */
  return(ptr->tfmw);
}

dviunits SetRule(dviunits a, dviunits b, subpixels hh,subpixels vv)
{
  /*                               This routine will draw a \rule */
  int Color;
  pixels    width=0, height=0;

  if ( a > 0 && b > 0 ) {
    /* Calculate width and height, round up */
    width = (b+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
    height = (a+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
  }
  if (page_imagep != NULL) {
    if ((height>0) && (width>0)) {
      /* This code produces too dark rules. But what the hell. Grey
       * rules look fuzzy. */
      Color = gdImageColorResolve(page_imagep, 
				  cstack[csp].red,
				  cstack[csp].green,
				  cstack[csp].blue);
      /* +1 and -1 are because the Rectangle coords include last pixels */
      gdImageFilledRectangle(page_imagep,hh,vv-height+1,hh+width-1,vv,Color);
      DEBUG_PRINT(DEBUG_DVI,("\n  RULE \t%dx%d at (%d,%d)",
		   width, height, hh, vv));
    }
  } else {
    /* The +1's are because things are cut _at_that_coordinate_. */
    min(x_min,hh);
    min(y_min,vv-height+1);
    max(x_max,hh+width);
    max(y_max,vv+1);
  }
  return(b);
}


#if 0
  if (page_imagep != NULL) {
    if ((a>0) && (b>0)) {
      int width,height,left=-1,right=-1,bottom=-1,top=-1;
      gdImagePtr rule;
      

      width  = (h+b+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor
	- h/dvi->conv/shrinkfactor;
      /* Calculate height, round up on the bottom and down on the top */
      height = (v+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor
	- (v-a)/dvi->conv/shrinkfactor;
      rule = gdImageCreate(width,height);
      /* Set background */
      gdImageColorAllocate(rule,Red,Green,Blue);
      
      /* Calculate blackness of edges of wide rule */
      left   = shrinkfactor - h/dvi->conv%shrinkfactor;
      right  = (h+b+dvi->conv-1)/dvi->conv%shrinkfactor;
      if (right==0)  right  = shrinkfactor;
      if (width==1) {
	/* Adjust blackness of narrow rule */
	left = shrinkfactor;
	/* The (correct) alternative produces "fuzzy" tables */ 
	/* left = ((h+b+dvi->conv-1)/dvi->conv - h/dvi->conv)%shrinkfactor;
	 */
      } 
      if (width==2 && right+left <= shrinkfactor +1) {
	/* Adjust blackness of narrow rule when spread across two columns */
	/* Cheat to make it sharp (+1 above, and wedging it around below) */
	if (right>left) { right=shrinkfactor; left=0; }
	if (right<left) { left=shrinkfactor; right=0; }
	if (right==left) { left=right=(shrinkfactor+1)/2; }
      }

      /* Calculate blackness of edges of tall rule */
      bottom = (v+dvi->conv-1)/dvi->conv%shrinkfactor;
      if (bottom==0) bottom = shrinkfactor;
      top    = shrinkfactor-(v-a)/dvi->conv%shrinkfactor;
      if (height == 1) {
	/* Adjust blackness of short rule */
	bottom = shrinkfactor;
	/* The (correct) alternative produces "fuzzy" tables */ 
	/* bottom = ((v+dvi->conv-1)/dvi->conv - (v-a)/dvi->conv)%shrinkfactor;
	 */
      } 
      if (height==2 && top+bottom <= shrinkfactor +1) {
	/* Adjust blackness of short rule when spread across two columns */
	/* Cheat to make it sharp (+1 above, and wedging it around below) */
	if (top>bottom) { top=shrinkfactor; bottom=0; }
	if (top<bottom) { bottom=shrinkfactor; top=0; }
	if (right==left) { left=right=(shrinkfactor+1)/2; }
      }
	
      Color = gdImageColorResolve(rule,
		  bRed-(bRed-Red)*left*bottom/shrinkfactor/shrinkfactor,
		  bGreen-(bGreen-Green)*left*bottom/shrinkfactor/shrinkfactor,
		  bBlue-(bBlue-Blue)*left*bottom/shrinkfactor/shrinkfactor);
      gdImageSetPixel(rule,0,height-1,Color);
      if (width>1) {
	Color = gdImageColorResolve(rule, 
		  bRed-(bRed-Red)*right*bottom/shrinkfactor/shrinkfactor,
		  bGreen-(bGreen-Green)*right*bottom/shrinkfactor/shrinkfactor,
		  bBlue-(bBlue-Blue)*right*bottom/shrinkfactor/shrinkfactor);
	gdImageSetPixel(rule,width-1,height-1,Color);
      }
      if (height>1) {
	Color = gdImageColorResolve(rule, 
	       	    bRed-(bRed-Red)*left*top/shrinkfactor/shrinkfactor,
		    bGreen-(bGreen-Green)*left*top/shrinkfactor/shrinkfactor,
		    bBlue-(bBlue-Blue)*left*top/shrinkfactor/shrinkfactor);
	gdImageSetPixel(rule,0,0,Color);
      }
      if (height>1 && width>1) {
	Color = gdImageColorResolve(rule, 
		    bRed-(bRed-Red)*right*top/shrinkfactor/shrinkfactor,
		    bGreen-(bGreen-Green)*right*top/shrinkfactor/shrinkfactor,
		    bBlue-(bBlue-Blue)*right*top/shrinkfactor/shrinkfactor);
	gdImageSetPixel(rule,width-1,0,Color);
      }

      if (width>2) {
	Color = gdImageColorResolve(rule, 
				    bRed-(bRed-Red)*bottom/shrinkfactor,
				    bGreen-(bGreen-Green)*bottom/shrinkfactor,
				    bBlue-(bBlue-Blue)*bottom/shrinkfactor);
	gdImageFilledRectangle(rule,1,height-1,width-2,height-1,Color);
      }
      if (height>2) {
	Color = gdImageColorResolve(rule, 
				    bRed-(bRed-Red)*left/shrinkfactor,
				    bGreen-(bGreen-Green)*left/shrinkfactor,
				    bBlue-(bBlue-Blue)*left/shrinkfactor);
	gdImageFilledRectangle(rule,0,1,0,height-2,Color);
      }
      if (height>1 && width>2) {
	Color = gdImageColorResolve(rule, 
				    bRed-(bRed-Red)*top/shrinkfactor,
				    bGreen-(bGreen-Green)*top/shrinkfactor,
				    bBlue-(bBlue-Blue)*top/shrinkfactor);
	gdImageFilledRectangle(rule,1,0,width-2,0,Color);
      }
      if (height>2 && width>1) {
	Color = gdImageColorResolve(rule, 
				    bRed-(bRed-Red)*right/shrinkfactor,
				    bGreen-(bGreen-Green)*right/shrinkfactor,
				    bBlue-(bBlue-Blue)*right/shrinkfactor);
	gdImageFilledRectangle(rule,width-1,1,width-1,height-2,Color);
      }
      gdImageCopy(page_imagep,rule,
		  h/dvi->conv/shrinkfactor,
		  (v-a)/dvi->conv/shrinkfactor,
		  0,0,width,height);
      DEBUG_PRINT(DEBUG_DVI,("\n  RULE \t%dx%d at (%d,%d)",
		   width,height,
		   PIXROUND(h, dvi->conv*shrinkfactor),
		   PIXROUND(v, dvi->conv*shrinkfactor)));
      DEBUG_PRINT(DEBUG_DVI,(" (lrtb %d %d %d %d)",left,right,top,bottom));
    }
  } else {
    min(x_min,hh);
    min(y_min,vv-height+1);
    max(x_max,hh+width);
    max(y_max,vv+1);
  }
#endif


