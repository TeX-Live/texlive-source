@x
\def\MP{MetaPost}
@y
\def\MP{MetaPost}
\def\pTeX{p\kern-.15em\TeX}
@z

@x
#include "avl.h"
@y
#include "avl.h"
#include <ptexenc/ptexenc.h>
@z

@x
@d virtual_space 1000000 /* maximum total bytes of typesetting commands for virtual fonts */
@d max_fonts 1000 /* maximum number of distinct fonts per \.{DVI} file */
@d max_fnums 3000 /* maximum number of fonts plus fonts local to virtual fonts */
@d max_widths (256*max_fonts) /* maximum number of different characters among all fonts */
@d line_length 79 /* maximum output line length (must be at least 60) */
@d stack_size 100 /* \.{DVI} files shouldn't |push| beyond this depth */
@d font_tolerance 0.00001
@y
@ @<Glob...@>=
#ifdef UPMP
#define virtual_space 2000000 /* maximum total bytes of typesetting commands for virtual fonts */
#else
#define virtual_space 1000000 /* maximum total bytes of typesetting commands for virtual fonts */
#endif
#define max_fonts 1000 /* maximum number of distinct fonts per \.{DVI} file */
#define max_fnums 3000 /* maximum number of fonts plus fonts local to virtual fonts */
#ifdef UPMP
#define max_widths 2000000 /* maximum number of different characters among all fonts */
#else
#define max_widths 512000 /* maximum number of different characters among all fonts */
#endif
#define line_length 79 /* maximum output line length (must be at least 60) */
#define stack_size 100 /* \.{DVI} files shouldn't |push| beyond this depth */
#define font_tolerance 0.00001
@z

@x
@d undefined_commands 250: case 251: case 252: case 253: case 254: case 255
@y
@d dir 255 /* \pTeX\ direction */
@d undefined_commands 250: case 251: case 252: case 253: case 254
@z

@x
mpx_read_tfm_word(mpx); lh=mpx->b2*(int)(256)+mpx->b3;
@y
mpx_read_tfm_word(mpx);
@<Read the pTeX header data@>;@/
lh=mpx->b2*(int)(256)+mpx->b3;
@z

@x
    if ( mpx->b0<128 ) 
      mpx->tfm_check_sum=((mpx->b0*(int)(256)+mpx->b1)*256+mpx->b2)*256+mpx->b3;
    else 
      mpx->tfm_check_sum=(((mpx->b0-256)*(int)(256)+mpx->b1)*256+mpx->b2)*256+mpx->b3;
  }
  if ( k==5 ) {
    if (mpx->mode == mpx_troff_mode) {
      mpx->font_design_size[f]=(((mpx->b0*(int)(256)+mpx->b1)*256+mpx->b2)*256+mpx->b3)/(65536.0*16);
    }
  }
}
@y
    if ( mpx->b0<128 ) 
      mpx->tfm_check_sum=((mpx->b0*(int)(256)+mpx->b1)*256+mpx->b2)*256+mpx->b3;
    else 
      mpx->tfm_check_sum=(((mpx->b0-256)*(int)(256)+mpx->b1)*256+mpx->b2)*256+mpx->b3;
  }
  if ( k==5 ) {
    if (mpx->mode == mpx_troff_mode) {
      mpx->font_design_size[f]=(((mpx->b0*(int)(256)+mpx->b1)*256+mpx->b2)*256+mpx->b3)/(65536.0*16);
    }
  }
}
@<Read JFM character type table@>;
@z

@x
@<Width of character |c| in font |f|@>=
floor(mpx->dvi_scale*mpx->font_scaled_size[f]*char_width(f,c))

@ @<Width of character |p| in font |cur_font|@>=
floor(mpx->dvi_scale*mpx->font_scaled_size[cur_font]*char_width(cur_font,p))
@y
@ @c @<Declare JFM character type table lookup routine@>@; /* \pTeX */
static integer mpx_scaled_char_width (MPX mpx,integer f,integer c)
{
  if (mpx->font_id[f]!=0) c=mpx_lookup_ctype(mpx, f,c);
  return floor(mpx->dvi_scale*mpx->font_scaled_size[f]*char_width(f,c));
}
@z

@x
@ @c @<Declare a procedure called |finish_last_char|@>@;
static void mpx_do_set_char (MPX mpx,web_integer f, web_integer c) {
  if ( (c<mpx->font_bc[f])||(c>mpx->font_ec[f]) )
    mpx_abort(mpx,"attempt to typeset invalid character %d",c);
@.attempt to typeset...@>
@y
@ (u)pDVItoMP: |do_set_char| is called with non-virtual font.
In the case of non-virtual Kanji font, the width is looked up
with the character type, and the character is printed by the
function |set_kanji_char|.

When the width written in the virtual font is same as
the width of the substituted font, the next character can be
written in the same string in output mpx file.
In other words, the width of the character is calculated
in |do_dvi_commands|. So even if the width is wrong here, the output PostScript file is not affected.

@ @c @<Declare a procedure called |finish_last_char|@>@;
static void mpx_do_set_char (MPX mpx,integer f,integer c)
{
integer kkk;

if (mpx->font_id[f]!=0) {
  kkk=mpx_lookup_ctype(mpx, f,c);
  if ( (kkk<mpx->font_bc[f]) || (kkk>mpx->font_ec[f]) )
    mpx_abort(mpx,"attempt to typeset invalid character (JFM) %d",c);
} else if ( (c<mpx->font_bc[f])||(c>mpx->font_ec[f]) ) {
  mpx_abort(mpx,"attempt to typeset invalid character %d",c);
}
@.attempt to typeset...@>
@z

@x
  if ((mpx->h!=mpx->str_h2)||(mpx->v!=mpx->str_v)||
      (f!=mpx->str_f)||(mpx->dvi_scale!=mpx->str_scale) ) {
@y
  if ((mpx->h!=mpx->str_h2)||(mpx->v!=mpx->str_v2)||
      (f!=mpx->str_f)||(mpx->dvi_scale!=mpx->str_scale) ) {
@z

@x
    fprintf(mpx->mpxfile,"_s("); mpx->print_col=3;@/
    mpx->str_scale=mpx->dvi_scale; mpx->str_f=f; 
    mpx->str_v=mpx->v; mpx->str_h1=mpx->h;
@y
  if ((mpx->d==0) || (mpx->font_id[f]==9)) {
    fprintf(mpx->mpxfile,"_s("); mpx->print_col=3;}@/
  else {
    fprintf(mpx->mpxfile,"_sr("); mpx->print_col=4;}@/
  mpx->str_scale=mpx->dvi_scale; mpx->str_f=f;
  mpx->str_v1=mpx->v; mpx->str_h1=mpx->h;
@z

@x
  mpx_print_char(mpx, (unsigned char)c);
  mpx->str_h2=(web_integer)(mpx->h+@<Width of character |c| in font |f|@>);
@y
  if (mpx->font_id[f]!=0)
    mpx_print_kanji_char(mpx, c);
  else
    mpx_print_char(mpx, c);
  if (mpx->d==0) {
    mpx->str_h2=mpx->h+mpx_scaled_char_width(mpx, f, c);
    mpx->str_v2=mpx->v;
  } else {
    mpx->str_h2=mpx->h;
    mpx->str_v2=mpx->v+mpx_scaled_char_width(mpx, f, c);
  }
@z

@x
web_integer str_h1;
web_integer str_v; /* starting position for current output string */
web_integer str_h2; /* where the current output string ends */
@y
integer str_h1;
integer str_v1; /* starting position for current output string */
integer str_h2;
integer str_v2; /* where the current output string ends */
@z

@x
  fprintf(mpx->mpxfile,"vardef _s(expr _t,_f,_m,_x,_y)(text _c)=\n");
  fprintf(mpx->mpxfile,
          "  addto _p also _t infont _f scaled _m shifted (_x,_y) _c; enddef;\n");
@y
  fprintf(mpx->mpxfile,"vardef _s(expr _t,_f,_m,_x,_y)(text _c)=\n");
  fprintf(mpx->mpxfile,
          "  addto _p also _t infont _f scaled _m shifted (_x,_y) _c; enddef;\n");
  fprintf(mpx->mpxfile,"vardef _sr(expr _t,_f,_m,_x,_y)=\n");  
  fprintf(mpx->mpxfile,"  addto _p also _t infont _f rotated -90");
  fprintf(mpx->mpxfile," scaled _m shifted (_x,_y); enddef;\n");
@z

@x
      x=mpx->conv*mpx->str_h1; 
      y=mpx->conv*(-mpx->str_v);
@y
      x=mpx->conv*mpx->str_h1;
      y=mpx->conv*(-mpx->str_v1);
@z

@x
@<Handle a special rule that determines the box size@>=
{ 
  mpx->pic_wd=mpx->h; mpx->pic_dp=mpx->v; mpx->pic_ht=ht-mpx->v; 
}
@y
@<Handle a special rule that determines the box size@>=
{ if (mpx->d==0) {
    mpx->pic_wd=mpx->h; mpx->pic_dp=mpx->v; mpx->pic_ht=ht-mpx->v;
  } else {
    mpx->pic_wd=mpx->v; mpx->pic_dp=-mpx->h; mpx->pic_ht=ht+mpx->h;
  }
}
@z

@x
  mpx->str_v=0;
  mpx->str_h2=0;
  mpx->str_scale=1.0; /* values don't matter */
@y
  mpx->str_h2=0;
  mpx->str_v2=0;
  mpx->str_scale=1.0; /* values don't matter */
@z

@x
dd=-mpx->pic_dp*mpx->conv;
w=mpx->conv*mpx->pic_wd; 
h=mpx->conv*mpx->pic_ht;
fprintf(mpx->mpxfile,
        "setbounds _p to (0,%1.4f)--(%1.4f,%1.4f)--\n" 
        " (%1.4f,%1.4f)--(0,%1.4f)--cycle;\n",dd,w,dd,w,h,h)
@y
if (mpx->d==0) {
  dd=-mpx->pic_dp*mpx->conv;
  w=mpx->conv*mpx->pic_wd;
  h=mpx->conv*mpx->pic_ht;
  fprintf(mpx->mpxfile,
        "setbounds _p to (0,%1.4f)--(%1.4f,%1.4f)--\n",dd,w,dd);
  fprintf(mpx->mpxfile,
        " (%1.4f,%1.4f)--(0,%1.4f)--cycle;\n",w,h,h);
} else {
  dd=-mpx->pic_dp*mpx->conv;
  w=-mpx->pic_wd*mpx->conv;
  h=mpx->conv*mpx->pic_ht;
  fprintf(mpx->mpxfile,
        "setbounds _p to (%1.4f,0)--(%1.4f,%1.4f)--\n",h,h,w);
  fprintf(mpx->mpxfile,
        " (%1.4f,%1.4f)--(%1.4f,0)--cycle;\n", dd,w,dd);
}
@z

@x
web_integer w;web_integer x;web_integer y;web_integer z;
  /* current state values (|h| and |v| have already been declared) */
web_integer hstack[(stack_size+1)];
web_integer vstack[(stack_size+1)];
web_integer wstack[(stack_size+1)];
web_integer xstack[(stack_size+1)];
web_integer ystack[(stack_size+1)];
web_integer zstack[(stack_size+1)]; /* pushed down values in \.{DVI} units */
@y
integer w;integer x;integer y;integer z;integer d;
  /* current state values (|h| and |v| have already been declared) */
integer hstack[(stack_size+1)];
integer vstack[(stack_size+1)];
integer wstack[(stack_size+1)];
integer xstack[(stack_size+1)];
integer ystack[(stack_size+1)];
integer zstack[(stack_size+1)];
integer dstack[(stack_size+1)]; /* pushed down values in \.{DVI} units */
@z

@x
mpx->h=0; mpx->v=0;
@y
mpx->h=0; mpx->v=0; mpx->d=0;
@z

@x
  mpx->hstack[mpx->stk_siz]=mpx->h; 
  mpx->vstack[mpx->stk_siz]=mpx->v; mpx->wstack[mpx->stk_siz]=mpx->w;
  mpx->xstack[mpx->stk_siz]=mpx->x; 
  mpx->ystack[mpx->stk_siz]=mpx->y; mpx->zstack[mpx->stk_siz]=mpx->z;
@y 
  mpx->hstack[mpx->stk_siz]=mpx->h; 
  mpx->vstack[mpx->stk_siz]=mpx->v; mpx->wstack[mpx->stk_siz]=mpx->w;
  mpx->xstack[mpx->stk_siz]=mpx->x; 
  mpx->ystack[mpx->stk_siz]=mpx->y; mpx->zstack[mpx->stk_siz]=mpx->z;
  mpx->dstack[mpx->stk_siz]=mpx->d;
@z

@x
    mpx->h=mpx->hstack[mpx->stk_siz]; 
    mpx->v=mpx->vstack[mpx->stk_siz]; mpx->w=mpx->wstack[mpx->stk_siz];
    mpx->x=mpx->xstack[mpx->stk_siz]; 
    mpx->y=mpx->ystack[mpx->stk_siz]; mpx->z=mpx->zstack[mpx->stk_siz];
@y
    mpx->h=mpx->hstack[mpx->stk_siz]; 
    mpx->v=mpx->vstack[mpx->stk_siz]; mpx->w=mpx->wstack[mpx->stk_siz];
    mpx->x=mpx->xstack[mpx->stk_siz]; 
    mpx->y=mpx->ystack[mpx->stk_siz]; mpx->z=mpx->zstack[mpx->stk_siz];
    mpx->d=mpx->dstack[mpx->stk_siz];
@z

@x
  case z0: return mpx->z; break;
@y
  case z0: return mpx->z; break;
  case dir: return mpx_get_byte(mpx); break;
@z

@x
    mpx->h += @<Width of character |p| in font |cur_font|@>;
@y
    if (mpx->d==0) {
      mpx->h+=mpx_scaled_char_width(mpx, cur_font, p);
    } else {
      mpx->v+=mpx_scaled_char_width(mpx, cur_font, p);
    }
@z

@x
      mpx->h += q;
@y
      if (mpx->d==0) {
        mpx->h += q;
      } else {
        mpx->v += q;
      }
@z

@x
case pop: 
  mpx_do_pop(mpx);
  break;
@y
case pop: 
  mpx_do_pop(mpx);
  break;
case dir:
  mpx->d=p;
  break;
@z

@x
case four_cases(right1):
  mpx->h += trunc(p*mpx->dvi_scale);
  break;
case w0: case four_cases(w1): 
  mpx->w = (web_integer)trunc(p*mpx->dvi_scale); mpx->h += mpx->w;
  break;
case x0: case four_cases(x1): 
  mpx->x = (web_integer)trunc(p*mpx->dvi_scale); mpx->h += mpx->x;
  break;
case four_cases(down1):
  mpx->v += trunc(p*mpx->dvi_scale);
  break;
case y0: case four_cases(y1): 
  mpx->y = (web_integer)trunc(p*mpx->dvi_scale); mpx->v += mpx->y;
  break;
case z0: case four_cases(z1): 
  mpx->z = (web_integer)trunc(p*mpx->dvi_scale); mpx->v += mpx->z;
  break;
@y
case four_cases(right1):
  if (mpx->d==0) {
    mpx->h+=trunc(p*mpx->dvi_scale);
  } else {
    mpx->v+=trunc(p*mpx->dvi_scale);
  }
  break;
case w0: case four_cases(w1):
  if (mpx->d==0) {
    mpx->h+=mpx->w;
  } else {
    mpx->v+=mpx->w;
  }
  break;
case x0: case four_cases(x1):
  if (mpx->d==0) {
    mpx->h+=mpx->x;
  } else {
    mpx->v+=mpx->x;
  }
  break;
case four_cases(down1):
  if (mpx->d==0) {
    mpx->v+=trunc(p*mpx->dvi_scale);
  } else {
    mpx->h-=trunc(p*mpx->dvi_scale);
  }
  break;
case y0: case four_cases(y1):
  if (mpx->d==0) {
    mpx->v+=mpx->y;
  } else {
    mpx->h-=mpx->y;
  }
  break;
case z0: case four_cases(z1):
  if (mpx->d==0) {
    mpx->v+=mpx->z;
  } else {
    mpx->h-=mpx->z;
  }
  break;
@z

@x
    mpx->progname = "dvitomp";
@y
#ifdef UPMP
    mpx->progname = "updvitomp";
#else
    mpx->progname = "pdvitomp";
#endif
@z

@x
@<Check if mp file is newer than mpxfile, exit if not@>=
if (mpx_newer(mpxopt->mpname, mpxopt->mpxname))
   return 0
@y
@<Check if mp file is newer than mpxfile, exit if not@>=
if (mpx_newer(mpxopt->mpname, mpxopt->mpxname))
   return 0

@ ASCII \pTeX\ JFM ID
@d yoko_jfm_id   11 /* for `yoko-kumi' fonts */
@d tate_jfm_id   9  /* for `tate-kumi' fonts */
@d font_jfm_p(A)   (mpx->font_id[(A)]!=0)

@ @<Global...@>=
integer font_nt[max_fonts+1]; /* number of words in ctype table */
integer font_id[max_fonts+1];
integer jfm_char_code[max_widths+1];
integer jfm_char_type[max_widths+1];
integer jfm_char_index[max_fonts+1];
integer next_jfm_char_index;

@ @<Set init...@>=
mpx->font_nt[0]=0;
mpx->font_id[0]=0;
mpx->jfm_char_type[0]=0;
mpx->next_jfm_char_index=0;

@ JFM character type table is stored in the array |jfm_char_code| and
|jfm_char_type|. The character code and the character type of $i$-th
record is stored in |jfm_char_code[i]| and |jfm_char_type[i]|, respectively.
The table is in the order of character code.

@<Read the pTeX header data@>=
mpx->font_id[f]=mpx->b0*(int)(256)+mpx->b1;
if ((mpx->font_id[f]==yoko_jfm_id) || (mpx->font_id[f]==tate_jfm_id)) {
  mpx->font_nt[f]=mpx->b2*(int)(256)+mpx->b3;
  mpx_read_tfm_word(mpx);
} else {
  mpx->font_id[f]=0;
  mpx->font_nt[f]=0;
}

@ @<Read JFM character type table@>=
mpx->jfm_char_index[f]=mpx->next_jfm_char_index;
k=mpx->jfm_char_index[f];
mpx->next_jfm_char_index+=mpx->font_nt[f];
while (k<mpx->next_jfm_char_index) {
  mpx_read_tfm_word(mpx);
  mpx->jfm_char_code[k]=mpx->b0*(int)(256)+mpx->b1;
  mpx->jfm_char_type[k]=mpx->b2*(int)(256)+mpx->b3;
  k++;
}

@ JFM character type table is looked up by binary search.

@<Declare JFM character type table lookup routine@>=
static integer mpx_lookup_ctype (MPX mpx,integer f, integer c)
{
  integer l, u, r, ch;
  l=0; u=mpx->font_nt[f]-1;
  while (l<u) {
    r=(l+u)/2;
    ch=mpx->jfm_char_code[mpx->jfm_char_index[f]+r];
    if (ch==c) {
      return mpx->jfm_char_type[mpx->jfm_char_index[f]+r];
    }
    if (ch<c)
      l=r+1;
    else
      u=r-1;
  }
  return 0;
}

@ All Kanji characters are supposed to be printable here,
so that the state always results in normal at the end of the procedure.
Kanji characters need to be converted into output Kanji encoding
from DVI(JIS).

@<Declare subroutines for printing strings@>=
static void mpx_print_kanji_char (MPX mpx,integer c)
{
#ifdef UPMP
#define BYTE1(x) (((x)>>24) & 0xff)
#define BYTE2(x) (((x)>>16) & 0xff)
#define BYTE3(x) (((x)>> 8) & 0xff)
#define BYTE4(x) ( (x)      & 0xff)
  int pflag = 0;
#else
#define Hi(x) (((x)>> 8) & 0xff)
#define Lo(x) ( (x)      & 0xff)
#endif
  if (mpx->print_col+2>line_length-2 ) {
    if (mpx->state==normal) {
      fprintf(mpx->mpxfile, "\"");
      mpx->state=special;
    }
    fprintf(mpx->mpxfile, " \n");
    mpx->print_col=0;
  }
  if (mpx->state==special) {
    fprintf(mpx->mpxfile,"&");
    mpx->print_col++;
  }
  if (mpx->state!=normal) {
    fprintf(mpx->mpxfile, "\"");
    mpx->print_col++;
    mpx->state=normal;
  }
  c=toBUFF(fromDVI(c));
#ifdef UPMP
  if (BYTE1(c)!=0) pflag = 1;
  if (pflag) {
    putc2(BYTE1(c), mpx->mpxfile);
    mpx->print_col++;
  }
  if (BYTE2(c)!=0) pflag = 1;
  if (pflag) {
    putc2(BYTE2(c), mpx->mpxfile);
    mpx->print_col++;
  }
  if (BYTE3(c)!=0) pflag = 1;
  if (pflag) {
    putc2(BYTE3(c), mpx->mpxfile);
    mpx->print_col++;
  }
  putc2(BYTE4(c), mpx->mpxfile);
#else
  putc2(Hi(c), mpx->mpxfile);
  mpx->print_col++;
  putc2(Lo(c), mpx->mpxfile);
#endif
  mpx->print_col++;
}
@z
