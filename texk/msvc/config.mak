################################################################################
#
# Makefile  : Coonfig, rules to build configuration files
#           : Perl required, relies on Perl script
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/07/18 10:10:28 popineau>
#
################################################################################

!if exist(config.h.in)

config.h: config.h.in
	$(perl) $(win32perldir)\conf-cauto.pl config.h.in $@

!else if exist(config.hin)

config.h: config.hin
	$(perl) $(win32perldir)\conf-cauto.pl config.hin $@

!else if exist(config.in)

config.h: config.in
	$(perl) $(win32perldir)\conf-cauto.pl config.in $@

!endif

configure:: config.h

!if exist(c-auto.in)

c-auto.h: c-auto.in
	$(perl) $(win32perldir)\conf-cauto.pl c-auto.in $@

configure:: c-auto.h

!endif

!if defined(USE_KPATHSEA) && !defined(MAKE_KPATHSEA)
$(kpathseadir)\c-auto.h: $(kpathseadir)\c-auto.in
	$(perl) $(win32perldir)\conf-cauto.pl $(kpathseadir)\c-auto.in $@
!endif
#
# Add version information
#
!include <msvc/version.mak>

#
# Add libraries build rules
#
!include <msvc/libsrul.mak>

# End of config.mk.
#
# Local Variables:
# mode: Makefile
# End:
