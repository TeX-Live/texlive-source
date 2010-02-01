AUTOMAKE_OPTIONS = 1.4 foreign dist-bzip2
ACLOCAL_AMFLAGS = -I m4
WANT_AUTOMAKE = 1.7
WANT_AUTOCONF = 2.57

DIST_SUBDIRS = zzip zzipwrap bins test docs  SDL
SUBDIRS =      zzip zzipwrap bins test docs @SDL@

# see Makefile.mk for the "make rpm" target
rpm2: dist-bzip2 $(PACKAGE).spec
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.bz2

zzip.html zzip.xml zzip.pdf \
doc docu clean-doc clean-docs clean-docu \
man manpages htmpages unpack clean-unpack \
omf install-omf install-doc install-sfnet install-man3 x :
	(cd docs && $(MAKE) `basename $@`)

sdl testsdl test-sdl install-sdl :
	(cd SDL && $(MAKE) `basename $@`)

check-test0 check-test1 check-zzdir check-zzcat \
check-zzxor check-zzxordir check-zzxorcat \
check-sfx     check-readme : \
	(cd test && $(MAKE) `basename $@`)

MSVC8 =   msvc8/README.TXT msvc8/zip.exe msvc8/test1.zip msvc8/test.zip \
msvc8/zzdir.vcproj    msvc8/zzipself.vcproj     msvc8/zzip.vcproj \
msvc8/zziplib.sln     msvc8/zzipsetstub.sln     msvc8/zzobfuscated.sln \
msvc8/zziplib.vcproj  msvc8/zzipsetstub.vcproj  msvc8/zzobfuscated.vcproj \
msvc8/zzcat.sln       msvc8/zzipself.bat        msvc8/zzip.sln \
msvc8/zzcat.vcproj    msvc8/zzipself.sln        msvc8/zziptest.sln \
msvc8/zzdir.sln       msvc8/zzipself.txt        msvc8/zziptest.vcproj

MSVC7 =   msvc7/pkzip.exe msvc7/test1.zip msvc7/test.zip \
msvc7/zzdir.vcproj    msvc7/zzipself.vcproj     msvc7/zzip.vcproj \
msvc7/zziplib.sln     msvc7/zzipsetstub.sln     msvc7/zzobfuscated.sln \
msvc7/zziplib.vcproj  msvc7/zzipsetstub.vcproj  msvc7/zzobfuscated.vcproj \
msvc7/zzcat.sln       msvc7/zzipself.bat        msvc7/zzip.sln \
msvc7/zzcat.vcproj    msvc7/zzipself.sln        msvc7/zziptest.sln \
msvc7/zzdir.sln       msvc7/zzipself.txt        msvc7/zziptest.vcproj

MSVC6 = \
msvc6/zzcat.dsp        msvc6/zziplib.dsp      msvc6/zzipwrap.dsp \
msvc6/zzdir.dsp        msvc6/zziplib.dsw      msvc6/zzobfuscated.dsp \
msvc6/zziptest.dsp     msvc6/zzip.dsp  

EXTRA_DIST =   zziplib.spec zzipback.sed Makefile.mk \
               $(MSVC8) $(MSVC7) $(MSVC6) $(am__aclocal_m4_deps)

PHONY = auto boottrap rpm doc docs man manpages htmpages sdl testsdl \
	comp compats msvc6 msvc7
docs : docu
# ------------------------------------------------------------------------

test-comp:
	test ! -d testing || rm -r testing
	mkdir testing
	(cd testing && tar xzvf ../$(PACKAGE)-$(VERSION).tar.gz)
	(cd "testing/$(PACKAGE)-$(VERSION)/zziplib" && sh configure)
	$(MAKE) -C  "testing/$(PACKAGE)-$(VERSION)/zziplib"
	$(MAKE) -C  "testing/$(PACKAGE)-$(VERSION)/zziplib"  check

clean-comp:
	test ! -d testing || rm -r testing

msvc :
	mkdir bin
	mkdir lib
	mkdir include
	mkdir include/zzip
	cp $(srcdir)/msvc6/Release/*.exe bin/
	cp $(srcdir)/msvc6/Release/*.dll bin/
	cp $(srcdir)/msvc6/Release/*.lib lib/
	cp $(srcdir)/zzip/*.h include/zzip/
	zip -9r $(srcdir)/$(PACKAGE)-$(VERSION)-msvc6-bin.zip bin/
	zip -9r $(srcdir)/$(PACKAGE)-$(VERSION)-msvc6-lib.zip lib/ include/
	rm -r bin lib include
