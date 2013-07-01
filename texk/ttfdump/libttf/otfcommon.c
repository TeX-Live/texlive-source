/* otfcommon.c -- OpenType Common Table Formats
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

static LangSysPtr
otfMakeLangSys (FILE *fp, ULONG offset)
{
    LangSysPtr langSys = XCALLOC1 (LangSys);
    USHORT lookupOrder;

    xfseek (fp, offset, SEEK_SET, "otfMakeLangSys");

    if ((lookupOrder = ttfGetUSHORT (fp)) != 0)
        ttfError ("Non-zero lookupOrder\n");
    langSys->reqFeatureIndex = ttfGetUSHORT (fp);
    langSys->featureCount = ttfGetUSHORT (fp);
    langSys->featureIndex = ttfMakeUSHORT (langSys->featureCount, fp);

    return langSys;
}

static void
otfLoadScriptRecord (FILE *fp, ScriptRecordPtr scriptRecord, ULONG offset)
{
    int i;
    USHORT dOffset;
    USHORT *lOffset;

    xfseek (fp, offset, SEEK_SET, "otfLoadScriptRecord");

    dOffset = ttfGetUSHORT (fp);

    scriptRecord->langSysCount = ttfGetUSHORT (fp);
    if (scriptRecord->langSysCount) {
        scriptRecord->langSysRecord = XCALLOC (scriptRecord->langSysCount, LangSysRecord);
        lOffset = XTALLOC (scriptRecord->langSysCount, USHORT);

        for (i = 0; i < scriptRecord->langSysCount; i++) {
            scriptRecord->langSysRecord[i].tag = ttfGetULONG (fp);
            lOffset[i] = ttfGetUSHORT (fp);
        }

        for (i = 0; i < scriptRecord->langSysCount; i++)
            scriptRecord->langSysRecord[i].langSys = otfMakeLangSys (fp, offset + lOffset[i]);

        free (lOffset);
    }

    if (dOffset)
        scriptRecord->defaultLangSys = otfMakeLangSys (fp, offset + dOffset);
}

ScriptListPtr
otfMakeScriptList (FILE *fp, ULONG offset)
{
    int i;
    USHORT *sOffset;
    ScriptListPtr scriptList = XCALLOC1 (ScriptList);

    xfseek (fp, offset, SEEK_SET, "otfMakeScriptList");

    scriptList->scriptCount = ttfGetUSHORT (fp);
    scriptList->scriptRecord = XCALLOC (scriptList->scriptCount, ScriptRecord);
    sOffset = XTALLOC (scriptList->scriptCount, USHORT);

    for (i = 0; i < scriptList->scriptCount; i++) {
        scriptList->scriptRecord[i].tag = ttfGetULONG (fp);
        sOffset[i] = ttfGetUSHORT (fp);
    }

    for (i = 0; i < scriptList->scriptCount; i++)
        otfLoadScriptRecord (fp, &scriptList->scriptRecord[i], offset + sOffset[i]);

    free (sOffset);

    return scriptList;
}

static void
otfPrintLangSys (FILE *fp, LangSysPtr langSys)
{
    int i;

    fprintf (fp, " lang - featureCount: %d\n", langSys->featureCount);
    if (langSys->reqFeatureIndex != 0xffff)
        fprintf (fp, "\t\t  reqFeatureIndex: %d\n", langSys->reqFeatureIndex);
    if (langSys->featureCount) {
        fprintf (fp, "\t\t  featureIndex: %d", langSys->featureIndex[0]);
        for (i = 1; i < langSys->featureCount; i++)
            fprintf (fp, i % 8 ? ", %d" : ",\n\t\t\t\t%d",
                         langSys->featureIndex[i]);
        fprintf (fp, "\n");
    }
}

static void
otfPrintScriptRecord (FILE *fp, ScriptRecordPtr scriptRecord)
{
    int i;

    fprintf (fp, "'%s' script - langSysCount: %d\n",
                 TagToStr (scriptRecord->tag), scriptRecord->langSysCount);
    if (scriptRecord->defaultLangSys) {
        fprintf (fp, "\t default");
        otfPrintLangSys (fp, scriptRecord->defaultLangSys);
    }
    for (i = 0; i < scriptRecord->langSysCount; i++) {
        fprintf (fp, "      %2d. '%s'", i,
                     TagToStr (scriptRecord->langSysRecord[i].tag));
        otfPrintLangSys (fp, scriptRecord->langSysRecord[i].langSys);
    }
}

void
otfPrintScriptList (FILE *fp, ScriptListPtr scriptList)
{
    int i;

    fprintf (fp, "    scriptCount: %d\n", scriptList->scriptCount);

    for (i = 0; i < scriptList->scriptCount; i++) {
        fprintf (fp, "  %2d. ", i);
        otfPrintScriptRecord (fp, &scriptList->scriptRecord[i]);
    }
    fprintf (fp, "\n");
}

static void
otfFreeLangSys (LangSysPtr langSys)
{
    free (langSys->featureIndex);
    free (langSys);
}

static void
otfFreeScriptRecord (ScriptRecordPtr scriptRecord)
{
    int i;

    if (scriptRecord->defaultLangSys)
        otfFreeLangSys (scriptRecord->defaultLangSys);
    for (i = 0; i < scriptRecord->langSysCount; i++)
        otfFreeLangSys (scriptRecord->langSysRecord[i].langSys);
        free (scriptRecord->langSysRecord);
}

void
otfFreeScriptList (ScriptListPtr scriptList)
{
    int i;

    for (i = 0; i < scriptList->scriptCount; i++)
        otfFreeScriptRecord (&scriptList->scriptRecord[i]);
    free (scriptList->scriptRecord);
    free (scriptList);
}

static void
otfLoadFeatureRecord (FILE *fp, FeatureRecordPtr featureRecord, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "otfLoadFeatureRecord");

    featureRecord->featureParams = ttfGetUSHORT (fp);
    featureRecord->lookupCount = ttfGetUSHORT (fp);
    featureRecord->lookupListIndex = ttfMakeUSHORT (featureRecord->lookupCount, fp);
}

FeatureListPtr
otfMakeFeatureList (FILE *fp, ULONG offset)
{
    int i;
    USHORT *fOffset;
    FeatureListPtr featureList = XCALLOC1 (FeatureList);

    xfseek (fp, offset, SEEK_SET, "otfMakeFeatureList");

    featureList->featureCount = ttfGetUSHORT (fp);
    if (featureList->featureCount) {
        featureList->featureRecord = XCALLOC (featureList->featureCount, FeatureRecord);
        fOffset = XTALLOC (featureList->featureCount, USHORT);

        for (i = 0; i < featureList->featureCount; i++) {
            featureList->featureRecord[i].tag = ttfGetULONG (fp);
            fOffset[i] = ttfGetUSHORT (fp);
        }

        for (i = 0; i < featureList->featureCount; i++)
            otfLoadFeatureRecord (fp, &featureList->featureRecord[i], offset + fOffset[i]);

        free (fOffset);
    }

    return featureList;
}

static void
otfPrintFeatureRecord (FILE *fp, FeatureRecordPtr featureRecord)
{
    int i;

    fprintf (fp, "'%s' feature - lookupCount: %d\n",
        TagToStr (featureRecord->tag), featureRecord->lookupCount);
    fprintf (fp, "\t\tlookupListIndex: %d", featureRecord->lookupListIndex[0]);
    for (i = 1; i < featureRecord->lookupCount; i++)
        fprintf (fp, i % 8 ? ", %d" : ",\n\t\t\t\t %d",
                     featureRecord->lookupListIndex[i]);
    fprintf (fp, "\n");
    if (featureRecord->featureParams)
        fprintf (fp, "\t\tfeatureParams Offset: 0x%04x\n", featureRecord->featureParams);
}

void
otfPrintFeatureList (FILE *fp, FeatureListPtr featureList)
{
    int i;

    fprintf (fp, "    featureCount: %d\n", featureList->featureCount);

    for (i = 0; i < featureList->featureCount; i++) {
        fprintf (fp, "  %2d. ", i);
        otfPrintFeatureRecord (fp, &featureList->featureRecord[i]);
    }
    fprintf (fp, "\n");
}

void
otfFreeFeatureList (FeatureListPtr featureList)
{
    int i;

    if (featureList->featureCount) {
        for (i = 0; i < featureList->featureCount; i++)
            free (featureList->featureRecord[i].lookupListIndex);
        free (featureList->featureRecord);
    }
    free (featureList);
}

static void
otfLoadLookupRecord (FILE *fp, LookupRecordPtr lookupRecord, ULONG offset, MakeLookupFunc makeLookup)
{
    int i;
    USHORT lookupType;
    USHORT *lOffset;

    xfseek (fp, offset, SEEK_SET, "otfLoadLookupRecord");

    lookupType = ttfGetUSHORT (fp);
    lookupRecord->lookupFlag = ttfGetUSHORT (fp);
    lookupRecord->subTableCount = ttfGetUSHORT (fp);
    lookupRecord->lookup = XCALLOC (lookupRecord->subTableCount, LookupPtr);
    lOffset = ttfMakeUSHORT (lookupRecord->subTableCount, fp);
    if (lookupRecord->lookupFlag & lookupFlag_UseMarkFilteringSet)
        lookupRecord->markFilteringSet = ttfGetUSHORT (fp);
    for (i = 0; i < lookupRecord->subTableCount; i++)
        lookupRecord->lookup[i] = (*makeLookup) (fp, lookupType, offset + lOffset[i]);
    free (lOffset);
}

LookupListPtr
otfMakeLookupList (FILE *fp, ULONG offset, MakeLookupFunc makeLookup)
{
    int i;
    USHORT *lOffset;
    LookupListPtr lookupList = XCALLOC1 (LookupList);

    xfseek (fp, offset, SEEK_SET, "otfMakeLookupList");

    lookupList->lookupCount = ttfGetUSHORT (fp);
    if (lookupList->lookupCount) {
        lookupList->lookupRecord = XCALLOC (lookupList->lookupCount, LookupRecord);
        lOffset = ttfMakeUSHORT (lookupList->lookupCount, fp);

        for (i = 0; i < lookupList->lookupCount; i++)
            otfLoadLookupRecord (fp, &lookupList->lookupRecord[i], offset + lOffset[i], makeLookup);

        free (lOffset);
    }

    return lookupList;
}

void
otfPrintLookupList (FILE *fp, LookupListPtr lookupList, PrintLookupFunc printLookup)
{
    int i;

    fprintf (fp, "    lookupCount: %d\n", lookupList->lookupCount);

    for (i = 0; i < lookupList->lookupCount; i++) {
        int j;

        fprintf (fp, "  %2d. lookupType: %d, lookupFlag: 0x%04x", i,
                     lookupList->lookupRecord[i].lookup[0].otf->lookupType,
                     lookupList->lookupRecord[i].lookupFlag);
        if (lookupList->lookupRecord[i].lookupFlag & lookupFlag_UseMarkFilteringSet)
            fprintf (fp, ", markFilteringSet: %d", lookupList->lookupRecord[i].markFilteringSet);
        fprintf (fp, "\n\tsubTableCount:\t%d\n", lookupList->lookupRecord[i].subTableCount);
        for (j = 0; j < lookupList->lookupRecord[i].subTableCount; j++) {
            fprintf (fp, "      %2d. lookupFormat: %d", j,
                         lookupList->lookupRecord[i].lookup[j].otf->lookupFormat);
            (*printLookup) (fp, lookupList->lookupRecord[i].lookup[j]);
        }
    }
    fprintf (fp, "\n");
}

void
otfFreeLookupList (LookupListPtr lookupList, FreeLookupFunc freeLookup)
{
    int i;

    if (lookupList->lookupCount) {
        for (i = 0; i < lookupList->lookupCount; i++) {
            int j;

            for (j = 0; j < lookupList->lookupRecord[i].subTableCount; j++) {
                (*freeLookup) (lookupList->lookupRecord[i].lookup[j]);
                free (lookupList->lookupRecord[i].lookup[j].otf);
            }
            free (lookupList->lookupRecord[i].lookup);
        }
        free (lookupList->lookupRecord);
    }
    free (lookupList);
}

CoveragePtr
otfMakeCoverage (FILE *fp, ULONG offset)
{
    CoveragePtr coverage;
    int i;
    USHORT format;

    xfseek (fp, offset, SEEK_SET, "otfMakeCoverage");

    format = ttfGetUSHORT (fp);
    switch (format)
        {
        case 1:
            coverage.coverage1 = XCALLOC1 (Coverage1);
            coverage.coverage1->glyphCount = ttfGetUSHORT (fp);
            coverage.coverage1->glyphArray = ttfMakeUSHORT (coverage.coverage1->glyphCount, fp);
            break;
        case 2:
            coverage.coverage2 = XCALLOC1 (Coverage2);
            coverage.coverage2->rangeCount = ttfGetUSHORT (fp);
            coverage.coverage2->rangeRecord = XTALLOC (coverage.coverage2->rangeCount, RangeRecord);
            for (i = 0; i < coverage.coverage2->rangeCount; i++) {
                coverage.coverage2->rangeRecord[i].start = ttfGetUSHORT (fp);
                coverage.coverage2->rangeRecord[i].end = ttfGetUSHORT (fp);
                coverage.coverage2->rangeRecord[i].startCoverageIndex = ttfGetUSHORT (fp);
            }
            break;
        default:
            ttfError ("Unrecognized coverageFormat\n");
        }

    *coverage.format = format;
    return coverage;
}

void
otfPrintCoverage (FILE *fp, CoveragePtr coverage)
{
    int i;

    fprintf (fp, "Coverage - ");
    switch (*coverage.format)
        {
        case 1:
            fprintf (fp, "glyphCount: %d\n\t\t  glyphArray: %d",
                         coverage.coverage1->glyphCount, coverage.coverage1->glyphArray[0]);
            for (i = 1; i < coverage.coverage1->glyphCount; i++)
                fprintf (fp, i % 8 ? ", %d" : ",\n\t\t\t      %d",
                             coverage.coverage1->glyphArray[i]);
            fprintf (fp, "\n");
            break;
        case 2:
            fprintf (fp, "rangeCount: %d\n", coverage.coverage2->rangeCount);
            for (i = 0; i < coverage.coverage2->rangeCount; i++) {
                fprintf (fp, "\t      %2d. start: %d, end: %d, startCoverageIndex: %d\n", i,
                             coverage.coverage2->rangeRecord[i].start,
                             coverage.coverage2->rangeRecord[i].end,
                             coverage.coverage2->rangeRecord[i].startCoverageIndex);
            }
            break;
        default:
            ttfError ("Internal error: otfPrintCoverage\n");
        }
}

void
otfFreeCoverage (CoveragePtr coverage)
{
    switch (*coverage.format)
        {
        case 1:
            free (coverage.coverage1->glyphArray);
            break;
        case 2:
            free (coverage.coverage2->rangeRecord);
            break;
        default:
            ttfError ("Internal error: otfFreeCoverage\n");
        }
    free (coverage.format);
}

ClassDefPtr
otfMakeClassDef (FILE *fp, ULONG offset)
{
    ClassDefPtr classDef;
    int i;
    USHORT format;

    xfseek (fp, offset, SEEK_SET, "otfMakeClassDef");

    format = ttfGetUSHORT (fp);
    switch (format)
        {
        case 1:
            classDef.classDef1 = XCALLOC1 (ClassDef1);
            classDef.classDef1->startGlyph = ttfGetUSHORT (fp);
            classDef.classDef1->glyphCount = ttfGetUSHORT (fp);
            classDef.classDef1->classValueArray = ttfMakeUSHORT (classDef.classDef1->glyphCount, fp);
            break;
        case 2:
            classDef.classDef2 = XCALLOC1 (ClassDef2);
            classDef.classDef2->classRangeCount = ttfGetUSHORT (fp);
            classDef.classDef2->classRangeRecord = XTALLOC (classDef.classDef2->classRangeCount, ClassRangeRecord);
            for (i = 0; i < classDef.classDef2->classRangeCount; i++) {
                classDef.classDef2->classRangeRecord[i].start = ttfGetUSHORT (fp);
                classDef.classDef2->classRangeRecord[i].end = ttfGetUSHORT (fp);
                classDef.classDef2->classRangeRecord[i].classValue = ttfGetUSHORT (fp);
            }
            break;
        default:
            ttfError ("Unrecognized classDefFormat\n");
        }

    *classDef.format = format;
    return classDef;
}

void
otfPrintClassDef (FILE *fp, ClassDefPtr classDef)
{
    int i;

    switch (*classDef.format)
        {
        case 1:
            fprintf (fp, "startGlyph: %d, glyphCount: %d\n\t\tclassValueArray ",
                         classDef.classDef1->startGlyph, classDef.classDef1->glyphCount);
            for (i = 0; i < classDef.classDef1->glyphCount; i++)
                fprintf (fp, i == 0 ? "- %d" : i % 8 ? ", %d" : ",\n\t\t\t\t  %d",
                             classDef.classDef1->classValueArray[i]);
            fprintf (fp, "\n");
            break;
        case 2:
            fprintf (fp, "classRangeCount: %d\n", classDef.classDef2->classRangeCount);
            for (i = 0; i < classDef.classDef2->classRangeCount; i++)
                fprintf (fp, "\t\t%2d. start: %d, end: %d, classValue: %d\n", i,
                             classDef.classDef2->classRangeRecord[i].start,
                             classDef.classDef2->classRangeRecord[i].end,
                             classDef.classDef2->classRangeRecord[i].classValue);
            break;
        default:
            ttfError ("Internal error: otfPrintClassDef\n");
        }
}

void
otfFreeClassDef (ClassDefPtr classDef)
{
    switch (*classDef.format)
        {
        case 1:
            free (classDef.classDef1->classValueArray);
            break;
        case 2:
            free (classDef.classDef2->classRangeRecord);
            break;
        default:
            ttfError ("Internal error: otfFreeClassDef\n");
        }
    free (classDef.format);
}

DevicePtr
otfMakeDevice (FILE *fp, ULONG offset)
{
    size_t num;
    DevicePtr device;
    USHORT startSize, endSize, deltaFormat;

    xfseek (fp, offset, SEEK_SET, "otfMakeDevice");

    startSize = ttfGetUSHORT (fp);
    endSize = ttfGetUSHORT (fp);
    deltaFormat = ttfGetUSHORT (fp);
    if (deltaFormat < 1 || deltaFormat > 3)
        ttfError ("Unrecognized deltaFormat\n");
    num = (endSize - startSize) >> (4 - deltaFormat);
    device = (DevicePtr) xcalloc (1, sizeof (Device) + num * sizeof (USHORT));
    device->startSize = startSize;
    device->endSize = endSize;
    device->deltaFormat = deltaFormat;
    ttfReadUSHORT (device->deltaValue, num + 1, fp);

    return device;
}

void
otfPrintDevice (FILE *fp, DevicePtr device)
{
   int i;
   size_t num = (device->endSize - device->startSize) >> (4 - device->deltaFormat);

   fprintf (fp, "startSize = %d, endSize = %d, deltaFormat = %d, deltaValue = 0x",
                device->startSize, device->endSize, device->deltaFormat);
   for (i = 0; i < num; i++)
       fprintf (fp, "%04x", device->deltaValue[i]);
   fprintf (fp, "%04x\n", device->deltaValue[num]);
}

static OtfLookupRecordPtr
makeOtfLookupRecord (USHORT otfCount, FILE *fp)
{
    int i;
    OtfLookupRecordPtr otf = XCALLOC (otfCount, OtfLookupRecord);

    for (i = 0; i < otfCount; i++) {
        otf[i].sequenceIndex = ttfGetUSHORT (fp);
        otf[i].lookupListIndex = ttfGetUSHORT (fp);
    }
    return otf;
}

static void
printOtfLookupRecord (FILE * fp, const char *str, USHORT otfCount, OtfLookupRecordPtr otf)
{
    int i;

    fprintf (fp, "\t%sotfCount: %d\n", str, otfCount);
    for (i = 0; i < otfCount; i++)
        fprintf (fp, "\t%s%2d. sequenceIndex: %d, lookupListIndex: %d\n",
                     str, i, otf[i].sequenceIndex, otf[i].lookupListIndex);
}

static void loadOtfRule (OtfRulePtr otfRule, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "loadOtfRule");

    otfRule->glyphCount = ttfGetUSHORT (fp);
    otfRule->otfCount = ttfGetUSHORT (fp);
    otfRule->input = ttfMakeUSHORT (otfRule->glyphCount - 1, fp);
    otfRule->otf = makeOtfLookupRecord (otfRule->otfCount, fp);
}

static void loadOtfRuleSet (OtfRuleSetPtr otfRuleSet, FILE *fp, ULONG offset)
{
    int i;
    USHORT *rOffset;

    xfseek (fp, offset, SEEK_SET, "loadOtfRuleSet");

    otfRuleSet->otfRuleCount = ttfGetUSHORT (fp);
    rOffset = ttfMakeUSHORT (otfRuleSet->otfRuleCount, fp);
    otfRuleSet->otfRule = XCALLOC (otfRuleSet->otfRuleCount, OtfRule);
    for (i = 0; i < otfRuleSet->otfRuleCount; i++)
        loadOtfRule (&otfRuleSet->otfRule[i], fp, offset + rOffset[i]);
    free (rOffset);
}

OtfCtx1Ptr makeOTFCtx1 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *rOffset;
    OtfCtx1Ptr otf = XCALLOC1 (OtfCtx1);

    cOffset = ttfGetUSHORT (fp);
    otf->otfRuleSetCount = ttfGetUSHORT (fp);
    rOffset = ttfMakeUSHORT (otf->otfRuleSetCount, fp);
    otf->otfRuleSet = XCALLOC (otf->otfRuleSetCount, OtfRuleSet);
    otf->coverage = otfMakeCoverage (fp, offset + cOffset);
    for (i = 0; i < otf->otfRuleSetCount; i++)
        loadOtfRuleSet (&otf->otfRuleSet[i], fp, offset + rOffset[i]);
    free (rOffset);

    return otf;
}

void printOTFCtx1 (FILE *fp, OtfCtx1Ptr otf)
{
    int i;

    fprintf (fp, " - Context %s Simple\n\t  ",
                 otf->lookupType == 7 ? "Positioning" : "Substitution");
    otfPrintCoverage (fp, otf->coverage);
    fprintf (fp, "\t  otfRuleSetCount: %d\n", otf->otfRuleSetCount);
    for (i = 0; i < otf->otfRuleSetCount; i++) {
        int j;

        fprintf (fp, "\t  %2d. otfRuleCount: %d\n", i, otf->otfRuleSet[i].otfRuleCount);
        for (j = 0; j < otf->otfRuleSet[i].otfRuleCount; j++) {
            int k;

            fprintf (fp, "\t    %2d. glyphCount: %d ", j,
                         otf->otfRuleSet[i].otfRule[j].glyphCount);
            for (k = 0; k < otf->otfRuleSet[i].otfRule[j].glyphCount - 1; k++)
                fprintf (fp, k == 0 ? "- %d" : ", %d", otf->otfRuleSet[i].otfRule[j].input[k]);
            fprintf (fp, "\n");
            printOtfLookupRecord (fp, "      ",
                                            otf->otfRuleSet[i].otfRule[j].otfCount,
                                            otf->otfRuleSet[i].otfRule[j].otf);
        }
    }
}

void freeOTFCtx1 (OtfCtx1Ptr otf)
{
    int i;

    otfFreeCoverage (otf->coverage);
    for (i = 0; i < otf->otfRuleSetCount; i++) {
        int j;

        for (j = 0; j < otf->otfRuleSet[i].otfRuleCount; j++) {
            free (otf->otfRuleSet[i].otfRule[j].input);
            free (otf->otfRuleSet[i].otfRule[j].otf);
        }
        free (otf->otfRuleSet[i].otfRule);
    }
    free (otf->otfRuleSet);
}

static void loadOtfClassRule (OtfClassRulePtr otfClassRule, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "loadOtfClassRule");

    otfClassRule->glyphCount = ttfGetUSHORT (fp);
    otfClassRule->otfCount = ttfGetUSHORT (fp);
    otfClassRule->class = ttfMakeUSHORT (otfClassRule->glyphCount - 1, fp);
    otfClassRule->otf = makeOtfLookupRecord (otfClassRule->otfCount, fp);
}

static void loadOtfClassSet (OtfClassSetPtr otfClassSet, FILE *fp, ULONG offset)
{
    int i;
    USHORT *sOffset;

    xfseek (fp, offset, SEEK_SET, "loadOtfClassSet");

    otfClassSet->otfClassRuleCnt = ttfGetUSHORT (fp);
    sOffset = ttfMakeUSHORT (otfClassSet->otfClassRuleCnt, fp);
    otfClassSet->otfClassRule = XCALLOC (otfClassSet->otfClassRuleCnt, OtfClassRule);
    for (i = 0; i < otfClassSet->otfClassRuleCnt; i++)
        loadOtfClassRule (&otfClassSet->otfClassRule[i], fp, offset + sOffset[i]);
    free (sOffset);
}

OtfCtx2Ptr makeOTFCtx2 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT clOffset;
    USHORT *sOffset;
    OtfCtx2Ptr otf = XCALLOC1 (OtfCtx2);

    cOffset = ttfGetUSHORT (fp);
    clOffset = ttfGetUSHORT (fp);
    otf->otfClassSetCnt = ttfGetUSHORT (fp);
    sOffset = ttfMakeUSHORT (otf->otfClassSetCnt, fp);
    otf->otfClassSet = XCALLOC (otf->otfClassSetCnt, OtfClassSet);
    otf->coverage = otfMakeCoverage (fp, offset + cOffset);
    otf->classDef = otfMakeClassDef (fp, offset + clOffset);
    for (i = 0; i < otf->otfClassSetCnt; i++)
        if (sOffset[i])
            loadOtfClassSet (&otf->otfClassSet[i], fp, offset + sOffset[i]);
    free (sOffset);

    return otf;
}

void printOTFCtx2 (FILE *fp, OtfCtx2Ptr otf)
{
    int i;

    fprintf (fp, " - Context %s Class-based\n\t  ",
                 otf->lookupType == 7 ? "Positioning" : "Substitution");
    otfPrintCoverage (fp, otf->coverage);
    fprintf (fp, "\t  ClassDef - ");
    otfPrintClassDef (fp, otf->classDef);
    fprintf (fp, "\t  otfClassSetCnt: %d\n", otf->otfClassSetCnt);
    for (i = 0; i < otf->otfClassSetCnt; i++) {
        int j;

        fprintf (fp, "\t  %2d. otfClassRuleCnt: %d\n", i, otf->otfClassSet[i].otfClassRuleCnt);
        for (j = 0; j < otf->otfClassSet[i].otfClassRuleCnt; j++) {
            int k;

            fprintf (fp, "\t    %2d. glyphCount: %d ", j,
                         otf->otfClassSet[i].otfClassRule[j].glyphCount);
            for (k = 0; k < otf->otfClassSet[i].otfClassRule[j].glyphCount - 1; k++)
                fprintf (fp, k == 0 ? "- %d" : ", %d", otf->otfClassSet[i].otfClassRule[j].class[k]);
            fprintf (fp, "\n");
            printOtfLookupRecord (fp, "      ",
                                            otf->otfClassSet[i].otfClassRule[j].otfCount,
                                            otf->otfClassSet[i].otfClassRule[j].otf);
        }
    }
}

void freeOTFCtx2 (OtfCtx2Ptr otf)
{
    int i;

    otfFreeCoverage (otf->coverage);
    otfFreeClassDef (otf->classDef);
    for (i = 0; i < otf->otfClassSetCnt; i++) {
        int j;

        for (j = 0; j < otf->otfClassSet[i].otfClassRuleCnt; j++) {
            free (otf->otfClassSet[i].otfClassRule[j].class);
            free (otf->otfClassSet[i].otfClassRule[j].otf);
        }
        free (otf->otfClassSet[i].otfClassRule);
    }
    free (otf->otfClassSet);
}

OtfCtx3Ptr makeOTFCtx3 (FILE *fp, ULONG offset)
{
    int i;
    USHORT *gOffset;
    OtfCtx3Ptr otf = XCALLOC1 (OtfCtx3);

    otf->glyphCount = ttfGetUSHORT (fp);
    otf->otfCount = ttfGetUSHORT (fp);
    gOffset = ttfMakeUSHORT (otf->glyphCount, fp);
    otf->otf = makeOtfLookupRecord (otf->otfCount, fp);
    otf->glyphs = XCALLOC (otf->glyphCount, CoveragePtr);
    for (i = 0; i < otf->glyphCount; i++)
        otf->glyphs[i] = otfMakeCoverage (fp, offset + gOffset[i]);
    free (gOffset);

    return otf;
}

void printOTFCtx3 (FILE *fp, OtfCtx3Ptr otf)
{
    int i;

    fprintf (fp, " - Context %s Coverage-based\n",
                 otf->lookupType == 7 ? "Positioning" : "Substitution");
    fprintf (fp, "\t  glyphCount: %d\n", otf->glyphCount);
    for (i = 0; i < otf->glyphCount; i++) {
        fprintf (fp, "\t  %2d. ", i);
        otfPrintCoverage (fp, otf->glyphs[i]);
    }
    printOtfLookupRecord (fp, "  ", otf->otfCount, otf->otf);
}

void freeOTFCtx3 (OtfCtx3Ptr otf)
{
    int i;

    for (i = 0; i < otf->glyphCount; i++)
        otfFreeCoverage (otf->glyphs[i]);
    free (otf->glyphs);
    free (otf->otf);
}

static void
loadChainOtfRule (ChainOtfRulePtr chainOtfRule, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "loadChainOtfRule");

    chainOtfRule->backtrackGlyphCount = ttfGetUSHORT (fp);
    chainOtfRule->backtrack = ttfMakeUSHORT (chainOtfRule->backtrackGlyphCount, fp);
    chainOtfRule->inputGlyphCount = ttfGetUSHORT (fp);
    chainOtfRule->input = ttfMakeUSHORT (chainOtfRule->inputGlyphCount - 1, fp);
    chainOtfRule->lookaheadGlyphCount = ttfGetUSHORT (fp);
    chainOtfRule->lookahead = ttfMakeUSHORT (chainOtfRule->lookaheadGlyphCount, fp);
    chainOtfRule->otfCount = ttfGetUSHORT (fp);
    chainOtfRule->otf = makeOtfLookupRecord (chainOtfRule->otfCount, fp);
}

static void
loadChainOtfRuleSet (ChainOtfRuleSetPtr chainOtfRuleSet, FILE *fp, ULONG offset)
{
    int i;
    USHORT *rOffset;

    xfseek (fp, offset, SEEK_SET, "loadChainOtfRuleSet");

    chainOtfRuleSet->chainOtfRuleCount = ttfGetUSHORT (fp);
    rOffset = ttfMakeUSHORT (chainOtfRuleSet->chainOtfRuleCount, fp);
    chainOtfRuleSet->chainOtfRule = XCALLOC (chainOtfRuleSet->chainOtfRuleCount, ChainOtfRule);
    for (i = 0; i < chainOtfRuleSet->chainOtfRuleCount; i++)
        loadChainOtfRule (&chainOtfRuleSet->chainOtfRule[i], fp, offset + rOffset[i]);
    free (rOffset);
}

OtfChn1Ptr makeOTFChn1 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT *rOffset;
    OtfChn1Ptr otf = XCALLOC1 (OtfChn1);

    cOffset = ttfGetUSHORT (fp);
    otf->chainOtfRuleSetCount = ttfGetUSHORT (fp);
    rOffset = ttfMakeUSHORT (otf->chainOtfRuleSetCount, fp);
    otf->coverage = otfMakeCoverage (fp, offset + cOffset);
    otf->chainOtfRuleSet = XCALLOC (otf->chainOtfRuleSetCount, ChainOtfRuleSet);
    for (i = 0; i < otf->chainOtfRuleSetCount; i++)
        loadChainOtfRuleSet (&otf->chainOtfRuleSet[i], fp, offset + rOffset[i]);
    free (rOffset);

    return otf;
}

void printOTFChn1 (FILE *fp, OtfChn1Ptr otf)
{
    int i;

    fprintf (fp, " - Chained Context %s Simple\n\t  ",
                 otf->lookupType == 8 ? "Positioning" : "Substitution");
    otfPrintCoverage (fp, otf->coverage);
    fprintf (fp, "\t  chainOtfRuleSetCount: %d\n", otf->chainOtfRuleSetCount);
    for (i = 0; i < otf->chainOtfRuleSetCount; i++) {
        int j;

        fprintf (fp, "\t  %2d. chainOtfRuleCount: %d\n", i,
                     otf->chainOtfRuleSet[i].chainOtfRuleCount);
        for (j = 0; j < otf->chainOtfRuleSet[i].chainOtfRuleCount; j++) {
            int k;

            fprintf (fp, "\t    %2d. backtrackGlyphCount: %d ", j,
                         otf->chainOtfRuleSet[i].chainOtfRule[j].backtrackGlyphCount);
            for (k = 0; k < otf->chainOtfRuleSet[i].chainOtfRule[j].backtrackGlyphCount; k++)
                fprintf (fp, k == 0 ? "- %d" : ", %d", otf->chainOtfRuleSet[i].chainOtfRule[j].backtrack[k]);
            fprintf (fp, "\n\t\tinputGlyphCount: %d ",
                         otf->chainOtfRuleSet[i].chainOtfRule[j].inputGlyphCount);
            for (k = 0; k < otf->chainOtfRuleSet[i].chainOtfRule[j].inputGlyphCount; k++)
                fprintf (fp, k == 0 ? "- %d" : ", %d", otf->chainOtfRuleSet[i].chainOtfRule[j].input[k]);
            fprintf (fp, "\n\t\tlookaheadGlyphCount: %d ",
                         otf->chainOtfRuleSet[i].chainOtfRule[j].lookaheadGlyphCount);
            for (k = 0; k < otf->chainOtfRuleSet[i].chainOtfRule[j].lookaheadGlyphCount; k++)
                fprintf (fp, k == 0 ? "- %d" : ", %d", otf->chainOtfRuleSet[i].chainOtfRule[j].lookahead[k]);
            fprintf (fp, "\n");
            printOtfLookupRecord (fp, "      ",
                                            otf->chainOtfRuleSet[i].chainOtfRule[j].otfCount,
                                            otf->chainOtfRuleSet[i].chainOtfRule[j].otf);
        }
    }
}

void freeOTFChn1 (OtfChn1Ptr otf)
{
    int i;

    otfFreeCoverage (otf->coverage);
    for (i = 0; i < otf->chainOtfRuleSetCount; i++) {
        int j;

        for (j = 0; j < otf->chainOtfRuleSet[i].chainOtfRuleCount; j++) {
            free (otf->chainOtfRuleSet[i].chainOtfRule[j].backtrack);
            free (otf->chainOtfRuleSet[i].chainOtfRule[j].input);
            free (otf->chainOtfRuleSet[i].chainOtfRule[j].lookahead);
            free (otf->chainOtfRuleSet[i].chainOtfRule[j].otf);
        }
        free (otf->chainOtfRuleSet[i].chainOtfRule);
    }
    free (otf->chainOtfRuleSet);
}

static void
loadChainOtfClassRule (ChainOtfClassRulePtr chainOtfClassRule, FILE *fp, ULONG offset)
{
    xfseek (fp, offset, SEEK_SET, "loadChainOtfClassRule");

    chainOtfClassRule->backtrackGlyphCount = ttfGetUSHORT (fp);
    chainOtfClassRule->backtrack = ttfMakeUSHORT (chainOtfClassRule->backtrackGlyphCount, fp);
    chainOtfClassRule->inputGlyphCount = ttfGetUSHORT (fp);
    chainOtfClassRule->input = ttfMakeUSHORT (chainOtfClassRule->inputGlyphCount - 1, fp);
    chainOtfClassRule->lookaheadGlyphCount = ttfGetUSHORT (fp);
    chainOtfClassRule->lookahead = ttfMakeUSHORT (chainOtfClassRule->lookaheadGlyphCount, fp);
    chainOtfClassRule->otfCount = ttfGetUSHORT (fp);
    chainOtfClassRule->otf = makeOtfLookupRecord (chainOtfClassRule->otfCount, fp);
}

static void
loadChainOtfClassSet (ChainOtfClassSetPtr chainOtfClassSet, FILE *fp, ULONG offset)
{
    int i;
    USHORT *rOffset;

    xfseek (fp, offset, SEEK_SET, "loadChainOtfClassSet");

    chainOtfClassSet->chainOtfClassRuleCnt = ttfGetUSHORT (fp);
    rOffset = ttfMakeUSHORT (chainOtfClassSet->chainOtfClassRuleCnt, fp);
    chainOtfClassSet->chainOtfClassRule = XCALLOC (chainOtfClassSet->chainOtfClassRuleCnt, ChainOtfClassRule);
    for (i = 0; i < chainOtfClassSet->chainOtfClassRuleCnt; i++)
        loadChainOtfClassRule (&chainOtfClassSet->chainOtfClassRule[i], fp, offset + rOffset[i]);
    free (rOffset);
}

OtfChn2Ptr makeOTFChn2 (FILE *fp, ULONG offset)
{
    int i;
    USHORT cOffset;
    USHORT bOffset, iOffset, lOffset;
    USHORT *sOffset;
    OtfChn2Ptr otf = XCALLOC1 (OtfChn2);

    cOffset = ttfGetUSHORT (fp);
    bOffset = ttfGetUSHORT (fp);
    iOffset = ttfGetUSHORT (fp);
    lOffset = ttfGetUSHORT (fp);
    otf->chainOtfClassSetCnt = ttfGetUSHORT (fp);
    sOffset = ttfMakeUSHORT (otf->chainOtfClassSetCnt, fp);
    otf->coverage = otfMakeCoverage (fp, offset + cOffset);
    otf->backtrackClassDef = otfMakeClassDef (fp, offset + bOffset);
    otf->inputClassDef = otfMakeClassDef (fp, offset + iOffset);
    otf->lookaheadClassDef = otfMakeClassDef (fp, offset + lOffset);
    otf->chainOtfClassSet = XCALLOC (otf->chainOtfClassSetCnt, ChainOtfClassSet);
    for (i = 0; i < otf->chainOtfClassSetCnt; i++)
        if (sOffset[i])
            loadChainOtfClassSet (&otf->chainOtfClassSet[i], fp, offset + sOffset[i]);
    free (sOffset);

    return otf;
}

void printOTFChn2 (FILE *fp, OtfChn2Ptr otf)
{
    int i;

    fprintf (fp, " - Chained Context %s Class-based\n\t  ",
                 otf->lookupType == 8 ? "Positioning" : "Substitution");
    otfPrintCoverage (fp, otf->coverage);
    fprintf (fp, "\t  backtrackClassDef - ");
    otfPrintClassDef (fp, otf->backtrackClassDef);
    fprintf (fp, "\t  inputClassDef - ");
    otfPrintClassDef (fp, otf->inputClassDef);
    fprintf (fp, "\t  lookaheadClassDef - ");
    otfPrintClassDef (fp, otf->lookaheadClassDef);
    fprintf (fp, "\t  chainOtfClassSetCnt: %d\n", otf->chainOtfClassSetCnt);
    for (i = 0; i < otf->chainOtfClassSetCnt; i++) {
        int j;

        fprintf (fp, "\t  %2d. chainOtfClassRuleCnt: %d\n", i,
                     otf->chainOtfClassSet[i].chainOtfClassRuleCnt);
        for (j = 0; j < otf->chainOtfClassSet[i].chainOtfClassRuleCnt; j++) {
            int k;

            fprintf (fp, "\t    %2d. backtrackGlyphCount: %2d ", j,
                         otf->chainOtfClassSet[i].chainOtfClassRule[j].backtrackGlyphCount);
            for (k = 0; k < otf->chainOtfClassSet[i].chainOtfClassRule[j].backtrackGlyphCount; k++)
                fprintf (fp, k == 0 ? "- %d" : k % 8 ? ", %d" : ",\n\t\t\t\t\t  %d",
                             otf->chainOtfClassSet[i].chainOtfClassRule[j].backtrack[k]);
            fprintf (fp, "\n\t\tinputGlyphCount: %2d ",
                         otf->chainOtfClassSet[i].chainOtfClassRule[j].inputGlyphCount);
            for (k = 0; k < otf->chainOtfClassSet[i].chainOtfClassRule[j].inputGlyphCount; k++)
                fprintf (fp, k == 0 ? "- %d" : k % 8 ? ", %d" : ",\n\t\t\t\t      %d",
                             otf->chainOtfClassSet[i].chainOtfClassRule[j].input[k]);
            fprintf (fp, "\n\t\tlookaheadGlyphCount: %2d ",
                         otf->chainOtfClassSet[i].chainOtfClassRule[j].lookaheadGlyphCount);
            for (k = 0; k < otf->chainOtfClassSet[i].chainOtfClassRule[j].lookaheadGlyphCount; k++)
                fprintf (fp, k == 0 ? "- %d" : k % 8 ? ", %d" : ",\n\t\t\t\t\t  %d",
                             otf->chainOtfClassSet[i].chainOtfClassRule[j].lookahead[k]);
            fprintf (fp, "\n");
            printOtfLookupRecord (fp, "      ",
                                            otf->chainOtfClassSet[i].chainOtfClassRule[j].otfCount,
                                            otf->chainOtfClassSet[i].chainOtfClassRule[j].otf);
        }
    }
}

void freeOTFChn2 (OtfChn2Ptr otf)
{
    int i;

    otfFreeCoverage (otf->coverage);
    otfFreeClassDef (otf->backtrackClassDef);
    otfFreeClassDef (otf->inputClassDef);
    otfFreeClassDef (otf->lookaheadClassDef);
    for (i = 0; i < otf->chainOtfClassSetCnt; i++) {
        int j;

        for (j = 0; j < otf->chainOtfClassSet[i].chainOtfClassRuleCnt; j++) {
            free (otf->chainOtfClassSet[i].chainOtfClassRule[j].backtrack);
            free (otf->chainOtfClassSet[i].chainOtfClassRule[j].input);
            free (otf->chainOtfClassSet[i].chainOtfClassRule[j].lookahead);
            free (otf->chainOtfClassSet[i].chainOtfClassRule[j].otf);
        }
        free (otf->chainOtfClassSet[i].chainOtfClassRule);
    }
    free (otf->chainOtfClassSet);
}

OtfChn3Ptr makeOTFChn3 (FILE *fp, ULONG offset)
{
    int i;
    USHORT *bOffset, *iOffset, *lOffset;
    OtfChn3Ptr otf = XCALLOC1 (OtfChn3);

    otf->backtrackGlyphCount =  ttfGetUSHORT (fp);
    bOffset = ttfMakeUSHORT (otf->backtrackGlyphCount, fp);
    otf->inputGlyphCount =  ttfGetUSHORT (fp);
    iOffset = ttfMakeUSHORT (otf->inputGlyphCount, fp);
    otf->lookaheadGlyphCount =  ttfGetUSHORT (fp);
    lOffset = ttfMakeUSHORT (otf->lookaheadGlyphCount, fp);
    otf->otfCount = ttfGetUSHORT (fp);
    otf->otf = makeOtfLookupRecord (otf->otfCount, fp);
    otf->backtrack = XCALLOC (otf->backtrackGlyphCount, CoveragePtr);
    for (i = 0; i < otf->backtrackGlyphCount; i++)
        otf->backtrack[i] = otfMakeCoverage (fp, offset + bOffset[i]);
    free (bOffset);
    otf->input = XCALLOC (otf->inputGlyphCount, CoveragePtr);
    for (i = 0; i < otf->inputGlyphCount; i++)
        otf->input[i] = otfMakeCoverage (fp, offset + iOffset[i]);
    free (iOffset);
    otf->lookahead = XCALLOC (otf->lookaheadGlyphCount, CoveragePtr);
    for (i = 0; i < otf->lookaheadGlyphCount; i++)
        otf->lookahead[i] = otfMakeCoverage (fp, offset + lOffset[i]);
    free (lOffset);

    return otf;
}

void printOTFChn3 (FILE *fp, OtfChn3Ptr otf)
{
    int i;

    fprintf (fp, " - Chained Context %s Coverage-based\n",
                 otf->lookupType == 8 ? "Positioning" : "Substitution");
    fprintf (fp, "\t  backtrackGlyphCount: %d\n", otf->backtrackGlyphCount);
    for (i = 0; i < otf->backtrackGlyphCount; i++) {
        fprintf (fp, "\t  %2d. backtrack", i);
        otfPrintCoverage (fp, otf->backtrack[i]);
    }
    fprintf (fp, "\t  inputGlyphCount: %d\n", otf->inputGlyphCount);
    for (i = 0; i < otf->inputGlyphCount; i++) {
        fprintf (fp, "\t  %2d. input", i);
        otfPrintCoverage (fp, otf->input[i]);
    }
    fprintf (fp, "\t  lookaheadGlyphCount: %d\n", otf->lookaheadGlyphCount);
    for (i = 0; i < otf->lookaheadGlyphCount; i++) {
        fprintf (fp, "\t  %2d. lookahead", i);
        otfPrintCoverage (fp, otf->lookahead[i]);
    }
    printOtfLookupRecord (fp, "  ", otf->otfCount, otf->otf);
}

void freeOTFChn3 (OtfChn3Ptr otf)
{
    int i;

    for (i = 0; i < otf->backtrackGlyphCount; i++)
        otfFreeCoverage (otf->backtrack[i]);
    free (otf->backtrack);
    for (i = 0; i < otf->inputGlyphCount; i++)
        otfFreeCoverage (otf->input[i]);
    free (otf->input);
    for (i = 0; i < otf->lookaheadGlyphCount; i++)
        otfFreeCoverage (otf->lookahead[i]);
    free (otf->lookahead);
    free (otf->otf);
}

