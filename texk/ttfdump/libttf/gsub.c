/* gsub.c -- Glyph Substitution Table
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

static Sub11Ptr makeGSUB11 (FILE *fp, ULONG offset)
{
    USHORT cOffset;
    Sub11Ptr sub = XCALLOC1 (Sub11);

    cOffset = ttfGetUSHORT (fp);
    sub->deltaGlyphID = ttfGetUSHORT (fp);
    sub->coverage = otfMakeCoverage (fp, offset + cOffset);

    return sub;
}

static void printGSUB11 (FILE *fp, Sub11Ptr sub)
{
    fprintf (fp, " - Single Substitution Delta\n\t  ");
    otfPrintCoverage (fp, sub->coverage);
    fprintf (fp, "\t  deltaGlyphID: %d\n", sub->deltaGlyphID);
}

static void freeGSUB11 (Sub11Ptr sub)
{
    otfFreeCoverage (sub->coverage);
}

static Sub12Ptr makeGSUB12 (FILE *fp, ULONG offset)
{
    USHORT cOffset;
    Sub12Ptr sub = XCALLOC1 (Sub12);

    cOffset = ttfGetUSHORT (fp);
    sub->glyphCount = ttfGetUSHORT (fp);
    sub->substitute = ttfMakeUSHORT (sub->glyphCount, fp);
    sub->coverage = otfMakeCoverage (fp, offset + cOffset);

    return sub;
}

static void printGSUB12 (FILE *fp, Sub12Ptr sub)
{
    int i;

    fprintf (fp, " - Single Substitution List\n\t  ");
    otfPrintCoverage (fp, sub->coverage);
    fprintf (fp, "\t  glyphCount: %d\n\t\t  substitute: %d",
                 sub->glyphCount, sub->substitute[0]);
    for (i = 1; i < sub->glyphCount; i++)
        fprintf (fp, i % 8 ? ", %d" : ",\n\t\t\t      %d", sub->substitute[i]);
    fprintf (fp, "\n");
}

static void freeGSUB12 (Sub12Ptr sub)
{
    otfFreeCoverage (sub->coverage);
    free (sub->substitute);
}

static void gsubLoadSequence (SequencePtr sequence, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "gsubLoadSequence");

    sequence->glyphCount = ttfGetUSHORT (fp);
    sequence->substitute = ttfMakeUSHORT (sequence->glyphCount, fp);
}

static Sub21Ptr makeGSUB21 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *sOffset;
    Sub21Ptr sub = XCALLOC1 (Sub21);

    cOffset = ttfGetUSHORT (fp);
    sub->sequenceCount = ttfGetUSHORT (fp);
    sOffset = ttfMakeUSHORT (sub->sequenceCount, fp);
    sub->coverage = otfMakeCoverage (fp, offset + cOffset);
    sub->sequence = XCALLOC (sub->sequenceCount, Sequence);
    for (i = 0; i < sub->sequenceCount; i++)
        gsubLoadSequence (&sub->sequence[i], fp, offset + sOffset[i]);
    free (sOffset);

    return sub;
}

static void printGSUB21 (FILE *fp, Sub21Ptr sub)
{
    int i;

    fprintf (fp, " - Multiple Substitution\n\t  ");
    otfPrintCoverage (fp, sub->coverage);
    fprintf (fp, "\t  sequenceCount: %d\n", sub->sequenceCount);
    for (i = 0; i < sub->sequenceCount; i++) {
        int j;

        fprintf (fp, "\t  %2d. glyphCount: %d - ", i, sub->sequence[i].glyphCount);
        for (j = 0; j < sub->sequence[i].glyphCount; j++)
            fprintf (fp, j == 0 ? "- %d" : ", %d", sub->sequence[i].substitute[j]);
        fprintf (fp, "\n");
    }
}

static void freeGSUB21 (Sub21Ptr sub)
{
    int i;

    otfFreeCoverage (sub->coverage);
    for (i = 0; i < sub->sequenceCount; i++)
        free (sub->sequence[i].substitute);
    free (sub->sequence);
}

static void gsubLoadAlternateSet (AlternateSetPtr alternateSet, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "gsubLoadAlternateSet");

    alternateSet->glyphCount = ttfGetUSHORT (fp);
    alternateSet->alternate = ttfMakeUSHORT (alternateSet->glyphCount, fp);
}

static Sub31Ptr makeGSUB31 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *aOffset;
    Sub31Ptr sub = XCALLOC1 (Sub31);

    cOffset = ttfGetUSHORT (fp);
    sub->alternateSetCount = ttfGetUSHORT (fp);
    aOffset = ttfMakeUSHORT (sub->alternateSetCount, fp);
    sub->coverage = otfMakeCoverage (fp, offset + cOffset);
    sub->alternateSet = XCALLOC (sub->alternateSetCount, AlternateSet);
    for (i = 0; i < sub->alternateSetCount; i++)
        gsubLoadAlternateSet (&sub->alternateSet[i], fp, offset + aOffset[i]);
    free (aOffset);

    return sub;
}

static void printGSUB31 (FILE *fp, Sub31Ptr sub)
{
    int i;

    fprintf (fp, " - Alternate Substitution\n\t  ");
    otfPrintCoverage (fp, sub->coverage);
    fprintf (fp, "\t  alternateSetCount: %d\n", sub->alternateSetCount);
    for (i = 0; i < sub->alternateSetCount; i++) {
        int j;

        fprintf (fp, "\t  %2d. glyphCount: %d ", i, sub->alternateSet[i].glyphCount);
        for (j = 0; j < sub->alternateSet[i].glyphCount; j++)
            fprintf (fp, j == 0 ? "- %d" : ", %d", sub->alternateSet[i].alternate[j]);
        fprintf (fp, "\n");
    }
}

static void freeGSUB31 (Sub31Ptr sub)
{
    int i;

    otfFreeCoverage (sub->coverage);
    for (i = 0; i < sub->alternateSetCount; i++)
        free (sub->alternateSet[i].alternate);
    free (sub->alternateSet);
}

static void gsubLoadLigature (LigaturePtr ligature, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "gsubLoadLigature");

    ligature->ligGlyph = ttfGetUSHORT (fp);
    ligature->compCount = ttfGetUSHORT (fp);
    ligature->component = ttfMakeUSHORT (ligature->compCount - 1, fp);
}

static void gsubLoadLigatureSet (LigatureSetPtr ligatureSet, FILE *fp, ULONG offset)
{
    int i;
    USHORT *lOffset;

    xfseek (fp, offset, SEEK_SET, "gsubLoadLigatureSet");

    ligatureSet->ligatureCount = ttfGetUSHORT (fp);
    lOffset = ttfMakeUSHORT (ligatureSet->ligatureCount, fp);
    ligatureSet->ligature = XCALLOC (ligatureSet->ligatureCount, Ligature);
    for (i = 0; i < ligatureSet->ligatureCount; i++)
        gsubLoadLigature (&ligatureSet->ligature[i], fp, offset + lOffset[i]);
    free (lOffset);
}

static Sub41Ptr makeGSUB41 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *lOffset;
    Sub41Ptr sub = XCALLOC1 (Sub41);

    cOffset = ttfGetUSHORT (fp);
    sub->ligSetCount = ttfGetUSHORT (fp);
    lOffset = ttfMakeUSHORT (sub->ligSetCount, fp);
    sub->coverage = otfMakeCoverage (fp, offset + cOffset);
    sub->ligatureSet = XCALLOC (sub->ligSetCount, LigatureSet);
    for (i = 0; i < sub->ligSetCount; i++)
        gsubLoadLigatureSet (&sub->ligatureSet[i], fp, offset + lOffset[i]);
    free (lOffset);

    return sub;
}

static void printGSUB41 (FILE *fp, Sub41Ptr sub)
{
    int i;

    fprintf (fp, " - Ligature Substitution\n\t  ");
    otfPrintCoverage (fp, sub->coverage);
    fprintf (fp, "\t  ligSetCount: %d\n", sub->ligSetCount);
    for (i = 0; i < sub->ligSetCount; i++) {
        int j;

        fprintf (fp, "\t  %2d. ligatureCount: %d\n", i, sub->ligatureSet[i].ligatureCount);
        for (j = 0; j < sub->ligatureSet[i].ligatureCount; j++) {
            int k;

            fprintf (fp, "\t      %2d. ligGlyph: %d, compCount: %d ", j,
                         sub->ligatureSet[i].ligature[j].ligGlyph,
                         sub->ligatureSet[i].ligature[j].compCount);
            for (k = 0; k < sub->ligatureSet[i].ligature[j].compCount - 1; k++)
                fprintf (fp, k == 0 ? "- %d" : ", %d", sub->ligatureSet[i].ligature[j].component[k]);
            fprintf (fp, "\n");
        }
    }
}

static void freeGSUB41 (Sub41Ptr sub)
{
    int i;

    otfFreeCoverage (sub->coverage);
    for (i = 0; i < sub->ligSetCount; i++) {
        int j;

        for (j = 0; j < sub->ligatureSet[i].ligatureCount; j++)
            free (sub->ligatureSet[i].ligature[j].component);
        free (sub->ligatureSet[i].ligature);
    }
    free (sub->ligatureSet);
}

#define makeGSUB51 makeOTFCtx1
#define printGSUB51 printOTFCtx1
#define freeGSUB51 freeOTFCtx1

#define makeGSUB52 makeOTFCtx2
#define printGSUB52 printOTFCtx2
#define freeGSUB52 freeOTFCtx2

#define makeGSUB53 makeOTFCtx3
#define printGSUB53 printOTFCtx3
#define freeGSUB53 freeOTFCtx3

#define makeGSUB61 makeOTFChn1
#define printGSUB61 printOTFChn1
#define freeGSUB61 freeOTFChn1

#define makeGSUB62 makeOTFChn2
#define printGSUB62 printOTFChn2
#define freeGSUB62 freeOTFChn2

#define makeGSUB63 makeOTFChn3
#define printGSUB63 printOTFChn3
#define freeGSUB63 freeOTFChn3

static Sub81Ptr makeGSUB81 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *bOffset, *lOffset;
    Sub81Ptr sub = XCALLOC1 (Sub81);

    cOffset = ttfGetUSHORT (fp);
    sub->backtrackGlyphCount = ttfGetUSHORT (fp);
    bOffset = ttfMakeUSHORT (sub->backtrackGlyphCount, fp);
    sub->lookaheadGlyphCount = ttfGetUSHORT (fp);
    lOffset = ttfMakeUSHORT (sub->lookaheadGlyphCount, fp);
    sub->glyphCount = ttfGetUSHORT (fp);
    sub->substitute = ttfMakeUSHORT (sub->glyphCount, fp);
    sub->coverage = otfMakeCoverage (fp, offset + cOffset);
    sub->backtrack = XCALLOC (sub->backtrackGlyphCount, CoveragePtr);
    for (i = 0; i < sub->backtrackGlyphCount; i++)
        sub->backtrack[i] = otfMakeCoverage (fp, offset + bOffset[i]);
    free (bOffset);
    sub->lookahead = XCALLOC (sub->lookaheadGlyphCount, CoveragePtr);
    for (i = 0; i < sub->lookaheadGlyphCount; i++)
        sub->lookahead[i] = otfMakeCoverage (fp, offset + lOffset[i]);
    free (lOffset);

    return sub;
}

static void printGSUB81 (FILE *fp, Sub81Ptr sub)
{
    int i;

    fprintf (fp, " - Reverse Chaining Context Single Substitution\n\t  ");
    otfPrintCoverage (fp, sub->coverage);
    fprintf (fp, "\t  backtrackGlyphCount: %d\n", sub->backtrackGlyphCount);
    for (i = 0; i < sub->backtrackGlyphCount; i++) {
        fprintf (fp, "\t  %2d. backtrack", i);
        otfPrintCoverage (fp, sub->backtrack[i]);
    }
    fprintf (fp, "\t  lookaheadGlyphCount: %d\n", sub->lookaheadGlyphCount);
    for (i = 0; i < sub->lookaheadGlyphCount; i++) {
        fprintf (fp, "\t  %2d. lookahead", i);
        otfPrintCoverage (fp, sub->lookahead[i]);
    }
    fprintf (fp, "\t  glyphCount: %d\n\t  substitute - %d",
                 sub->glyphCount, sub->substitute[0]);
    for (i = 1; i < sub->glyphCount; i++)
        fprintf (fp, i % 8 ? ",\n\t\t       %d" : ", %d", sub->substitute[i]);
}

static void freeGSUB81 (Sub81Ptr sub)
{
    int i;

    otfFreeCoverage (sub->coverage);
    for (i = 0; i < sub->backtrackGlyphCount; i++)
        otfFreeCoverage (sub->backtrack[i]);
    free (sub->backtrack);
    for (i = 0; i < sub->lookaheadGlyphCount; i++)
        otfFreeCoverage (sub->lookahead[i]);
    free (sub->lookahead);
    free (sub->substitute);
}

static LookupPtr makeGSUBLookup (FILE *fp, USHORT lookupType, ULONG offset)
{
    LookupPtr lookup;
    USHORT lookupFormat;

again:
    if (lookupType == 0 || lookupType > SubLookup_Max)
        ttfError ("Unrecognized GSUB lookupType\n");

    xfseek (fp, offset, SEEK_SET, "makeGSUBLookup");

    lookupFormat = ttfGetUSHORT (fp);
    if (lookupFormat & 0xfff0)
        lookupFormat = 0x000f;

    switch (lookupType << 4 | lookupFormat)
        {
        case 0x011:
            lookup.sub.sub11 = makeGSUB11 (fp, offset);
            break;
        case 0x012:
            lookup.sub.sub12 = makeGSUB12 (fp, offset);
            break;
        case 0x021:
            lookup.sub.sub21 = makeGSUB21 (fp, offset);
            break;
        case 0x031:
            lookup.sub.sub31 = makeGSUB31 (fp, offset);
            break;
        case 0x041:
            lookup.sub.sub41 = makeGSUB41 (fp, offset);
            break;
        case 0x051:
            lookup.sub.sub51 = makeGSUB51 (fp, offset);
            break;
        case 0x052:
            lookup.sub.sub52 = makeGSUB52 (fp, offset);
            break;
        case 0x053:
            lookup.sub.sub53 = makeGSUB53 (fp, offset);
            break;
        case 0x061:
            lookup.sub.sub61 = makeGSUB61 (fp, offset);
            break;
        case 0x062:
            lookup.sub.sub62 = makeGSUB62 (fp, offset);
            break;
        case 0x063:
            lookup.sub.sub63 = makeGSUB63 (fp, offset);
            break;
        case 0x081:
            lookup.sub.sub81 = makeGSUB81 (fp, offset);
            break;
        case 0x071:
            lookupType = ttfGetUSHORT (fp);
            if (lookupType == 7)
                ttfError ("Invalid GSUB extensionLookupType\n");
            offset += ttfGetULONG (fp);
            goto again;
        default:
            ttfError ("Unrecognized GSUB lookupFormat\n");
        }

    lookup.otf->lookupType = lookupType;
    lookup.otf->lookupFormat = lookupFormat;

    return lookup;
}

static void printGSUBLookup (FILE *fp, LookupPtr lookup)
{
    switch (lookup.otf->lookupType << 4 | lookup.otf->lookupFormat)
        {
        case 0x011:
            printGSUB11 (fp, lookup.sub.sub11);
            break;
        case 0x012:
            printGSUB12 (fp, lookup.sub.sub12);
            break;
        case 0x021:
            printGSUB21 (fp, lookup.sub.sub21);
            break;
        case 0x031:
            printGSUB31 (fp, lookup.sub.sub31);
            break;
        case 0x041:
            printGSUB41 (fp, lookup.sub.sub41);
            break;
        case 0x051:
            printGSUB51 (fp, lookup.sub.sub51);
            break;
        case 0x052:
            printGSUB52 (fp, lookup.sub.sub52);
            break;
        case 0x053:
            printGSUB53 (fp, lookup.sub.sub53);
            break;
        case 0x061:
            printGSUB61 (fp, lookup.sub.sub61);
            break;
        case 0x062:
            printGSUB62 (fp, lookup.sub.sub62);
            break;
        case 0x063:
            printGSUB63 (fp, lookup.sub.sub63);
            break;
        case 0x081:
            printGSUB81 (fp, lookup.sub.sub81);
            break;
        default:
            ttfError ("Internal error: printGSUBLookup\n");
        }
}

static void freeGSUBLookup (LookupPtr lookup)
{
    switch (lookup.otf->lookupType << 4 | lookup.otf->lookupFormat)
        {
        case 0x011:
            freeGSUB11 (lookup.sub.sub11);
            break;
        case 0x012:
            freeGSUB12 (lookup.sub.sub12);
            break;
        case 0x021:
            freeGSUB21 (lookup.sub.sub21);
            break;
        case 0x031:
            freeGSUB31 (lookup.sub.sub31);
            break;
        case 0x041:
            freeGSUB41 (lookup.sub.sub41);
            break;
        case 0x051:
            freeGSUB51 (lookup.sub.sub51);
            break;
        case 0x052:
            freeGSUB52 (lookup.sub.sub52);
            break;
        case 0x053:
            freeGSUB53 (lookup.sub.sub53);
            break;
        case 0x061:
            freeGSUB61 (lookup.sub.sub61);
            break;
        case 0x062:
            freeGSUB62 (lookup.sub.sub62);
            break;
        case 0x063:
            freeGSUB63 (lookup.sub.sub63);
            break;
        case 0x081:
            freeGSUB81 (lookup.sub.sub81);
            break;
        default:
            ttfError ("Internal error: freeGSUBLookup\n");
        }
}

static void ttfLoadGSUB (FILE *fp, GSUBPtr gsub, ULONG offset)
{
    USHORT sOffset, fOffset, lOffset;

    xfseek (fp, offset, SEEK_SET, "ttfLoadGSUB");

    gsub->version = ttfGetFixed (fp);
    sOffset = ttfGetUSHORT (fp);
    fOffset = ttfGetUSHORT (fp);
    lOffset = ttfGetUSHORT (fp);

    gsub->scriptList = otfMakeScriptList (fp, offset + sOffset);
    gsub->featureList = otfMakeFeatureList (fp, offset + fOffset);
    gsub->lookupList = otfMakeLookupList (fp, offset + lOffset, &makeGSUBLookup);
}

void ttfInitGSUB (TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('G', 'S', 'U', 'B');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir (tag, font)) != NULL)
        {
            font->gsub = XCALLOC1 (GSUB);
            ttfLoadGSUB (font->fp, font->gsub, ptd->offset);
        }
}

void ttfPrintGSUB (FILE *fp, GSUBPtr gsub)
{
    int b[2];

    FixedSplit (gsub->version, b);

    fprintf (fp, "'GSUB' Table - Glyph Substitution Data\n");
    fprintf (fp, "--------------------------------------\n");
    fprintf (fp, "\t 'GSUB' Version:\t %d.%d\n",b[1],b[0]);

    otfPrintScriptList (fp, gsub->scriptList);
    otfPrintFeatureList (fp, gsub->featureList);
    otfPrintLookupList (fp, gsub->lookupList, &printGSUBLookup);
}

void ttfFreeGSUB (GSUBPtr gsub)
{
    if (gsub != NULL)
        {
            otfFreeScriptList (gsub->scriptList);
            otfFreeFeatureList (gsub->featureList);
            otfFreeLookupList (gsub->lookupList, &freeGSUBLookup);
            free (gsub);
        }
}
