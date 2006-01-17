# Makefile fragment for Omega and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of Omega.

eomegadir = eomegadir
omegadir = omegadir-1.15

Makefile: $(eomegadir)/eomega.mk

# e-Omega itself.

# We build eomega
eomega =@EOMEGA@ eomega

# The C sources.
eomega_c = eomegaini.c eomega0.c eomega1.c eomega2.c eomega3.c
eomega_o = eomegaini.o eomega0.o eomega1.o eomega2.o \
	      eomegaextra.o eomega.o eomegabis.o \
              eomega3.o

# Linking
eomega: $(eomega_o)
	$(kpathsea_link) $(eomega_o) $(socketlibs) $(LOADLIBES)

# The C files
$(eomega_c) eomegacoerce.h eomegad.h: eomega.p $(web2c_texmf)
	$(web2c) eomega
eomegaextra.c: lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/eomega/ $(srcdir)/lib/texmfmp.c >$@
# Additional C files, just copy them.
eomega.c: $(omegadir)/omega.c
	-rm -f $@
	cp $(srcdir)/$(omegadir)/omega.c $@
eomegabis.c: $(omegadir)/omegabis.c 
	-rm -f $@
	cp $(srcdir)/$(omegadir)/omegabis.c $@
eomegamem.h: $(omegadir)/omegamem.h
	-rm -f $@
	cp $(srcdir)/$(omegadir)/omegamem.h $@

# The Pascal file
eomega.p eomega.pool: otangle eomega.web eomega.ch
	./otangle eomega.web eomega.ch

# Generation of the web and ch files.
eomega.web: tie tex.web $(eomegadir)/eomega.mk
eomega.web: $(omegadir)/om16bit.ch
eomega.web: $(omegadir)/omstr.ch
eomega.web: $(omegadir)/omfont.ch
eomega.web: $(omegadir)/omchar.ch
eomega.web: $(omegadir)/omfi.ch
eomega.web: $(omegadir)/ompar.ch
eomega.web: $(omegadir)/omocp.ch
eomega.web: $(omegadir)/omfilter.ch
eomega.web: $(omegadir)/omtrans.ch
eomega.web: $(omegadir)/omdir.ch
eomega.web: $(eomegadir)/eocprt.ch
eomega.web: $(eomegadir)/eobase.ch
eomega.web: $(eomegadir)/eomem-rc1.ch
eomega.web: $(eomegadir)/eo16bit-rc1.ch
eomega.web: $(eomegadir)/eofix.ch
eomega.web: $(eomegadir)/eofix2.ch
eomega.web: $(eomegadir)/eoeqtb.ch
eomega.web: $(eomegadir)/eonewdir-rc1.ch
eomega.web: $(eomegadir)/eover-rc1.ch
	./tie -m eomega.web $(srcdir)/tex.web \
	 $(srcdir)/$(omegadir)/om16bit.ch \
	 $(srcdir)/$(omegadir)/omstr.ch \
	 $(srcdir)/$(omegadir)/omfont.ch \
	 $(srcdir)/$(omegadir)/omchar.ch \
	 $(srcdir)/$(omegadir)/omfi.ch \
	 $(srcdir)/$(omegadir)/ompar.ch \
	 $(srcdir)/$(omegadir)/omocp.ch \
	 $(srcdir)/$(omegadir)/omfilter.ch \
	 $(srcdir)/$(omegadir)/omtrans.ch \
	 $(srcdir)/$(omegadir)/omdir.ch \
	 $(srcdir)/$(eomegadir)/eocprt.ch \
	 $(srcdir)/$(eomegadir)/eobase.ch \
	 $(srcdir)/$(eomegadir)/eomem-rc1.ch \
	 $(srcdir)/$(eomegadir)/eo16bit-rc1.ch \
	 $(srcdir)/$(eomegadir)/eofix.ch \
	 $(srcdir)/$(eomegadir)/eofix2.ch \
	 $(srcdir)/$(eomegadir)/eoeqtb.ch \
	 $(srcdir)/$(eomegadir)/eonewdir-rc1.ch \
	 $(srcdir)/$(eomegadir)/eover-rc1.ch
eomega.ch: tie eomega.web $(eomegadir)/eomega.mk
eomega.ch: $(eomegadir)/omega.ch0
eomega.ch: $(omegadir)/com16bit.ch
eomega.ch: $(omegadir)/comstr.ch
eomega.ch: $(omegadir)/comfont.ch
eomega.ch: $(omegadir)/comchar.ch
eomega.ch: $(omegadir)/comfi.ch
eomega.ch: $(omegadir)/compar.ch
eomega.ch: $(omegadir)/comocp.ch
eomega.ch: $(omegadir)/comfilter.ch
eomega.ch: $(omegadir)/comtrans.ch
eomega.ch: $(omegadir)/comdir.ch
eomega.ch: $(omegadir)/comsrcspec.ch
eomega.ch: $(eomegadir)/omega.ch1
eomega.ch: $(eomegadir)/eomega.ech
eomega.ch: $(eomegadir)/omega.ch2
	./tie -c eomega.ch eomega.web \
	 $(srcdir)/$(eomegadir)/omega.ch0 \
	 $(srcdir)/$(omegadir)/com16bit.ch \
	 $(srcdir)/$(omegadir)/comstr.ch \
	 $(srcdir)/$(omegadir)/comfont.ch \
	 $(srcdir)/$(omegadir)/comchar.ch \
	 $(srcdir)/$(omegadir)/comfi.ch \
	 $(srcdir)/$(omegadir)/compar.ch \
	 $(srcdir)/$(omegadir)/comocp.ch \
	 $(srcdir)/$(omegadir)/comfilter.ch \
	 $(srcdir)/$(omegadir)/comtrans.ch \
	 $(srcdir)/$(omegadir)/comdir.ch \
	 $(srcdir)/$(omegadir)/comsrcspec.ch \
	 $(srcdir)/$(eomegadir)/omega.ch1 \
	 $(srcdir)/$(eomegadir)/eomega.ech \
	 $(srcdir)/$(eomegadir)/omega.ch2 
# Check: right now all we do is build the format.
check: eomega-check
eomega-check: eomega eomega.eoft
# Cleaning up from building omega
clean:: eomega-clean
eomega-clean:
	$(LIBTOOL) --mode=clean $(RM) eomega
	rm -f $(eomega_c) eomegaextra.c eomegacoerce.h eomegad.h
	rm -f eomega.c eomegabis.c eomegamem.h
	rm -f eomega.p eomega.pool eomega.web eomega.ch


# Dumps.
all_eofmts =@FMU@ eomega.eoft $(eofmts)

dumps: eofmts
eofmts: $(all_eofmts)

eomega.eoft: eomega
	$(dumpenv) $(MAKE) progname=eomega files="omega.tex" prereq-check
	$(dumpenv) ./eomega --ini --progname=eomega --jobname=eomega \\input omega.tex \\dump </dev/null

elambda.eoft: eomega
	$(dumpenv) $(MAKE) progname=elambda files="lambda.tex" prereq-check
	$(dumpenv) ./eomega --ini --progname=elambda --progname=elambda --jobname=elambda \\input lambda.tex </dev/null



# Installation.
install-eomega: install-eomega-exec install-eomega-data
install-eomega-exec: install-eomega-links
install-eomega-data:: install-eomega-dumps
install-eomega-dumps: install-eomega-fmts

# The actual binary executables and pool files.
install-programs: install-eomega-programs
install-eomega-programs: $(eomega) $(bindir)
	for p in eomega; do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done

install-links: install-eomega-links
install-eomega-links: install-eomega-programs
	@FMU@cd $(bindir) && (rm -f inieomega vireomega; \
	@FMU@  $(LN) eomega inieomega; $(LN) eomega vireomega)
	eofmts="$(eofmts)";
	  for f in $$eofmts; do base=`basename $$f .eoft`; \
	    (cd $(bindir) && (rm -f $$base; $(LN) eomega $$base)); done

install-fmts: install-eomega-fmts
install-eomega-fmts: eofmts $(fmtdir)
	eofmts="$(all_eofmts)";
	  for f in $$eofmts; do $(INSTALL_DATA) $$f $(fmtdir)/$$f; done

# Auxiliary files.
install-data install-eomega-data:: $(texpooldir)
@EOMEGA@	$(INSTALL_DATA) eomega.pool $(texpooldir)/eomega.pool

# end of omega.mk
