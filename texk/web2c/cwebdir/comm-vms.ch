This is the change file for CWEB's COMMON under VAX/VMS.

created:
    1987 BL (Bjorn Larsen, University of Oslo)

modified:
    01-FEB-1992 ST (Stephan Trebels <trebels@ams02.dnet.gwdg.de>)
    > include ctype,stdio,ssdef from textlibrary SYS$SHARE:VAXCDEF.TLB
    > change error return code to SS$_ABORT
      (perhaps better than %NONAME-E-NOMSG, Message number 2 )

(also modified by Don Knuth to match changes in the master file)
(only the two changes by BL are necessary for initial bootstrapping
 via hand-editing of common.c)

@x section 5 (01-FEB-1992 ST)
#include <ctype.h>
@y
#include ctype /* VMS searches Textlibraries faster */
@z

@x section 8  (01-FEB-1992 ST)
#include <stdio.h>
@y
#include stdio /* VMS searches Textlibraries faster */
@z

@x section 61 (01-FEB-1992 ST)
programs are started. Here, for instance, we pass the operating system
a status of 0 if and only if only harmless messages were printed.
@y
programs are started. Here, for instance, we pass VAX/VMS
a status of |SS$_NORMAL| if and only if only harmless
messages have been printed.
A suitable status to signal an error condition to VAX/VMS could be
|SS$_ABORT|, telling the operating system to
print |"%SYSTEM-F-ABORT, abort"|, if |history > harmless_message|.
@z

@x section 61 (1987 BL) (01-FEB-1992 ST) (11-JAN-1993 DEK)
  if (history > harmless_message) return(1);
  else return(0);
@y
/*
   VAX/VMS and UNIX have different error status conventions.
   VAX/VMS uses odd values (for example |SS$_NORMAL|) to indicate success,
   even values indicate errors, resulting in messages displayed
   on the screen. |SS$_ABORT| has been chosen, to indicate an
   error and display something that's not complete nonsense.
*/
  if (history > harmless_message) exit(SS$_ABORT);
  else exit(SS$_NORMAL);
@z

@x section 69 (01-FEB-1992 ST)
An omitted change file argument means that |"/dev/null"| should be used,
when no changes are desired.
@y
An omitted change file argument means that the
null device |"NL:"| should be used, when no changes are desired.
@z

@x section 70 (1987 BL) (01-FEB-1992 ST) (05-APR-1992 DEK)
  if (found_change<=0) strcpy(change_file_name,"/dev/null");
@y
  if (found_change<=) strcpy(change_file_name,"NL:");
	/* {\tt NL:} is the VAX/VMS notation for {\tt /dev/null} */
@z

@x section 82 (01-FEB-1992 ST)
@** Index.
@y
@* VAX/VMS specific code.
We have used |SS$_NORMAL| and |SS$_ABORT| as return codes,
so we have to include the system message codes.

@<Include files@>=
#include ssdef /* we need VAX/VMS system messages */

@** Index.
@z
