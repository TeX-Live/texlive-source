/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrSlotState.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Copyright (C) 1999 by SIL International. All rights reserved.

Description:
    Classes GrSlotAbstract, GrSlotState, and GrSlotOutput
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef SLOTSTATE_INCLUDED
#define SLOTSTATE_INCLUDED

namespace gr
{

class GrSlotAbstract;
class GrSlotState;
class GrSlotOutput;
class GrSlotStream;
class Font;

//:End Ignore


/*----------------------------------------------------------------------------------------------
	Subsumes GrSlotState and GrSlotOutput, both of which represent a single glyph
	and its slot attributes and features.
	
	Hungarian: slab
----------------------------------------------------------------------------------------------*/
class GrSlotAbstract
{
	friend class GrSlotStream;
	friend class GlyphInfo;

public:
	//	Constructor:
	GrSlotAbstract()
	{
	}

	virtual ~GrSlotAbstract()
	{
		// the table manager is responsible for destroying the contents of m_prgnVarLenBuf
	}

	void BasicInitializeAbstract(int cnCompPerLig, u_intslot * pnBuf)
	{
		m_dirc = kNotYetSet8;
		m_lb = kNotYetSet8;
		m_fInsertBefore = true;
		m_nDirLevel = -1;

		m_bIsSpace = -1; // unknown

		m_fInsertBefore = true;

		m_mMeasureSol = 0;
		m_mMeasureEol = 0;

		m_mJStretch0 = 0;
		m_mJShrink0 = 0;
		m_mJStep0 = 0;
		m_nJWeight0 = 0;
		m_mJWidth0 = 0;

		m_cnCompPerLig = byte(cnCompPerLig);
		m_prgnVarLenBuf = pnBuf;
	}
	
	void CopyAbstractFrom(GrSlotState * pslot);

	void SetBufferPtr(u_intslot * pn)
	{
		m_prgnVarLenBuf = pn;
	}

	gid16 GlyphID()				{ return m_chwGlyphID; }
	gid16 RawActualGlyph()		{ return m_chwActual; }
	virtual float GetGlyphMetric(Font * pfont, int nGlyphMetricID, gid16 chwGlyphID);

protected:
	void GetGlyphMetricAux(Font * pfont, gid16 chwGlyphID,
		float & xysGlyphX, float & xysGlyphY,
		float & xysGlyphWidth, float & xysGlyphHeight, float & xysAdvX, float & xysAdvY,
		sdata8 & bIsSpace);

public:

	//GrSlotState * CompRefSlot(int i);

	int BreakWeight()
	{
		return (int)m_lb;
	}
	DirCode Directionality()
	{
		return DirCode(int(m_dirc));
	}
	bool InsertBefore()
	{
		return m_fInsertBefore;
	}
	int MeasureSol()
	{
		return m_mMeasureSol;
	}
	int MeasureEol()
	{
		return m_mMeasureEol;
	}

	int DirLevel()
	{
		return m_nDirLevel;
	}

	int SpecialSlotFlag()
	{
		return m_spsl;
	}
	void SetSpecialSlotFlag(int spsl)
	{
		m_spsl = sdata8(spsl);
	}

	enum {
		kNotYetSet = 0x7FFF,
		kNotYetSet8 = 0x7F,
		kInvalidGlyph = 0xFFFF
	};

	gid16 ActualGlyphForOutput(GrTableManager * ptman);

	// Needed for GlyphInfo:
	float XPosition()
	{
		return m_xsPositionX;
	}
	float YPosition()
	{
		return m_ysPositionY;	// relative to baseline (positive is up)
	}

protected:
	gid16 m_chwGlyphID;
	gid16 m_chwActual;		// actual glyph to output (which is a different glyph for pseudos)

	sdata8 m_spsl;			// special slot flag: LB, bidi marker

//	bool m_fInitialLB;		// for LB slots: true if this is the initial LB;
							// false if it is the terminating LB
							// TODO: remove

	sdata8 m_dirc;			// default = kNotYetSet8 (read from glyph attr)
	sdata8 m_lb;			// default = kNotYetSet8 (read from glyph attr)

	sdata8 m_nDirLevel;

	// Slot attributes that are used by GrSlotOutput:
	short	m_mMeasureSol;
	short	m_mMeasureEol;

	unsigned short	m_mJStretch0;
	unsigned short	m_mJShrink0;
	unsigned short	m_mJStep0;
	int	m_mJWidth0;
	byte m_nJWeight0;

	bool m_fInsertBefore;	// default = true

	sdata8 m_bIsSpace;		// 0 = false, 1 = true, -1 = unknown

	byte m_cnCompPerLig;
	// There is a large block managed by either the GrTableManager (for GrSlotState)
	// or the segment (for GrSlotOutput); this variable points at the sub-buffer for this
	// particular slot:
	u_intslot * m_prgnVarLenBuf;	

	float m_xsPositionX;
	float m_ysPositionY;

};	// end of class GrSlotAbstract


/*----------------------------------------------------------------------------------------------
	A GrSlotState represents one slot as modified by a pass in the table.
	Each time a slot state is modified, a new instance is created, copying the relevant
	information.

	Hungarian: slot

	Other hungarian:
		m - integer indicating glyph design units ("em-units")
		sr - integer indicating slot reference
----------------------------------------------------------------------------------------------*/
class GrSlotState : public GrSlotAbstract
{

	friend class GrSlotStream;
	friend class FontMemoryUsage;

public:
	enum { kNeutral = 99 };

	//	Constructors:
	GrSlotState()
		:	GrSlotAbstract(),
			m_pslotPrevState(NULL)
	{
		m_vpslotAssoc.clear();
		//m_fInitialLB = false; // TODO: remove
		m_spsl = kspslNone;
		m_fNeutralAssocs = false;
		m_dircProc = kdircUnknown;
		m_fDirProcessed = false;

		m_vdislotAttLeaves.clear();
		m_fAttachMod = false;
		m_fShiftMod = false;
		m_dislotRootFixed = 0;
		ZapCompositeMetrics();
	}

	virtual ~GrSlotState()
	{
	}

	void BasicInitialize(int cnUserDefn, int cnCompPerLig, int cnFeat, u_intslot * pnBuf)
	{
		BasicInitializeAbstract(cnCompPerLig, pnBuf);

		m_mAdvanceX = kNotYetSet;
		m_mAdvanceY = kNotYetSet;
		m_mShiftX = 0;
		m_mShiftY = 0;

		m_fAdvXSet = false; // for transduction logging
		m_fAdvYSet = false;

		m_srAttachTo = 0;
		m_nAttachLevel = 0;

		m_mAttachAtX = kNotYetSet;
		m_mAttachAtY = 0;
		m_nAttachAtGpoint = kNotYetSet;
		m_mAttachAtXOffset = 0;
		m_mAttachAtYOffset = 0;
		m_mAttachWithX = kNotYetSet;
		m_mAttachWithY = 0;
		m_nAttachWithGpoint = kNotYetSet;
		m_mAttachWithXOffset = 0;
		m_mAttachWithYOffset = 0;

		m_islotPosPass = kNotYetSet;

		m_cnUserDefn = byte(cnUserDefn);
		m_cnFeat = byte(cnFeat);

		m_fHasComponents = false;

		m_xysFontAscent = kNegInfFloat;
		m_xysFontDescent = kNegInfFloat;
		m_xysGlyphWidth = kNegInfFloat;
		m_xysGlyphHeight = kNegInfFloat;
		m_xysGlyphX = kNegInfFloat;
		m_xysGlyphY = kNegInfFloat;
		m_xysAdvX = kNegInfFloat;
		m_xysAdvY = kNegInfFloat;
	}

	void Initialize(gid16 chw, GrEngine *, GrFeatureValues fval,
		int ipass, int ichwSegOffset, int nUnicode = -1);
	void Initialize(gid16 chw, GrEngine *, GrSlotState * pslotFeat,
		int ipass, int ichwSegOffset);
	void Initialize(gid16 chw, GrEngine *, GrSlotState * pslotFeat,
		int ipass);

	void InitializeFrom(GrSlotState * pslot, int ipass);
	void CopyFeaturesFrom(GrSlotState * pslotSrc);
	void FixAttachmentTree(GrSlotState * pslotOld);

	void CopyFrom(GrSlotState * pslot, bool fCopyEverything = true);

	virtual float GetGlyphMetric(Font * pfont, int nGlyphMetricID, gid16 chwGlyphID);

	//	General:
	int RawSegOffset()		{ return m_ichwSegOffset; }

	void SetGlyphID(gid16 chw)
	{
		m_chwGlyphID = chw;
		m_chwActual = kInvalidGlyph;
		m_xysGlyphWidth = kNegInfFloat; // indicate glyph metrics are invalid
		m_ipassFsmCol = -1;
		m_colFsm = -1;
	}
	int PosPassIndex()
	{
		return m_islotPosPass;
	}
#ifdef NDEBUG
	void SetPosPassIndex(int islot, bool)
#else
	void SetPosPassIndex(int islot, bool fInputToPosPass1)
#endif
	{
		// If we're resetting it, it should be to the same value as before:
		Assert(fInputToPosPass1 || m_islotPosPass == kNotYetSet || m_islotPosPass == islot);
		m_islotPosPass = islot;
	}
	void IncPosPassIndex()
	{
		m_islotPosPass++;
	}
	void ZapPosPassIndex()
	{
		m_islotPosPass = kNotYetSet;
	}

	int StyleIndex()				{ return m_bStyleIndex; }

	int AttachTo()				{ return m_srAttachTo; }
	int AttachLevel()			{ return m_nAttachLevel; }

	int RawAttachAtX()			{ return m_mAttachAtX; }
	int AttachAtY()				{ return m_mAttachAtY; }
	int AttachAtGpoint()		{ return m_nAttachAtGpoint; }
	int AttachAtXOffset()		{ return m_mAttachAtXOffset; }
	int AttachAtYOffset()		{ return m_mAttachAtYOffset; }

	int RawAttachWithX()		{ return m_mAttachWithX; }
	int AttachWithY()			{ return m_mAttachWithY; }
	int AttachWithGpoint()		{ return m_nAttachWithGpoint; }
	int AttachWithXOffset()		{ return m_mAttachWithXOffset; }
	int AttachWithYOffset()		{ return m_mAttachWithYOffset; }

	void Associate(GrSlotState *);
	void Associate(GrSlotState *, GrSlotState *);
	void Associate(std::vector<GrSlotState*> &);
	void ClearAssocs();

	int AssocsSize()					{ return m_vpslotAssoc.size(); }
	GrSlotState * RawBeforeAssocSlot()
	{
		if (m_vpslotAssoc.size() == 0)
			return NULL;
		return m_vpslotAssoc[0];
	}
	GrSlotState * RawAfterAssocSlot()
	{
		if (m_vpslotAssoc.size() == 0)
			return NULL;
		return m_vpslotAssoc.back();
	}
	GrSlotState * AssocSlot(int i)		{ return m_vpslotAssoc[i]; }

	void AllAssocs(std::vector<int> & vichw);
	int BeforeAssoc();
	int AfterAssoc();

	void CleanUpAssocs();

	void SetComponentRefsFor(GrSlotOutput *, int iComp = -1);
	void AllComponentRefs(std::vector<int> & vichw, std::vector<int> & vicomp, int iComponent = -1);

	int PassModified()			{ return m_ipassModified; }
	int SegOffset();
	int UnderlyingPos();
	GrSlotState * PrevState()	{ return m_pslotPrevState; }

	void MarkDeleted()	// for now, do nothing
	{
	}

	sdata8 IsSpace(GrTableManager * ptman);

	// TODO: remove argument from these methods; it is no longer needed.
	bool IsLineBreak(gid16 chwLB)
	{
		return (IsInitialLineBreak(chwLB) || IsFinalLineBreak(chwLB));
		//return (m_chwGlyphID == chwLB); // TODO: remove
	}
	bool IsInitialLineBreak(gid16 /*chwLB*/)
	{
		return (m_spsl == kspslLbInitial);
		//return (IsLineBreak(chwLB) && m_fInitialLB == true); // TODO: remove
	}
	bool IsFinalLineBreak(gid16 /*chwLB*/)
	{
		return (m_spsl == kspslLbFinal);
		//return (IsLineBreak(chwLB) && m_fInitialLB == false); // TODO: remove
	}

	bool IsBidiMarker()
	{
		switch (m_spsl)
		{
		case kspslLRM:
		case kspslRLM:
		case kspslLRO:
		case kspslRLO:
		case kspslLRE:
		case kspslRLE:
		case kspslPDF:
			return true;
		default:
			return false;
		}
		return false;
	}

	bool HasComponents()	{ return m_fHasComponents; }

	// Directionality as determined by the bidi algorithm
	DirCode DirProcessed() // return the value
	{
		Assert(m_dirc != kNotYetSet8);
		if (m_dircProc == kdircUnknown)
			m_dircProc = DirCode(m_dirc);
		return m_dircProc;
	}
	void SetDirProcessed(DirCode dirc) // set the directionality
	{
		m_dircProc = dirc;
	}
	bool DirHasBeenProcessed() // has this slot been fully processed?
	{
		return m_fDirProcessed;
	}
	void MarkDirProcessed() // this slot has been fully processed
	{
		m_fDirProcessed = true;
	}

	int RawAdvanceX()			{ return m_mAdvanceX; }
	int RawAdvanceY()			{ return m_mAdvanceY; }
	int ShiftX()				{ return m_mShiftX; }
	int ShiftY()				{ return m_mShiftY; }

	//	Slot attributes that must be calculated:

	int AdvanceX(GrTableManager * ptman)
	{
		if (m_mAdvanceX == kNotYetSet)
			//	Initialize it from the glyph metric (adjusted for hinting).
			m_mAdvanceX = short(GlyphMetricEmUnits(ptman, kgmetAdvWidth));
		return m_mAdvanceX;
	}

	int AdvanceY(GrTableManager * ptman)
	{
		if (m_mAdvanceY == kNotYetSet)
			//	Initialize it from the glyph metric (adjusted for hinting).
			m_mAdvanceY = short(GlyphMetricEmUnits(ptman, kgmetAdvHeight));
		return m_mAdvanceY;
	}

	int AttachAtX(GrTableManager * ptman, GrSlotStream * psstrm)
	{
		if (m_mAttachAtX == kNotYetSet)
		{
			Assert(false);	// Should have already been set in HandleModifiedPosition,
							// but just in case...
			if (m_srAttachTo == 0)
				return 0;
			else
				m_mAttachAtX = short(AttachRoot(psstrm)->AdvanceX(ptman)); // attach on the right
		}
		return m_mAttachAtX;
	}

	int AttachWithX(GrTableManager * /*ptman*/, GrSlotStream * /*psstrm*/)
	{
		if (m_mAttachAtX == kNotYetSet)
		{
			Assert(false);	// Should have already been set in HandleModifiedPosition,
							// but just in case.
			if (!m_srAttachTo == 0)
				return 0;
			else
				m_mAttachAtX = 0; // attach on the right
		}
		return m_mAttachAtX;
	}

	int JStretch()
	{
		return m_mJStretch0;
	}
	int JShrink()
	{
		return m_mJShrink0;
	}
	int JStep()
	{
		return m_mJStep0;
	}
	int JWeight()
	{
		return m_nJWeight0;
	}
	int JWidth()
	{
		return m_mJWidth0;
	}

	//	Slot attribute setters:

	void SetAdvanceX(int mVal)
	{
		Assert(mVal < 0xFFFF); m_mAdvanceX = short(mVal & 0xFFFF); m_fShiftMod = true;
		m_fAdvXSet = true;	// for transduction logging
	}
	void SetAdvanceY(int mVal)
	{
		Assert(mVal < 0xFFFF); m_mAdvanceY = short(mVal & 0xFFFF); m_fShiftMod = true;
		m_fAdvYSet = true;	// for transduction logging
	}
	void SetShiftX(int mVal)
	{
		Assert(mVal < 0xFFFF); m_mShiftX = short(mVal & 0xFFFF);
		m_fShiftMod = true;
	}
	void SetShiftY(int mVal)
	{
		Assert(mVal < 0xFFFF); m_mShiftY = short(mVal & 0xFFFF);
		m_fShiftMod = true;
	}
	
	void SetAttachTo(int srVal)
	{
		Assert(srVal < 0xFFFF);
		m_srAttachTo = short(srVal & 0xFFFF);
		m_fAttachMod = true;
	}
	void SetAttachLevel(int nVal) {
		Assert(nVal < 0xFFFF); m_nAttachLevel = short(nVal & 0xFFFF); m_fAttachMod = true; }

	void SetAttachAtX(int mVal)
	{
		Assert(mVal < 0xFFFF);
		m_mAttachAtX = short(mVal) & 0xFFFF;
		m_fAttachMod = true;
	}
	void SetAttachAtY(int mVal) {
		Assert(mVal < 0xFFFF); m_mAttachAtY = short(mVal & 0xFFFF); m_fAttachMod = true; }
	void SetAttachAtGpoint(int nVal) {
		m_nAttachAtGpoint = short(nVal); m_fAttachMod = true; }
	void SetAttachAtXOffset(int mVal) {
		Assert(mVal < 0xFFFF); m_mAttachAtXOffset = short(mVal & 0xFFFF); m_fAttachMod = true; }
	void SetAttachAtYOffset(int mVal) {
		Assert(mVal < 0xFFFF); m_mAttachAtYOffset = short(mVal & 0xFFFF); m_fAttachMod = true; }

	void SetAttachWithX(int mVal)	{
		Assert(mVal < 0xFFFF); m_mAttachWithX = short(mVal & 0xFFFF); m_fAttachMod = true; }
	void SetAttachWithY(int mVal) {
		Assert(mVal < 0xFFFF); m_mAttachWithY = short(mVal & 0xFFFF); m_fAttachMod = true; }
	void SetAttachWithGpoint(int nVal) {
		m_nAttachWithGpoint = short(nVal); m_fAttachMod = true; }
	void SetAttachWithXOffset(int mVal) {
		Assert(mVal < 0xFFFF); m_mAttachWithXOffset = short(mVal & 0xFFFF); m_fAttachMod = true; }
	void SetAttachWithYOffset(int mVal) {
		Assert(mVal < 0xFFFF); m_mAttachWithYOffset = short(mVal & 0xFFFF); m_fAttachMod = true; }

	void SetCompRefSlot(GrTableManager * ptman, int i, GrSlotState * pslotComp);

	void SetBreakWeight(int lb)				{ m_lb = sdata8(lb); }
	void SetInsertBefore(bool f)			{ m_fInsertBefore = f; }

	void SetDirectionality(DirCode dirc)	{ m_dirc = sdata8(dirc); }
	void SetDirLevel(int n)					{ m_nDirLevel = sdata8(n); }

	void SetMeasureSol(int mVal)			{ m_mMeasureSol = short(mVal); }
	void SetMeasureEol(int mVal)			{ m_mMeasureEol = short(mVal); }

	void SetJStretch(int mVal)				{ m_mJStretch0 = short(mVal); }
	void SetJShrink(int mVal)				{ m_mJShrink0 = short(mVal); }
	void SetJStep(int mVal)					{ m_mJStep0 = short(mVal); }
	void SetJWeight(int nVal)				{ m_nJWeight0 = byte(nVal); }
	void SetJWidth(int mVal)				{ m_mJWidth0 = mVal; }
	void AddJWidthToAdvance(GrTableManager * ptman)
	{
		// Don't change m_fShiftMod.
		m_mAdvanceX = short(m_mJWidth0 + AdvanceX(ptman)); // make sure it is calculated
		m_mJWidth0 = 0;
		m_fAdvXSet = true;	// for transduction logging
	}

	int PostBidiDirLevel(GrTableManager * ptman);

	bool BaseEarlierInStream();

	void ZapDirLevel()
	{
		m_nDirLevel = -1;
		m_dircProc = kdircUnknown;
		m_fDirProcessed = false;
	}

	int GlyphAttrValueEmUnits(GrTableManager * ptman, int nAttrID);
	int GlyphMetricEmUnits(GrTableManager * ptman, int nGlyphMetricID);

	float GlyphAttrValueLogUnits(GrTableManager * ptman, int nAttrID);
	float GlyphMetricLogUnits(GrTableManager * ptman, int nGlyphMetricID);

//	void HandleModifiedCluster(GrTableManager * ptman,
//		GrSlotStream * psstrm, int islotThis);
	void HandleModifiedPosition(GrTableManager * ptman,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islotThis);

	void CalcCompositeMetrics(GrTableManager * ptman, GrSlotStream * psstrm,
		GrSlotStream * psstrmNext, int nLevel, bool fThorough = false);

	void Position(GrTableManager * ptman,
		GrSlotStream * psstrmOut, int * pmXPos, int * pmYPos);

	void ZapPosition()
	{
		m_xsPositionX = kNegInfFloat;
		m_ysPositionY = kNegInfFloat;
	}

	void SetXPos(float xs)
	{
		m_xsPositionX = xs;
	}
	void SetYPos(float ys)
	{
		m_ysPositionY = ys;	// relative to baseline (positive is up)
	}

	float ClusterRootOffset()	{ return -m_xsClusterXOffset; } // the offset of the root
																// relative to the whole cluster

	float ClusterAdvWidthFrom(float xs)	{ return xs + m_xsClusterAdv; }
	float ClusterBbLeftFrom(float xs)	{ return xs + m_xsClusterBbLeft; }
	float ClusterBbRightFrom(float xs)	{ return xs + m_xsClusterBbRight; }

	float ClusterLsb(GrSlotStream * psstrm, float xs)
	{
		return ClusterBbLeft(psstrm) + xs;
	}
	float ClusterRsb(GrSlotStream * /*psstrm*/, float xs)
	{
		return ClusterAdvWidthFrom(xs) - ClusterBbRightFrom(xs);
	}

	float ClusterAdvWidth(GrSlotStream * psstrm)
	{
		return ClusterAdvWidthFrom(Base(psstrm)->ClusterRootOffset());
	}
//	int ClusterAdvHeight(GrSlotStream * psstrm)
//	{
//		Assert(m_mAdvanceY != kNotYetSet);
//		return EmUnitsToTwips(m_mAdvanceY);
//	}
	float ClusterBbLeft(GrSlotStream * psstrm)
	{
		return ClusterBbLeftFrom(Base(psstrm)->ClusterRootOffset());
	}
	float ClusterBbRight(GrSlotStream * psstrm)
	{
		return ClusterBbRightFrom(Base(psstrm)->ClusterRootOffset());
	}
	float ClusterBbTop(GrSlotStream * /*psstrm*/)
	{
		return m_ysClusterBbTop;
	}
	float ClusterBbBottom(GrSlotStream * /*psstrm*/)
	{
		return m_ysClusterBbBottom;
	}
	float ClusterBbWidth(GrSlotStream * /*psstrm*/)
	{
		return m_xsClusterBbRight - m_xsClusterBbLeft + 1;
	}
	float ClusterBbHeight(GrSlotStream * /*psstrm*/)
	{
		return m_ysClusterBbTop - m_ysClusterBbBottom + 1;
	}
	float ClusterLsb(GrSlotStream * psstrm)
	{
		return ClusterBbLeft(psstrm);
	}
	float ClusterRsb(GrSlotStream * psstrm)
	{
		return ClusterAdvWidth(psstrm) - ClusterBbRight(psstrm);
	}

	float GlyphXOffset(GrSlotStream * psstrm, float fakeItalicRatio = 0);
	float GlyphYOffset(GrSlotStream * psstrm = NULL);

	bool IsBase()
	{
		Assert(m_dislotRootFixed == m_srAttachTo);
		return (m_dislotRootFixed == 0);
	}

	bool HasClusterMembers()
	{
		return (m_vdislotAttLeaves.size() > 0);
	}

	GrSlotState * AttachRoot(GrSlotStream * psstrm)
	{
		if (m_srAttachTo == 0)
			return NULL;
		else
			return SlotAtOffset(psstrm, m_srAttachTo);
	}

	int AttachRootPosPassIndex()
	{
		return PosPassIndex() + m_srAttachTo;
	}

	GrSlotState * Base(GrSlotStream * psstrm)
	{
		GrSlotState * pslotRoot = AttachRoot(psstrm);
		if (!pslotRoot)
			return this;
		else
			return pslotRoot->Base(psstrm);
	}

	int LastLeafOffset(GrSlotStream * psstrm);

	void AddLeaf(int dislot)
	{
		m_vdislotAttLeaves.push_back(dislot * -1);
	}
	void RemoveLeaf(int dislot)
	{
		for (size_t iislot = 0; iislot < m_vdislotAttLeaves.size(); iislot++)
		{
			if (m_vdislotAttLeaves[iislot] == dislot * -1)
			{
				m_vdislotAttLeaves.erase(m_vdislotAttLeaves.begin() + iislot);
				return;
			}
		}
		Assert(false);
	}

	bool HasAsRoot(GrSlotStream * psstrm, GrSlotState * pslot)
	{
		GrSlotState * pslotRoot = AttachRoot(psstrm);
		if (pslotRoot == pslot)
			return true;
		else if (pslotRoot == NULL)
			return false;
		else
			return pslotRoot->HasAsRoot(psstrm, pslot);
	}

	bool HasAsPreviousState(GrSlotState * pslot)
	{
		if (this == pslot)
			return true;
		else
			return m_pslotPrevState->HasAsPreviousState(pslot);
	}

	void EnsureCacheForOutput(GrTableManager * ptman);

	bool IsLrm();
	bool IsRlm();

	//	Cache of FSM column information for the most recent pass:
	int FsmColumn() { return m_colFsm; }
	int PassNumberForColumn() { return m_ipassFsmCol; }
	void CacheFsmColumn(int ipass, int col)
	{
		m_colFsm = col;
		m_ipassFsmCol = ipass;
	}

	//	Variable-length buffer--includes four sub-buffers:
	//	* user-defined variables
	//	* component.???.ref assignments
	//	* mapping from components used to global attribute IDs for components
	//	* feature values

	// user-defined slot attributes
	int UserDefn(int slati)
	{
		Assert(slati < m_cnUserDefn);
		return m_prgnVarLenBuf[slati].nValue;
	}
	void SetUserDefn(int slati, int nVal)
	{
		Assert(slati < m_cnUserDefn);
		m_prgnVarLenBuf[slati].nValue = nVal;
	}
	u_intslot * PUserDefnBuf()
	{
		return m_prgnVarLenBuf;
	}

	// pointer to the associated slot which is the value of the comp.ref attribute
	GrSlotState * CompRef(int slati)
	{
		Assert(slati < m_cnCompPerLig);
		return m_prgnVarLenBuf[m_cnUserDefn + slati].pslot;
	}
	void SetCompRef(int slati, GrSlotState * pvSlot)
	{
		Assert(slati < m_cnCompPerLig);
		m_prgnVarLenBuf[m_cnUserDefn + slati].pslot = pvSlot;
	}
	u_intslot * PCompRefBuf()
	{
		return m_prgnVarLenBuf + m_cnUserDefn;
	}

	// global component identifier
	int Slati(int i)
	{
		Assert(i < m_cnCompPerLig);
		return m_prgnVarLenBuf[m_cnUserDefn + m_cnCompPerLig + i].nValue;
	}
	void SetSlati(int i, int n)
	{
		Assert(i < m_cnCompPerLig);
		m_prgnVarLenBuf[m_cnUserDefn + m_cnCompPerLig + i].nValue = n;
	}
	u_intslot * PSlatiBuf()
	{
		return m_prgnVarLenBuf + m_cnUserDefn + m_cnCompPerLig;
	}

	// feature settings
	int FeatureValue(int i)
	{
		Assert(i < m_cnFeat);
		return m_prgnVarLenBuf[m_cnUserDefn + (m_cnCompPerLig * 2) + i].nValue;
	}

	u_intslot * PFeatureBuf()
	{
		return m_prgnVarLenBuf + m_cnUserDefn + (m_cnCompPerLig * 2);
	}

	int CExtraSpace()
	{
		return m_cnUserDefn + (m_cnCompPerLig * 2) + m_cnFeat;
	}

	GrSlotState * CompRefSlot(int i);

	void GetFeatureValues(GrFeatureValues * pfval)
	{
		pfval->m_nStyleIndex = m_bStyleIndex;
		std::fill(pfval->m_rgnFValues, pfval->m_rgnFValues + kMaxFeatures, 0);
		for (size_t i = 0; i < m_cnFeat; i++)
			pfval->m_rgnFValues[i] = PFeatureBuf()[i].nValue;
	}

	// Return true if this slot has all the same features as the argument slot.
	bool SameFeatures(GrSlotState * pslot)
	{
		for (size_t i = 0; i < m_cnFeat; i++)
		{
			if (FeatureValue(i) != pslot->FeatureValue(i))
				return false;
		}
		return true;
	}

//	For transduction logging:
#ifdef TRACING
	void SlotAttrsModified(bool * rgfMods, bool fPreJust, int * pccomp, int * pcassoc);
	void LogSlotAttribute(GrTableManager *, std::ostream &, int ipass, int slat, int icomp,
		bool fPreJust, bool fPostJust);
	void LogAssociation(GrTableManager * ptman,
		std::ostream & strmOut, int ipass, int iassoc, bool fBoth, bool fAfter);
	int m_islotTmpIn;		// for use by transduction log; index of slot in input stream
	int m_islotTmpOut;		// ditto; index of slot in output stream
#endif // TRACING

protected:

	//	Instance variables:
	int m_ipassModified;	// pass in which this slot was modified

	GrSlotState * m_pslotPrevState;

	int	m_ichwSegOffset;	// for original (pass 0) slot states: position in
							// underlying text relative to the official
							// beginning of the segment;
							// should == kInvalid for other slot states
	int m_islotPosPass;		// index of slot in positioning streams, relative to first official
							// slot in the segment (possibly the LB slot)

	int m_colFsm;			// which FSM column this glyph corresponds to...
	int m_ipassFsmCol;		// ...for the most recent pass

	std::vector<GrSlotState*> m_vpslotAssoc;	// association mappings

	bool m_fNeutralAssocs;		// true if we've set the associations to some neutral
								// default, rather than them being set explicitly
								// within a rule

	int m_nUnicode; // for debugging

	DirCode m_dircProc;		// directionality as processed in bidi algorithm
	bool m_fDirProcessed;

	//	affects length of variable-length buffer
	byte m_cnUserDefn;
	byte m_cnFeat;

	byte m_bStyleIndex;

	// Slot attributes:
	short	m_mAdvanceX;
	short	m_mAdvanceY;
	short	m_mShiftX;
	short	m_mShiftY;

	short	m_srAttachTo;
	short	m_nAttachLevel;

	short	m_mAttachAtX;
	short	m_mAttachAtY;
	short	m_mAttachAtXOffset;
	short	m_mAttachAtYOffset;
	short	m_mAttachWithX;
	short	m_mAttachWithY;
	short	m_mAttachWithXOffset;
	short	m_mAttachWithYOffset;

	short	m_nAttachAtGpoint;
	short	m_nAttachWithGpoint;

	// Raw glyph metrics (directly from font)
	float m_xysFontAscent;
	float m_xysFontDescent;
	float m_xysGlyphWidth;
	float m_xysGlyphHeight;
	float m_xysGlyphX;
	float m_xysGlyphY;
	float m_xysAdvX;
	float m_xysAdvY; 

	//	Attachment and metrics

	//	This is a flag that is set whenever we change the value of any of the attach
	//	slot attributes. Then when we want to find out something about the attachments,
	//	if it is set, we have some work to do in updating the pointers and metrics.
	bool m_fAttachMod;

	//	This is a flag that is set whenever we change the value of any of the shift or
	//	advance slot attributes. It forces us to zap the metrics.
	bool m_fShiftMod;

	//	This glyph (and all its leaves, if any) have a zero advance width;
	//	never allow it to affect the advance width of a cluster it is part of.
	bool m_fIgnoreAdvance;

	int m_dislotRootFixed;	// the offset of the slot (relative to this one) that considers
							// this slot to be one of its leaves
	std::vector<int> m_vdislotAttLeaves;

	//	The following are used by the CalcCompositeMetrics() method and depend on
	//	the cluster level that was passed as an argument.

	int m_nCompositeLevel;	// cluster level last used to calculate composite metrics;
							// kNegInfinity if uncalculated

	//	offsets for this node only, relative to cluster base
	float m_xsOffsetX;
	float m_ysOffsetY; // relative to baseline (positive is up)

	//	offsets for this node and its leaves, relative to cluster base (y-coords are
	//	relative to baseline)
	float m_xsClusterXOffset;
	float m_xsClusterAdv;
	float m_xsClusterBbLeft;
	float m_xsClusterBbRight;
	float m_ysClusterBbTop;
	float m_ysClusterBbBottom;

	//	cumulative total of shifts for this node and roots; advance needs to ignore these
	float m_xsRootShiftX;
	float m_ysRootShiftY;

	bool m_fHasComponents;	// default = false

	//	Private methods:
	
	void CopyAbstractFrom(GrSlotState * pslot);

	void AdjustRootMetrics(GrTableManager * ptman, GrSlotStream *);
	void InitMetrics(GrTableManager * ptman, GrSlotState * pslotRoot);
	void InitLeafMetrics(GrTableManager * ptman, GrSlotState * pslotRoot);
	void InitRootMetrics(GrTableManager * ptman);
	void CalcRootMetrics(GrTableManager * ptman, GrSlotStream * psstrm,
		GrSlotStream * psstrmNext, int nLevel);
	void AttachToRoot(GrTableManager * ptman, GrSlotStream *, GrSlotState * pslotNewRoot);
	void AttachLogUnits(GrTableManager * ptman,
		GrSlotState * pslotRoot,
		float * pxsAttAtX, float * pysAttAtY,
		float * pxsAttWithX, float * pysAttWithY);

	GrSlotState * SlotAtOffset(GrSlotStream * psstrm, int dislot);	// ENHANCE SharonC: inline?

	void EnsureLocalAttachmentTree(GrTableManager * ptman,
		GrSlotStream * psstrmIn, GrSlotStream * psstrmOut, int islotThis);

	void ZapCompositeMetrics()
	{
		m_nCompositeLevel = kNegInfinity;
		m_xsPositionX = kNegInfFloat;
		m_ysPositionY = kNegInfFloat;
		m_xsOffsetX = 0;
		m_ysOffsetY = 0;
		m_xsRootShiftX = 0;
		m_ysRootShiftY = 0;
		ZapRootMetrics();
	}

	void ZapRootMetrics()
	{
		m_xsClusterXOffset = 0;
		m_xsClusterAdv = 0;
		m_xsClusterBbLeft = 0;
		m_xsClusterBbRight = 0;
		m_ysClusterBbTop = 0;
		m_ysClusterBbBottom = 0;
		m_fIgnoreAdvance = false;
	}

	void ZapMetricsAndPositionDownToBase(GrSlotStream * psstrm)
	{
		ZapCompositeMetrics();
		if (m_dislotRootFixed)
			SlotAtOffset(psstrm, m_dislotRootFixed)->ZapMetricsAndPositionDownToBase(psstrm);
	}

	void ZapMetricsOfLeaves(GrSlotStream * psstrm, bool fThis = false)
	{
		if (fThis)
			ZapCompositeMetrics();
		for (size_t islot = 0; islot < m_vdislotAttLeaves.size(); islot++)
		{
			SlotAtOffset(psstrm, m_vdislotAttLeaves[islot])->ZapMetricsOfLeaves(psstrm, true);
		}
	}

public: 	// for transduction logging
	bool m_fAdvXSet;
	bool m_fAdvYSet;

};	// end of class GrSlotState


/*----------------------------------------------------------------------------------------------
	A GrSlotOutput represents one slot as the final output of the final pass. These
	are recorded in the segment.

	Hungarian: slout
----------------------------------------------------------------------------------------------*/
class GrSlotOutput : public GrSlotAbstract
{
	friend class GlyphInfo;
	friend class Segment;
	friend class SegmentMemoryUsage;

public:
	GrSlotOutput()
	{
		m_cComponents = 0;
		m_isloutClusterBase = -1;	// not part of any cluster
		m_disloutCluster = 0;
		m_igbb = -1;
	}

	void ExactCopyFrom(GrSlotOutput * pslout, u_intslot * pnVarLenBuf, int cnExtraPerSlot);

	void InitializeOutputFrom(GrSlotState * pslot);

	int BeforeAssoc()
	{
		return m_ichwBeforeAssoc;	// relative to the official beginning of the segment
	}
	int AfterAssoc()
	{
		return m_ichwAfterAssoc;	// relative to the official beginning of the segment
	}

	void SetBeforeAssoc(int ichw)
	{
		m_ichwBeforeAssoc = ichw;
	}
	void SetAfterAssoc(int ichw)
	{
		m_ichwAfterAssoc = ichw;
	}

	//u_intslot * PCompRefBufSlout()
	//{
	//	return m_prgnVarLenBuf;
	//}
	int CExtraSpaceSlout()
	{
		return (m_cnCompPerLig * 2);
	}

	int IbufUnderlyingCompOffset()
	{
		return 0;
	}
	int IbufCompIdOffset()
	{
		return m_cnCompPerLig;
	}

	void AddComponentReference(int ichw, int slati)
	{
		short ichw16 = short(ichw);
		if (m_cComponents >= m_cnCompPerLig)
		{
			Assert(false);	// too many--ignore the requested component ref
		}
		else
		{
			int iComp;
			for (iComp = 0; iComp < m_cComponents; iComp++)
			{
				if (ComponentId(iComp) == slati)
				{
					//	We already have at least one character for this component. Add another.
					if (m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[0] > ichw16)
						m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[0] = ichw16; // min
					if (m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[1] < ichw16)
						m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[1] = ichw16; // max
					return;
				}
			}

			iComp = m_cComponents;

			m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[0] = ichw16;	// min
			m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[1] = ichw16; // max

			// OBSOLETE comment:
			//	Maps the used components to the defined components. Normally this will be
			//	one-to-one and the buffer will hold [0,1,2...]. But possibly we may
			//	have defined components a, b, and c, but only mapped a and c to actual
			//	characters. This buffer will then hold [0,2].

			m_prgnVarLenBuf[IbufCompIdOffset() + iComp].nValue = slati;

//			Assert(iComp >= m_cComponents); // because we process them in order they are defined
											// in, but we could have skipped some that are
											// defined
			m_cComponents++;
		}
	}
	int NumberOfComponents() // the number used by the rules
	{
		return m_cComponents;
	}
	//	Index of ligature components, relative to the beginning of the segment.
	//	iComp is index of components USED in this glyph.
	int FirstUnderlyingComponent(int iComp)
	{
		Assert(iComp < m_cnCompPerLig);
		int ichw = m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[0];
		return ichw;
	}
	int LastUnderlyingComponent(int iComp)
	{
		Assert(iComp < m_cnCompPerLig);
		int ichw = m_prgnVarLenBuf[IbufUnderlyingCompOffset() + iComp].smallint[1];
		return ichw;
	}
	int ComponentId(int iComp)
	{
		return m_prgnVarLenBuf[IbufCompIdOffset() + iComp].nValue;
	}

	void SetClusterBase(int islout)
	{
		m_isloutClusterBase = islout;
	}

	int ClusterBase()
	{
		return m_isloutClusterBase;
	}

	bool IsPartOfCluster()
	{
		return (m_isloutClusterBase > -1);
	}

	//int NumClusterMembers()
	//{
	//	return m_visloutClusterMembers.size();
	//}
	//int ClusterMember(int iislout)
	//{
	//	return m_visloutClusterMembers[iislout];
	//}

	void AddClusterMember(int isloutThis, int isloutAttached)
	{
		m_disloutCluster = sdata8(max(int(m_disloutCluster), abs(isloutThis - isloutAttached)));
	}

	void ClusterMembers(Segment * pseg, int islout, std::vector<int> & visloutRet);

	int ClusterRange()
	{
		return m_disloutCluster;
	}

	float ClusterXOffset()	{ return m_xsClusterXOffset; }
	float ClusterAdvance()	{ return m_xsClusterAdvance; }
	void SetClusterXOffset(float xs)	{ m_xsClusterXOffset = xs; }
	void SetClusterAdvance(float xs)	{ m_xsClusterAdvance = xs; }

	float GlyphMetricLogUnits(Font * pfont, int nGlyphMetric);
	//float GlyphMetricLogUnits(int nMetricID);

	int GlyphBbIndex()				{ return m_igbb; }
	void SetGlyphBbIndex (int i)	{ m_igbb = i; }

	Rect BoundingBox(Font & font)
	{
		Rect rectBB;
		rectBB.left = m_xsPositionX + GlyphMetricLogUnits(&font, kgmetBbLeft);
		if (IsSpace())
			rectBB.right = m_xsPositionX + GlyphMetricLogUnits(&font, kgmetAdvWidth);
		else
			rectBB.right = m_xsPositionX + GlyphMetricLogUnits(&font, kgmetBbRight);
		rectBB.top = m_ysPositionY + GlyphMetricLogUnits(&font, kgmetBbTop);
		rectBB.bottom = m_ysPositionY + GlyphMetricLogUnits(&font, kgmetBbBottom);
		return rectBB;
	}

	bool IsLineBreak()
	{
		return (IsInitialLineBreak() || IsFinalLineBreak());
	}
	bool IsInitialLineBreak()
	{
		return (m_spsl == kspslLbInitial);
	}
	bool IsFinalLineBreak()
	{
		return (m_spsl == kspslLbFinal);
	}

	//float AdvanceXMetric()
	//{
	//	return m_xysAdvX;
	//}
	
	bool IsSpace();

	void AdjustPosXBy(float dxs)
	{
		m_xsPositionX += dxs;
		//m_rectBB.left += dxs;
		//m_rectBB.right += dxs;
	}

    void ShiftForDirDepthChange(float dxsSegWidth);

	// Used by GlyphInfo
	int IndexAttachedTo();
	//inline float AdvanceX(Segment * pseg);
	//inline float AdvanceY(Segment * pseg);
	float MaxStretch(Segment * pseg, int level);
	float MaxShrink(Segment * pseg, int level);
	float StretchStep(Segment * pseg, int level);
	int JustWeight(int level);
	float JustWidth(Segment * pseg, int level);
	float MeasureSolLogUnits(Segment * pseg);
	float MeasureEolLogUnits(Segment * pseg);
	Rect ComponentRect(Segment * pseg, int icomp);

protected:
	//	Instance variables:

	sdata8 m_cComponents;

	data8 m_disloutCluster;		// how far to search on either side of this glyph to find
								// other members of the cluster;
								// 0 means there are no cluster members or this glyph is
								// attached to some other base

	int m_isloutClusterBase;	// the index of the slot that serves as the base for the
								// cluster this slot is a part of; -1 if not part of cluster

	int	m_ichwBeforeAssoc;		// index of associated character(s) in the string
	int	m_ichwAfterAssoc;		// (relative to the official beginning of the segment)
								// char might possibly not be officially in this segment,
								// in which case value is infinity

	// Measurements for highlighting an entire cluster, relative to origin of this slot,
	// which is the cluster base.
	float m_xsClusterXOffset;
	float m_xsClusterAdvance;	// for single non-cluster glyphs, advance width of positioned glyph

	// Index into m_prggbb in Segment; -1 indicates a line break slot that is not rendered:
	int m_igbb;

	float m_xsAdvanceX;
//	float m_ysAdvanceY;	-- not used
//	Rect m_rectBB;

};	// end of class GrSlotOutput

} // namespace gr


#endif // !SLOTSTATE_INCLUDED
