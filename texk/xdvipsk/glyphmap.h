/* -------------------------------
 * glyphmap.h
 * header file for external map file loading functions
 */

#ifdef XDVIPSK

#define ENC_BUF_SIZE    0x1000
#define MAP_BUFSIZE     100000
#define PFB_EXT         ".pfb"
#define G2U_EXT         ".g2u"
#define LUA_EXT         ".lua"
#define G2U_FORMAT      kpse_cmap_format

extern void init_map_buf(char **p_map_buf, char **p_map_buf_ptr, size_t *p_map_buf_len);
extern void append_map_buf(char **p_map_buf, char **p_map_buf_ptr, size_t *p_map_buf_len, const char *str);

#define ENC_REF_SIGNATURE " ReEncodeFont"
#define ALIAS_ID_DELIM '#'
/*
 * removes alias id from the end of the aliased (encoding) file name
 * returns pointer to a static buffer, no need for deletion
 * not reentrant
 */
extern char *get_alias_fname(const char *alias_name);

#endif /* XDVIPSK */
