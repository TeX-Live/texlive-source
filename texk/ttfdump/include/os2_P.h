/* Private header file for os2.c */
#ifdef __TTF_OS2
#ifndef __TTF_OS2_H
#define __TTF_OS2_H

static const char *WeightClassName[] =
{
  "Thin",                   /* Ultra-light */
  "Extra-light",
  "Light",
  "Normal",                 /* Semi-light */
  "Medium",
  "Semi-bold",
  "Bold",
  "Extra-Bold",
  "Black"                   /* Ultra-bold */
};

static const char *WidthClassName[] =
{
  "Ultra-condensed",
  "Extra-condensed",
  "Condensed",
  "Semi-condensed",
  "Medium",
  "Semi-expanded",
  "Expanded",
  "Extra-expanded",
  "Ultra-expanded"
};

static const char *PanoseFamily[] =
{
  "Any",
  "No Fit",
  "Text and Display",
  "Script",
  "Decorative",
  "Pictorial"
};

static const char *PanoseSerif[] =
{
  "Any",
  "No Fit",
  "Cove",
  "Obtuse Cove",
  "Square Cove",
  "Obtuse Square Cove",
  "Square",
  "Thin",
  "Bone",
  "Exaggerated",
  "Triangle",
  "Normal Sans",
  "Obtuse Sans",
  "Perp Sans",
  "Flared",
  "Rounded"
};

static const char *PanoseWeight[] =
{
  "Any",
  "No Fit",
  "Very Light",
  "Ligth",
  "Thin",
  "Book",
  "Medium",
  "Demi",
  "Bold",
  "Heavy",
  "Black",
  "Nord",
};

static const char *PanoseProportion[] =
{
  "Any",
  "No Fit",
  "Old Style",
  "Modern",
  "Even Width",
  "Expanded",
  "Condensed",
  "Very Expanded",
  "Very Condensed",
  "Monospaced"
};

static const char *PanoseContrast[] =
{
  "Any",
  "No Fit",
  "None",
  "Very Low",
  "Low",
  "Medium Low",
  "Medium",
  "Medium High",
  "High",
  "Very High",
};

static const char *PanoseStroke[] =
{
  "Any",
  "No Fit",
  "Gradual/Diagonal",
  "Gradual/Transitional",
  "Gradual/Vertical",
  "Gradual/Horizontal",
  "Rapid/Vertical",
  "Rapid/Horizontal",
  "Rapid/Vertical"
};

static const char *PanoseArm[] =
{
  "Any",
  "No Fit"
  "Straight Arms/Horizontal",
  "Straight Arms/Wedge",
  "Straight Arms/Vertical",
  "Straight Arms/Single Serif",
  "Straight Arms/Double Serif",
  "Non-Straight Arms/Horizontal",
  "Non-Straight Arms/Wedge",
  "Non-Straight Arms/Vertical",
  "Non-Straight Arms/Single Serif",
  "Non-Straight Arms/Double Serif"
};

static const char *PanoseLetterform[] =
{
  "Any",
  "No Fit",
  "Normal/Contact",
  "Normal/Weighted",
  "Normal/Boxed",
  "Normal/Flttened",
  "Normal/Rounded",
  "Normal/Off Center",
  "Normal/Square",
  "Oblique/Contact",
  "Oblique/Weighted",
  "Oblique/Boxed",
  "Oblique/Flattened",
  "Oblique/Rounded",
  "Oblique/Off Center",
  "Oblique/Square"
};

static const char *PanoseMidline[] =
{
  "Any",
  "No Fit",
  "Standard/Trimmed",
  "Standard/Pointed",
  "Standard/Serifed",
  "High/Trimmed",
  "High/Pointed"
  "High/Serifed",
  "Constant/Trimmed",
  "Constant/Pointed",
  "Constant/Serifed",
  "Low/Trimmed"
  "Low/Pointed",
  "Low/Serifed",
};

static const char *PanoseXHeight[] =
{
  "Any",
  "No Fit",
  "Constant/Small",
  "Constant/Standard",
  "Constant/Large",
  "Ducking/Small",
  "Ducking/Standard",
  "Ducking/Large"
};

#define FS_FLAGS_ITALIC     0x01
#define FS_FLAGS_UNDERSCORE 0x02
#define FS_FLAGS_NEGATIVE   0x04
#define FS_FLAGS_OUTLINED   0x08
#define FS_FLAGS_STRIKEOUT  0x10
#define FS_FLAGS_BOLD       0x20
#define FS_FLAGS_REGULAR    0x40

#endif /* __TTF_OS2_H */
#endif /* __TTF_OS2 */


/* end of os2_P.h */
