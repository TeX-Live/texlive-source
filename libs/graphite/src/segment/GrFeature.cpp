/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrFeature.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Implements the GrFeature class.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "Main.h"
#include <cstring>

#ifdef _MSC_VER
#pragma hdrstop
#endif
#undef THIS_FILE
DEFINE_THIS_FILE

//:>********************************************************************************************
//:>	Forward declarations
//:>********************************************************************************************

//:>********************************************************************************************
//:>	Local Constants and static variables
//:>********************************************************************************************

namespace gr
{

//:>********************************************************************************************
//:>	GrFeature Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Initialize the feature.
----------------------------------------------------------------------------------------------*/	
void GrFeature::Initialize(featid nID, int nNameId, int cfset, int nDefault)
{
	m_nID = nID;
	m_nNameId = nNameId;
	m_nDefault = nDefault;

	m_vnVal.resize(cfset);
	for (unsigned int ifset = 0; ifset < m_vnVal.size(); ifset++)
		m_vnVal[ifset] = INT_MAX;
	m_vnNameId.resize(cfset);
}

/*----------------------------------------------------------------------------------------------
	Return the feature settings.
----------------------------------------------------------------------------------------------*/
int GrFeature::Settings(int cMax, int * prgnVal)
{
	int cRet = min(cMax, signed(m_vnVal.size()));
	for (int ifset = 0; ifset < cRet; ifset++)
		prgnVal[ifset] = m_vnVal[ifset];
	return cRet;
}

/*----------------------------------------------------------------------------------------------
	Add a feature setting.
----------------------------------------------------------------------------------------------*/
void GrFeature::AddSetting(int nVal, int nNameId)
{
	Assert(nVal != INT_MAX);
	unsigned int ifset;
	for (ifset = 0; ifset < m_vnVal.size(); ifset++)
	{
		if (m_vnVal[ifset] == nVal)
			return; // already there
	}
	for (ifset = 0; ifset < m_vnVal.size(); ifset++)
	{
		if (m_vnVal[ifset] == INT_MAX)
		{
			m_vnVal[ifset] = nVal;
			m_vnNameId[ifset] = nNameId;
			return;
		}
	}
	m_vnVal.push_back(nVal);
	m_vnNameId.push_back(nNameId);
}

/*----------------------------------------------------------------------------------------------
	Return true if the given setting is valid according to the font's feature table.
----------------------------------------------------------------------------------------------*/
bool GrFeature::IsValidSetting(int nVal)
{
	for (unsigned int ifset = 0; ifset < m_vnVal.size(); ifset++)
	{
		if (m_vnVal[ifset] == nVal)
			return true;
	}
	return false;
}

/*----------------------------------------------------------------------------------------------
	Read the feature label from the name table.
----------------------------------------------------------------------------------------------*/
std::wstring GrFeature::Label(GrEngine * pgreng, int nLang)
{
	std::wstring stu = pgreng->StringFromNameTable(nLang, m_nNameId);
	if (stu == L"NoName")
		stu.erase();
	return stu;
}

/*----------------------------------------------------------------------------------------------
	Read the feature value label from the name table.
----------------------------------------------------------------------------------------------*/
std::wstring GrFeature::SettingLabel(GrEngine * pgreng, int nVal, int nLang)
{
	for (unsigned int ifset = 0; ifset < m_vnVal.size(); ifset++)
	{
		if (m_vnVal[ifset] == nVal)
		{
			std::wstring stu = pgreng->StringFromNameTable(nLang, m_vnNameId[ifset]);
			if (stu == L"NoName")
				stu.erase();
			return stu;
		}
	}
	Assert(false); // setting is not valid
	return L"";
}

/*----------------------------------------------------------------------------------------------
	Return the given setting value.
----------------------------------------------------------------------------------------------*/
int GrFeature::NthSetting(int ifset)
{
	if (ifset >= (int)m_vnVal.size())
		return -1;
	else
		return m_vnVal[ifset];
}

/*----------------------------------------------------------------------------------------------
	Read the feature value label for the given index.
----------------------------------------------------------------------------------------------*/
std::wstring GrFeature::NthSettingLabel(GrEngine * pgreng, int ifset, int nLang)
{
	std::wstring stu;

	if (ifset >= (int)m_vnVal.size())
		stu.erase();
	else
	{
        stu = pgreng->StringFromNameTable(nLang, m_vnNameId[ifset]);
		if (stu == L"NoName")
			stu.erase();
	}
	return stu;
}

/*----------------------------------------------------------------------------------------------
	Return the feature setting label.
----------------------------------------------------------------------------------------------*/
//	void GrFeature::SetSettingLabel(int nVal, std::wstring stuLabel, int nLang)
//	{
//		for (int ifset = 0; ifset < m_vfset.size(); ifset++)
//		{
//			if (m_vfset[ifset].m_nVal == nVal)
//			{
//				m_vfset[ifset].m_hmnstuLabels.Insert(nLang, stuLabel, true);
//				return;
//			}			
//		}
//		Assert(false); // setting is not valid
//	}


//:>********************************************************************************************
//:>	GrLangTable Methods
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Read the languages from the font.
----------------------------------------------------------------------------------------------*/
bool GrLangTable::ReadFromFont(GrIStream * pgrstrm, int fxdVersion)
{
	GrIStream & grstrm = *pgrstrm;
	
	// number of languages
	m_clang = (size_t)grstrm.ReadUShortFromFont();

	// search constants
	m_dilangInit = grstrm.ReadUShortFromFont();
	m_cLoop = grstrm.ReadUShortFromFont();
	m_ilangStart = grstrm.ReadUShortFromFont();

	// Slurp the data into the buffers. The "+1" represents a bogus ending entry that quickly
	// gives us the size of the feature list. Note that the resulting data is all big-endian.
	int cb = (m_clang + 1) * sizeof(LangEntry);
	m_prglang = new LangEntry[m_clang + 1];
	grstrm.ReadBlockFromFont(m_prglang, cb);

	m_cbOffset0 = (lsbf)(m_prglang[0].cbOffsetBIG);

	Assert((lsbf)(m_prglang[m_clang].cFeaturesBIG) == 0); // bogus entry has no settings
	cb = (lsbf)(m_prglang[m_clang].cbOffsetBIG) - m_cbOffset0;
	Assert(cb % sizeof(FeatSet) == 0); // # of bytes fits nicely into FeatSet class
	int cfset = cb / sizeof(FeatSet);
	m_prgfset = new FeatSet[cfset];
	m_cfset = cfset;
	grstrm.ReadBlockFromFont(m_prgfset, cb);

	return true;
}

/*----------------------------------------------------------------------------------------------
	Create an empty language table to use for dumb rendering, or when there is no valid
	Sill table.
----------------------------------------------------------------------------------------------*/
void GrLangTable::CreateEmpty()
{
	m_prglang = NULL;
	m_prgfset = NULL;
	m_clang = 0;
	m_dilangInit = 0;
	m_cLoop = 0;
	m_ilangStart = 0;
}

/*----------------------------------------------------------------------------------------------
	Return information about the language with the given ID by filling in the given vectors
	with the feature ids and values. Will wipe out any data in the vectors already.
----------------------------------------------------------------------------------------------*/
void GrLangTable::LanguageFeatureSettings(isocode lgcode,
	std::vector<featid> & vnFeatId, std::vector<int> & vnValues)
{
	vnFeatId.clear();
	vnValues.clear();

	int ilang = FindIndex(lgcode);
	if (ilang == -1)
		return;	// no such language: leave vectors empty

	LangEntry * plang = m_prglang + ilang;
	int cbOffset = lsbf(plang->cbOffsetBIG) - m_cbOffset0;
	Assert(cbOffset % sizeof(FeatSet) == 0);
	byte * pbFeatSet = reinterpret_cast<byte*>(m_prgfset) + cbOffset;
	FeatSet * pfset = reinterpret_cast<FeatSet*>(pbFeatSet);
	for (int ifset = 0; ifset < lsbf(plang->cFeaturesBIG); ifset++)
	{
		vnFeatId.push_back(lsbf(pfset[ifset].featidBIG));
		vnValues.push_back(lsbf(pfset[ifset].valueBIG));
	}
}

/*----------------------------------------------------------------------------------------------
	Return the code for the language at the given index.
----------------------------------------------------------------------------------------------*/
isocode GrLangTable::LanguageCode(size_t ilang)
{
	isocode lgcodeRet;
	if (ilang > m_clang)
	{
		std::fill_n(lgcodeRet.rgch, 4, '\0');
		return lgcodeRet;
	}
	LangEntry * plang = m_prglang + ilang;
	std::copy(plang->rgchCode, plang->rgchCode + 4, lgcodeRet.rgch);
	return lgcodeRet;
}

/*----------------------------------------------------------------------------------------------
	Return the index of the language in the list.
----------------------------------------------------------------------------------------------*/
int GrLangTable::FindIndex(isocode lgcode)
{
	if (m_clang == 0)
		return -1;
#ifdef _DEBUG
	int nPowerOf2 = 1;
	while (nPowerOf2 <= signed(m_clang))
		nPowerOf2 <<= 1;
	nPowerOf2 >>= 1;
	//	Now nPowerOf2 is the max power of 2 <= m_clang
	gAssert((1 << m_cLoop) == nPowerOf2);		// m_cLoop == log2(nPowerOf2)
	gAssert(m_dilangInit == nPowerOf2);
	gAssert(m_ilangStart == m_clang - m_dilangInit);
#endif // _DEBUG

	int dilangCurr = m_dilangInit;

	int ilangCurr = m_ilangStart; // may become < 0
	while (dilangCurr > 0) 
	{
		int nTest;
		if (ilangCurr < 0)
			nTest = -1;
		else
		{
			LangEntry * plangCurr = m_prglang + ilangCurr;
			nTest = strcmp(plangCurr->rgchCode, lgcode.rgch);
		}

		if (nTest == 0)
			return ilangCurr;

		dilangCurr >>= 1;	// divide by 2
		if (nTest < 0)
			ilangCurr += dilangCurr;
		else // (nTest > 0)
			ilangCurr -= dilangCurr;
	}

	return -1;
}

} // namespace gr
