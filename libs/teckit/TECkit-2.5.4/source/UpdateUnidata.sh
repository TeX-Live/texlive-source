#!/bin/bash

# Update the Unicode data that TECKit uses

if [ $# -ne 1 ]; then
	echo "Usage: $(basename $0) VERSION" >&2
	exit 1
fi

VERSION=$1

BASE=http://www.unicode.org/Public/$VERSION/ucd

wget -N $BASE/{CompositionExclusions,UnicodeData}.txt

perl MakeNormData.pl >NormalizationData.c
perl MakeUnicodeNames.pl >UnicodeNames.cpp

cd ../test

wget -N $BASE/NormalizationTest.txt
