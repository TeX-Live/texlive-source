#!/bin/sh

BASE=`basename "$1" .idx`
IDX=$BASE.idx
IND=$BASE.ind
ILG=$BASE.ilg

sed '
/^\\indexentry{/ {
s/\\IeC {\\CYRA }/¾/g
s/\\IeC {\\cyra }/¿/g
s/\\IeC {\\CYRB }/À/g
s/\\IeC {\\cyrb }/Á/g
s/\\IeC {\\CYRV }/Â/g
s/\\IeC {\\cyrv }/Ã/g
s/\\IeC {\\CYRG }/Ä/g
s/\\IeC {\\cyrg }/Å/g
s/\\IeC {\\CYRD }/Æ/g
s/\\IeC {\\cyrd }/Ç/g
s/\\IeC {\\CYRE }/È/g
s/\\IeC {\\cyre }/É/g
s/\\IeC {\\CYRYO }/Ê/g
s/\\IeC {\\cyryo }/Ë/g
s/\\IeC {\\CYRZH }/Ì/g
s/\\IeC {\\cyrzh }/Í/g
s/\\IeC {\\CYRZ }/Î/g
s/\\IeC {\\cyrz }/Ï/g
s/\\IeC {\\CYRI }/Ð/g
s/\\IeC {\\cyri }/Ñ/g
s/\\IeC {\\CYRISHRT }/Ò/g
s/\\IeC {\\cyrishrt }/Ó/g
s/\\IeC {\\CYRK }/Ô/g
s/\\IeC {\\cyrk }/Õ/g
s/\\IeC {\\CYRL }/Ö/g
s/\\IeC {\\cyrl }/×/g
s/\\IeC {\\CYRM }/Ø/g
s/\\IeC {\\cyrm }/Ù/g
s/\\IeC {\\CYRN }/Ú/g
s/\\IeC {\\cyrn }/Û/g
s/\\IeC {\\CYRO }/Ü/g
s/\\IeC {\\cyro }/Ý/g
s/\\IeC {\\CYRP }/Þ/g
s/\\IeC {\\cyrp }/ß/g
s/\\IeC {\\CYRR }/à/g
s/\\IeC {\\cyrr }/á/g
s/\\IeC {\\CYRS }/â/g
s/\\IeC {\\cyrs }/ã/g
s/\\IeC {\\CYRT }/ä/g
s/\\IeC {\\cyrt }/å/g
s/\\IeC {\\CYRU }/æ/g
s/\\IeC {\\cyru }/ç/g
s/\\IeC {\\CYRF }/è/g
s/\\IeC {\\cyrf }/é/g
s/\\IeC {\\CYRH }/ê/g
s/\\IeC {\\cyrh }/ë/g
s/\\IeC {\\CYRC }/ì/g
s/\\IeC {\\cyrc }/í/g
s/\\IeC {\\CYRCH }/î/g
s/\\IeC {\\cyrch }/ï/g
s/\\IeC {\\CYRSH }/ð/g
s/\\IeC {\\cyrsh }/ñ/g
s/\\IeC {\\CYRSHCH }/ò/g
s/\\IeC {\\cyrshch }/ó/g
s/\\IeC {\\CYRHRDSN }/ô/g
s/\\IeC {\\cyrhrdsn }/õ/g
s/\\IeC {\\CYRERY }/ö/g
s/\\IeC {\\cyrery }/÷/g
s/\\IeC {\\CYRSFTSN }/ø/g
s/\\IeC {\\cyrsftsn }/ù/g
s/\\IeC {\\CYREREV }/ú/g
s/\\IeC {\\cyrerev }/û/g
s/\\IeC {\\CYRYU }/ü/g
s/\\IeC {\\cyryu }/ý/g
s/\\IeC {\\CYRYA }/þ/g
s/\\IeC {\\cyrya }/ÿ/g
}' $IDX | makeindex -t $ILG | tr '¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ' \
  'áÁâÂ÷×çÇäÄåÅ³£öÖúÚéÉêÊëËìÌíÍîÎïÏðÐòÒóÓôÔõÕæÆèÈãÃþÞûÛýÝÿßùÙøØüÜàÀñÑ' > $IND
