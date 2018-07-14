#!/bin/bash
kernel=`uname -s`
if test "${kernel#*CYGWIN}" != "$kernel"
then
    jarpath=`cygpath -w $(kpsewhich --progname=arara --format=texmfscripts arara.jar)`
else
    jarpath=`kpsewhich --progname=arara --format=texmfscripts arara.jar`
fi
java -jar "$jarpath" "$@"
