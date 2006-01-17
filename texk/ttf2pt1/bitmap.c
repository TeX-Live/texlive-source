/*
 * Handling of the bitmapped glyphs
 *
 * Copyright (c) 2001 by the TTF2PT1 project
 * Copyright (c) 2001 by Sergey Babkin
 *
 * see COPYRIGHT for the full copyright notice
 */

#include <stdio.h>
#include <stdlib.h>
#include "pt1.h"
#include "global.h"

/* possible values of limits */
#define L_NONE	0 /* nothing here */
#define L_ON	1 /* black is on up/right */
#define L_OFF	2 /* black is on down/left */

static int warnedhints = 0;

/*
 * Produce an outline from a bitmap.
 * scale - factor to scale the sizes
 * bmap - array of dots by lines, xsz * ysz
 * xoff, yoff - offset of the bitmap's lower left corner
 *              from the logical position (0,0)
 */

bmp_outline(
	GLYPH *g,
	int scale,
	char *bmap,
	int xsz,
	int ysz,
	int xoff,
	int yoff
)
{
	char *hlm, *vlm; /* arrays of the limits of outlines */
	char *amp; /* map of ambiguous points */
	int x, y;
	char *ip, *op;
	double fscale;

	fscale = (double)scale;

	if(!warnedhints) {
		warnedhints = 1;
		if(subhints) {
			WARNING_2 fprintf(stderr, 
				"Use of hint substitution on bitmap fonts is not recommended\n");
		}
	}

#if 0
	printbmap(bmap, xsz, ysz, xoff, yoff);
#endif

	/* now find the outlines */
	hlm=calloc( xsz, ysz+1 ); /* horizontal limits */
	vlm=calloc( xsz+1, ysz ); /* vertical limits */
	amp=calloc( xsz, ysz ); /* ambiguous points */

	if(hlm==0 || vlm==0 || amp==0)  {
		fprintf (stderr, "****malloc failed %s line %d\n", __FILE__, __LINE__);
		exit(255);
	}

	/*
	 * hlm and vlm represent a grid of horisontal and
	 * vertical lines. Each pixel is surrounded by the grid
	 * from all the sides. The values of [hv]lm mark the
	 * parts of grid where the pixel value switches from white
	 * to black and back.
	 */

	/* find the horizontal limits */
	ip=bmap; op=hlm;
	/* 1st row */
	for(x=0; x<xsz; x++) {
		if(ip[x])
			op[x]=L_ON;
	}
	ip+=xsz; op+=xsz;
	/* internal rows */
	for(y=1; y<ysz; y++) {
		for(x=0; x<xsz; x++) {
			if(ip[x]) {
				if(!ip[x-xsz])
					op[x]=L_ON;
			} else {
				if(ip[x-xsz])
					op[x]=L_OFF;
			}
		}
		ip+=xsz; op+=xsz;
	}

	/* last row */
	ip-=xsz;
	for(x=0; x<xsz; x++) {
		if(ip[x])
			op[x]=L_OFF;
	}

	/* find the vertical limits */
	ip=bmap; op=vlm;
	for(y=0; y<ysz; y++) {
		if(ip[0])
			op[0]=L_ON;
		for(x=1; x<xsz; x++) {
			if(ip[x]) {
				if(!ip[x-1])
					op[x]=L_ON;
			} else {
				if(ip[x-1])
					op[x]=L_OFF;
			}
		}
		if(ip[xsz-1])
			op[xsz]=L_OFF;
		ip+=xsz; op+=xsz+1; 
	}

	/*
	 * Ambiguous points are the nodes of the grids
	 * that are between two white and two black pixels
	 * located in a checkerboard style. Actually
	 * there are only two patterns that may be
	 * around an ambiguous point:
	 *
	 *    X|.    .|X
	 *    -*-    -*-
	 *    .|X    X|.
	 *
	 * where "|" and "-" represent the grid (respectively members
	 * of vlm and hlm), "*" represents an ambiguous point
	 * and "X" and "." represent black and white pixels.
	 *
	 * If these sample pattern occur in the lower left corner
	 * of the bitmap then this ambiguous point will be
	 * located at amp[1][1] or in other words amp[1*xsz+1].
	 *
	 * These points are named "ambiguous" because it's
	 * not easy to guess what did the font creator mean
	 * at these points. So we are going to treat them 
	 * specially, doing no aggressive smoothing.
	 */

	/* find the ambiguous points */
	for(y=ysz-1; y>0; y--)
		for(x=xsz-1; x>0; x--) {
			if(bmap[y*xsz+x]) {
				if( !bmap[y*xsz+x-1] && !bmap[y*xsz-xsz+x] && bmap[y*xsz-xsz+x-1] )
					amp[y*xsz+x]=1;
			} else {
				if( bmap[y*xsz+x-1] && bmap[y*xsz-xsz+x] && !bmap[y*xsz-xsz+x-1] )
					amp[y*xsz+x]=1;
			}
		}

#if 0
	printlimits(hlm, vlm, amp, xsz, ysz);
#endif

	/* generate the vectored outline */

	while(1) {
		int found = 0;
		int outer; /* flag: this is an outer contour */
		int hor, newhor; /* flag: the current contour direction is horizontal */
		int dir; /* previous direction of the coordinate, 1 - L_ON, 0 - L_OFF */
		int startx, starty; /* start of a contour */
		int firstx, firsty; /* start of the current line */
		int newx, newy; /* new coordinates to try */
		char *lm, val;
		int maxx, maxy, xor;

		for(y=ysz; !found &&  y>0; y--) 
			for(x=0; x<xsz; x++) 
				if(hlm[y*xsz+x] > L_NONE) 
					goto foundcontour;
		break; /* have no contours left */

	foundcontour:
		fg_rmoveto(g, fscale*(x+xoff), fscale*(y+yoff));

		startx = firstx = x;
		starty = firsty = y;

		if(hlm[y*xsz+x] == L_OFF) {
			outer = 1; dir = 0;
			hlm[y*xsz+x] = -hlm[y*xsz+x]; /* mark as seen */
			hor = 1; x++;
		} else {
			outer = 0; dir = 0;
			hor = 0; y--;
			vlm[y*(xsz+1)+x] = -vlm[y*(xsz+1)+x]; /* mark as seen */
		}

		while(x!=startx || y!=starty) {
#if 0
			printf("trace (%d, %d) outer=%d hor=%d dir=%d\n", x, y, outer, hor, dir);
#endif

			/* initialization common for try1 and try2 */
			if(hor) {
				lm = vlm; maxx = xsz+1; maxy = ysz; newhor = 0;
			} else {
				lm = hlm; maxx = xsz; maxy = ysz+1; newhor = 1;
			}
			xor = (outer ^ hor ^ dir);

		try1:
			/* first we try to change axis, to keep the
			 * contour as long as possible
			 */

			newx = x; newy = y;
			if(!hor && (!outer ^ dir))
				newx--;
			if(hor && (!outer ^ dir))
				newy--;

			if(newx < 0 || newx >= maxx || newy < 0 || newy >= maxy)
				goto try2;

			if(!xor)
				val = L_ON;
			else
				val = L_OFF;
#if 0
			printf("try 1, want %d have %d at %c(%d, %d)\n", val, lm[newy*maxx + newx],
				(newhor ? 'h':'v'), newx, newy);
#endif
			if( lm[newy*maxx + newx] == val )
				goto gotit;

		try2:
			/* try to change the axis anyway */

			newx = x; newy = y;
			if(!hor && (outer ^ dir))
				newx--;
			if(hor && (outer ^ dir))
				newy--;

			if(newx < 0 || newx >= maxx || newy < 0 || newy >= maxy)
				goto try3;

			if(xor)
				val = L_ON;
			else
				val = L_OFF;
#if 0
			printf("try 2, want %d have %d at %c(%d, %d)\n", val, lm[newy*maxx + newx],
				(newhor ? 'h':'v'), newx, newy);
#endif
			if( lm[newy*maxx + newx] == val )
				goto gotit;

		try3:
			/* try to continue in the old direction */

			if(hor) {
				lm = hlm; maxx = xsz; maxy = ysz+1;
			} else {
				lm = vlm; maxx = xsz+1; maxy = ysz;
			}
			newhor = hor;
			newx = x; newy = y;
			if(hor && dir)
				newx--;
			if(!hor && !dir)
				newy--;

			if(newx < 0 || newx >= maxx || newy < 0 || newy >= maxy)
				goto badtry;

			if(dir)
				val = L_ON;
			else
				val = L_OFF;
#if 0
			printf("try 3, want %d have %d at %c(%d, %d)\n", val, lm[newy*maxx + newx],
				(newhor ? 'h':'v'), newx, newy);
#endif
			if( lm[newy*maxx + newx] == val )
				goto gotit;

		badtry:
			fprintf(stderr, "**** Internal error in the contour detection code at (%d, %d)\n", x, y);
			fprintf(stderr, "glyph='%s' outer=%d hor=%d dir=%d\n", g->name, outer, hor, dir);
			fflush(stdout);
			exit(1);

		gotit:
			if(hor != newhor) { /* changed direction, end the previous line */
				fg_rlineto(g, fscale*(x+xoff), fscale*(y+yoff));
				firstx = x; firsty = y;
			}
			lm[newy*maxx + newx] = -lm[newy*maxx + newx];
			hor = newhor;
			dir = (val == L_ON);
			if(newhor)
				x -= (dir<<1)-1;
			else
				y += (dir<<1)-1;
		}
#if 0
		printf("trace (%d, %d) outer=%d hor=%d dir=%d\n", x, y, outer, hor, dir);
#endif
		fg_rlineto(g, fscale*(x+xoff), fscale*(y+yoff));
		g_closepath(g);
	}
	
	free(hlm); free(vlm); free(amp);
}

#if 0
/* print out the bitmap */
printbmap(bmap, xsz, ysz, xoff, yoff)
	char *bmap;
	int xsz, ysz, xoff, yoff;
{
	int x, y;

	for(y=ysz-1; y>=0; y--) {
		putchar( (y%10==0) ? y/10+'0' : ' ' );
		putchar( y%10+'0' );
		for(x=0; x<xsz; x++)
			putchar( bmap[y*xsz+x] ? 'X' : '.' );
		if(-yoff==y)
			putchar('_'); /* mark the baseline */
		putchar('\n');
	}
	putchar(' '); putchar(' ');
	for(x=0; x<xsz; x++)
		putchar( x%10+'0' );
	putchar('\n'); putchar(' '); putchar(' ');
	for(x=0; x<xsz; x++)
		putchar( (x%10==0) ? x/10+'0' : ' ' );
	putchar('\n');
}

/* print out the limits of outlines */
printlimits(hlm, vlm, amp, xsz, ysz)
	char *hlm, *vlm, *amp;
	int xsz, ysz;
{
	int x, y;
	static char h_char[]={ ' ', '~', '^' };
	static char v_char[]={ ' ', '(', ')' };

	for(y=ysz-1; y>=0; y--) {
		for(x=0; x<xsz; x++) {
			if(amp[y*xsz+x])
				putchar('!'); /* ambigouos point is always on a limit */
			else
				putchar(v_char[ vlm[y*(xsz+1)+x] & (L_ON|L_OFF) ]);
			putchar(h_char[ hlm[(y+1)*xsz+x] & (L_ON|L_OFF) ]);
		}
		putchar(v_char[ vlm[y*(xsz+1)+x] & (L_ON|L_OFF) ]);
		putchar('\n');
	}
	/* last line */
	for(x=0; x<xsz; x++) {
		putchar(' ');
		putchar(h_char[ hlm[x] & (L_ON|L_OFF) ]);
	}
	putchar(' ');
	putchar('\n');
}
#endif /* 0 */
