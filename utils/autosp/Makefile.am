## $Id$
## Makefile.am for the TeX Live subdirectory utils/autosp/
##
## Copyright 2016-2018 Karl Berry <tex-live@tug.org>
## You may freely use, modify and/or distribute this file.

## We want to re-distribute the whole original ps2eps source tree.
EXTRA_DIST = $(AUTOSP_TREE)

# Files not to be distributed
include $(srcdir)/../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

AM_CFLAGS = $(WARNING_CFLAGS)

# We intentionally omit the rebar program, since the name just seems to
# be generic to end up in system bindirs, and it's said to just be a
# convenience.  People can get it from the Werner Icking Music Archive.
bin_PROGRAMS = autosp tex2aspc

# must use @... here or subdir $(... (literally) is created for .deps.
autosp_SOURCES = \
	@AUTOSP_TREE@/autosp.c \
	@AUTOSP_TREE@/utils.c \
	@AUTOSP_TREE@/process_score.c \
	@AUTOSP_TREE@/process_command.c \
	@AUTOSP_TREE@/status.c \
	@AUTOSP_TREE@/spacing_note.c \
	@AUTOSP_TREE@/xtuplet.c

tex2aspc_SOURCES = \
	@AUTOSP_TREE@/tex2aspc.c \
	@AUTOSP_TREE@/utils.c

man1_MANS = \
	$(AUTOSP_TREE)/autosp.1 \
	$(AUTOSP_TREE)/tex2aspc.1

## Tests
AM_TESTS_ENVIRONMENT = AUTOSP_TREE=$(AUTOSP_TREE); export AUTOSP_TREE;
#
TESTS = $(AUTOSP_TREE)/autosp.test $(AUTOSP_TREE)/tex2aspc.test
EXTRA_DIST += $(TESTS)
autosp.log: autosp$(EXEEXT)
tex2aspc.log: tex2aspc$(EXEEXT)
#
## autosp.test files
EXTRA_DIST += $(AUTOSP_TREE)/tests
