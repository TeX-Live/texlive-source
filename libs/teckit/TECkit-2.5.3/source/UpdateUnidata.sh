#!/bin/bash

# Update the Unicode data that TECKit uses

wget -N http://www.unicode.org/Public/UNIDATA/{CompositionExclusions,UnicodeData}.txt

perl MakeNormData.pl >NormalizationData.c
perl MakeUnicodeNames.pl >UnicodeNames.cpp
