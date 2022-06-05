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
@d banner "This is CWEAVE (Version 4.8)"
@y
@d banner "This is CWEAVE (VAX/VMS Version 4.8)"
@z

@x section 4 (01-FEB-1992 ST)
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
