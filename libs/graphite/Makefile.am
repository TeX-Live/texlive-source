## Makefile.am for the TeX Live subdirectory libs/graphite/
##
## Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.
##
ACLOCAL_AMFLAGS = -I ../../m4

INCLUDES = -I$(top_srcdir)/include/graphite -I$(top_srcdir)/src/generic
INCLUDES += -I$(top_srcdir)/src/font -I$(top_srcdir)/src/segment 
AM_CPPFLAGS = -DNDEBUG

noinst_LIBRARIES = libgraphite.a

noinst_HEADERS = \
	include/graphite/FileFont.h \
	include/graphite/Font.h \
	include/graphite/GrAppData.h \
	include/graphite/GrClient.h \
	include/graphite/GrCommon.h \
	include/graphite/GrConstants.h \
	include/graphite/GrData.h \
	include/graphite/GrDebug.h \
	include/graphite/GrExt.h \
	include/graphite/GrFeature.h \
	include/graphite/GrMstypes.h \
	include/graphite/GrPlatform.h \
	include/graphite/GrResult.h \
	include/graphite/GrStructs.h \
	include/graphite/GrWrappers.h \
	include/graphite/GraphiteProcess.h \
	include/graphite/IGrEngine.h \
	include/graphite/IGrJustifier.h \
	include/graphite/IGraphiteProcess.h \
	include/graphite/ITextSource.h \
	include/graphite/MemoryUsage.h \
	include/graphite/Segment.h \
	include/graphite/SegmentAux.h \
	include/graphite/SegmentPainter.h

EXTRA_DIST = \
	build.bat \
	build_d.bat \
	debian \
	doc \
	installed-top.pc.in \
	license \
	makefile.vc \
	silgraphite.pc.in \
	test \
	uninstalled-top.pc.in

# in case of an SVN repository
dist-hook:
	rm -rf `find $(distdir) -name .svn`

libgraphite_a_SOURCES = \
	src/font/FileFont.cpp \
	src/font/Font.cpp \
	src/font/FontTableCache.h \
	src/font/TtfTypes.h \
	src/font/TtfUtil.cpp \
	src/font/TtfUtil.h \
	src/generic/BinTree.h \
	src/generic/Debug.cpp \
	src/generic/GenericResource.h \
	src/generic/HashMap.h \
	src/generic/Util.cpp \
	src/painter/SegmentPainter.cpp \
	src/segment/FileInput.cpp \
	src/segment/FileInput.h \
	src/segment/FontCache.cpp \
	src/segment/FontCache.h \
	src/segment/FontFace.cpp \
	src/segment/FontFace.h \
	src/segment/GrCharStream.cpp \
	src/segment/GrCharStream.h \
	src/segment/GrClassTable.cpp \
	src/segment/GrClassTable.h \
	src/segment/GrEngine.cpp \
	src/segment/GrEngine.h \
	src/segment/GrFSM.cpp \
	src/segment/GrFSM.h \
	src/segment/GrFeature.cpp \
	src/segment/GrFeatureValues.h \
	src/segment/GrGlyphTable.cpp \
	src/segment/GrGlyphTable.h \
	src/segment/GrPass.cpp \
	src/segment/GrPass.h \
	src/segment/GrPassActionCode.cpp \
	src/segment/GrPseudoMap.h \
	src/segment/GrSlotState.cpp \
	src/segment/GrSlotState.h \
	src/segment/GrSlotStream.cpp \
	src/segment/GrSlotStream.h \
	src/segment/GrTableManager.cpp \
	src/segment/GrTableManager.h \
	src/segment/Main.h \
	src/segment/MemoryUsage.cpp \
	src/segment/Platform.cpp \
	src/segment/Segment.cpp \
	src/segment/SegmentAux.cpp \
	src/segment/TestFSM.cpp \
	src/segment/TestPasses.cpp \
	src/segment/TransductionLog.cpp \
	src/segment/resource.h

## Not used
##
EXTRA_DIST += \
	include/Makefile.am \
	include/graphite/BinTree.h \
	include/graphite/GenericResource.h \
	include/graphite/HashMap.h \
	src/Makefile.am \
	src/Makefile.in \
	src/font/Makefile.am \
	src/generic/BinTree_i.cpp \
	src/generic/HashMap_i.cpp \
	src/generic/Makefile.am \
	src/painter/Makefile.am \
	src/segment/Makefile.am \
	src/segment/XmlTransductionLog.cpp

## Original files
##
EXTRA_DIST += Makefile.am.orig configure.ac.orig

