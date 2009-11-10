/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: SegmentPainter.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Defines the class for a Graphite text segment.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_SEGPAINTER_INCLUDED
#define GR_SEGPAINTER_INCLUDED

// undo automagic DrawTextA DrawTextW stuff
// #undef DrawText

//:End Ignore


namespace gr
{

class Segment;

/*----------------------------------------------------------------------------------------------
	A SegmentPainter handles the UI-related operations on for segment.

	This superclass does not know how to do any actual drawing, because it has no access
	to a device. Subclasses are needed to handle drawing on the various platforms. In addition,
	it is legitimate to make a subclass to override the behavior of a specific piece of
	functionality.

	Hungarian: segp
----------------------------------------------------------------------------------------------*/
class SegmentPainter
{
public:
	// Static methods

	// Constructors/destructors/etc.
	SegmentPainter(Segment * pseg, float xOrigin = 0, float yOrigin = 0);
	//SegmentPainter(Segment * pseg, float xOrigin, float yOrigin);

	virtual ~SegmentPainter();

	virtual void setOrigin(float xsOrigin, float ysOrigin);
	virtual void setPosition(float xdPosition, float ydPosition);
    virtual void setScalingFactors(float xFactor, float yFactor);

	virtual void setBuggyDisplayFormat(int /*strategy*/, long /*textColor*/, long /*backColor*/,
		long /*underlineColor*/)
	{
		// Currently not implemented to do anything useful.
	}

	// The paint method must be implemented by a concrete subclass:
	virtual void paint() = 0;

	virtual void drawInsertionPoint(int ichwIP, bool fAssocPrev, bool bOn, bool fForceSplit);
	virtual void positionsOfIP(int ichwIP, bool fAssocPrev, bool fForceSplit,
		Rect * prdPrimary, Rect * prdSecondary);
	virtual bool drawSelectionRange(int ichwMin, int ichwLim,
		float ydTop, float ydBottom, bool bOn);
	virtual bool positionsOfRange(int ichwMin, int ichwLim,
		float ydTop, float ydBottom, Rect * prsBounds);
	virtual size_t getUnderlinePlacement(int ichwMin, int ichwLim,
		bool fSkipSpace,
		size_t crgMax,	// number of ranges allowed
		float * prgxdLefts, float * prgxdRights, float * prgydUnderline);

	virtual void pointToChar(Point zptdClickPosition, int * pichw, bool * pfAssocPrev);	
	virtual LgIpValidResult isValidInsertionPoint(int ichwIP);
	virtual bool doBoundariesCoincide(bool fBoundaryEnd, bool fBoundaryRight);
	virtual int arrowKeyPosition(int ichwIP, bool * pfAssocPrev,
		bool fRight, bool * pfInThisSeg);
	virtual int extendSelectionPosition(
		int charIndex, bool assocPrevMatch, bool assocPrevNeeded,
		int anchorIndex, bool movingRight, bool * inThisSeg);

	//	For calculating underlines:
	struct LineSeg {	// hungarian: ls
		float left;
		float right;
	};

	static float ScaleX(float xs, Rect rs, Rect rd);
	static float ScaleY(float ys, Rect rs, Rect rd);

	float ScaleXToDest(float xs)
	{
		float xd = ((xs + m_xsOrigin) * m_xFactor) + m_xdPosition;
		return xd;
	}
	//int ScaleXToDest(int xs)
	//{
	//	int xd = (((xs + m_xsOriginInt) * m_xFactor64) >> 6) + m_xdPositionInt;
	//	return xd;                         // >> 6 means div by 64
	//}
	float ScaleYToDest(float ys)
	{
		float yd = ((ys + m_ysOrigin) * m_yFactor) + m_ydPosition;
		return yd;
	}

	float ScaleXToSource(float xd)
	{
		float xs = ((xd - m_xdPosition) / m_xFactor) - m_xsOrigin;
		return xs;
	}
	float ScaleYToSource(float yd)
	{
		float ys = ((yd - m_ydPosition) / m_yFactor) - m_ysOrigin;
		return ys;
	}

protected:
	//	Member variables:

	Segment * m_pseg;

	//	transformation factors:
	float m_xsOrigin;
	float m_ysOrigin;
	float m_xdPosition;
	float m_ydPosition;
	float m_xFactor;	// destination / source
	float m_yFactor;	// destination / source

	// optimizations for when we need integer arithmetic (converting between int and float is
	// very slow) - DELETE these.
	//int m_xsOriginInt;
	//int m_xdPositionInt;
	//int m_xFactor64;

	//gr::Rect m_rectSrc;

	//	Other methods:

	void CalcOrDrawInsertionPoint(
		int ich, bool fAssocPrev, bool bOn, bool fForceSplit,
		Rect * prectPrimary, Rect * prectSecondary);
	void InvertIBeam(float xs, float ysTop, float ysBottom, bool fAssocPrev,
		Rect * prdRet);
	void InvertSplitIP(float xs, float ysTop, float ysBottom,
		bool fTop, bool fAssocRight, bool fSecondary, float ysMinSplitHt,
		Rect * prdRet);
	void CalcIP(int ichw, bool fBefore,
		float * pxs, float * pysTop, float * pysBottom, bool * pfRtl);
	bool CloseIPPositions(
		float * pxsBefore, float ysBeforeTop, float ysBeforeBottom,
		float * pxsAfter, float ysAfterTop, float ysAfterBottom);
	bool AtEdgeOfCluster(GrSlotOutput * pslout, int islout, bool fBefore);
	bool AtEdgeOfCluster(GrSlotOutput * psloutBase, int isloutBase,
		GrSlotOutput * pslout, int islout, bool fBefore);
	bool CanInsertIntoCluster(GrSlotOutput * pslout, int islout);

	void CalcPartialLigatures(bool * prgfAllSelected,
		int ichwMinSeg, int ichwLimSeg,
		int ichwMinSel, int ichwLimSel);
	void CalcHighlightRect(int ichw,
		std::vector<Rect> & vrs, std::vector<bool> & vfEntireLineHt,
		bool fJustComponent, bool * rgfHighlighted, bool fSkipTrSpace);
	void CalcCompleteCluster(int islout,
		std::vector<Rect> & vrs, std::vector<bool> & vfEntireLineHt, bool * prgfHighlighted);
	void AddRectWithoutOverlaps(std::vector<Rect> & vrect, Rect rectToAdd);
	bool AnyArea(Rect * prect);
	bool AdjustRectsToNotOverlap(std::vector<Rect> & vrect, int irect, Rect * prect2,
		std::vector<Rect> & vrectMoreToAdd);
	void AssertNoOverlaps(std::vector<Rect> & vrect);

	void AddLineSegWithoutOverlaps(std::vector<LineSeg> & vls, LineSeg lsToAdd);
	bool AnyLength(LineSeg * pls);
	bool AdjustLineSegsToNotOverlap(std::vector<LineSeg> & vls, int ils, LineSeg * pls2,
		std::vector<LineSeg> & vlsMoreToAdd);
	void AssertNoOverlaps(std::vector<LineSeg> & vls);

	GrResult ArrowKeyPositionAux(int * pichwIP, bool * pfAssocPrev,
		bool fRight, bool fMovingIn,
		bool fAssocPrevMatch, bool fAssocPrevNeeded,
		bool * pfResult);
	bool ArrowKeyPositionInternal(int * pichw, bool * pfAssocPrev,
		bool fRight, bool fAssocPrevMatch, bool fAssocPrevNeeded, int * nNextOrPrevSeg);
	bool AdjacentLigComponent(int * pichw, bool * pfAssocPrev, bool fRight, bool fMove);

	int PointToCharAux(float xsTry, float ysTry,
		float * pxsGlyphOffset, float * pxsGlyphWidth);

	// Platform-specific routines:

	GrResult DrawSegmentPlatform(float xs, float ys);

	// Low-level routines--these must be implemented appropriate by subclasses:

	virtual void InvertRect(float /*xLeft*/, float /*yTop*/, float /*xRight*/, float /*yBottom*/)
	{
		GrAssert(false); // no device context
	}

	//void SetFontProps();
	virtual void SetFontProps(unsigned long clrFore, unsigned long clrBack);

};

/*----------------------------------------------------------------------------------------------
	This is a subclass of SegmentPainter for which the paint operation is unnecessary.
----------------------------------------------------------------------------------------------*/
class SegmentNonPainter : public SegmentPainter
{
public:
	SegmentNonPainter(Segment * pseg)
		: SegmentPainter(pseg)
	{
	}

	void paint()
	{
		GrAssert(false);
		throw;
	}
};

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif  // !GR_SEGPAINTER_INCLUDED
