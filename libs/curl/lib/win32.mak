################################################################################
#
# Makefile  : libcurl / lib
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/09/23 17:43:33 popineau>
#
################################################################################
root_srcdir=..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_CURL = 1
MAKE_CURL = 1
USE_GNUW32 = 1
USE_NETWORK2 = 1

!include <msvc/common.mak>

programs = $(curldll)
libfiles = $(curllib)
includefiles =
manfiles =
objects = \
	$(objdir)\base64.obj	\
	$(objdir)\cookie.obj	\
	$(objdir)\transfer.obj	\
	$(objdir)\escape.obj	\
	$(objdir)\formdata.obj	\
	$(objdir)\ftp.obj	\
	$(objdir)\http.obj	\
	$(objdir)\http_chunks.obj	\
	$(objdir)\ldap.obj	\
	$(objdir)\dict.obj	\
	$(objdir)\telnet.obj	\
	$(objdir)\getdate.obj	\
	$(objdir)\getenv.obj	\
	$(objdir)\getpass.obj	\
	$(objdir)\hostip.obj	\
	$(objdir)\if2ip.obj	\
	$(objdir)\mprintf.obj	\
	$(objdir)\netrc.obj	\
	$(objdir)\progress.obj	\
	$(objdir)\sendf.obj	\
	$(objdir)\speedcheck.obj	\
	$(objdir)\ssluse.obj	\
	$(objdir)\timeval.obj	\
	$(objdir)\url.obj	\
	$(objdir)\file.obj	\
	$(objdir)\getinfo.obj	\
	$(objdir)\version.obj	\
	$(objdir)\easy.obj	\
	$(objdir)\strequal.obj	\
	$(objdir)\strtok.obj	\
	$(objdir)\connect.obj	\
	$(objdir)\hash.obj	\
	$(objdir)\llist.obj

DEFS = $(DEFS) -DVC6 -DMSDOS

default: all

all: $(objdir) $(curl)

lib: $(objdir) $(curllib)

!ifdef CURL_DLL
DEFS = $(DEFS) -DMAKE_LIBCURL_DLL

$(curllib): libcurl.def $(objdir)\libcurl.res $(objects)
	$(archive) /DEF:libcurl.def $(objdir)\libcurl.res $(objects)

$(curldll): $(objects) $(gnuw32lib)
	$(link_dll) $(**) $(curllib:.lib=.exp) $(conlibs)
!else
$(curllib): $(objects)
	$(archive) $(objects)
!endif

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec install-lib install-include

!include <msvc/clean.mak>

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
