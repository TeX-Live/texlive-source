# $Id$
# This file is public domain.  (Originally written by Karl Berry, 2009.)

prg = epstopdf
e2p = ./$(prg).pl #--debug
re2p = ./r$(prg) --restricted
re2p_script = ./r$(prg)

default: check

$(re2p_script):
	ln -s $(prg).pl $@

# These are not comprehensive tests by any means.  Mostly we're just
# making sure we don't crash.
check: check-help check-version check-help-r check-usage \
       check-atend \
       check-binary check-bin2 \
       check-binhdr-lf check-binhdr-cr check-binhdr-crlf \
       check-filter check-filter-outfile \
       check-gray \
       check-gscmd check-gsopt check-gsopts \
       check-nogs \
       check-output \
       check-percent \
       check-restricted-device \
       check-restricted-gscmd \
       check-restricted-safe-in check-restricted-safe-out \
       check-simple \
       check-tn5002 \
       check-write-error \

check-help:
	$(e2p) --help

check-version:
	$(e2p) --version

check-help-r: $(re2p_script)
	$(re2p_script) --help | grep restricted  # should have default=true

check-usage:
	! $(e2p)                 # no args
	! $(e2p) arg1 arg2 arg3  # too many args
	! $(e2p) --nonesuch      # no option

check-restricted-device: $(re2p_script)
	! $(re2p) --device=unsafe test-simple.eps

check-restricted-gscmd: $(re2p_script)
	! $(re2p) --gscmd=/bin/unsafe test-simple.eps
	! $(re2p) --gscmd=unsafe test-simple.eps

check-restricted-gsopt: $(re2p_script)
	$(re2p) --gsopt=-dAutoFilterColorImages=true test-simple.eps
	! $(re2p) --gsopt=-dNOSAFER=false test-simple.eps

check-restricted-safe-in: $(re2p_script)
	! $(re2p) /unsafe/in.eps

check-restricted-safe-out: $(re2p_script)
	! $(re2p) test-simple.eps -o /unsafe/out.eps

check-filter:
	cat test-simple.eps | $(e2p) --filter >test-filter.pdf
	pdfinfo test-filter.pdf | grep "Page size:"

check-filter-outfile:
	rm -f test-filter.pdf
	cat test-simple.eps | $(e2p) --filter --outfile=test-filter.pdf
	pdfinfo test-filter.pdf | grep "Page size:"
#
	rm -f test-filter.pdf
	cat test-simple.eps | $(e2p) --outfile=test-filter.pdf --filter
	pdfinfo test-filter.pdf | grep "Page size:"

check-gray:
	$(e2p) --gray test-tiger.eps
	pdfinfo test-tiger.pdf | grep "Page size:"

check-write-error:
	! $(e2p) test-simple.eps --outfile=/dev/full

check-gscmd:
	! $(e2p) --gscmd=false test-simple.eps
	! $(e2p) --gscmd='foo bar' test-simple.eps

check-gsopt:
	! $(e2p) --gscmd=false --gsopt=foo --gsopt=bar test-simple.eps
	! $(e2p) --restricted --gsopt=bar test-simple.eps

check-gsopts:
	$(e2p) --gscmd=echo --gsopts='foo bar' test-simple.eps
	! $(e2p) --restricted --gsopts=bar test-simple.eps

check-nogs:
	$(e2p) --no-gs test-simple.eps >test-simple.out
# the two files should be different:
	cmp -s test-simple.eps test-simple.out || exit 0
	
check-output: $(re2p_script)
	$(re2p) test-simple.eps -o test-p.pdf               # not if restricted
	# should do this everywhere -- remove first, validate after.
	#rm -f test-o.pdf; $(e2p) -o test-o.pdf test-simple.eps; pdfinfo test-o.pdf
	$(e2p) -o test-o.pdf test-simple.eps
	$(e2p) --outfile test-o.pdf test-simple.eps
	$(e2p) --outfile=test-o.pdf test-simple.eps
	$(e2p) --output test-o.pdf test-simple.eps
	$(e2p) test-simple.eps test-p.pdf
	$(re2p) test-simple.eps test-p.pdf
	! $(e2p) test-simple.eps test-p.pdg                # not .pdf
	! $(e2p) -o test-o.pdf test-simple.eps test-p.pdf  # two methods

check-simple:
	$(MAKE) file=test-simple check1

check-atend:
	$(MAKE) file=test-atend check1

check-binary:
	$(MAKE) file=test-binary check1

check-bin2:
	$(MAKE) file=test-bin2 check1

check-binhdr-lf:
	$(MAKE) file=test-binhdr-lf check1

check-binhdr-cr:
	$(MAKE) file=test-binhdr-cr check1

check-binhdr-crlf:
	$(MAKE) file=test-binhdr-crlf check1

check-percent:
	$(MAKE) file=test-per%cent check1
	test -s test-per%cent.pdf

check-tn5002:
	$(MAKE) file=test-tn5002-cr check1
	$(MAKE) file=test-tn5002-crlf check1
	$(MAKE) file=test-tn5002-lf check1

# complicated, from http://www.vectorfloral.com/floral-swirly-flowers-with-butterflies-vector-graphic-174.html
# another, even bigger, is at http://www.backgroundvector.com/abstract-colorful-background-vector-image-94.html
# both are CC-BY 3.0.
check-floral:
	$(MAKE) file=test-floral check1

# subtarget for simple test that conversion of $(file) works.
check1:
	$(e2p) $(file).eps 
	pdfinfo $(file).pdf | grep "Page size"

# 
# put tar contents in a directory for CTAN.
dist: $(prg).tar.gz $(prg).html
.PHONY: dist
$(prg).tar.gz: README $(prg).pl $(prg).1 r$(prg).1 $(prg).man1.pdf
	rm -rf $(prg); mkdir $(prg)
	cp $^ $(prg)
	tar cvzf $(prg).tar.gz $(prg)
	rm -rf $(prg)
	# upload .tar.gz to ctan.org/upload, mv .html ~www/$(prg).

groff = groff
groff_opts = -man -t
pspdf = ps2pdf -sPAPERSIZE=a4
$(prg).man1.pdf: $(prg).1
	$(groff) $(groff_opts) $< | $(pspdf) - $@
$(prg).html: $(prg).1
	$(groff) $(groff_opts) -Thtml $< >$@
show-man:
	man ./$(prg).1 | col -b
#	$(groff) $(groff_opts) -Tascii -M. -mraggedright -P-cbouU $(prg).1
# -P-c... avoids ANSI color sequences and backspaces in the output.
