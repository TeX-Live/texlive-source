# Build
CXX = gcc
CXXFLAGS = -Wall -Wextra -pedantic
OPTIFLAG = -O2
DEBUGFLAG = -g
COVFLAG = --coverage
PROFFLAG = -g -pg

# Executables
UTFPATGEN_BIN = ./build/utfpatgen
UNITTEST_BIN = ./build/unit_test

# Test files
PARAMPROFILE = ./test/cshyphen.in
DICTIONARY = ./test/wortliste10k.wlh
OUTFILE = ./test/output.pat
TRANSLATEFILE = ./test/german.tr

ifeq ($(OS),Windows_NT)
    EXT = .exe
    EMPTY = NUL
else
    EXT =
    EMPTY = /dev/null
endif

# Targets
.PHONY: all coverage build-profile build-debug build-execute analyze-cov run run-tests run-patgen
all: clean utfpatgen.pdf build-execute run

# Run configurations
run:
	sed -b 's/1/\xFE\x01/g; s/2/\xFE\x02/g; s/3/\xFE\x03/g; s/4/\xFE\x04/g; s/5/\xFE\x05/g; s/6/\xFE\x06/g; s/7/\xFE\x07/g; s/8/\xFE\x08/g; s/9/\xFE\x09/g' $(DICTIONARY) > $(DICTIONARY)_utfp
	cat $(PARAMPROFILE) | $(UTFPATGEN_BIN) $(DICTIONARY)_utfp $(EMPTY) $(OUTFILE)_pre $(TRANSLATEFILE)
	sed -b 's/\xFE\x01/1/g; s/\xFE\x02/2/g; s/\xFE\x03/3/g; s/\xFE\x04/4/g; s/\xFE\x05/5/g; s/\xFE\x06/6/g; s/\xFE\x07/7/g; s/\xFE\x08/8/g; s/\xFE\x09/9/g' $(OUTFILE)_pre > $(OUTFILE)
	rm -f $(DICTIONARY)_utfp $(OUTFILE)_pre

run-tests:
	$(UNITTEST_BIN)

run-patgen:
	cat $(PARAMPROFILE) | patgen $(DICTIONARY) $(EMPTY) $(OUTFILE) $(TRANSLATEFILE)

# Executables
build-execute: test/unit_test.c | build build/utfpatgen.c
	cp utfpatgen.h build/
	$(CXX) $(CXXFLAGS) $(OPTIFLAG) -o build/utfpatgen build/utfpatgen.c
	$(CXX) $(CXXFLAGS) $(OPTIFLAG) -DTEST -o build/unit_test build/utfpatgen.c test/unit_test.c
	$(eval UTFPATGEN_BIN = ./build/utfpatgen)
	$(eval UNITTEST_BIN = ./build/unit_test)
	rm build/utfpatgen.h

build-coverage: test/unit_test.c | build build/utfpatgen.c
	cp utfpatgen.h build/
	$(CXX) $(CXXFLAGS) $(COVFLAG) -o build/utfpatgen_cov build/utfpatgen.c
	$(CXX) $(CXXFLAGS) $(COVFLAG) -DTEST -o build/unit_test_cov build/utfpatgen.c test/unit_test.c
	$(eval UTFPATGEN_BIN = ./build/utfpatgen_cov)
	$(eval UNITTEST_BIN = ./build/unit_test_cov)
	rm build/utfpatgen.h

build-profile: test/unit_test.c | build build/utfpatgen.c
	cp utfpatgen.h build/
	$(CXX) $(CXXFLAGS) $(PROFFLAG) -o build/utfpatgen_prof build/utfpatgen.c
	$(CXX) $(CXXFLAGS) $(PROFFLAG) -DTEST -o build/unit_test_prof build/utfpatgen.c test/unit_test.c
	$(eval UTFPATGEN_BIN = ./build/utfpatgen_prof)
	$(eval UNITTEST_BIN = ./build/unit_test_prof)
	rm build/utfpatgen.h

analyze-prof: build-profile run
	gprof -b $(UTFPATGEN_BIN)$(EXT) ./gmon.out | gprof2dot | dot -Tpng -o profile_visual.png

build-debug: test/unit_test.c | build build/utfpatgen.c
	cp utfpatgen.h build/
	$(CXX) $(CXXFLAGS) $(DEBUGFLAG) -o build/utfpatgen_debug build/utfpatgen.c
	$(CXX) $(CXXFLAGS) $(DEBUGFLAG) -DTEST -o build/unit_test_debug build/utfpatgen.c test/unit_test.c
	$(eval UTFPATGEN_BIN = ./build/utfpatgen_debug)
	$(eval UNITTEST_BIN = ./build/unit_test_debug)
	rm build/utfpatgen.h

build:
	mkdir build

# PDF documentation
build/utfpatgen.tex: utfpatgen.w | build
	cweave $< - $@

utfpatgen.pdf: build/utfpatgen.tex | build
	cp cweb.cls build/
	cp cwebacromac.tex build/
	cp cwebmac.tex build/
	cd build; pdftex utfpatgen.tex
	cd build; pdftex utfpatgen.tex
	rm build/cweb.cls build/cwebacromac.tex build/cwebmac.tex
	mv build/utfpatgen.pdf utfpatgen.pdf

# C source code
build/utfpatgen.c: utfpatgen.w | build
	ctangle $< - $@

# Cleaning
.PHONY: clean
clean:
	rm -rf utfpatgen.pdf build pattmp.* profile_visual.png