################################################################################
#
# Makefile  : Web2C / Kpathsea
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/06/22 01:11:48 popineau>
#
################################################################################
root_srcdir=..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# Add -DNO_DEBUG to disable debugging, for vanishingly better performance.

USE_KPATHSEA = 1
USE_GNUW32 = 1
MAKE_KPATHSEA = 1
USE_GETURL = 1
USE_REGEX = 1
USE_GSW32 = 1
USE_INET = 1
#USE_NETWORK = 1
#USE_NETWORK2 = 1
# USE_MKTEX = 1

!include <msvc/common.mak>

kpathseadir=.

{$(root_srcdir)/libs/libgnuw32}.c{$(objdir)}.obj:
	$(compile) $<

{$(root_srcdir)/libs/regex}.c{$(objdir)}.obj:
	$(compile) $<

DEFS = $(DEFS) -DHAVE_CONFIG_H -DHAVE_GETURL \
	-DNO_MULTIPLE_EXTENSIONS -UNORMALIZED \
	-DHASH_SHARED -DHASH_MAPPING_NAME=\"$(library_prefix)_hash_mapping\"

# Put $(objdir)\tex-file.obj first, because it's what depends on the paths, and may
# reduce frustration if the paths are wrong by doing it first.

objects = $(objdir)\tex-file.obj	\
	$(objdir)\absolute.obj		\
	$(objdir)\atou.obj	 	\
	$(objdir)\cnf.obj		\
	$(objdir)\concat.obj		\
	$(objdir)\concat3.obj		\
	$(objdir)\concatn.obj		\
	$(objdir)\db.obj		\
	$(objdir)\debug.obj		\
	$(objdir)\dir.obj		\
	$(objdir)\elt-dirs.obj		\
	$(objdir)\expand.obj		\
	$(objdir)\extend-fname.obj	\
	$(objdir)\find-suffix.obj	\
	$(objdir)\fn.obj		\
	$(objdir)\fontmap.obj		\
	$(objdir)\getopt.obj		\
	$(objdir)\getopt1.obj		\
	$(objdir)\hash.obj		\
	$(objdir)\win32hash.obj		\
	$(objdir)\kdefault.obj		\
	$(objdir)\line.obj		\
	$(objdir)\magstep.obj		\
	$(objdir)\make-suffix.obj	\
	$(objdir)\path-elt.obj		\
	$(objdir)\pathsearch.obj	\
	$(objdir)\proginit.obj		\
	$(objdir)\progname.obj		\
	$(objdir)\readable.obj		\
	$(objdir)\remote.obj		\
	$(objdir)\rm-suffix.obj		\
	$(objdir)\str-list.obj		\
	$(objdir)\str-llist.obj		\
	$(objdir)\tex-glyph.obj		\
	$(objdir)\tex-hush.obj		\
	$(objdir)\tex-make.obj		\
	$(objdir)\tilde.obj		\
	$(objdir)\truncate.obj		\
	$(objdir)\uppercasify.obj	\
	$(objdir)\variable.obj		\
	$(objdir)\version.obj		\
	$(objdir)\xbasename.obj		\
	$(objdir)\xcalloc.obj		\
	$(objdir)\xdirname.obj		\
	$(objdir)\xfopen.obj		\
	$(objdir)\xfseek.obj		\
	$(objdir)\xftell.obj 		\
	$(objdir)\xgetcwd.obj		\
	$(objdir)\xmalloc.obj		\
	$(objdir)\xputenv.obj		\
	$(objdir)\xrealloc.obj		\
	$(objdir)\xstrdup.obj		\
	$(liblobjs)                     \
	$(malloc)

liblobjs = 	\
	$(objdir)\error.obj  	\
	$(objdir)\lookcmd.obj  	\
	$(objdir)\stat.obj 	\
	$(objdir)\system.obj    \
	$(objdir)\popen.obj	\
#	$(objdir)\dirent.obj	\
	$(objdir)\user.obj	\
	$(objdir)\time.obj	\
	$(objdir)\volume.obj	\
	$(objdir)\filename.obj	\
	$(objdir)\home.obj	

programs = \
	$(objdir)\kpsewhich.exe	\
	$(objdir)\kpsecheck.exe	\
	$(objdir)\kpsestat.exe	\
	$(objdir)\access.exe

default: all

all: $(objdir) $(kpathsea) $(programs) info man

lib: $(objdir) $(kpathsealib)

!ifdef KPSE_DLL
DEFS = $(DEFS) -DMAKE_KPSE_DLL

$(kpathsealib): $(objects)
	$(archive) /DEF $(objects)

$(kpathseadll): $(objects) $(objdir)\kpathsea.res $(geturllib) # $(mktexlib)
	$(link_dll) $(**) $(kpathsealib:.lib=.exp) $(conlibs)
!else
$(kpathsealib): $(objects) $(geturllib) # $(mktexlib)
	$(archive) $(**)
!endif

$(objdir)\kpsewhich.exe: $(objdir)\kpsewhich.obj $(objdir)\kpsewhich.res $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\kpsecheck.exe: $(objdir)\kpsecheck.obj $(objdir)\fnmatch.obj $(gsw32lib) $(kpathsealib)
	$(link) $(**) $(conlibs)

$(objdir)\kpsestat.exe: $(objdir)\kpsestat.obj $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\access.exe: $(objdir)\access.obj
	$(link) $(objdir)\access.obj $(conlibs)

$(objdir)\kpsewhich.obj: kpsewhich.c
	$(compile) -UMAKE_KPSE_DLL kpsewhich.c

$(objdir)\kpsecheck.obj: kpsecheck.c
	$(compile) -UMAKE_KPSE_DLL kpsecheck.c

$(objdir)\kpsestat.obj: kpsestat.c
	$(compile) -UMAKE_KPSE_DLL -UKPSE_DLL kpsestat.c

$(objdir)\access.obj: access.c
	$(compile) access.c


# Make variable substitutions for paths.h.
texmf.cnf: texmf.in texmf.sed
	$(sed) -e "/^[^%]/s/:/;/g" texmf.in \
	| $(sed) -e "/^TEXMFMAIN/s/@texmf@/$$SELFAUTOPARENT\/texmf/" \
	| $(sed) -e "/^TEXMFCNF/s/\(.*\);[^;]*;\([^;]*\);@web2c@/\1;\2;@web2c@/" \
	| $(sed) -f texmf.sed \
# Better to ensure c:/TeX is choosen, instead of e:/...
#	| $(sed) -e "s%$(prefix:\=/)%c:/TeX%g" \
	>$@

# makevars.mk -- the directory names we pass.
# It's important that none of these values contain [ @%], for the sake
# of kpathsea/texmf.sed.
makevars = prefix=$(prefix) exec_prefix=$(exec_prefix) \
  bindir=$(bindir) scriptdir=$(scriptdir) libdir=$(libdir) \
  datadir=$(datadir) infodir=$(infodir) includedir=$(includedir) \
  manext=$(manext) mandir=$(mandir) \
  texmf=$(texmf) web2cdir=$(web2cdir) vartexfonts=$(vartexfonts)\
  texinputdir=$(texinputdir) mfinputdir=$(mfinputdir) mpinputdir=$(mpinputdir)\
  fontdir=$(fontdir) fmtdir=$(fmtdir) basedir=$(basedir) memdir=$(memdir) \
  texpooldir=$(texpooldir) mfpooldir=$(mfpooldir) mppooldir=$(mppooldir) \
  dvips_plain_macrodir=$(dvips_plain_macrodir) \
  dvilj_latex2e_macrodir=$(dvilj_latex2e_macrodir) \
  dvipsdir=$(dvipsdir) psheaderdir=$(psheaderdir) \
  default_texsizes="$(default_texsizes)"
# End of makevars.mk.

# The idea is to turn each var=value into s%@var@%value%g. Seems simpler
# to put the substitutions in a file than to play shell quoting games.
texmf.sed: $(win32makedir)/paths.mak
	-@$(del) texmf.sed
	@echo <<makevars.txt
$(makevars)
<<KEEP 
	type makevars.txt \
	| $(sed) -e "s%$(prefix:\=/)%$$PREFIX%g" \
	| $(sed) -e "s/ [ ]*/ /g" -e "s@\\@/@g" \
	| $(tr) " " "\012" \
	| $(sed) -e "s%$$PREFIX%$(prefix:\=/)%g" \
	| $(sed) -f $(win32seddir)/texmfsed.sed \
#	| $(sed)  -e "s/^/s%@/" -e "s/=/@%/" -e "s/$$/%/" -e "s/$$/g/" \
	> $@
# Insert $TEXMF in as many of the hardwired paths as possible.  We could
# use the slightly different rule 's%$(texmf)/%\$$TEXMF/%g' and avoid
# the need for the corrections below.  But if texmf.in is edited
# the heuristic may no longer work.
	echo s%$(texmf:\=/)%$$TEXMF%g>>$@
# Now we repair the damage this may have caused.  Don't replace the rhs of
# the TEXMFMAIN assignment itself, then we'd end up with TEXMFMAIN = $TEXMF
# and TEXMF = $TEXMFMAIN.  The (commented) assignment of TEXMFLOCAL is
# likely to have been mangled as well.
	echo /^ *TEXMFMAIN[ =]/s%\$$TEXMF%$(texmf:\=/)%>>$@
	echo /^[% ]*TEXMFLOCAL[ =]/s%\$$TEXMF%$(texmf:\=/)%>>$@
#	echo /^[% ]*TEXMF_CNF[ =]/s%\$$TEXMF%$(texmf:\=/)%>>$@
#	echo /^[% ]*TEXMFCNF[ =]/s%\$$TEXMF%$(texmf:\=/)%>>$@
# And fill in the last clause of TEXMFCNF with an absolute path.
	echo /^ *TEXMFCNF[ =]/s%@web2c@%$(web2cdir:\=/)%>>$@
	-@$(del) makevars.txt

# First null out comments and leading/trailing whitespace, then remove
# lines that define invalid C identifiers, then remove blank lines and
# lines that define lowercase values (those are never path values). Each
# line remaining looks like
# <name> = <value>
# (but = and surrounding spaces are optional, hence can't remove all spaces).
# Change this to #ifndef DEFAULT_name@#define DEFAULT_name "value"@#endif,
# then change the @'s to newlines (sed isn't good at multiline replacements).
# 
# No backslash-newline escapes in the long sed replacement because that
# will turn into a space in the output.
# 
# Without the $TEXMF/prefix/etc. substitutions, if the cnf file was
# not found, the compile-time paths would be of little use, since TEXMF
# (etc.) wouldn't be defined. Alternatively, we could have a way to
# specify compile-time default values for variables in general, but I
# think it's better to keep the last-resort paths as simple as possible.
# 
# The definition of DEFAULT_TEXMF (and other variables)
# that winds up in the final paths.h will not be used.
#
# We don't want to rewrite paths.h when we have only changed comments
# in texmf.in that have no effect on paths.h, since that would cause
# almost everything to be rebuilt.
#
# We also replace all semicolons with colons in the paths.  Ultimately
# the path-splitting code should be changed to understand both.
$(kpathseadir)/paths.h: stamp-paths
stamp-paths: texmf.cnf
	echo /* paths.h: Generated from texmf.cnf.  */ > paths.tmp
	$(sed) -e "s/%.*//" -e "s/^[ 	]*//" -e "s/[ 	]*$$//" texmf.cnf \
	| $(grep) "^[ 	]*[A-Z0-9_]*[ 	=]" \
	| $(sed) "/^$$/d" \
	| $(sed) "s/^\([^ 	=]*\)[ 	]*=*[ 	]*\(.*\)/#ifndef DEFAULT_\1@#define DEFAULT_\1 \"\2\"@#endif/" \
	| $(tr) @ "\012" \
	| $(sed) -e "s@\$$TEXMFMAIN@$(texmf:\=/)@g" \
	      -e "s@\$$TEXMF@$(texmf:\=/)@g" \
	      -e "s@\$$VARTEXFONTS@c:/tex/fonts@g" \
	      -e "s@\$$web2cdir@$(web2cdir:\=/)@g" \
	      -e "s@\$$prefix@$(prefix:\=/)@g" \
# Better to ensure c:/Program Files/TeXLive is choosen, instead of e:/...
	      -e "s@$(prefix:\=/)@c:/Program Files/TeXLive@g" \
#	| $(sed) -e "s%;%:%g" \
	>>paths.tmp
#	fc paths.h paths.tmp > nul
#	if %ERRORLEVEL% NEQ 0 \
#	  echo copying paths.tmp paths.h\
#	  & $(CP) paths.tmp paths.h
#	$(del) paths.tmp
	$(copy) paths.tmp paths.h & $(del) paths.tmp
	date /t > stamp-paths
	time /t >> stamp-paths

!include <msvc/config.mak>

# Need an extra definition for this. Dependencies included below.
$(objdir)\tex-file.obj:
	$(compile) -DDEFAULT_FONT_SIZES=\"$(default_texsizes)\" tex-file.c

check: $(objdir)\kpsewhich.exe
	.\$(objdir)\kpsewhich -expand-var $$TEXMF

install:: all install-exec install-data

install-exec::
	-@$(mkdir) $(bindir) $(libdir) $(web2cdir) 2>nul
	-@echo $(verbose) & for %i in ($(kpathseadll) $(programs)) do \
		$(copy) %i $(bindir) $(redir_stdout)

install-data:: install-info install-doc install-man kpathsea.h
# Should we install the headers?  They are not just system-dependent,
# which is bad enough, but even compiler-dependent.
	-@$(mkdir) $(includedir)\kpathsea 2>nul
	-@echo $(verbose) & for %f in (*.h) do \
	  $(copy) %f $(includedir)\kpathsea $(redir_stdout)
	-@$(del) $(includedir)\kpathsea\acconfig.h

# I don't use this, but other programmers want it.  acconfig.h is an
# autoheader input file, not an includable C header. Bad name.
kpathsea.h: always
	-@$(del) $@
	@echo off <<$@
#include <kpathsea/config.h>
<<KEEP
	dir /b *.h | grep -v "\(acconfig\|config\|kpathsea\|win32lib\|c-auto\)\.h" \
	  | $(sed) -e "s,^,#include <kpathsea/," -e s",$$,>," >>$@	  
#	touch -r `ls -1t *.h | tail +2 | head -1` $@ 
always:
.PHONY: always

# The manual pages
manfiles = access.1 kpsestat.1 kpsewhich.1 \
           mktexlsr.1 mktexmf.1 mktextfm.1 mktexpk.1

infofiles = kpathsea.info
docsubdir = kpathsea
pdfdocfiles = kpathsea.pdf
htmldocfiles = kpathsea.html

!include <msvc/install.mak>

info:	kpathsea.info
dvi:	kpathsea.dvi
ps:	kpathsea.ps

kpathsea.info: kpathsea.texi bugs.texi hier.texi install.texi unixtex.texi
	-$(makeinfo) $(makeinfo_flags) kpathsea.texi -o $@

!IFDEF MAINT
INSTALL.txt: install.texi
	$(makefinfo) $(one_info) $(MAKEINFO_FLAGS) install.texi -o $@
	$(perl) $(win32perldir)\add-info-toc.pl $@
	$(copy) $@ $(@:.txt=) $(redir_stderr)
BUGS: bugs.texi
	$(makefinfo) $(one_info) $(MAKEINFO_FLAGS) bugs.texi -o $@
	$(perl) $(win32perldir)\add-info-toc.pl $@
HIER: hier.texi
	$(makefinfo) $(one_info) $(MAKEINFO_FLAGS) hier.texi -o $@
unixtex.ftp: unixtex.texi
	$(makefinfo) $(one_info) $(MAKEINFO_FLAGS) unixtex.texi -o $@
doc: info dvi ps HIER BUGS INSTALL unixtex.ftp
!endif

!include <msvc/clean.mak>

extraclean::
	-@echo $(verbose) & ($(del) kpathsea*.html kpathsea*.pdf) 2>nul

distclean::
	-@$(del) paths.h texmf.cnf texmf.sed

!include <msvc/rdepend.mak>
!include "./depend.mak"

# Local Variables:
# mode: Makefile
# End:
