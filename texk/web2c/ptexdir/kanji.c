/*
 *  KANJI Code conversion routines.
 *  (for ptex only)
 */

#include "kanji.h"

/* FIXME:  why not boolean value */
boolean check_kanji(integer c)
{
    /* FIXME:  why not 255 (0xff) */
    if (0 <= c && c <= 256) return -1;  /* ascii without catcode */
    if (iskanji1(Hi(c)) && iskanji2(Lo(c))) return 1;
    return 0;  /* ascii with catcode */
}

boolean is_kanji(integer c)
{
    return (iskanji1(Hi(c)) && iskanji2(Lo(c)));
}

boolean is_char_ascii(integer c)
{
    return (0 <= c && c < 0x100);
}

boolean is_wchar_ascii(integer c)
{
    return (!is_char_ascii(c) && !is_kanji(c));
}

boolean ismultiprn(integer c)
{
    if (iskanji1(c) || iskanji2(c)) return true;
    return false;
}

#ifdef OLDSTYLE
integer calc_pos(integer c)
{
    int c1, c2;

    if(c<256) return(c<<1);
    c1 = c>>8;
    c2 = c & 0xff;
    if(c1) {
		if (is_internalSJIS()) 
			return((c2+(c2<<(c1-0x81)) & 0xff)<<1);
		else
			return((c2+(c2<<(c1-0xa1)) & 0xff)<<1);
    } else
        return(((c2+c2+1) & 0xff)<<1);
}
#else /* OLDSTYLE */
integer calc_pos(integer c)
{
    unsigned char c1, c2;

    if(c>=0 && c<=255) return(c);
    c1 = (c >> 8) & 0xff;
    c2 = c & 0xff;
    if(iskanji1(c1)) {
		if (is_internalSJIS()) {
			c1 = ((c1 - 0x81) % 4) * 64;  /* c1 = 0, 64, 128, 192 */
			c2 = c2 % 64;                 /* c2 = 0..63 */
		} else {
			c1 = ((c1 - 0xa1) % 4) * 64;  /* c1 = 0, 64, 128, 192 */
			c2 = c2 % 64;                 /* c2 = 0..63 */
		}
		return(c1 + c2);              /* ret = 0..255 */
    } else
        return(c2);
}
#endif /* OLDSTYLE */

integer kcatcodekey(integer c)
{
    return Hi(toDVI(c));
}
