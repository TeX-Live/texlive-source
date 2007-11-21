/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: explicit_instantiation.cpp
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Explicitly instantiate HashMap and Vector types used in the GrEngine module.
-------------------------------------------------------------------------------*//*:End Ignore*/

// Standard includes for the module, gives the types we have to instantiate
#include "Main.h"
#pragma hdrstop

// Standard includes for doing explicit instantiation:

//////#include "Vector_i.cpp"

namespace gr
{

// Types we use:
template class std::vector<int>;
// STL port doesn't like this
//template class std::vector<bool>;
template class std::vector<GrSlotState *>;
template class std::vector<GrGlyphSubTable *>;
template class std::vector<int *>;
/////template std::vector<std::wstring>;  // remove
template class std::vector<OLECHAR>;
template class std::vector<byte>;
template class std::vector<DirCode>;

// VS6.0 doesn't like these:
//template std::vector<RECT>;
//template std::vector<Segment::GlyphStrmKey>;
//template std::vector<Segment::LineSeg>;
//template std::vector<GrGlyphIndexPair>;

} // namespace gr
