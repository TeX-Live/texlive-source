#!/bin/bash
jarpath=`kpsewhich --progname=arara --format=texmfscripts arara.jar`
java -jar "$jarpath" "$@"
