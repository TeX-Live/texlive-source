
/*
Copyright (c) 2007 Taco Hoekwater, taco@luatex.org

This file is part of luaTeX.

luaTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

luaTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with luaTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: writetype2.c 1013 2008-02-14 00:09:02Z oneiros $
*/

#include "ptexlib.h"
#include "writettf.h"
#include "writecff.h"

#include "sfnt.h"
#include "tt_glyf.h"

/* forward*/
void make_tt_subset (fd_entry * fd,unsigned char *buffer, integer buflen) ;

unsigned long cidtogid_obj = 0;

/* low-level helpers */

#define test_loc(l)					\
  if ((f->loc+l)>f->buflen) {				\
    fprintf (stderr,"File ended prematurely\n");	\
    uexit(1);						\
  }


BYTE   get_unsigned_byte(sfnt* f) {
  test_loc(1);
  return (BYTE)f->buffer[(f->loc++)];
};

ICHAR get_signed_byte(sfnt* f) {
  test_loc(1);
  return (ICHAR)f->buffer[(f->loc++)];
};

USHORT get_unsigned_pair(sfnt* f){
  USHORT l;
  test_loc(2);
  l = f->buffer[(f->loc++)];
  l = l*0x100 + f->buffer[(f->loc++)];
  return l;
};

SHORT  get_signed_pair(sfnt* f){
  long l;
  test_loc(2);
  l = f->buffer[(f->loc++)];
  if (l>0x80) l -= 0x100;
  l = l*0x100 + f->buffer[(f->loc++)];
  return l;
};

ULONG get_unsigned_quad(sfnt* f){
  ULONG l;
  test_loc(4);
  l = f->buffer[(f->loc++)];
  l = l*0x100 + f->buffer[(f->loc++)];
  l = l*0x100 + f->buffer[(f->loc++)];
  l = l*0x100 + f->buffer[(f->loc++)];
  return l;
};

int do_sfnt_read (unsigned char *dest, int len, sfnt *f) {
  int i;
  test_loc(len);
  for (i=0;i<len;i++) {
    *(dest+i)=f->buffer[f->loc+i];
  }
  f->loc += len;
  return len;
};

pdf_obj *pdf_new_stream(void) {
  pdf_obj *stream = xmalloc(sizeof(pdf_obj)) ;
  stream->length =0; 
  stream->data=NULL;
  return stream;
};

void pdf_add_stream(pdf_obj *stream, unsigned char *buf, long len) {
  int i;
  assert (stream!=NULL);
  if (stream->data==NULL) {
    stream->data = xmalloc(len);
  } else {
    stream->data = xrealloc(stream->data,len+stream->length);
  }
  for (i=0;i<len;i++) {
    *(stream->data+stream->length+i)=*(buf+i);
  }
  stream->length+=len;
};

void pdf_release_obj(pdf_obj *stream) {
  if (stream!=NULL) {
    if (stream->data!=NULL) {
      xfree(stream->data);
    }
    xfree(stream);
  }
};


void
writetype2 (fd_entry * fd) {
  int callback_id;
  int file_opened = 0;
  char *ftemp = NULL;

  glyph_tab = NULL;
  
  fd_cur = fd;                /* fd_cur is global inside writettf.c */
  assert(fd_cur->fm != NULL);
  assert(is_truetype(fd_cur->fm));
  
  if (!is_subsetted(fd_cur->fm)) {
    writettf(fd);
    return;
  }
  
  assert(is_included(fd_cur->fm));

  set_cur_file_name(fd_cur->fm->ff_name);
  ttf_curbyte=0;
  ttf_size=0;
  callback_id=callback_defined(find_opentype_file_callback);
  if (callback_id>0) {
    if(run_callback(callback_id,"S->S",(char *)(nameoffile+1),&ftemp)) {
      if(ftemp!=NULL&&strlen(ftemp)) {
	free(nameoffile);
	namelength = strlen(ftemp);
	nameoffile = xmalloc(namelength+2);
	strcpy((char *)(nameoffile+1),ftemp);
	free(ftemp);
      }
    }
  }
  callback_id=callback_defined(read_opentype_file_callback);
  if (callback_id>0) {
    if(run_callback(callback_id,"S->bSd",(char *)(nameoffile+1),
		    &file_opened, &ttf_buffer,&ttf_size) &&
       file_opened && ttf_size>0) {
    } else {
      pdftex_fail ("cannot open OpenType font file for reading");
    }
  } else {
    if (!otf_open ()) {
      pdftex_fail ("cannot open OpenType font file for reading");
    }
    ttf_read_file();
    ttf_close();
  }
  
  cur_file_name = (char *) nameoffile + 1;
  fd_cur->ff_found = true;

  if (tracefilenames) {
    if (is_subsetted(fd_cur->fm))
      tex_printf ("<%s", cur_file_name);
    else
      tex_printf ("<<%s", cur_file_name);
  }

  /* here is the real work */

  make_tt_subset(fd,ttf_buffer,ttf_size);

  /*xfree (dir_tab);*/
  if (tracefilenames) {
    if (is_subsetted (fd_cur->fm))
      tex_printf (">");
    else 
      tex_printf (">>");
  }
  cur_file_name = NULL;
}

/*
 * PDF viewer applications use following tables (CIDFontType 2)
 *
 *  head, hhea, loca, maxp, glyf, hmtx, fpgm, cvt_, prep
 *
 *                                         - from PDF Ref. v.1.3, 2nd ed.
 *
 * The fpgm, cvt_, and prep tables appears only when TrueType instructions
 * requires them. Those tables must be preserved if they exist.
 * We use must_exist flag to indicate `preserve it if present'
 * and to make sure not to cause an error when it does not exist.
 *
 * post and name table must exist in ordinary TrueType font file,
 * but when a TrueType font is converted to CIDFontType 2 font, those tables
 * are no longer required.
 *
 * The OS/2 table (required for TrueType font for Windows and OS/2) contains
 * liscencing information, but PDF viewers seems not using them.
 *
 * The 'name' table added. See comments in ttf.c.
 */

static struct
{
  const char *name;
  int         must_exist;
} required_table[] = {
  {"OS/2", 0}, {"head", 1}, {"hhea", 1}, {"loca", 1}, {"maxp", 1},
  {"name", 1}, {"glyf", 1}, {"hmtx", 1}, {"fpgm", 0}, {"cvt ", 0},
  {"prep", 0}, {NULL, 0}
};


unsigned long ttc_read_offset (sfnt *sfont, int ttc_idx)
{
  long version;
  unsigned long offset = 0;
  unsigned long num_dirs = 0;

  sfnt_seek_set (sfont, 4); /* skip version tag */

  version = sfnt_get_ulong(sfont);
  num_dirs = sfnt_get_ulong(sfont);
  if (ttc_idx < 0 || ttc_idx > num_dirs - 1) {
    fprintf(stderr,"Invalid TTC index number\n");
    uexit(1);
  }

  sfnt_seek_set (sfont, 12 + ttc_idx * 4);
  offset = sfnt_get_ulong (sfont);

  return offset;
}

extern int ff_get_ttc_index(char *ffname, char*psname);

void make_tt_subset (fd_entry * fd,unsigned char *buffer, integer buflen) {

  long i, cid;
  unsigned int last_cid;
  glw_entry *glyph, *found;
  struct avl_traverser t;
  unsigned char *cidtogidmap ;
  unsigned short num_glyphs, gid;
  struct tt_glyphs *glyphs;
  char *used_chars;
  sfnt *sfont;
  pdf_obj *fontfile;
  int verbose = 0, error = 0;
  
  cidtogidmap = NULL;

  sfont = sfnt_open(buffer, buflen);

  if (sfont->type == SFNT_TYPE_TTC) {
	i = ff_get_ttc_index(fd->fm->ff_name,fd->fm->ps_name);
    error = sfnt_read_table_directory(sfont, ttc_read_offset(sfont, i));
  } else {
	error = sfnt_read_table_directory(sfont, 0);
  }

  if (error  < 0) {
    fprintf(stderr,"Could not parse the ttf directory.\n");
    uexit(1);
  }

  glyphs = tt_build_init();

  last_cid   = 0;
  num_glyphs = 1; /* .notdef */

  glyph = xtalloc(1,glw_entry);

  avl_t_init(&t, fd->gl_tree);
  for (found = (glw_entry *) avl_t_first(&t, fd->gl_tree); 
       found != NULL; 
       found = (glw_entry *) avl_t_next(&t)) {
    if (found->id > last_cid)
      last_cid = found->id;
  }
  
#ifndef NO_GHOSTSCRIPT_BUG
  cidtogidmap = NULL;
#else
  cidtogidmap = xmalloc(((last_cid + 1) * 2) * sizeof(unsigned char));
  memset(cidtogidmap, 0, (last_cid + 1) * 2);
#endif

  /* fill used_chars */
  used_chars = xmalloc((last_cid + 1) * sizeof(char));
  memset(used_chars, 0, (last_cid + 1));		       
  avl_t_init(&t, fd->gl_tree);
  for (found = (glw_entry *) avl_t_first(&t, fd->gl_tree); 
       found != NULL; 
       found = (glw_entry *) avl_t_next(&t)) {
    used_chars[found->id] = 1;
  }

  /*
   * Map CIDs to GIDs.
   */

  num_glyphs = 1; /* .notdef */
  for (cid = 1; cid <= last_cid; cid++) {
    if (used_chars[cid]==0)
      continue;
    gid = cid;


#ifndef NO_GHOSTSCRIPT_BUG
      gid = tt_add_glyph(glyphs, gid, cid);
#else
      gid = tt_add_glyph(glyphs, gid, num_glyphs);
      cidtogidmap[2*cid  ] = gid >> 8;
      cidtogidmap[2*cid+1] = gid & 0xff;
#endif /* !NO_GHOSTSCRIPT_BUG */
    
    num_glyphs++;
  }

  if (num_glyphs==1) {
    fprintf(stderr,"No glyphs in subset?.\n");
    uexit(1);
  }

  if (tt_build_tables(sfont, glyphs) < 0) {
    fprintf(stderr,"Could not parse the ttf buffer.\n");
    uexit(1);
  }
  
  if (verbose > 1) {
    fprintf(stdout,"[%u glyphs (Max CID: %u)]", glyphs->num_glyphs, last_cid);
  }

  tt_build_finish(glyphs);

  /* Create font file */

  for (i = 0; required_table[i].name; i++) {
    if (sfnt_require_table(sfont,
			   required_table[i].name,
			   required_table[i].must_exist) < 0) {
      fprintf(stderr,"Some required TrueType table does not exist.");
      uexit(1);
    }
  }

  fontfile = sfnt_create_FontFile_stream(sfont);

  if (verbose > 1) {
    fprintf(stdout,"[%ld bytes]", fontfile->length);
  }

  /* squeeze in the cidgidmap */
  if (cidtogidmap!=NULL) {
    cidtogid_obj = pdf_new_objnum();
    pdf_begin_dict(cidtogid_obj, 0);
    pdf_printf("/Length %i\n",((last_cid + 1) * 2));
    pdf_end_dict();
    pdf_printf("stream\n");
    pdfroom ((last_cid + 1) * 2);
    for (i = 0; i<((last_cid + 1) * 2); i++ ) {
      pdf_buf[pdf_ptr++] = cidtogidmap[i];
    }
    pdf_printf("\nendstream\n");
  }

  /* the tff subset */
  for (i = 0; i<fontfile->length; i++ )
    fb_putchar (fontfile->data[i]);

  
  /* other stuff that needs fixing: */

  /*
   * DW, W, DW2, and W2
   */
  /*
   if (opt_flags & CIDFONT_FORCE_FIXEDPITCH) {
     pdf_add_dict(font->fontdict,
 		 pdf_new_name("DW"), pdf_new_number(1000.0));
   } else {
     add_TTCIDHMetrics(font->fontdict, glyphs, used_chars, cidtogidmap, last_cid);
      if (v_used_chars)
       add_TTCIDVMetrics(font->fontdict, glyphs, used_chars, cidtogidmap, last_cid);
   }
  */

  /*
   * CIDSet
   */
  /*
  {
    pdf_obj *cidset;

    cidset = pdf_new_stream(STREAM_COMPRESS);
    pdf_add_stream(cidset, used_chars, last_cid/8 + 1);
    pdf_add_dict(font->descriptor,
		 pdf_new_name("CIDSet"),
		 pdf_ref_obj(cidset));
    pdf_release_obj(cidset);
  }
  */

  return;
}
