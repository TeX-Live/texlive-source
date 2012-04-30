
#ifdef DEBUG
#include <stdio.h>
#endif

#include "uniblock.h"

struct ublock {
  long min, max, cjk;
};

/*
 References:
 [1] http://www.unicode.org/Public/UNIDATA/Blocks.txt
        Blocks-6.1.0.txt
        Date: 2011-06-14, 18:26:00 GMT [KW, LI]
 [2] CMap files
     http://sourceforge.net/adobe/cmap/home/Home/
     cmapresources_cns1-6.tar.z     Adobe-CNS1-6 materials
        cid2code.txt (Version 10/25/2010)
     cmapresources_gb1-5.tar.z      Adobe-GB1-5 materials
        cid2code.txt (Version 10/29/2010)
     cmapresources_japan1-6.tar.z   Adobe-Japan1-6 materials
        cid2code.txt (Version 02/04/2012)
     cmapresources_korean1-2.tar.z  Adobe-Korea1-2 materials
        cid2code.txt (Version 01/31/2012)
*/

static struct ublock ublock_data[] = {
  {0x0000, 0x007F, ENTRY_GCJK}, /* Basic Latin */
  {0x0080, 0x00FF, ENTRY_GCJK}, /* Latin-1 Supplement */
  {0x0100, 0x017F, ENTRY_GCJK}, /* Latin Extended-A */
  {0x0180, 0x024F, ENTRY_GCJ }, /* Latin Extended-B */
  {0x0250, 0x02AF, ENTRY_GCJ }, /* IPA Extensions */
  {0x02B0, 0x02FF, ENTRY_GCJK}, /* Spacing Modifier Letters */
  {0x0300, 0x036F, ENTRY_CJ  }, /* Combining Diacritical Marks */
  {0x0370, 0x03FF, ENTRY_GCJK}, /* Greek and Coptic */
  {0x0400, 0x04FF, ENTRY_GCJK}, /* Cyrillic */
  {0x0500, 0x052F, ENTRY_NO  }, /* Cyrillic Supplement */
  {0x0530, 0x058F, ENTRY_NO  }, /* Armenian */
  {0x0590, 0x05FF, ENTRY_NO  }, /* Hebrew */
  {0x0600, 0x06FF, ENTRY_NO  }, /* Arabic */
  {0x0700, 0x074F, ENTRY_NO  }, /* Syriac */
  {0x0750, 0x077F, ENTRY_NO  }, /* Arabic Supplement */
  {0x0780, 0x07BF, ENTRY_NO  }, /* Thaana */
  {0x07C0, 0x07FF, ENTRY_NO  }, /* NKo */
  {0x0800, 0x083F, ENTRY_NO  }, /* Samaritan */
  {0x0840, 0x085F, ENTRY_NO  }, /* Mandaic */
  {0x08A0, 0x08FF, ENTRY_NO  }, /* Arabic Extended-A */
  {0x0900, 0x097F, ENTRY_NO  }, /* Devanagari */
  {0x0980, 0x09FF, ENTRY_NO  }, /* Bengali */
  {0x0A00, 0x0A7F, ENTRY_NO  }, /* Gurmukhi */
  {0x0A80, 0x0AFF, ENTRY_NO  }, /* Gujarati */
  {0x0B00, 0x0B7F, ENTRY_NO  }, /* Oriya */
  {0x0B80, 0x0BFF, ENTRY_NO  }, /* Tamil */
  {0x0C00, 0x0C7F, ENTRY_NO  }, /* Telugu */
  {0x0C80, 0x0CFF, ENTRY_NO  }, /* Kannada */
  {0x0D00, 0x0D7F, ENTRY_NO  }, /* Malayalam */
  {0x0D80, 0x0DFF, ENTRY_NO  }, /* Sinhala */
  {0x0E00, 0x0E7F, ENTRY_NO  }, /* Thai */
  {0x0E80, 0x0EFF, ENTRY_NO  }, /* Lao */
  {0x0F00, 0x0FFF, ENTRY_NO  }, /* Tibetan */
  {0x1000, 0x109F, ENTRY_NO  }, /* Myanmar */
  {0x10A0, 0x10FF, ENTRY_NO  }, /* Georgian */
  {0x1100, 0x11FF, ENTRY_K   }, /* Hangul Jamo */
  {0x1200, 0x137F, ENTRY_NO  }, /* Ethiopic */
  {0x1380, 0x139F, ENTRY_NO  }, /* Ethiopic Supplement */
  {0x13A0, 0x13FF, ENTRY_NO  }, /* Cherokee */
  {0x1400, 0x167F, ENTRY_NO  }, /* Unified Canadian Aboriginal Syllabics */
  {0x1680, 0x169F, ENTRY_NO  }, /* Ogham */
  {0x16A0, 0x16FF, ENTRY_NO  }, /* Runic */
  {0x1700, 0x171F, ENTRY_NO  }, /* Tagalog */
  {0x1720, 0x173F, ENTRY_NO  }, /* Hanunoo */
  {0x1740, 0x175F, ENTRY_NO  }, /* Buhid */
  {0x1760, 0x177F, ENTRY_NO  }, /* Tagbanwa */
  {0x1780, 0x17FF, ENTRY_NO  }, /* Khmer */
  {0x1800, 0x18AF, ENTRY_NO  }, /* Mongolian */
  {0x18B0, 0x18FF, ENTRY_NO  }, /* Unified Canadian Aboriginal Syllabics Extended */
  {0x1900, 0x194F, ENTRY_NO  }, /* Limbu */
  {0x1950, 0x197F, ENTRY_NO  }, /* Tai Le */
  {0x1980, 0x19DF, ENTRY_NO  }, /* New Tai Lue */
  {0x19E0, 0x19FF, ENTRY_NO  }, /* Khmer Symbols */
  {0x1A00, 0x1A1F, ENTRY_NO  }, /* Buginese */
  {0x1A20, 0x1AAF, ENTRY_NO  }, /* Tai Tham */
  {0x1B00, 0x1B7F, ENTRY_NO  }, /* Balinese */
  {0x1B80, 0x1BBF, ENTRY_NO  }, /* Sundanese */
  {0x1BC0, 0x1BFF, ENTRY_NO  }, /* Batak */
  {0x1C00, 0x1C4F, ENTRY_NO  }, /* Lepcha */
  {0x1C50, 0x1C7F, ENTRY_NO  }, /* Ol Chiki */
  {0x1CC0, 0x1CCF, ENTRY_NO  }, /* Sundanese Supplement */
  {0x1CD0, 0x1CFF, ENTRY_NO  }, /* Vedic Extensions */
  {0x1D00, 0x1D7F, ENTRY_NO  }, /* Phonetic Extensions */
  {0x1D80, 0x1DBF, ENTRY_NO  }, /* Phonetic Extensions Supplement */
  {0x1DC0, 0x1DFF, ENTRY_NO  }, /* Combining Diacritical Marks Supplement */
  {0x1E00, 0x1EFF, ENTRY_GCJ }, /* Latin Extended Additional */
  {0x1F00, 0x1FFF, ENTRY_J   }, /* Greek Extended */
  {0x2000, 0x206F, ENTRY_GCJK}, /* General Punctuation */
  {0x2070, 0x209F, ENTRY_JK  }, /* Superscripts and Subscripts */
  {0x20A0, 0x20CF, ENTRY_GCJK}, /* Currency Symbols */
  {0x20D0, 0x20FF, ENTRY_J   }, /* Combining Diacritical Marks for Symbols */
  {0x2100, 0x214F, ENTRY_GCJK}, /* Letterlike Symbols */
  {0x2150, 0x218F, ENTRY_GCJK}, /* Number Forms */
  {0x2190, 0x21FF, ENTRY_GCJK}, /* Arrows */
  {0x2200, 0x22FF, ENTRY_GCJK}, /* Mathematical Operators */
  {0x2300, 0x23FF, ENTRY_GCJK}, /* Miscellaneous Technical */
  {0x2400, 0x243F, ENTRY_CJ  }, /* Control Pictures */
  {0x2440, 0x245F, ENTRY_NO  }, /* Optical Character Recognition */
  {0x2460, 0x24FF, ENTRY_GCJK}, /* Enclosed Alphanumerics */
  {0x2500, 0x257F, ENTRY_GCJK}, /* Box Drawing */
  {0x2580, 0x259F, ENTRY_GCJK}, /* Block Elements */
  {0x25A0, 0x25FF, ENTRY_GCJK}, /* Geometric Shapes */
  {0x2600, 0x26FF, ENTRY_GCJK}, /* Miscellaneous Symbols */
  {0x2700, 0x27BF, ENTRY_CJK }, /* Dingbats */
  {0x27C0, 0x27EF, ENTRY_NO  }, /* Miscellaneous Mathematical Symbols-A */
  {0x27F0, 0x27FF, ENTRY_NO  }, /* Supplemental Arrows-A */
  {0x2800, 0x28FF, ENTRY_NO  }, /* Braille Patterns */
  {0x2900, 0x297F, ENTRY_J   }, /* Supplemental Arrows-B */
  {0x2980, 0x29FF, ENTRY_J   }, /* Miscellaneous Mathematical Symbols-B */
  {0x2A00, 0x2AFF, ENTRY_NO  }, /* Supplemental Mathematical Operators */
  {0x2B00, 0x2BFF, ENTRY_J   }, /* Miscellaneous Symbols and Arrows */
  {0x2C00, 0x2C5F, ENTRY_NO  }, /* Glagolitic */
  {0x2C60, 0x2C7F, ENTRY_NO  }, /* Latin Extended-C */
  {0x2C80, 0x2CFF, ENTRY_NO  }, /* Coptic */
  {0x2D00, 0x2D2F, ENTRY_NO  }, /* Georgian Supplement */
  {0x2D30, 0x2D7F, ENTRY_NO  }, /* Tifinagh */
  {0x2D80, 0x2DDF, ENTRY_NO  }, /* Ethiopic Extended */
  {0x2DE0, 0x2DFF, ENTRY_NO  }, /* Cyrillic Extended-A */
  {0x2E00, 0x2E7F, ENTRY_NO  }, /* Supplemental Punctuation */
  {0x2E80, 0x2EFF, ENTRY_GCJ }, /* CJK Radicals Supplement */
  {0x2F00, 0x2FDF, ENTRY_GCJK}, /* Kangxi Radicals */
  {0x2FF0, 0x2FFF, ENTRY_G   }, /* Ideographic Description Characters */
  {0x3000, 0x303F, ENTRY_GCJK}, /* CJK Symbols and Punctuation */
  {0x3040, 0x309F, ENTRY_GCJK}, /* Hiragana */
  {0x30A0, 0x30FF, ENTRY_GCJK}, /* Katakana */
  {0x3100, 0x312F, ENTRY_GC  }, /* Bopomofo */
  {0x3130, 0x318F, ENTRY_K   }, /* Hangul Compatibility Jamo */
  {0x3190, 0x319F, ENTRY_J   }, /* Kanbun */
  {0x31A0, 0x31BF, ENTRY_G   }, /* Bopomofo Extended */
  {0x31C0, 0x31EF, ENTRY_C   }, /* CJK Strokes */
  {0x31F0, 0x31FF, ENTRY_J   }, /* Katakana Phonetic Extensions */
  {0x3200, 0x32FF, ENTRY_GCJK}, /* Enclosed CJK Letters and Months */
  {0x3300, 0x33FF, ENTRY_GCJK}, /* CJK Compatibility */
  {0x3400, 0x4DBF, ENTRY_GCJ }, /* CJK Unified Ideographs Extension A */
  {0x4DC0, 0x4DFF, ENTRY_NO  }, /* Yijing Hexagram Symbols */
  {0x4E00, 0x9FFF, ENTRY_GCJK}, /* CJK Unified Ideographs */
  {0xA000, 0xA48F, ENTRY_G   }, /* Yi Syllables */
  {0xA490, 0xA4CF, ENTRY_G   }, /* Yi Radicals */
  {0xA4D0, 0xA4FF, ENTRY_NO  }, /* Lisu */
  {0xA500, 0xA63F, ENTRY_NO  }, /* Vai */
  {0xA640, 0xA69F, ENTRY_NO  }, /* Cyrillic Extended-B */
  {0xA6A0, 0xA6FF, ENTRY_NO  }, /* Bamum */
  {0xA700, 0xA71F, ENTRY_NO  }, /* Modifier Tone Letters */
  {0xA720, 0xA7FF, ENTRY_NO  }, /* Latin Extended-D */
  {0xA800, 0xA82F, ENTRY_NO  }, /* Syloti Nagri */
  {0xA830, 0xA83F, ENTRY_NO  }, /* Common Indic Number Forms */
  {0xA840, 0xA87F, ENTRY_NO  }, /* Phags-pa */
  {0xA880, 0xA8DF, ENTRY_NO  }, /* Saurashtra */
  {0xA8E0, 0xA8FF, ENTRY_NO  }, /* Devanagari Extended */
  {0xA900, 0xA92F, ENTRY_NO  }, /* Kayah Li */
  {0xA930, 0xA95F, ENTRY_NO  }, /* Rejang */
  {0xA960, 0xA97F, ENTRY_NO  }, /* Hangul Jamo Extended-A */
  {0xA980, 0xA9DF, ENTRY_NO  }, /* Javanese */
  {0xAA00, 0xAA5F, ENTRY_NO  }, /* Cham */
  {0xAA60, 0xAA7F, ENTRY_NO  }, /* Myanmar Extended-A */
  {0xAA80, 0xAADF, ENTRY_NO  }, /* Tai Viet */
  {0xAAE0, 0xAAFF, ENTRY_NO  }, /* Meetei Mayek Extensions */
  {0xAB00, 0xAB2F, ENTRY_NO  }, /* Ethiopic Extended-A */
  {0xABC0, 0xABFF, ENTRY_NO  }, /* Meetei Mayek */
  {0xAC00, 0xD7AF, ENTRY_K   }, /* Hangul Syllables */
  {0xD7B0, 0xD7FF, ENTRY_NO  }, /* Hangul Jamo Extended-B */
  {0xD800, 0xDB7F, ENTRY_NO  }, /* High Surrogates */
  {0xDB80, 0xDBFF, ENTRY_NO  }, /* High Private Use Surrogates */
  {0xDC00, 0xDFFF, ENTRY_NO  }, /* Low Surrogates */
  {0xE000, 0xF8FF, ENTRY_GCJK}, /* Private Use Area */
  {0xF900, 0xFAFF, ENTRY_GCJK}, /* CJK Compatibility Ideographs */
  {0xFB00, 0xFB4F, ENTRY_J   }, /* Alphabetic Presentation Forms */
  {0xFB50, 0xFDFF, ENTRY_NO  }, /* Arabic Presentation Forms-A */
  {0xFE00, 0xFE0F, ENTRY_NO  }, /* Variation Selectors */
  {0xFE10, 0xFE1F, ENTRY_GCJ }, /* Vertical Forms */
  {0xFE20, 0xFE2F, ENTRY_NO  }, /* Combining Half Marks */
  {0xFE30, 0xFE4F, ENTRY_GCJ }, /* CJK Compatibility Forms */
  {0xFE50, 0xFE6F, ENTRY_GC  }, /* Small Form Variants */
  {0xFE70, 0xFEFF, ENTRY_NO  }, /* Arabic Presentation Forms-B */
  {0xFF00, 0xFFEF, ENTRY_GCJK}, /* Halfwidth and Fullwidth Forms */
  {0xFFF0, 0xFFFF, ENTRY_NO  }, /* Specials */
  {0x10000, 0x1007F, ENTRY_NO  }, /* Linear B Syllabary */
  {0x10080, 0x100FF, ENTRY_NO  }, /* Linear B Ideograms */
  {0x10100, 0x1013F, ENTRY_NO  }, /* Aegean Numbers */
  {0x10140, 0x1018F, ENTRY_NO  }, /* Ancient Greek Numbers */
  {0x10190, 0x101CF, ENTRY_NO  }, /* Ancient Symbols */
  {0x101D0, 0x101FF, ENTRY_NO  }, /* Phaistos Disc */
  {0x10280, 0x1029F, ENTRY_NO  }, /* Lycian */
  {0x102A0, 0x102DF, ENTRY_NO  }, /* Carian */
  {0x10300, 0x1032F, ENTRY_NO  }, /* Old Italic */
  {0x10330, 0x1034F, ENTRY_NO  }, /* Gothic */
  {0x10380, 0x1039F, ENTRY_NO  }, /* Ugaritic */
  {0x103A0, 0x103DF, ENTRY_NO  }, /* Old Persian */
  {0x10400, 0x1044F, ENTRY_NO  }, /* Deseret */
  {0x10450, 0x1047F, ENTRY_NO  }, /* Shavian */
  {0x10480, 0x104AF, ENTRY_NO  }, /* Osmanya */
  {0x10800, 0x1083F, ENTRY_NO  }, /* Cypriot Syllabary */
  {0x10840, 0x1085F, ENTRY_NO  }, /* Imperial Aramaic */
  {0x10900, 0x1091F, ENTRY_NO  }, /* Phoenician */
  {0x10920, 0x1093F, ENTRY_NO  }, /* Lydian */
  {0x10980, 0x1099F, ENTRY_NO  }, /* Meroitic Hieroglyphs */
  {0x109A0, 0x109FF, ENTRY_NO  }, /* Meroitic Cursive */
  {0x10A00, 0x10A5F, ENTRY_NO  }, /* Kharoshthi */
  {0x10A60, 0x10A7F, ENTRY_NO  }, /* Old South Arabian */
  {0x10B00, 0x10B3F, ENTRY_NO  }, /* Avestan */
  {0x10B40, 0x10B5F, ENTRY_NO  }, /* Inscriptional Parthian */
  {0x10B60, 0x10B7F, ENTRY_NO  }, /* Inscriptional Pahlavi */
  {0x10C00, 0x10C4F, ENTRY_NO  }, /* Old Turkic */
  {0x10E60, 0x10E7F, ENTRY_NO  }, /* Rumi Numeral Symbols */
  {0x11000, 0x1107F, ENTRY_NO  }, /* Brahmi */
  {0x11080, 0x110CF, ENTRY_NO  }, /* Kaithi */
  {0x110D0, 0x110FF, ENTRY_NO  }, /* Sora Sompeng */
  {0x11100, 0x1114F, ENTRY_NO  }, /* Chakma */
  {0x11180, 0x111DF, ENTRY_NO  }, /* Sharada */
  {0x11680, 0x116CF, ENTRY_NO  }, /* Takri */
  {0x12000, 0x123FF, ENTRY_NO  }, /* Cuneiform */
  {0x12400, 0x1247F, ENTRY_NO  }, /* Cuneiform Numbers and Punctuation */
  {0x13000, 0x1342F, ENTRY_NO  }, /* Egyptian Hieroglyphs */
  {0x16800, 0x16A3F, ENTRY_NO  }, /* Bamum Supplement */
  {0x16F00, 0x16F9F, ENTRY_NO  }, /* Miao */
  {0x1B000, 0x1B0FF, ENTRY_NO  }, /* Kana Supplement */
  {0x1D000, 0x1D0FF, ENTRY_NO  }, /* Byzantine Musical Symbols */
  {0x1D100, 0x1D1FF, ENTRY_NO  }, /* Musical Symbols */
  {0x1D200, 0x1D24F, ENTRY_NO  }, /* Ancient Greek Musical Notation */
  {0x1D300, 0x1D35F, ENTRY_NO  }, /* Tai Xuan Jing Symbols */
  {0x1D360, 0x1D37F, ENTRY_NO  }, /* Counting Rod Numerals */
  {0x1D400, 0x1D7FF, ENTRY_NO  }, /* Mathematical Alphanumeric Symbols */
  {0x1EE00, 0x1EEFF, ENTRY_NO  }, /* Arabic Mathematical Alphabetic Symbols */
  {0x1F000, 0x1F02F, ENTRY_NO  }, /* Mahjong Tiles */
  {0x1F030, 0x1F09F, ENTRY_NO  }, /* Domino Tiles */
  {0x1F0A0, 0x1F0FF, ENTRY_NO  }, /* Playing Cards */
  {0x1F100, 0x1F1FF, ENTRY_J   }, /* Enclosed Alphanumeric Supplement */
  {0x1F200, 0x1F2FF, ENTRY_J   }, /* Enclosed Ideographic Supplement */
  {0x1F300, 0x1F5FF, ENTRY_NO  }, /* Miscellaneous Symbols And Pictographs */
  {0x1F600, 0x1F64F, ENTRY_NO  }, /* Emoticons */
  {0x1F680, 0x1F6FF, ENTRY_NO  }, /* Transport And Map Symbols */
  {0x1F700, 0x1F77F, ENTRY_NO  }, /* Alchemical Symbols */
  {0x20000, 0x2A6DF, ENTRY_GCJ }, /* CJK Unified Ideographs Extension B */
  {0x2A700, 0x2B73F, ENTRY_CJ  }, /* CJK Unified Ideographs Extension C */
  {0x2B740, 0x2B81F, ENTRY_J   }, /* CJK Unified Ideographs Extension D */
  {0x2F800, 0x2FA1F, ENTRY_CJ  }, /* CJK Compatibility Ideographs Supplement */
  {0xE0000, 0xE007F, ENTRY_NO  }, /* Tags */
  {0xE0100, 0xE01EF, ENTRY_NO  }, /* Variation Selectors Supplement */
  {0xF0000, 0xFFFFF, ENTRY_NO  }, /* Supplementary Private Use Area-A */
  {0x100000, 0x10FFFF, ENTRY_NO  }, /* Supplementary Private Use Area-B */
  {0x200000, 0xFFFFFF, ENTRY_NO  }, /* illegal */
};


int search_cjk_entry(int *ib, long ch, long cjk) {
  if (cjk==ENTRY_NO) return 1;
  if (cjk==ENTRY_JQ) return
	(ch==U_OPEN_SQUOTE || ch==U_CLOSE_SQUOTE
	 || ch==U_OPEN_DQUOTE || ch==U_CLOSE_DQUOTE);
  while(ublock_data[*ib].max<ch) (*ib)++;
  if (ublock_data[*ib].min<=ch && ch<=ublock_data[*ib].max) {
    return ublock_data[*ib].cjk & cjk;
  } else {
    return 0;
  }
}


#ifdef DEBUG
int main() {
  long ch;
  int ib;

  ib=0;
  for (ch=0x0;ch<0x10000;ch++) {
    printf(" %05x %2d %2d %2d %2d %2d\n", ch, ib,
	   search_cjk_entry(&ib,ch,ENTRY_G),
	   search_cjk_entry(&ib,ch,ENTRY_C),
	   search_cjk_entry(&ib,ch,ENTRY_J),
	   search_cjk_entry(&ib,ch,ENTRY_K));
  }

}
#endif
