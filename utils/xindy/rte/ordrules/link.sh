file_list=''
mod_list=''
if test -r ordrulei.c; then
  file_list="$file_list"' ordrulei.o'
  mod_list="$mod_list"' ordrulei'
fi
make clisp-module CC="${CC}" CFLAGS="${CFLAGS}" INCLUDES="$absolute_linkkitdir"
NEW_FILES="$file_list ordrules.o rxsub.o"
NEW_LIBS="$file_list ordrules.o rxsub.o"
NEW_MODULES="$mod_list"
TO_LOAD='ordrulei version'
