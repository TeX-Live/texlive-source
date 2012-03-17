/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrSlotState.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    GrSlotState class implementation.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"

#ifdef _MSC_VER
#pragma hdrstop
#endif

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
	Initialize slots.
----------------------------------------------------------------------------------------------*/
//	standard for pass 0 slots
void GrSlotState::Initialize(gid16 chw, GrEngine * pgreng,
	GrFeatureValues fval, int ipass, int ichwSegOffset, int nUnicode)
{
	Assert(ipass == 0);
	m_chwGlyphID = chw;
	m_chwActual = kInvalidGlyph;
	m_xysGlyphWidth = kNegInfFloat;
	m_bStyleIndex = byte(fval.m_nStyleIndex);
	u_intslot nullSlot;
	nullSlot.pslot = NULL;
	std::fill_n(PUserDefnBuf(), m_cnUserDefn, nullSlot);
	std::fill_n(PCompRefBuf(), m_cnCompPerLig, nullSlot);
	std::fill_n(PSlatiBuf(), m_cnCompPerLig, nullSlot);
	u_intslot * pFeatBuf = PFeatureBuf();
	for (size_t i = 0; i < m_cnFeat; i++)
		pFeatBuf[i].nValue = fval.m_rgnFValues[i];
	m_ipassFsmCol = -1;
	m_colFsm = -1;

	m_ipassModified = ipass;
	m_ichwSegOffset = ichwSegOffset;
	m_nUnicode = nUnicode;
	m_vpslotAssoc.clear();
	pgreng->InitSlot(this, nUnicode);

	switch (nUnicode)
	{
	case knLRM:	m_spsl = kspslLRM; break;
	case knRLM:	m_spsl = kspslRLM; break;
	case knLRO:	m_spsl = kspslLRO; break;
	case knRLO:	m_spsl = kspslRLO; break;
	case knLRE:	m_spsl = kspslLRE; break;
	case knRLE:	m_spsl = kspslRLE; break;
	case knPDF:	m_spsl = kspslPDF; break;
	default:
		Assert(m_spsl == kspslNone);
		m_spsl = kspslNone;
		break;
	}
}

//	line-break slots
void GrSlotState::Initialize(gid16 chw, GrEngine * pgreng,
	GrSlotState * pslotFeat, int ipass, int ichwSegOffset)
{
	m_chwGlyphID = chw;
	m_chwActual = kInvalidGlyph;
	m_xysGlyphWidth = kNegInfFloat;
	u_intslot nullSlot;
	nullSlot.pslot = NULL;
	std::fill_n(PUserDefnBuf(), m_cnUserDefn, nullSlot);
	std::fill_n(PCompRefBuf(), m_cnCompPerLig, nullSlot);
	std::fill_n(PSlatiBuf(), m_cnCompPerLig, nullSlot);
	CopyFeaturesFrom(pslotFeat);
	m_ipassModified = ipass;
	m_ichwSegOffset = ichwSegOffset;
	m_nUnicode = -1;
	m_vpslotAssoc.clear();
	pgreng->InitSlot(this);
	// Caller is responsible for setting m_spsl.
	m_ipassFsmCol = -1;
	m_colFsm = -1;
}

//	for inserting new slots after pass 0 (under-pos and unicode are irrelevant)
void GrSlotState::Initialize(gid16 chw, GrEngine * pgreng,
	GrSlotState * pslotFeat, int ipass)
{
	m_chwGlyphID = chw;
	m_chwActual = kInvalidGlyph;
	m_xysGlyphWidth = kNegInfFloat;
	u_intslot nullSlot;
	nullSlot.pslot = NULL;
	std::fill_n(PUserDefnBuf(), m_cnUserDefn, nullSlot);
	std::fill_n(PCompRefBuf(), m_cnCompPerLig, nullSlot);
	std::fill_n(PSlatiBuf(), m_cnCompPerLig, nullSlot);
	CopyFeaturesFrom(pslotFeat);
	m_ipassModified = ipass;
	m_ichwSegOffset = kInvalid;
	m_nUnicode = kInvalid;
	m_vpslotAssoc.clear();
	pgreng->InitSlot(this);
	m_spsl = kspslNone;
	m_ipassFsmCol = -1;
	m_colFsm = -1;
}

/*----------------------------------------------------------------------------------------------
	The slot has been modified by the given pass and therefore is in a new state;
	make a new SlotState initialized from the old one.
----------------------------------------------------------------------------------------------*/
void GrSlotState::InitializeFrom(GrSlotState * pslotOld, int ipass)
{
	CopyFrom(pslotOld, false);

	m_ipassModified = ipass;
	m_pslotPrevState = pslotOld;
	m_ichwSegOffset = kInvalid;
	m_vpslotAssoc.clear();
	m_vpslotAssoc.push_back(pslotOld);

	m_dircProc = pslotOld->m_dircProc;
	m_fDirProcessed = pslotOld->m_fDirProcessed;

	// Since we're going on to a new pass, no point in copying these:
	m_ipassFsmCol = -1;	
	m_colFsm = -1;

	////FixAttachmentTree(pslotOld);
}

/*----------------------------------------------------------------------------------------------
	Copy the features and style information from the given slot.
----------------------------------------------------------------------------------------------*/
void GrSlotState::CopyFeaturesFrom(GrSlotState * pslotSrc)
{
	m_bStyleIndex = pslotSrc->m_bStyleIndex;
	Assert(m_cnFeat == pslotSrc->m_cnFeat);
	std::copy(pslotSrc->PFeatureBuf(), pslotSrc->PFeatureBuf() + m_cnFeat, PFeatureBuf());
}

/*----------------------------------------------------------------------------------------------
	Copy the basic information.
	Warning: the functions below will break if GrSlotState and subclasses are given virtual
	methods. In that case, we will need to copy from the address of the first variable in
	GrSlotAbstract.
----------------------------------------------------------------------------------------------*/
void GrSlotState::CopyFrom(GrSlotState * pslot, bool fCopyEverything)
{
	GrSlotAbstract::CopyAbstractFrom(pslot);
	std::copy(pslot->m_prgnVarLenBuf, pslot->m_prgnVarLenBuf + CExtraSpace(), m_prgnVarLenBuf);

	if (fCopyEverything)
	{
		Assert(false);
		m_ipassModified = pslot->m_ipassModified;
		m_pslotPrevState = pslot->m_pslotPrevState;
		m_ichwSegOffset = pslot->m_ichwSegOffset;
		m_colFsm = pslot->m_colFsm;
		m_ipassFsmCol = pslot->m_ipassFsmCol;
		// TODO: copy m_vpslotAssocs.
		m_fNeutralAssocs = pslot->m_fNeutralAssocs;
	}

	m_dislotRootFixed = pslot->m_dislotRootFixed;

	m_vdislotAttLeaves.resize(pslot->m_vdislotAttLeaves.size());
	for (size_t i = 0; i < pslot->m_vdislotAttLeaves.size(); i++)
		m_vdislotAttLeaves[i] = pslot->m_vdislotAttLeaves[i];

	m_islotPosPass = pslot->m_islotPosPass;
	m_nUnicode = pslot->m_nUnicode;
	m_dircProc = pslot->m_dircProc;
	m_fDirProcessed = pslot->m_fDirProcessed;
	m_cnUserDefn = pslot->m_cnUserDefn;
	m_cnFeat = pslot->m_cnFeat;
	m_bStyleIndex = pslot->m_bStyleIndex;
	m_mAdvanceX = pslot->m_mAdvanceX;
	m_mAdvanceY = pslot->m_mAdvanceY;
	m_mShiftX = pslot->m_mShiftX;
	m_mShiftY = pslot->m_mShiftY;
	m_srAttachTo = pslot->m_srAttachTo;
	m_nAttachLevel = pslot->m_nAttachLevel;
	m_mAttachAtX = pslot->m_mAttachAtX;
	m_mAttachAtY = pslot->m_mAttachAtY;
	m_mAttachAtXOffset = pslot->m_mAttachAtXOffset;
	m_mAttachAtYOffset = pslot->m_mAttachAtYOffset;
	m_mAttachWithX = pslot->m_mAttachWithX;
	m_mAttachWithY = pslot->m_mAttachWithY;
	m_mAttachWithXOffset = pslot->m_mAttachWithXOffset;
	m_mAttachWithYOffset = pslot->m_mAttachWithYOffset;
	m_nAttachAtGpoint = pslot->m_nAttachAtGpoint;
	m_nAttachWithGpoint = pslot->m_nAttachWithGpoint;
	m_fAttachMod = pslot->m_fAttachMod;
	m_fShiftMod = pslot->m_fShiftMod;
	m_fIgnoreAdvance = pslot->m_fIgnoreAdvance;
	m_fHasComponents = pslot->m_fHasComponents;

	m_nCompositeLevel = kNegInfinity;	// uncalculated, so don't need to copy the positions??
}

void GrSlotAbstract::CopyAbstractFrom(GrSlotState * pslot)
{
	u_intslot * pnBufSave = m_prgnVarLenBuf;
	*this = *pslot;
	m_prgnVarLenBuf = pnBufSave;
	Assert(m_prgnVarLenBuf);
}

/*----------------------------------------------------------------------------------------------
	Initialize the output slot from the one use within the passes, with the basic information.
	Warning: this function will break if GrSlotState and subclasses are given virtual
	methods. In that case, we will need to copy from the address of the first variable in
	GrSlotAbstract.
----------------------------------------------------------------------------------------------*/
void GrSlotOutput::InitializeOutputFrom(GrSlotState * pslot)
{
	CopyAbstractFrom(pslot);

	// Copy just the component information, which is of length (m_cnCompPerLig * 2)
	//std::copy(pslot->PCompRefBuf(),
	//	pslot->PCompRefBuf() + (m_cnCompPerLig * 2),
	//	this->PCompRefBufSlout());
}

/*----------------------------------------------------------------------------------------------
	Return the ID of the actual glyph that will be used for output and metrics. This is the
	same for most glyphs, but will be different for pseudo-glyphs.
----------------------------------------------------------------------------------------------*/
gid16 GrSlotAbstract::ActualGlyphForOutput(GrTableManager * ptman)
{
	if (m_chwActual == kInvalidGlyph)
		m_chwActual = ptman->ActualGlyphForOutput(m_chwGlyphID);
	return m_chwActual;
}
/*----------------------------------------------------------------------------------------------
	We are replacing the old slot with the recipient. Replace the pointers in any attachment
	root or attached leaf slots.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void GrSlotState::FixAttachmentTree(GrSlotState * /*pslotOld*/)
{
#if 0
	pslotOld->m_vpslotAttLeaves.CopyTo(m_vpslotAttLeaves);
	for (int islot = 0; islot < m_vpslotAttLeaves.Size(); islot++)
	{
		Assert(m_vpslotAttLeaves[islot]->m_pslotAttRoot == pslotOld);
		m_vpslotAttLeaves[islot]->m_pslotAttRoot = this;
	}

	m_pslotAttRoot = pslotOld->m_pslotAttRoot;
	if (m_pslotAttRoot)
	{
		for (int islot = 0; islot < m_pslotAttRoot->m_vpslotAttLeaves.Size(); islot++)
		{
			if (m_pslotAttRoot->m_vpslotAttLeaves[islot] == pslotOld)
			{
				m_pslotAttRoot->m_vpslotAttLeaves.Delete(islot);
				m_pslotAttRoot->m_vpslotAttLeaves.Push(this);
				return;
			}
		}
		Assert(false);	// didn't find old slot in the list
	}
#endif // 0
}

/*----------------------------------------------------------------------------------------------
	Make sure all the values are cached that are needed to be copied to the output slots
----------------------------------------------------------------------------------------------*/
void GrSlotState::EnsureCacheForOutput(GrTableManager * ptman)
{
	// Make sure the actual glyph ID is set.
	gid16 gidActual = ActualGlyphForOutput(ptman);

	// Make sure the glyph metrics are stored.
	Font * pfont = ptman->State()->GetFont();
	if (IsLineBreak(ptman->LBGlyphID()))
	{
		GetGlyphMetric(pfont, kgmetAscent, 0);
		GetGlyphMetric(pfont, kgmetDescent, 0);
		m_xysGlyphX = 0;
		m_xysGlyphY = 0;
		m_xysGlyphHeight = 0;
		m_xysGlyphWidth = 0;
		m_xysAdvX = 0;
		m_xysAdvY = 0;
		m_bIsSpace = true;
	}
	else
	{
		//IsSpace(ptman); // cache this flag--doing bb-top below will do it

		GetGlyphMetric(pfont, kgmetAscent, gidActual);
		GetGlyphMetric(pfont, kgmetDescent, gidActual);
		GetGlyphMetric(pfont, kgmetBbTop, gidActual);
		// call above will also cache all the values below
		//GetGlyphMetric(pfont, kgmetBbBottom, gidActual);
		//GetGlyphMetric(pfont, kgmetBbLeft, gidActual);
		//GetGlyphMetric(pfont, kgmetBbRight, gidActual);
		//GetGlyphMetric(pfont, kgmetAdvWidth, gidActual);
		//GetGlyphMetric(pfont, kgmetAdvHeight, gidActual);
	}
}

/*----------------------------------------------------------------------------------------------
	Set the associations for the slot.
----------------------------------------------------------------------------------------------*/
void GrSlotState::Associate(GrSlotState * pslot)
{
	m_vpslotAssoc.clear();
	m_vpslotAssoc.push_back(pslot);
}

void GrSlotState::Associate(GrSlotState * pslotBefore, GrSlotState * pslotAfter)
{
	m_vpslotAssoc.clear();
	m_vpslotAssoc.push_back(pslotBefore);
	m_vpslotAssoc.push_back(pslotAfter);
}

void GrSlotState::Associate(std::vector<GrSlotState*> & vpslot)
{
	m_vpslotAssoc.clear();
	///vpslot.CopyTo(m_vpslotAssoc);  -- bug in CopyTo, so we do it ourselves:
	for (size_t islot = 0; islot < vpslot.size(); ++islot)
	{
		m_vpslotAssoc.push_back(vpslot[islot]);
	}

	//	Set its character styles and features from the associated slot.
	if (vpslot.size() > 0) // && !m_pslotPrevState
	{
		std::copy(m_vpslotAssoc[0]->PFeatureBuf(),
				  m_vpslotAssoc[0]->PFeatureBuf() + m_cnFeat, PFeatureBuf());
	}
}

/*----------------------------------------------------------------------------------------------
	Clear the associations for the slot.
----------------------------------------------------------------------------------------------*/
void GrSlotState::ClearAssocs()
{
	m_vpslotAssoc.clear();
}

/*----------------------------------------------------------------------------------------------
	Return a list of (ie, add into the vector) all the underlying associations, relative
	to the official beginning of the segment.
----------------------------------------------------------------------------------------------*/
void GrSlotState::AllAssocs(std::vector<int> & vichw)
{
	if (PassModified() == 0)
	{
		Assert(m_ichwSegOffset != kInvalid);
		vichw.push_back(m_ichwSegOffset);
	}
	else
	{
		for (size_t i = 0; i < m_vpslotAssoc.size(); ++i)
			m_vpslotAssoc[i]->AllAssocs(vichw);
	}
}

/*----------------------------------------------------------------------------------------------
	Return the underlying position of the before-association, relative to the official
	beginning of the segment. May be -1, if this slot maps to a character in the previous
	segment, or >= the length of the segment, if it maps to a character in the following
	segment.
----------------------------------------------------------------------------------------------*/
int GrSlotState::BeforeAssoc()
{
	GrSlotState * pslot = this;
	while (pslot->PassModified() > 0)
	{
		pslot = pslot->RawBeforeAssocSlot();
		if (pslot == NULL)
		{
			return kPosInfinity;
		}
	}
	Assert(pslot->m_ichwSegOffset != kInvalid);
	return pslot->m_ichwSegOffset;
}

/*----------------------------------------------------------------------------------------------
	Return the underlying position of the after-association, relative to the official
	beginning of the segment. May be -1, if this slot maps to a character in the previous
	segment, or >= the length of the segment, if it maps to a character in the following
	segment.
----------------------------------------------------------------------------------------------*/
int GrSlotState::AfterAssoc()
{
	GrSlotState * pslot = this;
	while (pslot->PassModified() > 0)
	{
		pslot = pslot->RawAfterAssocSlot();
		if (pslot == NULL)
		{
			return kNegInfinity;
		}
	}
	Assert(pslot->m_ichwSegOffset != kInvalid);
	return pslot->m_ichwSegOffset;
}

/*----------------------------------------------------------------------------------------------
	It is possible to get into a state where we are associated with an invalid state.
	For instance, slot C may be associated with slots B1 and B2, but slot B1 is not associated
	with any earlier slot, in which case slot C should remove the association with B1 and just
	be associated with B2.
----------------------------------------------------------------------------------------------*/
void GrSlotState::CleanUpAssocs()
{
	for (size_t i = 0; i < m_vpslotAssoc.size(); i++)
		m_vpslotAssoc[i]->CleanUpAssocs();

	GrSlotState * pslot;

	pslot = RawBeforeAssocSlot();
	while (pslot && BeforeAssoc() == kPosInfinity)
	{
		//	The before association is bogus--delete it.
		m_vpslotAssoc.erase(m_vpslotAssoc.begin());
		pslot = RawBeforeAssocSlot();
	}

	pslot = RawAfterAssocSlot();
	while (pslot && AfterAssoc() == kNegInfinity)
	{
		//	The after association is bogus--delete it.
		m_vpslotAssoc.pop_back();
		pslot = RawAfterAssocSlot();
	}
}

/*----------------------------------------------------------------------------------------------
	Fill in the array of the given output slot with the underlying positions of the
	ligature components, relative to the official beginning of the segment.
	Positions may be < 0, if this slot maps to a character in the previous segment,
	or >= the length of the segment, if it maps to a character in the following segment.
----------------------------------------------------------------------------------------------*/
void GrSlotState::SetComponentRefsFor(GrSlotOutput * pslout, int slatiArg)
{
	if (PassModified() > 0)
	{
		GrSlotState * pslot;
		int slati;
		if (HasComponents())
		{
			for (int iComponent = 0; iComponent < m_cnCompPerLig; iComponent++)
			{
				pslot = CompRefSlot(iComponent);
				slati = Slati(iComponent);
				if (pslot)
				{
					Assert(slati != -1);
					Assert(PassModified() >= pslot->PassModified());
					pslot->SetComponentRefsFor(pslout, slati);
				}
			}
		}
		else
		{
			// Follow the chain back through the passes.
			for (int islot = 0; islot < AssocsSize(); islot++)
			{
				pslot = m_vpslotAssoc[islot];
				if (pslot)
				{
					Assert(PassModified() >= pslot->PassModified());
					// Passing slati here is definitely needed for our Arabic font: for instance when
					// you type something like "mla", so the l is transformed into a temporary glyph
					// before creating the ligature. However, this seems to have broken something,
					// which I will probably find eventually.  :-(
					pslot->SetComponentRefsFor(pslout, slatiArg);
					//pslot->SetComponentRefsFor(pslout, -1);
				}
			}
		}
	}
	else
	{
		Assert(m_ichwSegOffset != kInvalid);
		pslout->AddComponentReference(m_ichwSegOffset, slatiArg);
	}
}

/*----------------------------------------------------------------------------------------------
	Fill in the vector with the underlying positions of the
	ligature components, relative to the official beginning of the segment.
	Positions may be < 0, if this slot maps to a character in the previous segment,
	or >= the length of the segment, if it maps to a character in the following segment.
	ENHANCE: merge with method above.
----------------------------------------------------------------------------------------------*/
void GrSlotState::AllComponentRefs(std::vector<int> & vichw, std::vector<int> & vicomp,
	int iComponent)
{
	if (PassModified() > 0)
	{
		GrSlotState * pslot;
		if (HasComponents())
		{
			for (int jComponent = 0; jComponent < m_cnCompPerLig; jComponent++)
			{
				pslot = CompRefSlot(jComponent);
				if (pslot)
				{
					Assert(PassModified() >= pslot->PassModified());
					pslot->AllComponentRefs(vichw, vicomp, jComponent);
				}
			}
		}
		else
		{
			for (int islot = 0; islot < AssocsSize(); islot++)
			{
				m_vpslotAssoc[islot]->AllComponentRefs(vichw, vicomp, iComponent);
			}
		}
	}
	else
	{
		Assert(m_ichwSegOffset != kInvalid);
		vichw.push_back(m_ichwSegOffset);
		vicomp.push_back(iComponent);
	}
}

/*----------------------------------------------------------------------------------------------
	Get the value of the component.???.ref attribute for the slot.
	Note that 'i' is the local index for the component as defined for this glyph.
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotState::CompRefSlot(int i)
{
	Assert(i < m_cnCompPerLig);
	if (i < m_cnCompPerLig)
		return CompRef(i);
	else
		return NULL;
}

/*----------------------------------------------------------------------------------------------
	Set the value of the component.???.ref attribute for the slot.
----------------------------------------------------------------------------------------------*/
void GrSlotState::SetCompRefSlot(GrTableManager * ptman, int slati, GrSlotState * pslotComp)
{
	//	Convert the global identifer for the component to the index for this glyph.
	int icomp = ptman->ComponentIndexForGlyph(GlyphID(), slati);
	Assert(icomp != -1);
	if (icomp == -1)
		//	Component not defined for this glyph--ignore.
		return;
	Assert(icomp < m_cnCompPerLig);

	if (m_fHasComponents == false)
	{
		//	None have been set yet; initialize them.
		for (int iLoop = 0; iLoop < m_cnCompPerLig; iLoop++)
		{
			SetCompRef(iLoop, NULL);
			SetSlati(iLoop, -1);
		}
	}

	m_fHasComponents = true;
	int iLoop;
	for (iLoop = 0; iLoop < m_cnCompPerLig; iLoop++)
	{
		if (Slati(iLoop) == slati)
			break;
		if (Slati(iLoop) == -1)
			break;
	}
	//if (icomp < m_cnCompPerLig)
	if (iLoop < m_cnCompPerLig)
	{
		//SetCompRef(icomp, pslotComp);
		SetCompRef(iLoop, pslotComp);
		SetSlati(iLoop, slati);
	}
}

/*----------------------------------------------------------------------------------------------
	Return the underlying position for the slot, relative to the official beginning of the
	segment.
----------------------------------------------------------------------------------------------*/
int GrSlotState::SegOffset()
{
	if (m_ichwSegOffset == kInvalid)
	{
		Assert(m_pslotPrevState);
		Assert(m_ipassModified > 0);
		return m_pslotPrevState->SegOffset();
	}
	else
	{
		Assert(m_pslotPrevState == NULL);
		Assert(m_ipassModified == 0);
		return m_ichwSegOffset;
	}
}

/*----------------------------------------------------------------------------------------------
	Return the value of the glyph attribute (in design units, if this is a measurement).
----------------------------------------------------------------------------------------------*/
int GrSlotState::GlyphAttrValueEmUnits(GrTableManager * ptman, int nAttrID)
{
	return ptman->GlyphAttrValue(m_chwGlyphID, nAttrID);
}

/*----------------------------------------------------------------------------------------------
	Return the value of the glyph metric, in design coordinates (ie, based on the font's
	em-square).
----------------------------------------------------------------------------------------------*/
int GrSlotState::GlyphMetricEmUnits(GrTableManager * ptman, int nMetricID)
{
	int mValue;
	if (ptman->State()->GetFont())
	{
		//	Get the actual metric, possibly adjusted for hinting, then convert back to
		//	design units.
		float xysValue = GlyphMetricLogUnits(ptman, nMetricID);
		mValue = ptman->LogToEmUnits(xysValue);
	}
	else
		//	Ask the font directly.
		////mValue = ???
		mValue = 0;

	return mValue;
}

/*----------------------------------------------------------------------------------------------
	Return the value of the glyph attribute, converted to source device coordinates.
	The attribute is assumed to be one whose value is a measurement.
----------------------------------------------------------------------------------------------*/
float GrSlotState::GlyphAttrValueLogUnits(GrTableManager * ptman, int nAttrID)
{
	int mValue = GlyphAttrValueEmUnits(ptman, nAttrID);
	float xysRet = ptman->EmToLogUnits(mValue);
	return xysRet;
}

/*----------------------------------------------------------------------------------------------
	Return the value of the glyph metric, in the source device's logical units.
----------------------------------------------------------------------------------------------*/
float GrSlotState::GlyphMetricLogUnits(GrTableManager * ptman, int nMetricID)
{
#ifdef _DEBUG
	if (IsLineBreak(ptman->LBGlyphID()))
	{
		Warn("Getting metrics of line-break slot");
	}
#endif // _DEBUG
	if (IsLineBreak(ptman->LBGlyphID()))
	{
		return 0;
	}

	return GetGlyphMetric(ptman->State()->GetFont(), nMetricID,
		ActualGlyphForOutput(ptman));
}


float GrSlotOutput::GlyphMetricLogUnits(Font * pfont, int nMetricID)
{
	Assert(m_chwActual != kInvalidGlyph);
	if (m_chwActual == kInvalidGlyph)
		return 0;

	return GetGlyphMetric(pfont, nMetricID, m_chwActual);
}

//float GrSlotOutput::GlyphMetricLogUnits(int gmet) -- obsolete: no longer caching these values
//{
//	// When the font is not passed as an argument, the values better be cached!
//
//	switch (gmet)
//	{ // There may be off by one errors below, depending on what width and height mean
//	case kgmetLsb:
//		return m_xysGlyphX;
//	case kgmetRsb:
//		return (m_xysAdvX - m_xysGlyphX - m_xysGlyphWidth);
//	case kgmetBbTop:
//		return m_xysGlyphY;
//	case kgmetBbBottom:
//		return (m_xysGlyphY - m_xysGlyphHeight);
//	case kgmetBbLeft:
//		return m_xysGlyphX;
//	case kgmetBbRight:
//		return (m_xysGlyphX + m_xysGlyphWidth);
//	case kgmetBbHeight:
//		return m_xysGlyphHeight;
//	case kgmetBbWidth:
//		return m_xysGlyphWidth;
//	case kgmetAdvWidth:
//		return m_xysAdvX;
//	case kgmetAdvHeight:
//		return m_xysAdvY;
//	default:
//		Warn("GetGlyphMetric was asked for an illegal metric.");
//	};
//
//	return 0;		
//}

float GrSlotState::GetGlyphMetric(Font * pfont, int nMetricID, gid16 chwGlyphID)
{
	GlyphMetric gmet = GlyphMetric(nMetricID);
	switch (gmet)
	{
	case kgmetAscent:
	case kgmetDescent:
		if (m_xysFontAscent == kNegInfFloat)
		{
			m_xysFontAscent = GrSlotAbstract::GetGlyphMetric(pfont, kgmetAscent, chwGlyphID);
			m_xysFontDescent = GrSlotAbstract::GetGlyphMetric(pfont, kgmetDescent, chwGlyphID);
		}
		break;
	default:
		if (m_xysGlyphWidth == kNegInfFloat)
		{
			GetGlyphMetricAux(pfont, chwGlyphID, m_xysGlyphX, m_xysGlyphY, m_xysGlyphWidth,
				m_xysGlyphHeight, m_xysAdvX, m_xysAdvY, m_bIsSpace);
		}
		break;
	}

	switch (gmet)
	{ // There may be off-by-one errors below, depending on what width and height mean
	case kgmetAscent:
		return m_xysFontAscent;
	case kgmetDescent:
		return m_xysFontDescent;
	case kgmetLsb:
		return m_xysGlyphX;
	case kgmetRsb:
		return (m_xysAdvX - m_xysGlyphX - m_xysGlyphWidth);
	case kgmetBbTop:
		return m_xysGlyphY;
	case kgmetBbBottom:
		return (m_xysGlyphY - m_xysGlyphHeight);
	case kgmetBbLeft:
		return m_xysGlyphX;
	case kgmetBbRight:
		return (m_xysGlyphX + m_xysGlyphWidth);
	case kgmetBbHeight:
		return m_xysGlyphHeight;
	case kgmetBbWidth:
		return m_xysGlyphWidth;
	case kgmetAdvWidth:
		return m_xysAdvX;
	case kgmetAdvHeight:
		return m_xysAdvY;
	default:
		Warn("GetGlyphMetric was asked for an illegal metric.");
	};

	return 0;
}

float GrSlotAbstract::GetGlyphMetric(Font * pfont, int nMetricID, gid16 chwGlyphID)
{
	GlyphMetric gmet = GlyphMetric(nMetricID);

	float yAscent, yDescent;

	if (kgmetAscent == gmet)
	{
		//if (m_xysFontAscent != -1)
		//	return m_xysFontAscent;
		pfont->getFontMetrics(&yAscent);
		//m_xysFontAscent = xysRet;
		//if (pfont)
		//	m_xysFontAscent = yAscent;
		return yAscent;
	}

	if (kgmetDescent == gmet)
	{
		//if (m_xysFontDescent != -1)
		//	return m_xysFontDescent;
		pfont->getFontMetrics(NULL, &yDescent);
		//m_xysFontDescent = xysRet;
		//if (pfont)
		//	m_xysFontDescent = yDescent;
		return yDescent;
	}

	float xysGlyphX, xysGlyphY, xysGlyphWidth, xysGlyphHeight, xysAdvX, xysAdvY;
	GetGlyphMetricAux(pfont, chwGlyphID, xysGlyphX, xysGlyphY, xysGlyphWidth,
		xysGlyphHeight, xysAdvX, xysAdvY, m_bIsSpace);

	switch (gmet)
	{ // There may be off-by-one errors below, depending on what width and height mean
	case kgmetLsb:
		return xysGlyphX;
	case kgmetRsb:
		return (xysAdvX - xysGlyphX - xysGlyphWidth);
	case kgmetBbTop:
		return xysGlyphY;
	case kgmetBbBottom:
		return (xysGlyphY - xysGlyphHeight);
	case kgmetBbLeft:
		return xysGlyphX;
	case kgmetBbRight:
		return (xysGlyphX + xysGlyphWidth);
	case kgmetBbHeight:
		return xysGlyphHeight;
	case kgmetBbWidth:
		return xysGlyphWidth;
	case kgmetAdvWidth:
		return xysAdvX;
	case kgmetAdvHeight:
		return xysAdvY;
	default:
		Warn("GetGlyphMetric was asked for an illegal metric.");
	};

	return 0;		
}

void GrSlotAbstract::GetGlyphMetricAux(Font * pfont, gid16 chwGlyphID,
	float & xysGlyphX, float & xysGlyphY,
	float & xysGlyphWidth, float & xysGlyphHeight, float & xysAdvX, float & xysAdvY, sdata8 & bIsSpace)
{
	gr::Point ptAdvances;
	gr::Rect rectBb;
	pfont->getGlyphMetrics(chwGlyphID, rectBb, ptAdvances);

	xysGlyphX = rectBb.left;
	xysGlyphY = rectBb.top;
	xysGlyphWidth = (rectBb.right - rectBb.left);
	xysGlyphHeight = (rectBb.top - rectBb.bottom);
	xysAdvX = ptAdvances.x;
	xysAdvY = ptAdvances.y;

	bIsSpace = (0 == xysGlyphX && 0 == xysGlyphY); // should agree with test done in IsSpace() below

	if (bIsSpace == 1)
	{
		// White space glyph - only case where nGlyphX == nGlyphY == 0
		// nGlyphWidth & nGlyphHeight are always set to 16 for unknown reasons, so correct.
		xysGlyphWidth = xysGlyphHeight = 0; 
	}
}

/*----------------------------------------------------------------------------------------------
	Test the glyph id to see if it is a white space glyph.
----------------------------------------------------------------------------------------------*/
sdata8 GrSlotState::IsSpace(GrTableManager * ptman)
{
	gid16 gidActual = ActualGlyphForOutput(ptman);

	////if (m_xysGlyphWidth == -1)
	////{
	////	res = ptman->State()->GraphicsObject()->GetGlyphMetrics(gidActual,
	////		&m_xysGlyphWidth, &m_xysGlyphHeight,
	////		&m_xysGlyphX, &m_xysGlyphY, &m_xysAdvX, &m_xysAdvY);
	////	if (ResultFailed(res)) 
	////	{
	////		WARN(res);
	////		return 2; // will test as true but can be distinguished by separate value
	////	}

	////	gr::Point ptAdvances;
	////	gr::Rect rectBb;
	////	ptman->State()->Font()->getGlyphMetrics(gidActual, rectBb, ptAdvances);
	////}

	if (m_bIsSpace == -1)
		GetGlyphMetric(ptman->State()->GetFont(), kgmetBbLeft, gidActual);
	// One call is enough to cache the information.
	//GetGlyphMetric(ptman->State()->Font(), kgmetBbBottom, gidActual);

	// should agree with test done in GetGlyphMetric() above
	//////m_bIsSpace = (0 == m_xysGlyphX && 0 == m_xysGlyphY);

	Assert(m_bIsSpace == 0 || m_bIsSpace == 1);

	return m_bIsSpace;
}

bool GrSlotOutput::IsSpace()
{
	Assert(m_bIsSpace == 0 || m_bIsSpace == 1);
    return (m_bIsSpace != 0)? true : false;
}

/*----------------------------------------------------------------------------------------------
	If any of the attach attributes have been modified, fix things up. Set up the attachment
	tree, and set any needed default positions. Zap the cached positions of any following
	glyphs in the stream.

	@param psstrm			- the stream in which the modifications were made
	@param islotThis		- the index of the attached (leaf) slot in the stream;
								-1 if we don't know
----------------------------------------------------------------------------------------------*/
void GrSlotState::HandleModifiedPosition(GrTableManager * ptman,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islotThis)
{
	if (!m_fAttachMod && !m_fShiftMod)
		return;

	if (islotThis == -1)
	{
		for (int islot = 0; islot < psstrmOut->WritePos(); islot++)
		{
			if (psstrmOut->SlotAt(islot) == this)
			{
				islotThis = islot;
				break;
			}
		}
		Assert(islotThis > -1);
	}

	if (m_fAttachMod)
	{
		//	Attachments.

		//	Note that it doesn't make sense to attach a slot to itself, so if the value of
		//	the attach.to attribute is 0, that means clear the attachment.
		GrSlotState * pslotNewRoot = AttachRoot(psstrmOut);

		AttachToRoot(ptman, psstrmOut, pslotNewRoot);

		if (pslotNewRoot)
		{
			//	If this is an attachment with no positions, attach the two glyphs side by side
			//	in the appropriate order.
			bool fRtl = ((pslotNewRoot->PostBidiDirLevel(ptman) % 2) != 0);
			if ((fRtl && m_srAttachTo < 0) || (!fRtl && m_srAttachTo > 0))
			{
				//	The root is on the right, the leaf is on the left.
				if (m_mAttachAtX == kNotYetSet && m_nAttachAtGpoint == kNotYetSet)
					m_mAttachAtX = 0;
				if (m_mAttachWithX == kNotYetSet && m_nAttachWithGpoint == kNotYetSet)
					m_mAttachWithX = short(AdvanceX(ptman));
			}
			else
			{
				//	The root is on the left, the leaf is on the right.
				if (m_mAttachAtX == kNotYetSet && m_nAttachAtGpoint == kNotYetSet)
					m_mAttachAtX = short(pslotNewRoot->AdvanceX(ptman));
				if (m_mAttachWithX == kNotYetSet && m_nAttachWithGpoint == kNotYetSet)
					m_mAttachWithX = 0;
			}
		}
	}
	else
	{
		// Shifting, or changing the advance width.
		Assert(m_fShiftMod);
		EnsureLocalAttachmentTree(ptman, psstrmIn, psstrmOut, islotThis);
		ZapMetricsAndPositionDownToBase(psstrmOut);
		ZapMetricsOfLeaves(psstrmOut);
	}

	if (psstrmOut->m_ipass == ptman->NumberOfPasses() - 1)
		ptman->InitPosCache();	// cached position is most likely no longer valid

	//	Invalidate the positions of this and any following glyphs.
	for (int islot = islotThis + 1; islot < psstrmOut->WritePos(); islot++)
		psstrmOut->SlotAt(islot)->ZapPosition();

	m_fAttachMod = false;
	m_fShiftMod = false;
}

/*----------------------------------------------------------------------------------------------
	The shift attribute of a slot has been modified. Make sure any slots that are part of the
	same attachment cluster are local to this stream. The reason for this is so that the
	position calculations stay consistent within the stream.
----------------------------------------------------------------------------------------------*/
void GrSlotState::EnsureLocalAttachmentTree(GrTableManager * ptman,
	GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islotThis)
{
	if (m_dislotRootFixed)
	{
		GrSlotState * pslotRoot = SlotAtOffset(psstrmOut, m_dislotRootFixed);
		psstrmOut->EnsureLocalCopy(ptman, pslotRoot, psstrmIn);
		pslotRoot = SlotAtOffset(psstrmOut, m_dislotRootFixed); // get the new one!
		pslotRoot->EnsureLocalAttachmentTree(ptman, psstrmIn, psstrmOut, islotThis + m_dislotRootFixed);
	}
	for (size_t islot = 0; islot < m_vdislotAttLeaves.size(); islot++)
	{
		GrSlotState * pslotLeaf = SlotAtOffset(psstrmOut, m_vdislotAttLeaves[islot]);
		psstrmOut->EnsureLocalCopy(ptman, pslotLeaf, psstrmIn);
	}
}

/*----------------------------------------------------------------------------------------------
	The recipient slot is being attached to the argument slot.
	NOTE: the caller is responsible to zap the cached positions of following glyphs
	in the stream.
----------------------------------------------------------------------------------------------*/
void GrSlotState::AttachToRoot(GrTableManager * /*ptman*/, GrSlotStream * psstrm,
	GrSlotState * pslotNewRoot)
{
	GrSlotState * pslotOldRoot = (m_dislotRootFixed == 0) ?
		NULL :
		SlotAtOffset(psstrm, m_dislotRootFixed);

	if (pslotOldRoot)
	{
		if (pslotOldRoot != pslotNewRoot)
			pslotOldRoot->RemoveLeaf(m_dislotRootFixed);
		pslotOldRoot->ZapMetricsAndPositionDownToBase(psstrm);
		pslotOldRoot->ZapMetricsOfLeaves(psstrm);
	}

	ZapCompositeMetrics();
	
	if (pslotNewRoot && pslotNewRoot != pslotOldRoot)
	{
		pslotNewRoot->AddLeaf(m_srAttachTo);
		pslotNewRoot->ZapMetricsAndPositionDownToBase(psstrm);
		pslotNewRoot->ZapMetricsOfLeaves(psstrm);
	}

	m_dislotRootFixed = m_srAttachTo;
}

/*----------------------------------------------------------------------------------------------
	Return the absolute position of the glyph, relative to the start of the segment,
	in font design units.
----------------------------------------------------------------------------------------------*/
void GrSlotState::Position(GrTableManager * ptman,
	GrSlotStream * psstrmOut, int * pmXPos, int * pmYPos)
{
	Assert(!m_fAttachMod);	// the attachment tree should be set up
	Assert(!m_fShiftMod);

	float xsWidth, xsVisWidth;
	if (m_xsPositionX == kNegInfFloat || m_ysPositionY == kNegInfFloat)
		ptman->CalcPositionsUpTo(psstrmOut->m_ipass, this, true, &xsWidth, &xsVisWidth);

	*pmXPos = ptman->LogToEmUnits(m_xsPositionX);
	*pmYPos = ptman->LogToEmUnits(m_ysPositionY);
}

/*----------------------------------------------------------------------------------------------
	Recalculate the metrics for this slot, which has attachments on it (or possibly
	did in the past).
----------------------------------------------------------------------------------------------*/
void GrSlotState::AdjustRootMetrics(GrTableManager * ptman, GrSlotStream * psstrm)
{
	Assert(m_dislotRootFixed == m_srAttachTo);
	GrSlotState * pslotRoot = AttachRoot(psstrm);
	CalcRootMetrics(ptman, psstrm, NULL, kPosInfinity);
	if (pslotRoot)
		pslotRoot->AdjustRootMetrics(ptman, psstrm);
}

/*----------------------------------------------------------------------------------------------
	Calculate the composite metrics for this slot.

	@param psstrm			- stream for which we are calculating it
	@param psstrmNext		- because when processing in the middle of a pass, we may need to
								get the slot from the following (output) stream
	@param nLevel			- attachment level we are asking for; kPosInifinity means all levels
	@param fThorough		- true: do a thorough recalculation; false: don't recalculate
								metrics for leaves (are they assumed to be accurate???)
								--currently not used
----------------------------------------------------------------------------------------------*/
void GrSlotState::CalcCompositeMetrics(GrTableManager * ptman, GrSlotStream * psstrm,
	GrSlotStream * psstrmNext, int nLevel, bool fThorough)
{
	if (m_nCompositeLevel == nLevel)
		return;

	if (fThorough)
	{
		Assert(m_dislotRootFixed == m_srAttachTo);
		GrSlotState * pslotRoot = AttachRoot(psstrm);
		// Kludge to handle the fact that we might have gotten the root from the wrong stream.
		// Calling MidPassSlotAt finds the right one.
		if (psstrmNext && pslotRoot)
		{
			int islotRoot = pslotRoot->PosPassIndex();
			pslotRoot = psstrm->MidPassSlotAt(islotRoot, psstrmNext);
		}

		InitMetrics(ptman, pslotRoot);

		for (size_t islot = 0; islot < m_vdislotAttLeaves.size(); islot++)
		{
			GrSlotState * pslotLeaf;
			if (psstrmNext)
			{
				// Calculating a position in the middle of processing a pass.
				pslotLeaf = psstrm->MidPassSlotAt(PosPassIndex() + m_vdislotAttLeaves[islot],
					psstrmNext);
			}
			else
			{
				// Calculating the final position.
				pslotLeaf = SlotAtOffset(psstrm, m_vdislotAttLeaves[islot]);
			}

			if (pslotLeaf->AttachLevel() <= nLevel)
				pslotLeaf->CalcCompositeMetrics(ptman, psstrm, psstrmNext, nLevel, fThorough);
			else
				//	this slot will be ignored in the composite metrics
				pslotLeaf->ZapRootMetrics();
		}
		CalcRootMetrics(ptman, psstrm, psstrmNext, nLevel);

		m_nCompositeLevel = nLevel;
	}
	else
	{
		Assert(false);	// for now

		//	Don't bother with the leaves.
		InitRootMetrics(ptman);
	}
}

/*----------------------------------------------------------------------------------------------
	Calculate the metrics for this node and all its leaf nodes.
----------------------------------------------------------------------------------------------*/
void GrSlotState::CalcRootMetrics(GrTableManager * /*ptman*/, GrSlotStream * psstrm,
	GrSlotStream * psstrmNext, int nLevel)
{
	for (size_t idislot = 0; idislot < m_vdislotAttLeaves.size(); idislot++)
	{
		GrSlotState * pslotLeaf = SlotAtOffset(psstrm, m_vdislotAttLeaves[idislot]);
		// Kludge to handle the fact that we might have gotten the leaf from the wrong stream.
		// Calling MidPassSlotAt finds the right one.
		if (psstrmNext)
		{
			int islot = pslotLeaf->PosPassIndex();
			pslotLeaf = psstrm->MidPassSlotAt(islot, psstrmNext);
		}
		if (pslotLeaf->AttachLevel() > nLevel)
			continue;

		m_xsClusterXOffset = min(m_xsClusterXOffset, pslotLeaf->m_xsClusterXOffset);
		if (!pslotLeaf->m_fIgnoreAdvance)
		{
			m_xsClusterAdv = max(
				m_xsClusterAdv,
				pslotLeaf->m_xsClusterAdv + m_xsRootShiftX);
		}
		m_xsClusterBbLeft = min(m_xsClusterBbLeft, pslotLeaf->m_xsClusterBbLeft);
		m_xsClusterBbRight = max(m_xsClusterBbRight, pslotLeaf->m_xsClusterBbRight);
		m_ysClusterBbTop = max(m_ysClusterBbTop, pslotLeaf->m_ysClusterBbTop);
		m_ysClusterBbBottom = min(m_ysClusterBbBottom, pslotLeaf->m_ysClusterBbBottom);
	}
}

/*----------------------------------------------------------------------------------------------
	Reset the cluster metrics of this slot.
----------------------------------------------------------------------------------------------*/
void GrSlotState::InitMetrics(GrTableManager * ptman, GrSlotState * pslotRoot)
{
	InitLeafMetrics(ptman, pslotRoot);
 	InitRootMetrics(ptman);
}

/*----------------------------------------------------------------------------------------------
	Initialize the variables that store the offsets of just this node (ignoring any of its
	leaves) relative to the cluster base.
----------------------------------------------------------------------------------------------*/
void GrSlotState::InitLeafMetrics(GrTableManager * ptman, GrSlotState * pslotRoot)
{
	float xsShiftX = ptman->EmToLogUnits(ShiftX());
	float ysShiftY = ptman->EmToLogUnits(ShiftY());

	if (ptman->RightToLeft())
		xsShiftX *= -1;

	if (IsBase())
	{
		//	The x-value below has to be zero because the shift is incorporated into
		//	m_xsRootShiftX. (m_ysRootShiftY, on the other hand, isn't used anywhere.)
		m_xsOffsetX = 0;
		m_ysOffsetY = ysShiftY;

		m_xsRootShiftX = xsShiftX;
		m_ysRootShiftY = ysShiftY;
		Assert(!IsLineBreak(ptman->LBGlyphID())
			|| (m_xsRootShiftX == 0 && m_ysRootShiftY == 0));
		return;
	}

	Assert(!IsLineBreak(ptman->LBGlyphID()));

	//	Hint-adjusted logical coordinates equivalent to attach.at and attach.with attributes.
	//	If attach.at or attach.with attributes were unset, the defaults for a side-by-side
	//	attachment should have been supplied in HandleModifiedPosition().
	float xsAttAtX, ysAttAtY, xsAttWithX, ysAttWithY;
	AttachLogUnits(ptman, pslotRoot, &xsAttAtX, &ysAttAtY, &xsAttWithX, &ysAttWithY);

	m_xsOffsetX = xsAttAtX - xsAttWithX;
	m_xsOffsetX += pslotRoot->m_xsOffsetX;
	m_xsOffsetX += xsShiftX;

	m_ysOffsetY = ysAttAtY - ysAttWithY;
	m_ysOffsetY += pslotRoot->m_ysOffsetY;
	m_ysOffsetY += ysShiftY;

	// Cumulative effect of shifts on this and all base nodes:
	m_xsRootShiftX = pslotRoot->m_xsRootShiftX + xsShiftX;
	m_ysRootShiftY = pslotRoot->m_ysRootShiftY + ysShiftY;
}

/*----------------------------------------------------------------------------------------------
	Initialize the variables that store the offsets of this node taking into account its
	leaves; these are relative to the cluster base.
----------------------------------------------------------------------------------------------*/
void GrSlotState::InitRootMetrics(GrTableManager * ptman)
{
	if (IsLineBreak(ptman->LBGlyphID()))
	{
		m_fIgnoreAdvance = true;
		m_xsClusterXOffset = 0;
		m_xsClusterAdv = 0;
		m_xsClusterBbLeft = 0;
		m_xsClusterBbRight = 0;
		m_ysClusterBbTop = 0;
		m_ysClusterBbBottom = 0;
		return;
	}

	float xsAdvanceX = ptman->EmToLogUnits(AdvanceX(ptman));

	//	If the glyph's advance width is zero, then we NEVER want it to have any affect,
	//	even if the glyph is attached way out to the right of its base's advance.
	m_fIgnoreAdvance = (xsAdvanceX == 0);

	float xsBbLeft = GlyphMetricLogUnits(ptman, kgmetBbLeft);
	float xsBbRight = GlyphMetricLogUnits(ptman, kgmetBbRight);
	float ysBbTop = GlyphMetricLogUnits(ptman, kgmetBbTop);
	float ysBbBottom = GlyphMetricLogUnits(ptman, kgmetBbBottom);

	//	Any shifts should be ignored for the sake of calculating actual position or width,
	//	hence we subtract the cumulative effect of the shifts.
	m_xsClusterXOffset = m_xsOffsetX - m_xsRootShiftX;
	m_xsClusterAdv = m_xsOffsetX + xsAdvanceX - m_xsRootShiftX;

	m_xsClusterBbLeft = m_xsOffsetX + xsBbLeft;
	m_xsClusterBbRight = m_xsOffsetX + xsBbRight;
	m_ysClusterBbTop = m_ysOffsetY + ysBbTop;
	m_ysClusterBbBottom = m_ysOffsetY + ysBbBottom;
}

/*----------------------------------------------------------------------------------------------
	X-offset of a single glyph relative to the previous advance position.
----------------------------------------------------------------------------------------------*/
float GrSlotState::GlyphXOffset(GrSlotStream * psstrm, float fakeItalicRatio)
{
	float xsRet = Base(psstrm)->ClusterRootOffset() + m_xsOffsetX;

	// fake an italic slant
	xsRet += m_ysOffsetY * fakeItalicRatio;

	return xsRet;
}

/*----------------------------------------------------------------------------------------------
	Y-offsets of a single glyph relative to the previous advance position.
----------------------------------------------------------------------------------------------*/
float GrSlotState::GlyphYOffset(GrSlotStream * /*psstrm*/)
{
	return m_ysOffsetY;
}

/*----------------------------------------------------------------------------------------------
	Return the offset of the last leaf of the cluster, relative to this slot, which is
	the base. Return kNegInfinity if there are not enough slots in the stream to tell.
----------------------------------------------------------------------------------------------*/
int GrSlotState::LastLeafOffset(GrSlotStream * psstrm)
{
	int islotRet = 0;
	for (size_t idislot = 0; idislot < m_vdislotAttLeaves.size(); idislot++)
	{
		int dislot = m_vdislotAttLeaves[idislot];
		Assert(dislot != 0);
		if (!psstrm->HasSlotAtPosPassIndex(PosPassIndex() + dislot))
			return kNegInfinity;
		GrSlotState * pslotLeaf = SlotAtOffset(psstrm, dislot);
		int islotTmp = pslotLeaf->LastLeafOffset(psstrm);
		if (islotTmp == kNegInfinity)
			return kNegInfinity;
		islotRet = max(islotRet, (dislot + islotTmp));
	}
	return islotRet;
}

/*----------------------------------------------------------------------------------------------
	Return the attach positions in the device context's logical units,
	adjusted for hinting if possible.

	Note that m_nAttachAt/WithGpoint = 0 is always an invalid value (resulting from a
	glyph attribute that was defined in terms of x/y coordinates that didn't map to an
	actual on-curve point). Therefore the x/y coordinates should be used to do the
	attachment. In the case where we actually want point #0 on the curve, 
	m_nAttachAt/WithGpoint will have the special value 'kGpointZero' (the glyph attribute
	is defined this way).
----------------------------------------------------------------------------------------------*/
void GrSlotState::AttachLogUnits(GrTableManager * ptman, GrSlotState * pslotRoot,
	float * pxsAttAtX, float * pysAttAtY,
	float * pxsAttWithX, float * pysAttWithY)
{
	if (m_nAttachAtGpoint == kNotYetSet || m_nAttachAtGpoint == 0)
	{
		//	Use x- and y-coordinates; no adjustment for hinting is done.
		int mX = m_mAttachAtX + m_mAttachAtXOffset;
		int mY = m_mAttachAtY + m_mAttachAtYOffset;

		*pxsAttAtX = ptman->EmToLogUnits(mX);
		*pysAttAtY = ptman->EmToLogUnits(mY);
	}
	else
	{
		//	Look up the actual on-curve point.
		int nGpoint = m_nAttachAtGpoint;
		if (nGpoint == kGpointZero)
			nGpoint = 0;
		bool fImpl = ptman->GPointToXY(pslotRoot->GlyphID(), nGpoint, pxsAttAtX, pysAttAtY);

		// Debuggers:
		//int mXTmpAt = m_mAttachAtX + m_mAttachAtXOffset;
		//int mYTmpAt = m_mAttachAtY + m_mAttachAtYOffset;
		//int xsAttAtXTmp = ptman->EmToLogUnits(mXTmpAt);
		//int ysAttAtYTmp = ptman->EmToLogUnits(mYTmpAt);
		//fImpl = false;

		if (!fImpl)
		{
			//	Fall back to using x- and y-coordinates; no adjustment for hinting.
			int mX = m_mAttachAtX + m_mAttachAtXOffset;
			int mY = m_mAttachAtY + m_mAttachAtYOffset;
			*pxsAttAtX = ptman->EmToLogUnits(mX);
			*pysAttAtY = ptman->EmToLogUnits(mY);
		}
		else
		{
			//	Adjust by offsets.
			*pxsAttAtX += ptman->EmToLogUnits(m_mAttachAtXOffset);
			*pysAttAtY += ptman->EmToLogUnits(m_mAttachAtYOffset);
		}
	}

	if (m_nAttachWithGpoint == kNotYetSet || m_nAttachWithGpoint == 0)
	{
		//	Use x- and y-coordinates; no adjustment for hinting is done.
		int mX = m_mAttachWithX + m_mAttachWithXOffset;
		int mY = m_mAttachWithY + m_mAttachWithYOffset;

		*pxsAttWithX = ptman->EmToLogUnits(mX);
		*pysAttWithY = ptman->EmToLogUnits(mY);
	}
	else
	{
		//	Look up the actual on-curve point.
		int nGpoint = m_nAttachWithGpoint;
		if (nGpoint == kGpointZero)
			nGpoint = 0;
		bool fImpl = ptman->GPointToXY(m_chwGlyphID, nGpoint, pxsAttWithX, pysAttWithY);

		// Debuggers:
		//int mXTmpWith = m_mAttachWithX + m_mAttachWithXOffset;
		//int mYTmpWith = m_mAttachWithY + m_mAttachWithYOffset;
		//int xsAttWithXTmp = ptman->EmToLogUnits(mXTmpWith);
		//int ysAttWithYTmp = ptman->EmToLogUnits(mYTmpWith);
		//fImpl = false;

		if (!fImpl)
		{
			//	Fall back to using x- and y-coordinates; no adjustment for hinting.
			int mX = m_mAttachWithX + m_mAttachWithXOffset;
			int mY = m_mAttachWithY + m_mAttachWithYOffset;
			*pxsAttWithX = ptman->EmToLogUnits(mX);
			*pysAttWithY = ptman->EmToLogUnits(mY);
		}
		else
		{
            //	Adjust by offsets.
			*pxsAttWithX += ptman->EmToLogUnits(m_mAttachWithXOffset);
			*pysAttWithY += ptman->EmToLogUnits(m_mAttachWithYOffset);
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Return the slot that is 'dislot' slots away from this slot in the given stream.
	Only valid for streams that are the input to or output of positioning passes.
----------------------------------------------------------------------------------------------*/
GrSlotState * GrSlotState::SlotAtOffset(GrSlotStream * psstrm, int dislot)
{
	Assert(psstrm->m_fUsedByPosPass);
	return psstrm->SlotAtPosPassIndex(PosPassIndex() + dislot);
}

/*----------------------------------------------------------------------------------------------
	If the direction level has not been calculated at this point, assume it is the
	top direction. This should only happen if there was no bidi pass to set it.
	This method should only be called by the positioning passes; it assumes any bidi
	pass has been run.
----------------------------------------------------------------------------------------------*/
int GrSlotState::PostBidiDirLevel(GrTableManager * ptman)
{
	if (m_nDirLevel == -1)
	{
		Assert(!ptman->HasBidiPass());
		return ptman->TopDirectionLevel();
	}
	return m_nDirLevel;
}

/*----------------------------------------------------------------------------------------------
	Return true if this glyph represents a LRM code.
----------------------------------------------------------------------------------------------*/
bool GrSlotState::IsLrm()
{
	if (PassModified() == 0)
		return (m_nUnicode == knLRM);
	return m_pslotPrevState->IsLrm();
}

/*----------------------------------------------------------------------------------------------
	Return true if this glyph represents a RLM code.
----------------------------------------------------------------------------------------------*/
bool GrSlotState::IsRlm()
{
	if (PassModified() == 0)
		return (m_nUnicode == knRLM);
	return m_pslotPrevState->IsRlm();
}


/*----------------------------------------------------------------------------------------------
	Used by GlyphInfo
----------------------------------------------------------------------------------------------*/
//float GrSlotOutput::AdvanceX(Segment * pseg)
//{
//	return pseg->EmToLogUnits(m_mAdvanceX);
//}
//float GrSlotOutput::AdvanceY(Segment * pseg)
//{
//	return pseg->EmToLogUnits(m_mAdvanceY);
//}
float GrSlotOutput::MaxStretch(Segment * pseg, int level)
{
	Assert(level == 0);
	return (level == 0) ? pseg->EmToLogUnits(m_mJStretch0) : 0;
}
float GrSlotOutput::MaxShrink(Segment * pseg, int level)
{
	Assert(level == 0);
	return (level == 0) ? pseg->EmToLogUnits(m_mJShrink0) : 0;
}
float GrSlotOutput::StretchStep(Segment * pseg, int level)
{
	Assert(level == 0);
	return (level == 0) ? pseg->EmToLogUnits(m_mJStep0) : 0;
}
int GrSlotOutput::JustWeight(int level)
{
	Assert(level == 0);
	return (level == 0) ? m_nJWeight0 : 0;
}
float GrSlotOutput::JustWidth(Segment * pseg, int level)
{
	Assert(level == 0);
	return (level == 0) ? pseg->EmToLogUnits(m_mJWidth0) : 0;
}
float GrSlotOutput::MeasureSolLogUnits(Segment * pseg)
{
	return pseg->EmToLogUnits(m_mMeasureSol);
}
float GrSlotOutput::MeasureEolLogUnits(Segment * pseg)
{
	return pseg->EmToLogUnits(m_mMeasureEol);
}

/*----------------------------------------------------------------------------------------------
	Make a copy of the GrSlotOutput. This is used in making an identical copy of the segment.
	Warning: this function will break if GrSlotState and subclasses are given virtual
	methods. In that case, we will need to copy from the address of the first variable in
	GrSlotAbstract.
----------------------------------------------------------------------------------------------*/
void GrSlotOutput::ExactCopyFrom(GrSlotOutput * pslout, u_intslot * pnVarLenBuf, int cnExtraPerSlot)
{
	// The chunk of the object from GrSlotAbstract can be copied exactly,
	// except for the variable-length buffer.
	*this = *pslout;
	m_prgnVarLenBuf = pnVarLenBuf;
	std::copy(pslout->m_prgnVarLenBuf, pslout->m_prgnVarLenBuf + cnExtraPerSlot,
				m_prgnVarLenBuf);

	// Now copy the stuff specific to GrSlotOutput.
	m_ichwBeforeAssoc = pslout->m_ichwBeforeAssoc;
	m_ichwAfterAssoc = pslout->m_ichwAfterAssoc;
	m_cComponents = pslout->m_cComponents;

    m_isloutClusterBase = pslout->m_isloutClusterBase;
	m_disloutCluster = pslout->m_disloutCluster;
	m_xsClusterXOffset = pslout->m_xsClusterXOffset;
	m_xsClusterAdvance = pslout->m_xsClusterAdvance;
	m_igbb = pslout->m_igbb;
	m_xsAdvanceX = pslout->m_xsAdvanceX;
//	m_ysAdvanceY = pslout->m_ysAdvanceY;
//	m_rectBB = pslout->m_rectBB;
}

/*----------------------------------------------------------------------------------------------
	Shift the glyph to the opposite end of the segment. This is needed for white-space-only
	segments whose direction is being changed.
----------------------------------------------------------------------------------------------*/
void GrSlotOutput::ShiftForDirDepthChange(float dxsSegWidth)
{
	float dxsShift = dxsSegWidth - m_xsAdvanceX - (2 * m_xsPositionX);
	int tmp; tmp = (int) dxsShift;
	m_xsPositionX += dxsShift;
//	m_rectBB.left += dxsShift;
//	m_rectBB.right += dxsShift;
}

/*----------------------------------------------------------------------------------------------
	Return the indices of all the glyphs attached to this cluster, in logical order.
	Return an empty vector if this glyph is not the base glyph or if there are no
	attached glyphs.
	This method return glyph indices, not slot indices.
----------------------------------------------------------------------------------------------*/
void GrSlotOutput::ClusterMembers(Segment * pseg, int isloutThis, std::vector<int> & visloutRet)
{
	if (m_isloutClusterBase == -1 || m_isloutClusterBase == isloutThis)	// yes, a base
		pseg->ClusterMembersForGlyph(isloutThis, m_disloutCluster, visloutRet);
	else
	{
		Assert(visloutRet.size() == 0);
	}
}

} // namespace gr
