/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrSlotStream.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    GrSlotStream class implementation.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#include "GrConstants.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif

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
	Answer true if there is nothing valid available to read from the stream.
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::AtEnd()
{
	Assert(AssertValid());

	if (m_islotReprocPos > -1 && m_islotReprocPos < signed(m_vpslotReproc.size()))
		return false;

	if (m_islotSegLim > -1 && m_islotReadPos >= m_islotSegLim)
		//	We've determined the segment limit, and we're at or past it.
		return true;

	return (m_islotReadPos == m_islotWritePos);
}

/*----------------------------------------------------------------------------------------------
	Answer true if there is nothing at all available to read from the stream.
	For substitution passes, we need to consider slots that may be beyond what we know
	will be the official end of the segment.
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::AtEndOfContext()
{
	if (m_fUsedByPosPass)
		return AtEnd();

	if (m_islotReprocPos > -1 && m_islotReprocPos < signed(m_vpslotReproc.size()))
		return false;

	return (m_islotReadPos == m_islotWritePos);
}

/*----------------------------------------------------------------------------------------------
	Append a slot to the end of the stream.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::NextPut(GrSlotState * pslot)
{
	Assert(AssertValid());

	if (m_islotWritePos < signed(m_vpslot.size()))
	{
		m_vpslot[m_islotWritePos] = pslot;
	}
	else
	{
		m_vpslot.push_back(pslot);
		m_vislotPrevChunkMap.push_back(-1);
		m_vislotNextChunkMap.push_back(-1);
	}

	if (m_fUsedByPosPass && GotIndexOffset())
		pslot->SetPosPassIndex(m_islotWritePos - m_cslotPreSeg, m_fInputToPosPass1);

	m_islotWritePos++;
}

/*----------------------------------------------------------------------------------------------
	Get the next slot from the stream.
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotStream::NextGet()
{
	Assert(AssertValid());
	Assert(!AtEndOfContext());

	GrSlotState * pslotRet;

	if (m_islotReprocPos > -1)
	{
		if (m_islotReprocPos >= signed(m_vpslotReproc.size()))
		{
			//	Finished reprocessing.
			Assert((unsigned)m_islotReprocPos == m_vpslotReproc.size());
			m_islotReprocPos = -1;
			//	But leave the m_vpslotReproc array in place so that earlier slots can
			//	be accessed by the rules.
			pslotRet = m_vpslot[m_islotReadPos];
			m_islotReadPos++;
		}
		else
		{
			//	Read from the reprocess buffer.
			pslotRet = m_vpslotReproc[m_islotReprocPos];
			m_islotReprocPos++;
		}
	}
	else
	{
		//	Read normally.
		pslotRet = m_vpslot[m_islotReadPos];
		m_islotReadPos++;
	}

	Assert(!m_fUsedByPosPass || pslotRet->PosPassIndex() != GrSlotAbstract::kNotYetSet);

	return pslotRet;
}

/*---------------------------------------------------------------------------------------------
	Peek at the next slot from the stream (dislot = 0 means the very next slot).
	Only used for peeking forward.  RuleInputSlot is used for accessing any possible 
	slot in the	rule's context.
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotStream::Peek(int dislot)
{
	Assert(AssertValid());
	Assert(!AtEndOfContext());
	Assert(dislot >= 0);

	GrSlotState * pslotRet;

	if (m_islotReprocPos > -1)
	{
		int cslotReproc = m_vpslotReproc.size() - m_islotReprocPos; // num left in reproc buffer
		if (dislot >= cslotReproc)
		{
			//	Not quite enough slots in the reprocess buffer.
			pslotRet = m_vpslot[m_islotReadPos + dislot - cslotReproc];
		}
		else if (m_islotReprocPos + dislot < 0)
		{
			//	Reading prior to the reprocess buffer
			pslotRet = m_vpslot[m_islotReadPos + dislot - cslotReproc];
		}
		else
		{
			//	Read from the reprocess buffer.
			pslotRet = m_vpslotReproc[m_islotReprocPos + dislot];
		}
	}
	else
	{
		//	Read normally.
		Assert(m_islotWritePos - m_islotReadPos > dislot);
		pslotRet = m_vpslot[m_islotReadPos + dislot];
	}

	return pslotRet;
}

/*---------------------------------------------------------------------------------------------
	Treat the stream, which is really the output stream, as an input stream,
	retrieving a recently output slot. This happens when we are reading before the 
	current stream position when the rule is being matched, or before the original
	stream position when the rule is being run.

	@param dislot		- how far back to peek before the write position
							WHEN THE RULE STARTED; a negative number
							(NOTE: the current write position is irrelevant)
	@param fNullOkay	- true if it's okay to return NULL in the situation where we're asking
							for something before the beginning of the stream
----------------------------------------------------------------------------------------------*/
#ifdef NDEBUG
GrSlotState * GrSlotStream::PeekBack(int dislot, bool)
#else
GrSlotState * GrSlotStream::PeekBack(int dislot, bool fNullOkay)
#endif
{
	Assert(dislot < 0);
	if (dislot < m_islotRuleStartWrite * -1)
	{
		Assert(fNullOkay);
		return NULL;
	}
	GrSlotState * pslotRet;
	if (m_islotReprocPos > -1)
	{
		if (dislot < m_islotReprocLim - m_islotRuleStartWrite &&
			dislot >= m_islotReprocLim - m_islotRuleStartWrite - signed(m_vpslotReproc.size()))
		{
			//	Read from the reprocess buffer.
			//	Review: this is weird; can it ever happen?
			pslotRet = m_vpslotReproc[m_vpslotReproc.size()
				- (m_islotReprocLim - m_islotRuleStartWrite) + dislot];
		}
		else
			pslotRet = m_vpslot[m_islotRuleStartWrite + dislot];
	}
	else
		pslotRet = m_vpslot[m_islotRuleStartWrite + dislot];

	return pslotRet;
}

/*---------------------------------------------------------------------------------------------
	Skip the given number of slots in the stream.
	ENHANCE: implement more efficiently
----------------------------------------------------------------------------------------------*/
void GrSlotStream::Skip(int dislot)
{
	for (int islot = 0; islot < dislot; ++islot)
		NextGet();
}

/*----------------------------------------------------------------------------------------------
	Perform assertions to ensure that the stream is in a valid state.
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::AssertValid()
{
	Assert(m_islotWritePos <= signed(m_vpslot.size()));
	Assert(m_islotReadPos <= m_islotWritePos);
	// Streams used by the positioning passes (input or output) should not be working
	// with slots beyond the segment lim
	Assert(m_islotSegLim == -1 || !m_fUsedByPosPass || m_islotReadPos <= m_islotSegLim);
	Assert(m_vpslot.size() == m_vislotPrevChunkMap.size());
	Assert(m_vpslot.size() == m_vislotNextChunkMap.size());
	return true;
}

/*----------------------------------------------------------------------------------------------
	Return the number of slots that have been completely output by the previous pass but not
	yet processed by the following pass. ("Completely" output means that they do not need
	to be reprocessed by the same pass, only then are they available as valid input.)
	Include any slots that are present by virtue of needing to be reprocessed by the
	corresponding output stream.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::SlotsPending()
{
	Assert(AssertValid());
	if (m_islotSegLim > -1)
		return (m_islotSegLim - m_islotReadPos + SlotsToReprocess());
	else
		return (m_islotWritePos - m_islotReadPos + SlotsToReprocess());
}

int GrSlotStream::SlotsPendingInContext()
{
	Assert(AssertValid());
	if (m_fUsedByPosPass)
		return SlotsPending();
	else
		return (m_islotWritePos - m_islotReadPos + SlotsToReprocess());
}

int GrSlotStream::TotalSlotsPending()
{
	int cslot = SlotsPendingInContext();
	if (m_fUsedByPosPass && m_islotSegLim > -1)
	{
		int ctmp = m_islotWritePos - m_islotReadPos + SlotsToReprocess();
		if (ctmp > cslot)
		{
			int x; x = 3;
		}
		cslot = max(cslot, ctmp);
	}
	return cslot;
}

/*----------------------------------------------------------------------------------------------
	Return true if this pass is input or output to a positioning pass and we're past the end
	of the segment. So this stream is virtually finished.
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::PastEndOfPositioning(bool fOutput)
{
	if (m_fUsedByPosPass && m_islotSegLim > -1)
	{
		if (fOutput) // this stream is functioning as an output stream
			return (m_islotWritePos >= m_islotSegLim);
		else		// this stream is functioning as an input stream
			return (m_islotReadPos >= m_islotSegLim);
	}
	return false;
}

/*----------------------------------------------------------------------------------------------
	Mark the stream as fully written, so that the output pass doesn't keep trying to ask
	for more input.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::MarkFullyWritten()
{
	m_fFullyWritten = true;
}

/*----------------------------------------------------------------------------------------------
	Return the position of the first glyph at or following the given position
	that is in the top direction, or -1 if we do not have a complete range.

	@param ptman
	@param islotStart
	@nTopDirLevel			- top direction: 0 = left-to-right, 1 = right-to-left
----------------------------------------------------------------------------------------------*/
int GrSlotStream::OldDirLevelRange(EngineState * pengst, int islotStart, int nTopDirLevel)
{
	Assert(m_islotReadPos <= islotStart);
	Assert(islotStart < m_islotWritePos);

	int islot = islotStart;
	int nSlotDirLevel = GetSlotDirLevel(pengst, islot, nTopDirLevel, nTopDirLevel,
		kdircUnknown, kdircNeutral);
	if (nSlotDirLevel == -1)
		return -1;
	while (nSlotDirLevel > nTopDirLevel)
	{
		islot++;
		if (islot >= m_islotWritePos)
		{
			if (m_fFullyWritten)
				return islot;
			else
				return -1;
		}
		else if (m_islotSegLim > -1 && islot >= m_islotSegLim)
		{
			return islot;
		}

		nSlotDirLevel = GetSlotDirLevel(pengst, islot, nTopDirLevel, nTopDirLevel,
			kdircUnknown, kdircNeutral);
		if (nSlotDirLevel == -1)
			return -1;
	}
	return islot;
}

/*----------------------------------------------------------------------------------------------
	Return the end of the range that has the same feature settings as the current slot.
	The recipient is functioning as the input stream.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::FeatureRangeLim(GrSlotStream * psstrmOut)
{
	GrSlotState * pslot = RuleInputSlot(0, psstrmOut);
	int islotResult = ReadPos();
	while (true)
	{
		if (WritePos() < islotResult)
			return WritePos();
		islotResult++;
		GrSlotState * pslotNext = Peek(islotResult - ReadPos() - 1); // Peek(0) means get the next
		if (!pslotNext->SameFeatures(pslot))
			break;
	}
	return islotResult;
}

/*----------------------------------------------------------------------------------------------
	Return true if the all the slots in the stream have the same feature settings.
	Assumes the stream has been fully run.
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::NoFeatureChanges()
{
	Assert(SlotsToReprocess() == 0);

	GrSlotState * pslotFirst = SlotAt(0);
	for (int islot = 1; islot < WritePos(); islot++)
	{
		GrSlotState * pslotAnother = SlotAt(islot);
		if (!pslotFirst->SameFeatures(pslotAnother))
			return false;
	}
	return true;
}

/*----------------------------------------------------------------------------------------------
	Return the position of the first glyph at or following the given position
	that is in the top direction, or -1 if we do not have a complete range.

	@param ptman
	@param islotStart
	@nTopDirLevel			- top direction: 0 = left-to-right, 1 = right-to-left
----------------------------------------------------------------------------------------------*/
int GrSlotStream::DirLevelRange(EngineState * pengst, int islotStart, int nTopDirLevel,
	std::vector<int> & vislotStarts, std::vector<int> & vislotStops)
{
	Assert(m_islotReadPos <= islotStart);
	Assert(islotStart < m_islotWritePos);

	std::vector<int> vislotStartStack; // indices of starts of ranges to reverse

	// Stack corresponding to embedding and override markers:
	std::vector<DirCode> vdircMarkerStack;
	std::vector<DirCode> vdircOverrideStack;
	std::vector<int> vnLevelStack;

	vislotStarts.clear();
	vislotStops.clear();

	//vislotStartStack.Push(islot);

	int islotSegLim = (FullyWritten() || m_islotSegLim > -1) ?
		FinalSegLim() :
		WritePos() + 100; // arbitrarily large number

	if (islotStart > islotSegLim)
	{
		// Don't do anything fancy with stuff beyond the end of the segment.
		return 0;
	}

	DirCode dircMarker = kdircNeutral; // none so far
	int nCurrLevel = nTopDirLevel;
	int nMarkerLevel = nTopDirLevel;
	//bool fHitMark = false;
	DirCode dircOverride = kdircNeutral;
	int islot = islotStart;
	Assert(islot <= m_islotWritePos);

	while (true)
	{
		GrSlotState * pslot = m_vpslot[islot];
		DirCode dirc = pslot->DirProcessed();
		bool fIncludeLast = false;
		int nSlotDirLevel;
		if (dirc == kdircLRO || dirc == kdircLRE) // steps X1, X3 in the Unicode bidi algorithm
		{
			vdircMarkerStack.push_back(dircMarker);
			vdircOverrideStack.push_back(dircOverride);
			vnLevelStack.push_back(nCurrLevel);
			dircMarker = dirc;
			dircOverride = (dirc == kdircLRO) ? kdircL : kdircNeutral;
			nSlotDirLevel = (nCurrLevel % 2) ?
				nCurrLevel + 1 : nCurrLevel + 2; // next highest LTR level
			nMarkerLevel = nSlotDirLevel;

			pslot->SetDirProcessed(dircMarker);
			pslot->SetDirLevel(nMarkerLevel);
			pslot->MarkDirProcessed();
		}
		else if (dirc == kdircRLO || dirc == kdircRLE) // steps X2, X4
		{
			vdircMarkerStack.push_back(dircMarker);
			vdircOverrideStack.push_back(dircOverride);
			vnLevelStack.push_back(nCurrLevel);
			dircMarker = dirc;
			dircOverride = (dirc == kdircRLO) ? kdircR : kdircNeutral;
			nSlotDirLevel = (nCurrLevel % 2) ?
				nCurrLevel + 2 : nCurrLevel + 1; // next highest RTL level
			nMarkerLevel = nSlotDirLevel;

			pslot->SetDirProcessed(dircMarker);
			pslot->SetDirLevel(nMarkerLevel);
			pslot->MarkDirProcessed();
		}
		else if (dirc == kdircPDF)
		{
			if (dircMarker == kdircNeutral)
				pslot->SetDirProcessed((nCurrLevel % 2) ? kdircR : kdircL);
			else
				pslot->SetDirProcessed(RightToLeftDir(dircMarker) ? kdircPdfR : kdircPdfL);
			pslot->SetDirLevel(nMarkerLevel);
			pslot->MarkDirProcessed();

			if (vnLevelStack.size())
			{
				dircMarker = vdircMarkerStack.back();
				vdircMarkerStack.pop_back();
				dircOverride = vdircOverrideStack.back();
				vdircOverrideStack.pop_back();
				nMarkerLevel = vnLevelStack.back();
				vnLevelStack.pop_back();
				nSlotDirLevel = nMarkerLevel;
				fIncludeLast = true;
			}
			else // ignore
				nSlotDirLevel = nCurrLevel;
		}
		else
		{
			nSlotDirLevel = GetSlotDirLevel(pengst, islot, nTopDirLevel, nMarkerLevel,
				dircMarker, dircOverride);
			if (nSlotDirLevel == -1)
				return -1; // not enough slots
		}

		Assert(pengst->WhiteSpaceOnly() || nCurrLevel >= nTopDirLevel);
		while (nSlotDirLevel > nCurrLevel)
		{
			vislotStartStack.push_back(islot);
			nCurrLevel++;
		}
		// otherwise...
		while (nTopDirLevel <= nCurrLevel && nSlotDirLevel < nCurrLevel)
		{
			if (vislotStartStack.size())
			{
				int islotStartTmp = vislotStartStack.back();
				vislotStartStack.pop_back();
				vislotStarts.push_back(islotStartTmp);
				vislotStops.push_back(islot - 1); // previous character was the end of the range
			}
			else
				Assert(pengst->WhiteSpaceOnly()); // the space is higher than the segment itself
			nCurrLevel--;
		}
		// In the case of white-space-only segments, the space may be bumped up to a level
		// higher than the segment itself.
		Assert(pengst->WhiteSpaceOnly() || vislotStartStack.size() + nTopDirLevel == (unsigned)nCurrLevel);
		Assert(pengst->WhiteSpaceOnly() || vnLevelStack.size() == vdircMarkerStack.size());
		Assert(pengst->WhiteSpaceOnly() || vdircOverrideStack.size() == vdircMarkerStack.size());

		if (nCurrLevel <= nTopDirLevel)
		{
			if (fIncludeLast)
			{
				// Include in the range to reverse the PDF or the char after the LRM/RLM.
				// (Note: don't use a size_t below: we are decrementing below zero.)
				for (int i = signed(vislotStops.size()) - 1; i >= 0 && vislotStops[i] == islot-1; i--)
				{
					vislotStops[i] = islot;
				}
				islot++;
			}
			break; // reached end of reversible range
		}

		islot++;
		if (islot >= m_islotWritePos)
		{
			if (m_fFullyWritten)
				break; // end of input
			else
				return -1; // not enough slots
		}
		else if (islot >= islotSegLim)
		{
			break; // end of input
		}
	}

	// Reached the end of a reversable range.
	while (vislotStartStack.size() > 0)
	{
		int islotStartTmp = vislotStartStack.back();
		vislotStartStack.pop_back();
		vislotStarts.push_back(islotStartTmp);
		//vislotStops.push_back(min(islot, ReadPos() - 1));
		//vislotStops.push_back(min(islot, max(ReadPos() - 1, islotStartTmp)));
		vislotStops.push_back(min(islot - 1, islotSegLim - 1));
		nCurrLevel--;
	}
	Assert(pengst->WhiteSpaceOnly() || nCurrLevel == nTopDirLevel);
	Assert(vislotStarts.size() == vislotStops.size());

	// The start- and stop-lists were generated in the order that we hit the stops, in other
	// words, from inner to outer. Which is the order in which we need to do the reversals, too.

	return islot;
}


/*----------------------------------------------------------------------------------------------
	Return the direction level of the glyph at the given position, according to the
	Unicode bidi algorithm. Return -1 if there aren't enough slots in the stream to
	figure it out.

	???? How should this handle the reprocessing sub-stream????

	@param ptman
	@param islot			- slot for which information is requested
	@param nOuterLevel		- 0 = left-to-right, 1 = right-to-left
	@param nCurrLevel
	@param dircMarker		- most recent override or embedding marker, if any
								(for interpreting PDF codes)
	@param dircOverride		- override imposed by LRO or RLO
----------------------------------------------------------------------------------------------*/
int GrSlotStream::GetSlotDirLevel(EngineState * pengst, int islot,
	int nOuterLevel, int nCurrLevel, DirCode dircMarker, DirCode dircOverride)
{
//	Assert(m_psstrmReprocess == NULL);

	GrSlotState * pslot = m_vpslot[islot];

	int nDirLevel = pslot->DirLevel();
	if (nDirLevel > -1)
		//	already figured out
		return nDirLevel;

	if (pengst->WhiteSpaceOnly())
	{
		//	White-space-only segments should not have any reordering.
		nDirLevel = pengst->ParaRightToLeft();
		pslot->SetDirLevel(nDirLevel);
		pslot->MarkDirProcessed();
		return nDirLevel;
	}

	DirCode dirc = pslot->DirProcessed();

	if (!pslot->DirHasBeenProcessed() && (WeakDir(dirc) || NeutralDir(dirc)) )
	{
		//	Resolve weak and neutral types; process an entire run of weak types at once,
		//	in several passes.

		int islotWeakRunLim = islot + 1;
		while (true)
		{
			if (islotWeakRunLim >= m_islotWritePos)
			{
				if (!m_fFullyWritten)
					return -1;	// get more slots
				else
					break;
			}
			else if (m_islotSegLim > -1 && islotWeakRunLim >= SegLimIfKnown())
			{
				break;
			}
			if (StrongDir(m_vpslot[islotWeakRunLim]->DirProcessed()))
				//	ignore the line-break; we need the next strong dir after that
				//// && !m_vpslot[islotWeakRunLim]->IsFinalLineBreak(ptman->LBGlyphID()))
			{
				break;
			}
			if (m_vpslot[islotWeakRunLim]->DirProcessed() == kdircPDF &&
				dircMarker != kdircUnknown)
			{
				// A valid PDF is a strong code. TODO: replace kdircPDF with kdircPdfL/R
				break;
			}

			islotWeakRunLim++;
		}

		int islotLp;

		// X6. Handle directional overrides: LRO, RLO.
		if (dircOverride != kdircNeutral)
		{
			for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
			{
				m_vpslot[islotLp]->SetDirProcessed(dircOverride);
				m_vpslot[islotLp]->MarkDirProcessed();
			}
			goto LSetDir;
		}

		DirCode dircLp;
		//DirCode dircTmp;

		//	W1. Non-spacing marks get the type of the previous slot.
		for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
		{
			Assert(!m_vpslot[islotLp]->DirHasBeenProcessed());

			if (m_vpslot[islotLp]->DirProcessed() == kdircNSM)
			{
#ifdef _DEBUG
				if (islotLp <= 0)
				{
					Warn("Pathological case in bidi algorithm");
				}
#endif // _DEBUG
				dircLp = AdjacentNonBndNeutralCode(pengst, islotLp - 1, -1, dircMarker);

				m_vpslot[islotLp]->SetDirProcessed(dircLp);
			}
		}

		//	W2. European numbers are changed to Arabic numbers if the previous strong type
		//	was an Arabic letter.
		DirCode dircPrevStrong = AdjacentStrongCode(pengst, islot - 1, -1, dircMarker);
		if (dircPrevStrong == kdircRArab)
		{
			for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
			{
				dircLp = m_vpslot[islotLp]->DirProcessed();
				if (dircLp == kdircEuroNum)
					m_vpslot[islotLp]->SetDirProcessed(kdircArabNum);
			}
		}

		//	W4. A single European separator between two Latin numbers changes to a Latin number.
		//	A single common separator between two numbers of the same type changes to that
		//	type.
		for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
		{
			dircLp = m_vpslot[islotLp]->DirProcessed();
			if ((dircLp == kdircEuroSep || dircLp == kdircComSep) &&
				islotLp + 1 < m_islotWritePos) // don't test last slot in stream
			{
				DirCode dircPrev = AdjacentNonBndNeutralCode(pengst, islotLp - 1, -1, dircMarker);
				DirCode dircNext = AdjacentNonBndNeutralCode(pengst, islotLp + 1,  1, dircMarker);
				if (dircPrev == dircNext)
				{
					if (dircLp == kdircEuroSep && dircPrev == kdircEuroNum)
					{
						m_vpslot[islotLp]->SetDirProcessed(kdircEuroNum);
					}
					else if (dircLp == kdircComSep &&
						(dircPrev == kdircEuroNum || dircPrev == kdircArabNum))
					{
						m_vpslot[islotLp]->SetDirProcessed(dircPrev);
					}
				}
			}
		}

		//	W5, W6. A sequence of European number terminators adjacent to Latin numbers changes
		//	to Latin numbers.
		//	This appears to be an ambiguity in the official algorithm: do we search backward
		//	first, or forward? For our purposes, it is easier to search backward first.
		for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
		{
			dircLp = m_vpslot[islotLp]->DirProcessed();
			if (dircLp == kdircEuroTerm || dircLp == kdircBndNeutral)
			{
				if (TerminatorSequence(pengst, islotLp - 1, -1, dircMarker) == kdircEuroNum)
					dircLp = kdircEuroNum;
				else
				{
					if (TerminatorSequence(pengst, islotLp + 1, 1, dircMarker) == kdircEuroNum)
						dircLp = kdircEuroNum;
					// else could possibly be unknown if we are at the end of the stream
				}
			}

			//	W6. Separators and terminators change to neutral. (Don't change
			//	boundary neutrals at this point, because that would confuse step Pre-L1.)
			if (dircLp == kdircEuroSep || dircLp == kdircEuroTerm || dircLp == kdircComSep)
			{
				dircLp = kdircNeutral;
			}
			m_vpslot[islotLp]->SetDirProcessed(dircLp);
		}

		//	W7. European numbers are changed to plain left-to-right if the previous strong type
		//	was left-to-right.
		dircPrevStrong = AdjacentStrongCode(pengst, islot - 1, -1, dircMarker);
		if (dircPrevStrong == kdircL)
		{
			for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
			{
				dircLp = (m_vpslot[islotLp]->DirProcessed());
				if (dircLp == kdircEuroNum)
					m_vpslot[islotLp]->SetDirProcessed(kdircL);
			}
		}

		//	Pre-L1. Make a list of all the trailing whitespace characters that need to take
		//	on the direction of the final line break (ie, the top direction). We can't actually
		//	change them yet, because that would confuse the loop below. But we have to
		//	record them now because they might get changed in the loop below.
		std::vector<int> vislotTrailingWS;
		int islotFinalLB = -1;
		for (islotLp = islotWeakRunLim; islotLp-- > islot; )
		{
			if (m_vpslot[islotLp]->IsFinalLineBreak(pengst->TableManager()->LBGlyphID()))
			{
				islotFinalLB = islotLp;
				for (int islotLp2 = islotLp; islotLp2-- > islot; )
				{
					dircLp = m_vpslot[islotLp2]->DirProcessed();
					if (dircLp == kdircWhiteSpace || dircLp == kdircBndNeutral)
						vislotTrailingWS.push_back(islotLp2);
					else
						break;	// hit something other than whitespace or neutral
				}
				break;
			}
		}

		//	N1. A sequence of neutrals takes the direction of the strong surrounding text if
		//	the text on both sides has the same direction. Latin and Arabic numbers are treated
		//	as if they are right-to-left.
		//	(If they are not the same, leave the type as neutral.)
		for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
		{
			dircLp = m_vpslot[islotLp]->DirProcessed();

			//	Change Boundary Neutrals to plain neutrals (must happen after Pre-L1).
			if (dircLp == kdircBndNeutral)
			{
				m_vpslot[islotLp]->SetDirProcessed(kdircNeutral);
				dircLp = kdircNeutral;
			}

			if (NeutralDir(dircLp))
			{
				//	First, look for either a strong code or a number. If we get at least
				//	one strong code and a number with the same direction, use that strong
				//	code.
				DirCode dircPrev = AdjacentStrongCode(pengst, islotLp - 1, -1, dircMarker, true);
				DirCode dircNext = AdjacentStrongCode(pengst, islotLp + 1,  1, dircMarker, true);

				if (dircNext == kdircUnknown)
					return -1;

				if (dircPrev == kdircUnknown)
					dircPrev = dircNext;

				DirCode dircResult = kdircUnknown;
				if (dircNext == kdircNeutral)
				{}
				else if (RightToLeftDir(dircPrev) == RightToLeftDir(dircNext) &&
					(StrongDir(dircNext) || StrongDir(dircPrev)))
				{
					//	Got a strong matching direction.
					dircResult = (StrongDir(dircPrev)) ? dircPrev : dircNext;
				}
				else if (dircNext == kdircPdfL || dircNext == kdircPdfR)
				{
					//	A following PDF flag wins, because it represents the edge of a run.
					dircResult = dircNext;
				}
				else if (StrongDir(dircNext) && StrongDir(dircPrev))
				{	//	We must have text in different directions.
				}
				else
				{
					//	Try again, insisting on strong codes, no numbers.
					if (!StrongDir(dircPrev))
						dircPrev = AdjacentStrongCode(pengst, islotLp - 1, -1, dircMarker, false);
					if (!StrongDir(dircNext))
						dircNext = AdjacentStrongCode(pengst, islotLp + 1,  1, dircMarker, false);
					if (dircPrev == kdircUnknown)
						dircPrev = dircNext;

					if (RightToLeftDir(dircPrev) == RightToLeftDir(dircNext) &&
						(StrongDir(dircNext) || StrongDir(dircPrev)))
					{
						//	Got a strong matching direction.
						dircResult = (StrongDir(dircPrev)) ? dircPrev : dircNext;
					}
				}

				if (dircResult != kdircUnknown)
				{
					dircResult = (RightToLeftDir(dircResult)) ? kdircR : kdircL;
					m_vpslot[islotLp]->SetDirProcessed(dircResult);
				}
				// else leave as neutral
			}
		}

		//	L1. Now change trailing neutrals recorded above to the directionality of
		//	the immediately following line-break, if any.
		if (islotFinalLB != -1)
		{
			DirCode dircLB = m_vpslot[islotFinalLB]->DirProcessed();
			dircLB = (dircLB == kdircLlb) ? kdircL : kdircR;
			for (int iislot = 0; iislot < signed(vislotTrailingWS.size()); iislot++)
			{
				int islotTmp = vislotTrailingWS[iislot];
				m_vpslot[islotTmp]->SetDirProcessed(dircLB);
			}
		}

		//	Mark all the slots processed.
		for (islotLp = islot; islotLp < islotWeakRunLim; islotLp++)
			m_vpslot[islotLp]->MarkDirProcessed();
	}
	else
		m_vpslot[islot]->MarkDirProcessed();

LSetDir:

	//	Final resolution.

	dirc = pslot->DirProcessed();

	if (dirc != kdircLlb && dirc != kdircRlb && dircOverride != kdircNeutral)
	{
		// Enforce whatever is imposed by the override.
		m_vpslot[islot]->SetDirProcessed(dircOverride);
		nDirLevel = nCurrLevel;
	}
	else if ((nCurrLevel % 2) == 0)	// current level is even (left-to-right)
	{
		switch (dirc)
		{
		case kdircNeutral:
		case kdircWhiteSpace:
			nDirLevel = nCurrLevel;
			break;

		case kdircL:
			nDirLevel = nCurrLevel;
			break;

		case kdircLlb:
		case kdircRlb: // all line-breaks have the direction of the paragraph
			nDirLevel = nOuterLevel;
			break;

		case kdircR:
		case kdircRArab:
			nDirLevel = nCurrLevel + 1;
			break;

		case kdircEuroNum:
		case kdircArabNum:
			nDirLevel = nCurrLevel + 2;
			break;

		default:
			Assert(false);
			nDirLevel = nCurrLevel;
		}
	}
	else	// current level is odd (right-to-left)
	{
		switch (dirc)
		{
		case kdircNeutral:
		case kdircWhiteSpace:
			nDirLevel = nCurrLevel;
			break;

		case kdircR:
		//case kdircRlb:
		case kdircRArab:
			nDirLevel = nCurrLevel;
			break;

		case kdircL:
		//case kdircLlb:
		case kdircEuroNum:
		case kdircArabNum:
			nDirLevel = nCurrLevel + 1;
			break;

		case kdircLlb:
		case kdircRlb: // all line-breaks have the direction of the paragraph
			nDirLevel = nOuterLevel;
			break;

		default:
			Assert(false);
			nDirLevel = nCurrLevel;
		}
	}
	pslot->SetDirLevel(nDirLevel);

	return nDirLevel;
}

/*----------------------------------------------------------------------------------------------
	Return the next or previous strong code, kdircNeutral if we don't find one,
	or kdircUnknown if we need more slots to process.

	@param islot				- index of slot of interest
	@param nInc					- +1 to search forward, -1 to search backward
	@param fNumbersAreStrong	- true if we want to treat numbers as having strong
									directionality
	@param dircPDF				- how to interpret PDF codes (from corresponding RLO, etc)
----------------------------------------------------------------------------------------------*/
DirCode GrSlotStream::AdjacentStrongCode(EngineState * pengst, int islot, int nInc,
	DirCode dircPDF, bool fNumbersAreStrong)
{
	if (islot < 0)
		return pengst->InitialStrongDir();

	else if (islot >= m_islotWritePos)
	{
		if (m_fFullyWritten || (m_islotSegLim > -1 && islot >= SegLimIfKnown()))
			return kdircNeutral;
		else
			return kdircUnknown;
	}

	DirCode dirc = m_vpslot[islot]->DirProcessed();
	
	if (dirc == kdircPDF && StrongDir(dircPDF))
		return (RightToLeftDir(dircPDF)) ? kdircPdfR : kdircPdfL;
	if (StrongDir(dirc))
		return dirc;
	if (fNumbersAreStrong && (dirc == kdircEuroNum || dirc == kdircArabNum))
		return dirc;

	return AdjacentStrongCode(pengst, islot + nInc, nInc, dircPDF, fNumbersAreStrong);
}

/*----------------------------------------------------------------------------------------------
	Return kdircEuroNum if there is a terminator sequence starting or ending with a
	European number. Return kdircUnknown if we need more slots to process.
	Return kdircNeutral otherwise.

	@param islot	- index of slot of interest
	@param nInc		- +1 to search forward, -1 to search backward
	@param dircPDF	- how to interpret PDF codes (from corresponding RLO, etc)
----------------------------------------------------------------------------------------------*/
DirCode GrSlotStream::TerminatorSequence(EngineState * pengst, int islot, int nInc,
	DirCode dircPDF)
{
	if (islot < 0)
		return pengst->InitialTermDir();

	else if (islot >= m_islotWritePos)
	{
		if (m_fFullyWritten)
			return kdircNeutral;
		else
			return kdircUnknown;
	}

	DirCode dirc = m_vpslot[islot]->DirProcessed();

	if (dirc == kdircPDF)
		return (RightToLeftDir(dircPDF)) ? kdircPdfR : kdircPdfL;
	else if (dirc == kdircEuroNum)
		//	number found at end of terminator sequence
		return kdircEuroNum;
	else if (dirc == kdircEuroTerm)
		//	yes, this slot is a terminator; are there more?
		return TerminatorSequence(pengst, islot + nInc, nInc, dircPDF);
	else if (dirc == kdircLlb || dirc == kdircRlb)
		//	line-break slot--ignore and keep going
		return TerminatorSequence(pengst, islot + nInc, nInc, dircPDF);
	else if (dirc == kdircBndNeutral)
		//	boundary neutral--ignore and keep going
		return TerminatorSequence(pengst, islot + nInc, nInc, dircPDF);
	else
		//	no number found
		return kdircNeutral;
}

/*----------------------------------------------------------------------------------------------
	Return the previous code that is not a Boundary Neutral. Return Other Neutral if there
	is no such.

	@param islot	- index of slot of interest
	@param nInc		- +1 to search forward, -1 to search backward
	@param dircPDF	- how to interpret PDF codes (from corresponding RLO, etc)
----------------------------------------------------------------------------------------------*/
DirCode GrSlotStream::AdjacentNonBndNeutralCode(EngineState * pengst, int islot, int nInc,
	DirCode dircPDF)
{
	Assert(islot < m_islotWritePos);
	if (islot < 0)
		return kdircNeutral;  // or should we use: ptman->InitialStrongDir() ??

	else if (islot >= m_islotWritePos)
	{
		if (m_fFullyWritten)
			return kdircNeutral;
		else
			return kdircUnknown;
	}

	DirCode dirc = m_vpslot[islot]->DirProcessed();
	
	if (dirc == kdircBndNeutral)
		return AdjacentNonBndNeutralCode(pengst, islot + nInc, nInc, dircPDF);
	else if (dirc == kdircPDF)
		return (RightToLeftDir(dircPDF)) ? kdircPdfR : kdircPdfL;
	else
		return dirc;
}

/*----------------------------------------------------------------------------------------------
	Directionality type functions.
----------------------------------------------------------------------------------------------*/
bool StrongDir(DirCode dirc)
{
	return (dirc == kdircL || dirc == kdircR || dirc == kdircRArab ||
		dirc == kdircLRO || dirc == kdircRLO || dirc == kdircLRE || dirc == kdircRLE ||
		dirc == kdircPdfL || dirc == kdircPdfR);
}

bool WeakDir(DirCode dirc)
{
	return (dirc == kdircEuroNum || dirc == kdircEuroSep || 
		dirc == kdircEuroTerm || dirc == kdircArabNum ||
		dirc == kdircComSep || dirc == kdircBndNeutral || dirc == kdircNSM);
}

bool NeutralDir(DirCode dirc)
{
	return (dirc == kdircWhiteSpace || dirc == kdircNeutral);
}

/*----------------------------------------------------------------------------------------------
	Special values used by the bidi algorithm.
----------------------------------------------------------------------------------------------*/
bool BidiCode(int nUnicode)
{
	switch (nUnicode)
	{
	case knLRM:
	case knRLM:
	case knLRO:
	case knRLO:
	case knLRE:
	case knRLE:
	case knPDF:
		return true;
	default:
		break;
	}
	return false;
}

/*----------------------------------------------------------------------------------------------
	Return true if the directionality should be treated as right-to-left.
	Used for the step of resolving neutrals (N1).
----------------------------------------------------------------------------------------------*/
bool RightToLeftDir(DirCode dirc)
{
	switch (dirc)
	{
	case kdircL:
	case kdircLRO:
	case kdircLRE:
	case kdircPdfL:
	case kdircLlb:
		return false;

	case kdircR:
	case kdircRArab:
	case kdircArabNum:
	case kdircEuroNum:
	case kdircRLO:
	case kdircRLE:
	case kdircPdfR:
	case kdircRlb:
		return true;

	case kdircNeutral:
	case kdircWhiteSpace:
	case kdircComSep:
	case kdircEuroSep:
	case kdircEuroTerm:
	case kdircBndNeutral:
	case kdircNSM:
	case kdircPDF:
		return false;

	default:
		Assert(false);
		return false;
	}
}

/*----------------------------------------------------------------------------------------------
	Copy a slot from prevStream, incrementing the positions.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::CopyOneSlotFrom(GrSlotStream * psstrmPrev)
{
#ifdef _DEBUG
	gid16 chw; chw = psstrmPrev->Peek()->GlyphID();
#endif

	// If we are exactly at the segment boundary, pass the information on to this stream.
 	if (psstrmPrev->AtSegMin())
		SetSegMinToWritePos();
	if (psstrmPrev->AtSegLim())
		SetSegLimToWritePos();

	NextPut(psstrmPrev->NextGet());

	AssertValid();
	psstrmPrev->AssertValid();
}

/*----------------------------------------------------------------------------------------------
	Copy one slot from the input stream to the recipient, which is the output stream.
	Used when reversing ranges of glyphs; caller is responsible for adjusting the read-
	and write-positions of the streams.

	REVIEW: How should this handle the reprocessing sub-stream? Currently this is not a problem
	because this function is only used by the bidi pass, which never has intra-pass
	reprocessing.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::SimpleCopyFrom(GrSlotStream * psstrmI, int islotInput, int islotOutput)
{
	if (signed(m_vpslot.size()) < islotOutput + 1)
	{
		m_vpslot.resize(islotOutput + 1);
		m_vislotPrevChunkMap.resize(islotOutput + 1);
		m_vislotNextChunkMap.resize(islotOutput + 1);
	}

	m_vpslot[islotOutput] = psstrmI->m_vpslot[islotInput];
	m_vislotPrevChunkMap[islotOutput] = -1;
	m_vislotNextChunkMap[islotOutput] = -1;

	Assert(m_fUsedByPosPass);
	if (m_fUsedByPosPass && GotIndexOffset())
		m_vpslot[islotOutput]->SetPosPassIndex(islotOutput - m_cslotPreSeg, m_fInputToPosPass1);

}

/*----------------------------------------------------------------------------------------------
	Return true if there is physical space to add more glyphs, false if the positioning
	has caused them to use all the alotted space. Recipient is assumed to be output
	stream from the final pass.

	@param ptman				- table manager, for handling the positioning
	@param pgg					- graphics device
	@param xsSpaceAllotted		- how much space is available
	@param fWidthIsCharCount	- kludge for test procedures
	@param fIgnoreTrailingWS	- true if we are ignoring trailing white-space (currently
									always true)
	@param twsh					- how the segment is handling trailing white-space
	@param pxsWidth				- return width of stuff so far
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::MoreSpace(GrTableManager * ptman,
	float xsSpaceAllotted, bool fWidthIsCharCount,
	bool fIgnoreTrailingWS, TrWsHandling twsh,
	float * pxsWidth)
{
	Assert(ptman->NumberOfPasses() - 1 == m_ipass);

	if (fWidthIsCharCount)
	{
		//	Used in test procedures
		*pxsWidth = (float)m_islotWritePos;
		return (m_islotWritePos < xsSpaceAllotted);
	}
	else
	{
		*pxsWidth = -1;
		if (!GotIndexOffset())
			return true;
		if (WritePos() <= IndexOffset())
			return true;
		int dislot = MaxClusterSlot(WritePos());
		if (dislot == kNegInfinity || dislot > 0)
			// We're in the middle of processing a cluster--we have to keep processing,
			// so for now assume there is more space.
			return true;

		float xsWidth, xsVisWidth;
		ptman->CalcPositionsUpTo(m_ipass, NULL, false, &xsWidth, &xsVisWidth);

		*pxsWidth = (fIgnoreTrailingWS || twsh == ktwshOnlyWs) ? xsVisWidth : xsWidth;
        return (*pxsWidth < xsSpaceAllotted);
	}
}

/*----------------------------------------------------------------------------------------------
	Work backwards from the prevLineBreak position (or if none, the readPos) to find the
	previous line-break of the given weight or less. Return the position of the inserted
	line-break slot, or -1 if no appropriate break point was found. Recipient should be
	the output of the final line break pass, or if none, the output of the
	glyph-generation pass.

	@param ptman			- table manager, for supplying new slots
	@param islotPrevBreak	- position of previous inserted line break
	@param fInsertedLB		- did we actually insert a line-break glyph before?
	@param islotStartTry	- where to start looking, or -1 for the end of the stream
	@param lb				- (max) break weight to allow
	@param twsh				- how we are handling trailing white-space
	@param islotMin			- first slot that is officially part of the segment (after initial LB)
	@param plbNextToTry		- the best we found
----------------------------------------------------------------------------------------------*/
int GrSlotStream::InsertLineBreak(GrTableManager * ptman,
	int islotPrevBreak, bool fInsertedLB, int islotStartTry,
	LineBrk lb, TrWsHandling twsh, int islotMin,
	LineBrk * plbNextToTry)
{
	EngineState * pengst = ptman->State();

	Assert(!fInsertedLB || islotPrevBreak > -1);

	int ichwSegOffset;
	int islot;
	LineBrk lbFound;
	if (!FindSegmentEnd(ptman, islotStartTry, lb, twsh, islotMin,
		&islot, &ichwSegOffset, &lbFound, plbNextToTry))
	{
		return -1; // didn't find a legal break
	}

	GrSlotState * pslotCopyFeat = pengst->AnAdjacentSlot(m_ipass, islot + 1);
	Assert(pslotCopyFeat);

	if (islotPrevBreak > -1 && fInsertedLB)
	{
		//GrSlotState * pslotOld = m_vpslot[islotPrevBreak];

		m_vpslot.erase(m_vpslot.begin() + islotPrevBreak);
		m_vislotPrevChunkMap.erase(m_vislotPrevChunkMap.begin() + islotPrevBreak);
		m_vislotNextChunkMap.erase(m_vislotNextChunkMap.begin() + islotPrevBreak);
		AdjustPrevStreamNextChunkMap(ptman, islotPrevBreak + 1, -1);
	}
	else
	{
		m_islotReadPos++;
		m_islotWritePos++;
	}

	GrSlotState * pslotNew;
	pengst->NewSlot(ptman->LBGlyphID(), pslotCopyFeat, 0, ichwSegOffset, &pslotNew);

	pslotNew->SetSpecialSlotFlag(kspslLbFinal);
	pslotNew->SetBreakWeight(lbFound);
	pslotNew->SetDirectionality(ptman->RightToLeft() ? kdircRlb : kdircLlb);

	int islotLB = islot + 1;
	m_vpslot.insert(m_vpslot.begin() + islotLB, pslotNew);
	//	Inserting -1 makes it part of a chunk with the previous char/glyph
	m_vislotPrevChunkMap.insert(m_vislotPrevChunkMap.begin() + islotLB, -1);
	m_vislotNextChunkMap.insert(m_vislotNextChunkMap.begin() + islotLB, -1);
	AdjustPrevStreamNextChunkMap(ptman, islotLB, 1);
	// We don't need to adjust the following stream because we are going to unwind it anyway.

	if (m_fUsedByPosPass && GotIndexOffset())
	{
		pslotNew->SetPosPassIndex(islot - m_cslotPreSeg, m_fInputToPosPass1);
		//	Increment the stream index for the following slots.
		for (int islotTmp = islotLB;
			islotTmp < ((islotPrevBreak == -1) ? m_islotWritePos : islotPrevBreak + 1);
			islotTmp++)
		{
			SlotAt(islotTmp)->IncPosPassIndex();
		}
	}

	m_islotSegLim = islotLB + 1;	// after the line-break glyph

	return islotLB;
}

/*----------------------------------------------------------------------------------------------
	Make a segment break that does not correspond to a line break; ie, don't insert
	a line-break glyph.

	Work backwards from the prevLineBreak position (or if none, the readPos) to find the
	previous line-break of the given weight or less. Return the position of the final slot
	in the segment, or -1 if no appropriate break point was found. Recipient should be
	the output of the final line break pass, if any, or the glyph-generation pass.

	@param ptman			- table manager, for supplying new slots
	@param islotPrevBreak	- position of previous inserted line break
	@param fInsertedLB		- did we actually insert a line-break glyph previously; currently
								ignored
	@param islotStartTry	- where to start looking, or -1 for the end of the stream
	@param lb				- (max) break weight to allow
	@param twsh				- how we are handling trailing white-space
	@param islotMin			- first slot that is officially part of the segment (after initial LB)
----------------------------------------------------------------------------------------------*/
int GrSlotStream::MakeSegmentBreak(GrTableManager * ptman,
	int /*islotPrevBreak*/, bool /*fInsertedLB*/, int islotStartTry,
	LineBrk lb, TrWsHandling twsh, int islotMin,
	LineBrk * plbNextToTry)
{
	int ichwSegOffset;
	int islot;
	LineBrk lbFound;
	if (!FindSegmentEnd(ptman, islotStartTry, lb, twsh, islotMin,
		&islot, &ichwSegOffset, &lbFound, plbNextToTry))
	{
		return -1; // didn't find a legal break
	}

	// Review: do we need to delete any previously-inserted line-break glyph from the stream?
	// Doesn't seem like this has been a problem so far.

	m_islotSegLim = islot + 1;	// after the final glyph of this segment

	return islot;
}

/*----------------------------------------------------------------------------------------------
	Work backwards from the prev line break position (or if none, the readPos) to find an
	earlier line-break of the given weight or less.
	Recipient should be the output of the final line break pass, if any, or the output
	of the glyph-generation pass.

	@param ptman			- table manager, for supplying new slots
	@param islotStartTry	- where to start looking, or -1 for the end of the stream
	@param lb				- (max) break weight to allow
	@param twsh				- how we are handling trailing white-space
	@param islotMin			- first slot that is officially part of the segment (after initial LB)
	@param pislot			- slot AFTER which the break will go (not the lim of the segment)
	@param pichwSegOffset	- offset relative to the segment
	@param plbFound			- weight of break to be created
	@param plbNextToTry		- best found, in case we didn't find one at the requested level

	@return Whether or not an appropriate break was found.
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::FindSegmentEnd(GrTableManager * ptman,
	int islotStartTry, LineBrk lb, TrWsHandling twsh, int islotMin,
	int * pislot, int * pichwSegOffset, LineBrk * plbFound,
	LineBrk * plbNextToTry)
{
	Assert(AssertValid());
	Assert(m_ipass == ptman->NumberOfLbPasses());

	if (islotStartTry < 0)
		return false;

	*pislot = islotStartTry;
	GrSlotState * pslot = m_vpslot[*pislot];
	*plbNextToTry = klbClipBreak; // worst break

	ptman->State()->SetRemovedTrWhiteSpace(false);

	while (twsh != ktwshOnlyWs)	// in white-space only case, ignore lb values
	{
		if (*pislot < islotMin)
		{
			return false;
		}
		*plbFound = LineBrk(pslot->m_lb);
		// Sanity check
		if (abs(*plbFound) > klbClipBreak)
		{
//			Assert(false);
			Warn("Unusually large breakweight");
			*plbFound = (LineBrk)((*plbFound < 0) ? -klbClipBreak : klbClipBreak);
		}
		*plbNextToTry = (LineBrk)(min(static_cast<int>(*plbNextToTry), abs(*plbFound)));
		*pichwSegOffset = pslot->SegOffset();

		if (int(*plbFound) >= 0 && *plbFound <= lb)
			break;

		--(*pislot);
		if (*pislot < 0)
		{
			return false;
		}
		pslot = m_vpslot[*pislot];

		if (*pislot == 0 && pslot->IsInitialLineBreak(ptman->LBGlyphID())) {
			// Can't break after the initial LB; that would produce an empty segment.
			return false;
		}

		if (int(*plbFound) <= 0 && ((int)*plbFound * -1) <= lb)
		{
			*plbFound = LineBrk(int(*plbFound) * -1);
			break;
		}
	}

	// Found a good break.

	if (twsh == ktwshOnlyWs)
	{
		//	white-space-only segment
		while (pslot->Directionality() != kdircWhiteSpace)
		{
			--(*pislot);
			if (*pislot < 0)
				return false;
			pslot = m_vpslot[*pislot];
			*pichwSegOffset = pslot->SegOffset();
		}
	}
	else if (twsh == ktwshNoWs)
	{
		// no trailing white-space: remove it
		while (pslot->Directionality() == kdircWhiteSpace)
		{
			--(*pislot);
			if (*pislot < 0)
				return false;
			pslot = m_vpslot[*pislot];
			*pichwSegOffset = pslot->SegOffset();
			ptman->State()->SetRemovedTrWhiteSpace(true);
		}
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Return true if the stream (which should be the output of the last linebreak pass) has 
	a better break at some earlier point than the one we found.

	@param islotBreak		- position of inserted or final break point in stream, or index
								of last valid slot
----------------------------------------------------------------------------------------------*/
bool GrSlotStream::HasEarlierBetterBreak(int islotBreak, LineBrk lbFound, gid16 chwLB)
{
	Assert(AssertValid());

	int islot = islotBreak;
	if (SlotAt(islot)->IsFinalLineBreak(chwLB))
		islot--;

	if (lbFound == klbNoBreak)
		lbFound = (LineBrk)SlotAt(islot)->BreakWeight();

	while (islot >= 0)
	{
		if (SlotAt(islot)->IsInitialLineBreak(chwLB))
			return false;
		if (SlotAt(islot)->BreakWeight() < lbFound)
			return true;
		islot--;
	}

	return false;
}

/*----------------------------------------------------------------------------------------------
	Make sure the stream contains a local instance of the given slot, not a copy used by
	a previous pass.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::EnsureLocalCopy(GrTableManager * ptman, GrSlotState * pslot,
	GrSlotStream * psstrmIn)
{
	//Assert(m_vpslot[islot + m_cslotPreSeg] == pslot);
	Assert(pslot->PassModified() <= m_ipass);

	if (pslot->PassModified() < m_ipass)
	{
		int islot = pslot->PosPassIndex();
		Assert(SlotAtPosPassIndex(islot) == pslot);
		GrSlotState * pslotNew;
		ptman->State()->NewSlotCopy(pslot, m_ipass, &pslotNew);
		m_vpslot[islot + m_cslotPreSeg] = pslotNew;

		psstrmIn->ReplaceSlotInReprocessBuffer(pslot, pslotNew);
	}
}

/*----------------------------------------------------------------------------------------------
	A slot has been replace within the coorsponding output stream. Make sure the reprocess
	buffer of this stream constains the same slot.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::ReplaceSlotInReprocessBuffer(GrSlotState * pslotOld, GrSlotState * pslotNew)
{
	if (m_islotReprocPos > -1)
	{
		for (size_t islot = 0; islot < m_vpslotReproc.size(); islot++)
		{
			if (m_vpslotReproc[islot] == pslotOld)
				m_vpslotReproc[islot] = pslotNew;
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Inserting a line break can potentially alter the directionality of preceeding glyphs.
	Zap the information that has been calculated.

	@param islotLB		- index of the inserted final line-break glyph
----------------------------------------------------------------------------------------------*/
void GrSlotStream::ZapCalculatedDirLevels(int islotLB)
{
	int islot;
	for (islot = m_islotWritePos; islot-- > islotLB; )
		m_vpslot[islot]->ZapDirLevel();

	for (islot = islotLB; islot-- > 0; )
	{
		if (StrongDir(m_vpslot[islot]->Directionality()))
			//	We've found a strong direction code; don't need to zap beyond this point.
			return;

		m_vpslot[islot]->ZapDirLevel();
	}
}

/*----------------------------------------------------------------------------------------------
	Return the slot index of the last slot in the cluster for the just-processed chunk,
	relative to the last slot in the chunk.

	Return kNegInfinity if there are not enough slots in the stream to tell. This can
	happen while we are in the middle of processing a cluster.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::MaxClusterSlot(int islotChunkMin, int islotChunkLim)
{	
	Assert(islotChunkLim > islotChunkMin);

	if (!m_fUsedByPosPass)
		return 0;	// no clusters yet

	int islotRet = SlotAt(islotChunkLim - 1)->PosPassIndex();

	for (int islot = islotChunkMin; islot < islotChunkLim; islot++)
	{
		GrSlotState * pslot = SlotAt(islot);

		if (m_cslotPreSeg == -1)
			return kNegInfinity;
		if (!HasSlotAtPosPassIndex(pslot->AttachRootPosPassIndex()))
			return kNegInfinity;

		GrSlotState * pslotBase = pslot->Base(this);

		int dislotOffset = pslotBase->LastLeafOffset(this);
		if (dislotOffset == kNegInfinity)
			return kNegInfinity;
		islotRet = max(islotRet, pslotBase->PosPassIndex() + dislotOffset);
	}

	//	Make it relative to the last slot in the chunk.
	islotRet -= SlotAt(islotChunkLim - 1)->PosPassIndex();

	return islotRet;
}

/*----------------------------------------------------------------------------------------------
	Return the break weight of the given slot, which should be a line-break.
	OBSOLETE??
----------------------------------------------------------------------------------------------*/
#ifdef NDEBUG
LineBrk GrSlotStream::BreakWeightAt(gid16 /*chwLB*/, int islot)
#else
LineBrk GrSlotStream::BreakWeightAt(gid16 chwLB, int islot)
#endif
{	
	GrSlotState * pslot = GetSlotAt(islot);
	Assert(pslot->IsLineBreak(chwLB));
	return LineBrk(pslot->m_lb);
}

/*----------------------------------------------------------------------------------------------
	Append an initial or final line break of the given weight to the stream.

	@param ptman			- table manager, for generating new slot
	@param lb				- break weight for the line-break glyph
	@param dirc				- directionality code for the line-break glyph
	@param islotLB			- where to insert, or -1 if at the end
	@param fInitial			- is this an initial line-break, or a final break?
	@param ichwSegOffset	- offset relative to the beginning of the segment
----------------------------------------------------------------------------------------------*/
void GrSlotStream::AppendLineBreak(GrTableManager * ptman, GrCharStream * pchstrm,
	LineBrk lb, DirCode dirc, int islotLB, bool fInitial, int ichwSegOffset)
{
	EngineState * pengst = ptman->State();

	Assert(AssertValid());
///	Assert(m_islotReprocPos == -1);

	Assert(islotLB == -1 || fInitial);
	if (islotLB == -1)
		islotLB = m_islotWritePos;

//	int islot = m_islotWritePos;

	GrSlotState * pslotCopyFeat = pengst->AnAdjacentSlot(m_ipass, islotLB);
	GrSlotState * pslotNew;
	if (pslotCopyFeat)
		pengst->NewSlot(ptman->LBGlyphID(), pslotCopyFeat, 0, ichwSegOffset, &pslotNew);
	else
	{
		GrFeatureValues fval;
		pchstrm->CurrentFeatures(ptman, &fval);
		pengst->NewSlot(ptman->LBGlyphID(), fval, 0, ichwSegOffset, -1, &pslotNew);
	}

	pslotNew->m_lb = sdata8(lb);
	pslotNew->SetSpecialSlotFlag(fInitial ? kspslLbInitial : kspslLbFinal);
	//pslotNew->m_fInitialLB = fInitial; // TODO: remove
	pslotNew->SetDirectionality(dirc);

	m_vpslot.insert(m_vpslot.begin() + islotLB, pslotNew);
	// Make it part of a chunk with previous char/glyph
	m_vislotPrevChunkMap.insert(m_vislotPrevChunkMap.begin() + islotLB, -1);
	m_vislotNextChunkMap.insert(m_vislotNextChunkMap.begin() + islotLB, -1);
	if (m_ipass > 0)
		AdjustPrevStreamNextChunkMap(ptman, islotLB, 1);

	m_islotWritePos++;

	if (m_fUsedByPosPass && GotIndexOffset())
	{
		pslotNew->SetPosPassIndex(islotLB - m_cslotPreSeg, m_fInputToPosPass1);
		//	Increment the stream index for the following slots.
		for (int islotTmp = islotLB + 1; islotTmp < m_islotWritePos; islotTmp++)
		{
			SlotAt(islotTmp)->IncPosPassIndex();
		}
	}

	if (fInitial)
		m_islotSegMin = islotLB;		// just before the LB
	else
		m_islotSegLim = islotLB + 1;	// just after the LB
}

/*----------------------------------------------------------------------------------------------
	Return the index of the line break glyph within the specified range, or -1 if none.
	Recipient is functioning as the output stream.

	@param chwLB		- glyph ID being used for line-break glyphs
	@param islotMin/Lim	- range to search
----------------------------------------------------------------------------------------------*/
int GrSlotStream::FindFinalLineBreak(gid16 chwLB, int islotMin, int islotLim)
{
	for (int islot = islotMin; islot < islotLim; ++islot)
	{
		if (m_vpslot[islot]->IsFinalLineBreak(chwLB))
			return islot;
	}
	return -1;
}

/*----------------------------------------------------------------------------------------------
	A slot has been inserted (nInc == 1) or deleted (nInc == -1). Adjust the next-chunk map
	of the previous stream to match.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::AdjustPrevStreamNextChunkMap(GrTableManager * ptman, int islotMod, int nInc)
{
	if (m_ipass == 0)
		return; // no previous stream to adjust

	int islotTmp = max(islotMod - 5, 0);
	while (islotTmp > 0 && m_vislotPrevChunkMap[islotTmp] == -1)
		islotTmp--;

	ptman->InputStream(m_ipass)->AdjustNextChunkMap(
		((islotTmp <= 0) ? 0 : m_vislotPrevChunkMap[islotTmp]),
		islotMod, nInc);
}

/*----------------------------------------------------------------------------------------------
	An insertion or deletion has occurred at the given location in the following stream
	(specifically of an line-break glyph). Adjust the next-chunk-map accordingly so
	the indices still match.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::AdjustNextChunkMap(int islotMin, int islotInsOrDel, int nInc)
{
	for (int islot = islotMin; islot < m_islotWritePos; islot++)
	{
		if (m_vislotNextChunkMap[islot] != -1 && m_vislotNextChunkMap[islot] >= islotInsOrDel)
			m_vislotNextChunkMap[islot] += nInc;
	}
}

/*----------------------------------------------------------------------------------------------
	Unwind the read position of the stream, so that the following pass will begin
	reading from the new position. The recipient is functioning as the input stream.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::UnwindInput(int islotNewPos, bool fPreBidiPass)
{
	Assert(m_islotReprocPos == -1);
	Assert(islotNewPos <= m_islotReadPos);

	int islot;
	for (islot = islotNewPos; islot < m_islotReadPos; ++islot)
		m_vislotNextChunkMap[islot] = -1;

	m_islotReadPos = islotNewPos;
	m_islotReadPosMax = m_islotReadPos;

	Assert(m_islotReadPos <= m_islotWritePos);
	if (fPreBidiPass)
	{
		for (islot = m_islotReadPos; islot < m_islotWritePos; ++islot)
			SlotAt(islot)->ZapDirLevel();
	}
}

/*----------------------------------------------------------------------------------------------
	Unwind the write position of the stream, so that the pass will begin writing
	at the new position. The recipient is functioning as the output stream
----------------------------------------------------------------------------------------------*/
void GrSlotStream::UnwindOutput(int islotNewPos, bool fOutputOfPosPass)
{
	Assert(islotNewPos <= m_islotWritePos);

	int islot;
	for (islot = islotNewPos; islot < m_islotWritePos; ++islot)
	{
		m_vislotPrevChunkMap[islot] = -1;

		if (!fOutputOfPosPass && m_fUsedByPosPass)
			//	This stream is the input to the first positioning pass, ie, the output
			//	of the last sub pass or the bidi pass. Zap the stream indices,
			//	since unwinding this pass could cause them to be invalid due to a later
			//	reordering.
			SlotAt(islot)->ZapPosPassIndex();
	}

	m_islotWritePos = islotNewPos;

	m_fFullyWritten = false;

	if (m_islotSegMin > m_islotWritePos)
		m_islotSegMin = -1;
	if (m_islotSegLim > m_islotWritePos)
		m_islotSegLim = -1;
}

/*----------------------------------------------------------------------------------------------
	We've just created a new chunk, with the recipient as the output stream. The beginning
	of the chunk is passed as 'islotOutput' and the end of the chunk is assumed to be the
	write-position of the recipient. Store the mappings into the corresponding
	input position.

	@param islotInputMin	- beginning of chunk in input stream
	@param islotOutputMin	- beginning of chunk in output stream (recipient)
	@param islotInputLim	- end of chunk in input stream (currently not used)
	@param fSkipChunkStart	- if true, either we have reprocessed part of a
								previously created chunk,
								or there was no input consumed;
								in either case, don't record the beginning of the chunk,
								just make this chunk be part of the previous
	@param fBackingUp		- this chunk results in the stream position moving backwards,
								so clear anything we're backing over
----------------------------------------------------------------------------------------------*/
void GrSlotStream::MapInputChunk(int islotInputMin, int islotOutputMin, int /*islotInputLim*/,
	bool fSkipChunkStart, bool fBackingUp)
{
	Assert(AssertValid());
	Assert(islotOutputMin >= -1);

	if (!fSkipChunkStart)
	{
		Assert(islotOutputMin >= 0);
		if (islotOutputMin >= 0) // just in case an invalid value was passed
			m_vislotPrevChunkMap[islotOutputMin] = islotInputMin;
	}

	int islot;
	for (islot = max(0, islotOutputMin + 1); islot < m_islotWritePos; ++islot)
		m_vislotPrevChunkMap[islot] = -1;	// not a chunk boundary

//	if (fBackingUp && m_islotWritePos < m_vislotPrevChunkMap.Size())
//		m_vislotPrevChunkMap[m_islotWritePos] = -1;

	if (fBackingUp)
		for (islot = m_islotWritePos; islot < signed(m_vislotPrevChunkMap.size()); islot++)
			m_vislotPrevChunkMap[islot] = -1;

//	m_vislotPrevChunkMap[m_islotWritePos] = islotInputLim;
}

/*----------------------------------------------------------------------------------------------
	Return the length of the final chunk, where the recipient is the input stream.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::LastNextChunkLength()
{
	int cslotRet = 1;
	for (int islot = m_islotReadPos; islot-- > 0; )
	{
		if (m_vislotNextChunkMap[islot] != -1)
			return cslotRet;
		cslotRet++;
	}
	return cslotRet + 1;
}

/*----------------------------------------------------------------------------------------------
	We've just created a new chunk, with the recipient as the input stream. The beginning of
	the chunk is passed as 'islotInputMin' and the end of the chunk is assumed to be the
	read-position of the recipient. Store the mappings into the corresponding
	output position.

	@param islotOutputMin	- beginning of chunk in output stream
	@param islotInputMin	- beginning of chunk in input stream (recipient)
	@param islotOutputLim	- end of chunk in output stream (currently not used)
	@param fSkipChunkStart	- if true, either we have reprocessed part of a
								previously created chunk,
								or there was no output generated;
								in either case, don't record the beginning of the chunk,
								just make this chunk be part of the previous
	@param fBackingUp		- this chunk results in the stream position moving backwards,
								so clear anything we're backing over
----------------------------------------------------------------------------------------------*/
void GrSlotStream::MapOutputChunk(int islotOutputMin, int islotInputMin, int /*islotOutputLim*/,
	bool fSkipChunkStart, int cslotReprocess, bool fBackingUp)
{
	Assert(AssertValid());

	// Note: islotInputMin can be less than -1 if there is a large reprocess buffer near the
	// beginning of the input stream. In which case, fSkipChunk start should be true and
	// cslotReprocess large enough to keep any of the stuff below from having any problems.
	//Assert(islotInputMin >= -1);

	if (!fSkipChunkStart)
	{
		Assert(islotInputMin >= 0);
		if (islotInputMin >= 0) // just in case an invalid value was passed
			m_vislotNextChunkMap[islotInputMin] = islotOutputMin;
	}

	int islot;
	for (islot = max(0, islotInputMin + 1 + cslotReprocess); islot < m_islotReadPos; ++islot)
		m_vislotNextChunkMap[islot] = -1;	// not a chunk boundary

//	if (fBackingUp && m_islotReadPos < m_vislotNextChunkMap.Size())
//		m_vislotNextChunkMap[m_islotReadPos] = -1;

	if (fBackingUp)
		for (islot = m_islotReadPos; islot < signed(m_vislotNextChunkMap.size()); islot++)
			m_vislotNextChunkMap[islot] = -1;

//	m_vislotNextChunkMap[m_islotReadPos] = islotOutputLim;
}

/*----------------------------------------------------------------------------------------------
	Ensure that the chunk maps for a pair of streams match properly. The recipient is
	the input stream.
----------------------------------------------------------------------------------------------*/
#ifdef _DEBUG
void GrSlotStream::AssertChunkMapsValid(GrSlotStream * psstrmOut)
#else
void GrSlotStream::AssertChunkMapsValid(GrSlotStream *)
#endif
{
#ifdef _DEBUG
	GrSlotStream * psstrmIn = this;

	Assert(m_ipass == 0 || (m_ipass + 1 == psstrmOut->m_ipass));

	int islotMapNext;
	int islotMapPrev;
	int islot;
	for (islot = 0; islot < m_islotReadPos; ++islot)
	{
		islotMapNext = psstrmIn->ChunkInNext(islot);
		Assert(islotMapNext != -2);
		if (islotMapNext != -1)
		{
			islotMapPrev = psstrmOut->ChunkInPrev(islotMapNext);
			if (islotMapPrev == -2)
			{
				Assert(islot == psstrmOut->WritePos());
			}
			else
			{
				Assert(islot == islotMapPrev);
			}
		}
	}

	for (islot = 0; islot < psstrmOut->m_islotWritePos; ++islot)
	{
		islotMapPrev = psstrmOut->ChunkInPrev(islot);
		Assert(islotMapPrev != -2);
		if (islotMapPrev != -1)
		{
			islotMapNext = psstrmIn->ChunkInNext(islotMapPrev);
			if (islotMapNext == -2)
			{
				Assert(islot == psstrmIn->ReadPos());
			}
			else
			{
				Assert(islot == islotMapNext);
			}
		}
	}
#endif // _DEBUG
}

/*----------------------------------------------------------------------------------------------
	Ensure that corresponding items in the streams of a positioning pass have matching
	stream indices. The recipient is the output stream.
----------------------------------------------------------------------------------------------*/
#ifdef _DEBUG
void GrSlotStream::AssertStreamIndicesValid(GrSlotStream * psstrmIn)
#else
void GrSlotStream::AssertStreamIndicesValid(GrSlotStream *)
#endif
{
#ifdef _DEBUG
	if (!GotIndexOffset())
		return;

	Assert(m_fUsedByPosPass);

	for (int islot = 0; islot < WritePos(); islot++)
	{
		GrSlotState * pslotOut = SlotAt(islot);
		GrSlotState * pslotIn =
			psstrmIn->SlotAt(islot + psstrmIn->IndexOffset() - IndexOffset());
		Assert(pslotOut->PosPassIndex() == pslotIn->PosPassIndex());
		Assert(pslotOut->HasAsPreviousState(pslotIn));
	}
#endif // _DEBUG
}

/*----------------------------------------------------------------------------------------------
	Ensure that the roots of all attachments made in this chunk are present
	in the output stream. (Currently the compiler ensures this by making it an error
	to write rules that don't do this.)
----------------------------------------------------------------------------------------------*/
#ifdef _DEBUG
void GrSlotStream::AssertAttachmentsInOutput(int islotMin, int islotLim)
#else
void GrSlotStream::AssertAttachmentsInOutput(int, int)
#endif
{
#ifdef _DEBUG
	for (int islot = islotMin; islot < islotLim; islot++)
	{
		GrSlotState * pslotOut = SlotAt(islot);
		int dislot = pslotOut->AttachTo();
		Assert(islotMin <= islot + dislot);
		Assert(islot + dislot < islotLim);
	}
#endif // _DEBUG
}

/*----------------------------------------------------------------------------------------------
	Answer the slot index corresponding to the start of the chunk (as mapped to the following
	stream).
	ENHANCE: make more robust.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::ChunkInNextMin(int islot)
{
	int islotRet = islot;
	while (m_vislotNextChunkMap[islotRet] == -1 && islotRet > 0)
		--islotRet;
	return islotRet;
}

/*----------------------------------------------------------------------------------------------
	Answer the slot index corresponding to the end of the chunk (as mapped to the following
	stream).
	ENHANCE: make more robust.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::ChunkInNextLim(int islot)
{
	int islotRet = islot + 1;
	if (islotRet == m_islotReadPos)
		return islot;
	while (m_vislotNextChunkMap[islotRet] == -1 && islotRet < m_islotReadPos)
	{
		++islotRet;
		//Assert(islotRet < signed(m_vislotNextChunkMap.size()));
        if (islotRet >= signed(m_vislotNextChunkMap.size()))
 		{
 			islotRet = m_vislotNextChunkMap.size() - 1;
 			break;
 		}
	}
	return islotRet;
}

/*----------------------------------------------------------------------------------------------
	Skip over the given number slots. This is used to resync the streams when we
	are restarting a new segment. Specifically the output stream informs the
	input stream of the number of slots to skip to get the boundary for a chunk that
	the output finds interesting.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::ResyncSkip(int cslot)
{
	Assert(AssertValid());
	m_islotReadPos += cslot;
	m_cslotSkippedForResync = cslot;
	// Should never skip past the beginning of the line.
	Assert(m_islotSegMin == -1 || m_islotReadPos <= m_islotSegMin);
}

/*----------------------------------------------------------------------------------------------
	Record the number of slots in the stream that are previous to the official start of the
	segment.
----------------------------------------------------------------------------------------------*/
void GrSlotStream::CalcIndexOffset(GrTableManager * /*ptman*/)
{
	if (GotIndexOffset())
		return; // already figured it

	if (m_islotSegMin > -1)
	{
		m_cslotPreSeg = m_islotSegMin;
		if (m_fUsedByPosPass)
		{
			for (int islotPre = 0; islotPre < WritePos(); islotPre++)
			{
				SlotAt(islotPre)->SetPosPassIndex(islotPre - m_cslotPreSeg,
					m_fInputToPosPass1);
			}
		}
	}
	// else can't figure it out yet

	// Old code:
#if 0
	gid16 chwLB = ptman->LBGlyphID();

	for (int islot = 0; islot < WritePos(); islot++)
	{
		if (SlotAt(islot)->IsLineBreak(chwLB))
		{
			if (SlotAt(islot)->IsInitialLineBreak(chwLB))
			{
				m_cslotPreSeg = islot;
			}
			else
			{
				//	Otherwise, we hit the final line break, which is kind of strange, because
				//	in this case there is no initial line break, and we should have set
				//	m_cslotPreSeg to 0 immediately when it was initialized.
				Assert(false);
				m_cslotPreSeg = 0;
			}

			if (m_fUsedByPosPass)
			{
				for (int islotPre = 0; islotPre < WritePos(); islotPre++)
				{
					SlotAt(islotPre)->SetPosPassIndex(islotPre - m_cslotPreSeg,
						m_fInputToPosPass1);
				}
			}
			return;
		}

	}
	//	No line breaks yet, so no way to tell.
#endif
}

/*----------------------------------------------------------------------------------------------
	Set the positions in the streams for the next rule. The recipient is the output stream.
	If we are moving backwards, copy part of the output stream back to (a temporary buffer
	in) the input stream, so that we can reprocess it.

	@param cslotArg			- how far to skip forward or back; most commonly zero
----------------------------------------------------------------------------------------------*/
void GrSlotStream::SetPosForNextRule(int cslotArg, GrSlotStream * psstrmIn,
	bool fOutputOfPosPass)
{
	Assert(AssertValid());

	int cslot = cslotArg;
	//	Can't reprocess what has already been read by the next pass. MaxBackup should allow
	//	the flexibility needed, but if not, we fix it here.
	if (m_islotWritePos + cslot < m_islotReadPos)
		cslot = m_islotReadPos - m_islotWritePos;

	Assert(psstrmIn->m_ipass == m_ipass-1);

	if (cslot >= 0)
	{
		//	Skipping forward.
		Assert(cslot <= psstrmIn->SlotsPendingInContext());
		for (int islot = 0; islot < cslot; islot++)
			CopyOneSlotFrom(psstrmIn);
	}
	else
	{
		//	Reprocessing.

		//	Save corresponding positions before doing the back-up.
		int islotReadPosInSave = psstrmIn->ReadPosForNextGet();
		int islotWritePosOutSave = this->WritePos();

		std::vector<GrSlotState*> vpslotTmp;
		int islot;
		if (psstrmIn->SlotsToReprocess() > 0)
		{
			//	Stick any slots still to reprocess in a temporary buffer.
			for (islot = psstrmIn->m_islotReprocPos;
				islot < signed(psstrmIn->m_vpslotReproc.size());
				islot++)
			{
				vpslotTmp.push_back(psstrmIn->m_vpslotReproc[islot]);
			}
		}

		psstrmIn->ClearReprocBuffer();
		psstrmIn->m_islotReprocLim = psstrmIn->m_islotReadPos;

		for (islot = cslot; islot < 0; islot++)
			psstrmIn->m_vpslotReproc.push_back(m_vpslot[m_islotWritePos+islot]);
		for (islot = 0; islot < signed(vpslotTmp.size()); islot++)
			psstrmIn->m_vpslotReproc.push_back(vpslotTmp[islot]);
		psstrmIn->m_islotReprocPos = 0;

		if (!fOutputOfPosPass && m_fUsedByPosPass)
		{
			//	Last substitution pass; zap the stream indices, since they may
			//	now be invalid.
			for (islot = 0; islot < signed(psstrmIn->m_vpslotReproc.size()); islot++)
				psstrmIn->m_vpslotReproc[islot]->ZapPosPassIndex();
		}

		// If either the min or lim is in the middle of the reprocess buffer,
		// adjust as necessary to match where it is in the output stream.
		int islotSegMinIn = psstrmIn->SegMin();
		if (islotSegMinIn > -1 && 
			psstrmIn->ReadPosForNextGet() <= islotSegMinIn &&
			islotSegMinIn < islotReadPosInSave)
		{
			Assert(this->SegMin() > -1);
			int dislotIn = islotReadPosInSave - islotSegMinIn;
			int dislotOut = islotWritePosOutSave - SegMin();
			psstrmIn->SetSegMin(islotSegMinIn + dislotIn - dislotOut, true);
			if (psstrmIn->m_cslotPreSeg > psstrmIn->SegMin())
				psstrmIn->m_cslotPreSeg = -1;
		}
		int islotSegLimIn = psstrmIn->SegLimIfKnown();
		if (islotSegLimIn > -1 && 
			psstrmIn->ReadPosForNextGet() <= islotSegLimIn &&
			islotSegLimIn < islotReadPosInSave)
		{
			int islotSegLimOut = this->SegLimIfKnown();
			Assert(islotSegLimOut > -1);
			int dislotIn = islotReadPosInSave - islotSegLimIn;
			int dislotOut = islotWritePosOutSave - islotSegLimOut;
			psstrmIn->SetSegLim(islotSegLimIn + dislotIn - dislotOut);
		}

		m_islotWritePos += cslot;

		if (m_islotSegMin >= m_islotWritePos)
			m_islotSegMin = -1;
		if (m_islotSegLim >= m_islotWritePos)
			m_islotSegLim = -1;
	}
}

/*----------------------------------------------------------------------------------------------
	If a rule was run over a LB glyph, set the appropriate flag in the table manager.
	CURRENTLY NOT USED
----------------------------------------------------------------------------------------------*/
void GrSlotStream::SetLBContextFlag(GrTableManager * ptman, int islotStart)
{
	gid16 chwLB = ptman->LBGlyphID();
	for (int islot = islotStart; islot < WritePos(); islot++)
	{
		GrSlotState * pslot = SlotAt(islot);
		if (pslot->IsInitialLineBreak(chwLB))
			ptman->State()->SetStartLineContext(true);
		if (pslot->IsFinalLineBreak(chwLB))
			ptman->State()->SetEndLineContext(true);
	}
}

/*----------------------------------------------------------------------------------------------
	Return the number of slots that have been output by the current pass but must be
	reprocessed by this same pass. The recipient is serving as the input stream.
----------------------------------------------------------------------------------------------*/
int GrSlotStream::SlotsToReprocess()
{
	Assert(AssertValid());

	if (m_islotReprocPos == -1)
		return 0;
	return (m_vpslotReproc.size() - m_islotReprocPos);
}

/*----------------------------------------------------------------------------------------------
	Get the input at the given slot, that is, the original input, ignoring the issue of
	reprocessing.
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotStream::GetSlotAt(int islot)
{
	return m_vpslot[islot];
}

/*----------------------------------------------------------------------------------------------
	In the middle of running a pass, return the given slot to use in processing.
	Read it from the reprocess buffer if appropriate, or for slots previous to the current
	position, read from the output stream (psstrmNext).

	psstrmNext may be NULL when processing is complete, therefore we only have one stream to
	deal with.
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotStream::MidPassSlotAt(int islot, GrSlotStream * psstrmNext)
{
	int islotInput = islot - ReadPosForNextGet() + 1; // +1 because RuleInputSlot takes 0 to mean the previously read slot
	GrSlotState * pslot = RuleInputSlot(islotInput, psstrmNext);
	return pslot;
}

/*----------------------------------------------------------------------------------------------
	Return the "current" input item from the rule's perspective, ie, the last slot read.
	So dislotOffset = 0 means not the slot at the read position but one slot earlier.

	If dislotOffset is less the the original read position that was in effect at the
	beginning of the rule, we need to read from the output stream, which has the
	up-to-date values. See the class comment for more details.

	@param dislotOffset	- offset from current stream position
	@param fNullOkay	- true if it's okay to return NULL in the situation where we're asking
							for something before the beginning of the stream
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotStream::RuleInputSlot(int dislotOffset, GrSlotStream * psstrmOutput,
	bool fNullOkay)
{
	Assert(m_islotRuleStartRead <= m_islotReadPos);

	if (dislotOffset > 0)
		return Peek(dislotOffset - 1);

	int cslotOffsetBack = dislotOffset * -1;

	if (m_islotReprocLim > -1)
	{
		//	There is a reprocess buffer.

		//	Position when starting the rule should never be earlier than the 
		//	beginning of the reprocess buffer.
		Assert(m_islotRuleStartRead >= m_islotReprocLim - signed(m_vpslotReproc.size()));

		// number of items following the reprocess buffer:
		int cslotPostReproc = m_islotReadPos - m_islotReprocLim;

		if (cslotOffsetBack >= cslotPostReproc)
		{
			// number of items in the reprocess buffer that were valid when the rule
			// was started:
			int cslotValidReproc = m_islotReprocLim - m_islotRuleStartRead;

			if (cslotOffsetBack >= cslotPostReproc + cslotValidReproc)
			{
				// Read from the output stream. (Remember that PeekBack works relative to
				// the rule-start write position, not the current write position.)
				int dislotTmp = dislotOffset - 1 + cslotPostReproc
					+ cslotValidReproc - SlotsToReprocess();
				Assert(dislotTmp < 0);
				return psstrmOutput->PeekBack(dislotTmp);
			}
			else
			{
				if (m_islotReprocPos > -1)
				{
					//	Current read pos could be inside reprocess buffer.
					Assert(cslotPostReproc == 0);
					int islotStartReadReprocBuf = m_vpslotReproc.size() - cslotValidReproc;
					Assert(islotStartReadReprocBuf >= 0);
					int islotInReprocBuf = m_islotReprocPos - cslotOffsetBack - 1;
					if (islotInReprocBuf < islotStartReadReprocBuf)
					{
						//	Return a slot from before the reprocess buffer, from the
						//	output stream (which is serving as our input stream for precontext
						//	items).
						return psstrmOutput->PeekBack(islotInReprocBuf - islotStartReadReprocBuf);
					}
					else if (islotInReprocBuf < 0)
					{
						Assert(false);
						//int islotReprocMin = m_islotReprocLim - m_vpslotReproc.Size();
						//return m_vpslot[islotReprocMin + islotInReprocBuf];
					}
					else
						//	Read from the reprocess buffer.
						return m_vpslotReproc[islotInReprocBuf];
				}
				else
					// Looking backwards into the reprocess buffer.
					return m_vpslotReproc[
						m_vpslotReproc.size() - cslotOffsetBack + cslotPostReproc - 1];
			}
		}
	}

	//	There is no reprocess buffer, or it is not a factor.

	if (m_islotReadPos + dislotOffset - 1 < m_islotRuleStartRead)
	{
		//	Read from the output stream.
		return psstrmOutput->PeekBack(m_islotReadPos + dislotOffset - 1 - m_islotRuleStartRead,
			fNullOkay);
	}
	else
	{
		//	Read normally from the input stream.
		return m_vpslot[m_islotReadPos - cslotOffsetBack - 1];
	}
}

/*----------------------------------------------------------------------------------------------
	Return the "current" output item from the rule's perspective, ie, the last slot written.
	So dislotOffset = 0 means not the slot at the write position but one slot earlier.
----------------------------------------------------------------------------------------------*/

GrSlotState * GrSlotStream::RuleOutputSlot(int dislotOffset)
{
	return m_vpslot[m_islotWritePos - 1 + dislotOffset];
}

/*----------------------------------------------------------------------------------------------
	For any slot in the final output that has no associations, set the "before" pointer to
	the following slot and the "after" pointer to the preceding slot. The idea of this is
	that it will make it impossible to select a the glyph separately.
	
	Note that if the slot is the first on the line, it will not be associated with 
	a slot in the previous segment.

	@param chwLB		- the glyph IB for the line-break glyphs
----------------------------------------------------------------------------------------------*/
void GrSlotStream::SetNeutralAssociations(gid16 chwLB)
{
	for (int islot = 0; islot < m_islotWritePos; ++islot)
	{
		GrSlotState * pslot = SlotAt(islot);
		if (pslot->PassModified() > 0 &&
			(pslot->BeforeAssoc() == kPosInfinity || pslot->AfterAssoc() == kNegInfinity))
		{
			pslot->CleanUpAssocs();
			if (pslot->BeforeAssoc() != kPosInfinity && pslot->AfterAssoc() != kNegInfinity)
				continue;

			Assert(pslot->BeforeAssoc() == kPosInfinity && pslot->AfterAssoc() == kNegInfinity);

			GrSlotState * pslotBefore = FindAssociatedSlot(islot, 1, chwLB);
			GrSlotState * pslotAfter  = FindAssociatedSlot(islot, -1, chwLB);

			if (pslotBefore && pslotAfter)
				pslot->Associate(pslotBefore, pslotAfter);
			else if (pslotBefore)
				pslot->Associate(pslotBefore);
			else if (pslotAfter)
				pslot->Associate(pslotAfter);
			else
			{
				// Weird, but can happen with an empty segment.
				Warn("No assocations");
			}

//			Assert(pslot->m_vpslotAssoc.Size() > 0);
			pslot->m_fNeutralAssocs = true;
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Find the next or previous slot in the final output that has explicit associations, or
	return NULL if none.

	@param islot		- starting slot
	@param nInc			- +1 if we want to find the next slot, -1 if we want the previous slot
	@param chwLB		- the glyph IB for the line-break glyphs
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotStream::FindAssociatedSlot(int islot, int nInc, gid16 chwLB)
{
	int islotNext = islot + nInc;

	while (islotNext >= 0 && islotNext < m_islotWritePos)
	{
		GrSlotState * pslotRet = SlotAt(islotNext);
		if (pslotRet->IsLineBreak(chwLB))
		{ }
		else if (pslotRet->PassModified() == 0 ||
			(pslotRet->m_vpslotAssoc.size() > 0 && pslotRet->m_fNeutralAssocs == false))
		{
			return pslotRet;
		}
		islotNext += nInc;
	}
	return NULL;
}

} // namespace gr
