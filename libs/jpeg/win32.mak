################################################################################
#
# Makefile  : JPEG
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/01/28 21:36:28 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_JPEG = 1
MAKE_JPEG = 1
USE_GNUW32 = 1
USE_ZLIB = 1

# Pull in standard variable definitions
!include <msvc/common.mak>

# Put here the object file name for the correct system-dependent memory
# manager file.  For NT we suggest jmemnobs.obj, which expects the OS to
# provide adequate virtual memory.
SYSDEPMEM= $(objdir)\jmemnobs.obj

# End of configurable options.

# source files: JPEG library proper
LIBSOURCES= jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c \
        jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c \
        jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c \
        jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c \
        jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c \
        jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c \
        jfdctint.c jidctflt.c jidctfst.c jidctint.c jidctred.c jquant1.c \
        jquant2.c jutils.c jmemmgr.c
# memmgr back ends: compile only one of these into a working library
SYSDEPSOURCES= jmemansi.c jmemname.c jmemnobs.c jmemdos.c jmemmac.c
# source files: cjpeg/djpeg/jpegtran applications, also rdjpgcom/wrjpgcom
APPSOURCES= cjpeg.c djpeg.c jpegtran.c rdjpgcom.c wrjpgcom.c cdjpeg.c \
        rdcolmap.c rdswitch.c transupp.c rdppm.c wrppm.c rdgif.c wrgif.c \
        rdtarga.c wrtarga.c rdbmp.c wrbmp.c rdrle.c wrrle.c
SOURCES= $(LIBSOURCES) $(SYSDEPSOURCES) $(APPSOURCES)
# files included by source files
INCLUDES= jchuff.h jdhuff.h jdct.h jerror.h jinclude.h jmemsys.h jmorecfg.h \
        jpegint.h jpeglib.h jversion.h cdjpeg.h cderror.h transupp.h
# documentation, test, and support files
DOCS= README install.doc usage.doc cjpeg.1 djpeg.1 jpegtran.1 rdjpgcom.1 \
        wrjpgcom.1 wizard.doc example.c libjpeg.doc structure.doc \
        coderules.doc filelist.doc change.log
MKFILES= configure makefile.cfg makefile.ansi makefile.unix makefile.bcc \
        makefile.mc6 makefile.dj makefile.wat makefile.vc makelib.ds \
        makeapps.ds makeproj.mac makcjpeg.st makdjpeg.st makljpeg.st \
        maktjpeg.st makefile.manx makefile.sas makefile.mms makefile.vms \
        makvms.opt
CONFIGFILES= jconfig.cfg jconfig.bcc jconfig.mc6 jconfig.dj jconfig.wat \
        jconfig.vc jconfig.mac jconfig.st jconfig.manx jconfig.sas \
        jconfig.vms
CONFIGUREFILES= config.guess config.sub install-sh ltconfig ltmain.sh
OTHERFILES= jconfig.doc ckconfig.c ansi2knr.c ansi2knr.1 jmemdosa.asm
TESTFILES= testorig.jpg testimg.ppm testimg.bmp testimg.jpg testprog.jpg \
        testimgp.jpg
DISTFILES= $(DOCS) $(MKFILES) $(CONFIGFILES) $(SOURCES) $(INCLUDES) \
        $(CONFIGUREFILES) $(OTHERFILES) $(TESTFILES)
# library object files common to compression and decompression
COMOBJECTS= $(objdir)\jcomapi.obj $(objdir)\jutils.obj $(objdir)\jerror.obj \
	$(objdir)\jmemmgr.obj $(SYSDEPMEM)
# compression library object files
CLIBOBJECTS= $(objdir)\jcapimin.obj $(objdir)\jcapistd.obj                   \
	$(objdir)\jctrans.obj $(objdir)\jcparam.obj $(objdir)\jdatadst.obj   \
        $(objdir)\jcinit.obj $(objdir)\jcmaster.obj $(objdir)\jcmarker.obj   \
	$(objdir)\jcmainct.obj $(objdir)\jcprepct.obj $(objdir)\jccoefct.obj \
	$(objdir)\jccolor.obj $(objdir)\jcsample.obj $(objdir)\jchuff.obj    \
	$(objdir)\jcphuff.obj $(objdir)\jcdctmgr.obj $(objdir)\jfdctfst.obj  \
	$(objdir)\jfdctflt.obj $(objdir)\jfdctint.obj
# decompression library object files
DLIBOBJECTS= $(objdir)\jdapimin.obj $(objdir)\jdapistd.obj                   \
	$(objdir)\jdtrans.obj $(objdir)\jdatasrc.obj $(objdir)\jdmaster.obj  \
	$(objdir)\jdinput.obj $(objdir)\jdmarker.obj $(objdir)\jdhuff.obj    \
	$(objdir)\jdphuff.obj $(objdir)\jdmainct.obj $(objdir)\jdcoefct.obj  \
	$(objdir)\jdpostct.obj $(objdir)\jddctmgr.obj $(objdir)\jidctfst.obj \
        $(objdir)\jidctflt.obj $(objdir)\jidctint.obj $(objdir)\jidctred.obj \
	$(objdir)\jdsample.obj $(objdir)\jdcolor.obj $(objdir)\jquant1.obj   \
	$(objdir)\jquant2.obj $(objdir)\jdmerge.obj
# These objectfiles are included in libjpeg.lib
LIBOBJECTS= $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS)
# object files for sample applications (excluding library files)
COBJECTS= $(objdir)\cjpeg.obj $(objdir)\rdppm.obj $(objdir)\rdgif.obj        \
	$(objdir)\rdtarga.obj $(objdir)\rdrle.obj $(objdir)\rdbmp.obj        \
        $(objdir)\rdswitch.obj $(objdir)\cdjpeg.obj
DOBJECTS= $(objdir)\djpeg.obj $(objdir)\wrppm.obj $(objdir)\wrgif.obj        \
	$(objdir)\wrtarga.obj $(objdir)\wrrle.obj $(objdir)\wrbmp.obj        \
        $(objdir)\rdcolmap.obj $(objdir)\cdjpeg.obj
TROBJECTS= $(objdir)\jpegtran.obj $(objdir)\rdswitch.obj                     \
	$(objdir)\cdjpeg.obj $(objdir)\transupp.obj

programs = $(jpegdll)		\
	$(objdir)\cjpeg.exe	\
	$(objdir)\djpeg.exe	\
	$(objdir)\jpegtran.exe	\
	$(objdir)\rdjpgcom.exe	\
	$(objdir)\wrjpgcom.exe
libfiles = $(jpeglib)
includefiles = jerror.h jpeglib.h jconfig.h
manfiles = cjpeg.1 djpeg.1 jpegtran.1 rdjpgcom.1 wrjpgcom.1

default: all

all: $(objdir) $(jpeg) $(programs)

lib: $(objdir) $(jpeg)

!ifdef JPEG_DLL
DEFS = $(DEFS) -DMAKE_JPEG_DLL

$(jpeglib): $(objdir) $(LIBOBJECTS)
	$(archive) /DEF  $(LIBOBJECTS)

$(jpegdll): $(LIBOBJECTS) $(zliblib) $(gnuw32lib)  $(objdir)\libjpeg.res
	$(link_dll) $(**) $(jpeglib:.lib=.exp) $(conlibs)
!else
$(jpeglib): $(objdir) $(LIBOBJECTS)
	$(archive) $(LIBOBJECTS)
!endif

jconfig.h: jconfig.vc
	$(copy) jconfig.vc jconfig.h

$(objdir)\cjpeg.exe: $(COBJECTS) $(jpeglib)
	$(link) /OUT:$@ $(COBJECTS) $(jpeglib) $(conlibs)

$(objdir)\djpeg.exe: $(DOBJECTS) $(jpeglib)
	$(link) /OUT:$@ $(DOBJECTS) $(jpeglib) $(conlibs)

$(objdir)\jpegtran.exe: $(TROBJECTS) $(jpeglib)
	$(link) /OUT:$@ $(TROBJECTS) $(jpeglib) $(conlibs)

$(objdir)\rdjpgcom.exe: $(objdir)\rdjpgcom.obj
	$(link) /OUT:$@ $(objdir)\rdjpgcom.obj $(conlibs)

$(objdir)\wrjpgcom.exe: $(objdir)\wrjpgcom.obj
	$(link) /OUT:$@ $(objdir)\wrjpgcom.obj $(conlibs)

test: $(objdir)\cjpeg.exe $(objdir)\djpeg.exe $(objdir)\jpegtran.exe
	-@$(del) testout*
	.\$(objdir)\djpeg -dct int -ppm -outfile testout.ppm  testorig.jpg
	.\$(objdir)\djpeg -dct int -bmp -colors 256 -outfile testout.bmp  testorig.jpg
	.\$(objdir)\cjpeg -dct int -outfile testout.jpg  testimg.ppm
	.\$(objdir)\djpeg -dct int -ppm -outfile testoutp.ppm testprog.jpg
	.\$(objdir)\cjpeg -dct int -progressive -opt -outfile testoutp.jpg testimg.ppm
	.\$(objdir)\jpegtran -outfile testoutt.jpg testprog.jpg
	fc /b testimg.ppm testout.ppm
	fc /b testimg.bmp testout.bmp
	fc /b testimg.jpg testout.jpg
	fc /b testimg.ppm testoutp.ppm
	fc /b testimgp.jpg testoutp.jpg
	fc /b testorig.jpg testoutt.jpg

!include <msvc/config.mak>

!include <msvc/install.mak>

install:: install-exec install-include install-lib

!include <msvc/clean.mak>

clean::
	-@$(del) .\testout*.jpg .\testout*.bmp .\testout*.ppm

distclean::
	-@$(del) .\jconfig.h

!include <msvc/rdepend.mak>
!include "./depend.mak"

# 
# Local Variables:
# mode: makefile
# End:

