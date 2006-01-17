################################################################################
#
# Makefile  : Clean, remove build files
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/01/07 11:50:03 popineau>
#
################################################################################

!CMDSWITCHES +S

clean::
	-@echo $(verbose) & ( \
	echo Cleaning up exe and obj files in $(MAKEDIR) & \
	$(del) $(objdir)\*.* \
	) $(redir_stderr)

extraclean::
	-@echo $(verbose) & ( \
	echo Cleaning up files in $(MAKEDIR) & \
	$(del) *.aux *.bak *.bbl *.blg *.dvi *.log *.toc *.tfm *.vf *.vpl & \
	$(del) *.[0-9]*gf *.[0-9]*pk *.lj & \
	$(del) *.mpx *.i *.s *~ *.orig  *.rej \#* & \
	$(del) *.pbi *.pbo *.pbt *.pdb & \
	$(del) CONTENTS.tex a.out core mfput.* texput.* mpout.* & \
	$(del) *.cp *.fn *.ky *.pg *.tp *.vr *.op *.fl & \
	$(del) *.cps *.fns *.kys *.pg *.tps *.vrs & \
	$(del) config.status config.cache config.log ) 2>nul

distclean:: clean extraclean configclean
	-@echo $(verbose) & ( \
	echo Cleaning up distribution in $(MAKEDIR) & \
#	$(del) config.status config.log config.cache & \
#	$(del) stamp-* & \
	for %i in ($(objtargets)) do $(deldir) %i ) 2>nul

!if exist(c-auto.in)
configclean::
	-@echo $(verbose) & ( \
	echo Cleaning up configuration in $(MAKEDIR) & \
	$(del) c-auto.h ) 2>nul
!endif

!if exist(config.h.in) || exist(config.hin)
configclean::
	-@echo $(verbose) & ( \
	echo Cleaning up configuration in $(MAKEDIR) & \
	$(del) config.h ) 2>nul
!endif

configclean::

!CMDSWITCHES -S

# End of clean.mk.
#
# Local Variables:
# mode: Makefile
# End: