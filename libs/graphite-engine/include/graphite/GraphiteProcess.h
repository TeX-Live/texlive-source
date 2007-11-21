/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GraphiteProcess.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	The interface that is needed for the Justifier object to call back to the Graphite engine.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRAPHITEPROCESS_INCLUDED
#define GRAPHITEPROCESS_INCLUDED

#include "GrResult.h"
//:End Ignore
namespace gr
{

/*----------------------------------------------------------------------------------------------

	Hungarian: fgje
----------------------------------------------------------------------------------------------*/
class GraphiteProcess
{
public:
	virtual ~GraphiteProcess() {};
	virtual GrResult getGlyphAttribute(int iGlyph, int jgat, int nLevel, float * pValueRet) = 0;
	virtual GrResult getGlyphAttribute(int iGlyph, int jgat, int nLevel, int * pValueRet) = 0;
	virtual GrResult setGlyphAttribute(int iGlyph, int jgat, int nLevel, float value) = 0;
	virtual GrResult setGlyphAttribute(int iGlyph, int jgat, int nLevel, int value) = 0;
};

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif  // !IGR_JENGINE_INCLUDED
