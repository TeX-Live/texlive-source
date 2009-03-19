/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrPseudoMap.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    The GrPseudoMap class.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GR_PSEUDOMAP_INCLUDED
#define GR_PSEUDOMAP_INCLUDED

//:End Ignore

namespace gr
{

/*----------------------------------------------------------------------------------------------
	A mapping between a Unicode value and a pseudo-glyph.

	Hungarian: psd
----------------------------------------------------------------------------------------------*/

class GrPseudoMap
{
public:
	unsigned int Unicode()	{ return m_nUnicode; }
	gid16 PseudoGlyph()	{ return m_chwPseudo; }

	void SetUnicode(int n)			{ m_nUnicode = n; }
	void SetPseudoGlyph(gid16 chw)	{ m_chwPseudo = chw; }

protected:
	//	Instance variables:
	unsigned int m_nUnicode;
	gid16 m_chwPseudo;
};

} // namespace gr

#endif // !GR_PSEUDOMAP_INCLUDED
