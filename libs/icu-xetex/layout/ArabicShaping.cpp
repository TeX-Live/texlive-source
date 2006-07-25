/*
 *
 * (C) Copyright IBM Corp. 1998-2004 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "OpenTypeTables.h"
#include "ArabicShaping.h"
#include "LEGlyphStorage.h"

U_NAMESPACE_BEGIN

enum {
    _c_ = ArabicShaping::ST_NOSHAPE_DUAL,
    _d_ = ArabicShaping::ST_DUAL,
    _n_ = ArabicShaping::ST_NONE,
    _r_ = ArabicShaping::ST_RIGHT,
    _t_ = ArabicShaping::ST_TRANSPARENT,
    _x_ = ArabicShaping::ST_NOSHAPE_NONE,
    _A_ = ArabicShaping::ST_ALAPH,
    _R_ = ArabicShaping::ST_DALATH_RISH
};

const ArabicShaping::ShapeType ArabicShaping::shapeTypes[] =
{
// Arabic block
    _t_, _t_, _t_, _t_, _t_, _t_, _x_, _x_, _x_, _x_, _x_, _n_, _x_, _x_, _x_, _n_,   // 0x610 - 0x61f
    _x_, _n_, _r_, _r_, _r_, _r_, _d_, _r_, _d_, _r_, _d_, _d_, _d_, _d_, _d_, _r_,   // 0x620 - 0x62f
    _r_, _r_, _r_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _x_, _x_, _x_, _x_, _x_,   // 0x630 - 0x63f
    _c_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _r_, _d_, _d_, _t_, _t_, _t_, _t_, _t_,   // 0x640 - 0x64f
    _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _x_,   // 0x650 - 0x65f
    _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _d_, _d_,   // 0x660 - 0x66f
    _t_, _r_, _r_, _r_, _n_, _r_, _r_, _r_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x670 - 0x67f
    _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_,   // 0x680 - 0x68f
    _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x690 - 0x69f
    _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x6a0 - 0x6af
    _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x6b0 - 0x6bf
    _r_, _d_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _r_, _d_, _r_, _d_, _r_,   // 0x6c0 - 0x6cf
    _d_, _d_, _r_, _r_, _n_, _r_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _x_, _t_, _t_,   // 0x6d0 - 0x6df
    _t_, _t_, _t_, _t_, _t_, _n_, _n_, _t_, _t_, _n_, _t_, _t_, _t_, _t_, _r_, _r_,   // 0x6e0 - 0x6ef
    _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _d_, _d_, _d_, _n_, _n_, _d_,   // 0x6f0 - 0x6ff
// Syriac
    _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _t_,   // 0x700 - 0x70f
    _A_, _t_, _d_, _d_, _d_, _R_, _R_, _r_, _r_, _r_, _d_, _d_, _d_, _d_, _r_, _d_,   // 0x710 - 0x71f
    _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _r_, _d_, _R_, _d_, _r_, _d_, _d_, _R_,   // 0x720 - 0x72f
    _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_,   // 0x730 - 0x73f
    _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _t_, _x_, _x_, _r_, _d_, _d_,   // 0x740 - 0x74f
// Arabic Supplement
    _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _r_, _r_, _r_, _d_, _d_, _d_, _d_,   // 0x750 - 0x75f
    _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _r_, _r_, _d_, _x_, _x_,   // 0x760 - 0x76f
    _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_, _x_    // 0x770 - 0x77f
};

const ArabicShaping::ShapeType ArabicShaping::mongolianTypes[] =
{
   _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _t_, _t_, _t_, _t_, _n_,   // 0x1800 - 0x180f
   _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_,   // 0x1810 - 0x181f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1820 - 0x182f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1830 - 0x183f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1840 - 0x184f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1850 - 0x185f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1860 - 0x186f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _n_, _n_, _n_, _n_, _n_, _n_, _n_, _n_,   // 0x1870 - 0x187f
   _n_, _n_, _n_, _n_, _n_, _n_, _n_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1880 - 0x188f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_,   // 0x1890 - 0x189f
   _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _d_, _t_, _n_, _n_, _n_, _n_, _n_, _n_    // 0x18a0 - 0x18af
};

/*
    shaping array holds types for Arabic/Syriac chars between 0610 and 077f;
    mongolian array holds types for Mongolian chars between 1800 and 18af;
    other values are either unshaped, or transparent if a mark or format
    code, except for format codes 200c (zero-width non-joiner) and 200d 
    (dual-width joiner) which are both unshaped and non_joining or
    dual-joining, respectively.
*/
ArabicShaping::ShapeType ArabicShaping::getShapeType(LEUnicode c)
{
    if (c >= 0x0300 && c <= 0x206f) {
        if (/*c >= 0x0300 &&*/ c <= 0x036f) { // Combining Marks
            return ST_TRANSPARENT;
        } else if (c >= 0x0610 && c <= 0x077f) { // Arabic/Syriac/Arabic Supplement
            return shapeTypes[c - 0x0610];
        } else if (c >= 0x1800 && c <= 0x18af) { // Mongolian
            return mongolianTypes[c - 0x1800];
        } else if (c == 0x200c) {   // ZWNJ
            return ST_NOSHAPE_NONE;
        } else if (c == 0x200d) {   // ZWJ
            return ST_NOSHAPE_DUAL;
        } else if (c >= 0x202a && c <= 0x202e) { // LRE - RLO
            return ST_TRANSPARENT;
        } else if (c >= 0x206a /*&& c <= 0x206f*/) { // Inhibit Symmetric Swapping - Nominal Digit Shapes
            return ST_TRANSPARENT;
        }
    }

    return ST_NOSHAPE_NONE;
}

static const LETag isolFeatureTag = LE_ISOL_FEATURE_TAG;
static const LETag initFeatureTag = LE_INIT_FEATURE_TAG;
static const LETag mediFeatureTag = LE_MEDI_FEATURE_TAG;
static const LETag finaFeatureTag = LE_FINA_FEATURE_TAG;
static const LETag ligaFeatureTag = LE_LIGA_FEATURE_TAG;
static const LETag msetFeatureTag = LE_MSET_FEATURE_TAG;
static const LETag markFeatureTag = LE_MARK_FEATURE_TAG;
static const LETag ccmpFeatureTag = LE_CCMP_FEATURE_TAG;
static const LETag rligFeatureTag = LE_RLIG_FEATURE_TAG;
static const LETag caltFeatureTag = LE_CALT_FEATURE_TAG;
static const LETag dligFeatureTag = LE_DLIG_FEATURE_TAG;
static const LETag cswhFeatureTag = LE_CSWH_FEATURE_TAG;
static const LETag cursFeatureTag = LE_CURS_FEATURE_TAG;
static const LETag kernFeatureTag = LE_KERN_FEATURE_TAG;
static const LETag mkmkFeatureTag = LE_MKMK_FEATURE_TAG;
static const LETag med2FeatureTag = LE_MED2_FEATURE_TAG;
static const LETag fin2FeatureTag = LE_FIN2_FEATURE_TAG;
static const LETag fin3FeatureTag = LE_FIN3_FEATURE_TAG;

static const LETag emptyTag       = 0x00000000; // ''

static const LETag featureOrder[] = 
{
    ccmpFeatureTag, isolFeatureTag, finaFeatureTag, fin2FeatureTag, fin3FeatureTag, mediFeatureTag,
    med2FeatureTag, initFeatureTag, rligFeatureTag, caltFeatureTag, ligaFeatureTag, dligFeatureTag,
    cswhFeatureTag, msetFeatureTag, cursFeatureTag, kernFeatureTag, markFeatureTag, mkmkFeatureTag,
    emptyTag
};

const LETag ArabicShaping::tagArray[] =
{
    isolFeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    finaFeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    initFeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    mediFeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    fin2FeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    fin3FeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    med2FeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag,

    med2FeatureTag, ligaFeatureTag, msetFeatureTag, markFeatureTag, ccmpFeatureTag, rligFeatureTag,
        caltFeatureTag, dligFeatureTag, cswhFeatureTag, cursFeatureTag, kernFeatureTag, mkmkFeatureTag, emptyTag
            // this entry repeated so that both fin2 and fin3 map to med2 via a +2 offset
};

#define TAGS_PER_GLYPH ((sizeof ArabicShaping::tagArray / sizeof ArabicShaping::tagArray[0]) / 8)

const LETag *ArabicShaping::getFeatureOrder()
{
    return featureOrder;
}

void ArabicShaping::adjustTags(le_int32 outIndex, le_int32 shapeOffset, LEGlyphStorage &glyphStorage)
{
    LEErrorCode success = LE_NO_ERROR;
    const LETag *glyphTags = (const LETag *) glyphStorage.getAuxData(outIndex, success);

    glyphStorage.setAuxData(outIndex, (void *) &glyphTags[TAGS_PER_GLYPH * shapeOffset], success);
}

void ArabicShaping::shape(const LEUnicode *chars, le_int32 offset, le_int32 charCount, le_int32 charMax,
                          le_bool rightToLeft, LEGlyphStorage &glyphStorage)
{
    // iterate in logical order, store tags in visible order
    // 
    // the effective right char is the most recently encountered 
    // non-transparent char
    //
    // four boolean states:
    //   the effective right char shapes
    //   the effective right char causes left shaping
    //   the current char shapes
    //   the current char causes right shaping
    // 
    // if both cause shaping, then
    //   shaper.shape(errout, 2) (isolate to initial, or final to medial)
    //   shaper.shape(out, 1) (isolate to final)

    // special cases for Syriac ALAPH:
    //   final ALAPH is fin2 when effective right char shapes but does not cause left shaping, and is not DALATH/RISH
    //   final ALAPH is fin3 when effective right char is DALATH/RISH
    //   medial ALAPH is med2 when effective right char shapes but does not cause left shaping
    // using one of:
    //   shaper.shape(out, 4) (isolate to fin2)
    //   shaper.shape(out, 5) (isolate to fin3)

    ShapeType rightType = ST_NOSHAPE_NONE, leftType = ST_NOSHAPE_NONE;
    LEErrorCode success = LE_NO_ERROR;
    le_int32 i;

    for (i = offset - 1; i >= 0; i -= 1) {
        rightType = getShapeType(chars[i]);
        
        if (rightType != ST_TRANSPARENT) {
            break;
        }
    }

    for (i = offset + charCount; i < charMax; i += 1) {
        leftType = getShapeType(chars[i]);

        if (leftType != ST_TRANSPARENT) {
            break;
        }
    }

    // erout is effective right logical index
    le_int32 erout = -1;
    le_bool rightShapes = FALSE;
    le_bool rightCauses = (rightType & MASK_SHAPE_LEFT) != 0;
    le_int32 in, e, out = 0, dir = 1;

    if (rightToLeft) {
        out = charCount - 1;
        erout = charCount;
        dir = -1;
    }

    for (in = offset, e = offset + charCount; in < e; in += 1, out += dir) {
        LEUnicode c = chars[in];
        ShapeType t = getShapeType(c);

        glyphStorage.setAuxData(out, (void *) tagArray, success);

        if ((t & MASK_TRANSPARENT) != 0) {
            continue;
        }

        le_bool curShapes = (t & MASK_NOSHAPE) == 0;
        le_bool curCauses = (t & MASK_SHAPE_RIGHT) != 0;

        if (rightCauses && curCauses) {
            if (rightShapes) {
                adjustTags(erout, 2, glyphStorage);
            }

            if (curShapes) {
                adjustTags(out, 1, glyphStorage);
            }
        } else if (t == ST_ALAPH) {
        	if (rightType == ST_DALATH_RISH) {
        		adjustTags(out, 5, glyphStorage);
        	} else if (rightShapes && !rightCauses) {
        		adjustTags(out, 4, glyphStorage);
        	}
        }

        rightShapes = curShapes;
        rightCauses = (t & MASK_SHAPE_LEFT) != 0;
        rightType = t; // remember this; might need to know if it was DALATH/RISH
        erout = out;
    }

    if (rightShapes && rightCauses && (leftType & MASK_SHAPE_RIGHT) != 0) {
        adjustTags(erout, 2, glyphStorage);
    }
}

U_NAMESPACE_END
