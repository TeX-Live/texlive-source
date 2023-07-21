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

@x section 3 (01-FEB-1992 ST)
#include <ctype.h> /* definition of |@!isalpha|, |@!isdigit| and so on */
#include <stdbool.h> /* definition of |@!bool|, |@!true| and |@!false| */
#include <stddef.h> /* definition of |@!ptrdiff_t| */
#include <stdint.h> /* definition of |@!uint8_t| and |@!uint16_t| */
#include <stdio.h> /* definition of |@!printf| and friends */
#include <stdlib.h> /* definition of |@!getenv| and |@!exit| */
#include <string.h> /* definition of |@!strlen|, |@!strcmp| and so on */
@y
#include ctype /* definition of |@!isalpha|, |@!isdigit| and so on */
               /* VMS searches text libraries faster */
#include stdbool /* definition of |@!bool|, |@!true| and |@!false| */
#include stddef /* definition of |@!ptrdiff_t| */
#include stdint /* definition of |@!uint8_t| and |@!uint16_t| */
#include stdio /* definition of |@!printf| and friends */
#include stdlib /* definition of |@!getenv| and |@!exit| */
#include string /* definition of |@!strlen|, |@!strcmp| and so on */
@z

@x section 68 (01-FEB-1992 ST)
programs are started. Here, for instance, we pass the operating system
a status of |EXIT_SUCCESS| if and only if only harmless messages were printed.
@y
programs are started. Here, for instance, we pass VAX/VMS
a status of |SS$_NORMAL| if and only if only harmless
messages have been printed.
A suitable status to signal an error condition to VAX/VMS could be
|SS$_ABORT|, telling the operating system to
print |"%SYSTEM-F-ABORT, abort"|, if |history > harmless_message|.
@z

@x section 68 (1987 BL) (01-FEB-1992 ST) (11-JAN-1993 DEK)
  if (history > harmless_message) return EXIT_FAILURE;
  else return EXIT_SUCCESS;
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

@x section 75 (01-FEB-1992 ST)
An omitted change file argument means that |"/dev/null"| should be used,
when no changes are desired.
@y
An omitted change file argument means that the
null device |"NL:"| should be used, when no changes are desired.
@z

@x section 75 (1987 BL) (01-FEB-1992 ST) (05-APR-1992 DEK)
  strcpy(change_file_name,"/dev/null");
@y
  strcpy(change_file_name,"NL:");
	/* {\tt NL:} is the VAX/VMS notation for {\tt /dev/null} */
@z

@x section 85 (01-FEB-1992 ST)
@* Index.
@y
@* VAX/VMS specific code.
We have used |SS$_NORMAL| and |SS$_ABORT| as return codes,
so we have to include the system message codes.

@<Include files@>=
#include ssdef /* we need VAX/VMS system messages */

@* Index.
@z
