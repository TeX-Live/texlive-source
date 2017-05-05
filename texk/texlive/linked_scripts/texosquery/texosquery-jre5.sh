#!/bin/sh


jarpath=`kpsewhich --progname=texosquery --format=texmfscripts texosquery-jre5.jar`
java -jar "$jarpath" "$@"
