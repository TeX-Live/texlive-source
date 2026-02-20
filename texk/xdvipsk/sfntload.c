/*
 *   Loads a SFNT tables based font file.  It marks the characters as undefined.
 */
#include "xdvips.h" /* The copyright notice in that file is included too! */
#ifdef KPATHSEA
#include <kpathsea/c-pathmx.h>
#include <kpathsea/concatn.h>
#include <kpathsea/variable.h>
#ifdef WIN32
#define getcwd _getcwd
#endif
#else
/* Be sure we have `isascii'.  */
#ifndef WIN32
#if !(defined(HAVE_DECL_ISASCII) && HAVE_DECL_ISASCII)
#define isascii(c) (((c) & ~0x7f) == 0)
#endif
#else
#define getcwd _getcwd
#endif
#endif
/*
 *   The external declarations:
 */
#include "protos.h"

#include "mem.h"
#include "error.h"
#include "dpxfile.h"
#include "sfnt.h"
#include "cmap.h"

#include "tt_aux.h"
#include "tt_cmap.h"
#include "tt_table.h"

#include "cff.h"


#ifndef KPATHSEA
char *expandfilename(const char *src)
{
  const char *s, *var_end;
  const char *st = NULL;
  unsigned len;
  char *var, *ret;

  s = src;
  if (*s=='$') {
    s++;
    var_end = s;
    do {
      var_end++;
    } while ((isascii(*var_end) && isalnum((unsigned char)*var_end)) || *var_end == '_');
    st = var_end;
    var_end--; /* had to go one past */
    len = var_end - s + 1;
    var = mymalloc(len + 1);
    strncpy(var, s, len);
    var[len] = 0;
	s = getenv(var);
	free(var);
	if ( s && *s ) {
	  ret = mymalloc(strlen(s) + strlen(st) + 1);
	  strcpy(ret,s);
	  strcat(ret,st);
	  return ret;
	}
  }
  else {
	  ret = mymalloc(strlen(s) + 1);
	  strcpy(ret,s);
	  return ret;
  }

  return NULL;
}
#endif

static long
scale(long what, unsigned short unitsPerEm)
{
   return(((what / unitsPerEm) << 20) +
          (((what % unitsPerEm) << 20) + 500) / unitsPerEm);
}

#define MAXFILENAME 2056
FILE *lookup_font_file(const char *fpath, char **p_real_fpath, int *p_is_dfont)
{
    FILE *fp = NULL;
    char *real_fpath = NULL;
    char *fpath_exp = NULL;
    char *fname_ptr;

    if (p_is_dfont)
        *p_is_dfont = 0;

#ifdef KPATHSEA
    fpath_exp = kpse_var_expand(fpath);
#else
    fpath_exp = expandfilename(fpath);
#endif
    if (fpath_exp)
    {
        if ((fpath_exp[0] == '.') && (fpath_exp[1] == '/'))
        {
            char cwd[MAXFILENAME + 1];
            if (getcwd(cwd, MAXFILENAME))
            {
                strcat(cwd, "/");
                strncat(cwd, fpath_exp + 2, MAXFILENAME - strlen(cwd));
                free(fpath_exp);
                fpath_exp = strdup(cwd);
            }
        }

        if ((real_fpath = dpx_find_dfont_file(fpath_exp)) != NULL &&
            (fp = fopen(real_fpath, "rb")) != NULL)
        {
            if (p_is_dfont)
                *p_is_dfont = 1;
        }
        else
        {
            if (real_fpath)
                free(real_fpath);
            real_fpath = NULL;

            if (((real_fpath = dpx_find_opentype_file(fpath_exp)) == NULL
                && (real_fpath = dpx_find_truetype_file(fpath_exp)) == NULL)
                || (fp = fopen(real_fpath, "rb")) == NULL)
            {
                if (real_fpath)
                    free(real_fpath);
                real_fpath = NULL;

                fname_ptr = fpath_exp + strlen(fpath_exp);
                while ((fname_ptr > fpath_exp) && !IS_DIR_SEP(*(fname_ptr - 1)))
                    --fname_ptr;
                if (fname_ptr > fpath_exp)
                {
                    if ((real_fpath = dpx_find_dfont_file(fname_ptr)) != NULL &&
                        (fp = fopen(real_fpath, "rb")) != NULL)
                    {
                        if (p_is_dfont)
                            *p_is_dfont = 1;
                    }
                    else
                    {
                        if (real_fpath)
                            free(real_fpath);
                        real_fpath = NULL;

                        if (((real_fpath = dpx_find_opentype_file(fname_ptr)) != NULL
                            || (real_fpath = dpx_find_truetype_file(fname_ptr)) != NULL))
                            fp = fopen(real_fpath, "rb");
                    }
                }
            }
        }
    }

    if (fpath_exp)
        free(fpath_exp);

    if (p_real_fpath)
        *p_real_fpath = real_fpath;
    else
        if (real_fpath)
            free(real_fpath);

    return (fp);
}

int sfntload(fontdesctype *curfnt)
{
	FILE         *fp;
	char         *path;
	sfnt         *sfont;
	unsigned      offset = 0;
	int is_dfont = 0, index = 0;
    char  fontname[256];
	int glyphCount;
	int   length, n, k, no_of_chars;
	struct tt_head_table *head;
	struct tt_hhea_table  *hhea;
	struct tt_maxp_table *maxp;
	struct tt_longMetrics *metrics;
	tt_cmap       *ttcmap;
	CMap *cmap;
	cff_font *cffont = NULL;
	halfword gid, cid;
	chardesctype *cd;
    char *d;

    if (curfnt->resfont->Fontfile == NULL)
    {
        char *msg = concatn("! No font file name for ", curfnt->resfont->PSname, NULL);
        error(msg);
        free(msg);
    }

    if (!(fp = lookup_font_file(curfnt->resfont->Fontfile, &path, &is_dfont)))
    {
		char *msg = concatn("! Cannot proceed without the font: ", curfnt->resfont->Fontfile, NULL);
		error(msg);
	}
	if ( path ) free(path);
	d = curfnt->resfont->specialinstructions;
	if (is_dfont) {
		if ( d && *d ) {
			char *tmp = strstr(d,"index=");
			if ( tmp )
				sscanf(tmp + 6,"%d",&index);
		}
		sfont = dfont_open(fp, index);
	}
	else
		sfont = sfnt_open(fp);
	if (sfont->type == SFNT_TYPE_TTC) {
		if ( d && *d ) {
			char *tmp = strstr(d,"index=");
			if ( tmp )
				sscanf(tmp + 6,"%d",&index);
		}
		offset = ttc_read_offset(sfont, index);
		if (offset == 0) {
			ERROR("Invalid TTC index for font %s",curfnt->resfont->PSname);
		}
		curfnt->resfont->otftype = TTC_font;
	}
	else if (sfont->type == SFNT_TYPE_DFONT) {
		offset = sfont->offset;
		curfnt->resfont->otftype = DFONT_font;
	}
	else if (sfont->type == SFNT_TYPE_POSTSCRIPT)
		curfnt->resfont->otftype = PostScript_font;
	else
		curfnt->resfont->otftype = TrueType_font;
	curfnt->resfont->index = index;
	if (sfnt_read_table_directory(sfont, offset) < 0) {
		ERROR("Could not read OpenType/TrueType table directory for font %s",curfnt->resfont->PSname);
	}
	length = tt_get_ps_fontname(sfont, fontname, 255);
	if (length < 1) {
		length = MIN(strlen(curfnt->resfont->PSname), 255);
		strncpy(fontname, curfnt->resfont->PSname, length);
	}
	fontname[length] = '\0';
	k = length;
	for (n = 0; n < length; n++) {
		if (fontname[n] == 0) {
			memmove(fontname + n, fontname + n + 1, length - n - 1);
			k--;
		}
	}
	fontname[k] = '\0';
	if (strlen(fontname) == 0)
		ERROR("Can't find valid fontname for \"%s\".", curfnt->resfont->Fontfile);
	maxp = tt_read_maxp_table(sfont);
	if ( maxp ) {
		glyphCount = maxp->numGlyphs;
	    RELEASE(maxp);
	}
	else
		glyphCount = 1000;
	head = tt_read_head_table(sfont);
	hhea = tt_read_hhea_table(sfont);
	sfnt_locate_table(sfont, "hmtx");
	metrics = tt_read_longMetrics(sfont, glyphCount, hhea->numOfLongHorMetrics,hhea->numOfExSideBearings);
	if ( metrics == NULL )
		ERROR("Can't read valid fontname for \"%s\" metric.", curfnt->resfont->Fontfile);
	if (curfnt->resfont->otftype == PostScript_font) {
		offset = sfnt_find_table_pos(sfont, "CFF ");
		cffont = cff_open(sfont->stream, offset, 0);
		if (!cffont)
			ERROR("Could not open CFF font for font %s", curfnt->resfont->Fontfile);
		cff_read_charsets(cffont);
	}
	curfnt->resfont->luamap_idx = LuaMap_cache_find(curfnt->resfont->Vectfile ? curfnt->resfont->Vectfile : curfnt->resfont->PSname);
	if (curfnt->resfont->luamap_idx == -1) {
		ttcmap = tt_cmap_read(sfont, 3, 10); /* Microsoft UCS4 */
		if (!ttcmap) {
			ttcmap = tt_cmap_read(sfont, 3, 1); /* Microsoft UCS2 */
			if (!ttcmap) {
				ttcmap = tt_cmap_read(sfont, 0, 3); /* Unicode 2.0 or later */
				if (!ttcmap) {
					ttcmap = tt_cmap_read(sfont, 3, 0); /* Windows Symbol encoding */
					if (!ttcmap) {
						ttcmap = tt_cmap_read(sfont, 1, 0); /* Mac encoding */
						if (!ttcmap) {
							ERROR("Unable to read cmap table for font %s",curfnt->resfont->PSname);
						}
					}
				}
			}
		}
		curfnt->resfont->cmap_fmt = ttcmap->format;
		if ( ttcmap->format >= 12 )
			no_of_chars = MAX_CODE;
		else
			no_of_chars = MAX_2BYTES_CODE;
		curfnt->maxchars=no_of_chars;
		curfnt->iswide = 1;
		curfnt->codewidth = 2;
		curfnt->dir = 0;
		for (n = 1; n < no_of_chars; n++) {
			gid = tt_cmap_lookup(ttcmap, n);
			if (cffont && (cffont->flag  & FONTTYPE_CIDFONT) && (gid > 0)) {
				cid = cff_charsets_lookup_inverse(cffont, gid);
			}
			else
				cid = gid;
			if (gid > 0) {
				int li = scalewidth(scale(metrics[gid].advance, head->unitsPerEm), curfnt->scaledsize);
				cd = add_chardesc(curfnt, n);
				cd->TFMwidth = li;
				cd->pixelwidth = ((integer)(conv*li + 0.5));
				cd->cid = cid;
				cd->flags = (curfnt->resfont ? EXISTS : 0);
				cd->flags2 = EXISTS;
			}
		}
		tt_cmap_release(ttcmap);
	}
	else {
		luamaptype *map, *current, *tmp;
		map = LuaMap_cache_get(curfnt->resfont->luamap_idx);
		curfnt->maxchars = MAX_CODE;
		curfnt->iswide = 1;
		curfnt->codewidth = 2;
		curfnt->dir = 0;
		HASH_ITER(hh, map, current, tmp) {
			gid = current->gid;
//			if (cffont && (cffont->flag  & FONTTYPE_CIDFONT) && (gid > 0)) {
//				cid = cff_charsets_lookup_inverse(cffont, gid);
//			}
//			else
				cid = gid;
			if (gid > 0) {
				int li = scalewidth(scale(metrics[gid].advance, head->unitsPerEm), curfnt->scaledsize);
				cd = add_chardesc(curfnt, current->charcode);
				cd->TFMwidth = li;
				cd->pixelwidth = ((integer)(conv*li + 0.5));
				cd->cid = cid;
				cd->flags = (curfnt->resfont ? EXISTS : 0);
				cd->flags2 = EXISTS;
			}
		}
	}
	curfnt->loaded = 1;
	free(metrics);
	free(hhea);
	free(head);
	if (cffont)
		cff_close(cffont);
	sfnt_close(sfont);
	fclose(fp);
	return glyphCount;
}
