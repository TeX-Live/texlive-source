/*
 *  KANJI Code conversion routines.
 *  (for upTeX and e-upTeX)
 */

#include "kanji.h"

#define CS_TOKEN_FLAG  0x1FFFFFFF
#define CJK_CHAR_LIMIT  0x1000000
#define CJK_TOKEN_FLAG   0xFFFFFF
#define KCAT_KANJI     16
#define KCAT_HANGUL    19

#if !defined(WIN32)
int sjisterminal;
#endif

/*  TOKEN */
boolean check_kanji (integer c)
{
    if (c > CS_TOKEN_FLAG) return false;
    else if (!(XXHi(c)>=KCAT_KANJI && XXHi(c)<=KCAT_HANGUL)) return false;
    else return is_char_kanji(c);
}

boolean is_char_ascii(integer c)
{
    return (0 <= c && c < 0x100);
}

boolean is_char_kanji(integer c)
{
    if (is_internalUPTEX()) 
        return (c>=0&&(c & CJK_TOKEN_FLAG) < CJK_CHAR_LIMIT);
    else
        return iskanji1(Hi(c)) && iskanji2(Lo(c));
}

boolean ismultiprn(integer c)
{
    int i, j;

    for (i=2; i<=4; i++) for (j=1; j<=i; j++) {
        if (ismultichr(i,j,c)) return true;
    }
    return false;
}

integer calc_pos(integer c)
{
    unsigned char c1, c2;

    if(c>=0 && c<=255) return(c);
    c1 = Hi(c);
    c2 = Lo(c);

    c1 = (c1 % 4) * 64;  /* c1 = 0, 64, 128, 192 */
    c2 = c2 % 64;        /* c2 = 0..63 */
    return(c1 + c2);     /* ret = 0..255 */
}

/* Ref. http://www.unicode.org/Public/UNIDATA/Blocks.txt */
static long ucs_range[]={
      0x0000, /* Basic Latin					0x00 */
      0x0080, /* Latin-1 Supplement				0x01 */
      0x0100, /* Latin Extended-A				0x02 */
      0x0180, /* Latin Extended-B				0x03 */
      0x0250, /* IPA Extensions					0x04 */
      0x02B0, /* Spacing Modifier Letters			0x05 */
      0x0300, /* Combining Diacritical Marks			0x06 */
      0x0370, /* Greek and Coptic				0x07 */
      0x0400, /* Cyrillic					0x08 */
      0x0500, /* Cyrillic Supplement				0x09 */
      0x0530, /* Armenian					0x0a */
      0x0590, /* Hebrew						0x0b */
      0x0600, /* Arabic						0x0c */
      0x0700, /* Syriac						0x0d */
      0x0750, /* Arabic Supplement				0x0e */
      0x0780, /* Thaana						0x0f */
      0x07C0, /* NKo						0x10 */
      0x0800, /* Samaritan					0x11 */
      0x0900, /* Devanagari					0x12 */
      0x0980, /* Bengali					0x13 */
      0x0A00, /* Gurmukhi					0x14 */
      0x0A80, /* Gujarati					0x15 */
      0x0B00, /* Oriya						0x16 */
      0x0B80, /* Tamil						0x17 */
      0x0C00, /* Telugu						0x18 */
      0x0C80, /* Kannada					0x19 */
      0x0D00, /* Malayalam					0x1a */
      0x0D80, /* Sinhala					0x1b */
      0x0E00, /* Thai						0x1c */
      0x0E80, /* Lao						0x1d */
      0x0F00, /* Tibetan					0x1e */
      0x1000, /* Myanmar					0x1f */
      0x10A0, /* Georgian					0x20 */
      0x1100, /* Hangul Jamo					0x21 */
      0x1200, /* Ethiopic					0x22 */
      0x1380, /* Ethiopic Supplement				0x23 */
      0x13A0, /* Cherokee					0x24 */
      0x1400, /* Unified Canadian Aboriginal Syllabics		0x25 */
      0x1680, /* Ogham						0x26 */
      0x16A0, /* Runic						0x27 */
      0x1700, /* Tagalog					0x28 */
      0x1720, /* Hanunoo					0x29 */
      0x1740, /* Buhid						0x2a */
      0x1760, /* Tagbanwa					0x2b */
      0x1780, /* Khmer						0x2c */
      0x1800, /* Mongolian					0x2d */
      0x18B0, /* Unified Canadian Aboriginal Syllabics Extended	0x2e */
      0x1900, /* Limbu						0x2f */
      0x1950, /* Tai Le						0x30 */
      0x1980, /* New Tai Lue					0x31 */
      0x19E0, /* Khmer Symbols					0x32 */
      0x1A00, /* Buginese					0x33 */
      0x1A20, /* Tai Tham					0x34 */
      0x1B00, /* Balinese					0x35 */
      0x1B80, /* Sundanese					0x36 */
      0x1C00, /* Lepcha						0x37 */
      0x1C50, /* Ol Chiki					0x38 */
      0x1CD0, /* Vedic Extensions				0x39 */
      0x1D00, /* Phonetic Extensions				0x3a */
      0x1D80, /* Phonetic Extensions Supplement			0x3b */
      0x1DC0, /* Combining Diacritical Marks Supplement		0x3c */
      0x1E00, /* Latin Extended Additional			0x3d */
      0x1F00, /* Greek Extended					0x3e */
      0x2000, /* General Punctuation				0x3f */
      0x2070, /* Superscripts and Subscripts			0x40 */
      0x20A0, /* Currency Symbols				0x41 */
      0x20D0, /* Combining Diacritical Marks for Symbols	0x42 */
      0x2100, /* Letterlike Symbols				0x43 */
      0x2150, /* Number Forms					0x44 */
      0x2190, /* Arrows						0x45 */
      0x2200, /* Mathematical Operators				0x46 */
      0x2300, /* Miscellaneous Technical			0x47 */
      0x2400, /* Control Pictures				0x48 */
      0x2440, /* Optical Character Recognition			0x49 */
      0x2460, /* Enclosed Alphanumerics				0x4a */
      0x2500, /* Box Drawing					0x4b */
      0x2580, /* Block Elements					0x4c */
      0x25A0, /* Geometric Shapes				0x4d */
      0x2600, /* Miscellaneous Symbols				0x4e */
      0x2700, /* Dingbats					0x4f */
      0x27C0, /* Miscellaneous Mathematical Symbols-A		0x50 */
      0x27F0, /* Supplemental Arrows-A				0x51 */
      0x2800, /* Braille Patterns				0x52 */
      0x2900, /* Supplemental Arrows-B				0x53 */
      0x2980, /* Miscellaneous Mathematical Symbols-B		0x54 */
      0x2A00, /* Supplemental Mathematical Operators		0x55 */
      0x2B00, /* Miscellaneous Symbols and Arrows		0x56 */
      0x2C00, /* Glagolitic					0x57 */
      0x2C60, /* Latin Extended-C				0x58 */
      0x2C80, /* Coptic						0x59 */
      0x2D00, /* Georgian Supplement				0x5a */
      0x2D30, /* Tifinagh					0x5b */
      0x2D80, /* Ethiopic Extended				0x5c */
      0x2DE0, /* Cyrillic Extended-A				0x5d */
      0x2E00, /* Supplemental Punctuation			0x5e */
      0x2E80, /* CJK Radicals Supplement			0x5f */
      0x2F00, /* Kangxi Radicals				0x60 */
      0x2FF0, /* Ideographic Description Characters		0x61 */
      0x3000, /* CJK Symbols and Punctuation			0x62 */
      0x3040, /* Hiragana					0x63 */
      0x30A0, /* Katakana					0x64 */
      0x3100, /* Bopomofo					0x65 */
      0x3130, /* Hangul Compatibility Jamo			0x66 */
      0x3190, /* Kanbun						0x67 */
      0x31A0, /* Bopomofo Extended				0x68 */
      0x31C0, /* CJK Strokes					0x69 */
      0x31F0, /* Katakana Phonetic Extensions			0x6a */
      0x3200, /* Enclosed CJK Letters and Months		0x6b */
      0x3300, /* CJK Compatibility				0x6c */
      0x3400, /* CJK Unified Ideographs Extension A		0x6d */
      0x4DC0, /* Yijing Hexagram Symbols			0x6e */
      0x4E00, /* CJK Unified Ideographs				0x6f */
      0xA000, /* Yi Syllables					0x70 */
      0xA490, /* Yi Radicals					0x71 */
      0xA4D0, /* Lisu						0x72 */
      0xA500, /* Vai						0x73 */
      0xA640, /* Cyrillic Extended-B				0x74 */
      0xA6A0, /* Bamum						0x75 */
      0xA700, /* Modifier Tone Letters				0x76 */
      0xA720, /* Latin Extended-D				0x77 */
      0xA800, /* Syloti Nagri					0x78 */
      0xA830, /* Common Indic Number Forms			0x79 */
      0xA840, /* Phags-pa					0x7a */
      0xA880, /* Saurashtra					0x7b */
      0xA8E0, /* Devanagari Extended				0x7c */
      0xA900, /* Kayah Li					0x7d */
      0xA930, /* Rejang						0x7e */
      0xA960, /* Hangul Jamo Extended-A				0x7f */
      0xA980, /* Javanese					0x80 */
      0xAA00, /* Cham						0x81 */
      0xAA60, /* Myanmar Extended-A				0x82 */
      0xAA80, /* Tai Viet					0x83 */
      0xABC0, /* Meetei Mayek					0x84 */
      0xAC00, /* Hangul Syllables				0x85 */
      0xD7B0, /* Hangul Jamo Extended-B				0x86 */
      0xD800, /* High Surrogates				0x87 */
      0xDB80, /* High Private Use Surrogates			0x88 */
      0xDC00, /* Low Surrogates					0x89 */
      0xE000, /* Private Use Area				0x8a */
      0xF900, /* CJK Compatibility Ideographs			0x8b */
      0xFB00, /* Alphabetic Presentation Forms			0x8c */
      0xFB50, /* Arabic Presentation Forms-A			0x8d */
      0xFE00, /* Variation Selectors				0x8e */
      0xFE10, /* Vertical Forms					0x8f */
      0xFE20, /* Combining Half Marks				0x90 */
      0xFE30, /* CJK Compatibility Forms			0x91 */
      0xFE50, /* Small Form Variants				0x92 */
      0xFE70, /* Arabic Presentation Forms-B			0x93 */
      0xFF00, /* Halfwidth and Fullwidth Forms			0x94 */
      0xFFF0, /* Specials                               	0x95 */
      0x10000, /* Linear B Syllabary				0x96 */
      0x10080, /* Linear B Ideograms				0x97 */
      0x10100, /* Aegean Numbers				0x98 */
      0x10140, /* Ancient Greek Numbers				0x99 */
      0x10190, /* Ancient Symbols				0x9a */
      0x101D0, /* Phaistos Disc					0x9b */
      0x10280, /* Lycian					0x9c */
      0x102A0, /* Carian					0x9d */
      0x10300, /* Old Italic					0x9e */
      0x10330, /* Gothic					0x9f */
      0x10380, /* Ugaritic					0xa0 */
      0x103A0, /* Old Persian					0xa1 */
      0x10400, /* Deseret					0xa2 */
      0x10450, /* Shavian					0xa3 */
      0x10480, /* Osmanya					0xa4 */
      0x10800, /* Cypriot Syllabary				0xa5 */
      0x10840, /* Imperial Aramaic				0xa6 */
      0x10900, /* Phoenician					0xa7 */
      0x10920, /* Lydian					0xa8 */
      0x10A00, /* Kharoshthi					0xa9 */
      0x10A60, /* Old South Arabian				0xaa */
      0x10B00, /* Avestan					0xab */
      0x10B40, /* Inscriptional Parthian			0xac */
      0x10B60, /* Inscriptional Pahlavi				0xad */
      0x10C00, /* Old Turkic					0xae */
      0x10E60, /* Rumi Numeral Symbols				0xaf */
      0x11080, /* Kaithi					0xb0 */
      0x12000, /* Cuneiform					0xb1 */
      0x12400, /* Cuneiform Numbers and Punctuation		0xb2 */
      0x13000, /* Egyptian Hieroglyphs				0xb3 */
      0x1D000, /* Byzantine Musical Symbols			0xb4 */
      0x1D100, /* Musical Symbols				0xb5 */
      0x1D200, /* Ancient Greek Musical Notation		0xb6 */
      0x1D300, /* Tai Xuan Jing Symbols				0xb7 */
      0x1D360, /* Counting Rod Numerals				0xb8 */
      0x1D400, /* Mathematical Alphanumeric Symbols		0xb9 */
      0x1F000, /* Mahjong Tiles					0xba */
      0x1F030, /* Domino Tiles					0xbb */
      0x1F100, /* Enclosed Alphanumeric Supplement		0xbc */
      0x1F200, /* Enclosed Ideographic Supplement		0xbd */
      0x20000, /* CJK Unified Ideographs Extension B		0xbe */
      0x2A700, /* CJK Unified Ideographs Extension C		0xbf */
      0x2F800, /* CJK Compatibility Ideographs Supplement	0xc0 */
      0x30000, /* reserved					0xc1 */
      0x40000, /* reserved					0xc2 */
      0x50000, /* reserved					0xc3 */
      0x60000, /* reserved					0xc4 */
      0x70000, /* reserved					0xc5 */
      0x80000, /* reserved					0xc6 */
      0x90000, /* reserved					0xc7 */
      0xA0000, /* reserved					0xc8 */
      0xB0000, /* reserved					0xc9 */
      0xC0000, /* reserved					0xca */
      0xD0000, /* reserved					0xcb */
      0xE0000, /* Tags						0xcc */
      0xE0100, /* Variation Selectors Supplement		0xcd */
      0xF0000, /* Supplementary Private Use Area-A		0xce */
      0x100000, /* Supplementary Private Use Area-B		0xcf */
  /* Value over 0x10FFFF is illegal under Unicode,
     They are for some special use.  *** experimental ***  */
      0x110000, /* Reserved					0xd0 */
      0x120000, /* Reserved					0xd1 */
      0x130000, /* Reserved					0xd2 */
      0x140000, /* Reserved					0xd3 */
      0x150000, /* Reserved					0xd4 */
      0x160000, /* Reserved					0xd5 */
      0x170000, /* Reserved					0xd6 */
      0x180000, /* Reserved					0xd7 */
      0x190000, /* Reserved					0xd8 */
      0x1A0000, /* Reserved					0xd9 */
      0x1B0000, /* Reserved					0xda */
      0x1C0000, /* Reserved					0xdb */
      0x1D0000, /* Reserved					0xdc */
      0x1E0000, /* Reserved					0xdd */
      0x1F0000, /* Reserved					0xde */
      0x200000, /* Reserved					0xdf */
      0x210000, /* Reserved					0xe0 */
      0x220000, /* Reserved					0xe1 */
      CJK_CHAR_LIMIT
};

#define NUCS_RANGE (sizeof(ucs_range)/sizeof(ucs_range[0]))

/* Return i, which satisfies a[i] <= x < a[i+1]. */
/* 'i' is serached between left <= i <= right. */
/* If no i satisfies such condition, return left-1. */
/* [Okumura et al.] An algorithm encyclopedia in Java, p.236. */
static int
binary_search (long x, long *a, int left, int right)
{
    right++;
    while (left < right) {
        int mid = (left + right) / 2;
        if (a[mid] <= x ) left = mid + 1;
        else             right = mid;
    }
    return left - 1;
}

#define FULLWIDTH_DIGIT_0    0xFF10
#define FULLWIDTH_DIGIT_9    0xFF19
#define FULLWIDTH_CAPITAL_A  0xFF21
#define FULLWIDTH_CAPITAL_Z  0xFF3A
#define FULLWIDTH_SMALL_A    0xFF41
#define FULLWIDTH_SMALL_Z    0xFF5A
#define HALFWIDTH_KATAKANA_WO         0xFF66
#define HALFWIDTH_KATAKANA_SMALL_TSU  0xFF6F
#define HALFWIDTH_KATAKANA_A          0xFF71
#define HALFWIDTH_KATAKANA_N          0xFF9D

integer kcatcodekey(integer c)
{
    if (is_internalUPTEX()) {
      if (  (FULLWIDTH_DIGIT_0  <=c && c<=FULLWIDTH_DIGIT_9  )
         || (FULLWIDTH_CAPITAL_A<=c && c<=FULLWIDTH_CAPITAL_Z)
         || (FULLWIDTH_SMALL_A  <=c && c<=FULLWIDTH_SMALL_Z  ) )
        return 0xFE;
      if (  (HALFWIDTH_KATAKANA_WO <=c && c<=HALFWIDTH_KATAKANA_SMALL_TSU )
         || (HALFWIDTH_KATAKANA_A  <=c && c<=HALFWIDTH_KATAKANA_N  ) )
        return 0xFF;
      return binary_search((long)c, ucs_range, 0, NUCS_RANGE-1);
    } else {
        return Hi(toDVI(c));
    }
}

integer multilenbuffchar(integer c)
{
    c=toBUFF(c);
    if (BYTE1(c)) return 4;
    if (BYTE2(c)) return 3;
    if (BYTE3(c)) return 2;
    if (BYTE4(c)) return 1;
    return 0;
}

void init_kanji (const_string file_str, const_string internal_str)
{
    if (!set_enc_string (file_str, internal_str)) {
        fprintf (stderr, "Bad kanji encoding \"%s\" or \"%s\".\n",
                 file_str ? file_str  : "NULL",
                 internal_str ? internal_str : "NULL");
        uexit(1);
    }
}

void init_default_kanji (const_string file_str, const_string internal_str)
{
    char *p;

    enable_UPTEX (true); /* enable */

    p = getenv ("PTEX_KANJI_ENC");
    if (p) {
        if (!set_enc_string (p, NULL))
            fprintf (stderr, "Ignoring bad kanji encoding \"%s\".\n", p);
    }

#ifdef WIN32
    p = kpse_var_value ("guess_input_kanji_encoding");
    if (p) {
        if (*p == '1' || *p == 'y' || *p == 't')
            infile_enc_auto = 1;
        free(p);
    }
#endif

    init_kanji (file_str, internal_str);
}
