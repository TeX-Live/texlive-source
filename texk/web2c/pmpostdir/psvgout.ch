@x
#include <math.h>
@y
#include <math.h>
#include <ptexenc/ptexenc.h>
#ifdef UPMP
#include <ptexenc/unicode.h>
#endif
@z

@x 
@<Character |k| is not allowed in SVG output@>=
 (k=='&')||(k=='>')||(k=='<')
@y
@<Character |k| is not allowed in SVG output@>=
 (k=='&')||(k=='>')||(k=='<')||(k>=0x7F)
@z

@x
@ Now for outputting the actual graphic objects. 
@y
@ Now for outputting the actual graphic objects. 

@d yoko_jfm_id   11 /* `yoko-kumi' fonts */
@d tate_jfm_id   9  /* `tate-kumi' fonts */
@d font_jfm_p(A) (mp->font_id[(A)]!=0)
@z

@x Even if prologues=3, Japanese texts are not converted into paths.
  if (prologues == 3 ) {
@y
  if (prologues == 3 && mp->font_id[gr_font_n(p)]==0) {
@z

@x To realise vertical Japanese texts ...
    mp_svg_attribute(mp, "font-size", mp->svg->buf);
@y
    mp_svg_attribute(mp, "font-size", mp->svg->buf);
    if ( mp->font_id[gr_font_n(p)]==tate_jfm_id ) {
      mp_svg_reset_buf(mp);
      append_string("tb-rl");
      mp_svg_attribute(mp, "writing-mode", mp->svg->buf);
    }
@z

@x
    while (l-->0) {
@y
    if ( mp->font_id[gr_font_n(p)]!=0 ) {
#ifdef UPMP
        if  (is_internalUPTEX() ) { 
            while (l>0) {
               append_string("&#");
                mp_svg_store_int(mp,UTF8StoUCS(s));
                append_char(';');
                k = UTF8length((int)*s); s += k; l -= k; 
            }
        } else /* 1 charatcer = 2 bytes */
#endif
        {
            l >>= 1;
            while (l-->0) {
                k =((int)*s++)*0x100; k += (int)*s++;
                append_string("&#");
                mp_svg_store_int(mp,toUCS(k));
                append_char(';');
            }
        }
    } else
    while (l-->0) {
@z

@x
    mp_svg_print(mp, "<?xml version=\"1.0\"?>");
@y
    mp_svg_print(mp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>");
@z
