/*
 *  KANJI Code conversion routines.
 */

#include <ptexenc/c-auto.h>
#include <ptexenc/kanjicnv.h>

boolean isEUCkanji1(int c)
{
    c &= 0xff;
    return (0xa1 <= c && c <= 0xfe);
}

boolean isEUCkanji2(int c)
{
    c &= 0xff;
    return (0xa1 <= c && c <= 0xfe);
}

boolean isSJISkanji1(int c)
{
    c &= 0xff;
    return ((0x81 <= c && c <= 0x9f) || (0xe0 <= c && c <= 0xfc));
}

boolean isSJISkanji2(int c)
{
    c &= 0xff;
    return (0x40 <= c && c <= 0xfc && c != 0x7f);
}

/* EUC <=> JIS X 0208 code conversion */
int EUCtoJIS(int kcode)
{
    return (kcode & 0x7f7f);
}

int JIStoEUC(int kcode)
{
    return (kcode | 0x8080);
}

/* Shift JIS <=> JIS Kanji code conversion */
int SJIStoJIS(int kcode)
{
    int byte1, byte2;

    byte1 = HI(kcode);
    byte2 = LO(kcode);
    byte1 -= ( byte1>=0xa0 ) ? 0xc1 : 0x81;
    kcode = ((byte1<<1) + 0x21)<<8;
    if ( byte2 >= 0x9f ) {
        kcode += 0x0100;
        kcode |= (byte2 - 0x7e) & 0xff;
    } else {
        kcode |= (byte2 - ((byte2<=0x7e) ? 0x1f : 0x20 )) & 0xff;
    }
    return kcode;
}

int JIStoSJIS(int kcode)
{
    int high, low;
    int nh,   nl;

    high = HI(kcode);
    low  = LO(kcode);
    nh = ((high-0x21)>>1) + 0x81;
    if (nh > 0x9f) nh += 0x40;
    if (high & 1) {
        nl = low + 0x1f;
        if (low > 0x5f) nl++;
    } else
        nl = low + 0x7e;
    if (isSJISkanji1(nh) && isSJISkanji2(nl)) {
        return HILO(nh, nl);
    } else {
        return 0x813f;
    }
}

/* Shift JIS <=> EUC Kanji code conversion */
int SJIStoEUC(int kcode)
{
    return JIStoEUC(SJIStoJIS(kcode));
}

int EUCtoSJIS(int kcode)
{
    return JIStoSJIS(EUCtoJIS(kcode));
}

/* KUTEN to JIS kanji code conversion */
int KUTENtoJIS(int kcode)
{
    /* in case of undefined in kuten code table */
    if (HI(kcode) == 0 || HI(kcode) > 95) return -1;
    if (LO(kcode) == 0 || LO(kcode) > 95) return -1;

    return kcode + 0x2020;
}
