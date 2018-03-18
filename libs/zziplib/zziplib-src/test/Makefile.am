AUTOMAKE_OPTIONS = 1.4 foreign
AUTOTOOL_VERSION=autoconf-2.52 automake-1.5 libtool-1.4.2
DEFAULT_INCLUDES = # nothing - no default of -I. -I$(srcdir)
DEFS = @DEFS@ -I$(top_builddir) -I$(top_srcdir) # also for automake 1.4

EXTRA_DIST = test.zip zziptests.py
DISTCLEANFILES = test*.dat test0.zip test1.zip test2.zip test3.zip

zzcat     = ../bins/zzcat$(EXEEXT)
zzdir     = ../bins/zzdir$(EXEEXT)
zziptest  = ../bins/zziptest$(EXEEXT)
zzxorcat  = ../bins/zzxorcat$(EXEEXT)
zzxordir  = ../bins/zzxordir$(EXEEXT)
zzxorcopy = ../bins/zzxorcopy$(EXEEXT)

$(zzcat) $(zzdir) $(zziptest) $(zzxorcat) $(zzxordir) $(xorcopy) : \
        @top_srcdir@/bins/*.c @top_srcdir@/zzip/*.c
	(cd ../bins && $(MAKE))

# -------------------------------------------------------------------
NULL=/dev/null
README = $(top_srcdir)/README
testdatadir = $(top_builddir)/testdata.d

testzip testzips : test.zip test1.zip test2.zip test3.zip 

test0.zip: $(README)
	- test -d tmp || mkdir tmp
	- cp $(README) tmp/README
	- rm -f $@
	- cd tmp && $(MKZIP) ../test0.zip README
	- rm -r tmp
	test -s test0.zip && cp -v test0.zip $(srcdir)/test.zip
	
test.zip: $(README)
	- $(MAKE) test0.zip
	- test -s $@ || cp $(srcdir)/test.zip $@

# -------------------------------------------------------------------

sfxtests = @ZIPTESTS@ check-sfx
newtests = @ZIPTESTS@ check-tests

check-local: check-readme $(sfxtests) $(newtests)

check-tests: tests
tests:
	@ test -f zziptests.py || ln -s -v $(srcdir)/zziptests.py . || cp -v $(srcdir)/zziptests.py .
	$(PYTHON) zziptests.py     --exeext "$(EXEEXT)" --topsrcdir "$(top_srcdir)"
test_%:
	@ test -f zziptests.py || ln -s -v $(srcdir)/zziptests.py . || cp -v $(srcdir)/zziptests.py .
	$(PYTHON) zziptests.py $@ -vvvv    --exeext "$(EXEEXT)" --topsrcdir "$(top_srcdir)"

check-readme : $(zzcat)  test.zip
	@ test -f test.zip || $(MAKE) test0.zip 
	test -s test.zip || cp $(srcdir)/test.zip test.zip
	@ echo :$@: checking $(zzcat) test/README 
	@ $(zzcat) test/README >test.out
	@ if diff test.out $(README) >$(NULL) \
	; then rm test.out ; echo check OK ; true \
	; else rm test.out ; echo check FAIL ; false ; fi

# -------------------------------------------------------------------

noinst_PROGRAMS = zzipself zzipsetstub

zzipself_LDFLAGS = @ZZIPLIB_LDFLAGS@
zzipself_LDADD = ../zzip/libzzip.la @RESOLVES@ -lz

../zzip/libzzip.la : @top_srcdir@/zzip/*.c
	(cd ../zzip && $(MAKE) `basename $@`)

zzshowme$(EXEEXT) : zzipself$(EXEEXT) zzipsetstub$(EXEEXT)
	$(MKZIP) -0 -j $@ .libs/zzipself$(EXEEXT)
	$(MKZIP) -9 -j $@ $(README)
	- test -s $@ || cp test2.zip $@
	test ! -f $@.zip || mv $@.zip $@
	./zzipsetstub $@ .libs/zzipself$(EXEEXT)
	- chmod +x $@

check-sfx : zzshowme$(EXEEXT)
	echo :$@: "./zzshowme readme >readme.out 2>readme.err" 
	export LD_LIBRARY_PATH="../zzip/.libs:$$LD_LIBRARY_PATH" \
	; ./zzshowme readme >readme.out 2>readme.err ; true
	@ echo 'diff readme.out $(README) || grep "libzzip-" readme.err' \
	; if test -s readme.out ; then diff readme.out $(README) \
        ; else grep "libzzip-" readme.err || echo "readme.out is empty!" ; fi
	rm readme.out readme.err

