/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2005 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: SegmentAux.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Implements auxiliary class that work with Segment:
	- GlyphInfo
	- GlyphIterator
	- LayoutEnvironment
-------------------------------------------------------------------------------*//*:End Ignore*/

//:>********************************************************************************************
//:>	   Include files
//:>********************************************************************************************
#include "Main.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif
// any other headers (not precompiled)
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

namespace gr
{

//:>********************************************************************************************
//:>	GlyphInfo methods
//:>********************************************************************************************

gid16 GlyphInfo::glyphID()
{
	return m_pslout->m_chwActual;
}

gid16 GlyphInfo::pseudoGlyphID()
{
	if (m_pslout->m_chwActual == m_pslout->m_chwGlyphID)
		return 0; // not a pseudo-glyph
	return m_pslout->m_chwGlyphID;
}

bool GlyphInfo::isAttached() const throw()
{
	return m_pslout->IsPartOfCluster();
}

gr::GlyphIterator GlyphInfo::attachedClusterBase() const throw()
{
	const int islout = m_pslout->ClusterBase();

	// Since we are not passing an array, the constructor expects a ginf index, not a slout index.
	return GlyphIterator(*m_pseg, ((islout >= 0) ? islout : m_islout) - m_pseg->m_isloutGinf0);
}

float GlyphInfo::attachedClusterAdvance() const throw()
{
	return m_pslout->ClusterAdvance();
}

std::pair<gr::GlyphSetIterator, gr::GlyphSetIterator> GlyphInfo::attachedClusterGlyphs() const
{
	std::vector<int> visloutClusterMembers;
	m_pslout->ClusterMembers(m_pseg, m_islout, visloutClusterMembers);
	RcVector * qvislout = new RcVector(visloutClusterMembers);
	return std::make_pair(
		GlyphSetIterator(*m_pseg, 0, qvislout), 
		GlyphSetIterator(*m_pseg, visloutClusterMembers.size(), qvislout));
}

size_t GlyphInfo::logicalIndex()
{
	if (m_pseg->OutputSlot(0)->IsLineBreak())
		return m_islout - 1;
	else
		return m_islout;
}

float GlyphInfo::origin()
{
	return m_pslout->XPosition();
}

float GlyphInfo::advanceWidth()		// logical units
{
	return m_pslout->m_xsAdvanceX;
}

float GlyphInfo::advanceHeight()	// logical units; zero for horizontal fonts
{
	Font & font = m_pseg->getFont();
	return m_pslout->GlyphMetricLogUnits(&font, kgmetAdvHeight);
}

float GlyphInfo::yOffset()
{
	return m_pslout->YPosition();
}

Rect GlyphInfo::bb()				// logical units
{
	return m_pslout->BoundingBox(m_pseg->getFont());
}

bool GlyphInfo::isSpace()
{
	return m_pslout->IsSpace();
}

// first char associated with this glyph, relative to start of the text-source
toffset GlyphInfo::firstChar()
{
	return m_pseg->LogicalSurfaceToUnderlying(m_islout, true);
}

// last char associated with this glyph, relative to start of the text-source
toffset GlyphInfo::lastChar()
{
	return m_pseg->LogicalSurfaceToUnderlying(m_islout, false);
}

unsigned int GlyphInfo::directionality()
{
	return m_pslout->Directionality();
}

// Embedding depth
unsigned int GlyphInfo::directionLevel()
{
	return m_pslout->DirLevel();
}

bool GlyphInfo::insertBefore()
{
	return m_pslout->InsertBefore();
}

int GlyphInfo::breakweight()
{
	return m_pslout->BreakWeight();
}

float GlyphInfo::maxStretch(size_t level)
{
	return m_pslout->MaxStretch(m_pseg, (int)level);
}

float GlyphInfo::maxShrink(size_t level)
{
	return m_pslout->MaxShrink(m_pseg, (int)level);
}

float GlyphInfo::stretchStep(size_t level)
{
	return m_pslout->StretchStep(m_pseg, (int)level);
}

byte GlyphInfo::justWeight(size_t level)
{
	return byte(m_pslout->JustWeight((int)level));
}

float GlyphInfo::justWidth(size_t level)
{
	return m_pslout->JustWidth(m_pseg, (int)level);
}

float GlyphInfo::measureStartOfLine()
{
	return m_pslout->MeasureSolLogUnits(m_pseg);
}

float GlyphInfo::measureEndOfLine()
{
	return m_pslout->MeasureEolLogUnits(m_pseg);
}

size_t GlyphInfo::numberOfComponents()
{
	return m_pslout->NumberOfComponents();
}

Rect GlyphInfo::componentBox(size_t icomp)
{
	return m_pseg->ComponentRect(m_pslout, icomp);
}

toffset GlyphInfo::componentFirstChar(size_t icomp)
{
	Assert((int)icomp < m_pslout->NumberOfComponents());
	return m_pslout->FirstUnderlyingComponent(icomp) + m_pseg->startCharacter();
}
toffset GlyphInfo::componentLastChar(size_t icomp)
{
	Assert((int)icomp < m_pslout->NumberOfComponents());
	return m_pslout->LastUnderlyingComponent(icomp) + m_pseg->startCharacter();
}

bool GlyphInfo::erroneous()
{
	return m_pseg->Erroneous();
}


//:>********************************************************************************************
//:>	GlyphIterator methods
//:>********************************************************************************************

// Constructor
GlyphIterator::GlyphIterator(Segment & seg, size_t iginf)
: m_pginf(seg.m_prgginf + iginf)
{}

// Copy constructor.
// Check if the incoming iterator is null, as we cannot dereference
//  it to get the segment, and create a null GlyphIterator.
//  Technically this might better be an assert as converting a null
//  GlyphSetIterator into a null GlyphIterator is pointless and suggests
//  some kind of error.
GlyphIterator::GlyphIterator(const GlyphSetIterator & sit)
: m_pginf((sit == GlyphSetIterator())
			? 0 
			: sit->segment().m_prgginf + sit->logicalIndex())
{}
 
/*----------------------------------------------------------------------------------------------
 Dereference the iterator, returning a GlyphInfo object.
----------------------------------------------------------------------------------------------*/

GlyphSetIterator::reference GlyphSetIterator::operator*() const
{
	assert(m_pseg != 0);
	assert(m_vit != std::vector<int>::const_iterator());
														// in the case of a non-contiguous list
	return m_pseg->m_prgginf[(*m_vit) - m_pseg->m_isloutGinf0];
}


} // namespace gr
