/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrEngine.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description: Contains the implementation of the GrEngine class.
----------------------------------------------------------------------------------------------*/

//:>********************************************************************************************
//:>	   Include files
//:>********************************************************************************************
#include "Main.h"
#include <functional>
#include <cstring>
#ifdef _MSC_VER
#pragma hdrstop
#endif
// any other headers (not precompiled)

//:End Ignore

namespace gr
{

//:>********************************************************************************************
//:>	   Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	   Local Constants and static variables
//:>********************************************************************************************


//:>********************************************************************************************
//:>	   General functions
//:>********************************************************************************************

// Stand-alone function:
void EngineVersion(int * nMajor, int * nMinor, int * nBugFix)
{
	*nMajor = GRAPHITE_VERSION_MAJOR;
	*nMinor = GRAPHITE_VERSION_MINOR;
	*nBugFix = GRAPHITE_VERSION_BUGFIX;
}

bool compareCmap(const byte *lhs, const byte *rhs)
{
    using namespace gr;
    
    typedef const struct index {data16 version; data16 numberSubtables;}   *const IndexPtr;
    typedef const struct enc {data16 platID; data16 platSpecID; data32 offset;} *const EncPtr;

    // Calculate the size of the cmap.

	IndexPtr rhsIndex = IndexPtr(rhs);
	size_t numSubTbl = lsbf(rhsIndex->numberSubtables);
    size_t cbCmapSz = sizeof(index) + sizeof(enc) * numSubTbl;
    EncPtr encTbl = EncPtr(rhs + sizeof(index));

    for (size_t iSubTbl = 0; iSubTbl < numSubTbl; iSubTbl++)
    //for (int iSubTbl = numSubTbl - 1; iSubTbl >= 0; --iSubTbl)
    {
        const data16 *const mapTbl = reinterpret_cast<const data16 *>(rhs + lsbf((int)encTbl[iSubTbl].offset));
		int format = lsbf(*mapTbl);
        switch (format)
        {
            // 16-bit lengths
            case 0:
			case 2:
			case 4:
			case 6:
                cbCmapSz += lsbf((data16)unsigned(mapTbl[1]));
                break;
            // 32-bit lengths
            case 8:
			case 10:
			case 12:
                cbCmapSz += lsbf((int)unsigned(reinterpret_cast<const data32 *>(mapTbl)[1]));
                break;
			default:
				Assert(false);
        }
    }

    // Do the comparison.
    return (memcmp(lhs, rhs, cbCmapSz) ? false : true);
}


//:>********************************************************************************************
//:>	   GrEngine class.
//:>********************************************************************************************

GrEngine::GrEngine()
{
	/////////////_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF); ////////////////


	m_cref = 1;
	BasicInit();
}

void GrEngine::BasicInit()
{
	m_pfface = NULL;

	m_ptman = NULL;
	m_pctbl = NULL;
	m_pgtbl = NULL;
	m_prgpsd = NULL;
	m_cpsd = 0;
	m_cfeat = 0;
	m_mFontEmUnits = -1;
	m_dysOffset = 0;

	////m_tbufCmap.reset();
	m_pCmap_3_1 = NULL;
	m_pCmap_3_10 = NULL;
	////m_tbufNameTbl.reset();
	// new interface stuff:
	m_pCmapTbl = NULL;
	m_pNameTbl = NULL;
	m_fCmapTblCopy = false;
	m_fNameTblCopy = false;

	m_fLogXductn = false;

	m_resFontValid = kresInvalidArg; // not yet initialized
	m_ferr = kferrUninitialized;
	m_fxdBadVersion = 0;

	m_fSmartReg = false;
	m_fSmartBold = false;
	m_fSmartItalic = false;
	m_fSmartBI = false;
	m_fFakeItalicCache = false;
	m_fFakeBICache = false;
	m_strCtrlFileReg.erase();
	m_strCtrlFileBold.erase();
	m_strCtrlFileItalic.erase();
	m_strCtrlFileBI.erase();
	m_fFakeItalic = false;
	m_stuCtrlFile.erase();
	m_stuInitError.erase();
	m_stuErrCtrlFile.erase();
	m_stuFaceName.erase();
	m_stuBaseFaceName.erase();
	m_fUseSepBase = false;
	m_stuFeatures.erase();

	m_nFontCheckSum = 0;

	m_fInErrorState = false;

	m_rglcidFeatLabelLangs = NULL;	// initialize lazily, when needed
	m_clcidFeatLabelLangs = 0;
}

GrEngine::~GrEngine()
{
	DestroyEverything();
	#ifdef _MSC_VER
	if (!_CrtCheckMemory())
	{
		OutputDebugString(L"bad memory");
	}
	#endif
}

/*----------------------------------------------------------------------------------------------
	Clean everything out (destructor or reinitializing with a different face name).
----------------------------------------------------------------------------------------------*/
void GrEngine::DestroyEverything()
{
	DestroyContents();
	#ifdef _MSC_VER
	if (!_CrtCheckMemory())
	{
		OutputDebugString(L"bad memory");
	}
	#endif

	m_strCtrlFileReg.erase();
	m_strCtrlFileBold.erase();
	m_strCtrlFileItalic.erase();
	m_strCtrlFileBI.erase();
	m_fSmartReg = false;
	m_fSmartBold = false;
	m_fSmartItalic = false;
	m_fSmartBI = false;
	m_fFakeItalicCache = false;
	m_fFakeBICache = false;
	m_stuBaseFaceName.erase();

	//m_stuFaceName.erase();
	//m_stuFaceName = L"this is not a usable or valid string";
	m_fUseSepBase = false;
	m_stuFeatures.erase();
}

void GrEngine::DestroyContents(bool fDestroyCmap)
{
	if (fDestroyCmap)
	{
		////m_tbufCmap.reset();
		m_pCmap_3_1 = NULL;
		m_pCmap_3_10 = NULL;

		if (m_fCmapTblCopy)
			delete[] m_pCmapTbl;
		if (m_fNameTblCopy)
			delete[] m_pNameTbl;
		m_pCmapTbl = NULL;
		m_pNameTbl = NULL;
		m_fCmapTblCopy = false;
		m_fNameTblCopy = false;
	}

	delete m_ptman;
	delete m_pctbl;
	delete m_pgtbl;
	delete[] m_prgpsd;
	m_ptman = NULL;
	m_pctbl = NULL;
	m_pgtbl = NULL;
	m_prgpsd = NULL;

	////m_tbufNameTbl.reset();

	m_stuCtrlFile.erase();
	m_stuInitError.erase();

	m_resFontValid = kresInvalidArg;
	m_ferr = kferrUninitialized;
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the number of features.
----------------------------------------------------------------------------------------------*/
size_t GrEngine::NumberOfFeatures_ff()
{
	return (size_t)m_cfeat;
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the feature ID for the ifeat-th feature.
----------------------------------------------------------------------------------------------*/
featid GrEngine::FeatureID_ff(size_t ifeat)
{
	return (featid)m_rgfeat[ifeat].ID();
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the index of the feature with the given ID.
----------------------------------------------------------------------------------------------*/
size_t GrEngine::FeatureWithID_ff(featid fid)
{
	int ifeatRet;
	/* GrFeature * pfeat = */
	FeatureWithID(fid, &ifeatRet);
	return (size_t)ifeatRet;
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the label for the ifeat-th feature.
----------------------------------------------------------------------------------------------*/
bool GrEngine::GetFeatureLabel_ff(size_t ifeat, lgid nLanguage, utf16 * rgchwLabel)
{
	std::wstring stu = m_rgfeat[ifeat].Label(this, nLanguage);

	int cch = stu.size();
	cch = min(cch, 127); // 1 char for zero-termination
	std::copy(stu.data(), stu.data() + cch, rgchwLabel);
	rgchwLabel[cch] = 0;

	return (cch > 0);
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the index of the default setting. Return -1 if it cannot be found.
----------------------------------------------------------------------------------------------*/
int GrEngine::GetFeatureDefault_ff(size_t ifeat)
{
	GrFeature * pfeat = m_rgfeat + ifeat;
	int defaultValue = pfeat->DefaultValue();
	int cfset = pfeat->NumberOfSettings();
	int rgnSettings[100];
	pfeat->Settings(100, rgnSettings);
	Assert(cfset < 100); // TODO: improve this
	for (int ifset = 0; ifset < cfset; ifset++)
	{
		if (rgnSettings[ifset] == defaultValue)
			return ifset;
	}
	return -1;
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the number of settings for the ifeat-th feature.
----------------------------------------------------------------------------------------------*/
size_t GrEngine::NumberOfSettings_ff(size_t ifeat)
{
	return m_rgfeat[ifeat].NumberOfSettings();
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the value of the ifset-th setting for the ifeat-th feature.
----------------------------------------------------------------------------------------------*/
int GrEngine::GetFeatureSettingValue_ff(size_t ifeat, size_t ifset)
{
	return m_rgfeat[ifeat].NthSetting(ifset);
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the UI label for the given feature setting.
----------------------------------------------------------------------------------------------*/
bool GrEngine::GetFeatureSettingLabel_ff(size_t ifeat, size_t ifset, lgid language,
	utf16 * rgchwLabel)
{
	std::wstring stu = m_rgfeat[ifeat].NthSettingLabel(this, ifset, language);

	int cch = stu.size();
	cch = min(cch, 127); // 1 char for zero-termination
	// Note: the wchar_t label was originally assigned from utf16 data, so although wchar_t is 
	// utf32 on some platforms the conversion back to utf16 should still give correct results.
	std::copy(stu.data(), stu.data() + cch, rgchwLabel);
	rgchwLabel[cch] = 0;

	return (cch > 0);
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the number of languages that are possible among the feature labels.
----------------------------------------------------------------------------------------------*/
size_t GrEngine::NumberOfFeatLangs_ff()
{
	SetUpFeatLangList();
	return m_clcidFeatLabelLangs;
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the language LCID for the feature-label language with the given index.
----------------------------------------------------------------------------------------------*/
short GrEngine::GetFeatLabelLang_ff(size_t ilang)
{
	SetUpFeatLangList();
	return m_rglcidFeatLabelLangs[ilang];
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the number of supported languages.
----------------------------------------------------------------------------------------------*/
size_t GrEngine::NumberOfLanguages_ff()
{
	return (size_t)m_langtbl.NumberOfLanguages();
}

/*----------------------------------------------------------------------------------------------
	For FontFace: return the language code for the language with the given index.
----------------------------------------------------------------------------------------------*/
isocode GrEngine::GetLanguageCode_ff(size_t ilang)
{
	return m_langtbl.LanguageCode(ilang);
}

/*----------------------------------------------------------------------------------------------
	Set up the list of all the languages that are present in the feature labels.
----------------------------------------------------------------------------------------------*/
void GrEngine::SetUpFeatLangList()
{
	if (m_rglcidFeatLabelLangs)
		return;

	int rgnNameIDs[kMaxFeatures];
	for (int ifeat = 0; ifeat < m_cfeat; ifeat++)
		rgnNameIDs[ifeat] = m_rgfeat[ifeat].NameId();
	short rglcid[128]; // 128 is the number expected by TtfUtil::GetLangsForNames
	m_clcidFeatLabelLangs = TtfUtil::GetLangsForNames(m_pNameTbl, 3, 1, rgnNameIDs, m_cfeat, rglcid);
	m_rglcidFeatLabelLangs = new short[m_clcidFeatLabelLangs];
	memcpy(m_rglcidFeatLabelLangs, rglcid, sizeof(short) * m_clcidFeatLabelLangs);
}

/*----------------------------------------------------------------------------------------------
	Return the maximum size needed for the block of data to pass between segments, that is,
	to reinitialize the engine based on the results of the previously generated segment.
	This value must match what is in GrTableManager::InitializeStreams() and
	InitializeForNextSeg(). 256 is an absolute maximum imposed by the interface.

	In Graphite, what this block of data will contain is information about cross-line
	contextualization and some directionality information.

	Assumes InitNew() has already been called to set the font name.
----------------------------------------------------------------------------------------------*/
GrResult GrEngine::get_SegDatMaxLength(int * pcb)
{
	ChkGrOutPtr(pcb);

	if (m_resFontValid == kresInvalidArg)
		ReturnResult(kresUnexpected);	// engine not initialized

	GrResult res = m_resFontValid;
	if (m_resFontValid == kresFail || m_resFontValid == kresUnexpected || m_resFontValid == kresFalse)
		res = kresOk;	// invalid font
	if (ResultFailed(res))
		ReturnResult(res);

	Assert(m_ptman);
	if (!m_ptman)
		*pcb = 256;
	else
		*pcb = m_ptman->NumberOfPasses() + 4;

	ReturnResult(res);
}

/*----------------------------------------------------------------------------------------------
	@return The supported script direction(s). If more than one, the application is
	responsible for choosing the most appropriate.
----------------------------------------------------------------------------------------------*/
GrResult GrEngine::get_ScriptDirection(unsigned int * pgrfsdc, OLECHAR * /*prgchwErrMsg*/, int /*cchMaxErrMsg*/)
{
	ChkGrOutPtr(pgrfsdc);
	ChkGrArrayArg(prgchwErrMsg, cchMaxErrMsg);

	if (m_resFontValid == kresInvalidArg)
		ReturnResult(kresUnexpected);	// engine not initialized

	GrResult res = m_resFontValid;

	*pgrfsdc = m_grfsdc;

//	ClearFontError(prgchwErrMsg, cchMaxErrMsg);
//	try
//	{
//		if (m_resFontValid == kresFail || m_resFontValid == kresUnexpected || m_resFontValid == kresFalse)
//		{
//			RecordFontLoadError(prgchwErrMsg, cchMaxErrMsg);
//			res = kresOk;
//		}
//		*pgrfsdc = m_grfsdc;
//	}
//	catch (Throwable & thr)
//	{
//		res = (GrResult)thr.Error();
//		*pgrfsdc = kfsdcHorizLtr;
//	}
//	catch (...)
//	{
//		res = WARN(kresFail);
//		*pgrfsdc = kfsdcHorizLtr;
//	}

	ReturnResult(res);
}

/*----------------------------------------------------------------------------------------------
	Get an glyph attribute from the engine that will help the GrJustifier in its work.
----------------------------------------------------------------------------------------------*/
GrResult GrEngine::getGlyphAttribute(int iGlyph, int jgat, int nLevel, float * pValueRet)
{
	return m_ptman->State()->GetGlyphAttrForJustification(iGlyph, jgat, nLevel, pValueRet);
}

GrResult GrEngine::getGlyphAttribute(int iGlyph, int jgat, int nLevel, int * pValueRet)
{
	return m_ptman->State()->GetGlyphAttrForJustification(iGlyph, jgat, nLevel, pValueRet);
}

/*----------------------------------------------------------------------------------------------
	Set an glyph attribute in the engine as a result of the decisions made by the
	GrJustifier.
----------------------------------------------------------------------------------------------*/
GrResult GrEngine::setGlyphAttribute(int iGlyph, int jgat, int nLevel, float value)
{
	return m_ptman->State()->SetGlyphAttrForJustification(iGlyph, jgat, nLevel, value);
}
	
GrResult GrEngine::setGlyphAttribute(int iGlyph, int jgat, int nLevel, int value)
{
	return m_ptman->State()->SetGlyphAttrForJustification(iGlyph, jgat, nLevel, value);
}

//:>********************************************************************************************
//:>	Non-FieldWorks interface methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Make a complete segment that can be used to measure and do line-breaking.

	OBSOLETE - delete
----------------------------------------------------------------------------------------------*/
/*
GrResult GrEngine::MakeMeasuredSegment(
	Font * pfont, ITextSource * pgts,
	int ichMin, int ichLim,
	bool fParaRtl,
	Segment ** ppsegRet, SegEnd * pest,
	std::ostream * pstrmLog,
	OLECHAR * prgchwErrMsg, int cchMaxErrMsg)
{
	int dichContext, dichLimSeg;
	float dxWidth;
	return MakeSegment(ksegmodeMsfr,
		pfont, pgts, NULL, ichMin, ichLim, false, false,
		100000, klbClipBreak, ktwshAll,	// not used
		fParaRtl,
		ppsegRet, &dxWidth,
		0, NULL, 0, NULL, NULL, &dichContext,	// not used
		pstrmLog,
		prgchwErrMsg, cchMaxErrMsg,
		-1, false, false, klbClipBreak,	// not used
		&dichLimSeg, pest,
		false, 100000, kestMoreLines);			// not used
}
*/

/*----------------------------------------------------------------------------------------------
	Make a segment from the given range, regardless of the kind of line-break that results.

	OBSOLETE - delete
----------------------------------------------------------------------------------------------*/
/*
GrResult GrEngine::MakeSegmentFromRange(
	Font * pfont, ITextSource * pgts, IGrJustifier * pgj,
	int ichMin, int ichLim,
	bool fStartLine, bool fEndLine,
	TrWsHandling twsh, bool fParaRtl,
	Segment ** ppsegRet, float * pdxWidth, SegEnd * pest,
	int cbPrev, byte * pbPrevSegDat, int cbNextMax, byte * pbNextSegDat, int * pcbNextSegDat,
	int * pdichwContext,
	std::ostream * pstrmLog,
	OLECHAR * prgchwErrMsg, int cchwMaxErrMsg)
{
	int dichLimSegBogus;
	return MakeSegment(ksegmodeMsfr, pfont, pgts, pgj,
		ichMin, ichLim,
		fStartLine, fEndLine,
		10000, klbWordBreak, // not used
		twsh, fParaRtl,
		ppsegRet, pdxWidth,
		//cbPrev, pbPrevSegDat, cbNextMax, pbNextSegDat, pcbNextSegDat, pdichwContext,
		0, NULL, 0, NULL, NULL, NULL,
		pstrmLog,
		prgchwErrMsg, cchwMaxErrMsg,
		ichLim, false, false, klbClipBreak,	// not used
		&dichLimSegBogus, pest,
		10000, kestMoreLines);	// not used
}
*/


//:>********************************************************************************************
//:>	Other methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Create a segment.
----------------------------------------------------------------------------------------------*/
void GrEngine::MakeSegment(
	// Common parameters
	Segment * psegNew,
	Font * pfont, ITextSource * pgts, IGrJustifier * pjus,
	LayoutEnvironment & layout,
	int ichMin, int ichLim,
	// for finding a break point
	float dxMaxWidth,
	bool fBacktracking,
	// for justification
	bool fJust, float dxJustifiedWidthJ, SegEnd estJ)
{
	ChkGrArgPtr(pgts);
	ChkGrArgPtrN(pjus);
	ChkGrOutPtr(psegNew);
	Assert(layout.bestBreak() <= layout.worstBreak());

	if (pjus == NULL)
		pjus = layout.justifier();

	if (m_resFontValid == kresInvalidArg)
		// Uninitialized.
		return;   // ReturnResult(kresUnexpected);

	SetCmapAndNameTables(pfont);

	int segmode = ksegmodeRange; // segment from range
	if (fJust)
		segmode = ksegmodeJust;	// justified segment
	else if (dxMaxWidth < kPosInfFloat)
		segmode = ksegmodeBreak;	// find break point

	bool fFeatureVariations = pgts->featureVariations();

	// Temporary:
	if (!this->m_ptman)
	{
		Warn("No Graphite tables");
		m_ptman = new GrTableManager(this);
		CreateEmpty();
	}

	// GrResult hr = kresOk;
	m_fInErrorState = false;

	bool fBold, fItalic;
	GetStyles(pfont, ichMin, &fBold, &fItalic);

	GetWritingSystemDirection(pgts, ichMin);

	//	Find the end of the range to render with the current font.
	int nDirDepth = 0;
	int ichFontLim = FindFontLim(pgts, ichMin, &nDirDepth);
    int ichStrmLim = 0;
    int ichSegLim = 0;
	switch (segmode)
	{
	case ksegmodeBreak:	// find break point
		//	Here ichLim should be the end of the text-source, unless we're backtracking.
		ichStrmLim = min(ichFontLim, ichLim);
		ichSegLim = -1; // unknown
		break;
	case ksegmodeJust:	// stretching an existing segment to achieve justification
		ichStrmLim = ichFontLim;
		ichSegLim = ichLim;
		break;
	//case ksegmodeMms:	// MakeMeasuredSegment
	//	ichLim = ichFontLim;
	//	ichStrmLim = ichFontLim;
	//	ichSegLim = -1; // process to the end of the stream
	//	break;
	case ksegmodeRange:	// segment from range
		ichLim = min(ichLim, ichFontLim);
		ichStrmLim = ichFontLim;
		ichSegLim = (ichLim == kPosInfinity) ? -1 : ichLim;
		break;
	default:
		Assert(false);
	}

	//	Initialize the graphics object with the font and character properties.
	//	SILE
	////if (m_fUseSepBase)
	////{
	////	Assert(m_stuBaseFaceName.size() > 0);
	////	SetUpGraphics(pgg, pgts, ichMin, m_stuBaseFaceName);
	////}
	////else
	////	SetUpGraphics(pgg, pgts, ichMin, m_stuFaceName);

	GrResult res = kresOk;

	Assert(m_ptman);
	m_ptman->State()->SetFont(pfont);

	//	Create a character stream on the text source.
	GrCharStream * pchstrm = new GrCharStream(pgts, ichMin, ichStrmLim,
		layout.startOfLine(), layout.endOfLine());

	// TODO: change vertical offset to work on a character-by-character basis
	m_dysOffset = (pgts->getVerticalOffset(ichMin) * pfont->getDPIy()) / 72;

	//	Run the tables and get back a segment.

	bool fMoreText = false;
	int ichCallerBtLim = -1;
	bool fInfiniteWidth = false;
	switch (segmode)
	{
	//case ksegmodeMms:
	case ksegmodeBreak:
	case ksegmodeRange:
		if (segmode == ksegmodeBreak)
		{
			fMoreText = ((ichFontLim < ichLim) || fBacktracking);
			ichCallerBtLim = (fBacktracking) ? ichLim : -1;
			fInfiniteWidth = false;
		}
		//else if (segmode == ksegmodeMms)
		//{
		//	fMoreText = false;
		//	ichCallerBtLim = -1;
		//	fInfiniteWidth = true;
		//	Assert(twsh == ktwshAll);
		//}
		else if (segmode == ksegmodeRange)
		{
			int ichTextLim;
			ichTextLim = pgts->getLength();
			fMoreText = (ichLim < ichTextLim);
			ichCallerBtLim = -1;
			fInfiniteWidth = true;
		}
		try {
			m_ptman->Run(psegNew, pfont, pchstrm, pjus,
				((pjus) ? kjmodiCanShrink : kjmodiNormal),
				layout,
				ichSegLim, dxMaxWidth, 0,
				fBacktracking, fMoreText, fFeatureVariations, ichFontLim, fInfiniteWidth, false,
				ichCallerBtLim,
				nDirDepth, estJ);
		}
		catch (...)
		{
			// Disastrous problem in rendering.
			if (!layout.dumbFallback())
				throw; // throws original exception
			
			// Try dumb rendering.
			if (m_fUseSepBase)
				SwitchGraphicsFont(true); // use the base font, which has real glyphs

			m_fInErrorState = true;
			res = RunUsingEmpty(psegNew, pfont, pchstrm, layout,
				ichSegLim,
				dxMaxWidth,
				fBacktracking, fMoreText, fInfiniteWidth,
				ichCallerBtLim,
				nDirDepth, estJ);
		}
		break;

	case ksegmodeJust: // justified segment
		{
		m_ptman->Run(psegNew, pfont, pchstrm, pjus, kjmodiJustify, layout,
			ichSegLim,
			dxJustifiedWidthJ, dxMaxWidth,
			false, false, false,	// not used
			ichFontLim,
			true,	// infinite width
			false,  // kludge
			-1,
			nDirDepth, estJ);
			//&dxWidth, &est);
		Assert((ichLim - ichMin) == (psegNew->stopCharacter() - psegNew->startCharacter()));
		break;
		}

	default:
		Assert(false);
	}

	delete pchstrm;

	if (m_fUseSepBase)
		SwitchGraphicsFont(false);
}

/*----------------------------------------------------------------------------------------------
	Return an indication of whether an error happened in trying to load Graphite tables.
----------------------------------------------------------------------------------------------*/
FontErrorCode GrEngine::IsValidForGraphite(int * pnVersion, int * pnSubVersion)
{
	if (pnVersion)
		*pnVersion = m_fxdBadVersion >> 16;
	if (pnSubVersion)
		*pnSubVersion = m_fxdBadVersion & 0x0000FFFF;

	return m_ferr;
}

/*----------------------------------------------------------------------------------------------
	Record a system error indicating that the font could not be loaded properly.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void GrEngine::RecordFontLoadError(OLECHAR * prgchwErrMsg, int cchMax)
{
	if (prgchwErrMsg == NULL || cchMax == 0)
		return;

	std::wstring stuMessage = L"Error in initializing Graphite font \"";
	stuMessage.append(m_stuFaceName);
	if (m_stuErrCtrlFile.size())
	{
		stuMessage.append(L"\" (");
		stuMessage.append(m_stuErrCtrlFile);
		stuMessage.append(L")");
	}
	else
		stuMessage.append(L"\"");
	if (m_stuInitError.size())
	{
		stuMessage.append(L"--\n");
		stuMessage.append(m_stuInitError);
	}

	std::fill_n(prgchwErrMsg, cchMax, L'\0');
	std::copy(stuMessage.data(), stuMessage.data() + min(cchMax - 1, signed(stuMessage.size())),
				prgchwErrMsg);
}

/*----------------------------------------------------------------------------------------------
	Record a system error indicating a bad error in rendering using a supposedly valid font.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void GrEngine::RecordFontRunError(OLECHAR * prgchwErrMsg, int cchMax, GrResult /*res*/)
{
	if (prgchwErrMsg == NULL || cchMax == 0)
		return;

	std::wstring stuMessage = L"Error in rendering using Graphite font \"";
	stuMessage.append(m_stuFaceName);
	if (m_stuErrCtrlFile.size())
	{
		stuMessage.append(L"\" (");
		stuMessage.append(m_stuErrCtrlFile);
		stuMessage.append(L")");
	}
	else
		stuMessage.append(L"\"");

	std::fill_n(prgchwErrMsg, cchMax, L'\0');
	std::copy(stuMessage.data(), stuMessage.data() + min(cchMax - 1, signed(stuMessage.size())),
			 prgchwErrMsg);
}

/*----------------------------------------------------------------------------------------------
	Initialize the font error message buffer to zero. This way we can be sure to recognize
	an error condition by the fact that the buffer is non-zero.
	OBSOLETE
----------------------------------------------------------------------------------------------*/
void GrEngine::ClearFontError(OLECHAR * prgchwErrMsg, int cchMaxErrMsg)
{
	std::fill_n(prgchwErrMsg, cchMaxErrMsg, L'\0');
}

/*----------------------------------------------------------------------------------------------
	Set the overall direction of the writing system based on the text properties.
----------------------------------------------------------------------------------------------*/
void GrEngine::GetWritingSystemDirection(ITextSource * pgts, int ichwMin)
{
	m_fRightToLeft = pgts->getRightToLeft(ichwMin);
}

/*----------------------------------------------------------------------------------------------
	Find the end of the range that uses the current font.
	
	@param pgts				- contains text to render
	@param ichwMinFont		- beginning of range to render with this font
	@param pnDirDepth		- return direction depth: 0=LTR, 1=RTL

	@return The lim of the range that could be rendered by this font. Also the direction depth.
----------------------------------------------------------------------------------------------*/
int GrEngine::FindFontLim(ITextSource * pgts, int ichwMinFont, int * pnDirDepth)
{
	int ichwTextLen = (int)pgts->getLength();

	int ichwLimRange;	// range that can be rendered without breaking contextualization.
	int ichwMinNext = ichwMinFont;

	while (true)
	{
		std::pair<toffset, toffset> pairRange = pgts->propertyRange(ichwMinNext);
		// int ichwMinRun = pairRange.first;
		int ichwLimRun = pairRange.second;
		*pnDirDepth = pgts->getDirectionDepth(ichwMinNext);

		//if (ichwLim > -1 && ichwLim < ichwLimRun)
		//{
		//	//	Stopping in the middle of a run
		//	return ichwLim;
		//}

		//	We can at least go to the end of this run.
		ichwLimRange = ichwLimRun;
		if (ichwLimRun >= ichwTextLen)
		{
			//	Hit the end of the text-source
			return ichwLimRange;
		}
		else
		{
			//	If the following run is the same as this run except for color and underlining,
			//	we don't have to break here.
			if (!pgts->sameSegment(ichwMinNext, ichwLimRun))
				return ichwLimRange;
		}
		ichwMinNext = ichwLimRun;
	}

	return ichwLimRange;
}

/*----------------------------------------------------------------------------------------------
	Read the version number from the tables, and return false if any are a version this
	implementation of the engine can't handle.
----------------------------------------------------------------------------------------------*/
bool GrEngine::CheckTableVersions(GrIStream * pgrstrm,
		const byte *pSilfTbl, int lSilfStart,
		const byte *pGlobTbl, int lGlocStart,
		const byte *pFeatTbl, int lFeatStart,
		int * pfxdBadVersion)
{
	pgrstrm->OpenBuffer(pSilfTbl, sizeof(int));
    pgrstrm->SetPositionInFont(lSilfStart);
	*pfxdBadVersion = ReadVersion(*pgrstrm);
	pgrstrm->CloseBuffer();
	if (*pfxdBadVersion > kSilfVersion)
		return false;

	pgrstrm->OpenBuffer(pGlobTbl, lGlocStart + sizeof(int));
	pgrstrm->SetPositionInFont(lGlocStart);
	*pfxdBadVersion = ReadVersion(*pgrstrm);
	pgrstrm->CloseBuffer();
	if (*pfxdBadVersion > kGlocVersion)
		return false;

	pgrstrm->OpenBuffer(pFeatTbl, sizeof(int));
	pgrstrm->SetPositionInFont(lFeatStart);
	*pfxdBadVersion = ReadVersion(*pgrstrm);
	pgrstrm->CloseBuffer();
	if (*pfxdBadVersion > kFeatVersion)
		return false;

	*pfxdBadVersion = 0;
	return true;
}

/*----------------------------------------------------------------------------------------------
	Reinterpret the version number from a font table.
----------------------------------------------------------------------------------------------*/
int GrEngine::ReadVersion(GrIStream & grstrm)
{
	int fxdVersion = grstrm.ReadIntFromFont();

	if (fxdVersion < 0x00010000)
		fxdVersion = 0x00010000; // kludge for bug with which some fonts were generated

	return fxdVersion;
}

/*----------------------------------------------------------------------------------------------
	Return whether the text is asking for bold and/or italic text.
----------------------------------------------------------------------------------------------*/
void GrEngine::GetStyles(Font * pfont, int /*ichwMin*/, bool * pfBold, bool * pfItalic)
{
	*pfBold = pfont->bold();
	*pfItalic = pfont->italic();
}

/*----------------------------------------------------------------------------------------------
	Switch the Graphics object between the Graphite table file and the base font.
	Should only be called when we know we are using a base font, or when we are reading
	the base font to see if it is valid.
----------------------------------------------------------------------------------------------*/
#ifdef NDEBUG
void GrEngine::SwitchGraphicsFont(bool)
#else
void GrEngine::SwitchGraphicsFont(bool fBase)
#endif
{
	Assert(!fBase || m_stuBaseFaceName.size() > 0);

	//LgCharRenderProps chrp;
	//pgg->get_FontCharProperties(&chrp);
	//if (fBase)
	//{
	//	wcsncpy(chrp.szFaceName, m_stuBaseFaceName.data(), m_stuBaseFaceName.size() + 1);
	//	chrp.szFaceName[31] = 0;
	//}
	//else
	//{
	//	wcsncpy(chrp.szFaceName, m_stuFaceName.data(), m_stuFaceName.size() + 1);
	//	chrp.szFaceName[31] = 0;
	//}
	//pgg->SetupGraphics(&chrp);
}

/*----------------------------------------------------------------------------------------------
	Read the contents of the "Silf" table from the stream, which is on an extended
	TrueType font file. Specifically, read the iSubTable-th sub-table (for now there is
	only one).

	WARNING: any changes to this method must be accompanied by equivalent changes to 
	CreateEmpty().
----------------------------------------------------------------------------------------------*/
bool GrEngine::ReadSilfTable(GrIStream & grstrm, long lTableStart, int iSubTable,
	long cbTableLen, int * pchwMaxGlyphID, int * pfxdSilfVersion)
{
	grstrm.SetPositionInFont(lTableStart);

	//	version
	*pfxdSilfVersion = ReadVersion(grstrm);
	if (*pfxdSilfVersion > kSilfVersion)
		// Version we don't know how to handle.
		return false;

	if (*pfxdSilfVersion >= 0x00030000)
	{
		//	compiler version--more accurately, the level of semantics needed to handle
		//	this font
		int fxdCompilerVersion = ReadVersion(grstrm);
        if (fxdCompilerVersion > 0x00040000)
            return false;
    }

	//	number of tables
	unsigned short cSubTables = grstrm.ReadUShortFromFont();
	Assert(cSubTables == 1);	// for now
	Assert(cSubTables <= kMaxSubTablesInFont);
	if (cSubTables != 1 || cSubTables > kMaxSubTablesInFont)
		return false;	// bad font

    if (*pfxdSilfVersion >= 0x00020000)
		// reserved
		grstrm.ReadShortFromFont();

	//	subtable offsets and lengths
	int nSubTableOffsets[kMaxSubTablesInFont+1];
	int cbSubTableLengths[kMaxSubTablesInFont];
	nSubTableOffsets[cSubTables] = cbTableLen;
	int i;
	for (i = 0; i < cSubTables; i++)
	{
		nSubTableOffsets[i] = grstrm.ReadIntFromFont();
        if (nSubTableOffsets[i] < 0 || nSubTableOffsets[i] > cbTableLen)
            return false;
		if (i > 0)
        {
			cbSubTableLengths[i-1] = nSubTableOffsets[i] - nSubTableOffsets[i-1];
            if (cbSubTableLengths[i-1] < 40)
                return false;
        }
	}
	cbSubTableLengths[cSubTables-1] = nSubTableOffsets[cSubTables] - nSubTableOffsets[cSubTables-1];

	grstrm.SetPositionInFont(lTableStart + nSubTableOffsets[iSubTable]);

	//	Now the stream is at the beginning of the desired sub-table.

	//	Get the position of the start of the table.
	long lSubTableStart;
	grstrm.GetPositionInFont(&lSubTableStart);

	//	rule version
	int fxdRuleVersion = (*pfxdSilfVersion >= 0x00030000) ?
		ReadVersion(grstrm) :
		*pfxdSilfVersion;

	long lPassBlockPos = -1;
	long lPseudosPos = -1;
	if (*pfxdSilfVersion >= 0x00030000)
	{
		lPassBlockPos = grstrm.ReadUShortFromFont();
		if (lPassBlockPos > cbSubTableLengths[iSubTable])
			return false;	// bad table
        lPassBlockPos += lSubTableStart;
		lPseudosPos = grstrm.ReadUShortFromFont();
		if (lPseudosPos > cbSubTableLengths[iSubTable])
			return false;	// bad table
        lPseudosPos += lSubTableStart;
	}

	//	maximum glyph ID
	data16 chwTmp;
	*pchwMaxGlyphID = grstrm.ReadUShortFromFont();

	//	extra ascent and descent
	m_mXAscent = grstrm.ReadShortFromFont();
	m_mXDescent = grstrm.ReadShortFromFont();

	// TODO: decide whether we want these:
	m_mXAscent = 0;
	m_mXDescent = 0;

	//	number of passes
	byte cPasses = grstrm.ReadByteFromFont();
	//	index of first substitution pass
	byte ipassSub1 = grstrm.ReadByteFromFont();
	//	index of first positioning pass
	byte ipassPos1 = grstrm.ReadByteFromFont();
	//	index of first justification pass
	byte ipassJust1 = grstrm.ReadByteFromFont();
	//	index of first reordered pass, or 0xFF if no reordering
	byte ipassReordered1 = grstrm.ReadByteFromFont();
    if (ipassReordered1 != 0xFF && ipassReordered1 > cPasses)
        return false;
	if (*pfxdSilfVersion < 0x00020000)
	{
		Assert(ipassJust1 == cPasses || ipassJust1 == ipassPos1);
		ipassJust1 = ipassPos1;
	}

	//	Sanity checks.
	if (cPasses >= kMaxPasses || ipassSub1 > ipassPos1 || ipassPos1 > ipassJust1 || ipassJust1 > cPasses)
		return false; // bad table
	
	//	line-break flag
	int nLineBreak = grstrm.ReadByteFromFont();
	if (nLineBreak > 3)
		return false; // bad table
    m_fLineBreak = ((nLineBreak & 0x0001) == 0)? false : true;
	// ignore other flag

	//	range of possible cross-line-boundary contextualization
	m_cchwPreXlbContext = grstrm.ReadByteFromFont();
	m_cchwPostXlbContext = grstrm.ReadByteFromFont();

	//	actual glyph ID for pseudo-glyph (ID of bogus attribute)
	byte bTmp; // unsigned
	bTmp = grstrm.ReadByteFromFont();
	m_chwPseudoAttr = bTmp;
	//	breakweight
	bTmp = grstrm.ReadByteFromFont();
	m_chwBWAttr = bTmp;
	//	directionality
	bTmp = grstrm.ReadByteFromFont();
	m_chwDirAttr = bTmp;

	//	Sanity checks--don't bother with these, I don't seem to be able to know what are reasonable values.
	//if (m_chwPseudoAttr > 200 || m_chwBWAttr > 200 || m_chwDirAttr > 200)
	//	return false; // bad table

	if (*pfxdSilfVersion >= 0x00020000)
	{
		bTmp = grstrm.ReadByteFromFont();
		if (bTmp != 0)
			// The mirror attributes are defined, so first component is 5.
			m_nCompAttr1 = 5;
		else if (m_chwPseudoAttr < 3)
		{
			// The *actualForPsuedo*, directionality, and breakweight attributes are first.
			gAssert(m_chwBWAttr < 3);
			gAssert(m_chwDirAttr < 3);
			m_nCompAttr1 = 3;
		}
		else
			// The component attributes are first.
			m_nCompAttr1 = 0;

		// reserved
		grstrm.ReadByteFromFont();

		//	justification levels
		m_cJLevels = grstrm.ReadByteFromFont();
		if (m_cJLevels > kMaxJLevels)
			return false; // bad table
		m_fBasicJust = (m_cJLevels == 0);
		m_chwJStretch0 = 0xffff; // if no justification
		m_chwJShrink0 = 0xffff;
		m_chwJStep0 = 0xffff;
		m_chwJWeight0 = 0xffff;
		for (i = 0; i < m_cJLevels; i++)
		{
			//	justification glyph attribute IDs
			bTmp = grstrm.ReadByteFromFont();
			if (i == 0)
				m_chwJStretch0 = bTmp;
			bTmp = grstrm.ReadByteFromFont();
			if (i == 0)
				m_chwJShrink0 = bTmp;
			bTmp = grstrm.ReadByteFromFont();
			if (i == 0)
				m_chwJStep0 = bTmp;
			bTmp = grstrm.ReadByteFromFont();
			if (i == 0)
				m_chwJWeight0 = bTmp;
			bTmp = grstrm.ReadByteFromFont(); // runto
			// reserved
			grstrm.ReadByteFromFont();
			grstrm.ReadByteFromFont();
			grstrm.ReadByteFromFont();
		}
	}
	else
	{
		m_cJLevels = 0;
		m_fBasicJust = true;
		m_chwJStretch0 = 0xffff;
		m_chwJShrink0 = 0xffff;
		m_chwJStep0 = 0xffff;
		m_chwJWeight0 = 0xffff;

		if (m_chwPseudoAttr == 0)
			// The *actualForPsuedo*, directionality, and breakweight attributes are first.
			m_nCompAttr1 = 3;
		else
			// The component attributes are first.
			m_nCompAttr1 = 0;

	}

	//	number of component attributes
	chwTmp = grstrm.ReadUShortFromFont();
	m_cComponents = chwTmp;

	//	number of user-defined slot attributes
	m_cnUserDefn = grstrm.ReadByteFromFont();
	if (m_cnUserDefn > kMaxUserDefinableSlotAttrs)
		return false; // bad table

	//	max number of ligature components per glyph
	m_cnCompPerLig = grstrm.ReadByteFromFont();
	if (m_cnCompPerLig > 16)
		return false; // bad table

	//	directions supported
	bTmp = grstrm.ReadByteFromFont();
	m_grfsdc = bTmp;
	if (m_grfsdc > kfsdcHorizLtr + kfsdcHorizRtl + kfsdcVertFromLeft + kfsdcVertFromRight)
		return false; // bad table

	//	reserved
	bTmp = grstrm.ReadByteFromFont();
	bTmp = grstrm.ReadByteFromFont();
	bTmp = grstrm.ReadByteFromFont();

	//	critical features
	int cCriticalFeatures;
	if (*pfxdSilfVersion >= 0x00020000)
	{
		//	reserved
		bTmp = grstrm.ReadByteFromFont();

		cCriticalFeatures = grstrm.ReadByteFromFont();
		Assert(cCriticalFeatures == 0);
		if (cCriticalFeatures != 0)
			return false; // bad table

		//	reserved
		bTmp = grstrm.ReadByteFromFont();
	}

	//	rendering behaviors--ignore for now
	byte cScripts = grstrm.ReadByteFromFont();
	int nTmp;
	//unsigned int nBehaviors[kMaxRenderingBehavior]; -- this big buffer causes a stack overflow in Multiscribe; rework eventually
	for (i = 0; i < cScripts; i++)
	{
		//nBehaviors[i] = unsigned(grstrm.ReadIntFromFont());
		nTmp = unsigned(grstrm.ReadIntFromFont());
	}

	//	linebreak glyph ID
	m_chwLBGlyphID = grstrm.ReadUShortFromFont();

	//	Jump to the beginning of the pass offset block, if we have this information.
	if (*pfxdSilfVersion >= 0x00030000)
		grstrm.SetPositionInFont(lPassBlockPos);
	else
		//	Otherwise assume that's where we are!
		Assert(lPassBlockPos == -1);

	//	offsets to passes, relative to the start of this subtable;
	//	note that we read (cPasses + 1) of these
	int nPassOffsets[kMaxPasses];
	int cbPassLengths[kMaxPasses];
	for (i = 0; i <= cPasses; i++)
	{
		nPassOffsets[i] = grstrm.ReadIntFromFont();
		if (nPassOffsets[i] > cbSubTableLengths[iSubTable])
			return false;	// bad table (note that the "last" bogus pass's offset will = the table length)
		if (i > 0)
        {
			cbPassLengths[i-1] = nPassOffsets[i] - nPassOffsets[i-1];
            if (cbPassLengths[i-1] < 44)
                return false;
        }
	}

	//	Jump to the beginning of the pseudo-glyph info block, if we have this information.
	if (*pfxdSilfVersion >= 0x00030000)
		grstrm.SetPositionInFont(lPseudosPos);
	else
		//	Otherwise assume that's where we are!
		Assert(lPseudosPos == -1);

	//	number of pseudo-glyphs and search constants
	unsigned short snTmp;
	snTmp = grstrm.ReadUShortFromFont();
	m_cpsd = snTmp;
	snTmp = grstrm.ReadUShortFromFont();
	m_dipsdInit = snTmp;
	snTmp = grstrm.ReadUShortFromFont();
	m_cPsdLoop = snTmp;
	snTmp = grstrm.ReadUShortFromFont();
	m_ipsdStart = snTmp;

    if (m_dipsdInit > m_cpsd || m_cPsdLoop > m_cpsd || m_ipsdStart > m_cpsd)
        return false;

	//	unicode-to-pseudo map
	m_prgpsd = new GrPseudoMap[m_cpsd];
	for (i = 0; i < m_cpsd; i++)
	{
		if (*pfxdSilfVersion <= 0x00010000)
		{
			utf16 chwUnicode = grstrm.ReadUShortFromFont();
			m_prgpsd[i].SetUnicode(chwUnicode);
		}
		else
		{
			int nUnicode = grstrm.ReadIntFromFont();
			m_prgpsd[i].SetUnicode(nUnicode);
		}
		gid16 chwPseudo = grstrm.ReadUShortFromFont();
		m_prgpsd[i].SetPseudoGlyph(chwPseudo);
	}

	//	class table
	m_pctbl = new GrClassTable();
	if (!m_pctbl->ReadFromFont(grstrm, *pfxdSilfVersion))
		return false;

	//	passes
	return m_ptman->CreateAndReadPasses(grstrm, *pfxdSilfVersion, fxdRuleVersion,
		cPasses, lSubTableStart, nPassOffsets, cbPassLengths,
		ipassSub1, ipassPos1, ipassJust1, ipassReordered1);
}

/*----------------------------------------------------------------------------------------------
	Set up the engine with no Graphite smarts at all, just dummy tables.
----------------------------------------------------------------------------------------------*/
void GrEngine::CreateEmpty()
{
	//	Silf table

	m_mXAscent = 0;
	m_mXDescent = 0;

	m_fLineBreak = false;

	//	range of possible cross-line-boundary contextualization
	m_cchwPreXlbContext = 0;
	m_cchwPostXlbContext = 0;

	//	Bogus attribute IDs:
	m_chwPseudoAttr = 1;	//	actual glyph ID for pseudo-glyph (ID of bogus attribute)
	m_chwBWAttr = 2;		// breakweight
	m_chwDirAttr = 3;		// directionality

	m_cComponents = 0;		//	number of component attributes
	m_nCompAttr1 = 5;		//  component attribute

	m_cnUserDefn = 0;		// number of user-defined slot attributes
	m_cnCompPerLig = 0;		// max number of ligature components

	m_grfsdc = kfsdcHorizLtr;	// supported script direction

	//	linebreak glyph ID
	m_chwLBGlyphID = 0xFFFE;
	
	//	number of pseudo-glyphs and search constants
	m_cpsd = 0;
	m_dipsdInit = 0;
	m_cPsdLoop = 0;
	m_ipsdStart = 0;

	//	class table
	m_pctbl = new GrClassTable();
	m_pctbl->CreateEmpty();

	//	passes
	if (m_ptman != 0)
		m_ptman->CreateEmpty();

	//	Gloc and Glat tables
	m_pgtbl = new GrGlyphTable();
	m_pgtbl->SetNumberOfGlyphs(0);
	m_pgtbl->SetNumberOfComponents(0);
	m_pgtbl->SetNumberOfStyles(1);	// for now

	m_pgtbl->CreateEmpty();

	//	Feat table
	m_cfeat = 0;
	m_rglcidFeatLabelLangs = NULL;
	m_clcidFeatLabelLangs = 0;

	//	Language table
	m_langtbl.CreateEmpty();
}

/*----------------------------------------------------------------------------------------------
	Create the glyph table and fill it in from the font stream.
----------------------------------------------------------------------------------------------*/	
bool GrEngine::ReadGlocAndGlatTables(GrIStream & grstrmGloc, long lGlocStart,
	GrIStream & grstrmGlat, long lGlatStart,
	int chwGlyphIDMax, int fxdSilfVersion)
{
	//	Determine the highest used glyph ID number.
//	int chwGlyphIDMax = m_chwLBGlyphID;
//	for (int ipsd = 0; ipsd < m_cpsd; ipsd++)
//	{
//		if (m_prgpsd[ipsd].PseudoGlyph() > chwGlyphIDMax)
//			chwGlyphIDMax = m_prgpsd[ipsd].PseudoGlyph();
//	}

	//	Create the glyph table.
	m_pgtbl = new GrGlyphTable();
	m_pgtbl->SetNumberOfGlyphs(chwGlyphIDMax + 1);
	m_pgtbl->SetNumberOfComponents(m_cComponents);
	m_pgtbl->SetNumberOfStyles(1);	// for now

	return m_pgtbl->ReadFromFont(grstrmGloc, lGlocStart, grstrmGlat, lGlatStart,
		m_chwBWAttr, m_chwJStretch0, m_cJLevels, m_nCompAttr1, m_cnCompPerLig,
		fxdSilfVersion);
}

/*----------------------------------------------------------------------------------------------
	Read the contents of the "Feat" table from the stream, which is on an extended
	TrueType font file.
----------------------------------------------------------------------------------------------*/
bool GrEngine::ReadFeatTable(GrIStream & grstrm, long lTableStart)
{
	short snTmp;
	data16 chwTmp;
	int nTmp;
	
	grstrm.SetPositionInFont(lTableStart);
	
	//	version
	int fxdVersion = ReadVersion(grstrm);
    if (fxdVersion > kFeatVersion)
		return false;
	
	//	number of features
	int cfeat;
	cfeat = grstrm.ReadUShortFromFont();
	if (cfeat > kMaxFeatures)
		return false; // bad table;

	//	reserved
	chwTmp = grstrm.ReadUShortFromFont();
	nTmp = grstrm.ReadIntFromFont();
	
	std::vector<featid> vnIDs; // unsigned ints
	std::vector<int> vnOffsets;
	std::vector<int> vcfset;

	m_cfeat = 0;
	int ifeat;
	for (ifeat = 0; ifeat < cfeat; ifeat++)
	{
		//	ID
		featid nID;
		if (fxdVersion >= 0x00020000)
			nID = (unsigned int)grstrm.ReadIntFromFont();
		else
			nID = grstrm.ReadUShortFromFont();
		vnIDs.push_back(nID);
		//	number of settings
		data16 cfset = grstrm.ReadUShortFromFont();
		vcfset.push_back(cfset);
		if (fxdVersion >= 0x00020000)
			grstrm.ReadShortFromFont(); // pad bytes
		//	offset to settings list
		nTmp = grstrm.ReadIntFromFont();
		vnOffsets.push_back(nTmp);
		//	flags
		chwTmp = grstrm.ReadUShortFromFont();
		Assert(chwTmp == 0x8000); // mutually exclusive
		//	index into name table of UI label
		int nNameId = grstrm.ReadShortFromFont();

		if (fxdVersion <= 0x00020000 && nID == GrFeature::knLangFeatV2)
		{
			// Ignore the obsolete "lang" feature which has ID = 1
			vnIDs.pop_back();
			vcfset.pop_back();
			vnOffsets.pop_back();
			continue;
		}
		AddFeature(nID, nNameId, cfset);
	}

	//	default feature setting value and name strings;
	for (ifeat = 0; ifeat < m_cfeat; ifeat++)
	{
		GrFeature * pfeat = m_rgfeat + ifeat;
		Assert(pfeat->ID() == vnIDs[ifeat]);

		grstrm.SetPositionInFont(lTableStart + vnOffsets[ifeat]);

		//	Each feature has been initialized with blank spots for the number of settings
		//	it has.
		int cfset = vcfset[ifeat];
		Assert(pfeat->NumberOfSettings() == cfset);
		for (int ifset = 0; ifset < cfset; ifset++)
		{
			snTmp = grstrm.ReadShortFromFont();  // yes, signed (TT feat table spec says unsigned)
			short snTmp2 = grstrm.ReadShortFromFont();
			pfeat->AddSetting(snTmp, snTmp2);
			
			if (ifset == 0)
			{
				//	default setting value--first in list
				pfeat->SetDefault(snTmp);
			}
		}
	}

	return true;
}

/*----------------------------------------------------------------------------------------------
	Read the contents of the "Sill" table from the stream, which is on an extended
	TrueType font file.
----------------------------------------------------------------------------------------------*/
bool GrEngine::ReadSillTable(GrIStream & grstrm, long lTableStart)
{
	grstrm.SetPositionInFont(lTableStart);
	
	//	version
	int fxdVersion = ReadVersion(grstrm);
    if (fxdVersion > kSillVersion)
		return false;

	return m_langtbl.ReadFromFont(&grstrm, fxdVersion);
}

/*----------------------------------------------------------------------------------------------
	An error happened in running the tables. Set up temporary dummy tables, and try running
	with dumb rendering.
	Return kresUnexpected if even running the empty tables crashed, kresFail otherwise.
----------------------------------------------------------------------------------------------*/
GrResult GrEngine::RunUsingEmpty(Segment * psegNew, Font * pfont,
	GrCharStream * pchstrm, LayoutEnvironment & layout,
	int ichStop,
	float dxMaxWidth,
	bool fNeedFinalBreak, bool fMoreText, bool fInfiniteWidth,
	int ichwCallerBtLim,
	int nDirDepth, SegEnd estJ)
{
	// Save the current state of the engine.
	int mXAscentSave = m_mXAscent;
	int mXDescentSave = m_mXDescent;
	bool fLineBreakSave = m_fLineBreak;
	int cchwPreXlbContextSave = m_cchwPreXlbContext;
	int cchwPostXlbContextSave = m_cchwPostXlbContext;
	data16 chwPseudoAttrSave = m_chwPseudoAttr;
	data16 chwBWAttrSave = m_chwBWAttr;
	data16 chwDirAttrSave = m_chwDirAttr;
	int cComponentsSave = m_cComponents;
	int cnUserDefnSave = m_cnUserDefn;
	int cnCompPerLigSave = m_cnCompPerLig;
	unsigned int grfsdcSave = m_grfsdc;
	gid16 chwLBGlyphIDSave = m_chwLBGlyphID;
	int cpsdSave = m_cpsd;
	int dipsdInitSave = m_dipsdInit;
	int cPsdLoopSave = m_cPsdLoop;
	int ipsdStartSave = m_ipsdStart;
	GrTableManager * ptmanSave = m_ptman;
	GrClassTable * pctblSave = m_pctbl;
	GrGlyphTable * pgtblSave = m_pgtbl;
	int cfeatSave = m_cfeat;
	GrResult resFontReadSave = m_resFontRead;

	GrResult res = kresFail;

	pchstrm->Restart();

	// Create dummy versions of the tables and run them.
	m_pctbl = NULL;
	m_pgtbl = NULL;
	m_ptman = new GrTableManager(this);
	m_ptman->State()->SetFont(pfont);
	CreateEmpty();

	try
	{
		m_ptman->Run(psegNew, pfont, pchstrm, NULL, kjmodiNormal, layout,
			ichStop, dxMaxWidth, 0,
			fNeedFinalBreak, fMoreText, false, -1, fInfiniteWidth, false,
			ichwCallerBtLim,
			nDirDepth, estJ);
	}
	catch(...)
	{
		// Still didn't work. Oh, well, not much we can do.
		res = kresUnexpected;
	}

	psegNew->SetErroneous(true);

	if (m_ptman)
		delete m_ptman;
	if (m_pctbl)
		delete m_pctbl;
	if (m_pgtbl)
		delete m_pgtbl;

	// Put everything back the way it was.
	m_mXAscent = mXAscentSave;
	m_mXDescent = mXDescentSave;
	m_fLineBreak = fLineBreakSave;
	m_cchwPreXlbContext = cchwPreXlbContextSave;
	m_cchwPostXlbContext = cchwPostXlbContextSave;
	m_chwPseudoAttr = chwPseudoAttrSave;
	m_chwBWAttr = chwBWAttrSave;
	m_chwDirAttr = chwDirAttrSave;
	m_cComponents = cComponentsSave;
	m_cnUserDefn = cnUserDefnSave;
	m_cnCompPerLig = cnCompPerLigSave;
	m_grfsdc = grfsdcSave;
	m_chwLBGlyphID = chwLBGlyphIDSave;
	m_cpsd = cpsdSave;
	m_dipsdInit = dipsdInitSave;
	m_cPsdLoop = cPsdLoopSave;
	m_ipsdStart = ipsdStartSave;
	m_ptman = ptmanSave;
	m_pctbl = pctblSave;
	m_pgtbl = pgtblSave;
	m_cfeat = cfeatSave;
	m_resFontRead = resFontReadSave;

	return res;
}

/*----------------------------------------------------------------------------------------------
	Return the glyph ID for the given Unicode value.
----------------------------------------------------------------------------------------------*/
gid16 GrEngine::GetGlyphIDFromUnicode(int nUnicode)
{
	gid16 chwGlyphID = MapToPseudo(nUnicode);
	if (chwGlyphID != 0)
		return chwGlyphID;	// return a pseudo-glyph

	//	Otherwise, get the glyph ID from the font's cmap.

	if (m_pCmap_3_10)
		return gid16(TtfUtil::Cmap310Lookup(m_pCmap_3_10, nUnicode));
	else if (m_pCmap_3_1)
		return gid16(TtfUtil::Cmap31Lookup(m_pCmap_3_1, nUnicode));
	else
		return 0;
}

/*----------------------------------------------------------------------------------------------
	Return the pseudo-glyph corresponding to the given Unicode input, or 0 if none.
----------------------------------------------------------------------------------------------*/
gid16 GrEngine::MapToPseudo(int nUnicode)
{
	if (m_cpsd == 0)
		return 0;

#ifdef _DEBUG
	int nPowerOf2 = 1;
	while (nPowerOf2 <= m_cpsd)
		nPowerOf2 <<= 1;
	nPowerOf2 >>= 1;
	//	Now nPowerOf2 is the max power of 2 <= m_cpds
	Assert((1 << m_cPsdLoop) == nPowerOf2);		// m_cPsdLoop == log2(nPowerOf2)
	Assert(m_dipsdInit == nPowerOf2);
	Assert(m_ipsdStart == m_cpsd - m_dipsdInit);
#endif // _DEBUG

	int dipsdCurr = m_dipsdInit;

	GrPseudoMap * ppsdCurr = m_prgpsd + m_ipsdStart;
	while (dipsdCurr > 0) 
	{
		int nTest;
		if (ppsdCurr < m_prgpsd)
			nTest = -1;
		else
			nTest = ppsdCurr->Unicode() - nUnicode;

		if (nTest == 0)
			return ppsdCurr->PseudoGlyph();

		dipsdCurr >>= 1;	// divide by 2
		if (nTest < 0)
			ppsdCurr += dipsdCurr;
		else // (nTest > 0)
			ppsdCurr -= dipsdCurr;
	}

	return 0;
}

/*----------------------------------------------------------------------------------------------
	Return the actual glyph ID to use for glyph metrics and output. For most glyphs, this
	is just the glyph ID we're already working with, but for pseudo-glyphs it will something
	different.
----------------------------------------------------------------------------------------------*/
gid16 GrEngine::ActualGlyphForOutput(gid16 chwGlyphID)
{
	gid16 chwActual = gid16(GlyphAttrValue(chwGlyphID, m_chwPseudoAttr));
	if (chwActual == 0)
		return chwGlyphID; // not a pseudo, we're already working with the actual glyph ID
	else
		return chwActual;
}

/*----------------------------------------------------------------------------------------------
	Look up name based on lang id and name id in name table.
	Currently used to look up feature names.
----------------------------------------------------------------------------------------------*/
std::wstring GrEngine::StringFromNameTable(int nLangID, int nNameID)
{
	std::wstring stuName;
	stuName.erase();
	size_t lOffset = 0;
	size_t lSize = 0;

	// The Graphite compiler stores our names in either 
	// the MS (platform id = 3) Unicode (writing system id = 1) table
	// or the MS Symbol (writing system id = 0) table. Try MS Unicode first.
	// lOffset & lSize are in bytes.
	// new interface:
	if (!TtfUtil::GetNameInfo(m_pNameTbl, 3, 1, nLangID, nNameID, lOffset, lSize))
	{
		if (!TtfUtil::GetNameInfo(m_pNameTbl, 3, 0, nLangID, nNameID, lOffset, lSize))
		{
			return stuName;
		}
	}

	size_t cchw = (unsigned(lSize) / sizeof(utf16));
	utf16 * pchwName = new utf16[cchw+1]; // lSize - byte count for Uni str
	const utf16 *pchwSrcName = reinterpret_cast<const utf16*>(m_pNameTbl + lOffset);
    std::transform(pchwSrcName, pchwSrcName + cchw, pchwName, std::ptr_fun<utf16,utf16>(lsbf));
	pchwName[cchw] = 0;  // zero terminate
	#ifdef _WIN32
		stuName.assign((const wchar_t*)pchwName, cchw);
	#else
		wchar_t * pchwName32 = new wchar_t[cchw+1]; // lSize - byte count for Uni str
		for (int i = 0; i <= signed(cchw); i++) {
			pchwName32[i] = pchwName[i];
		}
		stuName.assign(pchwName32, cchw);
		delete [] pchwName32;
	#endif

	delete [] pchwName;
	return stuName;
}

/*----------------------------------------------------------------------------------------------
	Add a feature (eg, when loading from the font).
----------------------------------------------------------------------------------------------*/
void GrEngine::AddFeature(featid nID, int nNameId, int cfset, int nDefault)
{
	if (m_cfeat >= kMaxFeatures)
		return;

	int ifeat = m_cfeat;
	m_rgfeat[ifeat].Initialize(nID, nNameId, cfset, nDefault);
//	m_hmnifeat.Insert(nID, ifeat);
	m_cfeat++;
}

/*----------------------------------------------------------------------------------------------
	Return the feature with the given ID, or NULL if none.
----------------------------------------------------------------------------------------------*/
GrFeature * GrEngine::FeatureWithID(featid nID, int * pifeat)
{
	for (int ifeat = 0; ifeat < m_cfeat; ifeat++)
	{
		if (m_rgfeat[ifeat].ID() == nID)
		{
			*pifeat = ifeat;
			return m_rgfeat + ifeat;
		}
	}
	*pifeat = -1;
	return NULL;


	//	Alternate implementation if we end up with so many features that we need to use
	//	a hash map:
//	int ifeat;
//	if (m_hmnifeat.Retrieve(nID, &ifeat))
//	{
//		Assert(ifeat < kMaxFeatures);
//		return m_rgfeat + ifeat;	// may be NULL
//	}
//	else
//		return NULL;
}

/*----------------------------------------------------------------------------------------------
	Return the default for the feature at the given index in the array.
----------------------------------------------------------------------------------------------*/
int GrEngine::DefaultForFeatureAt(int ifeat)
{
	Assert(ifeat < kMaxFeatures);
	if (ifeat >= m_cfeat || ifeat < 0)
		return 0;	// undefined feature

	return m_rgfeat[ifeat].DefaultValue();
}

/*----------------------------------------------------------------------------------------------
	Set the default for the feature at the given index in the array.
----------------------------------------------------------------------------------------------*/
void GrEngine::SetDefaultForFeatureAt(int ifeat, int nValue)
{
	Assert(ifeat < kMaxFeatures);
	Assert(ifeat < m_cfeat);
	if (ifeat >= 0)
		m_rgfeat[ifeat].SetDefault(nValue);
}

/*----------------------------------------------------------------------------------------------
	Return the default feature settings for the given language.
----------------------------------------------------------------------------------------------*/
void GrEngine::DefaultsForLanguage(isocode lgcode,
	std::vector<featid> & vnFeats, std::vector<int> & vnValues)
{
	m_langtbl.LanguageFeatureSettings(lgcode, vnFeats, vnValues);
}

/*----------------------------------------------------------------------------------------------
	Initialize the directionality and breakweight attributes of a new slot based on its
	glyph ID.
	REVIEW: we could optimize by not initializing from the glyph table right away,
	but waiting until we need the value.
----------------------------------------------------------------------------------------------*/
void GrEngine::InitSlot(GrSlotState * pslot, int nUnicode)
{
	gid16 chwGlyphID = pslot->GlyphID();

	if (m_ptman->InternalJustificationMode() != kjmodiNormal)
	{
		if (m_cJLevels > 0)
		{
			Assert(m_cJLevels == 1); // for now
			//	TODO: do this at level 0.
			pslot->SetJStretch(m_pgtbl->GlyphAttrValue(chwGlyphID, m_chwJStretch0));
			pslot->SetJShrink( m_pgtbl->GlyphAttrValue(chwGlyphID, m_chwJShrink0));
			pslot->SetJStep(   m_pgtbl->GlyphAttrValue(chwGlyphID, m_chwJStep0));
			pslot->SetJWeight( m_pgtbl->GlyphAttrValue(chwGlyphID, m_chwJWeight0));
		}
		else if (nUnicode == knSpace)
		{
			//	Basic stretch/shrink for whitespace.
			//	TODO: do this at level 0.
			Assert(m_fBasicJust);
			int mAdv = pslot->AdvanceX(m_ptman);
			pslot->SetJStretch(mAdv * 100);	// stretch up to 100 times natural width
			pslot->SetJShrink(mAdv / 4);	// shrink to 75%
			pslot->SetJWeight(1);
		}
	}

	if (m_pgtbl && !m_pgtbl->IsEmpty())
	{
		//	Initialize from glyph table.
		pslot->SetBreakWeight(m_pgtbl->GlyphAttrValue(chwGlyphID, m_chwBWAttr));

		DirCode dirc = DirCode(m_pgtbl->GlyphAttrValue(chwGlyphID, m_chwDirAttr));
		if (BidiCode(nUnicode) && (chwGlyphID == 0 || dirc == kdircNeutral))
			// A built-in character for the bidi algorithm that is not defined in the font but
			// must be given a special directionality:
			goto LDefaults;

		pslot->SetDirectionality(dirc);

		return;
	}

LDefaults:
	// Otherwise, in fill defaults for some basic values.
	if (pslot->BreakWeight() == GrSlotState::kNotYetSet8)
	{
		switch (nUnicode)
		{
		case knSpace:
			pslot->SetBreakWeight(klbWordBreak);
			break;
		case knHyphen:
			pslot->SetBreakWeight(klbHyphenBreak);
			break;
		default:
			pslot->SetBreakWeight(klbLetterBreak);
		}
	}

	if ((int)pslot->Directionality() == GrSlotState::kNotYetSet8)
	{
		switch (nUnicode)
		{
		case knSpace:
			pslot->SetDirectionality(kdircWhiteSpace);
			break;
		case knLRM:
			pslot->SetDirectionality(kdircL);
			break;
		case knRLM:
			pslot->SetDirectionality(kdircR);
			break;
		case knLRO:
			pslot->SetDirectionality(kdircLRO);
			break;
		case knRLO:
			pslot->SetDirectionality(kdircRLO);
			break;
		case knLRE:
			pslot->SetDirectionality(kdircLRE);
			break;
		case knRLE:
			pslot->SetDirectionality(kdircRLE);
			break;
		case knPDF:
			pslot->SetDirectionality(kdircPDF);
			break;
		default:
			if (chwGlyphID == LBGlyphID())
				pslot->SetDirectionality(kdircNeutral);
			else
				pslot->SetDirectionality(kdircL);
		}
	}
	else
		Assert(false);	// currently no way to set directionality ahead of time
}

/*----------------------------------------------------------------------------------------------
	We have just changed this slot's glyph ID. Change the directionality and breakweight
	attributes of a new slot based on its glyph ID.
----------------------------------------------------------------------------------------------*/
void GrEngine::SetSlotAttrsFromGlyphAttrs(GrSlotState * pslot)
{
	InitSlot(pslot);
}

/*----------------------------------------------------------------------------------------------
	Create a new segment.
----------------------------------------------------------------------------------------------*/
void GrEngine::NewSegment(Segment ** ppseg)
{
	*ppseg = new Segment;
}

/*----------------------------------------------------------------------------------------------
	Methods to pass on to the tables.
----------------------------------------------------------------------------------------------*/
gid16 GrEngine::GetClassGlyphIDAt(int nClass, int nIndex)
{
	if (nIndex < 0)
		return 0;
	if (nClass < 0)
		return 0;

	try {
		return m_pctbl->GetGlyphID(nClass, nIndex);
	}
	catch (...)
	{
		return 0;
	}
}

int GrEngine::GetIndexInGlyphClass(int nClass, gid16 chwGlyphID)
{
	try {
		return m_pctbl->FindIndex(nClass, chwGlyphID);
	}
	catch (...)
	{
		return -1;
	}
}

size_t GrEngine::NumberOfGlyphsInClass(int nClass)
{
	try {
		return m_pctbl->NumberOfGlyphsInClass(nClass);
	}
	catch (...)
	{
		return 0;
	}
}

int GrEngine::GlyphAttrValue(gid16 chwGlyphID, int nAttrID)
{
	return m_pgtbl->GlyphAttrValue(chwGlyphID, nAttrID);
}

int GrEngine::ComponentIndexForGlyph(gid16 chwGlyphID, int nCompID)
{
	return m_pgtbl->ComponentIndexForGlyph(chwGlyphID, nCompID);
}

//:>********************************************************************************************
//:>	New interface
//:>********************************************************************************************
/*----------------------------------------------------------------------------------------------
	Store the default features values as specified by the client.
----------------------------------------------------------------------------------------------*/
void GrEngine::AssignDefaultFeatures(int cfeat, FeatureSetting * prgfset)
{
	for (int ifeatIn = 0; ifeatIn < cfeat; ifeatIn++)
	{
		int ifeat;
		FeatureWithID(prgfset[ifeatIn].id, &ifeat);
		if (ifeat >= 0 )
			SetDefaultForFeatureAt(ifeat, prgfset[ifeatIn].value);
	}
}

bool GrEngine::LoggingTransduction()
{
	Assert(false);
	//return m_pfface->getLogging();
	//return m_fLogXductn;
	return true;
}

} // namespace gr

//:End Ignore

