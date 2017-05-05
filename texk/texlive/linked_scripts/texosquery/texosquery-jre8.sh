#!/bin/sh


jarpath=`kpsewhich --progname=texosquery --format=texmfscripts texosquery-jre8.jar`
java -Djava.locale.providers=CLDR,JRE -jar "$jarpath" "$@"
