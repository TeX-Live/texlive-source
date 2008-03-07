/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: MemoryUsage.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Calculates memory usage for the engine and segments.
----------------------------------------------------------------------------------------------*/

#pragma warning(disable: 4244)	// conversion from wchar_t to char
#pragma warning(disable: 4702)	// unreachable code

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************

#include "GrCommon.h"
#include "GrData.h"
#ifndef _WIN32
#include "GrMstypes.h"
#endif
#include "GrDebug.h"
//#include <fstream>
#include <iostream>
#include <cstring>
//#include <string>
// Forward declarations.

#include "Main.h"

//namespace gr
//{
//    class GrTableManager;
//	class Segment;
//	class GrEngine;
//}
//
//#include "GrTableManager.h"
//#include "GrFeatureValues.h"
//#include "GrSlotState.h"
//#include "SegmentAux.h"
//#include "Segment.h"
#include "MemoryUsage.h"

//#ifndef _MSC_VER
//#include "config.h"
//#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif
#undef THIS_FILE
DEFINE_THIS_FILE

//:End Ignore

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local Constants and static variables
//:>********************************************************************************************

const int stringOverhead = (3 * sizeof(int)) + (sizeof(void *)) + (8 * sizeof(short));
const int vectorOverhead = 3 * sizeof(void *);

namespace gr
{

//:>********************************************************************************************
//:>	Font/engine
//:>********************************************************************************************

void FontMemoryUsage::initialize()
{
	font = 0;
	fontCache = 0;
	fontFace = 0;

	eng_count = 0;
	eng_overhead = 0;
	eng_scalars = 0;
	eng_strings = 0;
	eng_pointers = 0;
	eng_cmap = 0;
	eng_nameTable = 0;

	pseudoMap = 0;

	clstbl_counters = 0;
	clstbl_offsets = 0;
	clstbl_glyphList = 0;

	glftbl_general = 0;
	glftbl_compDefns = 0;
	glftbl_attrTable = 0;
	glftbl_attrOffsets = 0;

	lngtbl_general = 0;
	lngtbl_entries = 0;
	lngtbl_featureSets = 0;

	tman_general = 0;

	pass_count = 0;
	pass_general = 0;
	pass_fsm = 0;
	pass_ruleExtras = 0;
	pass_constraintOffsets = 0;
	pass_constraintCode = 0;
	pass_actionOffsets = 0;
	pass_actionCode = 0;

	engst_general = 0;
	engst_passState = 0;

	sstrm_count = 0;
	sstrm_general = 0;
	sstrm_chunkMapsUsed = 0;
	sstrm_chunkMapsAlloc = 0;
	sstrm_reprocBuf = 0;

	slot_count = 0;
	slot_general = 0;
	slot_abstract = 0;
	slot_varLenBuf = 0;
	slot_assocsUsed = 0;
	slot_assocsAlloc = 0;
	slot_attachUsed = 0;
	slot_attachAlloc = 0;
}

/*----------------------------------------------------------------------------------------------
	Return the total byte count.
----------------------------------------------------------------------------------------------*/
int FontMemoryUsage::total()
{
	int grandTotal = 0;
	grandTotal += font + fontCache + fontFace;
	grandTotal += eng_overhead + eng_scalars + eng_strings + eng_pointers + eng_cmap + eng_nameTable;
	grandTotal += pseudoMap;
	grandTotal += clstbl_counters + clstbl_offsets + clstbl_glyphList;
	grandTotal += glftbl_general + glftbl_compDefns	+ glftbl_attrTable + glftbl_attrOffsets;
	grandTotal += lngtbl_general + lngtbl_entries + lngtbl_featureSets;
	grandTotal += tman_general;
	grandTotal += pass_general + pass_fsm + pass_ruleExtras
		+ pass_constraintOffsets + pass_constraintCode + pass_actionOffsets + pass_actionCode;
	grandTotal += engst_general + engst_passState;
	grandTotal += sstrm_general + sstrm_chunkMapsAlloc + sstrm_reprocBuf;
	grandTotal += slot_general + slot_abstract + slot_varLenBuf + slot_assocsAlloc + slot_attachAlloc;

	return grandTotal;
}

/*----------------------------------------------------------------------------------------------
	Add the results of two runs together.
----------------------------------------------------------------------------------------------*/
void FontMemoryUsage::add(FontMemoryUsage & fmu)
{
	font += fmu.font;
	fontCache += fmu.fontCache;
	fontFace += fmu.fontFace;

	eng_count += fmu.eng_count;
	eng_overhead += fmu.eng_overhead;
	eng_scalars += fmu.eng_scalars;
	eng_strings += fmu.eng_strings;
	eng_pointers += fmu.eng_pointers;
	eng_cmap += fmu.eng_cmap;
	eng_nameTable += fmu.eng_nameTable;

	pseudoMap += fmu.pseudoMap;

	clstbl_counters += fmu.clstbl_counters;
	clstbl_offsets += fmu.clstbl_offsets;
	clstbl_glyphList += fmu.clstbl_glyphList;

	glftbl_general += fmu.glftbl_general;
	glftbl_compDefns += fmu.glftbl_compDefns;
	glftbl_attrTable += fmu.glftbl_attrTable;
	glftbl_attrOffsets += fmu.glftbl_attrOffsets;

	lngtbl_general += fmu.lngtbl_general;
	lngtbl_entries += fmu.lngtbl_entries;
	lngtbl_featureSets += fmu.lngtbl_featureSets;

	tman_general += fmu.tman_general;

	pass_count += fmu.pass_count;
	pass_general += fmu.pass_general;
	pass_fsm += fmu.pass_fsm;
	pass_ruleExtras += fmu.pass_ruleExtras;
	pass_constraintOffsets += fmu.pass_constraintOffsets;
	pass_constraintCode += fmu.pass_constraintCode;
	pass_actionOffsets += fmu.pass_actionOffsets;
	pass_actionCode += fmu.pass_actionCode;

	engst_general += fmu.engst_general;
	engst_passState += fmu.engst_passState;

	sstrm_count += fmu.sstrm_count;
	sstrm_general += fmu.sstrm_general;
	sstrm_chunkMapsUsed += fmu.sstrm_chunkMapsUsed;
	sstrm_chunkMapsAlloc += fmu.sstrm_chunkMapsAlloc;
	sstrm_reprocBuf += fmu.sstrm_reprocBuf;

	slot_count += fmu.slot_count;
	slot_general += fmu.slot_general;
	slot_abstract += fmu.slot_abstract;
	slot_varLenBuf += fmu.slot_varLenBuf;
	slot_assocsUsed += fmu.slot_assocsUsed;
	slot_assocsAlloc += fmu.slot_assocsAlloc;
	slot_attachUsed += fmu.slot_attachUsed;
	slot_attachAlloc += fmu.slot_attachAlloc;
}

/*----------------------------------------------------------------------------------------------
	Add up the memory used by all the engines in the cache.
----------------------------------------------------------------------------------------------*/
FontMemoryUsage Font::calculateMemoryUsage()
{
	FontMemoryUsage fmu;
	FontFace::calculateAllMemoryUsage(fmu);
	return fmu;
}

void FontFace::calculateAllMemoryUsage(FontMemoryUsage & fmu)
{
	s_pFontCache->calculateMemoryUsage(fmu);
}

void FontCache::calculateMemoryUsage(FontMemoryUsage & fmuTotal)
{
	fmuTotal.fontCache += sizeof(int) * 4;	// m_cfci, m_cfciMax, m_cfface, m_flush
	fmuTotal.fontCache += sizeof(void*);	// m_prgfci;

	// Iterate through all the font faces in the cache.
	for (int ifci = 0; ifci < m_cfci; ifci++)
	{
		CacheItem * pfci = m_prgfci + ifci;
		char rgchFontName[32];
		memset(rgchFontName, 0, 32 * sizeof(char));
		std::copy(pfci->szFaceName, pfci->szFaceName + 32, rgchFontName);
		std::string strFontName(rgchFontName);
		fmuTotal.vstrFontNames.push_back(strFontName);
		int cface = 0;
		if (pfci->pffaceRegular)
		{
			FontMemoryUsage fmu;
			pfci->pffaceRegular->calculateMemoryUsage(fmu);
			fmuTotal.add(fmu);
			fmuTotal.vFontTotalsReg.push_back(fmu.total());
			cface++;
		}
		else
			fmuTotal.vFontTotalsReg.push_back(0);

		if (pfci->pffaceBold)
		{
			FontMemoryUsage fmu;
			pfci->pffaceBold->calculateMemoryUsage(fmu);
			fmuTotal.add(fmu);
			fmuTotal.vFontTotalsBold.push_back(fmu.total());
			cface++;
		}
		else
			fmuTotal.vFontTotalsBold.push_back(0);

		if (pfci->pffaceItalic)
		{
			FontMemoryUsage fmu;
			pfci->pffaceItalic->calculateMemoryUsage(fmu);
			fmuTotal.add(fmu);
			fmuTotal.vFontTotalsItalic.push_back(fmu.total());
			cface++;
		}
		else
			fmuTotal.vFontTotalsItalic.push_back(0);

		if (pfci->pffaceBI)
		{
			FontMemoryUsage fmu;
			pfci->pffaceBI->calculateMemoryUsage(fmu);
			fmuTotal.add(fmu);
			fmuTotal.vFontTotalsBI.push_back(fmu.total());
			cface++;
		}
		else
			fmuTotal.vFontTotalsBI.push_back(0);

		fmuTotal.vFaceCount.push_back(cface);
	}
}

void FontFace::calculateMemoryUsage(FontMemoryUsage & fmu)
{
	fmu.fontFace += sizeof(int);			// m_cfonts
	fmu.fontFace += sizeof(void *) * 2;		// s_pFontCache, m_pgreng

	fmu.font += m_cfonts * sizeof(void*);	// m_pfface

	fmu.addEngine(m_pgreng);
}

void FontMemoryUsage::addEngine(GrEngine * pgreng)
{
	eng_count++;

	eng_overhead += sizeof(long);		// m_cref
	eng_scalars += sizeof(bool) * 6;	// m_fBold, m_fItalic, m_fSmart...
	eng_scalars += sizeof(bool) * 2;	// m_fFakeItalicCache, m_fFakeBICache;
	eng_strings += stringOverhead * 4;	// m_strCtrlFileReg/Bold/Italic/BI
	eng_strings += sizeof(wchar_t) * pgreng->m_strCtrlFileReg.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_strCtrlFileBold.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_strCtrlFileItalic.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_strCtrlFileBI.length();
	eng_scalars += sizeof(bool);
	eng_strings += stringOverhead * 6;	// m_stuCtrlFile etc.
	eng_strings += sizeof(wchar_t) * pgreng->m_stuCtrlFile.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_stuInitError.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_stuFaceName.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_stuFeatures.length();
	eng_strings += sizeof(wchar_t) * pgreng->m_stuBaseFaceName.length();
	eng_scalars += sizeof(bool);			// m_fUseSepBase
	eng_scalars += sizeof(GrResult) * 2;	// m_resFontRead, m_resFontValid
	eng_scalars += sizeof(FontErrorCode);
	eng_scalars += sizeof(int) * 3;			// m_fxdBadVersion, m_nFontCheckSum, m_nScriptTag, m_grfsdc
	eng_scalars += sizeof(bool);			// m_fRightToLeft
	eng_scalars += sizeof(int) * 2;			// m_mXAscent, m_mXDescent
	eng_scalars += sizeof(float);			// m_dysOffset
	eng_scalars += sizeof(bool);			// m_fBasicJust
	eng_scalars += sizeof(int);				// m_cJLevels
	eng_pointers += sizeof(void*) * 4;		// m_ptman, m_pctbl, m_pgtbl, m_pfface
	eng_scalars += sizeof(GrFeature) * kMaxFeatures;
	eng_scalars += sizeof(int);				// m_cfeat
	eng_scalars += sizeof(size_t);			// m_clcidFeatLabelLangs
	eng_scalars += sizeof(short) * pgreng->m_clcidFeatLabelLangs;
	// m_langtbl
	eng_scalars += sizeof(bool);			// m_fLineBreak
	eng_scalars += sizeof(int) * 2;			// m_cchwPreXlbContext, m_cchwPostXlbContext
	eng_scalars += sizeof(data16) * 7;		// m_chwPseudoAttr, etc.
	eng_scalars += sizeof(gid16);			// m_chwLBGlyphID
	eng_scalars += sizeof(int) * 4;			// m_cComponents, m_cnUserDefn, m_cnCompPerLig, m_mFontEmUnits

	pseudoMap += sizeof(int) * 3;
	pseudoMap += sizeof(GrPseudoMap) * pgreng->m_cpsd;

	eng_cmap += sizeof(void*) * 2;
	eng_cmap += sizeof(byte*);
	eng_cmap += sizeof(bool);
	if (pgreng->m_fCmapTblCopy)
		eng_cmap += pgreng->m_cbCmapTbl;

	eng_nameTable += sizeof(byte *);
	eng_nameTable += sizeof(bool);
	if (pgreng->m_fNameTblCopy)
		eng_nameTable += pgreng->m_cbNameTbl;

	eng_scalars += sizeof(bool) * 2;		// m_fLogXductn, m_fInErrorState

	clstbl_counters += sizeof(int) * 2;		// m_ccls, m_cclsLinear
	clstbl_offsets += sizeof(data16) * (pgreng->m_pctbl->m_ccls + 1);
	clstbl_glyphList += sizeof(data16) * (pgreng->m_pctbl->m_prgichwOffsets[pgreng->m_pctbl->m_ccls]);

	glftbl_general += sizeof(int) * 3;		// m_cglf, m_cComponents, m_cgstbl
	glftbl_general += vectorOverhead;		// m_vpgstbl
	GrGlyphSubTable * pgstbl = pgreng->m_pgtbl->m_vpgstbl[0];
	glftbl_general += sizeof(int) * 4;		// m_fxdSilfVersion, m_nAttrIDLim, m_cComponents, m_cnCompPerLig
	glftbl_general += sizeof(bool) * 2;		// m_fHasDebugStrings, m_fGlocShort
	glftbl_general += sizeof(data16) * 3;	// m_chw...Attr
	glftbl_compDefns += sizeof(int) * (pgstbl->m_cnCompPerLig + 1) * pgreng->m_pgtbl->m_cglf;
	glftbl_attrTable += sizeof(int) * 2;	// m_fxdSilfVersion, m_cbEntryBufLen
	glftbl_attrTable += sizeof(byte) * pgstbl->m_pgatbl->m_cbEntryBufLen;
	glftbl_attrOffsets +=
		(pgreng->m_pgtbl->m_cglf + 1) * ((pgstbl->m_fGlocShort) ? sizeof(data16) : sizeof(data32));

	lngtbl_general += sizeof(size_t);		// m_clang
	lngtbl_general += sizeof(data16) * 3;	// m_dilangInit, m_cLoop, m_ilangStart
	lngtbl_general += sizeof(void*) * 2;	// m_prglang, m_prgfset
	lngtbl_general += sizeof(int);			// m_cbOffset0
	lngtbl_entries += sizeof(GrLangTable::LangEntry) * pgreng->m_langtbl.m_clang;
	lngtbl_featureSets += sizeof(GrLangTable::FeatSet) * pgreng->m_langtbl.m_cfset;

	GrTableManager * ptman = pgreng->m_ptman;
	tman_general += sizeof(int) * 4;	// m_cpass, m_cpassLB, m_ipassPos1, m_ipassJust1
	tman_general += sizeof(bool);		// m_fBidi
	tman_general += sizeof(void*) * 2;	// m_prgppass, m_pgreng
	tman_general += sizeof(void*) * ptman->m_cpass;	// m_prgppass
	tman_general += sizeof(bool);		// m_fLogging

	pass_count += ptman->m_cpass;
	for (int ipass = 0; ipass < ptman->m_cpass; ipass++)
	{
		GrPass * ppass = ptman->m_prgppass[ipass];
		pass_general = sizeof(GrPass);

		GrFSM * pfsm = ppass->m_pfsm;
		if (pfsm)
		{
			pass_fsm += sizeof(int) * 5;	// m_crow, m_crowFinal, m_rowFinalMin, m_crowNonAcpt, m_ccol
			pass_fsm += sizeof(void*) * 3;	// m_prgirulnMin, m_prgrulnMatched, m_prgrowTransitions
			int crowSuccess = pfsm->m_crow - pfsm->m_crowNonAcpt;
			pass_fsm += sizeof(data16) * (crowSuccess + 1);	// m_prgirulnMin
			pass_fsm += sizeof(data16) * (pfsm->m_crulnMatched);	// m_prgrulnMatched
			pass_fsm += sizeof(short) * ((pfsm->m_crow - pfsm->m_crowFinal) * pfsm->m_ccol);
			pass_fsm += sizeof(data16);		// m_dimcrInit, m_cLoop, m_imcrStart
			pass_fsm += sizeof(int);		// m_cmcr
			pass_fsm += sizeof(void*);		// m_prgmcr
			pass_fsm += sizeof(GrFSMClassRange) * pfsm->m_cmcr;
			pass_fsm += sizeof(int) * 2;	// m_critMinRulePreContext, m_critMaxRulePreContext
			pass_fsm += sizeof(void*);		// m_prgrowStartStates
			pass_fsm += sizeof(short) * (pfsm->m_critMaxRulePreContext - pfsm->m_critMinRulePreContext + 1);
		}

		pass_ruleExtras = sizeof(data16) * ppass->m_crul;	// m_prgchwRuleSortKeys
		pass_ruleExtras = sizeof(byte*) * ppass->m_crul;	// m_prgcritRulePreModContext

		pass_constraintOffsets += sizeof(data16) * (ppass->m_crul + 1);
		pass_actionOffsets =+ sizeof(data16) * (ppass->m_crul + 1);

		pass_constraintCode += sizeof(byte) * ppass->m_cbConstraints;
		pass_actionCode += sizeof(byte) * ppass->m_cbActions;

		pass_general += sizeof(bool) * ppass->m_crul;	// m_prgfRuleOkay
		pass_general += vectorOverhead;					// m_vnStack
		pass_general += sizeof(int) * ppass->m_vnStack.capacity();
	}

	engst_general += sizeof(EngineState);
	engst_general += sizeof(void *) * ptman->m_engst.m_vslotblk.capacity();
	engst_general += sizeof(void *) * ptman->m_engst.m_vprgnSlotVarLenBufs.capacity();
	engst_general += sizeof(void *) * ptman->m_engst.m_cpass;	// m_prgpsstrm

	engst_passState += sizeof(PassState) * ptman->m_engst.m_cpass;

	for (int isstrm = 0; isstrm < ptman->m_engst.m_cpass; isstrm++)
	{
		sstrm_count++;
		GrSlotStream * psstrm = ptman->m_engst.m_prgpsstrm[isstrm];
		sstrm_general += sizeof(GrSlotStream);
		sstrm_chunkMapsUsed += psstrm->m_vislotPrevChunkMap.size();
		sstrm_chunkMapsUsed += psstrm->m_vislotPrevChunkMap.size();
		sstrm_chunkMapsAlloc += psstrm->m_vislotPrevChunkMap.capacity();
		sstrm_chunkMapsAlloc += psstrm->m_vislotNextChunkMap.capacity();
		sstrm_reprocBuf += psstrm->m_vpslotReproc.capacity();
		sstrm_reprocBuf += sizeof(int) * 2;
		sstrm_general   -= sizeof(int) * 2;
	}

	slot_general +=	ptman->m_engst.m_vslotblk.size() * EngineState::kSlotBlockSize
		* (sizeof(GrSlotState) - sizeof(GrSlotAbstract));
	slot_abstract += ptman->m_engst.m_vslotblk.size() * EngineState::kSlotBlockSize
		* sizeof(GrSlotAbstract);
	int cnExtraPerSlot = ptman->m_engst.m_cUserDefn + (ptman->m_engst.m_cCompPerLig * 2)
		+ ptman->m_engst.m_cFeat;
	slot_varLenBuf += EngineState::kSlotBlockSize * cnExtraPerSlot * sizeof(u_intslot);
	for (size_t islotblk = 0; islotblk < ptman->m_engst.m_vslotblk.size(); islotblk++)
	{
		for (int islot = 0; islot < EngineState::kSlotBlockSize; islot++)
		{
			slot_count++;
			GrSlotState * pslot = ptman->m_engst.m_vslotblk[islotblk] + islot;
			slot_assocsUsed += pslot->m_vpslotAssoc.size();
			slot_assocsAlloc += pslot->m_vpslotAssoc.capacity();
			slot_attachUsed += pslot->m_vdislotAttLeaves.size();
			slot_attachAlloc += pslot->m_vdislotAttLeaves.capacity();
		}
	}

}

/*----------------------------------------------------------------------------------------------
	Write out the memory usage onto a stream.
----------------------------------------------------------------------------------------------*/
void FontMemoryUsage::prettyPrint(std::ostream & strm)
{
	int totalBytes = total();

	strm << "Number of engines:      " << eng_count << "\n";
	strm << "Number of passes:       " << pass_count << "\n\n";

	strm << "Number of slot streams: " << sstrm_count << "\n";
	strm << "Number of slots:        " << slot_count << "\n\n";

	int classTableTotal = clstbl_counters + clstbl_offsets + clstbl_glyphList;
	int glyphTableTotal = glftbl_general + glftbl_compDefns + glftbl_attrTable + glftbl_attrOffsets;
	int langTableTotal = lngtbl_general + lngtbl_entries + lngtbl_featureSets;
	int passTotal = pass_general + pass_fsm + pass_ruleExtras + pass_constraintOffsets + pass_constraintCode
		+ pass_actionOffsets + pass_actionCode;
	int engineTotal = eng_overhead + eng_scalars + eng_strings + eng_pointers + eng_cmap + eng_nameTable
		+ pseudoMap + classTableTotal + glyphTableTotal + langTableTotal + tman_general
		+ passTotal;

	strm << "BYTE COUNT TOTALS\n";
	strm << "Font:          " << font << "\n";
	strm << "Font cache:    " << fontCache << "\n";
	strm << "Font face:     " << fontFace << "\n";
	strm << "Engine         " << engineTotal << "\n";
	strm << "  Overhead:          " << eng_overhead << "\n";
	strm << "  Scalars:           " << eng_scalars << "\n";
	strm << "  Strings:           " << eng_strings << "\n";
	strm << "  Pointers:          " << eng_pointers << "\n";
	strm << "  Cmap table:        " << eng_cmap << "\n";
	strm << "  Name table:        " << eng_nameTable << "\n";
	strm << "  Pseudo map:        " << pseudoMap << "\n";
	strm << "  Class table:       " << classTableTotal << "\n";
	strm << "    Counters:              " << clstbl_counters << "\n";
	strm << "    Offsets:               " << clstbl_offsets << "\n";
	strm << "    Glyph list:            " << clstbl_glyphList << "\n";
	strm << "  Glyph table:       " << glyphTableTotal << "\n";
	strm << "    General:               " << glftbl_general << "\n";
	strm << "    Component defns:       " << glftbl_compDefns << "\n";
	strm << "    Attr table:            " << glftbl_attrTable << "\n";
	strm << "    Attr offsets:          " << glftbl_attrOffsets << "\n";
	strm << "  Language table:    " << langTableTotal << "\n";
	strm << "    General:               " << lngtbl_general << "\n";
	strm << "    Entries:               " << lngtbl_entries << "\n";
	strm << "    Feature sets:          " << lngtbl_featureSets << "\n";
	strm << "  Table manager:     " << tman_general << "\n";
	strm << "  Passes:            " << passTotal << "\n";
	strm << "    General:               " << pass_general << "\n";
	strm << "    FSM:                   " << pass_fsm << "\n";
	strm << "    Rule extras:           " << pass_ruleExtras << "\n";
	strm << "    Constraint offsets:    " << pass_constraintOffsets << "\n\n";
	strm << "    Constraint code:       " << pass_constraintCode << "\n";
	strm << "    Action offsets:        " << pass_actionOffsets << "\n";
	strm << "    Action code:           " << pass_actionCode << "\n\n";

	int slotTotal = slot_general + slot_abstract + slot_varLenBuf + slot_assocsAlloc + slot_attachAlloc;
	int streamTotal = sstrm_general + sstrm_chunkMapsAlloc + sstrm_reprocBuf + slotTotal;
	int engineStateTotal = engst_general + engst_passState + streamTotal;

	strm << " Engine State:  " << engineStateTotal << "\n";
	strm << "   General:           " << engst_general << "\n";
	strm << "   Pass states:       " << engst_passState << "\n";
	strm << "   Slot streams:      " << streamTotal << "\n";
	strm << "     General:               " << sstrm_general << "\n";
	strm << "     Chunk maps:            " << sstrm_chunkMapsAlloc << " (" << sstrm_chunkMapsUsed << " used)\n";
	strm << "     Reprocess buffer:      " << sstrm_reprocBuf << "\n";
	strm << "     Slots:                 " << slotTotal << "\n";
	strm << "       General:                   " << slot_general << "\n";
	strm << "       Abstract:                  " << slot_abstract << "\n";
	strm << "       Var-length buf:            " << slot_varLenBuf << "\n";
	strm << "       Associations:              " << slot_assocsAlloc << " (" << slot_assocsUsed << " used)\n";
	strm << "       Attachments:               " << slot_attachAlloc << " (" << slot_attachUsed << " used)\n\n";

	strm << "Total bytes used:    " << totalBytes << "\n\n";

	strm << "TOTALS PER FONT\n";
	for (size_t ifont = 0; ifont < vstrFontNames.size(); ifont++)
	{
		strm << vstrFontNames[ifont].c_str() << "\n";
		strm << "  Regular:     " << vFontTotalsReg[ifont] << "\n";
		strm << "  Bold:        " << vFontTotalsBold[ifont] << "\n";
		strm << "  Italic:      " << vFontTotalsItalic[ifont] << "\n";
		strm << "  Bold-italic: " << vFontTotalsBI[ifont] << "\n\n";
	}
}


//:>********************************************************************************************
//:>	Segment
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Initialize the data structure.
----------------------------------------------------------------------------------------------*/
void SegmentMemoryUsage::initialize()
{
	seg_count = 0;
	overhead = 0;
	pointers = 0;
	scalars = 0;
	strings = 0;
	metrics = 0;
	associations = 0;
	init = 0;
	obsolete = 0;

	slot_count = 0;
	slot_abstract = 0;
	slot_varLenBuf = 0;
	slot_scalars = 0;
	slot_clusterMembers = 0;

	glyphInfo_count = 0;
	glyphInfo = 0;

	wastedVector = 0;
}

/*----------------------------------------------------------------------------------------------
	Add the memory for the segemtn to the SegmentMemoryUsage data structure.
----------------------------------------------------------------------------------------------*/
void Segment::calculateMemoryUsage(SegmentMemoryUsage & smu)
{
	smu.addSegment(*this);
}


void SegmentMemoryUsage::addSegment(Segment & seg)
{
	seg_count++;

	overhead += sizeof(long);		// m_cref
	pointers += sizeof(void*);		// m_pgts
	scalars += sizeof(int);			// m_dichwLim
	scalars += sizeof(int);			// m_ichwMin
	pointers += sizeof(void*);		// m_pfont
	pointers += sizeof(void*);		// m_preneng
	scalars += sizeof(bool);		// m_fErroneous
	pointers += sizeof(void*);		// m_pgjus
	scalars += sizeof(LayoutEnvironment);
	scalars += sizeof(bool);		// m_fWsRtl
	scalars += sizeof(bool);		// m_fParaRtl
	scalars += sizeof(TrWsHandling);
	scalars += sizeof(int);			// m_nDirDepth
	init += sizeof(byte*);			// m_prgbNextSegDat
	init += sizeof(int);			// m_cbNextSegDat
	init += sizeof(byte) * seg.m_cbNextSegDat;
	init += sizeof(byte*);			// m_prgInitDat
	init += sizeof(int);			// m_cbInitDat
	init += sizeof(byte) * seg.m_cbInitDat;
	init += sizeof(int);			// m_dichPreContext
//	obsolete += sizeof(void *);		// m_psegPrev
//	obsolete += sizeof(void *);		// m_psegNext
//	strings += stringOverhead * 2;
//	strings += sizeof(wchar_t) * seg.m_stuFaceName.length();
//	strings += sizeof(wchar_t) * seg.m_stuBaseFaceName.length();
//	scalars += sizeof(bool);		// m_fUseSepBase;
//	scalars += sizeof(float);		// m_pixHeight
//	scalars += sizeof(bool);		// m_fBold
//	scalars += sizeof(bool);		// m_fItalic
	scalars += sizeof(LineBrk);		// m_lbStart
	scalars += sizeof(LineBrk);		// m_lbEnd
	scalars += sizeof(bool);		// m_fStartLine
	scalars += sizeof(bool);		// m_fEndLine
	scalars += sizeof(SegEnd);		// m_est
	metrics += sizeof(int);			// m_mFontEmUnits
	metrics += sizeof(float) * 17;		// was 19, removed m_x/ysDPI
	associations += sizeof(int);	// m_ichwAssocsMin
	associations += sizeof(int);	// m_ichwAssocsLim
	int dichw = seg.m_ichwAssocsLim - seg.m_ichwAssocsMin;
	associations += sizeof(int*);	// m_prgisloutBefore
	associations += sizeof(int) * dichw;
	associations += sizeof(int*);	// m_prgisloutAfter
	associations += sizeof(int) * dichw;
	associations += sizeof(void*);	// m_prgpvisloutAssocs
	associations += sizeof(void*) * dichw;
	for (int idichw = 0; idichw < dichw; idichw++)
	{
		std::vector<int> * pvisloutAssocs = seg.m_prgpvisloutAssocs[idichw];
		if (pvisloutAssocs)
		{
			associations += vectorOverhead;
			associations += sizeof(int) * pvisloutAssocs->capacity();
			wastedVector += sizeof(int) * (pvisloutAssocs->capacity() - pvisloutAssocs->size());
		}
	}
	associations += sizeof(int*);	// m_prgisloutLigature
	associations += sizeof(int) * dichw;
	associations += sizeof(sdata8*);	// m_prgiComponent
	associations += sizeof(sdata8) * dichw;
//	obsolete += sizeof(void*);		// m_psstrm
	scalars += sizeof(int);			// m_cslout
	slot_count += seg.m_cslout;
	for (int islout = 0; islout < seg.m_cslout; islout++)
	{
		GrSlotOutput * pslout = seg.m_prgslout + islout;
		slot_abstract += sizeof(gid16) * 2;		// m_chwGlyphID, m_chwActual
		slot_abstract += sizeof(sdata8) * 4;	// m_spsl, m_dirc, m_lb, m_nDirLevel
//		slot_abstract += sizeof(float) * 8;		// font/glyph metrics
		slot_abstract += sizeof(short) * 5;		// slot attributes - was 21
		slot_abstract += sizeof(int);			// m_mJWidth0;
		slot_abstract += sizeof(byte);			// m_nJWeight0;
		slot_abstract += sizeof(bool);			// m_fInsertBefore
		slot_abstract += sizeof(sdata8);		// m_bIsSpace
		slot_abstract += sizeof(byte);			// m_cnCompPerLig
		slot_abstract += sizeof(void*);			// m_prgnVarLenBuf
		slot_varLenBuf += pslout->CExtraSpaceSlout() * sizeof(u_intslot);
		slot_abstract += sizeof(float);			// m_xsPositionX, m_ysPositionY
//		slot_abstract += sizeof(bool);			// m_fAdvXSet, m_fAdvYSet
		slot_scalars += sizeof(int) * 2;		// m_ichwBefore/AfterAssoc
		slot_scalars += sizeof(sdata8);			// m_cComponents
		slot_scalars += sizeof(int);			// m_isloutClusterBase
		slot_scalars += sizeof(sdata8);			// m_disloutCluster
		slot_scalars += sizeof(float) * 3;		// m_xsClusterXOffset, m_xsClusterAdvance, m_xsAdvanceX
		slot_scalars += sizeof(int);			// m_igbb
//		slot_scalars += sizeof(Rect);			// m_rectBB
	}
	scalars += sizeof(int);				// m_cnUserDefn
	scalars += sizeof(int);				// m_cnCompPerLig
	scalars += sizeof(int);				// m_cnFeat
	glyphInfo_count += seg.m_cginf;
	scalars += sizeof(int);				// m_cginf;
	scalars += sizeof(int);				// m_isloutGinf0
	for (int iginf = 0; iginf < seg.m_cginf; iginf++)
	{
		///GlyphInfo * pginf = seg.m_prgginf + iginf;
		glyphInfo += sizeof(void *);	// m_pseg
		glyphInfo += sizeof(void *);	// m_pslout
		glyphInfo += sizeof(int);		// m_islout
	}
	init += sizeof(int);				// m_cslotRestartBackup
	init += vectorOverhead;
	init += sizeof(sdata8) * seg.m_vnSkipOffsets.capacity();
	wastedVector += sizeof(sdata8) * (seg.m_vnSkipOffsets.capacity() - seg.m_vnSkipOffsets.size());
	init += sizeof(DirCode);			// m_dircPrevStrong
	init += sizeof(DirCode);			// m_dircPrevTerm

	//int grandTotal;
	//grandTotal = overhead + pointers + scalars + strings + metrics + associations
	//	+ init + obsolete + slot_count + slot_abstract + slot_varLenBuf + slot_scalars
	//	+ slot_clusterMembers + glyphInfo_count + glyphInfo;

	//int totalPerSeg = grandTotal / seg_count;
}

/*----------------------------------------------------------------------------------------------
	Write out the memory usage onto a stream.
----------------------------------------------------------------------------------------------*/
void SegmentMemoryUsage::prettyPrint(std::ostream & strm)
{
	int totalBytes = overhead + pointers + scalars + strings + metrics + associations
		+ init + obsolete + slot_count + slot_abstract + slot_varLenBuf + slot_scalars
		+ slot_clusterMembers + glyphInfo_count + glyphInfo;

	int slotTotal = slot_abstract + slot_varLenBuf + slot_scalars + slot_clusterMembers;

	strm << "Number of segments: " << seg_count << "\n\n";

	strm << "TOTALS\n";
	strm << "Overhead:          " << overhead << "\n";
	strm << "Pointers:          " << pointers << "\n";
	strm << "Scalars:           " << scalars << "\n";
	strm << "Strings:           " << strings << "\n";
	strm << "Metrics:           " << metrics << "\n";
	strm << "Associations:      " << associations << "\n";
	strm << "Obsolete:          " << obsolete << "\n";
	strm << "Slot data:         " << slotTotal << "\n";
	strm << "  Abstract:              " << slot_abstract << "\n";
	strm << "  Var-length buffer:     " << slot_varLenBuf << "\n";
	strm << "  Scalars:               " << slot_scalars << "\n";
	strm << "  Cluster members:       " << slot_clusterMembers << "\n";
	strm << "Glyph info:        " << glyphInfo << "\n\n";

	strm << "Wasted in vectors: " << wastedVector << "\n\n";

	strm << "Total bytes used:  " << totalBytes << "\n\n";

	if (seg_count > 0)
	{
		strm << "AVERAGES\n";
		strm << "Overhead:         " << overhead/seg_count << "\n";
		strm << "Pointers:         " << pointers/seg_count << "\n";
		strm << "Scalars:          " << scalars/seg_count << "\n";
		strm << "Strings:          " << strings/seg_count << "\n";
		strm << "Metrics:          " << metrics/seg_count << "\n";
		strm << "Associations:     " << associations/seg_count << "\n";
		strm << "Obsolete:         " << obsolete/seg_count << "\n";
		strm << "Slot data:        " << slotTotal/seg_count << "\n";
		strm << "  Abstract:             " << slot_abstract/seg_count << "\n";
		strm << "  Var-length buffer:    " << slot_varLenBuf/seg_count << "\n";
		strm << "  Scalars:              " << slot_scalars/seg_count << "\n";
		strm << "  Cluster members:      " << slot_clusterMembers/seg_count << "\n";
		strm << "Glyph info:       " << glyphInfo/seg_count << "\n\n";

		strm << "Avg. bytes per segment: " << totalBytes / seg_count << "\n\n";

		strm << "Avg. # of slots per segment: " << slot_count / seg_count << "\n\n";
	}
}

} //namespace gr

