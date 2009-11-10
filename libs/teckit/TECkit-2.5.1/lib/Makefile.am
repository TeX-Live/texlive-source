AUTOMAKE_OPTIONS = 1.6

AM_CPPFLAGS  = -I$(top_srcdir)/source/Public-headers

if OLD_LIB_NAMES
lib_LTLIBRARIES = TECkit_Compiler_x86.la TECkit_x86.la
else
lib_LTLIBRARIES = libTECkit_Compiler.la libTECkit.la
endif


include ../source/Makefile.am

