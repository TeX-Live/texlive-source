################################################################################
#
# Makefile  : Common, definitions to build win32 tools
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 15:59:52 popineau>
#
################################################################################

################################################################################
#
# Version numbers
#
################################################################################

!include <msvc/texlive.inc>

################################################################################
#
# Configurable section
#
################################################################################

# MSVC main variables
CPU=i386
MSDEV=C:\Program Files\Microsoft Visual Studio .NET 2003
MSVCROOT=$(MSDEV)\VC7
# Change to 'on' if you want verbose builds
verbose = off
# Define for profiling
#PROFILE = 1
# define for optimized code
#NODEBUG = 1
# define for static binaries (no kpathsea.dll, standalone exe files)
#STATIC=
# define for debug malloc
#MEMDBG=1
# define for BoundsChecker FinalCheck
#BOUNDSCHECKER=1
#
#INTEL_COMPILER = 1

!ifndef NODEBUG
DEBUG = 1
!endif

# Configurable section

# Where to install things
# This would be better if it where
# relative to the main directory, but it clutters
# things too much.
#
prefix = c:\Progra~1\TeXLive
texprefix = c:\Progra~1\TeXLive
webdir = c:\InetPub\htdocs\fptex
ftpdir = c:\InetPub\ftp\fptex
masterdir = c:\source\TeXLive\Master

fptexversion = 0.7

#
# Source directories
#
fptex_srcdir = c:\Source\fpTeX

!ifndef root_srcdir
root_srcdir = $(fptex_srcdir)
!endif

!ifdef DEVELOPMENT
development = .development
!endif

libs_srcdir = $(root_srcdir)\libs
texk_srcdir = $(root_srcdir)\texk$(development)
gnuroot_srcdir = $(root_srcdir)\gnu
graphics_srcdir = $(root_srcdir)\graphics
utils_srcdir = $(root_srcdir)\utils

srcdir = .
msvcdir = $(texk_srcdir)\msvc

#
# Tools
#
gnutools = c:\Local\Gnu
netpbmtools = c:\Local\NetPBM
perltools = c:\Local\Perl
xemacstools = c:\Progra~1\XEmacs
xemtextools = c:\Progra~1\TeXLive\xemtex
xemacsversion = xemacs-21.5

# Cmd commands
copy    = copy /y
copydir = xcopy /q/y/r/i/e/k
sync	= $(perl) -I$(win32perldir) $(win32perldir)\sync.pl
del     = del /f/q
deldir  = rmdir /q/s
echo    = echo
make    = nmake -f win32.mak -nologo -$(MAKEFLAGS) verbose=$(verbose)
mkdir   = mkdir
move    = move /y
shell   = cmd /x

# Gnu and various commands
awk	= $(gnubin)\gawk
diff	= $(gnubin)\diff.exe -u
eqn	= $(gnubin)\eqn
expand	= $(gnubin)\expand
grep	= $(gnubin)\grep.exe
grodvi	= $(gnubin)\grodvi
grops	= $(gnubin)\grops
gzip	= $(gnubin)\gzip -f
lex	= $(gnubin)\flex
# if DYNAMIC, just copy the executable (because TeX engines are stored
# in the companion DLL)
!ifdef STATIC
# Make it absolute !
lnexe	= $(fptex_srcdir)\texk$(development)\contrib\$(objdir)\irun.exe
!else
lnexe	 = copy /y
!endif
makedepend = $(gnubin)\makedepend # -Y
perl	= c:\Local\Perl\bin\perl -I$(win32perldir)
rman	= $(gnubin)\rman
runperl	= $(texk_srcdir)\contrib\$(objdir)\irun.exe
sed	= $(gnubin)\sed.exe
sort	= $(gnubin)\sort.exe
tbl	= $(gnubin)\tbl
touch 	= $(gnubin)\touch.exe
tr	= $(gnubin)\tr.exe
troff	= $(gnubin)\groff
yacc	= $(gnubin)\bison -y

###############################################################################
#
# End of configurable section
#
###############################################################################

#
# Subdirectories of win32
#
win32perldir = $(root_srcdir)\mswin32
win32makedir = $(msvcdir)
win32seddir = $(msvcdir)
win32rcdir = . # $(msvcdir)\rc

#
# Where to find gnu tools and to put other stuff ?
# tr, sed, grep, cat are needed
#
gnuroot = $(gnutools)^\
gnubin = $(gnutools)\bin
gnuinclude = $(gnutools)\include
gnulib = $(gnutools)\lib
gnushare = $(gnutools)\share
gnuinfo = $(gnutools)\info
gnuman = $(gnutools)\man

perlbin = $(perltools)\bin
perllib = $(perltools)\lib

#
# To make the build process more silent
#
!if ("$(verbose)" == "on")
!else
silent = @
redir_stdout = >nul
redir_stderr = 2>nul
!endif

!if "$(OS)" == "Windows_NT"
null=
!else 
null=nul
!endif 

################################################################################
#
# Several targets:
#	* debug means non optimized
#	* static means statically linked
#	* profile implies static
# Choose one obj target among these
#
################################################################################
#
objtargets = static dynamic static-debug dynamic-debug profile profile-debug
#
CONFIG_MT=1
!ifndef STATIC
MSVC_DLL=1
!endif
!ifdef PROFILE
# Profile implies Static. Too much complicated otherwise
objdir=profile
!elseifdef STATIC
objdir=static
!else
objdir=dynamic
!endif


#
!ifndef NODEBUG
objdir=$(objdir)-debug
!endif
!ifdef STATIC
make    = $(make) STATIC=1
!endif
!ifdef NODEBUG
make    = $(make) NODEBUG=1
!endif
!ifdef PROFILE
make    = $(make) PROFILE=1
!endif
!ifdef MEMDBG
make    = $(make) MEMDBG=1
!endif
!ifdef BOUNDSCHECKER
make    = $(make) BOUNDSCHECKER=1
!endif

#
################################################################################


################################################################################
#
# The Perl Compiler
#
################################################################################

perlcomp = perlapp
perlcompflags = --force --freestanding --lib $(texmf)/context/perltk;$(texmf:-development=)/context/perltk --xclude --exe $@ --verbose --explain all

################################################################################
#
# Your C Compiler
#
################################################################################
# Build libraries
AR = $(silent)lib
# resource compiler
RC = $(silent)rc
#
# BoundsChecker
#
!ifdef BOUNDSCHECKER
BOUNDSCHECKERPATH = C:\Program Files\Compuware\BoundsChecker
PATH = $(BOUNDSCHECKERPATH);$(PATH)
CC = $(silent)nmcl
CCLD = $(silent)nmlink
optlibs = "$(BOUNDSCHECKER)"\bcinterf.lib
!elseifdef INTEL_COMPILER
#
# Intel compiler definitions
#
INTEL_COMPILER80=C:\Program Files\Intel\CPP\Compiler80
INTEL_SHARED=C:\Program Files\Common Files\Intel\Shared Files
INTEL_LICENSE_FILE=C:\Program Files\Common Files\Intel\Licenses
PATH=$(INTEL_COMPILER80)\Ia32\Bin;$(INTEL_SHARED)\Ia32\Bin;$(PATH)
LIB=$(INTEL_COMPILER80)\Ia32\Lib;$(INTEL_SHARED)\Ia32\Lib;$(LIB)
INCLUDE=$(INTEL_COMPILER80)\Ia32\Include;$(INCLUDE)
CC = $(silent)icl /Qvc7.1
CCLD = $(silent)xilink
AR = $(silent)xilib
optlibs = libircmt.lib
!else
#
# Microsoft C/C++ Compiler
#
CC = $(silent)cl
CCLD = $(silent)link
!endif
################################################################################

# Define to get strict Ansi conformance
!ifndef STRICT_ANSI
STRICT_ANSI = 1
!endif
# Define to use oldnames.lib instead of macros
!ifndef USE_OLDNAMES
USE_OLDNAMES = 1
!endif

# Ensure that if MSVCRT is choosen, MT is defined.
!ifdef MSVC_DLL
!ifndef CONFIG_MT
CONFIG_MT=1
!endif
!endif

cvars =

!IF "$(CPU)" == "i386"
cvars = $(cvars) -D_X86_=1
lflags = /MACHINE:IX86
libflags = /MACHINE:IX86
!ELSE
!IF "$(CPU)" == "MIPS"
cvars = $(cvars) -D_MIPS_=1
!ELSE
!IF "$(CPU)" == "PPC"
cvars = $(cvars) -D_PPC_=1
!ELSE
!IF "$(CPU)" == "ALPHA"
cvars = $(cvars) -D_ALPHA_=1
!endif
!endif
!endif
!endif

# For this release, for all values of APPVER, WINVER is 0x0400
cvars = $(cvars) -DWINVER=0x0400

# For Windows applications that use the C Run-Time libraries
# Use the DLL MultiThread C Library
cvars   = $(cvars) -DWIN32 -D_WIN32 -D_WINDOWS
!ifdef CONFIG_MT
cvars   = $(cvars) -D_MT
!endif
!ifdef MSVC_DLL
cvars   = $(cvars) -D_DLL
!endif

# Compilation flags.
!ifdef NODEBUG

!ifdef INTEL_COMPILER
optflags = -GL -O3 -Gr -Gy -Zi -QaxWNB -Qunroll -Qopt_report -Qinline_debug_info # Intel specific
!else
optflags = -O2x -G7 -Gr -Gy -GL $(optflags)
ldebug = /LTCG
!endif

!else
optflags = -Od -G7r -Zi $(optflags)
#optflags = -O2x -G5rs -Gy -Zi
!endif

!ifdef PROFILE
optflags = $(optflags) -Gh -Fm
lmap = /MAP /FIXED:NO /PROFILE
!endif

#
# Preprocessor flags
#
!if !defined(FPTEX)
FPTEX=1
!endif
!if $(FPTEX)
XCFLAGS = -DFPTEX=1
!endif
!ifdef MEMDBG
XCFLAGS = $(XCFLAGS) -D_DEBUG -D_CRTDBG_MAP_ALLOC
!endif

CFLAGS = -W3 -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -nologo \
	$(optflags) $(XCFLAGS)
CPPFLAGS = $(XCPPFLAGS)
DEFS = $(XDEFS) $(cvars) -I.

!if $(STRICT_ANSI)
#CFLAGS = $(CFLAGS) -Za
DEFS = $(DEFS) -D__STDC__=1
!endif

# The default `compile' command
CFLAGS = $(CFLAGS) $(DEFS)

compile = $(CC) $(CFLAGS) -c /Fo$@

makedepend_cflags = -I. -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -D_M_IX86=1 $(XCFLAGS) $(DEFS)

!ifdef NODEBUG
ldebug = $(ldebug) /RELEASE /OPT:REF /VERSION:$(major_version_number).$(minor_version_number)
!ELSE
ldebug = $(ldebug) -debug:full -debugtype:cv
# ldebug = -debug:full -debugtype:cv -opt:ref
!endif

lflags =  $(lflags) /NODEFAULTLIB /INCREMENTAL:NO /NOLOGO \
	$(ldebug) $(lmap)
conlflags = $(lflags) -subsystem:console -entry:mainCRTStartup
dlllflags = $(lflags) -entry:_DllMainCRTStartup@12 -dll
guilflags = $(lflags) -subsystem:windows -entry:WinMainCRTStartup

!ifdef PROFILE
optlibs =  $(optlibs) cap.lib
!endif

basewinlibs = kernel32.lib $(optlibs)

!if $(USE_OLDNAMES)
basewinlibs = oldnames.lib $(basewinlibs)
!else
DEFS = $(DEFS) -DOLDNAMES_WITH_MACROS=1
!endif

!if (defined(NODEBUG) && !defined(MEMDBG))
! ifdef MSVC_DLL
libc = msvcrt.lib $(basewinlibs)
! else
!  ifdef CONFIG_MT
libc = libcmt.lib $(basewinlibs)
!  else
libc = libc.lib $(basewinlibs)
!  endif
! endif
!else
! ifdef MSVC_DLL
libc = msvcrtd.lib $(basewinlibs)
! else
!  ifdef CONFIG_MT
libc = libcmtd.lib $(basewinlibs)
!  else
libc = libcd.lib $(basewinlibs)
!  endif
! endif
!endif /* NODEBUG */

winlibs = user32.lib gdi32.lib comdlg32.lib winspool.lib \
	version.lib comctl32.lib

!if defined(USE_GSW32)
USE_ADVAPI = 1
!endif

!ifdef USE_ADVAPI
advapiflags = /delayload:advapi32.dll
advapilibs = advapi32.lib

libc = $(libc) $(advapilibs)
lflags = $(lflags) $(advapiflags)
!endif

!if defined(USE_NETWORK) || defined(USE_NETWORK2)
USE_INET = 1
!endif

# Delay load wsock32 and wininet when required
!ifdef USE_NETWORK
networklibs = wsock32.lib
networkflags = /delayload:wsock32.dll

libc = $(libc) $(networklibs)
lflags = $(lflags) $(networkflags)
!endif

!ifdef USE_NETWORK2
network2libs = ws2_32.lib 
network2flags = /delayload:ws2_32.dll

libc = $(libc) $(network2libs)
lflags = $(lflags) $(network2flags)
!endif

!ifdef USE_INET
inetlibs = wininet.lib
inetflags = /delayload:wininet.dll

libc = $(libc) $(inetlibs)
lflags = $(lflags) $(inetflags)
!endif


!if defined(USE_ADVAPI) || defined(USE_NETWORK) || defined(USE_NETWORK2)
libc = $(libc) delayimp.lib
!endif
#
# This is stupid with MSVC7 : using winsock2
# you need to define WIN32_LEAN_AND_MEAN
# else winsock2.h conflicts with winsock.h !
#
!if defined(USE_NETWORK)
DEFS = $(DEFS) -DUSE_WINSOCK
!else
DEFS = $(DEFS) -DWIN32_LEAN_AND_MEAN
!endif

rcflags = /r /I$(win32makedir)
rcvars = -DWIN32 -D_WIN32 -DWINVER=0x0400 $(xrcvars)

.SUFFIXES: .c .cc .obj .l .y .res .rc .pl # in case the suffix list has been cleared, e.g., by web2c

{}.c{$(objdir)}.obj:
	$(compile) $<

{}.cc{$(objdir)}.obj:
	$(compile) /EHsc /GR /Tp $<

{}.cpp{$(objdir)}.obj:
	$(compile) /EHsc /GR /Tp $<

{}.pl{$(objdir)}.exe:
	$(perlcomp) $(perlcompflags) $< >>$(win32perldir)\perlfiles.lst
	irun -f $< $(bindir)\$(<:.pl=.exe)

.l.c:
	$(lex) -t $< | $(SED) "/^extern int isatty YY/d" > $@

.y.c:
	$(yacc) -d -v $< -o $@

{$(win32rcdir)}.rc{$(objdir)}.res:
	$(RC) $(rcvars) $(rcflags) /fo$@ $<

{}.rc{$(objdir)}.res:
	$(RC) $(rcvars) $(rcflags) /fo$@ $<

# We might link :
# - by default for console programs
# - for building dlls
# - for building gui apps
link = $(CCLD) /OUT:$@ $(conlflags)
link_dll = $(CCLD) /OUT:$@ $(dlllflags)
link_gui = $(CCLD) /OUT:$@ $(guilflags)

conlibs = $(libc) user32.lib $(XLIBS)
guilibs = $(libc) $(winlibs) $(XLIBS)

# for OLE applications
olelibs      = ole32.lib uuid.lib oleaut32.lib

archive = $(AR) -nologo $(libflags) /OUT:$@

################################################################################
#
# Various paths definitions
#
################################################################################
!ifdef GNU
!include <msvc/gnupaths.mak>
!elseifdef NETPBM
!include <msvc/netpbmpaths.mak>
!elseifdef XEMTEX
!include <msvc/xemtexpaths.mak>
!elseifdef XEMACS
!include <msvc/xemacspaths.mak>
!else
!include <msvc/paths.mak>
!endif

################################################################################
#
# Various personal libraries definitions
#
################################################################################
!include <msvc/libsdef.mak>

PATH = $(bindir);$(bindir:-static=);$(kpathseadir)\$(objdir);.\$(objdir);c:\Local\ActiveState\Perl\bin;$(PATH)

!ifdef XEMTEX
TEXMFCNF = $(texprefix)/texmf/web2c
TEXMFMAIN = $(texprefix)/texmf
TEXMFDIST = $(texprefix)/texmf-dist
VARTEXMF = $(texprefix)/texmf-var
!else
TEXMFCNF = $(texmf)/web2c
TEXMFMAIN = $(texmf)
TEXMFDIST = $(prefix)/texmf-dist
VARTEXMF = $(prefix)/texmf-var
!endif
MKTEXFMT = 1
TEXMF =

#
# Local Variables:
# mode: Makefile
# End:
