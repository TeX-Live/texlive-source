/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrPass.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    The GrPass class and subclasses.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef PASS_INCLUDED
#define PASS_INCLUDED

//:End Ignore

namespace gr
{

/*----------------------------------------------------------------------------------------------
	This class stores the state of processing in a single pass.
	
	Hungarian: zpst
----------------------------------------------------------------------------------------------*/

class PassState {
	friend class GrPass;
	friend class GrGlyphGenPass;
	friend class GrLineBreakPass;
	friend class GrSubPass;
	friend class GrBidiPass;
	friend class GrPosPass;
	friend class FontMemoryUsage;

public:
	PassState()
	{
	}

	void InitForNewSegment(int ipass, int nMaxChunk);
	void InitializeLogInfo();

	int MaxChunk()
	{
		return m_nMaxChunk;
	}

	void SetResyncSkip(int c)
	{
		m_cslotSkipToResync = c;
		m_fDidResyncSkip = false;
	}
	int NeededToResync()
	{
		if (m_fDidResyncSkip)
			return 0;
		return m_cslotSkipToResync;
	}
	bool DidResyncSkip()
	{
		return m_fDidResyncSkip;
	}
	bool CanResyncSkip(GrSlotStream * psstrmOutput)
	{
		if (m_fDidResyncSkip)
			return true; // already did it
		// Are we in the position to do the resync-skip?
		return m_cslotSkipToResync <= psstrmOutput->WritePos();
	}
	int DoResyncSkip(GrSlotStream * psstrmOutput);
	void UndoResyncSkip()
	{
		m_fDidResyncSkip = false;
	}

	void UnwindLogInfo(int islotIn, int islotOut);
	void RecordRule(int islot, int irul, bool fFired = false);
	void RecordDeletionBefore(int islot);
	void RecordInsertionAt(int islot);
#ifdef TRACING
	void LogRulesFiredAndFailed(std::ostream & strmOut, GrSlotStream * psstrmIn);
	void LogInsertionsAndDeletions(std::ostream & strmOut, GrSlotStream * psstrmOut);
#endif // TRACING

protected:
	int m_ipass;

	//	Used to avoid infinite loops. If m_nRulesSinceAdvance > m_nMaxRuleLoop,
	//	and we have not passed the stream's ReadPosMax, forcibly advance.
	int m_nRulesSinceAdvance;

	//	Maximum length of chunk, used when unwinding the streams during backtracking. This
	//	must be initialized to m_nMaxRuleContext, but that is not always adequate, because
	//	when there is reprocessing going on the chunks may be longer.
	int m_nMaxChunk;

	//	Indicates how much of the pass's initial output should be automatically
	//	skipped over in order to resync with a chunk boundary. This handles the situation
	//	where we are reprocessing a little of the previous segment in order to handle
	//	cross-line-boundary contextuals. The goal is to skip over the output until we hit
	//	what we know (from processing the previous segment) is a chunk boundary.
	//
	//	In most cases (and always when this is the first segment of the
	//	writing system) this number is 0.
	int m_cslotSkipToResync;
	bool m_fDidResyncSkip;

	enum {
		kHitMaxBackup = -1,
		kHitMaxRuleLoop = -2
	};

	//	For logging transduction process: record of one rule matched or fired
	struct RuleRecord
	{
		int m_irul;		// rule index, or kHitMaxBackup or kHitMaxRuleLoop
		int m_islot;	// slot index of input stream
		bool m_fFired;
	};
	RuleRecord m_rgrulrec[128];
	int m_crulrec;

	int m_rgcslotDeletions[128];	// number of deletions before slot i in output
	bool m_rgfInsertion[128];		// true if slot i in output was inserted
};

/*----------------------------------------------------------------------------------------------
	This class handles finding and applying rules from the pass and storing
	the results in the appropriate stream.
	
	Hungarian: pass
----------------------------------------------------------------------------------------------*/
class GrPass {
	friend class FontMemoryUsage;

protected:
	//	Action command codes; these MUST match the corresponding definitions in the compiler:
	enum ActionCommand {
		kopNop = 0,

		kopPushByte,		kopPushByteU,		kopPushShort,	kopPushShortU,	kopPushLong,

		kopAdd,				kopSub,				kopMul,			kopDiv,
		kopMin,				kopMax,
		kopNeg,
		kopTrunc8,			kopTrunc16,

		kopCond,
		
		kopAnd,				kopOr,				kopNot,
		kopEqual,			kopNotEq,
		kopLess,			kopGtr,				kopLessEq,		kopGtrEq,

		kopNext,			kopNextN,			kopCopyNext,
		kopPutGlyph8bitObs,	kopPutSubs8bitObs,	kopPutCopy,
		kopInsert,			kopDelete,
		kopAssoc,
		kopCntxtItem,

		kopAttrSet,			kopAttrAdd,			kopAttrSub,
		kopAttrSetSlot,
		kopIAttrSetSlot,
		kopPushSlotAttr,	kopPushGlyphAttrObs,kopPushGlyphMetric,		kopPushFeat,
		kopPushAttToGAttrObs,	kopPushAttToGlyphMetric,
		kopPushISlotAttr,

		kopPushIGlyphAttr,	// not implemented

		kopPopRet,			kopRetZero,			kopRetTrue,
		kopIAttrSet,		kopIAttrAdd,		kopIAttrSub,
		kopPushProcState,	kopPushVersion,
		kopPutSubs,			kopPutSubs2,		kopPutSubs3,
		kopPutGlyph,		kopPushGlyphAttr,	kopPushAttToGlyphAttr

	};

	enum StackMachineFlag {
		ksmfDone = 0,
		ksmfContinue,
		ksmfUnderflow,
		ksmfStackNotEmptied
	};

public:
	//	Constructor:
	GrPass(int i);
	//	Destructor:
	virtual ~GrPass();

	int PassNumber() { return m_ipass; }

	bool ReadFromFont(GrIStream & grstrm, int fxdSilfVersion, int fxdRuleVersion, int nOffset);
	void InitializeWithNoRules();

	virtual void SetTopDirLevel(int n)
	{	// only GrBidiPass does anything interesting
	}

	void SetPassState(PassState * pzpst)
	{
		m_pzpst = pzpst;
	}

	virtual void ExtendOutput(GrTableManager *,
		GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput, int nslotToGet,
		TrWsHandling twsh,
		int * pnRet, int * pcslotGot, int * pislotFinalBreak);

	int ExtendGlyphIDOutput(GrTableManager *, GrCharStream *,
		GrSlotStream *, int ichSegLim, int cchwPostXlbContext,
		LineBrk lb, int cslotToGet, bool fNeedFinalBreak,
		TrWsHandling twsh, int * pislotFinalBreak);

	int ExtendFinalOutput(GrTableManager *, GrSlotStream * psstrmInput,
		GrSlotStream * psstrmOutput,
		float xsSpaceAllotted, bool fWidthIsCharCount, bool fInfiniteWidth,
		bool fHaveLineBreak, bool fMustBacktrack,
		LineBrk lbMax, TrWsHandling twsh,
		int * pislotLB, float * pxsWidth);

	int RemoveTrailingWhiteSpace(GrTableManager * ptman, GrSlotStream * psstrmOut,
		TrWsHandling twsh, int * pislotFinalBreak);

	virtual int Unwind(GrTableManager * ptman,
		int islotChanged, GrSlotStream *psstrmInput, GrSlotStream * psstrmOutput,
		bool fFirst);

	int MaxBackup()
	{
		return m_nMaxBackup;
	}

	bool DidResyncSkip()
	{
		return m_pzpst->DidResyncSkip();
	}
	int DoResyncSkip(GrSlotStream * psstrmOutput)
	{
		return m_pzpst->DoResyncSkip(psstrmOutput);
	}

	void UndoResyncSkip()
	{
		m_pzpst->UndoResyncSkip();
	}
	void SetResyncSkip(int n)
	{
		m_pzpst->SetResyncSkip(n);
	}

	virtual void DoCleanUpSegMin(GrTableManager * ptman,
		GrSlotStream * psstrmIn, int islotInitReadPos, GrSlotStream * psstrmOut)
	{
	}

	virtual void DoCleanUpSegLim(GrTableManager * ptman, GrSlotStream * psstrmOut,
		TrWsHandling twsh)
	{
	}

	virtual bool IsPosPass()
	{
		return false;
	}

	virtual bool PreBidiPass() = 0;

	bool RunConstraint(GrTableManager *, int ruln,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
		int cslotPreModContext, int cslotMatched);

	int RunCommandCode(GrTableManager * ptman,
		byte * pbStart, bool fConstraints,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islot);

protected:
	int RunOneCommand(GrTableManager * ptman, bool fConstraints,
		ActionCommand op, byte ** ppbArg, bool * pfMustGet, bool * pfInserting,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islot,
		std::vector<int> & vnStack, StackMachineFlag * psmf);
	void DoStackArithmetic2Args(ActionCommand op, std::vector<int> & vnStack,
		StackMachineFlag * psmf);
	void DoStackArithmetic1Arg(ActionCommand op, std::vector<int> & vnStack,
		StackMachineFlag * psmf);
	void DoConditional(std::vector<int> & vnStack, StackMachineFlag * psmf);
	StackMachineFlag CheckStack(std::vector<int> & vnStack, int cn);
	void DoNext(GrTableManager * ptman,
		int cslot, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPutGlyph(GrTableManager * ptman, bool fInserting, int nReplacementClass,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPutCopy(GrTableManager * ptman, bool fInserting, int cslotCopyFrom,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPutSubs(GrTableManager * ptman, bool fInserting,
		int cslotSel, int nSelClass, int nReplacementClass,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPutSubs2(GrTableManager * ptman, bool fInserting,
		int cslotSel1, int nSelClass1, int cslotSel2, int nSelClass2, int nReplacementClass,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPutSubs3(GrTableManager * ptman, bool fInserting,
		int cslotSel1, int nSelClass1, int cslotSel2, int nSelClass2, int cslotSel3, int nSelClass3,
		int nReplacementClass, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPutSubsInit(GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, bool fInserting,
		GrSlotState ** ppslotNextPut, bool * pfAtSegMin, bool * pfAtSegLim);
	void DoPutSubsAux(GrTableManager * ptman, bool fInserting, gid16 nGlyphReplacement,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, GrSlotState * pslotNextInput,
		bool fAtSegMin, bool fAtSegLim);
	void SetNeutralAssocs(GrSlotState * pslotNew, GrSlotStream * psstrmIn);
	void DoDelete(GrTableManager * ptman,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoAssoc(int cnAssocs, std::vector<int> & vnAssocs, bool fInserting,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoSetAttr(GrTableManager * ptman,
		ActionCommand op, bool fInserrting,
		SlotAttrName slat, int slati, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushSlotAttr(GrTableManager * ptman,
		int nSlotRef, bool fInserting,
		SlotAttrName slat, int slati, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushGlyphAttr(GrTableManager * ptman, int nSlotRef, bool fInserting, 
		int nGlyphAttr, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushAttToGlyphAttr(GrTableManager * ptman, int nSlotRef, bool fInserting,
		int nGlyphAttr, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushGlyphMetric(GrTableManager * ptman, int nSlotRef, bool fInserting,
		int nGlyphAttr, int nAttLevel, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushAttToGlyphMetric(GrTableManager * ptman, int nSlotRef, bool fInserting,
		int nGlyphAttr, int nAttLevel, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushFeatValue(GrTableManager * ptman, int islot, bool fInsering,
		int nFeat, std::vector<int> & vnStack,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	void DoPushProcState(GrTableManager * ptman, int nPState, std::vector<int> & vnStack);
protected:
	void DoPushGlyphMetricAux(GrTableManager * ptman,
		GrSlotState * pslot, int nGlyphAttr, int nAttLevel,
		std::vector<int> & vnStack, GrSlotStream * psstrmIn);
public:
	int SortKeyForRule(int ruln)
	{
		Assert(ruln < m_crul);
		return m_prgchwRuleSortKeys[ruln];
	}
	int PreModContextForRule(int ruln)
	{
		Assert(ruln < m_crul);
		return m_prgcritRulePreModContext[ruln];
	}

	int MaxRulePreContext()
	{
		if (m_pfsm)
			return m_pfsm->MaxRulePreContext();
		else
			return 0;
	}

	void RecordRuleFailed(int islot, int irul);
	void RecordRuleFired(int islot, int irul);
	void RecordHitMaxRuleLoop(int islot);
	void RecordHitMaxBackup(int islot);
#ifdef TRACING
	void LogRulesFiredAndFailed(std::ostream & strmOut, GrSlotStream * psstrmIn);
	void LogInsertionsAndDeletions(std::ostream & strmOut, GrSlotStream * psstrmOut);
#endif // TRACING

protected:
	int CheckRuleValidity(int ruln);
	
	virtual void RunRule(GrTableManager *, int ruln,
		GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput) = 0;

	void CheckInputProgress(GrSlotStream* input, GrSlotStream* psstrmOutput,
		int islotOrigInput);

	void MapChunks(GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
		int islotChunkI, int islotChunkO, int cslotReprocessed);

	virtual void Unattach(GrSlotStream * psstrmIn, int islotIn,	// GrPosPass overrides
		GrSlotStream * psstrmOut, int islotOut, int islotLB)
	{
	}

	friend class EngineState;	// let it call the method below
	virtual int MaxRuleContext()	// GrBidiPass overrides
	{
		return m_nMaxRuleContext;
	}

protected:
	//	Instance variables:

	int m_ipass;			// index of pass

	int m_fxdVersion;

	//	number of items required from previous pass; don't access directly, use the getter
	//	method, because GrBidiPass overrides to always use 1.
	int m_nMaxRuleContext;

	GrFSM *	m_pfsm;			// Finite State Machine to give next rule

	int m_nMaxRuleLoop;		// maximum number of rules to process before forcibly
							// advancing input position

	int m_nMaxBackup;

	int m_crul;		// number of rules

	//	rule sort keys, indicating precedence of rules; m_crul of these
	data16 * m_prgchwRuleSortKeys;

	//	for each rule, the number of items in the context before the first modified item
	//	that the constraints need to be tested on
	byte * m_prgcritRulePreModContext;

	//	offset for pass-level constraints; just 1 of these
	data16 m_cbPassConstraint;
	//	offsets for constraint and action code; m_crul+1 of each of these (the last
	//	indicates the total byte count)
	data16 * m_prgibConstraintStart;
	data16 * m_prgibActionStart;

	//	blocks of constraint and action code
	byte * m_prgbPConstraintBlock; // pass-level constraints
	byte * m_prgbConstraintBlock;  // rule constraints
	byte * m_prgbActionBlock;

	int m_cbConstraints;	// needed for memory instrumentation only
	int m_cbActions;		// needed for memory instrumentation only

	bool m_fHasDebugStrings;
	data16 * m_prgibConstraintDebug;	// m_crul+1 of these
	data16 * m_prgibRuleDebug;		// m_crul+1 of these

	bool m_fCheckRules;
	bool * m_prgfRuleOkay;

	std::vector<int> m_vnStack;	// for stack machine processing (more efficient than creating the
								// vector each time)

	//	state of process for this pass
	PassState * m_pzpst;

public:
#ifdef OLD_TEST_STUFF
	//	For test procedures:
	StrAnsi	m_staBehavior;

	virtual void SetUpTestData();

	// overridden on appropriate subclasses:
	virtual void SetUpReverseNumbersTest()				{ Assert(false); }
	virtual void SetUpBidiNumbersTest()					{ Assert(false); }

	virtual void SetUpCrossLineContextTest()			{ Assert(false); }
	virtual void SetUpReprocessTest()					{ Assert(false); }
	virtual void SetUpLineEdgeContextTest(int ipass)	{ Assert(false); }
	virtual void SetUpBidiAlgorithmTest()				{ Assert(false); }
	virtual void SetUpPseudoGlyphsTest()				{ Assert(false); }
	virtual void SetUpSimpleFSMTest()					{ Assert(false); }
	virtual void SetUpRuleActionTest()					{ Assert(false); }
	virtual void SetUpRuleAction2Test()					{ Assert(false); }
	virtual void SetUpAssocTest()						{ Assert(false); }
	virtual void SetUpAssoc2Test()						{ Assert(false); }
	virtual void SetUpDefaultAssocTest()				{ Assert(false); }
	virtual void SetUpFeatureTest()						{ Assert(false); }
	virtual void SetUpLigatureTest()					{ Assert(false); }
	virtual void SetUpLigature2Test()					{ Assert(false); }
#endif // OLD_TEST_STUFF

};	// end of class GrPass


/*----------------------------------------------------------------------------------------------
	The initial pass that generates glyph IDs from Unicode input.
	
	Hungarian: pass
----------------------------------------------------------------------------------------------*/
class GrGlyphGenPass : public GrPass
{
	friend class FontMemoryUsage;

public:
	GrGlyphGenPass(int ipass) : GrPass(ipass)
	{
	}

	virtual bool PreBidiPass()
	{
		return true;
	}

protected:
	//	Irrelevant when generating glyphs.
	virtual void RunRule(GrTableManager *, int ruln,
		GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
	{
		Assert(false);
	}

};	// end of class GrGlyphGenPass


/*----------------------------------------------------------------------------------------------
	A pass containing rules that set the break weight values.

	Hungarian: pass
----------------------------------------------------------------------------------------------*/
class GrLineBreakPass : public GrPass
{
	friend class FontMemoryUsage;

public:
	GrLineBreakPass(int ipass) : GrPass(ipass)
	{
	}

	virtual bool PreBidiPass()
	{
		return true;
	}

protected:
	virtual void RunRule(GrTableManager *, int ruln,
		GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput);

public:
#ifdef OLD_TEST_STUFF
	//	For test procedures:
	bool RunTestRules(GrTableManager *, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);
	bool RunTestRules(GrTableManager *, int ruln, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);

	virtual void SetUpCrossLineContextTest();
	bool RunCrossLineContextTest(GrTableManager*, GrSlotStream* psstrmIn,
		GrSlotStream* psstrmOut);

	virtual void SetUpReprocessTest();
	bool RunReprocessTest(GrTableManager *, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut);

	virtual void SetUpAssocTest();
	virtual void SetUpLigatureTest();
#endif // OLD_TEST_STUFF

};	// end of class GrLineBreakPass


/*----------------------------------------------------------------------------------------------
	A pass containing substitution rules.

	Hungarian: pass
----------------------------------------------------------------------------------------------*/
class GrSubPass : public GrPass
{
	friend class FontMemoryUsage;

public:
	GrSubPass(int ipass) : GrPass(ipass)
	{
	}

	virtual bool PreBidiPass()
	{
		return true;
	}

protected:
	virtual void RunRule(GrTableManager *, int ruln,
		GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput);

	virtual void DoCleanUpSegMin(GrTableManager * ptman,
		GrSlotStream * psstrmIn, int islotInitReadPos, GrSlotStream * psstrmOut);
	virtual void DoCleanUpSegLim(GrTableManager * ptman, GrSlotStream * psstrmOut,
		TrWsHandling twsh);

public:
#ifdef OLD_TEST_STUFF
	//	For test procedures:
	bool RunTestRules(GrTableManager *, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);
	bool RunTestRules(GrTableManager *, int ruln, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);

	virtual void SetUpReverseNumbersTest();
	bool RunReverseNumbersTest(GrTableManager*, GrSlotStream* psstrmIn,
		GrSlotStream* psstrmOut);

	virtual void SetUpBidiNumbersTest();
	bool RunBidiNumbersTest(GrTableManager*, GrSlotStream* psstrmIn,
		GrSlotStream* psstrmOut);

	virtual void SetUpCrossLineContextTest();
	bool RunCrossLineContextTest(GrTableManager*, GrSlotStream* psstrmIn,
		GrSlotStream* psstrmOut);

	virtual void SetUpReprocessTest();
	bool RunReprocessTest(GrTableManager *, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);

	virtual void SetUpLineEdgeContextTest(int ipass);
	bool RunLineEdgeContextTest(GrTableManager *, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);

	virtual void SetUpBidiAlgorithmTest();
	bool RunBidiAlgorithmTest(GrTableManager *, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);

	virtual void SetUpPseudoGlyphsTest();
	bool RunPseudoGlyphsTest(GrTableManager *, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);

	virtual void SetUpSimpleFSMTest();
	bool RunSimpleFSMTest(GrTableManager *, int ruln, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);
	virtual void SetUpRuleActionTest();
	virtual void SetUpRuleAction2Test();
	virtual void SetUpAssocTest();
	virtual void SetUpAssoc2Test();
	virtual void SetUpDefaultAssocTest();
	virtual void SetUpFeatureTest();
	virtual void SetUpLigatureTest();
	virtual void SetUpLigature2Test();
#endif // OLD_TEST_STUFF

};	// end of class GrSubPass


/*----------------------------------------------------------------------------------------------
	This class is the one the knows how to handle bidi reordering, including backtracking.

	Hungarian: pass
----------------------------------------------------------------------------------------------*/
class GrBidiPass : public GrSubPass
{
	friend class FontMemoryUsage;

public:
	//	Constructor:
	GrBidiPass(int ipass)
		:	GrSubPass(ipass),
			m_nTopDirection(0)
	{
	}

	virtual void SetTopDirLevel(int n)
	{
		m_nTopDirection = n;
	}

	virtual bool PreBidiPass()
	{
		return false;
	}

	virtual void ExtendOutput(GrTableManager *,
		GrSlotStream * psstrmI, GrSlotStream * psstrmO, int nslotToGet,
		TrWsHandling twsh,
		int * pnRet, int * pcslotGot, int * pislotFinalBreak);

	virtual int Unwind(GrTableManager * ptman,
		int islotChanged, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,
		bool fFirst);
protected:
	virtual int MaxRuleContext()
	{
		return 1;
	}

private:
	//void OldReverse(int nCurrDirection,
	//		 GrSlotStream * psstrmI,	int islotI1, int islotI2,
	//		 GrSlotStream * psstrmO,	int islotO1, int islotO2);

	int Reverse(GrTableManager * ptman,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut,			 
		std::vector<int> & vislotStarts, std::vector<int> & vislotStops);

protected:
	//	Instance variables:
	int m_nTopDirection;	// 0 for LTR, 1 for RTL -- CURRENTLY NOT USED; need to
							// initialize it when we set the writing system direction

#ifdef OLD_TEST_STUFF
public:
	//	For test procedures:
	////virtual void SetUpTestData();
	virtual void SetUpBidiNumbersTest();
	virtual void SetUpBidiAlgorithmTest();
#endif // OLD_TEST_STUFF

};	// end of class GrBidiPass


/*----------------------------------------------------------------------------------------------
	A pass containing positioning rules.

	Hungarian: pass
----------------------------------------------------------------------------------------------*/
class GrPosPass : public GrPass
{
	friend class FontMemoryUsage;

public:
	GrPosPass(int ipass) : GrPass(ipass)
	{
	}

	virtual bool IsPosPass()
	{
		return true;
	}

	virtual bool PreBidiPass()
	{
		return false;
	}

protected:
	virtual void RunRule(GrTableManager *, int ruln,
		GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput);

	virtual void Unattach(GrSlotStream * psstrmIn, int islotIn,
		GrSlotStream * psstrmOut, int islotOut, int islotLB);

public:
#ifdef OLD_TEST_STUFF
	//	For test procedures:
	bool RunTestRules(GrTableManager *, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);
	bool RunTestRules(GrTableManager *, int ruln, GrSlotStream * psstrmIn,
		GrSlotStream * psstrmOut);
#endif // OLD_TEST_STUFF

};	// end of class GrPosPass

} // namespace gr


#endif // PASS_INCLUDED
