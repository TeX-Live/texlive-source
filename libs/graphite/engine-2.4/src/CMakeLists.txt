CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)
project(graphite)
INCLUDE(CheckTypeSize)
INCLUDE(CheckCXXSourceCompiles)

set(GRAPHITE_API_MAJOR 3)
set(GRAPHITE_API_MINOR 0)
set(GRAPHITE_API_PATCH 0)
set(GRAPHITE_SO_VERSION ${GRAPHITE_API_MAJOR}.${GRAPHITE_API_MINOR}.${GRAPHITE_API_PATCH})

include_directories(${PROJECT_SOURCE_DIR}/../include/graphite)
include_directories( ${PROJECT_SOURCE_DIR}/font ${PROJECT_SOURCE_DIR}/segment)

# check for fabsf definition
set(FABSF_TEST_SRC "#include <cmath>\nint main() { fabsf(1.2); }")
CHECK_CXX_SOURCE_COMPILES("${FABSF_TEST_SRC}" HAVE_FABSF)
if (HAVE_FABSF)
add_definitions(-DHAVE_FABSF -DTRACING)
endif (HAVE_FABSF)

#check sizeof wchar_t
CHECK_TYPE_SIZE(wchar_t SIZEOF_WCHAR_T)
add_definitions(-DSIZEOF_WCHAR_T=${SIZEOF_WCHAR_T})

set(GRAPHITE_HEADERS 
	../include/graphite/BinTree.h
	../include/graphite/FileFont.h
	../include/graphite/Font.h
	../include/graphite/GenericResource.h
	../include/graphite/GraphiteProcess.h
	../include/graphite/GrAppData.h
	../include/graphite/GrClient.h
	../include/graphite/GrCommon.h
	../include/graphite/GrConstants.h
	../include/graphite/GrData.h
	../include/graphite/GrDebug.h
	../include/graphite/GrExt.h
	../include/graphite/GrFeature.h
	../include/graphite/GrMstypes.h
	../include/graphite/GrPlatform.h
	../include/graphite/GrResult.h
	../include/graphite/GrStructs.h
	../include/graphite/GrWrappers.h
	../include/graphite/HashMap.h
	../include/graphite/IGraphiteProcess.h
	../include/graphite/IGrEngine.h
	../include/graphite/IGrJustifier.h
	../include/graphite/ITextSource.h
	../include/graphite/MemoryUsage.h
	../include/graphite/SegmentAux.h
	../include/graphite/Segment.h
	../include/graphite/SegmentPainter.h)

add_library(graphite
	font/FileFont.cpp
	font/Font.cpp
	font/TtfUtil.cpp
#	generic/BinTree_i.cpp
	generic/Debug.cpp
#	generic/HashMap_i.cpp
	generic/Util.cpp
	painter/SegmentPainter.cpp
	segment/FileInput.cpp
	segment/FontCache.cpp
	segment/FontFace.cpp
	segment/GrCharStream.cpp
	segment/GrClassTable.cpp
	segment/GrEngine.cpp
	segment/GrFSM.cpp
	segment/GrFeature.cpp
	segment/GrGlyphTable.cpp
	segment/GrPass.cpp
	segment/GrPassActionCode.cpp
	segment/GrSlotState.cpp
	segment/GrSlotStream.cpp
	segment/GrTableManager.cpp
	segment/MemoryUsage.cpp
	segment/Platform.cpp
	segment/Segment.cpp
	segment/SegmentAux.cpp
#	segment/TestFSM.cpp
#	segment/TestPasses.cpp
	segment/TransductionLog.cpp
	${GRAPHITE_HEADERS}
	)



set_target_properties(graphite PROPERTIES PUBLIC_HEADER "${GRAPHITE_HEADERS}")

set_target_properties(graphite PROPERTIES SOVERSION ${GRAPHITE_SO_VERSION})

install(TARGETS graphite EXPORT graphite LIBRARY DESTINATION lib ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include/graphite)
install(EXPORT graphite DESTINATION lib/graphite NAMESPACE gr_)
