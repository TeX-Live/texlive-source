/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: SimpleTextSrc.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	A simple text source that shows how to use this interface within Graphite.
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#pragma hdrstop
// any other headers (not precompiled)

#include "GrClient.h"
#include "GrDebug.h"
#include "ITextSource.h"
#include "SimpleTextSrc.h"

//:>********************************************************************************************
//:>	Initialization and destruction
//:>********************************************************************************************

/*----------------------------------------------------------------------------------------------
	Constructors.
----------------------------------------------------------------------------------------------*/
SimpleTextSrc::SimpleTextSrc(gr::utf16 * pszText)
{
	m_cref = 1; // COM-like behavior

	//m_cchLength = wcslen(pszText); // don't use wcslen, it's not cross-platform
	m_cchLength = 0;
	for (gr::utf16 * pch = pszText; *pch != 0; pch++)
		m_cchLength++;
	m_prgchText = new gr::utf16[m_cchLength + 1];
	std::copy(pszText, pszText + m_cchLength, m_prgchText);
	m_prgchText[m_cchLength] = 0; // zero-terminate
}

/*----------------------------------------------------------------------------------------------
	Destructor.
----------------------------------------------------------------------------------------------*/
SimpleTextSrc::~SimpleTextSrc()
{
	delete[] m_prgchText;
}

//:>********************************************************************************************
//:>	Interface methods
//:>********************************************************************************************
/*----------------------------------------------------------------------------------------------
	Get the specified range of text
----------------------------------------------------------------------------------------------*/
size_t SimpleTextSrc::fetch(toffset ichMin, size_t cch, gr::utf16 * prgchwBuffer)
{
	size_t ichRet = min(cch, size_t(m_cchLength - ichMin));
	std::copy(m_prgchText + ichMin, m_prgchText+ichMin + ichRet, prgchwBuffer);
	return ichRet;
}

/*----------------------------------------------------------------------------------------------
	Return true if the text uses a right-to-left writing system.
----------------------------------------------------------------------------------------------*/
bool SimpleTextSrc::getRightToLeft(toffset /*ich*/)
{
	return false;
}

/*----------------------------------------------------------------------------------------------
	Return the depth of embedding of the writing system.
----------------------------------------------------------------------------------------------*/
unsigned int SimpleTextSrc::getDirectionDepth(toffset /*ich*/)
{
	return 0;
}

/*----------------------------------------------------------------------------------------------
	Return the vertical offset of the text. This simple implementation provides no
	vertical offset.
----------------------------------------------------------------------------------------------*/
float SimpleTextSrc::getVerticalOffset(toffset /*ich*/)
{
	return 0;
}
