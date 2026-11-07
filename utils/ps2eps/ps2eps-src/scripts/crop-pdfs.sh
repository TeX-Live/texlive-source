#!/usr/bin/env bash
for file in *.pdf
do
    filebasename="${file%%.*}"
    if [ "${filebasename}.pptx" -nt "$file" ]; then
	echo "Need to reprint ${filebasename}.pptx"
    fi
    if [ "$file" -nt "../$file" ]; then
	~/scripts/rbpdfcrop >/dev/null 2>&1 $file 
	#echo "${filebasename}-crop.pdf"
	if [ -e "${filebasename}-crop.pdf" ]; then
	    mv -f "${filebasename}-crop.pdf" ../$file
	    echo "Cropped $file"
	fi
    fi
done
echo "Done."
