/* FILE:	filenames.h
 * PURPOSE:	functions for handling file names
 * VERSION: 1.3 (August 1992)
 * VERSION: 1.4 (January 1994)
 * VERSION: 1.5 (September 1995)
 */

/*
 * Determine extension of <str>.
 */
char *extension(char *str);

/* 
 * Basename is the functional equivalent of BSD's basename utility.
 * When the suffix is NULL no suffix removal is done. When needed
 * a new string is allocated so that the original value of str is
 * not changed.
 */   
char *basename(char *str, char *suffix);

/*
 * Newname builds a new filename by adding or replacing the extension
 * of the given <name> with the new supplied <ext>.
 */
char *newname(char *name, char *ext);

/*
 * ps_resource returns true if name can be treated as a PS resource
 */
int ps_resource(char *name);
