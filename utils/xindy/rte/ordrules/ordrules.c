/* $Id: ordrules.c,v 1.9 2008/01/09 11:33:44 jschrod Exp $
 *----------------------------------------------------------------------
 *
 * Ordering Rules for Strings
 *
 * [history at end]
 */

#include "ordrules.h"
#include "rxsub.h"

#include <string.h>

/* switch on debugging by excluding the undef directive. */
#define debug
#undef  debug /* */
#include "debug.h"

#ifdef debug
#define DEBUG
#endif

RULE_TABLE* SortRules;  /* rules for generating sortkeys */

RULE_TABLE MergeRules;	/* rules for generating mergekeys */
GROUP_LIST GroupList;	/* list of character groups */
GROUP_LIST HeadingList;	/* list of group headings */

/* The following rule-types can be set by the user: */
#define RT_AUTOMATIC         0
#define RT_STRING            1
#define RT_REGEXP_BASIC      2
#define RT_REGEXP_EXTENDED   3
#define RT_CHAR              4

static RULE_TYPE get_ruletype PROTO(( char *lside, char *rside ));
static int       is_regexp PROTO(( char *s ));
static int       is_CHR_RULE PROTO(( char *s, char *r ));

char  ordrules_string_buffer [BUFLEN];
int   ordrules_string_buffer_used_bytes = 0;

       int   ordrules_msg_logging      = 0;
       char* ordrules_msg_buffer       = 0;
       int   ordrules_msg_buffer_ptr   = 0;
static size_t ordrules_msg_buffer_len   = BUFLEN;
static size_t ordrules_msg_buffer_avail = 0;
static int   ordrules_sort_rule_tables = 0;

#define DEFAULT_SORT_RULE_TABLES 8

#define DBG( expr ) if (ordrules_msg_logging!=0) { expr; }


void
logs( msg )
char* msg;
{
  size_t msglen = strlen( msg );

  if (NULL == ordrules_msg_buffer) {
    ordrules_msg_buffer = (char*) malloc (ordrules_msg_buffer_len);
    ordrules_msg_buffer_avail = ordrules_msg_buffer_len - 1;
    ordrules_msg_buffer_ptr = 0;
  }
  if (msglen > ordrules_msg_buffer_avail) {
    ordrules_msg_buffer_len *= 2;
   ordrules_msg_buffer = (char*) realloc (ordrules_msg_buffer,
					       ordrules_msg_buffer_len);
    ordrules_msg_buffer_avail = ordrules_msg_buffer_len -
      ordrules_msg_buffer_ptr - 1;
  }
  strcpy(&ordrules_msg_buffer[ordrules_msg_buffer_ptr], msg );
  ordrules_msg_buffer_ptr += msglen;
  ordrules_msg_buffer_avail -= msglen;
  ordrules_msg_buffer[ordrules_msg_buffer_ptr] = '\0';
}

void
logc( c )
char c;
{
  char msg[2];
  msg[0] = c;
  msg[1] = '\0';
  logs(msg);
}

void
logi( n )
int n;
{
  char msg[16];
  sprintf(msg, "%d", n);
  logs(msg);
}

/*
 * This function initializes the memory for the SortRules. Since the
 * number of tables is defined dynamically, we allow the repeated
 * definition of tables. In this case the old table is discarded.
 */

int
initialize (num_sort_tables)
int num_sort_tables;
{
  size_t fullsize = num_sort_tables * sizeof(RULE_TABLE);
  if (SortRules != NULL) {
    logs("ORDRULES: Removing old Sort-Tables!\n");
    free( SortRules );
  }
  ordrules_sort_rule_tables = num_sort_tables;
  SortRules = (RULE_TABLE*) malloc (fullsize);
  if (SortRules == NULL) {
      fprintf(stderr,"ordrules: malloc() of %d SortTables failed!\n",
	      num_sort_tables);
      exit(1);
  }
  logs("ORDRULES: Initializing "); logi(num_sort_tables);
  logs(" Sort Tables.\n");
  memset(SortRules, 0, fullsize);
  return(num_sort_tables);
}

int
add_sort_rule (run, left, right, isreject, ruletype)
int run;
char *left;
char *right;
int isreject;
int ruletype;
{
  if (SortRules == NULL) { /* No malloc() done yet */
    if (ordrules_sort_rule_tables == 0) { /* No specification yet. */
      ordrules_sort_rule_tables = DEFAULT_SORT_RULE_TABLES; /* use default */
    }
    logs("ORDRULES: Performing implicit initialization.\n");
    initialize( ordrules_sort_rule_tables ); /* ...then initialize */
  }
  if (0 <= run && run < ordrules_sort_rule_tables) {
    return( add_rule( SortRules[run], left, right, isreject, ruletype, run ));
  } else {
    logs("ORDRULES: add_sort_rule(): run is out of range.\n");
    return -1;
  }
}

int
add_merge_rule (left, right, isreject, ruletype)
char *left;
char *right;
int isreject;
int ruletype;
{
  return( add_rule( MergeRules, left, right, isreject, ruletype, -1));
}

/* insert rule 'left --> right' into ruletable 'table' */
int
add_rule( table, left, right, isreject, ruletype, run)
RULE_TABLE table;
char *left;
char *right;
int isreject;
int ruletype;
int run;
{
  RULE *r;
  RULE_LIST *list;
  char buffer[STRING_MAX];
  char *bptr = buffer;
  unsigned int pos = 0;

  /* new stuff */
  int errcode;
  char errmsg[STRING_MAX];

  /* Use extended regular expression syntax as default. */
  int cflag = REG_EXTENDED;

  dispstart( add_rule );
  dispstr( left );
  dispstr( right );

  r = (RULE *) malloc( sizeof(RULE) );

  switch ( ruletype ) {
    case RT_AUTOMATIC:
      r->type = get_ruletype( left, right ); break;
    case RT_STRING:
      r->type = STR_RULE; break;
    case RT_CHAR:
      r->type = CHR_RULE; break;
    case RT_REGEXP_BASIC:
    case RT_REGEXP_EXTENDED:
      r->type = REG_RULE; break;
  }

  switch (r->type) {

    case REG_RULE:

      if (ruletype == RT_REGEXP_BASIC) {
	/* Use the basic regular expressions as specified. */
	cflag = 0;
      }
      dispstart( REG_RULE );
      dispmsg( bptr copy );
      /* insert '^' in regular expression to match string prefix only */
      *bptr++ = '^'; strcpy( bptr, left );
      r->r.reg.lstr  = (char *) malloc( strlen(buffer) + 1);
      r->r.reg.type = cflag;
      strcpy( r->r.reg.lstr, buffer );
      dispmsg( malloc );
      r->r.reg.lside = (regex_t*) malloc (sizeof(regex_t));
      displong( r->r.reg.lside );
      dispstart( regcomp );
      if ( 0 != (errcode = regcomp( r->r.reg.lside, buffer, REG_EXTENDED ))) {
	regerror( errcode, r->r.reg.lside, errmsg, STRING_MAX );
	DBG(logs("Regular Expression Error: "); logs(errmsg); logs("\n");)
	return( errcode );
      }
      dispend( regcomp );
      r->r.reg.rside = (char *) malloc( strlen(right) + 1);
      strcpy( r->r.reg.rside, right );
      dispstr( r->r.reg.rside );
      displong( r->type );

      /*
       * calculate table index as the first character of 'left'
       * or FORALL_POS if the rule does not start with a fixed charater.
       * A rule starts with a fixed character if this char is escaped.
       * Otherwise the first character might be variable (ie, a character
       * class) or it might be optional.
       */

      if ( left[0] == BSH )  pos = (unsigned char)(left[1]); /* quoted */
      else
        if ( strchr(".([",left[0])==NULL || strchr("*?",left[1])==NULL )
	  pos = FORALL_POS; /* character class or optional */
        else
	  pos = (unsigned char)(left[0]); /* fixed */

      dispend( REG_RULE );
      break;

    case CHR_RULE:
      dispstart( CHR_RULE );
      r->r.chr  = *right;
      pos = (unsigned char)(*left);
      dispend( CHR_RULE );
      break;

    case STR_RULE:
      dispstart( STR_RULE );
      r->r.str.llen = strlen(left);
      r->r.str.lside = (char *) malloc( (size_t)(r->r.str.llen + 1) );
      strcpy(r->r.str.lside,left);
      r->r.str.rlen = strlen(right);
      r->r.str.rside = (char *) malloc( (size_t)(r->r.str.rlen + 1) );
      strcpy(r->r.str.rside,right);
      pos = (unsigned char)(*left);
      dispend( STR_RULE );
      break;
  }

  if (isreject) r->type |= REJECT;

  /* insert new rule into table */
  dispstart(inserting into table);
  dispuint(pos);
  displong(&table[0]);
  list = &table[pos];
  displong(list);
  displong(NULL);
  displong(list->first);
  displong(r);
  if (list->first == NULL) {
    list->first = r;
  } else {
    displong(list->last);
    displong(list->last->next);
    list->last->next = r;
  }
  displong(list->last);
  list->last = r;
  displong(r->next);
  r->next = NULL;
  dispend(inserting into table);

  DBG(
    logs("Mappings: add (");
    if (table == MergeRules) logs("merge"); else logs("sort");
    logs("-rule ");
    switch ( r->type & ~REJECT ) {
      case CHR_RULE:
	logs("`");logc(*left);logs("' `");logc(r->r.chr);logs("' :char");
	break;
      case STR_RULE:
	logs("`");;logs(r->r.str.lside); logs("' `");
	logs(r->r.str.rside);logs("' :string");
	break;
      case REG_RULE:
	logs("`");logs(buffer);logs("' `");
	logs(r->r.reg.rside);logs("' :");
	logc( (r->r.reg.type == REG_EXTENDED) ? 'e' : 'b' );
	logs("regexp");
	break;
    }
    if (run != -1) {
	logs(" :run ");
	logi(run);
    }
    if (r->type&REJECT) logs(" :again");
    logs(").\n");
  )
  dispend( add_rule );
  return( 0 );
}

/* get the type of the rule 'lside --> rside' */
static
RULE_TYPE get_ruletype( lside, rside )
char *lside;
char *rside;
{
  if ( is_regexp( lside ) || is_regexp( rside ) ) return REG_RULE;

  if ( lside[0] != NUL && lside[1] == NUL &&
       rside[0] != NUL && rside[1] == NUL ) return CHR_RULE;

  return STR_RULE;
}

static
int is_CHR_RULE ( lside, rside )
char *lside;
char *rside;
{
  if ( lside[0] != NUL && lside[1] == NUL &&
       rside[0] != NUL && rside[1] == NUL ) return CHR_RULE;

  return STR_RULE;
}

/* return TRUE, if 's' is a regular expression. */
static
int is_regexp( s )
char *s;
{
  for (;*s ; s++ )
    if (*s == BSH) {
      s++;
      if ('0' <= *s && *s <= '9') return TRUE;
    }
    else if (strchr("[]().*+$^?&",*s)) return TRUE;
  return FALSE;
}

#ifdef UNESCAPE
/* not necessary anymore */

/* replace all '\x' sequences with the character they represent */
static void
unescape( s )
register char *s;
{
  register char *d;
  for (d = s; *d = *s; d++)
    if (*s++ == BSH) *d = *s++;
}
#endif

/* apply the rules from 'table' to 'source' and place the substituted result
   in 'dest' */
void apply_rules( table, source, dest, buflen )
RULE_TABLE table;
register char *source;
register char *dest;
register size_t buflen;
{
  register RULE *r;
  register RULE_TYPE rtype;
  register char *newdest = 0;
  char newsource[STRING_MAX];

  /* new stuff */
#define NMATCH 10
  regmatch_t regmatch [NMATCH];
  int eflags = 0;

#ifdef DEBUG
  char *in = source, *out = dest;
#endif

  dispstart( apply_rules );
  dispstr( source );
  dispstr( dest );
  dispint( buflen );

  while ( *source ) {

    /* first check the rules beginning with character '*source' */
    dispint( (unsigned) *source );
    dispint( (unsigned)(*source) );
    dispint( (unsigned char)(*source) );
    dispint( (unsigned int)(*source) );
    for ( r = table[(unsigned char) *source].first; r; r = r->next ) {

      displong( r );
      rtype = (r->type & ~REJECT);

      if ( rtype == CHR_RULE ) {
	dispstart( CHR_RULE );
      	if ((r->type & REJECT))
	  newdest = dest;
	DBG(
	  logs("Mappings: (compare `");logs(source);logs("' :char `");
	  logc(*source); logs("') match!\n");
	  )
	*dest++ = r->r.chr;
	source++;
	dispend( CHR_RULE );
	break;
      }

      else if ( rtype == STR_RULE &&
		!strncmp(source, r->r.str.lside, (size_t)r->r.str.llen)) {
	DBG(
	  logs("Mappings: (compare `");logs(source);logs("' :string `");
	  logs(r->r.str.lside); logs("') match!\n");
	  )
	dispstart( STR_RULE );
	displong( dest );
	strcpy(dest, r->r.str.rside);
      	if ((r->type & REJECT)) newdest = dest;
	displong( newdest );
	dest   += r->r.str.rlen;
	displong( dest );
	source += r->r.str.llen;
	displong( source );
	dispend( STR_RULE );
      	break;
      }

      else {
	if ( rtype == REG_RULE ) {
	  dispstart( REG_RULE );
	  DBG(
	    logs("Mappings: (compare `");logs(source);logs("' :");
	    logc( (r->r.reg.type == REG_EXTENDED) ? 'e' : 'b' );
	    logs("regexp `");
	    logs(r->r.reg.lstr); logs("')");
	    )
	  if ( 0 == regexec( r->r.reg.lside, source,
			     NMATCH, regmatch, eflags ) ) {
	    DBG( logs(" match!\n"); )
	    nrxsub( r->r.reg.rside, regmatch, source, dest, buflen );
	    if ((r->type & REJECT))
	      newdest = dest;
	    dest   = strchr(dest,0);
	    source += regmatch[0].rm_eo;
	    dispend( REG_RULE );
	    break;
	  }
	  DBG( logs("\n"); )
	  dispend( REG_RULE );
	}
      }

    }

    displong( r );
    if ( r == NULL ) {

      dispmsg( r == NULL );
      /* check regular expression rules, that could match all prefixes */
      for ( r = table[FORALL_POS].first; r; r = r->next ) {
	dispstart( for-loop regexps );
	dispstr( r->r.reg.lstr );
	dispstr( r->r.reg.rside );
	displong( r->type );
	dispstr( source );
	DBG(
	  logs("Mappings: (compare `");logs(source);logs("' :");
	  logc( (r->r.reg.type == REG_EXTENDED) ? 'e' : 'b' );
	  logs("regexp `");
	  logs(r->r.reg.lstr); logs("')");
	  )
	if ( 0 == regexec( r->r.reg.lside, source,
			   NMATCH, regmatch, eflags ) ) {
	  DBG( logs(" match!\n"); )
	  dispstart( regexec match! );
	  nrxsub( r->r.reg.rside, regmatch, source, dest, buflen );
      	  if ((r->type & REJECT)) newdest = dest;
	  displong( dest );
	  displong( newdest );
	  source += regmatch[0].rm_eo;
	  displong( source );
	  dest   = strchr(dest,0);
	  displong( dest );
	  dispend( regexec match! );
	  dispend( for-loop regexps );
      	  break;
        }
	DBG( logs("\n"); )
	dispend( for-loop regexps );
      }
      /* no rule matched, copy character */
      if ( r == NULL ){
	dispstart( no rule matched! );
	displong( dest );
	displong( source );
	*dest++ = *source++;
	displong( dest );
	displong( source );
	dispend( no rule matched! );
      }
    }

    displong( r );
    /* should we reject? */
    if ( r != NULL && (r->type & REJECT)) {
      dispstart( REJECT );
      strcpy(dest,source);
      strcpy(newsource,newdest);
#ifdef DEBUG
      fprintf(stderr,"\nordrules: reject in '%s': '%s'.",in,newsource);
#endif
      apply_rules( table, newsource, newdest, STRING_MAX );
      dispend( REJECT );
      dispend( apply_rules );
      return;
    }
  }

  dispstart( last part );
  displong( dest );
  *dest = 0;
  ordrules_string_buffer_used_bytes = strlen( ordrules_string_buffer );
#ifdef DEBUG
#ifdef debug
  dispstr( out );
#endif
#endif

#ifdef DEBUG
  /* if ( strcmp(in,out) ) */
  fprintf(stderr,"\nordrules: %s '%s' generated for '%s'.", table == MergeRules ? "mergekey" : "sortkey", out, in );
#endif

  dispend( last part );
  dispend( apply_rules );
}

char *gen_sortkey ( key, run )
char *key;
int run;
{
  char *sortkey;
  dispstart( gen_sortkey );
  dispstr( key );

  /* If no initialization has been done yet, then do a fallback to the
     default tables. This should not happen usually. */
  if (SortRules == NULL) {
    logs("ORDRULES: Performing implicit initialization.\n");
    initialize( DEFAULT_SORT_RULE_TABLES );
  }

  if (0 <= run && run < ordrules_sort_rule_tables) {
    apply_rules( SortRules[run], key, ordrules_string_buffer, BUFLEN );
  }

  sortkey = (char*) malloc (strlen( ordrules_string_buffer ) +1);
  strcpy( sortkey, ordrules_string_buffer );
  DBG(
    logs("Mappings: (sort-mapping `"); logs(key);
    logs("' :run "); logi(run); logs(") -> `");
    logs(sortkey); logs("'.\n\n");
  )
  dispend( gen_sortkey );
  return( sortkey );
}


char *gen_mergekey ( key )
char *key;
{
  char *mergekey;
  apply_rules( MergeRules, key, ordrules_string_buffer, BUFLEN );
  mergekey = (char*) malloc (strlen( ordrules_string_buffer ) +1);
  strcpy( mergekey, ordrules_string_buffer );
  DBG(
    logs("Mappings: (merge-mapping `"); logs(key); logs("') -> `");
    logs(mergekey); logs("'.\n\n");
    )
  return( mergekey );
}


/* insert a new group entry ('group','letter') into 'list' */
#if 0
void
add_group( list, group, letter )
GROUP_LIST *list;
int group;
char *letter;
{
  GROUP_RULE *new;
  new = (GROUP_RULE *) malloc(sizeof(GROUP_RULE));
  new->id = (group >= 0 ? group + USORTGROUP : group - USORTGROUP);
  new->len = strlen(letter);
  new->let = (char *) malloc(new->len + 1);
  strcpy(new->let,letter);

  /* insert rule into list */
  if (list->first == NULL) list->first = new;
                 else list->last->next = new;
  list->last = new;
  new->next = NULL;

#ifdef DEBUG
  fprintf(stderr,"\nordrules: added group into list '%s' '%s' -> group: %d.",list == &GroupList ? "GroupList" : "HeadingList", new->let,new->id);
#endif
}

/* find the user defined group id for 'sortkey' */
int get_group( sortkey )
char *sortkey;
{
  register GROUP_RULE *r;
  for (r = GroupList.first; r && strncmp(sortkey,r->let,r->len); r = r->next);
#ifdef DEBUG
  if (r != NULL)
    fprintf(stderr,"\nordrules: sortkey '%s' is in group: %d.",sortkey,r->id);
#endif
  return ( r ? r->id : *sortkey );
}

/* find the user defined group heading for id 'group' */
char *get_heading( group )
int group;
{
  register GROUP_RULE *r;
  for ( r = HeadingList.first; r && r->id != group; r = r->next);
#ifdef DEBUG
  if (r != NULL)
    fprintf(stderr,"\nordrules: group: %d has heading '%s'.",r->id,r->let);
#endif
  return ( r ? r->let : NULL );
}
#endif


/*======================================================================
 *
 * $Log: ordrules.c,v $
 * Revision 1.9  2008/01/09 11:33:44  jschrod
 *     Update to CLISP 2.43.
 *
 * Revision 1.8  2005/05/02 21:39:53  jschrod
 *     xindy run time engine 3.0; as used for CLISP 2.33.2.
 *
 * Revision 1.7  1999/07/30 08:34:32  kehr
 * Checked in some debug stuff that was integrated for better testing.
 *
 * Revision 1.6  1997/10/20 11:23:13  kehr
 * New version of sorting rules. Sorting of more complex indexes (i.e.
 * French) is now possible.
 *
 * Revision 1.5  1997/01/17 16:43:40  kehr
 * Several changes for new version 1.1.
 *
 * Revision 1.4  1996/07/18  15:56:40  kehr
 * Checkin after all changes that resulted from the define-letter-group
 * modification were finished. Additionally I found an ugly bug in the
 * ordrules.c file that was discovered when running the system under
 * Solaris (which seems to have signed chars..Whee!). This is fixed now
 * and the Imakefiles and that stuff was improved, too.
 *
 * Revision 1.3  1996/07/03  18:48:48  kehr
 * Changed ordrules.c. The unescape() mechanism was removed so that not too
 * much quoting is necessary anymore. This feature was olnly for the use in
 * the old makeindex-3 systems.
 *
 * Revision 1.2  1996/03/27  20:29:10  kehr
 * It works. Today I had my first success in getting the FFI running with
 * the ordrules-library. The interface is defined in `ordrulei.lsp' and
 * allows direct access to the functions in `ordrules.c'.
 *
 * Revision 1.1  1996/03/26  17:30:58  kehr
 * First check-in of the rx-support for clisp.
 *
 * Revision 1.9  1992/01/03  13:22:10  schrod
 * "unsigned" is not a type. Computation of table index was wrong.
 *
 * Revision 1.8  1991/12/16  14:44:54  schrod
 * fmalloc() statt allocate()
 *
 * Revision 1.7  1991/12/10  20:25:50  schrod
 * Changed all function declarations to ANSI prototypes using ARGS().
 *
 * Revision 1.6  1991/12/10  20:09:58  schrod
 * Reject-rules (*merge_rule and *sort_rule) implemented.
 *
 * Revision 1.5  1991/12/10  19:59:53  schrod
 * Fixed bug in ordrules. Inserting a new group rule at the beginning of
 * a rule list works not with groups, that have the same prefix:
 *
 * 	group_rule "ae" 1
 * 	group_rule "a"  2
 *
 * would put 'aexxx' in group 2.
 *
 * Revision 1.4  1991/12/10  19:49:59  schrod
 * Optimized group-rule handling.
 *
 * Revision 1.3  1991/12/10  19:43:38  schrod
 * Optimized module 'ordrules':
 * - all three types of rules (chr,str,regexp) now allowed for both sort
 *   and merge keys
 *
 * Revision 1.2  1991/12/10  19:20:31  schrod
 * Changed the group merging algorithm to create the group-id of an index key
 * using user defined rules (specified in the style file with commands:
 * `sort_group' and `group_head') instead of the sortkey's first character.
 *
 * Revision 1.1  1991/12/10  17:15:54  schrod
 * Initial revision
 *
 */
