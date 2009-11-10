/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrPass.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Implements the GrPass class and subclasses.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

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

namespace gr
{

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************
/*----------------------------------------------------------------------------------------------
	Constructors and initializers
----------------------------------------------------------------------------------------------*/
GrPass::GrPass(int i)
	:	m_ipass(i),
		m_fxdVersion(0),
		m_nMaxRuleContext(0),
		m_pfsm(NULL),
		m_nMaxRuleLoop(0),
		m_nMaxBackup(0),
		m_crul(0),
		m_prgchwRuleSortKeys(NULL),
		m_prgcritRulePreModContext(NULL),
		m_cbPassConstraint(0),
		m_prgibConstraintStart(NULL),
		m_prgibActionStart(NULL),
		m_prgbPConstraintBlock(NULL),
		m_prgbConstraintBlock(NULL),
		m_prgbActionBlock(NULL),
		m_cbConstraints(0),
		m_cbActions(0),
		m_prgibConstraintDebug(NULL),
		m_prgibRuleDebug(NULL),
		m_fCheckRules(false),
		m_prgfRuleOkay(NULL),
		m_vnStack(128),
		m_pzpst(NULL)
{
}

void PassState::InitForNewSegment(int ipass, int nMaxChunk)
{
	m_ipass = ipass;
	m_nRulesSinceAdvance = 0;
	m_nMaxChunk = nMaxChunk;
	m_cslotSkipToResync = 0;
	m_fDidResyncSkip = false;
	InitializeLogInfo();
}

void PassState::InitializeLogInfo()
{
	m_crulrec = 0;
	std::fill_n(m_rgcslotDeletions, 128, 0);
	std::fill_n(m_rgfInsertion, 128, false);
}

/*----------------------------------------------------------------------------------------------
	Destructors
----------------------------------------------------------------------------------------------*/
GrPass::~GrPass()
{
	delete m_pfsm;

	delete[] m_prgchwRuleSortKeys;

	delete[] m_prgcritRulePreModContext;

	delete[] m_prgibConstraintStart;
	delete[] m_prgibActionStart;

	delete[] m_prgbPConstraintBlock;
	delete[] m_prgbConstraintBlock;
	delete[] m_prgbActionBlock;

	delete[] m_prgfRuleOkay;

	delete[] m_prgibConstraintDebug;
	delete[] m_prgibRuleDebug;
}

/*----------------------------------------------------------------------------------------------
	Fill in the pass by reading from the font stream.
----------------------------------------------------------------------------------------------*/
bool GrPass::ReadFromFont(GrIStream & grstrm, int fxdSilfVersion, int fxdRuleVersion,
	int nOffset)
{
	long lPassInfoStart;
	grstrm.GetPositionInFont(&lPassInfoStart);

	m_fxdVersion = fxdSilfVersion;

	m_fCheckRules = (fxdRuleVersion > kRuleVersion);
	
//	Assert(nOffset == lPassInfoStart);
	if (lPassInfoStart != nOffset)
	{
		grstrm.SetPositionInFont(nOffset);
	}

	//	flags - ignore for now
	//byte bTmp = grstrm.ReadByteFromFont();
    grstrm.ReadByteFromFont();

	//	MaxRuleLoop
	m_nMaxRuleLoop = grstrm.ReadByteFromFont();

	//	max rule context
	m_nMaxRuleContext = grstrm.ReadByteFromFont();

	//	MaxBackup
	m_nMaxBackup = grstrm.ReadByteFromFont();

	//	number of rules
	m_crul = grstrm.ReadShortFromFont();
	// TODO: add a sanity check for the number of rules.

	//	offset to pass constraint code, relative to start of subtable
	int nPConstraintOffset = 0;
	long lFsmPos = -1;
	if (fxdSilfVersion >= 0x00020000)
	{
		if (fxdSilfVersion >= 0x00030000)
			lFsmPos = grstrm.ReadUShortFromFont() + nOffset; // offset to row info
		else
			grstrm.ReadShortFromFont();	// pad bytes
		nPConstraintOffset = grstrm.ReadIntFromFont();
	}
	//	offset to rule constraint code, relative to start of subtable
	//int nConstraintOffset = grstrm.ReadIntFromFont();
    grstrm.ReadIntFromFont();
	//	offset to action code, relative to start of subtable
	//int nActionOffset = grstrm.ReadIntFromFont();
    grstrm.ReadIntFromFont();
	//	offset to debug strings; 0 if stripped
	//int nDebugOffset = grstrm.ReadIntFromFont();
    grstrm.ReadIntFromFont();
	
	//	Jump to beginning of FSM, if we have this information.
	if (fxdSilfVersion >= 0x00030000)
		grstrm.SetPositionInFont(lFsmPos);
	else
		// Otherwise assume that's where we are!
		Assert(lFsmPos == -1);

	m_pfsm = new GrFSM();

	m_pfsm->ReadFromFont(grstrm, fxdSilfVersion);

	//	rule sort keys
	m_prgchwRuleSortKeys = new data16[m_crul];
	data16 * pchw = m_prgchwRuleSortKeys;
	int irul;
	for (irul = 0; irul < m_crul; irul++, pchw++)
	{
		*pchw = grstrm.ReadUShortFromFont();
	}

	//	rule pre-mod-context item counts
	m_prgcritRulePreModContext = new byte[m_crul];
	byte * pb = m_prgcritRulePreModContext;
	for (irul = 0; irul < m_crul; irul++, pb++)
	{
		*pb = grstrm.ReadByteFromFont();
	}

	//	constraint offset for pass-level constraints
	if (fxdSilfVersion >= 0x00020000)
	{
		// reserved - pad byte
		grstrm.ReadByteFromFont();
		// Note: pass constraints have not been fully implemented.
		m_cbPassConstraint = grstrm.ReadUShortFromFont();
	}
	else
		m_cbPassConstraint = 0;

	//	constraint and action offsets for rules
	m_prgibConstraintStart = new data16[m_crul + 1];
	pchw = m_prgibConstraintStart;
	for (irul = 0; irul <= m_crul; irul++, pchw++)
	{
		*pchw = grstrm.ReadUShortFromFont();
	}

	m_prgibActionStart = new data16[m_crul + 1];
	pchw = m_prgibActionStart;
	for (irul = 0; irul <= m_crul; irul++, pchw++)
	{
		*pchw = grstrm.ReadUShortFromFont();
	}

	//	FSM state table
	m_pfsm->ReadStateTableFromFont(grstrm, fxdSilfVersion);

	if (fxdSilfVersion >= 0x00020000)
		// reserved - pad byte
		grstrm.ReadByteFromFont();

	//	Constraint and action blocks
	int cb = m_cbPassConstraint;
	m_prgbPConstraintBlock = new byte[cb];
	grstrm.ReadBlockFromFont(m_prgbPConstraintBlock, cb);
	m_cbConstraints = cb;

	cb = m_prgibConstraintStart[m_crul];
	m_prgbConstraintBlock = new byte[cb];
	grstrm.ReadBlockFromFont(m_prgbConstraintBlock, cb);
	m_cbConstraints += cb;

	cb = m_prgibActionStart[m_crul];
	m_prgbActionBlock = new byte[cb];
	grstrm.ReadBlockFromFont(m_prgbActionBlock, cb);
	m_cbActions = cb;

	//	Rule-validity flags
	m_prgfRuleOkay = new bool[m_crul];
	for (irul = 0; irul < m_crul; irul++)
		m_prgfRuleOkay[irul] = !m_fCheckRules;

	//	TODO SharonC/AlanW: debuggers

	return true;
}

/*----------------------------------------------------------------------------------------------
	Initialize a bogus pass with no rules. Currently this is used to make a single positioning
	pass if there was none in the font.
----------------------------------------------------------------------------------------------*/
void GrPass::InitializeWithNoRules()
{
	m_crul = 0;
	m_nMaxRuleContext = 1;
	m_nMaxRuleLoop = 1;
	m_nMaxBackup = 0;
	m_pfsm = NULL;
	m_prgchwRuleSortKeys = NULL;
	m_prgcritRulePreModContext = NULL;
}

/*----------------------------------------------------------------------------------------------
	Extend the output stream by the given number of characters.
	(Overridden by GrBidiPass.)

	@param ptman				- table manager
	@param psstrmIn/Out			- streams being processed
	@param cslotNeededByNext	- the number of slots being requested by the following pass
	@param twsh					- how we are handling trailing white-space
	@param pnRet				- return value
	@param pcslotGot			- return the number of slots gotten
	@param pislotFinalBreak		- return the index of the final slot, when we are removing
									the trailing white-space and so the end of the segment
									will be before the any actual line-break slot

	@return kNextPass if we were able to generated the number requested, or processing is
		complete; otherwise return the number of slots needed from the previous pass.
----------------------------------------------------------------------------------------------*/
void GrPass::ExtendOutput(GrTableManager * ptman,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	int cslotNeededByNext, TrWsHandling twsh,
	int * pnRet, int * pcslotGot, int * pislotFinalBreak)
{
	// Kludge to generate an error in rendering:
	//int z = 50;
	//int t = ((5 * 10) / z) - 1;
	//if (this->m_ipass == 4)
	//{
	//	int x; x = z / t;
	//}

	int islotInitReadPos = psstrmIn->ReadPos();
	int islotInitWritePos = psstrmOut->WritePos();

	int cslotToGet = max(cslotNeededByNext,
		m_pzpst->NeededToResync() - psstrmOut->WritePos());
	int cslotGot = 0;

	gid16 chwLB = ptman->LBGlyphID();

	//	While we haven't got the number of slot we've been asked for, and there is enough
	//	available in the input, run rules, filling up the output.

	while ((cslotToGet > 0 && cslotGot < cslotToGet)
			//	Don't leave this pass until we've got the index offset. This is especially
			//	needed for input to the positioning pass, but it doesn't hurt to make it a
			//	general rule.
			|| !psstrmOut->GotIndexOffset()
			//	Don't leave this pass until all the slots to reprocess have been
			//	completely handled.
			|| (psstrmIn->SlotsToReprocess() > 0)
			//	Don't leave this pass until there is a complete cluster in the output
			|| (islotInitWritePos == psstrmOut->WritePos())
			|| (psstrmOut->MaxClusterSlot(islotInitWritePos, psstrmOut->WritePos()) > 0))
	{
		int cslotAvailable = psstrmIn->TotalSlotsPending();
		int cslotNeedMore = MaxRuleContext() - cslotAvailable + ptman->PrevPassMaxBackup(m_ipass);

		//	This test should not be necessary, but just in case.
		if (psstrmIn->PastEndOfPositioning(false))
			cslotNeedMore = 0; // the input stream is done

		if (//	Not enough available in the input:
			(cslotNeedMore > 0 && !psstrmIn->FullyWritten())

			// && psstrmIn->SegLimIfKnown() == -1) && -- no, because substitution passes may need
			//										to consider slots beyond the seg limit

			//	Positioning passes need to know where they are in relation to the initial
			//	line-break.
			|| (IsPosPass() && !psstrmIn->GotIndexOffset())

			//	The following can happen on backtracking, when we undo stuff before the
			//	beginning of the segment:
			|| !ptman->Pass(m_ipass - 1)->DidResyncSkip())
		{
			//	Ask previous pass for more input.
			*pnRet = max(cslotNeedMore, 1);
			*pnRet = max(*pnRet, cslotNeededByNext - cslotGot);
			*pcslotGot = cslotGot;
			return;
		}

		Assert(ptman->Pass(m_ipass - 1)->DidResyncSkip());

		bool fDoneThisPass = (cslotAvailable == 0);
		if (psstrmIn->SlotsToReprocess() == 0)
		{
			fDoneThisPass = fDoneThisPass ||
				psstrmIn->PastEndOfPositioning(false) ||
				psstrmOut->PastEndOfPositioning(true);
		}

		if (fDoneThisPass)
		{
			//	No more input to process--this pass is done.
			Assert(psstrmIn->SlotsToReprocess() == 0);
			psstrmIn->ClearReprocBuffer();

			if (twsh == ktwshNoWs && m_ipass == ptman->NumberOfLbPasses())
			{
				*pnRet = RemoveTrailingWhiteSpace(ptman, psstrmOut, twsh, pislotFinalBreak);
				if (*pnRet == kBacktrack)	// entire segment was white-space: 
					return;					// backtrack, which will fail
			}
			DoResyncSkip(psstrmOut);
			DoCleanUpSegMin(ptman, psstrmIn, islotInitReadPos, psstrmOut);
			DoCleanUpSegLim(ptman, psstrmOut, twsh);
			psstrmOut->MarkFullyWritten();
			*pnRet = kNextPass;
			*pcslotGot = cslotGot;
			return;
		}

		if (twsh == ktwshOnlyWs && m_ipass == ptman->NumberOfLbPasses() + 1)
		{
			//	Note that this is the first pass after the linebreak pass, so psstrmInput
			//	is the output of the linebreak pass.
			GrSlotState * pslotNext = psstrmIn->Peek();
			if (!pslotNext->IsLineBreak(chwLB) &&
				pslotNext->Directionality() != kdircWhiteSpace &&
				psstrmIn->SegMin() > -1 && psstrmIn->SegMin() <= psstrmIn->ReadPos())
			{
				//	We are only allowing white-space in this segment and we hit
				//	something else. Don't process any further.
				if (psstrmIn->SegLimIfKnown() > -1 &&
					psstrmIn->SegLimIfKnown() <= psstrmIn->ReadPos())
				{
					//	Already inserted a line-break; we're done.
					DoResyncSkip(psstrmOut);
					psstrmOut->MarkFullyWritten();
					*pnRet = kNextPass;
					*pcslotGot = cslotGot;
					return;
				}
				while (psstrmIn->SlotsToReprocess() > 0)
				{
					psstrmOut->CopyOneSlotFrom(psstrmIn);
					psstrmOut->SetPosForNextRule(0, psstrmIn, IsPosPass());
				}
				psstrmIn->ClearReprocBuffer();
				*pnRet = kBacktrack;
				return;
			}
		}

		//	Otherwise, we have enough input to run a rule.

		psstrmIn->SetRuleStartReadPos();
		psstrmOut->SetRuleStartWritePos();

		int ruln = -1;
		if (m_pfsm)
			ruln = m_pfsm->GetRuleToApply(ptman, this, psstrmIn, psstrmOut);
		ruln = CheckRuleValidity(ruln);
		RunRule(ptman, ruln, psstrmIn, psstrmOut);

		cslotGot = psstrmOut->WritePos() - islotInitWritePos;
		psstrmOut->CalcIndexOffset(ptman);
	}

	DoResyncSkip(psstrmOut);
	DoCleanUpSegMin(ptman, psstrmIn, islotInitReadPos, psstrmOut);

	//	We're past the point where we care about anything in the reprocessing buffer.
	Assert(psstrmIn->NoReproc());
	psstrmIn->ClearReprocBuffer();

	if (psstrmOut->PastEndOfPositioning(true))
	{
		DoCleanUpSegLim(ptman, psstrmOut, twsh);
		psstrmOut->MarkFullyWritten();
	}

	*pnRet = kNextPass;
	*pcslotGot = cslotGot;
}

/*----------------------------------------------------------------------------------------------
	Extend the output stream by the given number of characters.
	For a GrBidiPass, if we are at a direction change, get the entire
	range of upstream glyphs, reverse them, and treat them as one chunk.
	Otherwise just pass one slot through.

	@param ptman				- table manager
	@param psstrmInput/Output	- streams being processed
	@param cslotNeededByNext	- the number of slots being requested by the following pass
	@param twsh					- how we are handling trailing white-space
	@param pnRet				- return value
	@param pcslotGot			- return the number of slots gotten
	@param pislotFinalBreak		- not used in this version

	@return 1 if we need more glyphs from the previous pass
----------------------------------------------------------------------------------------------*/
void GrBidiPass::ExtendOutput(GrTableManager * ptman,
	GrSlotStream* psstrmIn, GrSlotStream* psstrmOut,
	int cslotNeededByNext, TrWsHandling twsh,
	int * pnRet, int * pcslotGot, int * pislotFinalBreak)
{
	Assert(psstrmIn->SlotsToReprocess() == 0);

	int islotInitReadPos = psstrmIn->ReadPos();
	int islotInitWritePos = psstrmOut->WritePos();

	Assert(m_pzpst->NeededToResync() == 0);

	int cslotToGet = max(cslotNeededByNext,
		m_pzpst->NeededToResync() - psstrmOut->WritePos());
	int cslotGot = 0;
	int nTopDir;
	if (twsh == ktwshOnlyWs)
		nTopDir = (ptman->State()->ParaRightToLeft()) ? 1 : 0;
	else
		nTopDir = ptman->TopDirectionLevel();

	while (cslotToGet > 0 && cslotGot < cslotToGet)
	{
		int islotChunkO = psstrmOut->WritePos();
		int islotChunkI = psstrmIn->ReadPos();

		//	Need at least one character to test.
		if (psstrmIn->SlotsPending() < 1 || !ptman->Pass(m_ipass-1)->DidResyncSkip())
		{
			if (!psstrmIn->FullyWritten())
			{
				//	Ask previous pass for more input.
				*pnRet = max(cslotToGet - psstrmIn->SlotsPending(), 1);
				*pnRet = max(*pnRet, cslotNeededByNext - cslotGot);
				*pcslotGot = cslotGot;
				return;
			}
			else
			{
				Assert(ptman->Pass(m_ipass-1)->DidResyncSkip());
				Assert(psstrmIn->SlotsToReprocess() == 0);
				psstrmIn->ClearReprocBuffer();
				DoResyncSkip(psstrmOut);
				DoCleanUpSegMin(ptman, psstrmIn, islotInitReadPos, psstrmOut);
				DoCleanUpSegLim(ptman, psstrmOut, twsh);
				psstrmOut->MarkFullyWritten();
				*pnRet = kNextPass;
				*pcslotGot = cslotGot;
				return;
			}
		}

		std::vector<int> vislotStarts;
		std::vector<int> vislotStops;
		int islotReverseLim = psstrmIn->DirLevelRange(ptman->State(),
			psstrmIn->ReadPos(), nTopDir,
			vislotStarts, vislotStops);
		//int islotReverseLim = psstrmIn->OldDirLevelRange(pengst, psstrmIn->ReadPos(), nTopDir);
		if (islotReverseLim == -1)
		{
			//	We haven't got the full range of reversed text yet--
			//	ask for more input.
			*pnRet = max(1, cslotNeededByNext - cslotGot);
			*pcslotGot = cslotGot;
			return;
		}

		//	Okay, we have enough input to do the reversal, if any.

		int cslotToReverse = islotReverseLim - psstrmIn->ReadPos();

		//	Never reverse the final linebreak; leave it at the end.
		if (cslotToReverse > 0 && islotReverseLim > 0)
		{
			GrSlotState * pslotLast = psstrmIn->SlotAt(islotReverseLim - 1);
			if (pslotLast->IsFinalLineBreak(ptman->LBGlyphID()))
			{
				for (size_t i = 0; i < vislotStops.size(); i++)
					if (vislotStops.back() == islotReverseLim - 1)
						vislotStops.back() = islotReverseLim - 2;
				islotReverseLim--;
				cslotToReverse--;
			}
		}

		psstrmIn->SetRuleStartReadPos();
		psstrmOut->SetRuleStartWritePos();

		if (cslotToReverse == 0)
		{
			psstrmOut->CopyOneSlotFrom(psstrmIn);
		}
		else
		{
			int islotNextWritePos = psstrmOut->WritePos() + cslotToReverse;
			int islotNextReadPos = psstrmIn->ReadPos() + cslotToReverse;
			if (vislotStarts.size() == 0)
			{
				Assert(false); // this should have been done by DirLevelRange
				vislotStarts.push_back(psstrmIn->ReadPos());
				vislotStops.push_back(islotNextReadPos - 1);
			}
			Assert(vislotStarts.back() == psstrmIn->ReadPos());
			Assert(vislotStops.back() == islotNextReadPos - 1);
			int cslotNotCopied = Reverse(ptman, psstrmIn, psstrmOut, vislotStarts, vislotStops);
			//Reverse(nTopDir + 1,
			//	psstrmIn, psstrmIn->ReadPos(), islotReverseLim,
			//	psstrmOut, psstrmOut->WritePos() + cslotToReverse - 1, psstrmOut->WritePos()-1);

			islotNextWritePos -= cslotNotCopied; // bidi markers that are not passed through

			psstrmIn->SetReadPos(islotNextReadPos);
			psstrmOut->SetWritePos(islotNextWritePos);

			//	It's quite strange to have the segment start or end in the middle of stuff to
			//	reverse (because the LB forms a natural terminator), but at any rate,
			//	if that happens, record the segment lim at the corresponding place in the
			//	output stream.
			int islotSegMinIn = psstrmIn->SegMin();
			if (islotSegMinIn > -1 &&
				psstrmIn->ReadPos() - cslotToReverse <= islotSegMinIn &&
				islotSegMinIn < psstrmIn->ReadPos())
			{
				Assert(islotSegMinIn == psstrmIn->ReadPos() - cslotToReverse); // normal situation
				psstrmOut->SetSegMin(
					psstrmOut->WritePos() - (psstrmIn->ReadPos() - islotSegMinIn));
			}
			int islotSegLimIn = psstrmIn->SegLimIfKnown();
			if (islotSegLimIn > -1 &&
				psstrmIn->ReadPos() - cslotToReverse <= islotSegLimIn &&
				islotSegLimIn < psstrmIn->ReadPos())
			{
				Assert(islotSegLimIn == psstrmIn->ReadPos() - cslotToReverse); // normal situation
				psstrmOut->SetSegLim(
					psstrmOut->WritePos() - (psstrmIn->ReadPos() - islotSegLimIn));
			}
		}

		psstrmOut->SetPosForNextRule(0, psstrmIn, false);

		//	Record the chunk mappings:
		MapChunks(psstrmIn, psstrmOut, islotChunkI, islotChunkO, 0);

		cslotGot = psstrmOut->WritePos() - islotInitWritePos;
		psstrmOut->CalcIndexOffset(ptman);
	}

	DoResyncSkip(psstrmOut);
	DoCleanUpSegMin(ptman, psstrmIn, islotInitReadPos, psstrmOut);

	//	We're past the point where we care about anything in the reprocessing buffer.
	Assert(psstrmIn->NoReproc());
	psstrmIn->ClearReprocBuffer();

	Assert(psstrmIn->SlotsToReprocess() == 0);

	if (psstrmOut->PastEndOfPositioning(true))
	{
		DoCleanUpSegLim(ptman, psstrmOut, twsh);
		psstrmOut->MarkFullyWritten();
	}

	*pnRet = kNextPass;
	*pcslotGot = cslotGot;
}

/*----------------------------------------------------------------------------------------------
	Generate slots containing glyph IDs for the underlying character data,
	incrementing the input pointer as we go.

	@param ptman				- table manager
	@param pchstrm				- input character stream
	@param psstrmOutput			- output slot stream
	@param ichSegLim			- known end of segment, index into the text; or -1;
									the lim of the of char-stream itself represents
									the end of the text-source; this lim is the
									known desired end of the segment
	@param cchwPostXlbContext	- number of characters that may be needed from the following line;
									valid when ichSegLim > -1
	@param lb					- breakweight to use for inserted LB
	@param cslotToGet			- the number of slots being requested by the following pass
	@param fNeedFinalBreak		- true if the end of the segment needs to be a valid break point
	@param pislotFinalBreak		- the end of this segment
----------------------------------------------------------------------------------------------*/
int GrPass::ExtendGlyphIDOutput(GrTableManager * ptman,
	GrCharStream * pchstrm, GrSlotStream * psstrmOut, int ichSegLim, int cchwPostXlbContext,
	LineBrk lb, int cslotToGet, bool fNeedFinalBreak, TrWsHandling twsh,
	int * pislotFinalBreak)
{
	EngineState * pengst = ptman->State();

	//	This pass should be the glyph-generation pass.
	Assert(dynamic_cast<GrGlyphGenPass*>(this));
	Assert(m_pzpst->m_cslotSkipToResync == 0);
	m_pzpst->m_fDidResyncSkip = true;

	for (int islot = 0; islot < cslotToGet; ++islot)
	{
		int islotChunkO = psstrmOut->WritePos();
		int islotChunkI = pchstrm->SegOffset();

		if (pchstrm->AtEnd()
			|| (ichSegLim > -1 && pchstrm->Pos() == ichSegLim))
		{
			if (psstrmOut->SegLimIfKnown() > -1 &&
				psstrmOut->SegLimIfKnown() <= psstrmOut->WritePos())
			{
				//	Already found the end of this stream.
			}
			else
			{
				if (pchstrm->EndLine() && !fNeedFinalBreak)
						// (if we need a good final break, don't just append an LB;
						// make it backtrack and find a valid break point)
				{
					//	Only need to get a good break when we're backtracking; otherwise we
					//	already know it.
					Assert(ichSegLim == -1 || !fNeedFinalBreak);
					//	Notice that we're cheating here: we're putting the LB in the zeroth
					//	stream instead of in the output of the linebreak table.
					psstrmOut->AppendLineBreak(ptman, pchstrm,
						(pchstrm->AtEnd() ? klbWordBreak : lb),
						((ptman->RightToLeft()) ? kdircRlb : kdircLlb), -1, false,
						pchstrm->SegOffset());
					if (pchstrm->AtEnd())
						pengst->SetFinalLB();
					else
						pengst->SetInsertedLB(true);
				}
				else
				{
					//	Don't actually insert a line-break glyph.
					psstrmOut->SetSegLimToWritePos();
				}
				*pislotFinalBreak = psstrmOut->WritePos() - 1;
				if (ptman->NumberOfLbPasses() > 0 && pengst->HasInitialLB())
				{
					//	Because we cheated above: the output stream of the linekbreak table
					//	has an initial LB, which this stream doesn't have. Adjust the position
					//	of the final break to match what it will be in the output of the
					//	lb table.
					*pislotFinalBreak += 1;
				}
			}

			if (twsh == ktwshNoWs && m_ipass == ptman->NumberOfLbPasses())
			{
				int nRet = RemoveTrailingWhiteSpace(ptman, psstrmOut, twsh, pislotFinalBreak);
				if (nRet == kBacktrack)		// entire segment was white-space: 
					return kBacktrack;		// backtrack, which will fail
			}
			if (pchstrm->AtEnd())
			{
				psstrmOut->MarkFullyWritten();
				return kNextPass;
			}
			// otherwise we may need a few more characters for line-boundary contextualization.
		}

		int ichwSegOffset;	// offset from the official start of the segment
		int cchw; // number of 16-bit chars consumed
		GrFeatureValues fval;
		int nUnicode = pchstrm->NextGet(ptman, &fval, &ichwSegOffset, &cchw);
		gid16 chwGlyphID = ptman->GetGlyphIDFromUnicode(nUnicode);

		if (nUnicode == knCR || nUnicode == knLF || nUnicode == knLineSep || nUnicode == knParaSep ||
			nUnicode == knORC)
		{
			//	Hard line-break: we're done.
			//	Note that we don't include the hard-break character in this segment.
			pchstrm->HitHardBreak();
			pengst->SetHitHardBreak();
			psstrmOut->MarkFullyWritten();
			return kNextPass;
		}

		GrSlotState * pslotNew;
		ptman->State()->NewSlot(chwGlyphID, fval, 0, ichwSegOffset, nUnicode, &pslotNew);

		psstrmOut->NextPut(pslotNew);
		psstrmOut->MapInputChunk(islotChunkI, islotChunkO, pchstrm->SegOffset(), false, false);
		// Mapping the output chunks of the char stream has already been handled by the
		// char stream.

	}

	psstrmOut->CalcIndexOffset(ptman);

	if (psstrmOut->PastEndOfPositioning(true)
		|| (ichSegLim > -1 && pchstrm->Pos() > ichSegLim + cchwPostXlbContext))
	{
		// We have enough for this segment.
		psstrmOut->MarkFullyWritten();
	}

	return kNextPass;
}

/*----------------------------------------------------------------------------------------------
	Extend the output stream until it is using up the allotted amount of
	physical space. Return kNextPass when all data has been processed successfully.
	Return kBacktrack when space overflows, indicating that we need to backtrack
	and find a break point. Otherwise, return the number of slots we
	need from the previous pass.

	@param fWidthIsCharCount	- kludge for test procedures
	@param fInfiniteWidth		- don't test for more space
	@param fMustBacktrack		- true if we need a good final break and haven't found one yet
	@param lbMax				- max allowed for the final slot
	@param twsh					- how we are handling trailing white-space
----------------------------------------------------------------------------------------------*/
int GrPass::ExtendFinalOutput(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput,
	float xsSpaceAllotted, bool fWidthIsCharCount, bool fInfiniteWidth,
	bool fHaveLineBreak, bool fMustBacktrack, LineBrk lbMax, TrWsHandling twsh,
	int * pislotLB, float * pxsWidth)
{
	EngineState * pengst = ptman->State();

	//	This pass should be the final positioning pass.
	Assert(dynamic_cast<GrPosPass*>(this));

	//	It would be very strange to be positioning based on something that happened in
	//	previous line. -- not true any more, since this will include the max-precontext-length.
//	Assert(m_cslotSkipToResync == 0);

	int islotOutputLB = -1;
	//int islotInitWritePos = psstrmOutput->WritePos();
	int islotNoLbUpTo = psstrmOutput->WritePos();;

	while (true)
	{
		// Do this right up front, so we are only measuring actual output.
		if (m_pzpst->CanResyncSkip(psstrmOutput))
			m_pzpst->DoResyncSkip(psstrmOutput);

		bool fMoreSpace;
		if (!m_pzpst->DidResyncSkip())
			fMoreSpace = true;
		else if (fInfiniteWidth)
			fMoreSpace = true;
		else
			fMoreSpace = psstrmOutput->MoreSpace(ptman,
				xsSpaceAllotted, fWidthIsCharCount,
				true,	// always ignore trailing white space when we are first making the segment
				twsh,
				pxsWidth);
		if (!fMoreSpace)
		{
			//	Overflowed available space; backtrack and find a line break.
			while (psstrmInput->SlotsToReprocess() > 0)
			{
				psstrmOutput->CopyOneSlotFrom(psstrmInput);
				psstrmOutput->SetPosForNextRule(0, psstrmInput, IsPosPass());
			}
			psstrmInput->ClearReprocBuffer();

			*pislotLB = -1;
			pengst->SetExceededSpace();
			pengst->SetHitHardBreak(false);
			return kBacktrack;
		}

		if (islotOutputLB != -1 && psstrmInput->SlotsToReprocess() == 0)
		{
			//	Hit the inserted line break--we're done.
			psstrmInput->ClearReprocBuffer();
			*pislotLB = islotOutputLB;
			return kNextPass;
		}

		int nslotAvailable = psstrmInput->SlotsPending();
		if ((nslotAvailable - ptman->PrevPassMaxBackup(m_ipass)
				< MaxRuleContext() && !psstrmInput->FullyWritten())
			|| !ptman->Pass(m_ipass-1)->DidResyncSkip())
		{
			//	Not enough available in the input--ask previous pass for more input.
			//	Ten is an arbitrary value--we ask for more that we really need to cut down
			//	on the number of times we loop between passes.
			return max(MaxRuleContext() - (nslotAvailable - 10), 1);
		}

		Assert(ptman->Pass(m_ipass - 1)->DidResyncSkip());

		if (nslotAvailable == 0)
		{
			//	No more input to process. If we have a valid line-break, or we don't care,
			//	we're done. Otherwise backtrack to find a valid break point.
			Assert(psstrmInput->SlotsToReprocess() == 0);
			psstrmInput->ClearReprocBuffer();
			if (fMustBacktrack)
			{
				*pislotLB = -1;
				return kBacktrack;
			}
			else
			{
				psstrmOutput->MarkFullyWritten();
				return kNextPass;
			}
		}
		
		//	Otherwise, we have enough input to run a rule.

		psstrmInput->SetRuleStartReadPos();
		psstrmOutput->SetRuleStartWritePos();

		int ruln = -1;
		if (m_pfsm)
			ruln = m_pfsm->GetRuleToApply(ptman, this, psstrmInput, psstrmOutput);
		ruln = CheckRuleValidity(ruln);
		RunRule(ptman, ruln, psstrmInput, psstrmOutput);

		if (fHaveLineBreak)
		{
			islotOutputLB =
				psstrmOutput->FindFinalLineBreak(ptman->LBGlyphID(),
					islotNoLbUpTo, psstrmOutput->WritePos());
			islotNoLbUpTo = psstrmOutput->WritePos();
		}

		psstrmOutput->CalcIndexOffset(ptman);
	}

	Assert(false);

	psstrmInput->ClearReprocBuffer();
	*pislotLB = -1;
	return kNextPass;
}

/*----------------------------------------------------------------------------------------------
	Remove undesirable trailing white-space.
----------------------------------------------------------------------------------------------*/
int GrPass::RemoveTrailingWhiteSpace(GrTableManager * ptman, GrSlotStream * psstrmOut,
	TrWsHandling twsh, int * pislotFinalBreak)
{
	EngineState * pengst = ptman->State();

	Assert(twsh == ktwshNoWs);
	Assert(m_ipass == ptman->NumberOfLbPasses());	// output of (final) lb pass

	int islotTmp = psstrmOut->FinalSegLim();
	if (islotTmp <= 0)
		return kNextPass;

	GrSlotState * pslotLast = psstrmOut->SlotAt(islotTmp-1);
	if (islotTmp > 0 && pslotLast->IsFinalLineBreak(ptman->LBGlyphID()))
	{
		islotTmp--;
		pslotLast = (islotTmp > 0) ? psstrmOut->SlotAt(islotTmp-1) : NULL;
	}
	bool fRemovedWs = false;
	while (islotTmp > 0 && pslotLast->Directionality() == kdircWhiteSpace)
	{
		islotTmp--;
		pslotLast = (islotTmp > 0) ? psstrmOut->SlotAt(islotTmp-1) : NULL;
		fRemovedWs = true;
	}
	if (fRemovedWs)
	{
		if (islotTmp <= 0)
		{
			//	Entire segment was white-space: backtrack, which will fail.
			return kBacktrack;
		}
		psstrmOut->SetSegLim(islotTmp);
		*pislotFinalBreak = islotTmp - 1;
		pengst->SetFinalLB(false);
		pengst->SetRemovedTrWhiteSpace();
		ptman->UnwindAndReinit(islotTmp - 1);
	}

	return kNextPass;
}

/*----------------------------------------------------------------------------------------------
	Keep track of whether we're advancing through the input satisfactorily;
	if not, forcibly advance. This is a safety net to avoid infinite loops; it
	should never be necessary if they've set up their tables right.
----------------------------------------------------------------------------------------------*/
void GrPass::CheckInputProgress(GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput,
	int islotOrigInput)
{
	int islotInput = psstrmInput->ReadPosForNextGet();
//	Assert(islotInput >= islotOrigInput); -- no longer true now that we can back up

	if (islotInput <= psstrmInput->ReadPosMax())
	{
		//	Didn't advance.
		if (m_pzpst->m_nRulesSinceAdvance >= m_nMaxRuleLoop)
		{
			bool fAdvanced = false;
			//	Forcibly advance. First try to advance to where we backed up from.
			while (!psstrmInput->AtEnd() &&
				psstrmInput->ReadPosForNextGet() < psstrmInput->ReadPosMax())
			{
				RecordHitMaxRuleLoop(psstrmInput->ReadPosForNextGet());
				psstrmOutput->CopyOneSlotFrom(psstrmInput);
				fAdvanced = true;
			}
			// If that didn't do anything productive, just advance one slot.
			if (!fAdvanced && !psstrmInput->AtEndOfContext())
			{
				RecordHitMaxRuleLoop(psstrmInput->ReadPosForNextGet());
				psstrmOutput->CopyOneSlotFrom(psstrmInput);
			}

			m_pzpst->m_nRulesSinceAdvance = 0;
		}
		else m_pzpst->m_nRulesSinceAdvance++;
	}
	else m_pzpst->m_nRulesSinceAdvance = 0;

	psstrmInput->SetReadPosMax(islotInput);
}

/*----------------------------------------------------------------------------------------------
	Record the chunks in the streams' chunk maps.

	psstrmIn/Out		- streams being processed
	islotChunkIn/Out	- start of chunks
	cslotReprocessed	- number of slots to reprocess before the rule was run
----------------------------------------------------------------------------------------------*/
void GrPass::MapChunks(GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	int islotChunkIn, int islotChunkOut, int cslotReprocessed)
{
	if (islotChunkOut > psstrmOut->WritePos())
	{
		//	backing up
		int islotReadPosTmp = psstrmIn->ReadPosForNextGet();
///		Assert((islotChunkIn - islotReadPosTmp) == (islotChunkOut - psstrmOut->WritePos()));
//		psstrmIn->MapOutputChunk(psstrmOut->WritePos() - 1, islotReadPosTmp - 1,
//			islotChunkOut, true, 0, true);
//		psstrmOut->MapInputChunk(islotReadPosTmp - 1, psstrmOut->WritePos() - 1,
//			islotChunkIn, true, true);

		//	Resync.
		if (psstrmOut->WritePos() == 0)
		{
			//	Clear all the chunks.
			psstrmIn->MapOutputChunk(-1, -1, 0, true, 0, true);
			psstrmOut->MapInputChunk(-1, -1, islotReadPosTmp, true, true);
			psstrmIn->AssertChunkMapsValid(psstrmOut);
			return;
		}
		else if (islotReadPosTmp == 0)
		{
			//	Clear all the chunks.
			psstrmIn->MapOutputChunk(-1, -1, psstrmOut->WritePos(), true, 0, true);
			psstrmOut->MapInputChunk(-1, -1, 0, true, true);
			psstrmIn->AssertChunkMapsValid(psstrmOut);
			return;
		}
		// Find the beginning of the current chunk.
		int islotChunkOutAdj = min(islotChunkOut, psstrmOut->WritePos() - 1);
		int islotChunkInAdj = psstrmOut->ChunkInPrev(islotChunkOutAdj);
		while (islotChunkInAdj == -1 && islotChunkOutAdj > 0)
			islotChunkInAdj = psstrmOut->ChunkInPrev(--islotChunkOutAdj);

		if (islotChunkInAdj == -1)
		{
			// Couldn't find the beginning of any chunk; zap them all.
			psstrmIn->MapOutputChunk(-1, -1, psstrmOut->WritePos(), true, 0, true);
			psstrmOut->MapInputChunk(-1, -1, psstrmOut->ReadPos(), true, true);
			psstrmIn->AssertChunkMapsValid(psstrmOut);
			return;
		}

		if (psstrmIn->ChunkInNext(islotChunkInAdj) != islotChunkOutAdj)
		{
			islotChunkOutAdj = psstrmIn->ChunkInNext(islotChunkInAdj);
			while (islotChunkOutAdj == -1 && islotChunkInAdj > 0)
				islotChunkOutAdj = psstrmIn->ChunkInNext(--islotChunkInAdj);
		}

		psstrmIn->MapOutputChunk(islotChunkOutAdj, islotChunkInAdj,
			psstrmOut->WritePos(), false, 0, true);
		psstrmOut->MapInputChunk(islotChunkInAdj, islotChunkOutAdj,
			psstrmIn->ReadPos(), false, true);
	}
	else if (islotChunkOut == psstrmOut->WritePos())
		// no output generated--continue the previous chunk
		;
	else if (islotChunkIn == psstrmIn->ReadPos())
		// no input consumed
		;
	else
	{
		psstrmIn->MapOutputChunk(islotChunkOut, islotChunkIn, psstrmOut->WritePos(),
			(cslotReprocessed > 0), cslotReprocessed, false);
		psstrmOut->MapInputChunk(islotChunkIn, islotChunkOut, psstrmIn->ReadPos(),
			(cslotReprocessed > 0), false);
	}

	psstrmIn->AssertChunkMapsValid(psstrmOut);

	m_pzpst->m_nMaxChunk = max(m_pzpst->m_nMaxChunk, psstrmIn->LastNextChunkLength());
}

/*----------------------------------------------------------------------------------------------
	This method is something of a kludge. During the course of running the rules we try to
	keep track of the seg-min location, but sometimes that gets confused due to insertions
	and deletions at the segment boundaries.
----------------------------------------------------------------------------------------------*/
void GrSubPass::DoCleanUpSegMin(GrTableManager * ptman,
	GrSlotStream * psstrmIn, int islotInitReadPos, GrSlotStream * psstrmOut)
{
	int islotSegMinIn = psstrmIn->SegMin();
	if (islotSegMinIn == -1)
		return;	// input doesn't even know it
	if (islotInitReadPos > islotSegMinIn)
		return;	// should already have figured it out

	// Otherwise this batch of processing likely set the seg-min on the output stream,
	// so check it out.

	// First, if the seg-min of the input stream is zero, it should be zero on the output.
	if (islotSegMinIn == 0)
	{
		psstrmOut->SetSegMin(0, true);
		return;
	}

	// If there is an initial line-break, the seg-min should be just before it.
	int islot;
	if (ptman->State()->HasInitialLB())
	{
		gid16 chwLB = ptman->LBGlyphID();

		// Unfortunately, the seg-min from the previous segment can get off, too. :-(
		// Fix it, while we're at it.
		if (!psstrmIn->SlotAt(islotSegMinIn)->IsInitialLineBreak(chwLB))
		{
			for (islot = 0; islot < psstrmIn->ReadPos(); islot++)
				if (psstrmIn->SlotAt(islot)->IsInitialLineBreak(chwLB))
				{
					psstrmIn->SetSegMin(islot, true);
					break;
				}
		}
		if (psstrmOut->SegMin() > -1
			&& psstrmOut->SlotAt(psstrmOut->SegMin())->IsInitialLineBreak(chwLB))
		{
			return;	// already okay
		}
		for (islot = 0; islot < psstrmOut->WritePos(); islot++)
			if (psstrmOut->SlotAt(islot)->IsInitialLineBreak(chwLB))
			{
				psstrmOut->SetSegMin(islot, true);
				return;
			}
		Assert(false);	// couldn't find it
	}

	// Otherwise, figure it out using the associations. First observe that the seg-min
	// will be in the corresponding chunk to the seg-min of the previous pass.
	int islotChunkMinIn = psstrmIn->ChunkInNextMin(islotSegMinIn);
	int islotChunkLimIn = psstrmIn->ChunkInNextLim(islotSegMinIn);
	if (islotChunkMinIn == -1) islotChunkMinIn = 0;
	if (islotChunkLimIn == -1) islotChunkLimIn = 1;

	int islotChunkMinOut = psstrmIn->ChunkInNext(islotChunkMinIn);
	int islotChunkLimOut = psstrmIn->ChunkInNext(islotChunkLimIn);
	if (islotChunkMinOut == -1) islotChunkMinOut = 0;
	if (islotChunkLimOut == -1) islotChunkLimOut = 1;

	int islotSegMinOut = psstrmOut->SegMin();
	if (islotSegMinOut == -1)
	{
		// No reasonable guess; try to figure it out.
		for (islot = islotChunkMinOut; islot < islotChunkLimOut; islot++)
		{
//			GrSlotState * pslotOut = psstrmOut->SlotAt(islot);
			if (psstrmOut->SlotAt(islot)->BeforeAssoc() == 0)
			{
				islotSegMinOut = islot;
				break;
			}
		}
		if (islotSegMinOut == -1)
		{
			// Ick, couldn't figure it out. Let's hope we set it to something reasonable earlier.
			Assert(psstrmOut->SegMin() > -1);
			return;
		}
	}
	// else we have a reasonable guess

	// islotSegMinOut is the best or at leastfirst slot in the chunk that maps into the segment.
	// But if there is an adjacent slot inserted before it and it is in the chunk,
	// we want to include that also.
	while (islotSegMinOut > islotChunkMinOut
		&& psstrmOut->SlotAt(islotSegMinOut-1)->BeforeAssoc() >= 0)
	{
		islotSegMinOut--;
	}
	psstrmOut->SetSegMin(islotSegMinOut, true);
}

/*----------------------------------------------------------------------------------------------
	This method is something of a kludge. We try to put inserted glyphs after the final
	line-break, but if we don't have line break, they should go inside the segment.
	This method is only called after the pass is fully written.
----------------------------------------------------------------------------------------------*/
void GrSubPass::DoCleanUpSegLim(GrTableManager * ptman, GrSlotStream * psstrmOut,
	TrWsHandling twsh)
{
	int islotSegLimOut = psstrmOut->SegLimIfKnown();
	if (islotSegLimOut == -1)
		return;	// output hasn't set it; nothing to fix

	if (twsh == ktwshNoWs)
		return;	// we just stripped off the whitespace; don't put it back!

	//gid16 chwLB = ptman->LBGlyphID();
	// If there is a LB glyph, it should be at islotSegLimOut--
	// unless we backtracked an inserted one earlier.
	if (ptman->State()->HasInsertedLB()) // && psstrmOut->SlotAt(islotSegLimOut)->IsFinalLineBreak(chwLB))
		return;

	// Okay, no LB; make sure all output glyphs are included in this segment.
	psstrmOut->SetSegLimToWritePos(true);
}

/*----------------------------------------------------------------------------------------------
	If we are in a state where we are supposed to skip some of the initial output,
	do so. The purpose of this is to resync when restarting for a segment other than the
	first. Caller should ensure that there are enough slots to skip.
----------------------------------------------------------------------------------------------*/
int PassState::DoResyncSkip(GrSlotStream * psstrmOutput)
{
	if (m_fDidResyncSkip)
		return 0;

	if (m_cslotSkipToResync == 0)
	{
		m_fDidResyncSkip = true;
		return 0;
	}

	if (!CanResyncSkip(psstrmOutput))
	{
		Assert(false); // caller makes sure this doesn't happen
		return (m_cslotSkipToResync - psstrmOutput->WritePos());
	}

	psstrmOutput->ResyncSkip(m_cslotSkipToResync);
	m_fDidResyncSkip = true;

	return 0;
}

/*----------------------------------------------------------------------------------------------
	Run a constraint for a single rule.

	@param ruln					- rule to test
	@param psstrmIn				- input stream
	@param psstrmOut			- for accessing items in the pre-context
	@param cslotPreModContext	- the number of items that need to be tested prior to the
									current stream position
	@param cslotMatched			- the number of items matched after the current stream position
----------------------------------------------------------------------------------------------*/
bool GrPass::RunConstraint(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	int cslotPreModContext, int cslotMatched)
{
	int nRet = 0;

	if (m_prgibConstraintStart == NULL)
		return true;

	int biStart = int(m_prgibConstraintStart[ruln]);
	if (biStart == 0)
		return true;	// no constraints

	for (int islot = -cslotPreModContext; islot < cslotMatched; islot++)
	{
		nRet = RunCommandCode(ptman, m_prgbConstraintBlock + biStart, true,
			psstrmIn, psstrmOut, islot);

		if (nRet == 0)	// one slot failed
			return false;
	}

	return (nRet != 0);
}

/*----------------------------------------------------------------------------------------------
	Check that we can interpret all the commands in the rule. If not, return -1 indicating
	that we don't want to run a rule after all. This can happen when the version of the compiler
	that generated the font is later than this engine.
----------------------------------------------------------------------------------------------*/
int GrPass::CheckRuleValidity(int ruln)
{
	if (ruln == -1)
		return -1;

	if (m_prgfRuleOkay[ruln])
		return ruln;

	int biStart = m_prgibActionStart[ruln];
	byte * pbNext = m_prgbActionBlock + biStart;

	//	General purpose variables:
//	int arg1, arg2, arg3, arg4;
	int nSlotRef;
	int nInputClass;
	int nOutputClass;
	int c;
	int islotArg;
	int nIndex;
	int nGlyphAttr;
	int nAttLevel;
	int nFeat;
	int nPState;
//	SlotAttrName slat;

	int i;

//	int nRet = 0;

	while (true) // exit by encountering PopRet or RetTrue or RetZero
	{
		ActionCommand op = ActionCommand(*pbNext++);

		switch (op)
		{
		case kopNop:
			break;

		case kopPushByte:
			pbNext++;
			break;
		case kopPushByteU:
			pbNext++;
			break;
		case kopPushShort:
			pbNext++;
			pbNext++;
			break;
		case kopPushShortU:
			pbNext++;
			pbNext++;
			break;
		case kopPushLong:
			pbNext++;
			pbNext++;
			pbNext++;
			pbNext++;
			break;

		case kopNeg:
		case kopTrunc8:	case kopTrunc16:
		case kopNot:
			break;

		case kopAdd:	case kopSub:
		case kopMul:	case kopDiv:
		case kopMin:	case kopMax:
		case kopAnd:	case kopOr:
		case kopEqual:	case kopNotEq:
		case kopLess:	case kopGtr:
		case kopLessEq:	case kopGtrEq:
			break;

		case kopCond:
			break;

		case kopNext:
			break;
		case kopNextN:
			c = *pbNext; pbNext++;	// count
			break;
		case kopPutGlyph8bitObs:
			nOutputClass = *pbNext++;
			break;
		case kopPutGlyph:
			pbNext += 2;
			break;
		case kopPutCopy:
			nSlotRef = *pbNext++;
			break;
		case kopPutSubs8bitObs:
			nSlotRef = *pbNext++;
			nInputClass = *pbNext++;
			nOutputClass = *pbNext++;
			break;
		case kopPutSubs:
			nSlotRef = *pbNext++;
			pbNext += 4;	// 2 bytes each for classes
			break;
		case kopCopyNext:
			break;
		case kopInsert:
			break;
		case kopDelete:
			break;
		case kopAssoc:
			c = *pbNext; pbNext++;
			for (i = 0; i < c; i++)
				pbNext++;
			break;
		case kopCntxtItem:
			islotArg = *pbNext++;
			c = *pbNext++;
			break;

		case kopAttrSet:
		case kopAttrAdd:
		case kopAttrSub:
		case kopAttrSetSlot:
			pbNext++;	// slot attribute ID
			break;
		case kopIAttrSet:
		case kopIAttrAdd:
		case kopIAttrSub:
		case kopIAttrSetSlot:
			pbNext++;	// slot attribute ID
			nIndex = *pbNext++; // index; eg, global ID for component
			break;

		case kopPushSlotAttr:
			pbNext++;
			nSlotRef = *pbNext++;
			break;
		case kopPushISlotAttr:
			pbNext++;
			nSlotRef = *pbNext++;
			nIndex = *pbNext++;
			break;

		case kopPushGlyphAttrObs:
		case kopPushAttToGAttrObs:
			nGlyphAttr = *pbNext++;
			nSlotRef = *pbNext++;
			break;
		case kopPushGlyphAttr:
		case kopPushAttToGlyphAttr:
			*pbNext += 3;
			break;
		case kopPushGlyphMetric:
		case kopPushAttToGlyphMetric:
			nGlyphAttr = *pbNext++;
			nSlotRef = *pbNext++;
			nAttLevel = *pbNext++;
			break;
		case kopPushFeat:
			nFeat = *pbNext++;
			nSlotRef = *pbNext++;
			break;

		case kopPushProcState:
			nPState = *pbNext++;
			break;
		case kopPushVersion:
			break;

		case kopPopRet:
		case kopRetZero:
		case kopRetTrue:
			m_prgfRuleOkay[ruln] = true;
			return ruln;

		default:
			// Uninterpretable command:
			return -1;
		}
	}

	return ruln; // to keep compiler from griping
}

/*----------------------------------------------------------------------------------------------
	Runs the rule, if any, otherwise just passes one character through.
	The rule does nothing but set the line-break weights for the slots.

	@param ptman				- the table manager that allocates slots
	@param ruln					- number of rule to run; -1 if none applies
	@param psstrmIn/Out			- input/output stream
----------------------------------------------------------------------------------------------*/
void GrLineBreakPass::RunRule(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	//	Remember the beginnings of the new chunk:
	int islotIn = psstrmIn->ReadPos();
	int islotOut = psstrmOut->WritePos();
	int cslotReprocessed = psstrmIn->SlotsToReprocess();

	if (ruln == -1)
	{
#ifdef OLD_TEST_STUFF
		if (RunTestRules(ptman, ruln, psstrmIn, psstrmOut))
#else
		if (false)
#endif // OLD_TEST_STUFF
		{}
		else
		//	Just pass one glyph through.
		{
			psstrmOut->CopyOneSlotFrom(psstrmIn);
			psstrmOut->SetPosForNextRule(0, psstrmIn, false);
		}
	}
	else
	{
		//	Run the rule.
#ifdef OLD_TEST_STUFF
		if (RunTestRules(ptman, ruln, psstrmIn, psstrmOut))
#else
		if (false)
#endif // OLD_TEST_STUFF
		{}
		else
		{
			int biStart = m_prgibActionStart[ruln];
			int iResult = RunCommandCode(ptman, m_prgbActionBlock + biStart, false,
				psstrmIn, psstrmOut, 0);
			psstrmOut->SetPosForNextRule(iResult, psstrmIn, false);
		}
	}

	CheckInputProgress(psstrmIn, psstrmOut, islotIn);

	MapChunks(psstrmIn, psstrmOut, islotIn, islotOut, cslotReprocessed);
}

/*----------------------------------------------------------------------------------------------
	Runs the rule, if any, otherwise just passes one character through.
	Keeps track of the association information in the slots and chunk
	mappings for backtracking.

	@param ptman				- the table manager that allocates slots
	@param ruln					- number of rule to run; -1 if none applies
	@param psstrmIn/Out			- input / output streams
----------------------------------------------------------------------------------------------*/
void GrSubPass::RunRule(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	//	Remember the beginnings of the new chunk:
	int islotIn = psstrmIn->ReadPosForNextGet();
	int islotOut = psstrmOut->WritePos();
	int cslotReprocessed = psstrmIn->SlotsToReprocess();

	if (ruln == -1)
	{
#ifdef OLD_TEST_STUFF
		if (RunTestRules(ptman, ruln, psstrmIn, psstrmOut))
#else
		if (false)
#endif // OLD_TEST_STUFF
		{}
		else
		//	Just pass one glyph through.
		{
			psstrmOut->CopyOneSlotFrom(psstrmIn);
			psstrmOut->SetPosForNextRule(0, psstrmIn, false);
		}
	}
	else
	{
		//	Run the rule.
#ifdef OLD_TEST_STUFF
		if (RunTestRules(ptman, ruln, psstrmIn, psstrmOut))
#else
		if (false)
#endif // OLD_TEST_STUFF
		{}
		else
		/****************************/
		{
			int biStart = m_prgibActionStart[ruln];
			int iResult = RunCommandCode(ptman, m_prgbActionBlock + biStart, false,
				psstrmIn, psstrmOut, 0);
			psstrmOut->SetPosForNextRule(iResult, psstrmIn, false);

			// Restore if we need line-boundary contextualization based on physical locations
			////psstrmOut->SetLBContextFlag(ptman, islotOut);
		}
	}

	CheckInputProgress(psstrmIn, psstrmOut, islotIn);

	MapChunks(psstrmIn, psstrmOut, islotIn, islotOut, cslotReprocessed);
}

/*----------------------------------------------------------------------------------------------
	Runs the rule, if any, otherwise just positions one character in the standard way.
	Keeps track of (among other things) the overall physical size of the output
	and chunk mappings.

	@param ptman				- the table manager that allocates slots
	@param ruln					- number of rule to run; -1 if none applies
	@param psstrmIn / Out		- input / output streams
----------------------------------------------------------------------------------------------*/
void GrPosPass::RunRule(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	//	Remember the beginnings of the new chunk:
	int islotIn = psstrmIn->ReadPosForNextGet();
	int islotOut = psstrmOut->WritePos();
	int cslotReprocessed = psstrmIn->SlotsToReprocess();

	int iResult = 0;

	Assert(psstrmIn->GotIndexOffset());
	//	Don't do any positioning in anything being processed from the previous line.
	if (psstrmIn->ReadPosForNextGet() < psstrmIn->IndexOffset())
		ruln = -1;

	if (ruln == -1)
	{
		//	Just position one glyph in the standard way.
		psstrmOut->CopyOneSlotFrom(psstrmIn);
		iResult = 0;
	}
	else
	{
		//	Run the rule.
#ifdef OLD_TEST_STUFF
		if (RunTestRules(ptman, ruln, psstrmIn, psstrmOut))
#else
		if (false)
#endif // OLD_TEST_STUFF
		{}
		else
		{
			int biStart = m_prgibActionStart[ruln];
			iResult = RunCommandCode(ptman, m_prgbActionBlock + biStart, false,
				psstrmIn, psstrmOut, 0);
		}
	}

	//	Make sure an entire cluster is present in the output stream. Actually there might be
	//	interleaved clusters, which is the purpose for the loop.
	int dislotClusterMax = 0;
	do {
		dislotClusterMax = psstrmIn->MaxClusterSlot(islotIn, psstrmIn->ReadPos());
		Assert(dislotClusterMax >= 0);
		for (int islot = 0; islot < dislotClusterMax; islot++)
			psstrmOut->CopyOneSlotFrom(psstrmIn);
		iResult -= dislotClusterMax;
	} while (dislotClusterMax > 0);

	psstrmOut->SetPosForNextRule(iResult, psstrmIn, true);

	// Restore if we need line-boundary contextualization based on physical locations
	////psstrmOut->SetLBContextFlag(ptman, islotOut);

	if (ruln > -1)
	{
		psstrmOut->CalcIndexOffset(ptman);
		//	Update the attachment trees and cluster metrics for slots modified by this rule.
		//	Note that this loop assumes that the reprocess buffer points to the identical
		//	slot objects as the output stream (not copies of the slots).
		for (int islot = islotIn - psstrmIn->SlotsSkippedToResync();
			islot < psstrmOut->WritePos() + psstrmIn->SlotsToReprocess();
			islot++)
		{
			psstrmOut->SlotAt(islot)->HandleModifiedPosition(ptman, psstrmIn, psstrmOut, islot);
		}
	}

	CheckInputProgress(psstrmIn, psstrmOut, islotIn);

	MapChunks(psstrmIn, psstrmOut, islotIn, islotOut, cslotReprocessed);

	psstrmOut->AssertStreamIndicesValid(psstrmIn);
}

/*----------------------------------------------------------------------------------------------
	Perform the reversal for internal bidirectionality.
	Caller is responsible for updating the read- and write-positions.
	Private.

	OBSOLETE

	@param nCurrDirection			- direction level of range of reverse
	@param psstrmIn, psstrmOut		- streams
	@param islotInMin, islotInLim	- range to reverse
	@param islotOutMin,islotOutLim	- corresponding output positions; note that islotOutMin
										will be greater than islotOutLim if we are reversing
										reversing the slots (direction is odd)
----------------------------------------------------------------------------------------------*/
#if 0
void GrBidiPass::OldReverse(int nCurrDirection,
	 GrSlotStream * psstrmIn,	int islotInMin,		int islotInLim,
	 GrSlotStream * psstrmOut,	int islotOutMin,	int islotOutLim)
{
	int oInc = (islotOutLim > islotOutMin)? 1: -1;	// which direction output is moving in
													// buffer (if dir is even, oInc == 1;
													// if odd, oInc == -1)

	int islotITmp = islotInMin;
	int islotOTmp = islotOutMin;

	while (islotITmp != islotInLim) {
		//	Find end of sub-range at current level to reverse.
		int islotSubLim = psstrmIn->OldDirLevelRange(NULL, islotITmp, nCurrDirection);
		Assert(islotSubLim >= 0);

		if (islotSubLim == islotITmp) {
			//	Current slot is at this level--just copy it.
			psstrmOut->SimpleCopyFrom(psstrmIn, islotITmp, islotOTmp);
			islotOTmp += oInc;
			islotITmp++;
		}
		else {
			//	There is a sub-range that needs to be reversed.
			int islotONext = islotOTmp + ((islotSubLim-islotITmp) * oInc);
			OldReverse(nCurrDirection+1,
				psstrmIn, islotITmp, islotSubLim,
				psstrmOut, islotONext - oInc, islotOTmp - oInc);
			islotOTmp = islotONext;
			islotITmp = islotSubLim;
		}
	}
}
#endif

/*----------------------------------------------------------------------------------------------
	Perform the reversal for internal bidirectionality.
	Caller is responsible for updating the read- and write-positions.
	Return the number of slots NOT copied because they were markers.
	Private.

	@param psstrmIn, psstrmOut			- streams
	@param vislotStarts, vislotStops	- indices of ranges to reverse, from inner to outer
----------------------------------------------------------------------------------------------*/
int GrBidiPass::Reverse(GrTableManager * ptman,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,			 
	std::vector<int> & vislotStarts, std::vector<int> & vislotStops)
{
	Assert(vislotStarts.size() == vislotStops.size());

	std::vector<int> vislotMap;	// list of slot indices in the order they should be copied
							// into the output

	int islotOuterStart = vislotStarts.back();
	int islotOuterStop = vislotStops.back();

	// Initialize map as if nothing is reversed; eg for [2, 9]: (2 3 4 5 6 7 8 9)
	int islot;
	for (islot = 0; islot <= (islotOuterStop - islotOuterStart); islot++)
		vislotMap.push_back(islot + islotOuterStart);

	// Do the inner reversals first, followed by the outer reversals.
	size_t iislot;
	for (iislot = 0; iislot < vislotStarts.size(); iislot++)
	{
		// Reverse the run.
		int islotStart = vislotStarts[iislot] - islotOuterStart;
		int islotStop = vislotStops[iislot] - islotOuterStart;
		int islot1, islot2;
		for (islot1 = islotStart, islot2 = islotStop; islot1 < islot2; islot1++, islot2--)
		{
			int islotTmp = vislotMap[islot1];
			vislotMap[islot1] = vislotMap[islot2];
			vislotMap[islot2] = islotTmp;
		}
	}
	
	// With vislotStarts = [7, 5, 2] and vislotStops = [8, 8, 9], we  get
	// first:       (2 3 4 5 6 8 7 9)
	// then:        (2 3 4 7 8 5 6 9)
	// and finally: (9 6 5 8 7 4 3 2)

	// Now copy the slots.

	int islotOutStart = psstrmOut->WritePos();
	int cslotNotCopied = 0;
	for (iislot = 0; iislot < vislotMap.size(); iislot++)
	{
		GrSlotState * pslot = psstrmIn->SlotAt(vislotMap[iislot]);
		// Don't copy bidi markers unless they have been set to use a specific glyph.
		if (pslot->IsBidiMarker() && pslot->ActualGlyphForOutput(ptman) == 0)
			cslotNotCopied++;
		else
			psstrmOut->SimpleCopyFrom(psstrmIn, vislotMap[iislot],
				iislot + islotOutStart - cslotNotCopied);
	}
	return cslotNotCopied;
}

/*----------------------------------------------------------------------------------------------
	Unwind the output for this pass, given that a change was assumed to have occurred
	in the input at the given position. Return the position to which we unwound.
----------------------------------------------------------------------------------------------*/
int GrPass::Unwind(GrTableManager * ptman,
	int islotChanged, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	bool fFirst)
{
	//	Back up the number of slots required for the longest rule context,
	//	but if we land in the middle of a chunk, go forward to its boundary.
	int islotIn = max(islotChanged - m_pzpst->MaxChunk(), 0);
	if (!psstrmIn->NoReproc())
		islotIn = min(islotIn, psstrmIn->ReprocMin());
	psstrmIn->ClearReprocBuffer();
	int islotOut = 0;
	if (islotIn < psstrmIn->SlotsSkippedToResync() ||
		islotIn == 0 ||
		psstrmIn->ReadPos() == 0)
	{
		//	The next-chunk-map is invalid. The beginning of the chunk is 0.
		islotIn = 0;
		islotOut = 0;
	}
	else if (islotIn >= psstrmIn->ReadPos())
	{
		//	No need to unwind.
		Assert(psstrmIn->NoReproc());
		return psstrmOut->WritePos();
	}
	else
	{
		Assert(islotIn < psstrmIn->ReadPos());
		islotIn = min(islotIn, psstrmIn->ReadPos() - 1);
		while (islotIn < psstrmIn->ReadPos() &&
			(islotOut = psstrmIn->ChunkInNext(islotIn)) == -1)
		{
			++islotIn;
		}
		if (islotIn == psstrmIn->ReadPos())
			islotOut = psstrmOut->WritePos();
	}

	Assert(islotOut != -2);	// because the chunk size must be <= than the max-chunk,
							// so we should never hit the end of the output stream

	//	Now we've found a chunk boundary.

//	if (fFirst)
//		Unattach(psstrmIn, islotIn, psstrmOut, islotOut, islotChanged);
//	else
//		Unattach(psstrmIn, islotIn, psstrmOut, islotOut, -1);

	psstrmIn->UnwindInput(islotIn, PreBidiPass());
	psstrmOut->UnwindOutput(islotOut, IsPosPass());

	if (psstrmIn->ReadPos() < psstrmIn->SlotsSkippedToResync())
	{
		ptman->Pass(m_ipass - 1)->UndoResyncSkip();
		psstrmIn->ClearSlotsSkippedToResync();
	}
	if (psstrmOut->WritePos() < psstrmOut->SlotsSkippedToResync())
	{
		Assert(psstrmOut->SlotsSkippedToResync() == m_pzpst->m_cslotSkipToResync);
		UndoResyncSkip();
		psstrmOut->ClearSlotsSkippedToResync();
	}

	if (ptman->LoggingTransduction())
		m_pzpst->UnwindLogInfo(islotIn, islotOut);

	return islotOut;
}

/*----------------------------------------------------------------------------------------------
	For a GrBidiPass, just unwind to the beginning of the reordered chunk.
----------------------------------------------------------------------------------------------*/
int GrBidiPass::Unwind(GrTableManager * ptman,
	int islotChanged, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	bool fFirst)
{
	int islotIn;
	int islotOut;

	if (islotChanged == 0)
	{
		islotIn = 0;
		islotOut = 0;
	}
	else {
		islotIn = min(islotChanged, psstrmIn->ReadPos());
		islotIn = max(islotIn - 1, 0);
		while (islotIn > 0 && !StrongDir(psstrmIn->SlotAt(islotIn)->Directionality()))
		{
			// Slightly redundant with ZapCalculatedDirLevels, but possibly needed for slots
			// that have been modified by the substitution passes.
			psstrmIn->SlotAt(islotIn)->ZapDirLevel();
			islotIn--;
		}
		Assert(islotIn == 0 || psstrmIn->SlotAt(islotIn)->DirHasBeenProcessed());
		islotOut = 0;
		while (islotIn > 0 && (islotOut = psstrmIn->ChunkInNext(islotIn)) == -1)
			--islotIn;
	}

	if (islotOut == -1 || islotOut == -2)
		islotOut = 0;

	//	Now we've found a chunk boundary; islotI and islotO are the positions in the
	//	input and output streams, respectively.

	psstrmIn->UnwindInput(islotIn, false);
	psstrmOut->UnwindOutput(islotOut, false);

	if (psstrmOut->WritePos() < m_pzpst->m_cslotSkipToResync)
	{
		Assert(false); // shouldn't be any of this mess for the Bidi pass
		m_pzpst->m_fDidResyncSkip = false;
	}

	if (ptman->LoggingTransduction())
		m_pzpst->UnwindLogInfo(islotIn, islotOut);

	return islotOut;
}

/*----------------------------------------------------------------------------------------------
	Reinitialize part of the transduction logging information when we unwind the pass.
----------------------------------------------------------------------------------------------*/
void PassState::UnwindLogInfo(int islotIn, int islotOut)
{
	while (m_crulrec > 0 && m_rgrulrec[m_crulrec-1].m_islot >= islotIn)
	{
		m_crulrec--;
		m_rgrulrec[m_crulrec].m_islot = 0;
		m_rgrulrec[m_crulrec].m_irul = 0;
		m_rgrulrec[m_crulrec].m_fFired = false;
	}

	for (int islot = islotOut; islot < 128; islot++)
	{
		m_rgcslotDeletions[islot] = 0;
		m_rgfInsertion[islot] = false;
	}
}

/*----------------------------------------------------------------------------------------------
	Undo the side effects of attachments, ie, fix up the attachment trees as we unwind.

	@param islotLB		the index of the line-break that was just inserted, or -1 if this is
						not the first pass to include line-breaks; we want to skip this slot
						in the processing in this method

	OBSOLETE
----------------------------------------------------------------------------------------------*/
//:Ignore
void GrPosPass::Unattach(GrSlotStream * psstrmIn, int islotIn,
	GrSlotStream * psstrmOut, int islotOut, int islotLB)
{
	//	Because this is a positioning pass, there is a one-to-one correspondence between
	//	the slots in the input and the slots in the output. Thus we can make simplifying
	//	assumptions. Specifically, the chunk sizes are equal, except for the possiblity
	//	of a LB slot that we just inserted in the input stream.
#if 0

#ifdef _DEBUG
	int islotDiff = (psstrmIn->ReadPos() - islotIn) - (psstrmOut->WritePos() - islotOut);
	if (islotLB == -1)
		Assert(islotDiff == 0);
	else
		Assert(islotDiff == 1);
#endif // _DEBUG

	int islotInLp = psstrmIn->ReadPos();
	int islotOutLp = psstrmOut->WritePos();
	for ( ; islotInLp-- > islotIn && islotOutLp-- > islotOut; )
	{
		if (islotLB == islotInLp)
		{
			GrSlotState * pslotTmp = psstrmIn->SlotAt(islotInLp);
			islotInLp--;
		}

		GrSlotState * pslotIn = psstrmIn->SlotAt(islotInLp);
		GrSlotState * pslotOut = psstrmOut->SlotAt(islotOutLp);

		if (pslotIn != pslotOut)
		{
			GrSlotState * pslotInRoot = pslotIn->AttachRoot();
			GrSlotState * pslotOutRoot = pslotOut->AttachRoot();

			if (pslotOutRoot)
				pslotOutRoot->RemoveLeaf(pslotOut);
			if (pslotInRoot)
				pslotInRoot->AddLeaf(pslotIn);
		}
	}
#endif // 0
}
//:End Ignore


/*----------------------------------------------------------------------------------------------
	Record the fact that the given rule's constraint failed, for the purpose of logging
	the transduction process.
----------------------------------------------------------------------------------------------*/
void GrPass::RecordRuleFailed(int islot, int irul)
{
	m_pzpst->RecordRule(islot, irul, false);
}


/*----------------------------------------------------------------------------------------------
	Record the fact that the given rule fired, for the purpose of logging
	the transduction process.
----------------------------------------------------------------------------------------------*/
void GrPass::RecordRuleFired(int islot, int irul)
{
	m_pzpst->RecordRule(islot, irul, true);
}


/*----------------------------------------------------------------------------------------------
	Record the fact that we forcibly advanced due to the max-rule-loop.
----------------------------------------------------------------------------------------------*/
void GrPass::RecordHitMaxRuleLoop(int islot)
{
	m_pzpst->RecordRule(islot, PassState::kHitMaxRuleLoop);
}


/*----------------------------------------------------------------------------------------------
	Record the fact that we forcibly advanced due to the max-backup.

	In theory it should be possible to call this, but in reality, it is pretty hard to be
	able to say that there is a rule that would have fired except it was prevent by
	MaxBackup. So currently this method is not called.
----------------------------------------------------------------------------------------------*/
void GrPass::RecordHitMaxBackup(int islot)
{
	m_pzpst->RecordRule(islot, PassState::kHitMaxBackup);
}


/*----------------------------------------------------------------------------------------------
	Record some rule-related activity on a slot.
----------------------------------------------------------------------------------------------*/
void PassState::RecordRule(int islot, int irul, bool fFired)
{
	if (m_crulrec >= 128)
		return;

	m_rgrulrec[m_crulrec].m_irul = irul;
	m_rgrulrec[m_crulrec].m_islot = islot;
	m_rgrulrec[m_crulrec].m_fFired = true;
	m_rgrulrec[m_crulrec].m_fFired = fFired;
	m_crulrec++;
}


/*----------------------------------------------------------------------------------------------
	Record the fact that a slot was deleted before the given slot (in the output).
----------------------------------------------------------------------------------------------*/
void PassState::RecordDeletionBefore(int islot)
{
	if (islot >= 128)
		return;

	(m_rgcslotDeletions[islot])++;
}

/*----------------------------------------------------------------------------------------------
	Record the fact that the given slot was inserted into the output.
----------------------------------------------------------------------------------------------*/
void PassState::RecordInsertionAt(int islot)
{
	if (islot >= 128)
		return;

	m_rgfInsertion[islot] = true;
}

} // namespace gr
