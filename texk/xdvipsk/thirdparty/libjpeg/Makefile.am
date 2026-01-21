## Wrapper Makefile.am to build libjpeg for TeX Live.
##
##   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
##
##   This file is free software; the copyright holder
##   gives unlimited permission to copy and/or distribute it,
##   with or without modifications, as long as this notice is preserved.
##
## We want to re-distribute the whole original libjpeg source tree.
##
EXTRA_DIST = $(LIBJPEG_TREE)

## Changes applied to the original source tree
##
EXTRA_DIST += $(LIBJPEG_TREE)-PATCHES

# Files not to be distributed
include $(srcdir)/../../../../am/dist_hook.am
NEVER_NAMES += $(NEVER_NAMES_SUB)

SUBDIRS = . include 

AM_CPPFLAGS = -I$(top_srcdir)/$(LIBJPEG_TREE) \
              $(LIBJPEG_DEFINES) $(LIBJPEG_USER_CPPFLAGS)
AM_CFLAGS = $(WARNING_CFLAGS)
##AM_CFLAGS = $(VISIBILITY_CFLAGS) $(WARNING_CFLAGS) 

noinst_LIBRARIES = libjpeg.a

libjpeg_a_SOURCES = \
	@LIBJPEG_TREE@/jaricom.c \
	@LIBJPEG_TREE@/jcapimin.c \
	@LIBJPEG_TREE@/jcapistd.c \
	@LIBJPEG_TREE@/jcarith.c \
	@LIBJPEG_TREE@/jccoefct.c \
	@LIBJPEG_TREE@/jccolor.c \
	@LIBJPEG_TREE@/jcdctmgr.c \
	@LIBJPEG_TREE@/jchuff.c \
	@LIBJPEG_TREE@/jcinit.c \
	@LIBJPEG_TREE@/jcmainct.c \
	@LIBJPEG_TREE@/jcmarker.c \
	@LIBJPEG_TREE@/jcmaster.c \
	@LIBJPEG_TREE@/jcomapi.c \
	@LIBJPEG_TREE@/jcparam.c \
	@LIBJPEG_TREE@/jcprepct.c \
	@LIBJPEG_TREE@/jcsample.c \
	@LIBJPEG_TREE@/jctrans.c \
	@LIBJPEG_TREE@/jdapimin.c \
	@LIBJPEG_TREE@/jdapistd.c \
	@LIBJPEG_TREE@/jdarith.c \
	@LIBJPEG_TREE@/jdatadst.c \
	@LIBJPEG_TREE@/jdatasrc.c \
	@LIBJPEG_TREE@/jdcoefct.c \
	@LIBJPEG_TREE@/jdcolor.c \
	@LIBJPEG_TREE@/jddctmgr.c \
	@LIBJPEG_TREE@/jdhuff.c \
	@LIBJPEG_TREE@/jdinput.c \
	@LIBJPEG_TREE@/jdmainct.c \
	@LIBJPEG_TREE@/jdmarker.c \
	@LIBJPEG_TREE@/jdmaster.c \
	@LIBJPEG_TREE@/jdmerge.c \
	@LIBJPEG_TREE@/jdpostct.c \
	@LIBJPEG_TREE@/jdsample.c \
	@LIBJPEG_TREE@/jdtrans.c \
	@LIBJPEG_TREE@/jerror.c \
	@LIBJPEG_TREE@/jfdctflt.c \
	@LIBJPEG_TREE@/jfdctfst.c \
	@LIBJPEG_TREE@/jfdctint.c \
	@LIBJPEG_TREE@/jidctflt.c \
	@LIBJPEG_TREE@/jidctfst.c \
	@LIBJPEG_TREE@/jidctint.c \
	@LIBJPEG_TREE@/jquant1.c \
	@LIBJPEG_TREE@/jquant2.c \
	@LIBJPEG_TREE@/jutils.c \
	@LIBJPEG_TREE@/jmemmgr.c \
	@LIBJPEG_TREE@/@MEMORYMGR@.c

$(libjpeg_a_OBJECTS): config.force

CLEANFILES = 

LDADD = libjpeg.a

clean-local:
	rm -rf libjpeg libjpeg.* 

# Rebuild libjpeg
@LIBJPEG_RULE@

#Reconfig
reconfig_prereq = $(LIBJPEG_DEPEND)
DISTCLEANFILES = 

#Rebuild
rebuild_prereq =
rebuild_target = all

include $(srcdir)/../../../../am/rebuild.am
