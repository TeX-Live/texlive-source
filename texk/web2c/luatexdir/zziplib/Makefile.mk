PUB=pub/

auto:
	aclocal && autoconf && autoheader && automake

boottrap:
	rm -rf .deps .libs
	rm -f config.guess config.sub stamp-h.in
	rm -f install-sh ltconfig ltmain.sh depcomp mkinstalldirs
	rm -f config.h config.h.in config.log config.cache configure
	rm -f aclocal.m4 Makefile Makefile.in
	aclocal 
	autoconf 
	autoheader 
	automake -a -c 

rpm: dist-bzip $(PACKAGE).spec
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.bz2

dist-bzip : dist-bzip2
	$(MAKE) dist-bzip2-done
dist-bzip2-done dist-done :
	test -d $(PUB) || mkdir $(PUB)
	@ echo cp $(BUILD)/$(PACKAGE)-$(VERSION).tar.bz2 $(PUB). \
	;      cp $(BUILD)/$(PACKAGE)-$(VERSION).tar.bz2 $(PUB).
snapshot:
	$(MAKE) dist-bzip2 VERSION=`date +%Y.%m%d`
distclean-done:
	- rm -r *.d

configsub :
	cp ../savannah.config/config.guess uses/config.guess
	cp ../savannah.config/config.sub   uses/config.sub

cf = $(cf_fedora)
cf_debian = x86-linux1
cf_fedora = x86-linux2
cf_freebsd = x86-freebsd1
cf_netbsd = x86-netbsd1
cf_openbsd = x86-openbsd1
cf_amd64 = amd64-linux1
cf_alpha = usf-cf-alpha-linux-1
cf_powermac = ppc-osx3
cf_powerpc = openpower-linux1
cf_sparc = sparc-solaris1
cf_solaris = x86-solaris1
cf_all = $(cf_debian) $(cf_fedora) $(cf_freebsd) $(cf_netbsd) $(cf_openbsd) \
         $(cf_amd64) $(cf_alpha) $(cf_powerpc) $(cf_sparc) $(cf_solaris) \
         $(cf_powermac)
cf : cf-upload cf-system-all
cf-upload: ;	scp $(PUB)$(PACKAGE)-$(VERSION).tar.bz2 cf-shell.sf.net:
cf-unpack: ; ssh cf-shell.sf.net \
	"ssh $(cf) 'tar xjvf $(PACKAGE)-$(VERSION).tar.bz2'"
cf-configure: ; ssh cf-shell.sf.net \
	"ssh $(cf) 'uname -msr; cd $(PACKAGE)-$(VERSION) && sh configure'"
cf-build: ; ssh cf-shell.sf.net \
	"ssh $(cf) 'uname -msr; cd $(PACKAGE)-$(VERSION) && make'"
cf-check: ; ssh cf-shell.sf.net \
	"ssh $(cf) 'uname -msr; cd $(PACKAGE)-$(VERSION) && make check'"
cf-system: ; ssh cf-shell.sf.net \
	"ssh $(cf) 'uname -msr | sed s,^,====$(cf)====,'"
cf-configure-all:
	@ for cf in $(cf_all) ; do echo $(MAKE) cf-configure cf=$$cf \
	; sleep 1;                      $(MAKE) cf-configure cf=$$cf ; done
cf-build-all:
	@ for cf in $(cf_all) ; do echo $(MAKE) cf-build cf=$$cf \
	; sleep 1;                      $(MAKE) cf-build cf=$$cf ; done
cf-check-all:
	@ for cf in $(cf_all) ; do echo $(MAKE) cf-check cf=$$cf \
	; sleep 1;                      $(MAKE) cf-check cf=$$cf ; done
cf-system-all:
	@ for cf in $(cf_all) ; do echo $(MAKE) cf-configure cf=$$cf \
	; sleep 1 ;                     $(MAKE) cf-system cf=$$cf ; done
