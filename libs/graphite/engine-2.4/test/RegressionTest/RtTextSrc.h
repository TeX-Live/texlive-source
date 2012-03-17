/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: RtTextSrc.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description: A text-source for the  RegressionTest app.

-------------------------------------------------------------------------------*//*:End Ignore*/
#pragma once
#ifndef RTTXTSRC_INCLUDED
#define RTTXTSRC_INCLUDED

#include <cstring>

/*----------------------------------------------------------------------------------------------
	Class: RtTextSrc
	This class extends the SimpleTextSource to allow setting of features.
----------------------------------------------------------------------------------------------*/
class RtTextSrc : public SimpleTextSrc
{
public:
	RtTextSrc(gr::utf16 * pszText) : SimpleTextSrc(pszText)
	{
		m_fRtl = false;
		memset(m_fset, 0, MAXFEAT * sizeof(FeatureSetting));
	}

	void setFeatures(FeatureSetting * fset)
	{
		m_cFeats = 0;
		for (int i = 0; i < MAXFEAT; i++)
		{
			if (fset[i].id > 0)
			{
				m_fset[i].id = fset[i].id;
				m_fset[i].value = fset[i].value;
				m_cFeats++;
			}
		}
	}

	virtual size_t getFontFeatures(toffset /*ich*/, FeatureSetting * prgfset)
	{
		// Note: size of prgfset buffer = gr::kMaxFeatures = 64
		std::copy(m_fset, m_fset + MAXFEAT, prgfset);
		return m_cFeats;
	}

	virtual bool getRightToLeft(toffset /*ich*/)
	{
		return m_fRtl;
	}
	virtual unsigned int getDirectionDepth(toffset /*ich*/)
	{
		return ((m_fRtl == 1) ? 1 : 0);
	}
	void setRightToLeft(bool f)
	{
		m_fRtl = f;
	}

protected:
	bool m_fRtl;
	int m_cFeats;
	FeatureSetting m_fset[MAXFEAT];
};


#endif // !RTTXTSRC_INCLUDED
