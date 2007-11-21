/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrSlotState.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Copyright (C) 1999 by SIL International. All rights reserved.

Description:
    Class GrSlotAbstract, GrSlotState, and GrSlotOutput
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

	~GrSlotAbstract()
	{
		// the table manager is responsible for destroying the contents of m_prgnVarLenBuf
	}

	void BasicInitialize(int cnUserDefn, int cnCompPerLig, int cnFeat, u_intslot * pnBuf)
	{
		m_dirc = kNotYetSet;
		m_lb = kNotYetSet;
		m_fInsertBefore = true;
		m_nDirLevel = -1;
		m_fHasComponents = false;

		m_xysGlyphWidth = -1;
		m_xysFontAscent = -1;
		m_xysFontDescent = -1;

		m_mAdvanceX = kNotYetSet;
		m_mAdvanceY = kNotYetSet;
		m_mShiftX = 0;
		m_mShiftY = 0;

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

		m_fInsertBefore = true;

		m_mMeasureSol = 0;
		m_mMeasureEol = 0;

		m_mJStretch0 = 0;
		m_mJShrink0 = 0;
		m_mJStep0 = 0;
		m_nJWeight0 = 0;
		m_mJWidth0 = 0;

		m_islotPosPass = kNotYetSet;

		m_cnUserDefn = byte(cnUserDefn);
		m_cnCompPerLig = byte(cnCompPerLig);
		m_cnFeat = byte(cnFeat);
		m_prgnVarLenBuf = pnBuf;

		m_fAdvXSet = false; // for transduction logging
		m_fAdvYSet = false;
	}

	void SetBufferPtr(u_intslot * pn)
	{
		m_prgnVarLenBuf = pn;
	}

	gid16 GlyphID()				{ return m_chwGlyphID; }
	gid16 RawActualGlyph()		{ return m_chwActual; }
	float GetGlyphMetric(Font * pfont, int nGlyphMetricID, gid16 chwGlyphID);

	int StyleIndex()			{ return m_bStyleIndex; }
	void GetFeatureValues(GrFeatureValues * pfval)
	{
		pfval->m_nStyleIndex = m_bStyleIndex;
		std::fill(pfval->m_rgnFValues, pfval->m_rgnFValues + kMaxFeatures, 0);
		for (size_t i = 0; i < m_cnFeat; i++)
			pfval->m_rgnFValues[i] = PFeatureBuf()[i].nValue;
	}

	int RawAdvanceX()			{ return m_mAdvanceX; }
	int RawAdvanceY()			{ return m_mAdvanceY; }
	int ShiftX()				{ return m_mShiftX; }
	int ShiftY()				{ return m_mShiftY; }

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

	bool HasComponents()	{ return m_fHasComponents; }

//	int CompRef(int i)
//	{
//		Assert(i < m_cnCompPerLig);
//		if (i < m_cnCompPerLig)
//			return m_rgsrCompRef[i];
//		else
//			return kNotYetSet;
//	}

	GrSlotState * CompRefSlot(int i);

	int BreakWeight()
	{
		return m_lb;
	}
	DirCode Directionality()
	{
		return DirCode(m_dirc);
	}
	int InsertBefore()
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
		m_spsl = spsl;
	}

	void CopyFrom(GrSlotState * pslot);

	enum {
		kNotYetSet = 0x7FFF,
		kInvalidGlyph = 0xFFFF
	};

	int PosPassIndex()
	{
		return m_islotPosPass;
	}
	void SetPosPassIndex(int islot, bool fInputToPosPass1)
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

	gid16 ActualGlyphForOutput(GrTableManager * ptman);

	//	Variable-length buffer--includes four sub-buffers:
	//	* user-defined variables
	//	* component.???.ref assignments
	//	* mapping from components used to global attribute IDs for components
	//	* feature values

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

	int m_islotPosPass;		// index of slot in positioning streams, relative to first official
							// slot in the segment (possibly the LB slot)

	int m_spsl;				// special slot flag: LB, bidi marker

//	bool m_fInitialLB;		// for LB slots: true if this is the initial LB;
							// false if it is the terminating LB
							// TODO: remove

	int m_dirc;				// default = kNotYetSet (read from glyph attr)
	int m_lb;				// default = kNotYetSet (read from glyph attr)

	int m_nDirLevel;

	// raw glyph metrics (directly from font)
	float m_xysFontAscent;
	float m_xysFontDescent;
	float m_xysGlyphWidth;
	float m_xysGlyphHeight;
	float m_xysGlyphX;
	float m_xysGlyphY;
	float m_xysAdvX;
	float m_xysAdvY; 

	//	Slot attributes:
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

	short	m_mMeasureSol;
	short	m_mMeasureEol;

	unsigned short	m_mJStretch0;
	unsigned short	m_mJShrink0;
	unsigned short	m_mJStep0;
	int	m_mJWidth0;
	byte m_nJWeight0;

	bool m_fInsertBefore;	// default = true

	bool m_fHasComponents;	// default = false

	bool m_fIsSpace;

	byte m_bStyleIndex;

	byte m_cnUserDefn;
	byte m_cnCompPerLig;
	byte m_cnFeat;
	// There is a large block managed by either the GrTableManager (for GrSlotState)
	// or the segment (for GrSlotOutput); this variable points at the sub-buffer for this
	// particular slot:
	u_intslot * m_prgnVarLenBuf;	

	float m_xsPositionX;
	float m_ysPositionY;

public: 	// for transduction logging
	bool m_fAdvXSet;
	bool m_fAdvYSet;

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

	~GrSlotState()
	{
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

	//	General:
	int RawSegOffset()		{ return m_ichwSegOffset; }

	void SetGlyphID(gid16 chw)
	{
		m_chwGlyphID = chw;
		m_chwActual = kInvalidGlyph;
		m_xysGlyphWidth = -1; // indicate glyph metrics are invalid
	}
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
	void AllComponentRefs(std::vector<int> & vichw);

	int PassModified()			{ return m_ipassModified; }
	int SegOffset();
	int UnderlyingPos();
	GrSlotState * PrevState()	{ return m_pslotPrevState; }

	void MarkDeleted()	// for now, do nothing
	{
	}

	int IsSpace(GrTableManager * ptman);

	// TODO: remove argument from these methods; it is no longer needed.
	bool IsLineBreak(gid16 chwLB)
	{
		return (IsInitialLineBreak(chwLB) || IsFinalLineBreak(chwLB));
		//return (m_chwGlyphID == chwLB); // TODO: remove
	}
	bool IsInitialLineBreak(gid16 chwLB)
	{
		return (m_spsl == kspslLbInitial);
		//return (IsLineBreak(chwLB) && m_fInitialLB == true); // TODO: remove
	}
	bool IsFinalLineBreak(gid16 chwLB)
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

	// Directionality as determined by the bidi algorithm
	DirCode DirProcessed() // return the value
	{
		Assert(m_dirc != kNotYetSet);
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

	int AttachWithX(GrTableManager * ptman, GrSlotStream * psstrm)
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
	void SetShiftX(int mVal) {
		Assert(mVal < 0xFFFF); m_mShiftX = short(mVal & 0xFFFF); m_fShiftMod = true; }
	void SetShiftY(int mVal) {
		Assert(mVal < 0xFFFF); m_mShiftY = short(mVal & 0xFFFF); m_fShiftMod = true; }

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

	void SetBreakWeight(int lb)				{ m_lb = lb; }
	void SetInsertBefore(bool f)			{ m_fInsertBefore = f; }

	void SetDirectionality(DirCode dirc)	{ m_dirc = dirc; }
	void SetDirLevel(int n)					{ m_nDirLevel = n; }

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
		int nLevel, bool fThorough = false);

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
	float ClusterRsb(GrSlotStream * psstrm, float xs)
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
	float ClusterBbTop(GrSlotStream * psstrm)
	{
		return m_ysClusterBbTop;
	}
	float ClusterBbBottom(GrSlotStream * psstrm)
	{
		return m_ysClusterBbBottom;
	}
	float ClusterBbWidth(GrSlotStream * psstrm)
	{
		return m_xsClusterBbRight - m_xsClusterBbLeft + 1;
	}
	float ClusterBbHeight(GrSlotStream * psstrm)
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
	int m_nUnicode;; // for debugging

	std::vector<GrSlotState*> m_vpslotAssoc;	// association mappings

	bool m_fNeutralAssocs;		// true if we've set the associations to some neutral
								// default, rather than them being set explicitly
								// within a rule

	DirCode m_dircProc;		// directionality as processed in bidi algorithm
	bool m_fDirProcessed;


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


	//	Private methods:

	void AdjustRootMetrics(GrTableManager * ptman, GrSlotStream *);
	void InitMetrics(GrTableManager * ptman, GrSlotState * pslotRoot);
	void InitLeafMetrics(GrTableManager * ptman, GrSlotState * pslotRoot);
	void InitRootMetrics(GrTableManager * ptman);
	void CalcRootMetrics(GrTableManager * ptman, GrSlotStream *, int nLevel);
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

public:
	GrSlotOutput()
	{
		m_cComponents = 0;
		m_isloutClusterBase = -1;	// not part of any cluster
		m_igbb = -1;
		m_visloutClusterMembers.clear();
	}

	void ExactCopyFrom(GrSlotOutput * pslout, u_intslot * pnVarLenBuf, int cnExtraPerSlot);

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
	void AddComponentReference(int ichw, int slati)
	{
		if (m_cComponents >= m_cnCompPerLig)
		{
			Assert(false);	// ignore the requested component ref
		}
		else
		{
			//	Must be kept consistent with UserDefn() and CompRef() methods inherited
			//	from GrSlotAbstract. The component indices come after the above variable-
			//	length buffers.
			m_prgnVarLenBuf[m_cnUserDefn + (m_cnCompPerLig * 2) + m_cnFeat + m_cComponents].nValue
				= ichw;

			// OBSOLETE comment:
			//	Maps the used components to the defined components. Normally this will be
			//	one-to-one and the buffer will hold [0,1,2...]. But possibly we may
			//	have defined components a, b, and c, but only mapped a and c to actual
			//	characters. This buffer will then hold [0,2].

			m_prgnVarLenBuf[m_cnUserDefn + (m_cnCompPerLig * 2) + m_cnFeat
				+ m_cnCompPerLig + m_cComponents].nValue
				= slati;

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
	int UnderlyingComponent(int iComp)
	{
		Assert(iComp < m_cnCompPerLig);
		//	Must be kept consistent with UserDefn() and CompRef() methods inherited
		//	from GrSlotAbstract. The component indices come after the above variable-
		//	length buffers.
		return m_prgnVarLenBuf[m_cnUserDefn + (m_cnCompPerLig * 2) + m_cnFeat + iComp].nValue;
	}
	int ComponentId(int iComp)
	{
		return m_prgnVarLenBuf[m_cnUserDefn + (m_cnCompPerLig * 2) + m_cnFeat
			+ m_cnCompPerLig + iComp].nValue;
	}

	//	Given the index of the component used by the rules, indicate the
	//	corresponding component as defined by the glyph attributes.
//	int DefinedComponent(int iCompUsed)
//	{
//		return m_prgnVarLenBuf[m_cnUserDefn + m_cnCompPerLig + m_cnFeat
//			+ m_cnUserDefn + iCompUsed];
//	}

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
		return m_isloutClusterBase > -1;
	}

	int NumClusterMembers()
	{
		return m_visloutClusterMembers.size();
	}

	int ClusterMember(int iislout)
	{
		return m_visloutClusterMembers[iislout];
	}

	void AddClusterMember(int islout)
	{
		m_visloutClusterMembers.push_back(islout);
	}

	float ClusterXOffset()	{ return m_xsClusterXOffset; }
	float ClusterAdvance()	{ return m_xsClusterAdvance; }
	void SetClusterXOffset(float xs)	{ m_xsClusterXOffset = xs; }
	void SetClusterAdvance(float xs)	{ m_xsClusterAdvance = xs; }

	float GlyphMetricLogUnits(Font * pfont, int nGlyphMetric);
	float GlyphMetricLogUnits(int nMetricID);

	int GlyphBbIndex()				{ return m_igbb; }
	void SetGlyphBbIndex (int i)	{ m_igbb = i; }

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

	float AdvanceXMetric()
	{
		return m_xysAdvX;
	}
	bool IsSpace();

	int CExtraSpaceSlout()
	{
		return m_cnUserDefn + (m_cnCompPerLig * 2) + m_cnFeat + (m_cnCompPerLig * 2);
	}

	void AdjustPosXBy(float dxs)
	{
		m_xsPositionX += dxs;
		m_rectBB.left += dxs;
		m_rectBB.right += dxs;
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

	int	m_ichwBeforeAssoc;		// index of associated character in the string
	int	m_ichwAfterAssoc;		// (relative to the official beginning of the segment)
								// char might possibly not be officially in this segment

	int m_cComponents;

	std::vector<int> m_visloutClusterMembers;	// indices (logical surface position)
												// of other slots that have this cluster as
												// its base (absolute root), not necessarily sorted;
												// does not include self

	int m_isloutClusterBase;	// the index of the slot that serves as the base for the
								// cluster this slot is a part of; -1 if not part of cluster

	// Measurements for highlighting an entire cluster, relative to origin of this slot,
	// which is the cluster base.
	float m_xsClusterXOffset;
	float m_xsClusterAdvance;	// for single non-cluster glyphs, advance width of positioned glyph

	// Index into m_prggbb in Segment; -1 indicates a line break slot that is not rendered:
	int m_igbb;

	float m_xsAdvanceX;
	float m_ysAdvanceY;
	Rect m_rectBB;

};	// end of class GrSlotOutput

} // namespace gr


#endif // !SLOTSTATE_INCLUDED
