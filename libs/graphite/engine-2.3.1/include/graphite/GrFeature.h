/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrFeature.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:

Note:
	The body of methods not complete contained in this file are located in GrEngine.cpp.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_FEATURE_INCLUDED
#define GR_FEATURE_INCLUDED

//:End Ignore

#include "GrAppData.h"

namespace gr
{


/*----------------------------------------------------------------------------------------------
	A valid setting and its UI labels in potentially various languages.

	Hungarian: fset
----------------------------------------------------------------------------------------------*/
/****
class GrFeatureSetting
{
	friend class GrFeature;
public:
	GrFeatureSetting()
	{
		m_hmnstuLabels.Clear();
	}
	GrFeatureSetting(GrFeatureSetting & fset)
	{
		m_hmnstuLabels.Clear();
	}
protected:
	int m_nVal;
	HashMap<int, std::wstring> m_hmnstuLabels;
};
****/

class GrEngine;

/*----------------------------------------------------------------------------------------------
	Each Graphite feature defines a way to customize or parameterize the rendering processs.

	Hungarian: feat
----------------------------------------------------------------------------------------------*/
class GrFeature
{
	//friend class GrFeatureSetting;
public:
	enum {
		knLangFeatV2 = 1
	};

	GrFeature()
	{
		m_vnVal.clear();
		m_vnNameId.clear();
	}

	void Initialize(featid nID, int nNameId, int cfset, int nDefault = 0);

	featid ID()
	{
		return m_nID;
	}

	int DefaultValue()
	{
		return m_nDefault;
	}

	void SetDefault(int nDefault)
	{
		m_nDefault = nDefault;
	}

	int NameId()
	{
		return m_nNameId;
	}

	void SetNameId(int n)
	{
		m_nNameId = n;
	}

	int NumberOfSettings()
	{
		return (int)m_vnVal.size();
	}

	int Settings(int cMax, int * prgnVal);
	void AddSetting(int nVal, int nNameId);
	bool IsValidSetting(int nVal);
	std::wstring Label(GrEngine * pgreng, int nLang);
	std::wstring SettingLabel(GrEngine * pgreng, int nVal, int nLang);
	int NthSetting(int ifset);
	std::wstring NthSettingLabel(GrEngine * pgreng, int ifset, int nLang);
//	void SetSettingLabel(int nVal, std::wstring stuLabel, int nLang);

protected:
	featid m_nID;
	int m_nDefault;
	int m_nNameId;
//	HashMap<int, std::wstrng> m_hmnstuLabels;

	// These are parallel arrays. The first gives the setting value for a given feature,
	// the second gives the index into the name table that is used to read UI strings.
	std::vector<int> m_vnVal;
	std::vector<int> m_vnNameId;
};


/*----------------------------------------------------------------------------------------------
	GrLangTable handles looking up language information.
	GrEngine has one instance of this class.
	Hungarian: langtbl
----------------------------------------------------------------------------------------------*/
class GrIStream;

class GrLangTable
{
	friend class FontMemoryUsage;

public:
	GrLangTable()
	{
		m_prglang = NULL;
		m_clang = 0;
		m_prgfset = NULL;
	}
	~GrLangTable()
	{
		delete[] m_prglang;
		delete[] m_prgfset;
	}

	bool ReadFromFont(GrIStream * pgrstrm, int fxdVersion);
	void CreateEmpty();

	size_t NumberOfLanguages()
	{
		return m_clang;
	}
	void LanguageFeatureSettings(isocode lgcode,
		std::vector<featid> & vnFeatId, std::vector<int> & vnValues);
	isocode LanguageCode(size_t ilang);

protected:
	size_t m_clang;
	//	constants for fast binary search of language list
	data16	m_dilangInit;	// (max power of 2 <= m_clang);
							//		size of initial range to consider
	data16	m_cLoop;		// log2(max power of 2 <= m_clang);
							//		indicates how many iterations are necessary
	data16	m_ilangStart;	// m_clang - m_dilangInit;
							//		where to start search

	struct LangEntry
	{
		char rgchCode[4];
		data16 cFeaturesBIG;	// big endian
		data16 cbOffsetBIG;		// big endian
	};
	LangEntry * m_prglang;
	int m_cbOffset0; // offset of first entry

	struct FeatSet
	{
		featid featidBIG;	// big endian
		data16 valueBIG;	// big endian
		data16 padBIG;
	};
	FeatSet * m_prgfset;
	int m_cfset;	// needed for memory instrumentation only

	int FindIndex(isocode lgcode);
};

} // namespace gr


#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif


#endif // !GR_FEATURE_INCLUDED
