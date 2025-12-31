#!/bin/bash

# Update the Unicode data that TECKit uses

if [ $# -ne 1 ]; then
	echo "Usage: $(basename $0) VERSION" >&2
	exit 1
fi

VERSION=$1

BASE=https://www.unicode.org/Public/$VERSION/ucd

curl -O $BASE/CompositionExclusions.txt
curl -O $BASE/UnicodeData.txt

perl MakeNormData.pl >NormalizationData.c
perl MakeUnicodeNames.pl >UnicodeNames.cpp

cd ../test

curl -O $BASE/NormalizationTest.txt
