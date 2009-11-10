/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: TestPasses.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Hard-coded passes for test procedures.
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

/*----------------------------------------------------------------------------------------------
	For release version.
----------------------------------------------------------------------------------------------*/
#ifndef _DEBUG

bool GrLineBreakPass::RunTestRules(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

bool GrSubPass::RunTestRules(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

bool GrPosPass::RunTestRules(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

#endif // !_DEBUG


#ifdef _DEBUG

/*----------------------------------------------------------------------------------------------
	Set up a table manager for testing.
----------------------------------------------------------------------------------------------*/
void GrTableManager::SetUpTest(std::wstring stuRendBehavior)
{
	if (stuRendBehavior == L"NoRules")
	{
		m_cpass = 2;
		m_cpassLB = 0;
		m_ipassPos1 = 1;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrPosPass(1);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpTestData();
	}
	else if (stuRendBehavior == L"JustReorder" || stuRendBehavior == "RightToLeftLayout")
	{
		m_cpass = 4;
		m_cpassLB = 0;
		m_ipassPos1 = 3;
		m_fBidi = true;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrBidiPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpReverseNumbersTest();
		m_prgppass[2]->SetUpTestData();
		m_prgppass[3]->SetUpTestData();

		m_prgppass[2]->SetTopDirLevel(TopDirectionLevel());
	}
	else if (stuRendBehavior == L"CrossLineContext")
	{
		m_cpass = 4;
		m_cpassLB = 1;
		m_ipassPos1 = 3;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrLineBreakPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpCrossLineContextTest();
		m_prgppass[2]->SetUpCrossLineContextTest();
		m_prgppass[3]->SetUpTestData();
	}
	else if (stuRendBehavior == L"Reprocess")
	{
		m_cpass = 6;
		m_cpassLB = 1;
		m_ipassPos1 = 5;
		m_fBidi = true;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrLineBreakPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrSubPass(3);
		m_prgppass[4] = new GrBidiPass(4);
		m_prgppass[5] = new GrPosPass(5);

		m_prgppass[0]->SetUpTestData();				// glyph-gen pass
		m_prgppass[1]->SetUpReprocessTest();		// line-break pass
		m_prgppass[2]->SetUpReverseNumbersTest();	// sub pass 1
		m_prgppass[3]->SetUpReprocessTest();		// sub pass 2
		m_prgppass[4]->SetUpTestData();				// bidi pass
		m_prgppass[5]->SetUpTestData();				// pos pass
		
		m_prgppass[4]->SetTopDirLevel(TopDirectionLevel());
	}
	else if (stuRendBehavior == L"LineEdgeContext")
	{
		m_cpass = 4;
		m_cpassLB = 0;
		m_ipassPos1 = 3;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpLineEdgeContextTest(1);
		m_prgppass[2]->SetUpLineEdgeContextTest(2);
		m_prgppass[3]->SetUpTestData();
	}
	else if (stuRendBehavior == L"BidiAlgorithm")
	{
		m_cpass = 4;
		m_cpassLB = 0;
		m_ipassPos1 = 3;
		m_fBidi = true;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrBidiPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpBidiAlgorithmTest();
		m_prgppass[2]->SetUpBidiAlgorithmTest();
		m_prgppass[3]->SetUpTestData();

		m_prgppass[2]->SetTopDirLevel(TopDirectionLevel());
	}
	else if (stuRendBehavior == L"PseudoGlyphs")
	{
		m_cpass = 3;
		m_cpassLB = 0;
		m_ipassPos1 = 2;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrPosPass(2);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpPseudoGlyphsTest();
		m_prgppass[2]->SetUpTestData();
	}
	else if (stuRendBehavior == L"SimpleFSM")
	{
		m_cpass = 3;
		m_cpassLB = 0;
		m_ipassPos1 = 2;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrPosPass(2);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpSimpleFSMTest();
		m_prgppass[2]->SetUpTestData();
	}
	else if (stuRendBehavior == L"RuleAction")
	{
		m_cpass = 3;
		m_cpassLB = 0;
		m_ipassPos1 = 2;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrPosPass(2);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpRuleActionTest();
		m_prgppass[2]->SetUpTestData();
	}
	else if (stuRendBehavior == L"RuleAction2")
	{
		m_cpass = 6;
		m_cpassLB = 0;
		m_ipassPos1 = 5;
		m_fBidi = true;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrSubPass(3);
		m_prgppass[4] = new GrBidiPass(4);
		m_prgppass[5] = new GrPosPass(5);

		m_prgppass[0]->SetUpTestData();			// glyph-gen pass
		m_prgppass[1]->SetUpRuleAction2Test();	// sub pass 1
		m_prgppass[2]->SetUpRuleAction2Test();	// sub pass 2
		m_prgppass[3]->SetUpRuleAction2Test();	// sub pass 3
		m_prgppass[4]->SetUpTestData();			// bidi pass
		m_prgppass[5]->SetUpTestData();			// pos pass

		m_prgppass[4]->SetTopDirLevel(TopDirectionLevel());
	}
	else if (stuRendBehavior == L"Assoc")
	{
		m_cpass = 5;
		m_cpassLB = 1;
		m_ipassPos1 = 4;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrLineBreakPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrSubPass(3);
		m_prgppass[4] = new GrPosPass(4);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpAssocTest();
		m_prgppass[2]->SetUpAssocTest();
		m_prgppass[3]->SetUpAssocTest();
		m_prgppass[4]->SetUpTestData();
	}
	else if (stuRendBehavior == L"Assoc2")
	{
		m_cpass = 3;
		m_cpassLB = 0;
		m_ipassPos1 = 2;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrPosPass(2);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpAssoc2Test();
		m_prgppass[2]->SetUpTestData();
	}
	else if (stuRendBehavior == L"DefaultAssoc")
	{
		m_cpass = 3;
		m_cpassLB = 0;
		m_ipassPos1 = 2;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrPosPass(2);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpDefaultAssocTest();
		m_prgppass[2]->SetUpTestData();
	}
	else if (stuRendBehavior == L"BidiNumbers")
	{
		m_cpass = 4;
		m_cpassLB = 0;
		m_ipassPos1 = 3;
		m_fBidi = true;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrBidiPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpBidiNumbersTest();
		m_prgppass[2]->SetUpBidiNumbersTest();
		m_prgppass[3]->SetUpTestData();

		m_prgppass[2]->SetTopDirLevel(TopDirectionLevel());
	}
	else if (stuRendBehavior == L"Feature")
	{
		m_cpass = 3;
		m_cpassLB = 0;
		m_ipassPos1 = 2;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrPosPass(2);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpFeatureTest();
		m_prgppass[2]->SetUpTestData();
	}
	else if (stuRendBehavior == L"Ligatures")
	{
		m_cpass = 4;
		m_cpassLB = 1;
		m_ipassPos1 = 3;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrLineBreakPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpLigatureTest();
		m_prgppass[2]->SetUpLigatureTest();
		m_prgppass[3]->SetUpTestData();
	}
	else if (stuRendBehavior == L"Ligatures2")
	{
		m_cpass = 4;
		m_cpassLB = 1;
		m_ipassPos1 = 3;
		m_fBidi = false;

		m_prgppass = new GrPass*[m_cpass];
		m_prgppass[0] = new GrGlyphGenPass(0);
		m_prgppass[1] = new GrSubPass(1);
		m_prgppass[2] = new GrSubPass(2);
		m_prgppass[3] = new GrPosPass(3);

		m_prgppass[0]->SetUpTestData();
		m_prgppass[1]->SetUpLigature2Test();
		m_prgppass[2]->SetUpLigature2Test();
		m_prgppass[3]->SetUpTestData();
	}
	else
		Assert(false);

	m_engst.m_prgpsstrm = new GrSlotStream*[m_cpass];
	for (int ipass = 0; ipass < m_cpass; ++ipass)
		m_engst.m_prgpsstrm[ipass] = new GrSlotStream(ipass);
}


/*----------------------------------------------------------------------------------------------
	Set the pass variables that normally will be read from the ECF.
----------------------------------------------------------------------------------------------*/

void GrPass::SetUpTestData()
{
	m_nMaxRuleContext = m_nMaxChunk = 2;
	m_nMaxRuleLoop = 2;
}

//void GrBidiPass::SetUpTestData()
//{
//	m_nMaxRuleContext = m_nMaxChunk = 1;
//	m_nMaxRuleLoop = 2;
//}

/*----------------------------------------------------------------------------------------------
	Call the appropriate test function.
----------------------------------------------------------------------------------------------*/

bool GrLineBreakPass::RunTestRules(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	if (m_staBehavior == "Reprocess")
		return RunReprocessTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "CrossLineContext")
		return RunCrossLineContextTest(ptman, psstrmInput, psstrmOutput);
	else
		return false;
}

bool GrSubPass::RunTestRules(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	if (m_staBehavior == "ReverseNumbers")
		return RunReverseNumbersTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "CrossLineContext")
		return RunCrossLineContextTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "Reprocess")
		return RunReprocessTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "LineEdgeContext")
		return RunLineEdgeContextTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "BidiAlgorithm")
		return RunBidiAlgorithmTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "PseudoGlyphs")
		return RunPseudoGlyphsTest(ptman, psstrmInput, psstrmOutput);
	else if (m_staBehavior == "BidiNumbers")
		return RunBidiNumbersTest(ptman, psstrmInput, psstrmOutput);
	else
		return false;
}

bool GrPosPass::RunTestRules(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	return false;
}

/*----------------------------------------------------------------------------------------------
	A substitution pass that causes numbers to be reversed. Used by both the JustReorder test
	and the Reprocess test.
----------------------------------------------------------------------------------------------*/

void GrSubPass::SetUpReverseNumbersTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "ReverseNumbers";
}

bool GrSubPass::RunReverseNumbersTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	data16 chw0;

	//	clsDigit { dir = DIR_RIGHT };
	if (psstrmInput->SlotsPending() >= 1)
	{
		bool fRtl = ptman->RightToLeft();
		chw0 = psstrmInput->Peek(0)->GlyphID();
		if ((chw0 >= '0') && (chw0 <= '9'))
		{
			GrSlotState * pslot0 = psstrmInput->NextGet();
//			GrFeatureValues fval0;
//			pslot0->GetFeatureValues(&fval0);

			GrSlotState * pslotNew;
			ptman->NewSlotCopy(pslot0, m_ipass, &pslotNew);
			pslotNew->SetDirectionality(fRtl ? kdircArabNum : kdircR);
			pslotNew->Associate(pslot0);
			psstrmOutput->NextPut(pslotNew);

			return true;
		}
		else
		{
			GrSlotState * pslot0 = psstrmInput->NextGet();
//			GrFeatureValues fval0;
//			pslot0->GetFeatureValues(&fval0);

			GrSlotState * pslotNew;
			ptman->NewSlotCopy(pslot0, m_ipass, &pslotNew);
			if (chw0 == 32)
				pslotNew->SetDirectionality(kdircWhiteSpace);
			else if (chw0 == '.')
				pslotNew->SetDirectionality(kdircComSep);
			else
				pslotNew->SetDirectionality(fRtl ? kdircR : kdircL);
			pslotNew->Associate(pslot0);
			psstrmOutput->NextPut(pslotNew);

			return true;
		}
	}
	else
		return false;
}

/*----------------------------------------------------------------------------------------------
	A substitution pass that will do some hard-coded actions resulting in cross-line-boundary
	contextuals.
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpCrossLineContextTest()
{
	m_fLineBreak = true;
	m_cchwPreXlbContext = 2;
	m_cchwPostXlbContext = 0;
}

void GrLineBreakPass::SetUpCrossLineContextTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "CrossLineContext";
}

void GrSubPass::SetUpCrossLineContextTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 3;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "CrossLineContext";
}

bool GrLineBreakPass::RunCrossLineContextTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	//  '=' { break = intra };
	if ((psstrmInput->SlotsPending() >= 1) &&
		(psstrmInput->Peek(0)->GlyphID() == 61))
	{
		GrSlotState * pslotNew0;
		ptman->NewSlotCopy(psstrmInput->NextGet(), m_ipass, &pslotNew0);
		pslotNew0->SetBreakWeight(int(klbHyphenBreak));
		psstrmOutput->NextPut(pslotNew0);

		psstrmOutput->SetPosForNextRule(0, psstrmInput, false);

		return true;
	}
	else
		return false;
}

bool GrSubPass::RunCrossLineContextTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	//	any '=' _ _  >  any '=' '=':2  any:1  /  _ _ # _ _
	if ((psstrmInput->SlotsPending() >= 3) &&
		(psstrmInput->Peek(1)->GlyphID() == 61) &&
		(psstrmInput->Peek(2)->GlyphID() == ptman->LBGlyphID()))
	{
		GrSlotState * pslot0 = psstrmInput->Peek(0);
		GrSlotState * pslot1 = psstrmInput->Peek(1);
		gid16 chw0 = pslot0->GlyphID();
//		GrFeatureValues fval0;
//		pslot0->GetFeatureValues(&fval0);
//		GrFeatureValues fval1;
//		pslot1->GetFeatureValues(&fval1);

		// copy the first 3 slots
		psstrmOutput->CopyOneSlotFrom(psstrmInput);
		psstrmOutput->CopyOneSlotFrom(psstrmInput);
		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		// insert 2 more
		GrSlotState * pslot3;
		ptman->NewSlot(61, pslot1, NULL, m_ipass, &pslot3);
		pslot3->Associate(pslot1);
		psstrmOutput->NextPut(pslot3);

		GrSlotState * pslot4;
		ptman->NewSlot(chw0, pslot0, NULL, m_ipass, &pslot4);
		pslot4->Associate(pslot0);
		psstrmOutput->NextPut(pslot4);

		return true;
	}
	else
		return false;
}

/*----------------------------------------------------------------------------------------------
	Line break and substitution passes that cause intra-pass reprocessing (ie, the ^ mechanism).
----------------------------------------------------------------------------------------------*/

void GrLineBreakPass::SetUpReprocessTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 2;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "Reprocess";
}

bool GrLineBreakPass::RunReprocessTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	gid16 chw;

	//	'^' {lb=2}  / _ lc
	if ((psstrmInput->SlotsPending() >=2) &&
		(psstrmInput->Peek(0)->GlyphID() == '^') &&
		(((chw = psstrmInput->Peek(1)->GlyphID()) >= 'a') &&
			(chw <= 'z')))
	{
		GrSlotState * pslotNew0;
		ptman->NewSlotCopy(psstrmInput->NextGet(), m_ipass, &pslotNew0);
		pslotNew0->SetBreakWeight(int(klbHyphenBreak));
		psstrmOutput->NextPut(pslotNew0);

		psstrmOutput->SetPosForNextRule(0, psstrmInput, false);

		return true;
	}
	else
		return false;
}


void GrSubPass::SetUpReprocessTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 3;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "Reprocess";
}

bool GrSubPass::RunReprocessTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	gid16 chw;
	gid16 chwLB = ptman->LBGlyphID();

	//	symbol  >  @2 {dir = DIR_ARABNUMBER}  /  any {dir != DIR_LEFT} ^ _
	if ((psstrmInput->SlotsPending() >= 2) &&
		(psstrmInput->Peek(0)->Directionality() != kdircL) &&
		(((chw = psstrmInput->Peek(1)->GlyphID()) == '@') ||
			(chw == '@') || (chw == '$') || (chw == '%') ||
			(chw == '^') || (chw == '&') || (chw == '*')))
	{
		psstrmOutput->CopyOneSlotFrom(psstrmInput);
		GrSlotState * pslotNew1;
		ptman->NewSlotCopy(psstrmInput->NextGet(), m_ipass, &pslotNew1);
		pslotNew1->SetDirectionality(kdircArabNum);
		psstrmOutput->NextPut(pslotNew1);

		psstrmOutput->SetPosForNextRule(-1, psstrmInput, false);

		return true;
	}

	//	'^' lc  >  @1  uc
	else if ((psstrmInput->SlotsPending() >= 2) &&
		(psstrmInput->Peek(0)->GlyphID() == '^') &&
		(((chw = psstrmInput->Peek(1)->GlyphID()) >= 'a') &&
			(chw <= 'z')))
	{
		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		gid16 chwNew = chw - 'a' + 'A';
		GrSlotState * pslotNew1;
		ptman->NewSlotCopy(psstrmInput->NextGet(), m_ipass, &pslotNew1);
		pslotNew1->SetGlyphID(chwNew);
		psstrmOutput->NextPut(pslotNew1);

		psstrmOutput->SetPosForNextRule(0, psstrmInput, false);

		return true;
	}

	//	'^' lc  >  @1  uc / _ # _
	else if ((psstrmInput->SlotsPending() >= 3) &&
		(psstrmInput->Peek(0)->GlyphID() == '^') &&
		(psstrmInput->Peek(1)->IsLineBreak(chwLB)) &&
		(((chw = psstrmInput->Peek(2)->GlyphID()) >= 'a') &&
			(chw <= 'z')))
	{
		psstrmOutput->CopyOneSlotFrom(psstrmInput);
		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		gid16 chwNew = chw - 'a' + 'A';
		GrSlotState * pslotNew1;
		ptman->NewSlotCopy(psstrmInput->NextGet(), m_ipass, &pslotNew1);
		pslotNew1->SetGlyphID(chwNew);
		psstrmOutput->NextPut(pslotNew1);

		psstrmOutput->SetPosForNextRule(0, psstrmInput, false);

		return true;
	}

	else
		return false;
}


/*----------------------------------------------------------------------------------------------
	Substitution passes that create contextual forms at the line breaks; also a cross-line-
	boundary contextual.
----------------------------------------------------------------------------------------------*/

void GrSubPass::SetUpLineEdgeContextTest(int ipass)
{
	m_nMaxRuleContext = (ipass == 2)? 5: 2;
	m_nMaxChunk = m_nMaxRuleContext;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "LineEdgeContext";
}

bool GrSubPass::RunLineEdgeContextTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	gid16 chwLB = ptman->LBGlyphID();

	//	any _ _  >  @1 '-' '-' /  _ _ _ #
	if ((m_ipass == 1) &&
		(psstrmInput->SlotsPending() >= 2) &&
		(psstrmInput->Peek(1)->IsLineBreak(chwLB)))
	{
		GrSlotState * pslot0 = psstrmInput->Peek(0);
//		GrFeatureValues fval;
//		pslot0->GetFeatureValues(&fval);

		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		GrSlotState * pslotNew1;
		ptman->NewSlot('-', pslot0, NULL, m_ipass, &pslotNew1);
		pslotNew1->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew1);

		GrSlotState * pslotNew2;
		ptman->NewSlot('-', pslot0, NULL, m_ipass, &pslotNew2);
		pslotNew2->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew2);

		return true;
	}

	//	_ _ any >  '+' '+' @4 /  # _ _ _
	else if ((m_ipass == 1) &&
		(psstrmInput->SlotsPending() >= 2) &&
		(psstrmInput->Peek(0)->IsLineBreak(chwLB)))
	{
		GrSlotState * pslot2 = psstrmInput->Peek(1);
//		GrFeatureValues fval;
//		pslot2->GetFeatureValues(&fval);

		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		GrSlotState * pslotNew0;
		ptman->NewSlot('+', pslot2, NULL, m_ipass, &pslotNew0);
		pslotNew0->Associate(pslot2);
		psstrmOutput->NextPut(pslotNew0);

		GrSlotState * pslotNew1;
		ptman->NewSlot('+', pslot2, NULL, m_ipass, &pslotNew1);
		pslotNew1->Associate(pslot2);
		psstrmOutput->NextPut(pslotNew1);

		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		return true;
	}

	//	'-' '-' '+' '+' >  @1 _ _ @4 /  _ _ # _ _
	else if ((m_ipass == 2) &&
		(psstrmInput->SlotsPending() >= 5) &&
		(psstrmInput->Peek(0)->GlyphID() == '-') &&
		(psstrmInput->Peek(1)->GlyphID() == '-') &&
		(psstrmInput->Peek(2)->IsLineBreak(chwLB)) &&
		(psstrmInput->Peek(3)->GlyphID() == '+') &&
		(psstrmInput->Peek(4)->GlyphID() == '+'))
	{
		psstrmOutput->CopyOneSlotFrom(psstrmInput);
		psstrmInput->Skip(1);
		psstrmOutput->CopyOneSlotFrom(psstrmInput);
		psstrmInput->Skip(1);
		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		return true;
	}

	else
		return false;
}

/*----------------------------------------------------------------------------------------------
	Substitution passes that do something special with pseudo glyphs.
----------------------------------------------------------------------------------------------*/

void GrSubPass::SetUpPseudoGlyphsTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 3;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "PseudoGlyphs";
}

bool GrSubPass::RunPseudoGlyphsTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	gid16 chwLB = ptman->LBGlyphID();

	//	pseudo  >  uppercase / _ ' '
	if ((psstrmInput->SlotsPending() >= 2) &&
		(psstrmInput->Peek(0)->GlyphID() > 1000) &&		// pseudo
		(psstrmInput->Peek(1)->GlyphID() == 32))
	{
		GrSlotState * pslot0 = psstrmInput->NextGet();
//		GrFeatureValues fval;
//		pslot0->GetFeatureValues(&fval);

		GrSlotState * pslotNew;
		ptman->NewSlotCopy(pslot0, m_ipass, &pslotNew);
		pslotNew->SetGlyphID(pslot0->GlyphID() - 1000);
		pslotNew->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew);

		psstrmOutput->SetPosForNextRule(0, psstrmInput, false);

		return true;
	}

	//	pseudo  >  uppercase / _ |
	//	pseudo  >  uppercase / _ #
	else if ((psstrmInput->SlotsPending() <= 2) &&
		(psstrmInput->Peek(0)->GlyphID() > 1000) &&	// pseudo
		(psstrmInput->SlotsPending() < 2 || psstrmInput->Peek(1)->GlyphID() == 35))
	{
		GrSlotState * pslot0 = psstrmInput->NextGet();
//		GrFeatureValues fval;
//		pslot0->GetFeatureValues(&fval);

		GrSlotState * pslotNew;
		ptman->NewSlotCopy(pslot0, m_ipass, &pslotNew);
		pslotNew->SetGlyphID(pslot0->GlyphID() - 1000);
		pslotNew->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew);

		psstrmOutput->SetPosForNextRule(0, psstrmInput, false);

		return true;
	}

	//	pseudo any _  >  lowercase:1 @2 @1 / _ _ ^ _;
	else if ((psstrmInput->SlotsPending() >= 3) &&
		(psstrmInput->Peek(0)->GlyphID() > 1000))	// pseudo
	{
		GrSlotState * pslot0 = psstrmInput->NextGet();
//		GrFeatureValues fval;
//		pslot0->GetFeatureValues(&fval);

		GrSlotState * pslotNew0;
		ptman->NewSlot(pslot0->GlyphID()-1000-65+97, pslot0, NULL, m_ipass, &pslotNew0);
		pslotNew0->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew0);

		psstrmOutput->CopyOneSlotFrom(psstrmInput);

		GrSlotState * pslotNew2;
		ptman->NewSlot(pslot0->GlyphID(), pslot0, NULL, m_ipass, &pslotNew2);
		pslotNew2->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew2);

		psstrmOutput->SetPosForNextRule(-1, psstrmInput, false);

		return true;
	}
	else
		return false;
}

/*----------------------------------------------------------------------------------------------
	Substitution pass that tests reversing numbers.
----------------------------------------------------------------------------------------------*/
void GrSubPass::SetUpBidiNumbersTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "BidiNumbers";
}

bool GrSubPass::RunBidiNumbersTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	gid16 chw0;

	//	clsDigit > clsDigit { dir = DIR_ARABNUMBER };
	if ((psstrmInput->SlotsPending() >= 1) &&
		(((chw0 = psstrmInput->Peek(0)->GlyphID()) >= '0' && chw0 <= '9') ||
		(chw0 >= 'A' && chw0 <= 'Z') || (chw0 >= 'a' && chw0 <= 'z')))
	{
		GrSlotState * pslot0 = psstrmInput->NextGet();
//		GrFeatureValues fval0;
//		pslot0->GetFeatureValues(&fval0);

		GrSlotState * pslotNew;
		ptman->NewSlot(chw0, pslot0, NULL, m_ipass, &pslotNew);
		if (chw0 >= '0' && chw0 <= '9')
			pslotNew->SetDirectionality(kdircArabNum);
		else // a-z and A-Z
			pslotNew->SetDirectionality(kdircR);
		pslotNew->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew);

		return true;
	}
	else
		return false;
}

void GrBidiPass::SetUpBidiNumbersTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 2;
	m_nTopDirection = 1;
}

/*----------------------------------------------------------------------------------------------
	Substitution pass that tests the bidi algorithm thoroughly.
----------------------------------------------------------------------------------------------*/
void GrSubPass::SetUpBidiAlgorithmTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 2;
	m_staBehavior = "BidiAlgorithm";
}

bool GrSubPass::RunBidiAlgorithmTest(GrTableManager * ptman,
	GrSlotStream * psstrmInput, GrSlotStream * psstrmOutput)
{
	gid16 chw0;

	if (psstrmInput->SlotsPending() >= 1)
	{
		chw0 = psstrmInput->Peek(0)->GlyphID();

		DirCode dircNew;
		
		if (chw0 >= '0' && chw0 <= '9')
			dircNew = kdircEuroNum;
		else if (chw0 >= 'a' && chw0 <= 'z')
			dircNew = kdircL;
		else if (chw0 >= 'A' && chw0 <= 'M')
			dircNew = kdircR;
		else if (chw0 >= 'N' && chw0 <= 'T')
			dircNew = kdircRArab;
		else if (chw0 >= 'U' && chw0 <= 'Z')
			dircNew = kdircArabNum;
		else if (chw0 == '$' || chw0 == '%')
			dircNew = kdircEuroTerm;
		else if (chw0 == '.' || chw0 == '*' || chw0 == '+')
			dircNew = kdircEuroSep;
		else if (chw0 == '@')
			dircNew = kdircComSep;
		else if (chw0 == '_' || chw0 == '=')
			dircNew = kdircNeutral;
		else if (chw0 == ':')
			dircNew = kdircNSM;
		else
			return false;

		GrSlotState * pslot0 = psstrmInput->NextGet();
//		GrFeatureValues fval0;
//		pslot0->GetFeatureValues(&fval0);

		GrSlotState * pslotNew;
		ptman->NewSlot(chw0, pslot0, NULL, m_ipass, &pslotNew);
		pslotNew->SetDirectionality(dircNew);
		pslotNew->Associate(pslot0);
		psstrmOutput->NextPut(pslotNew);

		return true;
	}
	else
		return false;
}

void GrBidiPass::SetUpBidiAlgorithmTest()
{
	m_nMaxRuleContext = m_nMaxChunk = 1;
	m_nMaxRuleLoop = 2;
	m_nTopDirection = 0;
}


#endif // _DEBUG

} // namespace gr

#endif // OLD_TEST_STUFF

//:End Ignore
