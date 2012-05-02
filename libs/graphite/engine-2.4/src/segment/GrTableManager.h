/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrTableManager.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    The GrTableManager class.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_TABLEMAN_INCLUDED
#define GR_TABLEMAN_INCLUDED

#include <cassert>
//:End Ignore

namespace gr
{

class Font;

/*----------------------------------------------------------------------------------------------
	There is a single instance of Engine State that keeps track of the state of the processing
	for a single segment generation.

	Hungarian: engst
----------------------------------------------------------------------------------------------*/
class EngineState
{
	friend class GrTableManager;
	friend class FontMemoryUsage;

public:
	EngineState();
	void Initialize(GrEngine *, GrTableManager *);

	~EngineState();
	void DestroySlotBlocks();

	void InitForNewSegment(GrTableManager * ptman);

	void CreateEmpty();

	GrEngine * Engine();
	GrTableManager * TableManager();

	void CreateSlotStreams();

	GrSlotStream * InputStream(int ipass)
	{
		Assert(ipass > 0); // the char stream serves as input to the zeroth pass
		return m_prgpsstrm[ipass - 1];
	}

	//	Return the stream that serves as output to the given pass.
	GrSlotStream * OutputStream(int ipass)
	{
		Assert(ipass < m_cpass);
		return m_prgpsstrm[ipass];
	}

	GrSlotState * AnAdjacentSlot(int ipassArg, int islotArg);

	GrResult GetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel, float * pxysValueRet);
	GrResult GetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel, int * pxysValueRet);
	GrResult SetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel, float xysValue);
	GrResult SetGlyphAttrForJustification(int iGlyph, int jgat, int nLevel, int xysValue);

	enum { kSlotBlockSize = 50 };	// number of slots to allocate in a block

	void InitPosCache()
	{
		m_islotPosNext = -1;
		m_xsPosXNext = 0;
		m_ysPosYNext = 0;
		m_dxsTotWidthSoFar = 0;
		m_dxsVisWidthSoFar = 0;
	}

	void SetFont(Font * pfont)
	{
		m_pfont = pfont;
	}
	Font * GetFont()
	{
		return m_pfont;
	}

	bool StartLineContext()
	{
		return m_fStartLineContext;
	}
	bool EndLineContext()
	{
		return m_fEndLineContext;
	}
	void SetStartLineContext(bool f)
	{
		m_fStartLineContext = f;
	}
	void SetEndLineContext(bool f)
	{
		m_fEndLineContext = f;
	}

	void SetInitialLB(bool f = true)
	{
		m_fInitialLB = f;
	}

	void SetFinalLB(bool f = true)
	{
		m_fFinalLB = f;
	}

	void SetInsertedLB(bool f = true)
	{
		m_fInsertedLB = f;
	}

	bool HasInitialLB()
	{
		 return m_fInitialLB;
	}
	bool HasInsertedLB()
	{
		return m_fInsertedLB;
	}
	bool HasFinalLB()
	{
		 return m_fFinalLB;
	}

	void SetExceededSpace(bool f = true)
	{
		m_fExceededSpace = f;
	}

	bool ExceededSpace()
	{
		return m_fExceededSpace;
	}

	void SetHitHardBreak(bool f = true)
	{
		 m_fHitHardBreak = f;
	}

	bool HitHardBreak()
	{
		return m_fHitHardBreak;
	}

	void SetRemovedTrWhiteSpace(bool f = true)
	{
		m_fRemovedWhtsp = f;
	}

	bool RemovedTrWhiteSpace()
	{
		return m_fRemovedWhtsp;
	}

	bool WhiteSpaceOnly()
	{
		return (m_twsh == ktwshOnlyWs);
	}

	bool ParaRightToLeft()
	{
		return m_fParaRtl;
	}

	//int PrevPassMaxBackup(int ipass)
	//{
	//	if (ipass == 0)
	//		return 0;
	//	return Pass(ipass - 1)->MaxBackup();
	//}

	DirCode InitialStrongDir()
	{
		return m_dircInitialStrongDir;
	}
	DirCode InitialTermDir()
	{
		return m_dircInitialTermDir;
	}

	void InitializeStreams(GrTableManager * ptman, GrCharStream * pchstrm,
		int cbPrev, byte * pbPrevSegDat, bool fNeedFinalBreak, int * pislotFinalBreak);
	int LbSlotToSegLim(int islot, GrCharStream * pchstrm, int cpassLB);
	int TraceStreamZeroPos(int islotFinal, int nTopDir);

	float EmToLogUnits(int m);
	int LogToEmUnits(float xys);
	bool GPointToXY(gid16 chwGlyphID, int nGPoint, float * xs, float * ys);

	void AddJWidthToAdvance(GrSlotStream * psstrm, GrSlotState ** ppslot, int islot,
		GrSlotState ** ppslotLast, GrSlotState ** ppslotLastBase);

	//	Memory management for slots:
	void NewSlot(gid16 gID, GrFeatureValues fval, int ipass, int ichwSegOffset, int nUnicode,
		GrSlotState **);
	void NewSlot(gid16 gID, GrSlotState * pslotFeat, int ipass, int ichwSegOffset,
		GrSlotState **);
	void NewSlot(gid16 gID, GrSlotState * pslotFeat, int ipass, GrSlotState **);
	void NewSlotCopy(GrSlotState * pslotCopyFrom, int ipass,
		GrSlotState ** pslotRet);
protected:
	void NextSlot(GrSlotState ** ppslot);

protected:
	GrTableManager * m_ptman;

	int m_cFeat;
	int m_cCompPerLig;
	int m_cUserDefn;

	int m_jmodi;			// justification mode
	int m_ipassJustCalled;	// pass after which justification routine was called;
							// -1 if we are not in the midst of interacting with the
							// GrJustifier

	//	Pointer to font
	Font * m_pfont;

	bool m_fStartLineContext;	// true if there was a rule that ran over the initial LB
	bool m_fEndLineContext;		// true if there was a rule that ran over the final LB

	//	the number of slots inserted in the first stream before the official beginning
	//	of the segment, including any initial line break
	int m_cslotPreSeg;

	//	List of allocated GrSlotStates; hungarian slotblk = prgslot
	std::vector<GrSlotState *> m_vslotblk;
	std::vector<u_intslot *> m_vprgnSlotVarLenBufs; // corresponding var-length blocks
	int m_islotblkCurr;
	int m_islotNext;

	//	Directionality codes from previous segment; these are used by the bidi algorithm
	//	at the initial line-break character.
	DirCode m_dircInitialStrongDir;
	DirCode m_dircInitialTermDir;

	LineBrk m_lbPrevEnd;

	//	All of the following 3 are true only when there is an actual LB glyph in the stream
	bool m_fInitialLB;		// is there is an initial LB (an actual LB glyph--in all streams)
	bool m_fFinalLB;		// is there is a final LB (in all streams--true when we've hit
							// the end of the input and end-of-line is true)
	bool m_fInsertedLB;		// is there is a final LB inserted in the output of LB pass
							// (true when we've backtracked)

	bool m_fExceededSpace;	// true if we backtracked due to exceeding space; false if we
							// haven't backtracked, or we did so for some other reason
							// (eg, omitting trailing whitespace)

	bool m_fHitHardBreak;	// true if we encountered a hard-break in the input stream
							// (before the natural end of the segment)

	bool m_fRemovedWhtsp;	// true if we've hit trailing whitespace characters that we've
							// removed

	TrWsHandling m_twsh;	// white-space handling
	bool m_fParaRtl;		// paragraph direction

	bool m_fFeatureVariations;	// can features vary over the course of the segment?

	float m_dxsShrinkPossible;

	//	The final positioning pass is where we are going to do most of the positioning.
	//	Cache the intermediate results so we don't have to calculate from the beginning of
	//	the segment every time.
	int m_islotPosNext;	// where to continue calculating positions in final output stream
						// (one past the last base that was calculated)
	float m_xsPosXNext;
	float m_ysPosYNext;
	float m_dxsTotWidthSoFar;
	float m_dxsVisWidthSoFar;

	int m_cpass;	// number of passes

	//	array of pass-state objects
	PassState * m_prgzpst;

	//	array of slot streams which are output of passes (input of zeroth
	//	pass is the character stream, not a slot stream):
	GrSlotStream ** m_prgpsstrm;
			//	For instance, a simple set of passes might look like this:
			//		CharStream
			//			GlyphGenPass		pass 0
			//		SlotStream				stream 0
			//			SubPass				pass 1
			//		SlotStream				stream 1
			//			PosPass				pass 2
			//		SlotStream				stream 2
			//
			//		m_cpass = 3

}; // end of class EngineState

/*----------------------------------------------------------------------------------------------
	There is a single instance of GrTableManager that handle the interactions between
	passes, including the demand-pull mechanism.

	Hungarian: tman
----------------------------------------------------------------------------------------------*/

class GrTableManager {
	friend class FontMemoryUsage;

public:
	//	Constructor & destructor:
	GrTableManager(GrEngine * pgreng)
		:	m_cpass(0),
			m_fBidi(false),
			m_prgppass(NULL),
			m_pgreng(pgreng)
	{
		Assert(pgreng);
	}

	~GrTableManager();

	bool CreateAndReadPasses(GrIStream & grstrm, int fxdSilfVersion, int fxdRuleVersion,
		int cpassFont, long lSubTableStart, int * rgnPassOffsets, int * rgcbPassLengths,
		int ipassSub1Font, int ipassPos1Font, int ipassJust1Font, byte ipassPostBidiFont);

	void CreateEmpty();

	GrEngine * Engine();
	EngineState * State();

	void Run(Segment * psegNew, Font * pfont, GrCharStream * pchstrm, 
		IGrJustifier * pgjus, int jmodi,
		LayoutEnvironment & layout,
		int ichStop, float dxWidth, float dxUnjustified,
		bool fNeedFinalBreak, bool fMoreText, bool fFeatureVariations, int ichFontLim,
		bool fInfiniteWidth, bool fWidthIsCharCount,
		int ichwCallerBtLim,
		int nDirDepth, SegEnd estJ);

	GrPass* Pass(int ipass)
	{
		Assert(ipass >= 0);
		return m_prgppass[ipass];
	}

	int NumberOfPasses()
	{
		return m_cpass;
	}

	int NumberOfLbPasses()
	{
		return m_cpassLB;
	}

	bool HasBidiPass()
	{
		return m_fBidi;
	}

	int FirstPosPass()
	{
		return m_ipassPos1;
	}

	void StorePassStates(PassState * rgzpst);

	//	Return the stream that serves as input to the given pass.
	GrSlotStream * InputStream(int ipass)
	{
		return m_engst.InputStream(ipass);
	}
	GrSlotStream * OutputStream(int ipass)
	{
		return m_engst.OutputStream(ipass);
	}

	void UnwindAndReinit(int islotNewBreak);

public:
	int InternalJustificationMode()
	{
		return m_engst.m_jmodi;
	}

	bool ShouldLogJustification()
	{
		return (m_engst.m_jmodi != kjmodiNormal);
	}

	bool FeatureVariations()
	{
		return (m_engst.m_fFeatureVariations);
	}
	void SetFeatureVariations(bool f)
	{
		m_engst.m_fFeatureVariations = f;
	}

	int PrevPassMaxBackup(int ipass)
	{
		if (ipass == 0)
			return 0;
		return Pass(ipass - 1)->MaxBackup();
	}

	//	Forward to the engine itself
	gid16 GetGlyphIDFromUnicode(int nUnicode);
	gid16 ActualGlyphForOutput(utf16 chwGlyphID);
	GrGlyphTable * GlyphTable();

	gid16 LBGlyphID();

	gid16 GetClassGlyphIDAt(int nClass, int nIndex);
	int GetIndexInGlyphClass(int nClass, gid16 chwGlyphID);
	size_t NumberOfGlyphsInClass(int nClass);

	void SetSlotAttrsFromGlyphAttrs(GrSlotState * pslot);

	int NumFeat();
	int DefaultForFeatureAt(int ifeat);
	GrFeature * FeatureWithID(featid nID, int * pifeat);
	GrFeature * Feature(int ifeat);
	void DefaultsForLanguage(isocode lgcode,
		std::vector<featid> & vnFeats, std::vector<int> & vnValues);

	bool RightToLeft();
	int TopDirectionLevel();

	float VerticalOffset();

	int NumUserDefn();
	int NumCompPerLig();

	int ComponentIndexForGlyph(gid16 chwGlyphID, int nCompID);
	int GlyphAttrValue(gid16 chwGlyphID, int nAttrID);

	//	---

	bool LoggingTransduction();

	float EmToLogUnits(int m);
	int LogToEmUnits(float xys);
	bool GPointToXY(gid16 chwGlyphID, int nGPoint, float * xs, float * ys);

	void CalcPositionsUpTo(int ipass, GrSlotState * pslotLast, bool fMidPass,
		float * pxsWidth, float * pxsVisibleWidth);

	void InitPosCache()
	{
		m_engst.InitPosCache();
	}

	bool IsWhiteSpace(GrSlotState * pslot);

	//	For transduction logging:
	//bool WriteTransductionLog(GrCharStream * pchstrm, Segment * psegRet,
	//	int cbPrev, byte * pbPrevSegDat, byte * pbNextSegDat, int * pcbNextSegDat);
	//bool WriteAssociationLog(GrCharStream * pchstrm, Segment * psegRet);
	bool WriteTransductionLog(std::ostream * pstrmLog,
		GrCharStream * pchstrm, Segment * psegRet, int cbPrevSetDat, byte * pbPrevSegDat);
	bool WriteAssociationLog(std::ostream * pstrmLog,
		GrCharStream * pchstrm, Segment * psegRet);
#ifdef TRACING
	void WriteXductnLog(std::ostream & strmOut, GrCharStream * pchstrm, Segment * psegRet,
		int cbPrevSegDat, byte * pbPrevSegDat);
	//bool LogFileName(std::string & staFile);
	void LogUnderlying(std::ostream & strmOut, GrCharStream * pchstrm, int cchwBackup);
	void LogPass1Input(std::ostream & strmOut);
	void LogPassOutput(std::ostream & strmOut, int ipass, int cslotSkipped);
	void LogAttributes(std::ostream & strmOut, int ipass, bool fJustWidths = false);
	void LogFinalPositions(std::ostream & strmOut);
	void LogUnderlyingHeader(std::ostream & strmOut, int ichwMin,
		int ichwLim, int cchwBackup, int * prgichw16bit);
	void LogSlotHeader(std::ostream & strmOut, int cslot,
		int cspPerSlot, int cspLeading, int islotMin = 0);
	void LogSlotGlyphs(std::ostream & strmOut, GrSlotStream * psstrm);
	void SlotAttrsModified(int ipass, bool * rgfMods, bool fPreJust, int * pccomp, int *pcassoc);
	void LogInTable(std::ostream & strmOut, int n);
	void LogInTable(std::ostream & strmOut, float n);
	void LogHexInTable(std::ostream & strmOut, gid16 chw, bool fPlus = false);
	void LogDecimalInTable(std::ostream & strmOut, utf16 chw);
	void LogDirCodeInTable(std::ostream & strmOut, int dirc);
	void LogBreakWeightInTable(std::ostream & strmOut, int lb);
#endif // TRACING

protected:
	void InitNewSegment(Segment * psegNew, Font * pfont, GrCharStream * pchstrm, IGrJustifier * pgjus,
		int islotStream0Break, int islotSurfaceBreak, bool fStartLine, bool fEndLine, int ichFontLim,
		LineBrk lbEnd, SegEnd est, int * dichSegLen);

	void InitSegmentAsEmpty(Segment * psegNew, Font * pfont, GrCharStream * pchstrm,
		bool fStartLine, bool fEndLine);

	void InitSegmentToDelete(Segment * psegNew, Font * pfont, GrCharStream * pchstrm);

	int ChunkInPrev(int ipass, int islot, GrCharStream * pchstrm);

	bool Backtrack(int * islotPrevLB,
	   LineBrk * plbPref, LineBrk lbMax, TrWsHandling, bool fMoreText,
	   int ichwCallerBtLim, bool fEndLine, LineBrk * plbFound);

	LineBrk IncLineBreak(LineBrk lb);

	void InitializeForNextSeg(Segment* pseg,
		int islotStream0Break, int islotSurfaceBreak, LineBrk lbEnd,
		bool fNextSegNeedsContext, GrCharStream * pchstrm);

	void RecordAssocsAndOutput(Font * pfont,
		Segment * pseg, bool fWidthIsCharCount,
		TrWsHandling twsh, bool fParaRtl, int nDirDepth);

	void CalculateAssociations(Segment * pseg, int csloutSurface);
	void AdjustAssocsForOverlaps(Segment * pseg);

	void UnstretchTrailingWs(GrSlotStream * psstrm, int iGlyphLim);
	void CallJustifier(IGrJustifier * pgjus, int ipassCurr, float dxUnjustified, float dxJustified,
		bool fEndLine);

	void DetermineShrink(IGrJustifier * pgjus, int ipass);

protected:
	//	Instance variables:

	int m_cpass;	//	number of passes; zero-th pass is Unicode character -> glyph ID mapping;
					//	Note thatwe always have one positioning pass to measure things even
					//	if there are no tables to run.

	int m_cpassLB;		// number of line-break passes
	int m_ipassPos1;	// first positioning pass
	int m_ipassJust1;	// first justification pass (== m_ipassPos1 if none)

	bool m_fBidi;	//	is there a bidi pass?

	//	array of passes:
	GrPass ** m_prgppass;

	//	Pointer back to Graphite engine, to give access to global constants and tables.
	GrEngine * m_pgreng;

	//	State of processing.
	EngineState m_engst;

	bool m_fLogging;

public:
	//	For test procedures:
	void SetUpTest(std::wstring);

protected:
	void TempDebug();
	int SurfaceLineBreakSlot(int ichw, GrCharStream * pchstrm, bool fInitial);
	std::wstring ChunkDebugString(int ipass);
};	// end of class GrTableManager

} // namespace gr


#endif // !GR_TABLEMAN_INCLUDED
