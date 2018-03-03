#!/bin/sh

kernel=`uname -s`
if test "${kernel#*CYGWIN}" != "$kernel"
then
 jarpath=`cygpath -w $(kpsewhich --progname=tlcockpit --format=texmfscripts tlcockpit.jar)`
else
 jarpath=`kpsewhich --progname=tlcockpit --format=texmfscripts tlcockpit.jar`
fi
exec java -jar "$jarpath" "$@"
