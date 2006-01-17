################################################################################
#
# Makefile  : version, dependencies on resource files 
#	    : holding version information
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/01/18 01:13:30 popineau>
#
################################################################################

$(win32makedir)\texlive.version:	$(win32makedir)\texlive-version.h

$(objdir)\afm2tfm.res:	$(win32rcdir)\afm2tfm.rc	$(win32makedir)\texlive.version
$(objdir)\bibtex.res:	$(win32rcdir)\bibtex.rc		$(win32makedir)\texlive.version
$(objdir)\dmp.res:	$(win32rcdir)\dmp.rc		$(win32makedir)\texlive.version
$(objdir)\dvicopy.res:	$(win32rcdir)\dvicopy.rc	$(win32makedir)\texlive.version
$(objdir)\dvihp.res:	$(win32rcdir)\dvihp.rc		$(win32makedir)\texlive.version
$(objdir)\dvilj.res:	$(win32rcdir)\dvilj.rc		$(win32makedir)\texlive.version
$(objdir)\dvips.res:	$(win32rcdir)\dvips.rc		$(win32makedir)\texlive.version
$(objdir)\dvitomp.res:	$(win32rcdir)\dvitomp.rc	$(win32makedir)\texlive.version
$(objdir)\dvitype.res:	$(win32rcdir)\dvitype.rc	$(win32makedir)\texlive.version
$(objdir)\etex.res:	$(win32rcdir)\etex.rc		$(win32makedir)\texlive.version
$(objdir)\gftodvi.res:	$(win32rcdir)\gftodvi.rc	$(win32makedir)\texlive.version
$(objdir)\gftopk.res:	$(win32rcdir)\gftopk.rc		$(win32makedir)\texlive.version
$(objdir)\gftype.res:	$(win32rcdir)\gftype.rc		$(win32makedir)\texlive.version
$(objdir)\kpathsea.res:	$(win32rcdir)\kpathsea.rc	$(win32makedir)\texlive.version
$(objdir)\makeindex.res:	$(win32rcdir)\makeindex.rc	$(win32makedir)\texlive.version
$(objdir)\makempx.res:	$(win32rcdir)\makempx.rc	$(win32makedir)\texlive.version
$(objdir)\maketex.res:	$(win32rcdir)\maketex.rc	$(win32makedir)\texlive.version
$(objdir)\mf.res:	$(win32rcdir)\mf.rc		$(win32makedir)\texlive.version
$(objdir)\mft.res:	$(win32rcdir)\mft.rc		$(win32makedir)\texlive.version
$(objdir)\mpost.res:	$(win32rcdir)\mpost.rc		$(win32makedir)\texlive.version
$(objdir)\mpto.res:	$(win32rcdir)\mpto.rc		$(win32makedir)\texlive.version
$(objdir)\patgen.res:	$(win32rcdir)\patgen.rc		$(win32makedir)\texlive.version
$(objdir)\pdftex.res:	$(win32rcdir)\pdftex.rc		$(win32makedir)\texlive.version
$(objdir)\pktogf.res:	$(win32rcdir)\pktogf.rc		$(win32makedir)\texlive.version
$(objdir)\pktype.res:	$(win32rcdir)\pktype.rc		$(win32makedir)\texlive.version
$(objdir)\pltotf.res:	$(win32rcdir)\pltotf.rc		$(win32makedir)\texlive.version
$(objdir)\pooltype.res:	$(win32rcdir)\pooltype.rc	$(win32makedir)\texlive.version
$(objdir)\tangle.res:	$(win32rcdir)\tangle.rc		$(win32makedir)\texlive.version
$(objdir)\tex.res:	$(win32rcdir)\tex.rc		$(win32makedir)\texlive.version
$(objdir)\tie.res:	$(win32rcdir)\tie.rc		$(win32makedir)\texlive.version
$(objdir)\tftopl.res:	$(win32rcdir)\tftopl.rc		$(win32makedir)\texlive.version
$(objdir)\vftovp.res:	$(win32rcdir)\vftovp.rc		$(win32makedir)\texlive.version
$(objdir)\vptovf.res:	$(win32rcdir)\vptovf.rc		$(win32makedir)\texlive.version
$(objdir)\weave.res:	$(win32rcdir)\weave.rc		$(win32makedir)\texlive.version
$(objdir)\web2c.res:	$(win32rcdir)\web2c.rc		$(win32makedir)\texlive.version

# End of version.mak
#
# Local Variables:
# mode:	makefile
# End: