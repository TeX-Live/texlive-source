s/^\([^ 	=]*\)[ 	]*=*[ 	]*\(.*\)/#ifndef DEFAULT_\1@#define DEFAULT_\1 \"\2\"@#endif/
