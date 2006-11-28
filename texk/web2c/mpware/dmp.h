/* $Id: dmp.h,v 1.2 2004/09/19 21:51:38 karl Exp $
   Public domain.  */

#ifndef DMP_H
#define DMP_H

#define TEXFONTS_TYPE 1
#define DB_TYPE 2
#define CHARLIB_TYPE 3
#define TRFONTS_TYPE 4

extern FILE *fsearch(char *nam, char *ext, int path_type);
extern void fsearch_init(int argc, char **argv);

#define POOLMAX	65000		/* total characters in all font and char names */

extern int poolsize;		/* chars used so far in strpool */
extern char strpool[];		/* chars used so far in strpool */


#endif
