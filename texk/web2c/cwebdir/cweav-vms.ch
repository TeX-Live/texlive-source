This is the change file for CWEB's CWEAVE for VAX/VMS.

created:
    1991 JM (John Mulhollen, Science Applications International Corporation)

modified:
    01-FEB-1992 ST (Stephan Trebels <trebels@ams02.dnet.gwdg.de>)
    > include ctype,stdio from textlibrary SYS$SHARE:VAXCDEF.TLB
    > change banner line to include (VAX/VMS)
    > allow $ in identifiers (*necessary* for VAX/VMS)
    ? will someone eventally make a CLD interface? (should be easy)

(also modified by Don Knuth to keep version numbers uptodate)

@x section 1 (01-FEB-1992 ST)
@d banner "This is CWEAVE (Version 3.64)\n"
@y
@d banner "This is CWEAVE (VAX/VMS Version 3.64)\n"
@z

@x section 6 (from common.h) (01-FEB-1992 ST)
#include <stdio.h>
@y
#include stdio /* VMS searches Textlibraries faster */
@z

@x section 38 (1991 JM) (01-FEB-1992 ST)
#include <ctype.h> /* definition of |isalpha|, |isdigit| and so on */
@y
#include ctype /* VMS searches Textlibraries faster */
@z

@x section 39 (01-FEB-1992 ST)
@d isxalpha(c) ((c)=='_') /* non-alpha character allowed in identifier */
@y
@d isxalpha(c) ((c)=='_' || (c)=='$') /* non-alpha characters allowed in id */
@z
