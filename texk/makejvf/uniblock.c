#include "uniblock.h"
#include "usrtable.h"

#ifdef DEBUG
#include <stdio.h>
int usertable_charset_max=0;
struct USERTABLE_CHARSET usertable_charset[MAX_CHAR_TABLE];
#endif

int uniblock_iskanji;
struct ublock {
  long min, max, cjk;
  int kanji;
};

/*
 References:
 [1] http://www.unicode.org/Public/UNIDATA/Blocks.txt
        Blocks-14.0.0.txt
        Date: 2021-01-22, 23:29:00 GMT [KW]
 [2] CMap files
     https://github.com/adobe-type-tools/cmap-resources/
     Adobe-CNS1-7/                  Adobe-CNS1-7 materials
        cid2code.txt (Version 10/24/2017)
     Adobe-GB1-5/                   Adobe-GB1-5 materials
        cid2code.txt (Version 12/05/2017)
     Adobe-Japan1-7/                Adobe-Japan1-7 materials
        cid2code.txt (Version 07/30/2019)
     Adobe-Korea1-2/                Adobe-Korea1-2 materials
        cid2code.txt (Version 01/31/2012)
 Following code points are omitted from ENTRY_J:
   block                      code point  name                      Adobe-Japan CID
   Tibetan                    U+0FD6      Left-facing Svasti Sign   12182
   Supplemental Puncutuation  U+2E40      Double Hyphen             15516
   Latin Extended-D           U+A7B5      Latin Small Letter Beta   15909
   Latin Extended-E           U+AB53      Latin Small Letter Chi    15911
   Geometric Shapes Extended  U+1F79C     Diamond Target            12244
*/

static struct ublock ublock_data[] = {
  {0x0000, 0x007F, ENTRY_GCJK, 0}, /* Basic Latin */
  {0x0080, 0x00FF, ENTRY_GCJK, 0}, /* Latin-1 Supplement */
  {0x0100, 0x017F, ENTRY_GCJK, 0}, /* Latin Extended-A */
  {0x0180, 0x024F, ENTRY_GCJ , 0}, /* Latin Extended-B */
  {0x0250, 0x02AF, ENTRY_GCJ , 0}, /* IPA Extensions */
  {0x02B0, 0x02FF, ENTRY_GCJK, 0}, /* Spacing Modifier Letters */
  {0x0300, 0x036F, ENTRY_CJ  , 0}, /* Combining Diacritical Marks */
  {0x0370, 0x03FF, ENTRY_GCJK, 0}, /* Greek and Coptic */
  {0x0400, 0x04FF, ENTRY_GCJK, 0}, /* Cyrillic */
  {0x0500, 0x052F, ENTRY_NO  , 0}, /* Cyrillic Supplement */
  {0x0530, 0x058F, ENTRY_NO  , 0}, /* Armenian */
  {0x0590, 0x05FF, ENTRY_NO  , 0}, /* Hebrew */
  {0x0600, 0x06FF, ENTRY_NO  , 0}, /* Arabic */
  {0x0700, 0x074F, ENTRY_NO  , 0}, /* Syriac */
  {0x0750, 0x077F, ENTRY_NO  , 0}, /* Arabic Supplement */
  {0x0780, 0x07BF, ENTRY_NO  , 0}, /* Thaana */
  {0x07C0, 0x07FF, ENTRY_NO  , 0}, /* NKo */
  {0x0800, 0x083F, ENTRY_NO  , 0}, /* Samaritan */
  {0x0840, 0x085F, ENTRY_NO  , 0}, /* Mandaic */
  {0x0860, 0x086F, ENTRY_NO  , 0}, /* Syriac Supplement */
  {0x0870, 0x089F, ENTRY_NO  , 0}, /* Arabic Extended-B */
  {0x08A0, 0x08FF, ENTRY_NO  , 0}, /* Arabic Extended-A */
  {0x0900, 0x097F, ENTRY_NO  , 0}, /* Devanagari */
  {0x0980, 0x09FF, ENTRY_NO  , 0}, /* Bengali */
  {0x0A00, 0x0A7F, ENTRY_NO  , 0}, /* Gurmukhi */
  {0x0A80, 0x0AFF, ENTRY_NO  , 0}, /* Gujarati */
  {0x0B00, 0x0B7F, ENTRY_NO  , 0}, /* Oriya */
  {0x0B80, 0x0BFF, ENTRY_NO  , 0}, /* Tamil */
  {0x0C00, 0x0C7F, ENTRY_NO  , 0}, /* Telugu */
  {0x0C80, 0x0CFF, ENTRY_NO  , 0}, /* Kannada */
  {0x0D00, 0x0D7F, ENTRY_NO  , 0}, /* Malayalam */
  {0x0D80, 0x0DFF, ENTRY_NO  , 0}, /* Sinhala */
  {0x0E00, 0x0E7F, ENTRY_NO  , 0}, /* Thai */
  {0x0E80, 0x0EFF, ENTRY_NO  , 0}, /* Lao */
  {0x0F00, 0x0FFF, ENTRY_NO  , 0}, /* Tibetan */
  {0x1000, 0x109F, ENTRY_NO  , 0}, /* Myanmar */
  {0x10A0, 0x10FF, ENTRY_NO  , 0}, /* Georgian */
  {0x1100, 0x11FF, ENTRY_K   , 0}, /* Hangul Jamo */
  {0x1200, 0x137F, ENTRY_NO  , 0}, /* Ethiopic */
  {0x1380, 0x139F, ENTRY_NO  , 0}, /* Ethiopic Supplement */
  {0x13A0, 0x13FF, ENTRY_NO  , 0}, /* Cherokee */
  {0x1400, 0x167F, ENTRY_NO  , 0}, /* Unified Canadian Aboriginal Syllabics */
  {0x1680, 0x169F, ENTRY_NO  , 0}, /* Ogham */
  {0x16A0, 0x16FF, ENTRY_NO  , 0}, /* Runic */
  {0x1700, 0x171F, ENTRY_NO  , 0}, /* Tagalog */
  {0x1720, 0x173F, ENTRY_NO  , 0}, /* Hanunoo */
  {0x1740, 0x175F, ENTRY_NO  , 0}, /* Buhid */
  {0x1760, 0x177F, ENTRY_NO  , 0}, /* Tagbanwa */
  {0x1780, 0x17FF, ENTRY_NO  , 0}, /* Khmer */
  {0x1800, 0x18AF, ENTRY_NO  , 0}, /* Mongolian */
  {0x18B0, 0x18FF, ENTRY_NO  , 0}, /* Unified Canadian Aboriginal Syllabics Extended */
  {0x1900, 0x194F, ENTRY_NO  , 0}, /* Limbu */
  {0x1950, 0x197F, ENTRY_NO  , 0}, /* Tai Le */
  {0x1980, 0x19DF, ENTRY_NO  , 0}, /* New Tai Lue */
  {0x19E0, 0x19FF, ENTRY_NO  , 0}, /* Khmer Symbols */
  {0x1A00, 0x1A1F, ENTRY_NO  , 0}, /* Buginese */
  {0x1A20, 0x1AAF, ENTRY_NO  , 0}, /* Tai Tham */
  {0x1AB0, 0x1AFF, ENTRY_NO  , 0}, /* Combining Diacritical Marks Extended */
  {0x1B00, 0x1B7F, ENTRY_NO  , 0}, /* Balinese */
  {0x1B80, 0x1BBF, ENTRY_NO  , 0}, /* Sundanese */
  {0x1BC0, 0x1BFF, ENTRY_NO  , 0}, /* Batak */
  {0x1C00, 0x1C4F, ENTRY_NO  , 0}, /* Lepcha */
  {0x1C50, 0x1C7F, ENTRY_NO  , 0}, /* Ol Chiki */
  {0x1C80, 0x1C8F, ENTRY_NO  , 0}, /* Cyrillic Extended-C */
  {0x1C90, 0x1CBF, ENTRY_NO  , 0}, /* Georgian Extended */
  {0x1CC0, 0x1CCF, ENTRY_NO  , 0}, /* Sundanese Supplement */
  {0x1CD0, 0x1CFF, ENTRY_NO  , 0}, /* Vedic Extensions */
  {0x1D00, 0x1D7F, ENTRY_NO  , 0}, /* Phonetic Extensions */
  {0x1D80, 0x1DBF, ENTRY_NO  , 0}, /* Phonetic Extensions Supplement */
  {0x1DC0, 0x1DFF, ENTRY_NO  , 0}, /* Combining Diacritical Marks Supplement */
  {0x1E00, 0x1EFF, ENTRY_GCJ , 0}, /* Latin Extended Additional */
  {0x1F00, 0x1FFF, ENTRY_J   , 0}, /* Greek Extended */
  {0x2000, 0x206F, ENTRY_GCJK, 0}, /* General Punctuation */
  {0x2070, 0x209F, ENTRY_JK  , 0}, /* Superscripts and Subscripts */
  {0x20A0, 0x20CF, ENTRY_GCJK, 0}, /* Currency Symbols */
  {0x20D0, 0x20FF, ENTRY_J   , 0}, /* Combining Diacritical Marks for Symbols */
  {0x2100, 0x214F, ENTRY_GCJK, 0}, /* Letterlike Symbols */
  {0x2150, 0x218F, ENTRY_GCJK, 0}, /* Number Forms */
  {0x2190, 0x21FF, ENTRY_GCJK, 0}, /* Arrows */
  {0x2200, 0x22FF, ENTRY_GCJK, 0}, /* Mathematical Operators */
  {0x2300, 0x23FF, ENTRY_GCJK, 0}, /* Miscellaneous Technical */
  {0x2400, 0x243F, ENTRY_CJ  , 0}, /* Control Pictures */
  {0x2440, 0x245F, ENTRY_NO  , 0}, /* Optical Character Recognition */
  {0x2460, 0x24FF, ENTRY_GCJK, 0}, /* Enclosed Alphanumerics */
  {0x2500, 0x257F, ENTRY_GCJK, 0}, /* Box Drawing */
  {0x2580, 0x259F, ENTRY_GCJK, 0}, /* Block Elements */
  {0x25A0, 0x25FF, ENTRY_GCJK, 0}, /* Geometric Shapes */
  {0x2600, 0x26FF, ENTRY_GCJK, 0}, /* Miscellaneous Symbols */
  {0x2700, 0x27BF, ENTRY_CJK , 0}, /* Dingbats */
  {0x27C0, 0x27EF, ENTRY_NO  , 0}, /* Miscellaneous Mathematical Symbols-A */
  {0x27F0, 0x27FF, ENTRY_NO  , 0}, /* Supplemental Arrows-A */
  {0x2800, 0x28FF, ENTRY_NO  , 0}, /* Braille Patterns */
  {0x2900, 0x297F, ENTRY_J   , 0}, /* Supplemental Arrows-B */
  {0x2980, 0x29FF, ENTRY_J   , 0}, /* Miscellaneous Mathematical Symbols-B */
  {0x2A00, 0x2AFF, ENTRY_NO  , 0}, /* Supplemental Mathematical Operators */
  {0x2B00, 0x2BFF, ENTRY_J   , 0}, /* Miscellaneous Symbols and Arrows */
  {0x2C00, 0x2C5F, ENTRY_NO  , 0}, /* Glagolitic */
  {0x2C60, 0x2C7F, ENTRY_NO  , 0}, /* Latin Extended-C */
  {0x2C80, 0x2CFF, ENTRY_NO  , 0}, /* Coptic */
  {0x2D00, 0x2D2F, ENTRY_NO  , 0}, /* Georgian Supplement */
  {0x2D30, 0x2D7F, ENTRY_NO  , 0}, /* Tifinagh */
  {0x2D80, 0x2DDF, ENTRY_NO  , 0}, /* Ethiopic Extended */
  {0x2DE0, 0x2DFF, ENTRY_NO  , 0}, /* Cyrillic Extended-A */
  {0x2E00, 0x2E7F, ENTRY_NO  , 0}, /* Supplemental Punctuation */
  {0x2E80, 0x2EFF, ENTRY_GCJ , 0}, /* CJK Radicals Supplement */
  {0x2F00, 0x2FDF, ENTRY_GCJK, 0}, /* Kangxi Radicals */
  {0x2FF0, 0x2FFF, ENTRY_G   , 0}, /* Ideographic Description Characters */
  {0x3000, 0x303F, ENTRY_GCJK, 0}, /* CJK Symbols and Punctuation */
  {0x3040, 0x309F, ENTRY_GCJK, 0}, /* Hiragana */
  {0x30A0, 0x30FF, ENTRY_GCJK, 0}, /* Katakana */
  {0x3100, 0x312F, ENTRY_GC  , 0}, /* Bopomofo */
  {0x3130, 0x318F, ENTRY_K   , 0}, /* Hangul Compatibility Jamo */
  {0x3190, 0x319F, ENTRY_J   , 0}, /* Kanbun */
  {0x31A0, 0x31BF, ENTRY_G   , 0}, /* Bopomofo Extended */
  {0x31C0, 0x31EF, ENTRY_C   , 0}, /* CJK Strokes */
  {0x31F0, 0x31FF, ENTRY_J   , 0}, /* Katakana Phonetic Extensions */
  {0x3200, 0x32FF, ENTRY_GCJK, 0}, /* Enclosed CJK Letters and Months */
  {0x3300, 0x33FF, ENTRY_GCJK, 0}, /* CJK Compatibility */
  {0x3400, 0x4DBF, ENTRY_GCJ , 1}, /* CJK Unified Ideographs Extension A */
  {0x4DC0, 0x4DFF, ENTRY_NO  , 0}, /* Yijing Hexagram Symbols */
  {0x4E00, 0x9FFF, ENTRY_GCJK, 1}, /* CJK Unified Ideographs */
  {0xA000, 0xA48F, ENTRY_G   , 0}, /* Yi Syllables */
  {0xA490, 0xA4CF, ENTRY_G   , 0}, /* Yi Radicals */
  {0xA4D0, 0xA4FF, ENTRY_NO  , 0}, /* Lisu */
  {0xA500, 0xA63F, ENTRY_NO  , 0}, /* Vai */
  {0xA640, 0xA69F, ENTRY_NO  , 0}, /* Cyrillic Extended-B */
  {0xA6A0, 0xA6FF, ENTRY_NO  , 0}, /* Bamum */
  {0xA700, 0xA71F, ENTRY_NO  , 0}, /* Modifier Tone Letters */
  {0xA720, 0xA7FF, ENTRY_NO  , 0}, /* Latin Extended-D */
  {0xA800, 0xA82F, ENTRY_NO  , 0}, /* Syloti Nagri */
  {0xA830, 0xA83F, ENTRY_NO  , 0}, /* Common Indic Number Forms */
  {0xA840, 0xA87F, ENTRY_NO  , 0}, /* Phags-pa */
  {0xA880, 0xA8DF, ENTRY_NO  , 0}, /* Saurashtra */
  {0xA8E0, 0xA8FF, ENTRY_NO  , 0}, /* Devanagari Extended */
  {0xA900, 0xA92F, ENTRY_NO  , 0}, /* Kayah Li */
  {0xA930, 0xA95F, ENTRY_NO  , 0}, /* Rejang */
  {0xA960, 0xA97F, ENTRY_NO  , 0}, /* Hangul Jamo Extended-A */
  {0xA980, 0xA9DF, ENTRY_NO  , 0}, /* Javanese */
  {0xA9E0, 0xA9FF, ENTRY_NO  , 0}, /* Myanmar Extended-B */
  {0xAA00, 0xAA5F, ENTRY_NO  , 0}, /* Cham */
  {0xAA60, 0xAA7F, ENTRY_NO  , 0}, /* Myanmar Extended-A */
  {0xAA80, 0xAADF, ENTRY_NO  , 0}, /* Tai Viet */
  {0xAAE0, 0xAAFF, ENTRY_NO  , 0}, /* Meetei Mayek Extensions */
  {0xAB00, 0xAB2F, ENTRY_NO  , 0}, /* Ethiopic Extended-A */
  {0xAB30, 0xAB6F, ENTRY_NO  , 0}, /* Latin Extended-E */
  {0xAB70, 0xABBF, ENTRY_NO  , 0}, /* Cherokee Supplement */
  {0xABC0, 0xABFF, ENTRY_NO  , 0}, /* Meetei Mayek */
  {0xAC00, 0xD7AF, ENTRY_K   , 0}, /* Hangul Syllables */
  {0xD7B0, 0xD7FF, ENTRY_NO  , 0}, /* Hangul Jamo Extended-B */
  {0xD800, 0xDB7F, ENTRY_NO  , 0}, /* High Surrogates */
  {0xDB80, 0xDBFF, ENTRY_NO  , 0}, /* High Private Use Surrogates */
  {0xDC00, 0xDFFF, ENTRY_NO  , 0}, /* Low Surrogates */
  {0xE000, 0xF8FF, ENTRY_GCJK, 0}, /* Private Use Area */
  {0xF900, 0xFAFF, ENTRY_GCJK, 1}, /* CJK Compatibility Ideographs */
  {0xFB00, 0xFB4F, ENTRY_J   , 0}, /* Alphabetic Presentation Forms */
  {0xFB50, 0xFDFF, ENTRY_NO  , 0}, /* Arabic Presentation Forms-A */
  {0xFE00, 0xFE0F, ENTRY_NO  , 0}, /* Variation Selectors */
  {0xFE10, 0xFE1F, ENTRY_GCJ , 0}, /* Vertical Forms */
  {0xFE20, 0xFE2F, ENTRY_NO  , 0}, /* Combining Half Marks */
  {0xFE30, 0xFE4F, ENTRY_GCJ , 0}, /* CJK Compatibility Forms */
  {0xFE50, 0xFE6F, ENTRY_GC  , 0}, /* Small Form Variants */
  {0xFE70, 0xFEFF, ENTRY_NO  , 0}, /* Arabic Presentation Forms-B */
  {0xFF00, 0xFFEF, ENTRY_GCJK, 0}, /* Halfwidth and Fullwidth Forms */
  {0xFFF0, 0xFFFF, ENTRY_NO  , 0}, /* Specials */
  {0x10000, 0x1007F, ENTRY_NO  , 0}, /* Linear B Syllabary */
  {0x10080, 0x100FF, ENTRY_NO  , 0}, /* Linear B Ideograms */
  {0x10100, 0x1013F, ENTRY_NO  , 0}, /* Aegean Numbers */
  {0x10140, 0x1018F, ENTRY_NO  , 0}, /* Ancient Greek Numbers */
  {0x10190, 0x101CF, ENTRY_NO  , 0}, /* Ancient Symbols */
  {0x101D0, 0x101FF, ENTRY_NO  , 0}, /* Phaistos Disc */
  {0x10280, 0x1029F, ENTRY_NO  , 0}, /* Lycian */
  {0x102A0, 0x102DF, ENTRY_NO  , 0}, /* Carian */
  {0x102E0, 0x102FF, ENTRY_NO  , 0}, /* Coptic Epact Numbers */
  {0x10300, 0x1032F, ENTRY_NO  , 0}, /* Old Italic */
  {0x10330, 0x1034F, ENTRY_NO  , 0}, /* Gothic */
  {0x10350, 0x1037F, ENTRY_NO  , 0}, /* Old Permic */
  {0x10380, 0x1039F, ENTRY_NO  , 0}, /* Ugaritic */
  {0x103A0, 0x103DF, ENTRY_NO  , 0}, /* Old Persian */
  {0x10400, 0x1044F, ENTRY_NO  , 0}, /* Deseret */
  {0x10450, 0x1047F, ENTRY_NO  , 0}, /* Shavian */
  {0x10480, 0x104AF, ENTRY_NO  , 0}, /* Osmanya */
  {0x104B0, 0x104FF, ENTRY_NO  , 0}, /* Osage */
  {0x10500, 0x1052F, ENTRY_NO  , 0}, /* Elbasan */
  {0x10530, 0x1056F, ENTRY_NO  , 0}, /* Caucasian Albanian */
  {0x10570, 0x105BF, ENTRY_NO  , 0}, /* Vithkuqi */
  {0x10600, 0x1077F, ENTRY_NO  , 0}, /* Linear A */
  {0x10780, 0x107BF, ENTRY_NO  , 0}, /* Latin Extended-F */
  {0x10800, 0x1083F, ENTRY_NO  , 0}, /* Cypriot Syllabary */
  {0x10840, 0x1085F, ENTRY_NO  , 0}, /* Imperial Aramaic */
  {0x10860, 0x1087F, ENTRY_NO  , 0}, /* Palmyrene */
  {0x10880, 0x108AF, ENTRY_NO  , 0}, /* Nabataean */
  {0x108E0, 0x108FF, ENTRY_NO  , 0}, /* Hatran */
  {0x10900, 0x1091F, ENTRY_NO  , 0}, /* Phoenician */
  {0x10920, 0x1093F, ENTRY_NO  , 0}, /* Lydian */
  {0x10980, 0x1099F, ENTRY_NO  , 0}, /* Meroitic Hieroglyphs */
  {0x109A0, 0x109FF, ENTRY_NO  , 0}, /* Meroitic Cursive */
  {0x10A00, 0x10A5F, ENTRY_NO  , 0}, /* Kharoshthi */
  {0x10A60, 0x10A7F, ENTRY_NO  , 0}, /* Old South Arabian */
  {0x10A80, 0x10A9F, ENTRY_NO  , 0}, /* Old North Arabian */
  {0x10AC0, 0x10AFF, ENTRY_NO  , 0}, /* Manichaean */
  {0x10B00, 0x10B3F, ENTRY_NO  , 0}, /* Avestan */
  {0x10B40, 0x10B5F, ENTRY_NO  , 0}, /* Inscriptional Parthian */
  {0x10B60, 0x10B7F, ENTRY_NO  , 0}, /* Inscriptional Pahlavi */
  {0x10B80, 0x10BAF, ENTRY_NO  , 0}, /* Psalter Pahlavi */
  {0x10C00, 0x10C4F, ENTRY_NO  , 0}, /* Old Turkic */
  {0x10C80, 0x10CFF, ENTRY_NO  , 0}, /* Old Hungarian */
  {0x10D00, 0x10D3F, ENTRY_NO  , 0}, /* Hanifi Rohingya */
  {0x10E60, 0x10E7F, ENTRY_NO  , 0}, /* Rumi Numeral Symbols */
  {0x10E80, 0x10EBF, ENTRY_NO  , 0}, /* Yezidi */
  {0x10F00, 0x10F2F, ENTRY_NO  , 0}, /* Old Sogdian */
  {0x10F30, 0x10F6F, ENTRY_NO  , 0}, /* Sogdian */
  {0x10F70, 0x10FAF, ENTRY_NO  , 0}, /* Old Uyghur */
  {0x10FB0, 0x10FDF, ENTRY_NO  , 0}, /* Chorasmian */
  {0x10FE0, 0x10FFF, ENTRY_NO  , 0}, /* Elymaic */
  {0x11000, 0x1107F, ENTRY_NO  , 0}, /* Brahmi */
  {0x11080, 0x110CF, ENTRY_NO  , 0}, /* Kaithi */
  {0x110D0, 0x110FF, ENTRY_NO  , 0}, /* Sora Sompeng */
  {0x11100, 0x1114F, ENTRY_NO  , 0}, /* Chakma */
  {0x11150, 0x1117F, ENTRY_NO  , 0}, /* Mahajani */
  {0x11180, 0x111DF, ENTRY_NO  , 0}, /* Sharada */
  {0x111E0, 0x111FF, ENTRY_NO  , 0}, /* Sinhala Archaic Numbers*/
  {0x11200, 0x1124F, ENTRY_NO  , 0}, /* Khojki */
  {0x11280, 0x112AF, ENTRY_NO  , 0}, /* Multani */
  {0x112B0, 0x112FF, ENTRY_NO  , 0}, /* Khudawadi */
  {0x11300, 0x1137F, ENTRY_NO  , 0}, /* Grantha */
  {0x11400, 0x1147F, ENTRY_NO  , 0}, /* Newa */
  {0x11480, 0x114DF, ENTRY_NO  , 0}, /* Tirhuta */
  {0x11580, 0x115FF, ENTRY_NO  , 0}, /* Siddham */
  {0x11600, 0x1165F, ENTRY_NO  , 0}, /* Modi */
  {0x11660, 0x1167F, ENTRY_NO  , 0}, /* Mongolian Supplement */
  {0x11680, 0x116CF, ENTRY_NO  , 0}, /* Takri */
  {0x11700, 0x1173F, ENTRY_NO  , 0}, /* Ahom */
  {0x11800, 0x1184F, ENTRY_NO  , 0}, /* Dogra */
  {0x118A0, 0x118FF, ENTRY_NO  , 0}, /* Warang Citi */
  {0x11900, 0x1195F, ENTRY_NO  , 0}, /* Dives Akuru */
  {0x119A0, 0x119FF, ENTRY_NO  , 0}, /* Nandinagari */
  {0x11A00, 0x11A4F, ENTRY_NO  , 0}, /* Zanabazar Square */
  {0x11A50, 0x11AAF, ENTRY_NO  , 0}, /* Soyombo */
  {0x11AB0, 0x11ABF, ENTRY_NO  , 0}, /* Unified Canadian Aboriginal Syllabics Extended-A */
  {0x11AC0, 0x11AFF, ENTRY_NO  , 0}, /* Pau Cin Hau */
  {0x11C00, 0x11C6F, ENTRY_NO  , 0}, /* Bhaiksuki */
  {0x11C70, 0x11CBF, ENTRY_NO  , 0}, /* Marchen */
  {0x11D00, 0x11D5F, ENTRY_NO  , 0}, /* Masaram Gondi */
  {0x11D60, 0x11DAF, ENTRY_NO  , 0}, /* Gunjala Gondi */
  {0x11EE0, 0x11EFF, ENTRY_NO  , 0}, /* Makasar */
  {0x11FB0, 0x11FBF, ENTRY_NO  , 0}, /* Lisu Supplement */
  {0x11FC0, 0x11FFF, ENTRY_NO  , 0}, /* Tamil Supplement */
  {0x12000, 0x123FF, ENTRY_NO  , 0}, /* Cuneiform */
  {0x12400, 0x1247F, ENTRY_NO  , 0}, /* Cuneiform Numbers and Punctuation */
  {0x12480, 0x1254F, ENTRY_NO  , 0}, /* Early Dynastic Cuneiform */
  {0x12F90, 0x12FFF, ENTRY_NO  , 0}, /* Cypro-Minoan */
  {0x13000, 0x1342F, ENTRY_NO  , 0}, /* Egyptian Hieroglyphs */
  {0x13430, 0x1343F, ENTRY_NO  , 0}, /* Egyptian Hieroglyph Format Controls */
  {0x14400, 0x1467F, ENTRY_NO  , 0}, /* Anatolian Hieroglyphs */
  {0x16800, 0x16A3F, ENTRY_NO  , 0}, /* Bamum Supplement */
  {0x16A40, 0x16A6F, ENTRY_NO  , 0}, /* Mro */
  {0x16A70, 0x16ACF, ENTRY_NO  , 0}, /* Tangsa */
  {0x16AD0, 0x16AFF, ENTRY_NO  , 0}, /* Bassa Vah */
  {0x16B00, 0x16B8F, ENTRY_NO  , 0}, /* Pahawh Hmong */
  {0x16E40, 0x16E9F, ENTRY_NO  , 0}, /* Medefaidrin */
  {0x16F00, 0x16F9F, ENTRY_NO  , 0}, /* Miao */
  {0x16FE0, 0x16FFF, ENTRY_NO  , 0}, /* Ideographic Symbols and Punctuation */
  {0x17000, 0x187FF, ENTRY_NO  , 0}, /* Tangut */
  {0x18800, 0x18AFF, ENTRY_NO  , 0}, /* Tangut Components */
  {0x18B00, 0x18CFF, ENTRY_NO  , 0}, /* Khitan Small Script */
  {0x18D00, 0x18D8F, ENTRY_NO  , 0}, /* Tangut Supplement */
  {0x1AFF0, 0x1AFFF, ENTRY_NO  , 0}, /* Kana Extended-B */
  {0x1B000, 0x1B0FF, ENTRY_NO  , 0}, /* Kana Supplement */
  {0x1B100, 0x1B12F, ENTRY_NO  , 0}, /* Kana Extended-A */
  {0x1B130, 0x1B16F, ENTRY_NO  , 0}, /* Small Kana Extension */
  {0x1B170, 0x1B2FF, ENTRY_NO  , 0}, /* Nushu */
  {0x1BC00, 0x1BC9F, ENTRY_NO  , 0}, /* Duployan */
  {0x1BCA0, 0x1BCAF, ENTRY_NO  , 0}, /* Shorthand Format Controls */
  {0x1CF00, 0x1CFCF, ENTRY_NO  , 0}, /* Znamenny Musical Notation */
  {0x1D000, 0x1D0FF, ENTRY_NO  , 0}, /* Byzantine Musical Symbols */
  {0x1D100, 0x1D1FF, ENTRY_NO  , 0}, /* Musical Symbols */
  {0x1D200, 0x1D24F, ENTRY_NO  , 0}, /* Ancient Greek Musical Notation */
  {0x1D2E0, 0x1D2FF, ENTRY_NO  , 0}, /* Mayan Numerals */
  {0x1D300, 0x1D35F, ENTRY_NO  , 0}, /* Tai Xuan Jing Symbols */
  {0x1D360, 0x1D37F, ENTRY_NO  , 0}, /* Counting Rod Numerals */
  {0x1D400, 0x1D7FF, ENTRY_NO  , 0}, /* Mathematical Alphanumeric Symbols */
  {0x1D800, 0x1DAAF, ENTRY_NO  , 0}, /* Sutton SignWriting */
  {0x1DF00, 0x1DFFF, ENTRY_NO  , 0}, /* Latin Extended-G */
  {0x1E000, 0x1E02F, ENTRY_NO  , 0}, /* Glagolitic Supplement */
  {0x1E100, 0x1E14F, ENTRY_NO  , 0}, /* Nyiakeng Puachue Hmong */
  {0x1E290, 0x1E2BF, ENTRY_NO  , 0}, /* Toto */
  {0x1E2C0, 0x1E2FF, ENTRY_NO  , 0}, /* Wancho */
  {0x1E7E0, 0x1E7FF, ENTRY_NO  , 0}, /* Ethiopic Extended-B */
  {0x1E800, 0x1E8DF, ENTRY_NO  , 0}, /* Mende Kikakui */
  {0x1E900, 0x1E95F, ENTRY_NO  , 0}, /* Adlam */
  {0x1EC70, 0x1ECBF, ENTRY_NO  , 0}, /* Indic Siyaq Numbers */
  {0x1ED00, 0x1ED4F, ENTRY_NO  , 0}, /* Ottoman Siyaq Numbers */
  {0x1EE00, 0x1EEFF, ENTRY_NO  , 0}, /* Arabic Mathematical Alphabetic Symbols */
  {0x1F000, 0x1F02F, ENTRY_NO  , 0}, /* Mahjong Tiles */
  {0x1F030, 0x1F09F, ENTRY_NO  , 0}, /* Domino Tiles */
  {0x1F0A0, 0x1F0FF, ENTRY_NO  , 0}, /* Playing Cards */
  {0x1F100, 0x1F1FF, ENTRY_J   , 0}, /* Enclosed Alphanumeric Supplement */
  {0x1F200, 0x1F2FF, ENTRY_J   , 0}, /* Enclosed Ideographic Supplement */
  {0x1F300, 0x1F5FF, ENTRY_NO  , 0}, /* Miscellaneous Symbols and Pictographs */
  {0x1F600, 0x1F64F, ENTRY_NO  , 0}, /* Emoticons */
  {0x1F650, 0x1F67F, ENTRY_NO  , 0}, /* Ornamental Dingbats */
  {0x1F680, 0x1F6FF, ENTRY_NO  , 0}, /* Transport and Map Symbols */
  {0x1F700, 0x1F77F, ENTRY_NO  , 0}, /* Alchemical Symbols */
  {0x1F780, 0x1F7FF, ENTRY_NO  , 0}, /* Geometric Shapes Extended */
  {0x1F800, 0x1F8FF, ENTRY_NO  , 0}, /* Supplemental Arrows-C */
  {0x1F900, 0x1F9FF, ENTRY_NO  , 0}, /* Supplemental Symbols and Pictographs */
  {0x1FA00, 0x1FA6F, ENTRY_NO  , 0}, /* Chess Symbols */
  {0x1FA70, 0x1FAFF, ENTRY_NO  , 0}, /* Symbols and Pictographs Extended-A */
  {0x1FB00, 0x1FBFF, ENTRY_NO  , 0}, /* Symbols for Legacy Computing */
  {0x20000, 0x2A6DF, ENTRY_GCJ , 1}, /* CJK Unified Ideographs Extension B */
  {0x2A700, 0x2B73F, ENTRY_CJ  , 1}, /* CJK Unified Ideographs Extension C */
  {0x2B740, 0x2B81F, ENTRY_J   , 1}, /* CJK Unified Ideographs Extension D */
  {0x2B820, 0x2CEAF, ENTRY_C   , 1}, /* CJK Unified Ideographs Extension E */
  {0x2CEB0, 0x2EBEF, ENTRY_CJ  , 1}, /* CJK Unified Ideographs Extension F */
  {0x2F800, 0x2FA1F, ENTRY_CJ  , 1}, /* CJK Compatibility Ideographs Supplement */
  {0x30000, 0x3134F, ENTRY_NO  , 1}, /* CJK Unified Ideographs Extension G */
  {0xE0000, 0xE007F, ENTRY_NO  , 0}, /* Tags */
  {0xE0100, 0xE01EF, ENTRY_NO  , 0}, /* Variation Selectors Supplement */
  {0xF0000, 0xFFFFF, ENTRY_NO  , 0}, /* Supplementary Private Use Area-A */
  {0x100000, 0x10FFFF, ENTRY_NO  , 0}, /* Supplementary Private Use Area-B */
  {0x200000, 0xFFFFFF, ENTRY_NO  , 0}, /* illegal */
};


int search_cjk_entry(long ch, long cjk) {
  static int ib = 0, ic = 0;
  uniblock_iskanji = 0; /* initialize */
  if (cjk==ENTRY_JQ) return
	(ch==U_OPEN_SQUOTE || ch==U_CLOSE_SQUOTE
	 || ch==U_OPEN_DQUOTE || ch==U_CLOSE_DQUOTE);
  while(ublock_data[ib].max<ch) ib++;
  if (ublock_data[ib].min<=ch && ch<=ublock_data[ib].max)
    uniblock_iskanji = ublock_data[ib].kanji;
  if (cjk==ENTRY_NO) {
    return 1;
  } else if (cjk==ENTRY_CUSTOM) {
    while(usertable_charset[ic].max<ch && ic<usertable_charset_max) ic++;
    return (ic<MAX_CHAR_TABLE && usertable_charset[ic].min<=ch && ch<=usertable_charset[ic].max);
  } else if (ublock_data[ib].min<=ch && ch<=ublock_data[ib].max) {
    return ublock_data[ib].cjk & cjk;
  } else {
    return 0;
  }
}


/* for unit test                                      */
/*   ex. $ gcc -g -o uniblock.test uniblock.c -DDEBUG */
#ifdef DEBUG
int main() {
  long ch;

  /* trial inputs */
  usertable_charset_max=2;
  usertable_charset[0].min=0xFFF0;
  usertable_charset[0].max=0xFFF3;
  usertable_charset[1].min=0xFFF8;
  usertable_charset[1].max=0xFFFB;

  for (ch=0x0;ch<0x10000;ch++) {
    printf(" %05lx %1d G:%2d C:%2d J:%2d K:%2d custom:%2d\n", ch, uniblock_iskanji,
	   search_cjk_entry(ch,ENTRY_G),
	   search_cjk_entry(ch,ENTRY_C),
	   search_cjk_entry(ch,ENTRY_J),
	   search_cjk_entry(ch,ENTRY_K),
	   search_cjk_entry(ch,ENTRY_CUSTOM));
  }
  return(0);
}
#endif
