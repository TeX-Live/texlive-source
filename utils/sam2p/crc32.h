/* crc32.c -- Calculate CRC-32 for GZIP + PNG
 */

#ifndef CRC32_H
#define CRC32_H 1

#ifdef __GNUC__
#pragma interface
#endif
 
#include "config2.h"

#define CRC32_INITIAL ((unsigned PTS_INT32_T)0)
/** Usage:
 * unsigned PTS_INT32_T crc=CRC32_INITIAL;
 * crc=crc32(crc, "alma", 4);
 * crc=crc32(crc, "korte", 5);
 * ...
 * putchar( (char)(crc & 0xff) );
 * putchar( (char)((crc >> 8) & 0xff) );
 * putchar( (char)((crc >> 16) & 0xff) );
 * putchar( (char)((crc >> 24) & 0xff) );
 */
extern
#ifdef __cplusplus
"C"
#endif
unsigned PTS_INT32_T crc32 _((unsigned PTS_INT32_T oldcrc, char PTS_const *s, slen_t slen));

#endif /* CRC32_H */
