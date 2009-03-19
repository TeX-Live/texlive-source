AUTOMAKE_OPTIONS = 1.6

noinst_PROGRAMS = profile-graphite

AM_CPPFLAGS  = -I$(top_srcdir)/include

profile_graphite_LDFLAGS = -L$(top_builddir)/src -lgraphite

profile_graphite_SOURCES  = \
	GrUtfTextSrc.cpp GrUtfTextSrc.h \
	ProfileHarness.cpp

#dist-hook:
#	rm -f grregtest.log tracelog.txt
