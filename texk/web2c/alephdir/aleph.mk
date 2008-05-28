# Makefile fragment for Aleph and web2c. --infovore@xs4all.nl. Public domain.
# This fragment contains the parts of the makefile that are most likely to
# differ between releases of Aleph.

alephdir = alephdir
calephdir = alephdir

Makefile: $(alephdir)/aleph.mk
# 
# Aleph itself.

# We build aleph
aleph = @ALEPH@ aleph
alephbuild=rc2

# The C sources.
aleph_c = alephini.c aleph0.c aleph1.c aleph2.c aleph3.c
aleph_o = alephini.o aleph0.o aleph1.o aleph2.o \
	      alephextra.o aleph.o alephbis.o \
              aleph3.o aleph-pool.o

# Linking
aleph: $(aleph_o)
	$(kpathsea_link) $(aleph_o) $(socketlibs) $(LOADLIBES)

# The C files
$(aleph_c) alephcoerce.h alephd.h: aleph.p $(web2c_texmf)
	$(web2c) aleph
alephextra.c: lib/texmfmp.c
	sed s/TEX-OR-MF-OR-MP/aleph/ $(srcdir)/lib/texmfmp.c >$@
$(alephdir)/alephextra.h: $(alephdir)/alephextra-$(alephbuild).h
	-rm -f $@
	test -d $(alephdir) || mkdir $(alephdir)
	cp $(srcdir)/$(alephdir)/alephextra-$(alephbuild).h $@

# Additional C files, just copy them.
aleph.c: $(calephdir)/aleph.c
	-rm -f $@
	cp $(srcdir)/$(calephdir)/aleph.c $@
alephbis.c: $(calephdir)/alephbis.c 
	-rm -f $@
	cp $(srcdir)/$(calephdir)/alephbis.c $@
alephmem.h: $(calephdir)/alephmem.h
	-rm -f $@
	cp $(srcdir)/$(calephdir)/alephmem.h $@

# The Pascal file
aleph.p aleph.pool: otangle aleph.web aleph.ch
	$(OTANGLE) aleph.web aleph.ch

# Generation of the web and ch files.
aleph.web: tie tex.web $(alephdir)/aleph.mk
aleph.web: $(alephdir)/om16bit.ch
aleph.web: $(alephdir)/omstr.ch
aleph.web: $(alephdir)/omfont.ch
aleph.web: $(alephdir)/omchar.ch
aleph.web: $(alephdir)/omfi.ch
aleph.web: $(alephdir)/ompar.ch
aleph.web: $(alephdir)/omocp.ch
aleph.web: $(alephdir)/omfilter.ch
aleph.web: $(alephdir)/omtrans.ch
aleph.web: $(alephdir)/omdir.ch
aleph.web: $(alephdir)/eobase.ch
aleph.web: $(alephdir)/eofmt.ch
aleph.web: $(alephdir)/eomem-$(alephbuild).ch
aleph.web: $(alephdir)/eo16bit-$(alephbuild).ch
aleph.web: $(alephdir)/eoext.ch
aleph.web: $(alephdir)/eoeqtb.ch
aleph.web: $(alephdir)/eofix.ch
aleph.web: $(alephdir)/eocprt.ch
aleph.web: $(alephdir)/eonewdir-$(alephbuild).ch
aleph.web: $(alephdir)/eover-$(alephbuild).ch
aleph.web: $(alephdir)/eopage-$(alephbuild).ch
aleph.web: $(alephdir)/eochar-$(alephbuild).ch
	$(TIE) -m aleph.web $(srcdir)/tex.web \
	 $(srcdir)/$(alephdir)/om16bit.ch \
	 $(srcdir)/$(alephdir)/omstr.ch \
	 $(srcdir)/$(alephdir)/omfont.ch \
	 $(srcdir)/$(alephdir)/omchar.ch \
	 $(srcdir)/$(alephdir)/omfi.ch \
	 $(srcdir)/$(alephdir)/ompar.ch \
	 $(srcdir)/$(alephdir)/omocp.ch \
	 $(srcdir)/$(alephdir)/omfilter.ch \
	 $(srcdir)/$(alephdir)/omtrans.ch \
	 $(srcdir)/$(alephdir)/omdir.ch \
	 $(srcdir)/$(alephdir)/eobase.ch \
	 $(srcdir)/$(alephdir)/eofmt.ch \
	 $(srcdir)/$(alephdir)/eomem-$(alephbuild).ch \
	 $(srcdir)/$(alephdir)/eo16bit-$(alephbuild).ch \
	 $(srcdir)/$(alephdir)/eoext.ch \
	 $(srcdir)/$(alephdir)/eoeqtb.ch \
	 $(srcdir)/$(alephdir)/eofix.ch \
	 $(srcdir)/$(alephdir)/eocprt.ch \
	 $(srcdir)/$(alephdir)/eonewdir-$(alephbuild).ch \
	 $(srcdir)/$(alephdir)/eover-$(alephbuild).ch \
	 $(srcdir)/$(alephdir)/eopage-$(alephbuild).ch \
	 $(srcdir)/$(alephdir)/eochar-$(alephbuild).ch
aleph.ch: tie aleph.web $(alephdir)/aleph.mk
aleph.ch: $(calephdir)/com16bit-$(alephbuild).ch
aleph.ch: $(calephdir)/comstr.ch
aleph.ch: $(calephdir)/comfont.ch
aleph.ch: $(calephdir)/comchar.ch
aleph.ch: $(calephdir)/comfi.ch
aleph.ch: $(calephdir)/compar.ch
aleph.ch: $(calephdir)/comocp.ch
aleph.ch: $(calephdir)/comfilter.ch
aleph.ch: $(calephdir)/comtrans.ch
aleph.ch: $(calephdir)/comdir.ch
aleph.ch: $(calephdir)/comsrcspec.ch
aleph.ch: $(calephdir)/ceostuff.ch
	$(TIE) -c aleph.ch aleph.web \
	 $(srcdir)/$(calephdir)/com16bit-$(alephbuild).ch \
	 $(srcdir)/$(calephdir)/comstr.ch \
	 $(srcdir)/$(calephdir)/comfont.ch \
	 $(srcdir)/$(calephdir)/comchar.ch \
	 $(srcdir)/$(calephdir)/comfi.ch \
	 $(srcdir)/$(calephdir)/compar.ch \
	 $(srcdir)/$(calephdir)/comocp.ch \
	 $(srcdir)/$(calephdir)/comfilter.ch \
	 $(srcdir)/$(calephdir)/comtrans.ch \
	 $(srcdir)/$(calephdir)/comdir.ch \
	 $(srcdir)/$(calephdir)/comsrcspec.ch \
	 $(srcdir)/$(calephdir)/ceostuff.ch

aleph-pool.c: aleph.pool $(makecpool)
	$(makecpool) aleph.pool alephdir/aleph-pool.h >$@ || rm -f $@

# Check: right now all we do is build the format.
check: @ALEPH@ aleph-check
aleph-check: aleph aleph.fmt
# Cleaning up from building aleph
clean:: aleph-clean
aleph-clean:
	$(LIBTOOL) --mode=clean $(RM) aleph
	rm -f $(aleph_c) alephextra.c alephcoerce.h alephd.h
	rm -f $(alephdir)/alephextra.h
	rm -f aleph.c alephbis.c alephmem.h
	rm -f aleph.p aleph.pool aleph.web aleph.ch

# 
# Dumps.
all_afmts = aleph.fmt $(afmts)

dumps: @ALEPH@ afmts
afmts: $(all_afmts)

afmtdir = $(web2cdir)/aleph
$(afmtdir)::
	$(SHELL) $(top_srcdir)/../mkinstalldirs $(afmtdir)

aleph.fmt: aleph
	$(dumpenv) $(MAKE) progname=aleph files="omega.ini" prereq-check
	$(dumpenv) ./aleph --ini --progname=aleph --jobname=aleph \\input omega.tex \\dump </dev/null
lamed.fmt: aleph
	$(dumpenv) $(MAKE) progname=lamed files="lambda.tex" prereq-check
	$(dumpenv) ./aleph --ini --progname=lamed --jobname=lamed \\input lambda.tex </dev/null

# 
# Installation.
install-aleph: install-aleph-exec
install-programs: @ALEPH@ install-aleph-exec
install-aleph-exec: aleph $(bindir)
	for p in aleph; do $(INSTALL_LIBTOOL_PROG) $$p $(bindir); done

# end of aleph.mk
