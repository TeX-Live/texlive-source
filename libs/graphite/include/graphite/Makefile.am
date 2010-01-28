## Proxy Makefile.am to install silgraphite headers for TeX Live.
##
##   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
GRAPHITE_SRC = $(top_srcdir)/$(GRAPHITE_TREE)/include/graphite

GRAPHITE_HDRS = \
	$(GRAPHITE_SRC)/FileFont.h \
	$(GRAPHITE_SRC)/Font.h \
	$(GRAPHITE_SRC)/GrAppData.h \
	$(GRAPHITE_SRC)/GrClient.h \
	$(GRAPHITE_SRC)/GrCommon.h \
	$(GRAPHITE_SRC)/GrConstants.h \
	$(GRAPHITE_SRC)/GrData.h \
	$(GRAPHITE_SRC)/GrDebug.h \
	$(GRAPHITE_SRC)/GrExt.h \
	$(GRAPHITE_SRC)/GrFeature.h \
	$(GRAPHITE_SRC)/GrMstypes.h \
	$(GRAPHITE_SRC)/GrPlatform.h \
	$(GRAPHITE_SRC)/GrResult.h \
	$(GRAPHITE_SRC)/GrStructs.h \
	$(GRAPHITE_SRC)/GrWrappers.h \
	$(GRAPHITE_SRC)/GraphiteProcess.h \
	$(GRAPHITE_SRC)/IGrEngine.h \
	$(GRAPHITE_SRC)/IGrJustifier.h \
	$(GRAPHITE_SRC)/IGraphiteProcess.h \
	$(GRAPHITE_SRC)/ITextSource.h \
	$(GRAPHITE_SRC)/MemoryUsage.h \
	$(GRAPHITE_SRC)/Segment.h \
	$(GRAPHITE_SRC)/SegmentAux.h \
	$(GRAPHITE_SRC)/SegmentPainter.h

all-local:
	@for file in $(GRAPHITE_HDRS); do \
	  test -f $$file || continue; \
	  inst=`echo $$file | sed -e 's/^.*\///'`; \
	  test -f $$inst || { \
	    echo "$(LN_S) $$file $$inst"; \
	    $(LN_S) $$file $$inst; } || exit 1; \
	done

distclean-local:
	rm -f *.h



