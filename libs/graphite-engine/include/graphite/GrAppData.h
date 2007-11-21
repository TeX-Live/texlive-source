/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrAppData.h
Responsibility: Sharon Correll
Last reviewed: not yet

Description:
    Data structures need by applications that use the Graphite engine.
----------------------------------------------------------------------------------------------*/


#ifndef GRAPPDATA_INCLUDED
#define GRAPPDATA_INCLUDED

#include "GrData.h"

namespace gr 
{

typedef unsigned int featid;		// font feature IDs
typedef unsigned int lgid;			// language ID (for access feature UI strings)
typedef unsigned int toffset;		// text-source index

typedef struct {		// ISO-639-3 language code (for mapping onto features)
	char rgch[4];
} isocode;

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif // GRAPPDATA_INCLUDED

