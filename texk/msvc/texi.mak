################################################################################
#
# Makefile  : texi, build info, pdf and html files out of texinfo files
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/20 15:52:19 popineau>
#
################################################################################

# paths to doc directories

!ifndef docdir
docdir = $(texmf)\doc
!endif

!ifndef infodir
infodir = $(docdir)\info
!endif

!ifndef htmldocdir
htmldocdir = $(docdir)\$(docsubdir)
!endif

!ifndef pdfdocdir
pdfdocdir = $(docdir)\$(docsubdir)
!endif

#
# Only if infofiles has been defined
#

!if ("$(infofiles)" != "" || "$(pdfdocfiles)" != "" || "$(htmldocfiles)" != "")

makeinfo = makeinfo
makeinfo_flags = --paragraph-indent=2 --no-split -I$(srcdir)

texi2html = perl $(texk_srcdir)\tetex\texi2html
texi2html_flags = -expandinfo -number -menu -split_chapter
# If you prefer one big .html file instead of several, remove
# -split-node or replace it by -split_chapter.

# For making normal text files out of Texinfo source.
one_info = --no-headers --no-split --no-validate

dvips = dvips
dvipsflags =
texflags=-interaction=batchmode

.SUFFIXES: .info .dvi .html .texi .txi .pdf .tex .texinfo

.texinfo.texi:
	-@$(copy) $< $@

.texi.info:
	-$(silent)$(makeinfo) $(makeinfo_flags) $< -o $@

.texi.dvi:
#	$(TEXI2DVI) $(TEXI2DVI_FLAGS) $<
	-@echo $(verbose) & echo "Compiling $@ out of $<" & ( \
	tex $(texflags) $< & \
	tex $(texflags) $< & \
	texindex $(<:.texi=.??) & \
	tex $(texflags) $< ) $(redir_stdout)
	-@$(del) $(<:.texi=.aux) $(<:.texi=.log) $(<:.texi=.toc)

.texi.html:
	$(silent)$(texi2html) $(texi2html_flags) $< 

.texi.pdf:
	-@echo $(verbose) & echo "Compiling $@ out of $<" & ( \
	pdftex $(texflags) $< & \
	pdftex $(texflags) $< & \
	texindex $(<:.texi=.??) & \
	pdftex $(texflags) $< )
	-@$(del) $(<:.texi=.aux) $(<:.texi=.log) $(<:.texi=.toc)

.txi.info:
	-$(makeinfo) $(makeinfo_flags) $< -o $@

.txi.dvi:
#	$(TEXI2DVI) $(TEXI2DVI_FLAGS) $<
	-tex $(texflags) $<
	-tex $(texflags) $<
	-texindex $(<:.txi=.??)
	-tex $(texflags) $<
	$(del) $(<:.txi=.aux) $(<:.txi=.log) $(<:.txi=.toc)

.txi.html:
	$(texi2html) $(texi2html_flags) $< 

.txi.pdf:
	-pdftex $(texflags) $<
	-pdftex $(texflags) $<
	-texindex $(<:.txi=.??)
	-pdftex $(texflags) $<
	-@$(del) $(<:.txi=.aux) $(<:.txi=.log) $(<:.txi=.toc)

.tex.pdf:
	-pdflatex $(texflags) $<
	-pdflatex $(texflags) $<

.tex.html:
	-if NOT exist "$(htmldocdir)" mkdir "$(htmldocdir)"
	-htlatex --force --output-dir "$(htmldocdir)" $<

.dvi.ps:
	-$(dvips) $(dvipsflags) $< -o

doc: $(infofiles) $(htmldocfiles) $(pdfdocfiles)

installdirs = $(installdirs) "$(infodir)" "$(docdir)" "$(htmldocdir)" "$(pdfdocdir)"

install:: install-info install-doc

install-info:: $(infofiles)
!if !exist("$(infodir)\dir") && exist($(texk_srcdir)\dir-example)
	if NOT EXIST "$(infodir)\dir" \
	    $(copy) $(texk_srcdir)\dir-example "$(infodir)\dir"
!endif
	-@echo $(verbose) & ( \
		for %i in ($(infofiles)) do ( \
			$(copy) %i "$(infodir)\%i" & \
			install-info --version > nul & \
		if ERRORLEVEL 0 \
	 		install-info --info-dir="$(infodir)" $(installinfoflags) "$(infodir)/%i" \
		) \
	)

install-doc:: $(pdfdocfiles) $(htmldocfiles)
	-@echo $(verbose) & ( \
		if "$(pdfdocfiles)" neq "" ( \
			for %i in ($(pdfdocfiles)) do ( \
				$(copy) %i "$(pdfdocdir)\%i" $(redir_stdout) ) ) & \
		if "$(htmldocfiles)" neq "" ( \
			for %i in ($(htmldocfiles:.html=*.html)) do ( \
				$(copy) %i "$(htmldocdir)\%i" $(redir_stdout) ) ) \
	)

extraclean::
	-@echo $(verbose) & ( \
	for %i in (cm cp fl fn ky op pg toc tp vr) do \
	$(del) *.%i *.%is ) 2>nul

distclean::
	-@echo $(verbose) & ( \
		for %i in ($(infofiles)) do $(del) %i $(redir_stderr) \
	)
	-@echo $(verbose) & ( \
		for %i in ($(htmldocfiles:.html=*.html)) do $(del) %i $(redir_stderr) \
	)
	-@echo $(verbose) & ( \
		for %i in ($(pdfdocfiles)) do $(del) %i $(redir_stderr) \
	)
	-@$(del) *.info-? *.info-??

!else
#
# Do nothing if no info files
#
doc:

install-info::

install-doc::

!endif

# End of texi.mak.
#
# Local Variables:
# mode: Makefile
# End:
