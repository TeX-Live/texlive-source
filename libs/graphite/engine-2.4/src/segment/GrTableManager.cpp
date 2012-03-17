/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrTableManager.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    GrTableManager, which handles the interactions between passes, including the demand-pull
	algorithm.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#else
#include <stdlib.h>
#endif
#include <math.h>

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

/*----------------------------------------------------------------------------------------------
	Constructors and initializers
----------------------------------------------------------------------------------------------*/
EngineState::EngineState()
	:	m_ipassJustCalled(-1),
		m_fStartLineContext(false),
		m_fEndLineContext(false),
		////m_pgg(NULL),
		m_cslotPreSeg(0),
		m_islotblkCurr(-1),
		m_islotNext(kSlotBlockSize),
		m_fInitialLB(false),
		m_fFinalLB(false),
		m_fInsertedLB(false),
		m_fExceededSpace(false),
		m_fHitHardBreak(false),
		m_fRemovedWhtsp(false),
		m_dxsShrinkPossible(GrSlotState::kNotYetSet),
		m_islotPosNext(-1),
		m_xsPosXNext(0),
		m_ysPosYNext(0),
		m_dxsTotWidthSoFar(0),
		m_dxsVisWidthSoFar(0),
		m_prgzpst(NULL),
		m_prgpsstrm(NULL)
{
	m_vslotblk.clear();
	m_vprgnSlotVarLenBufs.clear();
}

void EngineState::Initialize(GrEngine * pgreng, GrTableManager * ptman)
{
	m_ptman = ptman;
	m_cpass = ptman->NumberOfPasses(); // duplicate for convenience

	m_cUserDefn = pgreng->NumUserDefn();
	m_cFeat = pgreng->NumFeat();
	m_cCompPerLig = pgreng->NumCompPerLig();

	if (m_prgzpst)
		delete[] m_prgzpst;
	m_prgzpst = new PassState[m_cpass];
	ptman->StorePassStates(m_prgzpst);
}

/*----------------------------------------------------------------------------------------------
	Destructors
----------------------------------------------------------------------------------------------*/
GrTableManager::~GrTableManager()
{
	for (int ipass = 0; ipass < m_cpass; ++ipass)
		delete Pass(ipass);

	delete[] m_prgppass;
}

EngineState::~EngineState()
{
	DestroySlotBlocks();

	delete[] m_prgzpst;

	if (m_prgpsstrm)
	{
		for (int isstrm = 0; isstrm < m_cpass; ++isstrm)
			delete OutputStream(isstrm);

		delete[] m_prgpsstrm;
	}
}

/*----------------------------------------------------------------------------------------------
	Delete all the slots.
----------------------------------------------------------------------------------------------*/	
void EngineState::DestroySlotBlocks()
{
	Assert(m_vslotblk.size() == m_vprgnSlotVarLenBufs.size());
	for (size_t islotblk = 0; islotblk < m_vslotblk.size(); ++islotblk)
	{
		delete[] m_vslotblk[islotblk];
		delete[] m_vprgnSlotVarLenBufs[islotblk];
	}
	m_vslotblk.clear();
	m_vprgnSlotVarLenBufs.clear();
}

/*----------------------------------------------------------------------------------------------
	Create the passes, and them fill them in by reading from the file stream.
----------------------------------------------------------------------------------------------*/	
bool GrTableManager::CreateAndReadPasses(GrIStream & grstrm,
	int fxdSilfVersion, int fxdRuleVersion,
	int cpassFont, long lSubTableStart, int * rgnPassOffsets, int * rgcbPassLengths,
	int ipassSub1Font, int ipassPos1Font, int ipassJust1Font, byte ipassPostBidiFont)
{
	Assert(ipassSub1Font <= ipassJust1Font);
	Assert(ipassJust1Font <= ipassPos1Font);
	Assert(ipassPos1Font <= cpassFont);
	if (ipassSub1Font > ipassJust1Font || ipassJust1Font > ipassPos1Font
		|| ipassPos1Font > cpassFont)
	{
		return false; // bad table
	}

	//	Adjusted indices based on the fact that we have a glyph generation pass that is
	//	pass 0, and possibly a bidi pass:
	m_cpass = cpassFont + 1;
	int ipassLB1, ipassSub1, ipassBidi, ipassPos1, ipassJust1;
	ipassLB1 = 1;
	ipassSub1 = ipassSub1Font + 1;
	if (ipassPostBidiFont == 0xFF)
	{
		//	No bidi pass.
		m_fBidi = false;
		//	Add 1 below to account for the glyph-generation pass.
		ipassPos1 =  ipassPos1Font + 1;
		ipassJust1 = ipassJust1Font + 1;
		ipassBidi = ipassJust1;
	}
	else
	{
		m_fBidi = true;
		m_cpass++;
		Assert(ipassPostBidiFont == ipassJust1Font);
		ipassJust1 = ipassJust1Font + 2;
		ipassPos1 = ipassPos1Font + 2;
		ipassBidi = ipassJust1 - 1;
	}

	//	Always make at least one positioning pass, even if there are no rules.
	if (ipassPos1 == m_cpass)
	{
		m_cpass++;
	}

	m_prgppass = new GrPass*[m_cpass + 1];
	m_prgppass[0] = new GrGlyphGenPass(0);
	int ipass;
	for (ipass = 1; ipass < m_cpass; ipass++ )
		m_prgppass[ipass] = NULL;	// clear so that if loading a pass fails, we can delete the table-manager

	m_cpassLB = 0;
	m_ipassJust1 = 1;
	m_ipassPos1 = 1;
	bool f = true;
	ipass = 1;
	int ipassFont = 0;
	for ( ; ipass < m_cpass; ipass++, ipassFont++)
	{
		if (ipass < ipassSub1)
		{
			m_prgppass[ipass] = new GrLineBreakPass(ipass);
			f = m_prgppass[ipass]->ReadFromFont(grstrm, fxdSilfVersion, fxdRuleVersion,
				lSubTableStart + rgnPassOffsets[ipassFont], rgcbPassLengths[ipassFont]);
			m_cpassLB++;
			m_ipassJust1++;
			m_ipassPos1++;
		}
		else if (ipassSub1 <= ipass && ipass < ipassBidi)
		{
			m_prgppass[ipass] = new GrSubPass(ipass);
			f = m_prgppass[ipass]->ReadFromFont(grstrm, fxdSilfVersion, fxdRuleVersion,
				lSubTableStart + rgnPassOffsets[ipassFont], rgcbPassLengths[ipassFont]);
			m_ipassJust1++;
			m_ipassPos1++;
		}
		else if (ipassBidi == ipass && ipass < ipassJust1)
		{
			m_prgppass[ipass] = new GrBidiPass(ipass);
			m_prgppass[ipass]->SetTopDirLevel(TopDirectionLevel());
			ipassFont--;	// no corresponding pass in font
			m_ipassJust1++;
			m_ipassPos1++;
			f = true;
		}
		else if (ipassJust1 <= ipass && ipass < ipassPos1)
		{
			//	A justification pass is, in essence, a substitution pass.
			m_prgppass[ipass] = new GrSubPass(ipass);
			f = m_prgppass[ipass]->ReadFromFont(grstrm, fxdSilfVersion, fxdRuleVersion,
				lSubTableStart + rgnPassOffsets[ipassFont], rgcbPassLengths[ipassFont]);
			m_ipassPos1++;
		}
		else if (ipassPos1 <= ipass)
		{
			m_prgppass[ipass] = new GrPosPass(ipass);
			if (ipassFont < cpassFont)
			{
				f = m_prgppass[ipass]->ReadFromFont(grstrm, fxdSilfVersion, fxdRuleVersion,
					lSubTableStart + rgnPassOffsets[ipassFont], rgcbPassLengths[ipassFont]);
			}
			else
			{
				//	No positioning pass in font: create a bogus one.
				m_prgppass[ipass]->InitializeWithNoRules();
				f = true;
			}
		}
		else
		{
			Assert(false);
			f = false;	// bad table
		}

		if (!f)
			return false;	// bad table
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Create passes corresponding to an invalid font.
----------------------------------------------------------------------------------------------*/	
void GrTableManager::CreateEmpty()
{
	m_cpass = 2;

	m_prgppass = new GrPass*[2];
	m_prgppass[0] = new GrGlyphGenPass(0);

	m_prgppass[1] = new GrPosPass(1);
	m_prgppass[1]->InitializeWithNoRules();

	m_ipassPos1 = 1;
	m_cpassLB = 0;
	m_fBidi = false;

	m_engst.CreateEmpty();
}

void EngineState::CreateEmpty()
{
	m_fStartLineContext = false;
	m_fEndLineContext = false;
}

/*----------------------------------------------------------------------------------------------
	Create the slot streams corresponding to the passes.
----------------------------------------------------------------------------------------------*/
void EngineState::CreateSlotStreams()
{
	if (m_prgpsstrm)
		return;

	m_prgpsstrm = new GrSlotStream * [m_cpass];
	for (int ipass = 0; ipass < m_cpass; ++ipass)
	{
		m_prgpsstrm[ipass] = new GrSlotStream(ipass);
	}
}

/*----------------------------------------------------------------------------------------------
	Store information about the pass-states in the pass objects.
----------------------------------------------------------------------------------------------*/
void GrTableManager::StorePassStates(PassState * rgzpst)
{
	for (int ipass = 0; ipass < m_cpass; ipass++)
		Pass(ipass)->SetPassState(rgzpst + ipass);
}

/*----------------------------------------------------------------------------------------------
	Demand-driven loop: with goal of filling up final slot stream, loop backward and forward
	filling up slot streams until final stream has reached physical capacity.

	@param pgjus				- justification agent; NULL if justification is of no interest
	@param jmodi				- (internal) justification mode
	@param fStartLine, fEndLine
	@param ichSegLim			- known end of segment; -1 when using backtracking
	@param dxWidthRequested		- available width; when justifying, exact width desired
	@param dxUnjustifiedWidth	- when justifying, what the width would be normally
	@param fNeedFinalBreak		- true if the end of the segment needs to be a valid break point
	@param fMoreText			- true if char stream doesn't go up to text-stream lim
	@param ichFontLim			- end of the range that could be rendered by this font
	@param fInfiniteWidth		- used for "measured" segments
	@param fWidthIsCharCount	- kludge for test procedures: the width is a character count,
									not a pixel count
	@param lbPref				- try for longest segment ending with this breakweight
									- when justifying, lb to assume
	@param lbMax				- max (last resort) breakweight if no preferred break possible
	@param ichwCallerBtLim		- caller's backtrack lim; -1 if caller is not backtracking
	@param twsh					- how to handle trailing white-space
	@param fParaRtl				- overall paragraph direction
	@param nDirDepth			- direction depth of this segment
----------------------------------------------------------------------------------------------*/
void GrTableManager::Run(Segment * psegNew, Font * pfont,
	GrCharStream * pchstrm,	IGrJustifier * pgjus, int jmodi,
	LayoutEnvironment & layout,
	int ichSegLim,
	float dxWidthRequested, float dxUnjustifiedWidth,
	bool fNeedFinalBreak, bool fMoreText, bool fFeatureVariations, int ichFontLim,
	bool fInfiniteWidth, bool fWidthIsCharCount,
	int ichwCallerBtLim,
	int nDirDepth, SegEnd estJ)
{
	bool fStartLine = layout.startOfLine();
	bool fEndLine = layout.endOfLine();
	LineBrk lbPref = layout.bestBreak();
	LineBrk lbMax = layout.worstBreak();
	TrWsHandling twsh = layout.trailingWs();
	bool fParaRtl = layout.rightToLeft();
	Segment * psegPrev = layout.prevSegment();
	Segment * psegInit = layout.segmentForInit();
	std::ostream * pstrmLog = layout.loggingStream();

	m_engst.Initialize(Engine(), this); // do this after the tables have been fully read
	m_engst.m_jmodi = jmodi;
	m_engst.m_ipassJustCalled = -1;
	m_engst.m_twsh = twsh;
	m_engst.m_fParaRtl = fParaRtl;
	m_engst.m_dxsShrinkPossible = GrSlotState::kNotYetSet;
	m_engst.m_fFeatureVariations = fFeatureVariations;
	m_fLogging = (pstrmLog != NULL);

	int cbPrev;
	byte * pbPrevSegDat = NULL;
	if (psegPrev)
		cbPrev = psegPrev->NextSegInitBuffer(&pbPrevSegDat);
	else if (psegInit)
		cbPrev = psegInit->ThisSegInitBuffer(&pbPrevSegDat);
	else
		cbPrev = 0;

	if (pchstrm->IsEmpty())
	{
		m_engst.m_lbPrevEnd = klbNoBreak;
		InitSegmentAsEmpty(psegNew, pfont, pchstrm, fStartLine, fEndLine);
		psegNew->SetWidths(0, 0);
		psegNew->SetUpOutputArrays(pfont, this, NULL, 0, 0, 0, twsh,
			fParaRtl, nDirDepth, true);
		psegNew->SetLayout(layout);
		return;
	}

	int islotUnderBreak = -1;		// position of inserted or final break point in underlying
									// data; none so far (if an actual LB has been inserted,
									// this is the index of that slot; otherwise it is the
									// index of the last valid slot)
	int islotSurfaceBreak = -1;		// position of line break in surface stream; none so far
	LineBrk lbBestToTry = lbPref;
	LineBrk lbFound = klbNoBreak;

	float dxMaxWidth = dxWidthRequested;

	if (m_engst.m_jmodi == kjmodiJustify)
	{
		Assert(fInfiniteWidth);
		lbBestToTry = klbClipBreak;
		lbFound = lbPref;
	}

	float dxWidth;

	std::vector<int> vnslotNeeded;
	vnslotNeeded.resize(m_cpass);

	m_engst.InitializeStreams(this, pchstrm, cbPrev, pbPrevSegDat, fNeedFinalBreak, &islotUnderBreak);

	int ipassCurr;
	int nNeedInput;	// kNextPass: go on to the next pass
					// kBacktrack: need to insert a line break
					// > 0: need more input from previous pass

	int cchwPostXlbContext = m_pgreng->PostXlbContext();
	if (ichSegLim > -1)
	{
		//	When we know exactly where the end of the segment will be, get as much as
		//	we need from the character stream, and then fill up the remaining streams.
		//	(3 below is an arbitrary number; we just want to get slightly more than
		//	we really need.)
		vnslotNeeded[0] = ichSegLim - pchstrm->Min() + cchwPostXlbContext + 3;
		for (int ipass = 1; ipass < m_cpass; ipass++)
			vnslotNeeded[ipass] = 10000;
		//	Start at pass 0.
		ipassCurr = 0;
	}
	else
	{
		//	When we're doing line-breaking to find the end of the segment, use the
		//	demand-driven approach that tries to fill up the available space.
		vnslotNeeded[m_cpass - 1] = 1;

		//	Start at the last pass. (It might be more efficient to start at the first pass and
		//	generate a small arbitrary number of glyphs.)
		ipassCurr = m_cpass - 1;
	}

	while (ipassCurr < m_cpass)
	{
		if (ipassCurr == 0)
		{
			int nNeedNow = vnslotNeeded[0];
			//	Zeroth pass: generate glyph IDs from the underlying input.
			nNeedInput = Pass(0)->ExtendGlyphIDOutput(this,
				pchstrm, OutputStream(0), ichSegLim, cchwPostXlbContext, lbPref,
				nNeedNow, fNeedFinalBreak, m_engst.m_twsh, &islotUnderBreak);
			Assert(nNeedInput == kNextPass || nNeedInput == kBacktrack);
			vnslotNeeded[0] -= nNeedNow;
		}
		else if (ipassCurr < m_cpass - 1)
		{
			//	Middle pass.
			int cslotNeedNow = vnslotNeeded[ipassCurr];
			cslotNeedNow = max(1, cslotNeedNow);
			int cslotGot;
			Pass(ipassCurr)->ExtendOutput(this,
					InputStream(ipassCurr), OutputStream(ipassCurr), cslotNeedNow, twsh,
					&nNeedInput, &cslotGot, &islotUnderBreak);
			vnslotNeeded[ipassCurr] -= cslotGot;
			//if (nNeedInput != kNextPass && nNeedInput != kBacktrack)
			//	nNeedInput = max(nNeedInput, cslotNeedNow - cslotGot);
		}
		else // (ipassCurr == m_cpass - 1)
		{
			//	Final pass: position and test allotted space.
			float dxWidthWShrink = dxMaxWidth;
			if (m_engst.m_dxsShrinkPossible != GrSlotState::kNotYetSet)
				dxWidthWShrink += (int)m_engst.m_dxsShrinkPossible;
			nNeedInput = Pass(ipassCurr)->ExtendFinalOutput(this,
					InputStream(ipassCurr), OutputStream(ipassCurr),
					dxWidthWShrink, fWidthIsCharCount, fInfiniteWidth,
					(islotUnderBreak > -1), (fNeedFinalBreak && lbFound == klbNoBreak),
					lbMax, twsh,
					&islotSurfaceBreak, &dxWidth);
		}

		if (m_engst.m_jmodi == kjmodiJustify)
		{
			if (ipassCurr < m_ipassJust1 && !OutputStream(ipassCurr)->FullyWritten())
			{
				// When justifying, fill up all the streams up to the point where we need
				// to interact with the justification routine.
				// I think this code is obsolete.
				int nStillNeed = 1000;
				if ((ipassCurr == 1 && nNeedInput != kNextPass) || ipassCurr == 0)
					nStillNeed = ichSegLim - (pchstrm->Pos() - pchstrm->Min())
						+ cchwPostXlbContext + 3;
				if (nStillNeed > 0)
				{
					if (nNeedInput == kNextPass)
						ipassCurr++; // repeat this pass
					nNeedInput = nStillNeed;
				}
			}
			else if (nNeedInput == kNextPass && ipassCurr + 1 == m_ipassJust1)
			{
				CallJustifier(pgjus, ipassCurr, dxUnjustifiedWidth, dxWidthRequested, fEndLine);
			}
		}
		DetermineShrink(pgjus, ipassCurr);

		if (nNeedInput == kBacktrack)
		{
			//	Final output has overflowed its space--find (or adjust) the break point.

			bool fFoundBreak = Backtrack(&islotUnderBreak,
				&lbBestToTry, lbMax, twsh, fMoreText, ichwCallerBtLim, fEndLine, &lbFound);
			if (!fFoundBreak)
			{
				// Nothing will fit. Initialize the new segment just enough so that
				// we can delete it.
				InitSegmentToDelete(psegNew, pfont, pchstrm);
				return;
			}
		}
		else if (nNeedInput == kNextPass)
		{
			//	This pass is sufficiently full--return to following pass.
			ipassCurr++;
		}
		else
		{
			//	Get more input from previous pass.
			ipassCurr--;
			vnslotNeeded[ipassCurr] = nNeedInput;
		}
	}

	//	At this point we have fully transduced all the text for this segment.

	//	Figure out why we broke the segment and what the caller is supposed to do about it.
	SegEnd est;
	if (m_engst.m_jmodi == kjmodiJustify)
	{
		//	Don't change what was passed in.
		est = estJ;
	}
	else if (m_engst.m_fRemovedWhtsp) // (islotUnderBreak > -1 && twsh == ktwshNoWs)
	{
		Assert(!m_engst.m_fInsertedLB);
		est = kestMoreWhtsp;
	}

	else if (m_engst.m_fHitHardBreak)
	{
		est = kestHardBreak;
	}

	else if (!m_engst.m_fExceededSpace && !fMoreText)
		//	Rendered to the end of the original requested range.
		est = kestNoMore;

//	else if (!m_fExceededSpace && !fNextIsSameWs && fMoreText)
//		//	Writing system break; may be more room on the line. But see tweak below.
//		*pest = kestWsBreak;

	else if (m_engst.m_fExceededSpace)
	{
		//	Exceeded available space, found a legal break.
		if (twsh == ktwshNoWs && m_engst.m_fRemovedWhtsp)
			est = kestMoreWhtsp;
		else
			est = kestMoreLines;
	}

	else if (twsh == ktwshOnlyWs)
	{
		est = kestOkayBreak; // but see tweak below
	}

	else
	{
		//	Broke because of something like a font change. Determine whether this is a legal
		//	break or not.
		//	TODO SharonC: handle the situation where it is legal to break BEFORE the first
		//	character of the following segment.
		Assert(fMoreText);
		//Assert(fNextIsSameWs);
		if (ichwCallerBtLim > -1)
		{
			Assert(m_engst.m_fInsertedLB);
		}
		else if (ichSegLim > -1)
		{
			// We stopped where caller said to.
			Assert(fInfiniteWidth);
		}
		else
		{
			Assert(!m_engst.m_fInsertedLB);
			//Assert(islotUnderBreak == -1 || m_engst.m_fFinalLB); -- no, ExtendGlyphIDOutput clearly sets islotUnderBreak regardless
		}
		int islotTmp = OutputStream(m_cpass - 1)->WritePos();
		GrSlotState * pslotTmp;
		if (m_engst.m_fFinalLB || m_engst.m_fInsertedLB)
			pslotTmp = OutputStream(m_cpass-1)->SlotAt(islotTmp - 2);
		else
			pslotTmp = OutputStream(m_cpass-1)->SlotAt(islotTmp - 1);

		if (abs(pslotTmp->BreakWeight()) <= lbPref)
			est = kestOkayBreak;

		else if (abs(pslotTmp->BreakWeight()) > lbMax)
			est = kestBadBreak;

		else if (OutputStream(m_cpassLB)->HasEarlierBetterBreak(islotUnderBreak, lbFound,
			this->LBGlyphID()))
		{
  			est = kestBadBreak;
  		}
  		else
  			est = kestOkayBreak;
  
		// We no longer have the ability to know whether or not the next segment will use
		// the same writing system.
  		//if (est == kestBadBreak && m_engst.m_fExceededSpace && !fNextIsSameWs && fMoreText)
  		//	est = kestWsBreak;
  	}
  
  	//	Make a segment out of it and return it.
	int dichSegLen;
  	InitNewSegment(psegNew, pfont, pchstrm, pgjus,
  		islotUnderBreak, islotSurfaceBreak, fStartLine, fEndLine, ichFontLim, lbFound, est,
  		&dichSegLen);
	if (psegNew->segmentTermination() == kestNothingFit)
	{
		return;
	}
	else if (psegNew->segmentTermination() == kestHardBreak && 
		psegNew->startCharacter() == psegNew->stopCharacter())
	{
		// Empty segment caused by a hard-break.
		m_engst.m_lbPrevEnd = klbNoBreak;
		psegNew->SetWidths(0, 0);
		psegNew->SetUpOutputArrays(pfont, this, NULL, 0, 0, 0, twsh,
			fParaRtl, nDirDepth, true);
		psegNew->SetLayout(layout);
		return;
	}

	psegNew->RecordInitializationForThisSeg(cbPrev, pbPrevSegDat);

	if ((est == kestNoMore || est == kestWsBreak) && twsh == ktwshOnlyWs &&
		dichSegLen < pchstrm->Lim() - pchstrm->Min())
	{
		//	If we are asking for white-space only, and we didn't use up everything in the font
		//	range, we could possibly have gotten an initial white-space only segment
		//	(as opposed to a writing system break).
		est = kestOkayBreak;
		psegNew->FixTermination(est);
	}

	//	Do this before fixing up the associations below.
	//if (m_pgreng->LoggingTransduction())
	//	WriteTransductionLog(pchstrm, *ppsegRet,
	//		cbPrev, pbPrevSegDat, pbNextSegDat, pcbNextSegDat);
	WriteTransductionLog(pstrmLog, pchstrm, psegNew, cbPrev, pbPrevSegDat);

	RecordAssocsAndOutput(pfont, psegNew, fWidthIsCharCount, twsh, fParaRtl, nDirDepth);

	//	Do this after fixing up the associations.
	//if (m_pgreng->LoggingTransduction())
	//	WriteAssociationLog(pchstrm, *ppsegRet);
	WriteAssociationLog(pstrmLog, pchstrm, psegNew);

	psegNew->SetLayout(layout);
}

/*----------------------------------------------------------------------------------------------
	Create a new segment to return.

	@param pchstrm				- input stream
	@param pgjus				- justification agent, in case this segment needs to be
									stretched or shrunk
	@param islotLBStreamBreak	- position of inserted line break slot (which should be equal
									to the position in the glyph generation stream);
									-1 if no line-break was inserted (we rendered to the
									end of the range)
	@param islotSurfaceBreak	- position of line break slot in final (surface) stream
	@param fStartLine			- does this segment start a line?
	@param fEndLine				- does this segment end a line?
	@param ichFontLim			- end of range that could be rendered by this font
	@param lbEndFound			- line break found; possibly adjusted here
	@param est					- why did the segment terminate
----------------------------------------------------------------------------------------------*/
void GrTableManager::InitNewSegment(Segment * psegNew,
	Font * pfont, GrCharStream * pchstrm, IGrJustifier * pgjus,
	int islotLBStreamBreak, int islotSurfaceBreak, bool fStartLine, bool fEndLine, int ichFontLim,
	LineBrk lbEndFound, SegEnd est, int * pdichSegLen)
{
	LineBrk lbStart = m_engst.m_lbPrevEnd;
	LineBrk lbEnd = lbEndFound;
	if (est == kestBadBreak)
		lbEnd = klbLetterBreak;

	int ichwMin = pchstrm->Min();
	int ichwLim;

//	int cslotPreSegStream0 = m_cslotPreSeg;
//	if (m_fInitialLB && m_cpassLB > 0)
//	{
//		// Initial LB is included in m_cslotPreSeg, but is not in stream 0.
//		// --Not really needed, because both islotLBStreamBreak and m_cslotPreSeg take
//		// the initial LB into account if it is there.
//		cslotPreSegStream0--;
//	}

	// No longer have to subtract this, because the chunk map takes it into
	// consideration.
	//int cslotPreInitialLB = m_cslotPreSeg - (m_fInitialLB ? 1 : 0);

	//int ichwOldTmp;

	if (m_engst.m_fInsertedLB)
	{
		Assert(islotLBStreamBreak > -1);
		int ichwSegLim = m_engst.LbSlotToSegLim(islotLBStreamBreak, pchstrm, m_cpassLB);
		ichwLim = ichwSegLim + pchstrm->Min(); // - cslotPreInitialLB;

		// Old result, for debugging:
		//ichwOldTmp = islotLBStreamBreak + pchstrm->Min() - m_cslotPreSeg;
	}
	else if (m_engst.m_fFinalLB || islotLBStreamBreak == -1)
		ichwLim = pchstrm->Lim();
	else
	{
		int ichwSegLim = m_engst.LbSlotToSegLim(islotLBStreamBreak, pchstrm, m_cpassLB);
		ichwLim = ichwSegLim + pchstrm->Min(); // - cslotPreInitialLB;

		// Old result, for debugging:
		// islotLBStreamBreak is the last slot in the segment, not the LB glyph; hence +1.
		//ichwOldTmp = islotLBStreamBreak + pchstrm->Min() - m_cslotPreSeg + 1;
	}

	*pdichSegLen = ichwLim - ichwMin;

	if (ichwMin >= ichwLim)
	{
		if (est == kestHardBreak)
		{
			// Empty segment cause by a hard-break.
			InitSegmentAsEmpty(psegNew, pfont, pchstrm, fStartLine, fEndLine);
			psegNew->FixTermination(est);
		}
		else
		{
			// Invalid empty segment.
			Assert(ichwMin == ichwLim);
			InitSegmentToDelete(psegNew, pfont, pchstrm);
		}
		return;
	}

	Assert(psegNew);

	psegNew->Initialize(pchstrm->TextSrc(),
		ichwMin, ichwLim,
		lbStart, lbEnd, est, fStartLine, fEndLine, m_pgreng->RightToLeft());

	psegNew->SetEngine(m_pgreng);
	psegNew->SetFont(pfont);
	psegNew->SetJustifier(pgjus);
	psegNew->SetFaceName(m_pgreng->FaceName(), m_pgreng->BaseFaceName());

	bool fNextSegNeedsContext =
		!(est == kestNoMore || est == kestWsBreak
			|| ichwLim >= pchstrm->Lim() || ichwLim >= ichFontLim);

	InitializeForNextSeg(psegNew, islotLBStreamBreak, islotSurfaceBreak, lbEnd,
		fNextSegNeedsContext, pchstrm);

	psegNew->SetPreContext(0 - m_pgreng->PreXlbContext());
}

/*----------------------------------------------------------------------------------------------
	The newly created segment is invalid, for instance, because we were backtracking and
	couldn't find a valid break point. Initialize it just enough so that it can be
	deleted by the client.
----------------------------------------------------------------------------------------------*/
void GrTableManager::InitSegmentToDelete(Segment * psegNew, Font * pfont,
	GrCharStream * pchstrm)
{
	psegNew->Initialize(pchstrm->TextSrc(), 0, 0,
		klbClipBreak, klbClipBreak, kestNothingFit, false, false,
		m_pgreng->RightToLeft());

	psegNew->SetEngine(m_pgreng);
	psegNew->SetFont(pfont);
	psegNew->SetJustifier(NULL); // can't justify an empty segment
	psegNew->SetFaceName(m_pgreng->FaceName(), m_pgreng->BaseFaceName());
	psegNew->SetPreContext(0);
}

/*----------------------------------------------------------------------------------------------
	Map from the given slot (which is generally the final LB slot) to the end of the segment,
	taking into account the fact that a single slot may represent two 16-bit surrogates.

	There are several approaches that seem reasonable but do NOT work. (I tried them!)
	You can't calculate it directly from the length of the streams, because that doesn't take
	into account the possible presence of surrogate pairs in the character stream. You can't
	use the chunk map at the point of the LB, because if a rule spanned a LB the chunk won't
	have ended there and so there won't be any valid information.
	
	What seems to work is to figure out the slot(s) in the zeroth stream using the associations
	and then use the segment offset for this slot. This should be reliable because there is
	always a one-to-one correspondence between glyphs in the line-break passes (ie, no
	insertions or deletions), so the associations should be dependable and straightforward.
----------------------------------------------------------------------------------------------*/
int EngineState::LbSlotToSegLim(int islotLB, GrCharStream * pchstrm, int cpassLB)
{	
	Assert(islotLB < OutputStream(cpassLB)->WritePos());

	// Figure out the corresponding characters in the zeroth stream. These should be straight-
	// forward to figure out because no substitutions are permitted in the LB passes.
	// I do the slots before and after the LB, if possible, as a sanity check.
	GrSlotStream * psstrmLB = OutputStream(cpassLB);
	GrSlotState * pslotPreLB = (m_fFinalLB || m_fInsertedLB) ?
		psstrmLB->SlotAt(islotLB - 1) :
		psstrmLB->SlotAt(islotLB);
	GrSlotState * pslotPostLB = (psstrmLB->WritePos() > islotLB + 1) ?
		psstrmLB->SlotAt(islotLB + 1) :
		NULL;

	int ichwPreLB = pslotPreLB->AfterAssoc();

	int ichwSegLim;
	if (pslotPostLB)
	{
		ichwSegLim = pslotPostLB->BeforeAssoc();
		Assert(ichwPreLB + 1 == ichwSegLim
			|| ichwPreLB + 2 == ichwSegLim); // in case of a surrogate
	}
	else
	{
		ichwSegLim = ichwPreLB + 1;
		while (!pchstrm->AtUnicodeCharBoundary(ichwSegLim))
			ichwSegLim++;
		Assert(pchstrm->AtUnicodeCharBoundary(ichwSegLim));
	}

	// Old buggy routine that doesn't handle the case where a rule (and hence the chunk map) did
	// not end at the LB character:
	// Map backwards through the LB pass streams, if any.
	//int islotAdjusted = islotLB + 1; // after the LB
	//for (int ipass = m_cpassLB; ipass > 0; ipass--)
	//{
	//	islotAdjusted = ChunkInPrev(ipass, islotAdjusted, pchstrm);
	//}
	//
	//int ichwSegLim = ChunkInPrev(0, islotAdjusted, pchstrm);

	// Mapping from 32-bit to 16-bit should only increase the index. Except that there
	// may be a LB in the slot stream that is not in the character stream, so it could be
	// one less.
	Assert(ichwSegLim + m_cslotPreSeg >= islotLB - 1);

	return ichwSegLim;
}

/*----------------------------------------------------------------------------------------------
	Return the chunk mapping into the previous stream, taking into account the fact that it
	is not recorded for the write-position.
----------------------------------------------------------------------------------------------*/
int GrTableManager::ChunkInPrev(int ipass, int islot, GrCharStream * pchstrm)
{
	GrSlotStream * psstrmOut = OutputStream(ipass);
	GrSlotStream * psstrmIn = (ipass == 0) ? NULL : InputStream(ipass);

	int islotTmp = islot;
	int islotAdjusted;
	do {
		if (islot >= psstrmOut->WritePos())
		{
			Assert(islot == psstrmOut->WritePos());
			islotAdjusted = (ipass == 0) ? pchstrm->SegOffset() : psstrmIn->ReadPos();
			Assert(islotAdjusted > -1);
		}
		else
			islotAdjusted = psstrmOut->ChunkInPrev(islotTmp);
		islotTmp--;
	} while (islotAdjusted == -1);

	return islotAdjusted;
}

/*----------------------------------------------------------------------------------------------
	Create a new segment corresponding to an empty string.

	@param pchstrm				- input stream
	@param fStartLine			- does this segment start a line?
	@param fEndLine				- does this segment end a line?
----------------------------------------------------------------------------------------------*/
void GrTableManager::InitSegmentAsEmpty(Segment * psegNew, Font * pfont,
	GrCharStream * pchstrm, bool fStartLine, bool fEndLine)
{
	LineBrk lbStart = m_engst.m_lbPrevEnd;
	LineBrk lbEnd = klbNoBreak;

	int ichwMin = 0;
	int ichwLim = 0;

	Assert(psegNew);

	psegNew->Initialize(pchstrm->TextSrc(), ichwMin, ichwLim,
		lbStart, lbEnd, kestNoMore, fStartLine, fEndLine, m_pgreng->RightToLeft());

	psegNew->SetEngine(m_pgreng);
	psegNew->SetFont(pfont);
	psegNew->SetJustifier(NULL); // can't justify an empty segment
	psegNew->SetFaceName(m_pgreng->FaceName(), m_pgreng->BaseFaceName());

	byte pbNextSegDat[256];
	int cbNextSegDat;
	int * pcbNextSegDat = &cbNextSegDat;

	//	Initialization for (theoretical) next segment.
	byte * pb = pbNextSegDat;
	*pb++ = byte(lbEnd);
	*pb++ = kdircNeutral;
	*pb++ = kdircNeutral;
	*pb++ = 0;
	for (int ipass = 0; ipass < m_cpass; ipass++)
		*pb++ = 0;
	*pcbNextSegDat = 0;
	psegNew->RecordInitializationForNextSeg(*pcbNextSegDat, pbNextSegDat);

	psegNew->SetPreContext(0);
}

/*----------------------------------------------------------------------------------------------
	Find a line-break and unwind the passes. Return true if we were able to
	backtrack successfully (which may have meant using a less-than-optimal line-break).
	Return false if no line-break could be found (because maxBreakWeight did not allow
	letter breaks or clipping--generally because there was already something on this
	line).

	@param pislotPrevBreak	- position of previously-created break, -1 if none;
								this is the index of the line-break glyph if any,
								or the index of the last slot in the segment;
								adjusted to contain the position of the newly
								found break
	@param plbMin			- minimum (best) line break weight to try
	@param lbMax			- maximum (worst possible) line break weight
	@param twsh				- how to handle trailing white-space
	@param fMoreText		- true if char stream doesn't go up to original lim
	@param ichwCallerBtLim	- caller's backtrack lim; -1 if caller is not backtracking
	@param plbFound			- kind of line-break created
----------------------------------------------------------------------------------------------*/
bool GrTableManager::Backtrack(int * pislotPrevBreak,
	LineBrk * plbMin, LineBrk lbMax, TrWsHandling twsh, bool /*fMoreText*/,
	int ichwCallerBtLim, bool fEndLine,
	LineBrk * plbFound)
{
	int islotStartTry;
	//if (*pislotPrevBreak == ichwCallerBtLim - 1)
	//{
	//	islotStartTry = *pislotPrevBreak;
	//}
	//else
	if (*pislotPrevBreak == -1)
	{
		//	If no line break has been found so far, figure out where to start trying based
		//	on where the final stream choked.
		GrSlotStream * psstrmFinal = OutputStream(m_cpass-1);
		if ((islotStartTry
				= m_engst.TraceStreamZeroPos(psstrmFinal->WritePos()-1, TopDirectionLevel()))
			== -1)
		{
			// Just start looking at the end of what's been generated.
            islotStartTry = OutputStream(m_cpassLB)->ReadPos() - 1;
		}
	}
	else
	{
		//	Start just before previous line break.
		if (m_engst.m_fInsertedLB || m_engst.m_fFinalLB)
			islotStartTry = *pislotPrevBreak - 2;	// skip inserted LB and previous slot
		else
			islotStartTry = *pislotPrevBreak - 1;	// skip previous slot
	}
	if (ichwCallerBtLim > -1 && islotStartTry > ichwCallerBtLim - 1)
	{
		islotStartTry = ichwCallerBtLim - 1;
	}

	//	Determine if we want to insert an actual line-break "glyph". Don't do that if
	//	we are omitting trailing white space or we are at a writing-system or font break.
	bool fInsertLB;
	if (twsh == ktwshNoWs)
		// omitting trailing white-space
		fInsertLB = false;

	//	These rules seem more complicated than what is necessary... :-/
	//else if (twsh == ktwshOnlyWs)
	//	// trailing white-space segment
	//	fInsertLB = true;
	//else if (ichwCallerBtLim > -1)
	//	// backtracking
	//	fInsertLB = true;
	//else if (*pislotPrevBreak > -1)
	//	// no longer at the edge of the writing system or font
	//	fInsertLB = true;
	//else if (!fMoreText)
	//	// at final edge of total range to render
	//	fInsertLB = true;
	//else
	//	fInsertLB = false;

	else if (!fEndLine)
		fInsertLB = false;
	else
		fInsertLB = true;

	//	Try to insert a line-break in the output of the (final) line-break pass (if any, or
	//	the output of the glyph-generation pass). First try the preferred (strictest)
	//	break weight and gradually relax.
	LineBrk lb = *plbMin;
	Assert(*plbMin <= lbMax);
	GrSlotStream * psstrmLB = OutputStream(m_cpassLB);
	islotStartTry = min(islotStartTry, psstrmLB->WritePos() - 1);
	int islotNewBreak = -1;
	while (lb <= lbMax)
	{
		LineBrk lbNextToTry;
		if (fInsertLB)
		{
			islotNewBreak = psstrmLB->InsertLineBreak(this,
				*pislotPrevBreak, m_engst.m_fInsertedLB, islotStartTry,
				lb, twsh, m_engst.m_cslotPreSeg, &lbNextToTry);
		}
		else
		{
			islotNewBreak = psstrmLB->MakeSegmentBreak(this,
				*pislotPrevBreak, m_engst.m_fInsertedLB, islotStartTry,
				lb, twsh, m_engst.m_cslotPreSeg, &lbNextToTry);
		}
		if (islotNewBreak > -1)
			break;
		if (lb >= lbMax)
			break;
		lb = IncLineBreak(lb); // relax the break weight
	}

	if (islotNewBreak == -1)
	{
		return false;
	}

	// We've successfully inserted a line break.

	if (fInsertLB)
		m_engst.m_fInsertedLB = true;

	m_engst.m_fFinalLB = false;

	UnwindAndReinit(islotNewBreak);

	*pislotPrevBreak = islotNewBreak;
	*plbMin = lb;	// return the best break we were able to find, so we don't keep trying
					// for a better one when we know now that we can't find it
	*plbFound = lb;

	return true;
}

/*----------------------------------------------------------------------------------------------
	This method is called after backtracking or removing trailing white-space.
	Unwind the following slot streams as necessary depending on where the change was made.

	@param islotNewBreak		- in output of (final) line-break pass
----------------------------------------------------------------------------------------------*/
void GrTableManager::UnwindAndReinit(int islotNewBreak)
{
	OutputStream(m_cpassLB)->ZapCalculatedDirLevels(islotNewBreak);

	//	Mark the passes before the line-break pass as fully written, so that if there
	//	isn't a line-break glyph to intercept, we don't keep trying to get more input
	//	from them.
	int ipass;
	for (ipass = 1; ipass < m_cpassLB + 1; ++ipass)
	{
		InputStream(ipass)->MarkFullyWritten();
	}

	//	Unwind the passes to the changed positions.
	//	Note that we don't need to unwind pass 0, since all it does is generate glyph IDs.
	//	Also we don't need to unwind the line-break passes, because conceptually they happen
	//	before the break is inserted.

	int islotChgPos = islotNewBreak;
	bool fFirst = true;
	for (ipass = m_cpassLB + 1 ; ipass < m_cpass ; ++ipass)
	{
		islotChgPos =
			Pass(ipass)->Unwind(this, islotChgPos, InputStream(ipass), OutputStream(ipass),
				fFirst);
		fFirst = false;
	}
	// For anything that may have been skipped in the final output:
	OutputStream(m_cpass - 1)->SetReadPos(0);
	OutputStream(m_cpass - 1)->SetReadPosMax(0);
	Pass(m_cpass - 1)->UndoResyncSkip();
	OutputStream(m_cpass - 1)->ClearSlotsSkippedToResync();

	m_engst.InitPosCache();

	m_engst.m_dxsShrinkPossible = GrSlotState::kNotYetSet; // recalculate it
}

/*----------------------------------------------------------------------------------------------
	Store information in the newly created segment that will allow us to reinitalize
	the streams in order to process the next segment, appropriately handling any
	cross-line-boundary contextuals.

	The information required is the number of glyphs in the first stream that must
	be reprocessed, and a skip-offset between each stream indicating the beginning
	of chunk boundaries.
	
	In most cases, where there are no cross-line-boundary contextuals, all these numbers
	will be zero.

	The buffer generated is of the following format (all are 1 byte, unsigned):
			end breakweight
			previous strong directionality code
			previous terminator dir code
			restart backup
			skip offsets for each pass
	This format must match what is generated by InitializeStreams.

	@param pseg					- newly created segment
	@param islotUnderBreak		- position of line break inserted in underlying stream (output
									of glyph-generation pass--stream 0); -1 if we rendered to
									the end of the range
	@param islotSurfaceBreak	- position of line break in surface stream;
									-1 if we rendered to the end of the range
	@param lbEnd				- kind of line-break at the end of this segment
	@param fNextSegNeedsContext	- true if we need to remember the context for the next seg
	@param pchstrm				- character stream input for this segment
	@param cbNextMax			- amount of space available in the buffer
	@param pbNextSegDat			- buffer for initializing next segment
	@param pcbNextSegDat		- amount of space used in buffer
----------------------------------------------------------------------------------------------*/
void GrTableManager::InitializeForNextSeg(Segment * pseg,
	int islotUnderBreak, int islotSurfaceBreak, LineBrk lbEnd,
	bool fNextSegNeedsContext, GrCharStream * pchstrm)
{
	byte pbNextSegDat[256];
	int cbNextSegDat;
	int * pcbNextSegDat = &cbNextSegDat;

	std::vector<int> vcslotSkipOffsets;
	vcslotSkipOffsets.resize(m_cpass);

	byte * pb = pbNextSegDat;

	if (!fNextSegNeedsContext)
	{
		//	No contextualization between this segment and the next, whatever kind of renderer
		//	it might use.
		*pcbNextSegDat = 0;
		return;
	}

	gid16 chwLB = LBGlyphID();

	int islotUnderLim = islotUnderBreak;
	if (!m_engst.m_fInsertedLB && !m_engst.m_fFinalLB)
		// Then islotUnderBreak is the position of the last slot in the segment; increment
		// to get the lim in stream zero.
		islotUnderLim++;

	int islotSurfaceLim = islotSurfaceBreak;
	if (islotSurfaceLim == -1)
		islotSurfaceLim = OutputStream(m_cpass - 1)->FinalSegLim();

	*pb++ = byte(lbEnd);

	//	Find previous strong and terminator directionality codes.

	GrSlotStream * psstrm = OutputStream(m_cpass - 1);
	DirCode dircStrong = kdircNeutral;
	DirCode dircTerm = kdircNeutral;
	int islot;
	for (islot = islotSurfaceLim; islot-- > 0; )
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		DirCode dirc = pslot->Directionality();
		if (dircTerm == kdircNeutral && dirc == kdircEuroTerm)
			dircTerm = dirc;
		if (StrongDir(dirc))
		{
			dircStrong = dirc;
			break;
		}
	}
	*pb++ = byte(dircStrong);
	*pb++ = byte(dircTerm);

	//	Record how much of each pass to redo in the next segment.
	//	The basic algorithm is described in the "WR Data Transform Engine" document.
	//	But here we are doing a variation on what is described there. We are calculating
	//	the backup locations for BOTH the rule start locations and the necessary pre-contexts.
	//	It is the precontext values that are recorded for use by the following segment. But we
	//	also calculate the positions for the rule-start locations to make sure there is enough
	//	pre-context at each pass.

	int islotRS = islotSurfaceLim;	// rule-start positions
	int islotPC = islotRS;  // to handle pre-context positions

	// If line-breaks are irrelevant to this font, we don't need to bother.
	bool fGiveUp = !m_pgreng->LineBreakFlag();

	int ipass;
	for (ipass = m_cpass - 1; ipass >= 0 && !fGiveUp; --ipass)
	{
		GrSlotStream * psstrmIn = (ipass == 0) ? NULL : InputStream(ipass);
		GrSlotStream * psstrmOut = OutputStream(ipass);
		int islotBackupMin = (ipass == 0) ? 0 : psstrmOut->SegMin();
		if (ipass >= m_cpassLB && m_engst.m_fInitialLB)
			islotBackupMin++; // how far we can back up--not before the start of this segment

		int islotPrevRS = 0; // corresponding slot in the previous stream for rule-start position
		int islotPrevPC = 0; // same for pre-context position

		//	Initially we have to include at least this many prior slots:
		int cslotSkipOffset = 0;

		if (islotRS == psstrmOut->WritePos())
		{
			islotPrevRS = (ipass == 0) ? pchstrm->Pos() : psstrmIn->ReadPos();
		}
		else
		{
			//	Ignore the final line-break, because it is explicitly inserted during
			//	the (final) line-break pass.
//			if (ipass == m_cpassLB && psstrm->SlotAt(islotRS)->IsFinalLineBreak(chwLB))
//			{
//				Assert(islotRS > 0);
//				islotRS--;
//			}
			//	Ignore the chunk in the pass that generated the line break that corresponds
			//	to the inserted line break being chunked with the previous glyph--we don't
			//	want to treat that as a cross-line-boundary contextual!
			if (ipass == m_cpassLB && psstrmOut->SlotAt(islotRS)->IsFinalLineBreak(chwLB))
			{
//				Assert(psstrm->ChunkInPrev(islotRS) == -1);
				if (islotRS + 1 == psstrmOut->WritePos())
					islotPrevRS = (ipass == 0) ? pchstrm->SegOffset() : psstrmIn->ReadPos();
				else
					islotPrevRS = psstrmOut->ChunkInPrev(islotRS + 1);
			}
			else
			{
				//	Back up to the beginning of a chunk.
				while (islotRS >= islotBackupMin &&
					(islotPrevRS = psstrmOut->ChunkInPrev(islotRS)) == -1)
				{
					--islotRS;
					//++cslotSkipOffset;
				}
			}
		}
		if (islotPrevRS == -1 || islotRS < islotBackupMin) // hit the start of the segment
		{
			//	In order to get enough context to be sure to run the same rules again,
			//	we'd have to back up into the previous segment. Too complicated, so give up.
			fGiveUp = true;
			break;
		}

		int cslotPreContext = Pass(ipass)->MaxRulePreContext();
LBackupPC:
		if (islotPC == psstrmOut->WritePos())
		{
			islotPrevPC = (ipass == 0) ? pchstrm->Pos() : psstrmIn->ReadPos();
		}
		else
		{
			if (ipass == m_cpassLB && psstrmOut->SlotAt(islotPC)->IsFinalLineBreak(chwLB))
			{
				if (islotPC+1 == psstrmOut->WritePos())
					islotPrevPC = (ipass == 0) ? pchstrm->SegOffset() : psstrmIn->ReadPos();
				else
					islotPrevPC = psstrmOut->ChunkInPrev(islotPC+1);
			}
			else
			{
				// Back up to the beginning of a chunk; also make sure there are enough slots
				// to handle the required pre-context for this pass.
				while (islotPC >= islotBackupMin &&
					((islotPrevPC = psstrmOut->ChunkInPrev(islotPC)) == -1 ||
						islotPrevRS - islotPrevPC < cslotPreContext))
				{
					--islotPC;
					++cslotSkipOffset;
				}
			}
		}
		if (islotPC >= islotBackupMin && islotPrevRS - islotPrevPC < cslotPreContext)
		{
			--islotPC;
			++cslotSkipOffset;
			goto LBackupPC;
		}
		if (islotPrevPC == -1 || islotPC < islotBackupMin) // hit the start of the segment
		{
			fGiveUp = true;
			break;
		}

		vcslotSkipOffsets[ipass] = cslotSkipOffset;

		islotRS = islotPrevRS;
		islotPC = islotPrevPC;

		if (ipass == 0)
		{
			// Convert from underlying chars to stream-zero slots.
			islotRS += m_engst.m_cslotPreSeg; 
			islotPC += m_engst.m_cslotPreSeg;
		}
	}

	//	The following should be the case unless they tried to position contextually across
	//	the line break boundary, which is a very strange thing to do! - no longer true,
	//	because the pre-context is now included in this count, and it normal to have
	//	a pre-context in the final positioning pass.
	//Assert(vcslotSkipOffsets[m_cpass-1] == 0);

	//	Also no adjustments are made by the glyph-generation pass, so this should always
	//	be true:
	Assert(vcslotSkipOffsets[0] == 0);

	//	The number of slots in the underlying input previous to the line-break
	//	that must be reprocessed:
	int cslotPreLB = islotUnderLim - islotPC;

	if (fGiveUp)
	{
		// We can't reasonably provide enough context, so don't try to do it at all.
		cslotPreLB = 0;
		for (ipass = 0; ipass < m_cpass; ipass++)
			vcslotSkipOffsets[ipass] = 0;
	}

	Assert(cslotPreLB >= 0);
	Assert(cslotPreLB < 0xFF);

	*pb++ = byte(cslotPreLB);
	for (ipass = 0; ipass < m_cpass; ipass++)
		*pb++ = byte(vcslotSkipOffsets[ipass]);

	*pcbNextSegDat = 4 + m_cpass;

	pseg->RecordInitializationForNextSeg(*pcbNextSegDat, pbNextSegDat);
}

/*----------------------------------------------------------------------------------------------
	Calculate the associations, and record the output slots in the segment.
----------------------------------------------------------------------------------------------*/
void GrTableManager::RecordAssocsAndOutput(Font * pfont,
	Segment * pseg, bool /*fWidthIsCharCount*/,
	TrWsHandling twsh, bool fParaRtl, int nDirDepth)
{
	int cchwUnderlying = pseg->stopCharacter() - pseg->startCharacter();

	GrSlotStream * psstrmFinal = OutputStream(m_cpass-1);
	int csloutSurface = psstrmFinal->WritePos() - psstrmFinal->IndexOffset(); // # of output slots

	psstrmFinal->SetNeutralAssociations(LBGlyphID());

	float xsTotalWidth, xsVisWidth;

	//	Make sure the final positions are set for every glyph.
	CalcPositionsUpTo(m_cpass-1, reinterpret_cast<GrSlotState *>(NULL), false,
		&xsTotalWidth, &xsVisWidth); 
	pseg->SetWidths(xsVisWidth, xsTotalWidth);

	pseg->SetUpOutputArrays(pfont, this, psstrmFinal, cchwUnderlying, csloutSurface, LBGlyphID(),
		twsh, fParaRtl, nDirDepth);

	//	Set underlying-to-surface assocations in the segment.
	CalculateAssociations(pseg, csloutSurface);
}

/*----------------------------------------------------------------------------------------------
	Calculate the underlying-to-surface associations and ligature mappings.
	Assumes the arrays have been properly initialized.
----------------------------------------------------------------------------------------------*/
void GrTableManager::CalculateAssociations(Segment * pseg, int /*csloutSurface*/)
{
	GrSlotStream * psstrmFinal = OutputStream(m_cpass-1);

	std::vector<int> vichwAssocs;
	std::vector<int> vichwComponents;
	std::vector<int> vicomp;

	for (int islot = psstrmFinal->IndexOffset(); islot < psstrmFinal->WritePos(); islot++)
	{
		GrSlotState * pslot = psstrmFinal->SlotAt(islot);
		int islout = islot - psstrmFinal->IndexOffset();
		if (!pslot->IsLineBreak(LBGlyphID()))
		{
			vichwAssocs.clear();
			pslot->AllAssocs(vichwAssocs);

			size_t iichw;
			for (iichw = 0; iichw < vichwAssocs.size(); ++iichw)
			{
				pseg->RecordSurfaceAssoc(vichwAssocs[iichw], islout, 0);
			}

			vichwComponents.clear();
			vicomp.clear();
			if (pslot->HasComponents())
				pslot->AllComponentRefs(vichwComponents, vicomp);

			for (iichw = 0; iichw < vichwComponents.size(); iichw++)
			{
				pseg->RecordLigature(vichwComponents[iichw], islout, vicomp[iichw]);
			}
		}
	}

	AdjustAssocsForOverlaps(pseg);
	pseg->CleanUpAssocsVectors();

//	pseg->AdjustForOverlapsWithPrevSeg();	// for any characters officially in the
											// previous segment but rendered in this one,
											// or vice versa

	// TODO SharonC: for all characters that are right-to-left in the underlying stream,
	// reverse the before and after flags in the segment.
}

/*----------------------------------------------------------------------------------------------
	For any slots associated with characters in the new segment but not rendered in the
	segment, adjust the underlying-to-surface mappings accordingly. Ie, set the before
	mapping to kNegInfinity for slots at the beginning of the streams, and set the
	after mapping to kPosInfinity for slots at the end of the streams.

	TODO SharonC: Rework this method more carefully to handle each slot exactly once. The current
	approach could possibly, for instance, process a slot that is inserted in one pass
	and deleted in the next.
----------------------------------------------------------------------------------------------*/
void GrTableManager::AdjustAssocsForOverlaps(Segment * pseg)
{
	if (!m_engst.m_fInitialLB && !m_engst.m_fFinalLB && !m_engst.m_fInsertedLB)
		// no cross-line contextualization possible
		return;

	gid16 chwLB = LBGlyphID();
	std::vector<int> vichwAssocs;

	//	Process any slots output by any substitution pass (or later) on either side of the
	//	line-breaks. These are the relevant slots that are rendered in the previous and
	//	following segments.
	for (int ipass = m_cpass; ipass-- > m_cpassLB + 1; )
	{
		GrSlotStream * psstrm = OutputStream(ipass);
		int islotMin = (ipass == m_cpass - 1) ? psstrm->IndexOffset() : 0;

		if (m_engst.m_fInitialLB)
		{
			for (int islot = islotMin; ; islot++)
			{
				GrSlotState * pslot = psstrm->SlotAt(islot);
				if (pslot->IsInitialLineBreak(chwLB))
					break;
				Assert(!pslot->IsFinalLineBreak(chwLB));
				if (pslot->PassModified() != ipass)
					continue;

				vichwAssocs.clear();
				pslot->AllAssocs(vichwAssocs);
				for (size_t iichw = 0; iichw < vichwAssocs.size(); ++iichw)
					pseg->MarkSlotInPrevSeg(vichwAssocs[iichw], islot);
			}
		}

		if ((m_engst.m_fFinalLB || m_engst.m_fInsertedLB) && ipass > m_cpassLB)
		{
			for (int islot = psstrm->WritePos(); islot-- > islotMin ; )
			{
				GrSlotState * pslot = psstrm->SlotAt(islot);
				if (pslot->IsFinalLineBreak(chwLB))
					break;
				Assert(!pslot->IsInitialLineBreak(chwLB));
				if (pslot->PassModified() != ipass)
					continue;

				vichwAssocs.clear();
				pslot->AllAssocs(vichwAssocs);
				for (size_t iichw = 0; iichw < vichwAssocs.size(); ++iichw)
					pseg->MarkSlotInNextSeg(vichwAssocs[iichw], islot);
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Initialize all the streams. In particular, process the information about the
	skip-offsets to handle segments other than the first.

	@param cbPrev			- number of bytes in the pbPrevSegDat buffer
	@param pbPrevSegDat		- buffer in the following format (all are 1 byte, unsigned):
								prev seg's end breakweight
								prev seg's previous strong directionality code
								prev seg's previous terminator dir code
								restart backup
								skip offsets for each pass
							This format must match what is generated by InitializeForNextSeg.
----------------------------------------------------------------------------------------------*/
void EngineState::InitializeStreams(GrTableManager * ptman,
	GrCharStream * pchstrm,
	int cbPrev, byte * pbPrevSegDat, bool fNeedFinalBreak, int * pislotFinalBreak)
{
	int cpassLB = ptman->NumberOfLbPasses();
	int ipassPos1 = ptman->FirstPosPass();

	CreateSlotStreams();

	int cslotBackup = 0;

	InitForNewSegment(ptman);

	if (cbPrev == 0)
	{
		m_lbPrevEnd = klbNoBreak;
		m_dircInitialStrongDir = kdircNeutral;
		m_dircInitialTermDir = kdircNeutral;
		for (int ipass = 0; ipass < m_cpass; ++ipass)
		{
			OutputStream(ipass)->Initialize(ipassPos1, false);
			m_prgzpst[ipass].SetResyncSkip(0);

			if (ptman->LoggingTransduction())
				m_prgzpst[ipass].InitializeLogInfo();
		}
	}
	else
	{
		Assert(cbPrev >= 4);
		byte * pb = pbPrevSegDat;

		m_lbPrevEnd = LineBrk(*pb++);

		//	Directionality codes from the previous segment.
		m_dircInitialStrongDir = DirCode((int)*pb++);
		m_dircInitialTermDir = DirCode((int)*pb++);

		cslotBackup = (int)*pb++;
		Assert((cbPrev == 4 || cslotBackup == 0) || cbPrev == m_cpass + 4);

		for (int ipass = 0; ipass < m_cpass; ++ipass)
		{
			OutputStream(ipass)->Initialize(ipassPos1, true);
			if (cbPrev == 4)
				m_prgzpst[ipass].SetResyncSkip(0);	// different writing system
			else
				m_prgzpst[ipass].SetResyncSkip(*pb++);

			if (ptman->LoggingTransduction())
				m_prgzpst[ipass].InitializeLogInfo();
		}

		pchstrm->Backup(cslotBackup);
	}

	if (cbPrev > 0 || pchstrm->StartLine())
	{
		//	Go ahead and run the initial line-break passes (if any), and then insert
		//	a line-break at the appropriate place where the new segment will begin.
		int cslotToGet = cslotBackup;
LGetMore:
		ptman->Pass(0)->ExtendGlyphIDOutput(ptman, pchstrm, OutputStream(0),
			-1, 0, klbWordBreak,
			cslotToGet, fNeedFinalBreak, m_twsh, pislotFinalBreak);
		OutputStream(0)->SetSegMin(cslotBackup);
		for (int ipass = 1; ipass <= cpassLB; ipass++)
		{
			if (cslotToGet > 0)
			{
				int nRet = kNextPass;
				int cslotGot;
				ptman->Pass(ipass)->ExtendOutput(ptman,
					InputStream(ipass), OutputStream(ipass),
					cslotToGet, m_twsh, &nRet, &cslotGot, pislotFinalBreak);
				if (nRet != kNextPass)
				{
					cslotToGet = 1;
					goto LGetMore;
				}
			}
			OutputStream(ipass)->SetSegMin(cslotBackup);
		}
		Assert(OutputStream(cpassLB)->WritePos() >= cslotBackup);
		m_cslotPreSeg = cslotBackup;
		if (pchstrm->StartLine())
		{
			OutputStream(cpassLB)->AppendLineBreak(ptman, pchstrm, m_lbPrevEnd,
				(ptman->RightToLeft() ? kdircRlb : kdircLlb), cslotBackup, true, -1);
			SetInitialLB();
			m_cslotPreSeg++;
			if (cpassLB > 0 && *pislotFinalBreak > -1)
				// See corresponding kludge in GrPass::ExtendGlyphIDOutput.
				*pislotFinalBreak += 1;
		}
		else
		{
			OutputStream(cpassLB)->SetSegMin(cslotBackup);
		}
		OutputStream(cpassLB)->CalcIndexOffset(ptman);
	}

	else
	{
		Assert(cslotBackup == 0);
		OutputStream(0)->SetSegMin(0);
		m_cslotPreSeg = 0;
	}
}

/*----------------------------------------------------------------------------------------------
	Reinitialize as we start to generate a new segment.
----------------------------------------------------------------------------------------------*/
void EngineState::InitForNewSegment(GrTableManager * ptman)
{
	//	Set up a fresh batch of slots.
	DestroySlotBlocks();
	m_islotblkCurr = -1;
	m_islotNext = kSlotBlockSize;

	m_fInitialLB = false;
	m_fFinalLB = false;
	m_fInsertedLB = false;
	m_fExceededSpace = false;
	m_fHitHardBreak = false;
	m_fRemovedWhtsp = false;
	InitPosCache();
	//////m_pgg = pgg;

	for (int ipass = 0; ipass < m_cpass; ipass++)
		m_prgzpst[ipass].InitForNewSegment(ipass, ptman->Pass(ipass)->MaxRuleContext());
}

/*----------------------------------------------------------------------------------------------
	Calculate the position in stream zero of the given slot in the final stream,
	based on the associations. (This will be the same as the position at the end of the
	line-break passes, which is really what we want.)

	@param islotFinal	- index of the final slot in the final stream
----------------------------------------------------------------------------------------------*/
int EngineState::TraceStreamZeroPos(int islotFinal, int nTopDir)
{
	GrSlotStream * psstrmFinal = OutputStream(m_cpass - 1);
	if (psstrmFinal->WritePos() == 0)
		return -1;

	GrSlotState * pslot = psstrmFinal->SlotAt(islotFinal);

	//	If we're in the middle of reordered bidi stuff, just use the end of the stream.
	if (pslot->DirLevel() > nTopDir)
		return -1;

	int islot = pslot->BeforeAssoc();
	if (islot == kPosInfinity || islot < 0)
		return -1;
	return islot + m_cslotPreSeg;
}

/*----------------------------------------------------------------------------------------------
	Return the next higher (more relaxed, less desirable) line break weight.
----------------------------------------------------------------------------------------------*/
LineBrk GrTableManager::IncLineBreak(LineBrk lb)
{
	Assert(lb != klbClipBreak);
	return (LineBrk)((int)lb + 1);
}

/*----------------------------------------------------------------------------------------------
	Return a pointer to a slot that is near the current position in the given stream.
	Return NULL if no slots have been output at all.
	This is used for initializing features of line-break slots.
	@param ipassArg			- pass at which to start looking
	@param islot			- slot to look for; -1 if considering the current position
----------------------------------------------------------------------------------------------*/
GrSlotState * EngineState::AnAdjacentSlot(int ipassArg, int islot)
{
	int ipass = ipassArg;
	while (ipass >= 0)
	{
		GrSlotStream * psstrm = OutputStream(ipass);
		if (psstrm->NumberOfSlots() > 0)
		{
			if (islot == -1)
			{
				if (psstrm->AtEnd())
					return psstrm->SlotAt(psstrm->WritePos() - 1);
				else
					return psstrm->Peek();
			}
			else
			{
				if (psstrm->WritePos() <= islot)
					return psstrm->SlotAt(psstrm->WritePos() - 1);
				else
					return psstrm->SlotAt(islot);
			}
		}
		ipass--;
	}
	return NULL;
}

/*----------------------------------------------------------------------------------------------
	Simple access methods.
----------------------------------------------------------------------------------------------*/
GrEngine * GrTableManager::Engine()
{
	return m_pgreng;
}

EngineState * GrTableManager::State()
{
	return &m_engst;
}

GrEngine * EngineState::Engine()
{
	return m_ptman->Engine();
}

GrTableManager * EngineState::TableManager()
{
	return m_ptman;
}

/*----------------------------------------------------------------------------------------------
	Memory management for slots
----------------------------------------------------------------------------------------------*/

//	standard for pass 0 slots
void EngineState::NewSlot(
	gid16 gID, GrFeatureValues fval, int ipass, int ichwSegOffset, int nUnicode,
	GrSlotState ** ppslotRet)
{
	NextSlot(ppslotRet);
	(*ppslotRet)->Initialize(gID, Engine(), fval, ipass, ichwSegOffset, nUnicode);
}

//	line-break slots
void EngineState::NewSlot(
	gid16 gID, GrSlotState * pslotFeat, int ipass, int ichwSegOffset,
	GrSlotState ** ppslotRet)
{
	NextSlot(ppslotRet);
	(*ppslotRet)->Initialize(gID, Engine(), pslotFeat, ipass, ichwSegOffset);
}

//	for inserting new slots after pass 0 (under-pos and unicode are irrelevant)
void EngineState::NewSlot(
	gid16 gID, GrSlotState * pslotFeat, int ipass,
	GrSlotState ** ppslotRet)
{
	NextSlot(ppslotRet);
	(*ppslotRet)->Initialize(gID, Engine(), pslotFeat, ipass);
}

//	for making a new version of the slot initialized from the old
void EngineState::NewSlotCopy(GrSlotState * pslotCopyFrom, int ipass,
	GrSlotState ** ppslotRet)
{
	NextSlot(ppslotRet);
	(*ppslotRet)->InitializeFrom(pslotCopyFrom, ipass);
}

void EngineState::NextSlot(GrSlotState ** ppslotRet)
{
	int cnExtraPerSlot = m_cUserDefn + (m_cCompPerLig * 2) + m_cFeat;
	if (m_islotNext >= kSlotBlockSize)
	{
		//	Allocate a new block of slots
		GrSlotState * slotblkNew = new GrSlotState[kSlotBlockSize];
		u_intslot * prgnSlotBuf = new u_intslot[kSlotBlockSize * cnExtraPerSlot];

		m_vslotblk.push_back(slotblkNew);
		m_vprgnSlotVarLenBufs.push_back(prgnSlotBuf);
		m_islotblkCurr++;
		m_islotNext = 0;
		Assert((unsigned)m_islotblkCurr == m_vslotblk.size()-1);
	}

	*ppslotRet = m_vslotblk[m_islotblkCurr] + m_islotNext;
	(*ppslotRet)->BasicInitialize(m_cUserDefn, m_cCompPerLig, m_cFeat,
		(m_vprgnSlotVarLenBufs[m_islotblkCurr] + (m_islotNext * cnExtraPerSlot)));
	m_islotNext++;
}

/*----------------------------------------------------------------------------------------------
	Convert the number of font design units into source device logical units.
----------------------------------------------------------------------------------------------*/
float GrTableManager::EmToLogUnits(int m)
{
	return m_engst.EmToLogUnits(m);
}

float EngineState::EmToLogUnits(int m)
{
	if (m == 0)
		return 0;

	float xysFontEmSquare;
	m_pfont->getFontMetrics(NULL, NULL, &xysFontEmSquare);

	// mEmUnits should be equal to the design units in the font's em square
	int mEmUnits = Engine()->GetFontEmUnits();
	if (mEmUnits <= 0)
	{
		Warn("Failed to obtain font em-units");
		return (float)m;
	}

	return GrEngine::GrIFIMulDiv(m, xysFontEmSquare, mEmUnits);
}

/*----------------------------------------------------------------------------------------------
	Convert the source device coordinates to font design units.
----------------------------------------------------------------------------------------------*/
int GrTableManager::LogToEmUnits(float xys)
{
	return m_engst.LogToEmUnits(xys);
}

int EngineState::LogToEmUnits(float xys)
{
	if (xys == 0)
		return 0;

	float xysFontEmSquare;
	m_pfont->getFontMetrics(NULL, NULL, &xysFontEmSquare);

	// mEmUnits should be equal to the design units in the font's em square
	int mEmUnits = Engine()->GetFontEmUnits();
	if (mEmUnits < 0)
	{
		Warn("Failed to obtain font em-units");
		return (int)xys;
	}

	return GrEngine::GrFIFMulDiv(xys, mEmUnits, xysFontEmSquare);
}

/*----------------------------------------------------------------------------------------------
	Find the coordinates for a given glyph point.
	Return true on success, false otherwise.
----------------------------------------------------------------------------------------------*/
bool GrTableManager::GPointToXY(gid16 chwGlyphID, int nGPoint, float * pxs, float * pys)
{
	return m_engst.GPointToXY(chwGlyphID, nGPoint, pxs, pys);
}

bool EngineState::GPointToXY(gid16 chwGlyphID, int nGPoint, float * pxs, float * pys)
{
	*pxs = INT_MIN;
	*pys = INT_MIN;
	Point pointRet;
	m_pfont->getGlyphPoint(chwGlyphID, nGPoint, pointRet);
	*pxs = pointRet.x;
	*pys = pointRet.y;

	return true;
}

/*----------------------------------------------------------------------------------------------
	Call the justification routines.
----------------------------------------------------------------------------------------------*/
void GrTableManager::CallJustifier(IGrJustifier * pgjus, int ipassCurr,
	float dxUnjustified, float dxJustified, bool fEndLine)
{
	// Indicates the stream the justification routines will read and modify:
	m_engst.m_ipassJustCalled = ipassCurr;

	int iGlyphMin = OutputStream(ipassCurr)->SegMin();
	int iGlyphLim = OutputStream(ipassCurr)->SegLimIfKnown();
	if (iGlyphLim == -1)
		iGlyphLim = OutputStream(ipassCurr)->WritePos();

	GrSlotStream * psstrm = OutputStream(ipassCurr);
	if (m_pgreng->BasicJustification() && fEndLine)
        UnstretchTrailingWs(psstrm, iGlyphLim);

	pgjus->adjustGlyphWidths(m_pgreng, iGlyphMin, iGlyphLim, dxUnjustified, dxJustified);

	// After returning, reading and modifying slot attributes is not permitted:
	m_engst.m_ipassJustCalled = -1;
}

/*----------------------------------------------------------------------------------------------
	For the basic justification approach, remove stretch from the trailing whitespace.
----------------------------------------------------------------------------------------------*/
void GrTableManager::UnstretchTrailingWs(GrSlotStream * psstrm, int iGlyphLim)
{
	int islot = iGlyphLim - 1;
	while (islot >= 0)
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		if (pslot->IsLineBreak(LBGlyphID()))
		{ } // keep looking for the trailing space
		else if (pslot->IsSpace(this))
			pslot->SetJStretch(0); // TODO: do this at level 0
		else
			// something other than space--quit
			return;
		islot--;
	}
}

/*----------------------------------------------------------------------------------------------
	Determine how much shrink is permissible.

	This method is really used only for testing the shrink mechanism, since at this time
	we don't allow low-end justification to do any shrinking.
----------------------------------------------------------------------------------------------*/
void GrTableManager::DetermineShrink(IGrJustifier * pgjus, int ipassCurr)
{
	if (m_engst.m_dxsShrinkPossible != GrSlotState::kNotYetSet)
		return;

	if (m_engst.m_jmodi != kjmodiCanShrink || pgjus == NULL)
	{
		m_engst.m_dxsShrinkPossible = 0;
		return;
	}

	if (ipassCurr != m_ipassJust1 - 1)
		return;
	
	GrSlotStream * psstrm = OutputStream(ipassCurr);
	if (!psstrm->FullyWritten())
		return;

	// Normally just answer 0. To test shrinking, turn on the code below.
	m_engst.m_dxsShrinkPossible = 0;

#if TEST_SHRINK
	int mTotal = 0;
	int mShrink = 0;
	int islotLim = psstrm->SegLimIfKnown();
	if (islotLim == -1)
		islotLim = psstrm->WritePos();
	for ( ; islotLim >= 0; islotLim--)
	{
		// Trailing white space is not shrinkable.
		GrSlotState * pslot = psstrm->SlotAt(islotLim - 1);
		if (pslot->IsLineBreak(LBGlyphID()))
			continue;
		if (!pslot->IsSpace(this))
			break;
	}
	int islot;
	for (islot = psstrm->SegMin(); islot < islotLim; islot++)
	{
		GrSlotState * pslot = psstrm->SlotAt(islot);
		if (pslot->IsLineBreak(LBGlyphID()))
			continue;
		mTotal += pslot->AdvanceX(this);
		mShrink += pslot->JShrink();
	}

	// Say that we are allowed to shrink up to 10% of the width of the segment.
	mShrink = min(mShrink, mTotal/10);
	m_engst.m_dxsShrinkPossible = EmToLogUnits(mShrink);
#endif // TEST_SHRINK
}

/*----------------------------------------------------------------------------------------------
	Return the attribute of the given slot, back to the justification routine. Return
	logical units.
----------------------------------------------------------------------------------------------*/
GrResult EngineState::GetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel,
	float * pValueRet)
{
	GrResult res;
	int valueRetInt = 0;
	switch(jgat)
	{
	case kjgatWeight:
	case kjgatStretchInSteps:
	case kjgatBreak:
		res = GetGlyphAttrForJustification(iGlyph, jgat, nLevel, &valueRetInt);
		*pValueRet = (float)valueRetInt;
		return res;
	default:
		break;
	}

	if (m_ipassJustCalled == -1)
		return kresUnexpected;

	if (nLevel != 1)
		return kresInvalidArg;

	GrSlotStream * psstrm = OutputStream(m_ipassJustCalled);

	if (iGlyph < -1 || iGlyph >= psstrm->WritePos())
		return kresInvalidArg;

	GrSlotState * pslot = psstrm->SlotAt(iGlyph);

	// bool fConvertUnits = true;

	switch(jgat)
	{
	case kjgatStretch:
		*pValueRet = EmToLogUnits(pslot->JStretch());
		break;
	case kjgatShrink:
		*pValueRet = EmToLogUnits(pslot->JShrink());
		break;
	case kjgatStep:
		*pValueRet = EmToLogUnits(pslot->JStep());
		break;
	default:
		return kresNotImpl;
	}
	return kresOk;
}

GrResult EngineState::GetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel,
	int * pValueRet)
{
	GrResult res;
	float valueRetFloat;
	switch(jgat)
	{
	case kjgatStretch:
	case kjgatShrink:
	case kjgatStep:
		res = GetGlyphAttrForJustification(iGlyph, jgat, nLevel, &valueRetFloat);
		*pValueRet = GrEngine::RoundFloat(valueRetFloat);
		return res;
	default:
		break;
	}

	if (m_ipassJustCalled == -1)
		return kresUnexpected;

	if (nLevel != 1)
		return kresInvalidArg;

	GrSlotStream * psstrm = OutputStream(m_ipassJustCalled);

	if (iGlyph < -1 || iGlyph >= psstrm->WritePos())
		return kresInvalidArg;

	GrSlotState * pslot = psstrm->SlotAt(iGlyph);

	int mStretch, mStep;
	//float xsStretch, xsStep;

	switch(jgat)
	{
	case kjgatWeight:
		*pValueRet = pslot->JWeight();
		break;
	case kjgatStretchInSteps:
		// ???? should this be calculated in terms of logical units??
		// Probably doesn't matter, since the logical units are floating point, so the
		// level of precision is about the same.
		mStretch = pslot->JStretch();
		mStep = pslot->JStep();
		if (mStep == 0)
			return kresUnexpected;
		*pValueRet = int(mStretch / mStep); // round down
		break;
	//case kjgatStretchInSteps:
	//	xsStretch = EmToLogUnits(pslot->JStretch());
	//	xsStep = EmToLogUnits(pslot->JStep());
	//	if (xsStep == 0)
	//		return kresUnexpected;
	//	*pValueRet = int(xsStretch / xsStep);	// round down
	//	break;
	case kjgatBreak:
		*pValueRet = pslot->BreakWeight();
		break;
	default:
		return kresNotImpl;
	}
	return kresOk;
}

/*----------------------------------------------------------------------------------------------
	Set the attribute of the given slot, for the justification routine.
----------------------------------------------------------------------------------------------*/
GrResult EngineState::SetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel,
	float value)
{
	GrResult res;
	int valueInt;
	switch(jgat)
	{
	case kjgatWeight:
	case kjgatStretchInSteps:
	case kjgatBreak:
		valueInt = (int)value;
		res = SetGlyphAttrForJustification(iGlyph, jgat, nLevel, valueInt);
		return res;
	default:
		break;
	}

	if (m_ipassJustCalled == -1)
		return kresUnexpected;

	if (nLevel != 1)
		return kresInvalidArg;

	GrSlotStream * psstrm = OutputStream(m_ipassJustCalled);

	if (iGlyph < -1 || iGlyph >= psstrm->WritePos())
		return kresInvalidArg;

	GrSlotState * pslot = psstrm->SlotAt(iGlyph);

	int mValue = LogToEmUnits(value);
	mValue = min(mValue, 0xFFFF); // truncate to an unsigned short

	// Review: do we really want to allow them to set stretch/shrink/step/weight? The normal
	// thing for them to do is set width.
	switch(jgat)
	{
	case kjgatStretch:
		pslot->SetJStretch(mValue);
		break;
	case kjgatShrink:
		pslot->SetJShrink(mValue);
		break;
	case kjgatStep:
		pslot->SetJStep(mValue);
		break;
	case kjgatWidth:
		pslot->SetJWidth(mValue);
		break;
	default:
		return kresNotImpl;
	}
	return kresOk;
}

GrResult EngineState::SetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel,
	int value)
{
	GrResult res;
	float valueFloat;
	switch(jgat)
	{
	case kjgatStretch:
	case kjgatShrink:
	case kjgatStep:
	case kjgatWidth:
		valueFloat = (float)value;
		res = SetGlyphAttrForJustification(iGlyph, jgat, nLevel, valueFloat);
		return res;
	default:
		break;
	}

	if (m_ipassJustCalled == -1)
		return kresUnexpected;

	if (nLevel != 1)
		return kresInvalidArg;

	GrSlotStream * psstrm = OutputStream(m_ipassJustCalled);

	if (iGlyph < -1 || iGlyph >= psstrm->WritePos())
		return kresInvalidArg;

	GrSlotState * pslot = psstrm->SlotAt(iGlyph);

 	int mValue, cSteps;
	switch(jgat)
	{
	case kjgatWeight:
		pslot->SetJWeight(value);
		break;
	case kjgatWidthInSteps:
		cSteps = value;
		mValue = pslot->JStep();
		if (mValue == 0)
			return kresUnexpected;
		pslot->SetJWidth(mValue * cSteps);
		break;
	default:
		return kresNotImpl;
	}
	return kresOk;
}

//:>--------------------------------------------------------------------------------------------
//:>	Functions to forward to the engine.
//:>------------------------------------------------------------------------------------------*/
gid16 GrTableManager::GetGlyphIDFromUnicode(int nUnicode)
{
	return m_pgreng->GetGlyphIDFromUnicode(nUnicode);
}

/*--------------------------------------------------------------------------------------------*/

gid16 GrTableManager::ActualGlyphForOutput(utf16 chw)
{
	return m_pgreng->ActualGlyphForOutput(chw);
}

/*--------------------------------------------------------------------------------------------*/

GrGlyphTable * GrTableManager::GlyphTable()
{
	return m_pgreng->GlyphTable();
}

/*--------------------------------------------------------------------------------------------*/

gid16 GrTableManager::LBGlyphID()
{
	return m_pgreng->LBGlyphID();
}

/*--------------------------------------------------------------------------------------------*/

gid16 GrTableManager::GetClassGlyphIDAt(int nClass, int nIndex)
{
	return m_pgreng->GetClassGlyphIDAt(nClass, nIndex);
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::GetIndexInGlyphClass(int nClass, gid16 chwGlyphID)
{
	return m_pgreng->GetIndexInGlyphClass(nClass, chwGlyphID);
}

/*--------------------------------------------------------------------------------------------*/

size_t GrTableManager::NumberOfGlyphsInClass(int nClass)
{
	return m_pgreng->NumberOfGlyphsInClass(nClass);
}

/*--------------------------------------------------------------------------------------------*/

void GrTableManager::SetSlotAttrsFromGlyphAttrs(GrSlotState * pslot)
{
	m_pgreng->SetSlotAttrsFromGlyphAttrs(pslot);
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::NumFeat()
{
	return m_pgreng->NumFeat();
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::DefaultForFeatureAt(int ifeat)
{
	return m_pgreng->DefaultForFeatureAt(ifeat);
}

/*--------------------------------------------------------------------------------------------*/

void GrTableManager::DefaultsForLanguage(isocode lgcode,
	std::vector<featid> & vnFeats, std::vector<int> & vnValues)
{
	return m_pgreng->DefaultsForLanguage(lgcode, vnFeats, vnValues);
}

/*--------------------------------------------------------------------------------------------*/

GrFeature * GrTableManager::FeatureWithID(featid nID, int * pifeat)
{
	return m_pgreng->FeatureWithID(nID, pifeat);
}

/*--------------------------------------------------------------------------------------------*/

GrFeature * GrTableManager::Feature(int ifeat)
{
	return m_pgreng->Feature(ifeat);
}

/*--------------------------------------------------------------------------------------------*/

bool GrTableManager::RightToLeft()
{
	if (m_engst.WhiteSpaceOnly())
		return m_engst.m_fParaRtl;
	else
		return m_pgreng->RightToLeft();
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::TopDirectionLevel()
{
	return m_pgreng->TopDirectionLevel();
}

/*--------------------------------------------------------------------------------------------*/

float GrTableManager::VerticalOffset()
{
	return m_pgreng->VerticalOffset();
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::NumUserDefn()
{
	return m_pgreng->NumUserDefn();
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::NumCompPerLig()
{
	return m_pgreng->NumCompPerLig();
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::ComponentIndexForGlyph(gid16 chwGlyphID, int nCompID)
{
	return m_pgreng->ComponentIndexForGlyph(chwGlyphID, nCompID);
}

/*--------------------------------------------------------------------------------------------*/

int GrTableManager::GlyphAttrValue(gid16 chwGlyphID, int nAttrID)
{
	return m_pgreng->GlyphAttrValue(chwGlyphID, nAttrID);
}

/*--------------------------------------------------------------------------------------------*/

bool GrTableManager::LoggingTransduction()
{
	return m_fLogging;
	//return m_pgreng->LoggingTransduction();
}

/*----------------------------------------------------------------------------------------------
	Calculate the positions of the glyphs up to the given slot, within the output of the given
	pass. If the slot is NULL, go all the way to the end of what has been generated.

	@param ipass			- index of pass needing positioning; normally this will be the
								final pass, but it could be another if positions are
								requested by the rules themselves
	@param pslotLast		- last slot that needs to be positioned, or NULL
	@param fMidPass			- calculating the position of some slot in the middle of the pass
	@param pxsWidth			- return the total width used so far
	@param psxVisibleWidth	- return the visible width so far

	MOVE to EngineState
----------------------------------------------------------------------------------------------*/
void GrTableManager::CalcPositionsUpTo(int ipass, GrSlotState * pslotLast, bool fMidPass,
	float * pxsWidth, float * pxsVisibleWidth)
{
	Assert(ipass >= m_ipassPos1 - 1);

	int isstrm = ipass;
	GrSlotStream * psstrm = OutputStream(isstrm);
	GrSlotStream * psstrmNext = (isstrm >= m_cpass - 1) ? NULL : OutputStream(isstrm + 1);
	Assert(psstrm->GotIndexOffset());
	if (psstrm->WritePos() <= psstrm->IndexOffset())
	{
		Assert(psstrm->WritePos() == 0);
		*pxsWidth = 0;
		*pxsVisibleWidth = 0;
		return;
	}
	if (!pslotLast)
	{
		pslotLast = psstrm->SlotAt(psstrm->WritePos() - 1);
	}
	Assert(pslotLast);

	//	First set positions of all base slots.

	int islot = psstrm->IndexOffset();
	GrSlotState * pslot;
	float xs = 0;
	float ys = VerticalOffset();
	*pxsWidth = 0;
	*pxsVisibleWidth = 0;

	bool fLast = false;
	bool fLastBase = false;

	bool fFakeItalic = m_pgreng->FakeItalic();
	float fakeItalicRatio = 0;
	if (fFakeItalic)
		fakeItalicRatio = State()->GetFont()->fakeItalicRatio();
	bool fBasicJust = m_pgreng->BasicJustification();

	//	Figure out how to know when to stop.
	//	We need to calculate up to the base of the last leaf slot, if it happens
	//	to be later in the stream than the last actual slot passed in.
	if (!psstrm->HasSlotAtPosPassIndex(pslotLast->AttachRootPosPassIndex()))
		return;
	GrSlotState * pslotLastBase = (fMidPass && pslotLast->PosPassIndex() < psstrm->WritePos())
		? pslotLast->Base(psstrmNext)
		: pslotLast->Base(psstrm);

	if (ipass == m_cpass - 1 && m_engst.m_islotPosNext > -1)
	{
		//	For final pass, initialize from cache of previous calculations.
		islot = m_engst.m_islotPosNext;
		xs = m_engst.m_xsPosXNext;
		ys = m_engst.m_ysPosYNext;
		*pxsWidth = m_engst.m_dxsTotWidthSoFar;
		*pxsVisibleWidth = m_engst.m_dxsVisWidthSoFar;
		
		if (psstrm->SlotsPresent() <= islot)
			return;

		if (pslotLastBase->PosPassIndex() == GrSlotAbstract::kNotYetSet)
		{
			Assert(false);	// I think this case is handled above
			return;	// can't position this slot yet; its base has not been processed
		}

		if (pslotLastBase->PosPassIndex() + psstrm->IndexOffset() < islot)
		{
			fLastBase = true;
			if (pslotLast == pslotLastBase)
				fLast = true;
		}
	}

	std::vector<GrSlotState *> vpslotAttached;
	std::vector<GrSlotStream *> vpsstrmAttached;

	bool fRtl = RightToLeft();

	while (!fLast || !fLastBase)
	{
		Assert(islot < psstrm->SlotsPresent());

		GrSlotStream * psstrmThis = psstrm;
		if (fMidPass && islot < psstrm->WritePos())
		{
			pslot = psstrm->MidPassSlotAt(islot, psstrmNext);
			psstrmThis = psstrmNext;
		}
		else
		{
			//pslot = (isstrm == ipass) ?	psstrm->SlotAt(islot) :	psstrm->OutputSlotAt(islot);
			pslot = psstrm->SlotAt(islot);
		}

		if (!pslot->IsBase())
		{
			//	This slot is attached to another; it will be positioned strictly
			//	relative to that one. This happens in the loop below.
			vpslotAttached.push_back(pslot);
			vpsstrmAttached.push_back(psstrmThis);
		}
		else
		{
			//	Base character.

			bool fLB = pslot->IsLineBreak(LBGlyphID());
			if (InternalJustificationMode() == kjmodiJustify && fBasicJust 
				&& ipass == m_cpass - 1 && !fLB)
			{
				m_engst.AddJWidthToAdvance(psstrm, &pslot, islot, &pslotLast, &pslotLastBase);
			}

			//	Make sure the metrics are the complete ones.
			pslot->CalcCompositeMetrics(this, psstrm, psstrmNext, kPosInfinity, true);

			float xsInc = pslot->GlyphXOffset(psstrm, fakeItalicRatio);
			float ysInc = pslot->GlyphYOffset(psstrm);
			float xsAdvX = pslot->ClusterAdvWidth(psstrm);
			float ysAdvY = (fLB) ?
				0 :
				EmToLogUnits(pslot->AdvanceY(this));

			if (fRtl)
			{
				xs -= xsAdvX;
				ys -= ysAdvY;
				pslot->SetXPos(xs + xsInc);
				pslot->SetYPos(ys + ysInc);
			}
			else
			{
				pslot->SetXPos(xs + xsInc);
				pslot->SetYPos(ys + ysInc);
				xs += xsAdvX;
				ys += ysAdvY;
			}

			*pxsWidth = max(*pxsWidth, fabsf(xs));
			if (!IsWhiteSpace(pslot))
				*pxsVisibleWidth = *pxsWidth;

			if (isstrm == m_cpass - 1)
			{
				//	For the final output pass, cache the results of the calculation so far.
				//	Only do this for slots that have actually been processed by the final
				//	pass, because others may have intermediate values that may be changed
				//	later. 
				m_engst.m_islotPosNext = pslot->PosPassIndex() + psstrm->IndexOffset() + 1;
				m_engst.m_xsPosXNext = xs;
				m_engst.m_ysPosYNext = ys;
				m_engst.m_dxsTotWidthSoFar = *pxsWidth;
				m_engst.m_dxsVisWidthSoFar = *pxsVisibleWidth;
			}
		}

		//	Need to have calculated both the last leaf and the last base (if they happen to
		//	be different); if so, we're done.
		if (pslot == pslotLast)
			fLast = true;
		if (pslot == pslotLastBase)
			fLastBase = true;

		islot++;
	}

	Assert(fLast);
	Assert(fLastBase);

	//	Now set positions of non-base slots, relative to their bases.

	for (size_t ipslot = 0; ipslot < vpslotAttached.size(); ipslot++)
	{
		GrSlotState * pslotAtt = vpslotAttached[ipslot];
		GrSlotStream * psstrmAtt = vpsstrmAttached[ipslot];
		GrSlotState * pslotBase = pslotAtt->Base(psstrmAtt);
		if (pslotBase->XPosition() == kNegInfinity || pslotBase->YPosition() == kNegInfinity)
		{
			Assert(false);
			continue;
		}
		float xsCluster = pslotBase->XPosition() - pslotBase->GlyphXOffset(psstrm, fakeItalicRatio);
		float ysCluster = pslotBase->YPosition() - pslotBase->GlyphYOffset(psstrm);
		float xsInc = pslotAtt->GlyphXOffset(psstrm, fakeItalicRatio);
		float ysInc = pslotAtt->GlyphYOffset(psstrm);
		pslotAtt->SetXPos(xsCluster + xsInc);
		pslotAtt->SetYPos(ysCluster + ysInc);

		//	My theory is that we don't need to adjust *pxsWidth here, because the width of
		//	any non-base slots should be factored into the advance width of their cluster
		//	base, which was handled above.
	}
}

/*----------------------------------------------------------------------------------------------
	Return true if the given slot is white space.
----------------------------------------------------------------------------------------------*/
bool GrTableManager::IsWhiteSpace(GrSlotState * pslot)
{
	if (pslot->IsLineBreak(LBGlyphID()))
		return true;
    return (pslot->IsSpace(this) != 0)? true : false;
}

/*----------------------------------------------------------------------------------------------
	When doing basic justification, apply the value of justify.width to the advance width.
	This happens during the final positioning routine.
----------------------------------------------------------------------------------------------*/
void EngineState::AddJWidthToAdvance(GrSlotStream * psstrm, GrSlotState ** ppslot, int islot,
	GrSlotState ** ppslotLast, GrSlotState ** ppslotLastBase)
{
	Assert(psstrm->OutputOfPass() == m_cpass - 1);	// only used for final stream
	if ((*ppslot)->JWidth() > 0)
	{
		if ((*ppslot)->PassModified() != m_cpass - 1)
		{
			//	Replace the slot with a new slot, so it gets marked properly with the
			//	pass in which it was modified. (Otherwise the debug log gets screwed up.)
			GrSlotState * pslotNew;
			NewSlotCopy((*ppslot), m_cpass - 1, &pslotNew);
			psstrm->PutSlotAt(pslotNew, islot);
			if (*ppslot == *ppslotLast)
				*ppslotLast = pslotNew;
			if (*ppslot == *ppslotLastBase)
				*ppslotLastBase = pslotNew;
			*ppslot = pslotNew;
		}
		(*ppslot)->AddJWidthToAdvance(TableManager());
	}
}


//:>********************************************************************************************
//:>	Debuggers and utilities
//:>********************************************************************************************

//:Ignore
#if 0
void GrTableManager::TempDebug()
{
	for (int ipass = 0; ipass < m_cpass; ++ipass)
	{
		int wpos = OutputStream(ipass)->WritePos();
		int rpos = OutputStream(ipass)->ReadPos();
		GrSlotState * wslot = OutputStream(ipass)->LastSlotWritten();
		GrSlotState * rslot = OutputStream(ipass)->LastSlotRead();

	}
}
#endif
/*----------------------------------------------------------------------------------------------
	Given a character index in the original string, locate the corresponding line-break slot
	in the final surface stream. The character index must be one before which there is a
	line-break (seg lim), or the beginning or the end of the string.
	TODO SharonC: fix the bugs
----------------------------------------------------------------------------------------------*/
int GrTableManager::SurfaceLineBreakSlot(int ichw, GrCharStream * pchstrm, bool fInitial)
{
	if (ichw == 0)
	{
		Assert(fInitial);
		return -1;	// no initial line break
	}
	if (ichw == pchstrm->Lim())
	{
		Assert(!fInitial);
		return -1;	// no terminating line break
	}

	int islotIn = pchstrm->SegOffset(ichw);
	Assert(islotIn >= 0);
	islotIn += (fInitial? m_engst.m_cslotPreSeg - 1: m_engst.m_cslotPreSeg);

	gid16 chwLBGlyphID = LBGlyphID();

	//	Now islotIn is the position in stream 0;

	for (int ipass = 1; ipass < m_cpass; ++ipass)
	{
		int islotChunkMin;
		int islotChunkLim;
		GrSlotStream * psstrmOut = OutputStream(ipass);

		GrSlotStream * psstrmIn = InputStream(ipass);
		if (fInitial)
			islotIn = max(islotIn, psstrmIn->PreChunk());
		int islotT = psstrmIn->ChunkInNextMin(islotIn);
		islotChunkMin = psstrmIn->ChunkInNext(islotT);
		islotChunkMin = (islotChunkMin == -1)? 0: islotChunkMin;

		islotT = psstrmIn->ChunkInNextLim(islotIn);
		Assert(islotT <= psstrmIn->ReadPos());
		if (islotT == psstrmIn->ReadPos())
			islotChunkLim = psstrmOut->WritePos();
		else
			islotChunkLim = psstrmIn->ChunkInNext(islotT);

		int islotOut;
		for (islotOut = islotChunkMin; islotOut < islotChunkLim; ++islotOut)
		{
			gid16 chw = psstrmOut->GlyphIDAt(islotOut);
			if (chw == chwLBGlyphID)
				break;
		}
		Assert(psstrmOut->GlyphIDAt(islotOut) == chwLBGlyphID);

		islotIn = islotOut;
	}
	return islotIn;
}

/*----------------------------------------------------------------------------------------------
	Generate a debugger string showing the chunk boundaries for the given stream.
----------------------------------------------------------------------------------------------*/
std::wstring GrTableManager::ChunkDebugString(int ipass)
{
	GrSlotStream * psstrm = OutputStream(ipass);
	std::wstring stuRet;
	//utf16 chwN = '\\';
	//utf16 chwR = '^';

	gid16 chwLBGlyphID = LBGlyphID();

	for (int islot = 0; islot < psstrm->WritePos(); islot++)
	{
		if (psstrm->ChunkInPrev(islot) != -1)
		{
			if (psstrm->ChunkInNext(islot) != -1)
				stuRet.append(L">");
			else
				stuRet.append(L"\\");
		}
		else if (psstrm->ChunkInNext(islot) != -1)
			stuRet.append(L"/");
		else
			stuRet.append(L" ");

//		if (islot == psstrm->ReadPos())
//			stuRet.Append(&chwR, 1);

		wchar_t chw = psstrm->SlotAt(islot)->GlyphID();
		if (chw == chwLBGlyphID)
			stuRet.append(L"#");
		else
			stuRet.append(&chw, 1);
	}
	return stuRet;
}

} // namespace gr

//:End Ignore
