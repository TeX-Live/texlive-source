/*--------------------------------------------------------------------
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: FontFace.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Contains the definition of the FontFace class.
	TODO: Merge with GrEngine.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef FONTFACE_INCLUDED
#define FONTFACE_INCLUDED
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
class FontFace
{
	friend class gr::FontMemoryUsage;

public:
	FontFace()
	{
		m_cfonts = 0;
	}

	~FontFace()
	{
		if (s_pFontCache)
			s_pFontCache->RemoveFontFace(m_pgreng->FaceName(), m_pgreng->Bold(), m_pgreng->Italic());
		delete m_pgreng;
	}

	void IncFontCount()
	{
		m_cfonts++;
	}
	void DecFontCount()
	{
		m_cfonts--;
		if (m_cfonts <= 0 && (s_pFontCache == NULL || s_pFontCache->GetFlushMode() == kflushAuto))
			delete this;
	}

	bool NoFonts()
	{
		return (m_cfonts <= 0);
	}

	static FontFace * GetFontFace(Font * pfont,
		std::wstring strFaceName, bool fBold, bool fItalic,
		bool fDumbFallback = false);

	GrResult InitFontFace(Font * pfont,
		std::wstring stuFaceName, bool fBold, bool fItalic,
		bool fDumbLayout);

	FontErrorCode IsValidForGraphite(int * pnVersion, int * pnSubVersion)
	{
		return m_pgreng->IsValidForGraphite(pnVersion, pnSubVersion);
	}

	static void ZapFontCache()
	{
		if (s_pFontCache)
		{
			s_pFontCache->AssertEmpty();
			delete s_pFontCache;
		}
		s_pFontCache = NULL;
	}

	static void SetFlushMode(int flush)
	{
		if (s_pFontCache == NULL)
			s_pFontCache = new FontCache;
		s_pFontCache->SetFlushMode(flush);
	}
	static int GetFlushMode()
	{
		if (s_pFontCache == NULL)
			s_pFontCache = new FontCache;
		return s_pFontCache->GetFlushMode();
	}

	// Temporary, until interface gets thoroughly reworked:
	GrEngine * GraphiteEngine()
	{
		return m_pgreng;
	}

	// Feature access:
	size_t NumberOfFeatures()
	{
		return m_pgreng->NumberOfFeatures_ff();
	}
	featid FeatureID(size_t ifeat)
	{
		return m_pgreng->FeatureID_ff(ifeat);
	}
	size_t FeatureWithID(featid id)
	{
		return m_pgreng->FeatureWithID_ff(id);
	}
	bool GetFeatureLabel(size_t ifeat, lgid language, utf16 * label)
	{
		return m_pgreng->GetFeatureLabel_ff(ifeat, language, label);
	}
	int GetFeatureDefault(size_t ifeat) // index of default setting
	{
		return m_pgreng->GetFeatureDefault_ff(ifeat);
	}
	size_t NumberOfSettings(size_t ifeat)
	{
		return m_pgreng->NumberOfSettings_ff(ifeat);
	}
	int GetFeatureSettingValue(size_t ifeat, size_t ifset)
	{
		return m_pgreng->GetFeatureSettingValue_ff(ifeat, ifset);
	}
	bool GetFeatureSettingLabel(size_t ifeat, size_t ifset, lgid language, utf16 * label)
	{
		return m_pgreng->GetFeatureSettingLabel_ff(ifeat, ifset, language, label);
	}
	// Feature-label language access:
	size_t NumberOfFeatLangs()
	{
		return m_pgreng->NumberOfFeatLangs_ff();
	}
	short FeatLabelLang(int ilang)
	{
		return m_pgreng->GetFeatLabelLang_ff(ilang);
	}
	// Language access:
	size_t NumberOfLanguages()
	{
		return m_pgreng->NumberOfLanguages_ff();
	}
	isocode LanguageCode(size_t ilang)
	{
		return m_pgreng->GetLanguageCode_ff(ilang);
	}

	// Script Direction access:
	ScriptDirCode ScriptDirection() const throw()
	{
		unsigned int script_dirs = 0;
		OLECHAR err_dummy = 0;
		m_pgreng->get_ScriptDirection(&script_dirs, &err_dummy, 1);
		return ScriptDirCode(script_dirs);
	}

	bool BadFont(FontErrorCode * pferr = NULL)
	{
		return m_pgreng->BadFont(pferr);
	}
	bool DumbFallback(FontErrorCode * pferr = NULL)
	{
		return m_pgreng->DumbFallback(pferr);
	}

public:
	// For use in segment creation:
	void RenderLineFillSegment(Segment * pseg, Font * pfont, ITextSource * pts,
		LayoutEnvironment & layout,
		toffset ichStart, toffset ichStop, float xsMaxWidth, bool fBacktracking)
	{
		m_pgreng->MakeSegment(pseg, pfont, pts, NULL, layout,
			ichStart, ichStop, xsMaxWidth, fBacktracking, false, 0, kestMoreLines);
	}
	void RenderRangeSegment(Segment * pseg, Font * pfont,
		ITextSource * pts, LayoutEnvironment & layout,
		toffset ichStart, toffset ichStop)
	{
		m_pgreng->MakeSegment(pseg, pfont, pts, NULL, layout,
			ichStart, ichStop, kPosInfFloat, false, false, 0, kestMoreLines);
	}
	void RenderJustifiedSegment(Segment * pseg, Font * pfont,
		ITextSource * pts, LayoutEnvironment & layout,
		toffset ichStart, toffset ichStop, float xsCurrentWidth, float xsDesiredWidth)
	{
		m_pgreng->MakeSegment(pseg, pfont, pts, NULL, layout,
			ichStart, ichStop, xsCurrentWidth, false, true, xsDesiredWidth, kestMoreLines);
	}

	// Debugging.
	//static bool DbgCheckFontCache()
	//{
	//	if (s_pFontCache)
	//		return s_pFontCache->DbgCheckFontCache();
	//	else
	//		return true;
	//}
	//void DbgCheckFontFace();

	static void calculateAllMemoryUsage(FontMemoryUsage & fmu);
	void calculateMemoryUsage(FontMemoryUsage & fmu);

protected:
	// Number of fonts in existence that use this face; when it goes to zero, delete.
	int m_cfonts;

	// Static variable:
	static FontCache * s_pFontCache;

	// Member variables:
	GrEngine * m_pgreng;
};

} // namespace gr


#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif


#endif  // !FONTFACE_INCLUDED
