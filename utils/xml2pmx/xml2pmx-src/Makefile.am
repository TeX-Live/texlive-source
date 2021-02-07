bin_PROGRAMS = xml2pmx

xml2pmx_SOURCES = Args.c Builtin.c dynlink.c Files.c gc.c \
	image.c interp.c primtab0.c support.c xmain.c loader.c \
	exec.h keiko.h obcommon.h obx.h primtab.h

# Add action.c (included in interp.c), the test subdirectory, and
# Oberon sources from the obsrc directory.
EXTRA_DIST = action.c test \
	obsrc/b.m obsrc/Complete.m obsrc/Fifo.m obsrc/Makefile \
	obsrc/MyFiles.m obsrc/Strings1.m obsrc/Testbed.m

TESTS = xml2pmx.test
EXTRA_DIST += $(TESTS)
EXTRA_DIST += tests/Telemann.xml tests/Telemann.pmx
EXTRA_DIST += configure.ac Makefile.am

DISTCLEANFILES = Telemann* .deps/*
