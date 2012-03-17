/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrPassActionCode.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    A continution of the GrPass file, including functions to run the action commands.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif

//:End Ignore

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local type definitions
//:>********************************************************************************************

typedef signed   char Int8;		// signed value
typedef unsigned char Uint8;	// unsigned value


namespace gr
{

//:>********************************************************************************************
//:>	Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	The main function to run a block of action or constraint code and return a result.

	@param ptman			- table manager, for generating new slots; NULL when
								running constraints
	@param pbStart			- address of the first command
	@param fConstraints		- true if we are running constraint tests;
								false if we are running rule actions
	@param psstrmIn			- input stream
	@param psstrmOut		- output stream; when running constraints, used to access items
								in the precontext
	@param islot			- slot being processed relative to the start of the rule;
								used only for constraints; must be 0 for rule actions
----------------------------------------------------------------------------------------------*/
int GrPass::RunCommandCode(GrTableManager * ptman,
	byte * pbStart, bool fConstraints,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islot)
{
	byte * pbNext = pbStart;

//	std::vector<int> vnStack;
//	vnStack.EnsureSpace(128);	// make it nice and big
    m_vnStack.clear();

	gAssert(fConstraints || islot == 0);

	//	There are two states: fMustGet = true means we need to get a slot to work on from
	//	the input; fMustGet = false means we have a slot and we need to do a Next before
	//	we can get the next one.
	bool fMustGet = !fConstraints;

	bool fInserting = false;	// set to true by an Insert command

	int nRet = 0;

	while (true)
	{
		ActionCommand op = ActionCommand(*pbNext++);
		StackMachineFlag smf;
		nRet = RunOneCommand(ptman, fConstraints, op,
			&pbNext, &fMustGet, &fInserting,
			psstrmIn, psstrmOut, islot,
			m_vnStack, &smf);

		if (smf == ksmfDone)
			return nRet;

		if (smf == ksmfUnderflow)
		{
			Warn("Underflow in Graphite stack machine");
			gAssert(false);
			FontException fexptn;
			fexptn.version = -1;
			fexptn.subVersion = -1;
            fexptn.errorCode = kferrUnknown;
			throw fexptn; // disastrous error in rendering; fall back to dumb rendering
		}
	}
	return nRet; // not needed, but to avoid compiler warning
}

/*----------------------------------------------------------------------------------------------
	Perform a single command. Specifically, read the arguments from the byte buffer and the
	run the appropriate functions.

	@param ptman			- table manager, for generating new slots; or NULL
	@param fConstraints		- true if we are running constraints; false if we are running
								rule actions
	@param op				- command operator
	@param ppbArg			- pointer to first argument
	@param pfMustGet		- state: getting input or processing it; when running constraints,
								always false
	@param pfInserting		- true if current item is to be inserted, or was inserted
	@param psstrmIn			- input stream
	@param psstrmOut		- output stream
	@param islot			- the slot being processed relative to the beginning of the rule;
								only relevant for constraint testing, must be 0 for rule actions
	@param vnStack			- the stack of values being manipulated
	@param psmf				- error code, done flag, or continue flag
----------------------------------------------------------------------------------------------*/
int GrPass::RunOneCommand(GrTableManager * ptman, bool fConstraints,
	ActionCommand op, byte ** ppbArg, bool * pfMustGet, bool * pfInserting,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islot,
	std::vector<int> & vnStack, StackMachineFlag * psmf)
{
	*psmf = ksmfContinue;

	//	General purpose variables:
	int arg1, arg2, arg3, arg4;
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
	SlotAttrName slat;

	int i;
	std::vector<int> vnTmp;

	byte * pbNext = *ppbArg;

	int nRet = 0;

	switch (op)
	{
	case kopNop:
		break;

	case kopPushByte:
		arg1 = Int8(*pbNext++);
		vnStack.push_back(arg1);
		break;
	case kopPushByteU:
		arg1 = Uint8(*pbNext++);
		vnStack.push_back(arg1);
		break;
	case kopPushShort:
		arg1 = Int8(*pbNext++);
		arg2 = Uint8(*pbNext++);
		vnStack.push_back((arg1 << 8) + arg2);
		break;
	case kopPushShortU:
		arg1 = Uint8(*pbNext++);
		arg2 = Uint8(*pbNext++);
		vnStack.push_back((arg1 << 8) + arg2);
		break;
	case kopPushLong:
		arg1 = Int8(*pbNext++);
		arg2 = Uint8(*pbNext++);
		arg3 = Uint8(*pbNext++);
		arg4 = Uint8(*pbNext++);
		vnStack.push_back((arg1 << 24) + (arg2 << 16) + (arg3 << 8) + arg4);
		break;

	case kopNeg:
	case kopTrunc8:	case kopTrunc16:
	case kopNot:
		DoStackArithmetic1Arg(op, vnStack, psmf);
		if (*psmf != ksmfContinue)
			return 0;
		break;

	case kopAdd:	case kopSub:
	case kopMul:	case kopDiv:
	case kopMin:	case kopMax:
	case kopAnd:	case kopOr:
	case kopEqual:	case kopNotEq:
	case kopLess:	case kopGtr:
	case kopLessEq:	case kopGtrEq:
		DoStackArithmetic2Args(op, vnStack, psmf);
		if (*psmf != ksmfContinue)
			return 0;
		break;

	case kopCond:
		DoConditional(vnStack, psmf);
		if (*psmf != ksmfContinue)
			return 0;
		break;

	case kopNext:
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(!*pfMustGet);
			DoNext(ptman, 1, psstrmIn, psstrmOut);
			*pfMustGet = true;
			*pfInserting = false;
		}
		break;
	case kopNextN:
		c = Int8(*pbNext++);
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(false);	// for now we don't allow anything other than moving one slot
			gAssert(!*pfMustGet);									// forward at a time
			DoNext(ptman, c, psstrmIn, psstrmOut);
			*pfMustGet = true;
		}
		break;
	case kopPutGlyph8bitObs:
	case kopPutGlyph:
		if (op == kopPutGlyph8bitObs)
            nOutputClass = Uint8(*pbNext++);
		else
		{
			nOutputClass = (Uint8(*pbNext++) << 8);
			nOutputClass += Uint8(*pbNext++);
		}
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(*pfMustGet);
			DoPutGlyph(ptman, *pfInserting, nOutputClass, psstrmIn, psstrmOut);
			*pfMustGet = false;
		}
		break;
	case kopPutCopy:
		nSlotRef = Int8(*pbNext++);
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(*pfMustGet);
			DoPutCopy(ptman, *pfInserting, nSlotRef, psstrmIn, psstrmOut);
			*pfMustGet = false;
		}
		break;
	case kopPutSubs8bitObs:
	case kopPutSubs:
		nSlotRef = Int8(*pbNext++);
		if (op == kopPutSubs8bitObs)
		{
			nInputClass = Uint8(*pbNext++);
			nOutputClass = Uint8(*pbNext++);
		}
		else
		{
			nInputClass = (Uint8(*pbNext++) << 8);
			nInputClass += Uint8(*pbNext++);
			nOutputClass = (Uint8(*pbNext++) << 8);
			nOutputClass += Uint8(*pbNext++);
		}
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(*pfMustGet);
			DoPutSubs(ptman, *pfInserting, nSlotRef, nInputClass, nOutputClass,
				psstrmIn, psstrmOut);
			*pfMustGet = false;
		}
		break;
	case kopCopyNext:
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(*pfMustGet);
			gAssert(!*pfInserting);		// 0 means copy this slot
			DoPutCopy(ptman, *pfInserting, 0, psstrmIn, psstrmOut);
			DoNext(ptman, 1, psstrmIn, psstrmOut);
			// *pfMustGet still = true
		}
		break;
	case kopInsert:
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(*pfMustGet);
			*pfInserting = true;
			// *pfMustGet still = true

			if (ptman->LoggingTransduction())
				m_pzpst->RecordInsertionAt(psstrmOut->WritePos());
		}
		break;
	case kopDelete:
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(*pfMustGet);
			DoDelete(ptman, psstrmIn, psstrmOut);
			*pfMustGet = false;
		}
		break;
	case kopAssoc:
		c = Uint8(*pbNext++);
		// std::vector<int> vnTmp;
		for (i = 0; i < c; i++)
			vnTmp.push_back(Int8(*pbNext++));
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(!*pfMustGet);
			DoAssoc(c, vnTmp, *pfInserting, psstrmIn, psstrmOut);
		}
		break;
	case kopCntxtItem:
		islotArg = Int8(*pbNext++);
		c = Uint8(*pbNext++);
		if (fConstraints)
		{
			//	Old approach:
			//	If this is NOT the relevant item, push TRUE, which causes the subsequent tests
			//	(which are eventually ORed with it) to become irrelevant.
			//vnStack.Push((int)(islotArg != islot));

			//	If this is not the relevant item, skip the specified number of bytes.
			if (islotArg != islot)
			{
				pbNext += c;
				vnStack.push_back(1);
			}
		}
		else
		{
			gAssert(false);
		}
		break;

	case kopAttrSet:
	case kopAttrAdd:
	case kopAttrSub:
	case kopAttrSetSlot:
		slat = SlotAttrName(Uint8(*pbNext++));	// slot attribute ID
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(!*pfMustGet);
			DoSetAttr(ptman, op, *pfInserting, slat, -1, vnStack, psstrmIn, psstrmOut);
			if (*psmf != ksmfContinue)
				return 0;
		}
		break;
	case kopIAttrSet:
	case kopIAttrAdd:
	case kopIAttrSub:
	case kopIAttrSetSlot:
		slat = SlotAttrName(Uint8(*pbNext++));	// slot attribute ID
		nIndex = Uint8(*pbNext++); // index; eg, global ID for component
		if (fConstraints)
		{
			gAssert(false);
		}
		else
		{
			gAssert(!*pfMustGet);
			DoSetAttr(ptman, op, *pfInserting, slat, nIndex, vnStack,
				psstrmIn, psstrmOut);
			if (*psmf != ksmfContinue)
				return 0;
		}
		break;

	case kopPushSlotAttr:
		slat = SlotAttrName(Uint8(*pbNext++));
		nSlotRef = Int8(*pbNext++);
		if (fConstraints)
			nSlotRef += islot + 1;	// +1 to peek ahead to a slot we haven't "got" yet
		DoPushSlotAttr(ptman, nSlotRef, *pfInserting, slat, -1, vnStack, psstrmIn, psstrmOut);
		if (*psmf != ksmfContinue)
			return 0;
		break;
	case kopPushISlotAttr:
		slat = SlotAttrName(Uint8(*pbNext++));
		nSlotRef = Int8(*pbNext++);
		nIndex = Uint8(*pbNext++);
		if (fConstraints)
			nSlotRef += islot + 1;	// +1 to peek ahead to a slot we haven't "got" yet
		DoPushSlotAttr(ptman, nSlotRef, *pfInserting, slat, nIndex, vnStack, psstrmIn, psstrmOut);
		if (*psmf != ksmfContinue)
			return 0;
		break;

	case kopPushGlyphAttrObs:
	case kopPushAttToGAttrObs:
	case kopPushGlyphAttr:
	case kopPushAttToGlyphAttr:
		if (op == kopPushGlyphAttrObs || op == kopPushAttToGAttrObs)
            nGlyphAttr = Uint8(*pbNext++);
		else {
			nGlyphAttr = (Uint8(*pbNext++) << 8);
			nGlyphAttr += Uint8(*pbNext++);
		}
		nSlotRef = Int8(*pbNext++);
		if (fConstraints)
			nSlotRef += islot + 1;	// +1 to peek ahead to a slot we haven't "got" yet
		if (op == kopPushAttToGlyphAttr || op == kopPushAttToGAttrObs)
			DoPushAttToGlyphAttr(ptman, nSlotRef, *pfInserting, nGlyphAttr,
				vnStack, psstrmIn, psstrmOut);
		else
			DoPushGlyphAttr(ptman, nSlotRef, *pfInserting, nGlyphAttr, vnStack,
				psstrmIn, psstrmOut);
		if (*psmf != ksmfContinue)
			return 0;
		break;
	case kopPushGlyphMetric:
	case kopPushAttToGlyphMetric:
		nGlyphAttr = Uint8(*pbNext++);
		nSlotRef = Int8(*pbNext++);
		nAttLevel = Int8(*pbNext++);
		if (fConstraints)
			nSlotRef += islot + 1;	// +1 to peek ahead to a slot we haven't "got" yet
		if (op == kopPushAttToGlyphMetric)
			DoPushAttToGlyphMetric(ptman, nSlotRef, *pfInserting, nGlyphAttr, nAttLevel,
				vnStack, psstrmIn, psstrmOut);
		else
			DoPushGlyphMetric(ptman, nSlotRef, *pfInserting, nGlyphAttr, nAttLevel,
				vnStack, psstrmIn, psstrmOut);
		if (*psmf != ksmfContinue)
			return 0;
		break;
	case kopPushFeat:
		nFeat = Uint8(*pbNext++);
		nSlotRef = Int8(*pbNext++);
		if (fConstraints)
			nSlotRef += islot + 1;	// +1 to peek ahead to a slot we haven't "got" yet
//		else
//			nSlotRef = 0;
		DoPushFeatValue(ptman, nSlotRef, *pfInserting, nFeat, vnStack, psstrmIn, psstrmOut);
		if (*psmf != ksmfContinue)
			return 0;
		break;
	case kopPushProcState:
		nPState = Uint8(*pbNext++);
		gAssert(fConstraints);
		DoPushProcState(ptman, nPState, vnStack);
		break;
	case kopPushVersion:
		vnStack.push_back(kRuleVersion);
		break;
	case kopPopRet:
		if ((*psmf = CheckStack(vnStack, 1)) != ksmfContinue)
			return 0;
		nRet = vnStack.back();
		vnStack.pop_back();
		if (vnStack.size() != 0)
		{
			gAssert(false);
			*psmf = ksmfStackNotEmptied;
		}
		else
			*psmf = ksmfDone;
		break;
	case kopRetZero:
		nRet = 0;
		if (vnStack.size() != 0)
		{
			gAssert(false);
			*psmf = ksmfStackNotEmptied;
		}
		else
			*psmf = ksmfDone;
		break;
	case kopRetTrue:
		nRet = 1;
		if (vnStack.size() != 0)
		{
			gAssert(false);
			*psmf = ksmfStackNotEmptied;
		}
		else
			*psmf = ksmfDone;
		break;

	default:
		gAssert(false);
	}

	*ppbArg = pbNext;
	return nRet;
}

/*----------------------------------------------------------------------------------------------
	Perform arithmetic functions that take two arguments.
----------------------------------------------------------------------------------------------*/
void GrPass::DoStackArithmetic2Args(ActionCommand op, std::vector<int> & vnStack,
	StackMachineFlag * psmf)
{
	if ((*psmf = CheckStack(vnStack, 2)) != ksmfContinue)
		return;

	int nArg2 = vnStack.back();
	vnStack.pop_back();
	int nArg1 = vnStack.back();
	vnStack.pop_back();

	int nResult;

	switch (op)
	{
	case kopAdd:	nResult = nArg1 + nArg2;						break;
	case kopSub:	nResult = nArg1 - nArg2;						break;
	case kopMul:	nResult = nArg1 * nArg2;						break;
	case kopDiv:	nResult = nArg1 / nArg2;						break;
	case kopMin:	nResult = min(nArg1, nArg2);					break;
	case kopMax:	nResult = max(nArg1, nArg2);					break;
	case kopAnd:	nResult = (nArg1 != 0 && nArg2 != 0)? 1: 0;		break;
	case kopOr:		nResult = (nArg1 != 0 || nArg2 != 0)? 1: 0;		break;
	case kopEqual:	nResult = (nArg1 == nArg2)? 1: 0;				break;
	case kopNotEq:	nResult = (nArg1 != nArg2)? 1: 0;				break;
	case kopLess:	nResult = (nArg1 <  nArg2)? 1: 0;				break;
	case kopLessEq:	nResult = (nArg1 <= nArg2)? 1: 0;				break;
	case kopGtr:	nResult = (nArg1 >  nArg2)? 1: 0;				break;
	case kopGtrEq:	nResult = (nArg1 >= nArg2)? 1: 0;				break;
	default:
		Assert(false);
	}

	vnStack.push_back(nResult);
}

/*----------------------------------------------------------------------------------------------
	Perform arithmetic functions that take one argument.
----------------------------------------------------------------------------------------------*/
void GrPass::DoStackArithmetic1Arg(ActionCommand op, std::vector<int> & vnStack,
	StackMachineFlag * psmf)
{
	if ((*psmf = CheckStack(vnStack, 1)) != ksmfContinue)
		return;

	int nArg = vnStack.back();
	vnStack.pop_back();
	int nResult;	

	switch (op)
	{
	case kopNeg:		nResult = nArg * -1;			break;
	case kopTrunc8:		nResult = nArg & 0xFF;			break;
	case kopTrunc16:	nResult = nArg & 0xFFFF;		break;
	case kopNot:		nResult = (nArg == 0)? 1: 0;	break;
	default:
		Assert(false);
	}

	vnStack.push_back(nResult);
}

/*----------------------------------------------------------------------------------------------
	Perform the conditional statement.
----------------------------------------------------------------------------------------------*/
void GrPass::DoConditional(std::vector<int> & vnStack, StackMachineFlag * psmf)
{
	if ((*psmf = CheckStack(vnStack, 3)) != ksmfContinue)
		return;

	int nArg3 = vnStack.back();
	vnStack.pop_back();
	int nArg2 = vnStack.back();
	vnStack.pop_back();
	int nArg1 = vnStack.back();
	vnStack.pop_back();

	if (nArg1 == 0)
		vnStack.push_back(nArg3);
	else
		vnStack.push_back(nArg2);
}

/*----------------------------------------------------------------------------------------------
	Check to make sure the stack has the appropriate number of items on it; if not,
	return a stack error flag.
----------------------------------------------------------------------------------------------*/
GrPass::StackMachineFlag GrPass::CheckStack(std::vector<int> & vnStack, int cn)
{
	if (signed(vnStack.size()) < cn)
		return ksmfUnderflow;
	else
		return ksmfContinue;
}

/*----------------------------------------------------------------------------------------------
	We are finished processing a slot; go on to the next slot, or possibly go backwards.
----------------------------------------------------------------------------------------------*/
void GrPass::DoNext(GrTableManager * /*ptman*/,
#ifdef NDEBUG
    int,
#else
	int cslot, 
#endif
    GrSlotStream * /*psstrmIn*/, GrSlotStream * /*psstrmOut*/)
{
	gAssert(cslot == 1);	// for now anyway
}

/*----------------------------------------------------------------------------------------------
	Set the next glyph to be the given class. There should only be one member of the class,
	but if there happens to be more than one, just use the first member.
	All the slot attributes remain unchanged from the current input slot; associations are
	neutralized.

	@param ptman				- table manager, for generating new slots
	@param fInserting			- true if this slot is being inserted, not copied
	@param nReplacementClass	- class from which to take replacement glyph (corresponding to 
									selector slot's glyph's index in selector class)
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutGlyph(GrTableManager * ptman, bool fInserting, int nReplacementClass,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	EngineState * pengst = ptman->State();

	// If we are exactly at the segment boundary, pass the information on to the output stream.
	// Note that inserted items always go outside the segment, if they are at the boundary
	// (you DON'T want them between the segment boundary and the LB slot!).
	bool fSetSegMin = psstrmIn->AtSegMin() && !fInserting;
	bool fSetSegLim = psstrmIn->AtSegLim();

	//	Slot to copy features and text properties from:
	GrSlotState * pslotNextInput;
	if (psstrmIn->AtEndOfContext())
	{
		gAssert(fInserting);
		pslotNextInput = psstrmIn->RuleInputSlot(0, psstrmOut);
	}
	else
		pslotNextInput = (fInserting)? psstrmIn->Peek(): psstrmIn->NextGet();

	gid16 nGlyphReplacement = ptman->GetClassGlyphIDAt(nReplacementClass, 0);

	GrSlotState * pslotNew;
	if (fInserting)
	{
		pengst->NewSlot(nGlyphReplacement, pslotNextInput, m_ipass, &pslotNew);
		// leave associations empty; eventually they will be "neutralized"
	}
	else
	{
		pengst->NewSlotCopy(pslotNextInput, m_ipass, &pslotNew);
		pslotNew->SetGlyphID(nGlyphReplacement);
		ptman->SetSlotAttrsFromGlyphAttrs(pslotNew);
	}

	if (fSetSegMin)
		psstrmOut->SetSegMinToWritePos(false);
	if (fSetSegLim)
		psstrmOut->SetSegLimToWritePos(false);
	//gid16 chw; chw = pslotNew->GlyphID();
	psstrmOut->NextPut(pslotNew);
}

/*----------------------------------------------------------------------------------------------
	Copy a glyph from the given slot in the input to the output.  All the associations and slot
	attributes are copied from the specified slot as well. Consume a slot from the input
	(which may or may not be the slot we're copying).

	@param ptman				- table manager, for generating new slots
	@param fInserting			- true if this slot is being inserted, not copied
	@param cslotCopyFrom		- slot to copy from
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutCopy(GrTableManager * ptman, bool fInserting, int cslotCopyFrom,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	EngineState * pengst = ptman->State();

	// If we are exactly at the segment boundary, pass the information on to the output stream.
	// Note that inserted items always go outside the segment, if they are at the boundary.
	bool fSetSegMin = psstrmIn->AtSegMin() && !fInserting;
	bool fSetSegLim = psstrmIn->AtSegLim();

	if (!fInserting)
	{
		gAssert(!psstrmIn->AtEndOfContext());
		//	Absorb next slot
		psstrmIn->NextGet();
	}

	GrSlotState * pslotCopyFrom = psstrmIn->RuleInputSlot(cslotCopyFrom, psstrmOut);

	GrSlotState * pslotNew;
	pengst->NewSlotCopy(pslotCopyFrom, m_ipass, &pslotNew);

	if (fSetSegMin)
		psstrmOut->SetSegMinToWritePos(false);
	if (fSetSegLim)
		psstrmOut->SetSegLimToWritePos(false);
	//gid16 chw; chw = pslotNew->GlyphID();
	psstrmOut->NextPut(pslotNew);
}

/*----------------------------------------------------------------------------------------------
	Copy the current slot from the input to the output, substituting the corresponding glyph
	in the output class. All the associations and slot attributes remain unchanged from the
	current input slot.

	@param ptman				- table manager, for generating new slots
	@param fInserting			- true if this slot is being inserted, not copied
	@param cslotSel				- selector slot, relative to current slot
	@param nSelectorClass		- class of selector slot
	@param nReplacementClass	- class from which to take replacement glyph (corresponding to 
									selector slot's glyph's index in selector class)
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutSubs(GrTableManager * ptman, bool fInserting,
	int cslotSel, int nSelClass, int nReplacementClass,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	bool fAtSegMin, fAtSegLim;
	GrSlotState * pslotNextInput;
	DoPutSubsInit(psstrmIn, psstrmOut, fInserting, &pslotNextInput, &fAtSegMin, &fAtSegLim);

	GrSlotState * pslotInSelector = psstrmIn->RuleInputSlot(cslotSel, psstrmOut);

	gid16 gidSelector = pslotInSelector->GlyphID();
	int nSelIndex = ptman->GetIndexInGlyphClass(nSelClass, gidSelector);
	gAssert(nSelIndex != -1);
	gid16 gidReplacement = (nSelIndex == -1)?
		gidSelector:
		ptman->GetClassGlyphIDAt(nReplacementClass, nSelIndex);

	DoPutSubsAux(ptman, fInserting, gidReplacement, psstrmIn, psstrmOut, pslotNextInput,
		fAtSegMin, fAtSegLim);
}

/*----------------------------------------------------------------------------------------------
	Copy the current slot from the input to the output, substituting the corresponding glyph
	in the output class based on 2 input classes. All the associations and slot attributes
	remain unchanged from the current input slot.

	@param ptman				- table manager, for generating new slots
	@param fInserting			- true if this slot is being inserted, not copied
	@param cslotSel				- selector slot, relative to current slot
	@param nSelectorClass		- class of selector slot
	@param nReplacementClass	- class from which to take replacement glyph (corresponding to 
									selector slot's glyph's index in selector class)
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutSubs2(GrTableManager * ptman, bool fInserting,
	int cslotSel1, int nSelClass1, int cslotSel2, int nSelClass2, int nReplacementClass,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	bool fAtSegMin, fAtSegLim;
	GrSlotState * pslotNextInput;
	DoPutSubsInit(psstrmIn, psstrmOut, fInserting, &pslotNextInput, &fAtSegMin, &fAtSegLim);

	GrSlotState * pslotInSelector1 = psstrmIn->RuleInputSlot(cslotSel1, psstrmOut);
	gid16 gidSelector1 = pslotInSelector1->GlyphID();
	int nSelIndex1 = ptman->GetIndexInGlyphClass(nSelClass1, gidSelector1);
	gAssert(nSelIndex1 != -1);
	//size_t cClassLen1 = ptman->NumberOfGlyphsInClass(nSelClass1);

	GrSlotState * pslotInSelector2 = psstrmIn->RuleInputSlot(cslotSel2, psstrmOut);
	gid16 gidSelector2 = pslotInSelector2->GlyphID();
	int nSelIndex2 = ptman->GetIndexInGlyphClass(nSelClass2, gidSelector2);
	gAssert(nSelIndex2 != -1);
	size_t cClassLen2 = ptman->NumberOfGlyphsInClass(nSelClass2);

	int nSelIndex = (nSelIndex1 == -1 || nSelIndex2 == -1) ?
		-1 :
		(nSelIndex1 * static_cast<int>(cClassLen2)) + nSelIndex2;

	gid16 gidReplacement = (nSelIndex == -1)?
		gidSelector1:
		ptman->GetClassGlyphIDAt(nReplacementClass, nSelIndex);

	DoPutSubsAux(ptman, fInserting, gidReplacement, psstrmIn, psstrmOut, pslotNextInput,
		fAtSegMin, fAtSegLim);
}

/*----------------------------------------------------------------------------------------------
	Copy the current slot from the input to the output, substituting the corresponding glyph
	in the output class based on 2 input classes. All the associations and slot attributes
	remain unchanged from the current input slot.

	@param ptman				- table manager, for generating new slots
	@param fInserting			- true if this slot is being inserted, not copied
	@param cslotSel				- selector slot, relative to current slot
	@param nSelectorClass		- class of selector slot
	@param nReplacementClass	- class from which to take replacement glyph (corresponding to 
									selector slot's glyph's index in selector class)
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutSubs3(GrTableManager * ptman, bool fInserting,
	int cslotSel1, int nSelClass1, int cslotSel2, int nSelClass2, int /*cslotSel3*/, int nSelClass3,
	int nReplacementClass,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	bool fAtSegMin, fAtSegLim;
	GrSlotState * pslotNextInput;
	DoPutSubsInit(psstrmIn, psstrmOut, fInserting, &pslotNextInput, &fAtSegMin, &fAtSegLim);

	GrSlotState * pslotInSelector1 = psstrmIn->RuleInputSlot(cslotSel1, psstrmOut);
	gid16 gidSelector1 = pslotInSelector1->GlyphID();
	int nSelIndex1 = ptman->GetIndexInGlyphClass(nSelClass1, gidSelector1);
	gAssert(nSelIndex1 != -1);
	//size_t cClassLen1 = ptman->NumberOfGlyphsInClass(nSelClass1);

	GrSlotState * pslotInSelector2 = psstrmIn->RuleInputSlot(cslotSel2, psstrmOut);
	gid16 gidSelector2 = pslotInSelector2->GlyphID();
	int nSelIndex2 = ptman->GetIndexInGlyphClass(nSelClass2, gidSelector2);
	gAssert(nSelIndex2 != -1);
	size_t cClassLen2 = ptman->NumberOfGlyphsInClass(nSelClass2);

//	GrSlotState * pslotInSelector3 = psstrmIn->RuleInputSlot(cslotSel3, psstrmOut);
//	gid16 gidSelector3 = pslotInSelector3->GlyphID();
	int nSelIndex3 = ptman->GetIndexInGlyphClass(nSelClass3, gidSelector2);
	gAssert(nSelIndex3 != -1);
	size_t cClassLen3 = ptman->NumberOfGlyphsInClass(nSelClass3);

	int nSelIndex = (nSelIndex1 == -1 || nSelIndex2 == -1 || nSelIndex3 == -1) ?
		-1 :
		(((nSelIndex1 * static_cast<int>(cClassLen2)) + nSelIndex2) * static_cast<int>(cClassLen3)) + nSelIndex3;

	gid16 gidReplacement = (nSelIndex == -1)?
		gidSelector1:
		ptman->GetClassGlyphIDAt(nReplacementClass, nSelIndex);

	DoPutSubsAux(ptman, fInserting, gidReplacement, psstrmIn, psstrmOut, pslotNextInput,
		fAtSegMin, fAtSegLim);
}


/*----------------------------------------------------------------------------------------------
	Initial common part of all the DoPutSubs... methods.
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutSubsInit(GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, bool fInserting,
	GrSlotState ** ppslotNextInput, bool * pfAtSegMin, bool * pfAtSegLim)
{
	// Do this before reading from stream.
	*pfAtSegMin = psstrmIn->AtSegMin();
	*pfAtSegLim = psstrmIn->AtSegLim();

	//	Slot to copy features and text properties from:
	if (psstrmIn->AtEndOfContext())
	{
		gAssert(fInserting);
		*ppslotNextInput = psstrmIn->RuleInputSlot(0, psstrmOut);
	}
	else
		*ppslotNextInput = (fInserting)? psstrmIn->Peek(): psstrmIn->NextGet();
}

/*----------------------------------------------------------------------------------------------
	Common part of all the DoPutSubs... methods.
----------------------------------------------------------------------------------------------*/
void GrPass::DoPutSubsAux(GrTableManager * ptman, bool fInserting, gid16 nGlyphReplacement,
	GrSlotStream * /*psstrmIn*/, GrSlotStream * psstrmOut, GrSlotState * pslotNextInput,
	bool fAtSegMin, bool fAtSegLim)
{
	EngineState * pengst = ptman->State();

	// If we are exactly at the segment boundary, pass the information on to the output stream.
	// Note that inserted items always go outside the segment, if they are at the boundary.
	bool fSetSegMin = fAtSegMin && !fInserting;
	bool fSetSegLim = fAtSegLim;

	GrSlotState * pslotNew;
	if (fInserting)
	{
		pengst->NewSlot(nGlyphReplacement, pslotNextInput, m_ipass, &pslotNew);
		// leave associations empty; eventually they will be "neutralized"
	}
	else
	{
		pengst->NewSlotCopy(pslotNextInput, m_ipass, &pslotNew);
		pslotNew->SetGlyphID(nGlyphReplacement);
		ptman->SetSlotAttrsFromGlyphAttrs(pslotNew);
	}

	if (fSetSegMin)
		psstrmOut->SetSegMinToWritePos(false);
	if (fSetSegLim)
		psstrmOut->SetSegLimToWritePos(false);
	//gid16 chw; chw = pslotNew->GlyphID();
	psstrmOut->NextPut(pslotNew);
}

/*----------------------------------------------------------------------------------------------
	A slot has just been inserted. Clear all the associations; eventually (unless we go on
	to set the associations explicitly) we will set its before-assoc to the slot after it
	and its after-assoc to the slot before it. This makes it basically unselectable.
	OBSOLETE - handled by slot initialization code
----------------------------------------------------------------------------------------------*/
void GrPass::SetNeutralAssocs(GrSlotState * pslotNew, GrSlotStream * /*psstrmIn*/)
{
	pslotNew->ClearAssocs();
}

/*----------------------------------------------------------------------------------------------
	Delete the next slot in the input.

	@param psstrmIn			- input stream
	@param psstrmOut		- output stream
----------------------------------------------------------------------------------------------*/
void GrPass::DoDelete(GrTableManager * ptman, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	if (psstrmIn->AtSegMin())
		psstrmOut->SetSegMinToWritePos();
	if (psstrmIn->AtSegLim())
		psstrmOut->SetSegLimToWritePos();

	GrSlotState * pslot = psstrmIn->NextGet();
	//gid16 chw; chw = pslotNew->GlyphID();
	pslot->MarkDeleted();

	if (ptman->LoggingTransduction())
		m_pzpst->RecordDeletionBefore(psstrmOut->WritePos());
}

/*----------------------------------------------------------------------------------------------
	Set the associations for the current slot. Any previous associations will be overwritten.

	@param cn					- number of associations
	@param vnAssoc				- list of associations
	@param fInserting			- whether current slot was inserted
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
void GrPass::DoAssoc(int cnAssocs, std::vector<int> & vnAssocs, bool /*fInserting*/,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	gAssert((unsigned)cnAssocs == vnAssocs.size());

	//	Sort the list of associations. It's okay to use a simple bubble sort
	//	since we expect the list to be very short. Ideally we should remove duplicates
	//	too but I don't think duplicates actually hurt anything.
	for (int i1 = 0; i1 < cnAssocs - 1; i1++)
	{
		for (int i2 = i1 + 1; i2 < cnAssocs; i2++)
		{
			if (vnAssocs[i2] < vnAssocs[i1])
			{
				int nTmp = vnAssocs[i2];
				vnAssocs[i2] = vnAssocs[i1];
				vnAssocs[i1] = nTmp;
			}
		}
	}

	std::vector<GrSlotState *> vpslotAssocs;
	vpslotAssocs.resize(cnAssocs);
	for (int i = 0; i < cnAssocs; i++)
		vpslotAssocs[i] = psstrmIn->RuleInputSlot(vnAssocs[i], psstrmOut);

	GrSlotState * pslot = psstrmOut->RuleOutputSlot();
	pslot->Associate(vpslotAssocs);
}

/*----------------------------------------------------------------------------------------------
	Set a slot attribute for the current slot; the value is on the stack.

	@param op					- command
	@param fInserting			- whether current slot was inserted
	@param slat					- slot attribute to set
	@param slati				- slot attribute index, or -1 for non-indexed attribute
	@param vnStack				- stack to read value from
	@param psstrmIn / Out		- input/output streams
----------------------------------------------------------------------------------------------*/
#ifdef NDEBUG
void GrPass::DoSetAttr(GrTableManager * ptman, ActionCommand op, bool /*fInserting*/,
#else
void GrPass::DoSetAttr(GrTableManager * ptman, ActionCommand op, bool fInserting,
#endif
	SlotAttrName slat, int slati, std::vector<int> & vnStack,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	int nVal = vnStack.back();
	vnStack.pop_back();

	if (slat == kslatUserDefnV1)
		slat = kslatUserDefn;

	if (slati != -1 && slat != kslatCompRef && slat != kslatUserDefn)
	{
		//	Invalid slot attribute index.
		gAssert(false);
		slati = -1;
	}

	if (slati == -1 && (slat == kslatCompRef || slat == kslatUserDefn))
	{
		//	Missing slot attribute index.
		gAssert(false);
		slati = 0;
	}

	if (slat == kslatPosX || slat == kslatPosY)
	{
		//	Doesn't make sense to set the pos attribute.
		gAssert(false);
		return;
	}

	GrSlotState * pslotIn = psstrmIn->RuleInputSlot(0, psstrmOut);

	GrSlotState * pslotComp;

	int nOldVal;

	if (op == kopAttrAdd || op == kopAttrSub || op == kopIAttrAdd || op == kopIAttrSub)
	{
		//	Incrementing or decrementing.

		gAssert(!fInserting);

		switch (slat)
		{
		case kslatAdvX:			nOldVal = pslotIn->AdvanceX(ptman);		break;
		case kslatAdvY:			nOldVal = pslotIn->AdvanceY(ptman);		break;
		case kslatShiftX:		nOldVal = pslotIn->ShiftX();			break;
		case kslatShiftY:		nOldVal = pslotIn->ShiftY();			break;

		case kslatAttAtX:		nOldVal = pslotIn->AttachAtX(ptman, psstrmIn);	break;
		case kslatAttAtY:		nOldVal = pslotIn->AttachAtY();					break;
		case kslatAttAtXoff:	nOldVal = pslotIn->AttachAtXOffset();			break;
		case kslatAttAtYoff:	nOldVal = pslotIn->AttachAtYOffset();			break;

		case kslatAttWithX:		nOldVal = pslotIn->AttachWithX(ptman, psstrmIn);	break;
		case kslatAttWithY:		nOldVal = pslotIn->AttachWithY();					break;
		case kslatAttWithXoff:	nOldVal = pslotIn->AttachWithXOffset();				break;
		case kslatAttWithYoff:	nOldVal = pslotIn->AttachWithYOffset();				break;

		case kslatUserDefn:		nOldVal = pslotIn->UserDefn(slati);		break;

		case kslatMeasureSol:	nOldVal = pslotIn->MeasureSol();	break;
		case kslatMeasureEol:	nOldVal = pslotIn->MeasureEol();	break;

		case kslatJStretch:		nOldVal = pslotIn->JStretch();	break;
		case kslatJShrink:		nOldVal = pslotIn->JShrink();	break;
		case kslatJStep:		nOldVal = pslotIn->JStep();		break;
		case kslatJWeight:		nOldVal = pslotIn->JWeight();	break;
		case kslatJWidth:		nOldVal = pslotIn->JWidth();	break;

		//	These are kind of odd, but maybe.
		case kslatAttLevel:		nOldVal = pslotIn->AttachLevel();		break;
		case kslatBreak:		nOldVal = pslotIn->BreakWeight();		break;
		case kslatDir:			nOldVal = pslotIn->Directionality();	break;

		case kslatSegsplit:		nOldVal = 0;	break;

		default:
			//	Kind of attribute that it makes no sense to increment.
			gAssert(false);
			return;
		}

		if (op == kopAttrAdd || op == kopIAttrAdd)
			nVal = nOldVal + nVal;
		else
		{
			gAssert(op == kopAttrSub || op == kopIAttrSub);
			nVal = nOldVal - nVal;
		}
	}

	GrSlotState * pslotOut = psstrmOut->RuleOutputSlot();

	if (pslotOut->IsLineBreak(ptman->LBGlyphID()))
	{
		switch (slat)
		{
		case kslatAdvX:
		case kslatAdvY:
		case kslatShiftX:
		case kslatShiftY:

		case kslatAttTo:
		case kslatAttLevel:
		case kslatAttAtX:
		case kslatAttAtY:
		case kslatAttAtGpt:
		case kslatAttAtXoff:
		case kslatAttAtYoff:
		case kslatAttWithX:
		case kslatAttWithY:
		case kslatAttWithGpt:
		case kslatAttWithXoff:
		case kslatAttWithYoff:

		case kslatMeasureSol:
		case kslatMeasureEol:

		case kslatJStretch:
		case kslatJShrink:
		case kslatJStep:
		case kslatJWeight:
		case kslatJWidth:
			slat = kslatNoEffect;
			break;
		default:
			; // okay, do nothing
		}
	}

	nVal = GrGlyphSubTable::ConvertValueForVersion(nVal, slat, -1, m_fxdVersion);

	switch (slat)
	{
	case kslatAdvX:			pslotOut->SetAdvanceX(nVal);			break;
	case kslatAdvY:			pslotOut->SetAdvanceY(nVal);			break;
	case kslatShiftX:		pslotOut->SetShiftX(nVal);				break;
	case kslatShiftY:		pslotOut->SetShiftY(nVal);				break;

	case kslatAttTo:		pslotOut->SetAttachTo(nVal);			break;
	case kslatAttLevel:		pslotOut->SetAttachLevel(nVal);			break;
	case kslatAttAtX:		pslotOut->SetAttachAtX(nVal);			break;
	case kslatAttAtY:		pslotOut->SetAttachAtY(nVal);			break;
	case kslatAttAtGpt:		pslotOut->SetAttachAtGpoint(nVal);		break;
	case kslatAttAtXoff:	pslotOut->SetAttachAtXOffset(nVal);		break;
	case kslatAttAtYoff:	pslotOut->SetAttachAtYOffset(nVal);		break;
	case kslatAttWithX:		pslotOut->SetAttachWithX(nVal);			break;
	case kslatAttWithY:		pslotOut->SetAttachWithY(nVal);			break;
	case kslatAttWithGpt:	pslotOut->SetAttachWithGpoint(nVal);	break;
	case kslatAttWithXoff:	pslotOut->SetAttachWithXOffset(nVal);	break;
	case kslatAttWithYoff:	pslotOut->SetAttachWithYOffset(nVal);	break;

	case kslatUserDefn:		pslotOut->SetUserDefn(slati, nVal);		break;

	case kslatCompRef:
		gAssert(nVal - 1 >= psstrmIn->RuleStartReadPos() - psstrmIn->ReadPosForNextGet());
		pslotComp = psstrmIn->RuleInputSlot(nVal, psstrmOut);
		pslotOut->SetCompRefSlot(ptman, slati, pslotComp);		
		break;

	case kslatMeasureSol:	pslotOut->SetMeasureSol(nVal);	break;
	case kslatMeasureEol:	pslotOut->SetMeasureEol(nVal);	break;

	case kslatJStretch:		pslotOut->SetJStretch(nVal);	break;
	case kslatJShrink:		pslotOut->SetJShrink(nVal);		break;
	case kslatJStep:		pslotOut->SetJStep(nVal);		break;
	case kslatJWeight:		pslotOut->SetJWeight(nVal);		break;
	case kslatJWidth:		pslotOut->SetJWidth(nVal);		break;

	case kslatBreak:		pslotOut->SetBreakWeight(nVal);				break;
	case kslatDir:			pslotOut->SetDirectionality(DirCode(nVal)); break;
    case kslatInsert:		pslotOut->SetInsertBefore((nVal != 0)? true : false);		break;

	case kslatSegsplit:		break;	// not handled

	case kslatNoEffect:
		// Do nothing.
		break;

	default:
		//	Kind of attribute that it makes no sense to set.
		gAssert(false);
		return;
	}
}

/*----------------------------------------------------------------------------------------------
	Push value of a slot attribute onto the stack.

	@param nSlotRef				- slot whose attribute we are interested in
	@param fInserting			- whether current slot was inserted
	@param slat					- slot attribute to get
	@param slati				- slot attribute index, or -1 for non-indexed attribute
	@param vnStack				- stack to push onto
	@param psstrmIn				- input stream
----------------------------------------------------------------------------------------------*/
void GrPass::DoPushSlotAttr(GrTableManager * ptman,
	int nSlotRef, bool /*fInserting*/,
	SlotAttrName slat, int slati, std::vector<int> & vnStack,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	GrSlotState * pslot = psstrmIn->RuleInputSlot(nSlotRef, psstrmOut, true);
	if (pslot == NULL)
	{
		// Non-existent pre-context item.
		vnStack.push_back(0);
		return;
	}

	int nVal;
	int xyBogus;

	if (slat == kslatUserDefnV1)
		slat = kslatUserDefn;

	if (slati != -1 && slat != kslatCompRef && slat != kslatUserDefn)
	{
		//	Invalid slot attribute index.
		gAssert(false);
		slati = -1;
	}

	if (slati == -1 && (slat == kslatCompRef || slat == kslatUserDefn))
	{
		//	Missing slot attribute index.
		gAssert(false);
		slati = 0;
	}

	switch (slat)
	{
	case kslatAdvX:			nVal = pslot->AdvanceX(ptman);		break;
	case kslatAdvY:			nVal = pslot->AdvanceY(ptman);		break;
	case kslatShiftX:		nVal = pslot->ShiftX();				break;
	case kslatShiftY:		nVal = pslot->ShiftY();				break;

	case kslatPosX:			pslot->Position(ptman, psstrmIn, &nVal, &xyBogus); 	break;
	case kslatPosY:			pslot->Position(ptman, psstrmIn, &xyBogus, &nVal); 	break;

	case kslatAttTo:		nVal = pslot->AttachTo();			break;
	case kslatAttLevel:		nVal = pslot->AttachLevel();		break;
	case kslatAttAtY:		nVal = pslot->AttachAtY();			break;
	case kslatAttAtGpt:		nVal = pslot->AttachAtGpoint();		break;
	case kslatAttAtXoff:	nVal = pslot->AttachAtXOffset();	break;
	case kslatAttAtYoff:	nVal = pslot->AttachAtYOffset();	break;
	case kslatAttWithY:		nVal = pslot->AttachWithY();		break;
	case kslatAttWithGpt:	nVal = pslot->AttachWithGpoint();	break;
	case kslatAttWithXoff:	nVal = pslot->AttachWithXOffset();	break;
	case kslatAttWithYoff:	nVal = pslot->AttachWithYOffset();	break;
	case kslatAttAtX:		nVal = pslot->AttachAtX(ptman, psstrmIn);	break;
	case kslatAttWithX:		nVal = pslot->AttachWithX(ptman, psstrmIn);	break;

	case kslatMeasureSol:	nVal = pslot->MeasureSol();			break;
	case kslatMeasureEol:	nVal = pslot->MeasureEol();			break;

	case kslatJStretch:		nVal = pslot->JStretch();			break;
	case kslatJShrink:		nVal = pslot->JShrink();			break;
	case kslatJStep:		nVal = pslot->JStep();				break;
	case kslatJWeight:		nVal = pslot->JWeight();			break;
	case kslatJWidth:		nVal = pslot->JWidth();				break;

	case kslatBreak:		nVal = pslot->BreakWeight();		break;
	case kslatDir:			nVal = pslot->Directionality();		break;
	case kslatInsert:		nVal = pslot->InsertBefore();		break;

	case kslatSegsplit:		nVal = 0;							break;	// not used in this engine

	case kslatUserDefn:		nVal = pslot->UserDefn(slati);		break;

	//	Currently no way to look up component.X.ref.

	default:
		//	Invalid attribute.
		gAssert(false);
		nVal = 0;
	}

	vnStack.push_back(nVal);
}

/*----------------------------------------------------------------------------------------------
	Push value of a glyph attribute onto the stack.

	@param nSlotRef				- slot whose attribute we are interested in
	@param fInserting			- whether current slot was inserted
	@param nGlyphAttr			- glyph attribute to get
	@param vnStack				- stack to push onto
	@param psstrmIn				- input stream
----------------------------------------------------------------------------------------------*/
void GrPass::DoPushGlyphAttr(GrTableManager * ptman, int nSlotRef, bool /*fInserting*/,
	int nGlyphAttr,
	std::vector<int> & vnStack, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	GrSlotState * pslot = psstrmIn->RuleInputSlot(nSlotRef, psstrmOut, true);
	if (pslot == NULL)
	{
		// Non-existent pre-context item.
		vnStack.push_back(0);
		return;
	}
	int nVal = pslot->GlyphAttrValueEmUnits(ptman, nGlyphAttr);
	vnStack.push_back(nVal);
}

/*----------------------------------------------------------------------------------------------
	Push value of a glyph attribute onto the stack. The slot of interest is the slot to
	which the current slot is attached.

	@param nSlotRef				- leaf slot
	@param fInserting			- whether current slot was inserted
	@param nGlyphAttr			- glyph attribute to get
	@param vnStack				- stack to push onto
	@param psstrmIn				- input stream
----------------------------------------------------------------------------------------------*/
void GrPass::DoPushAttToGlyphAttr(GrTableManager * ptman, int nSlotRef, bool /*fInserting*/,
	int nGlyphAttr, std::vector<int> & vnStack,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	//	Note that it is the slot in the output stream that has
	//	the relevant attach.to attribute set.
	GrSlotState * pslotLeaf = psstrmOut->RuleOutputSlot(0);
	int srAttTo = pslotLeaf->AttachTo();
	if (srAttTo == 0)
	{
		gAssert(false);
		vnStack.push_back(0);
		return;
	}

	GrSlotState * pslotRoot = psstrmIn->RuleInputSlot(nSlotRef + srAttTo, psstrmOut);
	int nVal = pslotRoot->GlyphAttrValueEmUnits(ptman, nGlyphAttr);
	vnStack.push_back(nVal);
}

/*----------------------------------------------------------------------------------------------
	Push value of a glyph metric onto the stack.

	@param nSlotRef				- slot whose attribute we are interested in
	@param fInserting			- whether current slot was inserted
	@param nGlyphAttr			- glyph attribute to get
	@param nAttLevel			- for accessing composite metrics
	@param vnStack				- stack to push onto
	@param psstrmIn				- input stream
----------------------------------------------------------------------------------------------*/
void GrPass::DoPushGlyphMetric(GrTableManager * ptman, int nSlotRef, bool /*fInserting*/,
	int nGlyphAttr, int nAttLevel,
	std::vector<int> & vnStack, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	GrSlotState * pslot = psstrmIn->RuleInputSlot(nSlotRef, psstrmOut, true);
	if (pslot == NULL)
	{
		// Non-existent pre-context item.
		vnStack.push_back(0);
		return;
	}

	DoPushGlyphMetricAux(ptman, pslot, nGlyphAttr, nAttLevel, vnStack, psstrmIn);
}

/*----------------------------------------------------------------------------------------------
	Push value of a glyph metric onto the stack. The slot of interest is the slot to
	which the current slot is attached.

	@param nSlotRef				- slot whose attribute we are interested in
	@param fInserting			- whether current slot was inserted
	@param nGlyphAttr			- glyph attribute to get
	@param nAttLevel			- for accessing composite metrics
	@param vnStack				- stack to push onto
	@param psstrmIn				- input stream
	@param psstrmOut			- output stream
----------------------------------------------------------------------------------------------*/
void GrPass::DoPushAttToGlyphMetric(GrTableManager * ptman, int nSlotRef, bool /*fInserting*/,
	int nGlyphAttr, int nAttLevel,
	std::vector<int> & vnStack,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	//	Note that it is the slot in the output stream that has
	//	the relevant attach.to attribute set.
	GrSlotState * pslotLeaf = psstrmOut->RuleOutputSlot(0);
	int srAttTo = pslotLeaf->AttachTo();
	if (srAttTo == 0)
	{
		gAssert(false);
		vnStack.push_back(0);
		return;
	}

	GrSlotState * pslot = psstrmIn->RuleInputSlot(nSlotRef + srAttTo, psstrmOut);

	DoPushGlyphMetricAux(ptman, pslot, nGlyphAttr, nAttLevel, vnStack, psstrmIn);
}

/*--------------------------------------------------------------------------------------------*/

void GrPass::DoPushGlyphMetricAux(GrTableManager * ptman,
	GrSlotState * pslot, int nGlyphAttr, int nAttLevel,
	std::vector<int> & vnStack, GrSlotStream * psstrmIn)
{
	int mVal;
	GlyphMetric gmet = GlyphMetric(nGlyphAttr);
	if (nAttLevel == 0 || gmet == kgmetAscent || gmet == kgmetDescent)
	{
		mVal = pslot->GlyphMetricEmUnits(ptman, nGlyphAttr);
	}
	else
	{
		pslot->CalcCompositeMetrics(ptman, psstrmIn, NULL, nAttLevel, true);

		float xy;
		switch (gmet)
		{
		case kgmetLsb:			xy = pslot->ClusterLsb(psstrmIn);		break;
		case kgmetRsb:			xy = pslot->ClusterRsb(psstrmIn);		break;
		case kgmetBbTop:		xy = pslot->ClusterBbTop(psstrmIn);		break;
		case kgmetBbBottom:		xy = pslot->ClusterBbBottom(psstrmIn);	break;
		case kgmetBbLeft:		xy = pslot->ClusterBbLeft(psstrmIn);	break;
		case kgmetBbRight:		xy = pslot->ClusterBbRight(psstrmIn);	break;
		case kgmetBbHeight:		xy = pslot->ClusterBbHeight(psstrmIn);	break;
		case kgmetBbWidth:		xy = pslot->ClusterBbWidth(psstrmIn);	break;
//		case kgmetAdvHeight:	xy = pslot->ClusterAdvHeight(psstrmIn);	break;
		case kgmetAdvWidth:		xy = pslot->ClusterAdvWidth(psstrmIn);	break;
		default:
			gAssert(false);
			xy = 0;
		}
		mVal = ptman->LogToEmUnits(xy);
	}

	vnStack.push_back(mVal);
}

/*----------------------------------------------------------------------------------------------
	Push a feature value onto the stack. For rule actions, the slot of interest is the
	current slot.

	@param nSlotRef				- slot being examined
	@param fInserting			- whether current slot was inserted
	@param nFeat				- feature of interest
	@param vnStack				- stack to push onto
	@param psstrmIn				- input stream
	@param psstrmOut			- output stream
----------------------------------------------------------------------------------------------*/
#ifdef NDEBUG
void GrPass::DoPushFeatValue(GrTableManager * /*ptman*/, int nSlotRef, bool /*fInserting*/,
#else
void GrPass::DoPushFeatValue(GrTableManager * /*ptman*/, int nSlotRef, bool fInserting,
#endif
	int nFeat, std::vector<int> & vnStack, GrSlotStream * psstrmIn, GrSlotStream * psstrmOut)
{
	gAssert(!fInserting);

	GrSlotState * pslot = psstrmIn->RuleInputSlot(nSlotRef, psstrmOut, true);
	if (pslot == NULL)
	{
		// Non-existent pre-context item.
		vnStack.push_back(0);
		return;
	}

	int nVal = pslot->FeatureValue(nFeat);
	vnStack.push_back(nVal);
}

/*----------------------------------------------------------------------------------------------
	Push a processing-state value onto the stack.

	@param nFeat				- feature of interest
	@param vnStack				- stack to push onto
----------------------------------------------------------------------------------------------*/
void GrPass::DoPushProcState(GrTableManager * ptman, int nPState, std::vector<int> & vnStack)
{
	int nValue;
	if (nPState == kpstatJustifyMode)
	{
		// Convert from internal modes to user modes.
		int jmodi = ptman->InternalJustificationMode();
		switch (jmodi)
		{
		case kjmodiNormal:
		case kjmodiCanShrink:
			nValue = kjmoduNormal;
			break;
		case kjmodiMeasure:
			nValue = kjmoduMeasure;
			break;
		case kjmodiJustify:
			nValue = kjmoduJustify;
			break;
		default:
			gAssert(false);
			nValue = kjmoduNormal;
		}
	}
	else if (nPState == kpstatJustifyLevel)
		nValue = 1; // TODO: get justify level from ptman
	vnStack.push_back(nValue);
}

} // namespace gr
