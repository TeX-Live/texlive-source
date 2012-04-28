/*--------------------------------------------------------------------
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrEngine.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Contains the definition of the GrEngine class.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_ENGINE_INCLUDED
#define GR_ENGINE_INCLUDED
#include <limits>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

namespace gr 
{
class IGrJustifier;
class Segment;


/*----------------------------------------------------------------------------------------------
	The GrEngine serves as the top level object that knows how to run Graphite tables
	and generate Graphite segments.

	Primarily, this class implements IRenderEngine, which allows it to serve as a FW
	rendering engine. It also implements ISimpleInit, a general interface for initializing
	using a string. Finally, it implements ITraceControl, a very simple interface which
	allows a client to flip a flag indicating whether or not we want to output a log of
	the Graphite transduction process.

	Hungarian: greng
----------------------------------------------------------------------------------------------*/
class GrEngine : public GraphiteProcess
{
	friend class FontFace;
	friend class FontMemoryUsage;

public:
	// Constructors & destructors:
	GrEngine();
	void BasicInit();
	virtual ~GrEngine();
	void DestroyContents(bool fDestroyCmap = true);

	// ISimpleInit methods:
	////GrResult InitNew(const byte * prgb, int cb); -- obsolete
	////GrResult get_InitializationData(int cchMax, OLECHAR * rgchw, int * pcch); -- obsolete

	// ITraceControl methods:
	//GrResult SetTracing(int n);
	//GrResult GetTracing(int * pnOptions);

	// IRenderingFeatures methods:
	//GrResult GetFeatureIDs(int cMax, int * prgFids, int * pcfid);
	//GrResult GetFeatureLabel(int fid, int nLanguage, int cchMax, OLECHAR * rgchw, int * pcch);
	//GrResult GetFeatureValues(int fid, int cfvalMax,
	//		int * prgfval, int * pcfval, int * pfvalDefault);
	//GrResult GetFeatureValueLabel(int fid, int fval, int nLanguage,
	//	int cchMax, OLECHAR * rgchw, int * pcch);

	// IRenderEngine methods:
	////GrResult InitRenderer(IGrGraphics * pgg, OLECHAR * prgchData, int cchData); -- obsolete
	////GrResult FontIsValid(OLECHAR * prgchwErrMsg, int cchMaxErrMsg);

	GrResult get_SegDatMaxLength(int * pcb);

	//GrResult FindBreakPoint(Font * pfont, ITextSource * pgts,	DELETE
	//	int ichMin, int ichLim, int ichLimBacktrack,
	//	bool fNeedFinalBreak,
	//	bool fStartLine,
	//	float dxMaxWidth,
	//	LineBrk lbPref, LineBrk lbMax,
	//	TrWsHandling twsh, bool fParaRtoL,
	//	Segment ** ppsegRet,
	//	int * pdichwLimSeg, float * pdxWidth, SegEnd * pest,
	//	int cbPrev, byte * pbPrevSegDat,
	//	int cbNextMax, byte * pbNextSegDat, int * pcbNextSegDat,
	//	int * pdichwContext,
	//	std::ostream * pstrmLog,
	//	OLECHAR * prgchwErrMsg, int cchMaxErrMsg);

	//GrResult FindBreakPointWJust(Font * pfont, ITextSource * pgts, IGrJustifier * pgjus,
	//	int ichMin, int ichLim, int ichLimBacktrack,   DELETE
	//	bool fNeedFinalBreak,
	//	bool fStartLine,
	//	float dxMaxWidth,
	//	LineBrk lbPref, LineBrk lbMax,
	//	TrWsHandling twsh, bool fParaRtoL,
	//	Segment ** ppsegRet,
	//	int * pdichwLimSeg, float * pdxWidth, SegEnd * pest,
	//	int cbPrev, byte * pbPrevSegDat,
	//	int cbNextMax, byte * pbNextSegDat, int * pcbNextSegDat,
	//	int * pdichwContext,
	//	std::ostream  * pstrmLog,
	//	OLECHAR * prgchwErrMsg, int cchMaxErrMsg);

	GrResult get_ScriptDirection(unsigned int * pgrfsdc, OLECHAR * prgchwErrMsg, int cchMaxErrMsg);

	// DELETE
	//GrResult MakeJustifiedSegment(Font * pfont, ITextSource * pgts, IGrJustifier * pgj,
	//	int ichMin, int ichLim,
	//	bool fStartLine, bool fEndLine,
	//	float dxUnjustifiedWidth, float dxJustifiedWidth,
	//	TrWsHandling twsh, SegEnd est, LineBrk lb,
	//	bool fParaRtl,
	//	Segment ** ppsegRet, float * pdxWidth,
	//	//int cbPrev, byte * pbPrevSegDat, int cbNextMax, byte * pbNextSegDat, int * pcbNextSegDat, int * pdichwContext,
	//	std::ostream * pstrmLog,
	//	OLECHAR * prgchwErrMsg, int cchwMaxErrMsg);

	// IJustifyingRenderer methods:
	// These methods return kresInvalidArg if the attribute ID is invalid or inappropriate;
	// kresFail if the engine is not in an appropriate state to return the information.
	virtual GrResult getGlyphAttribute(int iGlyph, int jgat, int nLevel, float * pValueRet);
	virtual GrResult getGlyphAttribute(int iGlyph, int jgat, int nLevel, int * pValueRet);
	virtual GrResult setGlyphAttribute(int iGlyph, int jgat, int nLevel, float value);
	virtual GrResult setGlyphAttribute(int iGlyph, int jgat, int nLevel, int value);

	// Interface methods not used by FW:

	////GrResult IsGraphiteFont(IGrGraphics * pgg); -- obsolete

	////GrResult FontAscentAndDescent(IGrGraphics * pgg, int * pysAscent, int * pysDescent); -- obsolete

	//GrResult MakeMeasuredSegment(
	//	Font * pfont, ITextSource * pgts,
	//	int ichMin, int ichLim,
	//	bool fParaRtl,
	//	Segment ** ppsegRet, SegEnd * pest,
	//	std::ostream * pstrmLog,
	//	OLECHAR * prgchwErrMsg, int cchMaxErrMsg);

	//GrResult MakeSegmentFromRange(Font * pfont, ITextSource * pgts, IGrJustifier * pgj,
	//	int ichMin, int ichLim,
	//	bool fStartLine, bool fEndLine,
	//	TrWsHandling twsh, bool fParaRtl,
	//	Segment ** ppsegRet, float * pdxWidth, SegEnd * pest,
	//	int cbPrev, byte * pbPrevSegDat, int cbNextMax, byte * pbNextSegDat, int * pcbNextSegDat,
	//	int * pdichwContext,
	//	std::ostream * pstrmLog,
	//	OLECHAR * prgchwErrMsg, int cchwMaxErrMsg);

	// NEW interface

	GrResult ReadFontTables(Font * pfont, bool fItalic);

	FontErrorCode IsValidForGraphite(int * pnVersion, int * pnSubVersion);

	// Other public methods

	GrResult FindBreakPointAux(Font * pfont, ITextSource * pgts, IGrJustifier * pgjus,
		int ichwMin, int ichwLim, int ichwLimBacktrack,
		bool fNeedFinalBreakArg, bool fStartLine, bool fEndLine,
		float dxMaxWidth, bool fWidthIsCharCount,
		LineBrk lbPref, LineBrk lbMax, TrWsHandling twsh, bool fParaRtl,
		Segment ** ppsegRet,
		int * pdichwLimSeg,
		float * pdxWidth, SegEnd * pest,
		int cbPrev, byte * pbPrevSegDat,
		int cbNextMax, byte * pbNextSegDat, int * pcbNextSegDat,
		int * pdichwContext,
		std::ostream * pstrmLog,
		OLECHAR * prgchwErrMsg, int cchMaxErrMsg);

	gid16 GetGlyphIDFromUnicode(int nUnicode);
	gid16 ActualGlyphForOutput(utf16 chwGlyphID);

	int GetFontEmUnits() 
	{
		return m_mFontEmUnits;
	}

	gid16 LBGlyphID()
	{
		return m_chwLBGlyphID;
	}

	gid16 GetClassGlyphIDAt(int nClass, int nIndex);
	int GetIndexInGlyphClass(int nClass, gid16 chwGlyphID);
	size_t NumberOfGlyphsInClass(int nClass);
	int GlyphAttrValue(gid16 chwGlyphID, int nAttrID);
	int ComponentIndexForGlyph(gid16 chwGlyphID, int nCompID);

	////void InitFontName(std::wstring stuInitialize, std::wstring & stuFaceName, std::wstring & stuFeatures); -- obsolete
	void RecordFontLoadError(OLECHAR * prgchwErrMsg, int cchMaxErrMsg);
	void RecordFontRunError(OLECHAR * prgchwErrMsg, int cchMaxErrMsg, GrResult res);
	void ClearFontError(OLECHAR * prgchwErrMsg, int cchMaxErrMsg);
	void GetWritingSystemDirection(ITextSource * pgts, int ichwMin);
	////GrResult SetUp(IGrGraphics * pgg, std::wstring stuFeatures); -- obsolete
	////GrResult SetUp(IGrGraphics * pgg, std::wstring stuFeaturesArg, bool fBold, bool fItalic); -- obsolete

	GrResult RunUsingEmpty(Segment * psegNew, Font * pfont,
		GrCharStream * pchstrm, LayoutEnvironment & layout,
		int ichStop,
		float dxMaxWidth,
		bool fNeedFinalBreak, bool fMoreText, bool fInfiniteWidth,
		int ichwCallerBtLim,
		int nDirDepth, SegEnd estJ);
	bool InErrorState()
	{
		 return m_fInErrorState;
	}

	std::wstring FaceName()
	{
		return m_stuFaceName;
	}
	std::wstring BaseFaceName()
	{
		Assert(m_stuBaseFaceName.size() == 0 || m_fUseSepBase);
		return m_stuBaseFaceName; // empty if not using separate base font
	}
	bool Bold()
	{
		return m_fBold;
	}
	bool Italic()
	{
		return m_fItalic;
	}

	void GetStyles(Font * pfont, int ichwMin, bool * pfBold, bool * pfItalic);
	void SwitchGraphicsFont(bool fBase);

	void AddFeature(featid nID, int nNameId, int cfset, int nDefault = 0);
	GrFeature * FeatureWithID(featid nID, int * pifeat);
	int DefaultForFeatureAt(int ifeat);
	void SetDefaultForFeatureAt(int ifeat, int nValue);
	GrFeature * Feature(int ifeat)
	{
		if (ifeat >= kMaxFeatures)
			return NULL;
		return m_rgfeat + ifeat;
	}
	void DefaultsForLanguage(isocode lgcode,
		std::vector<featid> & vnFeats, std::vector<int> & vnValues);
	std::wstring StringFromNameTable(int nNameId, int nLang);

	void InitSlot(GrSlotState *, int nUnicode = -1);
	void SetSlotAttrsFromGlyphAttrs(GrSlotState *);

	bool RightToLeft()
	{
		return m_fRightToLeft;
	}
	int TopDirectionLevel()
	{
		return (m_fRightToLeft) ? 1 : 0;
	}

	GrGlyphTable * GlyphTable()
	{
		return m_pgtbl;
	}

	GrClassTable * ClassTable()
	{
		return m_pctbl;
	}

	int NumFeat()
	{
		return m_cfeat;
	}

	bool LineBreakFlag()
	{
		return m_fLineBreak;
	}

	int PreXlbContext()
	{
		return m_cchwPreXlbContext;
	}

	int PostXlbContext()
	{
		return m_cchwPostXlbContext;
	}

	int ExtraAscent()
	{
		return m_mXAscent;
	}

	int ExtraDescent()
	{
		return m_mXDescent;
	}

	float VerticalOffset()
	{
		return m_dysOffset;
	}

	int NumUserDefn()
	{
		return m_cnUserDefn;
	}

	int FirstCompAttr()
	{
		return m_nCompAttr1;
	}

	int NumCompPerLig()
	{
		return m_cnCompPerLig;
	}

	bool BasicJustification()
	{
		return m_fBasicJust;
	}

	bool LoggingTransduction();
	//{
	//	return m_pfface->getLogging();
	//	//return m_fLogXductn;
	//}

	bool FakeItalic()
	{
		return m_fFakeItalic;
	}

	void NewSegment(Segment ** ppseg);

	static int ReadVersion(GrIStream & grstrm);

	/*----------------------------------------------------------------------------------------------
	Function to  convert between coordinate systems.
	This is identical to the built-in MulDiv function except that it rounds with better
	precision.

	TODO: move these out of GrEngine to someplace more general.
	----------------------------------------------------------------------------------------------*/
	inline static int GrIntMulDiv(const int v, const int n, const int d)
	{
		return int(n < 0 ?
			(double(v * n)/double(d)) - 0.5 :
			(double(v * n)/double(d)) + 0.5);
	}
	inline static float GrFloatMulDiv(const float v, const float n, const float d)
	{
		return v * n / d;
	}
	inline static float GrIFIMulDiv(const int v, const float n, const int d)
	{
		return float(double(v) * n / double(d));
	}
	inline static int GrFIFMulDiv(const float v, const int n, const float d)
	{
		return int(n < 0 ?
			(v * double(n) / double(d)) - 0.5 :
			(v * double(n) / double(d)) + 0.5);
	}
	inline static int RoundFloat(const float n)
	{
		return int(n < 0 ? n - 0.5 : n + 0.5);
	}

protected:
	//	Member variables:

	long m_cref;

	//////////// Font information ////////////

	bool m_fBold;
	bool m_fItalic;

	//	do we have the capacity to perform smart rendering for the various styles?
	bool m_fSmartReg;
	bool m_fSmartBold;
	bool m_fSmartItalic;
	bool m_fSmartBI;
	//	are the italics faked using a slant?
	bool m_fFakeItalicCache;
	bool m_fFakeBICache;
	//	control files for styled text; empty means not yet determined
	std::wstring m_strCtrlFileReg;	// regular
	std::wstring m_strCtrlFileBold;	// bold
	std::wstring m_strCtrlFileItalic;	// italic
	std::wstring m_strCtrlFileBI;		// bold-italic

	bool m_fFakeItalic;			// true if we are simulating an italic slant in positioning

	std::wstring m_stuCtrlFile;		// control file (.ttf file) from which we got
									// currently loaded files

	std::wstring m_stuInitError;	// description of any error that happened while initializing
									// the engine
	std::wstring m_stuErrCtrlFile;	// file for which the error occurred

	std::wstring m_stuFaceName;		// eg, "Times New Roman"
	std::wstring m_stuFeatures;

	bool m_fUseSepBase;
	std::wstring m_stuBaseFaceName;	// from Sile table; empty if we are not using a separate
									// control file

	GrResult m_resFontRead;		// kresOk if the Graphite font was loaded successfully;
								// kresFail if the font could not be found or basic tables couldn't be read;
								// kresFalse if the Silf table is not present;
								// kresUnexpected if the Graphite tables could not be loaded

	GrResult m_resFontValid;	// kresInvalidArg if the engine is not yet initialized
								// Originally:
								//	kresOk if Graphite font & dc font match cmaps
								//	kresUnexpected/kresFalse if dumb rendering
								//	kresFail if dumb rendering with no cmap
								// But now it is apparently just = to m_resFontRead otherwise.
	FontErrorCode m_ferr;
	int m_fxdBadVersion;

	int m_nFontCheckSum;	// when loading a font using the GrGraphics, remember the
							// check sum as a unique identifer

	int m_nScriptTag;		// from the pertinent writing system; currently not used
	unsigned int m_grfsdc;	// supported script directions

	bool m_fRightToLeft;	// overall writing-system direction

	int m_mXAscent;			// extra ascent, in font's em-units
	int m_mXDescent;		// extra descent, in font's em_units

	float m_dysOffset;		// vertical offset, in logical units (eg, for super/subscript)

	bool m_fBasicJust;		// true if there are no complex justification tables/rules in the
							// font, so we use basic whitespace justification
	int m_cJLevels;			// number of justification levels

	GrTableManager * m_ptman;
	GrClassTable * m_pctbl;
	GrGlyphTable * m_pgtbl;

	FontFace * m_pfface;

	GrFeature m_rgfeat[kMaxFeatures];
//	HashMap<int, int> m_hmnifeat;	// maps from feature IDs to indices in the array
//									// Review: do we need this, or is a linear search adequate?
	int	m_cfeat;					// number of features present

	short * m_rglcidFeatLabelLangs;	// LCIDs (language IDs) for all the labels in the feature table
	size_t m_clcidFeatLabelLangs;

	GrLangTable m_langtbl;

	//	are line-breaks relevant at all?
	bool m_fLineBreak;
	//	ranges for possible cross-line-boundary contextualization
	int	m_cchwPreXlbContext;
	int m_cchwPostXlbContext;

	//	magic glyph attribute numbers
	data16 m_chwPseudoAttr;	// actual-for-pseudo fake glyph attribute
	data16 m_chwBWAttr;		// break-weight
	data16 m_chwDirAttr;	// directionality
	data16 m_chwJStretch0;	// justify.0.stretch
	data16 m_chwJShrink0;	// justify.0.shrink
	data16 m_chwJStep0;		// justify.0.step
	data16 m_chwJWeight0;	// justify.0.weight

	gid16 m_chwLBGlyphID;	// magic line-break glyph ID

	int m_cComponents;		// number of glyph attributes at the beginning of the glyph table
							// that actually represent ligature components
	int m_nCompAttr1;		// first lig component attribute

	int m_cnUserDefn;		// number of user-defined slot attributes
	int m_cnCompPerLig;		// max number of components needed per ligature

	int m_mFontEmUnits;		// number of design units in the Em square for the font

	//	for pseudo-glyph mappings
	int m_cpsd;				// number of psuedo-glyphs
	GrPseudoMap * m_prgpsd;
	int m_dipsdInit;		// (max power of 2 <= m_cpsd);
							//		size of initial range to consider
	int m_cPsdLoop;			// log2(max power of 2 <= m_cpsd);
							//		indicates how many iterations are necessary
	int m_ipsdStart;		// m_cpsd - m_dipsdInit;
							//		where to start search

	// for Unicode to glyph ID mapping
	////TableBuffer	m_tbufCmap;	// hold the full cmap table
	void * m_pCmap_3_1;		// point to the MS Unicode cmap subtable
							// uses platform 3 writing system 1 or 0, preferably 1
							//		use for Unicode to Glyph ID conversion
	void * m_pCmap_3_10;
	byte * m_pCmapTbl;
	bool m_fCmapTblCopy;
	int m_cbCmapTbl;		// needed only for memory instrumentation

	// for feature names and maybe other strings from font later
	////TableBuffer	m_tbufNameTbl;	// hold full name table; use Name() method to access
	byte * m_pNameTbl;
	bool m_fNameTblCopy;
	int m_cbNameTbl;		// needed only for memory instrumentation and sanity checks

	bool m_fLogXductn;		// true if we want to log the transduction process

	bool m_fInErrorState;	// true if we are handling a rendering error

	//	Other protected methods:

	enum {
		ksegmodeBreak = 0,	// FindBreakPoint
		ksegmodeJust,		// MakeJustifiedSegment
		//ksegmodeMms,		// MakeMeasuredSegment
		ksegmodeRange		// MakeSegmentFromRange
	};

	void MakeSegment(
		// Common parameters
		Segment * psegRet,
		Font * pfont, ITextSource * pgts, IGrJustifier * pjus,
		LayoutEnvironment & layout,
		int ichMin, int ichLim,
		// for finding a line break
		float dxMaxWidth,
		bool fBacktracking,
		// for making a justified segment
		bool fJust, float dxJustifiedWidthJ, SegEnd estJ);

	int FindFontLim(ITextSource * pgts, int ichFontMin, int * pnDirDepth);

#ifdef GR_FW
	////GrResult InitFromControlFile(IGrGraphics * pgg, std::wstring stuFaceName,
	////	bool fBold, bool fItalic); -- obsolete
	////GrResult ReadFromControlFile(std::wstring stuFontFile); -- obsolete
//	GrResult GetFallBackFont(std::wstring * pstu);
#endif

	////GrResult ReadFromGraphicsFont(IGrGraphics * pgg, bool fItalic); -- obsolete
	void DestroyEverything();

	bool CheckTableVersions(GrIStream * pgrstrm,
		const byte *silf_tbl, int lSilfStart,
		const byte *gloc_tbl, int lGlocStart,
		const byte *feat_tbl, int lFeatStart,
		int * pfxdBadVersion);
	//bool ReadSileTable(GrIStream & grstrm, long lTableSTart,
	//	int * pmFontEmUnits, bool * pfMismatchedBase);
	bool ReadSilfTable(GrIStream & grstrm, long lTableStart, int iSubTable,
		long cbTableLen, int * pchwMaxGlyphID, int * pfxdVersion);
	bool ReadGlocAndGlatTables(GrIStream & grstrm, long lGlocStart, GrIStream & glat_strm, long lGlatStart,
		int chwMaxGlyphID, int fxdSilfVersion);
	bool ReadFeatTable(GrIStream & grstrm, long lTableStart);
	bool ReadSillTable(GrIStream & grstrm, long lTableStart);
	bool SetCmapAndNameTables(Font * pfont);

	void CreateEmpty();

	bool BadFont(FontErrorCode * pferr = NULL)
	{
		if (pferr)
			*pferr = m_ferr;
		return (m_resFontValid == kresFail || m_resFontRead == kresFail);
	}
	bool DumbFallback(FontErrorCode * pferr = NULL)
	{
		if (pferr)
			*pferr = m_ferr;
		return (m_resFontValid != kresOk || m_resFontRead != kresOk);
	}

	gid16 MapToPseudo(int nUnicode);

	// NEW interface
	void AssignDefaultFeatures(int cfeat, FeatureSetting * prgfset);

	// Feature access for FontFace:
	size_t NumberOfFeatures_ff();
	featid FeatureID_ff(size_t ifeat);
	size_t FeatureWithID_ff(featid id);
	bool GetFeatureLabel_ff(size_t ifeat, lgid language, utf16 * label);
	int GetFeatureDefault_ff(size_t ifeat);
	size_t NumberOfSettings_ff(size_t ifeat);
	int GetFeatureSettingValue_ff(size_t ifeat, size_t ifset);
	bool GetFeatureSettingLabel_ff(size_t ifeat, size_t ifset, lgid language, utf16 * label);
	// Feature-label languages:
	size_t NumberOfFeatLangs_ff();
	short GetFeatLabelLang_ff(size_t ilang);
	// Language access for FontFace:
	size_t NumberOfLanguages_ff();
	isocode GetLanguageCode_ff(size_t ilang);

	void SetUpFeatLangList();

};

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif


#endif  // !GR_ENGINE_INCLUDED
