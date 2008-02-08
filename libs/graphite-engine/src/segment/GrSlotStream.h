/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrSlotStream.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Classes GrSlotStream and GrSlotState.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef SLOTSTREAM_INCLUDED
#define SLOTSTREAM_INCLUDED

//:End Ignore

namespace gr
{

class EngineState;
class Font;

/*----------------------------------------------------------------------------------------------
	Each GrSlotStream represents the output of one pass, which also serves as the input
	of the following pass. Hence it has two positions, a write position for when it is
	serving as an output stream, and a read position for when it is serving as an input
	stream. The TableManager holds a list of the streams.

	The reading of slot streams is complicated by two factors. First, it may be necessary
	to access the "pre-context" items of a rule, which are the slots before the current
	stream position at the point when the rule is matched. These items will not be modified 
	by the rule, but may have been modified by a previous rule in the same pass, Therefore
	we need to read these slots from the output stream, rather than the input stream, 
	because the output stream has the current slot attribute values that we want to work with.
	In order make this happen, we keep track of the read and write positions of 
	the streams at the time the current rule was matched (m_islotRuleStartRead and 
	m_islotRuleStartWrite) and whenever we try to read a slot before m_islotRuleStartRead, 
	we read the corresponding slot from the output stream instead.

	Second, a rule may explicitly set the advance position so that some items are 
	actually reprocessed (and potentially modified) by the same pass. When this happens, 
	we actually copy the slots to reprocess from the output stream back 
	into a temporary buffer (m_vpslotReproc) in the input stream, and set up the stream so 
	that the next read operations will read from the buffer.

	It gets tricky because these two mechanisms can overlap with each other, so care 
	must be taken to read from the right place: the input stream in the normal way,
	the reprocess buffer, or the output stream. See the RuleInputSlot method.

	Another messy bit of code has to do with maintaining chunks. These are used for 
	backtracking and unwinding after inserting a line-break. Adjacent streams have maps 
	showing how many slots were processed by a single rule; the maps must be consisent
	between adjacent rules. The following is an example of a pair of valid chunk maps:

				Input stream		Output stream
		Pos		  next-map			  prev-map
		0			0					0
		1			-1					-1
		2			2					2
		3			4					-1
		4			-1					3
		5			-1					-1
		6			7					-1
		7			-1					6
		8			-1					-1

	A -1 means that slot is not at the beginning of a chunk. Otherwise, there must be a
	corresponding value in the parallel map of the adjacent stream--eg, the input stream's 
	next-map(3) == 4 and the output stream's prev-map(4) == 3. Assertions are run 
	regularly to ensure that the chunk maps stay valid.
	
	So above, the first chunk corresponds to slots 0-1 in the input and 0-1 in the output;
	two slots were processed by the rule. The second chunk corresponds to slot 2 
	in the input and slots 2-3 in the output (a slot was inserted). 
	The third corresponds to slots 3-5 in the input and 4-6 in the output (three slots
	processed). The fourth chunk covers slots 6-8 in the input and 7-8 in the output
	(one slot was deleted).

	ENHANCE SharonC: explain the effect of skipped slots on the chunk maps.

	Hungarian: sstrm
----------------------------------------------------------------------------------------------*/
class GrSlotStream {

	friend class GrSlotState;
	friend class FontMemoryUsage;

public:
	//	Constructor:
	GrSlotStream(int ipass)
	{
		m_ipass = ipass;
		m_vpslot.clear();
		m_vislotPrevChunkMap.clear();
		m_vislotNextChunkMap.clear();
		m_vpslotReproc.clear();
	}

	//	Destructor:
	~GrSlotStream()
	{
		ReleaseSlots(0, m_vpslot.size());
	}

	void ReleaseSlots(int islotMin, int islotLim)
	{
		//	A slot stream is responsible for deleting the slot states that it created,
		//	that is, the ones whose modified tag equals this stream's pass index.
		//	NO LONGER TRUE - there is a master list of them managed by the GrTableManager.
//		for (int islot = islotMin; islot < islotLim; ++islot)
//		{
//			if (m_vpslot[islot]->m_ipassModified == m_ipass)
//				delete m_vpslot[islot];
//		}
	}

	//	Getters:
	int WritePos()		{ return m_islotWritePos; }
	int ReadPos()		{ return m_islotReadPos; }
	bool FullyWritten()	{ return m_fFullyWritten; }

	int SlotsPresent()	{ return m_vpslot.size(); }

	//	Setters:
	void SetWritePos(int islot)
	{
		m_islotWritePos = islot;
	}
	void SetReadPos(int islot)
	{
		m_islotReadPos = islot;
	}

	void Initialize(int ipassPos1, bool fAnythingPrevious)
	{
		m_islotWritePos = 0;
		m_islotReadPos = 0;
		m_islotReprocPos = -1;
		m_islotReprocLim = -1;
		m_islotRuleStartRead = 0;
		m_islotRuleStartWrite = 0;
		m_islotReadPosMax = 0;
		m_cslotSkippedForResync = 0;
		m_cslotPreSeg = (fAnythingPrevious) ? -1 : 0;
		m_fFullyWritten = false;
		m_islotSegMin = -1;
		m_islotSegLim = -1;
		m_fUsedByPosPass = (m_ipass + 1 >= ipassPos1);
		m_fInputToPosPass1 = (m_ipass + 1 == ipassPos1);
		m_vpslot.clear();	// not responsible for destroying the slots
		m_vislotPrevChunkMap.clear();
		m_vislotNextChunkMap.clear();
	}

public:
	bool AtEnd();
	bool AtEndOfContext();
	void NextPut(GrSlotState* pslot);
	GrSlotState * NextGet();
	GrSlotState * Peek(int dislot = 0);
	GrSlotState * PeekBack(int dislot, bool fNullOkay = false);
	void Skip(int = 1);
	GrSlotState * SlotAt(int islot)
	{
		return m_vpslot[islot];
	}

	int OutputOfPass()
	{
		return m_ipass;
	}

	//	This function is intended to be used within positioning passes where there is a
	//	one-to-one correspondence beween the slots in consecutive streams.
	GrSlotState * OutputSlotAt(int islot)
	{
		return Peek(islot - ReadPosForNextGet());
	}

	int ReadPosForNextGet()
	{
		return ReadPos() - SlotsToReprocess();
	}

	//	Only valid for streams that are the input or output to a positioning pass.
	GrSlotState * SlotAtPosPassIndex(int islot)
	{
		Assert(GotIndexOffset());
		Assert(m_fUsedByPosPass);
		return m_vpslot[islot + m_cslotPreSeg];
	}

	//	Only valid for streams that are the input or output to a positioning pass.
	bool HasSlotAtPosPassIndex(int islot)
	{
		Assert(GotIndexOffset());
		Assert(m_fUsedByPosPass);
		return (signed(m_vpslot.size()) > (islot + m_cslotPreSeg));
	}

	bool AssertValid();
	void AssertChunkMapsValid(GrSlotStream * psstrmOut);
	void AssertStreamIndicesValid(GrSlotStream * psstrmIn);
	void AssertAttachmentsInOutput(int islotMin, int islotLim);
	bool NoReproc()
	{
		return (m_islotReprocPos == -1 || (m_islotReprocPos >= signed(m_vpslotReproc.size())));
	}

	int SlotsPending();
	int SlotsPendingInContext();
	int TotalSlotsPending();
	bool PastEndOfPositioning(bool fOutput);

	void ClearReprocBuffer()
	{
		m_islotReprocPos = -1;
		m_islotReprocLim = -1;
		m_vpslotReproc.clear();
	}

	int ReprocLim()
	{
		return m_islotReprocLim;
	}

	int ReprocMin()
	{
		Assert(m_islotReprocPos > -1);
		return m_islotReprocLim - m_vpslot.size();
	}

	int RuleStartReadPos()
	{
		return m_islotRuleStartRead;
	}

	void SetRuleStartReadPos()
	{
		if (m_islotReprocPos > -1)
			m_islotRuleStartRead = m_islotReprocLim - SlotsToReprocess();
		else
			m_islotRuleStartRead = m_islotReadPos;
	}

	int RuleStartWritePos()
	{
		return m_islotRuleStartWrite;
	}

	void SetRuleStartWritePos()
	{
		m_islotRuleStartWrite = m_islotWritePos;
	}

	void MarkFullyWritten();

	void SetSegMin(int islot, bool fAdjusting = false)
	{
		Assert(fAdjusting || m_islotSegMin == -1 || m_islotSegMin == islot);
		m_islotSegMin = islot;
	}
	void SetSegMinToWritePos(bool fMod = true)
	{
		if (m_islotSegMin == -1)
			m_islotSegMin = m_islotWritePos;
		else
			Assert(m_islotSegMin <= m_islotWritePos); // eg, already set to before the initial LB
	}
	int SegMin()
	{
		return m_islotSegMin;
	}
	void SetSegLim(int islot)
	{
		m_islotSegLim = islot;
	}
	void SetSegLimToWritePos(bool fMod = true)
	{
		if (m_islotSegLim > -1 && !fMod)
		{
			Assert(m_islotSegLim <= m_islotWritePos);
			return;
		}
		m_islotSegLim = m_islotWritePos;
	}
	int SegLimIfKnown()
	{
		return m_islotSegLim;
	}
	int FinalSegLim()
	{
		if (m_islotSegLim > -1)
			return m_islotSegLim;
		else
			return m_islotWritePos;
	}

	// Return true if we are exactly at the segment min.
	bool AtSegMin()
	{
		if (m_islotSegMin == -1)
			return false;
		return (m_islotSegMin == ReadPosForNextGet());
	}

	// Return true if we are exactly at the segment lim.
	bool AtSegLim()
	{
		if (m_islotSegLim == -1)
			return false;
		return (m_islotSegLim == ReadPosForNextGet());
	}

	int ReadPosMax()
	{
		return m_islotReadPosMax;
	}
	void SetReadPosMax(int islot)
	{
		m_islotReadPosMax = std::max(m_islotReadPosMax, islot);
	}

	int OldDirLevelRange(EngineState * pengst, int islotStart, int nTopDirection);
	int DirLevelRange(EngineState * pengst, int islotStart, int nTopDirection,
		std::vector<int> & vislotStarts, std::vector<int> & vislotStops);
	int GetSlotDirLevel(EngineState * pengst, int islot,
		int nOuterLevel, int nCurrLevel, DirCode dircMarker, DirCode dircOverride);
	DirCode AdjacentStrongCode(EngineState * pengst, int islot, int nInc,
		DirCode dircPDF, bool fNumbersAreStrong = false);
	DirCode TerminatorSequence(EngineState * pengst, int islot, int nInc, DirCode dircPDF);
	DirCode AdjacentNonBndNeutralCode(EngineState * pengst, int islot, int nInc,
		DirCode dircPDF);

	void CopyOneSlotFrom(GrSlotStream * psstrmPrev);
	void SimpleCopyFrom(GrSlotStream * psstrmI, int islotInput, int islotOutput);

	bool MoreSpace(GrTableManager * ptman,
		float xsSpaceAllotted, bool fWidthIsCharCount,
		bool fIgnoreTrailingWS, TrWsHandling twsh,
		float * pxsWidth);

	int InsertLineBreak(GrTableManager * ptman,
		int islotPrevBreak, bool fInsertedLB, int islotStartTry,
		LineBrk lb, TrWsHandling twsh, int islotMin, LineBrk * plbNextToTry);
	int MakeSegmentBreak(GrTableManager * ptman,
		int islotPrevBreak, bool fInsertedLB, int islotStartTry,
		LineBrk lb, TrWsHandling twsh, int islotMin, LineBrk * plbNextToTry);
	bool FindSegmentEnd(GrTableManager * ptman,
		int islotStartTry, LineBrk lb, TrWsHandling twsh, int islotMin,
		int * pislot, int * pichwSegOffset,
		LineBrk * plbFound, LineBrk * plbNextToTry);
	bool HasEarlierBetterBreak(int islotBreak, LineBrk lbFound, gid16 chwLB);
	LineBrk BreakWeightAt(gid16 chwLB, int islot);
	void AppendLineBreak(GrTableManager *, GrCharStream * pchstrm,
		LineBrk, DirCode dirc, int islot, bool fInitial, int ichwSegOffset);
	int FindFinalLineBreak(gid16 chwLB, int islotMin, int islotLim);
	void AdjustPrevStreamNextChunkMap(GrTableManager * ptman, int islotMod, int nInc);
	void AdjustNextChunkMap(int islotMin, int islotMod, int nInc);

	void EnsureLocalCopy(GrTableManager * ptman, GrSlotState * pslot, GrSlotStream * psstrmIn);
	void ReplaceSlotInReprocessBuffer(GrSlotState * pslotOld, GrSlotState * pslotNew);

	void ZapCalculatedDirLevels(int islotLB);

	int MaxClusterSlot(int islotChunkLim)
	{
		return MaxClusterSlot(islotChunkLim - 1, islotChunkLim);
	}
	int MaxClusterSlot(int islotChunkMin, int islotChunkLim);

	void UnwindInput( int islot, bool fPreBidiPass);
	void UnwindOutput(int islot, bool fOutputOfPosPass);

	void MapInputChunk( int islotIn, int islotOut, int islotInLim,
		bool fReprocessing, bool fBackingUp);
	void MapOutputChunk(int islotOut, int islotIn, int islotOutLim,
		bool fReprocessing, int cslotReprocess, bool fBackingUp);

	int LastNextChunkLength();

	int ChunkInPrev(int i)
	{
		Assert(i >= 0);
		Assert(i < signed(m_vislotPrevChunkMap.size()));
		Assert(i < m_islotWritePos);
		return m_vislotPrevChunkMap[i];
	}
	int ChunkInNext(int i)
	{
		Assert(i >= 0);
		Assert(i < signed(m_vislotNextChunkMap.size()));
		Assert(i < m_islotReadPos);
		return m_vislotNextChunkMap[i];
	}

	int ChunkInNextMin(int islot);
	int ChunkInNextLim(int islot);

	void ResyncSkip(int nslot);
	int SlotsSkippedToResync()
	{
		return m_cslotSkippedForResync;
	}
	void ClearSlotsSkippedToResync()
	{
		m_cslotSkippedForResync = 0;
	}

	int IndexOffset()
	{
		Assert(GotIndexOffset());	// should have been calculated
		return m_cslotPreSeg;
	}
	void SetIndexOffset(int c)
	{
		m_cslotPreSeg = c;
	}
	bool GotIndexOffset()
	{
		return (m_cslotPreSeg >= 0);
	}
	void CalcIndexOffset(GrTableManager * ptman);

	void SetPosForNextRule(int nslot, GrSlotStream * psstrmInput, bool fOutputOfPosPass);
	void SetLBContextFlag(GrTableManager * ptman, int islotStart);
	int SlotsToReprocess();

	void SetNeutralAssociations(gid16 chwLB);
protected:
	GrSlotState * FindAssociatedSlot(int islot, int nInc, gid16 chwLB);

	GrSlotState * GetSlotAt(int islot);

public:
	//	Used by the action code:
	void BackupReadPos(int cslot = 1)
	{
		for (int islot = 0; islot < cslot; islot++)
		{
			if (m_islotReprocLim > -1 && m_islotReprocLim == m_islotReadPos)
			{
				if (m_islotReprocPos == -1)
					m_islotReprocPos = m_vpslotReproc.size() - 1;
				else
					--m_islotReprocPos;
				Assert(m_islotReprocPos >= 0);
			}
			else
				--m_islotReadPos;
		}
	}

	GrSlotState * RuleInputSlot(int dislot = 0, GrSlotStream * psstrmOut = NULL,
		bool fNullOkay = false);
	GrSlotState * RuleOutputSlot(int dislot = 0);

	void PutSlotAt(GrSlotState * pslot, int islot)
	{
		//	Currently only used for the final pass, when applying justify.width to
		//	the advance width.
		Assert(signed(m_vpslot.size()) > islot);
		Assert(WritePos() > islot);
		Assert(ReadPos() <= islot);
		Assert(SlotsToReprocess() == 0);
		m_vpslot[islot] = pslot;
	}

protected:
	//	Instance variables:
	int		m_ipass;	// which pass this stream is serving as OUTPUT of

	std::vector<GrSlotState*> m_vpslot;			// the slots being processed

	//	Chunking information:
	std::vector<int> m_vislotPrevChunkMap;	// chunk mapping into previous stream;
											// -1 if not at a chunk boundary
											// (this information is needed for
											// reinitialization after inserting a line break)

	std::vector<int> m_vislotNextChunkMap;	// chunk mapping into next stream, or -1
											// (this information is needed for backtracking)

	//	Since most streams serve as both input and output, there are two positions.
	//	Note that readPos <= writePos always.
	int m_islotWritePos;	// where previous pass should put next output
	int m_islotReadPos;		// where following pass should start processing from

	//	When we are reprocessing some of the output from the next stream:
	std::vector<GrSlotState*> m_vpslotReproc;	// temporary buffer
	int m_islotReprocLim;	// the read position corresponding to the end of the reprocess
							// buffer; -1 if buffer is invalid
	int m_islotReprocPos;	// index into m_vpslotReproc from where we should read next;
							// -1 if we are not reprocessing

	int m_islotRuleStartRead;	// read position when the rule was started; any requests before
								// this point should access the output stream, because it has 
								// the most up-to-date values.
	int m_islotRuleStartWrite;	// write position when the rule started

	bool m_fFullyWritten;	// set to true when we can't get any more from the
							// previous pass; initially false.

	int m_islotSegMin;	// the official min of the segment; -1 if not yet set
	int m_islotSegLim;	// the official lim of the segment; -1 if not yet set
						// (only set for the output of the final line-break pass
						// and those beyond)

	int m_cslotSkippedForResync;	// number of slots skipped to resync;
									// specifically, the number of slots' worth of the
									// next-chunk-map that are invalid, not mapped
									// to any slots in the following stream

	int m_cslotPreSeg;	// number of slots previous to the official beginning of the segment;
						// -1 if we haven't figured it out yet. Used for keeping streams
						// in the positioning pass in sync; not particularly useful for
						// substitution passes, and will possibly be left uncalculated.

	bool m_fUsedByPosPass;		// true if this is input to or output from a positioning pass
	bool m_fInputToPosPass1;	// true if this is the input to the first positioning pass

	//	Max value of m_islotReadPos; used for recognizing infinite loops and forcibly
	//	advancing (see GrPass::CheckInputProgress).
	int m_islotReadPosMax;

public:
	//	For transduction logging:
#ifdef TRACING
//	void LogSlotGlyphs(std::ofstream & strmOut, gid16 chwLB);
#endif // TRACING

	//	For test procedures:
	int NumberOfSlots() { return m_vpslot.size(); }
	GrSlotState * LastSlotWritten()
	{
		if (m_islotWritePos == 0)
			return NULL;
		else
			return m_vpslot[m_islotWritePos-1];
	}
	GrSlotState * LastSlotRead()
	{
		if (m_islotReadPos == 0)
			return NULL;
		else
			return m_vpslot[m_islotReadPos-1];
	}

	gid16 GlyphIDAt(int islot)
	{
		return m_vpslot[islot]->m_chwGlyphID;
	}

	int PreChunk()	// answer the number of prepended items that are not in any chunk;
					// should be obsolete now
	{
		int islot;
		for (islot = 0; islot < m_islotReadPos; ++islot)
		{
			if (m_vislotNextChunkMap[islot] != -1)
				return islot;
		}
		Assert(false);
		return islot;
	}

};	// end of class GrSlotStream

} // namespace gr


#endif // !SLOTSTREAM_INCLUDED
