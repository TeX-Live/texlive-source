#!/bin/sh


kernel=`uname -s`
if test "${kernel#*CYGWIN}" != "$kernel"
then
 jarpath=`cygpath -w $(kpsewhich --progname=texosquery --format=texmfscripts texosquery-jre8.jar)`
else
 jarpath=`kpsewhich --progname=texosquery --format=texmfscripts texosquery-jre8.jar`
fi
java -Djava.locale.providers=CLDR,JRE -jar "$jarpath" "$@"
