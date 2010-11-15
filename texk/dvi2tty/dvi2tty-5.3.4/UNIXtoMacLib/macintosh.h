/* macintosh.h  24.2.1996  Alex Viskovatoff
 *
 * Header file accompanying unix-to-macintosh.c.
 *
 * Written and copyright (c) 1996 by Alex Viskovatoff.
 * Permission is granted to distribute and modify this file.
 */


#define PATHNAME_LENGTH 256

#define isascii(c)	(((unsigned) c) <= 0x7f)


/* "DVI2ODVI" is equivalent to an arry of static unsigned long
   initialized to {'DVI2', 'ODVI'} - Textures and OzTeX
   type signatures of dvi files, respectively.                */

#define process_dvi_command_line(args) \
    docommand(args,0L,2,(unsigned long*)"DVI2ODVI")

int docommand(char ***,void*,int,unsigned long*);
void set_creator(const unsigned char *outfilename);
int getopt(int nargc, char * const *nargv, const char *ostr);
char *get_home_dir(void);
char *get_file_from_dialog(void);
pascal unsigned char not_rtf_file_p(void *parms);


/* The following are defined in getopt.c */

#define	index	strchr
#define rindex  strrchr
int getopt(int nargc, char * const *nargv, const char *ostr);
