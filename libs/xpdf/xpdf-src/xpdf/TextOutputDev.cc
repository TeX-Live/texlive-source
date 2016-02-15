//========================================================================
//
// TextOutputDev.cc
//
// Copyright 1997-2014 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <ctype.h>
#ifdef _WIN32
#include <fcntl.h> // for O_BINARY
#include <io.h>    // for setmode
#endif
#include "gmem.h"
#include "GString.h"
#include "GList.h"
#include "config.h"
#include "Error.h"
#include "GlobalParams.h"
#include "UnicodeMap.h"
#include "UnicodeTypeTable.h"
#include "GfxState.h"
#include "Link.h"
#include "TextOutputDev.h"

//------------------------------------------------------------------------
// parameters
//------------------------------------------------------------------------

// Size of bins used for horizontal and vertical profiles is
// splitPrecisionMul * minFontSize.
#define splitPrecisionMul 0.05

// Minimum allowed split precision.
#define minSplitPrecision 0.01

// yMin and yMax (or xMin and xMax for rot=1,3) are adjusted by this
// fraction of the text height, to allow for slightly overlapping
// lines (or large ascent/descent values).
#define ascentAdjustFactor 0
#define descentAdjustFactor 0.35

// Gaps larger than max{gap} - splitGapSlack * avgFontSize are
// considered to be equivalent.
#define splitGapSlack 0.2

// The vertical gap threshold (minimum gap required to split
// vertically) depends on the (approximate) number of lines in the
// block:
//   threshold = (max + slope * nLines) * avgFontSize
// with a min value of vertGapThresholdMin * avgFontSize.
#define vertGapThresholdMin 0.8
#define vertGapThresholdMax 3
#define vertGapThresholdSlope -0.5

// Vertical gap threshold for table mode.
#define vertGapThresholdTableMin 0.2
#define vertGapThresholdTableMax 0.5
#define vertGapThresholdTableSlope -0.02

// A large character has a font size larger than
// largeCharThreshold * avgFontSize.
#define largeCharThreshold 1.5

// A block will be split vertically only if the resulting chunk
// widths are greater than vertSplitChunkThreshold * avgFontSize.
#define vertSplitChunkThreshold 2

// Max difference in primary,secondary coordinates (as a fraction of
// the font size) allowed for duplicated text (fake boldface, drop
// shadows) which is to be discarded.
#define dupMaxPriDelta 0.1
#define dupMaxSecDelta 0.2

// Inter-character spacing that varies by less than this multiple of
// font size is assumed to be equivalent.
#define uniformSpacing 0.07

// Typical word spacing, as a fraction of font size.  This will be
// added to the minimum inter-character spacing, to account for wide
// character spacing.
#define wordSpacing 0.1

// Minimum paragraph indent from left margin, as a fraction of font
// size.
#define minParagraphIndent 0.5

// If the space between two lines is greater than
// paragraphSpacingThreshold * avgLineSpacing, start a new paragraph.
#define paragraphSpacingThreshold 1.2

// If font size changes by at least this much (measured in points)
// between lines, start a new paragraph.
#define paragraphFontSizeDelta 1

// Spaces at the start of a line in physical layout mode are this wide
// (as a multiple of font size).
#define physLayoutSpaceWidth 0.33

// Table cells (TextColumns) are allowed to overlap by this much
// in table layout mode (as a fraction of cell width or height).
#define tableCellOverlapSlack 0.05

// Primary axis delta which will cause a line break in raw mode
// (as a fraction of font size).
#define rawModeLineDelta 0.5

// Secondary axis delta which will cause a word break in raw mode
// (as a fraction of font size).
#define rawModeWordSpacing 0.15

// Secondary axis overlap which will cause a line break in raw mode
// (as a fraction of font size).
#define rawModeCharOverlap 0.2

// Max spacing (as a multiple of font size) allowed between the end of
// a line and a clipped character to be included in that line.
#define clippedTextMaxWordSpace 0.5

// Max width of underlines (in points).
#define maxUnderlineWidth 3

// Max horizontal distance between edge of word and start of underline
// (as a fraction of font size).
#define underlineSlack 0.2

// Max vertical distance between baseline of word and start of
// underline (as a fraction of font size).
#define underlineBaselineSlack 0.2

// Max distance between edge of text and edge of link border (as a
// fraction of font size).
#define hyperlinkSlack 0.2

//------------------------------------------------------------------------
// TextChar
//------------------------------------------------------------------------

class TextChar {
public:

  TextChar(Unicode cA, int charPosA, int charLenA,
	   double xMinA, double yMinA, double xMaxA, double yMaxA,
	   int rotA, GBool clippedA, GBool invisibleA,
	   TextFontInfo *fontA, double fontSizeA,
	   double colorRA, double colorGA, double colorBA);

  static int cmpX(const void *p1, const void *p2);
  static int cmpY(const void *p1, const void *p2);

  Unicode c;
  int charPos;
  int charLen;
  double xMin, yMin, xMax, yMax;
  Guchar rot;
  char clipped;
  char invisible;
  TextFontInfo *font;
  double fontSize;
  double colorR,
         colorG,
         colorB;
};

TextChar::TextChar(Unicode cA, int charPosA, int charLenA,
		   double xMinA, double yMinA, double xMaxA, double yMaxA,
		   int rotA, GBool clippedA, GBool invisibleA,
		   TextFontInfo *fontA, double fontSizeA,
		   double colorRA, double colorGA, double colorBA) {
  double t;

  c = cA;
  charPos = charPosA;
  charLen = charLenA;
  xMin = xMinA;
  yMin = yMinA;
  xMax = xMaxA;
  yMax = yMaxA;
  // this can happen with vertical writing mode, or with odd values
  // for the char/word spacing parameters
  if (xMin > xMax) {
    t = xMin; xMin = xMax; xMax = t;
  }
  if (yMin > yMax) {
    t = yMin; yMin = yMax; yMax = t;
  }
  rot = (Guchar)rotA;
  clipped = (char)clippedA;
  invisible = (char)invisibleA;
  font = fontA;
  fontSize = fontSizeA;
  colorR = colorRA;
  colorG = colorGA;
  colorB = colorBA;
}

int TextChar::cmpX(const void *p1, const void *p2) {
  const TextChar *ch1 = *(const TextChar **)p1;
  const TextChar *ch2 = *(const TextChar **)p2;

  if (ch1->xMin < ch2->xMin) {
    return -1;
  } else if (ch1->xMin > ch2->xMin) {
    return 1;
  } else {
    return 0;
  }
}

int TextChar::cmpY(const void *p1, const void *p2) {
  const TextChar *ch1 = *(const TextChar **)p1;
  const TextChar *ch2 = *(const TextChar **)p2;

  if (ch1->yMin < ch2->yMin) {
    return -1;
  } else if (ch1->yMin > ch2->yMin) {
    return 1;
  } else {
    return 0;
  }
}

//------------------------------------------------------------------------
// TextBlock
//------------------------------------------------------------------------

enum TextBlockType {
  blkVertSplit,
  blkHorizSplit,
  blkLeaf
};

enum TextBlockTag {
  blkTagMulticolumn,
  blkTagColumn,
  blkTagLine
};

class TextBlock {
public:

  TextBlock(TextBlockType typeA, int rotA);
  ~TextBlock();
  void addChild(TextBlock *child);
  void addChild(TextChar *child);
  void prependChild(TextChar *child);
  void updateBounds(int childIdx);

  TextBlockType type;
  TextBlockTag tag;
  int rot;
  double xMin, yMin, xMax, yMax;
  GBool smallSplit;		// true for blkVertSplit/blkHorizSplit
				//   where the gap size is small
  GList *children;		// for blkLeaf, children are TextWord;
				//   for others, children are TextBlock
};

TextBlock::TextBlock(TextBlockType typeA, int rotA) {
  type = typeA;
  tag = blkTagMulticolumn;
  rot = rotA;
  xMin = yMin = xMax = yMax = 0;
  smallSplit = gFalse;
  children = new GList();
}

TextBlock::~TextBlock() {
  if (type == blkLeaf) {
    delete children;
  } else {
    deleteGList(children, TextBlock);
  }
}

void TextBlock::addChild(TextBlock *child) {
  if (children->getLength() == 0) {
    xMin = child->xMin;
    yMin = child->yMin;
    xMax = child->xMax;
    yMax = child->yMax;
  } else {
    if (child->xMin < xMin) {
      xMin = child->xMin;
    }
    if (child->yMin < yMin) {
      yMin = child->yMin;
    }
    if (child->xMax > xMax) {
      xMax = child->xMax;
    }
    if (child->yMax > yMax) {
      yMax = child->yMax;
    }
  }
  children->append(child);
}

void TextBlock::addChild(TextChar *child) {
  if (children->getLength() == 0) {
    xMin = child->xMin;
    yMin = child->yMin;
    xMax = child->xMax;
    yMax = child->yMax;
  } else {
    if (child->xMin < xMin) {
      xMin = child->xMin;
    }
    if (child->yMin < yMin) {
      yMin = child->yMin;
    }
    if (child->xMax > xMax) {
      xMax = child->xMax;
    }
    if (child->yMax > yMax) {
      yMax = child->yMax;
    }
  }
  children->append(child);
}

void TextBlock::prependChild(TextChar *child) {
  if (children->getLength() == 0) {
    xMin = child->xMin;
    yMin = child->yMin;
    xMax = child->xMax;
    yMax = child->yMax;
  } else {
    if (child->xMin < xMin) {
      xMin = child->xMin;
    }
    if (child->yMin < yMin) {
      yMin = child->yMin;
    }
    if (child->xMax > xMax) {
      xMax = child->xMax;
    }
    if (child->yMax > yMax) {
      yMax = child->yMax;
    }
  }
  children->insert(0, child);
}

void TextBlock::updateBounds(int childIdx) {
  TextBlock *child;

  child = (TextBlock *)children->get(childIdx);
  if (child->xMin < xMin) {
    xMin = child->xMin;
  }
  if (child->yMin < yMin) {
    yMin = child->yMin;
  }
  if (child->xMax > xMax) {
    xMax = child->xMax;
  }
  if (child->yMax > yMax) {
    yMax = child->yMax;
  }
}

//------------------------------------------------------------------------
// TextUnderline
//------------------------------------------------------------------------

class TextUnderline {
public:

  TextUnderline(double x0A, double y0A, double x1A, double y1A)
    { x0 = x0A; y0 = y0A; x1 = x1A; y1 = y1A; horiz = y0 == y1; }
  ~TextUnderline() {}

  double x0, y0, x1, y1;
  GBool horiz;
};

//------------------------------------------------------------------------
// TextLink
//------------------------------------------------------------------------

class TextLink {
public:

  TextLink(double xMinA, double yMinA, double xMaxA, double yMaxA,
	   GString *uriA)
    { xMin = xMinA; yMin = yMinA; xMax = xMaxA; yMax = yMaxA; uri = uriA; }
  ~TextLink();

  double xMin, yMin, xMax, yMax;
  GString *uri;
};

TextLink::~TextLink() {
  if (uri) {
    delete uri;
  }
}

//------------------------------------------------------------------------
// TextOutputControl
//------------------------------------------------------------------------

TextOutputControl::TextOutputControl() {
  mode = textOutReadingOrder;
  fixedPitch = 0;
  fixedLineSpacing = 0;
  html = gFalse;
  clipText = gFalse;
}


//------------------------------------------------------------------------
// TextFontInfo
//------------------------------------------------------------------------

TextFontInfo::TextFontInfo(GfxState *state) {
  GfxFont *gfxFont;

  gfxFont = state->getFont();
  if (gfxFont) {
    fontID = *gfxFont->getID();
    ascent = gfxFont->getAscent();
    descent = gfxFont->getDescent();
    // "odd" ascent/descent values cause trouble more often than not
    // (in theory these could be legitimate values for oddly designed
    // fonts -- but they are more often due to buggy PDF generators)
    // (values that are too small are a different issue -- those seem
    // to be more commonly legitimate)
    if (ascent > 1) {
      ascent = 0.75;
    }
    if (descent < -0.5) {
      descent = -0.25;
    }
  } else {
    fontID.num = -1;
    fontID.gen = -1;
    ascent = 0.75;
    descent = -0.25;
  }
  fontName = (gfxFont && gfxFont->getName()) ? gfxFont->getName()->copy()
                                             : (GString *)NULL;
  flags = gfxFont ? gfxFont->getFlags() : 0;
  mWidth = 0;
  if (gfxFont && !gfxFont->isCIDFont()) {
    char *name;
    int code;
    for (code = 0; code < 256; ++code) {
      if ((name = ((Gfx8BitFont *)gfxFont)->getCharName(code)) &&
	  name[0] == 'm' && name[1] == '\0') {
	mWidth = ((Gfx8BitFont *)gfxFont)->getWidth(code);
	break;
      }
    }
  }
}

TextFontInfo::~TextFontInfo() {
  if (fontName) {
    delete fontName;
  }
}

GBool TextFontInfo::matches(GfxState *state) {
  Ref *id;

  if (!state->getFont()) {
    return gFalse;
  }
  id = state->getFont()->getID();
  return id->num == fontID.num && id->gen == fontID.gen;
}

//------------------------------------------------------------------------
// TextWord
//------------------------------------------------------------------------

// Build a TextWord object, using chars[start .. start+len-1].
// (If rot >= 2, the chars list is in reverse order.)
TextWord::TextWord(GList *chars, int start, int lenA,
		   int rotA, GBool spaceAfterA) {
  TextChar *ch;
  int i;

  rot = rotA;
  len = lenA;
  text = (Unicode *)gmallocn(len, sizeof(Unicode));
  edge = (double *)gmallocn(len + 1, sizeof(double));
  charPos = (int *)gmallocn(len + 1, sizeof(int));
  switch (rot) {
  case 0:
  default:
    ch = (TextChar *)chars->get(start);
    xMin = ch->xMin;
    yMin = ch->yMin;
    yMax = ch->yMax;
    ch = (TextChar *)chars->get(start + len - 1);
    xMax = ch->xMax;
    break;
  case 1:
    ch = (TextChar *)chars->get(start);
    xMin = ch->xMin;
    xMax = ch->xMax;
    yMin = ch->yMin;
    ch = (TextChar *)chars->get(start + len - 1);
    yMax = ch->yMax;
    break;
  case 2:
    ch = (TextChar *)chars->get(start);
    xMax = ch->xMax;
    yMin = ch->yMin;
    yMax = ch->yMax;
    ch = (TextChar *)chars->get(start + len - 1);
    xMin = ch->xMin;
    break;
  case 3:
    ch = (TextChar *)chars->get(start);
    xMin = ch->xMin;
    xMax = ch->xMax;
    yMax = ch->yMax;
    ch = (TextChar *)chars->get(start + len - 1);
    yMin = ch->yMin;
    break;
  }
  for (i = 0; i < len; ++i) {
    ch = (TextChar *)chars->get(rot >= 2 ? start + len - 1 - i : start + i);
    text[i] = ch->c;
    charPos[i] = ch->charPos;
    if (i == len - 1) {
      charPos[len] = ch->charPos + ch->charLen;
    }
    switch (rot) {
    case 0:
    default:
      edge[i] = ch->xMin;
      if (i == len - 1) {
	edge[len] = ch->xMax;
      }
      break;
    case 1:
      edge[i] = ch->yMin;
      if (i == len - 1) {
	edge[len] = ch->yMax;
      }
      break;
    case 2:
      edge[i] = ch->xMax;
      if (i == len - 1) {
	edge[len] = ch->xMin;
      }
      break;
    case 3:
      edge[i] = ch->yMax;
      if (i == len - 1) {
	edge[len] = ch->yMin;
      }
      break;
    }
  }
  ch = (TextChar *)chars->get(start);
  font = ch->font;
  fontSize = ch->fontSize;
  spaceAfter = spaceAfterA;
  underlined = gFalse;
  link = NULL;
  colorR = ch->colorR;
  colorG = ch->colorG;
  colorB = ch->colorB;
  invisible = ch->invisible;
}

TextWord::TextWord(TextWord *word) {
  *this = *word;
  text = (Unicode *)gmallocn(len, sizeof(Unicode));
  memcpy(text, word->text, len * sizeof(Unicode));
  edge = (double *)gmallocn(len + 1, sizeof(double));
  memcpy(edge, word->edge, (len + 1) * sizeof(double));
  charPos = (int *)gmallocn(len + 1, sizeof(int));
  memcpy(charPos, word->charPos, (len + 1) * sizeof(int));
}

TextWord::~TextWord() {
  gfree(text);
  gfree(edge);
  gfree(charPos);
}

// This is used to append a clipped character to a word.
void TextWord::appendChar(TextChar *ch) {
  if (ch->xMin < xMin) {
    xMin = ch->xMin;
  }
  if (ch->xMax > xMax) {
    xMax = ch->xMax;
  }
  if (ch->yMin < yMin) {
    yMin = ch->yMin;
  }
  if (ch->yMax > yMax) {
    yMax = ch->yMax;
  }
  text = (Unicode *)greallocn(text, len + 1, sizeof(Unicode));
  edge = (double *)greallocn(edge, len + 2, sizeof(double));
  charPos = (int *)greallocn(charPos, len + 2, sizeof(int));
  text[len] = ch->c;
  charPos[len] = ch->charPos;
  charPos[len+1] = ch->charPos + ch->charLen;
  switch (rot) {
  case 0:
  default:
    edge[len] = ch->xMin;
    edge[len+1] = ch->xMax;
    break;
  case 1:
    edge[len] = ch->yMin;
    edge[len+1] = ch->yMax;
    break;
  case 2:
    edge[len] = ch->xMax;
    edge[len+1] = ch->xMin;
    break;
  case 3:
    edge[len] = ch->yMax;
    edge[len+1] = ch->yMin;
    break;
  }
  ++len;
}

int TextWord::cmpYX(const void *p1, const void *p2) {
  const TextWord *word1 = *(const TextWord **)p1;
  const TextWord *word2 = *(const TextWord **)p2;
  double cmp;

  if ((cmp = word1->yMin - word2->yMin) == 0) {
    cmp = word1->xMin - word2->xMin;
  }
  return cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
}

int TextWord::cmpCharPos(const void *p1, const void *p2) {
  const TextWord *word1 = *(const TextWord **)p1;
  const TextWord *word2 = *(const TextWord **)p2;

  return word1->charPos[0] - word2->charPos[0];
}

GString *TextWord::getText() {
  GString *s;
  UnicodeMap *uMap;
  char buf[8];
  int n, i;

  s = new GString();
  if (!(uMap = globalParams->getTextEncoding())) {
    return s;
  }
  for (i = 0; i < len; ++i) {
    n = uMap->mapUnicode(text[i], buf, sizeof(buf));
    s->append(buf, n);
  }
  uMap->decRefCnt();
  return s;
}

void TextWord::getCharBBox(int charIdx, double *xMinA, double *yMinA,
			   double *xMaxA, double *yMaxA) {
  if (charIdx < 0 || charIdx >= len) {
    return;
  }
  switch (rot) {
  case 0:
    *xMinA = edge[charIdx];
    *xMaxA = edge[charIdx + 1];
    *yMinA = yMin;
    *yMaxA = yMax;
    break;
  case 1:
    *xMinA = xMin;
    *xMaxA = xMax;
    *yMinA = edge[charIdx];
    *yMaxA = edge[charIdx + 1];
    break;
  case 2:
    *xMinA = edge[charIdx + 1];
    *xMaxA = edge[charIdx];
    *yMinA = yMin;
    *yMaxA = yMax;
    break;
  case 3:
    *xMinA = xMin;
    *xMaxA = xMax;
    *yMinA = edge[charIdx + 1];
    *yMaxA = edge[charIdx];
    break;
  }
}

double TextWord::getBaseline() {
  switch (rot) {
  case 0:
  default:
    return yMax + fontSize * font->descent;
  case 1:
    return xMin - fontSize * font->descent;
  case 2:
    return yMin - fontSize * font->descent;
  case 3:
    return xMax + fontSize * font->descent;
  }
}

GString *TextWord::getLinkURI() {
  return link ? link->uri : (GString *)NULL;
}

//------------------------------------------------------------------------
// TextLine
//------------------------------------------------------------------------

TextLine::TextLine(GList *wordsA, double xMinA, double yMinA,
		   double xMaxA, double yMaxA, double fontSizeA) {
  TextWord *word;
  int i, j, k;

  words = wordsA;
  rot = 0;
  xMin = xMinA;
  yMin = yMinA;
  xMax = xMaxA;
  yMax = yMaxA;
  fontSize = fontSizeA;
  px = 0;
  pw = 0;

  // build the text
  len = 0;
  for (i = 0; i < words->getLength(); ++i) {
    word = (TextWord *)words->get(i);
    len += word->len;
    if (word->spaceAfter) {
      ++len;
    }
  }
  text = (Unicode *)gmallocn(len, sizeof(Unicode));
  edge = (double *)gmallocn(len + 1, sizeof(double));
  j = 0;
  for (i = 0; i < words->getLength(); ++i) {
    word = (TextWord *)words->get(i);
    if (i == 0) {
      rot = word->rot;
    }
    for (k = 0; k < word->len; ++k) {
      text[j] = word->text[k];
      edge[j] = word->edge[k];
      ++j;
    }
    edge[j] = word->edge[word->len];
    if (word->spaceAfter) {
      text[j] = (Unicode)0x0020;
      ++j;
      edge[j] = edge[j - 1];
    }
  }
  //~ need to check for other Unicode chars used as hyphens
  hyphenated = text[len - 1] == (Unicode)'-';
}

TextLine::~TextLine() {
  deleteGList(words, TextWord);
  gfree(text);
  gfree(edge);
}

double TextLine::getBaseline() {
  TextWord *word0;

  word0 = (TextWord *)words->get(0);
  switch (rot) {
  case 0:
  default:
    return yMax + fontSize * word0->font->descent;
  case 1:
    return xMin - fontSize * word0->font->descent;
  case 2:
    return yMin - fontSize * word0->font->descent;
  case 3:
    return xMax + fontSize * word0->font->descent;
  }
}

//------------------------------------------------------------------------
// TextParagraph
//------------------------------------------------------------------------

TextParagraph::TextParagraph(GList *linesA) {
  TextLine *line;
  int i;

  lines = linesA;
  xMin = yMin = xMax = yMax = 0;
  for (i = 0; i < lines->getLength(); ++i) {
    line = (TextLine *)lines->get(i);
    if (i == 0 || line->xMin < xMin) {
      xMin = line->xMin;
    }
    if (i == 0 || line->yMin < yMin) {
      yMin = line->yMin;
    }
    if (i == 0 || line->xMax > xMax) {
      xMax = line->xMax;
    }
    if (i == 0 || line->yMax > yMax) {
      yMax = line->yMax;
    }
  }
}

TextParagraph::~TextParagraph() {
  deleteGList(lines, TextLine);
}

//------------------------------------------------------------------------
// TextColumn
//------------------------------------------------------------------------

TextColumn::TextColumn(GList *paragraphsA, double xMinA, double yMinA,
		       double xMaxA, double yMaxA) {
  paragraphs = paragraphsA;
  xMin = xMinA;
  yMin = yMinA;
  xMax = xMaxA;
  yMax = yMaxA;
  px = py = 0;
  pw = ph = 0;
}

TextColumn::~TextColumn() {
  deleteGList(paragraphs, TextParagraph);
}

int TextColumn::cmpX(const void *p1, const void *p2) {
  const TextColumn *col1 = *(const TextColumn **)p1;
  const TextColumn *col2 = *(const TextColumn **)p2;

  if (col1->xMin < col2->xMin) {
    return -1;
  } else if (col1->xMin > col2->xMin) {
    return 1;
  } else {
    return 0;
  }
}

int TextColumn::cmpY(const void *p1, const void *p2) {
  const TextColumn *col1 = *(const TextColumn **)p1;
  const TextColumn *col2 = *(const TextColumn **)p2;

  if (col1->yMin < col2->yMin) {
    return -1;
  } else if (col1->yMin > col2->yMin) {
    return 1;
  } else {
    return 0;
  }
}

int TextColumn::cmpPX(const void *p1, const void *p2) {
  const TextColumn *col1 = *(const TextColumn **)p1;
  const TextColumn *col2 = *(const TextColumn **)p2;

  if (col1->px < col2->px) {
    return -1;
  } else if (col1->px > col2->px) {
    return 1;
  } else {
    return 0;
  }
}

//------------------------------------------------------------------------
// TextWordList
//------------------------------------------------------------------------

TextWordList::TextWordList(GList *wordsA) {
  words = wordsA;
}

TextWordList::~TextWordList() {
  deleteGList(words, TextWord);
}

int TextWordList::getLength() {
  return words->getLength();
}

TextWord *TextWordList::get(int idx) {
  if (idx < 0 || idx >= words->getLength()) {
    return NULL;
  }
  return (TextWord *)words->get(idx);
}

//------------------------------------------------------------------------
// TextPage
//------------------------------------------------------------------------

TextPage::TextPage(TextOutputControl *controlA) {
  control = *controlA;
  pageWidth = pageHeight = 0;
  charPos = 0;
  curFont = NULL;
  curFontSize = 0;
  curRot = 0;
  nTinyChars = 0;
  actualText = NULL;
  actualTextLen = 0;
  actualTextX0 = 0;
  actualTextY0 = 0;
  actualTextX1 = 0;
  actualTextY1 = 0;
  actualTextNBytes = 0;

  chars = new GList();
  fonts = new GList();

  underlines = new GList();
  links = new GList();

  findCols = NULL;
  findLR = gTrue;
  lastFindXMin = lastFindYMin = 0;
  haveLastFind = gFalse;
}

TextPage::~TextPage() {
  clear();
  deleteGList(chars, TextChar);
  deleteGList(fonts, TextFontInfo);
  deleteGList(underlines, TextUnderline);
  deleteGList(links, TextLink);
  if (findCols) {
    deleteGList(findCols, TextColumn);
  }
}

void TextPage::startPage(GfxState *state) {
  clear();
  if (state) {
    pageWidth = state->getPageWidth();
    pageHeight = state->getPageHeight();
  } else {
    pageWidth = pageHeight = 0;
  }
}

void TextPage::clear() {
  pageWidth = pageHeight = 0;
  charPos = 0;
  curFont = NULL;
  curFontSize = 0;
  curRot = 0;
  nTinyChars = 0;
  gfree(actualText);
  actualText = NULL;
  actualTextLen = 0;
  actualTextNBytes = 0;
  deleteGList(chars, TextChar);
  chars = new GList();
  deleteGList(fonts, TextFontInfo);
  fonts = new GList();
  deleteGList(underlines, TextUnderline);
  underlines = new GList();
  deleteGList(links, TextLink);
  links = new GList();

  if (findCols) {
    deleteGList(findCols, TextColumn);
    findCols = NULL;
  }
  findLR = gTrue;
  lastFindXMin = lastFindYMin = 0;
  haveLastFind = gFalse;
}

void TextPage::updateFont(GfxState *state) {
  GfxFont *gfxFont;
  double *fm;
  char *name;
  int code, mCode, letterCode, anyCode;
  double w;
  double m[4], m2[4];
  int i;

  // get the font info object
  curFont = NULL;
  for (i = 0; i < fonts->getLength(); ++i) {
    curFont = (TextFontInfo *)fonts->get(i);
    if (curFont->matches(state)) {
      break;
    }
    curFont = NULL;
  }
  if (!curFont) {
    curFont = new TextFontInfo(state);
    fonts->append(curFont);
  }

  // adjust the font size
  gfxFont = state->getFont();
  curFontSize = state->getTransformedFontSize();
  if (gfxFont && gfxFont->getType() == fontType3) {
    // This is a hack which makes it possible to deal with some Type 3
    // fonts.  The problem is that it's impossible to know what the
    // base coordinate system used in the font is without actually
    // rendering the font.  This code tries to guess by looking at the
    // width of the character 'm' (which breaks if the font is a
    // subset that doesn't contain 'm').
    mCode = letterCode = anyCode = -1;
    for (code = 0; code < 256; ++code) {
      name = ((Gfx8BitFont *)gfxFont)->getCharName(code);
      if (name && name[0] == 'm' && name[1] == '\0') {
	mCode = code;
      }
      if (letterCode < 0 && name && name[1] == '\0' &&
	  ((name[0] >= 'A' && name[0] <= 'Z') ||
	   (name[0] >= 'a' && name[0] <= 'z'))) {
	letterCode = code;
      }
      if (anyCode < 0 && name &&
	  ((Gfx8BitFont *)gfxFont)->getWidth(code) > 0) {
	anyCode = code;
      }
    }
    if (mCode >= 0 &&
	(w = ((Gfx8BitFont *)gfxFont)->getWidth(mCode)) > 0) {
      // 0.6 is a generic average 'm' width -- yes, this is a hack
      curFontSize *= w / 0.6;
    } else if (letterCode >= 0 &&
	       (w = ((Gfx8BitFont *)gfxFont)->getWidth(letterCode)) > 0) {
      // even more of a hack: 0.5 is a generic letter width
      curFontSize *= w / 0.5;
    } else if (anyCode >= 0 &&
	       (w = ((Gfx8BitFont *)gfxFont)->getWidth(anyCode)) > 0) {
      // better than nothing: 0.5 is a generic character width
      curFontSize *= w / 0.5;
    }
    fm = gfxFont->getFontMatrix();
    if (fm[0] != 0) {
      curFontSize *= fabs(fm[3] / fm[0]);
    }
  }

  // compute the rotation
  state->getFontTransMat(&m[0], &m[1], &m[2], &m[3]);
  if (gfxFont && gfxFont->getType() == fontType3) {
    fm = gfxFont->getFontMatrix();
    m2[0] = fm[0] * m[0] + fm[1] * m[2];
    m2[1] = fm[0] * m[1] + fm[1] * m[3];
    m2[2] = fm[2] * m[0] + fm[3] * m[2];
    m2[3] = fm[2] * m[1] + fm[3] * m[3];
    m[0] = m2[0];
    m[1] = m2[1];
    m[2] = m2[2];
    m[3] = m2[3];
  }
  if (fabs(m[0] * m[3]) > fabs(m[1] * m[2])) {
    curRot = (m[0] > 0 || m[3] < 0) ? 0 : 2;
  } else {
    curRot = (m[2] > 0) ? 1 : 3;
  }
}

void TextPage::addChar(GfxState *state, double x, double y,
		       double dx, double dy,
		       CharCode c, int nBytes, Unicode *u, int uLen) {
  double x1, y1, x2, y2, w1, h1, dx2, dy2, ascent, descent, sp;
  double xMin, yMin, xMax, yMax;
  double clipXMin, clipYMin, clipXMax, clipYMax;
  GfxRGB rgb;
  GBool clipped, rtl;
  int i, j;

  // if we're in an ActualText span, save the position info (the
  // ActualText chars will be added by TextPage::endActualText()).
  if (actualText) {
    if (!actualTextNBytes) {
      actualTextX0 = x;
      actualTextY0 = y;
    }
    actualTextX1 = x + dx;
    actualTextY1 = y + dy;
    actualTextNBytes += nBytes;
    return;
  }

  // subtract char and word spacing from the dx,dy values
  sp = state->getCharSpace();
  if (c == (CharCode)0x20) {
    sp += state->getWordSpace();
  }
  state->textTransformDelta(sp * state->getHorizScaling(), 0, &dx2, &dy2);
  dx -= dx2;
  dy -= dy2;
  state->transformDelta(dx, dy, &w1, &h1);

  // throw away chars that aren't inside the page bounds
  // (and also do a sanity check on the character size)
  state->transform(x, y, &x1, &y1);
  if (x1 + w1 < 0 || x1 > pageWidth ||
      y1 + h1 < 0 || y1 > pageHeight ||
      w1 > pageWidth || h1 > pageHeight) {
    charPos += nBytes;
    return;
  }

  // check the tiny chars limit
  if (!globalParams->getTextKeepTinyChars() &&
      fabs(w1) < 3 && fabs(h1) < 3) {
    if (++nTinyChars > 50000) {
      charPos += nBytes;
      return;
    }
  }

  // skip space characters
  if (uLen == 1 && u[0] == (Unicode)0x20) {
    charPos += nBytes;
    return;
  }

  // check for clipping
  clipped = gFalse;
  if (control.clipText) {
    state->getClipBBox(&clipXMin, &clipYMin, &clipXMax, &clipYMax);
    if (x1 + 0.1 * w1 < clipXMin || x1 + 0.9 * w1 > clipXMax ||
	y1 + 0.1 * h1 < clipYMin || y1 + 0.9 * h1 > clipYMax) {
      clipped = gTrue;
    }
  }

  // add the characters
  if (uLen > 0) {

    // handle right-to-left ligatures: if there are multiple Unicode
    // characters, and they're all right-to-left, insert them in
    // right-to-left order
    if (uLen > 1) {
      rtl = gTrue;
      for (i = 0; i < uLen; ++i) {
	if (!unicodeTypeR(u[i])) {
	  rtl = gFalse;
	  break;
	}
      }
    } else {
      rtl = gFalse;
    }

    w1 /= uLen;
    h1 /= uLen;
    ascent = curFont->ascent * curFontSize;
    descent = curFont->descent * curFontSize;
    for (i = 0; i < uLen; ++i) {
      x2 = x1 + i * w1;
      y2 = y1 + i * h1;
      switch (curRot) {
      case 0:
      default:
	xMin = x2;
	xMax = x2 + w1;
	yMin = y2 - ascent;
	yMax = y2 - descent;
	break;
      case 1:
	xMin = x2 + descent;
	xMax = x2 + ascent;
	yMin = y2;
	yMax = y2 + h1;
	break;
      case 2:
	xMin = x2 + w1;
	xMax = x2;
	yMin = y2 + descent;
	yMax = y2 + ascent;
	break;
      case 3:
	xMin = x2 - ascent;
	xMax = x2 - descent;
	yMin = y2 + h1;
	yMax = y2;
	break;
      }
      if ((state->getRender() & 3) == 1) {
	state->getStrokeRGB(&rgb);
      } else {
	state->getFillRGB(&rgb);
      }
      if (rtl) {
	j = uLen - 1 - i;
      } else {
	j = i;
      }
      chars->append(new TextChar(u[j], charPos, nBytes, xMin, yMin, xMax, yMax,
				 curRot, clipped,
				 state->getRender() == 3,
				 curFont, curFontSize,
				 colToDbl(rgb.r), colToDbl(rgb.g),
				 colToDbl(rgb.b)));
    }
  }

  charPos += nBytes;
}

void TextPage::incCharCount(int nChars) {
  charPos += nChars;
}

void TextPage::beginActualText(GfxState *state, Unicode *u, int uLen) {
  if (actualText) {
    gfree(actualText);
  }
  actualText = (Unicode *)gmallocn(uLen, sizeof(Unicode));
  memcpy(actualText, u, uLen * sizeof(Unicode));
  actualTextLen = uLen;
  actualTextNBytes = 0;
}

void TextPage::endActualText(GfxState *state) {
  Unicode *u;

  u = actualText;
  actualText = NULL;  // so we can call TextPage::addChar()
  if (actualTextNBytes) {
    // now that we have the position info for all of the text inside
    // the marked content span, we feed the "ActualText" back through
    // addChar()
    addChar(state, actualTextX0, actualTextY0,
	    actualTextX1 - actualTextX0, actualTextY1 - actualTextY0,
	    0, actualTextNBytes, u, actualTextLen);
  }
  gfree(u);
  actualText = NULL;
  actualTextLen = 0;
  actualTextNBytes = gFalse;
}

void TextPage::addUnderline(double x0, double y0, double x1, double y1) {
  underlines->append(new TextUnderline(x0, y0, x1, y1));
}

void TextPage::addLink(double xMin, double yMin, double xMax, double yMax,
		       Link *link) {
  GString *uri;

  if (link && link->getAction() && link->getAction()->getKind() == actionURI) {
    uri = ((LinkURI *)link->getAction())->getURI()->copy();
    links->append(new TextLink(xMin, yMin, xMax, yMax, uri));
  }
}

//------------------------------------------------------------------------
// TextPage: output
//------------------------------------------------------------------------

void TextPage::write(void *outputStream, TextOutputFunc outputFunc) {
  UnicodeMap *uMap;
  char space[8], eol[16], eop[8];
  int spaceLen, eolLen, eopLen;
  GBool pageBreaks;

  // get the output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    return;
  }
  spaceLen = uMap->mapUnicode(0x20, space, sizeof(space));
  eolLen = 0; // make gcc happy
  switch (globalParams->getTextEOL()) {
  case eolUnix:
    eolLen = uMap->mapUnicode(0x0a, eol, sizeof(eol));
    break;
  case eolDOS:
    eolLen = uMap->mapUnicode(0x0d, eol, sizeof(eol));
    eolLen += uMap->mapUnicode(0x0a, eol + eolLen, sizeof(eol) - eolLen);
    break;
  case eolMac:
    eolLen = uMap->mapUnicode(0x0d, eol, sizeof(eol));
    break;
  }
  eopLen = uMap->mapUnicode(0x0c, eop, sizeof(eop));
  pageBreaks = globalParams->getTextPageBreaks();

  switch (control.mode) {
  case textOutReadingOrder:
    writeReadingOrder(outputStream, outputFunc, uMap, space, spaceLen,
		      eol, eolLen);
    break;
  case textOutPhysLayout:
  case textOutTableLayout:
    writePhysLayout(outputStream, outputFunc, uMap, space, spaceLen,
		    eol, eolLen);
    break;
  case textOutLinePrinter:
    writeLinePrinter(outputStream, outputFunc, uMap, space, spaceLen,
		     eol, eolLen);
    break;
  case textOutRawOrder:
    writeRaw(outputStream, outputFunc, uMap, space, spaceLen,
	     eol, eolLen);
    break;
  }

  // end of page
  if (pageBreaks) {
    (*outputFunc)(outputStream, eop, eopLen);
  }

  uMap->decRefCnt();
}

void TextPage::writeReadingOrder(void *outputStream,
				 TextOutputFunc outputFunc,
				 UnicodeMap *uMap,
				 char *space, int spaceLen,
				 char *eol, int eolLen) {
  TextBlock *tree;
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  GList *columns;
  GBool primaryLR;
  GString *s;
  int colIdx, parIdx, lineIdx, rot, n;

  rot = rotateChars(chars);
  primaryLR = checkPrimaryLR(chars);
  tree = splitChars(chars);
#if 0 //~debug
  dumpTree(tree);
#endif
  if (!tree) {
    // no text
    unrotateChars(chars, rot);
    return;
  }
  columns = buildColumns(tree);
  delete tree;
  unrotateChars(chars, rot);
  if (control.html) {
    rotateUnderlinesAndLinks(rot);
    generateUnderlinesAndLinks(columns);
  }
#if 0 //~debug
  dumpColumns(columns);
#endif

  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    for (parIdx = 0; parIdx < col->paragraphs->getLength(); ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      for (lineIdx = 0; lineIdx < par->lines->getLength(); ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	n = line->len;
	if (line->hyphenated && lineIdx + 1 < par->lines->getLength()) {
	  --n;
	}
	s = new GString();
	encodeFragment(line->text, n, uMap, primaryLR, s);
	if (lineIdx + 1 < par->lines->getLength() && !line->hyphenated) {
	  s->append(space, spaceLen);
	}
	(*outputFunc)(outputStream, s->getCString(), s->getLength());
	delete s;
      }
      (*outputFunc)(outputStream, eol, eolLen);
    }
    (*outputFunc)(outputStream, eol, eolLen);
  }

  deleteGList(columns, TextColumn);
}

GList *TextPage::makeColumns() {
  TextBlock *tree;
  GList *columns;

  tree = splitChars(chars);
  if (!tree) {
    // no text
    return new GList();
  }
  columns = buildColumns(tree);
  delete tree;
  if (control.html) {
    generateUnderlinesAndLinks(columns);
  }
  return columns;
}

// This handles both physical layout and table layout modes.
void TextPage::writePhysLayout(void *outputStream,
			       TextOutputFunc outputFunc,
			       UnicodeMap *uMap,
			       char *space, int spaceLen,
			       char *eol, int eolLen) {
  TextBlock *tree;
  GString **out;
  int *outLen;
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  GList *columns;
  GBool primaryLR;
  int ph, colIdx, parIdx, lineIdx, rot, y, i;

#if 0 //~debug
  dumpChars(chars);
#endif
  rot = rotateChars(chars);
  primaryLR = checkPrimaryLR(chars);
  tree = splitChars(chars);
#if 0 //~debug
  dumpTree(tree);
#endif
  if (!tree) {
    // no text
    unrotateChars(chars, rot);
    return;
  }
  columns = buildColumns(tree);
  delete tree;
  unrotateChars(chars, rot);
  if (control.html) {
    rotateUnderlinesAndLinks(rot);
    generateUnderlinesAndLinks(columns);
  }
  ph = assignPhysLayoutPositions(columns);
#if 0 //~debug
  dumpColumns(columns);
#endif

  out = (GString **)gmallocn(ph, sizeof(GString *));
  outLen = (int *)gmallocn(ph, sizeof(int));
  for (i = 0; i < ph; ++i) {
    out[i] = NULL;
    outLen[i] = 0;
  }

  columns->sort(&TextColumn::cmpPX);
  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    y = col->py;
    for (parIdx = 0;
	 parIdx < col->paragraphs->getLength() && y < ph;
	 ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      for (lineIdx = 0;
	   lineIdx < par->lines->getLength() && y < ph;
	   ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	if (!out[y]) {
	  out[y] = new GString();
	}
	while (outLen[y] < col->px + line->px) {
	  out[y]->append(space, spaceLen);
	  ++outLen[y];
	}
	encodeFragment(line->text, line->len, uMap, primaryLR, out[y]);
	outLen[y] += line->pw;
	++y;
      }
      if (parIdx + 1 < col->paragraphs->getLength()) {
	++y;
      }
    }
  }

  for (i = 0; i < ph; ++i) {
    if (out[i]) {
      (*outputFunc)(outputStream, out[i]->getCString(), out[i]->getLength());
      delete out[i];
    }
    (*outputFunc)(outputStream, eol, eolLen);
  }

  gfree(out);
  gfree(outLen);

  deleteGList(columns, TextColumn);
}

void TextPage::writeLinePrinter(void *outputStream,
				TextOutputFunc outputFunc,
				UnicodeMap *uMap,
				char *space, int spaceLen,
				char *eol, int eolLen) {
  TextChar *ch, *ch2;
  GList *line;
  GString *s;
  char buf[8];
  double pitch, lineSpacing, delta;
  double yMin0, yShift, xMin0, xShift;
  double y, x;
  int rot, n, i, j, k;

  rot = rotateChars(chars);
  chars->sort(&TextChar::cmpX);
  removeDuplicates(chars, 0);
  chars->sort(&TextChar::cmpY);

  // get character pitch
  if (control.fixedPitch > 0) {
    pitch = control.fixedPitch;
  } else {
    // compute (approximate) character pitch
    pitch = pageWidth;
    for (i = 0; i < chars->getLength(); ++i) {
      ch = (TextChar *)chars->get(i);
      for (j = i + 1; j < chars->getLength(); ++j) {
	ch2 = (TextChar *)chars->get(j);
	if (ch2->yMin + ascentAdjustFactor * (ch2->yMax - ch2->yMin) <
	      ch->yMax - descentAdjustFactor * (ch->yMax - ch->yMin) &&
	    ch->yMin + ascentAdjustFactor * (ch->yMax - ch->yMin) <
	      ch2->yMax - descentAdjustFactor * (ch2->yMax - ch2->yMin)) {
	  delta = fabs(ch2->xMin - ch->xMin);
	  if (delta > 0 && delta < pitch) {
	    pitch = delta;
	  }
	}
      }
    }
  }

  // get line spacing
  if (control.fixedLineSpacing > 0) {
    lineSpacing = control.fixedLineSpacing;
  } else {
    // compute (approximate) line spacing
    lineSpacing = pageHeight;
    i = 0;
    while (i < chars->getLength()) {
      ch = (TextChar *)chars->get(i);
      // look for the first char that does not (substantially)
      // vertically overlap this one
      delta = 0;
      for (++i; delta == 0 && i < chars->getLength(); ++i) {
	ch2 = (TextChar *)chars->get(i);
	if (ch2->yMin + ascentAdjustFactor * (ch2->yMax - ch2->yMin) >
	    ch->yMax - descentAdjustFactor * (ch->yMax - ch->yMin)) {
	  delta = ch2->yMin - ch->yMin;
	}
      }
      if (delta > 0 && delta < lineSpacing) {
	lineSpacing = delta;
      }
    }
  }

  // shift the grid to avoid problems with floating point accuracy --
  // for fixed line spacing, this avoids problems with
  // dropping/inserting blank lines
  if (chars->getLength()) {
    yMin0 = ((TextChar *)chars->get(0))->yMin;
    yShift = yMin0 - (int)(yMin0 / lineSpacing + 0.5) * lineSpacing
             - 0.5 * lineSpacing;
  } else {
    yShift = 0;
  }

  // for each line...
  i = 0;
  j = chars->getLength() - 1;
  for (y = yShift; y < pageHeight; y += lineSpacing) {

    // get the characters in this line
    line = new GList;
    while (i < chars->getLength() &&
	   ((TextChar *)chars->get(i))->yMin < y + lineSpacing) {
      line->append(chars->get(i++));
    }
    line->sort(&TextChar::cmpX);

    // shift the grid to avoid problems with floating point accuracy
    // -- for fixed char spacing, this avoids problems with
    // dropping/inserting spaces
    if (line->getLength()) {
      xMin0 = ((TextChar *)line->get(0))->xMin;
      xShift = xMin0 - (int)(xMin0 / pitch + 0.5) * pitch - 0.5 * pitch;
    } else {
      xShift = 0;
    }

    // write the line
    s = new GString();
    x = xShift;
    k = 0;
    while (k < line->getLength()) {
      ch = (TextChar *)line->get(k);
      if (ch->xMin < x + pitch) {
	n = uMap->mapUnicode(ch->c, buf, sizeof(buf));
	s->append(buf, n);
	++k;
      } else {
	s->append(space, spaceLen);
	n = spaceLen;
      }
      x += (uMap->isUnicode() ? 1 : n) * pitch;
    }
    s->append(eol, eolLen);
    (*outputFunc)(outputStream, s->getCString(), s->getLength());
    delete s;
    delete line;
  }

  unrotateChars(chars, rot);
}

void TextPage::writeRaw(void *outputStream,
			TextOutputFunc outputFunc,
			UnicodeMap *uMap,
			char *space, int spaceLen,
			char *eol, int eolLen) {
  TextChar *ch, *ch2;
  GString *s;
  char buf[8];
  int n, i;

  s = new GString();

  for (i = 0; i < chars->getLength(); ++i) {

    // process one char
    ch = (TextChar *)chars->get(i);
    n = uMap->mapUnicode(ch->c, buf, sizeof(buf));
    s->append(buf, n);

    // check for space or eol
    if (i+1 < chars->getLength()) {
      ch2 = (TextChar *)chars->get(i+1);
      if (ch2->rot != ch->rot) {
	s->append(eol, eolLen);
      } else {
	switch (ch->rot) {
	case 0:
	default:
	  if (fabs(ch2->yMin - ch->yMin) > rawModeLineDelta * ch->fontSize ||
	      ch2->xMin - ch->xMax < -rawModeCharOverlap * ch->fontSize) {
	    s->append(eol, eolLen);
	  } else if (ch2->xMin - ch->xMax >
		     rawModeWordSpacing * ch->fontSize) {
	    s->append(space, spaceLen);
	  }
	  break;
	case 1:
	  if (fabs(ch->xMax - ch2->xMax) > rawModeLineDelta * ch->fontSize ||
	      ch2->yMin - ch->yMax < -rawModeCharOverlap * ch->fontSize) {
	    s->append(eol, eolLen);
	  } else if (ch2->yMin - ch->yMax >
		     rawModeWordSpacing * ch->fontSize) {
	    s->append(space, spaceLen);
	  }
	  break;
	case 2:
	  if (fabs(ch->yMax - ch2->yMax) > rawModeLineDelta * ch->fontSize ||
	      ch->xMin - ch2->xMax  < -rawModeCharOverlap * ch->fontSize) {
	    s->append(eol, eolLen);
	  } else if (ch->xMin - ch2->xMax >
		     rawModeWordSpacing * ch->fontSize) {
	    s->append(space, spaceLen);
	  }
	  break;
	case 3:
	  if (fabs(ch2->xMin - ch->xMin) > rawModeLineDelta * ch->fontSize ||
	      ch->yMin - ch2->yMax  < -rawModeCharOverlap * ch->fontSize) {
	    s->append(eol, eolLen);
	  } else if (ch->yMin - ch2->yMax >
		     rawModeWordSpacing * ch->fontSize) {
	    s->append(space, spaceLen);
	  }
	  break;
	}
      }
    } else {
      s->append(eol, eolLen);
    }

    if (s->getLength() > 1000) {
      (*outputFunc)(outputStream, s->getCString(), s->getLength());
      s->clear();
    }
  }

  if (s->getLength() > 0) {
    (*outputFunc)(outputStream, s->getCString(), s->getLength());
  }
  delete s;
}

void TextPage::encodeFragment(Unicode *text, int len, UnicodeMap *uMap,
			      GBool primaryLR, GString *s) {
  char lre[8], rle[8], popdf[8], buf[8];
  int lreLen, rleLen, popdfLen, n;
  int i, j, k;

  if (uMap->isUnicode()) {

    lreLen = uMap->mapUnicode(0x202a, lre, sizeof(lre));
    rleLen = uMap->mapUnicode(0x202b, rle, sizeof(rle));
    popdfLen = uMap->mapUnicode(0x202c, popdf, sizeof(popdf));

    if (primaryLR) {

      i = 0;
      while (i < len) {
	// output a left-to-right section
	for (j = i; j < len && !unicodeTypeR(text[j]); ++j) ;
	for (k = i; k < j; ++k) {
	  n = uMap->mapUnicode(text[k], buf, sizeof(buf));
	  s->append(buf, n);
	}
	i = j;
	// output a right-to-left section
	for (j = i;
	     j < len && !(unicodeTypeL(text[j]) || unicodeTypeNum(text[j]));
	     ++j) ;
	if (j > i) {
	  s->append(rle, rleLen);
	  for (k = j - 1; k >= i; --k) {
	    n = uMap->mapUnicode(text[k], buf, sizeof(buf));
	    s->append(buf, n);
	  }
	  s->append(popdf, popdfLen);
	  i = j;
	}
      }

    } else {

      // Note: This code treats numeric characters (European and
      // Arabic/Indic) as left-to-right, which isn't strictly correct
      // (incurs extra LRE/POPDF pairs), but does produce correct
      // visual formatting.
      s->append(rle, rleLen);
      i = len - 1;
      while (i >= 0) {
	// output a right-to-left section
	for (j = i;
	     j >= 0 && !(unicodeTypeL(text[j]) || unicodeTypeNum(text[j]));
	     --j) ;
	for (k = i; k > j; --k) {
	  n = uMap->mapUnicode(text[k], buf, sizeof(buf));
	  s->append(buf, n);
	}
	i = j;
	// output a left-to-right section
	for (j = i; j >= 0 && !unicodeTypeR(text[j]); --j) ;
	if (j < i) {
	  s->append(lre, lreLen);
	  for (k = j + 1; k <= i; ++k) {
	    n = uMap->mapUnicode(text[k], buf, sizeof(buf));
	    s->append(buf, n);
	  }
	  s->append(popdf, popdfLen);
	  i = j;
	}
      }
      s->append(popdf, popdfLen);
    }

  } else {
    for (i = 0; i < len; ++i) {
      n = uMap->mapUnicode(text[i], buf, sizeof(buf));
      s->append(buf, n);
    }
  }
}

//------------------------------------------------------------------------
// TextPage: layout analysis
//------------------------------------------------------------------------

// Determine primary (most common) rotation value.  Rotate all chars
// to that primary rotation.
int TextPage::rotateChars(GList *charsA) {
  TextChar *ch;
  int nChars[4];
  double xMin, yMin, xMax, yMax, t;
  int rot, i;

  // determine primary rotation
  nChars[0] = nChars[1] = nChars[2] = nChars[3] = 0;
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    ++nChars[ch->rot];
  }
  rot = 0;
  for (i = 1; i < 4; ++i) {
    if (nChars[i] > nChars[rot]) {
      rot = i;
    }
  }

  // rotate
  switch (rot) {
  case 0:
  default:
    break;
  case 1:
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xMin = ch->yMin;
      xMax = ch->yMax;
      yMin = pageWidth - ch->xMax;
      yMax = pageWidth - ch->xMin;
      ch->xMin = xMin;
      ch->xMax = xMax;
      ch->yMin = yMin;
      ch->yMax = yMax;
      ch->rot = (ch->rot + 3) & 3;
    }
    t = pageWidth;
    pageWidth = pageHeight;
    pageHeight = t;
    break;
  case 2:
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xMin = pageWidth - ch->xMax;
      xMax = pageWidth - ch->xMin;
      yMin = pageHeight - ch->yMax;
      yMax = pageHeight - ch->yMin;
      ch->xMin = xMin;
      ch->xMax = xMax;
      ch->yMin = yMin;
      ch->yMax = yMax;
      ch->rot = (ch->rot + 2) & 3;
    }
    break;
  case 3:
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xMin = pageHeight - ch->yMax;
      xMax = pageHeight - ch->yMin;
      yMin = ch->xMin;
      yMax = ch->xMax;
      ch->xMin = xMin;
      ch->xMax = xMax;
      ch->yMin = yMin;
      ch->yMax = yMax;
      ch->rot = (ch->rot + 1) & 3;
    }
    t = pageWidth;
    pageWidth = pageHeight;
    pageHeight = t;
    break;
  }

  return rot;
}

// Rotate the TextUnderlines and TextLinks to match the transform
// performed by rotateChars().
void TextPage::rotateUnderlinesAndLinks(int rot) {
  TextUnderline *underline;
  TextLink *link;
  double xMin, yMin, xMax, yMax;
  int i;

  switch (rot) {
  case 0:
  default:
    break;
  case 1:
    for (i = 0; i < underlines->getLength(); ++i) {
      underline = (TextUnderline *)underlines->get(i);
      xMin = underline->y0;
      xMax = underline->y1;
      yMin = pageWidth - underline->x1;
      yMax = pageWidth - underline->x0;
      underline->x0 = xMin;
      underline->x1 = xMax;
      underline->y0 = yMin;
      underline->y1 = yMax;
      underline->horiz = !underline->horiz;
    }
    for (i = 0; i < links->getLength(); ++i) {
      link = (TextLink *)links->get(i);
      xMin = link->yMin;
      xMax = link->yMax;
      yMin = pageWidth - link->xMax;
      yMax = pageWidth - link->xMin;
      link->xMin = xMin;
      link->xMax = xMax;
      link->yMin = yMin;
      link->yMax = yMax;
    }
    break;
  case 2:
    for (i = 0; i < underlines->getLength(); ++i) {
      underline = (TextUnderline *)underlines->get(i);
      xMin = pageWidth - underline->x1;
      xMax = pageWidth - underline->x0;
      yMin = pageHeight - underline->y1;
      yMax = pageHeight - underline->y0;
      underline->x0 = xMin;
      underline->x1 = xMax;
      underline->y0 = yMin;
      underline->y1 = yMax;
    }
    for (i = 0; i < links->getLength(); ++i) {
      link = (TextLink *)links->get(i);
      xMin = pageWidth - link->xMax;
      xMax = pageWidth - link->xMin;
      yMin = pageHeight - link->yMax;
      yMax = pageHeight - link->yMin;
      link->xMin = xMin;
      link->xMax = xMax;
      link->yMin = yMin;
      link->yMax = yMax;
    }
    break;
  case 3:
    for (i = 0; i < underlines->getLength(); ++i) {
      underline = (TextUnderline *)underlines->get(i);
      xMin = pageHeight - underline->y1;
      xMax = pageHeight - underline->y0;
      yMin = underline->x0;
      yMax = underline->x1;
      underline->x0 = xMin;
      underline->x1 = xMax;
      underline->y0 = yMin;
      underline->y1 = yMax;
      underline->horiz = !underline->horiz;
    }
    for (i = 0; i < links->getLength(); ++i) {
      link = (TextLink *)links->get(i);
      xMin = pageHeight - link->yMax;
      xMax = pageHeight - link->yMin;
      yMin = link->xMin;
      yMax = link->xMax;
      link->xMin = xMin;
      link->xMax = xMax;
      link->yMin = yMin;
      link->yMax = yMax;
    }
    break;
  }
}

// Undo the coordinate transform performed by rotateChars().
void TextPage::unrotateChars(GList *charsA, int rot) {
  TextChar *ch;
  double xMin, yMin, xMax, yMax, t;
  int i;

  switch (rot) {
  case 0:
  default:
    // no transform
    break;
  case 1:
    t = pageWidth;
    pageWidth = pageHeight;
    pageHeight = t;
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xMin = pageWidth - ch->yMax;
      xMax = pageWidth - ch->yMin;
      yMin = ch->xMin;
      yMax = ch->xMax;
      ch->xMin = xMin;
      ch->xMax = xMax;
      ch->yMin = yMin;
      ch->yMax = yMax;
      ch->rot = (ch->rot + 1) & 3;
    }
    break;
  case 2:
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xMin = pageWidth - ch->xMax;
      xMax = pageWidth - ch->xMin;
      yMin = pageHeight - ch->yMax;
      yMax = pageHeight - ch->yMin;
      ch->xMin = xMin;
      ch->xMax = xMax;
      ch->yMin = yMin;
      ch->yMax = yMax;
      ch->rot = (ch->rot + 2) & 3;
    }
    break;
  case 3:
    t = pageWidth;
    pageWidth = pageHeight;
    pageHeight = t;
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xMin = ch->yMin;
      xMax = ch->yMax;
      yMin = pageHeight - ch->xMax;
      yMax = pageHeight - ch->xMin;
      ch->xMin = xMin;
      ch->xMax = xMax;
      ch->yMin = yMin;
      ch->yMax = yMax;
      ch->rot = (ch->rot + 3) & 3;
    }
    break;
  }
}

// Undo the coordinate transform performed by rotateChars().
void TextPage::unrotateColumns(GList *columns, int rot) {
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  TextWord *word;
  double xMin, yMin, xMax, yMax, t;
  int colIdx, parIdx, lineIdx, wordIdx, i;

  switch (rot) {
  case 0:
  default:
    // no transform
    break;
  case 1:
    t = pageWidth;
    pageWidth = pageHeight;
    pageHeight = t;
    for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
      col = (TextColumn *)columns->get(colIdx);
      xMin = pageWidth - col->yMax;
      xMax = pageWidth - col->yMin;
      yMin = col->xMin;
      yMax = col->xMax;
      col->xMin = xMin;
      col->xMax = xMax;
      col->yMin = yMin;
      col->yMax = yMax;
      for (parIdx = 0;
	   parIdx < col->paragraphs->getLength();
	   ++parIdx) {
	par = (TextParagraph *)col->paragraphs->get(parIdx);
	xMin = pageWidth - par->yMax;
	xMax = pageWidth - par->yMin;
	yMin = par->xMin;
	yMax = par->xMax;
	par->xMin = xMin;
	par->xMax = xMax;
	par->yMin = yMin;
	par->yMax = yMax;
	for (lineIdx = 0;
	     lineIdx < par->lines->getLength();
	     ++lineIdx) {
	  line = (TextLine *)par->lines->get(lineIdx);
	  xMin = pageWidth - line->yMax;
	  xMax = pageWidth - line->yMin;
	  yMin = line->xMin;
	  yMax = line->xMax;
	  line->xMin = xMin;
	  line->xMax = xMax;
	  line->yMin = yMin;
	  line->yMax = yMax;
	  line->rot = (line->rot + 1) & 3;
	  for (wordIdx = 0; wordIdx < line->words->getLength(); ++wordIdx) {
	    word = (TextWord *)line->words->get(wordIdx);
	    xMin = pageWidth - word->yMax;
	    xMax = pageWidth - word->yMin;
	    yMin = word->xMin;
	    yMax = word->xMax;
	    word->xMin = xMin;
	    word->xMax = xMax;
	    word->yMin = yMin;
	    word->yMax = yMax;
	    word->rot = (word->rot + 1) & 3;
	  }
	}
      }
    }
    break;
  case 2:
    for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
      col = (TextColumn *)columns->get(colIdx);
      xMin = pageWidth - col->xMax;
      xMax = pageWidth - col->xMin;
      yMin = pageHeight - col->yMax;
      yMax = pageHeight - col->yMin;
      col->xMin = xMin;
      col->xMax = xMax;
      col->yMin = yMin;
      col->yMax = yMax;
      for (parIdx = 0;
	   parIdx < col->paragraphs->getLength();
	   ++parIdx) {
	par = (TextParagraph *)col->paragraphs->get(parIdx);
	xMin = pageWidth - par->xMax;
	xMax = pageWidth - par->xMin;
	yMin = pageHeight - par->yMax;
	yMax = pageHeight - par->yMin;
	par->xMin = xMin;
	par->xMax = xMax;
	par->yMin = yMin;
	par->yMax = yMax;
	for (lineIdx = 0;
	     lineIdx < par->lines->getLength();
	     ++lineIdx) {
	  line = (TextLine *)par->lines->get(lineIdx);
	  xMin = pageWidth - line->xMax;
	  xMax = pageWidth - line->xMin;
	  yMin = pageHeight - line->yMax;
	  yMax = pageHeight - line->yMin;
	  line->xMin = xMin;
	  line->xMax = xMax;
	  line->yMin = yMin;
	  line->yMax = yMax;
	  line->rot = (line->rot + 2) & 3;
	  for (i = 0; i <= line->len; ++i) {
	    line->edge[i] = pageWidth - line->edge[i];
	  }
	  for (wordIdx = 0; wordIdx < line->words->getLength(); ++wordIdx) {
	    word = (TextWord *)line->words->get(wordIdx);
	    xMin = pageWidth - word->xMax;
	    xMax = pageWidth - word->xMin;
	    yMin = pageHeight - word->yMax;
	    yMax = pageHeight - word->yMin;
	    word->xMin = xMin;
	    word->xMax = xMax;
	    word->yMin = yMin;
	    word->yMax = yMax;
	    word->rot = (word->rot + 2) & 3;
	    for (i = 0; i <= word->len; ++i) {
	      word->edge[i] = pageWidth - word->edge[i];
	    }
	  }
	}
      }
    }
    break;
  case 3:
    t = pageWidth;
    pageWidth = pageHeight;
    pageHeight = t;
    for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
      col = (TextColumn *)columns->get(colIdx);
      xMin = col->yMin;
      xMax = col->yMax;
      yMin = pageHeight - col->xMax;
      yMax = pageHeight - col->xMin;
      col->xMin = xMin;
      col->xMax = xMax;
      col->yMin = yMin;
      col->yMax = yMax;
      for (parIdx = 0;
	   parIdx < col->paragraphs->getLength();
	   ++parIdx) {
	par = (TextParagraph *)col->paragraphs->get(parIdx);
	xMin = par->yMin;
	xMax = par->yMax;
	yMin = pageHeight - par->xMax;
	yMax = pageHeight - par->xMin;
	par->xMin = xMin;
	par->xMax = xMax;
	par->yMin = yMin;
	par->yMax = yMax;
	for (lineIdx = 0;
	     lineIdx < par->lines->getLength();
	     ++lineIdx) {
	  line = (TextLine *)par->lines->get(lineIdx);
	  xMin = line->yMin;
	  xMax = line->yMax;
	  yMin = pageHeight - line->xMax;
	  yMax = pageHeight - line->xMin;
	  line->xMin = xMin;
	  line->xMax = xMax;
	  line->yMin = yMin;
	  line->yMax = yMax;
	  line->rot = (line->rot + 3) & 3;
	  for (i = 0; i <= line->len; ++i) {
	    line->edge[i] = pageHeight - line->edge[i];
	  }
	  for (wordIdx = 0; wordIdx < line->words->getLength(); ++wordIdx) {
	    word = (TextWord *)line->words->get(wordIdx);
	    xMin = word->yMin;
	    xMax = word->yMax;
	    yMin = pageHeight - word->xMax;
	    yMax = pageHeight - word->xMin;
	    word->xMin = xMin;
	    word->xMax = xMax;
	    word->yMin = yMin;
	    word->yMax = yMax;
	    word->rot = (word->rot + 3) & 3;
	    for (i = 0; i <= word->len; ++i) {
	      word->edge[i] = pageHeight - word->edge[i];
	    }
	  }
	}
      }
    }
    break;
  }
}

void TextPage::unrotateWords(GList *words, int rot) {
  TextWord *word;
  double xMin, yMin, xMax, yMax;
  int i, j;

  switch (rot) {
  case 0:
  default:
    // no transform
    break;
  case 1:
    for (i = 0; i < words->getLength(); ++i) {
      word = (TextWord *)words->get(i);
      xMin = pageWidth - word->yMax;
      xMax = pageWidth - word->yMin;
      yMin = word->xMin;
      yMax = word->xMax;
      word->xMin = xMin;
      word->xMax = xMax;
      word->yMin = yMin;
      word->yMax = yMax;
      word->rot = (word->rot + 1) & 3;
    }
    break;
  case 2:
    for (i = 0; i < words->getLength(); ++i) {
      word = (TextWord *)words->get(i);
      xMin = pageWidth - word->xMax;
      xMax = pageWidth - word->xMin;
      yMin = pageHeight - word->yMax;
      yMax = pageHeight - word->yMin;
      word->xMin = xMin;
      word->xMax = xMax;
      word->yMin = yMin;
      word->yMax = yMax;
      word->rot = (word->rot + 2) & 3;
      for (j = 0; j <= word->len; ++j) {
	word->edge[j] = pageWidth - word->edge[j];
      }
    }
    break;
  case 3:
    for (i = 0; i < words->getLength(); ++i) {
      word = (TextWord *)words->get(i);
      xMin = word->yMin;
      xMax = word->yMax;
      yMin = pageHeight - word->xMax;
      yMax = pageHeight - word->xMin;
      word->xMin = xMin;
      word->xMax = xMax;
      word->yMin = yMin;
      word->yMax = yMax;
      word->rot = (word->rot + 3) & 3;
      for (j = 0; j <= word->len; ++j) {
	word->edge[j] = pageHeight - word->edge[j];
      }
    }
    break;
  }
}

// Determine the primary text direction (LR vs RL).  Returns true for
// LR, false for RL.
GBool TextPage::checkPrimaryLR(GList *charsA) {
  TextChar *ch;
  int i, lrCount;

  lrCount = 0;
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    if (unicodeTypeL(ch->c)) {
      ++lrCount;
    } else if (unicodeTypeR(ch->c)) {
      --lrCount;
    }
  }
  return lrCount >= 0;
}

// Remove duplicate characters.  The list of chars has been sorted --
// by x for rot=0,2; by y for rot=1,3.
void TextPage::removeDuplicates(GList *charsA, int rot) {
  TextChar *ch, *ch2;
  double xDelta, yDelta;
  int i, j;

  if (rot & 1) {
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xDelta = dupMaxSecDelta * ch->fontSize;
      yDelta = dupMaxPriDelta * ch->fontSize;
      j = i + 1;
      while (j < charsA->getLength()) {
	ch2 = (TextChar *)charsA->get(j);
	if (ch2->yMin - ch->yMin >= yDelta) {
	  break;
	}
	if (ch2->c == ch->c &&
	    fabs(ch2->xMin - ch->xMin) < xDelta &&
	    fabs(ch2->xMax - ch->xMax) < xDelta &&
	    fabs(ch2->yMax - ch->yMax) < yDelta) {
	  charsA->del(j);
	} else {
	  ++j;
	}
      }
    }
  } else {
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      xDelta = dupMaxPriDelta * ch->fontSize;
      yDelta = dupMaxSecDelta * ch->fontSize;
      j = i + 1;
      while (j < charsA->getLength()) {
	ch2 = (TextChar *)charsA->get(j);
	if (ch2->xMin - ch->xMin >= xDelta) {
	  break;
	}
	if (ch2->c == ch->c &&
	    fabs(ch2->xMax - ch->xMax) < xDelta &&
	    fabs(ch2->yMin - ch->yMin) < yDelta &&
	    fabs(ch2->yMax - ch->yMax) < yDelta) {
	  charsA->del(j);
	} else {
	  ++j;
	}
      }
    }
  }
}

// Split the characters into trees of TextBlocks, one tree for each
// rotation.  Merge into a single tree (with the primary rotation).
TextBlock *TextPage::splitChars(GList *charsA) {
  TextBlock *tree[4];
  TextBlock *blk;
  GList *chars2, *clippedChars;
  TextChar *ch;
  int rot, i;

  // split: build a tree of TextBlocks for each rotation
  clippedChars = new GList();
  for (rot = 0; rot < 4; ++rot) {
    chars2 = new GList();
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      if (ch->rot == rot) {
	chars2->append(ch);
      }
    }
    tree[rot] = NULL;
    if (chars2->getLength() > 0) {
      chars2->sort((rot & 1) ? &TextChar::cmpY : &TextChar::cmpX);
      removeDuplicates(chars2, rot);
      if (control.clipText) {
	i = 0;
	while (i < chars2->getLength()) {
	  ch = (TextChar *)chars2->get(i);
	  if (ch->clipped) {
	    ch = (TextChar *)chars2->del(i);
	    clippedChars->append(ch);
	  } else {
	    ++i;
	  }
	}
      }
      if (chars2->getLength() > 0) {
	tree[rot] = split(chars2, rot);
      }
    }
    delete chars2;
  }

  // if the page contains no (unclipped) text, just leave an empty
  // column list
  if (!tree[0]) {
    delete clippedChars;
    return NULL;
  }

  // if the main tree is not a multicolumn node, insert one so that
  // rotated text has somewhere to go
  if (tree[0]->tag != blkTagMulticolumn) {
    blk = new TextBlock(blkHorizSplit, 0);
    blk->addChild(tree[0]);
    blk->tag = blkTagMulticolumn;
    tree[0] = blk;
  }

  // merge non-primary-rotation text into the primary-rotation tree
  for (rot = 1; rot < 4; ++rot) {
    if (tree[rot]) {
      insertIntoTree(tree[rot], tree[0]);
      tree[rot] = NULL;
    }
  }

  if (clippedChars->getLength()) {
    insertClippedChars(clippedChars, tree[0]);
  }
  delete clippedChars;

#if 0 //~debug
  dumpTree(tree[0]);
#endif

  return tree[0];
}

// Generate a tree of TextBlocks, marked as columns, lines, and words.
TextBlock *TextPage::split(GList *charsA, int rot) {
  TextBlock *blk;
  GList *chars2, *chars3;
  int *horizProfile, *vertProfile;
  double xMin, yMin, xMax, yMax;
  int xMinI, yMinI, xMaxI, yMaxI;
  int xMinI2, yMinI2, xMaxI2, yMaxI2;
  TextChar *ch;
  double minFontSize, avgFontSize, splitPrecision;
  double nLines, vertGapThreshold, ascentAdjust, descentAdjust, minChunk;
  int horizGapSize, vertGapSize;
  double horizGapSize2, vertGapSize2;
  int minHorizChunkWidth, minVertChunkWidth, nHorizGaps, nVertGaps;
  double largeCharSize;
  int nLargeChars;
  GBool doHorizSplit, doVertSplit, smallSplit;
  int i, x, y, prev, start;

  //----- compute bbox, min font size, average font size, and
  //      split precision for this block

  xMin = yMin = xMax = yMax = 0; // make gcc happy
  minFontSize = avgFontSize = 0;
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    if (i == 0 || ch->xMin < xMin) {
      xMin = ch->xMin;
    }
    if (i == 0 || ch->yMin < yMin) {
      yMin = ch->yMin;
    }
    if (i == 0 || ch->xMax > xMax) {
      xMax = ch->xMax;
    }
    if (i == 0 || ch->yMax > yMax) {
      yMax = ch->yMax;
    }
    avgFontSize += ch->fontSize;
    if (i == 0 || ch->fontSize < minFontSize) {
      minFontSize = ch->fontSize;
    }
  }
  avgFontSize /= charsA->getLength();
  splitPrecision = splitPrecisionMul * minFontSize;
  if (splitPrecision < minSplitPrecision) {
    splitPrecision = minSplitPrecision;
  }

  //----- compute the horizontal and vertical profiles

  // add some slack to the array bounds to avoid floating point
  // precision problems
  xMinI = (int)floor(xMin / splitPrecision) - 1;
  yMinI = (int)floor(yMin / splitPrecision) - 1;
  xMaxI = (int)floor(xMax / splitPrecision) + 1;
  yMaxI = (int)floor(yMax / splitPrecision) + 1;
  horizProfile = (int *)gmallocn(yMaxI - yMinI + 1, sizeof(int));
  vertProfile = (int *)gmallocn(xMaxI - xMinI + 1, sizeof(int));
  memset(horizProfile, 0, (yMaxI - yMinI + 1) * sizeof(int));
  memset(vertProfile, 0, (xMaxI - xMinI + 1) * sizeof(int));
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    // yMinI2 and yMaxI2 are adjusted to allow for slightly overlapping lines
    switch (rot) {
    case 0:
    default:
      xMinI2 = (int)floor(ch->xMin / splitPrecision);
      xMaxI2 = (int)floor(ch->xMax / splitPrecision);
      ascentAdjust = ascentAdjustFactor * (ch->yMax - ch->yMin);
      yMinI2 = (int)floor((ch->yMin + ascentAdjust) / splitPrecision);
      descentAdjust = descentAdjustFactor * (ch->yMax - ch->yMin);
      yMaxI2 = (int)floor((ch->yMax - descentAdjust) / splitPrecision);
      break;
    case 1:
      descentAdjust = descentAdjustFactor * (ch->xMax - ch->xMin);
      xMinI2 = (int)floor((ch->xMin + descentAdjust) / splitPrecision);
      ascentAdjust = ascentAdjustFactor * (ch->xMax - ch->xMin);
      xMaxI2 = (int)floor((ch->xMax - ascentAdjust) / splitPrecision);
      yMinI2 = (int)floor(ch->yMin / splitPrecision);
      yMaxI2 = (int)floor(ch->yMax / splitPrecision);
      break;
    case 2:
      xMinI2 = (int)floor(ch->xMin / splitPrecision);
      xMaxI2 = (int)floor(ch->xMax / splitPrecision);
      descentAdjust = descentAdjustFactor * (ch->yMax - ch->yMin);
      yMinI2 = (int)floor((ch->yMin + descentAdjust) / splitPrecision);
      ascentAdjust = ascentAdjustFactor * (ch->yMax - ch->yMin);
      yMaxI2 = (int)floor((ch->yMax - ascentAdjust) / splitPrecision);
      break;
    case 3:
      ascentAdjust = ascentAdjustFactor * (ch->xMax - ch->xMin);
      xMinI2 = (int)floor((ch->xMin + ascentAdjust) / splitPrecision);
      descentAdjust = descentAdjustFactor * (ch->xMax - ch->xMin);
      xMaxI2 = (int)floor((ch->xMax - descentAdjust) / splitPrecision);
      yMinI2 = (int)floor(ch->yMin / splitPrecision);
      yMaxI2 = (int)floor(ch->yMax / splitPrecision);
      break;
    }
    for (y = yMinI2; y <= yMaxI2; ++y) {
      ++horizProfile[y - yMinI];
    }
    for (x = xMinI2; x <= xMaxI2; ++x) {
      ++vertProfile[x - xMinI];
    }
  }

  //----- find the largest gaps in the horizontal and vertical profiles

  horizGapSize = 0;
  for (start = yMinI; start < yMaxI && !horizProfile[start - yMinI]; ++start) ;
  for (y = start; y < yMaxI; ++y) {
    if (horizProfile[y - yMinI] && !horizProfile[y + 1 - yMinI]) {
      start = y;
    } else if (!horizProfile[y - yMinI] && horizProfile[y + 1 - yMinI]) {
      if (y - start > horizGapSize) {
	horizGapSize = y - start;
      }
    }
  }
  vertGapSize = 0;
  for (start = xMinI; start < xMaxI && !vertProfile[start - xMinI]; ++start) ;
  for (x = start; x < xMaxI; ++x) {
    if (vertProfile[x - xMinI] && !vertProfile[x + 1 - xMinI]) {
      start = x;
    } else if (!vertProfile[x - xMinI] && vertProfile[x + 1 - xMinI]) {
      if (x - start > vertGapSize) {
	vertGapSize = x - start;
      }
    }
  }
  horizGapSize2 = horizGapSize - splitGapSlack * avgFontSize / splitPrecision;
  if (horizGapSize2 < 0.99) {
    horizGapSize2 = 0.99;
  }
  vertGapSize2 = vertGapSize - splitGapSlack * avgFontSize / splitPrecision;
  if (vertGapSize2 < 0.99) {
    vertGapSize2 = 0.99;
  }

  //----- count horiz/vert gaps equivalent to largest gaps

  minHorizChunkWidth = yMaxI - yMinI;
  nHorizGaps = 0;
  for (start = yMinI; start < yMaxI && !horizProfile[start - yMinI]; ++start) ;
  prev = start - 1;
  for (y = start; y < yMaxI; ++y) {
    if (horizProfile[y - yMinI] && !horizProfile[y + 1 - yMinI]) {
      start = y;
    } else if (!horizProfile[y - yMinI] && horizProfile[y + 1 - yMinI]) {
      if (y - start > horizGapSize2) {
	++nHorizGaps;
	if (start - prev < minHorizChunkWidth) {
	  minHorizChunkWidth = start - prev;
	}
	prev = y;
      }
    }
  }
  minVertChunkWidth = xMaxI - xMinI;
  nVertGaps = 0;
  for (start = xMinI; start < xMaxI && !vertProfile[start - xMinI]; ++start) ;
  prev = start - 1;
  for (x = start; x < xMaxI; ++x) {
    if (vertProfile[x - xMinI] && !vertProfile[x + 1 - xMinI]) {
      start = x;
    } else if (!vertProfile[x - xMinI] && vertProfile[x + 1 - xMinI]) {
      if (x - start > vertGapSize2) {
	++nVertGaps;
	if (start - prev < minVertChunkWidth) {
	  minVertChunkWidth = start - prev;
	}
	prev = x;
      }
    }
  }

  //----- compute splitting parameters

  // approximation of number of lines in block
  if (fabs(avgFontSize) < 0.001) {
    nLines = 1;
  } else if (rot & 1) {
    nLines = (xMax - xMin) / avgFontSize;
  } else {
    nLines = (yMax - yMin) / avgFontSize;
  }

  // compute the minimum allowed vertical gap size
  // (this is a horizontal gap threshold for rot=1,3
  if (control.mode == textOutTableLayout) {
    vertGapThreshold = vertGapThresholdTableMax
                       + vertGapThresholdTableSlope * nLines;
    if (vertGapThreshold < vertGapThresholdTableMin) {
      vertGapThreshold = vertGapThresholdTableMin;
    }
  } else {
    vertGapThreshold = vertGapThresholdMax + vertGapThresholdSlope * nLines;
    if (vertGapThreshold < vertGapThresholdMin) {
      vertGapThreshold = vertGapThresholdMin;
    }
  }
  vertGapThreshold = vertGapThreshold * avgFontSize / splitPrecision;

  // compute the minimum allowed chunk width
  if (control.mode == textOutTableLayout) {
    minChunk = 0;
  } else {
    minChunk = vertSplitChunkThreshold * avgFontSize / splitPrecision;
  }

  // look for large chars
  // -- this kludge (multiply by 256, convert to int, divide by 256.0)
  //    prevents floating point stability issues on x86 with gcc, where
  //    largeCharSize could otherwise have slightly different values
  //    here and where it's used below to do the large char partition
  //    (because it gets truncated from 80 to 64 bits when spilled)
  largeCharSize = (int)(largeCharThreshold * avgFontSize * 256) / 256.0;
  nLargeChars = 0;
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    if (ch->fontSize > largeCharSize) {
      ++nLargeChars;
    }
  }

  // figure out which type of split to do
  doHorizSplit = doVertSplit = gFalse;
  smallSplit = gFalse;
  if (rot & 1) {
    if (nHorizGaps > 0 &&
	(horizGapSize > vertGapSize || control.mode == textOutTableLayout) &&
	horizGapSize > vertGapThreshold &&
	minHorizChunkWidth > minChunk) {
      doHorizSplit = gTrue;
    } else if (nVertGaps > 0) {
      doVertSplit = gTrue;
    } else if (nLargeChars == 0 && nHorizGaps > 0) {
      doHorizSplit = gTrue;
      smallSplit = gTrue;
    }
  } else {
    if (nVertGaps > 0 &&
	(vertGapSize > horizGapSize || control.mode == textOutTableLayout) &&
	vertGapSize > vertGapThreshold &&
	minVertChunkWidth > minChunk) {
      doVertSplit = gTrue;
    } else if (nHorizGaps > 0) {
      doHorizSplit = gTrue;
    } else if (nLargeChars == 0 && nVertGaps > 0) {
      doVertSplit = gTrue;
      smallSplit = gTrue;
    }
  }

  //----- split the block

  //~ this could use "other content" (vector graphics, rotated text) --
  //~ presence of other content in a gap means we should definitely split

  // split vertically
  if (doVertSplit) {
    blk = new TextBlock(blkVertSplit, rot);
    blk->smallSplit = smallSplit;
    for (start = xMinI; start < xMaxI && !vertProfile[start - xMinI]; ++start) ;
    prev = start - 1;
    for (x = start; x < xMaxI; ++x) {
      if (vertProfile[x - xMinI] && !vertProfile[x + 1 - xMinI]) {
	start = x;
      } else if (!vertProfile[x - xMinI] && vertProfile[x + 1 - xMinI]) {
	if (x - start > vertGapSize2) {
	  chars2 = getChars(charsA, (prev + 0.5) * splitPrecision, yMin - 1,
			    (start + 1.5) * splitPrecision, yMax + 1);
	  blk->addChild(split(chars2, rot));
	  delete chars2;
	  prev = x;
	}
      }
    }
    chars2 = getChars(charsA, (prev + 0.5) * splitPrecision, yMin - 1,
		      xMax + 1, yMax + 1);
    blk->addChild(split(chars2, rot));
    delete chars2;

  // split horizontally
  } else if (doHorizSplit) {
    blk = new TextBlock(blkHorizSplit, rot);
    blk->smallSplit = smallSplit;
    for (start = yMinI;
	 start < yMaxI && !horizProfile[start - yMinI];
	 ++start) ;
    prev = start - 1;
    for (y = start; y < yMaxI; ++y) {
      if (horizProfile[y - yMinI] && !horizProfile[y + 1 - yMinI]) {
	start = y;
      } else if (!horizProfile[y - yMinI] && horizProfile[y + 1 - yMinI]) {
	if (y - start > horizGapSize2) {
	  chars2 = getChars(charsA, xMin - 1, (prev + 0.5) * splitPrecision,
			    xMax + 1, (start + 1.5) * splitPrecision);
	  blk->addChild(split(chars2, rot));
	  delete chars2;
	  prev = y;
	}
      }
    }
    chars2 = getChars(charsA, xMin - 1, (prev + 0.5) * splitPrecision,
		      xMax + 1, yMax + 1);
    blk->addChild(split(chars2, rot));
    delete chars2;

  // split into larger and smaller chars
  } else if (nLargeChars > 0) {
    chars2 = new GList();
    chars3 = new GList();
    for (i = 0; i < charsA->getLength(); ++i) {
      ch = (TextChar *)charsA->get(i);
      if (ch->fontSize > largeCharSize) {
	chars2->append(ch);
      } else {
	chars3->append(ch);
      }
    }
    blk = split(chars3, rot);
    insertLargeChars(chars2, blk);
    delete chars2;
    delete chars3;

  // create a leaf node
  } else {
    blk = new TextBlock(blkLeaf, rot);
    for (i = 0; i < charsA->getLength(); ++i) {
      blk->addChild((TextChar *)charsA->get(i));
    }
  }

  gfree(horizProfile);
  gfree(vertProfile);

  tagBlock(blk);

  return blk;
}

// Return the subset of chars inside a rectangle.
GList *TextPage::getChars(GList *charsA, double xMin, double yMin,
			  double xMax, double yMax) {
  GList *ret;
  TextChar *ch;
  double x, y;
  int i;

  ret = new GList();
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    // because of {ascent,descent}AdjustFactor, the y coords (or x
    // coords for rot 1,3) for the gaps will be a little bit tight --
    // so we use the center of the character here
    x = 0.5 * (ch->xMin + ch->xMax);
    y = 0.5 * (ch->yMin + ch->yMax);
    if (x > xMin && x < xMax && y > yMin && y < yMax) {
      ret->append(ch);
    }
  }
  return ret;
}

// Decide whether this block is a line, column, or multiple columns:
// - all leaf nodes are lines
// - horiz split nodes whose children are lines or columns are columns
// - other horiz split nodes are multiple columns
// - vert split nodes, with small gaps, whose children are lines are lines
// - other vert split nodes are multiple columns
// (for rot=1,3: the horiz and vert splits are swapped)
// In table layout mode:
// - all leaf nodes are lines
// - vert split nodes, with small gaps, whose children are lines are lines
// - everything else is multiple columns
void TextPage::tagBlock(TextBlock *blk) {
  TextBlock *child;
  int i;

  if (control.mode == textOutTableLayout) {
    if (blk->type == blkLeaf) {
      blk->tag = blkTagLine;
    } else if (blk->type == ((blk->rot & 1) ? blkHorizSplit : blkVertSplit) &&
	       blk->smallSplit) {
      blk->tag = blkTagLine;
      for (i = 0; i < blk->children->getLength(); ++i) {
	child = (TextBlock *)blk->children->get(i);
	if (child->tag != blkTagLine) {
	  blk->tag = blkTagMulticolumn;
	  break;
	}
      }
    } else {
      blk->tag = blkTagMulticolumn;
    }
    return;
  }

  if (blk->type == blkLeaf) {
    blk->tag = blkTagLine;

  } else {
    if (blk->type == ((blk->rot & 1) ? blkVertSplit : blkHorizSplit)) {
      blk->tag = blkTagColumn;
      for (i = 0; i < blk->children->getLength(); ++i) {
	child = (TextBlock *)blk->children->get(i);
	if (child->tag != blkTagColumn && child->tag != blkTagLine) {
	  blk->tag = blkTagMulticolumn;
	  break;
	}
      }
    } else {
      if (blk->smallSplit) {
	blk->tag = blkTagLine;
	for (i = 0; i < blk->children->getLength(); ++i) {
	  child = (TextBlock *)blk->children->get(i);
	  if (child->tag != blkTagLine) {
	    blk->tag = blkTagMulticolumn;
	    break;
	  }
	}
      } else {
	blk->tag = blkTagMulticolumn;
      }
    }
  }
}

// Insert a list of large characters into a tree.
void TextPage::insertLargeChars(GList *largeChars, TextBlock *blk) {
  TextChar *ch, *ch2;
  GBool singleLine;
  double xLimit, yLimit, minOverlap;
  int i;

  //~ this currently works only for characters in the primary rotation

  // check to see if the large chars are a single line, in the
  // upper-left corner of blk (this is just a rough estimate)
  xLimit = blk->xMin + 0.5 * (blk->xMin + blk->xMax);
  yLimit = blk->yMin + 0.5 * (blk->yMin + blk->yMax);
  singleLine = gTrue;
  // note: largeChars are already sorted by x
  for (i = 0; i < largeChars->getLength(); ++i) {
    ch2 = (TextChar *)largeChars->get(i);
    if (ch2->xMax > xLimit || ch2->yMax > yLimit) {
      singleLine = gFalse;
      break;
    }
    if (i > 0) {
      ch = (TextChar *)largeChars->get(i-1);
      minOverlap = 0.5 * (ch->fontSize < ch2->fontSize ? ch->fontSize
			                               : ch2->fontSize);
      if (ch->yMax - ch2->yMin < minOverlap ||
	  ch2->yMax - ch->yMin < minOverlap) {
	singleLine = gFalse;
	break;
      }
    }
  }

  if (singleLine) {
    // if the large chars are a single line, prepend them to the first
    // leaf node in blk
    insertLargeCharsInFirstLeaf(largeChars, blk);
  } else {
    // if the large chars are not a single line, prepend each one to
    // the appropriate leaf node -- this handles cases like bullets
    // drawn in a large font, on the left edge of a column
    for (i = largeChars->getLength() - 1; i >= 0; --i) {
      ch = (TextChar *)largeChars->get(i);
      insertLargeCharInLeaf(ch, blk);
    }
  }
}

// Find the first leaf (in depth-first order) in blk, and prepend a
// list of large chars.
void TextPage::insertLargeCharsInFirstLeaf(GList *largeChars, TextBlock *blk) {
  TextChar *ch;
  int i;

  if (blk->type == blkLeaf) {
    for (i = largeChars->getLength() - 1; i >= 0; --i) {
      ch = (TextChar *)largeChars->get(i);
      blk->prependChild(ch);
    }
  } else {
    insertLargeCharsInFirstLeaf(largeChars, (TextBlock *)blk->children->get(0));
    blk->updateBounds(0);
  }
}

// Find the leaf in <blk> where large char <ch> belongs, and prepend
// it.
void TextPage::insertLargeCharInLeaf(TextChar *ch, TextBlock *blk) {
  TextBlock *child;
  double y;
  int i;

  //~ this currently works only for characters in the primary rotation

  //~ this currently just looks down the left edge of blk
  //~   -- it could be extended to do more

  // estimate the baseline of ch
  y = ch->yMin + 0.75 * (ch->yMax - ch->yMin);

  if (blk->type == blkLeaf) {
    blk->prependChild(ch);
  } else if (blk->type == blkHorizSplit) {
    for (i = 0; i < blk->children->getLength(); ++i) {
      child = (TextBlock *)blk->children->get(i);
      if (y < child->yMax || i == blk->children->getLength() - 1) {
	insertLargeCharInLeaf(ch, child);
	blk->updateBounds(i);
	break;
      }
    }
  } else {
    insertLargeCharInLeaf(ch, (TextBlock *)blk->children->get(0));
    blk->updateBounds(0);
  }
}

// Merge blk (rot != 0) into primaryTree (rot == 0).
void TextPage::insertIntoTree(TextBlock *blk, TextBlock *primaryTree) {
  TextBlock *child;

  // we insert a whole column at a time - so call insertIntoTree
  // recursively until we get to a column (or line)

  if (blk->tag == blkTagMulticolumn) {
    while (blk->children->getLength()) {
      child = (TextBlock *)blk->children->del(0);
      insertIntoTree(child, primaryTree);
    }
    delete blk;
  } else {
    insertColumnIntoTree(blk, primaryTree);
  }
}

// Insert a column (as an atomic subtree) into tree.
// Requirement: tree is not a leaf node.
void TextPage::insertColumnIntoTree(TextBlock *column, TextBlock *tree) {
  TextBlock *child;
  int i;

  for (i = 0; i < tree->children->getLength(); ++i) {
    child = (TextBlock *)tree->children->get(i);
    if (child->tag == blkTagMulticolumn &&
	column->xMin >= child->xMin &&
	column->yMin >= child->yMin &&
	column->xMax <= child->xMax &&
	column->yMax <= child->yMax) {
      insertColumnIntoTree(column, child);
      tree->tag = blkTagMulticolumn;
      return;
    }
  }

  if (tree->type == blkVertSplit) {
    if (tree->rot == 1 || tree->rot == 2) {
      for (i = 0; i < tree->children->getLength(); ++i) {
	child = (TextBlock *)tree->children->get(i);
	if (column->xMax > 0.5 * (child->xMin + child->xMax)) {
	  break;
	}
      }
    } else {
      for (i = 0; i < tree->children->getLength(); ++i) {
	child = (TextBlock *)tree->children->get(i);
	if (column->xMin < 0.5 * (child->xMin + child->xMax)) {
	  break;
	}
      }
    }
  } else if (tree->type == blkHorizSplit) {
    if (tree->rot >= 2) {
      for (i = 0; i < tree->children->getLength(); ++i) {
	child = (TextBlock *)tree->children->get(i);
	if (column->yMax > 0.5 * (child->yMin + child->yMax)) {
	  break;
	}
      }
    } else {
      for (i = 0; i < tree->children->getLength(); ++i) {
	child = (TextBlock *)tree->children->get(i);
	if (column->yMin < 0.5 * (child->yMin + child->yMax)) {
	  break;
	}
      }
    }
  } else {
    // this should never happen
    return;
  }
  tree->children->insert(i, column);
  tree->tag = blkTagMulticolumn;
}

// Insert clipped characters back into the TextBlock tree.
void TextPage::insertClippedChars(GList *clippedChars, TextBlock *tree) {
  TextChar *ch, *ch2;
  TextBlock *leaf;
  double y;
  int i;

  //~ this currently works only for characters in the primary rotation

  clippedChars->sort(TextChar::cmpX);
  while (clippedChars->getLength()) {
    ch = (TextChar *)clippedChars->del(0);
    if (ch->rot != 0) {
      continue;
    }
    if (!(leaf = findClippedCharLeaf(ch, tree))) {
      continue;
    }
    leaf->addChild(ch);
    i = 0;
    while (i < clippedChars->getLength()) {
      ch2 = (TextChar *)clippedChars->get(i);
      if (ch2->xMin > ch->xMax + clippedTextMaxWordSpace * ch->fontSize) {
	break;
      }
      y = 0.5 * (ch2->yMin + ch2->yMax);
      if (y > leaf->yMin && y < leaf->yMax) {
	ch2 = (TextChar *)clippedChars->del(i);
	leaf->addChild(ch2);
	ch = ch2;
      } else {
	++i;
      }
    }
  }
}

// Find the leaf in <tree> to which clipped char <ch> can be appended.
// Returns NULL if there is no appropriate append point.
TextBlock *TextPage::findClippedCharLeaf(TextChar *ch, TextBlock *tree) {
  TextBlock *ret, *child;
  double y;
  int i;

  //~ this currently works only for characters in the primary rotation

  y = 0.5 * (ch->yMin + ch->yMax);
  if (tree->type == blkLeaf) {
    if (tree->rot == 0) {
      if (y > tree->yMin && y < tree->yMax &&
	  ch->xMin <= tree->xMax + clippedTextMaxWordSpace * ch->fontSize) {
	return tree;
      }
    }
  } else {
    for (i = 0; i < tree->children->getLength(); ++i) {
      child = (TextBlock *)tree->children->get(i);
      if ((ret = findClippedCharLeaf(ch, child))) {
	return ret;
      }
    }
  }
  return NULL;
}

// Convert the tree of TextBlocks into a list of TextColumns.
GList *TextPage::buildColumns(TextBlock *tree) {
  GList *columns;

  columns = new GList();
  buildColumns2(tree, columns);
  return columns;
}

void TextPage::buildColumns2(TextBlock *blk, GList *columns) {
  TextColumn *col;
  int i;

  switch (blk->tag) {
  case blkTagLine:
  case blkTagColumn:
    col = buildColumn(blk);
    columns->append(col);
    break;
  case blkTagMulticolumn:
    for (i = 0; i < blk->children->getLength(); ++i) {
      buildColumns2((TextBlock *)blk->children->get(i), columns);
    }
    break;
  }
}

TextColumn *TextPage::buildColumn(TextBlock *blk) {
  GList *lines, *parLines;
  GList *paragraphs;
  TextLine *line0, *line1;
  double spaceThresh, indent0, indent1, fontSize0, fontSize1;
  int i;

  lines = new GList();
  buildLines(blk, lines);

  spaceThresh = paragraphSpacingThreshold * getAverageLineSpacing(lines);

  //~ could look for bulleted lists here: look for the case where
  //~   all out-dented lines start with the same char

  // build the paragraphs
  paragraphs = new GList();
  i = 0;
  while (i < lines->getLength()) {

    // get the first line of the paragraph
    parLines = new GList();
    line0 = (TextLine *)lines->get(i);
    parLines->append(line0);
    ++i;

    if (i < lines->getLength()) {
      line1 = (TextLine *)lines->get(i);
      indent0 = getLineIndent(line0, blk);
      indent1 = getLineIndent(line1, blk);
      fontSize0 = line0->fontSize;
      fontSize1 = line1->fontSize;

      // inverted indent
      if (indent1 - indent0 > minParagraphIndent * fontSize0 &&
	  fabs(fontSize0 - fontSize1) <= paragraphFontSizeDelta &&
	  getLineSpacing(line0, line1) <= spaceThresh) {
	parLines->append(line1);
	indent0 = indent1;
	for (++i; i < lines->getLength(); ++i) {
	  line1 = (TextLine *)lines->get(i);
	  indent1 = getLineIndent(line1, blk);
	  fontSize1 = line1->fontSize;
	  if (indent0 - indent1 > minParagraphIndent * fontSize0) {
	    break;
	  }
	  if (fabs(fontSize0 - fontSize1) > paragraphFontSizeDelta) {
	    break;
	  }
	  if (getLineSpacing((TextLine *)lines->get(i - 1), line1)
	        > spaceThresh) {
	    break;
	  }
	  parLines->append(line1);
	}

      // drop cap
      } else if (fontSize0 > largeCharThreshold * fontSize1 &&
		 indent1 - indent0 > minParagraphIndent * fontSize1 &&
		 getLineSpacing(line0, line1) < 0) {
	parLines->append(line1);
	fontSize0 = fontSize1;
	for (++i; i < lines->getLength(); ++i) {
	  line1 = (TextLine *)lines->get(i);
	  indent1 = getLineIndent(line1, blk);
	  if (indent1 - indent0 <= minParagraphIndent * fontSize0) {
	    break;
	  }
	  if (getLineSpacing((TextLine *)lines->get(i - 1), line1)
	        > spaceThresh) {
	    break;
	  }
	  parLines->append(line1);
	}
	for (; i < lines->getLength(); ++i) {
	  line1 = (TextLine *)lines->get(i);
	  indent1 = getLineIndent(line1, blk);
	  fontSize1 = line1->fontSize;
	  if (indent1 - indent0 > minParagraphIndent * fontSize0) {
	    break;
	  }
	  if (fabs(fontSize0 - fontSize1) > paragraphFontSizeDelta) {
	    break;
	  }
	  if (getLineSpacing((TextLine *)lines->get(i - 1), line1)
	        > spaceThresh) {
	    break;
	  }
	  parLines->append(line1);
	}

      // regular indent or no indent
      } else if (fabs(fontSize0 - fontSize1) <= paragraphFontSizeDelta &&
		 getLineSpacing(line0, line1) <= spaceThresh) {
	parLines->append(line1);
	indent0 = indent1;
	for (++i; i < lines->getLength(); ++i) {
	  line1 = (TextLine *)lines->get(i);
	  indent1 = getLineIndent(line1, blk);
	  fontSize1 = line1->fontSize;
	  if (indent1 - indent0 > minParagraphIndent * fontSize0) {
	    break;
	  }
	  if (fabs(fontSize0 - fontSize1) > paragraphFontSizeDelta) {
	    break;
	  }
	  if (getLineSpacing((TextLine *)lines->get(i - 1), line1)
	        > spaceThresh) {
	    break;
	  }
	  parLines->append(line1);
	}
      }
    }

    paragraphs->append(new TextParagraph(parLines));
  }

  delete lines;

  return new TextColumn(paragraphs, blk->xMin, blk->yMin,
			blk->xMax, blk->yMax);
}

double TextPage::getLineIndent(TextLine *line, TextBlock *blk) {
  double indent;

  switch (line->rot) {
  case 0:
  default: indent = line->xMin - blk->xMin;  break;
  case 1:  indent = line->yMin - blk->yMin;  break;
  case 2:  indent = blk->xMax  - line->xMax; break;
  case 3:  indent = blk->yMax  - line->yMax; break;
  }
  return indent;
}

// Compute average line spacing in column.
double TextPage::getAverageLineSpacing(GList *lines) {
  double avg, sp;
  int n, i;

  avg = 0;
  n = 0;
  for (i = 1; i < lines->getLength(); ++i) {
    sp = getLineSpacing((TextLine *)lines->get(i - 1),
			(TextLine *)lines->get(i));
    if (sp > 0) {
      avg += sp;
      ++n;
    }
  }
  if (n > 0) {
    avg /= n;
  }
  return avg;
}

// Compute the space between two lines.
double TextPage::getLineSpacing(TextLine *line0, TextLine *line1) {
  double sp;

  switch (line0->rot) {
  case 0:
  default: sp = line1->yMin - line0->yMax; break;
  case 1:  sp = line0->xMin - line1->xMax; break;
  case 2:  sp = line0->yMin - line1->yMin; break;
  case 3:  sp = line1->xMin - line1->xMax; break;
  }
  return sp;
}

void TextPage::buildLines(TextBlock *blk, GList *lines) {
  TextLine *line;
  int i;

  switch (blk->tag) {
  case blkTagLine:
    line = buildLine(blk);
    if (blk->rot == 1 || blk->rot == 2) {
      lines->insert(0, line);
    } else {
      lines->append(line);
    }
    break;
  case blkTagColumn:
  case blkTagMulticolumn: // multicolumn should never happen here
    for (i = 0; i < blk->children->getLength(); ++i) {
      buildLines((TextBlock *)blk->children->get(i), lines);
    }
    break;
  }
}

TextLine *TextPage::buildLine(TextBlock *blk) {
  GList *charsA;
  GList *words;
  TextChar *ch, *ch2;
  TextWord *word;
  double wordSp, lineFontSize, sp;
  GBool spaceAfter, spaceAfter2;
  int i, j;

  charsA = new GList();
  getLineChars(blk, charsA);

  wordSp = computeWordSpacingThreshold(charsA, blk->rot);

  words = new GList();
  lineFontSize = 0;
  spaceAfter = gFalse;
  i = 0;
  while (i < charsA->getLength()) {
    sp = wordSp - 1;
    for (j = i+1; j < charsA->getLength(); ++j) {
      ch = (TextChar *)charsA->get(j-1);
      ch2 = (TextChar *)charsA->get(j);
      sp = (blk->rot & 1) ? (ch2->yMin - ch->yMax) : (ch2->xMin - ch->xMax);
      if (sp > wordSp ||
	  ch->font != ch2->font ||
	  fabs(ch->fontSize - ch2->fontSize) > 0.01 ||
	  (control.mode == textOutRawOrder &&
	   ch2->charPos != ch->charPos + ch->charLen)) {
	break;
      }
      sp = wordSp - 1;
    }
    spaceAfter2 = spaceAfter;
    spaceAfter = sp > wordSp;
    word = new TextWord(charsA, i, j - i, blk->rot,
			(blk->rot >= 2) ? spaceAfter2 : spaceAfter);
    i = j;
    if (blk->rot >= 2) {
      words->insert(0, word);
    } else {
      words->append(word);
    }
    if (i == 0 || word->fontSize > lineFontSize) {
      lineFontSize = word->fontSize;
    }
  }

  delete charsA;

  return new TextLine(words, blk->xMin, blk->yMin, blk->xMax, blk->yMax,
		      lineFontSize);
}

void TextPage::getLineChars(TextBlock *blk, GList *charsA) {
  int i;

  if (blk->type == blkLeaf) {
    charsA->append(blk->children);
  } else {
    for (i = 0; i < blk->children->getLength(); ++i) {
      getLineChars((TextBlock *)blk->children->get(i), charsA);
    }
  }
}

// Compute the inter-word spacing threshold for a line of chars.
// Spaces greater than this threshold will be considered inter-word
// spaces.
double TextPage::computeWordSpacingThreshold(GList *charsA, int rot) {
  TextChar *ch, *ch2;
  double avgFontSize, minSp, maxSp, sp;
  int i;

  avgFontSize = 0;
  minSp = maxSp = 0;
  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    avgFontSize += ch->fontSize;
    if (i < charsA->getLength() - 1) {
      ch2 = (TextChar *)charsA->get(i+1);
      sp = (rot & 1) ? (ch2->yMin - ch->yMax) : (ch2->xMin - ch->xMax);
      if (i == 0 || sp < minSp) {
	minSp = sp;
      }
      if (sp > maxSp) {
	maxSp = sp;
      }
    }
  }
  avgFontSize /= charsA->getLength();
  if (minSp < 0) {
    minSp = 0;
  }

  // if spacing is completely uniform, assume it's a single word
  // (technically it could be either "ABC" or "A B C", but it's
  // essentially impossible to tell)
  if (maxSp - minSp < uniformSpacing * avgFontSize) {
    return maxSp + 1;

  // if there is some variation in spacing, but it's small, assume
  // there are some inter-word spaces
  } else if (maxSp - minSp < wordSpacing * avgFontSize) {
    return 0.5 * (minSp + maxSp);

  // otherwise, assume a reasonable threshold for inter-word spacing
  // (we can't use something like 0.5*(minSp+maxSp) here because there
  // can be outliers at the high end)
  } else {
    return minSp + wordSpacing * avgFontSize;
  }
}

int TextPage::assignPhysLayoutPositions(GList *columns) {
  assignLinePhysPositions(columns);
  return assignColumnPhysPositions(columns);
}

// Assign a physical x coordinate for each TextLine (relative to the
// containing TextColumn).  This also computes TextColumn width and
// height.
void TextPage::assignLinePhysPositions(GList *columns) {
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  UnicodeMap *uMap;
  int colIdx, parIdx, lineIdx;

  if (!(uMap = globalParams->getTextEncoding())) {
    return;
  }

  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    col->pw = col->ph = 0;
    for (parIdx = 0; parIdx < col->paragraphs->getLength(); ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      for (lineIdx = 0; lineIdx < par->lines->getLength(); ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	computeLinePhysWidth(line, uMap);
	if (control.fixedPitch > 0) {
	  line->px = (int)((line->xMin - col->xMin) / control.fixedPitch);
	} else if (fabs(line->fontSize) < 0.001) {
	  line->px = 0;
	} else {
	  line->px = (int)((line->xMin - col->xMin) /
			   (physLayoutSpaceWidth * line->fontSize));
	}
	if (line->px + line->pw > col->pw) {
	  col->pw = line->px + line->pw;
	}
      }
      col->ph += par->lines->getLength();
    }
    col->ph += col->paragraphs->getLength() - 1;
  }

  uMap->decRefCnt();
}

void TextPage::computeLinePhysWidth(TextLine *line, UnicodeMap *uMap) {
  char buf[8];
  int n, i;

  if (uMap->isUnicode()) {
    line->pw = line->len;
  } else {
    line->pw = 0;
    for (i = 0; i < line->len; ++i) {
      n = uMap->mapUnicode(line->text[i], buf, sizeof(buf));
      line->pw += n;
    }
  }
}

// Assign physical x and y coordinates for each TextColumn.  Returns
// the text height (max physical y + 1).
int TextPage::assignColumnPhysPositions(GList *columns) {
  TextColumn *col, *col2;
  double slack, xOverlap, yOverlap;
  int ph, i, j;

  if (control.mode == textOutTableLayout) {
    slack = tableCellOverlapSlack;
  } else {
    slack = 0;
  }

  // assign x positions
  columns->sort(&TextColumn::cmpX);
  for (i = 0; i < columns->getLength(); ++i) {
    col = (TextColumn *)columns->get(i);
    if (control.fixedPitch) {
      col->px = (int)(col->xMin / control.fixedPitch);
    } else {
      col->px = 0;
      for (j = 0; j < i; ++j) {
	col2 = (TextColumn *)columns->get(j);
	xOverlap = col2->xMax - col->xMin;
	if (xOverlap < slack * (col2->xMax - col2->xMin)) {
	  if (col2->px + col2->pw + 2 > col->px) {
	    col->px = col2->px + col2->pw + 2;
	  }
	} else {
	  yOverlap = (col->yMax < col2->yMax ? col->yMax : col2->yMax) -
	             (col->yMin > col2->yMin ? col->yMin : col2->yMin);
	  if (yOverlap > 0 && xOverlap < yOverlap) {
	    if (col2->px + col2->pw > col->px) {
	      col->px = col2->px + col2->pw;
	    }
	  } else {
	    if (col2->px > col->px) {
	      col->px = col2->px;
	    }
	  }
	}
      }
    }
  }

  // assign y positions
  ph = 0;
  columns->sort(&TextColumn::cmpY);
  for (i = 0; i < columns->getLength(); ++i) {
    col = (TextColumn *)columns->get(i);
    col->py = 0;
    for (j = 0; j < i; ++j) {
      col2 = (TextColumn *)columns->get(j);
      yOverlap = col2->yMax - col->yMin;
      if (yOverlap < slack * (col2->yMax - col2->yMin)) {
	if (col2->py + col2->ph + 1 > col->py) {
	  col->py = col2->py + col2->ph + 1;
	}
      } else {
	xOverlap = (col->xMax < col2->xMax ? col->xMax : col2->xMax) -
	           (col->xMin > col2->xMin ? col->xMin : col2->xMin);
	if (xOverlap > 0 && yOverlap < xOverlap) {
	  if (col2->py + col2->ph > col->py) {
	    col->py = col2->py + col2->ph;
	  }
	} else {
	  if (col2->py > col->py) {
	    col->py = col2->py;
	  }
	}
      }
    }
    if (col->py + col->ph > ph) {
      ph = col->py + col->ph;
    }
  }

  return ph;
}

void TextPage::generateUnderlinesAndLinks(GList *columns) {
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  TextWord *word;
  TextUnderline *underline;
  TextLink *link;
  double base, uSlack, ubSlack, hSlack;
  int colIdx, parIdx, lineIdx, wordIdx, i;

  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    for (parIdx = 0; parIdx < col->paragraphs->getLength(); ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      for (lineIdx = 0; lineIdx < par->lines->getLength(); ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	for (wordIdx = 0; wordIdx < line->words->getLength(); ++wordIdx) {
	  word = (TextWord *)line->words->get(wordIdx);
	  base = word->getBaseline();
	  uSlack = underlineSlack * word->fontSize;
	  ubSlack = underlineBaselineSlack * word->fontSize;
	  hSlack = hyperlinkSlack * word->fontSize;

	  //----- handle underlining
	  for (i = 0; i < underlines->getLength(); ++i) {
	    underline = (TextUnderline *)underlines->get(i);
	    if (underline->horiz) {
	      if (word->rot == 0 || word->rot == 2) {
		if (fabs(underline->y0 - base) < ubSlack &&
		    underline->x0 < word->xMin + uSlack &&
		    word->xMax - uSlack < underline->x1) {
		  word->underlined = gTrue;
		}
	      }
	    } else {
	      if (word->rot == 1 || word->rot == 3) {
		if (fabs(underline->x0 - base) < ubSlack &&
		    underline->y0 < word->yMin + uSlack &&
		    word->yMax - uSlack < underline->y1) {
		  word->underlined = gTrue;
		}
	      }
	    }
	  }

	  //----- handle links
	  for (i = 0; i < links->getLength(); ++i) {
	    link = (TextLink *)links->get(i);
	    if (link->xMin < word->xMin + hSlack &&
		word->xMax - hSlack < link->xMax &&
		link->yMin < word->yMin + hSlack &&
		word->yMax - hSlack < link->yMax) {
	      word->link = link;
	    }
	  }
	}
      }
    }
  }
}

//------------------------------------------------------------------------
// TextPage: access
//------------------------------------------------------------------------

GBool TextPage::findText(Unicode *s, int len,
			 GBool startAtTop, GBool stopAtBottom,
			 GBool startAtLast, GBool stopAtLast,
			 GBool caseSensitive, GBool backward,
			 GBool wholeWord,
			 double *xMin, double *yMin,
			 double *xMax, double *yMax) {
  TextBlock *tree;
  TextColumn *column;
  TextParagraph *par;
  TextLine *line;
  Unicode *s2, *txt;
  Unicode *p;
  double xStart, yStart, xStop, yStop;
  double xMin0, yMin0, xMax0, yMax0;
  double xMin1, yMin1, xMax1, yMax1;
  GBool found;
  int txtSize, m, rot, colIdx, parIdx, lineIdx, i, j, k;

  //~ need to handle right-to-left text

  if (!findCols) {
    rot = rotateChars(chars);
    if ((tree = splitChars(chars))) {
      findCols = buildColumns(tree);
      delete tree;
    } else {
      // no text
      findCols = new GList();
    }
    unrotateChars(chars, rot);
    unrotateColumns(findCols, rot);
  }

  // convert the search string to uppercase
  if (!caseSensitive) {
    s2 = (Unicode *)gmallocn(len, sizeof(Unicode));
    for (i = 0; i < len; ++i) {
      s2[i] = unicodeToUpper(s[i]);
    }
  } else {
    s2 = s;
  }

  txt = NULL;
  txtSize = 0;

  xStart = yStart = xStop = yStop = 0;
  if (startAtLast && haveLastFind) {
    xStart = lastFindXMin;
    yStart = lastFindYMin;
  } else if (!startAtTop) {
    xStart = *xMin;
    yStart = *yMin;
  }
  if (stopAtLast && haveLastFind) {
    xStop = lastFindXMin;
    yStop = lastFindYMin;
  } else if (!stopAtBottom) {
    xStop = *xMax;
    yStop = *yMax;
  }

  found = gFalse;
  xMin0 = xMax0 = yMin0 = yMax0 = 0; // make gcc happy
  xMin1 = xMax1 = yMin1 = yMax1 = 0; // make gcc happy

  for (colIdx = backward ? findCols->getLength() - 1 : 0;
       backward ? colIdx >= 0 : colIdx < findCols->getLength();
       colIdx += backward ? -1 : 1) {
    column = (TextColumn *)findCols->get(colIdx);

    // check: is the column above the top limit?
    if (!startAtTop && (backward ? column->yMin > yStart
			         : column->yMax < yStart)) {
      continue;
    }

    // check: is the column below the bottom limit?
    if (!stopAtBottom && (backward ? column->yMax < yStop
			           : column->yMin > yStop)) {
      continue;
    }

    for (parIdx = backward ? column->paragraphs->getLength() - 1 : 0;
	 backward ? parIdx >= 0 : parIdx < column->paragraphs->getLength();
	 parIdx += backward ? -1 : 1) {
      par = (TextParagraph *)column->paragraphs->get(parIdx);

      // check: is the paragraph above the top limit?
      if (!startAtTop && (backward ? par->yMin > yStart
			           : par->yMax < yStart)) {
	continue;
      }

      // check: is the paragraph below the bottom limit?
      if (!stopAtBottom && (backward ? par->yMax < yStop
			             : par->yMin > yStop)) {
	continue;
      }

      for (lineIdx = backward ? par->lines->getLength() - 1 : 0;
	   backward ? lineIdx >= 0 : lineIdx < par->lines->getLength();
	   lineIdx += backward ? -1 : 1) {
	line = (TextLine *)par->lines->get(lineIdx);

	// check: is the line above the top limit?
	if (!startAtTop && (backward ? line->yMin > yStart
			             : line->yMax < yStart)) {
	  continue;
	}

	// check: is the line below the bottom limit?
	if (!stopAtBottom && (backward ? line->yMax < yStop
			               : line->yMin > yStop)) {
	  continue;
	}

	// convert the line to uppercase
	m = line->len;
	if (!caseSensitive) {
	  if (m > txtSize) {
	    txt = (Unicode *)greallocn(txt, m, sizeof(Unicode));
	    txtSize = m;
	  }
	  for (k = 0; k < m; ++k) {
	    txt[k] = unicodeToUpper(line->text[k]);
	  }
	} else {
	  txt = line->text;
	}

	// search each position in this line
	j = backward ? m - len : 0;
	p = txt + j;
	while (backward ? j >= 0 : j <= m - len) {
	  if (!wholeWord ||
	      ((j == 0 || !unicodeTypeWord(txt[j - 1])) &&
	       (j + len == m || !unicodeTypeWord(txt[j + len])))) {

	    // compare the strings
	    for (k = 0; k < len; ++k) {
	      if (p[k] != s2[k]) {
		break;
	      }
	    }

	    // found it
	    if (k == len) {
	      switch (line->rot) {
	      case 0:
		xMin1 = line->edge[j];
		xMax1 = line->edge[j + len];
		yMin1 = line->yMin;
		yMax1 = line->yMax;
		break;
	      case 1:
		xMin1 = line->xMin;
		xMax1 = line->xMax;
		yMin1 = line->edge[j];
		yMax1 = line->edge[j + len];
		break;
	      case 2:
		xMin1 = line->edge[j + len];
		xMax1 = line->edge[j];
		yMin1 = line->yMin;
		yMax1 = line->yMax;
		break;
	      case 3:
		xMin1 = line->xMin;
		xMax1 = line->xMax;
		yMin1 = line->edge[j + len];
		yMax1 = line->edge[j];
		break;
	      }
	      if (backward) {
		if ((startAtTop ||
		     yMin1 < yStart || (yMin1 == yStart && xMin1 < xStart)) &&
		    (stopAtBottom ||
		     yMin1 > yStop || (yMin1 == yStop && xMin1 > xStop))) {
		  if (!found ||
		      yMin1 > yMin0 || (yMin1 == yMin0 && xMin1 > xMin0)) {
		    xMin0 = xMin1;
		    xMax0 = xMax1;
		    yMin0 = yMin1;
		    yMax0 = yMax1;
		    found = gTrue;
		  }
		}
	      } else {
		if ((startAtTop ||
		     yMin1 > yStart || (yMin1 == yStart && xMin1 > xStart)) &&
		    (stopAtBottom ||
		     yMin1 < yStop || (yMin1 == yStop && xMin1 < xStop))) {
		  if (!found ||
		      yMin1 < yMin0 || (yMin1 == yMin0 && xMin1 < xMin0)) {
		    xMin0 = xMin1;
		    xMax0 = xMax1;
		    yMin0 = yMin1;
		    yMax0 = yMax1;
		    found = gTrue;
		  }
		}
	      }
	    }
	  }
	  if (backward) {
	    --j;
	    --p;
	  } else {
	    ++j;
	    ++p;
	  }
	}
      }
    }
  }

  if (!caseSensitive) {
    gfree(s2);
    gfree(txt);
  }

  if (found) {
    *xMin = xMin0;
    *xMax = xMax0;
    *yMin = yMin0;
    *yMax = yMax0;
    lastFindXMin = xMin0;
    lastFindYMin = yMin0;
    haveLastFind = gTrue;
    return gTrue;
  }

  return gFalse;
}

GString *TextPage::getText(double xMin, double yMin,
			   double xMax, double yMax) {
  UnicodeMap *uMap;
  char space[8], eol[16];
  int spaceLen, eolLen;
  GList *chars2;
  GString **out;
  int *outLen;
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  TextChar *ch;
  GBool primaryLR;
  TextBlock *tree;
  GList *columns;
  GString *ret;
  double xx, yy;
  int rot, colIdx, parIdx, lineIdx, ph, y, i;

  // get the output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    return NULL;
  }
  spaceLen = uMap->mapUnicode(0x20, space, sizeof(space));
  eolLen = 0; // make gcc happy
  switch (globalParams->getTextEOL()) {
  case eolUnix:
    eolLen = uMap->mapUnicode(0x0a, eol, sizeof(eol));
    break;
  case eolDOS:
    eolLen = uMap->mapUnicode(0x0d, eol, sizeof(eol));
    eolLen += uMap->mapUnicode(0x0a, eol + eolLen, sizeof(eol) - eolLen);
    break;
  case eolMac:
    eolLen = uMap->mapUnicode(0x0d, eol, sizeof(eol));
    break;
  }

  // get all chars in the rectangle
  // (i.e., all chars whose center lies inside the rectangle)
  chars2 = new GList();
  for (i = 0; i < chars->getLength(); ++i) {
    ch = (TextChar *)chars->get(i);
    xx = 0.5 * (ch->xMin + ch->xMax);
    yy = 0.5 * (ch->yMin + ch->yMax);
    if (xx > xMin && xx < xMax && yy > yMin && yy < yMax) {
      chars2->append(ch);
    }
  }
#if 0 //~debug
  dumpChars(chars2);
#endif

  rot = rotateChars(chars2);
  primaryLR = checkPrimaryLR(chars2);
  tree = splitChars(chars2);
  if (!tree) {
    unrotateChars(chars2, rot);
    delete chars2;
    return new GString();
  }
#if 0 //~debug
  dumpTree(tree);
#endif
  columns = buildColumns(tree);
  delete tree;
  ph = assignPhysLayoutPositions(columns);
#if 0 //~debug
  dumpColumns(columns);
#endif
  unrotateChars(chars2, rot);
  delete chars2;
  
  out = (GString **)gmallocn(ph, sizeof(GString *));
  outLen = (int *)gmallocn(ph, sizeof(int));
  for (i = 0; i < ph; ++i) {
    out[i] = NULL;
    outLen[i] = 0;
  }

  columns->sort(&TextColumn::cmpPX);
  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    y = col->py;
    for (parIdx = 0;
	 parIdx < col->paragraphs->getLength() && y < ph;
	 ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      for (lineIdx = 0;
	   lineIdx < par->lines->getLength() && y < ph;
	   ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	if (!out[y]) {
	  out[y] = new GString();
	}
	while (outLen[y] < col->px + line->px) {
	  out[y]->append(space, spaceLen);
	  ++outLen[y];
	}
	encodeFragment(line->text, line->len, uMap, primaryLR, out[y]);
	outLen[y] += line->pw;
	++y;
      }
      if (parIdx + 1 < col->paragraphs->getLength()) {
	++y;
      }
    }
  }

  ret = new GString();
  for (i = 0; i < ph; ++i) {
    if (out[i]) {
      ret->append(out[i]);
      delete out[i];
    }
    if (ph > 1) {
      ret->append(eol, eolLen);
    }
  }

  gfree(out);
  gfree(outLen);
  deleteGList(columns, TextColumn);
  uMap->decRefCnt();

  return ret;
}

GBool TextPage::findCharRange(int pos, int length,
			      double *xMin, double *yMin,
			      double *xMax, double *yMax) {
  TextChar *ch;
  double xMin2, yMin2, xMax2, yMax2;
  GBool first;
  int i;

  //~ this doesn't correctly handle ranges split across multiple lines
  //~ (the highlighted region is the bounding box of all the parts of
  //~ the range)

  xMin2 = yMin2 = xMax2 = yMax2 = 0;
  first = gTrue;
  for (i = 0; i < chars->getLength(); ++i) {
    ch = (TextChar *)chars->get(i);
    if (ch->charPos >= pos && ch->charPos < pos + length) {
      if (first || ch->xMin < xMin2) {
	xMin2 = ch->xMin;
      }
      if (first || ch->yMin < yMin2) {
	yMin2 = ch->yMin;
      }
      if (first || ch->xMax > xMax2) {
	xMax2 = ch->xMax;
      }
      if (first || ch->yMax > yMax2) {
	yMax2 = ch->yMax;
      }
      first = gFalse;
    }
  }
  if (first) {
    return gFalse;
  }
  *xMin = xMin2;
  *yMin = yMin2;
  *xMax = xMax2;
  *yMax = yMax2;
  return gTrue;
}

TextWordList *TextPage::makeWordList() {
  TextBlock *tree;
  GList *columns;
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  TextWord *word;
  GList *words;
  int rot, colIdx, parIdx, lineIdx, wordIdx;

  rot = rotateChars(chars);
  tree = splitChars(chars);
  if (!tree) {
    // no text
    unrotateChars(chars, rot);
    return new TextWordList(new GList());
  }
  columns = buildColumns(tree);
  delete tree;
  unrotateChars(chars, rot);
  if (control.html) {
    rotateUnderlinesAndLinks(rot);
    generateUnderlinesAndLinks(columns);
  }

  words = new GList();
  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    for (parIdx = 0; parIdx < col->paragraphs->getLength(); ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      for (lineIdx = 0; lineIdx < par->lines->getLength(); ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	for (wordIdx = 0; wordIdx < line->words->getLength(); ++wordIdx) {
	  word = (TextWord *)line->words->get(wordIdx);
	  words->append(word->copy());
	}
      }
    }
  }

  switch (control.mode) {
  case textOutReadingOrder:
    // already in reading order
    break;
  case textOutPhysLayout:
  case textOutTableLayout:
  case textOutLinePrinter:
    words->sort(&TextWord::cmpYX);
    break;
  case textOutRawOrder:
    words->sort(&TextWord::cmpCharPos);
    break;
  }

  // this has to be done after sorting with cmpYX
  unrotateColumns(columns, rot);
  unrotateWords(words, rot);

  deleteGList(columns, TextColumn);

  return new TextWordList(words);
}

//------------------------------------------------------------------------
// TextPage: debug
//------------------------------------------------------------------------

#if 0 //~debug

void TextPage::dumpChars(GList *charsA) {
  TextChar *ch;
  int i;

  for (i = 0; i < charsA->getLength(); ++i) {
    ch = (TextChar *)charsA->get(i);
    printf("char: U+%04x '%c' xMin=%g yMin=%g xMax=%g yMax=%g fontSize=%g rot=%d\n",
	   ch->c, ch->c & 0xff, ch->xMin, ch->yMin, ch->xMax, ch->yMax,
	   ch->fontSize, ch->rot);
  }
}

void TextPage::dumpTree(TextBlock *tree, int indent) {
  TextChar *ch;
  int i;

  printf("%*sblock: type=%s tag=%s small=%d rot=%d xMin=%g yMin=%g xMax=%g yMax=%g\n",
	 indent, "",
	 tree->type == blkLeaf ? "leaf" :
	                 tree->type == blkHorizSplit ? "horiz" : "vert",
	 tree->tag == blkTagMulticolumn ? "multicolumn" :
	                tree->tag == blkTagColumn ? "column" : "line",
	 tree->smallSplit,
	 tree->rot, tree->xMin, tree->yMin, tree->xMax, tree->yMax);
  if (tree->type == blkLeaf) {
    for (i = 0; i < tree->children->getLength(); ++i) {
      ch = (TextChar *)tree->children->get(i);
      printf("%*schar: '%c' xMin=%g yMin=%g xMax=%g yMax=%g font=%d.%d\n",
	     indent + 2, "", ch->c & 0xff,
	     ch->xMin, ch->yMin, ch->xMax, ch->yMax,
	     ch->font->fontID.num, ch->font->fontID.gen);
    }
  } else {
    for (i = 0; i < tree->children->getLength(); ++i) {
      dumpTree((TextBlock *)tree->children->get(i), indent + 2);
    }
  }
}

void TextPage::dumpColumns(GList *columns) {
  TextColumn *col;
  TextParagraph *par;
  TextLine *line;
  int colIdx, parIdx, lineIdx, i;

  for (colIdx = 0; colIdx < columns->getLength(); ++colIdx) {
    col = (TextColumn *)columns->get(colIdx);
    printf("column: xMin=%g yMin=%g xMax=%g yMax=%g px=%d py=%d pw=%d ph=%d\n",
	   col->xMin, col->yMin, col->xMax, col->yMax,
	   col->px, col->py, col->pw, col->ph);
    for (parIdx = 0; parIdx < col->paragraphs->getLength(); ++parIdx) {
      par = (TextParagraph *)col->paragraphs->get(parIdx);
      printf("  paragraph:\n");
      for (lineIdx = 0; lineIdx < par->lines->getLength(); ++lineIdx) {
	line = (TextLine *)par->lines->get(lineIdx);
	printf("    line: xMin=%g yMin=%g xMax=%g yMax=%g px=%d pw=%d rot=%d\n",
	       line->xMin, line->yMin, line->xMax, line->yMax,
	       line->px, line->pw, line->rot);
	printf("          ");
	for (i = 0; i < line->len; ++i) {
	  printf("%c", line->text[i] & 0xff);
	}
	printf("\n");
      }
    }
  }
}

#endif //~debug

//------------------------------------------------------------------------
// TextOutputDev
//------------------------------------------------------------------------

static void outputToFile(void *stream, const char *text, int len) {
  fwrite(text, 1, len, (FILE *)stream);
}

TextOutputDev::TextOutputDev(char *fileName, TextOutputControl *controlA,
			     GBool append) {
  text = NULL;
  control = *controlA;
  ok = gTrue;

  // open file
  needClose = gFalse;
  if (fileName) {
    if (!strcmp(fileName, "-")) {
      outputStream = stdout;
#ifdef WIN32
      // keep DOS from munging the end-of-line characters
      setmode(fileno(stdout), O_BINARY);
#endif
    } else if ((outputStream = fopen(fileName, append ? "ab" : "wb"))) {
      needClose = gTrue;
    } else {
      error(errIO, -1, "Couldn't open text file '{0:s}'", fileName);
      ok = gFalse;
      return;
    }
    outputFunc = &outputToFile;
  } else {
    outputStream = NULL;
  }

  // set up text object
  text = new TextPage(&control);
}

TextOutputDev::TextOutputDev(TextOutputFunc func, void *stream,
			     TextOutputControl *controlA) {
  outputFunc = func;
  outputStream = stream;
  needClose = gFalse;
  control = *controlA;
  text = new TextPage(&control);
  ok = gTrue;
}

TextOutputDev::~TextOutputDev() {
  if (needClose) {
    fclose((FILE *)outputStream);
  }
  if (text) {
    delete text;
  }
}

void TextOutputDev::startPage(int pageNum, GfxState *state) {
  text->startPage(state);
}

void TextOutputDev::endPage() {
  if (outputStream) {
    text->write(outputStream, outputFunc);
  }
}

void TextOutputDev::restoreState(GfxState *state) {
  text->updateFont(state);
}

void TextOutputDev::updateFont(GfxState *state) {
  text->updateFont(state);
}

void TextOutputDev::beginString(GfxState *state, GString *s) {
}

void TextOutputDev::endString(GfxState *state) {
}

void TextOutputDev::drawChar(GfxState *state, double x, double y,
			     double dx, double dy,
			     double originX, double originY,
			     CharCode c, int nBytes, Unicode *u, int uLen) {
  text->addChar(state, x, y, dx, dy, c, nBytes, u, uLen);
}

void TextOutputDev::incCharCount(int nChars) {
  text->incCharCount(nChars);
}

void TextOutputDev::beginActualText(GfxState *state, Unicode *u, int uLen) {
  text->beginActualText(state, u, uLen);
}

void TextOutputDev::endActualText(GfxState *state) {
  text->endActualText(state);
}

void TextOutputDev::stroke(GfxState *state) {
  GfxPath *path;
  GfxSubpath *subpath;
  double x[2], y[2];

  if (!control.html) {
    return;
  }
  path = state->getPath();
  if (path->getNumSubpaths() != 1) {
    return;
  }
  subpath = path->getSubpath(0);
  if (subpath->getNumPoints() != 2) {
    return;
  }
  state->transform(subpath->getX(0), subpath->getY(0), &x[0], &y[0]);
  state->transform(subpath->getX(1), subpath->getY(1), &x[1], &y[1]);

  // look for a vertical or horizontal line
  if (x[0] == x[1] || y[0] == y[1]) {
    text->addUnderline(x[0], y[0], x[1], y[1]);
  }
}

void TextOutputDev::fill(GfxState *state) {
  GfxPath *path;
  GfxSubpath *subpath;
  double x[5], y[5];
  double rx0, ry0, rx1, ry1, t;
  int i;

  if (!control.html) {
    return;
  }
  path = state->getPath();
  if (path->getNumSubpaths() != 1) {
    return;
  }
  subpath = path->getSubpath(0);
  if (subpath->getNumPoints() != 5) {
    return;
  }
  for (i = 0; i < 5; ++i) {
    if (subpath->getCurve(i)) {
      return;
    }
    state->transform(subpath->getX(i), subpath->getY(i), &x[i], &y[i]);
  }

  // look for a rectangle
  if (x[0] == x[1] && y[1] == y[2] && x[2] == x[3] && y[3] == y[4] &&
      x[0] == x[4] && y[0] == y[4]) {
    rx0 = x[0];
    ry0 = y[0];
    rx1 = x[2];
    ry1 = y[1];
  } else if (y[0] == y[1] && x[1] == x[2] && y[2] == y[3] && x[3] == x[4] &&
	     x[0] == x[4] && y[0] == y[4]) {
    rx0 = x[0];
    ry0 = y[0];
    rx1 = x[1];
    ry1 = y[2];
  } else {
    return;
  }
  if (rx1 < rx0) {
    t = rx0;
    rx0 = rx1;
    rx1 = t;
  }
  if (ry1 < ry0) {
    t = ry0;
    ry0 = ry1;
    ry1 = t;
  }

  // skinny horizontal rectangle
  if (ry1 - ry0 < rx1 - rx0) {
    if (ry1 - ry0 < maxUnderlineWidth) {
      ry0 = 0.5 * (ry0 + ry1);
      text->addUnderline(rx0, ry0, rx1, ry0);
    }

  // skinny vertical rectangle
  } else {
    if (rx1 - rx0 < maxUnderlineWidth) {
      rx0 = 0.5 * (rx0 + rx1);
      text->addUnderline(rx0, ry0, rx0, ry1);
    }
  }
}

void TextOutputDev::eoFill(GfxState *state) {
  if (!control.html) {
    return;
  }
  fill(state);
}

void TextOutputDev::processLink(Link *link) {
  double x1, y1, x2, y2;
  int xMin, yMin, xMax, yMax, x, y;

  if (!control.html) {
    return;
  }
  link->getRect(&x1, &y1, &x2, &y2);
  cvtUserToDev(x1, y1, &x, &y);
  xMin = xMax = x;
  yMin = yMax = y;
  cvtUserToDev(x1, y2, &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  cvtUserToDev(x2, y1, &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  cvtUserToDev(x2, y2, &x, &y);
  if (x < xMin) {
    xMin = x;
  } else if (x > xMax) {
    xMax = x;
  }
  if (y < yMin) {
    yMin = y;
  } else if (y > yMax) {
    yMax = y;
  }
  text->addLink(xMin, yMin, xMax, yMax, link);
}

GBool TextOutputDev::findText(Unicode *s, int len,
			      GBool startAtTop, GBool stopAtBottom,
			      GBool startAtLast, GBool stopAtLast,
			      GBool caseSensitive, GBool backward,
			      GBool wholeWord,
			      double *xMin, double *yMin,
			      double *xMax, double *yMax) {
  return text->findText(s, len, startAtTop, stopAtBottom,
			startAtLast, stopAtLast,
			caseSensitive, backward, wholeWord,
			xMin, yMin, xMax, yMax);
}

GString *TextOutputDev::getText(double xMin, double yMin,
				double xMax, double yMax) {
  return text->getText(xMin, yMin, xMax, yMax);
}

GBool TextOutputDev::findCharRange(int pos, int length,
				   double *xMin, double *yMin,
				   double *xMax, double *yMax) {
  return text->findCharRange(pos, length, xMin, yMin, xMax, yMax);
}

TextWordList *TextOutputDev::makeWordList() {
  return text->makeWordList();
}

TextPage *TextOutputDev::takeText() {
  TextPage *ret;

  ret = text;
  text = new TextPage(&control);
  return ret;
}
