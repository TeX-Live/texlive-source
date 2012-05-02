/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2000, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TransductionLog.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Contains the functions for writing a log of the transduction process.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif
#include <cstring>
#include <math.h>

//:End Ignore

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local Constants and static variables
//:>********************************************************************************************

namespace gr
{

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************

#define SP_PER_SLOT 7
#define LEADING_SP 15
#define MAX_SLOTS 128

/*----------------------------------------------------------------------------------------------
	Output a file showing a log of the transduction process and the resulting segment.
----------------------------------------------------------------------------------------------*/
//bool GrTableManager::WriteTransductionLog(GrCharStream * pchstrm, Segment * psegRet,
//	int cbPrevSegDat, byte * pbPrevSegDat, byte * pbNextSegDat, int * pcbNextSegDat)
//{
//#ifdef TRACING
//	std::string staFile;
//	if (!LogFileName(staFile))
//		return false;
//
//	std::ofstream strmOut;
//	if (cbPrevSegDat > 0)
//		strmOut.open(staFile.c_str(), std::ios::app);	// append
//	else
//		strmOut.open(staFile.c_str());
//	if (strmOut.fail())
//		return false;
//
//	WriteXductnLog(strmOut, pchstrm, psegRet, cbPrevSegDat, pbPrevSegDat);
//
//	strmOut.close();
//	return true;
//#else
//	return false;
//#endif // TRACING
//}

#ifdef TRACING
bool GrTableManager::WriteTransductionLog(std::ostream * pstrmLog,
	GrCharStream * pchstrm, Segment * psegRet, int cbPrevSegDat, byte * pbPrevSegDat)
#else
bool GrTableManager::WriteTransductionLog(std::ostream *,GrCharStream *,
                                          Segment *, int, byte *)
#endif // !TRACING
{
#ifdef TRACING
	if (!pstrmLog)
		return false;
	std::ostream & strmOut = *pstrmLog;
	WriteXductnLog(strmOut, pchstrm, psegRet, cbPrevSegDat, pbPrevSegDat);
	return true;
#else
	return false;
#endif // !TRACING
}

/*----------------------------------------------------------------------------------------------
	Append to the file, showing the surface/underlying mappings. This must be done after
	figuring out all the associations.

	Also write the final glyph positions, since they are now determined.
----------------------------------------------------------------------------------------------*/
//bool GrTableManager::WriteAssociationLog(GrCharStream * pchstrm, Segment * psegRet)
//{
//#ifdef TRACING
//	std::string staFile;
//	if (!LogFileName(staFile))
//		return false;
//
//	std::ofstream strmOut;
//	strmOut.open(staFile.c_str(), std::ios::app);	// append
//	if (strmOut.fail())
//		return false;
//
//	LogFinalPositions(strmOut);
//
//	strmOut << "\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n";
//
//	psegRet->LogUnderlyingToSurface(this, strmOut, pchstrm);
//	psegRet->LogSurfaceToUnderlying(this, strmOut);
//
//	strmOut << "\n\n=======================================================================\n\n";
//
//	strmOut.close();
//	return true;
//#else
//	return false;
//#endif // TRACING
//}


#ifdef TRACING
bool GrTableManager::WriteAssociationLog(std::ostream * pstrmLog,
	GrCharStream * pchstrm, Segment * psegRet)
#else
bool GrTableManager::WriteAssociationLog(std::ostream *,
	GrCharStream *, Segment *)
#endif
{
#ifdef TRACING
	if (!pstrmLog)
		return false;

	std::ostream & strmOut = *pstrmLog;
	LogFinalPositions(strmOut);

	strmOut << "\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n";

	psegRet->LogUnderlyingToSurface(this, strmOut, pchstrm);
	psegRet->LogSurfaceToUnderlying(this, strmOut);

	strmOut << "\n\n=======================================================================\n\n";

	return true;
#else
	return false;
#endif // TRACING
}

#ifdef TRACING

/*----------------------------------------------------------------------------------------------
	Generate the name of the file where the log will be written. It goes in the directory
	defined by either the TEMP or TMP environment variable.

	TODO: remove
----------------------------------------------------------------------------------------------*/
//bool GrTableManager::LogFileName(std::string & staFile)
//{
//	char * pchTmpEnv = getenv("TEMP");
//	if (pchTmpEnv == 0)
//		pchTmpEnv = getenv("TMP");
//	if (pchTmpEnv == 0)
//		return false;
//
//	staFile.assign(pchTmpEnv);
//	if (staFile[staFile.size() - 1] != '\\')
//		staFile.append("\\");
//    staFile.append("gr_xductn.log");
//
//	return true;
//}

/*----------------------------------------------------------------------------------------------
	Output a file showing a log of the transduction process and the resulting segment.
----------------------------------------------------------------------------------------------*/
void GrTableManager::WriteXductnLog(std::ostream & strmOut,
	GrCharStream * pchstrm, Segment * /*psegRet*/,
	int cbPrevSegDat, byte * pbPrevSegDat)
{
	if (cbPrevSegDat == 0)
		LogUnderlying(strmOut, pchstrm, 0);
	else
	{
		Assert(*(pbPrevSegDat + 4) == 0);	// skip offset for first pass
		LogUnderlying(strmOut, pchstrm, *(pbPrevSegDat + 3));
	}

	LogPass1Input(strmOut);

	for (int ipass = 1; ipass < m_cpass; ipass++)
	{
		if (cbPrevSegDat == 0)
			LogPassOutput(strmOut, ipass, 0);
		else
			LogPassOutput(strmOut, ipass, *(pbPrevSegDat + 4 + ipass));
	}

	// Do this later, after outputting final positions:
	//strmOut << "\n\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n";
}

/*----------------------------------------------------------------------------------------------
	Write out a log of the underlying input.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogUnderlying(std::ostream & strmOut, GrCharStream * pchstrm,
	int cchwBackup)
{
	strmOut << "UNDERLYING INPUT\n\n";

	int rgnChars[MAX_SLOTS];
	bool rgfNewRun[MAX_SLOTS];
	std::fill_n(rgfNewRun, MAX_SLOTS, false);
	GrFeatureValues rgfval[MAX_SLOTS];
	int cchwMaxRawChars;

	int cnUtf32 = pchstrm->GetLogData(this, rgnChars, rgfNewRun, rgfval,
		cchwBackup, &cchwMaxRawChars);
	cnUtf32 = min(cnUtf32, MAX_SLOTS);

	int ichw;

	// For UTF-8 and surrogate representation:
	utf16 rgchwChars2[MAX_SLOTS];
	utf16 rgchwChars3[MAX_SLOTS];
	utf16 rgchwChars4[MAX_SLOTS];
	utf16 rgchwChars5[MAX_SLOTS];
	utf16 rgchwChars6[MAX_SLOTS];
	int rgcchwRaw[MAX_SLOTS];

	if (cchwMaxRawChars > 1)
	{
		cchwMaxRawChars = min(cchwMaxRawChars, 6); // max of 6 raw (UTF-8 or UTF-16) chars per slot
		pchstrm->GetLogDataRaw(this, cnUtf32, cchwBackup, cchwMaxRawChars,
			rgnChars, rgchwChars2, rgchwChars3, rgchwChars4, rgchwChars5, rgchwChars6,
			rgcchwRaw);
	}
	else
	{
		for (ichw = 0; ichw < cnUtf32; ichw++)
		{
			rgcchwRaw[ichw] = 1;
			rgchwChars2[ichw] = 0;
			rgchwChars3[ichw] = 0;
			rgchwChars4[ichw] = 0;
			rgchwChars5[ichw] = 0;
			rgchwChars6[ichw] = 0;
		}
	}

	LogUnderlyingHeader(strmOut, pchstrm->Min(), (pchstrm->Min() + cnUtf32 - cchwBackup),
		cchwBackup, rgcchwRaw);

	//	Text
	strmOut << "Text:          ";	// 15 spaces
	for (ichw = 0; ichw < cnUtf32; ichw++)
	{
		if (rgnChars[ichw] < 0x0100 && rgchwChars2[ichw] == 0) // ANSI
			strmOut << (char)rgnChars[ichw] << "      ";	// 6 spaces
		else if (rgnChars[ichw] == knLRM)
			strmOut << "<LRM>  ";
		else if (rgnChars[ichw] == knRLM)
			strmOut << "<RLM>  ";
		else if (rgnChars[ichw] == knLRO)
			strmOut << "<LRO>  ";
		else if (rgnChars[ichw] == knRLO)
			strmOut << "<RLO>  ";
		else if (rgnChars[ichw] == knLRE)
			strmOut << "<LRE>  ";
		else if (rgnChars[ichw] == knRLE)
			strmOut << "<RLE>  ";
		else if (rgnChars[ichw] == knPDF)
			strmOut << "<PDF>  ";
		else
			strmOut << ".      ";
	}
	strmOut << "\n";

	//	Unicode
	strmOut << "Unicode:       ";
	for (ichw = 0; ichw < cnUtf32; ichw++)
		LogHexInTable(strmOut, utf16(rgnChars[ichw]));
	strmOut << "\n";
	for (int icchRaw = 2; icchRaw <= cchwMaxRawChars; icchRaw++)
	{
		strmOut << "               ";
		for (ichw = 0; ichw < cnUtf32; ichw++)
		{
			utf16 chw;
			switch (icchRaw)
			{
			case 2: chw = rgchwChars2[ichw]; break;
			case 3: chw = rgchwChars3[ichw]; break;
			case 4: chw = rgchwChars4[ichw]; break;
			case 5: chw = rgchwChars5[ichw]; break;
			case 6: chw = rgchwChars6[ichw]; break;
			default: chw = 0;
			}
			if (chw == 0)
				strmOut << ".      ";
			else
				LogHexInTable(strmOut, chw);
		}
		strmOut << "\n";
	}

	//	Runs
	strmOut << "Runs:          ";
	int crun = 0;
	for (ichw = 0; ichw < cnUtf32; ichw++)
	{
		if (rgfNewRun[ichw])
		{
			crun++;
			strmOut << "|" << crun << ((crun < 10) ? "     " : "    ");
		}
		else
			strmOut << ".      ";
	}
	strmOut << "\n";

	//	Features
	strmOut << "Features and character properties:\n";
	crun = 0;
	for (ichw = 0; ichw < cnUtf32; ichw++)
	{
		if (rgfNewRun[ichw])
		{
			crun++;
			strmOut << "  Run " << crun << ": ";
			rgfval[ichw].WriteXductnLog(this, strmOut);

		}
	}
}


void GrFeatureValues::WriteXductnLog(GrTableManager * ptman, std::ostream & strmOut)
{
	bool fFirst = true;
	for (int i = 0; i < kMaxFeatures; i++)
	{
		if (m_rgnFValues[i] != 0)
		{
			GrFeature * pfeat = ptman->Feature(i);
			if (!fFirst)
				strmOut << ",";
			strmOut << pfeat->ID() << "=" << m_rgnFValues[i];
			fFirst = false;
		}
	}
	if (fFirst)
		strmOut << "all features=0";

	strmOut << "\n\n";
}

/*----------------------------------------------------------------------------------------------
	Loop through the input stream of underlying characters, storing information in the
	arrays for the log.
	ENHANCE SharonC: do we need to show the underline properties?
----------------------------------------------------------------------------------------------*/
int GrCharStream::GetLogData(GrTableManager * ptman, int * rgchl, bool * rgfNewRun,
	GrFeatureValues * rgfval, int cchrBackup, int * pcchrMax)
{
	Assert(cchrBackup <= m_cchrBackedUp);

	int ichrPosToStop = m_ichrPos;
	int ichrStart = m_ichrMin - max(cchrBackup, m_cchrBackedUp);

	*pcchrMax = 0;

	//	Restart the stream.
	m_ichrPos = ichrStart;
	m_ichrRunMin = 0;
	m_ichrRunLim = 0;
	m_ichrRunOffset = kPosInfinity;
	m_vislotNextChunkMap.clear();

	int cchrSkipBackup = m_cchrBackedUp - cchrBackup;

	int c = 0;
	int cchr = 0;

	while (m_ichrPos < ichrPosToStop)
	{
		if ((c < MAX_SLOTS) && (m_ichrPos >= m_ichrRunLim))
		{
			rgfNewRun[c] = true;
		}

		GrFeatureValues fval;
		int ichrOffset, cchrThis;
		int chl = NextGet(ptman, &fval, &ichrOffset, &cchrThis);
		cchr += cchrThis;
		if (cchr <= cchrSkipBackup)
		{}	// ignore - this is before the pre-segment stuff that we want to skip
		else if (c < MAX_SLOTS)
		{
			rgchl[c] = chl;
			*pcchrMax = max(*pcchrMax, cchrThis);
			if (rgfNewRun[c])
			{
				rgfval[c] = fval;
			}
			c++;
		}
	}

	////return (m_ichwPos - (m_ichwMin - cchwBackup));
	return c;
}

/*----------------------------------------------------------------------------------------------
	If any of the characters are comprised of surrogates or UTF-8, fill in the arrays with 
	the raw (UTF-16 or UTF-8) chars for display. To do this we get the raw characters
	directly from the text source.
----------------------------------------------------------------------------------------------*/
void GrCharStream::GetLogDataRaw(GrTableManager * /*ptman*/, int cchl, int cchrBackup,
	int /*cchrMaxRaw*/, int * prgchl,
	utf16 * prgchw2, utf16 * prgchw3, utf16 * prgchw4, utf16 * prgchw5, utf16 * prgchw6, 
	int * prgcchr)
{
	for (int i = 0; i < cchl; i++)
	{
		prgchw2[i] = 0;
	}

	int ichrLim = m_ichrPos;
	int ichrMin = m_ichrMin - cchrBackup;

	int cchrRange = ichrLim - ichrMin;
	int ichr;
	utf16 * prgchwRunText = new utf16[cchrRange];
	utf8 * prgchsRunText8 = NULL;

	UtfType utf = m_pgts->utfEncodingForm();

	switch (utf)
	{
	case kutf8:
		prgchsRunText8 = new utf8[cchrRange];
		m_pgts->fetch(ichrMin, cchrRange, prgchsRunText8);
		for (ichr = 0; ichr < cchrRange; ichr++)
			prgchwRunText[ichr] = (utf16)prgchsRunText8[ichr];	// zero-extend into UTF-16 buffer
		break;
	case kutf16:
		m_pgts->fetch(ichrMin, cchrRange, prgchwRunText);
		break;
	default:
	case kutf32:	// this method should never have been called
		Assert(false);	
		for (int ichrLp = 0; ichrLp < cchrRange; ichrLp++)
		{
			prgcchr[ichrLp] = 1;
			prgchw2[ichrLp] = 0;
			prgchw3[ichrLp] = 0;
			prgchw4[ichrLp] = 0;
			prgchw5[ichrLp] = 0;
			prgchw6[ichrLp] = 0;
		}
		return;
	}

	ichr = ichrMin;
	int ichl = 0;
	while (ichr < ichrLim)
	{
		int cchrThis = 1;
		// Replace 32-bit char with (first) 16-bit or 8-bit char.
		prgchl[ichl] = (int)prgchwRunText[ichr - ichrMin];
		prgcchr[ichr] = cchrThis;
		ichr++;

		while (!AtUnicodeCharBoundary(prgchwRunText, cchrRange, ichr - ichrMin, utf))
		{
			cchrThis++;
			switch (cchrThis)
			{
			case 2:	prgchw2[ichl] = prgchwRunText[ichr - ichrMin];	break;
			case 3:	prgchw3[ichl] = prgchwRunText[ichr - ichrMin];	break;
			case 4:	prgchw4[ichl] = prgchwRunText[ichr - ichrMin];	break;
			case 5:	prgchw5[ichl] = prgchwRunText[ichr - ichrMin];	break;
			case 6:	prgchw6[ichl] = prgchwRunText[ichr - ichrMin];	break;
			default:
				break;
			}
			prgcchr[ichr] = cchrThis;
			ichr++;
		}
		switch (cchrThis)
		{
		case 1:
			prgchw2[ichl] = 0;
			// fall through
		case 2:
			prgchw3[ichl] = 0;
			// fall through
		case 3:
			prgchw4[ichl] = 0;
			// fall through
		case 4:
			prgchw5[ichl] = 0;
			// fall through
		case 5:
			prgchw6[ichl] = 0;
		}
		ichl++;
	}

	delete[] prgchwRunText;
	delete[] prgchsRunText8;
}

/*----------------------------------------------------------------------------------------------
	Output the glyph IDs generated by the glyph generation pass (pass 0).
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogPass1Input(std::ostream & strmOut)
{
	strmOut << "INPUT TO PASS 1\n\n";

	GrSlotStream * psstrm = OutputStream(0);

	LogSlotHeader(strmOut, psstrm->WritePos(), SP_PER_SLOT, LEADING_SP);

	LogSlotGlyphs(strmOut, psstrm);

	strmOut << "\n";
}

/*----------------------------------------------------------------------------------------------
	Output the the results of pass.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogPassOutput(std::ostream & strmOut, int ipass, int cslotSkipped)
{
	strmOut << "\n";

	GrPass * ppass = Pass(ipass);
	GrSlotStream * psstrmIn = InputStream(ipass);
	GrSlotStream * psstrmOut = OutputStream(ipass);

	int islot;
	//	Mark each slot with its index in the input and output streams.
	for (islot = 0; islot < psstrmIn->ReadPos(); islot++)
		psstrmIn->SlotAt(islot)->m_islotTmpIn = islot;

	for (islot = 0; islot < psstrmOut->WritePos(); islot++)
		psstrmOut->SlotAt(islot)->m_islotTmpOut = islot;

	ppass->LogRulesFiredAndFailed(strmOut, psstrmIn, m_ipassJust1);

	strmOut << "\nOUTPUT OF PASS " << ipass;
	//if (dynamic_cast<GrBidiPass *>(ppass))
	//	strmOut << " (bidi)";
	//else if (dynamic_cast<GrSubPass *>(ppass))
	//{
	//	if (ipass >= m_ipassJust1)
	//		strmOut << " (justification)";
	//	else
	//		strmOut << " (substitution)";
	//}
	//else if (dynamic_cast<GrPosPass *>(ppass))
	//	strmOut << " (positioning)";
	//else if (dynamic_cast<GrLineBreakPass *>(ppass))
	//	strmOut << " (linebreak)";
	strmOut << "\n";

	ppass->LogInsertionsAndDeletions(strmOut, psstrmOut);

	LogSlotHeader(strmOut, psstrmOut->WritePos(), SP_PER_SLOT, LEADING_SP);

	LogSlotGlyphs(strmOut, psstrmOut);

	bool fAnyPseudos = false;
	if (dynamic_cast<GrPosPass *>(ppass))
	{
		for (islot = 0; islot < psstrmOut->WritePos(); islot++)
		{
			GrSlotState * pslotTmp = psstrmOut->SlotAt(islot);
			if (pslotTmp->GlyphID() != pslotTmp->ActualGlyphForOutput(this))
			{
				fAnyPseudos = true;
				break;
			}
		}
	}
	if (fAnyPseudos)
	{
		strmOut << "Actual glyphs: ";
		for (islot = 0; islot < psstrmOut->WritePos(); islot++)
		{
			GrSlotState * pslotTmp = psstrmOut->SlotAt(islot);
			if (pslotTmp->GlyphID() != pslotTmp->ActualGlyphForOutput(this))
				LogHexInTable(strmOut, pslotTmp->ActualGlyphForOutput(this));
			else
				strmOut << ".      ";
		}
		strmOut << "\n";
	}

	LogAttributes(strmOut, ipass);

	// Do this later, after we're sure the positions have been set:
	//if (ipass == m_cpass - 1)
	//	LogFinalPositions(strmOut);

	if (cslotSkipped > 0)
	{
		strmOut << "\n               ";
		for (islot = 0; islot < cslotSkipped; islot++)
			strmOut << "SKIP   ";
		strmOut << "\n";
	}

	//	If this was the pass just before the justification routines get run, output a
	//	special line that just shows the results, ie, the values of justify.width.
	//	TODO: adjust this when measure mode get functioning.
	if (ipass == m_ipassJust1 - 1 && ShouldLogJustification()
		&& m_engst.m_jmodi == kjmodiJustify)
	{
		strmOut << "\nJUSTIFICATION\n\n";
		LogSlotHeader(strmOut, psstrmOut->WritePos(), SP_PER_SLOT, LEADING_SP);
		LogSlotGlyphs(strmOut, psstrmOut);
		LogAttributes(strmOut, ipass, true);
	}
}

/*----------------------------------------------------------------------------------------------
	Write the list of rules fired and failed for a given pass.
----------------------------------------------------------------------------------------------*/
void GrPass::LogRulesFiredAndFailed(std::ostream & strmOut, GrSlotStream * psstrmIn, int ipassJust1)
{
	strmOut << "PASS " << m_ipass;
	if (dynamic_cast<GrBidiPass *>(this))
		strmOut << " (bidi)";
	else if (dynamic_cast<GrSubPass *>(this))
	{
		if (m_ipass >= ipassJust1)
			strmOut << " (justification)";
		else
			strmOut << " (substitution)";
	}
	else if (dynamic_cast<GrPosPass *>(this))
		strmOut << " (positioning)";
	else if (dynamic_cast<GrLineBreakPass *>(this))
		strmOut << " (linebreak)";

	strmOut << "\n\n" ;

	if (m_cbPassConstraint > 0 && m_prgbPConstraintBlock != NULL)
		m_pzpst->LogPassConstraints(strmOut);

	if (!dynamic_cast<GrBidiPass *>(this))
		m_pzpst->LogRulesFiredAndFailed(strmOut, psstrmIn);
}

void PassState::LogPassConstraints(std::ostream & strmOut)
{
	if (m_crngrec == 0)
	{}
	else if (m_crngrec == 1)
	{
		if (m_rgrngrec[0].m_fSucceeds)
			strmOut << "Pass constraint passed\n\n";
		else
			strmOut << "Pass constraint failed\n\n";
	}
	else
	{
		strmOut << "Pass constraint:\n";
		for (int irngrec = 0; irngrec < m_crngrec; irngrec++)
		{
			strmOut << " " << m_rgrngrec[irngrec].m_islotMin << ".." << m_rgrngrec[irngrec].m_islotLim-1 << ": ";
			strmOut << ((m_rgrngrec[irngrec].m_fSucceeds) ? "passed\n" : "failed\n");
		}
		strmOut << "\n";
	}
}

void PassState::LogRulesFiredAndFailed(std::ostream & strmOut, GrSlotStream * /*psstrmIn*/)
{
	strmOut << "Rules matched: ";
	if (m_crulrec == 0)
		strmOut << "none";
	strmOut << "\n";

	for (int irulrec = 0; irulrec < m_crulrec; irulrec++)
	{
		if (m_rgrulrec[irulrec].m_fFired)
			strmOut << " * ";
		else
			strmOut << "   ";

		strmOut << m_rgrulrec[irulrec].m_islot << ". ";

		if (m_rgrulrec[irulrec].m_irul == PassState::kHitMaxRuleLoop)
			strmOut << "hit MaxRuleLoop\n";
		else if (m_rgrulrec[irulrec].m_irul == PassState::kHitMaxBackup)
			strmOut << "hit MaxBackup\n";
		else
		{
			strmOut << "rule " << m_ipass << "." << m_rgrulrec[irulrec].m_irul;

			if (m_rgrulrec[irulrec].m_fFired)
				strmOut << " FIRED\n";
			else
				strmOut << " failed\n";
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Write out the marks on top of the slot table indicating insertions and deletions.
----------------------------------------------------------------------------------------------*/
void GrPass::LogInsertionsAndDeletions(std::ostream & strmOut, GrSlotStream * psstrmOut)
{
	m_pzpst->LogInsertionsAndDeletions(strmOut, psstrmOut);
}

void PassState::LogInsertionsAndDeletions(std::ostream & strmOut, GrSlotStream * psstrmOut)
{
	int cslotDel = m_rgcslotDeletions[0];
	bool fIns;
	if (cslotDel > 1)
		strmOut << "\n          DEL-" << cslotDel;
	else if (cslotDel == 1)
		strmOut << "\n           DEL ";
	else
		strmOut << "\n               ";

	for (int islot = 0; islot < psstrmOut->WritePos(); islot++)
	{
		cslotDel = (islot + 1 >= MAX_SLOTS) ? 0 : m_rgcslotDeletions[islot + 1];
		fIns = (islot >= MAX_SLOTS) ? 0 : m_rgfInsertion[islot];

		if (fIns)
		{
			strmOut << "INS";
			if (cslotDel > 0)
			{
				if (cslotDel > 1)
					strmOut << "/D-" << cslotDel;
				else
					strmOut << "/DEL";
			}
			else
				strmOut << "    ";

		}
		else if (cslotDel > 0)
		{
			if (cslotDel >= 10)
				strmOut << "DEL-" << cslotDel << " ";
			else if (cslotDel > 1)
				strmOut << " DEL-" << cslotDel << " ";
			else
				strmOut << "  DEL  ";
		}
		else
			strmOut << ".      ";
	}

	strmOut << "\n";
}

/*----------------------------------------------------------------------------------------------
	Write out a line for each slot attribute that changed during the pass.

	@param	fJustWidths - a special pass that writes out just the effects of the justification
				routines, ie, the values of justify.width
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogAttributes(std::ostream & strmOut, int ipass,
	bool fJustWidths)
{
	//	Figure out which slot attributes were modified for some slot during the pass.
	bool * prgfMods = new bool[kslatMax + NumUserDefn() - 1];

	bool fPreJust = (!fJustWidths && ipass == m_ipassJust1 - 1 && ShouldLogJustification());
	bool fPostJust = ((fJustWidths || ipass == m_ipassJust1) && ShouldLogJustification());

	int ccomp;	// max number of components per slot
	int cassoc;	// max number of associations per slot
	SlotAttrsModified(ipass, prgfMods, fPreJust, &ccomp, &cassoc);

	if (fPreJust)
	{
	//	prgfMods[kslatJStretch] = true;
	//	prgfMods[kslatJShrink] = true;
	//	prgfMods[kslatJStep] = true;
	//	prgfMods[kslatWeight] = true;
		prgfMods[kslatJWidth] = false;	// output j.width in its own line (we call this
										// method with fJustWidths == true)
	}
	else if (fPostJust)
		prgfMods[kslatJWidth] = true;

	GrPass * ppass = Pass(ipass);
	GrPass * ppassNext = (ipass < m_cpass - 1) ? Pass(ipass + 1) : NULL;
	GrSlotStream * psstrm = OutputStream(ipass);

	if (fJustWidths)
	{}
	else if (dynamic_cast<GrBidiPass *>(ppass))
	{
		//	Special stuff for Bidi pass:
		//	Log value of directionality attribute.
		strmOut << "directionality ";
		int islot;
		for (islot = 0; islot < psstrm->WritePos(); islot++)
		{
			GrSlotState * pslot = psstrm->SlotAt(islot);
			LogDirCodeInTable(strmOut, pslot->DirProcessed());
		}
		strmOut << "\n";

		//	Log final direction level for bidi pass.
		strmOut << "dir level:     ";
		for (islot = 0; islot < psstrm->WritePos(); islot++)
		{
			GrSlotState * pslot = psstrm->SlotAt(islot);
			LogInTable(strmOut, pslot->DirLevel());
		}
		strmOut << "\n";
	}
	else if (ppassNext && dynamic_cast<GrBidiPass *>(ppassNext))
	{
		//	Next pass is Bidi: log input values of directionality attribute that are input for it.
		strmOut << "directionality ";
		for (int islot = 0; islot < psstrm->WritePos(); islot++)
		{
			GrSlotState * pslot = psstrm->SlotAt(islot);
			LogDirCodeInTable(strmOut, pslot->Directionality());
		}
		strmOut << "\n";
	}

	for (int slat = 0; slat < kslatMax + NumUserDefn() - 1; slat++)
	{
		int cIndexLim = 1;
		if (slat == kslatCompRef)
			cIndexLim = ccomp;
		else if (slat == kslatUserDefn)
			cIndexLim = 1; // kMaxUserDefinedSlotAttributes;

		if (fJustWidths && slat != kslatJWidth)
			continue;

		bool fValidAttr = true;
		for (int iIndex = 0; iIndex < cIndexLim; iIndex++)
		{
			if (prgfMods[slat])
			{
				switch(slat)
				{
				case kslatAdvX:			strmOut << "advance.x      "; break;
				case kslatAdvY:			strmOut << "advance.y      "; break;
				case kslatAttTo:		strmOut << "att.to         "; break;
				case kslatAttAtX:		strmOut << "att.at.x       "; break;
				case kslatAttAtY:		strmOut << "att.at.y       "; break;
				case kslatAttAtGpt:		strmOut << "att.at.gpt     "; break;
				case kslatAttAtXoff:	strmOut << "att.at.xoff    "; break;
				case kslatAttAtYoff:	strmOut << "att.at.yoff    "; break;
				case kslatAttWithX:		strmOut << "att.with.x     "; break;
				case kslatAttWithY:		strmOut << "att.with.y     "; break;
				case kslatAttWithGpt:	strmOut << "att.with.gpt   "; break;
				case kslatAttWithXoff:	strmOut << "att.with.xoff  "; break;
				case kslatAttWithYoff:	strmOut << "att.with.yoff  "; break;
				case kslatAttLevel:		strmOut << "att.level      "; break;
				case kslatBreak:		strmOut << "breakweight    "; break;
				case kslatCompRef:		strmOut << "component " << iIndex + 1 // 1-based
											<< "    "; break;
				case kslatDir:			strmOut << "dir            "; break;
				case kslatInsert:		strmOut << "insert         "; break;
				case kslatMeasureSol:	strmOut << "measure.sol    "; break;
				case kslatMeasureEol:	strmOut << "measure.eol    "; break;
				case kslatJStretch:		strmOut << "j.stretch      "; break;
				case kslatJShrink:		strmOut << "j.shrink       "; break;
				case kslatJStep:		strmOut << "j.step         "; break;
				case kslatJWeight:		strmOut << "j.weight       "; break;
				case kslatJWidth:		strmOut << "j.width        "; break;
				case kslatPosX:
				case kslatPosY:
					Assert(false);
					break;
				case kslatShiftX:		strmOut << "shift.x        "; break;
				case kslatShiftY:		strmOut << "shift.y        "; break;
				case kslatSegsplit:		strmOut << "segsplit       "; break;
				default:
					if (kslatUserDefn <= slat &&
						slat < kslatUserDefn + NumUserDefn())
					{
						strmOut << "user" << (slat - kslatUserDefn + 1) // 1-based
							<< ((iIndex >= 9) ? "         " : "          ");
					}
					else
					{
						// Invalid attribute:
						Warn("bad slot attribute");
						fValidAttr = false;
						break;
					}
				}

				if (!fValidAttr)
					break; // out of iIndex loop

				for (int islot = 0; islot < psstrm->WritePos(); islot++)
				{
					GrSlotState * pslot = psstrm->SlotAt(islot);
					pslot->LogSlotAttribute(this, strmOut, ipass, slat, iIndex,
						fPreJust, fPostJust);
				}

				strmOut << "\n";
			}
		}
	}

	if (fJustWidths)
		goto LDone;

	for (int iassoc = 0; iassoc < cassoc; iassoc++)
	{
		//	Log associations. Put them on one line it that will work; otherwise use several.
		bool fBoth = (cassoc <= 2);
		bool fAfter = (iassoc == (cassoc - 1));
		if (fBoth)
			strmOut << "assocs         ";
		else if (iassoc == 0)
			strmOut << "assocs-before  ";
		else if (fAfter)
			strmOut << "      -after   ";
		else
			strmOut << "      -other   ";

		for (int islot = 0; islot < psstrm->WritePos(); islot++)
		{
			GrSlotState * pslot = psstrm->SlotAt(islot);
			pslot->LogAssociation(this, strmOut, ipass, iassoc, fBoth, fAfter);
		}

		strmOut << "\n";

		if (fBoth)
			break;
	}

	if (cassoc == 0 && dynamic_cast<GrBidiPass *>(ppass))
	{
		strmOut << "assocs         ";

		//	Log associations for all slots moved during the Bidi pass.
		for (int islot = 0; islot < psstrm->WritePos(); islot++)
		{
			GrSlotState * pslot = psstrm->SlotAt(islot);
			if (pslot->m_islotTmpIn != pslot->m_islotTmpOut)
				LogInTable(strmOut, pslot->m_islotTmpIn);
			else
				strmOut << ".      ";
		}

		strmOut << "\n";
	}

LDone:

	delete[] prgfMods;
}

/*----------------------------------------------------------------------------------------------
	Write out the final positions.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogFinalPositions(std::ostream & strmOut)
{
	GrSlotStream * psstrm = OutputStream(m_cpass - 1);

	strmOut << "x position:    ";
	for (int islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		if (pslot->IsLineBreak(LBGlyphID()))
		{
			strmOut << ".      ";
			continue;
		}
		LogInTable(strmOut, pslot->XPosition());
	}
	strmOut << "\n";

	strmOut << "y position:    ";
	for (int islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		if (pslot->IsLineBreak(LBGlyphID()))
		{
			strmOut << ".      ";
			continue;
		}
		LogInTable(strmOut, pslot->YPosition());
	}
	strmOut << "\n";
}
#endif // TRACING

/*----------------------------------------------------------------------------------------------
	Write out the final underlying-to-surface associations.
----------------------------------------------------------------------------------------------*/
#ifdef TRACING
void Segment::LogUnderlyingToSurface(GrTableManager * ptman, std::ostream & strmOut,
	GrCharStream * pchstrm)
#else
void Segment::LogUnderlyingToSurface(GrTableManager *, std::ostream &,
	GrCharStream *)
#endif
{
#ifdef TRACING
	strmOut << "\n\nUNDERLYING TO SURFACE MAPPINGS\n\n";

	size_t cassocs = 0;
	int fLigs = false;
	int ichw;
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		if (m_prgpvisloutAssocs[ichw])
			cassocs = max(cassocs, m_prgpvisloutAssocs[ichw]->size());
		if (m_prgisloutLigature[ichw] != kNegInfinity)
			fLigs = true;
	}

//	ptman->LogSlotHeader(strmOut, m_ichwAssocsLim, SP_PER_SLOT, LEADING_SP, m_ichwAssocsMin);
	ptman->LogUnderlyingHeader(strmOut, pchstrm->Min(), (pchstrm->Min() + m_ichwAssocsLim),
		-m_ichwAssocsMin, NULL);

	int rgnChars[MAX_SLOTS];
	bool rgfNewRun[MAX_SLOTS];
	std::fill_n(rgfNewRun, MAX_SLOTS, false);
	GrFeatureValues rgfval[MAX_SLOTS];
	int cchwMaxRawChars;

	int cchw = pchstrm->GetLogData(ptman, rgnChars, rgfNewRun, rgfval,
		-m_ichwAssocsMin, &cchwMaxRawChars);
	cchw = min(cchw, MAX_SLOTS);

	utf16 rgchwChars2[MAX_SLOTS];
	utf16 rgchwChars3[MAX_SLOTS];
	utf16 rgchwChars4[MAX_SLOTS];
	utf16 rgchwChars5[MAX_SLOTS];
	utf16 rgchwChars6[MAX_SLOTS];
	int rgcchwRaw[MAX_SLOTS];
	if (cchwMaxRawChars > 1)
	{
		cchwMaxRawChars = min(cchwMaxRawChars, 6);
		pchstrm->GetLogDataRaw(ptman, cchw, -m_ichwAssocsMin, cchwMaxRawChars,
			rgnChars, rgchwChars2, rgchwChars3, rgchwChars4, rgchwChars5, rgchwChars6,
			rgcchwRaw);
	}
	else
	{
		for (ichw = 0 ; ichw <(m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			rgcchwRaw[ichw] = 1;
			rgchwChars2[ichw] = 0;
			rgchwChars3[ichw] = 0;
			rgchwChars4[ichw] = 0;
			rgchwChars5[ichw] = 0;
			rgchwChars6[ichw] = 0;
		}
	}

	//	Text
	strmOut << "Text:          ";	// 15 spaces
	int inUtf32 = 0;
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		utf16 chw, chwNext;
		switch (rgcchwRaw[ichw])
		{
		default:
		case 1:	chw = utf16(rgnChars[inUtf32]); chwNext = rgchwChars2[inUtf32];	break;
		case 2:	chw = rgchwChars2[inUtf32]; chwNext = rgchwChars3[inUtf32];	break;
		case 3:	chw = rgchwChars3[inUtf32]; chwNext = rgchwChars4[inUtf32];	break;
		case 4:	chw = rgchwChars4[inUtf32]; chwNext = rgchwChars5[inUtf32];	break;
		case 5:	chw = rgchwChars5[inUtf32]; chwNext = rgchwChars6[inUtf32];	break;
		case 6:	chw = rgchwChars6[inUtf32]; chwNext = 0;					break;
		}
		if (rgcchwRaw[ichw] == 1 && chwNext == 0 && chw < 0x0100) // ANSI
			strmOut << (char)chw << "      ";	// 6 spaces
		else
			strmOut << ".      "; // 7 spaces
		if (chwNext == 0)
			inUtf32++;
	}
	strmOut << "\n";

	//	Unicode
	strmOut << "Unicode:       ";
	inUtf32 = 0;
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		utf16 chw, chwNext;
		switch (rgcchwRaw[ichw])
		{
		default:
		case 1:	chw = utf16(rgnChars[inUtf32]); chwNext = rgchwChars2[inUtf32];	break;
		case 2:	chw = rgchwChars2[inUtf32]; chwNext = rgchwChars3[inUtf32];	break;
		case 3:	chw = rgchwChars3[inUtf32]; chwNext = rgchwChars4[inUtf32];	break;
		case 4:	chw = rgchwChars4[inUtf32]; chwNext = rgchwChars5[inUtf32];	break;
		case 5:	chw = rgchwChars5[inUtf32]; chwNext = rgchwChars6[inUtf32];	break;
		case 6:	chw = rgchwChars6[inUtf32]; chwNext = 0;					break;
		}
		ptman->LogHexInTable(strmOut, chw, chwNext != 0);
		if (chwNext == 0)
			inUtf32++;
	}
	strmOut << "\n";

	strmOut << "before         ";
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		if (rgcchwRaw[ichw] > 1)
			// continuation of Unicode codepoint
			strmOut << ".      ";
		else if (m_prgisloutBefore[ichw] == kNegInfinity)
			strmOut << "<--    ";
		else if (m_prgisloutBefore[ichw] == kPosInfinity)
			strmOut << "-->    ";
		else
			ptman->LogInTable(strmOut, m_prgisloutBefore[ichw]);
	}
	strmOut <<"\n";

	for (int ix = 1; ix < signed(cassocs) - 1; ix++) //(cassocs > 2)
	{
		if (ix == 1)
			strmOut << "other          ";
		else
			strmOut << "               ";
		for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			std::vector<int> * pvislout = m_prgpvisloutAssocs[ichw];
			if (pvislout == NULL)
				strmOut << ".      ";
			else if (signed(pvislout->size()) <= ix)
				strmOut << ".      ";
			else if ((*pvislout)[ix] != m_prgisloutAfter[ichw])
				ptman->LogInTable(strmOut, (*pvislout)[ix]);
			else
				strmOut << ".      ";
		}
		strmOut << "\n";
	}

	strmOut << "after          ";
	for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
	{
		if (rgcchwRaw[ichw] > 1)
			// continuation of Unicode codepoint
			strmOut << ".      ";
		else if (m_prgisloutAfter[ichw] == kNegInfinity)
			strmOut << "<--    ";
		else if (m_prgisloutAfter[ichw] == kPosInfinity)
			strmOut << "-->    ";
		else
			ptman->LogInTable(strmOut, m_prgisloutAfter[ichw]);
	}
	strmOut <<"\n";

	if (fLigs)
	{
		strmOut << "ligature       ";
		for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			if (rgcchwRaw[ichw] > 1)
				// continuation of Unicode codepoint
				strmOut << ".      ";
			else if (m_prgisloutLigature[ichw] != kNegInfinity)
				ptman->LogInTable(strmOut, m_prgisloutLigature[ichw]);
			else
				strmOut << ".      ";
		}
		strmOut << "\n";

		strmOut << "component      ";
		for (ichw = 0; ichw < (m_ichwAssocsLim - m_ichwAssocsMin); ichw++)
		{
			if (rgcchwRaw[ichw] > 1)
				// continuation of Unicode codepoint
				strmOut << ".      ";
			else if (m_prgisloutLigature[ichw] != kNegInfinity)
				ptman->LogInTable(strmOut, m_prgiComponent[ichw] + 1);	// 1-based
			else
				strmOut << ".      ";
		}
		strmOut << "\n";
	}

	strmOut << "\n";
#endif
}

/*----------------------------------------------------------------------------------------------
	Write out the final surface-to-underlying associations.
----------------------------------------------------------------------------------------------*/
#ifdef TRACING
void Segment::LogSurfaceToUnderlying(GrTableManager * ptman, std::ostream & strmOut)
#else
void Segment::LogSurfaceToUnderlying(GrTableManager *, std::ostream &)
#endif
{
#ifdef TRACING
	strmOut << "\nSURFACE TO UNDERLYING MAPPINGS\n\n";

	ptman->LogSlotHeader(strmOut, m_cslout, SP_PER_SLOT, LEADING_SP);

	int ccomp = 0;

	strmOut << "Glyph IDs      ";
	strmOut << "      - hex    ";
	int islout;
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << "#      ";
		}
		else
		{
			ptman->LogDecimalInTable(strmOut, psloutTmp->GlyphID());
			ccomp = max(ccomp, psloutTmp->NumberOfComponents());
		}
	}
	strmOut << "\n";

	strmOut << "        hex   ";
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << ".      ";
		}
		else
		{
			ptman->LogHexInTable(strmOut, psloutTmp->GlyphID());
			ccomp = max(ccomp, psloutTmp->NumberOfComponents());
		}
	}
	strmOut << "\n";

	bool fAnyPseudos = false;
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->GlyphID() != psloutTmp->ActualGlyphForOutput(ptman))
		{
			fAnyPseudos = true;
			break;
		}
	}
	if (fAnyPseudos)
	{
		strmOut << "Actual glyphs: ";
		for (islout = 0; islout < m_cslout; islout++)
		{
			GrSlotOutput * psloutTmp = m_prgslout + islout;
			if (psloutTmp->GlyphID() != psloutTmp->ActualGlyphForOutput(ptman))
				ptman->LogHexInTable(strmOut, psloutTmp->ActualGlyphForOutput(ptman));
			else
				strmOut << ".      ";
		}
		strmOut << "\n";
	}

	strmOut << "before         ";
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << ".      ";
		}
		else
			ptman->LogInTable(strmOut, psloutTmp->BeforeAssoc());
	}
	strmOut << "\n";

	strmOut << "after          ";
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutTmp = m_prgslout + islout;
		if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
			psloutTmp->SpecialSlotFlag() == kspslLbFinal)
		{
			strmOut << ".      ";
		}
		else
			ptman->LogInTable(strmOut, psloutTmp->AfterAssoc());
	}
	strmOut << "\n";

	for (int icomp = 0; icomp < ccomp; icomp++)
	{
		strmOut << "component " << icomp + 1	// 1=based
			<< "    ";
		for (islout = 0; islout < m_cslout; islout++)
		{
			GrSlotOutput * psloutTmp = m_prgslout + islout;
			if (psloutTmp->SpecialSlotFlag() == kspslLbInitial ||
				psloutTmp->SpecialSlotFlag() == kspslLbFinal)
			{
				strmOut << ".      ";
			}
			else if (icomp < psloutTmp->NumberOfComponents())
			{
				if (psloutTmp->FirstUnderlyingComponent(icomp)
						!= psloutTmp->LastUnderlyingComponent(icomp)) // $$$$$
				{
					//	Multiple characters represented by this component.
					char rgch[10];
					std::string strTmp;
					sprintf(rgch, "%d", psloutTmp->FirstUnderlyingComponent(icomp));
					strTmp += rgch;
					std::memset(rgch, 0, 10);
					sprintf(rgch, "%d", psloutTmp->LastUnderlyingComponent(icomp));
					strTmp += "/";
					strTmp += rgch;
					if (std::strlen(strTmp.c_str()) > SP_PER_SLOT-1)
						strmOut << "****** ";
					else
					{
						while (std::strlen(strTmp.c_str()) < SP_PER_SLOT)  // pad with spaces
							strTmp += " ";
						strmOut << strTmp;
					}
				}
				else
					ptman->LogInTable(strmOut, psloutTmp->FirstUnderlyingComponent(icomp)); // $$$$$
			}
			else
				strmOut << ".      ";
		}
		strmOut << "\n";
	}
#endif
}

#ifdef TRACING
/*----------------------------------------------------------------------------------------------
	Write out the header lines for the underlying data.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogUnderlyingHeader(std::ostream & strmOut, int iMin,
	int iLim, int cBackup, int * prgichr)
{
	strmOut << "string         ";
	int i, iLabel;
	int * pichr = prgichr;
	for (i = iMin - cBackup, iLabel = i; i < iLim; i++, iLabel++, pichr++)
	{
		while (prgichr && *pichr > 1)
		{
			// continuation of upper-plane char
			iLabel++;
			pichr++;
		}
		LogInTable(strmOut, iLabel);
	}
	strmOut << "\n";

	strmOut << "segment        ";
	pichr = prgichr;
	for (i = 0 - cBackup, iLabel = i; i < (iLim - iMin); i++, iLabel++, pichr++)
	{
		while (prgichr && *pichr > 1)
		{
			// continuation of upper-plane char
			iLabel++;
			pichr++;
		}
		LogInTable(strmOut, iLabel);
	}
	strmOut << "\n\n";
}

/*----------------------------------------------------------------------------------------------
	Write out the header lines for the slot contents.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogSlotHeader(std::ostream & strmOut, int islotLim,
	int /*cspPerSlot*/, int cspLeading, int islotMin)
{
	islotLim = min(islotLim, MAX_SLOTS);

	int isp, islot;

	for (isp = 0; isp < cspLeading; isp++)
		strmOut << " ";

	for (islot = islotMin; islot < islotLim; islot++)
		LogInTable(strmOut, islot);
	strmOut << "\n\n";

/****
	if (cslot >= 100)
	{
		for (isp = 0; isp < cspLeading; isp++)
			strmOut << " ";

		for (islot = 0; islot < 100; islot++)
		{
			for (isp = 0; isp < cspPerSlot; isp++)
				strmOut << " ";
		}

		for (islot = 100; islot < cslot; islot++)
		{
			strmOut << "1";
			for (isp = 1; isp < cspPerSlot; isp++)
				strmOut << " ";
		}
	}

	if (cslot > 10)
	{
		for (isp = 0; isp < cspLeading; isp++)
			strmOut << " ";

		for (islot = 0; islot < 10; islot++)
		{
			for (isp = 0; isp < cspPerSlot; isp++)
				strmOut << " ";
		}

		for (islot = 10; islot < cslot; islot++)
		{
			if (true) // (islot % 10 == 0)
			{
				if (islot >= 100)
					strmOut << ((islot - 100) / 10);
				else
					strmOut << (islot / 10);
			}
			else
				strmOut << " ";

			for (isp = 1; isp < cspPerSlot; isp++)
				strmOut << " ";
		}
		strmOut << "\n";
	}

	for (isp = 0; isp < cspLeading; isp++)
		strmOut << " ";

	for (islot = 0; islot < cslot; islot++)
	{
		strmOut << (islot % 10);
		for (isp = 1; isp < cspPerSlot; isp++)
			strmOut << " ";

	}
	strmOut << "\n";
****/
}

/*----------------------------------------------------------------------------------------------
	Write out the glyph numbers for the given stream.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogSlotGlyphs(std::ostream & strmOut, GrSlotStream * psstrm)
{
	strmOut << "Glyph IDs      ";
	int islot;
	for (islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslotTmp = psstrm->SlotAt(islot);
		if (pslotTmp->IsLineBreak(LBGlyphID()))
			strmOut << "#      ";
		else
			LogDecimalInTable(strmOut, pslotTmp->GlyphID());
	}
	strmOut << "\n";

	strmOut << "      - hex    ";
	for (islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslotTmp = psstrm->SlotAt(islot);
		if (pslotTmp->IsLineBreak(LBGlyphID()))
			strmOut << ".      ";
		else
			LogHexInTable(strmOut, pslotTmp->GlyphID());
	}
	strmOut << "\n";
}

/*----------------------------------------------------------------------------------------------
	Return flags indicating which attributes were modified for some slot during the pass
	and therefore need a line in the log file.

	The 'pfMods' array has one flag per slot attribute; the final group are the user-
	definable attributes. 'pccomp' returns the maximum number of ligature components
	in this pass. Associations are assumed to have changed if anything else for the slot did.
----------------------------------------------------------------------------------------------*/
void GrTableManager::SlotAttrsModified(int ipass, bool * rgfMods, bool fPreJust,
	int * pccomp, int * pcassoc)
{
	//	Zero the flags
	std::fill_n(rgfMods, kslatMax + NumUserDefn() - 1, false);
	*pccomp = 0;
	*pcassoc = 0;

	//GrSlotStream * psstrmIn = InputStream(ipass);
	GrSlotStream * psstrmOut = OutputStream(ipass);

	for (int islot = 0; islot < psstrmOut->WritePos(); islot++)
	{
		GrSlotState * pslot = psstrmOut->SlotAt(islot);
		Assert(pslot->PassModified() <= ipass);
		if (pslot->PassModified() < ipass && !fPreJust)
			continue; // not modified during this pass

		pslot->SlotAttrsModified(rgfMods, fPreJust, pccomp, pcassoc);
	}
}

void GrSlotState::SlotAttrsModified(bool * rgfMods, bool fPreJust, int * pccomp, int * pcassoc)
{
	//	To handle reprocessing, in which case there may be a chain of slots modified
	//	in the same pass:
	GrSlotState * pslotPrev = m_pslotPrevState;
	while (pslotPrev && pslotPrev->PassModified() == PassModified())
		pslotPrev = pslotPrev->m_pslotPrevState;

	if (!pslotPrev)
	{
		//	Inserted slot?
		if (m_fAdvXSet)
			rgfMods[kslatAdvX] = true;
		if (m_fAdvYSet)
			rgfMods[kslatAdvY] = true;

		if (m_srAttachTo != 0)
			rgfMods[kslatAttTo] = true;

		if (m_mAttachAtX != kNotYetSet || m_mAttachAtY != 0)
		{	// always do these in pairs
			rgfMods[kslatAttAtX] = true;
			rgfMods[kslatAttAtY] = true;
		}
		if (m_nAttachAtGpoint != kNotYetSet)
			rgfMods[kslatAttAtGpt] = true;
		if (m_mAttachAtXOffset != 0 || m_mAttachAtYOffset != 0)
		{	// always do these in pairs
			rgfMods[kslatAttAtXoff] = true;
			rgfMods[kslatAttAtYoff] = true;
		}

		if (m_mAttachWithX != kNotYetSet || m_mAttachWithY != 0)
		{	// always do these in pairs
			rgfMods[kslatAttWithX] = true;
			rgfMods[kslatAttWithY] = true;
		}
		if (m_nAttachWithGpoint != kNotYetSet)
			rgfMods[kslatAttWithGpt] = true;
		if (m_mAttachWithXOffset != 0 || m_mAttachWithYOffset != 0)
		{	// always do these in pairs
			rgfMods[kslatAttWithXoff] = true;
			rgfMods[kslatAttWithYoff] = true;
		}

		if (m_nAttachLevel != 0)
			rgfMods[kslatAttLevel] = true;

		if (m_lb != kNotYetSet8)
			rgfMods[kslatBreak] = true;
		if (m_dirc != kNotYetSet8)
			rgfMods[kslatDir] = true;
		if (m_fInsertBefore != true)
			rgfMods[kslatInsert] = true;

		if (m_mMeasureSol != kNotYetSet && m_mMeasureSol != 0)
			rgfMods[kslatMeasureSol] = true;
		if (m_mMeasureEol != kNotYetSet && m_mMeasureEol != 0)
			rgfMods[kslatMeasureEol] = true;

		if (m_mJStretch0 != kNotYetSet && m_mJStretch0 != 0)
			rgfMods[kslatJStretch] = true;
		if (m_mJShrink0 != kNotYetSet && m_mJShrink0 != 0)
			rgfMods[kslatJShrink] = true;
		if (m_mJStep0 != kNotYetSet && m_mJStep0 != 0)
			rgfMods[kslatJStep] = true;
		if (m_nJWeight0 != 0 && m_nJWeight0 != 1)
			rgfMods[kslatJWeight] = true;
		if (m_mJWidth0 != kNotYetSet && m_mJWidth0 != 0)
			rgfMods[kslatJWidth] = true;

		if (m_mShiftX != 0)
			rgfMods[kslatShiftX] = true;
		if (m_mShiftY != 0)
			rgfMods[kslatShiftY] = true;

		int i;
		for (i = 0; i < m_cnCompPerLig; i++)
		{
			if (CompRef(i) != NULL)
				rgfMods[kslatCompRef] = true;
			if (CompRef(i))
				*pccomp = max(*pccomp, i + 1);	// max number of ligatures in this pass
		}

		for (i = 0; i < m_cnUserDefn; i++)
		{
			if (UserDefn(i) != 0)
				rgfMods[kslatUserDefn + i] = true;
		}
	}
	else
	{
		if (m_fAdvXSet && m_mAdvanceX != pslotPrev->m_mAdvanceX)
			rgfMods[kslatAdvX] = true;
		if (m_fAdvYSet && m_mAdvanceY != pslotPrev->m_mAdvanceY)
			rgfMods[kslatAdvY] = true;

		if (m_srAttachTo != pslotPrev->m_srAttachTo)
			rgfMods[kslatAttTo] = true;

		if (m_mAttachAtX != pslotPrev->m_mAttachAtX || m_mAttachAtY != pslotPrev->m_mAttachAtY)
		{
			rgfMods[kslatAttAtX] = true;
			rgfMods[kslatAttAtY] = true;
		}
		if (m_nAttachAtGpoint != pslotPrev->m_nAttachAtGpoint)
			rgfMods[kslatAttAtGpt] = true;
		if (m_mAttachAtXOffset != pslotPrev->m_mAttachAtXOffset ||
			m_mAttachAtYOffset != pslotPrev->m_mAttachAtYOffset)
		{
			rgfMods[kslatAttAtXoff] = true;
			rgfMods[kslatAttAtYoff] = true;
		}

		if (m_mAttachWithX != pslotPrev->m_mAttachWithX ||
			m_mAttachWithY != pslotPrev->m_mAttachWithY)
		{
			rgfMods[kslatAttWithX] = true;
			rgfMods[kslatAttWithY] = true;
		}
		if (m_nAttachWithGpoint != pslotPrev->m_nAttachWithGpoint)
			rgfMods[kslatAttWithGpt] = true;
		if (m_mAttachWithXOffset != pslotPrev->m_mAttachWithXOffset ||
			m_mAttachWithYOffset != pslotPrev->m_mAttachWithYOffset)
		{
			rgfMods[kslatAttWithXoff] = true;
			rgfMods[kslatAttWithYoff] = true;
		}

		if (m_nAttachLevel != pslotPrev->m_nAttachLevel)
			rgfMods[kslatAttLevel] = true;

		if (m_lb != pslotPrev->m_lb)
			rgfMods[kslatBreak] = true;
		if (m_dirc != pslotPrev->m_dirc)
			rgfMods[kslatDir] = true;
		if (m_fInsertBefore != pslotPrev->m_fInsertBefore)
			rgfMods[kslatInsert] = true;

		if (m_mMeasureSol != pslotPrev->m_mMeasureSol)
			rgfMods[kslatMeasureSol] = true;
		if (m_mMeasureEol != pslotPrev->m_mMeasureEol)
			rgfMods[kslatMeasureEol] = true;

		if (m_mJStretch0 != pslotPrev->m_mJStretch0 || (fPreJust && m_mJStretch0 != 0))
			rgfMods[kslatJStretch] = true;
		if (m_mJShrink0 != pslotPrev->m_mJShrink0 || (fPreJust && m_mJShrink0 != 0))
			rgfMods[kslatJShrink] = true;
		if (m_mJStep0 != pslotPrev->m_mJStep0 || (fPreJust && m_mJStep0 != 0))
			rgfMods[kslatJStep] = true;
		if (m_nJWeight0 != pslotPrev->m_nJWeight0 || (fPreJust && m_nJWeight0 != 0))
			rgfMods[kslatJWeight] = true;
		if (m_mJWidth0 != pslotPrev->m_mJWidth0)
			rgfMods[kslatJWidth] = true;

		if (m_mShiftX != pslotPrev->m_mShiftX)
			rgfMods[kslatShiftX] = true;
		if (m_mShiftY != pslotPrev->m_mShiftY)
			rgfMods[kslatShiftY] = true;

		int i;
		for (i = 0; i < m_cnCompPerLig; i++)
		{
			if (CompRef(i) != pslotPrev->CompRef(i))
				rgfMods[kslatCompRef] = true;
			if (CompRef(i))
				*pccomp = max(*pccomp, i + 1);	// max number of ligatures in this pass
		}

		for (i = 0; i < m_cnUserDefn; i++)
		{
			if (UserDefn(i) != pslotPrev->UserDefn(i))
				rgfMods[kslatUserDefn + i] = true;
		}
	}

	*pcassoc = max(*pcassoc, AssocsSize());
}
	
/*----------------------------------------------------------------------------------------------
	Log the value of the slot attribute for the given slot, if it changed.
----------------------------------------------------------------------------------------------*/
void GrSlotState::LogSlotAttribute(GrTableManager * ptman,
	std::ostream & strmOut, int ipass, int slat, int iIndex,
	bool fPreJust, bool fPostJust)
{
	if (m_ipassModified != ipass && !fPreJust && !fPostJust)
	{
		strmOut << ".      ";
		return;
	}

	//	To handle reprocessing, in which case there may be a chain of slots modified
	//	in the same pass:
	GrSlotState * pslotPrev = m_pslotPrevState;
	while (pslotPrev && pslotPrev->PassModified() == PassModified())
		pslotPrev = pslotPrev->m_pslotPrevState;

	//	General-purpose variables for em-unit attributes:
	int mThis = 0;
	int mPrev = 0;
	switch (slat)
	{
	case kslatShiftX:
		mThis = m_mShiftX;
		mPrev = (pslotPrev) ? pslotPrev->m_mShiftX : 0;
		break;
	case kslatShiftY:
		mThis = m_mShiftY;
		mPrev = (pslotPrev) ? pslotPrev->m_mShiftY : 0;
		break;
	case kslatMeasureSol:
		mThis = m_mMeasureSol;
		mPrev = (pslotPrev) ? pslotPrev->m_mMeasureSol : 0;
		break;
	case kslatMeasureEol:
		mThis = m_mMeasureEol;
		mPrev = (pslotPrev) ? pslotPrev->m_mMeasureEol : 0;
		break;
	case kslatJStretch:
		mThis = m_mJStretch0;
		mPrev = (pslotPrev) ? pslotPrev->m_mJStretch0 : 0;
		mPrev = (fPreJust && mThis > 0) ? -1 : mPrev; // log it even if it didn't change
		break;
	case kslatJShrink:
		mThis = m_mJShrink0;
		mPrev = (pslotPrev) ? pslotPrev->m_mJShrink0 : 0;
		mPrev = (fPreJust && mThis > 0) ? -1 : mPrev; // log it even if it didn't change
		break;
	case kslatJStep:
		mThis = m_mJStep0;
		mPrev = (pslotPrev) ? pslotPrev->m_mJStep0 : 0;
		mPrev = (fPreJust && mThis > 1) ? -1 : mPrev; // log it even if it didn't change
		break;
	case kslatJWeight:
		mThis = m_nJWeight0;
		mPrev = (pslotPrev) ? pslotPrev->m_nJWeight0 : 0;
		mPrev = (fPreJust && mThis > 1) ? -1 : mPrev; // log it even if it didn't change
		break;
	case kslatJWidth:
		mThis = m_mJWidth0;
		mPrev = (pslotPrev) ? pslotPrev->m_mJWidth0 : 0;
		mPrev = (fPostJust && mThis > 0) ? -1 : mPrev; // log it even if it didn't change
		break;
	default:
		// don't use these variables
		break;
	}

	switch (slat)
	{
	case kslatAdvX:
		if (m_fAdvXSet && (!pslotPrev || m_mAdvanceX != pslotPrev->m_mAdvanceX))
		{
			ptman->LogInTable(strmOut, m_mAdvanceX);
			return;
		}
		break;
	case kslatAdvY:
		if (m_fAdvYSet && (!pslotPrev || m_mAdvanceY != pslotPrev->m_mAdvanceY))
		{
			ptman->LogInTable(strmOut, m_mAdvanceY);
			return;
		}
		break;

	case kslatAttTo:
		if (m_srAttachTo != (pslotPrev ? pslotPrev->m_srAttachTo : 0))
		{
			ptman->LogInTable(strmOut, m_srAttachTo);
			return;
		}
		break;

	case kslatAttAtX:	// always do these in pairs
	case kslatAttAtY:
		if (m_mAttachAtX != (pslotPrev ? pslotPrev->m_mAttachAtX : static_cast<short>(kNotYetSet)) ||
			m_mAttachAtY != (pslotPrev ? pslotPrev->m_mAttachAtY : 0))
		{
			ptman->LogInTable(strmOut,
				((slat == kslatAttAtX) ? m_mAttachAtX : m_mAttachAtY));
			return;
		}
		break;
	case kslatAttAtGpt:
		if (m_nAttachAtGpoint != (pslotPrev ? pslotPrev->m_nAttachAtGpoint : static_cast<short>(kNotYetSet)))
		{
			ptman->LogInTable(strmOut,
				((m_nAttachAtGpoint == kGpointZero) ? 0 : m_nAttachAtGpoint));
			return;
		}
		break;
	case kslatAttAtXoff:	// always do these in pairs
	case kslatAttAtYoff:
		if (m_mAttachAtXOffset != (pslotPrev ? pslotPrev->m_mAttachAtXOffset : 0) ||
			m_mAttachAtYOffset != (pslotPrev ? pslotPrev->m_mAttachAtYOffset : 0))
		{
			ptman->LogInTable(strmOut,
				((slat == kslatAttAtXoff) ? m_mAttachAtXOffset : m_mAttachAtYOffset));
			return;
		}
		break;

	case kslatAttWithX:	// always do these in pairs
	case kslatAttWithY:
		if (m_mAttachWithX != (pslotPrev ? pslotPrev->m_mAttachWithX : static_cast<short>(kNotYetSet)) ||
			m_mAttachWithY != (pslotPrev ? pslotPrev->m_mAttachWithY : 0))
		{
			ptman->LogInTable(strmOut,
				((slat == kslatAttWithX) ? m_mAttachWithX : m_mAttachWithY));
			return;
		}
		break;
	case kslatAttWithGpt:
		if (m_nAttachWithGpoint != (pslotPrev ? pslotPrev->m_nAttachWithGpoint : static_cast<short>(kNotYetSet)))
		{
			ptman->LogInTable(strmOut,
				((m_nAttachWithGpoint == kGpointZero) ? 0 : m_nAttachWithGpoint));
			return;
		}
		break;
	case kslatAttWithXoff:	// always do these in pairs
	case kslatAttWithYoff:
		if (m_mAttachWithXOffset != (pslotPrev ? pslotPrev->m_mAttachWithXOffset : 0) ||
			m_mAttachWithYOffset != (pslotPrev ? pslotPrev->m_mAttachWithYOffset : 0))
		{
			ptman->LogInTable(strmOut,
				((slat == kslatAttWithXoff) ? m_mAttachWithXOffset : m_mAttachWithYOffset));
			return;
		}
		break;

	case kslatAttLevel:
		if (m_nAttachLevel != (pslotPrev ? pslotPrev->m_nAttachLevel : 0))
		{
			ptman->LogInTable(strmOut, m_nAttachLevel);
			return;
		}
		break;

	case kslatBreak:
		if (m_lb != (pslotPrev ? pslotPrev->m_lb : static_cast<sdata8>(kNotYetSet8)))
		{
			ptman->LogBreakWeightInTable(strmOut, m_lb);
			return;
		}
		break;
	case kslatDir:
		if (m_dirc != (pslotPrev ? pslotPrev->m_dirc : static_cast<sdata8>(kNotYetSet8)))
		{
			ptman->LogDirCodeInTable(strmOut, m_dirc);
			return;
		}
		break;
	case kslatInsert:
		if (m_fInsertBefore != (pslotPrev ? pslotPrev->m_fInsertBefore : true))
		{
			if (m_fInsertBefore)
				strmOut << "true   ";
			else
				strmOut << "false  ";
			return;
		}
		break;

	case kslatJWeight:
		if (m_mJStretch0 == 0 && m_mJShrink0 == 0)
			mPrev = mThis;	// don't log; weight is irrelevant
		// fall through
	case kslatShiftX:
	case kslatShiftY:
	case kslatMeasureSol:
	case kslatMeasureEol:
	case kslatJStretch:
	case kslatJShrink:
	case kslatJStep:
	case kslatJWidth:
		if (mThis != mPrev)
		{
			ptman->LogInTable(strmOut, mThis);
			return;
		}
		break;

	case kslatCompRef:
		if (CompRef(iIndex) != (pslotPrev ? pslotPrev->CompRef(iIndex) : 0))
		{
			GrSlotState * pslotComp = reinterpret_cast<GrSlotState *>(CompRef(iIndex));
			ptman->LogInTable(strmOut, pslotComp->m_islotTmpIn);
			return;
		}
		break;

	default:
		if (kslatUserDefn <= slat && slat <= kslatUserDefn + m_cnUserDefn)
		{
			int iTmp = slat - kslatUserDefn;
			if (UserDefn(iTmp) != (pslotPrev ? pslotPrev->UserDefn(iTmp) : 0))
			{
				ptman->LogInTable(strmOut, UserDefn(iTmp));
				return;
			}
		}
		else
			gAssert(false);
	}

	strmOut << ".      ";
}
	
/*----------------------------------------------------------------------------------------------
	Log the value of the association, if the slot changed.
----------------------------------------------------------------------------------------------*/
void GrSlotState::LogAssociation(GrTableManager * ptman,
	std::ostream & strmOut, int ipass, int iassoc, bool fBoth, bool fAfter)
{
	if (m_ipassModified != ipass)
	{
		strmOut << ".      ";
		return;
	}

	if (fBoth)
	{
		GrSlotState * pslotBefore = (m_vpslotAssoc.size()) ? m_vpslotAssoc[0] : NULL;
		GrSlotState * pslotAfter = (m_vpslotAssoc.size()) ? m_vpslotAssoc.back() : NULL;
		// handle possible reprocessing
		while (pslotBefore && pslotBefore->PassModified() == m_ipassModified)
			pslotBefore = pslotBefore->m_pslotPrevState;
		while (pslotAfter && pslotAfter->PassModified() == m_ipassModified)
			pslotAfter = pslotAfter->m_pslotPrevState;

		int nBefore, nAfter;


		int csp = 4;
		if (pslotBefore)
		{
			nBefore = pslotBefore->m_islotTmpIn;
			strmOut << nBefore;
			if (nBefore > 99) csp--;
			if (nBefore > 9) csp--;
		}
		else
		{
			strmOut << "??";
			csp--;
		}
		if (pslotAfter)
		{
			nAfter = pslotAfter->m_islotTmpIn;
			strmOut << "/" << nAfter;
			if (nAfter > 99) csp--;
			if (nAfter > 9) csp--;
		}
		else
		{
			if (pslotBefore)
			{
				strmOut << "/" << "??";
				csp--;
			}
			else
				csp = 5;
		}

		for (int isp = 0; isp < csp; isp++)
			strmOut << " ";
	}

	else if (fAfter)
	{
		Assert(m_vpslotAssoc.size());
		GrSlotState * pslotAfter = m_vpslotAssoc.back();
		// handle possible reprocessing
		while (pslotAfter && pslotAfter->PassModified() == m_ipassModified)
			pslotAfter = pslotAfter->m_pslotPrevState;

		if (pslotAfter)
		{
			int nAfter = pslotAfter->m_islotTmpIn;
			ptman->LogInTable(strmOut, nAfter);
		}
		else
			strmOut << "??     ";
	}
	else if (iassoc < signed(m_vpslotAssoc.size()))
	{
		GrSlotState * pslot = m_vpslotAssoc[iassoc];
		// handle possible reprocessing
		while (pslot && pslot->PassModified() == m_ipassModified)
			pslot = pslot->m_pslotPrevState;

		if (pslot)
		{
			int n = pslot->m_islotTmpIn;
			ptman->LogInTable(strmOut, n);
		}
		else
			strmOut << "??     ";
	}
	else
		strmOut << ".      ";
}

/*----------------------------------------------------------------------------------------------
	Write a hex value (a glyphID or Unicode codepoint) into the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogInTable(std::ostream & strmOut, int n)
{
	if (n == kNegInfinity)
	{
		strmOut << "-inf   ";
		return;
	}
	if (n == kPosInfinity)
	{
		strmOut << "+inf   ";
		return;
	}
	if (n > 999999)
	{
		strmOut << "****** ";
		return;
	}
	if (n < -99999)
	{
		strmOut << "-***** ";
		return;
	}

	strmOut << n;
	int csp = SP_PER_SLOT - 1;

	if (abs(n) > 99999) csp--;
	if (abs(n) > 9999) csp--;
	if (abs(n) > 999) csp--;
	if (abs(n) > 99) csp--;
	if (abs(n) > 9) csp--;

	if (n < 0) csp--;

	for (int isp = 0; isp < csp; isp++)
		strmOut << " ";
}

void GrTableManager::LogInTable(std::ostream & strmOut, float n)
{
	if (n == kNegInfFloat)
	{
		strmOut << "-inf   ";
		return;
	}
	if (n == kPosInfFloat)
	{
		strmOut << "+inf   ";
		return;
	}
	if (n > 9999)
	{
		strmOut << "****.* ";
		return;
	}
	if (n < -999)
	{
		strmOut << "-***.* ";
		return;
	}

	int csp = SP_PER_SLOT - 4;

	int nInt = (int)(fabsf(n));
	int nTenths = (int)fabsf((fabsf(n) - float(nInt) + 0.05f) * 10);
	if (nTenths >= 10)
	{
		nTenths = 0;
		nInt++;
	}

	if (nInt >= 1000) csp--;
	if (nInt >= 100) csp--;
	if (nInt >= 10) csp--;

	if (n < 0) csp--;

	for (int isp = 0; isp < csp; isp++)
		strmOut << " ";
	if (n < 0)
		strmOut << "-";
	strmOut << nInt << "." << nTenths << " ";
}

/*----------------------------------------------------------------------------------------------
	Write a hex value (a glyphID or Unicode codepoint) into the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogHexInTable(std::ostream & strmOut, utf16 chw, bool fPlus)
{
	//char rgch[20];
	if (chw <= 0x0fff) strmOut << "0";
	if (chw <= 0x00ff) strmOut << "0";
	if (chw <= 0x000f) strmOut << "0";
    
	strmOut << std::hex << chw << std::dec;

	for (int i = 4; i < SP_PER_SLOT - 2; i++)
		strmOut << " ";

	if (fPlus)
		strmOut << "+ ";
	else
		strmOut << "  ";
}

/*----------------------------------------------------------------------------------------------
	Write a decimal value (a glyphID or Unicode codepoint) into the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogDecimalInTable(std::ostream & strmOut, utf16 chw)
{
	//char rgch[20];
	int nSp = SP_PER_SLOT - 6;
	if (chw < 100000) nSp++;
	if (chw < 10000) nSp++;
	if (chw < 1000) nSp++;
	if (chw < 100) nSp++;
	if (chw < 10) nSp++;
    
	strmOut << std::dec << chw;

	for (int i = 0; i < nSp; i++)
		strmOut << " ";
}

/*----------------------------------------------------------------------------------------------
	Write a directionality code to the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogDirCodeInTable(std::ostream & strmOut, int dirc)
{
	switch (dirc)
	{
	case kdircUnknown:		strmOut << "???    "; break;
	case kdircNeutral:		strmOut << "ON     "; break;
	case kdircL:			strmOut << "L      "; break;
	case kdircR:			strmOut << "R      "; break;
	case kdircRArab:		strmOut << "AR     "; break;
	case kdircEuroNum:		strmOut << "EN     "; break;
	case kdircEuroSep:		strmOut << "ES     "; break;
	case kdircEuroTerm:		strmOut << "ET     "; break;
	case kdircArabNum:		strmOut << "AN     "; break;
	case kdircComSep:		strmOut << "CS     "; break;
	case kdircWhiteSpace:	strmOut << "WS     "; break;
	case kdircBndNeutral:	strmOut << "BN     "; break;
	case kdircNSM:			strmOut << "NSM    "; break;
	case kdircLRO:			strmOut << "LRO    "; break;
	case kdircRLO:			strmOut << "RLO    "; break;
	case kdircLRE:			strmOut << "LRE    "; break;
	case kdircRLE:			strmOut << "RLE    "; break;
	case kdircPDF:			strmOut << "PDF    "; break;
	case kdircPdfL:			strmOut << "PDF-L  "; break;
	case kdircPdfR:			strmOut << "PDF-R  "; break;
	case kdircLlb:			strmOut << "L      "; break;
	case kdircRlb:			strmOut << "R      "; break;
	default:				LogInTable(strmOut, dirc); break;
	}
}

/*----------------------------------------------------------------------------------------------
	Write a breakweight code to the table.
----------------------------------------------------------------------------------------------*/
void GrTableManager::LogBreakWeightInTable(std::ostream & strmOut, int lb)
{
	if (lb < 0)
	{
		lb = lb * -1;
		switch (lb)
		{
		case klbWsBreak:		strmOut << "-ws    "; break;
		case klbWordBreak:		strmOut << "-word  "; break;
		case klbHyphenBreak:	strmOut << "-intra "; break;
		case klbLetterBreak:	strmOut << "-lettr "; break;
		case klbClipBreak:		strmOut << "-clip  "; break;
		default:				LogInTable(strmOut, lb); break;
		}
	}
	else
	{
		switch (lb)
		{
		case klbNoBreak:		strmOut << "none   "; break;
		case klbWsBreak:		strmOut << "ws     "; break;
		case klbWordBreak:		strmOut << "word   "; break;
		case klbHyphenBreak:	strmOut << "intra  "; break;
		case klbLetterBreak:	strmOut << "letter "; break;
		case klbClipBreak:		strmOut << "clip   "; break;
		default:				LogInTable(strmOut, lb); break;
		}
	}
}

#endif // TRACING

} // namespace gr
