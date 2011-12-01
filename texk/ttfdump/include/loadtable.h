#ifndef __TTF_LOADTABLE_H
#define __TTF_LOADTABLE_H

#include "tables.h"

/* $Id: loadtable.h,v 1.2 1998/07/06 06:07:01 werner Exp $ */

void ttfLoadRequiredTables(TTFontPtr font);
void ttfFreeRequiredTables(TTFontPtr font);
void ttfLoadOptionalTables(TTFontPtr font);
void ttfFreeOptionalTables(TTFontPtr font);
void ttfLoadOpenTypeTables(TTFontPtr font);
void ttfFreeOpenTypeTables(TTFontPtr font);

void ttfInitTableDir(TTFontPtr font, ULONG offset);
void ttfPrintTableDir(FILE *fp, TableDirPtr p);
void ttfFreeTableDir(TableDirPtr p);
TableDirPtr ttfLookUpTableDir(ULONG tag, TTFontPtr font);

void ttfInitCMAP(TTFontPtr font);
void ttfPrintCMAP(FILE *fp, CMAPPtr cmap);
USHORT ttfLookUpCMAP(MapPtr map, USHORT cc);
void ttfFreeCMAP(CMAPPtr cmap);

void ttfInitGLYF(TTFontPtr font);
void ttfLoadGLYF(FILE *fp, GLYFPtr glyf, ULONG offset);
void ttfPrintGLYF(FILE *fp, GLYFPtr glyf);
void ttfFreeGLYF(GLYFPtr glyf);
GLYFPtr ttfLoadGlyphIndex(TTFont * font, USHORT idx);
GLYFPtr ttfLoadGlyphCode(TTFont * fnt, USHORT cc);

void ttfInitGlyphCache(TTFontPtr font);
void ttfCleanUpGlyphCache(TTFontPtr font);
GLYFPtr ttfLoadGlyphCached(TTFontPtr font, ULONG offset);

void ttfInitHEAD(TTFontPtr font);
void ttfPrintHEAD(FILE *, HEADPtr head);
void ttfFreeHEAD(HEADPtr head);

void ttfInitHHEA(TTFontPtr font);
void ttfPrintHHEA(FILE *fp, HHEAPtr hhea);
void ttfFreeHHEA(HHEAPtr hhea);

void ttfInitHMTX(TTFontPtr font);
void ttfPrintHMTX(FILE *fp, HMTXPtr hmtx);
void ttfFreeHMTX(HMTXPtr hmtx);

void ttfInitLOCA(TTFontPtr font);
void ttfPrintLOCA(FILE *fp, LOCAPtr loca);
void ttfFreeLOCA(LOCAPtr loca);
ULONG ttfLookUpGlyfLOCA(LOCAPtr loca, USHORT idx);

void ttfInitMAXP(TTFontPtr font);
void ttfPrintMAXP(FILE *fp, MAXPPtr maxp);
void ttfFreeMAXP(MAXPPtr maxp);

void ttfInitNAME(TTFontPtr font);
void ttfPrintNAME(FILE *fp, NAMEPtr name);
void ttfFreeNAME(NAMEPtr name);

void ttfInitPOST(TTFontPtr fot);
void ttfPrintPOST(FILE *fp, POSTPtr post);
void ttfFreePOST(POSTPtr post);

void ttfInitOS2(TTFontPtr font);
void ttfPrintOS2(FILE *fp, OS_2Ptr os2);
void ttfFreeOS2(OS_2Ptr os2);

void ttfInitCVT(TTFontPtr font);
void ttfPrintCVT(FILE *fp, FWord * cvt, USHORT cvtLength);
void ttfFreeCVT(FWord * cvt);

void ttfInitFPGM(TTFontPtr font);
void ttfPrintFPGM(FILE *fp, BYTE * fpgm, USHORT length);
void ttfFreeFPGM(BYTE * fpgm);

void ttfInitGASP(TTFontPtr font);
void ttfPrintGASP(FILE *fp, GASPPtr gasp);
void ttfFreeGASP(GASPPtr gasp);

void ttfInitHDMX(TTFontPtr font);
void ttfPrintHDMX(FILE *fp, HDMXPtr hdmx);
void ttfFreeHDMX(HDMXPtr hdmx);

void ttfInitPREP(TTFontPtr font);
void ttfPrintPREP(FILE *fp, BYTE * prep, USHORT length);
void ttfFreePREP(BYTE * prep);

void ttfInitLTSH(TTFontPtr font);
void ttfPrintLTSH(FILE *fp, LTSHPtr ltsh);
void ttfFreeLTSH(LTSHPtr ltsh);

void ttfInitVDMX(TTFontPtr font);
void ttfPrintVDMX(FILE *fp, VDMXPtr vdmx);
void ttfFreeVDMX(VDMXPtr vdmx);

ScriptListPtr otfMakeScriptList(FILE *fp, ULONG offset);
void otfPrintScriptList(FILE *fp, ScriptListPtr scriptList);
void otfFreeScriptList(ScriptListPtr scriptList);

FeatureListPtr otfMakeFeatureList(FILE *fp, ULONG offset);
void otfPrintFeatureList(FILE *fp, FeatureListPtr featureList);
void otfFreeFeatureList(FeatureListPtr featureList);

LookupListPtr otfMakeLookupList(FILE *fp, ULONG offset, MakeLookupFunc makeLookup);
void otfPrintLookupList(FILE *fp, LookupListPtr lookupList, PrintLookupFunc printLookup);
void otfFreeLookupList(LookupListPtr lookupList, FreeLookupFunc freeLookup);

CoveragePtr otfMakeCoverage(FILE *fp, ULONG offset);
void otfPrintCoverage(FILE *fp, CoveragePtr coverage);
void otfFreeCoverage(CoveragePtr coverage);

ClassDefPtr otfMakeClassDef(FILE *fp, ULONG offset);
void otfPrintClassDef(FILE *fp, ClassDefPtr classDef);
void otfFreeClassDef(ClassDefPtr classDef);

DevicePtr otfMakeDevice(FILE *fp, ULONG offset);
void otfPrintDevice(FILE *fp, DevicePtr device);

void ttfInitGPOS(TTFontPtr font);
void ttfPrintGPOS(FILE *fp, GPOSPtr gpos);
void ttfFreeGPOS(GPOSPtr gpos);

void ttfInitGSUB(TTFontPtr font);
void ttfPrintGSUB(FILE *fp, GSUBPtr gsub);
void ttfFreeGSUB(GSUBPtr gsub);

void ttfInitGraphicsState(TTFontPtr font);
void ttfInitStack(TTFontPtr font);
void ttfInitStorageArea(TTFontPtr font);

void ttfPrintInstructions(FILE *fp, BYTE * ins);

#endif /* __TTF_LOADTABLE_H */


/* end of loadtable.h */
