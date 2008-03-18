/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrFSM.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Implements the finite state machine.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	   Include files
//:>********************************************************************************************
#include "Main.h"
#include <cstring>
#ifdef _MSC_VER
#pragma hdrstop
#endif
// any other headers (not precompiled)

#undef THIS_FILE
DEFINE_THIS_FILE

//:End Ignore

//:>********************************************************************************************
//:>	   Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	   Local Constants and static variables
//:>********************************************************************************************

namespace gr
{

//:>********************************************************************************************
//:>	   Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Fill in the FSM by reading from the font stream.
	Assumes the stream is in the correct position.
----------------------------------------------------------------------------------------------*/
bool GrFSM::ReadFromFont(GrIStream & grstrm, int fxdVersion)
{
	short snTmp;
	
	//	number of FSM states
	snTmp = grstrm.ReadShortFromFont();
	m_crow = snTmp;
	//	number of transitional states
	snTmp = grstrm.ReadShortFromFont();
	int crowTransitional = snTmp;
	//	number of success states
	snTmp = grstrm.ReadShortFromFont();
	int crowSuccess = snTmp;
	m_crowFinal = m_crow - crowTransitional;
	m_crowNonAcpt = m_crow - crowSuccess;
	m_rowFinalMin = crowTransitional;
	//	number of columns
	snTmp = grstrm.ReadShortFromFont();
	m_ccol = snTmp;

	//	Sanity checks.
	if (crowTransitional > m_crow || crowSuccess > m_crow)
		return false; // bad table
	//	TODO: add a sanity check for m_ccol.

	//	number of FSM glyph ranges and search constants
	snTmp = grstrm.ReadShortFromFont();
	m_cmcr = snTmp;
	snTmp = grstrm.ReadShortFromFont();
	m_dimcrInit = snTmp;
	snTmp = grstrm.ReadShortFromFont();
	m_cLoop = snTmp;
	snTmp = grstrm.ReadShortFromFont();
	m_imcrStart = snTmp;

	m_prgmcr = new GrFSMClassRange[m_cmcr];
	for (int imcr = 0; imcr < m_cmcr; imcr++)
	{
		m_prgmcr[imcr].m_chwFirst = grstrm.ReadUShortFromFont();
		m_prgmcr[imcr].m_chwLast = grstrm.ReadUShortFromFont();
		m_prgmcr[imcr].m_col = grstrm.ReadUShortFromFont();
	}

	// rule map and offsets (extra item at end gives final offset, ie, total)
	m_prgirulnMin = new data16[crowSuccess + 1];
	data16 * pchw = m_prgirulnMin;
	int i;
	for (i = 0; i < (crowSuccess + 1); i++, pchw++)
	{
		*pchw = grstrm.ReadUShortFromFont();
	}

	//	Last offset functions as the total length of the rule list.
	//	Note that the number of rules in the map is not necessarily equal to the number of
	//	rules in the pass; some rules may be listed multiply, if they are matched by more
	//	than one state.
	int crulInMap = m_prgirulnMin[crowSuccess];
	m_prgrulnMatched = new data16[crulInMap];
	m_crulnMatched = crulInMap;
	pchw = m_prgrulnMatched;
	for (i = 0; i < crulInMap; i++, pchw++)
	{
		*pchw = grstrm.ReadUShortFromFont();
	}

	//	min rule pre-context number of items
	byte bTmp = grstrm.ReadByteFromFont();
	m_critMinRulePreContext = bTmp;
	//	max rule pre-context number of items
	bTmp = grstrm.ReadByteFromFont();
	m_critMaxRulePreContext = bTmp;

	if (m_critMinRulePreContext > kMaxSlotsPerRule || m_critMaxRulePreContext > kMaxSlotsPerRule)
		return false; // bad table

	int cStartStates = m_critMaxRulePreContext - m_critMinRulePreContext + 1;

	//	start states
	m_prgrowStartStates = new short[cStartStates];
	short * psn = m_prgrowStartStates;
	for (int ic = 0; ic < cStartStates;	ic++, psn++)
	{
		*psn = grstrm.ReadShortFromFont();
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Fill in the FSM's state table by reading from the font stream.
	Assumes the stream is in the correct position.
----------------------------------------------------------------------------------------------*/
bool GrFSM::ReadStateTableFromFont(GrIStream & grstrm, int fxdVersion)
{
	int cCells = ((m_crow - m_crowFinal) * m_ccol);
	m_prgrowTransitions = new short[cCells];
	short * psn = m_prgrowTransitions;
	for (int iCell = 0; iCell < cCells; iCell++, psn++)
	{
		*psn = grstrm.ReadShortFromFont();
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Use the transition table to find a rule or rules that match; then run the contraint
	code to see if the rule is active. Return the rule number of the rule to be applied.

	@param ppass		- pass, which knows how to run constraint code
	@param psstrmIn		- input stream
	@param psstrmOut	- output stream (from which to get recently output slots)
----------------------------------------------------------------------------------------------*/
int GrFSM::GetRuleToApply(GrTableManager * ptman, GrPass * ppass,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	//	List of accepting states and corresponding count of slots that were
	//	matched by each.
	int prgrowAccepting[kMaxSlotsPerRule];
	int prgcslotMatched[kMaxSlotsPerRule];

	//	Run the transition table until it jams.
	int crowAccepting = RunTransitionTable(ppass, psstrmIn, psstrmOut,
		prgrowAccepting, prgcslotMatched);

	//	Nothing matched; fail quickly.
	if (crowAccepting == 0)
		return -1;

	int * prow   = prgrowAccepting + crowAccepting - 1;
	int * pcslot = prgcslotMatched + crowAccepting - 1; // # of slots matched AFTER the curr pos

	//	Quick test for common case of exactly one rule matched.
	if (crowAccepting == 1 &&
		(m_prgirulnMin[*prow - m_crowNonAcpt + 1] - m_prgirulnMin[*prow - m_crowNonAcpt]) == 1)
	{
		int iruln = m_prgirulnMin[*prow - m_crowNonAcpt];
		int ruln = m_prgrulnMatched[iruln];
		if (RunConstraintAndRecordLog(ptman, ppass, ruln, psstrmIn, psstrmOut,
			ppass->PreModContextForRule(ruln), *pcslot))
		{
			return ruln;
		}
		else
			return -1;
	}

	//	Several rules were matched. We need to sort them and then run the constraints
	//	for each rule, accepting the first one whose constraints succeed.

	//	Count up the number of rules that matched and allocate a buffer of the right size.
	int crulMatched = 0;
	while (prow >= prgrowAccepting)
	{
		crulMatched +=
			m_prgirulnMin[*prow - m_crowNonAcpt + 1] - m_prgirulnMin[*prow - m_crowNonAcpt];
		--prow;
	}
	MatchedRule * prgmr = new MatchedRule[crulMatched];

	prow = prgrowAccepting + crowAccepting - 1;

	//	Do a dumb insertion sort, ordering primarily by sort key (largest first)
	//	and secondarily by the order in the original file (rule number).
	//	Review: is this kind of sort fast enough? We assuming that we usually only
	//	have a handful of matched rules.
	int cmr = 0;
	while (prow >= prgrowAccepting)
	{
		int iruln = m_prgirulnMin[*prow - m_crowNonAcpt];
		//	Get all the rules matches by a single state.
		while (iruln < m_prgirulnMin[*prow - m_crowNonAcpt + 1])
		{
			int ruln = m_prgrulnMatched[iruln];
			int nSortKey = ppass->SortKeyForRule(ruln);
			int cslot = *pcslot;
			int imr;
			for (imr = 0; imr < cmr; imr++)
			{
				if (nSortKey > prgmr[imr].nSortKey ||
					(nSortKey == prgmr[imr].nSortKey && ruln < prgmr[imr].ruln))
				{
					//	Insert here.
					memmove(prgmr+imr+1, prgmr+imr, (isizeof(MatchedRule) * (cmr - imr)));
					prgmr[imr].ruln = ruln;
					prgmr[imr].nSortKey = nSortKey;
					prgmr[imr].cslot = cslot;
					break;
				}
			}
			if (imr >= cmr)
			{
				prgmr[cmr].ruln = ruln;
				prgmr[cmr].nSortKey = nSortKey;
				prgmr[cmr].cslot = cslot;
			}
			cmr++;

			iruln++;
		}
		--prow;
		--pcslot;
	}


	for (int imr = 0; imr < cmr; imr++)
	{
		int ruln = prgmr[imr].ruln;
		if (RunConstraintAndRecordLog(ptman, ppass, ruln, psstrmIn, psstrmOut,
			ppass->PreModContextForRule(ruln), prgmr[imr].cslot))
		{
			//	Success!
			delete[] prgmr;
			return ruln;
		}
	}

	//	No rule matched or the constraints failed.
	delete[] prgmr;
	return -1;
}

/*----------------------------------------------------------------------------------------------
	Run the transition table, generating a list of rows corresponding to accepting states.

	@param psstrmIn			- input stream
	@param prgrowAccepting	- array to fill in with accepting rows
	@param prgcslotMatched	- corresponding array with number of slots matched
----------------------------------------------------------------------------------------------*/
int GrFSM::RunTransitionTable(GrPass * ppass, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	int * prgrowAccepting, int * prgcslotMatched)
{
	int ipass = ppass->PassNumber();

	// Remember we're reading the pre-context items from the output stream, not the input.
	int cslotPreContextAvail = psstrmOut->WritePos();

	if (cslotPreContextAvail < m_critMinRulePreContext)
		return 0;	// not enough input to match any rule

	//	Look backwards by the number of pre-context items for this pass, but not earlier
	//	than the beginning of the stream.
	int cslot = -1 * min(cslotPreContextAvail, m_critMaxRulePreContext);

	//	Normally the start state is state 0. But if we are near the beginning of the stream,
	//	we may need to adjust due to the fact that we don't have enough input yet to
	//	match the pre-context items in all the rules. So in effect we skip some states
	//	corresponding to the earlier items that aren't present.
	int row = m_prgrowStartStates[max(0, m_critMaxRulePreContext - cslotPreContextAvail)];

	int * prowTop = prgrowAccepting;
	int * pcslotTop = prgcslotMatched;
	while (true)
	{
		if (row >= m_rowFinalMin)
			//	final state--jammed
			return (prowTop - prgrowAccepting);

		if (psstrmIn->SlotsPendingInContext() <= cslot)
			//	no more input
			return (prowTop - prgrowAccepting);

		//	Figure out what column the input glyph belong in.
		//	Since this is a time-critical routine, we cache the results for the current pass.
		GrSlotState * pslot = (cslot < 0) ?
			psstrmOut->PeekBack(cslot) :
			psstrmIn->Peek(cslot);
		int col;
		if (pslot->PassNumberForColumn() == ipass)
		{
			col = pslot->FsmColumn();
		}
		else
		{
			gid16 chwGlyphID = (cslot < 0) ?
				psstrmOut->PeekBack(cslot)->GlyphID() :
				psstrmIn->Peek(cslot)->GlyphID();
			col = FindColumn(chwGlyphID);
			pslot->CacheFsmColumn(ipass, col);
		}
		int rowNext;
		if (col < 0)
			rowNext = 0;
		else
			rowNext = m_prgrowTransitions[(row * m_ccol) + col];

		if (rowNext == 0)	// jammed
			return (prowTop - prgrowAccepting);

		cslot++;
		if (rowNext >= m_crowNonAcpt)	// or (rowNext < 0)
		{
			//	Accepting state--push it on the stack.
			//rowNext *= -1;
			gAssert((prowTop - prgrowAccepting) < kMaxSlotsPerRule);
			*prowTop++ = rowNext;
			*pcslotTop++ = cslot;
		}
		row = rowNext;

		gAssert((prowTop - prgrowAccepting) == (pcslotTop - prgcslotMatched));
	}
}

/*----------------------------------------------------------------------------------------------
	Run the transition table, generating a list of rows corresponding to accepting states.
	Optimized to avoid matrix arithmetic for non-accepting states.

	Review: do we need to use this version?

	@param psstrmIn			- input stream
	@param prgrowAccepting	- array to fill in with accepting rows
	@param prgcslotMatched	- corresponding array with number of slots matched
----------------------------------------------------------------------------------------------*/
#if 0

int GrFSM::RunTransitionTableOptimized(GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	int * prgrowAccepting, int * prgcslotMatched)
{
	int cslot = 0;
	int ichwRowOffset = 0;
	int ichwRowOffsetLim = m_crowFinal * m_ccol;	// number of cells
	int * prowTop = prgrowAccepting;
	int * pcslotTop = prgcslotMatched;
	while (true)
	{
		if (ichwRowOffset >= ichwRowOffsetLim)
			//	final state--jammed
			return (prowTop - prgrowAccepting);

		if (psstrmIn->SlotsPendingInContext() < cslot)
			//	no more input
			return (prowTop - prgrowAccepting);

		gid16 chwGlyphID = psstrmIn->Peek(cslot)->GlyphID();
		int col = FindColumn(chwGlyphID);
		short cellValue = *(m_prgprgrowXitions + ichwRowOffset + col);

		if (cellValue == 0)	// jammed
			return (prowTop - prgrowAccepting);

		cslot++;
		if (cellValue < 0)
		{
			//	Accepting state--push it on the stack.
			int rowNext = cellValue * -1;
			gAssert((prowTop - prgrowAccepting) < kMaxSlotsPerRule);
			*prowTop++ = rowNext;
			*pcslotTop++ = cslot;
			ichwRowOffset = rowNext * m_ccol;
		}
		else
			ichwRowOffset = cellValue;
		
		gAssert((prowTop - prgrowAccepting) == (pcslotTop - prgcslotMatched));
	}
}

#endif // 0


/*----------------------------------------------------------------------------------------------
	Run all the constraints for the rules matched by the state that we have reached.
	The rules are ordered as they were in the GDL file. When we hit a constraint that
	matches, return the corresponding rule number. Return -1 if no rule passed.

	@param ppass			- pass, which knows how to actually run the constraints
	@param psstrmIn			- input stream
	@param cslotMatched		- number of slots matched; constraints must be run for each of these
----------------------------------------------------------------------------------------------*/
//:Ignore
int GrFSM::RunConstraints_Obsolete(GrTableManager * ptman, GrPass * ppass, int row,
	GrSlotStream * psstrmIn, int cslotMatched)
{
	int iruln = m_prgirulnMin[row - m_crowNonAcpt];
	while (iruln < m_prgirulnMin[row - m_crowNonAcpt + 1])
	{
		int ruln = m_prgrulnMatched[iruln];
		bool fSuccess = ppass->RunConstraint(ptman, ruln, psstrmIn, NULL, 0, cslotMatched);
		if (fSuccess)
		{
			if (ptman->LoggingTransduction())
				ppass->RecordRuleFired(psstrmIn->ReadPosForNextGet(), ruln);
			return ruln;
		}
		else
		{
			if (ptman->LoggingTransduction())
				ppass->RecordRuleFailed(psstrmIn->ReadPosForNextGet(), ruln);
		}

		iruln++;
	}
	return -1;	// no rule succeeded
}
//:End Ignore

/*----------------------------------------------------------------------------------------------
	Run the constraints for the given rule, and record whether it failed or succeeded in the
	transduction log.

	@param ppass				- pass, which knows how to actually run the constraints
	@param psstrmIn				- input stream
	@param cslotPreModContext	- number of slots before the current position that must be tested
	@param islotLimMatched		- number of slots matched AFTER the current position

	@return True if the constraints succeeded.
----------------------------------------------------------------------------------------------*/
bool GrFSM::RunConstraintAndRecordLog(GrTableManager * ptman, GrPass * ppass, int ruln,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
	int cslotPreModContext, int islotLimMatched)
{
	bool fSuccess = ppass->RunConstraint(ptman, ruln, psstrmIn, psstrmOut,
		cslotPreModContext, islotLimMatched);

	if (ptman->LoggingTransduction())
	{
		if (fSuccess)
			ppass->RecordRuleFired(psstrmIn->ReadPosForNextGet(), ruln);
		else
			ppass->RecordRuleFailed(psstrmIn->ReadPosForNextGet(), ruln);
	}

	return fSuccess;
}

/*----------------------------------------------------------------------------------------------
	Search for the class range containing the glyph ID, and return the matching FSM column.
	Does a fast binary search that uses only shifts, no division.
----------------------------------------------------------------------------------------------*/
int GrFSM::FindColumn(gid16 chwGlyphID)
{
	#ifdef _DEBUG
		//	Check that the pre-calculated constants are correct.
		int nPowerOf2 = 1;
		while (nPowerOf2 <= m_cmcr)
			nPowerOf2 <<= 1;
		nPowerOf2 >>= 1;
		//	Now nPowerOf2 is the max power of 2 <= m_cclsrg.
		//	m_cLoop is not needed for our purposes, but it is included because it is part of
		//	the TrueType standard.
		gAssert(1 << m_cLoop == nPowerOf2);		// m_cloop == log2(nPowerOf2)
		gAssert(m_dimcrInit == nPowerOf2);
		gAssert(m_imcrStart == m_cmcr - m_dimcrInit);
	#endif

	int dimcrCurr = m_dimcrInit;

	GrFSMClassRange * pmcrCurr = m_prgmcr + m_imcrStart;
	while (dimcrCurr > 0) 
	{
		int nTest;
		if (pmcrCurr < m_prgmcr)
			nTest = -1;
		else
		{
			nTest = pmcrCurr->m_chwFirst - chwGlyphID;
			if (nTest < 0 && chwGlyphID <= pmcrCurr->m_chwLast)
				//nTest = 0;
				// This is a tiny bit more efficient:
				return pmcrCurr->m_col;
		}

		if (nTest == 0)
			//	found it
			return pmcrCurr->m_col;

		dimcrCurr >>= 1;	// divide by 2
		if (nTest < 0)
			pmcrCurr += dimcrCurr;
		else // (nTest > 0)
			pmcrCurr -= dimcrCurr;
	}

	return -1;
}

} // namespace gr
