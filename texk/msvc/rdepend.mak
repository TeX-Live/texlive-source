################################################################################
#
# Makefile  : rdepend, rules to build depend.mak dependencies
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/08/19 12:12:54 popineau>
#
################################################################################
#
# Have to use -M, not -MM, since we use <kpathsea/...> instead of
# "kpathsea/..." in the sources.  But that means we have to remove the
# directory prefixes and all the system include files.
# And <kpathsea/paths.h> is generated, not part of the distribution.
# 
# And, there's no need for any installer/user to ever run this, it can
# only cause trouble. So comment it out in the distribution.
# (It doesn't work when the source and build directories are different.)
!if exist(c-auto.in)
c_auto_h_dir = .
!else
c_auto_h_dir = $(kpathseadir)
!endif

makedepend_files = $(makedepend_files) *.c *.cc

makedepend_include = $(makedepend_include) -I"$(MSVCROOT:\=/)/include" -I"$(MSVCROOT:\=/)/PlatformSDK/include"

depend:: $(c_auto_h_dir)/c-auto.h $(win32makedir)/rdepend.mak
	$(makedepend) -f- -p"$$(objdir)\\" $(makedepend_cflags) -I$(c_auto_h_dir) $(makedepend_include) $(makedepend_files) \
	| $(sed) \
		-e "s,\\,/,g" \
		-e "s,/[ 	]*$$,\\," \
	        -e "s,$(MSVCROOT:\=/)/include/[^ ]*,,g" \
	        -e "s,$(MSVCROOT:\=/)/PlatformSDK/include/[^ ]*,,g" \
!ifdef kpathseadir
		-e "s,$(kpathseadir:\=/)/\.\./kpathsea/,$$(kpathseadir)/,g" \
!if "$(kpathseadir)" != "."
		-e "s,$(kpathseadir:\=/)/,$$(kpathseadir)/,g" \
!endif
!endif
!ifdef bmepsdir
		-e "s,$(bmepsdir:\=/)/,$$(bmepsdir)/,g" \
!endif
!ifdef bzip2dir
		-e "s,$(bzip2dir:\=/)/,$$(bzip2dir)/,g" \
!endif
!ifdef compfacedir
		-e "s,$(compfacedir:\=/)/,$$(compfacedir)/,g" \
!endif
!ifdef curldir
		-e "s,$(curldir:\=/)/,$$(curldir)/,g" \
!endif
!ifdef expatdir
		-e "s,$(expatdir:\=/)/,$$(expatdir)/,g" \
!endif
!ifdef freetypedir
		-e "s,$(freetypedir:\=/)/,$$(freetypedir)/,g" \
!endif
!ifdef freetype2dir
		-e "s,$(freetype2dir:\=/)/,$$(freetype2dir)/,g" \
!endif
!ifdef geturldir
		-e "s,$(geturldir:\=/)/,$$(geturldir)/,g" \
!endif
!ifdef gifdir
		-e "s,$(gifdir:\=/)/,$$(gifdir)/,g" \
!endif
!ifdef gnuw32dir
		-e "s,$(gnuw32dir:\=/)/,$$(gnuw32dir)/,g" \
!endif
!ifdef gsw32dir
		-e "s,$(gsw32dir:\=/)/,$$(gsw32dir)/,g" \
!endif
!ifdef jpegdir
		-e "s,$(jpegdir:\=/)/,$$(jpegdir)/,g" \
!endif
!ifdef md5dir
		-e "s,$(md5dir:\=/)/,$$(md5dir)/,g" \
!endif
!ifdef mktexdir
		-e "s,$(mktexdir:\=/)/,$$(mktexdir)/,g" \
!endif
!ifdef netpbmdir
		-e "s,$(netpbmdir:\=/)/,$$(netpbmdir)/,g" \
!endif
!ifdef pngdir
		-e "s,$(pngdir:\=/)/,$$(pngdir)/,g" \
!endif
!ifdef regexdir
		-e "s,$(regexdir:\=/)/,$$(regexdir)/,g" \
!endif
!ifdef t1dir
		-e "s,$(t1dir:\=/)/,$$(t1dir)/,g" \
!endif
!ifdef tiffdir
		-e "s,$(tiffdir:\=/)/,$$(tiffdir)/,g" \
!endif
!ifdef ttfdir
		-e "s,$(ttfdir:\=/)/,$$(ttfdir)/,g" \
!endif
!ifdef unzipdir
		-e "s,$(unzipdir:\=/)/,$$(unzipdir)/,g" \
!endif
!ifdef wmfdir
		-e "s,$(wmfdir:\=/)/,$$(wmfdir)/,g" \
!endif
!ifdef wwwdir
		-e "s,$(wwwdir:\=/)/,$$(wwwdir)/,g" \
!endif
!ifdef xpdfdir
		-e "s,$(xpdfdir:\=/)/,$$(xpdfdir)/,g" \
!endif
!ifdef xpmdir
		-e "s,$(xpmdir:\=/)/,$$(xpmdir)/,g" \
!endif
!ifdef zlibdir
		-e "s,$(zlibdir:\=/)/,$$(zlibdir)/,g" \
!endif
	        -e "s,$$(objdir)\\dvi2xx.obj,$$(objdir)\\dvilj.obj $$(objdir)\\dvilj2p.obj $$(objdir)\\dvilj4.obj $$(objdir)\\dvilj4l.obj," \
	        -e "s,lex.yy,$$(LEX_OUTPUT_ROOT),g" \
	| $(perl) $(win32perldir)/fix-depend.pl \
	| $(grep) -ve "^[ 	]*\\$$" \
	| $(awk) -f $(texk_srcdir)/unbackslsh.awk \
	| sed -e "s/texk\.development/texk/g" \
	  >depend.mak
# If kpathsea, we're making .lo library objects instead of .o's.
#	pwd | grep -v kpathsea >/dev/null \
#	  || (sed -e 's/\.o:/.lo:/' -e 's/kpsewhich.lo:/kpsewhich.o:/' \
#	      <depend.mk >depend-tmp.mk; mv depend-tmp.mk depend.mk)

# Let's stick a rule for TAGS here, just in case someone wants them.
# (We don't put them in the distributions, to keep them smaller.)
#TAGS: *.c *.h
#	pwd | grep kpathsea >/dev/null && append=../kpathsea/TAGS; \
#	  etags $$append *.[ch]

depend.mak: depend

# Prevent GNU make 3.[59,63) from overflowing arg limit on system V.
.NOEXPORT:

# End of rdepend.mak.
#
# Local Variables:
# mode: Makefile
# End:
