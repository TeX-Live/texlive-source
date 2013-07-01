/* gpos.c -- Glyph Positioning Table
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

static USHORT getValueFormat (FILE *fp)
{
    USHORT valueFormat = ttfGetUSHORT (fp);

    if (valueFormat & ValueFormat_Reserved)
        ttfError ("Unrecognized GPOS valueFormat\n");

    return valueFormat;
}

static ValueRecordPtr
gposMakeValueRecord (USHORT valueFormat, FILE *fp)
{
    int i;
    ValueRecordPtr value;

    if (valueFormat == 0)
        return NULL;

    value = XCALLOC1 (ValueRecord);
    for (i = 0; i < 4; i++)
        if (valueFormat & (ValueFormat_XPlacement << i))
            value->valDesign[i] = ttfGetSHORT (fp);
    for (i = 0; i < 4; i++)
        if (valueFormat & (ValueFormat_XPlaDevice << i))
            value->valDevice[i].offset = ttfGetUSHORT (fp);

    return value;
}

static void
gposLoadValueRecord (ValueRecordPtr value, FILE *fp, ULONG offset)
{
    int i;

    if (value == NULL)
        return;
    for (i = 0; i < 4; i++)
        if (value->valDevice[i].offset)
            value->valDevice[i].device = otfMakeDevice (fp, offset + value->valDevice[i].offset);
}

static const char *txtDesign[4] = {
    "XPlacement",
    "YPlacement",
    "XAdvance",
    "YAdvance"
};

static const char *txtDevice[4] = {
    "XPlaDevice",
    "YPlaDevice",
    "XAdvDevice",
    "YAdvDevice"
};

static void
gposPrintValueRecord (FILE *fp, const char *str, USHORT valueFormat, ValueRecordPtr value)
{
    const char *s = ":";
    int i;

    for (i = 0; i < 4; i++)
        if (valueFormat & (ValueFormat_XPlacement << i)) {
            fprintf (fp, "%s %s = %d\n", s, txtDesign[i], value->valDesign[i]);
            s = str;
        }
    for (i = 0; i < 4; i++)
        if (value->valDevice[i].device) {
            fprintf (fp, "%s %s:", s, txtDevice[i]);
            otfPrintDevice (fp, value->valDevice[i].device);
            s = str;
        }
}

static void freeValueRecord (ValueRecordPtr value)
{
    if (value) {
        int i;

        for (i = 0; i < 4; i++)
            if (value->valDevice[i].device)
                free (value->valDevice[i].device);
        free (value);
    }
}

static AnchorPtr gposMakeAnchor (FILE *fp, ULONG offset)
{
    AnchorPtr anchor;
    USHORT anchorFormat;
    USHORT xOffset, yOffset;

    xfseek (fp, offset, SEEK_SET, "gposMakeAnchor");

    anchorFormat = ttfGetUSHORT (fp);
    switch (anchorFormat)
        {
        case 1:
            anchor.anchor1 = XCALLOC1 (Anchor1);
            break;
        case 2:
            anchor.anchor2 = XCALLOC1 (Anchor2);
            break;
        case 3:
            anchor.anchor3 = XCALLOC1 (Anchor3);
            break;
        default:
            ttfError ("Unrecognized GPOS anchorFormat\n");
        }
    anchor.anchor1->anchorFormat = anchorFormat;
    anchor.anchor1->xCoordinate = ttfGetSHORT (fp);
    anchor.anchor1->yCoordinate = ttfGetSHORT (fp);

    switch (anchorFormat)
        {
        case 2:
            anchor.anchor2->anchorPoint = ttfGetUSHORT (fp);
            break;
        case 3:
            xOffset = ttfGetUSHORT (fp);
            yOffset = ttfGetUSHORT (fp);
            if (xOffset)
                anchor.anchor3->xDevice = otfMakeDevice (fp, offset + xOffset);
            if (yOffset)
                anchor.anchor3->yDevice = otfMakeDevice (fp, offset + yOffset);
            break;
        }

    return anchor;
}

static void
gposPrintAnchor (FILE *fp, const char *str, AnchorPtr anchor)
{
    fprintf (fp, "anchorFormat = %d, xCoordinate = %d, yCoordinate = %d\n",
                 anchor.anchor1->anchorFormat, anchor.anchor1->xCoordinate, anchor.anchor1->yCoordinate);

    switch (anchor.anchor1->anchorFormat)
        {
        case 2:
            fprintf (fp, "%sanchorPoint = %d\n", str, anchor.anchor2->anchorPoint);
            break;
        case 3:
            if (anchor.anchor3->xDevice) {
                fprintf (fp, "%sxDevice: ", str);
                otfPrintDevice (fp, anchor.anchor3->xDevice);
            }
            if (anchor.anchor3->yDevice) {
                fprintf (fp, "%syDevice: ", str);
                otfPrintDevice (fp, anchor.anchor3->yDevice);
            }
            break;
        }
}

static void freeAnchor (AnchorPtr anchor)
{
    if (anchor.anchor3) {
        if (anchor.anchor3->anchorFormat == 3) {
            if (anchor.anchor3->xDevice)
                free (anchor.anchor3->xDevice);
            if (anchor.anchor3->yDevice)
                free (anchor.anchor3->yDevice);
        }
        free (anchor.anchor3);
    }
}

static MarkRecordPtr
gposMakeMarkArray (FILE *fp, USHORT *markCount, ULONG offset)
{
    int i;
    MarkRecordPtr markArray;
    USHORT *aOffset;

    xfseek (fp, offset, SEEK_SET, "gposMakeMarkArray");

    *markCount = ttfGetUSHORT (fp);
    markArray = XCALLOC (*markCount, MarkRecord);
    aOffset = XCALLOC (*markCount, USHORT);
    for (i = 0; i < *markCount; i++) {
        markArray[i].class = ttfGetUSHORT (fp);
        aOffset[i] = ttfGetUSHORT (fp);
    }
    for (i = 0; i < *markCount; i++)
        markArray[i].markAnchor = gposMakeAnchor (fp, offset + aOffset[i]);
    free (aOffset);

    return markArray;
}

static void
gposPrintMarkArray (FILE *fp, USHORT markCount, MarkRecordPtr markArray)
{
    int i;

    for (i = 0; i < markCount; i++) {
        fprintf (fp, "\t  %2d. class: %d - ", i, markArray[i].class);
        gposPrintAnchor (fp, "\t\t  ", markArray[i].markAnchor);
    }
}

/* Used for both pos41->baseArray and pos61->mark2Array.  */
static AnchorPtr *
gposMakeBaseArray (FILE *fp, USHORT *baseCount, USHORT classCount, ULONG offset)
{
    int i;
    AnchorPtr *baseArray;
    USHORT *aOffset;

    xfseek (fp, offset, SEEK_SET, "gposMakeBaseArray");

    *baseCount = ttfGetUSHORT (fp);
    aOffset = ttfMakeUSHORT (*baseCount * classCount, fp);
    baseArray = XCALLOC (*baseCount * classCount, AnchorPtr);
    for (i = 0; i < *baseCount * classCount; i++)
        if (aOffset[i])
            baseArray[i] = gposMakeAnchor (fp, offset + aOffset[i]);
    free (aOffset);

    return baseArray;
}

static Pos11Ptr makeGPOS11 (FILE *fp, ULONG offset)
{
    USHORT cOffset;
    Pos11Ptr pos = XCALLOC1 (Pos11);

    cOffset = ttfGetUSHORT (fp);
    pos->valueFormat = getValueFormat (fp);
    pos->value = gposMakeValueRecord (pos->valueFormat, fp);
    pos->coverage = otfMakeCoverage (fp, offset + cOffset);
    gposLoadValueRecord (pos->value, fp, offset);

    return pos;
}

static void printGPOS11 (FILE *fp, Pos11Ptr pos)
{
    fprintf (fp, " - Single Adjustment Value\n\t  ");
    otfPrintCoverage (fp, pos->coverage);
    fprintf (fp, "\t  valueFormat: 0x%04x\n", pos->valueFormat);
    if (pos->valueFormat) {
        fprintf (fp, "\t  value");
        gposPrintValueRecord (fp, "\t\t", pos->valueFormat, pos->value);
    }
}

static void freeGPOS11 (Pos11Ptr pos)
{
    freeValueRecord (pos->value);
    otfFreeCoverage (pos->coverage);
}

static Pos12Ptr makeGPOS12 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    Pos12Ptr pos = XCALLOC1 (Pos12);

    cOffset = ttfGetUSHORT (fp);
    pos->valueFormat = getValueFormat (fp);
    pos->valueCount = ttfGetUSHORT (fp);
    pos->value = XCALLOC (pos->valueCount, ValueRecordPtr);
    for (i = 0; i < pos->valueCount; i++)
        pos->value[i] = gposMakeValueRecord (pos->valueFormat, fp);
    pos->coverage = otfMakeCoverage (fp, offset + cOffset);
    for (i = 0; i < pos->valueCount; i++)
        gposLoadValueRecord (pos->value[i], fp, offset);

    return pos;
}

static void printGPOS12 (FILE *fp, Pos12Ptr pos)
{
    int i;

    fprintf (fp, " - Single Adjustment List\n\t  ");
    otfPrintCoverage (fp, pos->coverage);
    fprintf (fp, "\t  valueFormat: 0x%04x, valueCount: %d\n", pos->valueFormat, pos->valueCount);
    for (i = 0; i < pos->valueCount; i++) {
        fprintf (fp, "\t  %2d. value", i);
        gposPrintValueRecord (fp, "\t\t    ", pos->valueFormat, pos->value[i]);
    }
}

static void freeGPOS12 (Pos12Ptr pos)
{
    int i;

    for (i = 0; i < pos->valueCount; i++)
        freeValueRecord (pos->value[i]);
    free (pos->value);
    otfFreeCoverage (pos->coverage);
}

static void gposLoadPairSet (Pos21Ptr pos, int i, FILE *fp, ULONG offset)
{
    int j;
    PairSetPtr pairSet = &pos->pairSet[i];

    xfseek (fp, offset, SEEK_SET, "gposLoadPairSet");

    pairSet->pairValueCount = ttfGetUSHORT (fp);
    pairSet->pairValue = XCALLOC (pairSet->pairValueCount, PairValueRecord);
    for (j = 0; j < pairSet->pairValueCount; j++) {
        pairSet->pairValue[j].secondGlyph = ttfGetUSHORT (fp);
        pairSet->pairValue[j].value1 = gposMakeValueRecord (pos->valueFormat1, fp);
        pairSet->pairValue[j].value2 = gposMakeValueRecord (pos->valueFormat2, fp);
    }
}

static Pos21Ptr makeGPOS21 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *pOffset;
    Pos21Ptr pos = XCALLOC1 (Pos21);

    cOffset = ttfGetUSHORT (fp);
    pos->valueFormat1 = getValueFormat (fp);
    pos->valueFormat2 = getValueFormat (fp);
    pos->pairSetCount = ttfGetUSHORT (fp);
    pOffset = ttfMakeUSHORT (pos->pairSetCount, fp);
    pos->coverage = otfMakeCoverage (fp, offset + cOffset);
    pos->pairSet = XCALLOC (pos->pairSetCount, PairSet);
    for (i = 0; i < pos->pairSetCount; i++)
        gposLoadPairSet (pos, i, fp, offset + pOffset[i]);
    free (pOffset);
    for (i = 0; i < pos->pairSetCount; i++) {
        int j;

        for (j = 0; j < pos->pairSet[i].pairValueCount; j++) {
            gposLoadValueRecord (pos->pairSet[i].pairValue[j].value1, fp, offset);
            gposLoadValueRecord (pos->pairSet[i].pairValue[j].value2, fp, offset);
        }
    }

    return pos;
}

static void printGPOS21 (FILE *fp, Pos21Ptr pos)
{
    int i;

    fprintf (fp, " - Pair Adjustment List\n\t  ");
    otfPrintCoverage (fp, pos->coverage);
    fprintf (fp, "\t  valueFormat1: 0x%04x, valueFormat2: 0x%04x, pairSetCount: %d\n",
                 pos->valueFormat1, pos->valueFormat2, pos->pairSetCount);
    for (i = 0; i < pos->pairSetCount; i++) {
        int j;

        fprintf (fp, "\t  %2d. pairValueCount: %d\n", i, pos->pairSet[i].pairValueCount);
        for (j = 0; j < pos->pairSet[i].pairValueCount; j++) {
            fprintf (fp, "\t      %2d. secondGlyph: %d\n", j,
                         pos->pairSet[i].pairValue[j].secondGlyph);
            if (pos->valueFormat1) {
                fprintf (fp, "\t\t  value1");
                gposPrintValueRecord (fp, "\t\t\t ", pos->valueFormat1,
                                          pos->pairSet[i].pairValue[j].value1);
            }
            if (pos->valueFormat2) {
                fprintf (fp, "\t\t  value2");
                gposPrintValueRecord (fp, "\t\t\t ", pos->valueFormat2,
                                          pos->pairSet[i].pairValue[j].value2);
            }
        }
    }
}

static void freeGPOS21 (Pos21Ptr pos)
{
    int i;

    for (i = 0; i < pos->pairSetCount; i++) {
        int j;
        for (j = 0; j < pos->pairSet[i].pairValueCount; j++) {
            freeValueRecord (pos->pairSet[i].pairValue[j].value1);
            freeValueRecord (pos->pairSet[i].pairValue[j].value2);
        }
        free (pos->pairSet[i].pairValue);
    }
    free (pos->pairSet);
    otfFreeCoverage (pos->coverage);
}

static Pos22Ptr makeGPOS22 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset, cOffset1, cOffset2;
    Pos22Ptr pos = XCALLOC1 (Pos22);

    cOffset = ttfGetUSHORT (fp);
    pos->valueFormat1 = getValueFormat (fp);
    pos->valueFormat2 = getValueFormat (fp);
    cOffset1 = ttfGetUSHORT (fp);
    cOffset2 = ttfGetUSHORT (fp);
    pos->class1Count = ttfGetUSHORT (fp);
    pos->class2Count = ttfGetUSHORT (fp);
    pos->values = XCALLOC (2 * pos->class1Count * pos->class2Count, ValueRecordPtr);
    for (i = 0; i < 2 * pos->class1Count * pos->class2Count; i++)
        pos->values[i] = gposMakeValueRecord (i & 1 ? pos->valueFormat2 : pos->valueFormat1, fp);
    pos->coverage = otfMakeCoverage (fp, offset + cOffset);
    pos->classDef1 = otfMakeClassDef (fp, offset + cOffset1);
    pos->classDef2 = otfMakeClassDef (fp, offset + cOffset2);
    for (i = 0; i < 2 * pos->class1Count * pos->class2Count; i++)
        gposLoadValueRecord (pos->values[i], fp, offset);

    return pos;
}

static void printGPOS22 (FILE *fp, Pos22Ptr pos)
{
    int i, num = 0;

    fprintf (fp, " - Pair Adjustment Class\n\t  ");
    otfPrintCoverage (fp, pos->coverage);
    fprintf (fp, "\t  valueFormat1: 0x%04x, valueFormat2: 0x%04x\n",
                 pos->valueFormat1, pos->valueFormat2);
    fprintf (fp, "\t  ClassDef1 - ");
    otfPrintClassDef (fp, pos->classDef1);
    fprintf (fp, "\t  ClassDef2 - ");
    otfPrintClassDef (fp, pos->classDef2);
    fprintf (fp, "\t  class1Count: %d, class2Count: %d\n",
                 pos->class1Count, pos->class2Count);
    for (i = 0; i < pos->class1Count; i++) {
        int j;
        const char *s = "";

        fprintf (fp, "\t  %2d.", i);
        for (j = 0; j < pos->class2Count; j++) {
            fprintf (fp, "%s %2d.", s, j);
            s = "\t     ";
            
            if (pos->valueFormat1) {
                fprintf (fp, " value1");
                gposPrintValueRecord (fp, "\t\t\t ", pos->valueFormat1,
                                          pos->values[num]);
            }
            num++;
            if (pos->valueFormat2) {
                fprintf (fp, " value2");
                gposPrintValueRecord (fp, "\t\t\t ", pos->valueFormat2,
                                          pos->values[num]);
            }
            num++;
        }
    }
}

static void freeGPOS22 (Pos22Ptr pos)
{
    int i;

    for (i = 0; i < 2 * pos->class1Count * pos->class2Count; i++)
        freeValueRecord (pos->values[i]);
    free (pos->values);
    otfFreeCoverage (pos->coverage);
    otfFreeClassDef (pos->classDef1);
    otfFreeClassDef (pos->classDef2);
}

static Pos31Ptr makeGPOS31 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *eOffset;
    Pos31Ptr pos = XCALLOC1 (Pos31);

    cOffset = ttfGetUSHORT (fp);
    pos->entryExitCount = ttfGetUSHORT (fp);
    eOffset = ttfMakeUSHORT (2 * pos->entryExitCount, fp);
    pos->coverage = otfMakeCoverage (fp, offset + cOffset);
    pos->entryExit = XCALLOC (2 * pos->entryExitCount, AnchorPtr);
    for (i = 0; i < 2 * pos->entryExitCount; i++)
        if (eOffset[i])
            pos->entryExit[i] = gposMakeAnchor (fp, offset + eOffset[i]);
    free (eOffset);

    return pos;
}

static void printGPOS31 (FILE *fp, Pos31Ptr pos)
{
    int i, num = 0;

    fprintf (fp, " - Cursive Attachment\n\t  ");
    otfPrintCoverage (fp, pos->coverage);
    fprintf (fp, "\t  entryExitCount: %d\n", pos->entryExitCount);
    for (i = 0; i < pos->entryExitCount; i++) {
        int j;
        const char *s = "";

        fprintf (fp, "\t  %2d. ", i);
        for (j = 0; j < 2; j++) {
            if (pos->entryExit[num].anchor1) {
                fprintf (fp, "%s%sAnchor - ", s, j ? "exit" : "entry");
                gposPrintAnchor (fp, "\t\t  ", pos->entryExit[num]);
                s = "\t      ";
            }
            num++;
        }
    }
}

static void freeGPOS31 (Pos31Ptr pos)
{
    int i;

    for (i = 0; i < 2 * pos->entryExitCount; i++)
        freeAnchor (pos->entryExit[i]);
    free (pos->entryExit);
    otfFreeCoverage (pos->coverage);
}

static Pos41Ptr makeGPOS41 (FILE *fp, ULONG offset)
{
    USHORT mcOffset, bcOffset, maOffset, baOffset;
    Pos41Ptr pos = XCALLOC1 (Pos41);

    mcOffset = ttfGetUSHORT (fp);
    bcOffset = ttfGetUSHORT (fp);
    pos->classCount = ttfGetUSHORT (fp);
    maOffset = ttfGetUSHORT (fp);
    baOffset = ttfGetUSHORT (fp);
    pos->markCoverage = otfMakeCoverage (fp, offset + mcOffset);
    pos->baseCoverage = otfMakeCoverage (fp, offset + bcOffset);
    pos->markArray = gposMakeMarkArray (fp, &pos->markCount, offset + maOffset);
    pos->baseArray = gposMakeBaseArray (fp, &pos->baseCount, pos->classCount, offset + baOffset);

    return pos;
}

static void printGPOS41 (FILE *fp, Pos41Ptr pos)
{
    int i, num = 0;

    fprintf (fp, " - Mark To Base Attachment\n\t  mark");
    otfPrintCoverage (fp, pos->markCoverage);
    fprintf (fp, "\t  markArray - markCount: %d\n", pos->markCount);
    gposPrintMarkArray (fp, pos->markCount, pos->markArray);
    fprintf (fp, "\t  base");
    otfPrintCoverage (fp, pos->baseCoverage);
    fprintf (fp, "\t  baseArray - baseCount: %d, classCount: %d\n",
                 pos->baseCount, pos->classCount);
    for (i = 0; i < pos->baseCount; i++) {
        int j;
        const char *s = "";

        fprintf (fp, "\t  %2d. ", i);
        for (j = 0; j < pos->classCount; j++) {
            if (pos->baseArray[num].anchor1) {
                fprintf (fp, "%s%2d. ", s, j);
                gposPrintAnchor (fp, "\t\t  ", pos->baseArray[num]);
                s = "\t      ";
            }
            num++;
        }
    }
}

static void freeGPOS41 (Pos41Ptr pos)
{
    int i;

    for (i = 0; i < pos->markCount; i++)
        freeAnchor (pos->markArray[i].markAnchor);
    free (pos->markArray);
    for (i = 0; i < pos->baseCount * pos->classCount; i++)
        freeAnchor (pos->baseArray[i]);
    free (pos->baseArray);
    otfFreeCoverage (pos->markCoverage);
    otfFreeCoverage (pos->baseCoverage);
}

static void
gposLoadLigatureArray (LigatureAttachPtr ligatureArray, USHORT classCount, FILE *fp, ULONG offset)
{
    int i;
    USHORT *aOffset;

    xfseek (fp, offset, SEEK_SET, "gposLoadLigatureArray");

    ligatureArray->componentCount = ttfGetUSHORT (fp);
    aOffset = ttfMakeUSHORT (ligatureArray->componentCount * classCount, fp);
    ligatureArray->componentRecord = XCALLOC (ligatureArray->componentCount * classCount, AnchorPtr);
    for (i = 0; i < ligatureArray->componentCount * classCount; i++)
        if (aOffset[i])
            ligatureArray->componentRecord[i] = gposMakeAnchor (fp, offset + aOffset[i]);
    free (aOffset);
}

static LigatureAttachPtr
gposMakeLigatureArray (FILE *fp, USHORT *ligatureCount, USHORT classCount, ULONG offset)
{
    int i;
    LigatureAttachPtr ligatureArray;
    USHORT *aOffset;

    xfseek (fp, offset, SEEK_SET, "gposMakeLigatureArray");

    *ligatureCount = ttfGetUSHORT (fp);
    aOffset = ttfMakeUSHORT (*ligatureCount, fp);
    ligatureArray = XCALLOC (*ligatureCount, LigatureAttach);
    for (i = 0; i < *ligatureCount; i++)
        gposLoadLigatureArray (&ligatureArray[i], classCount, fp, offset + aOffset[i]);
    free (aOffset);

    return ligatureArray;
}

static Pos51Ptr makeGPOS51 (FILE *fp, ULONG offset)
{
    USHORT mcOffset, lcOffset, maOffset, laOffset;
    Pos51Ptr pos = XCALLOC1 (Pos51);

    mcOffset = ttfGetUSHORT (fp);
    lcOffset = ttfGetUSHORT (fp);
    pos->classCount = ttfGetUSHORT (fp);
    maOffset = ttfGetUSHORT (fp);
    laOffset = ttfGetUSHORT (fp);
    pos->markCoverage = otfMakeCoverage (fp, offset + mcOffset);
    pos->ligatureCoverage = otfMakeCoverage (fp, offset + lcOffset);
    pos->markArray = gposMakeMarkArray (fp, &pos->markCount, offset + maOffset);
    pos->ligatureArray = gposMakeLigatureArray (fp, &pos->ligatureCount, pos->classCount, offset + laOffset);

    return pos;
}

static void printGPOS51 (FILE *fp, Pos51Ptr pos)
{
    int i;

    fprintf (fp, " - Mark To Ligature Attachment\n\t  mark");
    otfPrintCoverage (fp, pos->markCoverage);
    fprintf (fp, "\t  markArray - markCount: %d\n", pos->markCount);
    gposPrintMarkArray (fp, pos->markCount, pos->markArray);
    fprintf (fp, "\t  ligature");
    otfPrintCoverage (fp, pos->ligatureCoverage);
    fprintf (fp, "\t  ligatureArray - ligatureCount: %d\n", pos->ligatureCount);
    for (i = 0; i < pos->ligatureCount; i++) {
        int j, num = 0;

        fprintf (fp, "\t  %2d. componentCount: %d, classCount: %d\n", i,
                     pos->ligatureArray[i].componentCount, pos->classCount);
        for (j = 0; j < pos->ligatureArray[i].componentCount; j++) {
            int k;
            const char *s = "";

            fprintf (fp, "\t      %2d. ", j);
            for (k = 0; k < pos->classCount; k++) {
                if (pos->ligatureArray[i].componentRecord[num].anchor1) {
                    fprintf (fp, "%s%2d. ", s, k);
                    gposPrintAnchor (fp, "\t\t      ", pos->ligatureArray[i].componentRecord[num]);
                    s = "\t\t  ";
                }
                num++;
            }
        }
    }
}

static void freeGPOS51 (Pos51Ptr pos)
{
    int i;

    for (i = 0; i < pos->markCount; i++)
        freeAnchor (pos->markArray[i].markAnchor);
    free (pos->markArray);
    for (i = 0; i < pos->ligatureCount; i++) {
        int j;

        for (j = 0; j < pos->ligatureArray[i].componentCount * pos->classCount; j++)
            freeAnchor (pos->ligatureArray[i].componentRecord[j]);
    }
    free (pos->ligatureArray);
    otfFreeCoverage (pos->markCoverage);
    otfFreeCoverage (pos->ligatureCoverage);
}

static Pos61Ptr makeGPOS61 (FILE *fp, ULONG offset)
{
    USHORT c1Offset, c2Offset, a1Offset, a2Offset;
    Pos61Ptr pos = XCALLOC1 (Pos61);

    c1Offset = ttfGetUSHORT (fp);
    c2Offset = ttfGetUSHORT (fp);
    pos->classCount = ttfGetUSHORT (fp);
    a1Offset = ttfGetUSHORT (fp);
    a2Offset = ttfGetUSHORT (fp);
    pos->mark1Coverage = otfMakeCoverage (fp, offset + c1Offset);
    pos->mark2Coverage = otfMakeCoverage (fp, offset + c2Offset);
    pos->mark1Array = gposMakeMarkArray (fp, &pos->mark1Count, offset + a1Offset);
    pos->mark2Array = gposMakeBaseArray (fp, &pos->mark2Count, pos->classCount, offset + a2Offset);

    return pos;
}

static void printGPOS61 (FILE *fp, Pos61Ptr pos)
{
    int i, num = 0;

    fprintf (fp, " - Mark To Mark Attachment\n\t  mark1");
    otfPrintCoverage (fp, pos->mark1Coverage);
    fprintf (fp, "\t  mark1Array - mark1Count: %d\n", pos->mark1Count);
    gposPrintMarkArray (fp, pos->mark1Count, pos->mark1Array);
    fprintf (fp, "\t  mark2");
    otfPrintCoverage (fp, pos->mark2Coverage);
    fprintf (fp, "\t  mark2Array - mark2Count: %d, classCount: %d\n",
                 pos->mark2Count, pos->classCount);
    for (i = 0; i < pos->mark2Count; i++) {
        int j;
        const char *s = "";

        fprintf (fp, "\t  %2d. ", i);
        for (j = 0; j < pos->classCount; j++) {
            if (pos->mark2Array[num].anchor1) {
                fprintf (fp, "%s%2d. ", s, j);
                gposPrintAnchor (fp, "\t\t  ", pos->mark2Array[num]);
                s = "\t      ";
            }
            num++;
        }
    }
}

static void freeGPOS61 (Pos61Ptr pos)
{
    int i;

    for (i = 0; i < pos->mark1Count; i++)
        freeAnchor (pos->mark1Array[i].markAnchor);
    free (pos->mark1Array);
    for (i = 0; i < pos->mark2Count * pos->classCount; i++)
        freeAnchor (pos->mark2Array[i]);
    free (pos->mark2Array);
    otfFreeCoverage (pos->mark1Coverage);
    otfFreeCoverage (pos->mark2Coverage);
}

#define makeGPOS71 makeOTFCtx1
#define printGPOS71 printOTFCtx1
#define freeGPOS71 freeOTFCtx1

#define makeGPOS72 makeOTFCtx2
#define printGPOS72 printOTFCtx2
#define freeGPOS72 freeOTFCtx2

#define makeGPOS73 makeOTFCtx3
#define printGPOS73 printOTFCtx3
#define freeGPOS73 freeOTFCtx3

#define makeGPOS81 makeOTFChn1
#define printGPOS81 printOTFChn1
#define freeGPOS81 freeOTFChn1

#define makeGPOS82 makeOTFChn2
#define printGPOS82 printOTFChn2
#define freeGPOS82 freeOTFChn2

#define makeGPOS83 makeOTFChn3
#define printGPOS83 printOTFChn3
#define freeGPOS83 freeOTFChn3

static LookupPtr makeGPOSLookup (FILE *fp, USHORT lookupType, ULONG offset)
{
    LookupPtr lookup;
    USHORT lookupFormat;

again:
    if (lookupType == 0 || lookupType > PosLookup_Max)
        ttfError ("Unrecognized GPOS lookupType\n");

    xfseek (fp, offset, SEEK_SET, "makeGPOSLookup");

    lookupFormat = ttfGetUSHORT (fp);
    if (lookupFormat & 0xfff0)
        lookupFormat = 0x000f;

    switch (lookupType << 4 | lookupFormat)
        {
        case 0x011:
            lookup.pos.pos11 = makeGPOS11 (fp, offset);
            break;
        case 0x012:
            lookup.pos.pos12 = makeGPOS12 (fp, offset);
            break;
        case 0x021:
            lookup.pos.pos21 = makeGPOS21 (fp, offset);
            break;
        case 0x022:
            lookup.pos.pos22 = makeGPOS22 (fp, offset);
            break;
        case 0x031:
            lookup.pos.pos31 = makeGPOS31 (fp, offset);
            break;
        case 0x041:
            lookup.pos.pos41 = makeGPOS41 (fp, offset);
            break;
        case 0x051:
            lookup.pos.pos51 = makeGPOS51 (fp, offset);
            break;
        case 0x061:
            lookup.pos.pos61 = makeGPOS61 (fp, offset);
            break;
        case 0x071:
            lookup.pos.pos71 = makeGPOS71 (fp, offset);
            break;
        case 0x072:
            lookup.pos.pos72 = makeGPOS72 (fp, offset);
            break;
        case 0x073:
            lookup.pos.pos73 = makeGPOS73 (fp, offset);
            break;
        case 0x081:
            lookup.pos.pos81 = makeGPOS81 (fp, offset);
            break;
        case 0x082:
            lookup.pos.pos82 = makeGPOS82 (fp, offset);
            break;
        case 0x083:
            lookup.pos.pos83 = makeGPOS83 (fp, offset);
            break;
        case 0x091:
            lookupType = ttfGetUSHORT (fp);
            if (lookupType == 9)
                ttfError ("Invalid GPOS extensionLookupType\n");
            offset += ttfGetULONG (fp);
            goto again;
        default:
            ttfError ("Unrecognized GPOS lookupFormat\n");
        }

    lookup.otf->lookupType = lookupType;
    lookup.otf->lookupFormat = lookupFormat;

    return lookup;
}

static void printGPOSLookup (FILE *fp, LookupPtr lookup)
{
    switch (lookup.otf->lookupType << 4 | lookup.otf->lookupFormat)
        {
        case 0x011:
            printGPOS11 (fp, lookup.pos.pos11);
            break;
        case 0x012:
            printGPOS12 (fp, lookup.pos.pos12);
            break;
        case 0x021:
            printGPOS21 (fp, lookup.pos.pos21);
            break;
        case 0x022:
            printGPOS22 (fp, lookup.pos.pos22);
            break;
        case 0x031:
            printGPOS31 (fp, lookup.pos.pos31);
            break;
        case 0x041:
            printGPOS41 (fp, lookup.pos.pos41);
            break;
        case 0x051:
            printGPOS51 (fp, lookup.pos.pos51);
            break;
        case 0x061:
            printGPOS61 (fp, lookup.pos.pos61);
            break;
        case 0x071:
            printGPOS71 (fp, lookup.pos.pos71);
            break;
        case 0x072:
            printGPOS72 (fp, lookup.pos.pos72);
            break;
        case 0x073:
            printGPOS73 (fp, lookup.pos.pos73);
            break;
        case 0x081:
            printGPOS81 (fp, lookup.pos.pos81);
            break;
        case 0x082:
            printGPOS82 (fp, lookup.pos.pos82);
            break;
        case 0x083:
            printGPOS83 (fp, lookup.pos.pos83);
            break;
        default:
            ttfError ("Internal error: printGPOSLookup\n");
        }
}

static void freeGPOSLookup (LookupPtr lookup)
{
    switch (lookup.otf->lookupType << 4 | lookup.otf->lookupFormat)
        {
        case 0x011:
            freeGPOS11 (lookup.pos.pos11);
            break;
        case 0x012:
            freeGPOS12 (lookup.pos.pos12);
            break;
        case 0x021:
            freeGPOS21 (lookup.pos.pos21);
            break;
        case 0x022:
            freeGPOS22 (lookup.pos.pos22);
            break;
        case 0x031:
            freeGPOS31 (lookup.pos.pos31);
            break;
        case 0x041:
            freeGPOS41 (lookup.pos.pos41);
            break;
        case 0x051:
            freeGPOS51 (lookup.pos.pos51);
            break;
        case 0x061:
            freeGPOS61 (lookup.pos.pos61);
            break;
        case 0x071:
            freeGPOS71 (lookup.pos.pos71);
            break;
        case 0x072:
            freeGPOS72 (lookup.pos.pos72);
            break;
        case 0x073:
            freeGPOS73 (lookup.pos.pos73);
            break;
        case 0x081:
            freeGPOS81 (lookup.pos.pos81);
            break;
        case 0x082:
            freeGPOS82 (lookup.pos.pos82);
            break;
        case 0x083:
            freeGPOS83 (lookup.pos.pos83);
            break;
        default:
            ttfError ("Internal error: freeGPOSLookup\n");
        }
}

static void ttfLoadGPOS (FILE *fp, GPOSPtr gpos, ULONG offset)
{
    USHORT sOffset, fOffset, lOffset;

    xfseek (fp, offset, SEEK_SET, "ttfLoadGPOS");

    gpos->version = ttfGetFixed (fp);
    sOffset = ttfGetUSHORT (fp);
    fOffset = ttfGetUSHORT (fp);
    lOffset = ttfGetUSHORT (fp);

    gpos->scriptList = otfMakeScriptList (fp, offset + sOffset);
    gpos->featureList = otfMakeFeatureList (fp, offset + fOffset);
    gpos->lookupList = otfMakeLookupList (fp, offset + lOffset, &makeGPOSLookup);
}

void ttfInitGPOS (TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('G', 'P', 'O', 'S');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir (tag, font)) != NULL)
        {
            font->gpos = XCALLOC1 (GPOS);
            ttfLoadGPOS (font->fp, font->gpos, ptd->offset);
        }
}

void ttfPrintGPOS (FILE *fp, GPOSPtr gpos)
{
    int b[2];

    FixedSplit (gpos->version, b);

    fprintf (fp, "'GPOS' Table - Glyph Positioning Data\n");
    fprintf (fp, "-------------------------------------\n");
    fprintf (fp, "\t 'GPOS' Version:\t %d.%d\n",b[1],b[0]);

    otfPrintScriptList (fp, gpos->scriptList);
    otfPrintFeatureList (fp, gpos->featureList);
    otfPrintLookupList (fp, gpos->lookupList, &printGPOSLookup);
}

void ttfFreeGPOS (GPOSPtr gpos)
{
    if (gpos != NULL)
        {
            otfFreeScriptList (gpos->scriptList);
            otfFreeFeatureList (gpos->featureList);
            otfFreeLookupList (gpos->lookupList, &freeGPOSLookup);
            free (gpos);
        }
}
