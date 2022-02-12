/* From potracelib_demo.c */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#include "potracelib.h"

struct MFoutlines_param_s {
 double xoffs ;
 double yoffs ;
 potrace_param_t *potrace_param;
};
typedef struct MFoutlines_param_s MFoutlines_param_t;

/* raster: bitmap wxh, pixel packed into a byte  */
/* w: nr. of columns                             */
/* h: nr. of rows                                */
/* opt_parmam: optional parameters for potrace   */
/* trace: curves returned by potrace             */
/* Return 1 if error, 0 if ok                    */
int potrace_getMFoutlines(const uint8_t *raster, int w, int h, MFoutlines_param_t opt_param,potrace_state_t **trace);
