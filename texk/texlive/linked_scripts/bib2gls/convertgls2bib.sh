#!/bin/sh

kernel=`uname -s`
if test "${kernel#*CYGWIN}" != "$kernel"
then
 jarpath=`cygpath -w $(kpsewhich --progname=convertgls2bib --format=texmfscripts convertgls2bib.jar)`
else
 jarpath=`kpsewhich --progname=convertgls2bib --format=texmfscripts convertgls2bib.jar`
fi

java -jar "$jarpath" "$@"

