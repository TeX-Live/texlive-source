/* $Id: ordrules.h,v 1.9 2008/01/09 11:33:44 jschrod Exp $
 *----------------------------------------------------------------------
 *
 * Ordering Rules for Strings
 *
 * [history at end]
 */

#ifndef _ORDRULES_H
#define _ORDRULES_H

#include <stdio.h>
#include "mkind.h"
#undef debug
#include "debug.h"
#include "regex.h"

#define BEGIN	1		/* representation of '\b' */
#define END	255		/* representation of '\e' */

/* values for RULE_TYPE */
#define CHR_RULE	0001
#define STR_RULE	0002
#define REG_RULE	0004
#define REJECT		0200

#define RULE_TYPE	unsigned char

/* structure representing a substitution rule */
typedef struct rule {

  RULE_TYPE type;	/* rule descriptor */

  union {

    char chr;		/* character transformation: 'chr1' --> 'chr2' */

    struct {		/* string substitution: 'str1' --> 'str2' */
      char *lside;
      int llen;
      char *rside;
      int rlen;
    } str;

    struct {		/* regular expression rule: 'rexp' --> 'str' */
      regex_t *lside;
      char    *lstr;
      char    *rside;
      char    type;
    } reg;

  } r;

  struct rule *next;	/* link to next rule */

} RULE;

/* RULE_LIST is a linked list of RULE's with a pointer to the last entry,
   which allows efficient appending of new rules. */
typedef struct {
  RULE *first;
  RULE *last;
} RULE_LIST;

/* RULE_TABLE is a table of RULE_LIST's, where at position 'c' it contains
   rules that could match strings beginning with character 'c'. The
   last position (FORALL_POS) in this table has a special meaning. It stands for
   regular expression rules that begin with a character class and
   therefore they should be matched against all strings.

     RULE_TABLE[c]		rules in this list: 'cxxxxx' --> 'foo'
     RULE_TABLE[FORALL_POS]	rules in this list: '[y-z]xxxx' --> 'foo'

*/

#define MAX_RULE_TABLE	257	/* # entries in a rule table */
#define FORALL_POS	256	/* position of regexp rules, that must be applied to all strings */

typedef RULE_LIST RULE_TABLE[MAX_RULE_TABLE];

extern RULE_TABLE* SortRules;	/* rules for generating sortkeys */
extern RULE_TABLE MergeRules;	/* rules for generating mergekeys */

typedef struct grouprule {
   short id;			/* group id */
   char *let;			/* sortkey prefix */
   int  len;			/* len(let) */
   struct grouprule *next;	/* link to next rule */
} GROUP_RULE;

typedef struct {
  GROUP_RULE *first;
  GROUP_RULE *last;
} GROUP_LIST;

extern GROUP_LIST GroupList;	/* list of character groups */
extern GROUP_LIST HeadingList;	/* list of group headings */

/* These variables are used in ordrulei.lsp. The generated code, in
   ordrulei.c uses sizeof() on them, thus the buffer size must be
   defined. */
#define BUFLEN ((size_t)1024)
extern char  ordrules_string_buffer[BUFLEN];
extern int   ordrules_string_buffer_used_bytes;
extern int   ordrules_msg_logging;
extern char* ordrules_msg_buffer;
extern int   ordrules_msg_buffer_ptr;


#define add_group_rule(letter, group) \
		add_group( &GroupList, group, letter )

#define add_heading(group, head) \
		add_group( &HeadingList, group, head )

int initialize PROTO(( int num_sort_tables ));
int open_debug_file PROTO(( char* filename ));
int close_debug_file PROTO(( ));

int get_group PROTO(( char *sortkey ));
char *get_heading PROTO(( int group ));

int  add_rule PROTO(( RULE_TABLE table, char *left, char *right,
		      int isreject, int ruletype, int run ));
void apply_rules PROTO(( RULE_TABLE table, char *source, char *dest, size_t buflen ));
void add_group PROTO(( GROUP_LIST *list, int group, char *letter ));

int add_sort_rule  PROTO((int run, char *left, char *right, int isreject, int ruletype));
int add_merge_rule PROTO((char *left, char *right, int isreject, int ruletype));

char* gen_sortkey  PROTO((char *key, int run));
char* gen_mergekey  PROTO((char *key));

#endif /* _ORDRULES_H */


/*======================================================================
 *
 * $Log: ordrules.h,v $
 * Revision 1.9  2008/01/09 11:33:44  jschrod
 *     Update to CLISP 2.43.
 *
 * Revision 1.8  2005/06/14 11:22:04  jschrod
 *     ordrulei.lisp is now ordrulei.lsp in distribution, as it is in CVS.
 *
 * Revision 1.7  2005/05/02 21:39:53  jschrod
 *     xindy run time engine 3.0; as used for CLISP 2.33.2.
 *
 * Revision 1.6  1999/07/30 08:37:38  kehr
 * Intermediate checkin.
 *
 * Revision 1.5  1997/10/20 11:23:14  kehr
 * New version of sorting rules. Sorting of more complex indexes (i.e.
 * French) is now possible.
 *
 * Revision 1.4  1997/01/17 16:43:42  kehr
 * Several changes for new version 1.1.
 *
 * Revision 1.3  1996/07/18  15:56:42  kehr
 * Checkin after all changes that resulted from the define-letter-group
 * modification were finished. Additionally I found an ugly bug in the
 * ordrules.c file that was discovered when running the system under
 * Solaris (which seems to have signed chars..Whee!). This is fixed now
 * and the Imakefiles and that stuff was improved, too.
 *
 * Revision 1.2  1996/03/27  20:29:11  kehr
 * It works. Today I had my first success in getting the FFI running with
 * the ordrules-library. The interface is defined in `ordrulei.lsp' and
 * allows direct access to the functions in `ordrules.c'.
 *
 * Revision 1.1  1996/03/26  17:31:00  kehr
 * First check-in of the rx-support for clisp.
 *
 * Revision 1.8  1991/12/16  14:47:04  schrod
 * updated documentation
 *
 * Revision 1.7  1991/12/10  20:25:53  schrod
 * Changed all function declarations to ANSI prototypes using ARGS().
 *
 * Revision 1.6  1991/12/10  20:10:02  schrod
 * Reject-rules (*merge_rule and *sort_rule) implemented.
 *
 * Revision 1.5  1991/12/10  19:59:57  schrod
 * Fixed bug in ordrules. Inserting a new group rule at the beginning of
 * a rule list works not with groups, that have the same prefix:
 *
 * 	group_rule "ae" 1
 * 	group_rule "a"  2
 *
 * would put 'aexxx' in group 2.
 *
 * Revision 1.4  1991/12/10  19:50:03  schrod
 * Optimized group-rule handling.
 *
 * Revision 1.3  1991/12/10  19:43:42  schrod
 * Optimized module 'ordrules':
 * - all three types of rules (chr,str,regexp) now allowed for both sort
 *   and merge keys
 *
 * Revision 1.2  1991/12/10  19:20:35  schrod
 * Changed the group merging algorithm to create the group-id of an index key
 * using user defined rules (specified in the style file with commands:
 * `sort_group' and `group_head') instead of the sortkey's first character.
 *
 * Revision 1.1  1991/12/10  17:15:58  schrod
 * Initial revision
 *
 */
