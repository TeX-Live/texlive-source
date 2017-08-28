/* Some operating systems might not have memmem. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stddef.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

void*
memmem(const void* haystack, size_t haystack_len,
       const void* needle, size_t needle_len)
{
    const unsigned char* s = (const unsigned char*) haystack;
    const unsigned char* ends = s + haystack_len - needle_len;
    const unsigned char* p = (const unsigned char*) needle;
    void* try;
    if (needle_len == 0)
        return (void*) haystack;
    while (s <= ends && (try = memchr(s, *p, ends - s + 1)))
        if (memcmp(try, p, needle_len) == 0)
            return (void*) try;
        else
            s = (const unsigned char*) try + 1;
    return 0;
}

#ifdef __cplusplus
}
#endif
