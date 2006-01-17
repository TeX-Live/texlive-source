# Makefile fragment for Omega and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of Omega.

Makefile: $(srcdir)/omegadir/omega.mk

omegaware_programs = otangle odvicopy odvitype

omega_programs = $(omega) $(otangle) $(odvicopy) $(odvitype) \
                 $(otps_programs) $(omegafonts_programs)

# The C sources.
omega_c = omegaini.c omega0.c omega1.c omega2.c omega3.c
omegaware_c = odvicopy.c odvitype.c otangle.c
omega_o = omegaini.o omega0.o omega1.o omega2.o omega3.o \
	      omegaextra.o omega.o omegabis.o

# Generation of the web and ch files.
odvicopy.web: omegaware/odvicopy.web
	rm -f $@
	$(LN) $(srcdir)/omegaware/odvicopy.web $@
odvicopy.ch: omegaware/odvicopy.ch
	rm -f $@
	$(LN) $(srcdir)/omegaware/odvicopy.ch $@
odvitype.web: omegaware/odvitype.web
	rm -f $@
	$(LN) $(srcdir)/omegaware/odvitype.web $@
odvitype.ch: omegaware/odvitype.ch
	rm -f $@
	$(LN) $(srcdir)/omegaware/odvitype.ch $@
omega.web: tie tex.web
omega.web: omegadir/om16bit.ch
omega.web: omegadir/omstr.ch
omega.web: omegadir/omfont.ch
omega.web: omegadir/omchar.ch
omega.web: omegadir/omfi.ch
omega.web: omegadir/ompar.ch
omega.web: omegadir/omocp.ch
omega.web: omegadir/omfilter.ch
omega.web: omegadir/omtrans.ch
omega.web: omegadir/omdir.ch
omega.web: omegadir/omxml.ch
	./tie -m omega.web $(srcdir)/tex.web \
	 $(srcdir)/omegadir/om16bit.ch \
	 $(srcdir)/omegadir/omstr.ch \
	 $(srcdir)/omegadir/omfont.ch \
	 $(srcdir)/omegadir/omchar.ch \
	 $(srcdir)/omegadir/omfi.ch \
	 $(srcdir)/omegadir/ompar.ch \
	 $(srcdir)/omegadir/omocp.ch \
	 $(srcdir)/omegadir/omfilter.ch \
	 $(srcdir)/omegadir/omtrans.ch \
	 $(srcdir)/omegadir/omdir.ch \
	 $(srcdir)/omegadir/omxml.ch
omega.ch: tie omega.web
omega.ch: omegadir/com16bit.ch
omega.ch: omegadir/comstr.ch
omega.ch: omegadir/comfont.ch
omega.ch: omegadir/comchar.ch
omega.ch: omegadir/comfi.ch
omega.ch: omegadir/compar.ch
omega.ch: omegadir/comocp.ch
omega.ch: omegadir/comfilter.ch
omega.ch: omegadir/comtrans.ch
omega.ch: omegadir/comdir.ch
omega.ch: omegadir/comxml.ch
omega.ch: omegadir/comsrcspec.ch
	./tie -c omega.ch omega.web \
	 $(srcdir)/omegadir/com16bit.ch \
	 $(srcdir)/omegadir/comstr.ch \
	 $(srcdir)/omegadir/comfont.ch \
	 $(srcdir)/omegadir/comchar.ch \
	 $(srcdir)/omegadir/comfi.ch \
	 $(srcdir)/omegadir/compar.ch \
	 $(srcdir)/omegadir/comocp.ch \
	 $(srcdir)/omegadir/comfilter.ch \
	 $(srcdir)/omegadir/comtrans.ch \
	 $(srcdir)/omegadir/comdir.ch \
	 $(srcdir)/omegadir/comxml.ch \
	 $(srcdir)/omegadir/comsrcspec.ch
otangle.web: omegaware/otangle.web
	rm -f $@
	$(LN) $(srcdir)/omegaware/otangle.web $@
otangle.ch: omegaware/otangle.ch
	rm -f $@
	$(LN) $(srcdir)/omegaware/otangle.ch $@

# Bootstrapping otangle requires making it with itself.
otangle: otangle.o
	$(kpathsea_link) otangle.o $(LOADLIBES)
	$(MAKE) $(common_makeargs) otangleboot.p
# otangle.p is a special case, since it is needed to compile itself.  We
# convert and compile the (distributed) otangleboot.p to make a otangle
# which we use to make the other programs.
otangle.p: otangleboot otangle.web otangle.ch
	$(shared_env) ./otangleboot otangle.web otangle.ch

otangleboot: otangleboot.o
	$(kpathsea_link) otangleboot.o $(LOADLIBES)
otangleboot.c otangleboot.h: stamp-otangle $(web2c_programs) $(web2c_aux)
	$(web2c) otangleboot
# omegaware/otangleboot.p is in the distribution.
stamp-otangle: omegaware/otangleboot.p
	rm -f otangleboot.p
	$(LN) $(srcdir)/omegaware/otangleboot.p otangleboot.p
	date >stamp-otangle
# This is not run unless otangle.web or otangle.ch is changed.
otangleboot.p: omegaware/otangle.web omegaware/otangle.ch
	$(shared_env) ./otangle otangle.web otangle.ch
	test -d omegaware || mkdir omegaware
	mv otangle.p omegaware/otangleboot.p
	rm -f otangleboot.p
	$(LN) omegaware/otangleboot.p otangleboot.p
	date >stamp-otangle
	$(MAKE) $(common_makeargs) otangle

# Three additional files
omega.c: omegadir/omega.c 
	rm -f $@
	$(LN) $(srcdir)/omegadir/omega.c $@
omegabis.c: omegadir/omegabis.c 
	rm -f $@
	$(LN) $(srcdir)/omegadir/omegabis.c $@
omegamem.h: omegadir/omegamem.h
	rm -f $@
	$(LN) $(srcdir)/omegadir/omegamem.h $@

# Some additional programs for Omega: the programs themselves are named
# in the variable otps_programs, defined above.
otps/otp2ocp:
	cd otps && $(MAKE) $(common_makeargs) otp2ocp
otps/outocp:
	cd otps && $(MAKE) $(common_makeargs) outocp
omegafonts/omfonts:
	cd omegafonts && $(MAKE) $(common_makeargs) omfonts

install-omega: install-omega-exec install-omega-data
install-omega-exec:: install-omega-links
install-omega-data:: install-omega-dumps

# The actual binary executables and pool files.
install-omega-programs: $(omega_programs)
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(bindir)
	for p in omega; do $(INSTALL_LIBTOOL_PROG) $(bindir) $$p; done
	cd otps && $(MAKE) $(install_makeargs) install-programs
	cd omegafonts && $(MAKE) $(install_makeargs) install-programs

# The links to {mf,mp,tex} for each format and for {ini,vir}{mf,mp,tex},
# plus the equivalents for e-TeX, Omega, pdfTeX, and pdfeTeX.
install-omega-links: install-omega-programs
	cd omegafonts && $(MAKE) $(install_makeargs) install-links
	cd $(bindir) && (rm -f iniomega viromega; \
	  $(LN) omega iniomega; $(LN) omega viromega)
# The ugly "" avoids a syntax error in case the lists are empty.
        test -z "$(ofmts)" || \
          for f in ""$(ofmts); do base=`basename $$f .fmt`; \
            (cd $(bindir) && (rm -f $$base; $(LN) omega $$base)); done

# Always do plain.*, so examples from the TeXbook (etc.) will work.
install-omega-dumps: install-omega-fmts
install-omega-fmts: ofmts
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(fmtdir)
	test -z "$(all_ofmts)" || \
	  for f in ""$(all_ofmts); \
	    do $(INSTALL_DATA) $$f $(fmtdir)/$$f; done

# Auxiliary files.
install-omega-data::
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(texpooldir) 
	$(INSTALL_DATA) omega.pool $(texpooldir)/omega.pool
	$(INSTALL_DATA) $(srcdir)/tiedir/tie.1 $(mandir)/tie.$(manext)

# end of omega.mk
