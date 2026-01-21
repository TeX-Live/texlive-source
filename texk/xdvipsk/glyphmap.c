/* -------------------------------
 * glyphmap.c
 * external map file loading functions
 */

#ifdef XDVIPSK
#include "xdvips.h" /* The copyright notice in that file is included too! */
#include "glyphmap.h"
/*
 *   The external declarations:
 */
#include "protos_add.h"

void init_map_buf(char **p_map_buf, char **p_map_buf_ptr, size_t *p_map_buf_len)
{
    assert(p_map_buf);
    assert(p_map_buf_ptr);
    assert(p_map_buf_len);

    *p_map_buf_len = MAP_BUFSIZE;
    *p_map_buf = malloc(*p_map_buf_len);
    if (!*p_map_buf)
        error("Out of memory");
    *p_map_buf_ptr = *p_map_buf;
}

void append_map_buf(char **p_map_buf, char **p_map_buf_ptr, size_t *p_map_buf_len, const char *str)
{
    size_t map_line_len = 0;
    int cur_g2u_buf_offset = 0;

    assert(p_map_buf);
    assert(p_map_buf_ptr);
    assert(p_map_buf_len);
    assert(str);

    map_line_len = strlen(str);
    while (*p_map_buf_ptr + map_line_len >= *p_map_buf + *p_map_buf_len - 1)
    {
        cur_g2u_buf_offset = *p_map_buf_ptr - *p_map_buf;
        *p_map_buf_len += MAP_BUFSIZE;
        *p_map_buf = realloc(*p_map_buf, *p_map_buf_len);
        if (!*p_map_buf)
            error("Out of memory");
        *p_map_buf_ptr = *p_map_buf + cur_g2u_buf_offset;
    }
    strcpy(*p_map_buf_ptr, str);
    *p_map_buf_ptr += map_line_len;
}
#endif /* XDVIPSK */
