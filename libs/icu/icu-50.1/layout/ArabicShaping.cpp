/*
 *
 * (C) Copyright IBM Corp. 1998-2008 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "OpenTypeTables.h"
#include "ArabicShaping.h"
#include "LEGlyphStorage.h"
#include "ClassDefinitionTables.h"

U_NAMESPACE_BEGIN

// This table maps Unicode joining types to
// ShapeTypes.
const ArabicShaping::ShapeType ArabicShaping::shapeTypes[] =
{
    ArabicShaping::ST_NOSHAPE_NONE, // [U]
    ArabicShaping::ST_NOSHAPE_DUAL, // [C]
    ArabicShaping::ST_DUAL,         // [D]
    ArabicShaping::ST_LEFT,         // [L]
    ArabicShaping::ST_RIGHT,        // [R]
    ArabicShaping::ST_TRANSPARENT   // [T]
};

// Draft Mongolian shaping classes, not provided by the Unicode data files at this time
enum {
    _c_ = ArabicShaping::ST_NOSHAPE_DUAL,
    _d_ = ArabicShaping::ST_DUAL,
    _n_ = ArabicShaping::ST_NONE,
    _r_ = ArabicShaping::ST_RIGHT,
    _t_ = ArabicShaping::ST_TRANSPARENT,
    _x_ = ArabicShaping::ST_NOSHAPE_NONE
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
    shaping array holds types for Arabic chars between 0610 and 0700
    other values are either unshaped, or transparent if a mark or format
    code, except for format codes 200c (zero-width non-joiner) and 200d 
    (dual-width joiner) which are both unshaped and non_joining or
    dual-joining, respectively.
*/
ArabicShaping::ShapeType ArabicShaping::getShapeType(LEUnicode c)
{
    const ClassDefinitionTable *joiningTypes = (const ClassDefinitionTable *) ArabicShaping::shapingTypeTable;
    le_int32 joiningType = joiningTypes->getGlyphClass(c);

    if (joiningType == ArabicShaping::JT_RIGHT_JOINING) { // check for Syriac exceptions ALAPH, DALATH, RISH
        if (c == 0x0710)
            return ArabicShaping::ST_ALAPH;
        if (c == 0x0715 || c == 0x0716 || c == 0x072A || c == 0x072F)
            return ArabicShaping::ST_DALATH_RISH;
    }

    if (joiningType == 0) { // check for Mongolian range, not supported by ArabicShaping::shapingTypeTable
        if (c >= 0x1800 && c <= 0x18af)
            return ArabicShaping::mongolianTypes[c - 0x1800];
    }

    if (joiningType >= 0 && joiningType < ArabicShaping::JT_COUNT) {
        return ArabicShaping::shapeTypes[joiningType];
    }

    return ArabicShaping::ST_NOSHAPE_NONE;
}

#define isolFeatureTag LE_ISOL_FEATURE_TAG
#define initFeatureTag LE_INIT_FEATURE_TAG
#define mediFeatureTag LE_MEDI_FEATURE_TAG
#define med2FeatureTag LE_MED2_FEATURE_TAG
#define finaFeatureTag LE_FINA_FEATURE_TAG
#define fin2FeatureTag LE_FIN2_FEATURE_TAG
#define fin3FeatureTag LE_FIN3_FEATURE_TAG
#define ligaFeatureTag LE_LIGA_FEATURE_TAG
#define msetFeatureTag LE_MSET_FEATURE_TAG
#define markFeatureTag LE_MARK_FEATURE_TAG
#define ccmpFeatureTag LE_CCMP_FEATURE_TAG
#define rligFeatureTag LE_RLIG_FEATURE_TAG
#define caltFeatureTag LE_CALT_FEATURE_TAG
#define dligFeatureTag LE_DLIG_FEATURE_TAG
#define cswhFeatureTag LE_CSWH_FEATURE_TAG
#define cursFeatureTag LE_CURS_FEATURE_TAG
#define kernFeatureTag LE_KERN_FEATURE_TAG
#define mkmkFeatureTag LE_MKMK_FEATURE_TAG

// NOTE:
// The isol, fina, init and medi features must be
// defined in the above order, and have masks that
// are all in the same byte.
#define isolFeatureMask 0x80000000UL
#define finaFeatureMask 0x40000000UL
#define initFeatureMask 0x20000000UL
#define mediFeatureMask 0x10000000UL
#define fin2FeatureMask 0x08000000UL
#define fin3FeatureMask 0x04000000UL
#define med2FeatureMask 0x03000000UL /* two bits, from shifting either fin2 or fin3 */

#define ccmpFeatureMask 0x00800000UL
#define rligFeatureMask 0x00400000UL
#define caltFeatureMask 0x00200000UL
#define ligaFeatureMask 0x00100000UL
#define dligFeatureMask 0x00080000UL
#define cswhFeatureMask 0x00040000UL
#define msetFeatureMask 0x00020000UL
#define cursFeatureMask 0x00010000UL
#define kernFeatureMask 0x00008000UL
#define markFeatureMask 0x00004000UL
#define mkmkFeatureMask 0x00002000UL

#define NO_FEATURES   0
#define ISOL_FEATURES (isolFeatureMask | ligaFeatureMask | msetFeatureMask | markFeatureMask | ccmpFeatureMask | rligFeatureMask | caltFeatureMask | dligFeatureMask | cswhFeatureMask | cursFeatureMask | kernFeatureMask | mkmkFeatureMask)

#define SHAPE_MASK 0xFF000000UL

static const FeatureMap featureMap[] = {
    {ccmpFeatureTag, ccmpFeatureMask},
    {isolFeatureTag, isolFeatureMask},
    {finaFeatureTag, finaFeatureMask},
    {fin2FeatureTag, fin2FeatureMask},
    {fin3FeatureTag, fin3FeatureMask},
    {mediFeatureTag, mediFeatureMask},
    {med2FeatureTag, med2FeatureMask},
    {initFeatureTag, initFeatureMask},
    {rligFeatureTag, rligFeatureMask},
    {caltFeatureTag, caltFeatureMask},
    {ligaFeatureTag, ligaFeatureMask},
    {dligFeatureTag, dligFeatureMask},
    {cswhFeatureTag, cswhFeatureMask},
    {msetFeatureTag, msetFeatureMask},
    {cursFeatureTag, cursFeatureMask},
    {kernFeatureTag, kernFeatureMask},
    {markFeatureTag, markFeatureMask},
    {mkmkFeatureTag, mkmkFeatureMask}
};

const FeatureMap *ArabicShaping::getFeatureMap(le_int32 &count)
{
    count = LE_ARRAY_SIZE(featureMap);

    return featureMap;
}

void ArabicShaping::adjustTags(le_int32 outIndex, le_int32 shapeOffset, LEGlyphStorage &glyphStorage)
{
    LEErrorCode success = LE_NO_ERROR;
    FeatureMask featureMask = (FeatureMask) glyphStorage.getAuxData(outIndex, success);
    FeatureMask shape = featureMask & SHAPE_MASK;

    shape >>= shapeOffset;

    glyphStorage.setAuxData(outIndex, ((featureMask & ~SHAPE_MASK) | shape), success);
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

        if (t == ST_NOSHAPE_NONE) {
            glyphStorage.setAuxData(out, NO_FEATURES, success);
        } else {
            glyphStorage.setAuxData(out, ISOL_FEATURES, success);
        }

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
