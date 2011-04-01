#!/bin/sh
        texexec --arg="ht-1=$2" --use=tex4ht --dvi --nobackend $5 $1 
        tex4ht -f/$1 -i~/tex4ht.dir/texmf/tex4ht/ht-fonts/$3
        t4ht -f/$1 $4 ## -d~/WWW/temp/ -m644 



