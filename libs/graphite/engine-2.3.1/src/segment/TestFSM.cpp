/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TestFSM.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Hard-coded FSMs for test procedures.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:Ignore

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif
#undef THIS_FILE
DEFINE_THIS_FILE

#ifdef OLD_TEST_STUFF

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

#ifndef _DEBUG

bool GrLineBreakPass::RunTestRules(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

bool GrSubPass::RunTestRules(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

bool GrPosPass::RunTestRules(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}
#endif // !_DEBUG


#ifdef _DEBUG

/*----------------------------------------------------------------------------------------------
	Call the appropriate test function.
----------------------------------------------------------------------------------------------*/

bool GrLineBreakPass::RunTestRules(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

bool GrSubPass::RunTestRules(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	if (m_staBehavior == "SimpleFSM")
		return RunSimpleFSMTest(ptman, ruln, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "RuleAction")
		return false;
	else if (m_staBehavior == "RuleAction2")
		return false;
	else if (m_staBehavior == "Assoc")
		return false;
	else
		return false;
}

bool GrPosPass::RunTestRules(GrTableManager * ptman, int ruln,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}


/*----------------------------------------------------------------------------------------------
	A simple FSM.
	
	Rules:
		r1.	clsCons clsVowel	clsDiacrAcute
		r2.	clsCons clsVowelAE	clsDiacr
		r3.	clsCons clsVowelI
		r4.	clsCons clsVowelI	clsDiacrGrave
		r5.	clsCons clsVowelI	clsDiacr
	
	Machine classes:
		mcCons:	b,c,d / f,g,h / j,k,l,m,n / p,q,r,s,t / v,w,x,y,z
		mcVae:	a 97/ e 101
		mcVi:	i 105
		mcVou:	o 111 / u 117
		mcDa:	acute '/' 47
		mcDg:	grave '\' 92
		mcDct:	other diacritics-circumflex '^' 94 / tilde '~' 126


	FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	mcCons	mcVae	mcVi	mcVou	mcDa	mcDg	mcDct
	---------------------------------------------------------------------------
	T NA		s0	|	s1
					|		-		-		-		-		-		-		-
	T NA		s1	|			s2		s4		s3
					|		-		-		-		-		-		-		-
	T NA		s2	|									s5		s6		s6
					|		-		-		-		-		-		-		-
	T NA		s3	|									s10
					|		-		-		-		-		-		-		-
	T A			s4	|									s7		s8		s9
					|		-		-		-		-		-		-		-
	F A			s5	|
					|		-		-		-		-		-		-		-
	F A			s6	|
					|		-		-		-		-		-		-		-
	F A			s7	|
					|		-		-		-		-		-		-		-
	F A			s8	|
					|		-		-		-		-		-		-		-
	F A			s9	|
					|		-		-		-		-		-		-		-
	F A			s10	|
					|		-		-		-		-		-		-		-
	
	Rules Matched:
		s4:		r3
		s5:		r1, r2
		s6:		r2
		s7:		r1, r5
		s8:		r4, r5
		s9:		r5
		s10:	r1

----------------------------------------------------------------------------------------------*/

void GrSubPass::SetUpSimpleFSMTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 3;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "SimpleFSM";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpSimpleFSMTest();
}

void GrFSM::SetUpSimpleFSMTest()
{
	//	Create machine class ranges.
	m_cmcr = 14;
	m_prgmcr = new GrFSMClassRange[14];
	m_prgmcr[0].m_chwFirst	= 47;	// forward slash (acute): mcDa
	m_prgmcr[0].m_chwLast	= 47;
	m_prgmcr[0].m_col		= 4;

	m_prgmcr[1].m_chwFirst	= 92;	// backslash: mcDg
	m_prgmcr[1].m_chwLast	= 92;
	m_prgmcr[1].m_col		= 5;

	m_prgmcr[2].m_chwFirst	= 94;	// caret: mcDct
	m_prgmcr[2].m_chwLast	= 94;
	m_prgmcr[2].m_col		= 6;

	m_prgmcr[3].m_chwFirst	= 97;	// a: mcVae
	m_prgmcr[3].m_chwLast	= 97;
	m_prgmcr[3].m_col		= 1;

	m_prgmcr[4].m_chwFirst	= 98;	// b - d: mcCons
	m_prgmcr[4].m_chwLast	= 100;
	m_prgmcr[4].m_col		= 0;

	m_prgmcr[5].m_chwFirst	= 101;	// e: mcVae
	m_prgmcr[5].m_chwLast	= 101;
	m_prgmcr[5].m_col		= 1;

	m_prgmcr[6].m_chwFirst	= 102;	// f - h: mcCons
	m_prgmcr[6].m_chwLast	= 104;
	m_prgmcr[6].m_col		= 0;

	m_prgmcr[7].m_chwFirst	= 105;	// i: mcVi
	m_prgmcr[7].m_chwLast	= 105;
	m_prgmcr[7].m_col		= 2;

	m_prgmcr[8].m_chwFirst	= 106;	// j - n: mcCons
	m_prgmcr[8].m_chwLast	= 110;
	m_prgmcr[8].m_col		= 0;

	m_prgmcr[9].m_chwFirst	= 111;	// o: mcVou
	m_prgmcr[9].m_chwLast	= 111;
	m_prgmcr[9].m_col		= 3;

	m_prgmcr[10].m_chwFirst	= 112;	// p - t: mcCons
	m_prgmcr[10].m_chwLast	= 116;
	m_prgmcr[10].m_col		= 0;

	m_prgmcr[11].m_chwFirst	= 117;	// u: mcVou
	m_prgmcr[11].m_chwLast	= 117;
	m_prgmcr[11].m_col		= 3;

	m_prgmcr[12].m_chwFirst	= 118;	// v - z: mcCons
	m_prgmcr[12].m_chwLast	= 122;
	m_prgmcr[12].m_col		= 0;

	m_prgmcr[13].m_chwFirst	= 126;	// tilde: mcDct
	m_prgmcr[13].m_chwLast	= 126;
	m_prgmcr[13].m_col		= 6;

	m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
	m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
	m_imcrStart = m_cmcr - m_dimcrInit;


	m_crow = 11;
	m_crowNonAcpt = 4;
	m_crowFinal = 6;
	m_rowFinalMin = m_crow - m_crowFinal;
	m_ccol = 7;

	//	Set up transition table.
	m_prgrowTransitions = new short[35]; // 35 = (m_crow-m_crowFinal) * m_ccol
	short * psn = m_prgrowTransitions;
	*psn++ = 1; *psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 0;
	*psn++ = 0; *psn++ = 2; *psn++ = 4; *psn++ = 3; *psn++ = 0; *psn++ = 0; *psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 5; *psn++ = 6; *psn++ = 6;
	*psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ =10; *psn++ = 0; *psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 0; *psn++ = 7; *psn++ = 8; *psn++ = 9;

	//	Set up matched-rules tables.
	m_prgrulnMatched = new data16[10]; // 10 = sum of rules matched for each accepting state
	m_prgirulnMin = new data16[7+1];	// 7 = m_crow - m_crowNonAcpt

	m_prgirulnMin[0]	= 0;	// s4: r3
	m_prgrulnMatched[0] = 3;

	m_prgirulnMin[1]	= 1;	// s5: r1, r2
	m_prgrulnMatched[1] = 1;
	m_prgrulnMatched[2] = 2;

	m_prgirulnMin[2]	= 3;	// s6: r2
	m_prgrulnMatched[3] = 2;

	m_prgirulnMin[3]	= 4;	// s7: r1, r5
	m_prgrulnMatched[4] = 1;
	m_prgrulnMatched[5] = 5;

	m_prgirulnMin[4]	= 6;	// s8: r4, r5
	m_prgrulnMatched[6] = 4;
	m_prgrulnMatched[7] = 5;

	m_prgirulnMin[5]	= 8;	// s9: r5
	m_prgrulnMatched[8] = 5;

	m_prgirulnMin[6]	= 9;	// s10: r1
	m_prgrulnMatched[9] = 1;

	m_prgirulnMin[7]	= 10;
}


/*----------------------------------------------------------------------------------------------
	The "rule" does something silly: just inserts *'s--the number equals the number of the
	rule.
----------------------------------------------------------------------------------------------*/
bool GrSubPass::RunSimpleFSMTest(GrTableManager * ptman, int ruln, GrSlotStream * psstrmInput,
	GrSlotStream * psstrmOutput)
{
	int cslotMatched = (ruln == 3)? 2: 3;
	
	GrSlotState * pslot0 = psstrmInput->Peek(0);

	for (int islot = 0; islot < cslotMatched; islot++)
		psstrmOutput->CopyOneSlotFrom(psstrmInput);

	for (islot = 0; islot < ruln; islot++)
	{
		GrSlotState * pslot;
		ptman->NewSlot('*', pslot0, NULL, m_ipass, &pslot);
		pslot->Associate(pslot0);
		psstrmOutput->NextPut(pslot);
	}

	return true;
}


/*----------------------------------------------------------------------------------------------
	A simple set of rules with actions.
	
	Rules:
		r0.	clsDigit gSlash clsDigit _ > @3 _ @1 gTilde;			// tilde = 126
		r1. _ clsDigit gBackSlash clsDigit > gDollar @4 _ @2;		// dollar sign = 36
	
	Machine classes:
		mcDigit:	'0' - '9'  48 - 57
		mcSlash:	'/' 47
		mcBSlash:	'\' 92


	FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	clsDig		mcSlash		mcBSlash
	------------------------------------------------------
	T NA		s0	|	s1
					|		-			-			-
	T NA		s1	|				s2			s3
					|		-			-			-
	T NA		s2	|	s4
					|		-			-			-
	T NA		s3	|	s5
					|		-			-			-
	F A			s4	|
					|		-			-			-
	F A			s5	|
					|		-			-			-
	
	Rules Matched:
		s4:		r0
		s5:		r1

----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpRuleActionTest()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpRuleActionTest();
}

void GrClassTable::SetUpRuleActionTest()
{
	m_ccls = 2;				// number of classes
	m_cclsLinear = 2;		// number of classes in linear format

	m_prgchwBIGGlyphList = new data16[100];

	m_prgichwOffsets = new data16[2+1];

	gid16 * pchw = m_prgchwBIGGlyphList;

	//	Output class 0: tilde
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(126));	// '~'

	//	Output class 1: dollar
	m_prgichwOffsets[1] = 1;
	*pchw++ = msbf(data16(36));	// '$'

	m_prgichwOffsets[2] = 2;
}

void GrSubPass::SetUpRuleActionTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 3;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "RuleAction";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpRuleActionTest();

	m_crul = 2;

	//	Set up constraint code--succeed trivially.
	m_prgbConstraintBlock = new byte[3];
	byte * pb = m_prgbConstraintBlock;
	*pb++ = kopPushByte; *pb++ = 1;		// Push true
	*pb++ = kopPopRet;					// PopAndRet

	m_prgibConstraintStart = new data16[m_crul];
	m_prgibConstraintStart[0] = 0;
	m_prgibConstraintStart[1] = 0;

	//	Set up rule action code.
	m_prgbActionBlock = new byte[30];
	pb = m_prgbActionBlock;

	//	Rule 1: clsDigit gSlash clsDigit _ > @3 _ @1 gTilde;

	*pb++ = kopPutCopy;		*pb++ = 2;		// PutCopy 2
	*pb++ = kopNext;						// Next
	*pb++ = kopDelete;						// Delete
	*pb++ = kopNext;						// Next
	*pb++ = kopPutCopy;		*pb++ = -2;		// PutCopy -2
	*pb++ = kopNext;						// Next
	*pb++ = kopInsert;						// Insert
	*pb++ = kopPutGlyph8bitObs;	*pb++ = 0;	// PutGlyph 0 ('~' = 126)
	*pb++ = kopNext;						// Next
	*pb++ = kopPushByte;	*pb++ = 0;		// Push 0
	*pb++ = kopPopRet;						// PopAndRet

	//	Rule 2: _ clsDigit gBackSlash clsDigit > gDollar @4 _ @2;

	*pb++ = kopInsert;						// Insert
	*pb++ = kopPutGlyph8bitObs;	*pb++ = 1;	// PutGlyph 1 ('$' = 36)
	*pb++ = kopNext;						// Next
	*pb++ = kopPutCopy;		*pb++ = 2;		// PutCopy 2
	*pb++ = kopNext;						// Next
	*pb++ = kopDelete;						// Delete
	*pb++ = kopNext;						// Next
	*pb++ = kopPutCopy;		*pb++ = -2;		// PutCopy -2
	*pb++ = kopNext;						// Next
	*pb++ = kopPushByte;	*pb++ = 0;		// Push 0
	*pb++ = kopPopRet;						// PopAndRet

	m_prgibActionStart = new data16[m_crul];
	m_prgibActionStart[0] = 0;
	m_prgibActionStart[1] = 15;
}

void GrFSM::SetUpRuleActionTest()
{
	//	Create machine class ranges.
	m_cmcr = 3;
	m_prgmcr = new GrFSMClassRange[3];
	m_prgmcr[0].m_chwFirst	= 47;	// forward slash: mcSlash
	m_prgmcr[0].m_chwLast	= 47;
	m_prgmcr[0].m_col		= 1;

	m_prgmcr[1].m_chwFirst	= 48;	// 0 - 9: mcDigit
	m_prgmcr[1].m_chwLast	= 57;
	m_prgmcr[1].m_col		= 0;

	m_prgmcr[2].m_chwFirst	= 92;	// backslash
	m_prgmcr[2].m_chwLast	= 92;
	m_prgmcr[2].m_col		= 2;


	m_dimcrInit = 2;		// (max power of 2 <= m_cmcr);
	m_cLoop = 1;			// log2(max power of 2 <= m_cmcr);
	m_imcrStart = m_cmcr - m_dimcrInit;


	m_crow = 6;
	m_crowNonAcpt = 4;
	m_crowFinal = 2;
	m_rowFinalMin = m_crow - m_crowFinal;
	m_ccol = 3;

	//	Set up transition table.
	m_prgrowTransitions = new short[12]; // 12 = (m_crow-m_crowFinal) * m_ccol
	short * psn = m_prgrowTransitions;
	*psn++ = 1; *psn++ = 0; *psn++ = 0;
	*psn++ = 0; *psn++ = 2; *psn++ = 3;
	*psn++ = 4; *psn++ = 0; *psn++ = 0;
	*psn++ = 5; *psn++ = 0; *psn++ = 0;

	//	Set up matched-rules tables.
	m_prgrulnMatched = new data16[2];	// 2 = sum of rules matched for each accepting state
	m_prgirulnMin = new data16[2+1];	// 2 = m_crow - m_crowNonAcpt

	m_prgirulnMin[0]	= 0;	// s4: r0
	m_prgrulnMatched[0] = 0;

	m_prgirulnMin[1]	= 1;	// s5: r1
	m_prgrulnMatched[1] = 1;

	m_prgirulnMin[2]	= 2;
}

/*----------------------------------------------------------------------------------------------
	A more complicated set of rules with actions.
	
	PASS 1:
		r0. clsDigit   >  @1 {dir = DIR_RIGHT} / ^ _ {dir == DIR_LEFT};
		r1. clsSpecial >  @2 {dir = DIR_ARABNUMBER} / clsDigit {dir != DIR_LEFT} ^ _;
		r2. clsSpecial >  @2 {dir = @1.dir}  /  clsSpecial ^ _;

	Pass 1 machine classes:
		mcDigit:	'0' - '9'  48 - 57
		mcSpecial:	'-', '_', '=', '~'  45, 95, 61, 126

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	mcDigit		mcSpecial
	------------------------------------------
	T NA		s0	|	s2			s1
					|		-			_
	T NA		s1	|				s4
					|		-			-
	T A			s2	|				s3
					|		-			-
	F A			s3	|
					|		-			-
	F A			s4	|
					|		-			-
	
	Rules Matched:
		s2:		r0
		s3:		r1
		s4:		r2

	PASS 2:
		r0.	clsVowel gCaret  >  clsCirumVowel _;
		r1.	clsVowel gSlash  >  _ clsAcuteVowel$1;

	Pass 2 machine classes:
		mcVowel
		mcCaret:	'^' 94
		mcSlash:	'/' 47


	Pass 2 FSM:
					|	mcVowel		mcCaret		mcSlash
	-----------------------------------------------------
	T NA		s0	|	s1
					|		-			_
	T NA		s1	|				s2			s3
					|		-			-
	F A			s2	|
					|		-			-
	F A			s3	|
					|		-			-

	Rules matched:
		s2:		r0
		s3:		r1


	PASS 3:
		r0. clsLeft   >  clsRight {dir = @1.dir} / _ clsDir {dir==DIR_RIGHT};
		r1. clsRight  >  clsLeft  {dir = @1.dir} / clsDir {dir==DIR_RIGHT} _;

	Pass 3 machine classes:
		mcDir:		(clsDigit, clsSpecial)
		mcLeft:		'(', '[', '{'  40, 91, 123
		mcRight:	')', ']', '}'  41, 93, 125

	Pass 3 FSM:
					|	mcDir		mcLeft		mcRight
	-----------------------------------------------------
	T NA		s0	|	s2			s1
					|		-			_
	T NA		s1	|	s3
					|		-			-
	T NA		s2	|							s4
					|		-			-
	F A			s3	|
					|		-			-
	F A			s4	|
					|		-			-

	Rules matched:
		s3:		r0
		s4:		r1


----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpRuleAction2Test()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpRuleAction2Test();
}

void GrClassTable::SetUpRuleAction2Test()
{
	m_ccls = 7;				// number of classes
	m_cclsLinear = 4;		// number of classes in linear format

	m_prgchwBIGGlyphList = new data16[70];

	m_prgichwOffsets = new data16[7+1];

	data16 * pchw = m_prgchwBIGGlyphList;

	//	Output class 0: clsCirumVowel
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(226));	// a
	*pchw++ = msbf(data16(234));	// e
	*pchw++ = msbf(data16(238));	// i
	*pchw++ = msbf(data16(244));	// o
	*pchw++ = msbf(data16(251));	// u
	*pchw++ = msbf(data16(194));	// A
	*pchw++ = msbf(data16(202));	// E
	*pchw++ = msbf(data16(206));	// I
	*pchw++ = msbf(data16(212));	// O
	*pchw++ = msbf(data16(219));	// U

	//	Output class 1: clsAcuteVowel
	m_prgichwOffsets[1] = 10;
	*pchw++ = msbf(data16(225));	// a
	*pchw++ = msbf(data16(233));	// e
	*pchw++ = msbf(data16(237));	// i
	*pchw++ = msbf(data16(243));	// o
	*pchw++ = msbf(data16(250));	// u
	*pchw++ = msbf(data16(193));	// A
	*pchw++ = msbf(data16(201));	// E
	*pchw++ = msbf(data16(205));	// I
	*pchw++ = msbf(data16(211));	// O
	*pchw++ = msbf(data16(218));	// U

	//	Output class 2: clsLeft
	m_prgichwOffsets[2] = 10 + 10;
	*pchw++ = msbf(data16(40));	// (
	*pchw++ = msbf(data16(91));	// [
	*pchw++ = msbf(data16(123));	// {

	//	Output class 3: clsRight
	m_prgichwOffsets[3] = 20 + 3;
	*pchw++ = msbf(data16(41));	// )
	*pchw++ = msbf(data16(93));	// ]
	*pchw++ = msbf(data16(125));	// }

	//	Input class 4: clsVowel
	m_prgichwOffsets[4] = 23 + 3;	// 26
	*pchw++ = msbf(data16(10));
	*pchw++ = msbf(data16(8));	*pchw++ = msbf(data16(3)); *pchw++ = msbf(data16(10-8));
	*pchw++ = msbf(data16(65));	*pchw++ = msbf(data16(5));	// A
	*pchw++ = msbf(data16(69));	*pchw++ = msbf(data16(6));	// E
	*pchw++ = msbf(data16(73));	*pchw++ = msbf(data16(7));	// I
	*pchw++ = msbf(data16(79));	*pchw++ = msbf(data16(8));	// O
	*pchw++ = msbf(data16(85));	*pchw++ = msbf(data16(9));	// U
	*pchw++ = msbf(data16(97));	*pchw++ = msbf(data16(0));	// a
	*pchw++ = msbf(data16(101));	*pchw++ = msbf(data16(1));	// e
	*pchw++ = msbf(data16(105));	*pchw++ = msbf(data16(2));	// i
	*pchw++ = msbf(data16(111));	*pchw++ = msbf(data16(3));	// o
	*pchw++ = msbf(data16(117));	*pchw++ = msbf(data16(4));	// u

	//	Input class 5: clsLeft
	m_prgichwOffsets[5] = 26 + 4 + 10*2;	// 50
	*pchw++ = msbf(data16(3));
	*pchw++ = msbf(data16(2));	*pchw++ = msbf(data16(1));	*pchw++ = msbf(data16(3-2));
	*pchw++ = msbf(data16(40));	*pchw++ = msbf(data16(0));	// (
	*pchw++ = msbf(data16(91));	*pchw++ = msbf(data16(1));	// [
	*pchw++ = msbf(data16(123));	*pchw++ = msbf(data16(2));	// {

	//	Input class 6: clsRight
	m_prgichwOffsets[6] = 50 + 4 + 3*2;		// 60
	*pchw++ = msbf(data16(3));
	*pchw++ = msbf(data16(2));	*pchw++ = msbf(data16(1));	*pchw++ = msbf(data16(3-2));
	*pchw++ = msbf(data16(41));	*pchw++ = msbf(data16(0));	// )
	*pchw++ = msbf(data16(93));	*pchw++ = msbf(data16(1));	// ]
	*pchw++ = msbf(data16(125));	*pchw++ = msbf(data16(2));	// }

	m_prgichwOffsets[7] = 60 + 4 + 3*2;		// 70
}

void GrSubPass::SetUpRuleAction2Test()
{
	if (m_ipass == 1)
	{
		m_nMaxRuleContext = m_nMaxChunk = 2;
		m_nMaxRuleLoop = 5;
		m_staBehavior = "RuleAction2";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpRuleAction2Test(m_ipass);
	
		m_crul = 3;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1 + 10 + 10];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		*pb++ = kopCntxtItem;	*pb++ = 0;			// ContextItem 0
		*pb++ = kopPushSlotAttr; *pb++= kslatDir;	// PushSlotAttr dir 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = kdircL;		// Push DIR_LEFT
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		*pb++ = kopCntxtItem;	*pb++ = 0;			// ContextItem 0
		*pb++ = kopPushSlotAttr; *pb++= kslatDir;	// PushSlotAttr dir 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = kdircL;		// Push DIR_LEFT
		*pb++ = kopNotEq;							// NotEqual
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 1;
		m_prgibConstraintStart[1] = 11;
		m_prgibConstraintStart[2] = 0;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[10 + 11 + 12];
		pb = m_prgbActionBlock;

		//	Rule 0: clsDigit   >  @1 {dir = DIR_RIGHT} / ^ _ {dir == DIR_LEFT};

		*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
		*pb++ = kopPushByte;	*pb++ = kdircR;		// PushByte DIR_RIGHT
		*pb++ = kopAttrSet;		*pb++ = kslatDir;	// AttrSet dir
		*pb++ = kopNext;							// Next
		*pb++ = kopPushByte;	*pb++ = -1;			// Push -1
		*pb++ = kopPopRet;							// PopAndRet

		//	Rule 1: clsSpecial >  @2 {dir = DIR_ARABNUMBER} / clsDigit {dir != DIR_LEFT} ^ _;

		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
		*pb++ = kopPushByte;  *pb++ = kdircArabNum;	// Push DIR_ARABNUMBER
		*pb++ = kopAttrSet;		*pb++ = kslatDir;	// AttrSet dir
		*pb++ = kopNext;							// Next
		*pb++ = kopPushByte;	*pb++ = -1;			// Push -1
		*pb++ = kopPopRet;							// PopAndRet

		//	Rule 2: clsSpecial >  @2 {dir = @1.dir} / clsSpecial ^ _;

		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
		*pb++ = kopPushSlotAttr;*pb++ = kslatDir;	// PushSlotAttr dir -1
								*pb++ = -1;
		*pb++ = kopAttrSet;		*pb++ = kslatDir;	// AttrSet dir
		*pb++ = kopNext;							// Next
		*pb++ = kopPushByte;	*pb++ = -1;			// Push -1
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
		m_prgibActionStart[1] = 10;
		m_prgibActionStart[2] = 10 + 11;
	}
	else if (m_ipass == 2)
	{
		m_nMaxRuleContext = m_nMaxChunk = 2;
		m_nMaxRuleLoop = 3;
		m_staBehavior = "RuleAction2";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpRuleAction2Test(m_ipass);
	
		m_crul = 2;

		//	Set up constraint code--succeed trivially.
		m_prgbConstraintBlock = new byte[1];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;				// RetTrue

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 0;
		m_prgibConstraintStart[1] = 0;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[16];
		pb = m_prgbActionBlock;

		//	Rule 0: clsVowel gCaret  >  clsCirumVowel _;

		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 clsVowel clsCircumVowel
			*pb++ = 4;				*pb++ = 0;
		*pb++ = kopNext;							// Next
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 1: clsVowel gSlash  >  _ clsAcuteVowel$1;

		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopPutSubs8bitObs;	*pb++ = -1;		// PutSubs -1 clsVowel clsAcuteVowel
			*pb++ = 4;				*pb++ = 1;
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
		m_prgibActionStart[1] = 8;
	}
	else if (m_ipass == 3)
	{
		m_nMaxRuleContext = m_nMaxChunk = 2;
		m_nMaxRuleLoop = 3;
		m_staBehavior = "RuleAction2";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpRuleAction2Test(m_ipass);
	
		m_crul = 2;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1 + 10 + 10];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		*pb++ = kopCntxtItem;	*pb++ = 1;			// ContextItem 1
		*pb++ = kopPushSlotAttr;*pb++ = kslatDir;	// PushSlotAttr dir 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = kdircR;		// Push DIR_RIGHT
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		*pb++ = kopCntxtItem;	*pb++ = 0;			// ContextItem 0
		*pb++ = kopPushSlotAttr;*pb++ = kslatDir;	// PushSlotAttr dir 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = kdircR;		// Push DIR_RIGHT
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 1;
		m_prgibConstraintStart[1] = 1 + 10;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[23];
		pb = m_prgbActionBlock;

		//	Rule 0: clsLeft  >  clsRight {dir = @1.dir} / _ clsDir {dir == DIR_RIGHT};

		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 clsLeft clsRight
			*pb++ = 5;				*pb++ = 3;
		*pb++ = kopPushSlotAttr;*pb++ = kslatDir;	// PushSlotAttr dir 1
								*pb++ = 1;
		*pb++ = kopAttrSet;		*pb++ = kslatDir;	// AttrSet dir
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 1: clsRight >  clsLeft  {dir = @1.dir} / clsDir {dir == DIR_RIGHT} _;

		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 clsRight clsLeft
			*pb++ = 6;				*pb++ = 2;
		*pb++ = kopPushSlotAttr;*pb++ = kslatDir;	// PushSlotAttr dir -1
								*pb++ = -1;
		*pb++ = kopAttrSet;		*pb++ = kslatDir;	// AttrSet dir
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
		m_prgibActionStart[1] = 11;
	}
}


void GrFSM::SetUpRuleAction2Test(int ipass)
{
	if (ipass == 1)
	{
		//	Create machine class ranges.
		m_cmcr = 5;
		m_prgmcr = new GrFSMClassRange[5];
		m_prgmcr[0].m_chwFirst	= 45;	// hyphen
		m_prgmcr[0].m_chwLast	= 45;
		m_prgmcr[0].m_col		= 1;

		m_prgmcr[1].m_chwFirst	= 48;	// 0 - 9: mcDigit
		m_prgmcr[1].m_chwLast	= 57;
		m_prgmcr[1].m_col		= 0;

		m_prgmcr[2].m_chwFirst	= 61;	// equals sign
		m_prgmcr[2].m_chwLast	= 61;
		m_prgmcr[2].m_col		= 1;

		m_prgmcr[3].m_chwFirst	= 95;	// underscore
		m_prgmcr[3].m_chwLast	= 95;
		m_prgmcr[3].m_col		= 1;

		m_prgmcr[4].m_chwFirst	= 126;	// tilde
		m_prgmcr[4].m_chwLast	= 126;
		m_prgmcr[4].m_col		= 1;

		m_dimcrInit = 4;		// (max power of 2 <= m_cmcr);
		m_cLoop = 2;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 5;
		m_crowNonAcpt = 2;
		m_crowFinal = 2;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 2;

		//	Set up transition table.
		m_prgrowTransitions = new short[6]; // 6 = (m_crow-m_crowFinal) * m_ccol
		short * psn = m_prgrowTransitions;
		*psn++ = 2; *psn++ = 1;
		*psn++ = 0; *psn++ = 4;
		*psn++ = 0; *psn++ = 3;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[3];	// 3 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[3+1];	// 3 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s2: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;	// s3: r1
		m_prgrulnMatched[1] = 1;

		m_prgirulnMin[2]	= 2;	// s3: r2
		m_prgrulnMatched[2] = 2;

		m_prgirulnMin[3]	= 3;
	}
	else if (ipass == 2)
	{
		//	Create machine class ranges.
		m_cmcr = 12;
		m_prgmcr = new GrFSMClassRange[12];
		m_prgmcr[0].m_chwFirst	= 47;	// slash
		m_prgmcr[0].m_chwLast	= 47;
		m_prgmcr[0].m_col		= 2;

		m_prgmcr[1].m_chwFirst	= 65;	// A
		m_prgmcr[1].m_chwLast	= 65;
		m_prgmcr[1].m_col		= 0;

		m_prgmcr[2].m_chwFirst	= 69;	// E
		m_prgmcr[2].m_chwLast	= 69;
		m_prgmcr[2].m_col		= 0;

		m_prgmcr[3].m_chwFirst	= 73;	// I
		m_prgmcr[3].m_chwLast	= 73;
		m_prgmcr[3].m_col		= 0;

		m_prgmcr[4].m_chwFirst	= 79;	// O
		m_prgmcr[4].m_chwLast	= 79;
		m_prgmcr[4].m_col		= 0;

		m_prgmcr[5].m_chwFirst	= 85;	// U
		m_prgmcr[5].m_chwLast	= 85;
		m_prgmcr[5].m_col		= 0;

		m_prgmcr[6].m_chwFirst	= 94;	// caret
		m_prgmcr[6].m_chwLast	= 94;
		m_prgmcr[6].m_col		= 1;

		m_prgmcr[7].m_chwFirst	= 97;	// a
		m_prgmcr[7].m_chwLast	= 97;
		m_prgmcr[7].m_col		= 0;

		m_prgmcr[8].m_chwFirst	= 101;	// e
		m_prgmcr[8].m_chwLast	= 101;
		m_prgmcr[8].m_col		= 0;

		m_prgmcr[9].m_chwFirst	= 105;	// i
		m_prgmcr[9].m_chwLast	= 105;
		m_prgmcr[9].m_col		= 0;

		m_prgmcr[10].m_chwFirst	= 111;	// o
		m_prgmcr[10].m_chwLast	= 111;
		m_prgmcr[10].m_col		= 0;

		m_prgmcr[11].m_chwFirst	= 117;	// u
		m_prgmcr[11].m_chwLast	= 117;
		m_prgmcr[11].m_col		= 0;

		m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
		m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 4;
		m_crowNonAcpt = 2;
		m_crowFinal = 2;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 3;

		//	Set up transition table.
		m_prgrowTransitions = new short[6]; // 6 = (m_crow-m_crowFinal) * m_ccol
		short * psn = m_prgrowTransitions;
		*psn++ = 1; *psn++ = 0; *psn++ = 0;
		*psn++ = 0; *psn++ = 2; *psn++ = 3;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[2];	// 2 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[2+1];	// 2 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s1: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;	// s2: r1
		m_prgrulnMatched[1] = 1;

		m_prgirulnMin[2]	= 2;
	}
	else if (ipass == 3)
	{
		//	Create machine class ranges.
		m_cmcr = 11;
		m_prgmcr = new GrFSMClassRange[11];
		m_prgmcr[0].m_chwFirst	= 40;	// (
		m_prgmcr[0].m_chwLast	= 40;
		m_prgmcr[0].m_col		= 1;

		m_prgmcr[1].m_chwFirst	= 41;	// )
		m_prgmcr[1].m_chwLast	= 41;
		m_prgmcr[1].m_col		= 2;

		m_prgmcr[2].m_chwFirst	= 45;	// hyphen
		m_prgmcr[2].m_chwLast	= 45;
		m_prgmcr[2].m_col		= 0;

		m_prgmcr[3].m_chwFirst	= 48;	// 0 - 9: mcDigit
		m_prgmcr[3].m_chwLast	= 57;
		m_prgmcr[3].m_col		= 0;

		m_prgmcr[4].m_chwFirst	= 61;	// =
		m_prgmcr[4].m_chwLast	= 61;
		m_prgmcr[4].m_col		= 0;

		m_prgmcr[5].m_chwFirst	= 91;	// [
		m_prgmcr[5].m_chwLast	= 91;
		m_prgmcr[5].m_col		= 1;

		m_prgmcr[6].m_chwFirst	= 93;	// ]
		m_prgmcr[6].m_chwLast	= 93;
		m_prgmcr[6].m_col		= 2;

		m_prgmcr[7].m_chwFirst	= 95;	// underscore
		m_prgmcr[7].m_chwLast	= 95;
		m_prgmcr[7].m_col		= 0;

		m_prgmcr[8].m_chwFirst	= 123;	// {
		m_prgmcr[8].m_chwLast	= 123;
		m_prgmcr[8].m_col		= 1;

		m_prgmcr[9].m_chwFirst	= 125;	// }
		m_prgmcr[9].m_chwLast	= 125;
		m_prgmcr[9].m_col		= 2;

		m_prgmcr[10].m_chwFirst	= 126;	// tilde
		m_prgmcr[10].m_chwLast	= 126;
		m_prgmcr[10].m_col		= 0;

		m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
		m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 5;
		m_crowNonAcpt = 3;
		m_crowFinal = 2;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 3;

		//	Set up transition table.
		m_prgrowTransitions = new short[9]; // 9 = (m_crow-m_crowFinal) * m_ccol
		short * psn = m_prgrowTransitions;
		*psn++ = 2; *psn++ = 1; *psn++ = 0;
		*psn++ = 3; *psn++ = 0; *psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 4;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[2];	// 2 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[2+1];	// 2 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s3: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;	// s4: r1
		m_prgrulnMatched[1] = 1;

		m_prgirulnMin[2]	= 2;
	}
}


/*----------------------------------------------------------------------------------------------
	Tests of various kinds of assocations.
	
	PASS 1 (line-break):
		r0. gX / # _ ^ ;	// do nothing
		r1. gX {break = -2};

	Pass 1 machine classes:
		mcX:	'x' 120
		mcLB:	35

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	gX		gLB
	--------------------------------
	T NA		s0	|	s2		s1
					|
	T NA		s1	|	s3
					|
	F A			s2	|
					|
	F A			s3	|
	

	Rules Matched:
		s2:		r1
		s3:		r0



	PASS 2:
		r0.	clsOA clsE  >  clsELig:(1 2) _;
		r1.	clsLC _ _ gX  >  @1 @5 @1 @5 / _ _ # {break==2} _ _ ;
		r2. _ clsFraction _  >  clsNumerator:2  gSlash:2  clsDenom:2;

	Pass 2 machine classes:
		mcOA:			O A			79 65
		mcOAlc:			o a			111 97
		mcE				E e			69 101
		mcFraction:					188 189 190
		mcX:			x			120
		mcLCother
		mcLB:			35


	Pass 2 FSM:											mcLC
					|	mcOA	mcOAlc	mcE		mcElc	other	mcX		mcFractn	mcLB
	-----------------------------------------------------------------------------------------
	T NA		s0	|	s1		s2				s4		s3		s3		s6
					|		-		-		-		-		-		-			-
	T NA		s1	|					s7
					|		-		-		-		-		-		-			-
	T NA		s2	|							s7									s5
					|		-		-		-		-		-		-			-
	T NA		s3	|																s5
					|		-		-		-		-		-		-			-
	T NA		s4	|																s5													
					|		-		-		-		-		-		-			-
	T NA		s5	|											s8													
					|		-		-		-		-		-		-			-
	F A			s6	|
					|		-		-		-		-		-		-
	F A			s7	|
					|		-		-		-		-		-		-
	F A			s8	|
					|		-		-		-		-		-		-

	Rules matched:
		s6:		r2
		s7:		r0
		s8:		r1

  
	PASS 3:
		r0. _  >  gHyphen  / _ # {break == 2} ^;

	Pass 3 machine classes:
		mcLB:	35

	Pass 3 FSM:

					|	gLB
	--------------------------------
	T NA		s0	|	s1
					|
	F A			s1	|
					|

	Rules Matched:
		s1:		r0
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpAssocTest()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpAssocTest();

	m_fLineBreak = true;
	m_cchwPreXlbContext = 1;
	m_cchwPostXlbContext = 1;
}

void GrClassTable::SetUpAssocTest()
{
	m_ccls = 11;			// number of classes
	m_cclsLinear = 5;		// number of classes in linear format (output classes)

	m_prgchwBIGGlyphList = new data16[143];

	m_prgichwOffsets = new data16[m_ccls+1];

	data16 * pchw = m_prgchwBIGGlyphList;

	//	Output class 0: clsELig
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(140));	// OE
	*pchw++ = msbf(data16(198));	// AE
	*pchw++ = msbf(data16(156));	// oe
	*pchw++ = msbf(data16(230));	// ae

	//	Output class 1: clsNumerator
	m_prgichwOffsets[1] = 4;
	*pchw++ = msbf(data16(49));	// 1
	*pchw++ = msbf(data16(49));	// 1
	*pchw++ = msbf(data16(51));	// 3

	//	Output class 2: clsDenom
	m_prgichwOffsets[2] = 4 + 3;
	*pchw++ = msbf(data16(52));	// 4
	*pchw++ = msbf(data16(50));	// 2
	*pchw++ = msbf(data16(52));	// 4

	//	Output class 3: gSlash
	m_prgichwOffsets[3] = 7 + 3;
	*pchw++ = msbf(data16(47));	// '/'

	//	Output class 4: gHyphen
	m_prgichwOffsets[4] = 10 + 1;
	*pchw++ = msbf(data16(45));	// '-'

	//	Input class 5: clsOA
	m_prgichwOffsets[5] = 11 + 1;
	*pchw++ = msbf(data16(4));
	*pchw++ = msbf(data16(4));	*pchw++ = msbf(data16(2)); *pchw++ = msbf(data16(0));
	*pchw++ = msbf(data16(65));	*pchw++ = msbf(data16(1));	// A
	*pchw++ = msbf(data16(79));	*pchw++ = msbf(data16(0));	// O
	*pchw++ = msbf(data16(97));	*pchw++ = msbf(data16(3));	// a
	*pchw++ = msbf(data16(111));	*pchw++ = msbf(data16(2));	// o

	//	Input class 6: clsE
	m_prgichwOffsets[6] = 12 + 4 + 4*2;		// 24
	*pchw++ = msbf(data16(3));
	*pchw++ = msbf(data16(2));	*pchw++ = msbf(data16(1));	*pchw++ = msbf(data16(3-2));
	*pchw++ = msbf(data16(40));	*pchw++ = msbf(data16(0));	// E
	*pchw++ = msbf(data16(91));	*pchw++ = msbf(data16(1));	// e

	//	Input class 7: clsLC
	m_prgichwOffsets[7] = 24 + 4 + 2*2;		// 32
	*pchw++ = msbf(data16(26));
	*pchw++ = msbf(data16(16));	*pchw++ = msbf(data16(4));	*pchw++ = msbf(data16(26-16));
	for (int i = 0; i < 26; ++i)
	{
		*pchw++ = msbf(data16(97+i));	*pchw++ = msbf(data16(i));
	}

	//	Input class 8: gX
	m_prgichwOffsets[8] = 32 + 4 + 26*2;	// 88
	*pchw++ = msbf(data16(1));
	*pchw++ = msbf(data16(1));	*pchw++ = msbf(data16(0));	*pchw++ = msbf(data16(0));
	*pchw++ = msbf(data16(120));	*pchw++ = msbf(data16(0));

	//	Input class 9: clsFraction
	m_prgichwOffsets[9] = 88 + 4 + 1*2;	// 127
	*pchw++ = msbf(data16(3));
	*pchw++ = msbf(data16(2));	*pchw++ = msbf(data16(1));	*pchw++ = msbf(data16(3-2));
	*pchw++ = msbf(data16(188));	*pchw++ = msbf(data16(0));	// 1/4
	*pchw++ = msbf(data16(189));	*pchw++ = msbf(data16(1));	// 1/2
	*pchw++ = msbf(data16(190));	*pchw++ = msbf(data16(2));	// 3/4

	//	Input class 10: #
	m_prgichwOffsets[10] = 127 + 4 + 3*2;	// 137
	*pchw++ = msbf(data16(1));
	*pchw++ = msbf(data16(1));	*pchw++ = msbf(data16(0));	*pchw++ = msbf(data16(0));
	*pchw++ = msbf(data16(35));	*pchw++ = msbf(data16(0));	// 1/4

	m_prgichwOffsets[11] = 137 + 4 + 1*2;	// 143
}

void GrLineBreakPass::SetUpAssocTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 5;
	m_staBehavior = "Assoc";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpAssocTest(m_ipass);

	m_crul = 2;

	//	Set up constraint code--succeed trivially.
	m_prgbConstraintBlock = new byte[1];
	byte * pb = m_prgbConstraintBlock;
	*pb++ = kopRetTrue;							// RetTrue

	m_prgibConstraintStart = new data16[m_crul];
	m_prgibConstraintStart[0] = 0;
	m_prgibConstraintStart[1] = 0;

	//	Set up rule action code.
	m_prgbActionBlock = new byte[7 + 8];
	pb = m_prgbActionBlock;

	//	Rule 0: gX / # _ ^ ;	// do nothing

	*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
	*pb++ = kopNext;							// Next
	*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 1: gX {break = -2};

	*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
	*pb++ = kopPushByte;	*pb++ = -2;			// PushByte -2
	*pb++ = kopAttrSet;		*pb++ = kslatBreak;	// AttrSet breakweight
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	m_prgibActionStart = new data16[m_crul];
	m_prgibActionStart[0] = 0;
	m_prgibActionStart[1] = 7;
}

void GrSubPass::SetUpAssocTest()
{
	if (m_ipass == 2)
	{
		m_nMaxRuleContext = m_nMaxChunk = 3;
		m_nMaxRuleLoop = 5;
		m_staBehavior = "Assoc";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpAssocTest(m_ipass);

		m_crul = 3;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1 + 10];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		*pb++ = kopCntxtItem;	*pb++ = 1;			// ContextItem 1
		*pb++ = kopPushSlotAttr;*pb++ = kslatBreak;	// PushSlotAttr break 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = 2;			// Push 2
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 0;
		m_prgibConstraintStart[1] = 1;
		m_prgibConstraintStart[2] = 0;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[12 + 12 + 22];
		pb = m_prgbActionBlock;

		//	Rule 0: clsOA clsE  >  clsELig:(1 2) _ ;

		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 clsOA clsELig
			*pb++ = 5;				*pb++ = 0;
		*pb++ = kopAssoc;		*pb++ = 2;			// Assoc 2 0 1
			*pb++ = 0;			*pb++ = 1;
		*pb++ = kopNext;							// Next
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 1: clsLC _ _ gX  >  @1 @5 @1 @5 / _ _ # {break==2} _ _ ;

		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopInsert;							// Insert
		*pb++ = kopPutCopy;		*pb++ = 2;			// PutCopy 2
		*pb++ = kopNext;							// Next
		*pb++ = kopCopyNext;						// CopyNext - line break
		*pb++ = kopInsert;							// Insert
		*pb++ = kopPutCopy;		*pb++ = -1;			// PutCopy -1
		*pb++ = kopNext;							// Next
		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopRetZero;							// RetZero

		//	Rule 2: _ clsFraction _  >  clsNumerator:2  gSlash:2  clsDenom:2;

		*pb++ = kopInsert;							// Insert
		*pb++ = kopPutSubs8bitObs;	*pb++ = 1;		// PutSubs 1 clsFraction clsNumerator
			*pb++ = 9;			*pb++ = 1;
		*pb++ = kopAssoc;		*pb++ = 1;			// Assoc 1 1
								*pb++ = 1;
		*pb++ = kopNext;							// Next
		*pb++ = kopPutGlyph8bitObs;	*pb++ = 3;		// PutGlyph gSlash
		*pb++ = kopNext;							// Next
		*pb++ = kopInsert;							// Insert
		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 clsFraction clsDenom
			*pb++ = 9;			*pb++ = 2;
		*pb++ = kopAssoc;		*pb++ = 1;			// Assoc 1 0
								*pb++ = 0;
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero
                                                                                                                                                  
		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
		m_prgibActionStart[1] = 12;
		m_prgibActionStart[2] = 12 + 12;
	}
	else if (m_ipass == 3)
	{
		m_nMaxRuleContext = m_nMaxChunk = 1;
		m_nMaxRuleLoop = 3;
		m_staBehavior = "Assoc";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpAssocTest(m_ipass);

		m_crul = 1;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1 + 10];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		*pb++ = kopCntxtItem;	*pb++ = 0;			// ContextItem 0
		*pb++ = kopPushSlotAttr;*pb++ = kslatBreak;	// PushSlotAttr break 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = 2;			// Push 2
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 1;

		m_prgbActionBlock = new byte[7];
		pb = m_prgbActionBlock;

		//	Rule 0: _  >  gHyphen / _ # {break == 2} ^;

		*pb++ = kopInsert;							// Insert
		*pb++ = kopPutGlyph8bitObs;	*pb++ = 4;		// PutGlyph 4 ('-' = 45)
		*pb++ = kopNext;							// Next
		*pb++ = kopPushByte;	*pb++ = 1;			// Push 1
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
	}
	else
		Assert(false);
}


void GrFSM::SetUpAssocTest(int ipass)
{
	if (ipass == 1)
	{
		//	Create machine class ranges.
		m_cmcr = 2;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 35;	// #
		m_prgmcr[0].m_chwLast	= 35;
		m_prgmcr[0].m_col		= 1;

		m_prgmcr[1].m_chwFirst	= 120;	// x
		m_prgmcr[1].m_chwLast	= 120;
		m_prgmcr[1].m_col		= 0;

		m_dimcrInit = 2;		// (max power of 2 <= m_cmcr);
		m_cLoop = 1;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 4;
		m_crowNonAcpt = 2;
		m_crowFinal = 2;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 2;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 4
		short * psn = m_prgrowTransitions;
		*psn++ = 2;	*psn++ = 1;
		*psn++ = 3;	*psn++ = 0;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[2];	// 2 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[m_crow - m_crowNonAcpt + 1];

		m_prgirulnMin[0]	= 0;	// s2: r1
		m_prgrulnMatched[0] = 1;

		m_prgirulnMin[1]	= 1;	// s3: r0
		m_prgrulnMatched[1]	= 0;

		m_prgirulnMin[2]	= 2;
	}
	else if (ipass == 2)
	{
		//	Create machine class ranges.
		m_cmcr = 13;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 35;	// #
		m_prgmcr[0].m_chwLast	= 35;
		m_prgmcr[0].m_col		= 7;

		m_prgmcr[1].m_chwFirst	= 65;	// A
		m_prgmcr[1].m_chwLast	= 65;
		m_prgmcr[1].m_col		= 0;

		m_prgmcr[2].m_chwFirst	= 69;	// E
		m_prgmcr[2].m_chwLast	= 69;
		m_prgmcr[2].m_col		= 2;

		m_prgmcr[3].m_chwFirst	= 79;	// O
		m_prgmcr[3].m_chwLast	= 79;
		m_prgmcr[3].m_col		= 0;

		m_prgmcr[4].m_chwFirst	= 97;	// a
		m_prgmcr[4].m_chwLast	= 97;
		m_prgmcr[4].m_col		= 1;

		m_prgmcr[5].m_chwFirst	= 98;	// b - d
		m_prgmcr[5].m_chwLast	= 100;
		m_prgmcr[5].m_col		= 4;

		m_prgmcr[6].m_chwFirst	= 101;	// e
		m_prgmcr[6].m_chwLast	= 101;
		m_prgmcr[6].m_col		= 3;

		m_prgmcr[7].m_chwFirst	= 102;	// f - n
		m_prgmcr[7].m_chwLast	= 110;
		m_prgmcr[7].m_col		= 4;

		m_prgmcr[8].m_chwFirst	= 111;	// o
		m_prgmcr[8].m_chwLast	= 111;
		m_prgmcr[8].m_col		= 1;

		m_prgmcr[9].m_chwFirst	= 112;	// p - w
		m_prgmcr[9].m_chwLast	= 119;
		m_prgmcr[9].m_col		= 4;

		m_prgmcr[10].m_chwFirst	= 120;	// x
		m_prgmcr[10].m_chwLast	= 120;
		m_prgmcr[10].m_col		= 5;

		m_prgmcr[11].m_chwFirst	= 121;	// y - z
		m_prgmcr[11].m_chwLast	= 122;
		m_prgmcr[11].m_col		= 4;

		m_prgmcr[12].m_chwFirst	= 188;	// 1/4, 1/2, 3/4
		m_prgmcr[12].m_chwLast	= 190;
		m_prgmcr[12].m_col		= 6;


		m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
		m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 9;
		m_crowNonAcpt = 6;
		m_crowFinal = 3;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 8;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 48
		short * psn = m_prgrowTransitions;
		*psn++ = 1; *psn++ = 2; *psn++ = 0;	*psn++ = 4; *psn++ = 3; *psn++ = 3;	*psn++ = 6;	*psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 7;	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 7; *psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 5;
		*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 5;
		*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 5;
		*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0; *psn++ = 8;	*psn++ = 0;	*psn++ = 0;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[3];	// 3 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[3+1];	// 3 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s6: r2
		m_prgrulnMatched[0] = 2;

		m_prgirulnMin[1]	= 1;	// s7: r0
		m_prgrulnMatched[1] = 0;

		m_prgirulnMin[2]	= 2;	// s8: r1
		m_prgrulnMatched[2] = 1;

		m_prgirulnMin[3]	= 3;
	}
	else if (ipass == 3)
	{
		//	Create machine class ranges.
		m_cmcr = 1;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 35;	// #
		m_prgmcr[0].m_chwLast	= 35;
		m_prgmcr[0].m_col		= 0;

		m_dimcrInit = 1;		// (max power of 2 <= m_cmcr);
		m_cLoop = 0;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 2;
		m_crowNonAcpt = 1;
		m_crowFinal = 1;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 1;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 1
		short * psn = m_prgrowTransitions;
		*psn++ = 1;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[1];	// 1 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[m_crow - m_crowNonAcpt + 1];

		m_prgirulnMin[0]	= 0;	// s1: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;
	}
}


/*----------------------------------------------------------------------------------------------
	Second batch of association tests.
	
	PASS 1:
		r0. clsSymbol _  >  _ @1 /  _ gNumber gHyphen # gNumber _;
		r1. clsSymbol _  >  _ @1 /  _ gHyphen # _;
		r2. _ clsSymbol  >  @6 _ /  _ gNumber gHyphen # gNumber _;
		r3. _ clsSymbol  >  @4 _ /  _ gHyphen # _;
		r4. clsLeft _  >  @1  clsRight$1:1 / _ clsLetter gHyphen # _ gSpace;
		r5. clsLeft _  >  @1  clsRight$1:1 / _ clsLetter gHyphen # clsLetter _ gSpace;
		r6. clsLeft _  >  @1  clsRight$1:1 / _ clsLetter _ gSpace;
		r7. clsLeft _  >  @1  clsRight$1:1 / _ clsLetter clsLetter _ gSpace;

	Pass 1 machine classes:
		mcSym:		@, $, %, &		64, 36, 37, 38
		mcNum:		48-57
		mcHyphen:	45
		mcSpace:	32
		mcLet:		97-122
		mcLeft:		( [ {	40, 91, 123
		mcRight:	) ] }	41, 93, 125

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	mcSym	mcNum	mcHyph	mcSpace	mcLet	mcLeft	mcLB
	----------------------------------------------------------------------------------
	T NA		s0	|	s1		s2		s3						s4
					|
	T NA		s1	|			s5		s6
					|
	T NA		s2	|					s7
					|
	T NA		s3	|													s8
					|
	T NA		s4	|									s9
					|
	T NA		s5	|					s10
					|
	T NA		s6	|													s18
					|
	T NA		s7	|													s11
					|
	T NA		s8	|	s19
					|
	T NA		s9	|					s12		s20		s13
					|
	T NA		s10	|													s14
					|
	T NA		s11	|			s15
					|
	T NA		s12	|													s16
					|
	T NA		s13	|							s21
					|
	T NA		s14	|			s22
					|
	T NA		s15	|	s23
					|
	T NA		s16	|							s24		s17
					|
	T NA		s17	|							s25
					|
	F A			s18	|
					|
	F A			s19	|
					|
	F A			s20	|
					|
	F A			s21	|
					|
	F A			s22	|
					|
	F A			s23	|
					|
	F A			s24	|
					|
	F A			s25	|


	Rules Matched:
		s18:	r1
		s19:	r3
		s20:	r6
		s21:	r7
		s22:	r0
		s23:	r2
		s24:	r4
		s25:	r5
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpAssoc2Test()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpAssoc2Test();

	m_fLineBreak = true;
	m_cchwPreXlbContext = 3;
	m_cchwPostXlbContext = 2;
}

void GrClassTable::SetUpAssoc2Test()
{
	m_ccls = 2;			// number of classes
	m_cclsLinear = 1;	// number of classes in linear format (output classes)

	m_prgchwBIGGlyphList = new data16[13];

	m_prgichwOffsets = new data16[m_ccls+1];

	data16 * pchw = m_prgchwBIGGlyphList;

	//	Output class 0: clsRight
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(41));	// )
	*pchw++ = msbf(data16(93));	// ]
	*pchw++ = msbf(data16(125));	// }

	//	Input class 1: clsLeft
	m_prgichwOffsets[1] = 3;
	*pchw++ = msbf(data16(3));
	*pchw++ = msbf(data16(2));	*pchw++ = msbf(data16(1)); *pchw++ = msbf(data16(3-2));
	*pchw++ = msbf(data16(40));	*pchw++ = msbf(data16(0));	// (
	*pchw++ = msbf(data16(91));	*pchw++ = msbf(data16(1));	// [
	*pchw++ = msbf(data16(123));	*pchw++ = msbf(data16(2));	// {

	m_prgichwOffsets[2] = 3 + 4 + 3*2;	// 13
}

void GrSubPass::SetUpAssoc2Test()
{
	m_nMaxRuleContext = m_nMaxChunk = 6;
	m_nMaxRuleLoop = 5;
	m_staBehavior = "Assoc2";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpAssoc2Test(m_ipass);

	m_crul = 8;

	//	Set up constraint code.
	m_prgbConstraintBlock = new byte[1];
	byte * pb = m_prgbConstraintBlock;
	*pb++ = kopRetTrue;							// RetTrue

	m_prgibConstraintStart = new data16[m_crul];
	m_prgibConstraintStart[0] = 0;
	m_prgibConstraintStart[1] = 0;
	m_prgibConstraintStart[2] = 0;
	m_prgibConstraintStart[3] = 0;
	m_prgibConstraintStart[4] = 0;
	m_prgibConstraintStart[5] = 0;
	m_prgibConstraintStart[6] = 0;
	m_prgibConstraintStart[7] = 0;

	//	Set up rule action code.
	m_prgbActionBlock = new byte[11 + 9 + 11 + 9 + 14 + 15 + 12 + 13];
	pb = m_prgbActionBlock;

	//	Rule 0: clsSymbol _  >  _ @1 /  _ gNumber gHyphen # gNumber _;

	*pb++ = kopDelete;							// Delete
	*pb++ = kopNext;							// Next
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutCopy;		*pb++ = -4;			// PutCopy -4
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 1: clsSymbol _  >  _ @1 /  _ gHyphen # _;

	*pb++ = kopDelete;							// Delete
	*pb++ = kopNext;							// Next
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutCopy;		*pb++ = -2;			// PutCopy -2
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 2: _ clsSymbol  >  @6 _ /  _ gNumber gHyphen # gNumber _;

	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutCopy;		*pb++ = 5;			// PutCopy 5
	*pb++ = kopNext;							// Next
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopDelete;							// Delete
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero
                                                                                                                                              
	//	Rule 3: _ clsSymbol  >  @4 _ /  _ gHyphen # _;

	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutCopy;		*pb++ = 3;			// PutCopy 3
	*pb++ = kopNext;							// Next
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopDelete;							// Delete
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 4: clsLeft _  >  @1  clsRight$1 / _ clsLetter gHyphen # _ gSpace;

	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutSubs8bitObs;	*pb++ = -3;		// PutSubs -3 1 0
		*pb++ = 1;			*pb++ = 0;
	*pb++ = kopAssoc;		*pb++ = 1;			// Assoc 1 -3
							*pb++ = -3;
	*pb++ = kopNext;							// Next;
	*pb++ = kopRetZero;							// RetZero

	//	Rule 5: clsLeft _  >  @1  clsRight$1 / _ clsLetter gHyphen # clsLetter _ gSpace;

	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutSubs8bitObs;	*pb++ = -4;		// PutSubs -4 1 0
		*pb++ = 1;			*pb++ = 0;
	*pb++ = kopAssoc;		*pb++ = 1;			// Assoc 1 -4
							*pb++ = -4;
	*pb++ = kopNext;							// Next;
	*pb++ = kopRetZero;							// RetZero

	//	Rule 6: clsLeft _  >  @1  clsRight$1 / _ clsLetter _ gSpace;

	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutSubs8bitObs; *pb++ = -1;		// PutSubs -1 1 0
		*pb++ = 1;			*pb++ = 0;
	*pb++ = kopAssoc;		*pb++ = 1;			// Assoc 1 -1
							*pb++ = -1;
	*pb++ = kopNext;							// Next;
	*pb++ = kopRetZero;							// RetZero

	//	Rule 7: clsLeft _  >  @1  clsRight$1 / _ clsLetter clsLetter _ gSpace;

	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutSubs8bitObs; *pb++ = -2;		// PutSubs -2 1 0
		*pb++ = 1;			*pb++ = 0;
	*pb++ = kopAssoc;		*pb++ = 1;			// Assoc 1 -2
							*pb++ = -2;
	*pb++ = kopNext;							// Next;
	*pb++ = kopRetZero;							// RetZero


	m_prgibActionStart = new data16[m_crul];
	m_prgibActionStart[0] = 0;
	m_prgibActionStart[1] = 11;
	m_prgibActionStart[2] = 11 + 9;
	m_prgibActionStart[3] = 20 + 11;
	m_prgibActionStart[4] = 31 + 9;
	m_prgibActionStart[5] = 40 + 14;
	m_prgibActionStart[6] = 54 + 15;
	m_prgibActionStart[7] = 69 + 12;
}


void GrFSM::SetUpAssoc2Test(int ipass)
{
	//	Create machine class ranges.
	m_cmcr = 10;
	m_prgmcr = new GrFSMClassRange[m_cmcr];
	m_prgmcr[0].m_chwFirst	= 32;	// space
	m_prgmcr[0].m_chwLast	= 32;
	m_prgmcr[0].m_col		= 3;

	m_prgmcr[1].m_chwFirst	= 35;	// #
	m_prgmcr[1].m_chwLast	= 35;
	m_prgmcr[1].m_col		= 6;

	m_prgmcr[2].m_chwFirst	= 36;	// $, %, &
	m_prgmcr[2].m_chwLast	= 38;
	m_prgmcr[2].m_col		= 0;

	m_prgmcr[3].m_chwFirst	= 40;	// (
	m_prgmcr[3].m_chwLast	= 40;
	m_prgmcr[3].m_col		= 5;

	m_prgmcr[4].m_chwFirst	= 45;	// -
	m_prgmcr[4].m_chwLast	= 45;
	m_prgmcr[4].m_col		= 2;

	m_prgmcr[5].m_chwFirst	= 48;	// 0 - 9
	m_prgmcr[5].m_chwLast	= 57;
	m_prgmcr[5].m_col		= 1;

	m_prgmcr[6].m_chwFirst	= 64;	// @
	m_prgmcr[6].m_chwLast	= 64;
	m_prgmcr[6].m_col		= 0;

	m_prgmcr[7].m_chwFirst	= 91;	// [
	m_prgmcr[7].m_chwLast	= 91;
	m_prgmcr[7].m_col		= 5;

	m_prgmcr[8].m_chwFirst	= 97;	// a - z
	m_prgmcr[8].m_chwLast	= 122;
	m_prgmcr[8].m_col		= 4;

	m_prgmcr[9].m_chwFirst	= 123;	// {
	m_prgmcr[9].m_chwLast	= 123;
	m_prgmcr[9].m_col		= 5;


	m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
	m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
	m_imcrStart = m_cmcr - m_dimcrInit;


	m_crow = 26;
	m_crowNonAcpt = 18;
	m_crowFinal = 8;
	m_rowFinalMin = m_crow - m_crowFinal;
	m_ccol = 7;

	//	Set up transition table.
	m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 126
	short * psn = m_prgrowTransitions;
	*psn++ = 1; *psn++ = 2; *psn++ = 3;	*psn++ = 0; *psn++ = 0;	*psn++ = 4;	*psn++ = 0;
	*psn++ = 0; *psn++ = 5; *psn++ = 6;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 7;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 8;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 9;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ =10;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ =18;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ =11;
	*psn++ =19;	*psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ =12;	*psn++ =20; *psn++ =13;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ =14;
	*psn++ = 0; *psn++ =15; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ =16;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ =21; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ =22; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ =23; *psn++ = 0; *psn++ = 0;	*psn++ = 0; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ =24; *psn++ =17;	*psn++ = 0;	*psn++ = 0;
	*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ =25; *psn++ = 0;	*psn++ = 0;	*psn++ = 0;

	//	Set up matched-rules tables.
	m_prgrulnMatched = new data16[8];	// 3 = sum of rules matched for each accepting state
	m_prgirulnMin = new data16[m_crow - m_crowNonAcpt + 1];

	m_prgirulnMin[0]	= 0;	// s18: r1
	m_prgrulnMatched[0] = 1;

	m_prgirulnMin[1]	= 1;	// s19: r3
	m_prgrulnMatched[1] = 3;

	m_prgirulnMin[2]	= 2;	// s20: r6
	m_prgrulnMatched[2] = 6;

	m_prgirulnMin[3]	= 3;	// s21: r7
	m_prgrulnMatched[3] = 7;

	m_prgirulnMin[4]	= 4;	// s22: r0
	m_prgrulnMatched[4] = 0;

	m_prgirulnMin[5]	= 5;	// s23: r2
	m_prgrulnMatched[5] = 2;

	m_prgirulnMin[6]	= 6;	// s24: r4
	m_prgrulnMatched[6] = 4;

	m_prgirulnMin[7]	= 7;	// s25: r5
	m_prgrulnMatched[7] = 5;

	m_prgirulnMin[8]	= 8;
}

/*----------------------------------------------------------------------------------------------
	Test of default associations
	
	PASS 1:
		r0. gDollar _  >  @1 gEquals;
		r1. gAtSign  >  _ ;

	Pass 1 machine classes:
		mcDollar:	$	36
		mcAt:		@	64

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	mcDollar	mcAt
	--------------------------------------
	T NA		s0	|	s1			s2
					|
	F A			s1	|
					|
	F A			s2	|


	Rules Matched:
		s1:		r0
		s2:		r1
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpDefaultAssocTest()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpDefaultAssocTest();
}

void GrClassTable::SetUpDefaultAssocTest()
{
	m_ccls = 1;			// number of classes
	m_cclsLinear = 1;	// number of classes in linear format

	m_prgchwBIGGlyphList = new data16[1];

	m_prgichwOffsets = new data16[m_ccls+1];

	data16 * pchw = m_prgchwBIGGlyphList;

	//	Output class 0: gEquals
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(61));

	m_prgichwOffsets[1] = 1;
}

void GrSubPass::SetUpDefaultAssocTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 5;
	m_staBehavior = "DefaultAssoc";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpDefaultAssocTest();

	m_crul = 2;

	//	Set up constraint code.
	m_prgbConstraintBlock = new byte[1];
	byte * pb = m_prgbConstraintBlock;
	*pb++ = kopRetTrue;

	m_prgibConstraintStart = new data16[m_crul];
	m_prgibConstraintStart[0] = 0;
	m_prgibConstraintStart[1] = 0;

	//	Set up rule action code.
	m_prgbActionBlock = new byte[6 + 3];
	pb = m_prgbActionBlock;

	//	Rule 0: gDollar _  >  @1 gEquals;

	*pb++ = kopCopyNext;						// CopyNext
	*pb++ = kopInsert;							// Insert
	*pb++ = kopPutGlyph8bitObs;	*pb++ = 0;		// PutGlyph 0
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 1: gAtSign  >  _ ;

	*pb++ = kopDelete;							// Delete
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	m_prgibActionStart = new data16[m_crul];
	m_prgibActionStart[0] = 0;
	m_prgibActionStart[1] = 6;
}


void GrFSM::SetUpDefaultAssocTest()
{
	//	Create machine class ranges.
	m_cmcr = 2;
	m_prgmcr = new GrFSMClassRange[m_cmcr];
	m_prgmcr[0].m_chwFirst	= 36;	// $
	m_prgmcr[0].m_chwLast	= 36;
	m_prgmcr[0].m_col		= 0;

	m_prgmcr[1].m_chwFirst	= 64;	// @
	m_prgmcr[1].m_chwLast	= 64;
	m_prgmcr[1].m_col		= 1;


	m_dimcrInit = 2;		// (max power of 2 <= m_cmcr);
	m_cLoop = 1;			// log2(max power of 2 <= m_cmcr);
	m_imcrStart = m_cmcr - m_dimcrInit;


	m_crow = 3;
	m_crowNonAcpt = 1;
	m_crowFinal = 2;
	m_rowFinalMin = m_crow - m_crowFinal;
	m_ccol = 2;

	//	Set up transition table.
	m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 2
	short * psn = m_prgrowTransitions;
	*psn++ = 1; *psn++ = 2;

	//	Set up matched-rules tables.
	m_prgrulnMatched = new data16[2];	// 2 = sum of rules matched for each accepting state
	m_prgirulnMin = new data16[m_crow - m_crowNonAcpt + 1];

	m_prgirulnMin[0]	= 0;	// s1: r0
	m_prgrulnMatched[0] = 0;

	m_prgirulnMin[1]	= 1;	// s2: r1
	m_prgrulnMatched[1] = 1;

	m_prgirulnMin[2]	= 2;
}

/*----------------------------------------------------------------------------------------------
	Test of features. The ID of "theFeature" == 234, its index is 0. The default value is 1.
	
	PASS 1:
	if (theFeature == 0)			// * -> $
		r0. gAsterisk > gDollar ;
	else if (theFeature == 1)		// * -> %
		r1. gAsterisk > gPercent ;
	else							// * -> @
		r2.	gAsterisk > gAt ;

	Pass 1 machine classes:
		mcAsterisk:	*	42

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	mcAsterisk
	--------------------------------
	T NA		s0	|	s1
					|
	F A			s1	|


	Rules Matched:
		s1:		r0, r1, r2
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpFeatureTest()
{
	AddFeature(234, 0, 1, 1);

	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpFeatureTest();
}

void GrClassTable::SetUpFeatureTest()
{
	m_ccls = 4;			// number of classes
	m_cclsLinear = 4;	// number of classes in linear format

	m_prgchwBIGGlyphList = new data16[4];

	m_prgichwOffsets = new data16[m_ccls + 1];

	data16 * pchw = m_prgchwBIGGlyphList;

	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(36));	// $

	m_prgichwOffsets[1] = 1;
	*pchw++ = msbf(data16(37));	// %

	m_prgichwOffsets[2] = 2;
	*pchw++ = msbf(data16(64));	// @

	m_prgichwOffsets[3] = 3;
	*pchw++ = msbf(data16(42));	// *

	m_prgichwOffsets[4] = 4;
}

void GrSubPass::SetUpFeatureTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 5;
	m_staBehavior = "Feature";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpFeatureTest();

	m_crul = 3;

	//	Set up constraint code.
	m_prgbConstraintBlock = new byte[1 + 6 + 12 + 12];
	byte * pb = m_prgbConstraintBlock;
	*pb++ = kopRetTrue;

	//	(theFeature == 0)
	*pb++ = kopPushFeat;	*pb++ = 0;				// PushFeat 0
	*pb++ = kopPushByte;	*pb++ = 0;				// PushByte 0
	*pb++ = kopEqual;								// Equal
	*pb++ = kopPopRet;								// PopAndRet

	//	(theFeature != 0 && theFeature == 1)
	*pb++ = kopPushFeat;	*pb++ = 0;				// PushFeat 0
	*pb++ = kopPushByte;	*pb++ = 0;				// PushByte 0
	*pb++ = kopNotEq;								// NotEq
	*pb++ = kopPushFeat;	*pb++ = 0;				// PushFeat 0
	*pb++ = kopPushByte;	*pb++ = 1;				// PushByte 1
	*pb++ = kopEqual;								// Equal
	*pb++ = kopAnd;									// And
	*pb++ = kopPopRet;								// PopAndRet

	//	(theFeature != 0 && theFeature != 1)
	*pb++ = kopPushFeat;	*pb++ = 0;				// PushFeat 0
	*pb++ = kopPushByte;	*pb++ = 0;				// PushByte 0
	*pb++ = kopNotEq;								// NotEq
	*pb++ = kopPushFeat;	*pb++ = 0;				// PushFeat 0
	*pb++ = kopPushByte;	*pb++ = 1;				// PushByte 1
	*pb++ = kopNotEq;								// NotEq
	*pb++ = kopAnd;									// And
	*pb++ = kopPopRet;								// PopAndRet

	m_prgibConstraintStart = new data16[m_crul];
	m_prgibConstraintStart[0] = 1;
	m_prgibConstraintStart[1] = 1 + 6;
	m_prgibConstraintStart[2] = 1 + 6 + 12;

	//	Set up rule action code.
	m_prgbActionBlock = new byte[4 + 4 + 4];
	pb = m_prgbActionBlock;

	//	Rule 0: gAsterisk > gDollar ;

	*pb++ = kopPutGlyph8bitObs;	*pb++ = 0;		// PutGlyph 0
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 1: gAsterisk > gPercent ;

	*pb++ = kopPutGlyph8bitObs;	*pb++ = 1;		// PutGlyph 1
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	//	Rule 2: gAsterisk > gAt ;

	*pb++ = kopPutGlyph8bitObs;	*pb++ = 2;		// PutGlyph 2
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	m_prgibActionStart = new data16[m_crul];
	m_prgibActionStart[0] = 0;
	m_prgibActionStart[1] = 4;
	m_prgibActionStart[2] = 4 + 4;
}


void GrFSM::SetUpFeatureTest()
{
	//	Create machine class ranges.
	m_cmcr = 1;
	m_prgmcr = new GrFSMClassRange[m_cmcr];
	m_prgmcr[0].m_chwFirst	= 42;	// *
	m_prgmcr[0].m_chwLast	= 42;
	m_prgmcr[0].m_col		= 0;


	m_dimcrInit = 1;		// (max power of 2 <= m_cmcr);
	m_cLoop = 0;			// log2(max power of 2 <= m_cmcr);
	m_imcrStart = m_cmcr - m_dimcrInit;


	m_crow = 2;
	m_crowNonAcpt = 1;
	m_crowFinal = 1;
	m_rowFinalMin = m_crow - m_crowFinal;
	m_ccol = 1;

	//	Set up transition table.
	m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 1
	short * psn = m_prgrowTransitions;
	*psn++ = 1;

	//	Set up matched-rules tables.
	m_prgrulnMatched = new data16[3];	// 3 = sum of rules matched for each accepting state
	m_prgirulnMin = new data16[m_crow - m_crowNonAcpt + 1];

	m_prgirulnMin[0]	= 0;	// s1: r0, r1, r2
	m_prgrulnMatched[0] = 0;
	m_prgrulnMatched[1] = 1;
	m_prgrulnMatched[2] = 2;

	m_prgirulnMin[1]	= 3;
}


/*----------------------------------------------------------------------------------------------
	Tests of ligatures and components.
	
	PASS 1 (line-break):
		r0. clsLetter { break = 2 }

	Pass 1 machine classes:
		mcLetter:	a - z   97 - 122

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	clsLetter
	--------------------------------
	T NA		s0	|	s1
					|
	F A			s1	|
	

	Rules Matched:
		s1:		r0


	PASS 2 (substitution):
		r0.	gX  gY  gZ  >  gCapX:(1 2 4) { comp {X.ref=@1; Y.ref=@2; Z.ref=@4 }} _ _
		                     / _  _  # {break==2}  _;
		r1. gX  gY  gZ  >  _ _ gCapZ:(1 3 4) { comp {X.ref=@1; Y.ref=@3; Z.ref=@4 }}
		                     / _  # {break==2}  _  _;
		r2. gX  gY  gZ  >  _ gCapY:(1 2 3) { comp {X.ref=@1; Y.ref=@2; Z.ref=@3 }}  gPlus;

	Pass 2 machine classes:
		mcX:			x			120
		mcY:			y			121
		mcZ:			z			122
		mcLB:			35


	Pass 2 FSM:	
					|	mcX		mcY		mcZ		mcLB
	----------------------------------------------------
	T NA		s0	|	s1
					|		-		-		-
	T NA		s1	|			s2				s3
					|		-		-		-
	T NA		s2	|					s6		s4
					|		-		-		-
	T NA		s3	|			s5
					|		-		-		-
	T NA		s4	|					s7												
					|		-		-		-
	T NA		s5	|					s8						
					|		-		-		-
	F A			s6	|
					|		-		-		-
	F A			s7	|
					|		-		-		-
	F A			s8	|
					|		-		-		-

	Rules matched:
		s6:		r2
		s7:		r0
		s8:		r1
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpLigatureTest()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpLigatureTest();

	m_pgtbl = new GrGlyphTable();
	m_pgtbl->SetUpLigatureTest();

	m_fLineBreak = true;
	m_cchwPreXlbContext = 2;
	m_cchwPostXlbContext = 2;
}

void GrClassTable::SetUpLigatureTest()
{
	m_ccls = 7;			// number of classes
	m_cclsLinear = 7;	// number of classes in linear format (all classes are single-item)

	m_prgchwBIGGlyphList = new data16[7];

	m_prgichwOffsets = new data16[m_ccls+1];

	data16 * pchw = m_prgchwBIGGlyphList;

	//	Class 0: gCapX
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(88));	// X

	//	Class 1: gCapY
	m_prgichwOffsets[1] = 1;
	*pchw++ = msbf(data16(89));	// Y

	//	Class 2: gCapZ
	m_prgichwOffsets[2] = 2;
	*pchw++ = msbf(data16(90));	// Z

	//	Class 3: gPlus
	m_prgichwOffsets[3] = 3;
	*pchw++ = msbf(data16(43));	// +

	//	Class 4: gX
	m_prgichwOffsets[4] = 4;
	*pchw++ = msbf(data16(120));	// x

	//	Class 5: gY
	m_prgichwOffsets[5] = 5;
	*pchw++ = msbf(data16(121));	// y

	//	Class 6: gZ
	m_prgichwOffsets[6] = 6;
	*pchw++ = msbf(data16(122));	// z

	m_prgichwOffsets[7] = 7;
}

void GrGlyphTable::SetUpLigatureTest()
{
	SetNumberOfGlyphs(128);
	SetNumberOfStyles(1);

	GrGlyphSubTable * pgstbl = new GrGlyphSubTable();
	Assert(pgstbl);

	pgstbl->Initialize(1, 0, 128, 10, 4);
	SetSubTable(0, pgstbl);

	SetNumberOfComponents(4);	// comp.w, comp.x, comp.y, comp.z

	pgstbl->SetUpLigatureTest();
}

/***********************************************************************************************
	TODO: This method is BROKEN because m_prgibBIGAttrValues has been changed. It is no
	longer a data16 *. The Gloc table can contain 16-bit or 32-bit entries and must be
	accessed accordingly.
***********************************************************************************************/
void GrGlyphSubTable::SetUpLigatureTest()
{
	m_nAttrIDLim = 20;

	m_pgatbl = new GrGlyphAttrTable();
	m_pgatbl->Initialize(0, 102);

	for (int i = 0; i < 88; i++)
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(0));

	m_prgibBIGAttrValues[88] = (byte)msbf(data16(0));
	m_prgibBIGAttrValues[89] = (byte)msbf(data16(34));
	m_prgibBIGAttrValues[90] = (byte)msbf(data16(68));

	for (i = 91; i < 128; i++)
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(102));

	m_pgatbl->SetUpLigatureTest();
}

void GrGlyphAttrTable::SetUpLigatureTest()
{
	//	Glyph 88 'X' (2 runs; offset = 0):
	//		0 = 0		comp.w undefined
	//		1 = 8		comp.x: (0,0,	25,100)
	//		2 = 12		comp.y:	(25,0,	75,100)
	//		3 = 16		comp.z:	(75,0,	100,100)
	//	  4-7 = 0		comp.w box corners
	//		8 =	100		comp.x.	top
	//		9 =	0				bottom
	//		10=	0				left
	//		11=	25				right
	//		12=	100		comp.y.	top
	//		13=	0				bottom
	//		14=	25				left
	//		15=	75				right
	//		16=	100		comp.z.	top
	//		17=	0				bottom
	//		18=	75				left
	//		19=	100				right
	//
	//	Glyph 89 (2 runs; offset = 34):
	//		0 = 0		comp.w undefined
	//		1 = 8		comp.x: (0,0,	33,100)
	//		2 = 12		comp.y:	(33,0,	67,100)
	//		3 = 16		comp.z:	(67,0,	100,100)
	//	  4-7 = 0		comp.w box corners
	//		8 =	100		comp.x.	top
	//		9 =	0				bottom
	//		10=	0				left
	//		11=	33				right
	//		12=	100		comp.y.	top
	//		13=	0				bottom
	//		14=	33				left
	//		15=	67				right
	//		16=	100		comp.z.	top
	//		17=	0				bottom
	//		18=	67				left
	//		19=	100				right
	//
	//	Glyph 90 (2 runs; offset = 68):
	//		0 = 0		comp.w undefined
	//		1 = 8		comp.x: (0,0,	10,100)
	//		2 = 12		comp.y:	(10,0,	90,100)
	//		3 = 16		comp.z:	(90,0,	100,100)
	//	  4-7 = 0		comp.w box corners
	//		8 =	100		comp.x.	top
	//		9 =	0				bottom
	//		10=	0				left
	//		11=	10				right
	//		12=	100		comp.y.	top
	//		13=	0				bottom
	//		14=	10				left
	//		15=	90				right
	//		16=	100		comp.z.	top
	//		17=	0				bottom
	//		18=	90				left
	//		19=	100				right

	byte * pbBIG = m_prgbBIGEntries;

	GrGlyphAttrRun gatrun;

	//	Glyph 88 'X'
	gatrun.m_bMinAttrID = 1;
	gatrun.m_cAttrs = 3;
	gatrun.m_rgchwBIGValues[0] = msbf(data16(8));
	gatrun.m_rgchwBIGValues[1] = msbf(data16(12));
	gatrun.m_rgchwBIGValues[2] = msbf(data16(16));
	memcpy(pbBIG, &gatrun, 8);	// 8 = 3*2 + 2
	pbBIG += 8;
	gatrun.m_bMinAttrID = 8;	// comp.x.top
	gatrun.m_cAttrs = 12;
	gatrun.m_rgchwBIGValues[0] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[1] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[2] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[3] = msbf(data16(25));
	gatrun.m_rgchwBIGValues[4] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[5] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[6] = msbf(data16(25));
	gatrun.m_rgchwBIGValues[7] = msbf(data16(75));
	gatrun.m_rgchwBIGValues[8] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[9] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[10] = msbf(data16(75));
	gatrun.m_rgchwBIGValues[11] = msbf(data16(100));
	memcpy(pbBIG, &gatrun, 26);		// 26 = 12*2 + 2
	pbBIG += 26;

	//	Glyph 89 'Y'
	gatrun.m_bMinAttrID = 1;
	gatrun.m_cAttrs = 3;
	gatrun.m_rgchwBIGValues[0] = msbf(data16(8));
	gatrun.m_rgchwBIGValues[1] = msbf(data16(12));
	gatrun.m_rgchwBIGValues[2] = msbf(data16(16));
	memcpy(pbBIG, &gatrun, 8);	// 8 = 3*2 + 2
	pbBIG += 8;
	gatrun.m_bMinAttrID = 8;	// comp.x.top
	gatrun.m_cAttrs = 12;
	gatrun.m_rgchwBIGValues[0] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[1] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[2] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[3] = msbf(data16(33));
	gatrun.m_rgchwBIGValues[4] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[5] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[6] = msbf(data16(33));
	gatrun.m_rgchwBIGValues[7] = msbf(data16(67));
	gatrun.m_rgchwBIGValues[8] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[9] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[10] = msbf(data16(67));
	gatrun.m_rgchwBIGValues[11] = msbf(data16(100));
	memcpy(pbBIG, &gatrun, 26);		// 26 = 12*2 + 2
	pbBIG += 26;

	//	Glyph 90 'Z'
	gatrun.m_bMinAttrID = 1;
	gatrun.m_cAttrs = 3;
	gatrun.m_rgchwBIGValues[0] = msbf(data16(8));
	gatrun.m_rgchwBIGValues[1] = msbf(data16(12));
	gatrun.m_rgchwBIGValues[2] = msbf(data16(16));
	memcpy(pbBIG, &gatrun, 8);	// 8 = 3*2 + 2
	pbBIG += 8;
	gatrun.m_bMinAttrID = 8;	// comp.x.top
	gatrun.m_cAttrs = 12;
	gatrun.m_rgchwBIGValues[0] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[1] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[2] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[3] = msbf(data16(10));
	gatrun.m_rgchwBIGValues[4] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[5] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[6] = msbf(data16(10));
	gatrun.m_rgchwBIGValues[7] = msbf(data16(90));
	gatrun.m_rgchwBIGValues[8] = msbf(data16(100));
	gatrun.m_rgchwBIGValues[9] = msbf(data16(0));
	gatrun.m_rgchwBIGValues[10] = msbf(data16(90));
	gatrun.m_rgchwBIGValues[11] = msbf(data16(100));
	memcpy(pbBIG, &gatrun, 26);		// 26 = 12*2 + 2
	pbBIG += 26;

	Assert(pbBIG == m_prgbBIGEntries + 102);
}

void GrLineBreakPass::SetUpLigatureTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 5;
	m_staBehavior = "Ligatures";

	m_pfsm = new GrFSM();
	Assert(m_pfsm);
	m_pfsm->SetUpLigatureTest(m_ipass);

	m_crul = 1;

	//	Set up constraint code--succeed trivially.
	m_prgbConstraintBlock = new byte[1];
	byte * pb = m_prgbConstraintBlock;
	*pb++ = kopRetTrue;							// RetTrue

	m_prgibConstraintStart = new data16[m_crul];
	m_prgibConstraintStart[0] = 0;

	//	Set up rule action code.
	m_prgbActionBlock = new byte[8];
	pb = m_prgbActionBlock;

	//	Rule 0: clsLetter { break = 2 }

	*pb++ = kopPutCopy;		*pb++ = 0;			// PutCopy 0
	*pb++ = kopPushByte;	*pb++ = 2;			// PushByte 2
	*pb++ = kopAttrSet;		*pb++ = kslatBreak;	// AttrSet breakweight
	*pb++ = kopNext;							// Next
	*pb++ = kopRetZero;							// RetZero

	m_prgibActionStart = new data16[m_crul];
	m_prgibActionStart[0] = 0;
}

void GrSubPass::SetUpLigatureTest()
{
	if (m_ipass == 2)
	{
		m_nMaxRuleContext = m_nMaxChunk = 4;
		m_nMaxRuleLoop = 5;
		m_staBehavior = "Ligatures";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpLigatureTest(m_ipass);

		m_crul = 3;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1 + 10 + 10];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		*pb++ = kopCntxtItem;	*pb++ = 2;			// ContextItem 2
		*pb++ = kopPushSlotAttr;*pb++ = kslatBreak;	// PushSlotAttr break 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = 2;			// Push 2
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		*pb++ = kopCntxtItem;	*pb++ = 1;			// ContextItem 1
		*pb++ = kopPushSlotAttr;*pb++ = kslatBreak;	// PushSlotAttr break 0
								*pb++ = 0;
		*pb++ = kopPushByte;	*pb++ = 2;			// Push 2
		*pb++ = kopEqual;							// Equal
		*pb++ = kopOr;								// Or
		*pb++ = kopPopRet;							// PopAndRet

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 1;
		m_prgibConstraintStart[1] = 11;
		m_prgibConstraintStart[2] = 0;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[29 + 29 + 29];
		pb = m_prgbActionBlock;

		//	Rule 0: gX  gY  gZ  >  gCapX:(1 2 4) { comp {X.ref=@1; Y.ref=@2; Z.ref=@4 }} _ _
		//                     / _  _ # {break==2}  _;

		*pb++ = kopPutGlyph8bitObs;		*pb++ = 0;	// PutGlyph 0 (gCapX)
		*pb++ = kopAssoc;			*pb++ = 3;		// Assoc 3 0 1 3
			*pb++ = 0;	*pb++ = 1;	*pb++ = 3;
		*pb++ = kopPushByte;		*pb++ =	0;		// Push 0
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref x
			*pb++ = kslatCompRef;	*pb++ = 1;
		*pb++ = kopPushByte;		*pb++ =	1;		// Push 1
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref y
			*pb++ = kslatCompRef;	*pb++ = 2;
		*pb++ = kopPushByte;		*pb++ =	3;		// Push 3
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref z
			*pb++ = kslatCompRef;	*pb++ = 3;
		*pb++ = kopNext;							// Next
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 1: gX  gY  gZ  >  _ _ gCapZ:(1 3 4) { comp {X.ref=@1; Y.ref=@3; Z.ref=@4 }}
		//                     / _  # {break==2} _ _;

		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopCopyNext;						// CopyNext
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopPutGlyph8bitObs;	*pb++ = 2;		// PutGlyph 2 (gCapZ)
		*pb++ = kopAssoc;			*pb++ = 3;		// Assoc 3 -3 -1 0
			*pb++ = -3;	*pb++ = -1;	*pb++ = 0;
		*pb++ = kopPushByte;		*pb++ =	-3;		// Push -3
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref x
			*pb++ = kslatCompRef;	*pb++ = 1;
		*pb++ = kopPushByte;		*pb++ =	-1;		// Push -1
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref y
			*pb++ = kslatCompRef;	*pb++ = 2;
		*pb++ = kopPushByte;		*pb++ =	0;		// Push 0
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref z
			*pb++ = kslatCompRef;	*pb++ = 3;
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 2: gX  gY  gZ  >  _ gCapY:(1 2 3) { comp {X.ref=@1; Y.ref=@2; Z.ref=@3 }} gPlus;

		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopPutGlyph8bitObs;	*pb++ = 1;		// PutGlyph 1 (gCapY)
		*pb++ = kopAssoc;			*pb++ = 3;		// Assoc 3 -1 0 1
			*pb++ = -1;	*pb++ = 0;	*pb++ = 1;
		*pb++ = kopPushByte;		*pb++ =	-1;		// Push -1
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref x
			*pb++ = kslatCompRef;	*pb++ = 1;
		*pb++ = kopPushByte;		*pb++ =	0;		// Push 0
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref y
			*pb++ = kslatCompRef;	*pb++ = 2;
		*pb++ = kopPushByte;		*pb++ =	1;		// Push 1
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot comp_ref z
			*pb++ = kslatCompRef;	*pb++ = 3;
		*pb++ = kopNext;							// Next
		*pb++ = kopPutGlyph8bitObs;	*pb++ = 3;		// PutGlyph 3 (gPlus)
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero
                                                                                                                                                  
		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
		m_prgibActionStart[1] = 29;
		m_prgibActionStart[2] = 29 + 29;
	}
	else
		Assert(false);
}


void GrFSM::SetUpLigatureTest(int ipass)
{
	if (ipass == 1)
	{
		//	Create machine class ranges.
		m_cmcr = 1;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 97;	// a - z
		m_prgmcr[0].m_chwLast	= 122;
		m_prgmcr[0].m_col		= 0;

		m_dimcrInit = 1;		// (max power of 2 <= m_cmcr);
		m_cLoop = 0;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 2;
		m_crowNonAcpt = 1;
		m_crowFinal = 1;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 1;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 1
		short * psn = m_prgrowTransitions;
		*psn++ = 1;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[1];	// 1 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[m_crow - m_crowNonAcpt + 1];

		m_prgirulnMin[0]	= 0;	// s1: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;
	}
	else if (ipass == 2)
	{
		//	Create machine class ranges.
		m_cmcr = 4;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 35;	// #
		m_prgmcr[0].m_chwLast	= 35;
		m_prgmcr[0].m_col		= 3;

		m_prgmcr[1].m_chwFirst	= 120;	// x
		m_prgmcr[1].m_chwLast	= 120;
		m_prgmcr[1].m_col		= 0;

		m_prgmcr[2].m_chwFirst	= 121;	// y
		m_prgmcr[2].m_chwLast	= 121;
		m_prgmcr[2].m_col		= 1;

		m_prgmcr[3].m_chwFirst	= 122;	// z
		m_prgmcr[3].m_chwLast	= 123;
		m_prgmcr[3].m_col		= 2;


		m_dimcrInit = 4;		// (max power of 2 <= m_cmcr);
		m_cLoop = 2;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 9;
		m_crowNonAcpt = 6;
		m_crowFinal = 3;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 4;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 24
		short * psn = m_prgrowTransitions;
		*psn++ = 1; *psn++ = 0; *psn++ = 0;	*psn++ = 0;
		*psn++ = 0; *psn++ = 2; *psn++ = 0;	*psn++ = 3;
		*psn++ = 0; *psn++ = 0; *psn++ = 6;	*psn++ = 4;
		*psn++ = 0; *psn++ = 5; *psn++ = 0;	*psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 7;	*psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 8;	*psn++ = 0;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[3];	// 3 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[3+1];	// 3 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s6: r2
		m_prgrulnMatched[0] = 2;

		m_prgirulnMin[1]	= 1;	// s7: r0
		m_prgrulnMatched[1] = 0;

		m_prgirulnMin[2]	= 2;	// s8: r1
		m_prgrulnMatched[2] = 1;

		m_prgirulnMin[3]	= 3;
	}
}


/*----------------------------------------------------------------------------------------------
	Tests of ligatures and components where the components are modified glyphs.
	
	PASS 1:
		r0. gX  gI   >   @2  @1;
		r1. clsPlainVowel  gAcute   >  clsVowelAcute:(1 2)  _;
		r2. clsPlainVowel  gGrave   >  clsVowelGrave:(1 2)  _;

	Pass 1 machine classes:
		mcX:		x  120
		mcI:		i  105
		mcPV:		a e o u   97 101 111 117
		mcAcute:	/  47
		mcGrave:	\  92

	Pass 1 FSM: (T=transition, F=final, A=accepting, NA=non-accepting)

					|	mcX		mcPV	mcI		mcAcute		mcGrave
	---------------------------------------------------------------
	T NA		s0	|	s1		s2		s2
					|		-		-		-			-
	T NA		s1	|					s3
					|		-		-		-			-
	T NA		s2	|							s4			s5
					|		-		-		-			-
	F A			s3	|
					|		-		-		-			-
	F A			s4	|
					|		-		-		-			-
	F A			s5	|
					|		-		-		-			-


	Rules Matched:
		s3:		r0
		s4:		r1
		s5:		r2


	PASS 2:
		r0.	clsVowel gI  >  clsCapVowel:(1 2) { comp {v.ref = @1; i.ref = @2}} _;

	Pass 2 machine classes:
		mcV:		97 101 111 117
						224 225 232 233 236 237 242 243 249 250 
		mcI:		i 105


	Pass 2 FSM:	
					|	mcV		mcI
	----------------------------------------------------
	T NA		s0	|	s1		s1
					|		-
	T NA		s1	|			s2
					|		-
	F A			s2	|
					|		-

	Rules matched:
		s2:		r0
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpLigature2Test()
{
	if (m_pctbl)
		delete m_pctbl;

	m_pctbl = new GrClassTable();
	m_pctbl->SetUpLigature2Test();

	m_pgtbl = new GrGlyphTable();
	m_pgtbl->SetUpLigature2Test();
}

void GrClassTable::SetUpLigature2Test()
{
	m_ccls = 5;			// number of classes
	m_cclsLinear = 3;	// number of classes in linear format

	m_prgchwBIGGlyphList = new data16[73];

	m_prgichwOffsets = new data16[m_ccls+1];

	data16 * pchw = m_prgchwBIGGlyphList;

	//	Output class 0: clsVowelAcute
	m_prgichwOffsets[0] = 0;
	*pchw++ = msbf(data16(225));	// a-acute
	*pchw++ = msbf(data16(233));	// e-acute
	*pchw++ = msbf(data16(237));	// i-acute
	*pchw++ = msbf(data16(243));	// o-acute
	*pchw++ = msbf(data16(250));	// u-acute

	//	Output class 1: clsVowelGrave
	m_prgichwOffsets[1] = 5;
	*pchw++ = msbf(data16(224));	// a-grave
	*pchw++ = msbf(data16(232));	// e-grave
	*pchw++ = msbf(data16(236));	// i-grave
	*pchw++ = msbf(data16(242));	// o-grave
	*pchw++ = msbf(data16(249));	// u-grave

	//	Output class 2: clsCapVowel
	m_prgichwOffsets[2] = 5 + 5;	// 10
	*pchw++ = msbf(data16(65));	// A
	*pchw++ = msbf(data16(69));	// E
	*pchw++ = msbf(data16(73));	// I
	*pchw++ = msbf(data16(79));	// O
	*pchw++ = msbf(data16(85));	// U
	*pchw++ = msbf(data16(193));	// A-acute
	*pchw++ = msbf(data16(201));	// E-acute
	*pchw++ = msbf(data16(205));	// I-acute
	*pchw++ = msbf(data16(211));	// O-acute
	*pchw++ = msbf(data16(218));	// U-acute
	*pchw++ = msbf(data16(192));	// A-grave
	*pchw++ = msbf(data16(200));	// E-grave
	*pchw++ = msbf(data16(204));	// I-grave
	*pchw++ = msbf(data16(210));	// O-grave
	*pchw++ = msbf(data16(217));	// U-grave

	//	Input class 3: clsPlainVowel
	m_prgichwOffsets[3] = 10 + 15;	// 25
	*pchw++ = msbf(data16(5));
	*pchw++ = msbf(data16(4));	*pchw++ = msbf(data16(2)); *pchw++ = msbf(data16(5-4));
	*pchw++ = msbf(data16(97));	*pchw++ = msbf(data16(0));	// a
	*pchw++ = msbf(data16(101));	*pchw++ = msbf(data16(1));	// e
	*pchw++ = msbf(data16(105));	*pchw++ = msbf(data16(2));	// i
	*pchw++ = msbf(data16(111));	*pchw++ = msbf(data16(3));	// o
	*pchw++ = msbf(data16(117));	*pchw++ = msbf(data16(4));	// u

	//	Input class 4: clsVowel
	m_prgichwOffsets[4] = 25 + 4+(5*2);	// 39
	*pchw++ = msbf(data16(15));
	*pchw++ = msbf(data16(8));	*pchw++ = msbf(data16(3)); *pchw++ = msbf(data16(15-8));
	*pchw++ = msbf(data16(97));	*pchw++ = msbf(data16(0));	// a
	*pchw++ = msbf(data16(101));	*pchw++ = msbf(data16(1));	// e
	*pchw++ = msbf(data16(105));	*pchw++ = msbf(data16(2));	// i
	*pchw++ = msbf(data16(111));	*pchw++ = msbf(data16(3));	// o
	*pchw++ = msbf(data16(117));	*pchw++ = msbf(data16(4));	// u
	*pchw++ = msbf(data16(224));	*pchw++ = msbf(data16(10));	// a-grave
	*pchw++ = msbf(data16(225));	*pchw++ = msbf(data16(5));	// a-acute
	*pchw++ = msbf(data16(232));	*pchw++ = msbf(data16(11));	// e-grave
	*pchw++ = msbf(data16(233));	*pchw++ = msbf(data16(6));	// e-acute
	*pchw++ = msbf(data16(236));	*pchw++ = msbf(data16(12));	// i-grave
	*pchw++ = msbf(data16(237));	*pchw++ = msbf(data16(7));	// i-acute
	*pchw++ = msbf(data16(242));	*pchw++ = msbf(data16(13));	// o-grave
	*pchw++ = msbf(data16(243));	*pchw++ = msbf(data16(8));	// o-acute
	*pchw++ = msbf(data16(249));	*pchw++ = msbf(data16(14));	// u-grave
	*pchw++ = msbf(data16(250));	*pchw++ = msbf(data16(9));	// u-acute

	m_prgichwOffsets[5] = 39 + 4+(15*2);	// 73
}

void GrGlyphTable::SetUpLigature2Test()
{
	SetNumberOfGlyphs(256);
	SetNumberOfStyles(1);

	GrGlyphSubTable * pgstbl = new GrGlyphSubTable();
	Assert(pgstbl);

	pgstbl->Initialize(1, 0, 256, 10, 4);
	SetSubTable(0, pgstbl);

	SetNumberOfComponents(2);	// comp.base, comp.i

	pgstbl->SetUpLigature2Test();
}

/***********************************************************************************************
	TODO: This method is BROKEN because m_prgibBIGAttrValues has been changed. It is no
	longer a data16 *. The Gloc table can contain 16-bit or 32-bit entries and must be
	accessed accordingly.
***********************************************************************************************/
void GrGlyphSubTable::SetUpLigature2Test()
{
	m_nAttrIDLim = 10;

	m_pgatbl = new GrGlyphAttrTable();
	m_pgatbl->Initialize(0, 330);

	for (int i = 0; i <= 65; i++)					// A
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(0));

	for (i = 66; i <= 69; i++)						// E
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(22));

	for (i = 70; i <= 73; i++)						// I
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(44));

	for (i = 74; i <= 79; i++)						// O
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(66));

	for (i = 80; i <= 85; i++)						// U
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(88));

	for (i = 86; i <= 192; i++)						// A-grave
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(110));

	m_prgibBIGAttrValues[193] = (byte)msbf(data16(132));	// A-acute

	for (i = 194; i <= 200; i++)					// E-grave
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(154));

	m_prgibBIGAttrValues[201] = (byte)msbf(data16(176));	// E-acute

	for (i = 202; i <= 204; i++)					// I-grave
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(198));

	m_prgibBIGAttrValues[205] = (byte)msbf(data16(220));	// I-acute

	for (i = 206; i <= 210; i++)					// O-grave
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(242));

	m_prgibBIGAttrValues[211] = (byte)msbf(data16(264));	// O-acute

	for (i = 212; i <= 217; i++)					// U-grave
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(286));

	m_prgibBIGAttrValues[218] = (byte)msbf(data16(308));	// O-acute

	for (i = 219; i < 256; i++)
		m_prgibBIGAttrValues[i] = (byte)msbf(data16(330));

	m_pgatbl->SetUpLigature2Test();
}

void GrGlyphAttrTable::SetUpLigature2Test()
{
	//	All ligatures have the following defined:
	//
	//		0 = 2		comp.base		(0,30, 100,100)
	//		1 = 6		comp.i			(0,0, 100,30)
	//		2=	100		comp.base.top
	//		3=	30				bottom
	//		4=	0				left
	//		5=	100				right
	//		6 =	30		comp.i.top
	//		7 =	0				bottom
	//		8=	0				left
	//		9=	100				right

	byte * pbBIG = m_prgbBIGEntries;

	GrGlyphAttrRun gatrun;

	for (int i = 0; i < 15; i++)
	{
		gatrun.m_bMinAttrID = 0;
		gatrun.m_cAttrs = 10;
		gatrun.m_rgchwBIGValues[0] = msbf(data16(2));
		gatrun.m_rgchwBIGValues[1] = msbf(data16(6));
		gatrun.m_rgchwBIGValues[2] = msbf(data16(100));
		gatrun.m_rgchwBIGValues[3] = msbf(data16(30));
		gatrun.m_rgchwBIGValues[4] = msbf(data16(0));
		gatrun.m_rgchwBIGValues[5] = msbf(data16(100));
		gatrun.m_rgchwBIGValues[6] = msbf(data16(30));
		gatrun.m_rgchwBIGValues[7] = msbf(data16(0));
		gatrun.m_rgchwBIGValues[8] = msbf(data16(0));
		gatrun.m_rgchwBIGValues[9] = msbf(data16(100));
		memcpy(pbBIG, &gatrun, 22);	// 22 = 10*2 + 2
		pbBIG += 22;
	}

	Assert(pbBIG == m_prgbBIGEntries + (22 * 15));
}

void GrSubPass::SetUpLigature2Test()
{
	if (m_ipass == 1)
	{
		m_nMaxRuleContext = m_nMaxChunk = 2;
		m_nMaxRuleLoop = 5;
		m_staBehavior = "Ligatures2";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpLigature2Test(m_ipass);

		m_crul = 3;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 0;
		m_prgibConstraintStart[1] = 0;
		m_prgibConstraintStart[2] = 0;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[7 + 12 + 12];
		pb = m_prgbActionBlock;

		//	Rule 0: gX  gI  >  @2  @1;

		*pb++ = kopPutCopy;			*pb++ = 1;		// PutCopy 1
		*pb++ = kopNext;							// Next
		*pb++ = kopPutCopy;			*pb++ = -1;		// PutCopy -1
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 1: clsPlainVowel  gAcute   >  clsVowelAcute:(1 2)  _;

		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 3 0
			*pb++ = 3;				*pb++ = 0;
		*pb++ = kopAssoc;			*pb++ = 2;		// Assoc 2 0 1
			*pb++ = 0;				*pb++ = 1;
		*pb++ = kopNext;							// Next
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero

		//	Rule 2: clsPlainVowel  gGrave   >  clsVowelGrave:(1 2)  _;

		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 3 1
			*pb++ = 3;				*pb++ = 1;
		*pb++ = kopAssoc;			*pb++ = 2;		// Assoc 2 0 1
			*pb++ = 0;				*pb++ = 1;
		*pb++ = kopNext;							// Next
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero
                                                                                                                                                  
		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
		m_prgibActionStart[1] = 7;
		m_prgibActionStart[2] = 7 + 12;
	}
	else if (m_ipass == 2)
	{
		m_nMaxRuleContext = m_nMaxChunk = 2;
		m_nMaxRuleLoop = 5;
		m_staBehavior = "Ligatures2";

		m_pfsm = new GrFSM();
		Assert(m_pfsm);
		m_pfsm->SetUpLigature2Test(m_ipass);

		m_crul = 1;

		//	Set up constraint code.
		m_prgbConstraintBlock = new byte[1];
		byte * pb = m_prgbConstraintBlock;
		*pb++ = kopRetTrue;							// RetTrue

		m_prgibConstraintStart = new data16[m_crul];
		m_prgibConstraintStart[0] = 0;

		//	Set up rule action code.
		m_prgbActionBlock = new byte[7 + 12 + 12];
		pb = m_prgbActionBlock;

		//	Rule 0: clsVowel gI  >  clsCapVowel:(1 2) { comp {v.ref = @1; i.ref = @2}} _;

		*pb++ = kopPutSubs8bitObs;	*pb++ = 0;		// PutSubs 0 4 2
			*pb++ = 4;				*pb++ = 2;
		*pb++ = kopAssoc;			*pb++ = 2;		// Assoc 2 0 1
			*pb++ = 0;				*pb++ = 1;
		*pb++ = kopPushByte;		*pb++ = 0;		// Push 0
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot compRef v
			*pb++ = kslatCompRef;	*pb++ = 0;
		*pb++ = kopPushByte;		*pb++ = 1;		// Push 1
		*pb++ = kopIAttrSetSlot;					// IAttrSetSlot compRef i
			*pb++ = kslatCompRef;	*pb++ = 1;
		*pb++ = kopNext;							// Next
		*pb++ = kopDelete;							// Delete
		*pb++ = kopNext;							// Next
		*pb++ = kopRetZero;							// RetZero
                                                                                                                               
		m_prgibActionStart = new data16[m_crul];
		m_prgibActionStart[0] = 0;
	}
	else
		Assert(false);
}


void GrFSM::SetUpLigature2Test(int ipass)
{
	if (ipass == 1)
	{
		//	Create machine class ranges.
		m_cmcr = 8;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 47;	// acute
		m_prgmcr[0].m_chwLast	= 47;
		m_prgmcr[0].m_col		= 3;

		m_prgmcr[1].m_chwFirst	= 92;	// grave
		m_prgmcr[1].m_chwLast	= 92;
		m_prgmcr[1].m_col		= 4;

		m_prgmcr[2].m_chwFirst	= 97;	// a
		m_prgmcr[2].m_chwLast	= 97;
		m_prgmcr[2].m_col		= 1;

		m_prgmcr[3].m_chwFirst	= 101;	// e
		m_prgmcr[3].m_chwLast	= 101;
		m_prgmcr[3].m_col		= 1;

		m_prgmcr[4].m_chwFirst	= 105;	// i
		m_prgmcr[4].m_chwLast	= 105;
		m_prgmcr[4].m_col		= 2;

		m_prgmcr[5].m_chwFirst	= 111;	// o
		m_prgmcr[5].m_chwLast	= 111;
		m_prgmcr[5].m_col		= 1;

		m_prgmcr[6].m_chwFirst	= 117;	// u
		m_prgmcr[6].m_chwLast	= 117;
		m_prgmcr[6].m_col		= 1;

		m_prgmcr[7].m_chwFirst	= 120;	// x
		m_prgmcr[7].m_chwLast	= 120;
		m_prgmcr[7].m_col		= 0;

		m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
		m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 6;
		m_crowNonAcpt = 3;
		m_crowFinal = 3;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 5;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 15
		short * psn = m_prgrowTransitions;
		*psn++ = 1; *psn++ = 2; *psn++ = 2;	*psn++ = 0;	*psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 3;	*psn++ = 0;	*psn++ = 0;
		*psn++ = 0; *psn++ = 0; *psn++ = 0;	*psn++ = 4;	*psn++ = 5;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[3];	// 3 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[3+1];	// 3 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s3: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;	// s4: r1
		m_prgrulnMatched[1] = 1;

		m_prgirulnMin[2]	= 2;	// s5: r2
		m_prgrulnMatched[2] = 2;

		m_prgirulnMin[3]	= 3;
	}
	else if (ipass == 2)
	{
		//	Create machine class ranges.
		m_cmcr = 10;
		m_prgmcr = new GrFSMClassRange[m_cmcr];
		m_prgmcr[0].m_chwFirst	= 97;	// a
		m_prgmcr[0].m_chwLast	= 97;
		m_prgmcr[0].m_col		= 0;

		m_prgmcr[1].m_chwFirst	= 101;	// e
		m_prgmcr[1].m_chwLast	= 101;
		m_prgmcr[1].m_col		= 0;

		m_prgmcr[2].m_chwFirst	= 105;	// i
		m_prgmcr[2].m_chwLast	= 105;
		m_prgmcr[2].m_col		= 1;

		m_prgmcr[3].m_chwFirst	= 111;	// o
		m_prgmcr[3].m_chwLast	= 111;
		m_prgmcr[3].m_col		= 0;

		m_prgmcr[4].m_chwFirst	= 117;	// u
		m_prgmcr[4].m_chwLast	= 117;
		m_prgmcr[4].m_col		= 0;

		m_prgmcr[5].m_chwFirst	= 224;	// a-grave, a-acute
		m_prgmcr[5].m_chwLast	= 225;
		m_prgmcr[5].m_col		= 0;

		m_prgmcr[6].m_chwFirst	= 232;	// e-grave, e-acute
		m_prgmcr[6].m_chwLast	= 233;
		m_prgmcr[6].m_col		= 0;

		m_prgmcr[7].m_chwFirst	= 236;	// i-grave, i-acute
		m_prgmcr[7].m_chwLast	= 237;
		m_prgmcr[7].m_col		= 0;

		m_prgmcr[8].m_chwFirst	= 242;	// o-grave, o-acute
		m_prgmcr[8].m_chwLast	= 243;
		m_prgmcr[8].m_col		= 0;

		m_prgmcr[9].m_chwFirst	= 249;	// u-grave, u-acute
		m_prgmcr[9].m_chwLast	= 250;
		m_prgmcr[9].m_col		= 0;


		m_dimcrInit = 8;		// (max power of 2 <= m_cmcr);
		m_cLoop = 3;			// log2(max power of 2 <= m_cmcr);
		m_imcrStart = m_cmcr - m_dimcrInit;


		m_crow = 3;
		m_crowNonAcpt = 2;
		m_crowFinal = 1;
		m_rowFinalMin = m_crow - m_crowFinal;
		m_ccol = 2;

		//	Set up transition table.
		m_prgrowTransitions = new short[(m_crow-m_crowFinal) * m_ccol];	// 4
		short * psn = m_prgrowTransitions;
		*psn++ = 1; *psn++ = 1;
		*psn++ = 0; *psn++ = 2;

		//	Set up matched-rules tables.
		m_prgrulnMatched = new data16[1];	// 1 = sum of rules matched for each accepting state
		m_prgirulnMin = new data16[1+1];	// 1 = m_crow - m_crowNonAcpt

		m_prgirulnMin[0]	= 0;	// s2: r0
		m_prgrulnMatched[0] = 0;

		m_prgirulnMin[1]	= 1;
	}
}

#endif // _DEBUG

} // namespace gr

#endif // OLD_TEST_STUFF

//:End Ignore
