
#define ENTRY_NO    0x01
#define ENTRY_CUSTOM   0x10000
/* for JIS, quote only */
#define ENTRY_JQ    0x1000
/* for GB, CNS, JIS, KS */
#define ENTRY_G     0x02
#define ENTRY_C     0x04
#define ENTRY_J     0x08
#define ENTRY_K     0x10
#define ENTRY_GC    ENTRY_G|ENTRY_C
#define ENTRY_GJ    ENTRY_G        |ENTRY_J
#define ENTRY_GK    ENTRY_G                |ENTRY_K
#define ENTRY_CJ            ENTRY_C|ENTRY_J
#define ENTRY_CK            ENTRY_C        |ENTRY_K
#define ENTRY_JK                    ENTRY_J|ENTRY_K
#define ENTRY_GCJ   ENTRY_G|ENTRY_C|ENTRY_J
#define ENTRY_GCK   ENTRY_G|ENTRY_C        |ENTRY_K
#define ENTRY_GJK   ENTRY_G        |ENTRY_J|ENTRY_K
#define ENTRY_CJK           ENTRY_C|ENTRY_J|ENTRY_K
#define ENTRY_GCJK  ENTRY_G|ENTRY_C|ENTRY_J|ENTRY_K

extern int search_cjk_entry(long ch, long cjk);
extern int uniblock_iskanji;

#define U_OPEN_SQUOTE   0x2018
#define U_CLOSE_SQUOTE  0x2019
#define U_OPEN_DQUOTE   0x201C
#define U_CLOSE_DQUOTE  0x201D
