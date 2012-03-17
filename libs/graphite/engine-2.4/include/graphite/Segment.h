/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Segment.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Defines the class for a Graphite text segment.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_SEGMENT_INCLUDED
#define GR_SEGMENT_INCLUDED

#include "Font.h"
#include "GrConstants.h"
#include "SegmentAux.h"

// undo automagic DrawTextA DrawTextW stuff
// #undef DrawText

//:End Ignore

namespace gr
{
class GrEngine;
class SegmentMemoryUsage;

class IGrJustifier;
class GrTableManager;
class GrSlotStream;
class GrSlotOutput;

/*----------------------------------------------------------------------------------------------
	A Graphite segment consists of a sequence of well-positioned glyphs all on one line,
	Each glyph understands its relationship to the underlying text that was used to generate
	the segment.

	Hungarian: seg
----------------------------------------------------------------------------------------------*/
class Segment
{
	friend class GrEngine;
	friend class SegmentPainter;
	friend class GlyphInfo;
	friend class GlyphIterator;
	friend class GlyphSetIterator;
	friend class SegmentMemoryUsage;

public:
	// Static methods

	// Constructors, destructors, etc.
	Segment();
//	Segment(ITextSource * pgts, int ichMin, int ichLim,
//		LineBrk lbStart, LineBrk lbEnd,
//		bool fStartLine, bool fEndLine, bool fWsRtl);

	virtual ~Segment();

	// Basic copy constructor:
	Segment(const Segment & seg);

	// For making modified copies of segments:
	static Segment * LineContextSegment(Segment & seg, bool fStartLine, bool fEndLine);
	static Segment * JustifiedSegment(Segment & seg, float xsNewWidth);
	static Segment * WhiteSpaceSegment(Segment & seg, int newDirDepth);

	static int GetSegmentCount();

public:

	long IncRefCount();
	long DecRefCount();

	Font & getFont();
	ITextSource & getText();
	float advanceWidth();
	Rect boundingRect();

	bool rightToLeft();
	bool paraRightToLeft() { return m_fParaRtl; }
	int directionDepth(bool * pfWeak = NULL);
	bool setDirectionDepth(int nNewDepth);
	int startCharacter();
	int stopCharacter();
	bool startOfLine();
	bool endOfLine();
	bool hasLineBoundaryContext();
	SegEnd segmentTermination();
	LineBrk startBreakWeight();
	LineBrk endBreakWeight();
	float stretch();
	float maxStretch();
	float maxShrink();
	void setTextSourceOffset(int ichMin)
	{
		m_ichwMin = ichMin;
	}

	//GrResult changeLineEnds(bool fNewStart, bool fNewEnd);

	std::pair<GlyphIterator, GlyphIterator> glyphs();
	std::pair<GlyphSetIterator, GlyphSetIterator> charToGlyphs(toffset ich);

	// OBSOLETE:
	GrResult GetCharData(int cchMax, utf16 * prgch, int * pcchRet);
	GrResult GlyphToChar(int iginf, bool fFirst, int * pich); 
	GrResult GlyphToAllChars(int iginf, int cichMax, int * prgich, int *pcichRet);
	GrResult CharToGlyph(int ich, bool fFirst, int * pigbb);

	// Not part of FW interface:

	float getRangeWidth(int ichMin, int ichLim,
		bool fStartLine = true, bool fEndLine = true, bool fSkipSpace = true);
	int findNextBreakPoint(int ichStart,
		LineBrk lbPref, LineBrk lbWorst, float dxMaxWidth,
		float * pdxBreakWidth, bool fStartLine = true, bool fEndLine = true);

	GrResult getUniscribeClusters(
		int * prgigbb1stOfCluster, int cchMax, int * pcch,
		bool * pfClusterStart, int cfMax, int * pcf);

	//GrResult GetGlyphsAndPositions(
	//	Rect rsArg, Rect rdArg,	int cgidMax, int * pcgidRet, utf16 * prggid,
	//	float * prgxd, float * prgyd, float * prgdxdAdv);
	//GrResult GetUniscribePositions(
	//	Rect rs, Rect rd, int cgidMax, int * pcgidRet,
	//	float * prgxd, float * prgyd, float * prgdxdAdv);
	//GrResult GetUniscribeGlyphsAndPositions(
	//	Rect rsArg, Rect rdArg,	int cgidMax, int * pcchRet, utf16 * prgchGlyphs,
	//	float * prgxd, float * prgyd, float * prgdxdAdv);

	// Other public methods:

	void Initialize(ITextSource * pgts, int ichMin, int ichLim,
		LineBrk lbStart, LineBrk lbEnd, SegEnd est,
		bool fStartLine, bool fEndLine, bool fWsRtl);
	void DestroyContents();
	void SwapWith(Segment * pgrseg);
	void ClearAltEndLineSeg();

	ITextSource * GetString(); // client is responsible to Release()

	//  The following three functions handle storing and retrieving information that
	//	allows us to restart a new segment following a line break. This is necessary
	//	in case there is some sort of contextualization across the line break.
	void StoreRestartInfo(int cslotRestartBackup, std::vector<int>& vnSkipOffsets)
	{
		m_cslotRestartBackup = cslotRestartBackup;
		m_vnSkipOffsets.resize(vnSkipOffsets.size());
		for (size_t i = 0; i < vnSkipOffsets.size(); i++)
			m_vnSkipOffsets[i] = sdata8(vnSkipOffsets[i]);
	}
	int NextRestartBackup()
	{
		return m_cslotRestartBackup;
	}
	int RestartBackup()
	{
		//return (m_psegPrev)? m_psegPrev->NextRestartBackup(): 0;
		return 0;
	}
	int SkipOffset(int ipass)
	{
		return int(m_vnSkipOffsets[ipass]);
	}
	DirCode PreviousStrongDir()
	{
		return m_dircPrevStrong;
	}
	void SetPreviousStrongDir(DirCode dirc)
	{
		m_dircPrevStrong = dirc;
	}
	DirCode PreviousTermDir()
	{
		return m_dircPrevTerm;
	}
	void SetPreviousTermDir(DirCode dirc)
	{
		m_dircPrevTerm = dirc;
	}

	int NextSegInitBuffer(byte ** ppbDat)
	{
		*ppbDat = m_prgbNextSegDat;
		return m_cbNextSegDat;
	}
	int ThisSegInitBuffer(byte ** ppbDat)
	{
		*ppbDat = m_prgInitDat;
		return m_cbInitDat;
	}

//	void SetNextSeg(Segment * psegNext)
//	{
//		m_psegNext = psegNext;
//	}

	void SetFaceName(std::wstring /*stu*/, std::wstring /*stuBase*/)
	{
//		m_stuFaceName = stu;
//		m_stuBaseFaceName = stuBase;
//		m_fUseSepBase = (m_stuBaseFaceName.size() > 0);
	}

	void SetEngine(GrEngine * pgreng);
	void SetFont(Font * pfont);
	void SwitchFont(Font * pfont);
	GrEngine * EngineImpl();

	void SetJustifier(IGrJustifier * pgjus);
	IGrJustifier * Justifier()
	{
		return m_pgjus;
	}

	void SetPreContext(int dich)
	{
		m_dichPreContext = dich;
	}

	//void SetToBaseFont(IGrGraphics * pgg);
	//void RestoreFont(IGrGraphics * pgg);

	void SetUpOutputArrays(Font * pfont, GrTableManager * ptm,
		GrSlotStream * psstrmFinal,
		int cchwUnderlying, int csloutSurface, gid16 chwLB,
		TrWsHandling twsh, bool fParaRtl, int nDirDepth,
		bool fEmpty = false);
	void RecordSurfaceAssoc(int ichw, int islot, int nDir);
	void RecordLigature(int ichwUnder, int islotSurface, int iComponent);
	void AdjustForOverlapsWithPrevSeg();
	void MarkSlotInPrevSeg(int ichw, int islot);
	void MarkSlotInNextSeg(int ichw, int islot);

	void CleanUpAssocsVectors()
	{
		for (int iv = 0; iv < m_ichwAssocsLim - m_ichwAssocsMin; iv++)
		{
			std::vector<int> * pvislout = m_prgpvisloutAssocs[iv];
			if (pvislout->size() <= 1)
			{
				delete pvislout;
				*(m_prgpvisloutAssocs + iv) = NULL;
			}
		}
	}

	GrSlotOutput * OutputSlot(int islout);

	void RecordInitializationForNextSeg(int cbNextSegDat, byte * pbNextSegDat)
	{
		m_cbNextSegDat = cbNextSegDat;
		m_prgbNextSegDat = new byte[m_cbNextSegDat];
		std::copy(pbNextSegDat, pbNextSegDat + cbNextSegDat, m_prgbNextSegDat);
	}
	void RecordInitializationForThisSeg(int cbDat, byte * pbDat)
	{
		m_cbInitDat = cbDat;
		if (cbDat == 0)
			m_prgInitDat = NULL;
		else
		{
			m_prgInitDat = new byte[m_cbInitDat];
			std::copy(pbDat, pbDat + cbDat, m_prgInitDat);
		}
	}

	void SetWidths(float dxsVisWidth, float dxsTotalWidth)
	{
		m_dxsVisibleWidth = dxsVisWidth;
		m_dxsTotalWidth = dxsTotalWidth;
	}

	void SetLayout(LayoutEnvironment & layout)
	{
		m_layout.setStartOfLine(layout.startOfLine());
		m_layout.setEndOfLine(layout.endOfLine());
		m_layout.setBestBreak(layout.bestBreak());
		m_layout.setWorstBreak(layout.worstBreak());
		m_layout.setRightToLeft(layout.rightToLeft());
		m_layout.setTrailingWs(layout.trailingWs());
		m_layout.setPrevSegment(layout.prevSegment());
		m_layout.setJustifier(layout.justifier());
		m_layout.setLoggingStream(layout.loggingStream());
		m_layout.setDumbFallback(layout.dumbFallback());
	}
	LayoutEnvironment & Layout()
	{
		return m_layout;
	}

	bool Erroneous()
	{
		return m_fErroneous;
	}
	void SetErroneous(bool f)
	{
		m_fErroneous = f;
	}

	void FixTermination(SegEnd est)
	{
		m_est = est;
	}

	// Convert from em-units to logical units.
	float EmToLogUnits(int m);

	Rect ComponentRect(GrSlotOutput * pslout, int icomp);

	void ClusterMembersForGlyph(int islout, int disloutCluster, std::vector<int> & visloutRet);

	//	for transduction logging:
	// implementations are empty unless TRACING is defined
	void LogUnderlyingToSurface(GrTableManager * ptman, std::ostream & strmOut,
		GrCharStream * pchstrm);
	void LogSurfaceToUnderlying(GrTableManager * ptman, std::ostream & strmOut);

	// calculate memory usage
	void calculateMemoryUsage(SegmentMemoryUsage & smu);

protected:
	//	Member variables:
	long m_cref;		// standard COM ref count

	ITextSource * m_pgts;		// the string we are a segment of
	int m_dichwLim;				// number of characters officially in this segment
	int m_ichwMin;				// first character relative to the text-source

	Font * m_pfont;
	GrEngine * m_preneng;

	bool m_fErroneous;			// an error occurred in generating this segment

	IGrJustifier * m_pgjus;

	LayoutEnvironment m_layout;

	bool m_fWsRtl;				// writing system direction
	bool m_fParaRtl;			// paragraph direction; redundant with layout?

	TrWsHandling m_twsh;		// // redundant with layout?
	int m_nDirDepth;			// segment direction (white-space-only segments can switch)

	byte * m_prgbNextSegDat;	// for initializing the following segment
	int m_cbNextSegDat;			// length of above buffer

	byte * m_prgInitDat;		// what was used to initialize this segment
	int m_cbInitDat;

	int m_dichPreContext;		// what part of this segment affects the previous;
								// not really used for anything right now

//	Segment * m_psegPrev;	// OBSOLETE
//	Segment * m_psegNext;

	// Font information:
//	std::wstring m_stuFaceName;
//	std::wstring m_stuBaseFaceName;
//	bool m_fUseSepBase;

//	float m_pixHeight; // character height of font in pixels (NOT = ascent + descent)
//	bool m_fBold;
//	bool m_fItalic;

	LineBrk m_lbStart;
	LineBrk m_lbEnd;

	bool m_fStartLine;	// redundant with layout?
	bool m_fEndLine;	// redundant with layout?

	SegEnd m_est;

	//Segment * m_psegAltEndLine;	// segment having the alternate m_fEndLine value

	int m_mFontEmUnits;
	float m_dysFontAscent;
	float m_dysFontDescent;
	float m_xysEmSquare;	// em-square of the font in display units
//	float m_xsDPI;			// DPI of device on which segment was measured
//	float m_ysDPI;

	float m_dxsStretch;	// difference between actual and natural width
	float m_dxsWidth;	// width in absence of any stretch. -1 if not computed.
	float m_dysHeight;
	float m_dysAscent;	// distance from common baseline to top of this segment,
						// including extra ascent

	float m_dysXAscent;	// extra for this renderer, beyond what is standard for the font
	float m_dysXDescent;

	float m_dysAscentOverhang;	// extra height above official top of segment; >= 0
	float m_dysDescentOverhang;	// extra height below bottom of segment; <= 0;
	float m_dxsLeftOverhang;		// <= 0
	float m_dxsRightOverhang;

	float m_dxsVisibleWidth;	// not including trailing white space
	float m_dxsTotalWidth;	// including trailing white space

	int m_isloutVisLim;		// lim of visible stuff (to handle trailing ws underlines)

	float m_dysOffset;

	//	Index of the first item in the before/after arrays and the ligature/component arrays,
	//	relative to the start of the segment. Normally == 0, but will be some negative number
	//	if there are slots "borrowed" from the previous segment (officially located in the
	//	previous segment but rendered in this one).
	int m_ichwAssocsMin;

	//	1 + index of last item in the before/after arrays and the ligature/component arrays,
	//	relative to the start of the segment. Normally == m_dichwLim, but will be
	//	larger if there are slots "borrowed" from the following segment.
	int	m_ichwAssocsLim;

	//	Underlying-to-surface associations:
	int * m_prgisloutBefore;			// logical first
	int * m_prgisloutAfter;				// logical last
	std::vector<int> ** m_prgpvisloutAssocs;	// all; used by range selections

	int * m_prgisloutLigature;	// similar to above, index of associated ligature, or kNegInfinity

	sdata8 * m_prgiComponent;	// component of the ligature that the corresponding character
								// represents; only meaningful if corresponding item in
								// m_prgisloutLigature is set

//	GrSlotStream * m_psstrm;	// TODO: rework to remove this
	int m_cslout;
	GrSlotOutput * m_prgslout;	// final output slots
	u_intslot * m_prgnSlotVarLenBuf;	// var-length buffer for slots--one big buffer managed by the
								// segment, but pointed to by the slots

//	int m_cnUserDefn;	// this information is not stored in the segment itself
	int m_cnCompPerLig;
//	int m_cnFeat;		// this information is not stored in the segment itself

	int m_cginf;
	int m_isloutGinf0;	// islout index for m_prgginf[0];
	GlyphInfo * m_prgginf;

	// Data structures to hold glyphs for rendering:
////#include "GrSegmentPlatform.h"

	//	For initializing the following segment:
	int m_cslotRestartBackup;	// the number of slots in the underlying
								// input that need to be reprocessed when starting
								// the next segment

	std::vector<sdata8> m_vnSkipOffsets;		// skip offset for each pass when restarting

	DirCode m_dircPrevStrong;		// previous strong directionality code
	DirCode m_dircPrevTerm;			// previous terminator code, if any


	//	Other protected methods:

	void InitLineContextSegment(bool fStartLine, bool fEndLine);
	void InitWhiteSpaceSegment(int nNewDepth);

	void ComputeDimensions();

	void SetUpGlyphInfo(GrTableManager * ptman, GrSlotStream * psstrmFinal,
		gid16 chwLB, int nDirDepth, int islotMin, int cslot);
	void SetUpLigInfo(GrTableManager * ptman, GlyphInfo & ginf, GrSlotOutput * pslout);

	void InitBeforeArrayFromPrevSeg();	// obsolete
	void AdjustBeforeArrayFromPrevSeg();
	void AdjustAfterArrayFromNextSeg();
	void EnsureSpaceAtLineBoundaries(int ichwUnder);

	int LogicalSurfaceToUnderlying(int islout,
		float xsOffset, float ysOffset,
		float dxsGlyphWidth = 0, float dysGlyphHeight = 0, bool * pfAfter = NULL);
	int LogicalSurfaceToUnderlying(int islout, bool fBefore);
	int PhysicalSurfaceToUnderlying(int iginf,
		float xsOffset, float xyOffset,
		float dxsGlyphWidth = 0, float dysGlyphHeight = 0, bool * pfAfter = NULL);
	int PhysicalSurfaceToUnderlying(int iginf, bool fBefore);
	int UnderlyingToLogicalSurface(int ichw, bool fBefore);
	int UnderlyingToPhysicalSurface(int ichw, bool fBefore);
	int LogicalToPhysicalSurface(int islout);
	void UnderlyingToPhysicalAssocs(int ichw, std::vector<int> & viginf);
	std::vector<int> UnderlyingToLogicalAssocs(int ichw);
	int UnderlyingToLogicalInThisSeg(int ichw);
	bool SameSurfaceGlyphs(int ichw1, int ichw2);

	void ShiftGlyphs(float dxsShift);

	float GlyphLeftEdge(int iginf);

	int DirLevelOfChar(int ichw, bool fBefore);
	bool CharIsRightToLeft(int ichw, bool fBefore);
	bool SlotIsRightToLeft(GrSlotOutput * pslout);
	int TopDirLevel()
	{
		if (m_fWsRtl)
			return 1;
		else
			return 0;
	}

	void AssertValidClusters(GrSlotStream * psstrm);

	void MergeUniscribeCluster(
		std::vector<int> & visloutBefore, std::vector<int> & visloutAfter,
		int ich1, int ich2);

	int GlyphHit(float xsClick, float ysClick);
	int SelectBb(std::vector<int> & viginf, bool fRTL);

	int RightMostGlyph();
	int LeftMostGlyph();

	void ComputeOverhangs(float * pdysVisAscent, float * pdysNegVisDescent,
		float * pdxsVisLeft, float * pdxsVisRight);

public:
	LineBrk getBreakWeight(int ich, bool fBreakBefore = false);

	// Platform-specific routines:
protected:
	//void ShiftGlyphsPlatform(float dxsShift);
	//bool SetDirectionDepthPlatform(int nNewDepth);

	//void InitializePlatform();
	//void DestroyContentsPlatform();
	//GrResult GetGlyphsAndPositionsPlatform(float xs, float ys,
	//	Rect rsArg, Rect rdArg,	
	//	utf16 * prgchGlyphs, float * prgxd, float * prgyd, float * prgdxdAdv);
	//void SetUpOutputArraysPlatform(GrTableManager * ptman, gid16 chwLB, int nDirDepth,
	//	int islotMin, int cslot);

	// Temporary - for WinSegmentPainter
public:
	int CSlout()
	{
		return m_cslout;
	}
	gid16 LBGlyphID();
	float AscentOffset()
	{
		if (m_dysAscent == -1)
			ComputeDimensions();
		return m_dysAscent - m_dysFontAscent;
	}
};


//:>********************************************************************************************
//:>	The purpose of these subclasses is simply to provide meaningful names for the
//:>	constructors. Instances of these classes are in every way equivalent to Segment.
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Creating a LineFillSegment fits as much of the text on the line as possible,
	finding a reasonable break point if necessary.
----------------------------------------------------------------------------------------------*/
class LineFillSegment : public Segment
{
public:
	LineFillSegment(
		Font * pfont,
		ITextSource * pts,
		LayoutEnvironment * playout,
		toffset ichStart,
		toffset ichStop,
		float maxWidth,
		bool fBacktracking = false);
protected:
	// Default constructor:
	LineFillSegment() : Segment()
	{
	}
};

/*----------------------------------------------------------------------------------------------
	Creating a RangeSegment makes a segment representing the given range of characters, or
	the entire text-source.
----------------------------------------------------------------------------------------------*/
class RangeSegment : public Segment
{
public:
	RangeSegment(
		Font * pfont,
		ITextSource * pts,
		LayoutEnvironment * playout,
		toffset ichStart = 0,
		toffset ichStop = kPosInfinity,
		Segment * psegInitLike = NULL);
};

/*----------------------------------------------------------------------------------------------
	The JustSegmentAux class is used by the factory method that creates a justified segment.
	It is called JustSegmentAux to avoid conflicting with the factory method.
----------------------------------------------------------------------------------------------*/
class JustSegmentAux : public Segment
{
public:
	JustSegmentAux(
		Font * pfont,
		ITextSource * pts,
		LayoutEnvironment * playout,
		toffset ichStart,
		toffset ichStop,
		float xsNaturalWidth,
		float xsJustifiedWidth,
		Segment * psegInitLike = NULL);
};

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif  // !GR_SEGMENT_INCLUDED
