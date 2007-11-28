/* $Id$ */

#include "mplib.h"

#define bezier_error (720<<20)+1

#define sign(v) ((v)>0 ? 1 : ((v)<0 ? -1 : 0 ))

#define print_roots(a) { if (debuglevel>(65536*2))			\
      fprintf(stdout,"bezierslope(): %s, i=%f, o=%f, angle=%f\n",	\
	      (a),in,out,res); }

#define out ((double)(xo>>20))
#define mid ((double)(xm>>20))
#define in  ((double)(xi>>20))

#define divisor (256*256)

#define double2angle(a) (int)floor(a*256.0*256.0*16.0)

angle 
bezierslope(integer AX,integer AY,integer BX,integer BY,
            integer CX,integer CY,integer DX,integer DY, int debuglevel) {
  double a, b, c;
  integer deltax,deltay;
  double ax,ay,bx,by,cx,cy,dx,dy;

  angle xi = 0, xo = 0, xm = 0;
  double res = 0;

  ax=AX/divisor;  ay=AY/divisor;
  bx=BX/divisor;  by=BY/divisor;
  cx=CX/divisor;  cy=CY/divisor;
  dx=DX/divisor;  dy=DY/divisor;

  deltax = (BX-AX); deltay = (BY-AY);
  if (deltax==0 && deltay == 0) { deltax=(CX-AX); deltay=(CY-AY); }
  if (deltax==0 && deltay == 0) { deltax=(DX-AX); deltay=(DY-AY); }
  xi = anangle(deltax,deltay);

  deltax = (CX-BX); deltay = (CY-BY);
  xm = anangle(deltax,deltay);

  deltax = (DX-CX); deltay = (DY-CY);
  if (deltax==0 && deltay == 0) { deltax=(DX-BX); deltay=(DY-BY); }
  if (deltax==0 && deltay == 0) { deltax=(DX-AX); deltay=(DY-AY); }
  xo = anangle(deltax,deltay);

  a = (bx-ax)*(cy-by) - (cx-bx)*(by-ay); /* a = (bp-ap)x(cp-bp); */
  b = (bx-ax)*(dy-cy) - (by-ay)*(dx-cx);; /* b = (bp-ap)x(dp-cp);*/
  c = (cx-bx)*(dy-cy) - (dx-cx)*(cy-by); /* c = (cp-bp)x(dp-cp);*/

  if (debuglevel>(65536*2)) {
    fprintf(stdout,
    "bezierslope(): (%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)\n",
             ax,ay,bx,by,cx,cy,dx,dy);
    fprintf(stdout,"bezierslope(): a,b,c,b^2,4ac: (%.2f,%.2f,%.2f,%.2f,%.2f)\n",a,b,c,b*b,4*a*c);
  }

  if ((a==0)&&(c==0)) {
    res = (b==0 ?  0 :  (out-in)); 
    print_roots("no roots (a)");
  } else if ((a==0)||(c==0)) {
    if ((sign(b) == sign(a)) || (sign(b) == sign(c))) {
      res = out-in; /* ? */
      if (res<-180.0) 
	res += 360.0;
      else if (res>180.0)
	res -= 360.0;
      print_roots("no roots (b)");
    } else {
      res = out-in; /* ? */
      print_roots("one root (a)");
    }
  } else if ((sign(a)*sign(c))<0) {
    res = out-in; /* ? */
      if (res<-180.0) 
	res += 360.0;
      else if (res>180.0)
	res -= 360.0;
    print_roots("one root (b)");
  } else {
    if (sign(a) == sign(b)) {
      res = out-in; /* ? */
      if (res<-180.0) 
	res += 360.0;
      else if (res>180.0)
	res -= 360.0;
      print_roots("no roots (d)");
    } else {
      if ((b*b) == (4*a*c)) {
	res = bezier_error;
	/* print_roots("double root"); *//* cusp */
      } else if ((b*b) < (4*a*c)) {
	res = out-in; /* ? */
	if (res<=0.0 &&res>-180.0) 
	  res += 360.0;
        else if (res>=0.0 && res<180.0)
	  res -= 360.0;
	print_roots("no roots (e)");
      } else {
	res = out-in;
	if (res<-180.0) 
	  res += 360.0;
        else if (res>180.0)
	  res -= 360.0;
	print_roots("two roots"); /* two inflections */
      }
    }
  }
  return double2angle(res);
}

