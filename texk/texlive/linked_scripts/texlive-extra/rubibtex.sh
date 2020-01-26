#!/bin/sh

# rubibtex, based on the original version contained in the t2 bundle.
# Thomas Esser, Public Domain.

progname=rubibtex
tmpdir=${TMPDIR-${TEMP-${TMP-/tmp}}}/$progname.$$
job=$1
backup="$tmpdir/orig.aux"

case $job in
  "")
    echo "usage: $progname file" >&2
    exit 1
esac

if test ! -f "$job.aux"; then
  echo "$progname: file \`$job.aux' does not exist." >&2
  exit 1
fi

trap '
  rm -rf "$tmpdir"
  exit 1
' 1 2 3 7 13 15
(umask 077; mkdir "$tmpdir") \
  || { echo "$progname: could not create directory \`$tmpdir'" >&2; exit 1; }

cat <"$job.aux" >"$backup" || {
  echo "$progname: could not create backup of file \`$job.aux' as \`$backup'." >&2
  rm -rf "$tmpdir"
  exit 1
}

sed '
  /^\\citation/ {
    s/\\IeC {\\CYRA }/á/g
    s/\\IeC {\\CYRB }/â/g
    s/\\IeC {\\CYRV }/÷/g
    s/\\IeC {\\CYRG }/ç/g
    s/\\IeC {\\CYRD }/ä/g
    s/\\IeC {\\CYRE }/å/g
    s/\\IeC {\\CYRYO }/³/g
    s/\\IeC {\\CYRZH }/ö/g
    s/\\IeC {\\CYRZ }/ú/g
    s/\\IeC {\\CYRI }/é/g
    s/\\IeC {\\CYRISHRT }/ê/g
    s/\\IeC {\\CYRK }/ë/g
    s/\\IeC {\\CYRL }/ì/g
    s/\\IeC {\\CYRM }/í/g
    s/\\IeC {\\CYRN }/î/g
    s/\\IeC {\\CYRO }/ï/g
    s/\\IeC {\\CYRP }/ð/g
    s/\\IeC {\\CYRR }/ò/g
    s/\\IeC {\\CYRS }/ó/g
    s/\\IeC {\\CYRT }/ô/g
    s/\\IeC {\\CYRU }/õ/g
    s/\\IeC {\\CYRF }/æ/g
    s/\\IeC {\\CYRH }/è/g
    s/\\IeC {\\CYRC }/ã/g
    s/\\IeC {\\CYRCH }/þ/g
    s/\\IeC {\\CYRSH }/û/g
    s/\\IeC {\\CYRSHCH }/ý/g
    s/\\IeC {\\CYRHRDSN }/ÿ/g
    s/\\IeC {\\CYRERY }/ù/g
    s/\\IeC {\\CYRSFTSN }/ø/g
    s/\\IeC {\\CYREREV }/ü/g
    s/\\IeC {\\CYRYU }/à/g
    s/\\IeC {\\CYRYA }/ñ/g
    s/\\IeC {\\cyra }/Á/g
    s/\\IeC {\\cyrb }/Â/g
    s/\\IeC {\\cyrv }/×/g
    s/\\IeC {\\cyrg }/Ç/g
    s/\\IeC {\\cyrd }/Ä/g
    s/\\IeC {\\cyre }/Å/g
    s/\\IeC {\\cyryo }/£/g
    s/\\IeC {\\cyrzh }/Ö/g
    s/\\IeC {\\cyrz }/Ú/g
    s/\\IeC {\\cyri }/É/g
    s/\\IeC {\\cyrishrt }/Ê/g
    s/\\IeC {\\cyrk }/Ë/g
    s/\\IeC {\\cyrl }/Ì/g
    s/\\IeC {\\cyrm }/Í/g
    s/\\IeC {\\cyrn }/Î/g
    s/\\IeC {\\cyro }/Ï/g
    s/\\IeC {\\cyrp }/Ð/g
    s/\\IeC {\\cyrr }/Ò/g
    s/\\IeC {\\cyrs }/Ó/g
    s/\\IeC {\\cyrt }/Ô/g
    s/\\IeC {\\cyru }/Õ/g
    s/\\IeC {\\cyrf }/Æ/g
    s/\\IeC {\\cyrh }/È/g
    s/\\IeC {\\cyrc }/Ã/g
    s/\\IeC {\\cyrch }/Þ/g
    s/\\IeC {\\cyrsh }/Û/g
    s/\\IeC {\\cyrshch }/Ý/g
    s/\\IeC {\\cyrhrdsn }/ß/g
    s/\\IeC {\\cyrery }/Ù/g
    s/\\IeC {\\cyrsftsn }/Ø/g
    s/\\IeC {\\cyrerev }/Ü/g
    s/\\IeC {\\cyryu }/À/g
    s/\\IeC {\\cyrya }/Ñ/g
  }
' <"$backup" >"$job.aux"

bibtex "$job"

cat "$backup" > "$job.aux"
rm -rf "$tmpdir"
exit 0
