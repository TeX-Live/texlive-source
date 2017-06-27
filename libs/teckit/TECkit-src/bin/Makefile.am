AUTOMAKE_OPTIONS = -Wno-portability

bin_PROGRAMS = teckit_compile txtconv

AM_CPPFLAGS  = -I$(top_srcdir)/source/Public-headers

expatdir = $(top_srcdir)/SFconv/expat
tool_src = $(top_srcdir)/source/Sample-tools

if SYSTEM_EXPAT
EXPAT_CFLAGS = -DXML_DTD @expat_CFLAGS@
else
EXPAT_CFLAGS = -DXML_DTD -I$(expatdir)/xmlparse -I$(expatdir)/xmltok
noinst_LIBRARIES = libexpat.a
libexpat_a_SOURCES  = @top_builddir@/SFconv/expat/xmlparse/xmlparse.c
libexpat_a_SOURCES += @top_builddir@/SFconv/expat/xmlparse/hashtable.c
libexpat_a_SOURCES += @top_builddir@/SFconv/expat/xmltok/xmlrole.c
libexpat_a_SOURCES += @top_builddir@/SFconv/expat/xmltok/xmltok.c
libexpat_a_CFLAGS = $(AM_CPPFLAGS) $(EXPAT_CFLAGS)
$(am_libexpat_a_OBJECTS): CFLAGS := $(filter-out -Wextra, $(CFLAGS))
endif

# AM_CPPFLAGS  += $(EXPAT_CFLAGS)
AM_CFLAGS    = $(AM_CPPFLAGS)

teckit_compile_SOURCES = @top_builddir@/source/Sample-tools/TECkit_Compile.cpp

txtconv_SOURCES = @top_builddir@/source/Sample-tools/TxtConv.cpp

sfconv_SOURCES  = @top_builddir@/SFconv/SFconv.cpp
sfconv_SOURCES += @top_builddir@/SFconv/UtfCodec.cpp
sfconv_CPPFLAGS = $(AM_CPPFLAGS) $(EXPAT_CFLAGS)
sfconv_CXXFLAGS = -std=c++11
sfconv_CFLAGS   = $(AM_CPPFLAGS) $(EXPAT_CFLAGS)
sfconv_SOURCES += @top_builddir@/SFconv/sfReader.h
sfconv_SOURCES += @top_builddir@/SFconv/UtfCodec.h
sfconv_SOURCES += @top_builddir@/SFconv/Debug_Prefix.h
sfconv_SOURCES += @top_builddir@/SFconv/Final_Prefix.h
sfconv_SOURCES += @top_builddir@/SFconv/ushort_chartraits.h
if OLD_LIB_NAMES
teckit_compile_LDADD = $(top_builddir)/lib/TECkit_Compiler_x86.la
txtconv_LDADD        = $(top_builddir)/lib/TECkit_x86.la
sfconv_LDADD         = $(top_builddir)/lib/TECkit_x86.la @expat_LIBS@
else
teckit_compile_LDADD = $(top_builddir)/lib/libTECkit_Compiler.la
txtconv_LDADD        = $(top_builddir)/lib/libTECkit.la
sfconv_LDADD         = $(top_builddir)/lib/libTECkit.la @expat_LIBS@
endif
sfconv_LDADD += $(noinst_LIBRARIES)

# --> RC Support
if OLD_LIB_NAMES
teckit_compile_DEPENDENCIES = TECkit_Compile_ver.o
teckit_compile_LDFLAGS      = -Wl,TECkit_Compile_ver.o
TECkit_Compile_ver.o: $(tool_src)/TECkit_Compile_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -o $@ $<

txtconv_DEPENDENCIES = TxtConv_ver.o
txtconv_LDFLAGS      = -Wl,TxtConv_ver.o
TxtConv_ver.o: $(tool_src)/TxtConv_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -o $@ $<

sfconv_DEPENDENCIES = SFconv_ver.o
sfconv_LDFLAGS      = -Wl,SFconv_ver.o
SFconv_ver.o: $(top_srcdir)/SFconv/SFconv_ver.rc
	$(RC) $(AM_RCFLAGS) $(RCFLAGS) -I$(tool_src) -o $@ $<
endif
