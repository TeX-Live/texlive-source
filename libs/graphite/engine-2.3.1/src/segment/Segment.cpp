/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Segment.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Implements a Graphite segment--a range of rendered text in one writing system, that can be
	rendered with a single font, and that fits on a single line.
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
#if 0  // TODO remove
namespace 
{

static bool g_fDrawing;

} // namespace
#endif


static int g_csegTotal = 0;

namespace gr
{

//:>********************************************************************************************
//:>	Segment methods
//:>********************************************************************************************


int Segment::GetSegmentCount()  /// TEMP
{
	return g_csegTotal;
}


/*----------------------------------------------------------------------------------------------
	Constructor: basic initialization.
----------------------------------------------------------------------------------------------*/
Segment::Segment() : 
	m_pgts(NULL),
	m_pfont(NULL),
	m_preneng(NULL),
	m_fErroneous(false),
	m_pgjus(NULL),
	m_prgbNextSegDat(NULL),
	m_cbNextSegDat(0),
	m_prgInitDat(NULL),
	m_cbInitDat(0),
//	m_fUseSepBase(0),
	//m_psegAltEndLine(NULL),
	m_prgisloutBefore(NULL),
	m_prgisloutAfter(NULL),
	m_prgpvisloutAssocs(NULL),
	m_prgisloutLigature(NULL),
	m_prgiComponent(NULL),
//	m_psstrm(NULL),
	m_cslout(0),
	m_prgslout(NULL),
	m_prgnSlotVarLenBuf(NULL),
//	m_cnUserDefn(0),
	m_cnCompPerLig(0),
//	m_cnFeat(0),
	m_cginf(0),
	m_prgginf(NULL)
{
	m_cref = 1;

	g_csegTotal++;

//	m_stuFaceName.erase();
//	m_stuBaseFaceName.erase();
	m_vnSkipOffsets.clear();
}

/*----------------------------------------------------------------------------------------------
	Reference counting.
----------------------------------------------------------------------------------------------*/

long Segment::IncRefCount(void)
{
	AssertPtr(this);
	return InterlockedIncrement(&m_cref);
}

long Segment::DecRefCount(void)
{
	AssertPtr(this);
	long cref = InterlockedDecrement(&m_cref);
	if (cref == 0) {
		m_cref = 1;
		delete this;
	}
	return cref;
}

/*----------------------------------------------------------------------------------------------
	Constructor to fit as much of the text on the line as possible,
	finding a reasonable break point if necessary.
----------------------------------------------------------------------------------------------*/
LineFillSegment::LineFillSegment(
		Font * pfont,
		ITextSource * pts,
		LayoutEnvironment * playoutArg,
		toffset ichStart,
		toffset ichStop,
		float xsMaxWidth,
		bool fBacktracking)
	: Segment()
{
	if (!pfont)
		throw;
	if (!pts)
		throw;

	LayoutEnvironment layoutDefault;
	LayoutEnvironment * playout = (!playoutArg) ? &layoutDefault : playoutArg;

	pfont->RenderLineFillSegment(this, pts, *playout, ichStart, ichStop, xsMaxWidth,
		fBacktracking);
}

/*----------------------------------------------------------------------------------------------
	Constructor to create a segment representing the given range of characters.
----------------------------------------------------------------------------------------------*/
RangeSegment::RangeSegment(
		Font * pfont,
		ITextSource * pts,
		LayoutEnvironment * playoutArg,
		toffset ichStart,
		toffset ichStop,
		Segment * psegInitLike)
	: Segment()
{
	if (!pfont)
		throw;
	if (!pts)
		throw;

	LayoutEnvironment layoutDefault;
	LayoutEnvironment * playout = (!playoutArg) ? &layoutDefault : playoutArg;
	playout->setSegmentForInit(psegInitLike);

	pfont->RenderRangeSegment(this, pts, *playout, ichStart, ichStop);

	playout->setSegmentForInit(NULL);
}

/*----------------------------------------------------------------------------------------------
	Constructor to create a justified segment. Private.
----------------------------------------------------------------------------------------------*/
JustSegmentAux::JustSegmentAux(
		Font * pfont,
		ITextSource * pts,
		LayoutEnvironment * playoutArg,
		toffset ichStart,
		toffset ichStop,
		float xsNaturalWidth,
		float xsJustifiedWidth,
		Segment * psegInitLike)
	: Segment()
{
	if (!pfont)
		throw;
	if (!pts)
		throw;

	LayoutEnvironment layoutDefault;
	LayoutEnvironment * playout = (!playoutArg) ? &layoutDefault : playoutArg;
	playout->setSegmentForInit(psegInitLike);

	pfont->RenderJustifiedSegment(this, pts, *playout, ichStart, ichStop,
		xsNaturalWidth, xsJustifiedWidth);

	playout->setSegmentForInit(NULL);
}

//Segment::Segment(ITextSource * pgts, int ichwMin, int ichwLim,
//	LineBrk lbStart, LineBrk lbEnd,
//	bool fStartLine, bool fEndLine, bool fWsRtl)
//{
//	m_cref = 1;
//	Initialize(pgts, ichwMin, ichwLim, lbStart, lbEnd, fStartLine, fEndLine, fWsRtl);
//}

void Segment::Initialize(ITextSource * pgts, int ichwMin, int ichwLim,
	LineBrk lbStart, LineBrk lbEnd, SegEnd est,
	bool fStartLine, bool fEndLine, bool fWsRtl)
{
	AssertPtrN(pgts);
	Assert(ichwMin >= 0);
	Assert(ichwLim >= 0);
	Assert(ichwMin <= ichwLim);

	//pgts->TextSrcObject(&m_pgts); // create a permanent wrapper, if necessary
	//	m_pgts->AddRef(); no, smart pointer
	m_pgts = pgts;

	m_ichwMin = ichwMin;
	m_dichwLim = ichwLim - ichwMin;
	m_lbStart = lbStart;
	m_lbEnd = lbEnd;
	m_est = est;
	m_dxsStretch = 0;
	m_dxsWidth = -1;
	m_dysAscent = -1;	// to properly init, ComputeDimensions must be called
	m_dysHeight = -1;
	m_dysXAscent = -1;
	m_dysXDescent = -1;
	m_dysAscentOverhang = -1;
	m_dysDescentOverhang = -1;
	m_dxsLeftOverhang = -1;
	m_dxsRightOverhang = -1;
	m_dysOffset = 0;
	m_fStartLine = fStartLine;
	m_fEndLine = fEndLine;
	m_fWsRtl = fWsRtl;

	m_dxsVisibleWidth = -1;
	m_dxsTotalWidth = -1;

//	m_psstrm = NULL;
	m_prgslout = NULL;
	m_prgnSlotVarLenBuf = NULL;
	m_prgisloutBefore = NULL;
	m_prgisloutAfter = NULL;
	m_prgpvisloutAssocs = NULL;
	m_prgisloutLigature = NULL;
	m_prgiComponent = NULL;

	m_prgbNextSegDat = NULL;
	m_cbNextSegDat = 0;

	//m_psegAltEndLine = NULL;

//	m_stuFaceName.erase();
//	m_stuBaseFaceName.erase();
//	m_fUseSepBase = false;

//	m_psegPrev = NULL;	// obsolete
//	m_psegNext = NULL;

	//InitializePlatform();
}

/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
Segment::~Segment()
{
	g_csegTotal--;

	DestroyContents();
}

void Segment::DestroyContents()
{
	delete m_pfont;
	m_pfont = NULL;

	//if (m_pgts)
	//	// A specially-created wrapper object has to be deleted.
	//	m_pgts->DeleteTextSrcPtr();
	m_pgts = NULL;

	//if (m_pgjus)
	//	// A specially-created wrapper object has to be deleted.
	//	m_pgjus->DeleteJustifierPtr();
	m_pgjus = NULL;

	//if (m_psegAltEndLine)
	//{
	//	if (m_psegAltEndLine->m_psegAltEndLine == this)
	//		m_psegAltEndLine->m_psegAltEndLine = NULL;
	//	delete m_psegAltEndLine;
	//}

	//for (int iginf = 0; iginf < m_cginf; iginf++) // DELETE
	//	delete m_prgginf[iginf].components;

	Assert((m_prgslout && m_prgnSlotVarLenBuf) || (!m_prgslout && !m_prgnSlotVarLenBuf));
	delete[] m_prgslout;
	delete[] m_prgnSlotVarLenBuf;
	delete[] m_prgisloutBefore;
	delete[] m_prgisloutAfter;
	for (int ichw = 0; ichw < m_ichwAssocsLim - m_ichwAssocsMin; ichw++)
	{
		if (m_prgpvisloutAssocs && m_prgpvisloutAssocs[ichw])
			delete m_prgpvisloutAssocs[ichw];
	}
	delete[] m_prgpvisloutAssocs;
	delete[] m_prgisloutLigature;
	delete[] m_prgiComponent;
	delete[] m_prgbNextSegDat;
	delete[] m_prgInitDat;
	delete[] m_prgginf;
	m_cginf = 0;

	//DestroyContentsPlatform();

	//	ReleaseObj(m_pgts); automatic because smart pointer
}

/*----------------------------------------------------------------------------------------------
	Factory method to create a new version of an existing segment with different
	line-boundary flags.
----------------------------------------------------------------------------------------------*/
Segment * Segment::LineContextSegment(Segment & seg, bool fStartLine, bool fEndLine)
{
	if (!seg.hasLineBoundaryContext())
	{
		// Make an identical segment and just change the flags.
		Segment * psegNew = new Segment(seg);
		psegNew->InitLineContextSegment(fStartLine, fEndLine);
		return psegNew;
	}
	else
	{
		// Rerun the processing.
		LayoutEnvironment layout = seg.Layout();
		layout.setStartOfLine(fStartLine);
		layout.setEndOfLine(fEndLine);
		ITextSource & gts = seg.getText();
		Font & font = seg.getFont();
		return new RangeSegment(&font, &gts, &layout,
			seg.startCharacter(), seg.stopCharacter(), &seg);
	}
}

void Segment::InitLineContextSegment(bool fStartLine, bool fEndLine)
{
	// For RTL end-of-line segments, trailing white space hangs off the left of
	// the origin of the segment. So when changing that flag
	// shift appropriately.
	bool fShift = ((m_nDirDepth % 2) && (m_fEndLine != fEndLine));

	m_fStartLine = fStartLine;
	m_fEndLine = fEndLine;
	
	m_layout.setStartOfLine(fStartLine);
	m_layout.setEndOfLine(fEndLine);

	if (fShift)
	{
		(m_fEndLine) ?
			ShiftGlyphs(m_dxsVisibleWidth - m_dxsTotalWidth) :	// shift left
			ShiftGlyphs(m_dxsTotalWidth - m_dxsVisibleWidth);	// shift right
	}

	m_dxsWidth = -1;
}

/*----------------------------------------------------------------------------------------------
	A factory method to create a justified version of an existing segment.
----------------------------------------------------------------------------------------------*/
Segment * Segment::JustifiedSegment(Segment & seg, float xsNewWidth)
{
	LayoutEnvironment layout(seg.Layout());
	ITextSource & gts = seg.getText();

	// Why do we have to do this?
	layout.setJustifier(seg.Justifier());
	Font & font = seg.getFont();
	return new JustSegmentAux(&font, &gts, &layout,
		seg.startCharacter(), seg.stopCharacter(),
		seg.advanceWidth(), xsNewWidth, &seg);
}

/*----------------------------------------------------------------------------------------------
	A factory method to create a new version of an existing trailing white-space segment
	with a different direction.
----------------------------------------------------------------------------------------------*/
Segment * Segment::WhiteSpaceSegment(Segment & seg, int nNewDepth)
{
	Segment * psegNew = new Segment(seg);
	psegNew->InitWhiteSpaceSegment(nNewDepth);
	return psegNew;
}

void Segment::InitWhiteSpaceSegment(int nNewDepth)
{
	if (nNewDepth == m_nDirDepth)
	{
		return;
	}
	else if ((nNewDepth % 2) == (m_nDirDepth % 2))
	{
		// Same direction, not much to change.
		m_nDirDepth = nNewDepth;
		return;
	}
	else if (m_twsh != ktwshOnlyWs)
		return;	// couldn't change it, oh, well

	// Otherwise, do the hard stuff: reverse the positions of the glyphs.
	for (int islout = 0; islout < m_cslout; islout++)
	{
		OutputSlot(islout)->ShiftForDirDepthChange(m_dxsTotalWidth);
	}
	m_nDirDepth = nNewDepth;

	//SetDirectionDepthPlatform(nNewDepth);
}

/*----------------------------------------------------------------------------------------------
	Basic copy method.
----------------------------------------------------------------------------------------------*/
Segment::Segment(const Segment & seg)
{
	int islout;

	m_pgts = seg.m_pgts;
	m_dichwLim = seg.m_dichwLim;
	m_ichwMin = seg.m_ichwMin;
	m_pfont = seg.m_pfont->copyThis();
	m_preneng = seg.m_preneng;
	m_fErroneous = seg.m_fErroneous;
	m_pgjus = seg.m_pgjus;
	m_fWsRtl = seg.m_fWsRtl;
	m_fParaRtl = seg.m_fParaRtl;
	m_twsh = seg.m_twsh;
	m_nDirDepth = seg.m_nDirDepth;
	m_cbNextSegDat = seg.m_cbNextSegDat;
	m_prgbNextSegDat = new byte[m_cbNextSegDat];
	std::copy(seg.m_prgbNextSegDat, seg.m_prgbNextSegDat + m_cbNextSegDat, m_prgbNextSegDat);
//	m_psegPrev = seg.m_psegPrev;
//	m_psegNext = seg.m_psegNext;
//	m_stuFaceName = seg.m_stuFaceName;
//	m_stuBaseFaceName = seg.m_stuBaseFaceName;
//	m_fUseSepBase = seg.m_fUseSepBase;
//	m_pixHeight = seg.m_pixHeight;
//	m_fBold = seg.m_fBold;
//	m_fItalic = seg.m_fItalic;
	m_lbStart = seg.m_lbStart;
	m_lbEnd = seg.m_lbEnd;
	m_fStartLine = seg.m_fStartLine;
	m_fEndLine = seg.m_fEndLine;
	m_est = seg.m_est;
	m_mFontEmUnits = seg.m_mFontEmUnits;
	m_dysFontAscent = seg.m_dysFontAscent;
	m_dysFontDescent = seg.m_dysFontDescent;
	m_xysEmSquare = seg.m_xysEmSquare;
//	m_xsDPI = seg.m_xsDPI;
//	m_ysDPI = seg.m_ysDPI;
	m_dxsStretch = seg.m_dxsStretch;
	m_dxsWidth = seg.m_dxsWidth;
	m_dysHeight = seg.m_dysHeight;
	m_dysAscent = seg.m_dysAscent;
	m_dysXAscent = seg.m_dysXAscent;
	m_dysXDescent = seg.m_dysXDescent;
	m_dysAscentOverhang = seg.m_dysAscentOverhang;
	m_dysDescentOverhang = seg.m_dysDescentOverhang;
	m_dxsLeftOverhang = seg.m_dxsLeftOverhang;
	m_dxsRightOverhang = seg.m_dxsRightOverhang;
	m_dxsVisibleWidth = seg.m_dxsVisibleWidth;
	m_dxsTotalWidth = seg.m_dxsTotalWidth;
	m_isloutVisLim = seg.m_isloutVisLim;
	m_dysOffset = seg.m_dysOffset;

	m_ichwAssocsMin = seg.m_ichwAssocsMin;
	m_ichwAssocsLim = seg.m_ichwAssocsLim;

	int dichw = m_ichwAssocsLim - m_ichwAssocsMin;
	m_prgisloutBefore = new int[dichw];
	std::copy(seg.m_prgisloutBefore, seg.m_prgisloutBefore + dichw, m_prgisloutBefore);
	m_prgisloutAfter = new int[dichw];
	std::copy(seg.m_prgisloutAfter, seg.m_prgisloutAfter + dichw, m_prgisloutAfter);
	m_prgpvisloutAssocs = new std::vector<int> * [dichw];
	for (int ivislout = 0; ivislout < dichw; ivislout++)
	{
		std::vector<int> * pvislout = seg.m_prgpvisloutAssocs[ivislout];
		if (pvislout)
		{
			std::vector<int> * pvisloutNew = new std::vector<int>;
			m_prgpvisloutAssocs[ivislout] = pvisloutNew;
            *pvisloutNew = *pvislout; // copy the vector
		}
	}
	m_prgisloutLigature = new int[dichw];
	std::copy(seg.m_prgisloutLigature, seg.m_prgisloutLigature + dichw, m_prgisloutLigature);
	m_prgiComponent = new sdata8[dichw];
	std::copy(seg.m_prgiComponent, seg.m_prgiComponent + dichw, m_prgiComponent);

	m_cslout = seg.m_cslout;
	int cnExtraPerSlot = (m_cslout > 0) ? seg.m_prgslout[0].CExtraSpaceSlout() : 0;
	m_prgnSlotVarLenBuf = new u_intslot[m_cslout * cnExtraPerSlot];
	m_prgslout = new GrSlotOutput[m_cslout];
	for (islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * psloutOrig = seg.m_prgslout + islout;
		GrSlotOutput * psloutThis = m_prgslout + islout;
		psloutThis->ExactCopyFrom(psloutOrig,
			m_prgnSlotVarLenBuf + (islout * cnExtraPerSlot), // var-length buffer location
			cnExtraPerSlot);
	}

//	m_cnUserDefn = seg.m_cnUserDefn;
	m_cnCompPerLig = seg.m_cnCompPerLig;
//	m_cnFeat = seg.m_cnFeat;
	m_cslotRestartBackup = seg.m_cslotRestartBackup;
	std::copy(seg.m_prgnSlotVarLenBuf, seg.m_prgnSlotVarLenBuf + 
				(m_cslout * cnExtraPerSlot), m_prgnSlotVarLenBuf);

	m_cginf = seg.m_cginf;
	m_isloutGinf0 = seg.m_isloutGinf0;
	m_prgginf = new GlyphInfo[m_cginf];
	std::copy(seg.m_prgginf, seg.m_prgginf + m_cginf, m_prgginf);
	for (int iginf = 0; iginf < m_cginf; iginf++)
	{
		m_prgginf[iginf].m_pseg = this;
		m_prgginf[iginf].m_pslout = m_prgslout + m_prgginf[iginf].m_islout;
	}

	// Platform-specific; remove eventually:
	//m_cgstrm = seg.m_cgstrm;
	//m_prggstrm = new GlyphStrm[m_cgstrm];
	//for (int igstrm = 0; igstrm < m_cgstrm; igstrm++)
	//{
	//	m_prggstrm[igstrm].gsk.ys = seg.m_prggstrm[igstrm].gsk.ys;
	//	m_prggstrm[igstrm].gsk.clrFore = seg.m_prggstrm[igstrm].gsk.clrFore;
	//	m_prggstrm[igstrm].gsk.clrBack = seg.m_prggstrm[igstrm].gsk.clrBack;
	//	m_prggstrm[igstrm].vchwGlyphId = seg.m_prggstrm[igstrm].vchwGlyphId;
	//	m_prggstrm[igstrm].vdxs = seg.m_prggstrm[igstrm].vdxs;
	//	m_prggstrm[igstrm].vigbb = seg.m_prggstrm[igstrm].vigbb;
	//	m_prggstrm[igstrm].xsStart = seg.m_prggstrm[igstrm].xsStart;
	//	m_prggstrm[igstrm].xsStartInt = seg.m_prggstrm[igstrm].xsStartInt;
	//}
	//m_cgbb = seg.m_cgbb;
	//m_prggbb = new GlyphBb[m_cgbb];
	//memcpy(m_prggbb, seg.m_prggbb, m_cgbb * sizeof(GlyphBb));
	//-------------------------------------

	m_vnSkipOffsets = seg.m_vnSkipOffsets;
	m_dircPrevStrong = seg.m_dircPrevStrong;
	m_dircPrevTerm = seg.m_dircPrevTerm;
	m_cbInitDat = seg.m_cbInitDat;
	m_prgInitDat = new byte[m_cbInitDat];
	std::copy(seg.m_prgInitDat, seg.m_prgInitDat + m_cbInitDat, m_prgInitDat);
}

/*----------------------------------------------------------------------------------------------
	Swap the guts of the two segments.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void Segment::SwapWith(Segment * pgrseg)
{
	int crefThis = m_cref;
	int crefOther = pgrseg->m_cref;

	std::swap(*this, *pgrseg);

	m_cref = crefThis;
	pgrseg->m_cref = crefOther;
}

/*----------------------------------------------------------------------------------------------
	Delete the pointer to the alternate-end-of-line segment.
----------------------------------------------------------------------------------------------*/
void Segment::ClearAltEndLineSeg()
{
	//if (m_psegAltEndLine)
	//{
	//	if (m_psegAltEndLine->m_psegAltEndLine == this)
	//		m_psegAltEndLine->m_psegAltEndLine = NULL; // so we don't get into a loop
	//	delete m_psegAltEndLine;
	//	m_psegAltEndLine = NULL;
	//}
}


//:>********************************************************************************************
//:>	Interface methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Return the font that was used to create this segment.
	Note that in some implementations, the font may be invalid; eg, on Windows the font
	may not contain a valid device context.
----------------------------------------------------------------------------------------------*/
Font & Segment::getFont()
{
	return *m_pfont;
}

/*----------------------------------------------------------------------------------------------
	Return the text-source that the segment is representing.
----------------------------------------------------------------------------------------------*/
ITextSource & Segment::getText()
{
	return *m_pgts;
}

/*----------------------------------------------------------------------------------------------
	The distance the drawing point should advance after drawing this segment.
	Always positive, even for RtoL segments.
----------------------------------------------------------------------------------------------*/
float Segment::advanceWidth()
{
	if (m_dxsWidth < 0)
	{
		////SetUpGraphics(ichwBase, pgg, true);
		ComputeDimensions();
	}
	return m_dxsWidth;
}

/*----------------------------------------------------------------------------------------------
	Compute the rectangle in destination coords which contains all the pixels drawn by
	this segment. This should be a sufficient rectangle to invalidate if the segment is
	about to be discarded.
----------------------------------------------------------------------------------------------*/
Rect Segment::boundingRect()
{
	if (m_dxsWidth < 0)
	{
		////SetUpGraphics(ichwBase, pgg, true);
		ComputeDimensions();
	}

	Rect rectRet;
	rectRet.top = m_dysAscent + m_dysAscentOverhang;
	rectRet.bottom = (m_dysAscent - m_dysHeight) - m_dysDescentOverhang;
	rectRet.left = m_dxsLeftOverhang;
	rectRet.right = m_dxsVisibleWidth + m_dxsRightOverhang;

	return rectRet;
}

/*----------------------------------------------------------------------------------------------
	Answer whether the primary direction of the segment is right-to-left. This is based
	on the direction of the writing system, except for white-space-only segments, in
	which case it is based on the main paragraph direction.
----------------------------------------------------------------------------------------------*/
bool Segment::rightToLeft()
{
	if (m_twsh == ktwshOnlyWs)
	{
		//	White-space-only segment: use main paragraph direction.
		return (m_nDirDepth % 2);
	}

	GrEngine * pgreng = EngineImpl();
	if (pgreng)
	{
		return pgreng->RightToLeft();
	}
	return (m_pgts->getRightToLeft(m_ichwMin));
}

/*----------------------------------------------------------------------------------------------
	Get the depth of direction embedding used by this segment. It is presumably the same
	for all runs of the segment, otherwise, some of them would use a different writing
	system and therefore be part of a different segment. So just use the first.
----------------------------------------------------------------------------------------------*/
int Segment::directionDepth(bool * pfWeak)
{
	if (pfWeak)
		*pfWeak = (m_twsh == ktwshOnlyWs);

//	*pnDepth = m_pgts->getDirectionDepth(m_ichwLim);
//	Assert(*pnDepth == m_nDirDepth);

	return m_nDirDepth;
}

/*----------------------------------------------------------------------------------------------
	Change the direction of the segment. This is needed specifically for white-space-only
	segments, which are initially created to be in the direction of the paragraph, but then
	later are discovered to not be line-end after all, and need to be changed to use the
	directionality of the writing system.

	@return kresFail for segments that do not have weak directionality and therefore
	cannot be changed.
----------------------------------------------------------------------------------------------*/
bool Segment::setDirectionDepth(int nNewDepth)
{
	Assert(false);

	if (nNewDepth == m_nDirDepth)
	{
		return true;
	}
	else if ((nNewDepth % 2) == (m_nDirDepth % 2))
	{
		// Same direction, not much to change.
		m_nDirDepth = nNewDepth;
		return true;
	}
	else if (m_twsh != ktwshOnlyWs)
		return false;

	// Otherwise, do the hard stuff: reverse the positions of the glyphs.
	for (int islout = 0; islout < m_cslout; islout++)
	{
		OutputSlot(islout)->ShiftForDirDepthChange(m_dxsTotalWidth);
	}

	//SetDirectionDepthPlatform(nNewDepth);

	return true;

}

/*----------------------------------------------------------------------------------------------
	The logical range of characters covered by the segment, relative to the beginning of 
	the segment. The stop character is the index of the first character beyond the end 
	of the segment.
	
	These values should be exact at a writing system or string boundary,
	but may be somewhat fuzzy at a line-break, since characters may be re-ordered
	across such boundaries. The renderer is free to	apply any definition it likes of
	where a line-break occurs. This should always be the same value obtained from the 
	renderer as pdichLimSeg.
----------------------------------------------------------------------------------------------*/
int Segment::startCharacter()
{
	return m_ichwMin;
}

int Segment::stopCharacter()
{
	return m_ichwMin + m_dichwLim;
}

/*----------------------------------------------------------------------------------------------
	Returns an indication of why the segment ended.
----------------------------------------------------------------------------------------------*/
bool Segment::startOfLine()
{
	return m_fStartLine;
}

/*----------------------------------------------------------------------------------------------
	Returns an indication of why the segment ended.
----------------------------------------------------------------------------------------------*/
bool Segment::endOfLine()
{
	return m_fEndLine;
}

/*----------------------------------------------------------------------------------------------
	Returns true if this segment has something special happening at line boundaries.
----------------------------------------------------------------------------------------------*/
bool Segment::hasLineBoundaryContext()
{
	return (EngineImpl()->LineBreakFlag());
}

/*----------------------------------------------------------------------------------------------
	Returns an indication of why the segment ended.
----------------------------------------------------------------------------------------------*/
SegEnd Segment::segmentTermination()
{
	return m_est;
}

/*----------------------------------------------------------------------------------------------
	Indicates the last character of interest to the segment, relative to the beginning 
	of the segment. The meaning of this is that no behavior of this segment will be 
	affected if characters beyond that change. This does not necessarily mean that 
	a different line break could not have been obtained by the renderer if characters 
	beyond that change, just that a segment with the boundaries of this one would 
	not behave differently.
----------------------------------------------------------------------------------------------*/
//GrResult Segment::get_LimInterest(int ichwBase, int * pdichw)
//{
//	ChkGrOutPtr(pdichw);
//
//	GrEngine * pgreng = EngineImpl();
//	Assert(pgreng);
//	*pdichw = m_dichwLim + pgreng->PostXlbContext();
//	ReturnResult(kresOk);
//}

/*----------------------------------------------------------------------------------------------
	Changes the end-of-line status of the segment. This is used after making the last segment
	of a string if we want to attempt to put some more text after it (e.g., in another
	writing system), or if we have reordered due to bidirectionality.

	ENHANCE: the current version handles switching back and forth when we are trying to
	decide whether we can put more stuff on a line. So it assumes we are at the end of 
	the contextual run (ie, the next batch of stuff will be in a different ows). 
	This will not correctly handle the way this function could or possibly 
	should be used for bidirectional reordering.
----------------------------------------------------------------------------------------------*/
/*
GrResult Segment::changeLineEnds(bool fNewStart, bool fNewEnd)
{
	if (m_fStartLine != fNewStart || m_fEndLine != fNewEnd)
	{
		if (EngineImpl()->LineBreakFlag())
		{
			if (!m_psegAltEndLine)
			{
				int dichwLimSeg, dichwContext;
				float dxWidth;
				SegEnd est;
				int cbNextSegDat;
				byte rgbNextSegDat[256];
				OLECHAR rgchErrMsg[256];
				Segment * pseg;

				EngineImpl()->FindBreakPointAux(m_pfont, m_pgts, m_pgjus,
					m_ichwMin, m_ichwMin + m_dichwLim, m_ichwMin + m_dichwLim, 
					false, fNewStart, fNewEnd, kPosInfFloat, false,
					klbNoBreak, klbNoBreak, m_twsh, m_fParaRtl,
					&pseg,
					&dichwLimSeg, &dxWidth, &est,
					m_cbInitDat, m_prgInitDat, 256, rgbNextSegDat, &cbNextSegDat,
					&dichwContext,
					NULL,
					rgchErrMsg, 256);
				// TODO: do something with the error message. This shouldn't really happen,
				// since to get here we already created a segment with this font.
				if (!pseg)
				{
					// Because we provide INT_MAX as the amount of space, there should
					// always be enough room to generate the alternate segment.
					Assert(false);
					THROW(kresUnexpected);
				}
				Assert(cbNextSegDat == 0);
				m_psegAltEndLine = pseg;
			}
			Assert(m_psegAltEndLine->m_fEndLine == fNewEnd);
			Segment * pseg = m_psegAltEndLine; // save the value
			SwapWith(m_psegAltEndLine);
			m_psegAltEndLine = pseg; // reset it after swap
			pseg->m_psegAltEndLine = this;
		}
		else
		{
			bool fShift = ((m_nDirDepth % 2) && (m_fEndLine != fNewEnd));
			m_fStartLine = fNewStart;
			m_fEndLine = fNewEnd;
			if (fShift)
			{
				(m_fEndLine) ?
					ShiftGlyphs(m_dxsVisibleWidth - m_dxsTotalWidth) :	// shift left
					ShiftGlyphs(m_dxsTotalWidth - m_dxsVisibleWidth);	// shift right
			}
			m_dxsWidth = -1;
		}
	}

	ReturnResult(kresOk);
}
*/

/*----------------------------------------------------------------------------------------------
	Get the type of break that occurs at the logical start of the segment.
----------------------------------------------------------------------------------------------*/
LineBrk Segment::startBreakWeight()
{
	return m_lbStart;
}

/*----------------------------------------------------------------------------------------------
	Get the type of break that occurs at the logical end of the segment.
----------------------------------------------------------------------------------------------*/
LineBrk Segment::endBreakWeight()
{
	return m_lbEnd;
}

/*----------------------------------------------------------------------------------------------
	Read the amount of stretch, that is, the difference between the actual and natural width.
	If the segment has been shrunk, returns a negative number.
	TODO: implement properly.
----------------------------------------------------------------------------------------------*/
float Segment::stretch()
{
	return (float)m_dxsStretch;
}

/*----------------------------------------------------------------------------------------------
	Return the maximum stretch possible.
	TODO: implement properly.
----------------------------------------------------------------------------------------------*/
float Segment::maxStretch()
{
	return 1000;
}

/*----------------------------------------------------------------------------------------------
	Return the maximum shrink possible.
	TODO: implement properly.
----------------------------------------------------------------------------------------------*/
float Segment::maxShrink()
{
	return 0;
}

/*----------------------------------------------------------------------------------------------
	Return iterators representing the complete range of glyphs in the segment.
----------------------------------------------------------------------------------------------*/
std::pair<GlyphIterator, GlyphIterator> Segment::glyphs()
{
	return std::make_pair(GlyphIterator(*this, 0), GlyphIterator(*this, m_cginf));
}

/*----------------------------------------------------------------------------------------------
	Return iterators representing the set of glyphs for the given character.
----------------------------------------------------------------------------------------------*/
std::pair<GlyphSetIterator, GlyphSetIterator> Segment::charToGlyphs(toffset ich)
{
	std::vector<int> vislout = UnderlyingToLogicalAssocs(ich);

	if (vislout.size() == 0)
		return std::make_pair(GlyphSetIterator(), GlyphSetIterator());
	else
	{
		// Note that BOTH instances of GlyphSetIterator must use the same underlying vector,
		// so that comparisons make sense.
		RcVector * qvislout = new RcVector(vislout);
		return std::make_pair(
			GlyphSetIterator(*this, 0, qvislout), 
			GlyphSetIterator(*this, vislout.size(), qvislout));
	}
}

/*----------------------------------------------------------------------------------------------
	Return the rendered glyphs and their x- and y-coordinates, in the order generated by
	the final positioning pass, relative to the top-left of the segment. They are always
	returned in (roughly) left-to-right order. Note: any of the arrays may be null.
----------------------------------------------------------------------------------------------*/
/*
GrResult Segment::GetGlyphsAndPositions(
	Rect rs, Rect rd, int cchMax, int * pcchRet,
	utf16 * prgchGlyphs, float * prgxd, float * prgyd, float * prgdxdAdv)
{
	ChkGrOutPtr(pcchRet);
	if (prgchGlyphs)
		ChkGrArrayArg(prgchGlyphs, cchMax);
	if (prgxd)
		ChkGrArrayArg(prgxd, cchMax);
	if (prgyd)
		ChkGrArrayArg(prgyd, cchMax);
	if (prgdxdAdv)
		ChkGrArrayArg(prgdxdAdv, cchMax);

	GrResult res;

	//SetUpGraphics(ichwBase, pgg, true);

	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	*pcchRet = m_cginf;
	if (cchMax < m_cginf)
	{
		res = (cchMax == 0) ? kresFalse : kresInvalidArg;
		//RestoreFont(pgg);
		ReturnResult(res);
	}

	if (m_dxsWidth < 0)
		ComputeDimensions();

	float dysFontAscent = m_dysFontAscent;
	//////res = GetFontAscentSourceUnits(pgg, &dysFontAscent);
	//////if (ResultFailed(res))
	//////{
	//////	RestoreFont(pgg);
	//////	ReturnResult(res);
	//////}

	float xs = 0;
	//	Top of text that sits on the baseline, relative to top of segment:
	float ys = m_dysAscent - dysFontAscent;

	float * prgxdLocal;
	if (prgxd == NULL)
		prgxdLocal = new float[cchMax];
	else
		prgxdLocal = prgxd;

	res = GetGlyphsAndPositionsPlatform(xs, ys, rs, rd,
		prgchGlyphs, prgxdLocal, prgyd, prgdxdAdv);

	if (!prgxd)
		delete[] prgxdLocal;

	//RestoreFont(pgg);

	ReturnResult(res);
}
*/

/*----------------------------------------------------------------------------------------------
	For debugging. Return the characters in this segment.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
GrResult Segment::GetCharData(int cchMax, utf16 * prgch, int * pcchRet)
{
	ChkGrArgPtr(prgch);
	ChkGrOutPtr(pcchRet);

	int ichLimTmp = m_ichwMin + min(m_dichwLim, cchMax);
	m_pgts->fetch(m_ichwMin, ichLimTmp - m_ichwMin, prgch);
	*pcchRet = ichLimTmp - m_ichwMin;
	ReturnResult(kresOk);
}

/*----------------------------------------------------------------------------------------------
	Return either the first or last character that the the glyph corresponds to. The
	index of the glyph matches what was returned by GetGlyphsAndPositions.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
GrResult Segment::GlyphToChar(int iginf, bool fFirst, int * pich)
{
	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	*pich = PhysicalSurfaceToUnderlying(iginf, fFirst);
	ReturnResult(kresOk);
}

/*----------------------------------------------------------------------------------------------
	Return all the characters that the glyph corresponds to. Most commonly this will
	be only one char, but ligatures, for instance, will have several. The index of the
	glyph matches what was returned by GetGlyphsAndPositions.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
GrResult Segment::GlyphToAllChars(int iginf,
		int cichMax, int * prgich, int * pcichRet)
{
	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	std::vector<int> vich;
	int ichFirst = PhysicalSurfaceToUnderlying(iginf, true);
	int ichLast = PhysicalSurfaceToUnderlying(iginf, false);
	Assert(ichFirst <= ichLast);

	if (ichFirst < ichLast)
	{
		// Loop over the range of characters that might be associated with this glyph
		// and add any that actually are.
		// TODO: we should probably use a method that gets us all the physical associations,
		// but this should get us 99.5% of the way there for now.
		for (int ich = ichFirst; ich <= ichLast; ich++)
		{
			int iginfFirst = UnderlyingToPhysicalSurface(ich, true);
			int iginfLast = UnderlyingToPhysicalSurface(ich, false);
			if (iginfFirst == iginf || iginfLast == iginf)
				vich.push_back(ich);
		}
	}
	else
		vich.push_back(ichFirst);

	*pcichRet = vich.size();
	if (cichMax < signed(vich.size()))
	{
		GrResult res = (cichMax == 0 ? kresFalse : kresInvalidArg);
		ReturnResult(res);
	}
	int * pi = prgich;
	for (size_t i = 0; i < vich.size(); i++)
		*pi++ = vich[i];

	ReturnResult(kresOk);
}

/*----------------------------------------------------------------------------------------------
	Return either the first or last glyph that corresponds to the character. The
	index of the glyph matches what was returned by GetGlyphsAndPositions.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
GrResult Segment::CharToGlyph(int ich, bool fFirst, int * pigbb)
{
	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	*pigbb = UnderlyingToPhysicalSurface(ich, fFirst);
	ReturnResult(kresOk);
}


//:>********************************************************************************************
//:>	Non-FieldWorks interface methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Return the breakweight for the given character. Returns the best value if the character is
	mapped to glyphs with different values.
----------------------------------------------------------------------------------------------*/
LineBrk Segment::getBreakWeight(int ich, bool fBreakBefore)
{
	LineBrk lbFirst, lbLast;

	int isloutFirst = UnderlyingToLogicalSurface(ich, true);
	if (isloutFirst == kPosInfinity || isloutFirst == kNegInfinity)
		return klbClipBreak;
	GrSlotOutput * psloutFirst = m_prgslout + isloutFirst;
	bool fNotFirst = false;
	if ((psloutFirst->NumberOfComponents() > 0)
		&& psloutFirst->FirstUnderlyingComponent(0) != ich)
	{
		// Not the first component of a ligature.
		lbFirst = (LineBrk) ((int)klbClipBreak * -1);
		fNotFirst = true;
	}
	else
	{
		lbFirst = (LineBrk) psloutFirst->BreakWeight();
	}

	int isloutLast = UnderlyingToLogicalSurface(ich, false);
	if (isloutLast == kPosInfinity || isloutLast == kNegInfinity)
		return klbClipBreak;
	GrSlotOutput * psloutLast = m_prgslout + isloutLast;
	bool fNotLast = false;
	int icompLast = psloutLast->NumberOfComponents() - 1;
	if ((psloutLast->NumberOfComponents() > 0)
		&& psloutLast->LastUnderlyingComponent(icompLast) != ich)
	{
		// Not the last component of a ligature.
		lbLast = klbClipBreak;
		fNotLast = true;
	}
	else
	{
		lbLast = (LineBrk) psloutLast->BreakWeight();
	}

	LineBrk lbRet;
	if (fNotLast && fNotFirst)
		// middle of a ligature
		lbRet = klbClipBreak;
	else if (fNotLast)
		lbRet = (fBreakBefore) ? lbFirst : klbClipBreak;
	else if (fNotFirst)
		lbRet = (fBreakBefore) ? klbClipBreak : lbLast;
	else
		lbRet = (fBreakBefore) ? lbFirst : lbLast;

	return lbRet;
}

/*----------------------------------------------------------------------------------------------
	Return the width of the range of characters. Line-boundary contextualization is handled
	by the measure attributes on the glyphs. The width returned is equivalent to the sum of
	the widths of any line-segments that would be needed to underline all the characters.

	Since this method is intended to be used on "measured" segments that include the entire
	paragraph, it is not very smart about handling cross-line-boundary contextualization.
	It basically ignores any glyphs that are rendered by this segment but not officially
	part of the segment.
----------------------------------------------------------------------------------------------*/
float Segment::getRangeWidth(int ichMin, int ichLim,
	bool fStartLine, bool fEndLine, bool fSkipSpace)
{
	if (m_dxsWidth < 0)
	{
		////SetUpGraphics(ichwBase, pgg, true);
		ComputeDimensions();
	}

	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	//float xsSegRight = m_dxsTotalWidth;

	int ichMinRange = min(ichMin, ichLim);
	int ichLimRange = max(ichMin, ichLim);

	//int ichMinSeg = max(ichMinRange, m_ichwMin + m_ichwAssocsMin);
	//int ichLimSeg = min(ichLimRange, m_ichwMin + m_ichwAssocsLim);
	int ichMinSeg = max(ichMinRange, m_ichwMin);
	int ichLimSeg = min(ichLimRange, m_ichwMin + m_dichwLim);

	if (ichLimSeg < m_ichwMin) // not + m_ichwAssocsMin
		return 0;
	if (ichMinSeg >= m_ichwMin + m_dichwLim) // not m_ichwAssocsLim
		return 0;

	if (fSkipSpace)
	{
		int islout = UnderlyingToLogicalSurface(ichLimSeg - 1, true);
		GrSlotOutput * pslout = (islout == kNegInfinity || islout == kPosInfinity) ?
			NULL : 
			OutputSlot(islout);
		while (pslout && pslout->IsSpace())
		{
			ichLimSeg--;
			islout = UnderlyingToLogicalSurface(ichLimSeg - 1, true);
//			pslout = OutputSlot(islout);
            pslout = (islout == kNegInfinity || islout == kPosInfinity) ?
				NULL : OutputSlot(islout);
		}
	}

	float xsWidth = 0;
	SegmentNonPainter segp(this); // doesn't really need to paint
	float rgxdLefts[100];
	float rgxdRights[100];
	size_t cxd = 0;
	if (ichMinRange < ichLimRange)
		cxd = segp.getUnderlinePlacement(ichMinSeg, ichLimSeg, fSkipSpace,
			100, rgxdLefts, rgxdRights, NULL);
	for (size_t i = 0; i < cxd; i++)
		xsWidth += rgxdRights[i] - rgxdLefts[i];

	//	Add in the line-boundary contextualization.
	int isloutFirst = UnderlyingToLogicalSurface(ichMin, true);
	int isloutLast = UnderlyingToLogicalSurface(ichLim - 1, false);
	int mSol = 0;
	int mEol = 0;
	if (0 <= isloutFirst && isloutFirst < m_cslout)
		mSol = m_prgslout[isloutFirst].MeasureSol();
	if (0 <= isloutLast && isloutLast < m_cslout)
		mEol = m_prgslout[isloutLast].MeasureEol();
	float dxsSol = GrEngine::GrIFIMulDiv(mSol, m_xysEmSquare, m_mFontEmUnits);
	float dxsEol = GrEngine::GrIFIMulDiv(mEol, m_xysEmSquare, m_mFontEmUnits);
	xsWidth += dxsSol;
	xsWidth += dxsEol;

	//RestoreFont(pgg);

	return xsWidth;
}

/*----------------------------------------------------------------------------------------------
	Return the next reasonable breakpoint after ichStart resulting in range that fits in
	the specified width. 
----------------------------------------------------------------------------------------------*/
int Segment::findNextBreakPoint(int ichStart,
	LineBrk lbPref, LineBrk lbWorst, float dxMaxWidth,
	float * pdxBreakWidth, bool fStartLine, bool fEndLine)
{
	ChkGrOutPtr(pdxBreakWidth);

	int ichBreak;

	//	First make a rough estimation of how much will fit, using a binary chop approach.
	int iginfStart = UnderlyingToPhysicalSurface(ichStart, !m_fWsRtl);
	int iginfEnd = UnderlyingToPhysicalSurface(m_ichwMin + m_dichwLim - 1, m_fWsRtl);
	int iginfLeft = min(iginfStart, iginfEnd);
	int iginfRight= max(iginfStart, iginfEnd);

	int iginfMin = iginfLeft;
	int iginfLim = iginfRight;
	int ichFit;
	if (m_fWsRtl) // right-to-left
	{
		float xRight = GlyphLeftEdge(iginfRight + 1);
		float xLeftMost = xRight - dxMaxWidth;
		while (GlyphLeftEdge(iginfLim) < xLeftMost)
		{
			if (iginfLim - iginfMin <= 1)
				break;
			int iginfMid = (iginfLim + iginfMin) >> 1; // divide by 2
			if (GlyphLeftEdge(iginfMid) < xLeftMost)
				iginfLim = iginfMid;
			else
				iginfMin = iginfMid;
		}
		ichFit = PhysicalSurfaceToUnderlying(iginfLim, false);
	}
	else // left-to-right
	{
		float xLeft = GlyphLeftEdge(iginfLeft);
		float xRightMost = dxMaxWidth - xLeft;   // QUESTION: shouldn't this be +, not -?
		iginfLim++;
		while (GlyphLeftEdge(iginfLim) > xRightMost)
		{
			if (iginfLim - iginfMin <= 1)
				break;
			int iginfMid = (iginfLim + iginfMin) >> 1; // divide by 2
			if (GlyphLeftEdge(iginfMid) > xRightMost)
				iginfLim = iginfMid;
			else
				iginfMin = iginfMid;
		}
		Assert(iginfLim <= m_cginf);
		if (iginfLim >= m_cginf) iginfLim = m_cginf - 1;
		ichFit = PhysicalSurfaceToUnderlying(iginfLim, false);
	}

	//	Breaking after ichFit should pretty much fit in the space. But make sure.
	float dxWidthFit;
	while (ichFit > ichStart)
	{
		dxWidthFit = getRangeWidth(ichStart, ichFit, fStartLine, fEndLine);
		if (dxWidthFit <= dxMaxWidth)
			break;
		ichFit--;
	}

	if (ichFit <= ichStart)
	{
		// Nothing will fit.
		return ichStart - 1;
	}

	//	Now look for a good break. First look forward until the range doesn't fit anymore.
	int ichTry = ichFit;
	int nExtra = 2;
	LineBrk lbBest = klbClipBreak;
	int ichBestBreak = -1;
	while (nExtra > 0 && ichTry < m_ichwMin + m_dichwLim)
	{
		float dxWTmp;
		dxWTmp = getRangeWidth(ichStart, ichTry, fStartLine, fEndLine);
		if (dxWTmp <= dxMaxWidth)
		{
			LineBrk lbTmp;
			lbTmp = getBreakWeight(ichTry - 1, false);
			if (lbTmp > 0 && lbTmp < lbWorst && max(lbTmp, lbPref) <= lbBest)
			{
				ichBestBreak = ichTry;
				lbBest = max(lbTmp, lbPref); // any value better than lbPref is insignificant
			}
			else
			{
				lbTmp = getBreakWeight(ichTry - 1, true);
				if (lbTmp < 0)
				{
					lbTmp = LineBrk(max(lbTmp * -1L, long(lbPref)));
					if (lbTmp < lbWorst && lbTmp <= lbBest)
					{
						ichBestBreak = ichTry - 1;
						lbBest = lbTmp;
					}
				}
			}
		}
		else
		{
			// Didn't fit. But it is theoretically possible that actually adding another
			// character will! Try a few more.
			nExtra--;
		}
		ichTry++;
	}

	if (ichBestBreak > -1)
	{
		if (ichTry >= m_ichwMin + m_dichwLim)
		{
			// Break at the end of the segment.
			ichBreak = m_ichwMin + m_dichwLim;
			*pdxBreakWidth = getRangeWidth(ichStart, m_dichwLim, fStartLine, fEndLine);
			return ichBreak;
		}

		if (lbBest <= lbPref)
		{
			// Found a reasonable break by looking forward; return it.
			ichBreak = ichBestBreak;// + m_ichwMin;
			*pdxBreakWidth = getRangeWidth(ichStart, ichBestBreak, fStartLine, fEndLine);
			return ichBreak;
		}
	}

	//	Didn't find a really good break point looking forward from our rough guess.
	//	Now look backward.

	ichTry = ichFit;
	while (ichTry > ichStart)
	{
		LineBrk lbTmp;
		lbTmp = getBreakWeight(ichTry - 1, false);
		if (lbTmp > 0 && lbTmp < lbWorst && max(lbTmp, lbPref) < lbBest)
		{
			ichBestBreak = ichTry;
			lbBest = max(lbTmp, lbPref); // any value better than lbPref is insignificant
		}
		else
		{
			lbTmp = getBreakWeight(ichTry - 1, true);
			if (lbTmp < 0)
			{
				lbTmp = LineBrk(max(lbTmp * -1L, long(lbPref)));
				if (lbTmp < lbWorst && lbTmp < lbBest)
				{
					ichBestBreak = ichTry - 1;
					lbBest = lbTmp;
				}
			}
		}
		if (lbBest <= lbPref)
			break;

		ichTry--;
	}

	if (ichBestBreak > -1 && ichBestBreak > ichStart)
	{
		// Found a reasonable break by looking backward; return it.
		ichBreak = ichBestBreak;// + m_ichwMin;
		*pdxBreakWidth = getRangeWidth(ichStart, ichBestBreak, fStartLine, fEndLine);
		return ichBreak;
	}
	
	// No reasonable break.
	return ichStart - 1;
}

/*----------------------------------------------------------------------------------------------
	Private; used by findNextBreakPoint().
----------------------------------------------------------------------------------------------*/
float Segment::GlyphLeftEdge(int iginf)
{
	if (iginf >= m_cginf)
        return m_prgginf[m_cginf - 1].origin() + m_prgginf[m_cginf - 1].advanceWidth();
//		return m_prgginf[m_cginf].origin() + m_prgginf[m_cginf].advanceWidth();
	else
		return m_prgginf[iginf].origin();
}


//:>********************************************************************************************
//:>	Other methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Get the embedded string. For temporary use; AddRef is not automatically called.
	Do not release unless you AddRef.
----------------------------------------------------------------------------------------------*/
ITextSource * Segment::GetString()
{
	return m_pgts;
}

/*----------------------------------------------------------------------------------------------
	Compute the ascent, height, width, and overhangs of the segment.
	The ascent is the ascent of the font (distance from the baseline to the top)
	plus any extra ascent height specified by the font.
	The descent is distance from the baseline to the bottom of font, plus any extra descent
	specified by the font (ExtraDescent).
	The ascent overhang is the distance from the top of the font to the top of the highest
	glyph, or zero if all the glyphs fit within the ascent.
	The descent overhang is the distance from the bottom of the font to the bottom of the lowest
	glyph, or zero if all the glyphs fit within the descent.
	The height is the sum of the ascent and descent.
----------------------------------------------------------------------------------------------*/
void Segment::ComputeDimensions()
{
	if (m_fEndLine)
		m_dxsWidth = m_dxsVisibleWidth;	// don't include trailing white space.
	else
		m_dxsWidth = m_dxsTotalWidth;

	if (m_dxsWidth == -1)
	{
		for (int iginf = 0; iginf < m_cginf; iginf++)
		{
			GlyphInfo * pginf = m_prgginf + iginf;
			if (pginf->isSpace())
				continue;
			m_dxsWidth = max(m_dxsWidth, (pginf->origin() + pginf->advanceWidth()));
		}
	}
	
	int dysNegFontDescent = (int)m_dysFontDescent * -1;

	// if there were no glyphs in the segment then there is sometimes no EngineImpl
	if (!EngineImpl()) 
	{
		Assert(m_cslout == 0);
		Assert(m_cginf == 0);
		m_dxsWidth = 0;
		m_dysAscent = 0; // max(0.0, m_dysFontAscent);
		m_dysHeight = 0; // max(0.0, m_dysAscent - dysNegFontDescent);
		m_dysAscentOverhang = 0;
		m_dysDescentOverhang = 0;
		m_dxsLeftOverhang = 0;
		m_dxsRightOverhang = 0;
		return;
	}

	//	Calculate the extra ascent and descent.
	int mXAscent = EngineImpl()->ExtraAscent();
	int mXDescent = EngineImpl()->ExtraDescent();

	m_dysXAscent = GrEngine::GrIFIMulDiv(mXAscent, m_xysEmSquare, m_mFontEmUnits);
	m_dysXDescent = GrEngine::GrIFIMulDiv(mXDescent, m_xysEmSquare, m_mFontEmUnits);

	m_dysAscent = m_dysFontAscent + m_dysXAscent;
	float dysNegDescent = dysNegFontDescent - m_dysXDescent;	// dysNegDescent < 0
	m_dysHeight = m_dysAscent - dysNegDescent;

	//	Calculate the overhangs.
	float dysVisAscent = m_dysAscent;
	float dysNegVisDescent = dysNegDescent;
	float dxsVisLeft = 0;
	float dxsVisRight = m_dxsTotalWidth;
	ComputeOverhangs(&dysVisAscent, &dysNegVisDescent, &dxsVisLeft, &dxsVisRight);
	m_dysAscentOverhang = max(float(0), (dysVisAscent - m_dysAscent));
	m_dysDescentOverhang = max(float(0), (dysNegDescent - dysNegVisDescent));
	m_dxsLeftOverhang = min(float(0), dxsVisLeft);
	m_dxsRightOverhang = max(float(0), dxsVisRight - m_dxsTotalWidth);

	m_dysOffset = EngineImpl()->VerticalOffset();
}

/*----------------------------------------------------------------------------------------------
	Compute the visible ascent and descent, based on how much the bounding box of any
	glyph may extend beyond the ascent or descent of the font.
	Also compute the left- and right-overhangs.
----------------------------------------------------------------------------------------------*/
void Segment::ComputeOverhangs(float * pdysVisAscent, float * pdysNegVisDescent,
	float * pdxsVisLeft, float * pdxsVisRight)
{
	for (int iginf = 0; iginf < m_cginf; iginf++)
	{
		*pdysVisAscent = max(*pdysVisAscent, m_prgginf[iginf].bb().top);
		*pdysNegVisDescent = min(*pdysNegVisDescent, m_prgginf[iginf].bb().bottom);

		*pdxsVisLeft = min(*pdxsVisLeft, m_prgginf[iginf].bb().left);
		*pdxsVisRight = max(*pdxsVisRight, m_prgginf[iginf].bb().right);
	}
}

/*----------------------------------------------------------------------------------------------
	Create the arrays to store the final output and association information.
----------------------------------------------------------------------------------------------*/
void Segment::SetUpOutputArrays(Font * pfont, GrTableManager * ptman,
	GrSlotStream * psstrmFinal,
	int cchwInThisSeg, int csloutSurface, gid16 chwLB,
	TrWsHandling twsh, bool fParaRtl, int nDirDepth, bool fEmpty)
{
	m_mFontEmUnits = EngineImpl()->GetFontEmUnits();

	pfont->getFontMetrics(&m_dysFontAscent, &m_dysFontDescent, &m_xysEmSquare);
//	m_xsDPI = (float)pfont->getDPIx();
//	m_ysDPI = (float)pfont->getDPIy();

	// Note that storing both of these values is redundant; they should be the same.
//	Assert(m_xysEmSquare == m_pixHeight);

	m_twsh = twsh;
	m_fParaRtl = fParaRtl;
	if (m_twsh == ktwshOnlyWs)
		m_nDirDepth = (int)fParaRtl;
	else if (fParaRtl && nDirDepth == 0)
		m_nDirDepth = 2;
	else
		m_nDirDepth = nDirDepth;

	//	Create association mappings and set up final versions of slots.

	Assert((psstrmFinal == NULL) == fEmpty);

	m_ichwAssocsMin = 0;
	m_ichwAssocsLim = cchwInThisSeg;

	m_prgisloutBefore = new int[cchwInThisSeg];

	m_prgisloutAfter = new int[cchwInThisSeg];

	m_prgpvisloutAssocs = new std::vector<int> * [cchwInThisSeg];

	m_prgisloutLigature = new int[cchwInThisSeg];

	m_prgiComponent = new sdata8[cchwInThisSeg];

	int cslot = 0;
	//m_psstrm = psstrmFinal; // TODO: make a local variable
	if (psstrmFinal)
		cslot = psstrmFinal->FinalSegLim();
	else
		Assert(fEmpty);

	float xsMin = 0;
	int islot;
	int islotMin = (psstrmFinal) ? psstrmFinal->IndexOffset() : 0;
	for (islot = islotMin; islot < cslot; islot++)
		xsMin = min(xsMin, psstrmFinal->SlotAt(islot)->XPosition());

	//	For right-to-left segments, the draw origin is at the left side of the visible
	//	portion of the text. So if necessary, scoot everything left so that the invisible
	//	trailing white-space is to the left of the draw position.
	Assert(m_dxsTotalWidth > -1);
	Assert(m_dxsVisibleWidth > -1);
	float dxsInvisible = (m_fEndLine) ? m_dxsTotalWidth - m_dxsVisibleWidth : 0;
	for (islot = islotMin; ((m_nDirDepth % 2) && (islot < cslot)); islot++)
	{
		GrSlotState * pslot = psstrmFinal->SlotAt(islot);
		// int islout = islot - islotMin;
		if (pslot->GlyphID() == chwLB)
			continue; // skip over linebreak markers
		if (m_nDirDepth % 2) // redundant test
		{
			// RTL will have descending neg values, adjust to be positive. Also make sure
			// invisible trailing white space is to the left of the draw origin.
			// Review: will changing this value have side affects?
			pslot->SetXPos(pslot->XPosition() - xsMin - dxsInvisible); 
		}
	}

	Assert(kPosInfinity > 0);
	Assert(kNegInfinity < 0);
	for (int ichw = 0; ichw < cchwInThisSeg; ++ichw)
	{
		m_prgisloutBefore[ichw] = kPosInfinity;
		m_prgisloutAfter[ichw]  = kNegInfinity;
		m_prgpvisloutAssocs[ichw] = new std::vector<int>;
		m_prgisloutLigature[ichw] = kNegInfinity;
		m_prgiComponent[ichw] = 0;
	}

	m_cslout = csloutSurface;
	if (ptman->NumUserDefn() > 0 && ptman->NumCompPerLig() > 0)
	{
		int x; x = 3;
	}

//	m_cnUserDefn = ptman->NumUserDefn();
	m_cnCompPerLig = ptman->NumCompPerLig();
//	m_cnFeat = ptman->NumFeat();
	//	Normal buffers, plus:
	//		- underlying indices of ligature components
	//		- map from used components to defined components
//	int cnExtraPerSlot = m_cnUserDefn +	(m_cnCompPerLig * 2) + m_cnFeat + (m_cnCompPerLig * 2);

	// We don't need to store the user-defined slot attributes or the features in the segment itself.
	// What we need is: (1) component.ref attr settings, (2) slot-attribute indices,
	// (3) underlying indices of ligature components, and (4) map from used components
	// to defined components
	// 18Jul08 - it looks like all we need are underlying chars for ligature components and
	// map from components to defined components.
	int cnExtraPerSlot = m_cnCompPerLig * 2;
	m_prgslout = new GrSlotOutput[m_cslout];
	m_prgnSlotVarLenBuf = new u_intslot[m_cslout * cnExtraPerSlot];

	m_isloutVisLim = 0;
	if (psstrmFinal)
	{
		for (islot = islotMin; islot < psstrmFinal->FinalSegLim(); ++islot)
		{
			int isloutRel = islot - islotMin;

			GrSlotState * pslot = psstrmFinal->SlotAt(islot);
			pslot->SetPosPassIndex(isloutRel, false);
			pslot->EnsureCacheForOutput(ptman);
			GrSlotOutput * pslout = OutputSlot(isloutRel);
			pslout->SetBufferPtr(m_prgnSlotVarLenBuf + (isloutRel * cnExtraPerSlot));
			pslout->InitializeOutputFrom(pslot);
			pslout->SetBeforeAssoc(pslot->BeforeAssoc());
			pslout->SetAfterAssoc(pslot->AfterAssoc());
			if (pslot->HasComponents())
				pslot->SetComponentRefsFor(pslout);
			Assert(pslot->HasComponents() == (pslout->NumberOfComponents() > 0));
			//if (pslot->HasClusterMembers())
			//{
				//Assert(pslot->HasClusterMembers() || pslot->ClusterRootOffset() == 0);
				pslout->SetClusterXOffset(pslot->ClusterRootOffset() * -1);
				pslout->SetClusterAdvance(pslot->ClusterAdvWidthFrom(0));

			//}

			if (!ptman->IsWhiteSpace(pslot))
				m_isloutVisLim = islot + 1;
		}
	}

	// Set up the clusters.
	for (islot = islotMin; islot < m_cslout; islot++)
	{
		GrSlotState * pslot = psstrmFinal->SlotAt(islot);
		int isloutAdj = islot - islotMin;
		int isloutBaseIndex = pslot->Base(psstrmFinal)->PosPassIndex();
		if (!pslot->IsBase())
		{
			OutputSlot(isloutBaseIndex)->AddClusterMember(isloutBaseIndex, pslot->PosPassIndex());
			OutputSlot(isloutAdj)->SetClusterBase(isloutBaseIndex);
		}
		else if (pslot->HasClusterMembers() && pslot->IsBase())
		{
			Assert(isloutBaseIndex == islot - islotMin);
			OutputSlot(isloutAdj)->SetClusterBase(isloutBaseIndex);
		}
	}

	///AssertValidClusters(psstrmFinal);

#ifdef OLD_TEST_STUFF
	if (ptman->GlyphTable() == NULL)
		return;	// test procedures
#endif // OLD_TEST_STUFF

	//	CalcPositionsUpTo() called on final pass already from Table Mgr
	
	//	Final output for draw routines.

	SetUpGlyphInfo(ptman, psstrmFinal, chwLB, nDirDepth, islotMin, cslot);

	//SetUpOutputArraysPlatform(ptman, chwLB, nDirDepth, islotMin, cslot);

}

/*----------------------------------------------------------------------------------------------
	Set up the data structures that represent the actual rendered glyphs for the new segment.
----------------------------------------------------------------------------------------------*/
void Segment::SetUpGlyphInfo(GrTableManager * ptman, GrSlotStream * psstrmFinal,
	gid16 chwLB, int nDirDepth, int islotMin, int cslot)
{
	//int paraDirLevel = (ptman->State()->ParaRightToLeft()) ? 1 : 0;

	m_cginf = 0;

	int islot;
	for (islot = islotMin; islot < cslot; islot++)
	{
		if (psstrmFinal->SlotAt(islot)->GlyphID() != chwLB)
		{
			m_cginf++;
		}
	}

	//	For right-to-left segments, the draw origin will be at the left side of the visible
	//	portion of the text. So if necessary, scoot everything left so that the invisible
	//	trailing white-space is to the left of the draw position.
	Assert(m_dxsTotalWidth > -1);
	Assert(m_dxsVisibleWidth > -1);
	//float dxsInvisible = (m_fEndLine) ? m_dxsTotalWidth - m_dxsVisibleWidth : 0;

	m_prgginf = new GlyphInfo [m_cginf];

	m_isloutGinf0 = -1;
	int iginf = 0;
	for (int islot = islotMin; islot < cslot; islot++)
	{
		GrSlotState * pslot = psstrmFinal->SlotAt(islot);

		if (pslot->GlyphID() == chwLB)
		{
			continue; // skip over linebreak markers
		}

		int islout = islot - islotMin;
		GrSlotOutput * pslout = OutputSlot(islout);

		if (m_isloutGinf0 == -1)
			m_isloutGinf0 = islout;

		m_prgginf[iginf].m_pslout = pslout;
		m_prgginf[iginf].m_islout = islout;
		m_prgginf[iginf].m_pseg = this;

		// Fill in stuff in the output slot that is needed by the GlyphInfo object.
		////pslout->m_xsAdvanceX = pslot->GlyphMetricLogUnits(ptman, kgmetAdvWidth); // wrong
		pslout->m_xsAdvanceX = ptman->EmToLogUnits(pslot->AdvanceX(ptman));
		//pslout->m_ysAdvanceY = ptman->EmToLogUnits(pslot->AdvanceY(ptman));
		//pslout->m_rectBB.top
		//	= pslot->YPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbTop);
		//pslout->m_rectBB.bottom
		//	= pslot->YPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbBottom);
		//pslout->m_rectBB.left
		//	= pslot->XPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbLeft);
		//if (pslot->IsSpace(ptman))
		//	pslout->m_rectBB.right
		//		= pslot->XPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetAdvWidth);
		//else
		//	pslout->m_rectBB.right
		//		= pslot->XPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbRight);

		iginf++;
	}

	if (cslot - islotMin == 0)
		m_isloutGinf0 = 0;
	if (m_isloutGinf0 == -1)
	{
		Assert(m_cginf == 0);
		m_isloutGinf0 = ((OutputSlot(0)->IsInitialLineBreak()) ? 1 : 0);
	}

	Assert(m_isloutGinf0 == 0 || m_isloutGinf0 == 1);
}

/*
// OLD VERSION
void Segment::SetUpGlyphInfo(GrTableManager * ptman, GrSlotStream * psstrmFinal, gid16 chwLB, int nDirDepth,
	int islotMin, int cslot)
{
	int paraDirLevel = (ptman->State()->ParaRightToLeft()) ? 1 : 0;

	m_cginf = 0;
	int islot;
	for (islot = islotMin; islot < cslot; islot++)
	{
		if (psstrmFinal->SlotAt(islot)->GlyphID() != chwLB)
			m_cginf++;
	}

	//	For right-to-left segments, the draw origin will be at the left side of the visible
	//	portion of the text. So if necessary, scoot everything left so that the invisible
	//	trailing white-space is to the left of the draw position.
	Assert(m_dxsTotalWidth > -1);
	Assert(m_dxsVisibleWidth > -1);
	float dxsInvisible = (m_fEndLine) ? m_dxsTotalWidth - m_dxsVisibleWidth : 0;

	m_prgginf = new GlyphInfo [m_cginf];

	//	Fill in glyph information structures.
	m_isloutGinf0 = -1;
	for (islot = islotMin; islot < cslot; islot++)
	{
		GrSlotState * pslot = psstrmFinal->SlotAt(islot);

		int islout = islot - islotMin;
		if (pslot->GlyphID() == chwLB)
		{
			continue; // skip over linebreak markers
		}
		if (m_isloutGinf0 == -1)
			m_isloutGinf0 = islout;

		GrSlotOutput * pslout = OutputSlot(islout);

		int iginf = islout - m_isloutGinf0;
		GlyphInfo & ginf = m_prgginf[iginf];

		ginf.glyphID = pslot->ActualGlyphForOutput(ptman);
		ginf.pseudoGlyphID = pslot->GlyphID();
		if (ginf.glyphID == ginf.pseudoGlyphID)
			ginf.pseudoGlyphID = 0;

		ginf.isSpace = pslot->IsSpace(ptman);

		ginf.origin = pslot->XPosition();
		ginf.yOffset = pslot->YPosition();

		ginf.bbLeft = pslot->XPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbLeft);
		ginf.bbTop = pslot->YPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbTop);
		if (ginf.isSpace)
		{
			ginf.bbRight = pslot->XPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetAdvWidth);
		}
		else
		{
			ginf.bbRight = pslot->XPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbRight);
		}
		ginf.bbBottom = pslot->YPosition() + pslot->GlyphMetricLogUnits(ptman, kgmetBbBottom);

		ginf.advanceWidth = pslot->GlyphMetricLogUnits(ptman, kgmetAdvWidth);
		ginf.advanceHeight = 0;

		if (pslot->AttachTo() == 0) // not attached
			ginf.attachedTo = -1;
		else
			ginf.attachedTo = pslot->AttachTo() + islot;

		ginf.directionality = byte(pslot->Directionality());
		if (pslot->DirLevel() == -1)
			ginf.directionLevel = paraDirLevel;
		else
			ginf.directionLevel = byte(pslot->DirLevel());

		//ginf.firstChar = LogicalSurfaceToUnderlying(islout, true);
		//ginf.lastChar = LogicalSurfaceToUnderlying(islout, false);

		if (pslot->HasComponents())
		{
			SetUpLigInfo(ptman, ginf, pslout);
		}
		else
		{
            ginf.numberOfComponents = 0;
			ginf.components = NULL;
		}

		ginf.insertBefore = pslot->InsertBefore();

		ginf.maxStretch[0] = ptman->EmToLogUnits(pslot->JStretch());
		ginf.maxShrink[0]  = ptman->EmToLogUnits(pslot->JShrink());
		ginf.stretchStep[0]= ptman->EmToLogUnits(pslot->JStep());
		ginf.justWidth[0]  = ptman->EmToLogUnits(pslot->JWidth());
		ginf.justWeight[0] = pslot->JWeight();

		ginf.measureStartOfLine = pslout->MeasureSolLogUnits();
		ginf.measureEndOfLine = pslout->MeasureEolLogUnits();
	}
}
*/

/*----------------------------------------------------------------------------------------------
	Set up the data structures that represent the components of a ligature glyph.
----------------------------------------------------------------------------------------------*/
/*
void Segment::SetUpLigInfo(GrTableManager * ptman, GlyphInfo & ginf, GrSlotOutput * pslout)
{
	GrGlyphTable * pgtbl = ptman->GlyphTable();

	int ccomp = pslout->NumberOfComponents();
	ginf.numberOfComponents = ccomp;
	ginf.components = new ComponentBox[ccomp];
	for (int icomp = 0; icomp < ccomp; icomp++)
	{
		ComponentBox & cb = ginf.components[icomp];

		// TODO: Rework ComponentIndexForGlyph to take slati rather than iForGlyph.
		float xsLeft, xsRight, ysTop, ysBottom;
		int slati = pslout->ComponentId(icomp);
		int iForGlyph = pgtbl->ComponentIndexForGlyph(pslout->GlyphID(), slati);
		pgtbl->ComponentBoxLogUnits(m_xysEmSquare, pslout->GlyphID(), iForGlyph,
			m_mFontEmUnits, m_dysAscent,
			&xsLeft, &ysTop, &xsRight, &ysBottom, false);

		cb.firstChar = pslout->UnderlyingComponent(icomp);
		cb.lastChar = cb.firstChar; // TODO: is this supported?
		cb.left = xsLeft;
		cb.right = xsRight;
		cb.top = ysTop;
		cb.bottom = ysBottom;
	}
}
*/


//	---------  OBSOLETE - replaced by GrTableManager::AdjustAssocsForOverlaps()  ---------------
#if 0

/*----------------------------------------------------------------------------------------------
	Now that this segment is finished and has its associations set, handle any overlaps in
	the associations between the previous segment and this one.

	Review: Think about whether we need to distinguish between before==kPosInfinity meaning
	that the glyph is in the next segment vs. there is no association at all.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void Segment::AdjustForOverlapsWithPrevSeg()
{
	if (m_psegPrev)
	{
		AdjustBeforeArrayFromPrevSeg();
		m_psegPrev->AdjustAfterArrayFromNextSeg();
	}
}

/*----------------------------------------------------------------------------------------------
	For any characters that are officially in this segment but are rendered in
	the previous segment, initialize this segment's before array appropriately. Specifically,
	set this segment's before value to something invalid--negative infinity, which will
	never be overwritten.
	OBSOLETE - replaced by AdjustAfterArrayFromNextSeg
----------------------------------------------------------------------------------------------*/
void Segment::InitBeforeArrayFromPrevSeg()
{
	Assert(m_psegPrev);

	int ichwPrevMin = m_psegPrev->m_ichwMin;
	int ichwPrevLim = m_psegPrev->m_ichwLim;
//	int ichwPrevContextLim =
//		m_psegPrev->m_ichwAssocsLim - m_psegPrev->m_ichwAssocsMin + m_psegPrev->m_ichwMin;
	int cchwPrevAssocsMin = m_psegPrev->m_ichwAssocsMin;
	int cchwPrevAssocsLim = m_psegPrev->m_ichwAssocsLim;

	//	Loop over associations appended to the end of the previous segment;
	//	ichwPrev and ichwThis are relative to the start of their respective segments.
	for (int ichwPrev = ichwPrevLim - ichwPrevMin; ichwPrev < cchwPrevAssocsLim; ++ichwPrev)
	{
		int ichwThis = ichwPrev - (ichwPrevLim - ichwPrevMin);	// relative to start of this seg
		if (m_psegPrev->m_prgisloutBefore[ichwPrev - cchwPrevAssocsMin] < kPosInfinity)
			//	The "before" value is in the previous segment, so make this segment's
			//	before value invalid.
			m_prgisloutBefore[ichwThis - m_ichwAssocsMin] = kNegInfinity;
	}
}

/*----------------------------------------------------------------------------------------------
	For any associations that overlap between the previous segment and this, adjust
	this segment's before array appropriately. Specifically, for any characters that
	are rendered in both, set this segment's before value to something invalid.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void Segment::AdjustBeforeArrayFromPrevSeg()
{
	Assert(m_psegPrev);

	int ichwPrevMin = m_psegPrev->m_ichwMin;
	int ichwPrevLim = m_psegPrev->m_ichwLim;
	int ichwPrevAssocsMin = m_psegPrev->m_ichwAssocsMin;
	int ichwPrevAssocsLim = m_psegPrev->m_ichwAssocsLim;

	//	Loop over associations appended to the end of the previous segment
	//	(ie, rendered in the previous segment but officially in this one);
	//	ichwPrev and ichwThis are relative to the start of their respective segments.
	for (int ichwPrev = ichwPrevLim - ichwPrevMin; ichwPrev < ichwPrevAssocsLim; ++ichwPrev)
	{
		int ichwThis = ichwPrev - (ichwPrevLim - ichwPrevMin);	// relative to start of this seg
		if (m_psegPrev->m_prgisloutBefore[ichwPrev - ichwPrevAssocsMin] < kPosInfinity)
			//	The "before" value is in the previous segment, so make this segment's
			//	before value invalid.
			m_prgisloutBefore[ichwThis - m_ichwAssocsMin] = kNegInfinity;
	}
	//	Loop over associations prepended to the start of this segment
	//	(ie, rendered in this segment but officially in the previous one);
	//	ichwPrev and ichwThis are relative to the start of their respective segments.
	for (int ichwThis = m_ichwAssocsMin; ichwThis < 0; ++ichwThis)
	{
		int ichwPrev = ichwThis + (ichwPrevLim - ichwPrevMin);
		if (m_psegPrev->m_prgisloutBefore[ichwPrev - ichwPrevAssocsMin] < kPosInfinity)
			//	The "before" value is in the previous segment, so make this segment's
			//	before value invalid.
			m_prgisloutBefore[ichwThis - m_ichwAssocsMin] = kNegInfinity;
	}
}

/*----------------------------------------------------------------------------------------------
	For any associations that overlap between this segment and the next, adjust
	this segment's after array appropriately. Specifically, for any characters that
	are rendered in both, set this segment's after value to something invalid.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void Segment::AdjustAfterArrayFromNextSeg()
{
	Assert(m_psegNext);

	int ichwNextMin = m_psegNext->m_ichwMin;
	int ichwNextContextMin = ichwNextMin + m_psegNext->m_ichwAssocsMin;
	int ichwNextAssocsMin = m_psegNext->m_ichwAssocsMin;

	//	Loop over associations prepended to the beginning of the following segment
	//	(ie, rendered in the next segment but officially in this one);
	//	ichwThis and ichwNext are relative to the start of their respective segments.
	for (int ichwNext = cchwNextAssocsMin; ichwNext < 0; ++ichwNext)
	{
		int ichwThis = ichwNext + (m_ichwLim - m_ichwMin);
		if (m_psegNext->m_prgisloutAfter[ichwNext - ichwNextAssocsMin] > kNegInfinity)
			//	The "after" value is in the next segment, so make this segment's
			//	after value invalid.
			m_prgisloutAfter[ichwThis - m_ichwAssocsMin] = kPosInfinity;
	}

	//	Loop over associations appended to the end of this segment
	//	(ie, rendered in this segment but officially in the next one);
	//	ichwThis and ichwNext are relative to the start of their respective segments.
	for (int ichwThis = m_ichwLim - m_ichwMin; ichwThis < m_cchwAssocsLim; ++ichwThis)
	{
		int ichwNext = ichwThis - (m_ichwLim - m_ichwMin);
		if (m_psegNext->m_prgisloutAfter[ichwNext - ichwNextAssocsMin] > kNegInfinity)
			//	The "after" value is in the next segment, so make this segment's
			//	after value invalid.
			m_prgisloutAfter[ichwThis - m_ichwAssocsMin] = kPosInfinity;
	}
}

#endif  //	--------------------------  END OF OBSOLETE CODE  ----------------------------------


/*----------------------------------------------------------------------------------------------
	Generate a list of all the glyphs that are attached to the base with the given index.
	Note that we want to generate glyph indices, not slot indices.

	@param disloutCluster	- indicates how far on either side of the base to look.
----------------------------------------------------------------------------------------------*/
void Segment::ClusterMembersForGlyph(int isloutBase, int disloutCluster,
	std::vector<int> & visloutRet)
{
	for (int islout = max(0, isloutBase - disloutCluster);
		islout < min(m_cslout, isloutBase + disloutCluster + 1);
		islout++)
	{
		if (islout == isloutBase)
			continue;	// don't include the base itself
		GrSlotOutput * pslout = m_prgslout + islout;
		if (pslout->ClusterBase() == isloutBase)
		{
			visloutRet.push_back(islout);
		}
	}
}

/*----------------------------------------------------------------------------------------------
	Record an underlying-to-surface association mapping, based on the fact that there is
	a corresponding surface-to-underlying association in the streams.

	@param ichwUnder	- character index relative to the official beginning of the segment
	@param islotSurface	- surface glyph it maps to
----------------------------------------------------------------------------------------------*/
void Segment::RecordSurfaceAssoc(int ichwUnder, int islotSurface, int nDir)
{
	//	If we are recording an association across the line boundary, make sure there
	//	is space for it.
	EnsureSpaceAtLineBoundaries(ichwUnder);

	//	For left-to-right chars, the before-value is the minimum of the previous
	//	and new values, and the after-value is the maximum.
	if (nDir % 2 == 0)
	{
		// left-to-right
		m_prgisloutBefore[ichwUnder - m_ichwAssocsMin] =
			min(m_prgisloutBefore[ichwUnder - m_ichwAssocsMin], islotSurface);
		m_prgisloutAfter[ ichwUnder - m_ichwAssocsMin] =
			max(m_prgisloutAfter[ ichwUnder - m_ichwAssocsMin], islotSurface);
	}
	else
	{
		// right-to-left: the before-value is the max and the after-value is the min
		m_prgisloutBefore[ichwUnder - m_ichwAssocsMin] =
			max(m_prgisloutBefore[ichwUnder - m_ichwAssocsMin], islotSurface);
		m_prgisloutAfter[ ichwUnder - m_ichwAssocsMin] =
			min(m_prgisloutAfter[ ichwUnder - m_ichwAssocsMin], islotSurface);
	}

	m_prgpvisloutAssocs[ichwUnder - m_ichwAssocsMin]->push_back(islotSurface);
}

/*----------------------------------------------------------------------------------------------
	Record an underlying-to-surface ligature mapping, based on the fact that there is
	a component.ref attribute in the surface stream. Note that if there was a previous
	ligature value there, it will be overwritten.

	@param ichwUnder		- character index relative to the official beginning of the segment
	@param islotSurface		- surface glyph it maps to
	@param iComponent		- which component this glyph represents
----------------------------------------------------------------------------------------------*/
void Segment::RecordLigature(int ichwUnder, int islotSurface, int iComponent)
{
	//	If we are recording a mapping across the line boundary, make sure there
	//	is space for it.
	EnsureSpaceAtLineBoundaries(ichwUnder);

	Assert(m_prgisloutLigature[ichwUnder - m_ichwAssocsMin] == kNegInfinity);

	m_prgisloutLigature[ichwUnder - m_ichwAssocsMin] = islotSurface;
	Assert(iComponent < kMaxComponentsPerGlyph);
	m_prgiComponent[    ichwUnder - m_ichwAssocsMin] = sdata8(iComponent);
}

/*----------------------------------------------------------------------------------------------
	The given slot is one that is rendered in the previous segment.
	If it is associated with a character officially in this segment, mark the
	before-association kNegInfinity.

	@param ichwUnder		- character index relative to the official beginning of the segment
	@param islot			- processed glyph it maps to
----------------------------------------------------------------------------------------------*/
void Segment::MarkSlotInPrevSeg(int ichwUnder, int islot)
{
	if (ichwUnder >= m_ichwAssocsMin)
		m_prgisloutBefore[ichwUnder - m_ichwAssocsMin] = kNegInfinity;
}

/*----------------------------------------------------------------------------------------------
	The given slot is one that is rendered in the following segment.
	If it is associated with a character officially in this segment, mark the
	after-association kPosInfinity.

	@param ichwUnder		- character index relative to the official beginning of the segment
	@param islot			- processed glyph it maps to
----------------------------------------------------------------------------------------------*/
void Segment::MarkSlotInNextSeg(int ichwUnder, int islot)
{
	if (ichwUnder < m_ichwAssocsLim)
		m_prgisloutAfter[ichwUnder - m_ichwAssocsMin] = kPosInfinity;
}

/*----------------------------------------------------------------------------------------------
	If we are recording an association across the line boundary (in either direction),
	make sure there is space for it. This involves adding space to the beginning or
	the end of the association arrays, and adjusting the min and lim indicators.
----------------------------------------------------------------------------------------------*/
void Segment::EnsureSpaceAtLineBoundaries(int ichwUnder)
{
	int cchwNewMin = min(ichwUnder, m_ichwAssocsMin);
	int cchwNewLim = max(ichwUnder+1, m_ichwAssocsLim);
	if (cchwNewMin < m_ichwAssocsMin || cchwNewLim > m_ichwAssocsLim)
	{
		//	Make space either at the beginning or the end of the arrays.
		int cchwPreAdd = m_ichwAssocsMin - cchwNewMin;
		int cchwPostAdd = cchwNewLim - m_ichwAssocsLim;

		int * prgisloutTmp = m_prgisloutBefore;
		m_prgisloutBefore = new int[cchwNewLim - cchwNewMin];
		std::copy(prgisloutTmp, prgisloutTmp + (m_ichwAssocsLim - m_ichwAssocsMin), 
					m_prgisloutBefore + cchwPreAdd);
		delete[] prgisloutTmp;

		prgisloutTmp = m_prgisloutAfter;
		m_prgisloutAfter = new int[cchwNewLim - cchwNewMin];
		std::copy(prgisloutTmp, prgisloutTmp + (m_ichwAssocsLim - m_ichwAssocsMin), 
					m_prgisloutAfter + cchwPreAdd);
		delete[] prgisloutTmp;

		std::vector<int> ** ppvisloutTmp = m_prgpvisloutAssocs;
		m_prgpvisloutAssocs = new std::vector<int> * [cchwNewLim - cchwNewMin];
		std::swap_ranges(m_prgpvisloutAssocs + cchwPreAdd,
						m_prgpvisloutAssocs + cchwPreAdd +
						(m_ichwAssocsLim - m_ichwAssocsMin), ppvisloutTmp);
		delete[] ppvisloutTmp;

		prgisloutTmp = m_prgisloutLigature;
		m_prgisloutLigature = new int[cchwNewLim - cchwNewMin];
		std::copy(prgisloutTmp, prgisloutTmp + (m_ichwAssocsLim - m_ichwAssocsMin),
				m_prgisloutLigature + cchwPreAdd);
		delete[] prgisloutTmp;

		sdata8 * prgiCompTmp = m_prgiComponent;
		m_prgiComponent = new sdata8[cchwNewLim - cchwNewMin];
		std::copy(prgiCompTmp, prgiCompTmp + (m_ichwAssocsLim - m_ichwAssocsMin),
					m_prgiComponent + cchwPreAdd);
		delete[] prgiCompTmp;

		//	Initialize new slots.
		int i;
		for (i = 0; i < cchwPreAdd; ++i)
		{
			m_prgisloutBefore[i] = kPosInfinity;
			m_prgisloutAfter[i]  = kNegInfinity;
			m_prgpvisloutAssocs[i] = new std::vector<int>;
			m_prgisloutLigature[i] = kNegInfinity;
			m_prgiComponent[i] = 0;
		}
		for (i = m_ichwAssocsLim - m_ichwAssocsMin + cchwPreAdd;
			i < m_ichwAssocsLim - m_ichwAssocsMin + cchwPreAdd + cchwPostAdd;
			++i)
		{
			m_prgisloutBefore[i] = kPosInfinity;
			m_prgisloutAfter[i]  = kNegInfinity;
			m_prgpvisloutAssocs[i] = new std::vector<int>;
			m_prgisloutLigature[i] = kNegInfinity;
			m_prgiComponent[i] = 0;
		}
		m_ichwAssocsMin = cchwNewMin;
		m_ichwAssocsLim = cchwNewLim;
	}
}

/*----------------------------------------------------------------------------------------------
	Return the given output slot.
----------------------------------------------------------------------------------------------*/
GrSlotOutput * Segment::OutputSlot(int islout)
{
	return m_prgslout + islout;
}

/*----------------------------------------------------------------------------------------------
	Set the rendering engine to the current one.
----------------------------------------------------------------------------------------------*/
void Segment::SetEngine(GrEngine * pgreng)
{
	//if (m_preneng)
	//	m_preneng->DecRefCount();
	m_preneng = pgreng;
	//if (m_preneng)
	//	m_preneng->IncRefCount();
}

/*----------------------------------------------------------------------------------------------
	Set the font.
----------------------------------------------------------------------------------------------*/
void Segment::SetFont(Font * pfont)
{
	m_pfont = pfont->copyThis();

//	m_fBold = pfont->bold();
//	m_fItalic = pfont->italic();
	// Note that we store the character height (which does not include the internal leading),
	// not the actual font height, ie, ascent + descent. This is what is used in the LOGFONT.
	pfont->getFontMetrics(NULL, NULL, &m_xysEmSquare); // m_xysEmSquare = m_pixHeight
}

/*----------------------------------------------------------------------------------------------
	Switch the font out.
	??? Do we need to change the member variables too?
----------------------------------------------------------------------------------------------*/
void Segment::SwitchFont(Font * pfont)
{
	m_pfont = pfont;
}

/*----------------------------------------------------------------------------------------------
	Return the engine implementation if it is a proper GrEngine, or NULL otherwise.
----------------------------------------------------------------------------------------------*/
GrEngine * Segment::EngineImpl()
{
	return m_preneng;
}

/*----------------------------------------------------------------------------------------------
	Set the justification agent, in case this segment will ever need to be stretched
	or shrunk.
----------------------------------------------------------------------------------------------*/
void Segment::SetJustifier(IGrJustifier * pgjus)
{
	m_pgjus = pgjus;
	//if (pgjus)
	//	pgjus->JustifierObject(&m_pgjus);
	//else
	//	m_pgjus = NULL;
}

/*----------------------------------------------------------------------------------------------
	Set the graphics object to use the base font.
----------------------------------------------------------------------------------------------*/
//void Segment::SetToBaseFont(IGrGraphics * pgg)
//{
//	if (m_fUseSepBase)
//		EngineImpl()->SwitchGraphicsFont(pgg, true);
//}

/*----------------------------------------------------------------------------------------------
	Restore the graphics object to use the font that it originally had when passed in to
	the interface method.
----------------------------------------------------------------------------------------------*/
//void Segment::RestoreFont(IGrGraphics * pgg)
//{
//	if (m_fUseSepBase)
//		EngineImpl()->SwitchGraphicsFont(pgg, false);
//}

/*----------------------------------------------------------------------------------------------
	Given a logical surface location, return the underlying position (relative to the
	beginning of the string). NOTE that we are returning the index of the closest charater
	(plus an indication of which side the click was on), not a position between characters.

	@param islout			- output slot
	@param xsOffset			- x coordinate of physical click, relative to left of glyph;
								kPosInfinity means all the way to the right;
								kNegInfinity means all the way to the left
	@param ysOffset			- y coordinate of physical click, relative to TOP of segment (NOT
								upwards from baseline); kPosInfinity means all the way at
								the top; kNegInfinity means all the way at the bottom
	@param dxsGlyphWidth	- width of glyph clicked on, or 0 if irrelevant
	@param dysGlyphHeight	- actually line height -- CURRENTLY NOT USED
	@param pfAfter			- return true if they clicked on trailing side; possibly NULL
----------------------------------------------------------------------------------------------*/
int Segment::LogicalSurfaceToUnderlying(int islout, float xsOffset, float ysClick,
	float dxsGlyphWidth, float dysGlyphHeight, bool * pfAfter)
{
	Assert(islout >= 0);
	Assert(islout < m_cslout);

	GrSlotOutput * pslout = OutputSlot(islout);
	bool fGlyphRtl = SlotIsRightToLeft(pslout);

	int dichw;
	bool fAfter, fRight;

	float ysForGlyph = pslout->YPosition();

	float dysFontAscent = m_dysFontAscent;
	////GrResult res = GetFontAscentSourceUnits(pgg, &dysFontAscent);
	////if (ResultFailed(res))
	////	THROW(WARN(res));
	dysFontAscent += m_dysXAscent;

	float dysOffset = ysClick - (m_dysAscent - dysFontAscent); // relative to top of 0-baseline text

	//	If the slot has components, see if the offset falls within one of them.

	if (pslout->NumberOfComponents() > 0)
	{
		GrEngine * pgreng = EngineImpl();
		if (!pgreng)
			goto LNotLigature;

		GrGlyphTable * pgtbl = pgreng->GlyphTable();

		for (int icomp = 0; icomp < pslout->NumberOfComponents(); icomp++)
		{
			float xsLeft, xsRight, ysTop, ysBottom;

			// TODO: Rework ComponentIndexForGlyph to take slati rather than iForGlyph.
			int slati = pslout->ComponentId(icomp);
			int iForGlyph = pgtbl->ComponentIndexForGlyph(pslout->GlyphID(), slati);
			if (!pgtbl->ComponentBoxLogUnits(m_xysEmSquare, pslout->GlyphID(), iForGlyph,
				m_mFontEmUnits, m_dysAscent,
				&xsLeft, &ysTop, &xsRight, &ysBottom))
			{
				continue;	// component not defined
			}
			Assert(xsLeft <= xsRight);
			Assert(ysTop <= ysBottom);
			if (xsOffset < xsLeft || xsOffset > xsRight)
				continue;
			if ((dysOffset - ysForGlyph) < ysTop || (dysOffset - ysForGlyph) > ysBottom)
				continue;
			
			//	Click was within the component's box.

			fRight = (xsOffset - xsLeft > xsRight - xsOffset);
			fAfter = (fGlyphRtl) ? !fRight : fRight;
			if (pfAfter)
				*pfAfter = fAfter;

			if (fAfter)
				dichw = pslout->LastUnderlyingComponent(icomp);
			else
				dichw = pslout->FirstUnderlyingComponent(icomp);
			Assert(m_ichwMin + dichw >= 0);
			Assert(dichw < m_dichwLim);

			Assert(GrCharStream::AtUnicodeCharBoundary(m_pgts, m_ichwMin + dichw));

			return m_ichwMin + dichw;
		}
	}

LNotLigature:

	//	No relevant ligature component.
	//	If the x-offset is less than half way across,
	//	answer the before-association; otherwise answer the after-association.

	if (xsOffset == kPosInfinity)
		fRight = true;
	else if (xsOffset == kNegInfinity)
		fRight = false;
	else
		fRight = (xsOffset > (dxsGlyphWidth / 2));

	fAfter = (fGlyphRtl) ? !fRight : fRight;

	int diCharAdj = 0;
	if (fAfter)
	{
		dichw = pslout->AfterAssoc();
		if (pfAfter)
			*pfAfter = true;
		// The following should not be necessary because of the way associations are set up:
		//while (!GrCharStream::AtUnicodeCharBoundary(m_pgts, ichwBase + dichw + diCharAdj))
		//	diCharAdj++;
	}
	else
	{
		dichw = pslout->BeforeAssoc();
		if (pfAfter)
			*pfAfter = false;
		// The following should not be necessary because of the way associations are set up:
		//while (!GrCharStream::AtUnicodeCharBoundary(m_pgts, ichwBase + dichw + diCharAdj))
		//	diCharAdj--;
	}

	if (dichw == kNegInfinity || dichw == kPosInfinity)
		return dichw;

	// This should be true because of the way the associations are set up:
	Assert(GrCharStream::AtUnicodeCharBoundary(m_pgts, m_ichwMin + dichw)); // + diCharAdj));

	return m_ichwMin + dichw + diCharAdj;
}

/*--------------------------------------------------------------------------------------------*/

int Segment::LogicalSurfaceToUnderlying(int islout, bool fBefore)
{
	if (fBefore)
		return LogicalSurfaceToUnderlying(islout, kNegInfFloat, kNegInfFloat);
	else
		return LogicalSurfaceToUnderlying(islout, kPosInfFloat, kPosInfFloat);
}

/*----------------------------------------------------------------------------------------------
	Given a physical surface location (an index into the glyph boxes),
	return the underlying position, relative to the beginning of the string.

	@param igbb				- index of glyph
	@param xsOffset			- relative to left side of glyph
	@param ysOffset			- downwards from top of segment (not upwards from baseline)
----------------------------------------------------------------------------------------------*/
int Segment::PhysicalSurfaceToUnderlying(
	int iginf, float xsOffset, float ysOffset,
	float dxsGlyphWidth, float dysGlyphHeight, bool * pfAfter)
{
	int islout = iginf + m_isloutGinf0;
	return LogicalSurfaceToUnderlying(islout, xsOffset, ysOffset,
		dxsGlyphWidth, dysGlyphHeight, pfAfter);
}
	
/*--------------------------------------------------------------------------------------------*/

int Segment::PhysicalSurfaceToUnderlying(int iginf, bool fBefore)
{
	int islout = iginf + m_isloutGinf0;
	bool fGlyphRtl = SlotIsRightToLeft(OutputSlot(islout));
	if (fBefore == fGlyphRtl)
		return PhysicalSurfaceToUnderlying(iginf, kPosInfFloat, kPosInfFloat);
	else
		return PhysicalSurfaceToUnderlying(iginf, kNegInfFloat, kNegInfFloat);
}

/*----------------------------------------------------------------------------------------------
	Given an underlying position (relative to the beginning of the string),
	return the logical surface location (ignoring right-to-left).
	NOTE that ichw indicates a CHARACTER, not a location between characters. So ichw = 3,
	fBefore = false indicates a selection between characters 3 and 4.
	For surrogates, it indicates the FIRST of the pair of 16-bit chars.
----------------------------------------------------------------------------------------------*/
int Segment::UnderlyingToLogicalSurface(int ichw, bool fBefore)
{
	int ichwSegOffset = ichw - m_ichwMin;

	if (ichwSegOffset < m_ichwAssocsMin)
		return kNegInfinity;	// probably rendered in previous segment


	// if the buffers aren't even allocated, then it probably means there 
	// wasn't room for any glyphs, so the following segment is most likely
//	else if (ichwSegOffset >= m_ichwAssocsLim)
	else if (ichwSegOffset >= m_ichwAssocsLim || !m_prgisloutBefore || !m_prgisloutAfter)
		return kPosInfinity;	// probably rendered in following segment

	else if (fBefore)
	{
		int isloutRet;
		int ichw = ichwSegOffset;
		//	If no association has been made, loop forward to the next slot
		//	we are before. As a last resort, answer kPosInfinity, meaning we
		//	aren't before anything.
		do
		{
			isloutRet = m_prgisloutBefore[ichw - m_ichwAssocsMin];
			do { ++ichw; }
			while (!GrCharStream::AtUnicodeCharBoundary(m_pgts, ichw));
		} while (isloutRet == kPosInfinity && ichw < m_ichwAssocsLim);
		return isloutRet;
	}
	else
	{
		int isloutRet;
		int ichw = ichwSegOffset;
		//	If no association has been made, loop backward to the previous slot
		//	we are after. As a last resort, answer kNegInfinity, meaning we
		//	aren't after anything.
		do
		{
			isloutRet = m_prgisloutAfter[ichw - m_ichwAssocsMin];
			do { --ichw; }
			while (!GrCharStream::AtUnicodeCharBoundary(m_pgts, ichw));
		} while (isloutRet == kNegInfinity && ichw >= 0);
		return isloutRet;
	}
	Assert(false); // should never reach here
}

/*----------------------------------------------------------------------------------------------
	Given an underlying position (relative to the beginning of the string),
	return the physical surface location, the index into m_prgginf.
----------------------------------------------------------------------------------------------*/
int Segment::UnderlyingToPhysicalSurface(int ichw, bool fBefore)
{
	int islout = UnderlyingToLogicalSurface(ichw, fBefore);
	return LogicalToPhysicalSurface(islout);
}

/*----------------------------------------------------------------------------------------------
	Given a logical position (index into output slots), return the physical position,
	the index into the list of glyphs (m_prgginf).
----------------------------------------------------------------------------------------------*/
int Segment::LogicalToPhysicalSurface(int islout)
{
	if (islout == kNegInfinity || islout == kPosInfinity)
		return islout;

	Assert(m_prgginf[islout - m_isloutGinf0].logicalIndex() == (unsigned)(islout - m_isloutGinf0));

	return islout - m_isloutGinf0;
}

/*----------------------------------------------------------------------------------------------
	Given an underlying position (relative to the beginning of the string),
	return the physical surface locations of all the associated surface glyphs.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
#if 0
void Segment::UnderlyingToPhysicalAssocs(int ichw, std::vector<int> & viginf)
{
	int ichwSegOffset = ichw - m_ichwMin;

	if (ichwSegOffset < m_ichwAssocsMin)
		return;	// possibly rendered in previous segment

	else if (ichwSegOffset >= m_ichwAssocsLim)
		return;	// possibly rendered in following segment

	else
	{
		std::vector<int> * pvisloutTmp = &(m_prgvisloutAssocs[ichwSegOffset - m_ichwAssocsMin]);

		for (size_t i = 0; i < pvisloutTmp->size(); i++)
		{
			int islout = (*pvisloutTmp)[i];
			Assert(islout != kNegInfinity);
			Assert(islout != kPosInfinity);
			viginf.push_back(LogicalToPhysicalSurface(islout));
		}
	}
}
#endif

/*----------------------------------------------------------------------------------------------
	Given an underlying character position (relative to the beginning of the string),
	return a pointer to the vector containing the logical surface locations of
	all the associated surface glyphs.

	Returns an empty vector (not something containing infinities) if the character is
	"invisible."
----------------------------------------------------------------------------------------------*/
std::vector<int> Segment::UnderlyingToLogicalAssocs(int ichw)
{
	std::vector<int> vnEmpty;
	vnEmpty.clear();
	Assert(vnEmpty.size() == 0);

	int ichwSegOffset = ichw - m_ichwMin;

	if (ichwSegOffset < m_ichwAssocsMin)
		return vnEmpty;	// probably rendered in previous segment

	else if (ichwSegOffset >= m_ichwAssocsLim)
		return vnEmpty;	// probably rendered in following segment

	else if (m_prgpvisloutAssocs[ichwSegOffset - m_ichwAssocsMin] == NULL)
	{
		// Create a vector using the before and after values.
		std::vector<int> visloutRet;
		int isloutBefore = m_prgisloutBefore[ichwSegOffset - m_ichwAssocsMin];
		int isloutAfter = m_prgisloutAfter[ichwSegOffset - m_ichwAssocsMin];
		if (isloutBefore != kPosInfinity && isloutBefore != kNegInfinity)
            visloutRet.push_back(isloutBefore);
		if (isloutAfter != kPosInfinity && isloutAfter != kNegInfinity && isloutBefore != isloutAfter)
		{
			visloutRet.push_back(isloutAfter);
		}
		return visloutRet;
	}
	else
	{
		std::vector<int> * pvisloutRet = m_prgpvisloutAssocs[ichwSegOffset - m_ichwAssocsMin];
		return *pvisloutRet;
	}
}

/*----------------------------------------------------------------------------------------------
	Given an underlying character position (relative to the beginning of the string),
	return an index of a slot in this segment that can used to test things like
	InsertBefore. Don't return kNegInfinity or kPosInfinity except as a last resort.
----------------------------------------------------------------------------------------------*/
int Segment::UnderlyingToLogicalInThisSeg(int ichw)
{
	int isloutTest = kNegInfinity;
	std::vector<int> vislout = UnderlyingToLogicalAssocs(ichw);
	for (size_t iislout = 0; iislout < vislout.size(); iislout++)
	{
		isloutTest = vislout[iislout];
		if (isloutTest != kNegInfinity && isloutTest != kPosInfinity)
			return isloutTest;
	}
	return isloutTest;
}

/*----------------------------------------------------------------------------------------------
	Return true if the two characters map to exactly the same set of glyphs.
----------------------------------------------------------------------------------------------*/
bool Segment::SameSurfaceGlyphs(int ichw1, int ichw2)
{
    std::vector<int> vislout1 = UnderlyingToLogicalAssocs(ichw1);
	std::vector<int> vislout2 = UnderlyingToLogicalAssocs(ichw2);

	bool fRet = true;
	if (vislout1.size() == 0 || vislout2.size() == 0)
		fRet = false;
	else if (vislout1.size() != vislout2.size())
		fRet = false;
	else
	{
		for (size_t islout = 0; islout < vislout1.size(); islout++)
		{
			if (vislout1[islout] != vislout2[islout])
			{
				fRet = false;
				break;
			}
		}
	}
	return fRet;
}

/*----------------------------------------------------------------------------------------------
	Return the direction level of the given character.
----------------------------------------------------------------------------------------------*/
int Segment::DirLevelOfChar(int ichw, bool fBefore)
{
	if (m_twsh == ktwshOnlyWs)
		return m_nDirDepth;
	int islot = UnderlyingToLogicalSurface(ichw, fBefore);
	if (islot == kNegInfinity || islot == kPosInfinity)
		return 0;
	int nDir = OutputSlot(islot)->DirLevel();
	if (nDir == -1)
		//	Not calculated: assume to be the top direction level.
		nDir = TopDirLevel();

	return nDir;
}

/*----------------------------------------------------------------------------------------------
	Return whether the given character is right-to-left.
----------------------------------------------------------------------------------------------*/
int Segment::CharIsRightToLeft(int ichw, bool fBefore)
{
	int nDir = DirLevelOfChar(ichw, fBefore);
	return ((nDir % 2) != 0);
}

/*----------------------------------------------------------------------------------------------
	Return whether the given slot is right-to-left.
----------------------------------------------------------------------------------------------*/
int Segment::SlotIsRightToLeft(GrSlotOutput * pslout)
{
	if (m_twsh == ktwshOnlyWs)
		return (m_nDirDepth % 2);
	int nDir = pslout->DirLevel();
	if (nDir == -1)
		nDir = TopDirLevel();
	return (nDir % 2);
}

/*----------------------------------------------------------------------------------------------
	Check to make sure that the attachments are valid. Specifically, for every glyph that
	is part of a cluster, all glyphs between that glyph and its root must (ultimately) have
	that root glyph as one of its roots.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void Segment::AssertValidClusters(GrSlotStream * psstrm)
{
#ifdef _DEBUG
	for (int islot = 0; islot < psstrm->WritePos(); islot++)
	{
		GrSlotState * pslotThis = psstrm->SlotAt(islot);
		GrSlotState * pslotRoot = pslotThis->AttachRoot(psstrm);
		if (!pslotRoot)
			continue;
		Assert(pslotRoot != pslotThis);

		int inc = (pslotThis->PosPassIndex() > pslotRoot->PosPassIndex()) ? -1 : 1;

		for (int islot2 = pslotThis->PosPassIndex() + inc;
			islot2 != pslotRoot->PosPassIndex();
			islot2 += inc)
		{
			GrSlotState * pslotMid = psstrm->SlotAt(islot2);
			Assert(pslotMid->HasAsRoot(psstrm, pslotRoot));
		}
	}
#endif // _DEBUG
}

/*----------------------------------------------------------------------------------------------
	Return information about (editing) clusters as appropriate for Uniscribe, ie, consistent
	with the ScriptShape funciton.
	A cluster includes all attachments, all clumps of characters created by insert = false,
	and clumps of reordered glyphs.

	prgiginfFirstOfCluster returns the first glyph of the cluster for each CHARACTER
	(glyph indices returned are zero-based and refer to physical order--left-to-right, etc.).
	pfClusterStart returns a flag for each GLYPH indicating whether it is the first (left-most)
	of a cluster.
	Both arrays are optional.
	CORRECTION: for now we use strictly logical order. It's not clear whether physical
	order is needed for right-to-left text or not. See note below.

	TODO: convert from GlyphInfo objects which are RTL.
----------------------------------------------------------------------------------------------*/
GrResult Segment::getUniscribeClusters(
	int * prgiginfFirstOfCluster, int cchMax, int * pcch,
	bool * pfClusterStart, int cfMax, int * pcf)
{
	ChkGrOutPtr(pcch);
	if (prgiginfFirstOfCluster)
		ChkGrArrayArg(prgiginfFirstOfCluster, cchMax);
	ChkGrOutPtr(pcf);
	if (pfClusterStart)
		ChkGrArrayArg(pfClusterStart, cfMax);

	GrResult res = kresOk;

	//SetUpGraphics(ichwBase, pgg, true);

	if (m_dxsWidth == -1)
		ComputeDimensions();

	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	if (pcch)
		*pcch = m_dichwLim;
	if (pcf)
		*pcf = m_cginf;
	if (cchMax < m_dichwLim)
	{
		if (cchMax == 0 && cfMax == 0)
			res = kresFalse; // just asking for size information
		else if (prgiginfFirstOfCluster || cchMax > 0)
			res = kresInvalidArg; // not enough space
	}
	if (cfMax < m_cginf)
	{
		if (cchMax == 0 && cfMax == 0)
			res = kresFalse;
		else if (pfClusterStart || cfMax > 0)
			res = kresInvalidArg;
	}
	if (res != kresOk)
	{
		//RestoreFont(pgg);
		ReturnResult(res);
	}

	//	Generate a pair of arrays, indicating the first and last (logical) slots
	//	of the cluster for each character. Initially clusters are based just on 
	//	character-to-glyph associations, but they expand as we process 
	//	insert=false settings, attachments, and reordering.

	std::vector<int> visloutBefore;
	std::vector<int> visloutAfter;
	visloutBefore.resize(m_dichwLim);
	visloutAfter.resize(m_dichwLim);

	int ich;
	for (ich = 0; ich < m_dichwLim; ich++)
	{
		visloutBefore[ich] = m_prgisloutBefore[ich - m_ichwAssocsMin];
		visloutAfter[ich]  = m_prgisloutAfter[ich - m_ichwAssocsMin];
	}

	if (m_dichwLim == 1)
	{
		//	Only one character, therefore only one cluster.
		visloutBefore[0] = m_isloutGinf0;
		visloutAfter[0] = max(m_cginf - 1, 0) + m_isloutGinf0;
	}
	else
	{
		//	Make all glyphs attached to each other part of a cluster, or glyphs where
		//	insertion is not allowed.
		for (ich = 0; ich < m_dichwLim; ich++)
		{
			std::vector<int> vislout = UnderlyingToLogicalAssocs(ich + m_ichwMin);

			if (vislout.size() == 0)
			{
				// No glyphs represent this character. Merge it with the previous
				// character.
				visloutBefore[ich] = (ich == 0) ? visloutBefore[ich + 1] : visloutBefore[ich - 1];
				visloutAfter[ich] = (ich == 0) ? visloutAfter[ich + 1] : visloutAfter[ich - 1];
				continue;
			}

			size_t iislout;
			for (iislout = 0; iislout < vislout.size(); iislout++)
			{
				int islout = vislout[iislout];
				if (!m_prgslout[islout].InsertBefore())
				{
					// This glyph does not allow insertion before it; make its character part
					// of the same cluster as the previous character.
					MergeUniscribeCluster(visloutBefore, visloutAfter, ich - 1, ich);
/*
+					// Keith Stribley's fix:
+					// NOTE: this is NOT always the same as the previous character, so
+					// check whether the next character actually has a glyph before this
+					// one
+					if ((ich == m_dichwLim - 1) || 
+					    (std::min(visloutBefore[ich + 1], visloutAfter[ich + 1]) > 
+					     std::max(visloutBefore[ich], visloutAfter[ich])))
+					{
+					  MergeUniscribeCluster(visloutBefore, visloutAfter, ich - 1, ich);
+					}
+					else
+					  MergeUniscribeCluster(visloutBefore, visloutAfter, ich, ich + 1);
*/
				}
				else if (m_prgslout[islout].ClusterBase() != -1
					&& m_prgslout[islout].ClusterBase() != islout)
				{
					// This glyph is attached to something; make them part of the same cluster.
					int isloutBase = m_prgslout[islout].ClusterBase();
					int ichBase = LogicalSurfaceToUnderlying(isloutBase, false) - m_ichwMin;
					MergeUniscribeCluster(visloutBefore, visloutAfter,
						ich, ichBase);
					int ichBase1 = LogicalSurfaceToUnderlying(isloutBase, false) - m_ichwMin;
					if (ichBase1 != ichBase)
						MergeUniscribeCluster(visloutBefore, visloutAfter,
							ich, ichBase1);
				}
			}
		}

		// Handle reordering. Whenever the islout values get out of order, we need to
		// merge into a single cluster.
		for (ich = 0; ich < m_dichwLim - 1; ich++)
		{
			Assert(visloutBefore[ich] <= visloutAfter[ich]);
			Assert(visloutBefore[ich] != kPosInfinity);
			Assert(visloutBefore[ich] != kNegInfinity);
			Assert(visloutAfter[ich] != kPosInfinity);
			Assert(visloutAfter[ich] != kNegInfinity);
			const int b1 = visloutBefore[ich];
			const int a1 = visloutAfter[ich];
			const int b2 = visloutBefore[ich + 1];
			const int a2 = visloutAfter[ich + 1];
			if (b1 == b2 && a1 == a2)
				continue; // already a cluster

			if (std::min(b2, a2) <= std::max(b1, a1))
			{
				MergeUniscribeCluster(visloutBefore, visloutAfter, ich, ich + 1);
			}
		}
	}

	if (pfClusterStart)
	{
		//	Initialize.
		int iginf;
		for (iginf = 0; iginf < m_cginf; iginf++)
			pfClusterStart[iginf] = false;
	}

	//	To skip line-break slots; remember that the number of output slots equals the
	//	number of gbbs plus initial and/or final line-break slots if any.
	//	In this case the beginning of the first cluster indicates the first real glyph.
	int isloutFirstReal = (m_dichwLim) ? ((m_fWsRtl)? visloutAfter[0] : visloutBefore[0]) : 0;

	//	Kludge to make assertions below easier:
	if (m_dichwLim > 0)
	{
		visloutBefore.push_back(visloutAfter.back() + 1);
		visloutAfter.push_back(visloutAfter.back() + 1);
	}

	//	Convert (logical) slots to actual glyphs.
	for (ich = 0; ich < m_dichwLim; ich++)
	{
		// NOTE: to treat the first of the cluster as the left-most, use visloutAfter for
		// right-to-left situations.

		if (prgiginfFirstOfCluster)
			prgiginfFirstOfCluster[ich]
				= LogicalToPhysicalSurface((m_fWsRtl) ? visloutAfter[ich] : visloutBefore[ich]);
		int clusterStartPos = ((m_fWsRtl) ? visloutAfter[ich] : visloutBefore[ich]) - isloutFirstReal;
		if (pfClusterStart)
			pfClusterStart[clusterStartPos] = true;

		Assert((visloutBefore[ich] == visloutBefore[ich + 1] &&
			visloutAfter[ich] == visloutAfter[ich + 1])
			|| (visloutBefore[ich] <= visloutAfter[ich] &&
				visloutAfter[ich] < visloutBefore[ich + 1]));
	}

	//RestoreFont(pgg);

	return kresOk;
}


// suppress GCC 4.3 warning for optimized min()/max() when called with (ich, ich+1) or similar
#pragma GCC diagnostic ignored "-Wstrict-overflow"

/*----------------------------------------------------------------------------------------------
	Merge the given characters into the same Uniscribe cluster. This means merging any
	intervening characters as well.
----------------------------------------------------------------------------------------------*/
void Segment::MergeUniscribeCluster(
	std::vector<int> & visloutBefore, std::vector<int> & visloutAfter,
	int ich1, int ich2)
{
	int ichStartOrig = min(ich1, ich2);
	int ichStopOrig = max(ich1, ich2);

	int isloutMin = visloutBefore[ichStopOrig];
	int isloutMax = visloutAfter[ichStartOrig];

	int ichStart = ichStopOrig;
	int ichStop = ichStartOrig;
	// Find the beginning of the cluster.
	while (ichStart > 0 &&
		(ichStart > ichStartOrig
			|| visloutBefore[ichStart - 1] >= visloutBefore[ichStopOrig]
			|| visloutBefore[ichStart - 1] == visloutBefore[ichStart]))
	{
		ichStart--;
		isloutMin = std::min(isloutMin, visloutBefore[ichStart]);
		isloutMax = std::max(isloutMax, visloutAfter[ichStart]);
	}
	// Find end of cluster.
	while (ichStop < m_dichwLim - 1 &&
		(ichStop < ichStopOrig
			|| visloutAfter[ichStop + 1] <= visloutAfter[ichStartOrig]
			|| visloutAfter[ichStop + 1] == visloutAfter[ichStop]))
	{
		ichStop++;
		isloutMin = std::min(isloutMin, visloutBefore[ichStop]);
		isloutMax = std::max(isloutMax, visloutAfter[ichStop]);
	}

	int ich;
	for (ich = ichStart; ich <= ichStop; ich++)
	{
		visloutBefore[ich] = isloutMin;
		visloutAfter[ich] = isloutMax;
	}
}

/*----------------------------------------------------------------------------------------------
	Return information about the positioning of glyphs as appropriate for Uniscribe.
	The advance widths returned are the natural ones (what would be applied if no smart
	rendering were happening), and the x-offsets are the difference between the
	natural location of the glyph and what is produced by Graphite. The y-offset
	is the same as GetGlyphsAndPositions.

	This is an experimental method. Its behavior may change signficantly in the future.
----------------------------------------------------------------------------------------------*/
/*
GrResult Segment::GetUniscribePositions(
	Rect rs, Rect rd, int cgidMax, int *pcgidRet,
	float * prgxd, float * prgyd, float * prgdxdAdv)
{
	ChkGrOutPtr(pcgidRet);
	ChkGrArrayArg(prgxd, cgidMax);
	ChkGrArrayArg(prgyd, cgidMax);
	ChkGrArrayArg(prgdxdAdv, cgidMax);

	GrResult res;

	//SetUpGraphics(ichBase, pgg, true);

	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	*pcgidRet = m_cginf;
	if (cgidMax < m_cginf)
	{
		res = (cgidMax == 0) ? kresFalse : kresInvalidArg;
		//RestoreFont(pgg);
		ReturnResult(res);
	}

	if (m_dxsWidth < 0)
		ComputeDimensions();

	float * rgxdTmp = new float[cgidMax];
	float * rgdxdAdvTmp = new float[cgidMax];
	res = GetGlyphsAndPositions(rs, rd, cgidMax, pcgidRet,
		NULL, rgxdTmp, prgyd, rgdxdAdvTmp);
	if (ResultFailed(res))
	{
		delete[] rgxdTmp;
		delete[] rgdxdAdvTmp;
		//RestoreFont(pgg);
		ReturnResult(res);
	}

	float xdLeft = SegmentPainter::ScaleX(0, rs, rd);
	float xdXPosNatural = xdLeft;
	for (int iginf = 0; iginf < *pcgidRet; iginf++)
	{
		int islout = iginf + m_isloutGinf0;
		GrSlotOutput * pslout = OutputSlot(islout);
		float dxsAdvNatural = pslout->AdvanceXMetric();
		float dxdAdvNatural = SegmentPainter::ScaleX(dxsAdvNatural, rs, rd) - xdLeft;
		prgdxdAdv[iginf] = dxdAdvNatural;
		prgxd[iginf] = rgxdTmp[iginf] - xdXPosNatural;

		xdXPosNatural += dxdAdvNatural;
	}

	delete[] rgxdTmp;
	delete[] rgdxdAdvTmp;

	//RestoreFont(pgg);

	ReturnResult(res);

}
*/

/*----------------------------------------------------------------------------------------------
	Return information about the positioning of glyphs as appropriate for Uniscribe.
	The advance widths returned are the natural ones (what would be applied if no smart
	rendering were happening), and the x-offsets are the difference between the
	natural location of the glyph and what is produced by Graphite. The y-offset
	is the same as GetGlyphsAndPositions.

	This is an experimental method. Its behavior may change signficantly in the future.
----------------------------------------------------------------------------------------------*/
/*
GrResult Segment::GetUniscribeGlyphsAndPositions(
	Rect rs, Rect rd, int cgidMax, int *pcgidRet, utf16 * prgchGlyphs,
	float * prgxd, float * prgyd, float * prgdxdAdv)
{
	ChkGrOutPtr(pcgidRet);
	ChkGrArrayArg(prgxd, cgidMax);
	ChkGrArrayArg(prgyd, cgidMax);
	ChkGrArrayArg(prgdxdAdv, cgidMax);

	GrResult res;

	//SetUpGraphics(ichBase, pgg, true);

	Assert(m_dxsWidth >= 0);
	Assert(m_dysAscent >= 0);

	*pcgidRet = m_cginf;
	if (cgidMax < m_cginf)
	{
		res = (cgidMax == 0) ? kresFalse : kresInvalidArg;
		//RestoreFont(pgg);
		ReturnResult(res);
	}

	if (m_dxsWidth < 0)
		ComputeDimensions();

	float * rgxdTmp = new float[cgidMax];
	float * rgdxdAdvTmp = new float[cgidMax];
	res = GetGlyphsAndPositions(rs, rd, cgidMax, pcgidRet,
		prgchGlyphs, rgxdTmp, prgyd, rgdxdAdvTmp);
	if (ResultFailed(res))
	{
		delete[] rgxdTmp;
		delete[] rgdxdAdvTmp;
		//RestoreFont(pgg);
		ReturnResult(res);
	}

	float xdLeft = SegmentPainter::ScaleX(0, rs, rd);
	float xdXPosNatural = xdLeft;
	for (int iginf = 0; iginf < *pcgidRet; iginf++)
	{
		int islout = iginf + m_isloutGinf0;
		GrSlotOutput * pslout = OutputSlot(islout);
		float dxsAdvNatural = pslout->AdvanceXMetric();
		float dxdAdvNatural = SegmentPainter::ScaleX(dxsAdvNatural, rs, rd) - xdLeft;
		prgdxdAdv[iginf] = dxdAdvNatural;
		prgxd[iginf] = rgxdTmp[iginf] - xdXPosNatural;

		xdXPosNatural += dxdAdvNatural;
	}

	delete[] rgxdTmp;
	delete[] rgdxdAdvTmp;

	//RestoreFont(pgg);

	ReturnResult(res);
}
*/

/*----------------------------------------------------------------------------------------------
	Return the visual box for the given ligature component. Return (0,0,0,0) if the component
	is not defined.
----------------------------------------------------------------------------------------------*/
Rect Segment::ComponentRect(GrSlotOutput * pslout, int icomp)
{
	Rect rectRet;
	rectRet.top = rectRet.bottom = rectRet.left = rectRet.right = 0;
	if (icomp < 0 || icomp > pslout->NumberOfComponents())
		return rectRet;

	GrEngine * pgreng = EngineImpl();
	if (!pgreng)
		return rectRet;
	GrGlyphTable * pgtbl = pgreng->GlyphTable();
	if (!pgtbl)
		return rectRet;

	float xsLeft, xsRight, ysTop, ysBottom;

	// TODO: Rework ComponentIndexForGlyph to take slati rather than iForGlyph.
	int slati = pslout->ComponentId(icomp);
	int iForGlyph = pgtbl->ComponentIndexForGlyph(pslout->GlyphID(), slati);
	if (!pgtbl->ComponentBoxLogUnits(m_xysEmSquare, pslout->GlyphID(), iForGlyph,
		m_mFontEmUnits, m_dysAscent,
		&xsLeft, &ysTop, &xsRight, &ysBottom,
		false))	// origin is baseline
	{
		return rectRet;
	}
	rectRet.top = ysTop;
	rectRet.bottom = ysBottom;
	rectRet.left = xsLeft;
	rectRet.right = xsRight;

	return rectRet;
}

/*----------------------------------------------------------------------------------------------
	Shift the glyphs physically by the given amount. This happens when a right-to-left
	segment is changed from being end-of-line to not, so we have to account for the fact
	that the trailing white-space was previous invisible but is now visible.
----------------------------------------------------------------------------------------------*/
void Segment::ShiftGlyphs(float dxsShift)
{
	for (int islout = 0; islout < m_cslout; islout++)
	{
		GrSlotOutput * pslout = OutputSlot(islout);
		pslout->AdjustPosXBy(dxsShift);
	}

	//ShiftGlyphsPlatform(dxsShift);
}

/*----------------------------------------------------------------------------------------------
	Find the glyph that best matches the given click location.

	@param xsClick		- relative to start of segment
	@param ysClick		- relative to segment baseline
----------------------------------------------------------------------------------------------*/
int Segment::GlyphHit(float xsClick, float ysClick)
{
	float xsLeft = kPosInfFloat;
	float xsRight = kNegInfFloat;

	int iginf;
	for (iginf = 0; iginf < m_cginf; iginf++)
	{
		GlyphInfo * pginf = m_prgginf + iginf;
		xsLeft = min(xsLeft, pginf->origin());
		xsRight = max(xsRight, pginf->origin() + pginf->advanceWidth());
	}

	if (xsClick < xsLeft)
		return m_fWsRtl ? m_cginf - 1 : 0;
	else if (xsClick > xsRight)
		return m_fWsRtl ? 0 : m_cginf - 1;

	int iginfHit;
	std::vector<int> viginfNear;
	std::vector<int> viginfHit;
	std::vector<int> viginfInside;

	//	Find glyph which might contain click point.

	for (iginfHit = m_cginf; iginfHit-- > 0; )
	{
		if (m_fWsRtl)
		{
			if (m_prgginf[iginfHit].bb().right >= xsClick)
				break;
		}
		else
		{
			if (m_prgginf[iginfHit].bb().left <= xsClick)
				break;
		}
	}
	if (iginfHit < 0) // click within lsb of first glyph
	{
		return 0;
	}

	//	Find all BB that OVERLAP the click point (bb.left < XClick < bb.right).
	for (iginf = iginfHit; iginf >= 0; iginf--)
	{
		Rect rectBB = m_prgginf[iginf].bb();
		if (rectBB.left <= xsClick && xsClick <= rectBB.right)
		{
			viginfNear.push_back(iginf);
		}
	}

	//	Find all BB that CONTAIN the click point (considering the click's Y coor too).
	int iiginf;
	for (iiginf = 0; iiginf < (int)viginfNear.size(); iiginf++)
	{
		Rect rectBB = m_prgginf[viginfNear[iiginf]].bb();
		if (rectBB.bottom <= ysClick && ysClick <= rectBB.top)
		{
			viginfHit.push_back(viginfNear[iiginf]);
		}
	}

	if (viginfNear.size() > 2 && viginfHit.size() == 0)
	{
		// No hit along y-axis; find the closest thing.
		float dy = float(10000000.0);
		for (int iiginf2 = 0; iiginf2 < (int)viginfNear.size(); iiginf2++)
		{
			Rect rectBB = m_prgginf[viginfNear[iiginf2]].bb();
			if (fabsf(rectBB.top - ysClick) < dy)
			{
				dy = fabsf(rectBB.top - ysClick);
				viginfHit.clear();
				viginfHit.push_back(viginfNear[iiginf2]);
			}
			if (fabsf(rectBB.bottom - ysClick) < dy)
			{
				dy = fabsf(rectBB.bottom - ysClick);
				viginfHit.clear();
				viginfHit.push_back(viginfNear[iiginf2]);
			}
		}
	}

	//	Find all BB for which the click is within their advance width, if any.
	for (iiginf = 0; iiginf < (int)viginfHit.size(); iiginf++)
	{
		//Rect rectBB = m_prgginf[viginfHit[iiginf]].bb(); -- ?? this tests the bounding box again :-/
		//if (rectBB.left <= xsClick && xsClick <= rectBB.right)
		gr::GlyphInfo * pginf = m_prgginf + viginfHit[iiginf];
		if (pginf->advanceWidth() == 0 // advance width is irrelevant
			|| (pginf->origin() < xsClick && xsClick <= pginf->origin() + pginf->advanceWidth()))
		{
			viginfInside.push_back(viginfHit[iiginf]);
		}
	}

	//	If no BB coincides in any way with the click point, take the BB whose leading edge
	//	is closest to the click point.
	//	There could be more than one BB with the same leading x coor.
	if (viginfHit.size() == 0 && viginfNear.size() == 0 && viginfInside.size() == 0)
	{
		float xsLeading;
		if (iginfHit + 1 == m_cginf)
		{
			viginfNear.push_back(iginfHit);
		}
		else if (!m_fWsRtl)
		{
			// Left-to-right:
			xsLeading = m_prgginf[iginfHit + 1].bb().left;
			for (iginf = iginfHit + 1; iginf < m_cginf; iginf++)
			{
				if (m_prgginf[iginf].bb().left == xsLeading)
					viginfNear.push_back(iginf);
			}
		}
		else
		{
			// Right-to-left:
			xsLeading = m_prgginf[iginfHit].bb().right;
			for (iginf = iginfHit; iginf < m_cginf; iginf++)
			{
				if (m_prgginf[iginf].bb().right == xsLeading)
					viginfNear.push_back(iginf);
			}
		}
	}

	Assert(viginfNear.size());

	int iginfLoc;

	//	Select the BB based on its leading edge, offset from baseline, position in final pass.
	if (viginfInside.size())
		iginfLoc = SelectBb(viginfInside, m_fWsRtl);
	else if (viginfHit.size())
		iginfLoc = SelectBb(viginfHit, m_fWsRtl);
	else
		iginfLoc = SelectBb(viginfNear, m_fWsRtl);

	return iginfLoc;
}

/*----------------------------------------------------------------------------------------------
	Select the bounding box we will use for a mouse hit.
	First select any glyph with a significantly smaller bounding box.
	Then, select the BB with the left-most (LTR) or right-most (RTL) edge.
	If more than one BB has same edge of interest, select the one that is closest to the
		the baseline.
	If that is a tie, select the one with the lowest islout.

	@param vigbb - list of indexes into m_prggbb
	@param fRTL - writing system direction
----------------------------------------------------------------------------------------------*/
int Segment::SelectBb(std::vector<int> & viginf, bool fRTL)
{
	if (!viginf.size())
	{
		Assert(false);
		return -1;
	}

	if (viginf.size() == 1)
		return viginf[0];

	// Find a glyph that has a signficantly smaller bounding box.
	// The idea is that it is going to be harder to hit than the bigger glyph.
	float smallestArea = float(1000000000.0);
	float largestArea  = 0.0;
	size_t iiginfSmallest = 0;
	size_t iiginf;
	for (iiginf = 0; iiginf < viginf.size(); iiginf++)
	{
		gr::GlyphInfo * pginf = m_prgginf + viginf[iiginf];
		Rect rectBB = pginf->bb();
		float thisArea = (rectBB.right - rectBB.left) * (rectBB.top - rectBB.bottom);
		if (smallestArea > thisArea)
		{
			smallestArea = thisArea;
			iiginfSmallest = iiginf;
		}
		largestArea = max(largestArea, thisArea);
	}
	if (smallestArea * 2.0 < largestArea)
		return viginf[iiginfSmallest];

	//	Find appropriate x coor of leading edge.
	float xsLeading;
	if (!fRTL)
		xsLeading = m_prgginf[viginf[0]].bb().left;
	else
		xsLeading = m_prgginf[viginf[0]].bb().right;

	for (iiginf = 1; iiginf < viginf.size(); iiginf++)
	{
		Rect rectBB = m_prgginf[viginf[iiginf]].bb();
		if (!fRTL)
			xsLeading = min(xsLeading, rectBB.left);
		else
			xsLeading = max(xsLeading, rectBB.right);
	}

	//	Find BBs with the leading edge.
	std::vector<int> viginfSameEdge;
	for (iiginf = 0; iiginf < viginf.size(); iiginf++)
	{
		Rect rectBB = m_prgginf[viginf[iiginf]].bb();
		if (!fRTL)
		{
			if (rectBB.left == xsLeading)
				viginfSameEdge.push_back(viginf[iiginf]);
		}
		else
		{
			if (rectBB.right == xsLeading)
				viginfSameEdge.push_back(viginf[iiginf]);
		}
	}

	if (viginfSameEdge.size() == 1)
		return viginfSameEdge[0];

	//	Find minimum distance from baseline.
	float ysMin = fabsf(m_prgginf[viginfSameEdge[0]].yOffset());
	for (iiginf = 1; iiginf < viginfSameEdge.size(); iiginf++)
	{
		float ysThis = fabsf(m_prgginf[viginfSameEdge[iiginf]].yOffset());
		ysMin = min(ysMin, ysThis);
	}
	
	// Find BBs with minimum distance from baseline.
	std::vector<int> viginfSameY;
	for (iiginf = 0; iiginf < viginfSameEdge.size(); iiginf++)
	{
		if (fabsf(m_prgginf[viginfSameEdge[iiginf]].yOffset()) == ysMin)
			viginfSameY.push_back(viginfSameEdge[iiginf]);
	}

	if (viginfSameY.size() == 1)
		return viginfSameY[0];

	//	Find minimum index of current candidates.
	int islout = m_prgginf[viginfSameY[0]].logicalIndex();
	for (iiginf = 1; iiginf < viginfSameY.size(); iiginf++)
	{
		int isloutThis = m_prgginf[viginfSameY[iiginf]].logicalIndex();
		islout = min(islout, isloutThis);
	}

	//	Find BB with minimum islout - islout should be unique for all glyphs.
	for (iiginf = 0; iiginf < viginfSameY.size(); iiginf++)
	{
		if (m_prgginf[viginfSameY[iiginf]].logicalIndex() == (unsigned)islout)
			return viginfSameY[iiginf];
	}

	Assert(false);
	return -1; // needed to stop compiler warning
}

/*----------------------------------------------------------------------------------------------
	Return the right-most glyph on the physical surface. If there are ties, it will
	prefer the one that is logically closest to the edge.
----------------------------------------------------------------------------------------------*/
int Segment::RightMostGlyph()
{
	float xsRight = kNegInfFloat;
	int iginfRet = -1;
	for (int iginf = 0; iginf < m_cginf; iginf++)
	{
		float xsThis = m_prgginf[iginf].bb().right;
		// For RTL, we prefer the glyph that is logically first; for LTR we prefer logically last.
		if (xsThis > xsRight || (!m_fWsRtl && xsRight == xsThis))
		{
			xsRight = xsThis;
			iginfRet = iginf;
		}
	}
	return iginfRet;
}

/*----------------------------------------------------------------------------------------------
	Return the left-most glyph on the physical surface. If there are ties, it will
	prefer the one that is logically closest to the edge.
----------------------------------------------------------------------------------------------*/
int Segment::LeftMostGlyph()
{
	float xsLeft = kPosInfFloat;
	int iginfRet = -1;
	for (int iginf = 0; iginf < m_cginf; iginf++)
	{
		float xsThis = m_prgginf[iginf].bb().left;
		// For RTL, we prefer the glyph that is logically last; for LTR we prefer logically first.
		if (xsThis < xsLeft || (m_fWsRtl && xsLeft == xsThis))
		{
			xsLeft = xsThis;
			iginfRet = iginf;
		}
	}
	return iginfRet;
}

/*----------------------------------------------------------------------------------------------
	Convert from em-units to logical units.
----------------------------------------------------------------------------------------------*/
float Segment::EmToLogUnits(int m)
{
	return GrEngine::GrIFIMulDiv(m, m_xysEmSquare, m_mFontEmUnits);
}

/*----------------------------------------------------------------------------------------------
	Return the line-break pseudo glyph ID.
----------------------------------------------------------------------------------------------*/
gid16 Segment::LBGlyphID()
{
	return m_preneng->LBGlyphID();
}

} // namespace gr
