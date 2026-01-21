/*
*   Loads a opentype font charcodes mapping file from Luatex.
*/
#include "xdvips.h" /* The copyright notice in that file is included too! */
#ifdef KPATHSEA
#include <kpathsea/c-pathmx.h>
#include <kpathsea/concatn.h>
#include <kpathsea/variable.h>
#else
/* Be sure we have `isascii'.  */
#ifndef WIN32
#if !(defined(HAVE_DECL_ISASCII) && HAVE_DECL_ISASCII)
#define isascii(c) (((c) & ~0x7f) == 0)
#endif
#endif
#endif
/*
*   The external declarations:
*/
#include "protos.h"
#include "mem.h"
#include "error.h"
#include "dpxfile.h"
#include "glyphmap.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "unicode.h"

#define LUAMAP_CACHE_ALLOC_SIZE 16u
#define LUAMAP_DEBUG_STR "LuaMap"

struct LuaMap {
	char   *name;
	luamaptype *luamap;
};

struct LuaMap_cache {
  int    num;
  int    max;
  struct LuaMap **luamaps;
};

static struct LuaMap_cache *__cache = NULL;

#define CHECK_ID(n) do {\
                        if (! __cache)\
                           ERROR("%s: LuaMap cache not initialized.", LUAMAP_DEBUG_STR);\
                        if ((n) < 0 || (n) >= __cache->num)\
                           ERROR("Invalid LuaMap ID %d", (n));\
                    } while (0)

static const long hextable[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static long hexdec(unsigned const char *hex) {
	long ret = 0;
	while (*hex && ret >= 0) {
		ret = (ret << 4) | hextable[*hex++];
	}
	return ret;
}

static struct LuaMap *LuaMap_new()
{
	struct LuaMap *map = NEW(1, struct LuaMap);
	map->name = NULL;
	map->luamap = NULL;
	return map;
}

static int LuaMap_cache_add(const char* luamap_name, struct LuaMap** map_ptr)
{
    int id = -1;
    struct LuaMap* map = NULL;

    assert(map_ptr);

    if (__cache->num >= __cache->max)
    {
        __cache->max += LUAMAP_CACHE_ALLOC_SIZE;
        __cache->luamaps = RENEW(__cache->luamaps, __cache->max, struct LuaMap*);
    }
    id = __cache->num;
    (__cache->num)++;
    map = __cache->luamaps[id] = LuaMap_new();

    map->name = NEW(strlen(luamap_name) + 1, char);
    strcpy(map->name, luamap_name);

    *map_ptr = map;
    return id;
}

static int LuaMap_parse(struct LuaMap *map, FILE *fp)
{
	luamaptype *lmp;
	int k, m, n, charcode, cid;
	unsigned short gid;
	char strbuf[512], str[5];
	unsigned int *tounicode;
	char seps[] = ",";
	char *token;

	while (fgets(strbuf, 512, fp) != NULL) {
		token = strtok(strbuf, seps);
		m = 0;
		while (token != NULL) {
			switch (m) {
			case 0:
				charcode = atoi(token);
				break;
			case 1:
				gid = atoi(token);
				break;
			case 2:
				if (token[strlen(token) - 1] == '\n')
					token[strlen(token) - 1] = 0;
				if (strcmp(token, "nil") == 0) {
					n = 0;
					tounicode = NEW(1, unsigned int);
				}
				else {
					n = strlen(token) / 4;
					if (strlen(token) % 4)
					{
						n++;
						PRINTF_PR("Error: wrong format of the unicode %s in the map %s.\n", token, map->name);
					}
					tounicode = NEW(n, unsigned int);
					for (k = 0; k < n; k++) {
						memset(str, 0, 5);
						strncpy(str, &token[k * 4], 4);
						tounicode[k] = hexdec(str);
					}
				}
				lmp = NEW(1, luamaptype);
				lmp->charcode = (Otf_Enc_Type == enc_gid)? gid : charcode;
				lmp->gid = gid;
				lmp->tu_count = n;
				lmp->tounicode = tounicode;
				HASH_ADD_INT(map->luamap, gid, lmp);
				break;
			default:
				break;
			}
			if (m < 2 ) {
				token = strtok(NULL, seps);
				m++;
			}
			else
				break;
		}
	}

	return 0;
}

static boolean is_private(unsigned long ucode)
{
    return (((ucode >= 0xE000) && (ucode < 0xF900)) || /* Private Use Areas */
            ((ucode >= 0xF0000) && (ucode < 0xFFFFE)) ||
            ((ucode >= 0x100000) && (ucode < 0x10FFFE)));
}

unsigned int *uni32_2_utf16(unsigned long ucode, int *p_ucnt)
{
    unsigned int *tounicode = NULL;
    int ucnt = 0;

    assert(p_ucnt);

    if (ucode <= 0x10FFFF)
    {
        if (ucode >= 0x10000L)
        {
            ucnt = 2;
            tounicode = NEW(ucnt, unsigned int);
            assert(tounicode);
            tounicode[0] = UTF32toUTF16HS(ucode);
            tounicode[1] = UTF32toUTF16LS(ucode);
        }
        else
        {
            ucnt = 1;
            tounicode = NEW(ucnt, unsigned int);
            assert(tounicode);
            tounicode[0] = ucode;
        }
    }

    *p_ucnt = ucnt;
    return (tounicode);
}

unsigned long *utf16_2_uni32(const unsigned int *tounicode, int ucnt, int *p_uni32_cnt)
{
    unsigned long *uni32_arr = NULL;

    assert(p_uni32_cnt);
    assert(tounicode);

    uni32_arr = NEW(ucnt, unsigned long);
    assert(uni32_arr);
    unsigned long uni32;
    int ii, jj;

    jj = 0;
    for (ii = 0; ii < ucnt; ii++)
    {
        if ((tounicode[ii] & 0xFC00) == 0xD800)
            uni32 = (tounicode[ii] & 0x3FF) << 10;
        else if ((tounicode[ii] & 0xFC00) == 0xDC00)
        {
            uni32 = (uni32 | (tounicode[ii] & 0x3FF)) + 0x10000;
            uni32_arr[jj++] = uni32;
        }
        else
            uni32_arr[jj++] = tounicode[ii];
    }

    *p_uni32_cnt = jj;

    return(uni32_arr);
}

void add_glyph_tounicode(
    struct LuaMap *map,
    FT_Face ft_face,
    FT_UInt gid,
    FT_ULong ucode,
    boolean overwrite
)
{
    unsigned int *tounicode = NULL;
    char glyph_name[GLYPH_NAME_LEN + 1];
    unsigned long *uni32_arr;
    int ucnt, uni32_cnt;
    int igid, ii;
    luamaptype *lmp;
    static char map_line[ENC_BUF_SIZE + 1];
    FT_Error ft_error = FT_Err_Ok;

    ft_error = FT_Get_Glyph_Name(ft_face, gid, glyph_name, GLYPH_NAME_LEN);
    if (ft_error)
        glyph_name[0] = '\0';

    if (is_private(ucode) && glyph_name[0])
    {
        tounicode = get_glname_tounicode(glyph_name, map->name, "otf:", FALSE, &ucnt, NULL);
        if (tounicode)
        {
            uni32_arr = utf16_2_uni32(tounicode, ucnt, &uni32_cnt);
            if (uni32_arr)
            {
                for (ii = 0; ii < uni32_cnt; ii++)
                    if (is_private(uni32_arr[ii]))
                    {
                        RELEASE(tounicode);
                        tounicode = NULL;
                        break;
                    }
                RELEASE(uni32_arr);
            }
        }
    }

    lmp = NULL;
    igid = gid;
    HASH_FIND_INT(map->luamap, &igid, lmp);
    if (lmp)
    {
        if (overwrite)
        {
            RELEASE(lmp->tounicode);
        }
        else
            lmp = NULL;
    }
    else
        if (Otf_Enc_Type == enc_gid) /* no new entries for charcode encoded .dvi's */
        {
            lmp = NEW(1, luamaptype);
            assert(lmp);
            lmp->charcode = lmp->gid = gid;
            HASH_ADD_INT(map->luamap, gid, lmp);
        }

    if (lmp)
    {
        if (tounicode)
        {
            lmp->tounicode = tounicode;
            lmp->tu_count = ucnt;
        }
        else
        {
            lmp->tounicode = uni32_2_utf16(ucode, &ucnt);
            lmp->tu_count = ucnt;
        }

        if (glyph_name[0])
        {
            strcpy(map_line, "load_touni({['otf:");
            strncat(map_line, map->name, ENC_BUF_SIZE - strlen(map_line) - 20);
            map_line[ENC_BUF_SIZE] = '\0';
            strlwr(map_line);
            strcat(map_line, "/");
            strncat(map_line, glyph_name, ENC_BUF_SIZE - strlen(map_line) - 20);
            map_line[ENC_BUF_SIZE] = '\0';
            strcat(map_line, "'] = {");
            for (ii = 0; ii < ucnt; ii++)
            {
                if (strlen(map_line) >= ENC_BUF_SIZE - 20)
                    break;
                sprintf(map_line + strlen(map_line), "0x%04X, ", lmp->tounicode[ii]);
            }
            strcat(map_line, "}}, false, \'\')");
            if (luaL_dostring(L, map_line))
            {
                PRINTF_PR("Error: Lua script %.500s execution error: %.500s.\n", map_line, lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
    }
}

/* iterates through all unicode values of the font encoding and fills LuaMap */
static void LuaMap_ftget(struct LuaMap *map, FT_Face ft_face)
{
    FT_UInt gid;
    FT_ULong ucode;

    gid = 0;
    ucode = FT_Get_First_Char(ft_face, &gid);
    while (TRUE)
    {
        add_glyph_tounicode(map, ft_face, gid, ucode, TRUE);

        if (gid == 0)
            break;
        ucode = FT_Get_Next_Char(ft_face, ucode, &gid);
    }

    /* adding unicodeless gid's */
    for (gid = 0; gid < ft_face->num_glyphs; gid++)
        add_glyph_tounicode(map, ft_face, gid, 0x100000 + (gid & 0x7FFF), FALSE);
}

static void LuaMap_release(struct LuaMap *map)
{
	luamaptype *current, *tmp;

	if (map->name)
		RELEASE(map->name);
	HASH_ITER(hh, map->luamap, current, tmp) {
		RELEASE(current->tounicode);
		HASH_DEL(map->luamap, current);  /* delete it (users advances to next) */
		RELEASE(current);            /* free it */
	}
	RELEASE(map);
}


void
LuaMap_cache_init (void)
{
  static unsigned char range_min[2] = {0x00, 0x00};
  static unsigned char range_max[2] = {0xff, 0xff};

  if (__cache)
    ERROR("%s: Already initialized.", LUAMAP_DEBUG_STR);

  __cache = NEW(1, struct LuaMap_cache);

  __cache->max   = LUAMAP_CACHE_ALLOC_SIZE;
  __cache->luamaps = NEW(__cache->max, struct LuaMap *);
  __cache->num   = 0;
}

luamaptype *
LuaMap_cache_get (int id)
{
  CHECK_ID(id);
  return __cache->luamaps[id]->luamap;
}

int
LuaMap_cache_find (const char *luamap_name)
{
  int   n, id = 0;
  char strbuf[500], *p;
  FILE *fp = NULL;
  struct LuaMap* map = NULL;
  resfont_ref* rfnt_ref = NULL;
  struct resfont *rfnt = NULL;
  FT_Error ft_error = FT_Err_Ok;
  FT_Face ft_face = NULL;
  char *otf_name = NULL;

  if (!__cache)
    LuaMap_cache_init();
  ASSERT(__cache);

  for (id = 0; id < __cache->num; id++) {
    char *name = NULL;
    /* CMapName may be undefined when processing usecmap. */
    name = __cache->luamaps[id]->name;
    if (name && strcmp(luamap_name, name) == 0) {
      return id;
    }
  }

  id = -1;
  if (Otf_Enc_Type == enc_charcode)
  {
  if ((fulliname == 0) || (strlen(fulliname) == 0))
	  return -1;
  strcpy(strbuf, fulliname);
  p = strbuf + strlen(strbuf) - 1;
  while (p > strbuf) {
	  if ((*p == '\\') || (*p == '/')) {
		  *(p + 1) = 0;
		  break;
	  }
	  p--;
  }
  if (p == strbuf) 
	  *p = 0;
  strcat(strbuf, OTFMAPFILEDIR);
  strcat(strbuf, luamap_name);
  strcat(strbuf, OTFENCDFILEEXT);
  fp = DPXFOPEN(strbuf, DPX_RES_TYPE_TEXT);
  if (fp)
  {
      id = LuaMap_cache_add(luamap_name, &map);

      if (id >= 0)
          if (LuaMap_parse(map, fp) < 0)
              ERROR("%s: Parsing CMap file failed.", LUAMAP_DEBUG_STR);

      DPXFCLOSE(fp);
  }
  }
  if ((Otf_Enc_Type == enc_gid) || (charcode_otf_g2u && (id >= 0))) /* no new map for charcode encoded .dvi's */
  {
      rfnt_ref = lookup_v(luamap_name);
      if (rfnt_ref)
      {
          rfnt = rfnt_ref->resfont_ptr;
          if (rfnt && rfnt->Fontfile)
          {
              otf_name = kpse_var_expand(rfnt->Fontfile);
              if (otf_name)
              {
                  ft_error = FT_New_Face(ft_lib, otf_name, rfnt->index, &ft_face);
                  if (!ft_error)
                  {
                      ft_error = FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE);
                   /*
                      if (ft_error)
                          ft_error = FT_Select_Charmap(ft_face, FT_ENCODING_MS_UNICODE);
                      if (ft_error)
                          ft_error = FT_Select_Charmap(ft_face, FT_ENCODING_APPLE_UNICODE);
                    */
                      if (!ft_error)
                      {
                          if (id < 0) /* if (Otf_Enc_Type == enc_charcode) branch skipped, the map is not yet present */
                              id = LuaMap_cache_add(luamap_name, &map);

                          if (id >= 0)
                              LuaMap_ftget(map, ft_face);
                      }
                      else
                          WARN("%s: Opentype font charmap selection error.", LUAMAP_DEBUG_STR); /* TODO: ft_error evaluation */

                      FT_Done_Face(ft_face);
                  }
                  else
                      WARN("%s: Unable to open font %s", LUAMAP_DEBUG_STR, otf_name); /* TODO: ft_error evaluation */
                  free(otf_name);
                  otf_name = NULL;
              }
              else
                  WARN("%s: Unable to expand font file name %s", LUAMAP_DEBUG_STR, rfnt->Fontfile);
          }
          else
              WARN("%s: Font object error: %s", LUAMAP_DEBUG_STR, luamap_name);
      }
      else
          WARN("%s: Font object not found: %s", LUAMAP_DEBUG_STR, luamap_name);
  }

  return id;
}

void
LuaMap_cache_close (void)
{
  if (__cache) {
    int id;
    for (id = 0; id < __cache->num; id++) {
      LuaMap_release(__cache->luamaps[id]);
    }
    RELEASE(__cache->luamaps);
    RELEASE(__cache);
    __cache = NULL;
  }
}
