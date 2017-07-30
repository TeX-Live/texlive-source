%
%
% change file of psout.w for jMetaPost (CWEB version)
% Akira Kakuto (translated the WEB version)
%
%

@x
#include <math.h>
@y
#include <math.h>
#include <ptexenc/ptexenc.h>
#ifdef UPMP
#include <ptexenc/unicode.h>
#endif
@z

% Treat all Kanji fonts as used
@x
    if ( mp->font_info[p].qqqq.b3==mp_used ) 
@y
    if ( mp->font_info[p].qqqq.b3==mp_used || mp->font_id[f]!=0)
@z

%
% Kanji string output
@x
static void mp_print_initial_comment(MP mp,mp_edge_object *hh, int prologues);
@y
static void mp_print_initial_comment(MP mp,mp_edge_object *hh, int prologues);

#define Hi(x) (((x) >> 8) & 0xff)
#define Lo(x) ((x) & 0xff)

@ @c
static void mp_ps_kanji_string_out (MP mp, char *s)
{
  int i, c;
  size_t len;

  len = strlen(s);
  i=0;
  mp_ps_print(mp, "<");
  while (i<len) {
    if ( mp->ps->ps_offset+5>mp->max_print_line )
      mp_ps_print_ln(mp);
#ifdef UPMP
    c=toDVI(fromBUFF((unsigned char*)s, len, i));
    i=i+multistrlen((unsigned char*)s, len, i);
    if (isinternalUPTEX() && c>65535) {
      int cx=UTF32toUTF16HS(c); /* High surrogate */
      mp_hex_digit_out(mp, Hi(cx) / 16);
      mp_hex_digit_out(mp, Hi(cx) % 16);
      mp_hex_digit_out(mp, Lo(cx) / 16);
      mp_hex_digit_out(mp, Lo(cx) % 16);
      c=UTF32toUTF16LS(c); /* Low surrogate */
    }
#else
    c=toDVI(fromBUFF((unsigned char*)s, i+2, i));
    i=i+2;
#endif
    mp_hex_digit_out(mp, Hi(c) / 16);
    mp_hex_digit_out(mp, Hi(c) % 16);
    mp_hex_digit_out(mp, Lo(c) / 16);
    mp_hex_digit_out(mp, Lo(c) % 16);
    };
  mp_ps_print(mp, ">");
};
@z

@x
  mp_ps_print_nl(mp, "%%Creator: MetaPost ");
@y
  mp_ps_print_nl(mp, "%%Creator: MetaPost ("@= @>P_UP@= @>"MetaPost) ");
@z

%
% Call Kanji string output routine if the font is JFM.
@x
        mp_ps_string_out(mp, gr_text_p(p),gr_text_l(p));
@y
        if (mp->font_id[gr_font_n(p)]!=0)
           mp_ps_kanji_string_out(mp, gr_text_p(p));
        else
           mp_ps_string_out(mp, gr_text_p(p),gr_text_l(p));
@z
