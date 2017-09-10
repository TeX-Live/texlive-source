#!/bin/sh

kernel=`uname -s`
if test "${kernel#*CYGWIN}" != "$kernel"
then
 jarpath=`cygpath -w $(kpsewhich --progname=bib2gls --format=texmfscripts bib2gls.jar)`
else
 jarpath=`kpsewhich --progname=bib2gls --format=texmfscripts bib2gls.jar`
fi

java -Djava.locale.providers=CLDR,JRE -jar "$jarpath" "$@"

