% jMetaPost change file for MetaPost
%
% written by Michio Matsuyama <fwhw5892@mb.infoweb.ne.jp>
%            Hideyuki Suzuki <hideyuki@sat.t.u-tokyo.ac.jp>
%
% $Id: jmp.ch,v 1.42 2000/03/20 02:55:50 hideyuki Exp $

%
%
% change file of mp.w for pMetaPost (CWEB version)
% Akira Kakuto (translated the WEB version)
%
%

@x
@d default_banner "This is MetaPost, Version 2.02" /* printed when \MP\ starts */
@y
@z

@x
#define metapost_version "2.02"
@y
#ifdef UPMP
#define P_UP "up"
#define metapost_version "2.02-0.04-u1.27"
#else
#define P_UP "p"
#define metapost_version "2.02-0.04"
#endif
#define default_banner "This is " P_UP "MetaPost, Version " metapost_version /* printed when \MP\ starts */
@z

@x
#  include <unistd.h>           /* for access */
#endif
@y
#  include <unistd.h>           /* for access */
#endif
#include <kpathsea/config.h>
#include <kpathsea/kpathsea.h>
#include <ptexenc/ptexenc.h>
#include <errno.h>
@z

@x
void mp_close_file (MP mp, void *f) {
  (void) mp;
  if (f != NULL)
    fclose ((FILE *) f);
@y
void mp_close_file (MP mp, void *f) {
  (void) mp;
#ifdef WIN32
  if (f != NULL) {
    clear_infile_enc((FILE *)f);
    fclose ((FILE *) f);
  }
#else
  if (f != NULL)
    fclose ((FILE *) f);
#endif
@z

@x
mp->buf_size = 200;
@y
{
  char *kpse_buf = kpse_var_value("buf_size");
  if(kpse_buf) {
    mp->buf_size = atoi(kpse_buf);
    free(kpse_buf);
  } else {
    mp->buf_size = 500000;
  }
}
@z

@x
static boolean mp_input_ln (MP mp, void *f) {
  /* inputs the next line or returns |false| */
  char *s;
  size_t size = 0;
  mp->last = mp->first;         /* cf.\ Matthew 19\thinspace:\thinspace30 */
  s = (mp->read_ascii_file) (mp, f, &size);
  if (s == NULL)
    return false;
  if (size > 0) {
    mp->last = mp->first + size;
    if (mp->last >= mp->max_buf_stack) {
      mp->max_buf_stack = mp->last + 1;
      while (mp->max_buf_stack > mp->buf_size) {
        mp_reallocate_buffer (mp, (mp->buf_size + (mp->buf_size >> 2)));
      }
    }
    (void) memcpy ((mp->buffer + mp->first), s, size);
  }
  free (s);
  return true;
}
@y
static boolean mp_input_ln (MP mp, void *f ) {
  int i = EOF;
  mp->last = input_line2((FILE *)f, mp->buffer, NULL, mp->first, mp->buf_size, &i);
  if (i == EOF && errno != EINTR && mp->last == mp->first)
    return false;
  if (i != EOF && i != '\n' && i != '\r') {
    fprintf (stderr, "! Unable to read an entire line---bufsize=%u.\n",
                     (unsigned) mp->buf_size);
    fputs ("Please increase buf_size in texmf.cnf.\n", stderr);
    exit (1);
  }
  if (i == '\r') {
    while ((i = getc ((FILE *)f)) == EOF && errno == EINTR)
      ;
    if (i != '\n')
      ungetc (i, f);
  }
  return true;
}
@z

@x
wterm (mp->banner);
@y
wterm (mp->banner);
wterm(" (");
wterm(getencstring());
wterm(")");
@z

@x
} four_quarters;
typedef union {
  integer sc;
  four_quarters qqqq;
} font_data;
@y
} four_quarters;
typedef struct {
    halfword RH, LH;
} two_halves;
typedef union {
  two_halves hh;
  integer sc;
  four_quarters qqqq;
} font_data;
@z

%
% tategaki support
%
% Suppose h==(0,height), d==(0,-depth) && w==(width,0) in horizontal string,
% && h==(height,0), d==(-depth,0) && w==(0,-width) in vertical string.
% Four vertices of the bounding box is h, d, h+w && d+w && those of the
% transformed boundig box is Th, Td, T(h+w) && T(d+w), so that the values
% of Th, Td && Tw are compared here.

@x
@ The height width and depth information stored in a text node determines a
rectangle that needs to be transformed according to the transformation
parameters stored in the text node.

@y
@ The height width and depth information stored in a text node determines a
rectangle that needs to be transformed according to the transformation
parameters stored in the text node.

Boundig box depends on JFM font ID.

@d yoko_jfm_id   11 /* `yoko-kumi' fonts */
@d tate_jfm_id   9  /* `tate-kumi' fonts */
@d font_jfm_p(A) (mp->font_id[(A)]!=0)

@z

@x
  mp_number x0a, y0a, x1a, y1a, arg1;
  mp_text_node p0 = (mp_text_node)p;
  new_number (x0a);
  new_number (x1a);
  new_number (y0a);
  new_number (y1a);
  new_number (arg1);
  number_clone (arg1, p0->depth);
  number_negate (arg1);
  take_scaled (x1a, p0->txx, p0->width);
  take_scaled (y0a, p0->txy, arg1);
  take_scaled (y1a, p0->txy, p0->height);
@y
  mp_number x0a, y0a, x1a, y1a, arg1, arg2;
  mp_text_node p0 = (mp_text_node)p;
  new_number (x0a);
  new_number (x1a);
  new_number (y0a);
  new_number (y1a);
  new_number (arg1);
  new_number (arg2);
  number_clone (arg1, p0->depth);
  number_negate (arg1);
  number_clone (arg2, p0->width);
  number_negate (arg2);
  if ( mp->font_id[mp_font_n(p)]!=tate_jfm_id ) {
     take_scaled (x1a, p0->txx, p0->width);
     take_scaled (y0a, p0->txy, arg1);
     take_scaled (y1a, p0->txy, p0->height);
  } else {
     take_scaled (x1a, p0->txy, arg2);
     take_scaled (y0a, p0->txx, arg1);
     take_scaled (y1a, p0->txx, p0->height);
  }
@z

@x
  take_scaled (x1a, p0->tyx, p0->width);
  number_clone (arg1, p0->depth);
  number_negate (arg1);
  take_scaled (y0a, p0->tyy, arg1);
  take_scaled (y1a, p0->tyy, p0->height);
@y
  if ( mp->font_id[mp_font_n(p)]!=tate_jfm_id ) {
     take_scaled (x1a, p0->tyx, p0->width);
     number_clone (arg1, p0->depth);
     number_negate (arg1);
     take_scaled (y0a, p0->tyy, arg1);
     take_scaled (y1a, p0->tyy, p0->height);
  } else {
     number_clone (arg1, p0->depth);
     number_negate (arg1);
     number_clone (arg2, p0->width);
     number_negate (arg2);
     take_scaled (x1a, p0->tyy, arg2);
     take_scaled (y0a, p0->tyx, arg1);
     take_scaled (y1a, p0->tyx, p0->height);
  }
@z

@x
  wlog (mp->banner);
@y
  wlog (mp->banner);
  wlog (" (");
  wlog (getencstring());
  wlog (")");
@z

%
% char type pointers
@x
eight_bits *font_bc;
eight_bits *font_ec;    /* first and last character code */
@y
eight_bits  *font_bc;
eight_bits  *font_ec;  /* first and last character code */
halfword    *font_nt;
halfword    *font_id;
@z

@x
int *char_base; /* base address for |char_info| */
@y
int *char_base;  /* base address for |char_info| */
int *ctype_base;
@z

@x
xfree (mp->char_base);
@y
xfree (mp->font_id);
xfree (mp->font_nt);
xfree (mp->char_base);
xfree (mp->ctype_base);
@z

@x
  XREALLOC (mp->char_base, l, int);
@y
  XREALLOC (mp->font_id, l, halfword);
  XREALLOC (mp->font_nt, l, halfword);
  XREALLOC (mp->char_base, l, int);
  XREALLOC (mp->ctype_base, l, int);
@z


@x
mp->char_base[null_font] = 0;
@y
mp->font_id[null_font] = 0;
mp->font_nt[null_font] = 0;
mp->char_base[null_font] = 0;
mp->ctype_base[null_font] = 0;
@z

@x
@d char_mp_info(A,B) mp->font_info[mp->char_base[(A)]+(B)].qqqq
@y
@d char_mp_info(A,B) mp->font_info[mp->char_base[(A)]+(B)].qqqq
@d ctype_char_end(A) (A)].hh.LH
@d ctype_char(A) mp->font_info[mp->ctype_base[(A)]+ctype_char_end
@d ctype_type_end(A) (A)].hh.RH
@d ctype_type(A) mp->font_info[mp->ctype_base[(A)]+ctype_type_end
@z

%
% lookup character type table
@x
void mp_set_text_box (MP mp, mp_text_node p) {
@y
@<Declare JFM function for text measuring@>;
void mp_set_text_box (MP mp, mp_text_node p) {
@z

@x
  if ((*(mp_text_p (p)->str + k) < bc) || (*(mp_text_p (p)->str + k) > ec)) {
    mp_lost_warning (mp, f, *(mp_text_p (p)->str + k));
  } else {
    cc = char_mp_info (f, *(mp_text_p (p)->str + k));
@y
  if ( ((*(mp_text_p (p)->str + k) < bc) || (*(mp_text_p (p)->str + k) > ec)) && (mp->font_id[f]==0) ) {
    mp_lost_warning (mp, f, *(mp_text_p (p)->str + k));
  } else {
    if (mp->font_id[f]!=0) {
#ifdef UPMP
      integer cx;           /* code for Japanese two byte character */
      if (multistrlen(mp_text_p (p)->str, kk, k)>1) {
        cx=fromBUFF(mp_text_p (p)->str, kk, k);
        k=k+multistrlen(mp_text_p (p)->str, kk, k)-1;
      } else {
        cx=*(mp_text_p (p)->str + k);
      }
      cc=char_mp_info(f,mp_lookup_ctype(mp, f, cx));
#else
      cc=char_mp_info(f,mp_lookup_ctype(mp, f,fromBUFF(mp_text_p(p)->str,limit,k)));
      k++;
#endif
    } else {
      cc = char_mp_info (f, *(mp_text_p (p)->str + k));
    }
@z

@x
@* Debugging.
@y
@ @<Declare JFM function for text measuring@>=
static int mp_lookup_ctype (MP mp,font_number f, integer c)
{
  int l, u, r, ch;
  l=0; u=mp->font_nt[f]-1;
  while ( l<u ) {
    r=(l+u)/2;
    ch=ctype_char(f)(r);
    if ( ch==c ) {
      return ctype_type(f)(r);};
    if ( ch<c ) l=r+1;
    else u=r-1;
  };
  return 0;
}
@* Debugging.
@z
