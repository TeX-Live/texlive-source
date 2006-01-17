This is the change file for CWEB's CTANGLE for VAX/VMS.

created:
    01-FEB-1992 ST (Stephan Trebels <trebels@ams02.dnet.gwdg.de>)
    > include ctype,stdio from textlibrary SYS$SHARE:VAXCDEF.TLB
    > change banner line to include (VAX/VMS)
    ? will someone make a CLD interface? (should be easy)

(also modified by Don Knuth to keep version numbers uptodate)
(these changes not necessary for initial bootstrapping)

@x section 1 (01-FEB-1992 ST)
@d banner "This is CTANGLE (Version 3.64)\n"
@y
@d banner "This is CTANGLE (VAX/VMS Version 3.64)\n"
@z

@x section 6 (from common.h) (01-FEB-1992 ST)
#include <stdio.h>
@y
#include stdio /* VMS searches Textlibraries faster */
@z

@x section 62 (01-FEB-1992 ST)
#include <ctype.h> /* definition of |isalpha|, |isdigit| and so on */
@y
#include ctype /* definition of |isalpha|, |isdigit| and so on */
               /* VMS searches text libraries faster */
@z

@x section 63 (01-FEB-1992 ST)
@d isxalpha(c) ((c)=='_') /* non-alpha character allowed in identifier */
@y
@d isxalpha(c) ((c)=='_' || (c)=='$') /* non-alpha characters allowed in id */
@z
