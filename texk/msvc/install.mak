################################################################################
#
# Makefile  : Install, rules to install things
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/02/10 11:31:46 popineau>
#
################################################################################

installdirs = "$(bindir)" "$(etcdir)" "$(libdir)" "$(includedir)" $(installdirs)

install:: all install-dirs

install-exec::
!if ("$(programs)" != "" || "$(scripts)" != "")
	@echo $(verbose) & ( \
#		if "$(programs) $(scripts)" neq " " ( \
		echo "Installing program files in $(MAKEDIR)" & \
		for %i in ($(programs) $(scripts)) do copy %i "$(bindir)\%~nxi"  $(redir_stdout) \
#		) \
	)
!endif

install-lib::
!if ("$(libfiles)" != "")
	@echo $(verbose) & ( \
#		if "$(libfiles)" neq "" ( \
		echo "Installing library files in $(MAKEDIR)" & \
		for %i in ($(libfiles)) do copy %i "$(libdir)\%~nxi"  $(redir_stdout) \
#		) \
	)
!endif

install-include::
!if ("$(includefiles)" != "")
	@echo $(verbose) & ( \
#		if "$(includefiles)" neq "" ( \
		echo "Installing include files in $(MAKEDIR)" & \
		for %i in ($(includefiles)) do copy %i "$(includedir)\%~nxi"  $(redir_stdout) \
#		) \
	)
!endif

install-data::
!if ("$(etcfiles)" != "")
	@echo $(verbose) & ( \
#		if "$(etcfiles)" neq "" ( \
		echo "Installing data files in $(MAKEDIR)" & \
		for %i in ($(etcfiles)) do copy %i "$(etcdir)\%~nxi" $(redir_stdout) \
#		) \
	)
!endif

#
# Add manual stuff
#
!include <msvc/man.mak>

#
# Add info stuff
#
!include <msvc/texi.mak>

"$(objdir)" :
    -@echo $(verbose) & ( \
	if not exist "$(objdir)/$(null)" mkdir "$(objdir)" \
	)

install-dirs::
#!if ($(installdirs) != "")
	-@echo $(verbose) & ( \
		echo "Building install directories in $(MAKEDIR)" & \
		for %i in ( $(installdirs:/=\) ) do \
	          if %i neq "" ( \
	            $(mkdir) %i $(redir_stderr) \
		  ) \
	)
#!endif

# End of install.mak
#
# Local Variables:
# mode: makefile
# End:
