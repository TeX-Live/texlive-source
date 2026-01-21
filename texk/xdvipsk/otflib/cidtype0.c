/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright (C) 2007-2014 by Jin-Hwan Cho and 
	Shunsaku Hirata, the dvipdfmx project team.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

/*
 * CID-Keyed Font support:
 *
 *  Only CFF/OpenType CID-Keyed Font with Type 2 charstrings is supported.
 *
 */ 

#include <config.h>

#include "system.h"
#include "numbers.h"
#include "mem.h"
#include "error.h"

#include "xdvips.h"
#include "protos.h"

#include "cff_types.h"
#include "cff_limits.h"
#include "cff.h"
#include "cff_dict.h"
#include "cs_type2.h"

#include "cmap.h"
#include "cmap_write.h"
#include "cidtype0.h"

#define c1	52845
#define c2	22719

static int  verbose   = 0;
static UT_icd subr_icd = {sizeof(cs_type1subr), NULL, NULL, NULL };


/* These subroutines are code by Adobe for this exact use (from T1_Spec.pdf) */

	/* 3 0 callothersubr pop pop setcurrentpoint return */
static const card8 subrs0[] = { 3+139, 0+139, 12, 16, 12, 17, 12, 17, 12, 33, 11 };
	/* 0 1 callothersubr return */
static const card8 subrs1[] = { 0+139, 1+139, 12, 16, 11 };
	/* 0 2 callothersubr return */
static const card8 subrs2[] = { 0+139, 2+139, 12, 16, 11 };
	/* return */
static const card8 subrs3[] = { 11 };
	/* This one I created myself to do hint substitution */
	/* <subr number presumed to be on stack> 1 3 callother pop callsubr */
static const card8 subrs4[] = { 1+139, 3+139, 12, 16, 12, 17, 10, 11 };

	/* These others from adobe for multiple master */
	/* They need some fix up before they are used (the stack count depends on the # instances). */
	/* <n> 14 callothersubr pop return */
static const card8 subrs5[] = { 139, 14+139, 12, 16, 12, 17, 11 };
	/* 2*<n> 15 callothersubr pop pop return */
static const card8 subrs6[] = { 139, 15+139, 12, 16, 12, 17, 12, 17, 11 };
	/* 3*<n> 16 callothersubr pop pop pop return */
static const card8 subrs7[] = { 139, 16+139, 12, 16, 12, 17, 12, 17, 12, 17, 11 };
	/* 4*<n> 17 callothersubr pop pop pop pop return */
static const card8 subrs8[] = { 139, 17+139, 12, 16, 12, 17, 12, 17, 12, 17, 12, 17, 11 };
	/* 6*<n> 18 callothersubr pop pop pop pop  pop pop return */
static const card8 subrs9[] = { 139, 18+139, 12, 16, 12, 17, 12, 17, 12, 17, 12, 17, 12, 17, 12, 17, 11 };


static const card8 *const subrs[] = { subrs0, subrs1, subrs2, subrs3, subrs4,
	subrs5, subrs6, subrs7, subrs8, subrs9 };
static const int subrslens[] = { sizeof(subrs0), sizeof(subrs1), sizeof(subrs2),
	sizeof(subrs3), sizeof(subrs4), sizeof(subrs5), sizeof(subrs6),
	sizeof(subrs7), sizeof(subrs8), sizeof(subrs9) };

static void initsubrs(UT_array *t1_subrs) {
    int i;
	cs_type1subr subr;

    for ( i=0; i<5; ++i ) {
		subr.size = subrslens[i];
		subr.data = NEW(subrslens[i],card8);
		memcpy(subr.data,subrs[i],subrslens[i]);
		utarray_push_back(t1_subrs,&subr);
    }
}
    
/* Encode a string in adobe's format. choose a different set of initial random*/
/*  bytes every time. (the expected value of leniv is 4. we have some support */
/*  for values bigger than 5 but not as much as for values <=4) */
static void encodestrout(card8 *binary,long *offset,unsigned char *value, int len, int leniv) {
    unsigned short r = 4330;
	long k = *offset;
    unsigned char plain, cypher;
    static unsigned char randombytes[10] = { 0xaa, 0x55, 0x3e, 0x4d, 0x89, 0x98, 'a', '4', 0, 0xff };

    randombytes[0] += 3;
    randombytes[1] += 5;
    randombytes[2] += 7;
    randombytes[3] += 11;
    randombytes[4] += 13;

    while ( leniv>0 ) {
	plain = randombytes[leniv--%10];
	cypher = ( plain ^ (r>>8));
	r = (cypher + r) * c1 + c2;
	binary[k] = cypher;
	k++;
    }
    while ( len-->0 ) {
	plain = *value++;
	cypher = ( plain ^ (r>>8));
	r = (cypher + r) * c1 + c2;
	binary[k] = cypher;
	k++;
    }
}

static void dumpt1str(card8 *binary,long *offset,card8 *data, int len, int leniv) {
	long k;

	if ( len == 0 ) return;
	k = *offset;
	if ( leniv==-1 ) {
		memcpy(binary + k,data,len);
		k += len;
	}
	else {
		encodestrout(binary,offset,data,len,leniv);
		k += len + leniv;
	}
	*offset = k;
}

static void dump_index(card8 *binary,long *offset,int size,int val) {
	long k;

	if ( size == 0 ) return;
	k = *offset;
	if ( size>=4 ) {
		binary[k] = (val>>24)&0xff;
		k++;
	}
	if ( size>=3 ) {
		binary[k] = (val>>16)&0xff;
		k++;
	}
	if ( size>=2 ) {
		binary[k] = (val>>8)&0xff;
		k++;
	}
	if ( size>=1 ) {			/* Yes, size may be 0 for the fd index */
		binary[k] = (val)&0xff;
		k++;
	}
	*offset = k;
}

static long 
gencidbinarydata(cff_font *cffont, const UsedMapElem *used_chars,
				 UT_array *t1_glyphs, UT_array *t1_subrs,
				 struct cidbytes *cidbytes, card8 **bindata) 
{
    int i,j, gid, leniv, subrtot;
    struct fddata *fd;
    card8 *binary;
    long offset, subrs_len, chrs_len, binary_len, bin_offset;
    int len, glyphscnt;
	cs_type1subr *p;

    memset(cidbytes,'\0',sizeof(struct cidbytes));
	cidbytes->cidcnt = (int)cff_dict_get(cffont->topdict, "CIDCount", 0);
	cidbytes->fdcnt = cffont->num_fds;
    cidbytes->fds = NEW(cidbytes->fdcnt,struct fddata);
    subrtot = 0;
	subrs_len = 0;
	chrs_len = 0;
	leniv = 1;
    for ( i=0; i<cidbytes->fdcnt; ++i ) {
		fd = &cidbytes->fds[i];
		fd->subrs = t1_subrs;
		if ( cff_dict_known(cffont->privat[i],"lenIV") )
		    fd->leniv = (int)cff_dict_get(cffont->privat[i],"lenIV",0);
		else
			fd->leniv = 1;
		fd->subrcnt = utarray_len(fd->subrs);
		subrtot += fd->subrcnt;
		for ( j=0; j<fd->subrcnt; j++ ) {
			p = (cs_type1subr *)utarray_eltptr(fd->subrs,j);
			subrs_len += p->size;
		}
		if ( fd->leniv > 0 )
			subrs_len += (fd->subrcnt * fd->leniv);
    }
	glyphscnt = utarray_len(t1_glyphs);
	for ( j=0; j<glyphscnt; j++ ) {
		p = (cs_type1subr *)utarray_eltptr(t1_glyphs,j);
		chrs_len += p->size;
	}
	if ( fd->leniv > 0 )
		chrs_len += (glyphscnt * fd->leniv);

    cidbytes->fdbytes = ( cidbytes->fdcnt==1 )? 0 :
			( cidbytes->fdcnt<256 )? 1 : 2;
    if ( (cidbytes->cidcnt+1)*(cidbytes->fdbytes+3) +		/* size of the CID map region */
	    (subrtot+1) * 3 +									/* size of the Subr map region */
	    subrs_len +											/* size of the subr region */
		chrs_len < 0x1000000 )								/* size of the charstring region */ /* Are all our offsets less than 3 bytes? */
	cidbytes->gdbytes = 3;			/* Adobe's convention is to use 3, so don't bother checking for anything less */
    else
	cidbytes->gdbytes = 4;			/* but I suppose we might need more... */

    offset = (cidbytes->cidcnt+1)*(cidbytes->fdbytes+cidbytes->gdbytes) +
	    (subrtot+1) * cidbytes->gdbytes + subrs_len;
	binary_len = (cidbytes->cidcnt+1)*(cidbytes->fdbytes+cidbytes->gdbytes) +
	    (subrtot+1) * cidbytes->gdbytes + subrs_len + chrs_len;
    binary = NEW(binary_len,card8);
	memset(binary,0,binary_len);
	bin_offset = 0;
	gid = 0;
    for ( i=0; i<cidbytes->cidcnt; ++i ) {
		if (IS_USED_CHAR(used_chars, i)) {
			dump_index(binary,&bin_offset,cidbytes->fdbytes,0);
			dump_index(binary,&bin_offset,cidbytes->gdbytes,offset);
			p = (cs_type1subr *)utarray_eltptr(t1_glyphs,gid);
			offset += p->size;
			if ( fd->leniv > 0 )
				offset += fd->leniv;
			gid++;
		}
		else {
			dump_index(binary,&bin_offset,cidbytes->fdbytes,0);
			dump_index(binary,&bin_offset,cidbytes->gdbytes,offset);
		}
    }
    dump_index(binary,&bin_offset,cidbytes->fdbytes,-1);		/* Adobe says undefined */
    dump_index(binary,&bin_offset,cidbytes->gdbytes,offset);

    offset = (cidbytes->cidcnt+1)*(cidbytes->fdbytes+cidbytes->gdbytes) +
	    (subrtot+1) * cidbytes->gdbytes;
    for ( i=0; i<cidbytes->fdcnt; ++i ) {
		fd = &cidbytes->fds[i];
		fd->subrmapoff = bin_offset;
		fd->sdbytes = cidbytes->gdbytes;
		fd->subrcnt = utarray_len(fd->subrs);
		for ( j=0; j<fd->subrcnt; ++j ) {
			dump_index(binary,&bin_offset,fd->sdbytes,offset);
			p = (cs_type1subr *)utarray_eltptr(fd->subrs,j);
			offset += p->size;
			if ( fd->leniv > 0 )
				offset += fd->leniv;
		}
    }
    dump_index(binary,&bin_offset,cidbytes->gdbytes,offset);
	if ( fd->leniv > 0 )
		offset += fd->leniv;

    for ( i=0; i<cidbytes->fdcnt; ++i ) {
		fd = &cidbytes->fds[i];
		for ( j=0; j<fd->subrcnt; ++j ) {
			p = (cs_type1subr *)utarray_eltptr(fd->subrs,j);
			dumpt1str(binary,&bin_offset,p->data,p->size,fd->leniv);
		}
    }

	for ( j=0; j<glyphscnt; ++j ) {
		p = (cs_type1subr *)utarray_eltptr(t1_glyphs,j);
		dumpt1str(binary,&bin_offset,p->data,p->size,leniv);
	}
	*bindata = binary;

	return( binary_len );
}

long
CIDFont_type0_dofont (const char *PSName, cff_font *cffont, UsedMapElem *used_glyphs,
					  struct cidbytes *cidbytes, card8 **bindata)
{
  cff_index    *idx;
  cff_charsets *charset = NULL;
  cff_fdselect *fdselect = NULL;
  long   charstring_len, max_len;
  long   size, offset = 0;
  card8 *data;
  card16 num_glyphs = 0, gid;
  long cid;
  card16 cs_count, last_cid = 0;
  int    fd, prev_fd;
  unsigned char *CIDToGIDMap = NULL;
  long cid_count;
  UT_array    *t1_charstrings, *t1_subrs;
  cs_type1subr *p;
  double default_width, nominal_width;

  ASSERT(cffont);

  cff_read_charsets(cffont);

  if (cff_dict_known(cffont->topdict, "CIDCount")) {
    cid_count = (long) cff_dict_get(cffont->topdict, "CIDCount", 0);
  } else {
    cid_count = CID_MAX + 1;
  }

  CIDToGIDMap = NEW(2 * cid_count, unsigned char);
  memset(CIDToGIDMap, 0, 2 * cid_count);
  ADD_TO_USED_CHARS(used_glyphs, 0); /* .notdef */
  for (cid = 0; cid <= CID_MAX; cid++) {
    if (IS_USED_CHAR(used_glyphs, cid)) {
      gid = cff_charsets_lookup(cffont, (card16)cid);
      if (cid != 0 && gid == 0) {
//        WARN("Glyph for CID %u missing in font \"%s\".", (CID) cid, PSName);
        REMOVE_FROM_USED_CHARS(used_glyphs, cid);
        continue;
      }
      CIDToGIDMap[2*cid]   = (gid >> 8) & 0xff;
      CIDToGIDMap[2*cid+1] = gid & 0xff;
      last_cid = cid;
      num_glyphs++;
    }
  }

  cff_dict_set(cffont->topdict, "CIDCount", 0, last_cid + 1);

  /*
   * Embed font subset.
   */
  cff_read_fdselect(cffont);
  cff_read_fdarray(cffont);
  cff_read_private(cffont);

  cff_read_subrs(cffont);

  offset = (long) cff_dict_get(cffont->topdict, "CharStrings", 0);
  cff_seek_set(cffont, offset);
  idx = cff_get_index_header(cffont);
  /* offset is now absolute offset ... bad */
  offset = cff_tell(cffont);
  
  if ((cs_count = idx->count) < 2) {
    ERROR("No valid charstring data found.");
  }

  /* New Charsets data */
  charset = NEW(1, cff_charsets);
  charset->format = 0;
  charset->num_entries = 0;
  charset->data.glyphs = NEW(num_glyphs, s_SID);

  /* New FDSelect data */
  fdselect = NEW(1, cff_fdselect);
  fdselect->format = 3;
  fdselect->num_entries = 0;
  fdselect->data.ranges = NEW(num_glyphs, cff_range3);

  /* Convert Charstrings to Type1 */
  utarray_new(t1_charstrings,&subr_icd);
  utarray_new(t1_subrs,&subr_icd);
  initsubrs(t1_subrs);
  cs_start_conversion(5, true,	t1_charstrings, t1_subrs);

  charstring_len = 0;

  prev_fd = -1; gid = 0;
  data = NEW(CS_STR_LEN_MAX, card8);
  for (cid = 0; cid <= last_cid; cid++) {
    unsigned short gid_org;

    if (!IS_USED_CHAR(used_glyphs, cid))
      continue;

    gid_org = (CIDToGIDMap[2*cid] << 8)|(CIDToGIDMap[2*cid+1]);
    if ((size = (idx->offset)[gid_org+1] - (idx->offset)[gid_org])
        > CS_STR_LEN_MAX)
      ERROR("Charstring too long: gid=%u", gid_org);
    cff_seek(cffont, offset + (idx->offset)[gid_org] - 1);
    cff_read_data(data, size, cffont);
    fd = cff_fdselect_lookup(cffont, gid_org);

    if (cffont->privat[fd] && cff_dict_known(cffont->privat[fd], "defaultWidthX")) {
      default_width = (double) cff_dict_get(cffont->privat[fd], "defaultWidthX", 0);
    } else {
      default_width = CFF_DEFAULTWIDTHX_DEFAULT;
    }
    if (cffont->privat[fd] && cff_dict_known(cffont->privat[fd], "nominalWidthX")) {
      nominal_width = (double) cff_dict_get(cffont->privat[fd], "nominalWidthX", 0);
    } else {
      nominal_width = CFF_NOMINALWIDTHX_DEFAULT;
    }

    charstring_len += cs_convert_charstring(data, size, cffont->gsubr, (cffont->subrs)[fd], default_width, nominal_width);

    if (cid > 0 && gid_org > 0) {
      charset->data.glyphs[charset->num_entries] = cid;
      charset->num_entries += 1;
    }
    if (fd != prev_fd) {
      fdselect->data.ranges[fdselect->num_entries].first = gid;
      fdselect->data.ranges[fdselect->num_entries].fd    = fd;
      fdselect->num_entries += 1;
      prev_fd = fd;
    }
    gid++;
  }

  cs_end_conversion();

  if (gid != num_glyphs)
    ERROR("Unexpeced error: ?????");
  RELEASE(data);
  cff_release_index(idx);

  RELEASE(CIDToGIDMap);
  
  /* discard old one, set new data */
  cff_release_charsets(cffont->charsets);
  cffont->charsets = charset;
  cff_release_fdselect(cffont->fdselect);
  cffont->fdselect = fdselect;

  /* no Global subr */
  if (cffont->gsubr)
    cff_release_index(cffont->gsubr);
  cffont->gsubr = cff_new_index(0);

  for (fd = 0; fd < cffont->num_fds; fd++) {
    if (cffont->subrs && cffont->subrs[fd]) {
      cff_release_index(cffont->subrs[fd]);
      cffont->subrs[fd] = NULL;
    }
    if (cffont->privat && (cffont->privat)[fd]) {
      cff_dict_remove((cffont->privat)[fd], "Subrs"); /* no Subrs */
    }
  }

  max_len = gencidbinarydata(cffont, used_glyphs, t1_charstrings, t1_subrs, cidbytes, bindata);

  for(p=(cs_type1subr *)utarray_front(t1_charstrings); p!=NULL; p=(cs_type1subr *)utarray_next(t1_charstrings,p)) {
	if ( p->data )
		free(p->data);
  }
  utarray_free(t1_charstrings);

  for(p=(cs_type1subr *)utarray_front(t1_subrs); p!=NULL; p=(cs_type1subr *)utarray_next(t1_subrs,p)) {
	if ( p->data )
		free(p->data);
	}
  utarray_free(t1_subrs);

  return max_len;
}

long
CIDFont_type0_t1cdofont (const char *PSName, cff_font *cffont, UsedMapElem *used_glyphs,
						 struct cidbytes *cidbytes, card8 **bindata)
{
  cff_index *idx;
  long   charstring_len, max_len;
  long   size, offset = 0;
  card8 *data;
  card16 num_glyphs, gid, last_cid;
  long   i, cid;
  double default_width, nominal_width;
  UT_array    *t1_charstrings, *t1_subrs;
  cs_type1subr *p;

  ASSERT(cffont);

  cff_read_private(cffont);
  cff_read_subrs  (cffont);

  if (cffont->privat[0] && cff_dict_known(cffont->privat[0], "defaultWidthX")) {
    default_width = (double) cff_dict_get(cffont->privat[0], "defaultWidthX", 0);
  } else {
    default_width = CFF_DEFAULTWIDTHX_DEFAULT;
  }
  if (cffont->privat[0] && cff_dict_known(cffont->privat[0], "nominalWidthX")) {
    nominal_width = (double) cff_dict_get(cffont->privat[0], "nominalWidthX", 0);
  } else {
    nominal_width = CFF_NOMINALWIDTHX_DEFAULT;
  }

  num_glyphs = 0; last_cid = 0;
  ADD_TO_USED_CHARS(used_glyphs, 0); /* .notdef */
  for (i = 0; i < (cffont->num_glyphs + BITS_PER_USED_ELEM - 1) / BITS_PER_USED_ELEM; i++) {
    int c, j;

    c = used_glyphs[i];
    for (j = BITS_PER_USED_ELEM - 1; j >= 0; j--) {
      if (c & (1 << j)) {
        num_glyphs++;
        last_cid = (i + 1) * BITS_PER_USED_ELEM - j - 1;
      }
    }
  }

  {
    cff_fdselect *fdselect;

    fdselect = NEW(1, cff_fdselect);
    fdselect->format = 3;
    fdselect->num_entries = 1;
    fdselect->data.ranges = NEW(1, cff_range3);
    fdselect->data.ranges[0].first = 0;
    fdselect->data.ranges[0].fd    = 0;
    cffont->fdselect = fdselect;
  }

  {
    cff_charsets *charset;

    charset  = NEW(1, cff_charsets);
    charset->format = 0;
    charset->num_entries = num_glyphs-1;
    charset->data.glyphs = NEW(num_glyphs-1, s_SID);

    for (gid = 0, cid = 0; cid <= last_cid; cid++) {
      if (IS_USED_CHAR(used_glyphs, cid)) {
        if (gid > 0)
          charset->data.glyphs[gid-1] = cid;
        gid++;
      }
    }
    /* cff_release_charsets(cffont->charsets); */
    cffont->charsets = charset;
  }

  cff_dict_add(cffont->topdict, "CIDCount", 1);
  cff_dict_set(cffont->topdict, "CIDCount", 0, last_cid + 1);

  cffont->fdarray    = NEW(1, cff_dict *);
  cffont->fdarray[0] = cff_new_dict();
  cff_dict_add(cffont->fdarray[0], "FontName", 1);
  cff_dict_set(cffont->fdarray[0], "FontName", 0,
               (double) cff_add_string(cffont, PSName, 1)); /* FIXME: Skip XXXXXX+ */
  cff_dict_add(cffont->fdarray[0], "Private", 2);
  cff_dict_set(cffont->fdarray[0], "Private", 0, 0.0);
  cff_dict_set(cffont->fdarray[0], "Private", 0, 0.0);
  /* FDArray  - index offset, not known yet */
  cff_dict_add(cffont->topdict, "FDArray", 1);
  cff_dict_set(cffont->topdict, "FDArray", 0, 0.0);
  /* FDSelect - offset, not known yet */
  cff_dict_add(cffont->topdict, "FDSelect", 1);
  cff_dict_set(cffont->topdict, "FDSelect", 0, 0.0);

  cff_dict_remove(cffont->topdict, "UniqueID");
  cff_dict_remove(cffont->topdict, "XUID");
  cff_dict_remove(cffont->topdict, "Private");
  cff_dict_remove(cffont->topdict, "Encoding");


  /* */
  offset = (long) cff_dict_get(cffont->topdict, "CharStrings", 0);
  cff_seek_set(cffont, offset);
  idx = cff_get_index_header(cffont);
  /* offset is now absolute offset ... bad */
  offset = cff_tell(cffont);

  if (idx->count < 2)
    ERROR("No valid charstring data found.");

  /* Convert Charstrings to Type1 */
  utarray_new(t1_charstrings,&subr_icd);
  utarray_new(t1_subrs,&subr_icd);
  initsubrs(t1_subrs);
  cs_start_conversion(5, true,	t1_charstrings, t1_subrs);

  charstring_len = 0;

  gid  = 0;
  data = NEW(CS_STR_LEN_MAX, card8);
  for (cid = 0; cid <= last_cid; cid++) {
    if (!IS_USED_CHAR(used_glyphs, cid))
      continue;

    if ((size = (idx->offset)[cid+1] - (idx->offset)[cid])
        > CS_STR_LEN_MAX)
      ERROR("Charstring too long: gid=%u", cid);
    cff_seek(cffont, offset + (idx->offset)[cid] - 1);
    cff_read_data(data, size, cffont);

    charstring_len += cs_convert_charstring(data, size, cffont->gsubr, (cffont->subrs)[0], default_width, nominal_width);
    gid++;
  }

  cs_end_conversion();

  if (gid != num_glyphs)
    ERROR("Unexpeced error: ?????");
  RELEASE(data);
  cff_release_index(idx);
 
  /* no Global subr */
  if (cffont->gsubr)
    cff_release_index(cffont->gsubr);
  cffont->gsubr = cff_new_index(0);

  if (cffont->subrs && cffont->subrs[0]) {
    cff_release_index(cffont->subrs[0]);
    cffont->subrs[0] = NULL;
  }
  if (cffont->privat && (cffont->privat)[0]) {
    cff_dict_remove((cffont->privat)[0], "Subrs"); /* no Subrs */
  }

  cff_add_string(cffont, "Adobe", 1);
  cff_add_string(cffont, "Identity", 1);

  cff_dict_update(cffont->topdict, cffont);
  cff_dict_update(cffont->privat[0], cffont);
  cff_update_string(cffont);

  /* CFF code need to be rewrote... */
  cff_dict_add(cffont->topdict, "ROS", 3);
  cff_dict_set(cffont->topdict, "ROS", 0,
               (double) cff_get_sid(cffont, "Adobe"));
  cff_dict_set(cffont->topdict, "ROS", 1,
               (double) cff_get_sid(cffont, "Identity"));
  cff_dict_set(cffont->topdict, "ROS", 2, 0.0);

  max_len = gencidbinarydata(cffont, used_glyphs, t1_charstrings, t1_subrs, cidbytes, bindata);

  for(p=(cs_type1subr *)utarray_front(t1_charstrings); p!=NULL; p=(cs_type1subr *)utarray_next(t1_charstrings,p)) {
	if ( p->data )
		free(p->data);
  }
  utarray_free(t1_charstrings);

  for(p=(cs_type1subr *)utarray_front(t1_subrs); p!=NULL; p=(cs_type1subr *)utarray_next(t1_subrs,p)) {
	if ( p->data )
		free(p->data);
	}
  utarray_free(t1_subrs);

  return max_len;
}
