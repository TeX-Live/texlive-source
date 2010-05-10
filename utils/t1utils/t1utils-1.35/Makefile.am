## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign check-news

bin_PROGRAMS = t1ascii t1binary t1asm t1disasm t1unmac t1mac
man_MANS = t1ascii.1 t1binary.1 t1asm.1 t1disasm.1 t1unmac.1 t1mac.1

t1ascii_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1ascii.c
t1binary_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1binary.c
t1asm_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1asm.c
t1disasm_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1disasm.c
t1unmac_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1unmac.c
t1mac_SOURCES = include/lcdf/clp.h include/lcdf/inttypes.h \
	clp.c t1lib.h t1lib.c t1mac.c

AM_CPPFLAGS = -I$(top_srcdir)/include
LDADD = @LIBOBJS@

EXTRA_DIST = $(man_MANS) t1utils.spec

srclinks:
	cd $(top_srcdir); test -d include/lcdf || mkdir -p include/lcdf
	cd $(top_srcdir); for i in clp.h inttypes.h; do \
	ln -sf ../../../liblcdf/include/lcdf/$$i include/lcdf/$$i; done
	cd $(top_srcdir); for i in clp.c strerror.c; do \
	ln -sf ../liblcdf/liblcdf/$$i $$i; done

versionize:
	perl -pi -e "s/^\\.ds V.*/.ds V $(VERSION)/;" t1ascii.1 t1binary.1 t1disasm.1 t1asm.1 t1unmac.1 t1mac.1
	perl -pi -e 's/^Version:(\s*).*/Version:$${1}$(VERSION)/; s/$(PACKAGE)-[\w.]+\.tar\.gz/$(PACKAGE)-$(VERSION).tar.gz/;' t1utils.spec
	perl -pi -e 's/VERSION [\d.ab]+/VERSION $(VERSION)/;' README

rpm: dist
	buildarch=`rpm --showrc | awk '/^build arch/ { print $$4; }'` ; \
	mkdir -p /tmp/rpm-t1u/SOURCES /tmp/rpm-t1u/RPMS/$$buildarch \
	/tmp/rpm-t1u/BUILD ; \
	echo 'include: /usr/lib/rpm/rpmrc' > /tmp/rpm-t1u/rc ; \
	echo 'macrofiles: /usr/lib/rpm/macros:/tmp/rpm-t1u/macros' >> /tmp/rpm-t1u/rc ; \
	echo '%_topdir /tmp/rpm-t1u' > /tmp/rpm-t1u/macros ; \
	cp $(PACKAGE)-$(VERSION).tar.gz /tmp/rpm-t1u/SOURCES ; \
	rpmbuild --rcfile /tmp/rpm-t1u/rc -bb t1utils.spec ; \
	cp /tmp/rpm-t1u/RPMS/$$buildarch/*.rpm .
	rm -rf /tmp/rpm-t1u

.PHONY: srclinks versionize rpm
