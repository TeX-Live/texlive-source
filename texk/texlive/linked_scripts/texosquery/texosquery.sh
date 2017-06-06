#!/bin/sh


kernel=`uname -s`
if test "${kernel#*CYGWIN}" != "$kernel"
then
 jarpath=`cygpath -w $(kpsewhich --progname=texosquery --format=texmfscripts texosquery.jar)`
else
 jarpath=`kpsewhich --progname=texosquery --format=texmfscripts texosquery.jar`
fi
java -jar "$jarpath" "$@"
