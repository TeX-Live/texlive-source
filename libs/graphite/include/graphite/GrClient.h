#ifndef GR_CLIENT_H
#define GR_CLIENT_H

#include "GrCommon.h"
#include "GrConstants.h"

#define __RPC_FAR

namespace gr
{

// These are internal classes, whose forward declarations are needed to allow the public interfaces
//   to compile but are not intended for client use.
class GrSlotState;
class GrSlotStream;
class GrGlyphTable;
class GrClassTable;
class GrTableManager;
class GrPseudoMap;
class GrFileIStream;
class GrSlotOutput;
class GrCharStream;
class GrIStream;

// Forward declarations of client APIs and classes.
class Segment;
}

#include "GrAppData.h"
#include "GrResult.h"
#include "GrFeature.h"


#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif //GR_CLIENT_H

