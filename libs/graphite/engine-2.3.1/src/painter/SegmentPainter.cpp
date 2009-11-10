/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: SegmentPainter.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Implements the object that handles drawing, mouse clicks, and other UI-related behavior
	for a Graphite segment.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	   Include files
//:>********************************************************************************************
#include "Main.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif
// any other headers (not precompiled)
#include <math.h>
#ifndef _WIN32
#include <stdlib.h>
#endif

#undef THIS_FILE
DEFINE_THIS_FILE

//:>********************************************************************************************
//:>	   Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	   Local Constants and static variables
//:>********************************************************************************************
namespace 
{

static bool g_fDrawing;

} // namespace


namespace gr
{

//:>********************************************************************************************
//:>	   Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Constructors.
----------------------------------------------------------------------------------------------*/
//SegmentPainter::SegmentPainter(Segment * pseg)
//{
//	m_pseg = pseg;
//
//	m_xsOrigin = 0;
//	m_ysOrigin = 0;
//	m_xdPosition = 0;
//	m_ydPosition = 0;
//	m_xFactor = 1.0;
//	m_yFactor = 1.0;
//}

SegmentPainter::SegmentPainter(Segment * pseg, float xsOrigin, float ysOrigin)
{
	m_pseg = pseg;

	m_xsOrigin = xsOrigin;
	m_ysOrigin = ysOrigin;
	m_xdPosition = m_xsOrigin;
	m_ydPosition = m_ysOrigin;
	m_xFactor = 1.0;
	m_yFactor = 1.0;

	//m_xsOriginInt = GrEngine::RoundFloat(xsOrigin);
	//m_xdPositionInt = m_xsOriginInt;
	//m_xFactor64 = 64;
}

/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
SegmentPainter::~SegmentPainter()
{
}

/*----------------------------------------------------------------------------------------------
	Initialization.

	The correspondence with the old rectangle approach is:
	m_xsOrigin = sourceRect.left;
	m_ysOrigin = sourceRect.top;
	m_xdPosition = destRect.left;
	m_ydPosition = destRect.top;

	m_xFactor = (destRect.right - destRect.left) / (sourceRect.right - sourceRect.left);
	m_yFactor = (destRect.top - destRect.bottom) / (sourceRect.top - sourceRect.bottom);

----------------------------------------------------------------------------------------------*/
void SegmentPainter::setOrigin(float xsOrigin, float ysOrigin)
{
	m_xsOrigin = xsOrigin;
	m_ysOrigin = ysOrigin;

	//m_xsOriginInt = GrEngine::RoundFloat(xsOrigin);
}

void SegmentPainter::setPosition(float xdPosition, float ydPosition)
{
	m_xdPosition = xdPosition;
	m_ydPosition = ydPosition;

	//m_xdPositionInt = GrEngine::RoundFloat(xdPosition);
}

void SegmentPainter::setScalingFactors(float xFactor, float yFactor)
{
	m_xFactor = xFactor;
	m_yFactor = yFactor;

	//int temp = GrEngine::RoundFloat(xFactor);
	//m_xFactor64 = GrEngine::RoundFloat(xFactor * 64);
}


//:>********************************************************************************************
//:>	SegmentPainter interface methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Draw the text of the segment at the specified point in the graphics context.
	This method must be implemented by a concrete subclass for painters that really need
	to paint.
----------------------------------------------------------------------------------------------*/
//void SegmentPainter::paint()
//{
//	Assert(false);
//}

/*----------------------------------------------------------------------------------------------
	Indicate if the character position is a valid place for an insertion point.

	@oaram ich			- position to test
	@param pipvr		- answer: kipvrOK = IP here is valid;
							kipvrBad = IP here no good;
							kipvrUnknown = this seg can't decide, ask the next
----------------------------------------------------------------------------------------------*/
LgIpValidResult SegmentPainter::isValidInsertionPoint(int ichw)
{
	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	int ichwSegOffset = ichw - m_pseg->m_ichwMin;
	if (ichwSegOffset < m_pseg->m_ichwAssocsMin || ichwSegOffset >= m_pseg->m_ichwAssocsLim)
	{
		// If we're pointing at a hard line break character (Unicode 2028), then we know
		// that it's a good insertion point in spite of missing the other conditions.
		utf16 ch;
		m_pseg->m_pgts->fetch(ichw, 1, &ch);
		if (ch != knLineSep)
			return kipvrUnknown; // Definitely in a different segment of the paragraph.
		return kipvrOK;
	}

	int isloutLig = m_pseg->m_prgisloutLigature[ichwSegOffset - m_pseg->m_ichwAssocsMin];
	if (isloutLig != kNegInfinity)
	{
		//	Ligature.
		int isloutLigAfter = m_pseg->m_prgisloutLigature[ichwSegOffset - 1 - m_pseg->m_ichwAssocsMin];
		if (isloutLig == isloutLigAfter)
		{
			int icompBefore = m_pseg->m_prgiComponent[ichwSegOffset - m_pseg->m_ichwAssocsMin];
			int icompAfter =  m_pseg->m_prgiComponent[ichwSegOffset - 1 - m_pseg->m_ichwAssocsMin];
			if (icompBefore == icompAfter)
				//	If inserting before this character yields the same visible component
				//	as inserting after the previous, then this character is invisible, or has
				//	been merged with the previous, and it doesn't make sense to put an insertion
				//	point here.
				return kipvrBad;
		}
		return kipvrOK;
	}

	// TODO: do we need to test ALL the associated glyphs? Here we just test the first.

	LgIpValidResult ipvrRet;

	int isloutBefore = m_pseg->UnderlyingToLogicalSurface(ichw, true);	// leading edge
	int isloutAfter = m_pseg->UnderlyingToLogicalSurface(ichw - 1, false);	// trailing edge of previous
	int isloutTest = m_pseg->UnderlyingToLogicalInThisSeg(ichw);
	if (isloutBefore == isloutAfter)
		//	If inserting before this character yields the same visible glyph as
		//	inserting after the previous, then this character is invisible, or has
		//	been merged with the previous, and it doesn't make sense to put an insertion
		//	point here.
		ipvrRet = kipvrBad;
	else if (isloutTest == kNegInfinity || isloutTest == kPosInfinity)
		ipvrRet = kipvrBad;	// can't figure it out
	else if (!m_pseg->OutputSlot(isloutTest)->InsertBefore())
		ipvrRet = kipvrBad;
	else if (m_pseg->SameSurfaceGlyphs(ichw - 1, ichw))
		//	The insertion point would be between two characters that map to the same
		//	set of glyphs. Not a good place.
		ipvrRet = kipvrBad;
	else
		ipvrRet = kipvrOK;

	return ipvrRet;
}

/*----------------------------------------------------------------------------------------------
	Answer whether the logical and physical boundaries of the string coincide.
	This method is called by the application to handle writing system boundaries, and by
	other segments in a writing-system chain to handle line-breaks.

	@param fBoundaryEnd		- asking about the logical end boundary?
	@param fBoundaryRight	- asking about the physical right boundary?
	@param pfResult			- return value
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::doBoundariesCoincide(bool fBoundaryEnd, bool fBoundaryRight)
{
	float xsBefore, ysBeforeTop, ysBeforeBottom;
	float xsAfter, ysAfterTop, ysAfterBottom;
	bool fRtlBefore, fRtlAfter;

	if (m_pseg->m_cginf == 0)
	{
		//	Empty segment.
		return true;
	}

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	if (fBoundaryEnd)
	{
		CalcIP(m_pseg->m_ichwMin + m_pseg->m_dichwLim, true,
			&xsBefore, &ysBeforeTop, &ysBeforeBottom, &fRtlBefore);
		CalcIP(m_pseg->m_ichwMin + m_pseg->m_dichwLim, false,
			&xsAfter, &ysAfterTop, &ysAfterBottom, &fRtlAfter);
	}
	else
	{
		CalcIP(m_pseg->m_ichwMin, true,
			&xsBefore, &ysBeforeTop, &ysBeforeBottom, &fRtlBefore);
		CalcIP(m_pseg->m_ichwMin, false,
			&xsAfter, &ysAfterTop, &ysAfterBottom, &fRtlAfter);
	}

	//	We allow for rounding errors which might cause differences between the expected
	//	and the actual values, hence the test for a difference of <= 1.
	bool fRet;
	if (fBoundaryRight)
	{
		//	right edge
		if (fBoundaryEnd)	// logical end
			fRet = (!fRtlAfter && fabsf(xsAfter - (m_pseg->m_dxsTotalWidth + m_pseg->m_dxsStretch)) <= 1);
		else				// logical start
			fRet = (fRtlBefore && fabsf(xsBefore - (m_pseg->m_dxsTotalWidth + m_pseg->m_dxsStretch)) <= 1);
	}
	else
	{
		//	left edge
		if (fBoundaryEnd)	// logical end
			fRet = (fRtlAfter && fabsf(xsAfter) <= 1);
		else				// logical start
			fRet = (!fRtlBefore && fabsf(xsBefore) <= 1);
	}

	return fRet;
}

/*----------------------------------------------------------------------------------------------
	Draw an insertion point at an appropriate position.
 
	@param twx, twy		- same origin used to Draw segment
	@param ich			- character position; must be valid
	@param fAssocPrev	- associated with previous character?
	@param bOn			- turning on or off? Caller should alternate, first turning on
							(ignored in this implementation)
	@param dm			- draw mode:
							kdmNormal = draw complete insertion pt (I-beam or split cursor);
							kdmPrimary = only draw primary half of split cursor;
							kdmSecondary = only draw secondary half of split cursor
----------------------------------------------------------------------------------------------*/
void SegmentPainter::drawInsertionPoint(int ichwIP, bool fAssocPrev,
	bool bOn, bool fForceSplit) // LgIPDrawMode dm)
{
	if (g_fDrawing)
		return;

	g_fDrawing = true;

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	CalcOrDrawInsertionPoint(ichwIP,
		fAssocPrev, bOn, fForceSplit,
		NULL, NULL);	// passing NULLs here means we want to draw

	//RestoreFont(pgg);

	g_fDrawing = false;
}

/*----------------------------------------------------------------------------------------------
	Fill in bounding rectangles for the (possibly 2-part) IP, in destination device coordinates.
	Return flags indicating which of the 2 parts where rendered here.

	@param rs,rd			- source/destination coordinates, for scaling
	@param ichwIP			- insertion point
	@param fAssocPrev		- is the IP "leaning" backward?
	@param dm				- draw mode:
								kdmNormal = draw complete insertion pt (I-beam or split cursor);
								kdmPrimary = only draw primary half of split cursor;
								kdmSecondary = only draw secondary half of split cursor
	@param prdPrimary		- return location of primary selection, in dest coords
	@param prdSecondary		- return location of secondary selection, in dest coords
----------------------------------------------------------------------------------------------*/
void SegmentPainter::positionsOfIP(
	int ichwIP, bool fAssocPrev, bool fForceSplit,
	Rect * prdPrimary, Rect * prdSecondary)
{
	ChkGrArgPtr(prdPrimary);
	ChkGrArgPtr(prdSecondary);

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	CalcOrDrawInsertionPoint(ichwIP, fAssocPrev, true, fForceSplit,
		prdPrimary, prdSecondary);

	//RestoreFont(pgg);
}

/*----------------------------------------------------------------------------------------------
	Highlight a range of text.

	@param ichwAnchor/End	- selected range
	@param ydLineTop/Bottom	- top/bottom of area to highlight if whole line height;
								includes half of inter-line spacing.
	@param bOn				- true if we are turning on (ignored in this implementation)
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::drawSelectionRange(int ichwAnchor, int ichwEnd,
	float ydLineTop, float ydLineBottom, bool bOn)
{
	if (g_fDrawing)
		return true;

	g_fDrawing = true;

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	float xsSegLeft = 0;
	float ysSegTop = 0;

	float xdSegRight = ScaleXToDest(m_pseg->m_dxsTotalWidth);

	int ichwMinSel = min(ichwAnchor, ichwEnd);
	int ichwLimSel = max(ichwAnchor, ichwEnd);

	int ichwMinSeg = max(ichwMinSel, m_pseg->m_ichwMin + m_pseg->m_ichwAssocsMin);
	int ichwLimSeg = min(ichwLimSel, m_pseg->m_ichwMin + m_pseg->m_ichwAssocsLim);

	std::vector<Rect> vrs;
	std::vector<bool> vfEntireHt;

	bool * prgfAllCompsSelected = new bool[m_pseg->m_dichwLim];
	std::fill_n(prgfAllCompsSelected, m_pseg->m_dichwLim, false);
	CalcPartialLigatures(prgfAllCompsSelected, ichwMinSeg, ichwLimSeg,
		ichwMinSel, ichwLimSel);

	//	Array to keep track of which glyphs were highlighted.
	int cginf = m_pseg->m_cginf;
	bool * prgfHighlighted = new bool[cginf];
	std::fill_n(prgfHighlighted, cginf, false);

	for (int ichwLp = ichwMinSeg; ichwLp < ichwLimSeg; ichwLp++)
	{
		CalcHighlightRect(ichwLp, vrs, vfEntireHt,
			!prgfAllCompsSelected[ichwLp - m_pseg->m_ichwMin], prgfHighlighted, false);
	}

	//	If all the glyphs of a given cluster were highlighted, highlight the whole cluster
	//	as a unit.
	for (int islout = 0; islout < m_pseg->m_cslout; islout++)
	{
		CalcCompleteCluster(islout, vrs, vfEntireHt, prgfHighlighted);
	}

	//	Convert to destination coordinates; also extend full-line rectangles to the top
	//	and bottom of the line height.
	std::vector<Rect> vrd;
	for (size_t irs = 0; irs < vrs.size(); irs++)
	{
		Rect rsTmp = vrs[irs];
		Rect rdTmp;
		if (vfEntireHt[irs])
		{
			rdTmp.top = ydLineTop;
			rdTmp.bottom = ydLineBottom;
		}
		else
		{
			float ydTmp = ScaleYToDest(rsTmp.top + ysSegTop);
			rdTmp.top = min(ydLineBottom, max(ydLineTop, ydTmp));
			ydTmp = ScaleYToDest(rsTmp.bottom + ysSegTop);
			rdTmp.bottom = max(ydLineTop, min(ydLineBottom, ydTmp));
		}
		float xdTmp = ScaleXToDest(rsTmp.left + xsSegLeft);
		rdTmp.left = min(xdSegRight, max(float(0), xdTmp));
		xdTmp = ScaleXToDest(rsTmp.right + xsSegLeft);
		rdTmp.right = max(float(0), min(xdSegRight, xdTmp));
		if (rdTmp.left > rdTmp.right)
		{
			// I think this can happen due to rounding errors.
			float tmp = rdTmp.left;
			rdTmp.left = rdTmp.right;
			rdTmp.right = tmp;
		}
		vrd.push_back(rdTmp);
	}

	//	Create a list of rectangles with no overlaps, so the selection draws cleanly.
	std::vector<Rect> vrdNoOverlaps;
	size_t irect;
	for (irect = 0; irect < vrd.size(); irect++)
		AddRectWithoutOverlaps(vrdNoOverlaps, vrd[irect]);

	AssertNoOverlaps(vrdNoOverlaps);

	//	Draw the highlight.
	for (irect = 0; irect < vrdNoOverlaps.size(); irect++)
	{
		Rect rdTmp = vrdNoOverlaps[irect];
		InvertRect(rdTmp.left, rdTmp.top, rdTmp.right, rdTmp.bottom);
	}

	delete[] prgfAllCompsSelected;
	delete[] prgfHighlighted;

	//RestoreFont(pgg);

	g_fDrawing = false;

	return (vrdNoOverlaps.size() > 0);
}

/*----------------------------------------------------------------------------------------------
	Get a bounding rectangle that will contain the area highlighted by this segment
	when drawing the specified range.

	@param ichwAnchor/End	- selected range
	@param ydLineTop/Bottom	- top/bottom of area to highlight if whole line height;
								includes half of inter-line spacing.
	@param prdBounds		- return location of range
	@return					- true if the return rectange has a meaningful value
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::positionsOfRange(int ichwAnchor, int ichwEnd,
	float ydLineTop, float ydLineBottom, Rect * prdBounds)
{
	ChkGrArgPtr(prdBounds);

	std::vector<Rect> vrs;
	std::vector<bool> vfEntireHt;

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	int ichwMin = min(ichwAnchor, ichwEnd);
	int ichwLim = max(ichwAnchor, ichwEnd);

	for (int ichwLp = ichwMin; ichwLp < ichwLim; ichwLp++)
	{
		CalcHighlightRect(ichwLp, vrs, vfEntireHt, true, NULL, false);
	}

	if (vrs.size() == 0)
	{
		return false;
	}

	Rect * prsBounds = &(vrs[0]);
	for (size_t irect = 1; irect < vrs.size(); irect++)
	{
		Rect * prs = &(vrs[irect]);
		prsBounds->left = min(prsBounds->left, prs->left);
		prsBounds->right = max(prsBounds->right, prs->right);
	}

	prdBounds->top = ydLineTop;
	prdBounds->bottom = ydLineBottom;
	prdBounds->left = ScaleXToDest(prsBounds->left);
	prdBounds->right = ScaleXToDest(prsBounds->right);

	return true;
}

/*----------------------------------------------------------------------------------------------
	Convert a click position to a character position.

	@param zptdClickPosition	- relative to the segment draw origin
	@param pichw				- return character clicked before
	@param pfAssocPrev			- return true if they clicked on the trailing half of
									the previous char, false if click was on the
									leading half of the following
----------------------------------------------------------------------------------------------*/
void SegmentPainter::pointToChar(Point zptdClickPosition, int * pichw, bool * pfAssocPrev)
{
	ChkGrOutPtr(pichw);
	ChkGrOutPtr(pfAssocPrev);

	//SetUpGraphics(ichwBase, pgg, true);

	bool fRtl = m_pseg->rightToLeft();

	if (m_pseg->m_dysAscent < 0 || m_pseg->m_dxsWidth < 0)
		m_pseg->ComputeDimensions();

	if (m_pseg->m_dichwLim == 0 || m_pseg->m_cginf == 0)
	{
		*pichw = m_pseg->m_ichwMin;
		*pfAssocPrev = false;
		//RestoreFont(pgg);
		return;
	}

	float xsClick = ScaleXToSource(zptdClickPosition.x);
	float ysClick = ScaleYToSource(zptdClickPosition.y); 
	ysClick = m_pseg->m_dysAscent - ysClick;	// make relative to common base line

	float xsTry = xsClick;
	float ysTry = ysClick;	
	
	float xysInc = 1.0;
	//res = pgg->LogUnitsFromXPixels(1, &xysInc);
	//if (ResultFailed(res)) xysInc = 20;
	int cTries = 1;
	int nAdjustDir = 0;

	while (true)
	{
		float xsGlyphOffset;
		float xsGlyphWidth;
		int iginfLoc = PointToCharAux(xsTry, ysTry, &xsGlyphOffset, &xsGlyphWidth);

		bool fClickedAfter;
		int ichwCharClicked;
		if (xsTry < m_pseg->m_prgginf[m_pseg->LeftMostGlyph()].bb().left)
		{
			fClickedAfter = fRtl;
			ichwCharClicked = (fClickedAfter) ?
				m_pseg->m_ichwMin + m_pseg->m_ichwAssocsLim - 1 :
				m_pseg->m_ichwMin + m_pseg->m_ichwAssocsMin;
		}
		else if (xsTry > m_pseg->m_prgginf[m_pseg->RightMostGlyph()].bb().right)
		{
			fClickedAfter = !fRtl;
			ichwCharClicked = (fClickedAfter) ?
				m_pseg->m_ichwMin + m_pseg->m_ichwAssocsLim - 1 :
				m_pseg->m_ichwMin + m_pseg->m_ichwAssocsMin;
		}
		else
		{
			ichwCharClicked = m_pseg->PhysicalSurfaceToUnderlying(iginfLoc,
				xsTry - xsGlyphOffset, m_pseg->m_dysAscent - ysClick, 
				xsGlyphWidth, m_pseg->m_dysHeight, &fClickedAfter); // handles ligature components
		}

		if (fClickedAfter)
		{
			*pichw = ichwCharClicked + 1;
			while (!GrCharStream::AtUnicodeCharBoundary(m_pseg->GetString(), *pichw))
				*pichw = *pichw + 1;
			*pfAssocPrev = true;
		}
		else
		{
			*pichw = ichwCharClicked;
			*pfAssocPrev = false;
		}

		if (*pichw < m_pseg->m_ichwMin + m_pseg->m_ichwAssocsMin)
		{
			//RestoreFont(pgg);
			return;
		}
		else if (*pichw >= m_pseg->m_ichwMin + m_pseg->m_ichwAssocsLim)
		{
			//RestoreFont(pgg);
			return;
		}

		//int islout = UnderlyingToLogicalInThisSeg(ichwBase, ichwCharClicked);
		int isloutBefore = m_pseg->UnderlyingToLogicalInThisSeg(*pichw);
		if (isloutBefore == kNegInfinity || isloutBefore == kPosInfinity)
		{ }
		else
		{
			Assert(0 <= isloutBefore && isloutBefore < m_pseg->m_cslout);
			if (m_pseg->OutputSlot(isloutBefore)->InsertBefore())
			{
				//	Legal insertion point
				//RestoreFont(pgg);
				return;
			}
		}

		//	Gradually adjust the click position in various directions and try again
		//	until we find a legal insertion point.
		switch (nAdjustDir)
		{
		case 0:	// Adjust to the left
		case 4:
			xsTry = xsClick - (xysInc * cTries);
			ysTry = ysClick;
			nAdjustDir = 0;
			break;
		case 1:	// Adjust to the right
			xsTry = xsClick + (xysInc * cTries);
			ysTry = ysClick;
			break;
		case 2:	// Adjust up
			xsTry = xsClick;
			ysTry = ysClick - (xysInc * cTries);
			break;
		case 3:	// Adjust down
			xsTry = xsClick;
			ysTry = ysClick + (xysInc * cTries);
			cTries++;
			break;
		default:
			Assert(false);
			nAdjustDir = 0;
			cTries++;
		}
		nAdjustDir++;

		if (cTries > 50)
		{	// Safety net
			*pichw = 0;
			*pfAssocPrev = false;
			THROW(kresFail);
		}
		else if (cTries > 20)
			xysInc = xysInc * 2;
	}

	//RestoreFont(pgg);
}

/*----------------------------------------------------------------------------------------------
	Convert a click position to a glyph bounding-box position.
----------------------------------------------------------------------------------------------*/
int SegmentPainter::PointToCharAux(float xsTry, float ysTry,
	float * pxsGlyphOffset, float * pxsGlyphWidth)
{
	int iginfLoc = m_pseg->GlyphHit(xsTry, ysTry);

	GlyphInfo * pginf = m_pseg->m_prgginf + iginfLoc;
	*pxsGlyphOffset = pginf->bb().left;
	*pxsGlyphWidth = pginf->bb().right - *pxsGlyphOffset;

	return iginfLoc;
}

// Review: may need a way to do this and subsequent methods with one result per method?
/*----------------------------------------------------------------------------------------------
	Returns a value what logical position an arrow key should move the IP to.

	@param ichwIP			- initial position
	@param pfAssocPrev		- is the IP "leaning" backwards? input and output
	@param fRight			- direction of desired movement (physical or logical?)
	@param pfInThisSeg		- initially true if ichwIP is in the current segment;
								return value is true if there is a legitimate output
----------------------------------------------------------------------------------------------*/
int SegmentPainter::arrowKeyPosition(int ichwIP, bool * pfAssocPrev,
	bool fRight, bool * pfInThisSeg)
//GrResult SegmentPainter::arrowKeyPosition(
//	int * pichwIP, bool * pfAssocPrev,
//	bool fRight, bool fMovingIn,
//	bool * pfResult)
{
	ChkGrArgPtr(pfAssocPrev);
	//ChkGrOutPtr(pfInThisSeg);

	bool fMovingIn = !*pfInThisSeg;
	int ichwTmp = ichwIP;
	bool fResult;
	ArrowKeyPositionAux(&ichwTmp, pfAssocPrev, fRight, fMovingIn, false,  false, &fResult);
	*pfInThisSeg = fResult;
	return ichwTmp;

	//return ArrowKeyPositionAux(pichwIP, pfAssocPrev,
	//	fRight, fMovingIn, false,  false, pfResult);
}

/*----------------------------------------------------------------------------------------------
	Indicate what logical position a shift-arrow-key combination should move the
	end of the selection to.
 
	@param pichw			- initial endpoint and also adjusted result
	@param fAssocPrevMatch	- true if we need a certain orientation for the result;
								should be false when *pichw = 0 and we are moving within the
								same segment
	@param fAssocPrevNeeded	- association of the end-point, ie, true if it follows the anchor
	@param ichAnchor		- -1 if anchor is in a different segment
	@param fRight			- direction of desired movement
	@param fMovingIn		- true if we are moving in to this segment;
								if so, initial pichw meaningless
	@param pfRet			- if false try next seg or string
----------------------------------------------------------------------------------------------*/
int SegmentPainter::extendSelectionPosition(int ichwEnd, bool fAssocPrevMatch, bool fAssocPrevNeeded,
	int ichwAnchor, bool fRight, bool * pfInThisSeg)
//GrResult SegmentPainter::extendSelectionPosition(
//	int * pichw, bool fAssocPrevMatch, bool fAssocPrevNeeded, int ichwAnchor,
//	bool fRight, bool fMovingIn,
//	bool* pfRet)
{
	//ChkGrArgPtr(pichw);
	//ChkGrOutPtr(pfRet);

	// Make the same adjustment as for an insertion point.
	bool fAssocPrevCopy = fAssocPrevMatch;
	bool fMovingIn = !*pfInThisSeg;
	int ichwOld = ichwEnd;
	int ichwNew = ichwEnd;
	bool fResult;
	ArrowKeyPositionAux(&ichwNew, &fAssocPrevCopy,
		fRight, fMovingIn, fAssocPrevMatch, fAssocPrevNeeded, &fResult);
	*pfInThisSeg = fResult;

	//int ichwNew = *pichw;
	//bool fAssocPrevCopy = fAssocPrevMatch;
	//// TODO: adjust fAssocPrevCopy based on assoc-prev value of IP
	//*pfRet = false;
	//int ichwOld = *pichw;
	//GrResult res = ArrowKeyPositionAux(&ichwNew, &fAssocPrevCopy,
	//	fRight, fMovingIn,
	//	fAssocPrevMatch, fAssocPrevNeeded,
	//	pfRet);

//	if (*pfRet && ichwNew == *pichw)
//	{
//		// The selection wasn't actually moved, just switched the direction it leans.
//		// Try one more time.
//		res = ArrowKeyPositionAux(&ichwNew, &fAssocPrevCopy,
//			fRight, fMovingIn,
//			(ichwAnchor != *pichw && fAssocPrevNeeded), fAssocPrevNeeded,
//			pfRet);
//	}
	if (ichwAnchor != -1 && fResult &&
		((ichwNew < ichwAnchor && ichwAnchor < ichwOld) ||
			(ichwOld < ichwAnchor && ichwAnchor < ichwNew)))
	{
		// Don't switch directions in one operation--force to an insertion point.
		ichwNew = ichwAnchor;
	}
	return ichwNew;
	//*pichw = ichwNew;
	//ReturnResult(res);
}

/*----------------------------------------------------------------------------------------------
	Handle arrow key movement for both plain and shifted arrows.
 
	@param pichw			- initial endpoint and also adjusted result
	@param pfAssocPrev		- association of the end-point, ie, true if it follows the anchor
	@param fRight			- direction of desired movement
	@param fMovingIn		- true if we are moving in to this segment;
								if so, initial pichw meaningless
	@param fAssocPrevMatch	- true if the direction of the result should
								match the requested orientation, to avoid flipping around and
								infinite loops
	@param fAssocPrevNeeded - what is needed for the result, if fAssocPrevMatch == true
	@param pfRet			- if false try next segment or string
----------------------------------------------------------------------------------------------*/
GrResult SegmentPainter::ArrowKeyPositionAux(
	int * pichwIP, bool * pfAssocPrev,
	bool fRight, bool fMovingIn,
	bool fAssocPrevMatch, bool fAssocPrevNeeded,
	bool * pfResult)
{
	ChkGrArgPtr(pfResult);

	//bool fAssocPrevTmp = *pfAssocPrev;

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	int nNextOrPrevSeg;
	if (!fMovingIn)
	{
		*pfResult = ArrowKeyPositionInternal(pichwIP, pfAssocPrev, fRight,
			fAssocPrevMatch, fAssocPrevNeeded, &nNextOrPrevSeg);
		ReturnResult(kresOk);
	}
	else
	{
		int ichw;
		bool fAssocPrevNew;
		if (fRight)
		{
			//	Moving right: make selection at left edge.
			int iginf = m_pseg->LeftMostGlyph();
			bool fBefore = !m_pseg->rightToLeft();
			ichw = m_pseg->PhysicalSurfaceToUnderlying(iginf, fBefore) +
				((fBefore) ? 0 : 1);
			fAssocPrevNew = !fBefore;	// Review: should it be based on the char dir?
		}
		else
		{
			//	Moving left: make selection at right edge;
			int iginf = m_pseg->RightMostGlyph();
			bool fBefore = m_pseg->rightToLeft();
			ichw = m_pseg->PhysicalSurfaceToUnderlying(iginf, fBefore) +
				((fBefore) ? 0 : 1);
			fAssocPrevNew = !fBefore;	// Review: should it be based on the char dir?
		}

		//	Okay, now we have a selection at the edge of the segment. Try moving inside by
		//	one glyph.
		*pfResult = ArrowKeyPositionInternal(&ichw, &fAssocPrevNew, fRight,
			fAssocPrevMatch, fAssocPrevNeeded, &nNextOrPrevSeg);
		if (*pfResult)
		{
			*pichwIP = ichw;
			*pfAssocPrev = fAssocPrevNew;
		}

	}

	ReturnResult(kresOk);
}

/*----------------------------------------------------------------------------------------------
	Used to find out where underlines should be drawn.

	@param ichwMin/Lim		- range of text of interest
	@param rs, rd			- source/destination coordinates, for scaling
	@param fSkipSpace		- true if white space should not be underlined; some renderers may
								ignore this
	@param cxdMax			- number of ranges allowed
	@param prgxdLefts/Rights/UnderTops
							- arrays of corresponding values indicating where an underline
								should be drawn, in logical order

	TODO: fix the bug where if you have trailing spaces in different segments (eg,
	at different sizes), the spaces all but the last segment will be erroneously included
	in the underline result.
----------------------------------------------------------------------------------------------*/
size_t SegmentPainter::getUnderlinePlacement(int ichwMin, int ichwLim,
	bool fSkipSpace, size_t cxdMax,
	float * prgxdLefts, float * prgxdRights, float * prgydUnderTops)
{
	ChkGrArrayArg(prgxdLefts, cxdMax);
	ChkGrArrayArg(prgxdRights, cxdMax);
	//ChkGrArrayArg(prgydUnderTops, cxdMax);

	GrResult res = kresOk;

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_pseg->m_dxsWidth >= 0);
	Assert(m_pseg->m_dysAscent >= 0);

	float xsSegLeft = m_pseg->m_dxsTotalWidth;
	for (int iginf = 0; iginf < m_pseg->m_cginf; iginf++)
	{
		xsSegLeft = min(xsSegLeft, m_pseg->GlyphLeftEdge(iginf));
	}

	float xdSegLeft = this->ScaleXToDest(xsSegLeft);
	float xdSegRight = xdSegLeft + m_pseg->m_dxsTotalWidth;

	int ichwMinSel = min(ichwMin, ichwLim);
	int ichwLimSel = max(ichwMin, ichwLim);

	int ichwMinSeg = max(ichwMinSel, m_pseg->m_ichwMin + m_pseg->m_ichwAssocsMin);
	int ichwLimSeg = min(ichwLimSel, m_pseg->m_ichwMin + m_pseg->m_ichwAssocsLim);

	std::vector<Rect> vrs;
	std::vector<bool> vfEntireHt;

	bool * prgfAllCompsSelected = new bool[m_pseg->m_dichwLim];
	std::fill_n(prgfAllCompsSelected, m_pseg->m_dichwLim, false);
	CalcPartialLigatures(prgfAllCompsSelected, ichwMinSeg, ichwLimSeg,
		ichwMinSel, ichwLimSel);

	//	Array to keep track of which glyphs were highlighted:
	int cginf = m_pseg->m_cginf;
	bool * prgfHighlighted = new bool[cginf];
	std::fill_n(prgfHighlighted, cginf, false);

	for (int ichwLp = ichwMinSeg; ichwLp < ichwLimSeg; ichwLp++)
	{
		CalcHighlightRect(ichwLp, vrs, vfEntireHt,
			!prgfAllCompsSelected[ichwLp - m_pseg->m_ichwMin], prgfHighlighted,
			(m_pseg->m_fEndLine && fSkipSpace));
	}

	//	If all the glyphs of a given cluster were highlighted, highlight the whole cluster
	//	as a unit.
	for (int islout = 0; islout < m_pseg->m_cslout; islout++)
	{
		CalcCompleteCluster(islout, vrs, vfEntireHt, prgfHighlighted);
	}

	//	Convert rectangles in source coordinates to line segments in destination 
	//	coordinates.
	std::vector<LineSeg> vlsd;
	for (size_t irs = 0; irs < vrs.size(); irs++)
	{
		Rect rsTmp = vrs[irs];
		LineSeg lsdTmp;
		lsdTmp.left = min(xdSegRight, max(xdSegLeft, ScaleXToDest(rsTmp.left)));
		lsdTmp.right = max(xdSegLeft, min(xdSegRight, ScaleXToDest(rsTmp.right)));
		vlsd.push_back(lsdTmp);
	}

	//	Create a list of line segments with no overlaps.
	std::vector<LineSeg> vlsdNoOverlaps;
	size_t ils;
	for (ils = 0; ils < vlsd.size(); ils++)
		AddLineSegWithoutOverlaps(vlsdNoOverlaps, vlsd[ils]);

	AssertNoOverlaps(vlsdNoOverlaps);

	// The gap between the baseline and underlining is one pixel, on a normal screen
	// at standard magnification, and as near as we can get to the equivalent in any 
	// special case.
	//float dydGap = rdHeight / 96;
	//float dydSubscript = max(float(0), (-1 * GrEngine::GrFloatMulDiv(m_pseg->m_dysOffset, rsHeight, rdHeight)));
	float dydGap = 1;
	float dydSubscript = max(float(0), (-1 * (m_pseg->m_dysOffset * m_yFactor)));
	float ydBottom = ScaleYToDest(m_pseg->m_dysAscent) + dydGap + dydSubscript;

	//	Put the results in the output buffers.
	size_t cxdRet = vlsdNoOverlaps.size();
	for (ils = 0; ils < min(cxdRet, size_t(cxdMax)); ils++)
	{
		prgxdLefts[ils] = vlsdNoOverlaps[ils].left;
		prgxdRights[ils] = vlsdNoOverlaps[ils].right;
		if (prgydUnderTops)
			prgydUnderTops[ils] = ydBottom;
	}
	delete[] prgfAllCompsSelected;
	delete[] prgfHighlighted;

	//RestoreFont(pgg);

	if (vlsdNoOverlaps.size() > cxdMax)
		res = kresFail;

	return cxdRet;
}


//:>********************************************************************************************
//:>	Other methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Handle either drawing an insertion point or returning the position at which it
	would be drawn.

	@param ichwIP			- character position
	@param fAssocPrev		- true if selection is associated with previous character
	@param bOn				- turning on or off? (ignored in this implementation)
	@param fForceSplit		- force a split insertion point
	@param prdPrimary		- if NULL, do drawing; otherwise, return location of primary IP
								in destination device coordinates
	@param prdSecondary		- return location of secondary IP in dest device coordinates
	@param pfPrimaryHere	- true if primary selection is rendered in this segment
	@param pfSecHere		- true if (distinct) secondary selection is rendered in this segment;
								false if it is in a different segment, or we are rendering
								an I-beam (std selection)
----------------------------------------------------------------------------------------------*/
void SegmentPainter::CalcOrDrawInsertionPoint(
	int ichwIP, bool fAssocPrev, bool bOn, bool fForceSplit,
	Rect * prdPrimary, Rect * prdSecondary)
{
	GrResult res = kresOk;

	float xsSegLeft = 0;
	float ysSegTop = 0;

	if (prdPrimary)
	{
		prdPrimary->top = 0;
		prdPrimary->bottom = 0;
		prdPrimary->left = 0;
		prdPrimary->right = 0;
	}
	if (prdSecondary)
	{
		prdSecondary->top = 0;
		prdSecondary->bottom = 0;
		prdSecondary->left = 0;
		prdSecondary->right = 0;
	}

	int ichwLim = m_pseg->m_ichwMin + m_pseg->m_dichwLim;
	bool fRtl = m_pseg->rightToLeft();

	if (m_pseg->m_dxsWidth < 0)
		m_pseg->ComputeDimensions();

	//	Get the locations for the before and after selections.

	float xsBefore, ysBeforeTop, ysBeforeBottom;
	float xsAfter, ysAfterTop, ysAfterBottom;
	bool fRtlBefore, fRtlAfter;
	CalcIP(ichwIP, true, &xsBefore, &ysBeforeTop, &ysBeforeBottom, &fRtlBefore);
	CalcIP(ichwIP, false, &xsAfter, &ysAfterTop,  &ysAfterBottom,  &fRtlAfter);

	if (m_pseg->m_cginf == 0 && ichwIP == ichwLim)
	{
		Assert(kPosInfFloat == xsBefore && kPosInfFloat == xsAfter);
		xsBefore = 0;
		xsAfter = 0;
	}
	else if ((kPosInfFloat == xsBefore || kNegInfFloat == xsBefore) &&
		(kPosInfFloat == xsAfter  || kNegInfFloat == xsAfter))
	{
		return; // quick exit for nothing-to-draw case
	}

	bool fPrimary = true;
	if (fForceSplit)
	{
		// Supposedly we can't find both halves of the IP in this segment.
		Assert(xsBefore == kPosInfFloat || xsBefore == kNegInfFloat
			|| xsAfter == kPosInfFloat || xsAfter == kNegInfFloat);
		if (xsBefore == kPosInfFloat || xsBefore == kNegInfFloat)
		{
			// Can't find the char the IP is before.
			fPrimary = fAssocPrev;
		}
		else // if (xsAfter == kPosInfinity || xsAfter == kNegInfinity)
		{
			// Can't find the char the IP is after.
			fPrimary = !fAssocPrev;
		}
		////Assert((dm == kdmSplitPrimary) == fPrimary);
	}

	if (xsBefore == kPosInfinity || xsBefore == kNegInfinity)
	{
		ysBeforeTop = ysSegTop;
		ysBeforeBottom = ysSegTop + m_pseg->m_dysHeight;
	}
	else
	{
		xsBefore += xsSegLeft;
		ysBeforeTop += ysSegTop;
		ysBeforeBottom += ysSegTop;
	}
	if (xsAfter == kNegInfinity || xsAfter == kPosInfinity)
	{
		ysAfterTop = ysSegTop;
		ysAfterBottom = ysSegTop + m_pseg->m_dysHeight;
	}
	else
	{
		xsAfter += xsSegLeft;
		ysAfterTop += ysSegTop;
		ysAfterBottom += ysSegTop;
	}

	float xsLeft = xsSegLeft
		+ ((fRtl && m_pseg->m_fEndLine) ? (m_pseg->m_dxsVisibleWidth - m_pseg->m_dxsTotalWidth) :0);
	float xsRight = xsSegLeft
		+ ((fRtl && m_pseg->m_fEndLine) ? m_pseg->m_dxsVisibleWidth : m_pseg->m_dxsTotalWidth)
		+ m_pseg->m_dxsStretch;
	float dysMinSplitHeight = m_pseg->m_dysHeight / 4;

	if (!fForceSplit)  // kdmNormal == dm)
	{
		//	Standard I-beam, or both halves of split IP.

		bool fIBeam = false;

		if (xsBefore == xsAfter)
			//	Positions coincide within this segment: make an I-beam.
			fIBeam = true;

		else if (fabsf(xsBefore - xsLeft) <= 1 && ichwIP == m_pseg->m_ichwMin)
		{
			//	Logical start is at a left edge: allow I-beam if previous segment has one
			//	at the end (or there is no previous segment).
			//Assert(!fRtlBefore);
			//if (m_pseg->m_psegPrev)
			//{
			//	SegmentPainter segpPrev(m_pseg->m_psegPrev);
			//	fIBeam = segpPrev.doBoundariesCoincide(true, true);
			//}
			//else

			fIBeam = true;
		}
		else if (fabsf(xsBefore - xsRight) <= 1 && ichwIP == m_pseg->m_ichwMin)
		{
			//	Logical start is at a right edge: allow I-beam if previous segment has one
			//	at the end (or there is no previous segment).
			//Assert(fRtlBefore);
			//if (m_pseg->m_psegPrev)
			//{
			//	SegmentPainter segpPrev(m_pseg->m_psegPrev);
			//	fIBeam = segpPrev.doBoundariesCoincide(true, false);
			//}
			//else
			
			fIBeam = true;
		}
		else if (fabsf(xsAfter - xsLeft) <= 1 && ichwIP == ichwLim)
		{
			//	Logical end is at a left edge: allow I-beam if next segment has one
			//	at the beginning (or there is no next segment).
			//Assert(fRtlAfter);
			//if (m_pseg->m_psegNext == NULL)
			//{
			//	SegmentPainter segpPrev(m_pseg->m_psegPrev);
			//	fIBeam = segpPrev.doBoundariesCoincide(false, true);
			//}
			//else

			fIBeam = true;
		}
		else if (fabsf(xsAfter - xsRight) <= 1 && ichwIP == ichwLim)
		{
			//	Logical end is at a right edge: allow I-beam if next segment has one
			//	at the beginning (or there is no next segment).
			//Assert(!fRtlAfter);
			//if (m_pseg->m_psegNext == NULL)
			//{
			//	SegmentPainter segpPrev(m_pseg->m_psegPrev);
			//	fIBeam = segpPrev.doBoundariesCoincide(false, false);
			//}
			//else

			fIBeam = true;
		}
		else
		{
			//	If the horizontal positions of the two halves are sufficiently close,
			//	make an I-beam.
			fIBeam = CloseIPPositions(
				&xsBefore, ysBeforeTop, ysBeforeBottom,
				&xsAfter,  ysAfterTop,  ysAfterBottom);
		}
		if (ResultFailed(res))
			THROW(WARN(res));

		if (fIBeam &&
			((ichwIP == m_pseg->m_ichwMin && fAssocPrev) || (ichwIP == ichwLim && !fAssocPrev)))
		{
			//	Adjacent segment will show the I-beam, so don't show it here.
			if (m_pseg->m_cginf == 0)
			{
				//	Empty segment--assume no adjacent segment.
			}
			else
				return;
		}

		if (fIBeam)
		{	// I-beam caret

			float ysTop = min(ysBeforeTop, ysAfterTop);
			float ysBottom = max(ysBeforeBottom, ysAfterBottom);
			float xsMid = (xsBefore == kPosInfFloat) ? xsAfter : xsBefore;
			InvertIBeam(xsMid, ysTop, ysBottom, fAssocPrev, prdPrimary);
		}
		else
		{	// split caret
			if (fAssocPrev)
			{	// xsAfter is primary, xsBefore is secondary
				if (xsAfter != kNegInfinity)
				{
					InvertSplitIP(xsAfter, ysAfterTop, ysAfterBottom,
						true, fRtlAfter, false, dysMinSplitHeight, prdPrimary);
				}
				if (xsBefore != kPosInfFloat)
				{
					InvertSplitIP(xsBefore, ysBeforeTop, ysBeforeBottom,
						false, !fRtlBefore, true, dysMinSplitHeight, prdSecondary);
				}
			}
			else 
			{	// xsBefore is primary, xsAfter is secondary
				if (xsBefore != kPosInfinity)
				{
					InvertSplitIP(xsBefore, ysBeforeTop, ysBeforeBottom,
						false, !fRtlBefore, false, dysMinSplitHeight, prdPrimary);
				}
				if (xsAfter != kNegInfinity)
				{
					InvertSplitIP(xsAfter, ysAfterTop, ysAfterBottom,
						true, fRtlAfter, true, dysMinSplitHeight, prdSecondary);
				}
			}
			//*pfSecHere = true;
		}
		//*pfPrimaryHere = true;
	}
	else if (fForceSplit && fPrimary) // kdmSplitPrimary == dm)
	{	
		// Draw just the primary caret.
		if (fAssocPrev)
		{	// xsAfter is primary
			if (xsAfter == kNegInfinity)
			{} // no primary
			else
				InvertSplitIP(xsAfter, ysAfterTop, ysAfterBottom,
					true, fRtlAfter, false, dysMinSplitHeight, prdPrimary);
		}
		else
		{	// xsBefore is primary
			if (xsBefore == kPosInfinity)
			{} // no primary
			else
				InvertSplitIP(xsBefore, ysBeforeTop, ysBeforeBottom,
					false, !fRtlBefore, false, dysMinSplitHeight, prdPrimary);
		}
	}
	else // fForceSplit && !fPrimary
	{
		// Draw just the secondary caret.
		//Assert(kdmSplitSecondary == dm);
		if (fAssocPrev)
		{	// xsBefore is secondary
			if (xsBefore == kPosInfinity)
			{} // no secondary
			else
				InvertSplitIP(xsBefore,  ysBeforeTop, ysBeforeBottom,
					false, !fRtlBefore, true, dysMinSplitHeight, prdSecondary);
		}
		else
		{	// xsAfter is secondary
			if (xsAfter == kNegInfinity)
			{} // no secondary
			else
				InvertSplitIP(xsAfter,  ysAfterTop, ysAfterBottom,
					true, fRtlAfter, true, dysMinSplitHeight, prdSecondary);
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Draw an I-beam cursor, or return the location at which it would be drawn.

	Note that currently we do not draw a true I-beam, but rather just a vertical line.

	@param xs			- horizontal center of IP
	@param ysTop		- top of glyph (or ligature component) box
	@param ysBottom		- bottom of glyph (or ligature component) box
	@param fAssocRight	- true if the IP is associated with the text to the right
	@param rs, rd		- source/destination device coordinates
	@param prdRet		- return location to be drawn in dest device coords;
							if NULL, do the drawing
----------------------------------------------------------------------------------------------*/
void SegmentPainter::InvertIBeam(float xs, float ysTop, float ysBottom,
	bool fAssocPrev, Rect * prdRet)
{
	float xd = ScaleXToDest(xs);
	float ydTop = ScaleYToDest(ysTop);
	float ydBottom = ScaleYToDest(ysBottom);

	//	For now, assume that destination coordinates are in terms of pixels.
	float dxdTwoXPixels = 2;

	float dxdLeftAdjust = dxdTwoXPixels / 2;
	float dxdRightAdjust = dxdTwoXPixels - dxdLeftAdjust;

//	int dysFontAscent;
//	pgg->GetFontAscentSourceUnits(pgg, &dysFontAscent);
//	dysFontAscent += m_dysXAscent;
//	if (dysFontAscent < 20)
//	{
//		dxdRightAdjust--;
//		dxdRightHook = 1;
//	}

	if (prdRet)
	{
		prdRet->left = xd - dxdLeftAdjust;
		prdRet->right = xd + dxdRightAdjust;
		prdRet->top = ydTop;
		prdRet->bottom = ydBottom;
	}
	else
	{
		InvertRect(xd-dxdLeftAdjust, ydTop,
			xd+dxdRightAdjust, ydBottom);

		//	To make a true I-beam:
//		InvertRect(xd-dxdTwoXPixels, ydTop-dydOneYPixel,
//			xd+dxdRightHook, ydTop);
//		InvertRect(xd-dxdTwoXPixels, ydBottom,
//			xd+dxdRightHook, ydBottom+dydOneYPixel);
	}
}

/*----------------------------------------------------------------------------------------------
	Draw a split insertion point, or return the location at which it would be drawn.

	@param xs			- horizontal center of IP
	@param ysTop		- top of glyph (or ligature component) box
	@param ysBottom		- bottom of glyph (or ligature component) box
	@param fTop			- true if the IP is to be drawn at the top of the line of text
	@param fAssocRight	- true if the IP is associated with the text to the right
	@param fSecondary	- true if the IP is secondary, false if it is primary
	@param dysMinSplitHeight
						- minimum split cursor height						
	@param rs, rd		- source/destination device coordinates
	@param prdRet		- return location to be drawn in dest device coords;
							if NULL, do the drawing
----------------------------------------------------------------------------------------------*/
void SegmentPainter::InvertSplitIP(float xs, float ysTop, float ysBottom,
	bool fTop, bool fAssocRight, bool fSecondary, float dysMinSplitHeight,
	Rect * prdRet)
{
	if (prdRet == NULL && ysTop == ysBottom)
		return;	// invisible IP

	float xd = ScaleXToDest(xs);
	float ydTop = ScaleYToDest(ysTop);
	float ydBottom = ScaleYToDest(ysBottom);
	float dydMinSplitHeight = dysMinSplitHeight * m_yFactor;

	float ydMid = (ydTop + ydBottom) / 2;

	//	For now, assume that destination coordinates are in terms of pixels.
	float dxdOneXPixel = 1;
	float dydOneYPixel = 1;
	float dxdTwoXPixels = 2;

	float xdLeft, xdRight, dxdHookLen;
	if (fSecondary)
	{
		xdLeft = xd - (dxdOneXPixel / 2);
		xdRight = xdLeft + 1;
		dxdHookLen = 2;
	}
	else
	{
		xdLeft = xd - (dxdTwoXPixels / 2);
		xdRight = xdLeft + 2;
		dxdHookLen = 3;
	}

	float yd1, yd2;
	if (fTop)
	{
		yd1 = ydTop;
		if (ydMid - ydTop < dydMinSplitHeight)
			ydMid = ydTop + dydMinSplitHeight;
		yd2 = (fSecondary) ? ydMid : ydMid + 3;
	}
	else
	{
		yd2 = ydBottom;
		if (ydBottom - ydMid < dydMinSplitHeight)
			ydMid = ydBottom - dydMinSplitHeight;
		yd1 = (fSecondary) ? ydMid : ydMid - 3;
	}

	if (prdRet)
	{
		prdRet->left = xdLeft;
		prdRet->right = xdRight;
		// The most common reason for returning the positions of the IP is to scroll it into
		// view, in which case we want to return the entire line height, not just the 
		// height of the IP itself.
		prdRet->top = ydTop; // yd1;
		prdRet->bottom = ydBottom; // yd2;
	}
	else
	{
		InvertRect(xdLeft, yd1, xdRight, yd2);
	}

	if (fAssocRight)
	{
		if (prdRet)
			prdRet->right = xdRight + dxdHookLen;
		else
		{
			if (fTop)
				InvertRect(xdRight, ydTop,
					xdRight + dxdHookLen, ydTop + dydOneYPixel);
			else
				InvertRect(xdRight, ydBottom-dydOneYPixel,
					xdRight + dxdHookLen, ydBottom);
		}
	}
	else
	{
		if (prdRet)
			prdRet->left = xdLeft - dxdHookLen;
		else
		{
			if (fTop)
				InvertRect(xdLeft - dxdHookLen, ydTop,
					xdLeft, ydTop + dydOneYPixel);
			else
				InvertRect(xdLeft - dxdHookLen, ydBottom - dydOneYPixel,
					xdLeft, ydBottom);
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Given a character position (relative to the beginning of the string), return the
	position for the insertion point, relative to the top-left of the segment.
	Notice that these are before/after positions, not left/right positions.
	Return an infinity value to indicate no appropriate IP.

	@param ichwSel			- selection position, relative to beginning of string
	@param fBefore			- true if we are interested in the character the IP is before.
	@param pxs, pysTop, pysBottom
							- return locations for drawing the IP (top and bottom are not
								adjusted for half-selections)
	@param pfRtl			- return true if the character is right-to-left

	Review: it might be useful to figure out if the two surface glyphs are physically adjacent
	in such a way that their IPs can be combined into one I-beam. For now we use a simple
	rule that assumes that any glyph or ligature component is at least 10% as wide as the
	height of the font (see CloseIPPositions).
----------------------------------------------------------------------------------------------*/
void SegmentPainter::CalcIP(int ichwSel, bool fBefore,
	float * pxs, float * pysTop, float * pysBottom, bool * pfRtl)
{
	Font & font = this->m_pseg->getFont();

	int ichw = (fBefore) ? ichwSel : ichwSel - 1;

	int iginfOutput;

	*pfRtl = m_pseg->CharIsRightToLeft(ichw, fBefore);

	GrEngine * pgreng = m_pseg->EngineImpl();
	GrGlyphTable * pgtbl = (pgreng) ? pgreng->GlyphTable() : NULL;

	float dysFontAscent = m_pseg->m_dysFontAscent;
	////GrResult res = GetFontAscentSourceUnits(pgg, &dysFontAscent);
	////if (ResultFailed(res))
	////	THROW(WARN(res));
	dysFontAscent += m_pseg->m_dysXAscent;

	*pysTop = 0;
	*pysBottom = m_pseg->m_dysHeight;

	int ichwSegOffset = ichw - m_pseg->m_ichwMin;
	int isloutLig;
	if (ichwSegOffset < m_pseg->m_ichwAssocsMin || ichwSegOffset >= m_pseg->m_ichwAssocsLim)
		isloutLig = kNegInfinity;
	else
		isloutLig = m_pseg->m_prgisloutLigature[ichwSegOffset - m_pseg->m_ichwAssocsMin];
	bool fOkLig = (isloutLig != kNegInfinity);
	if (pgtbl && fOkLig)
	{
		//	Ligature component; place the IP at the boundary
		//	of the component box.

		GrSlotOutput * pslout = m_pseg->OutputSlot(isloutLig);
		int icomp = m_pseg->m_prgiComponent[ichwSegOffset - m_pseg->m_ichwAssocsMin];

		iginfOutput = m_pseg->LogicalToPhysicalSurface(isloutLig);

		float xsGlyphOffset = m_pseg->GlyphLeftEdge(iginfOutput);

		float xsLeft, xsRight, ysTop, ysBottom;

		// TODO: Rework ComponentIndexForGlyph to take slati rather than iForGlyph.
		int slati = pslout->ComponentId(icomp);
		int iForGlyph = pgtbl->ComponentIndexForGlyph(pslout->GlyphID(), slati);
		fOkLig = pgtbl->ComponentBoxLogUnits(m_pseg->m_xysEmSquare, pslout->GlyphID(), iForGlyph,
			m_pseg->m_mFontEmUnits, m_pseg->m_dysAscent,
			&xsLeft, &ysTop, &xsRight, &ysBottom);
		if (fOkLig)
		{
			if (fBefore == *pfRtl)
				//	Position of interest is to the right.
				*pxs = xsGlyphOffset + xsRight;
			else
				//	Position of interest is to the left.
				*pxs = xsGlyphOffset + xsLeft;

			GlyphInfo * pginf = m_pseg->m_prgginf + iginfOutput;
			float ysGlyph = pginf->yOffset();

			*pysTop = ysTop + (m_pseg->m_dysAscent - dysFontAscent) - ysGlyph;
			*pysBottom = ysBottom + (m_pseg->m_dysAscent - dysFontAscent) - ysGlyph;
		}
	}
	
	if (!fOkLig)
	{
		//	Not a ligature component.

		int islout = m_pseg->UnderlyingToLogicalSurface(ichw, fBefore);
		GrSlotOutput * psloutTmp = m_pseg->OutputSlot(islout);
		iginfOutput = m_pseg->LogicalToPhysicalSurface(islout);

		if (kPosInfinity == iginfOutput || kNegInfinity == iginfOutput)
			*pxs = kPosInfFloat;

		else
		{
			bool fCluster = (psloutTmp->IsPartOfCluster());
			bool fHighlightBB = fCluster;
			// Test below should match the one in CalcHighlightRect:
			if ((psloutTmp->ClusterAdvance() == 0
				|| psloutTmp->GlyphMetricLogUnits(&font, kgmetAdvWidth) == 0)
				&& !psloutTmp->IsSpace())
			{
				fHighlightBB = true;	// use bounding box for zero-advance glyph
			}
			if (fHighlightBB)
			{
				//	Cluster member or zero-width glyph.

				if (fCluster && AtEdgeOfCluster(psloutTmp, islout, fBefore) &&
					!CanInsertIntoCluster(psloutTmp, islout))
				{
					//	Place the IP at the edge of the whole cluster.

					int isloutBase = psloutTmp->ClusterBase();
					GrSlotOutput * psloutBase = m_pseg->OutputSlot(isloutBase);
					//int igbbBase = psloutBase->GlyphBbIndex();
					int iginfBase = m_pseg->LogicalToPhysicalSurface(isloutBase);
					
					float xsBase = m_pseg->GlyphLeftEdge(iginfBase);

					float xsLeft = xsBase + psloutBase->ClusterXOffset();
					float xsRight = xsBase + psloutBase->ClusterAdvance();

					if (fBefore == *pfRtl)
						//	Position of interest is to the right
						*pxs = xsRight;
					else
						//	Position of interest is to the left.
						*pxs = xsLeft;
				}
				else
				{
					//	Place the IP on the perimeter of the bounding box of the cluster member.

					float xsGlyphStart = m_pseg->GlyphLeftEdge(iginfOutput);

//					float dysFontAscent;
//					res = GetFontAscentSourceUnits(pgg, &dysFontAscent);
//					if (ResultFailed(res))	THROW(WARN(res));
//					dysFontAscent += m_dysXAscent;

					float ysBbTop = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbTop);
					float ysBbBottom = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbBottom);
					float xsBbLeft = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbLeft);
					float xsBbRight = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbRight);

					int twoPixels = 2;	// assume dest coords are in pixels

					if (fBefore == *pfRtl)
						//	Position of interest is to the right
						*pxs = xsGlyphStart + xsBbRight + twoPixels;
					else
						//	Position of interest is to the left.
						*pxs = xsGlyphStart + xsBbLeft - twoPixels;

					GlyphInfo * pginf = m_pseg->m_prgginf + iginfOutput;
					float ysGlyph = pginf->yOffset();
					float ysGlyphBaseline = m_pseg->m_dysAscent - ysGlyph;	// relative to top

					*pysTop = ysGlyphBaseline - ysBbTop - twoPixels;
					*pysBottom = ysGlyphBaseline - ysBbBottom + twoPixels;
				}
			}
			else
			{
				// Insertion point goes at origin or advance width of glyph.
	
				*pxs = m_pseg->GlyphLeftEdge(iginfOutput);
				if (fBefore == *pfRtl)
				{
					//	Position of interest is to the right
					//*pxs += psloutTmp->GlyphMetricLogUnits(&font, kgmetAdvWidth);
					*pxs += psloutTmp->ClusterAdvance();
				}
				// else position of interest is to the left
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------
	If the two positions for the insertion point are close enough, merge them into
	one insertion point, and return true to indicate that we want an I-beam.

	@param pxsBefore, pxsAfter - for comparing, and also adjusted results
	@param ysBeforeTop/Bottom, ysAfterTop/Bottom - for comparing
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::CloseIPPositions(float * pxsBefore, float ysBeforeTop, float ysBeforeBottom,
	float * pxsAfter, float ysAfterTop, float ysAfterBottom)
{
	//	No adjustment if vertical positions are disjoint.
	if (ysBeforeBottom <= ysAfterTop)
		return false;
	if (ysAfterBottom <= ysBeforeTop)
		return false;

	float dysHt = max(ysBeforeBottom, ysAfterBottom) - min(ysBeforeTop, ysAfterTop);
	float dxsDiff = fabsf(*pxsBefore - *pxsAfter);

	//	If the difference in the x positions is less than 10% of the height,
	//	we want an I-beam--use the average.
	if (dxsDiff * 10 < dysHt)
	{
		*pxsBefore = (*pxsBefore + *pxsAfter) / 2;
		*pxsAfter = *pxsBefore;
		return true;
	}

	return false;
}

/*----------------------------------------------------------------------------------------------
	Return true if the slot in question is the first or last slot of its cluster,
	or if it is not part of a cluster at all. Return false if the specified side
	of the slot is interior to a cluster.

	@param fBefore		- if true, we are examining the slot the insertion point is before,
							so we are interested in whether this slot is at the
							leading edge of the cluster.
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::AtEdgeOfCluster(GrSlotOutput * pslout, int islout, bool fBefore)
{
	if (!pslout->IsPartOfCluster())
		return true;

	if (fBefore && (islout == 0))
		return true;
	if (!fBefore && (m_pseg->m_cslout == islout + 1))
		return true;

	//	If the adjacent slot in whatever direction we're interested in has the same
	//	cluster base, this slot is not on the edge of the cluster.
	//  -- NO, because you could have an intervening glyph that is not part of the
	//	cluster.
	//int isloutAdjacentBase = (fBefore) ?
	//	m_pseg->OutputSlot(islout - 1)->ClusterBase() :
	//	m_pseg->OutputSlot(islout + 1)->ClusterBase();
	//return (isloutAdjacentBase != m_pseg->OutputSlot(islout)->ClusterBase());

	// Ask the cluster base.
	return AtEdgeOfCluster(m_pseg->OutputSlot(pslout->ClusterBase()), pslout->ClusterBase(),
		pslout, islout, fBefore);
}

bool SegmentPainter::AtEdgeOfCluster(GrSlotOutput * psloutBase, int isloutBase,
	GrSlotOutput * pslout, int islout, bool fBefore)
{
	//	Compare pslout to all the members of the cluster. If it is the minimum or maximum, it
	//	is at an edge.
	if (fBefore && isloutBase < islout)
		return false;
	if (!fBefore && isloutBase > islout)
		return false;

	std::vector<int> visloutCluster;
	m_pseg->ClusterMembersForGlyph(isloutBase, psloutBase->ClusterRange(), visloutCluster);

	for (size_t iislout = 0; iislout < visloutCluster.size(); iislout++)
	{
		int isloutMember = visloutCluster[iislout];
		if (fBefore && isloutMember < islout)
			return false;
		if (!fBefore && isloutMember > islout)
			return false;
	}
	return true;
}

/*----------------------------------------------------------------------------------------------
	Return true if it is permitted to put an insertion point somewhere in the middle of
	the given slot's cluster.
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::CanInsertIntoCluster(GrSlotOutput * pslout, int islout)
{
	if (!pslout->IsPartOfCluster())
		return false;
	
	int isloutBase = pslout->ClusterBase();
	if (islout != isloutBase)
	{
		//	Ask the cluster base.
		Assert(pslout->ClusterRange() == 0);
		return CanInsertIntoCluster(m_pseg->OutputSlot(isloutBase), isloutBase);
	}

	if (!AtEdgeOfCluster(pslout, islout, true) && pslout->InsertBefore())
		return true;

	std::vector<int> visloutCluster;
	m_pseg->ClusterMembersForGlyph(isloutBase, pslout->ClusterRange(), visloutCluster);

	for (size_t iislout = 0; iislout < visloutCluster.size(); iislout++)
	{
		int isloutMember = visloutCluster[iislout];
		if (AtEdgeOfCluster(m_pseg->OutputSlot(isloutMember), isloutMember, true))
			// Inserting before the first member of the cluster doesn't count.
			continue;

		if (m_pseg->OutputSlot(isloutMember)->InsertBefore())
			return true;
	}

	return false;
}

/*----------------------------------------------------------------------------------------------
	Determine which characters in the selected range are part of ligatures that are fully
	selected vs. those that are only partially selected. Those that are only partially selected
	must have their components highlighted individually, but those that are fully selected
	should be highlighted fully, to avoid blank spots in the case that the components do not
	fully cover the surface of the glyph.

	@param prgfAllSelected		- array of flags to fill in with results
	@param ichwMin/LimSeg		- range of characters to examine
	@param ichwMin/LimSel		- range of characters that are selected
----------------------------------------------------------------------------------------------*/
void SegmentPainter::CalcPartialLigatures(bool * prgfAllSelected,
	int ichwMinSeg, int ichwLimSeg,
	int ichwMinSel, int ichwLimSel)
{
	GrEngine * pgreng = m_pseg->EngineImpl();
	GrGlyphTable * pgtbl = (pgreng) ? pgreng->GlyphTable() : NULL;

	for (int ichw = ichwMinSeg; ichw < ichwLimSeg; ichw++)
	{
		int ichwSegOffset = ichw - m_pseg->m_ichwMin;
		int isloutLig;
		if (ichwSegOffset < m_pseg->m_ichwAssocsMin || ichwSegOffset >= m_pseg->m_ichwAssocsLim)
			isloutLig = kNegInfinity;
		else
			isloutLig = m_pseg->m_prgisloutLigature[ichwSegOffset - m_pseg->m_ichwAssocsMin];
		bool fLigComp = (pgtbl && isloutLig != kNegInfinity);
		if (fLigComp && !prgfAllSelected[ichwSegOffset])
		{
			GrSlotOutput * pslout = m_pseg->OutputSlot(isloutLig);
			bool fAll = true;
			int icomp;
			for (icomp = 0; icomp < pslout->NumberOfComponents(); icomp++)
			{
				int ichwUnderlying = pslout->FirstUnderlyingComponent(icomp); // is it possible to select one char of a component and not another?
				if (ichwUnderlying < ichwMinSel - m_pseg->m_ichwMin ||
					ichwUnderlying >= ichwLimSel - m_pseg->m_ichwMin)
				{
					fAll = false;	// found a component that is not selected
				}
			}
			for (icomp = 0; icomp < pslout->NumberOfComponents(); icomp++)
			{
				for (int ichw = pslout->FirstUnderlyingComponent(icomp) ; 
					ichw <= pslout->LastUnderlyingComponent(icomp) ;
					ichw++)
				{
					if (m_pseg->m_prgiComponent[ichw - m_pseg->m_ichwAssocsMin] == icomp)
						prgfAllSelected[ichw] = fAll;
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------------------------
	The given character is part of the selected range. Add to the vector a rectangle (or
	eventually, several) indicating the corresponding screen area(s) of the character.
	The rectangle is relative to the top left of the segment.

	@param ichw				- selected char, relative to beginning of string
	@param vrs				- list of rectangles to highlight, in source coords, relative to
								segment draw position (top-left)
	@param vfEntireHt		- for each of the above rectangles, true if the rect should be
								extended to include the entire line
	@param fJustComponent	- true if we want to highlight just the single ligature component 
								this character maps to; false if the entire ligature is selected
	@param rgfHighlighted	- for each glyph in this segment, whether it is being highlighted
								by this operation
	@param fSkipTrSpace		- should we skip trailing white space?
----------------------------------------------------------------------------------------------*/
void SegmentPainter::CalcHighlightRect(int ichw,
	std::vector<Rect> & vrs, std::vector<bool> &  vfEntireHt,
	bool fJustComponent, bool * rgfHighlighted, bool fSkipTrSpace)
{
	GrResult res;

	GrEngine * pgreng = m_pseg->EngineImpl();
	GrGlyphTable * pgtbl = (pgreng) ? pgreng->GlyphTable() : NULL;
	//bool fRtl = m_pseg->rightToLeft();

	Font & font = m_pseg->getFont();

	if (!GrCharStream::AtUnicodeCharBoundary(m_pseg->GetString(), ichw))
		//	Second half of a surrogate pair, or UTF-8: ignore.
		return;

	//	Calculate the location for the glyph that this IP is before.

	int ichwSegOffset = ichw - m_pseg->m_ichwMin;
	int isloutLig;
	if (ichwSegOffset < m_pseg->m_ichwAssocsMin || ichwSegOffset >= m_pseg->m_ichwAssocsLim)
		isloutLig = kNegInfinity;
	else
		isloutLig = m_pseg->m_prgisloutLigature[ichwSegOffset - m_pseg->m_ichwAssocsMin];
	bool fLigComp = (pgtbl && isloutLig != kNegInfinity);
	if (fLigComp && fJustComponent)
	{
		//	Character is a ligature component.

		GrSlotOutput * pslout = m_pseg->OutputSlot(isloutLig);
		int icomp = m_pseg->m_prgiComponent[ichwSegOffset - m_pseg->m_ichwAssocsMin];
		int iginfOutput = m_pseg->LogicalToPhysicalSurface(isloutLig);

		float xsGlyphStart = m_pseg->GlyphLeftEdge(iginfOutput);

		float xsLeft, xsRight, ysTop, ysBottom;

		// TODO: Rework ComponentIndexForGlyph to take slati rather than iForGlyph.
		int slati = pslout->ComponentId(icomp);
		int iForGlyph = pgtbl->ComponentIndexForGlyph(pslout->GlyphID(), slati);
		pgtbl->ComponentBoxLogUnits(m_pseg->m_xysEmSquare, pslout->GlyphID(), iForGlyph,
			m_pseg->m_mFontEmUnits, m_pseg->m_dysAscent,
			&xsLeft, &ysTop, &xsRight, &ysBottom);

		float dysFontAscent = m_pseg->m_dysFontAscent;
		////GetFontAscentSourceUnits(pgg, &dysFontAscent);
		dysFontAscent += m_pseg->m_dysXAscent;
		GlyphInfo * pginf = m_pseg->m_prgginf + iginfOutput;
		float ysForGlyph = pginf->yOffset();
		float dysGlyphTop = (m_pseg->m_dysAscent - dysFontAscent) - ysForGlyph; // rel. to common baseline

		Rect rsNew;
		rsNew.top = ysTop + dysGlyphTop;
		rsNew.bottom = ysBottom + dysGlyphTop;
		rsNew.left = xsGlyphStart + xsLeft;
		rsNew.right = xsGlyphStart + xsRight;
		vrs.push_back(rsNew);

		vfEntireHt.push_back(false);
	}
	else
	{
		std::vector<int> vislout = m_pseg->UnderlyingToLogicalAssocs(ichw);
		if (vislout.size() == 0)
			return;

		float xsGlyphLeft, xsGlyphRight;

		res = kresOk;

		for (size_t iislout = 0; iislout < vislout.size(); iislout++)
		{
			int islout = vislout[iislout];
			if (islout == kNegInfinity || islout == kPosInfinity)
				continue;
			int iginfTmp = m_pseg->LogicalToPhysicalSurface(islout);
			Assert(iginfTmp != kNegInfinity);
			Assert(iginfTmp != kPosInfinity);

			if (fSkipTrSpace && islout >= m_pseg->m_isloutVisLim)
				continue;

			Rect rsNew;
			GrSlotOutput * psloutTmp = m_pseg->OutputSlot(islout);
			bool fHighlightBB = psloutTmp->IsPartOfCluster();
			//	Test below should match the one in CalcIP--
			//	If the glyph has no advance width but does have a real bounding box,
			//	highlight the bounding box. Review: do we want this???
			if ((psloutTmp->ClusterAdvance() == 0
				|| psloutTmp->GlyphMetricLogUnits(&font, kgmetAdvWidth) == 0)
				&& !psloutTmp->IsSpace())
			{
				fHighlightBB = true;
			}
			xsGlyphLeft = m_pseg->GlyphLeftEdge(iginfTmp);
			if (fHighlightBB)
			{
				//	For now, highlight just the bounding box of this glyph. After we finish 
				//	highlighting all glyphs individually, we'll go back and highlight
				//	as one unit any clusters that are completely selected.

//				int dysFontAscent;
//				res = GetFontAscentSourceUnits(pgg, &dysFontAscent); // if we restore this code, add in ExtraAscent
//				if (ResultFailed(res)) THROW(WARN(res));
//				dysFontAscent += m_dysXAscent;

				GlyphInfo * pginf = m_pseg->m_prgginf + iginfTmp;
				float ysForGlyph = pginf->yOffset();
				float ysGlyphBaseline = m_pseg->m_dysAscent - ysForGlyph;	// relative to top

				float ysBbTop = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbTop);
				float ysBbBottom = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbBottom);
				float xsBbLeft = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbLeft);
				float xsBbRight = psloutTmp->GlyphMetricLogUnits(&font, kgmetBbRight);

				rsNew.top = ysGlyphBaseline - ysBbTop - 2;
				rsNew.bottom = ysGlyphBaseline - ysBbBottom + 2;
				rsNew.left = xsGlyphLeft + xsBbLeft - 2;
				rsNew.right = xsGlyphLeft + xsBbRight + 2;

				vfEntireHt.push_back(false);
			}
			else
			{
				//	Highlight entire width of the glyph from the top of the line to the bottom.
				//	Horizontally, highlight from the origin of the glyph to the advance width.

				//xsGlyphRight = xsGlyphLeft + psloutTmp->GlyphMetricLogUnits(&font, kgmetAdvWidth);
				xsGlyphRight = xsGlyphLeft + psloutTmp->ClusterAdvance();

				if (xsGlyphLeft > xsGlyphRight)
					std::swap(xsGlyphLeft, xsGlyphRight);
				
				rsNew.top = 0;
				rsNew.bottom = m_pseg->m_dysHeight;
				rsNew.left = xsGlyphLeft;
				rsNew.right = xsGlyphRight;

				vfEntireHt.push_back(true);
			}
			vrs.push_back(rsNew);

			if (rgfHighlighted)
				rgfHighlighted[iginfTmp] = true;
		}
	}
}

/*----------------------------------------------------------------------------------------------
	If the output slot is a cluster base and all the members of the cluster are highlighted,
	simply highlight the cluster as a single rectangle, to avoid unslightly gaps
	between glyphs. (Note that this does not remove the rectangles that were already included
	to handle the individual members.)
	The rectangles are relative to the top left of the segment.

	@param islout			- output slot index
	@param vrect			- list of rectangles to highlight; add result here
	@param prgfHighlighted	- flags indicating which glyphs have been highlighted, indexed by
								physical surface position
----------------------------------------------------------------------------------------------*/
void SegmentPainter::CalcCompleteCluster(int islout,
	std::vector<Rect> & vrs, std::vector<bool> & vfEntireHt, bool * prgfHighlighted)
{
	//GrEngine * pgreng = m_pseg->EngineImpl();

	GrSlotOutput * pslout = m_pseg->OutputSlot(islout);
	if (pslout->ClusterRange() == 0)
		return;

	int iginfDraw = m_pseg->LogicalToPhysicalSurface(islout);
	if (!prgfHighlighted[iginfDraw])
		return;

	bool fAllHighlighted = true;

	std::vector<int> visloutCluster;
	m_pseg->ClusterMembersForGlyph(islout, pslout->ClusterRange(), visloutCluster);
	for (size_t i = 0; i < visloutCluster.size(); i++)
	{
		int iginfMemberDraw = m_pseg->LogicalToPhysicalSurface(visloutCluster[i]);
		if (!prgfHighlighted[iginfMemberDraw])
		{
			fAllHighlighted = false;
			break;
		}
	}

	if (!fAllHighlighted)
		return;

	//	All the members of this cluster are highlighted.

	float xsGlyphStart = m_pseg->GlyphLeftEdge(iginfDraw);

	Rect rsNew;
	rsNew.top = 0;
	rsNew.bottom = m_pseg->m_dysHeight;
	rsNew.left = xsGlyphStart + pslout->ClusterXOffset();
	rsNew.right = xsGlyphStart + pslout->ClusterAdvance();
	vrs.push_back(rsNew);

	vfEntireHt.push_back(true);
}

/*----------------------------------------------------------------------------------------------
	Add the given rectangle to the vector, eliminating any overlapping areas.

	Note this method can be recursive: if we have to break the current rectangle into
	two, we'll call the method again the for new 
----------------------------------------------------------------------------------------------*/
void SegmentPainter::AddRectWithoutOverlaps(std::vector<Rect> & vrect, Rect rectToAdd)
{
	std::vector<Rect> vrectMoreToAdd;

	bool fAnythingToAdd = AnyArea(&rectToAdd);

	size_t irect;
	for (irect = 0; irect < vrect.size(); irect++)
	{
		bool f = AdjustRectsToNotOverlap(vrect, irect, &rectToAdd, vrectMoreToAdd);
		if (!AnyArea(&vrect[irect]))
		{
			vrect.erase(vrect.begin() + irect);
			irect--;
		}
		if (!f)
		{
			fAnythingToAdd = false;
			break;
		}
	}

	//	Now rectToAdd is in a state that it does not intersect with any other rectangles
	//	in the list.

	if (fAnythingToAdd)
		vrect.push_back(rectToAdd);

	//	Add in any extra rectangles that are needed to handle overlaps.

	for (irect = 0; irect < vrectMoreToAdd.size(); irect++)
		AddRectWithoutOverlaps(vrect, vrectMoreToAdd[irect]);
}

/*----------------------------------------------------------------------------------------------
	Return true if the rectangle has an area greater than 0.
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::AnyArea(Rect * prect)
{
	Assert(prect->left <= prect->right);
	Assert(prect->top <= prect->bottom);

	return (prect->left < prect->right && prect->top < prect->bottom);
}

/*----------------------------------------------------------------------------------------------
	Adjust the rectangle prect2 to avoid overlaps with the given one in the vector.
	Return false if the rectangle does not need to be added after all.

	If we have to split this rectangle into two, add the second one to vrectMoreToAdd, so
	we can do the same process on it.

	If we can merge the two rectangles, add the result to vrectMoreToAdd (so it can be properly
	checked for further overlaps), and adjust the rectangle in the vector so it will be
	deleted.

	@param vrect			- current list of valid rectangles
	@param irect			- item in vrect we're comparing with prect2
	@param prect2			- rectangle to add
	@param vrectMoreToAdd	- list of accumulated rectangles that need to be added
								(caused by splitting one into two)

	@return True if we want to add prect2 (possibly adjusted) to the list;
	false it it is completely subsumed by vrect[irect], or has been merged with it, 
	or has no area.
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::AdjustRectsToNotOverlap(std::vector<Rect> & vrect, int irect, Rect * prect2,
	std::vector<Rect> & vrectMoreToAdd)
{
	Rect * prect1 = &(vrect[irect]);

	float xLeft1 = prect1->left;
	float xRight1 = prect1->right;
	float yTop1 = prect1->top;
	float yBottom1 = prect1->bottom;
	Assert(xLeft1 < xRight1);
	Assert(yTop1 < yBottom1);

	float xLeft2 = prect2->left;
	float xRight2 = prect2->right;
	float yTop2 = prect2->top;
	float yBottom2 = prect2->bottom;
	Assert(xLeft2 <= xRight2);
	Assert(yTop2 <= yBottom2);

	if (!AnyArea(prect2))
		return false;

	//	No overlap: keep both as they are.
	if (yBottom2 < yTop1 || yBottom1 < yTop2)
		return true;
	if (xRight2 < xLeft1 || xRight1 < xLeft2)
		return true;

	//	Present rect subsumes new: ignore new
	if (xLeft1 <= xLeft2 && xRight2 <= xRight1 && yTop1 <= yTop2 && yBottom2 <= yBottom1)
		return false;

	//	New rect subsumes present: replace.
	if (xLeft2 <= xLeft1 && xRight1 <= xRight2 && yTop2 <= yTop1 && yBottom1 <= yBottom2)
	{
		prect1->bottom = prect1->top;	// delete
		return true;	// keep this one
	}

	//	Abutting rectangles of same height or width: merge.
	if (yTop1 == yTop2 && yBottom1 == yBottom2)
	{
		if (xRight1 == xLeft2)
		{
			//	prect2 is to the right
			prect2->left = prect1->left;
			vrectMoreToAdd.push_back(*prect2);
			prect1->bottom = prect1->top;	// delete previous
			return false;
		}
		else if (xRight2 == xLeft1)
		{
			//	prect2 is to the left
			prect2->right = prect1->right;
			vrectMoreToAdd.push_back(*prect2);
			prect1->bottom = prect1->top;	// delete
			return false;
		}
	}
	if (xLeft1 == xLeft2 && xRight1 == xRight2)
	{
		if (yBottom1 == yTop2)
		{
			//	prect2 is below
			prect2->top = prect1->top;
			vrectMoreToAdd.push_back(*prect2);
			prect1->bottom = prect1->top;	// delete
			return false;
		}
		else if (yBottom2 == yTop1)
		{
			//	prect2 is above
			prect2->bottom = prect1->bottom;
			vrectMoreToAdd.push_back(*prect2);
			prect1->bottom = prect1->top;	// delete
			return false;
		}
	}

	//	Abutting rectangles of different heights or widths: leave as they are.
	if (xRight1 == xLeft2 || xRight2 == xLeft1)
		return true;
	if (yBottom1 == yTop2 || yBottom2 == yTop1)
		return true;

	//	Overlapping on one axis: adjust smaller rectangle
	if (yTop1 <= yTop2 && yBottom2 <= yBottom1)
	{
		if (xLeft1 <= xLeft2 && xLeft2 < xRight1)
		{
			Assert(xRight1 < xRight2);	// otherwise rect2 is subsumed
			prect2->left = prect1->right;
			return true;
		}
		else if (xLeft1 < xRight2 && xRight2 <= xRight1)
		{
			Assert(xLeft2 < xLeft1);	// otherwise rect2 is subsumed
			prect2->right = prect1->left;
			return true;
		}
	}
	else if (yTop2 <= yTop1 && yBottom1 <= yBottom2)
	{
		if (xLeft2 <= xLeft1 && xLeft1 < xRight2)
		{
			Assert(xRight2 < xRight1);	// otherwise rect1 is subsumed
			prect1->left = prect2->right;
			return true;
		}
		else if (xLeft2 < xRight1 && xRight1 <= xRight2)
		{
			Assert(xLeft1 < xLeft2);	// otherwise rect1 is subsumed
			prect1->right = prect2->left;
			return true;
		}
	}

	if (xLeft1 <= xLeft2 && xRight2 <= xRight1)
	{
		if (yTop1 <= yTop2 && yTop2 < yBottom1)
		{
			Assert(yBottom1 < yBottom2);	// otherwise rect2 is subsumed
			prect2->top = prect1->bottom;
			return true;
		}
		else if (yTop1 < yBottom2 && yBottom2 <= yBottom1)
		{
			Assert(yTop2 < yTop1);	// otherwise rect2 is subsumed
			prect2->bottom = prect1->top;
			return true;
		}
	}
	else if (xLeft2 <= xLeft1 && xRight1 <= xRight2)
	{
		if (yTop2 <= yTop1 && yTop1 < yBottom2)
		{
			Assert(yBottom2 < yBottom1);	// otherwise rect1 is subsumed
			prect1->top = prect2->bottom;
			return true;
		}
		else if (yTop2 < yBottom1 && yBottom1 <= yBottom2)
		{
			Assert(yTop1 < yTop2);	// otherwise rect1 is subsumed
			prect1->bottom = prect2->top;
			return true;
		}
	}

	//	Centers overlap: break prect2 into two disjoint rectangles.
	if (xLeft1 < xLeft2 && xRight2 < xRight1 && yTop2 < yTop1 && yBottom1 < yBottom2)
	{
		//	create a new rectangle for the bottom of prect2 that is below prect1
		Rect rectNew = *prect2;
		rectNew.top = prect1->bottom;
		prect2->bottom = prect1->top;
		vrectMoreToAdd.push_back(rectNew);
		return true;
	}
	else if (xLeft2 < xLeft1 && xRight1 < xRight2 && yTop1 < yTop2 && yBottom2 < yBottom1)
	{
		//	create a new rectangle for the right of prect2 that is to the right of prect1
		Rect rectNew = *prect2;
		rectNew.left = prect1->right;
		prect2->right = prect1->left;
		vrectMoreToAdd.push_back(rectNew);
		return true;
	}

	//	Corners overlap: break prect2 into two adjacent rectangles.
	if (xLeft1 < xLeft2 && xRight1 < xRight2)
	{
		//	prect2 is partially to the right
		if (yTop1 < yTop2 && yBottom1 < yBottom2)
		{
			//	prect2 is partially below
			Rect rectNew = *prect2;
			rectNew.left = prect1->right;
			prect2->top = prect1->bottom;
			vrectMoreToAdd.push_back(rectNew);
			return true;
		}
		else if (yTop2 < yTop1 && yBottom2 < yBottom1)
		{
			//	prect2 is partially above
			Rect rectNew = *prect2;
			rectNew.left = prect1->right;
			prect2->bottom = prect1->top;
			vrectMoreToAdd.push_back(rectNew);
			return true;
		}
	}
	else if (xLeft2 < xLeft1 && xRight2 < xRight1)
	{
		//	prect2 is partially to the left
		if (yTop1 < yTop2 && yBottom1 < yBottom2)
		{
			//	prect2 is partially below
			Rect rectNew = *prect2;
			rectNew.right = prect1->left;
			prect2->top = prect1->bottom;
			vrectMoreToAdd.push_back(rectNew);
			return true;
		}
		else if (yTop2 < yTop1 && yBottom2 < yBottom1)
		{
			//	prect2 is partially above
			Rect rectNew = *prect2;
			rectNew.right = prect1->left;
			prect2->bottom = prect1->top;
			vrectMoreToAdd.push_back(rectNew);
			return true;
		}
	}

	//	No overlap.
	return true;
}

/*----------------------------------------------------------------------------------------------
	Assert that there are no overlaps among all the rectangles in the array, which should
	be the case if AdjustRectsToNotOverlap is working properly.
----------------------------------------------------------------------------------------------*/
void SegmentPainter::AssertNoOverlaps(std::vector<Rect> & vrect)
{
#ifdef _DEBUG
	for (int irect1 = 0; irect1 < signed(vrect.size() - 1); irect1++)
	{
		for (int irect2 = irect1 + 1; irect2 < signed(vrect.size()); irect2++)
		{
			float xLeft1 = vrect[irect1].left;
			float xRight1 = vrect[irect1].right;
			float yTop1 = vrect[irect1].top;
			float yBottom1 = vrect[irect1].bottom;
			Assert(xLeft1 < xRight1);
			Assert(yTop1 < yBottom1);

			float xLeft2 = vrect[irect2].left;
			float xRight2 = vrect[irect2].right;
			float yTop2 = vrect[irect2].top;
			float yBottom2 = vrect[irect2].bottom;
			Assert(xLeft2 <= xRight2);
			Assert(yTop2 <= yBottom2);

			Assert(xRight1 <= xLeft2 || xRight2 <= xLeft1 ||
				yBottom1 <= yTop2 || yBottom2 <= yTop1);
		}
	}
#endif
}

/*----------------------------------------------------------------------------------------------
	Add the given line segment to the vector, eliminating any overlapping areas.
----------------------------------------------------------------------------------------------*/
void SegmentPainter::AddLineSegWithoutOverlaps(std::vector<LineSeg> & vls, LineSeg lsToAdd)
{
	std::vector<LineSeg> vlsMoreToAdd;

	bool fAnythingToAdd = AnyLength(&lsToAdd);

	size_t ils;
	for (ils = 0; ils < vls.size(); ils++)
	{
		bool f = AdjustLineSegsToNotOverlap(vls, ils, &lsToAdd, vlsMoreToAdd);
		if (!AnyLength(&vls[ils]))
		{
			vls.erase(vls.begin() + ils);
			ils--;
		}
		if (!f)
		{
			fAnythingToAdd = false;
			break;
		}
	}

	//	Now lsToAdd is in a state that it does not overlap with any other line segments
	//	in the list.

	if (fAnythingToAdd)
		vls.push_back(lsToAdd);

	//	Add in any extra line segments that are needed to handle overlaps.

	for (ils = 0; ils < vlsMoreToAdd.size(); ils++)
		AddLineSegWithoutOverlaps(vls, vlsMoreToAdd[ils]);
}

/*----------------------------------------------------------------------------------------------
	Return true if the rectangle has an area greater than 0.
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::AnyLength(LineSeg * pls)
{
	Assert(pls->left <= pls->right);
	return (pls->left < pls->right);
}

/*----------------------------------------------------------------------------------------------
	Adjust the line segment pls2 to avoid overlaps with the given one in the vector.
	Return false if the line segment does not need to be added after all.

	If we can merge the two line segments, add the result to vrectMoreToAdd (so it can be properly
	checked for further overlaps), and adjust the rectangle in the vector so it will be
	deleted.

	@param vls				- current list of valid line segments
	@param ils				- item in vrect we're comparing with pls2
	@param pls2				- line segment to add
	@param vlsMoreToAdd		- list of accumulated line segments that need to be added
								(caused by splitting one into two)

	@return True if we want to add pls2 (possibly adjusted) to the list;
	false it it is completely subsumed by vls[ils], or has been merged with it, 
	or has no length.
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::AdjustLineSegsToNotOverlap(std::vector<LineSeg> & vls, int ils, LineSeg * pls2,
	std::vector<LineSeg> & vlsMoreToAdd)
{
	LineSeg * pls1 = &(vls[ils]);

	float xLeft1 = pls1->left;
	float xRight1 = pls1->right;
	Assert(xLeft1 < xRight1);

	float xLeft2 = pls2->left;
	float xRight2 = pls2->right;
	Assert(xLeft2 <= xRight2);

	if (!AnyLength(pls2))
		return false;

	//	No overlap: keep both as they are.
	if (xRight2 < xLeft1 || xRight1 < xLeft2)
		return true;

	//	Present line segment subsumes new: ignore new
	if (xLeft1 <= xLeft2 && xRight2 <= xRight1)
		return false;

	//	New line segment subsumes present: replace.
	if (xLeft2 <= xLeft1 && xRight1 <= xRight2)
	{
		pls1->left = pls1->right;	// delete previous
		return true;	// keep this one
	}

	//	Adjacent or partially overlapping line segments: merge.
	if (xLeft1 <= xLeft2 && xLeft2 <= xRight1 && xRight1 <= xRight2)
	{
		pls2->left = xLeft1;
		vlsMoreToAdd.push_back(*pls2);
		pls1->left = pls1->right;	// delete previous
		return false;
	}
	else if (xLeft2 <= xLeft1 && xLeft1 <= xRight2 && xRight2 <= xRight1)
	{
		pls2->right = xRight1;
		vlsMoreToAdd.push_back(*pls2);
		pls1->left = pls1->right;	// delete previous
		return false;
	}
	else
		Assert(false);

	return true;
}

/*----------------------------------------------------------------------------------------------
	Assert that there are no overlaps among all the rectangles in the array, which should
	be the case if AdjustRectsToNotOverlap is working properly.
----------------------------------------------------------------------------------------------*/
void SegmentPainter::AssertNoOverlaps(std::vector<LineSeg> & vls)
{
#ifdef _DEBUG
	for (int ils1 = 0; ils1 < (int)vls.size() - 1; ils1++)
	{
		for (int ils2 = ils1 + 1; ils2 < (int)vls.size(); ils2++)
		{
			float xLeft1 = vls[ils1].left;
			float xRight1 = vls[ils1].right;
			Assert(xLeft1 < xRight1);

			float xLeft2 = vls[ils2].left;
			float xRight2 = vls[ils2].right;
			Assert(xLeft2 <= xRight2);

			Assert(xRight1 <= xLeft2 || xRight2 <= xLeft1);
		}
	}
#endif
}

/*----------------------------------------------------------------------------------------------
	Find the next physical location for the IP. Return true if we found one.

	@param pichw			- current insertion point, and adjusted result to return
	@param pfAssocPrev		- is the IP "leaning" backwards? adjust to return
	@param fRight			- true if we are moving right
	@param fAssocPrevMatch	- the assoc-prev of the result must match the original selection.
								This is used when extending range selections; the orientation
								of the resulting end-point must be the same as previously.
								This helps avoids circular loops when dealing with reordering.
	@param fAssocPrevNeeded - value needed for result; ignored if fAssocPrevMatch == false
	@param pnNextOrPrevSeg	- return 1 if we need to try the following segment, -1 if we
								need to try the previous - CURRENTLY NOT USED for anything
								practical
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::ArrowKeyPositionInternal(
	int * pichw, bool * pfAssocPrev,
	bool fRight, bool fAssocPrevMatch, bool fAssocPrevNeeded, int * pnNextOrPrevSeg)
{
	int kBefore = true;
	int kAfter = false;

	bool fResult = false;

	//	Handle ligatures.

	fResult = AdjacentLigComponent(pichw, pfAssocPrev, fRight, true);
	if (fResult)
		return true;	

	//	Not within a ligature, or on the outside edge of one.

	int iginfStart;
	bool fCharRtl;
	if (*pfAssocPrev)
	{
		iginfStart = m_pseg->UnderlyingToPhysicalSurface(*pichw - 1, kAfter);
		fCharRtl = m_pseg->CharIsRightToLeft(*pichw - 1, kAfter);
	}
	else
	{
		iginfStart = m_pseg->UnderlyingToPhysicalSurface(*pichw, kBefore);
		fCharRtl = m_pseg->CharIsRightToLeft(*pichw, kBefore);
	}
	if (kNegInfinity == iginfStart || kPosInfinity == iginfStart)
	{
		*pnNextOrPrevSeg = 0;
		return false;
	}

	//if (*pfAssocPrev)
	//	fCharRtl ? iginfStart : iginfStart++;
	//else
	//	fCharRtl ? iginfStart++ : iginfStart;

	if (*pfAssocPrev)
		iginfStart++;

	//	At this point iginfStart indicates the index of the glyph we are before.

	int ichwNewIndex;
	bool fAPNew;

	bool fRtl = m_pseg->rightToLeft();
	bool fForward = (fRtl) ? !fRight : fRight;
	int dir = (fForward) ? 1 : -1;
	int diginf = 0;

	//	When moving forward, we first move to the opposite side of the current glyph,
	//	so initially diginf should be zero.
	if (fForward)
		diginf = -1;

	while (true)
	{
		diginf++;
		int iginfTry = iginfStart + (dir * diginf);

		if (iginfTry < 0)
		{
			*pnNextOrPrevSeg = -1;
			return false;
		}
		if (iginfTry > m_pseg->m_cginf)
		{
			*pnNextOrPrevSeg = 1;
			return false;
		}
		if (iginfTry == m_pseg->m_cginf && fForward)
		{
			*pnNextOrPrevSeg = 1;
			return false;
		}

		bool fAfter;
		int ichwTryChar;
		if (iginfTry == m_pseg->m_cginf)
		{
			ichwTryChar = m_pseg->PhysicalSurfaceToUnderlying(iginfTry - 1,
				kPosInfFloat, kPosInfFloat, 0, 0, &fAfter);
		}
		else
		{
			ichwTryChar = m_pseg->PhysicalSurfaceToUnderlying(iginfTry,
				(fRight) ? kPosInfFloat : kNegInfFloat,
				(fRight) ? kPosInfFloat : kNegInfFloat,
				0, 0, &fAfter);
		}

		if (fAfter)
		{
			ichwNewIndex = ichwTryChar + 1;
			fAPNew = true;
		}
		else
		{
			ichwNewIndex = ichwTryChar;
			fAPNew = false;
		}

		if (ichwNewIndex == *pichw)
			continue;	// didn't make any progress

		LgIpValidResult ipvr = isValidInsertionPoint(ichwNewIndex);
		if (ipvr == kipvrBad)
			continue;	// bad insertion point

		if (fAssocPrevMatch && (fAssocPrevNeeded != fAPNew))
		{
			//	Try switching fAPNew and see if that gives the right result
			bool fAPAlt = !fAPNew;
			int iginfAlt = m_pseg->UnderlyingToPhysicalSurface(
				fAPAlt ? ichwTryChar - 1 : ichwTryChar + 1,
				fAPAlt ? kAfter : kBefore);
			fCharRtl = m_pseg->CharIsRightToLeft(
				fAPAlt ? ichwTryChar - 1 : ichwTryChar + 1,
				fAPAlt ? kAfter : kBefore);

			if (fAPAlt)
				fCharRtl ? iginfAlt : iginfAlt++;
			else
				fCharRtl ? iginfAlt++ : iginfAlt;

			if ((fForward && iginfAlt <= iginfStart)
				|| (!fForward && iginfAlt >= iginfStart))
			{
				//	Switching fAPNew would cause us to move in the opposite direction, so
				//	that's not a good solution. Keep going.
				if ((iginfStart == 0 && !fForward) || (iginfStart == m_pseg->m_cginf && fForward))
				{
					//	Can't go any further, so go with this even though AP is wrong.
				}
				else
					continue;
			}
			fAPNew = fAPAlt;
		}
		break;
	}

	//	If we have a selection within a ligature, adjust to either the right-most or the
	//	left-most.
	AdjacentLigComponent(&ichwNewIndex, &fAPNew, fRight, false);

	*pichw = ichwNewIndex;
	*pfAssocPrev = fAPNew;
	return true;
}

/****** OLD VERSION
bool Segment::ArrowKeyPositionInternal(
	int * pichw, bool * pfAssocPrev,
	bool fRight, bool fAssocPrevMatch, bool fAssocPrevNeeded, int * pnNextOrPrevSeg)
{
	int kBefore = true;
	int kAfter = false;

	bool fResult = false;

	//	Handle ligatures.

	fResult = AdjacentLigComponent(pichw, pfAssocPrev, fRight, true);
	if (fResult)
		return true;	

	//	Not within a ligature, or on the outside edge of one.

	int igbbOutput;
	bool fCharRtl;
	if (*pfAssocPrev)
	{
		igbbOutput = UnderlyingToPhysicalSurface(*pichw - 1, kAfter);
		fCharRtl = CharIsRightToLeft(*pichw - 1, kAfter);
	}
	else
	{
		igbbOutput = UnderlyingToPhysicalSurface(*pichw, kBefore);
		fCharRtl = CharIsRightToLeft(*pichw, kBefore);
	}
	if (kNegInfinity == igbbOutput || kPosInfinity == igbbOutput)
	{
		*pnNextOrPrevSeg = 0;
		return false;
	}

	if (*pfAssocPrev)
		fCharRtl ? igbbOutput : igbbOutput++;
	else
		fCharRtl ? igbbOutput++ : igbbOutput;

	bool fBackward = (fRight == m_fWsRtl);

	//	Try to find an underlying position that would produce an IP in the expected place.
	//	First look in the same direction in the underlying text, then, if necessary, try the
	//	opposite direction (which may be necessary when reordering occurred).

	int igbbNeed = (fRight) ? igbbOutput + 1 : igbbOutput - 1;
	int ichwNew;
	bool fAssocPrevNew;

	while (0 <= igbbNeed && igbbNeed <= NumGbb())	// include end-points--we may need to
													// be before or after them.
	{
		int dichwInc = fBackward ? -1 : 1;
		int dichwLim = m_ichwMin + (fBackward ? 0 : m_dichwLim);

		int nDirectionsTried = 0;
//		while (nDirectionsTried < 2) -- Nope, for now we are being very strict above moving
		while (nDirectionsTried < 1)		// only in the desired direction
		{
			for (int ichwTry = *pichw;
				((dichwInc == 1 && ichwTry <= dichwLim) ||
					(dichwInc == -1 && ichwTry >= dichwLim));
				ichwTry += dichwInc)
			{
				int igbbTry = UnderlyingToPhysicalSurface(ichwTry, kBefore);
				fCharRtl = CharIsRightToLeft(ichwTry, kBefore);
				igbbTry = fCharRtl ? igbbTry + 1 : igbbTry;
				if (igbbTry == igbbNeed &&
					(ichwTry != *pichw || *pfAssocPrev) &&
					(!fAssocPrevMatch || !fAssocPrevNeeded))
				{
					ichwNew = ichwTry;
					fAssocPrevNew = false;
					goto LFixLigatures;
				}

				igbbTry = UnderlyingToPhysicalSurface(ichwTry - 1, kAfter);
				fCharRtl = CharIsRightToLeft(ichwTry - 1, kAfter);
				igbbTry = fCharRtl ? igbbTry : igbbTry + 1;
				if (igbbTry == igbbNeed &&
					(ichwTry != *pichw || !*pfAssocPrev) &&
					(!fAssocPrevMatch || fAssocPrevNeeded))
				{
					ichwNew = ichwTry;
					fAssocPrevNew = true;
					goto LFixLigatures;
				}
			}

			nDirectionsTried++;

			//	Didn't find an answer going the expected way; try the opposite direction.
			dichwInc = (fRight) ? -1 : 1;
			dichwLim = m_ichwMin + ((fRight) ? 0 : m_dichwLim);
		}
			
		//	Didn't find an underlying position that would produce an IP in the next glyph;
		//	try the glyph after that.

		igbbNeed = (fRight) ? igbbNeed + 1 : igbbNeed - 1;
	}

	if (igbbNeed >= NumGbb())
		*pnNextOrPrevSeg = 1;	// try next segment
	else if (igbbNeed < 0)
		*pnNextOrPrevSeg = -1;	// try previous segment
	else
	{
		Assert(false);
		*pnNextOrPrevSeg = 0;
	}

	return false;

LFixLigatures:

	//	If we have a selection within a ligature, adjust to either the right-most or the
	//	left-most.
	AdjacentLigComponent(&ichwNew, &fAssocPrevNew, fRight, false);

	*pichw = ichwNew;
	*pfAssocPrev = fAssocPrevNew;
	return true;
}
******************/

/*----------------------------------------------------------------------------------------------
	Return position for the physically next or previous ligature component, or the right-most
	or left-most. Return true if this routine handled it.

	@param pichw			- starting underlying position
	@param pfAssocPrev		- is selection leaning backwards?
	@param fMovingRight		- true if right arrow key was pressed, false if left
	@param fMove			- if true, we want to find the next one over from *pichw;
								if false, we want to find either the right-most
								(fMovingRight = false)
								or the left-most (fMovingRight = true)
----------------------------------------------------------------------------------------------*/
bool SegmentPainter::AdjacentLigComponent(int * pichw, bool * pfAssocPrev,
	bool fMovingRight, bool fMove)
{
	int kBefore = true;
	int kAfter = false;

	GrEngine * pgreng = m_pseg->EngineImpl();
	GrGlyphTable * pgtbl = (pgreng) ? pgreng->GlyphTable() : NULL;

	if (!pgtbl)
	{
		return false;
	}

	//	Find the slot containing the ligature glyph.

	int ichwSegOffset = *pichw - m_pseg->m_ichwMin;
	int isloutLig = kNegInfinity;
	if (*pfAssocPrev)
	{
		if (ichwSegOffset - 1 >= m_pseg->m_ichwAssocsMin && ichwSegOffset  - 1 < m_pseg->m_ichwAssocsLim)
			isloutLig = m_pseg->m_prgisloutLigature[ichwSegOffset - 1 - m_pseg->m_ichwAssocsMin];
	}
	else
	{
		if (ichwSegOffset >= m_pseg->m_ichwAssocsMin && ichwSegOffset < m_pseg->m_ichwAssocsLim)
			isloutLig = m_pseg->m_prgisloutLigature[ichwSegOffset - m_pseg->m_ichwAssocsMin];
	}

	if (isloutLig == kNegInfinity)
	{
		return false;
	}	

	if (*pfAssocPrev)
		ichwSegOffset--;

	bool fGlyphRtl = (*pfAssocPrev) ?
		m_pseg->CharIsRightToLeft(*pichw - 1, kAfter) :
		m_pseg->CharIsRightToLeft(*pichw, kBefore);

	//	Comparing right-sides.
	bool fIPOnRight = (fGlyphRtl != *pfAssocPrev);

	//	When two ligatures are horizontally equal, take vertical position into account.
	//	In left-to-right writing systems, moving right means moving down.
	bool fMovingDown = (fMovingRight != m_pseg->m_fWsRtl);

	//	Get the coordinates for all the ligatures.

	std::vector<float> vysTops;
	std::vector<float> vysBottoms;
	std::vector<float> vxsLefts;
	std::vector<float> vxsRights;

	float ysTopCurr, ysBottomCurr, xsLeftCurr, xsRightCurr;
	int icompCurr = -1;
	if (fMovingRight)
		ysTopCurr = ysBottomCurr = xsLeftCurr = xsRightCurr = kNegInfFloat;
	else
		ysTopCurr = ysBottomCurr = xsLeftCurr = xsRightCurr = kPosInfFloat;

	GrSlotOutput * pslout = m_pseg->OutputSlot(isloutLig);
	//int iginfOutput = m_pseg->LogicalToPhysicalSurface(isloutLig);

	int icomp;
	for (icomp = 0; icomp < pslout->NumberOfComponents(); icomp++)
	{
		float xsLeft, ysTop, xsRight, ysBottom;

		// TODO: Rework ComponentIndexForGlyph to take slati rather than iForGlyph.
		int slati = pslout->ComponentId(icomp);
		int iForGlyph = pgtbl->ComponentIndexForGlyph(pslout->GlyphID(), slati);
		pgtbl->ComponentBoxLogUnits(m_pseg->m_xysEmSquare, pslout->GlyphID(), iForGlyph,
			m_pseg->m_mFontEmUnits, m_pseg->m_dysAscent,
			&xsLeft, &ysTop, &xsRight, &ysBottom);

		vysTops.push_back(ysTop);
		vysBottoms.push_back(ysBottom);
		vxsLefts.push_back(xsLeft);
		vxsRights.push_back(xsRight);

		//	If we're moving, record this component as the one we're moving relative to.
		if (fMove && icomp == m_pseg->m_prgiComponent[ichwSegOffset - m_pseg->m_ichwAssocsMin])
		{
			icompCurr = icomp;
			ysTopCurr = ysTop;
			ysBottomCurr = ysBottom;
			xsLeftCurr = xsLeft;
			xsRightCurr = xsRight;
		}
	}

	float xsHorizCurr = (fIPOnRight) ? xsRightCurr : xsLeftCurr;
	float xsHorizNext;

	//	Find the first ligature that is in the proper direction.
	int icompNext;
	for (icompNext = 0; icompNext < pslout->NumberOfComponents(); icompNext++)
	{
		if (icompNext != icompCurr)
		{
			float xsHorizNext = (fIPOnRight) ? vxsRights[icompNext] : vxsLefts[icompNext];
			if (fMovingRight && xsHorizNext > xsHorizCurr)
				break;
			else if (!fMovingRight && xsHorizNext < xsHorizCurr)
				break;
		}
	}
	if (icompNext == pslout->NumberOfComponents())
	{
		bool fResult;
		//	No component in the proper direction. Can we switch to the opposite edge of the
		//	current component?
		if (fMovingRight && !fIPOnRight)
		{
			if (fGlyphRtl)
				--(*pichw);
			else
				++(*pichw);
			*pfAssocPrev = !*pfAssocPrev;
			fResult = true;
		}
		else if (!fMovingRight && fIPOnRight)
		{
			if (fGlyphRtl)
				++(*pichw);
			else
				--(*pichw);

			*pfAssocPrev = !*pfAssocPrev;
			fResult = true;
		}
		else
			fResult = false;
		return fResult;
	}

	xsHorizNext = (fIPOnRight) ? vxsRights[icompNext] : vxsLefts[icompNext];

	//	Now find the closest one in the proper direction.
	for (icomp = icompNext + 1; icomp < pslout->NumberOfComponents(); icomp++)
	{
		if (icomp != icompCurr)
		{
			float xsHorizTry = (fIPOnRight) ? vxsRights[icomp] : vxsLefts[icomp];

			if (fMovingRight && xsHorizTry > xsHorizCurr &&
				(xsHorizTry < xsHorizNext ||
					(xsHorizTry == xsHorizNext && 
						(vysTops[icomp] > vysTops[icompNext]) == fMovingDown)))
			{
				icompNext = icomp;
				xsHorizNext = xsHorizTry;
			}
			else if (!fMovingRight && xsHorizTry < xsHorizCurr &&
				(xsHorizTry > xsHorizNext ||
					(xsHorizTry == xsHorizNext &&
						(vysTops[icomp] > vysTops[icompNext]) == fMovingDown)))
			{
				icompNext = icomp;
				xsHorizNext = xsHorizTry;
			}
		}
	}

	*pichw = pslout->FirstUnderlyingComponent(icompNext) + m_pseg->m_ichwMin; // TODO: handle multiple components per ligature
	if (*pfAssocPrev)
		++(*pichw);

	return true;
}

/*----------------------------------------------------------------------------------------------
	Scale the x-coordinate by the scaling factors for this painter.
----------------------------------------------------------------------------------------------*/
float SegmentPainter::ScaleX(float xs, Rect rs, Rect rd)
{
	//return rs.MapXTo(xs, rd);

	float dxs = rs.right - rs.left;
	float dxd = rd.right - rd.left;
	Assert(dxs > 0);
	if (dxs == dxd)
		return xs + rd.left - rs.left;

	float xd = rd.left + GrEngine::GrFloatMulDiv((xs - rs.left), dxd, dxs);
	return xd;
}

/*----------------------------------------------------------------------------------------------
	Scale the y-coordinate by the scaling factors for this painter.
----------------------------------------------------------------------------------------------*/
float SegmentPainter::ScaleY(float ys, Rect rs, Rect rd)
{
	//return rs.MapYTo(ys, rd);

	float dys = rs.bottom - rs.top;
	float dyd = rd.bottom - rd.top;
	Assert(dys > 0);
	if (dys == dyd)
		return ys + rd.top - rs.top;
	float yd = rd.top + GrEngine::GrFloatMulDiv((ys - rs.top), dyd, dys);
	return yd;
}


//:>********************************************************************************************
//:>	Low-level platform-specific drawing methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Make sure the font is set to use the character properties required by this segment.
----------------------------------------------------------------------------------------------*/
void SegmentPainter::SetFontProps(unsigned long clrFore, unsigned long clrBack)
{
	return;
}

} // namespace gr
