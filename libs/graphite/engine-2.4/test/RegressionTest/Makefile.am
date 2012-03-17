AUTOMAKE_OPTIONS = 1.6

EXTRA_DIST = \
	burmeseText.wpx \
	romanText.wpx \
	xlineText.wpx \
	grtest_arabic.ttf \
	grtest_badVersion.ttf \
	grtest_badCmap.ttf \
	grtest_burmese.ttf \
	grtest_burmese2.ttf \
	grtest_burmese3.ttf \
	grtest_roman.ttf \
	grtest_taiviet.ttf \
	grtest_xline.ttf \
    grtest_pconstraints.ttf \
	xline.gdl \
	Makefile.vc \
	makedebug.bat \
	readme.txt

noinst_PROGRAMS = regression-test

AM_CPPFLAGS  = -I$(top_srcdir)/include/graphite

regression_test_LDFLAGS = -L$(top_builddir)/src -lgraphite

regression_test_SOURCES  = \
	main.h \
	stdafx.cpp stdafx.h \
	RegressionTest.cpp \
	TestCase.cpp TestCase.h \
	RtTextSrc.h \
	SimpleTextSrc.cpp SimpleTextSrc.h \
	GrJustifier.cpp GrJustifier.h

check-local: regression-test
	$(top_builddir)/test/RegressionTest/regression-test -p $(srcdir)

dist-hook:
	rm -f grregtest.log tracelog.txt
