##
INCLUDES = -I$(top_builddir)/.. -I$(top_srcdir) $(KPATHSEA_INCLUDES)

## Rebuild libkpathsea
@KPATHSEA_RULE@

noinst_LIBRARIES = lib.a

## We don't compile `texmfmp.c'; this file is converted into 'texextra.c'
## for TeX, 'mfextra.c' for Metafont, 'mfextra.c' for MetaPost, ...
lib_a_SOURCES = \
	basechsuffix.c \
	chartostring.c \
	coredump.c \
	eofeoln.c \
	fprintreal.c \
	inputint.c \
	input2int.c \
	main.c \
	openclose.c \
	printversion.c \
	uexit.c \
	usage.c \
	version.c \
	zround.c

## Not yet used
##
EXTRA_DIST = alloca.c win32main.c

