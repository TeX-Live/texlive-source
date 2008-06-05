/*  $Header$
 
    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "system.h"
#include "config.h"
#include "mem.h"
#include "error.h"
#include "mfileio.h"
#include "numbers.h"
#include "dvi.h"
#include "pdflimits.h"
#include "pdfobj.h"
#include "pdfdev.h"
#include "pdfdoc.h"
#include "pdfspecial.h"

#ifdef HAVE_LIBPNG
#include "thumbnail.h"
#endif

static pdf_obj *catalog = NULL;
static pdf_obj *docinfo = NULL;
static pdf_obj *page_tree = NULL, *page_tree_ref = NULL;

int outline_depth=0;
static struct 
{
  int kid_count;
  pdf_obj *entry;
} outline[MAX_OUTLINE_DEPTH];


static pdf_obj *current_page_resources = NULL;
static pdf_obj *this_page_contents = NULL;
static pdf_obj *glob_page_bop, *glob_page_eop;
static pdf_obj *coord_xform_stream = NULL, *coord_xform_ref = NULL;
static pdf_obj *this_page_bop = NULL;
static pdf_obj *this_page_beads = NULL;
static pdf_obj *this_page_annots = NULL;
static pdf_obj *this_page_xobjects = NULL, *this_page_fonts = NULL;
static pdf_obj *tmp1;

static unsigned long page_count = 0;
static struct pages {
  pdf_obj *page_dict;
  pdf_obj *page_ref;
} *pages = NULL;
static unsigned long max_pages = 0;

static void start_page_tree (void);
static void create_catalog (void);
static void start_current_page_resources (void);
static void finish_page_tree(void);
static void start_name_tree(void);
static void finish_dests_tree(void);
static void finish_pending_xobjects(void);
static void start_articles(void);

static unsigned char verbose = 0, debug=0;

void pdf_doc_set_verbose(void)
{
  if (verbose < 255) 
    verbose += 1;
}

void pdf_doc_set_debug(void)
{
  debug = 1;
}

static void resize_pages (unsigned long newsize)
{
  unsigned long i;
  if (newsize > max_pages) {
    pages = RENEW (pages, newsize, struct pages);
    for (i=max_pages; i<newsize; i++) {
      pages[i].page_dict = NULL;
      pages[i].page_ref = NULL;
    }
    max_pages = newsize;
  }
}

static pdf_obj *type_name, *page_name, *pages_name, *contents_name, *annots_name,
  *resources_name, *bead_name, *count_name, *kids_name, *parent_name,
  *mediabox_name, *limits_name, *thumb_name;

static void make_short_cuts(void) 
{
#ifdef MEM_DEBUG
MEM_START
#endif
  /* Define some shorthand for names that will conserve memory (and time)
     (similar to the latex \@ne trick */
  type_name = pdf_new_name("Type");
  page_name = pdf_new_name("Page");
  pages_name = pdf_new_name("Pages");
  count_name = pdf_new_name("Count");
  kids_name = pdf_new_name("Kids");
  parent_name = pdf_new_name("Parent");
  contents_name = pdf_new_name("Contents");
  annots_name = pdf_new_name("Annots");
  resources_name = pdf_new_name ("Resources");
  bead_name = pdf_new_name ("B");
  mediabox_name = pdf_new_name ("MediaBox");
  limits_name = pdf_new_name ("Limits");
  thumb_name = pdf_new_name ("Thumb");
#ifdef MEM_DEBUG
MEM_END
#endif
}
static void release_short_cuts(void)
{
  /* Release those shorthand name we created */
  pdf_release_obj (type_name);
  pdf_release_obj (page_name);
  pdf_release_obj (pages_name);
  pdf_release_obj (count_name);
  pdf_release_obj (kids_name);
  pdf_release_obj (parent_name);
  pdf_release_obj (contents_name);
  pdf_release_obj (annots_name);
  pdf_release_obj (resources_name);
  pdf_release_obj (bead_name);
  pdf_release_obj (mediabox_name);
  pdf_release_obj (limits_name);
  pdf_release_obj (thumb_name);
}

static void start_page_tree (void)
{
  if (debug) {
    fprintf (stderr, "(start_page_tree)");
  }
  /* Create empty page tree */
  page_tree = pdf_new_dict();
  page_tree_ref = pdf_ref_obj (page_tree);
  /* Both page_tree and page_tree_ref are kept open until the
     document is closed.  This allows the user to write to page_tree
     if he so choses. */
  /* Link /Pages into the catalog
     and poing it to indirect reference since we
     haven't built the tree yet */
  glob_page_bop = pdf_new_stream(0);
  glob_page_eop = pdf_new_stream(0);
  coord_xform_stream = pdf_new_stream(0);
  coord_xform_ref = pdf_ref_obj (coord_xform_stream);
  return;
}

void pdf_doc_bop (char *string, unsigned length)
{
  if (length > 0)
    pdf_add_stream (glob_page_bop, string, length);
}

void pdf_doc_this_bop (char *string, unsigned length)
{
  if (this_page_bop == NULL)
    this_page_bop = pdf_new_stream (STREAM_COMPRESS);
  if (length > 0)
    pdf_add_stream (this_page_bop, string, length);
}

void pdf_doc_set_origin (double x, double y)
{
  int len;
  static char first = 1;
  if (first) {
    sprintf (work_buffer, "%g 0 0 %g %g %g cm\n",
	     dvi_tell_mag()*pdf_dev_scale(), dvi_tell_mag()*pdf_dev_scale(),
	     x, y);
    len = strlen (work_buffer);
    pdf_add_stream (coord_xform_stream, work_buffer, len);
    first = 0;
  }
}


void pdf_doc_eop (char *string, unsigned length)
{
  if (length > 0)
    pdf_add_stream (glob_page_eop, string, length);
}

static void start_outline_tree (void)
{
  if (debug) {
    fprintf (stderr, "(start_outline_tree)");
  }
  /* Create empty outline tree */
  outline[outline_depth].entry = pdf_new_dict();
  outline[outline_depth].kid_count = 0;
  return;
}

static pdf_obj *names_dict;

static void start_name_tree (void)
{
  if (debug) {
    fprintf (stderr, "(start_name_tree)");
  }
  names_dict = pdf_new_dict ();
}

static char *asn_date (void)
{
  long tz_offset;
  static char date_string[24];
  time_t current_time;
  struct tm *bd_time;
  if (debug) {
    fprintf (stderr, "(asn_date)");
  }
  time(&current_time);
  bd_time = localtime(&current_time);

#ifdef HAVE_TM_GMTOFF  /* Preferred way to get time zone offset */
  tz_offset = bd_time->tm_gmtoff;
#else
#ifdef HAVE_TIMEZONE   /* Plan B --- use external variable 'timezone'
                       /* (may not provide correct offset for daylight savings time) */
  tz_offset = - timezone;
#else                  /* Last resort --- without more information, set offset to zero */
  tz_offset = 0l;
#endif /* HAVE_TIMEZONE */
#endif /* HAVE_TM_GMTOFF */

  if (tz_offset == 0l) {
    sprintf (date_string, "D:%04d%02d%02d%02d%02d%02dZ00'00'",
	     bd_time -> tm_year+1900, bd_time -> tm_mon+1, bd_time -> tm_mday,
	     bd_time -> tm_hour, bd_time -> tm_min, bd_time -> tm_sec);
  } else {
    sprintf (date_string, "D:%04d%02d%02d%02d%02d%02d%c%02ld'%02ld'",
             bd_time -> tm_year+1900, bd_time -> tm_mon+1, bd_time -> tm_mday,
             bd_time -> tm_hour, bd_time -> tm_min, bd_time -> tm_sec,
	     (tz_offset>0)? '+':'-', labs(tz_offset)/3600, (labs(tz_offset)/60)%60);
  }
  return date_string;
}

#define BANNER "dvipdfm %s, Copyright \251 1998, by Mark A. Wicks"
static void create_docinfo (void)
{
  /* Create an empty Info entry and make it
     be the root object */
  if (debug) {
    fprintf (stderr, "(create_docinfo)");
  }
  docinfo = pdf_new_dict ();
  pdf_set_info (docinfo);
  return;
}

static void finish_docinfo(void)
{
  char *time_string, *banner;
  banner = NEW (strlen(BANNER)+20,char);
  sprintf(banner, BANNER, VERSION);
  pdf_add_dict (docinfo, 
		pdf_new_name ("Producer"),
		pdf_new_string (banner, strlen (banner)));
  RELEASE (banner);
  time_string = asn_date();
  pdf_add_dict (docinfo, 
		pdf_new_name ("CreationDate"),
		pdf_new_string (time_string, strlen (time_string)));
  pdf_release_obj (docinfo);
  return;
}

void pdf_doc_merge_with_docinfo (pdf_obj *dictionary)
{
  pdf_merge_dict (docinfo, dictionary);
}

void pdf_doc_merge_with_catalog (pdf_obj *dictionary)
{
  pdf_merge_dict (catalog, dictionary);
}

static void create_catalog (void)
{
  if (debug) {
    fprintf (stderr, "(create_catalog)");
  }
  catalog = pdf_new_dict ();
  pdf_set_root (catalog);
  /* Create /Type attribute */
  pdf_add_dict (catalog,
		pdf_link_obj (type_name),
		pdf_new_name("Catalog"));
 /* Create only those parts of the page tree required for the catalog.
    That way, the rest of the page tree can be finished at any time */
  start_page_tree(); 
  /* Likewise for outline tree */
  start_outline_tree ();
  start_name_tree();
  start_articles();
  return;
}

static void start_current_page_resources (void)
{
  /* work on resources to put in Pages */
  if (debug) {
    fprintf (stderr, "(start_current_page_resources)");
  }
  current_page_resources = pdf_new_dict ();
  tmp1 = pdf_new_array ();
  pdf_add_array (tmp1, pdf_new_name ("PDF"));
  pdf_add_array (tmp1, pdf_new_name ("Text"));
  pdf_add_array (tmp1, pdf_new_name ("ImageC"));
  pdf_add_dict (current_page_resources,
		pdf_new_name ("ProcSet"),
		tmp1);
  return;
}

void pdf_doc_add_to_page_fonts (const char *name, pdf_obj
				   *resource)
{
#ifdef MEM_DEBUG
  MEM_START;
#endif

  if (debug) {
    fprintf (stderr, "(pdf_doc_add_to_page_fonts)");
  }
  if (this_page_fonts == NULL)
    this_page_fonts = pdf_new_dict();
  pdf_add_dict (this_page_fonts,
		pdf_new_name (name), resource);
#ifdef MEM_DEBUG
MEM_END
#endif
}

void pdf_doc_add_to_page_xobjects (const char *name, pdf_obj
				   *resource)
{
  if (debug) {
    fprintf (stderr, "(pdf_doc_add_to_page_xojects)");
  }
  if (this_page_xobjects == NULL)
    this_page_xobjects = pdf_new_dict ();
  pdf_add_dict (this_page_xobjects,
		pdf_new_name (name), 
		resource);
}


void pdf_doc_add_to_page_resources (const char *name, pdf_obj *resource)
{
  if (debug) {
    fprintf (stderr, "(pdf_doc_add_to_page_resources)");
  }
  pdf_add_dict (current_page_resources,
		pdf_new_name (name), 
		resource);
}

void pdf_doc_add_to_page_annots (pdf_obj *annot)
{
  if (debug) {
    fprintf (stderr, "(pdf_doc_add_to_page_annots)");
  }
  if (this_page_annots == NULL)
    this_page_annots = pdf_new_array ();
  pdf_add_array (this_page_annots,
		 annot);
}

static pdf_obj *page_subtree (struct pages *pages, unsigned long npages,
		       pdf_obj *parent_ref)
{
#define PAGE_CLUSTER 4
  pdf_obj *self, *self_ref, *kid_array;
  self = pdf_new_dict();
  /* This is a slight kludge which allow the subtree
     dictionary generated by this routine to be merged with the
     real page_tree dictionary, while keeping the indirect 
     object references right */
  if (parent_ref == NULL)
    self_ref = pdf_ref_obj (page_tree);
  else
    self_ref = pdf_ref_obj (self);
  pdf_add_dict (self, pdf_link_obj (type_name),
		pdf_link_obj (pages_name));
  pdf_add_dict (self, pdf_link_obj (count_name),
		pdf_new_number((double) npages));
  kid_array = pdf_new_array();
  pdf_add_dict (self, pdf_link_obj (kids_name), 
		kid_array);
  if (parent_ref != NULL) {
    pdf_add_dict (self, pdf_link_obj(parent_name),
		  parent_ref);
  }
  if (npages > 0 && npages <= PAGE_CLUSTER) {
    int i;
    for (i=0; i<npages; i++) {
      pdf_add_array (kid_array, pdf_link_obj(pages[i].page_ref));
      pdf_add_dict (pages[i].page_dict, pdf_link_obj (parent_name),
		    pdf_link_obj(self_ref));
      pdf_release_obj (pages[i].page_dict);
      pdf_release_obj (pages[i].page_ref);
      pages[i].page_dict = NULL;
      pages[i].page_ref = NULL;
    }
  } else if (npages > 0) {
    int i;
    for (i=0; i<PAGE_CLUSTER; i++) {
      pdf_obj *subtree;
      unsigned long start, end;
      start = (i*npages)/PAGE_CLUSTER;
      end = ((i+1)*npages)/PAGE_CLUSTER;
      if (end-start>1) {
	subtree = page_subtree (pages+start, end-start, pdf_link_obj(self_ref));
	pdf_add_array (kid_array, pdf_ref_obj (subtree));
	pdf_release_obj (subtree);
      }
      else {
	pdf_add_array (kid_array, pdf_link_obj(pages[start].page_ref));
	pdf_add_dict (pages[start].page_dict, pdf_link_obj(parent_name),
		      pdf_link_obj (self_ref));
	pdf_release_obj (pages[start].page_dict);
	pdf_release_obj (pages[start].page_ref);
	pages[start].page_dict = NULL;
	pages[start].page_ref = NULL;
      }
    }
  }
  pdf_release_obj (self_ref);
  return self;
}

static void finish_page_tree(void)
{
  pdf_obj *subtree;
  if (debug) {
    fprintf (stderr, "(finish_page_tree)");
  }
  
  subtree = page_subtree (pages, page_count, NULL);
  pdf_merge_dict (page_tree, subtree);
  pdf_release_obj (subtree);
  /* Generate media box at root of page tree and let the
     other pages inherit it */
  {
    tmp1 = pdf_new_array ();
    pdf_add_array (tmp1, pdf_new_number (0));
    pdf_add_array (tmp1, pdf_new_number (0));
    pdf_add_array (tmp1, pdf_new_number (ROUND(dev_page_width(),1.0)));
    pdf_add_array (tmp1, pdf_new_number (ROUND(dev_page_height(),1.0)));
    pdf_add_dict (page_tree, pdf_link_obj (mediabox_name), tmp1);
  }
  pdf_release_obj (page_tree);
  pdf_add_dict (catalog,
		pdf_link_obj (pages_name),
		pdf_link_obj (page_tree_ref));
  pdf_release_obj (page_tree_ref);
  RELEASE (pages);
  pdf_add_stream (glob_page_bop, "\n", 1);
  pdf_release_obj (glob_page_bop);
  pdf_add_stream (glob_page_eop, "\n", 1);
  pdf_release_obj (glob_page_eop);
  pdf_release_obj (coord_xform_stream);
  pdf_release_obj (coord_xform_ref);
  return;
}

void pdf_doc_change_outline_depth(int new_depth)
{
  int i;
  if (debug) {
    fprintf (stderr, "(change_outline_depth)");
  }
  if (outline_depth >= MAX_OUTLINE_DEPTH -1)
    ERROR ("Outline is too deep.");
  if (new_depth == outline_depth)
    /* Nothing to do */
    return;
  if (new_depth > outline_depth+1)
    ERROR ("Can't increase outline depth by more than one at a time\n");
  if (outline[outline_depth].entry == NULL)
    ERROR ("change_outline_depth: Fix me, I'm broke. This shouldn't happen!");
  /* Terminate all entries above this depth */
  for (i=outline_depth-1; i>=new_depth; i--) {
    pdf_add_dict (outline[i].entry,
		  pdf_new_name ("Last"),
		  pdf_ref_obj (outline[i+1].entry));
    if (i > 0) 
      tmp1 = pdf_new_number (-outline[i].kid_count);
    else
      tmp1 = pdf_new_number (outline[i].kid_count);

    pdf_add_dict (outline[i].entry,
		  pdf_link_obj (count_name),
		  tmp1);
  }
  /* Flush out all entries above this depth */
  for (i=new_depth+1; i<=outline_depth; i++) {
    pdf_release_obj (outline[i].entry);
    outline[i].entry = NULL;
    outline[i].kid_count = 0;
  }
  outline_depth = new_depth;
}

static void finish_outline(void)
{
  if (debug)
    fprintf (stderr, "(finish_outline)");
  /* Link it into the catalog */
  /* Point /Outline attribute to indirect reference */
  pdf_doc_change_outline_depth (0);
  pdf_add_dict (catalog,
		pdf_new_name ("Outlines"),
		pdf_ref_obj(outline[outline_depth].entry));
  pdf_release_obj (outline[0].entry);
  outline[0].entry = NULL;
}


void pdf_doc_add_outline (pdf_obj *dict)
{
  pdf_obj *new_entry;
  if (outline_depth < 1)
    ERROR ("Can't add to outline at depth < 1");
  new_entry = pdf_new_dict ();
  pdf_merge_dict (new_entry, dict);
  /* Caller doesn't know we don't actually use the dictionary,
     so he *gave* dict to us.  We have to free it */
  pdf_release_obj (dict);
  /* Tell it where its parent is */
  pdf_add_dict (new_entry,
		pdf_link_obj (parent_name),
		pdf_ref_obj (outline[outline_depth-1].entry));
  /* Give mom and dad the good news */
  outline[outline_depth-1].kid_count += 1;

  /* Is this the first entry at this depth? */
  if (outline[outline_depth].entry == NULL) {
    /* Is so, tell the parent we are first born */
    pdf_add_dict (outline[outline_depth-1].entry,
		  pdf_new_name ("First"),
		  pdf_ref_obj (new_entry));
  }
  else {
    /* Point us back to sister */
    pdf_add_dict (new_entry,
		  pdf_new_name ("Prev"),
		  pdf_ref_obj (outline[outline_depth].entry));
    /* Point our elder sister toward us */
    pdf_add_dict (outline[outline_depth].entry,
		  pdf_new_name ("Next"),
		  pdf_ref_obj (new_entry));
    /* Bye-Bye sis */
    pdf_release_obj (outline[outline_depth].entry);
  }
  outline[outline_depth].entry = new_entry;
  /* Just born, so don't have any kids */
  outline[outline_depth].kid_count = 0;
}

struct dests 
{
  char *name;
  unsigned length;
  pdf_obj *array;
};
typedef struct dests dest_entry;
static dest_entry *dests = NULL;
unsigned long max_dests = 0;

static int CDECL cmp_dest (const void *d1, const void *d2)
{
  unsigned length;
  int tmp;
  length = MIN (((dest_entry *) d1) -> length, ((dest_entry *) d2) ->
		length);
  if ((tmp = memcmp (((dest_entry *) d1) -> name, ((dest_entry *) d2)
		      -> name, length)) != 0)
    return tmp;
  if (((dest_entry *) d1) -> length == ((dest_entry *) d2) -> length)
    return 0;
  return (((dest_entry *) d1) -> length < ((dest_entry *) d2) -> length ? -1 : 1 );
}

static pdf_obj *name_subtree (dest_entry *dests, unsigned long ndests)
{
#define CLUSTER 4
  pdf_obj *result, *name_array, *limit_array, *kid_array;
  result = pdf_new_dict();
  limit_array = pdf_new_array();
  pdf_add_dict (result, pdf_link_obj(limits_name), limit_array);
  pdf_add_array (limit_array, pdf_new_string(dests[0].name,
					     dests[0].length)); 
  pdf_add_array (limit_array, pdf_new_string(dests[ndests-1].name,
					     dests[ndests-1].length));
  if (ndests > 0 && ndests <= CLUSTER) {
    int i;
    name_array = pdf_new_array();
    pdf_add_dict (result, pdf_new_name ("Names"),
		  name_array);
    for (i=0; i<ndests; i++) {
      pdf_add_array (name_array, pdf_new_string (dests[i].name,
						 dests[i].length));
      RELEASE (dests[i].name);
      pdf_add_array (name_array, dests[i].array);
    }
  } else if (ndests > 0) {
    int i;
    kid_array = pdf_new_array();
    pdf_add_dict (result, pdf_link_obj (kids_name), kid_array);
    for (i=0; i<CLUSTER; i++) {
      pdf_obj *subtree;
      unsigned long start, end;
      start = (i*ndests)/CLUSTER;
      end = ((i+1)*ndests)/CLUSTER;
      subtree = name_subtree (dests+start, end-start);
      pdf_add_array (kid_array, pdf_ref_obj (subtree));
      pdf_release_obj (subtree);
    }
  }
  return result;
}

static unsigned long number_dests = 0;

static void finish_dests_tree (void)
{
  pdf_obj *kid;
  if (number_dests > 0) {
    /* Sort before writing any /Dests entries */
    qsort(dests, number_dests, sizeof(dests[0]), cmp_dest);
    kid = name_subtree (dests, number_dests);
    /* Each entry in dests has been assigned to another object, so
       we can free the entire array without freeing the entries. */
    RELEASE (dests);
    pdf_add_dict (names_dict,
		  pdf_new_name ("Dests"),
		  pdf_ref_obj (kid));
    pdf_release_obj (kid);
  }
}

void pdf_doc_add_dest (char *name, unsigned length, pdf_obj *array_ref)
{
#ifdef MEM_DEBUG
MEM_START
#endif
  if (number_dests >= max_dests) {
    max_dests += DESTS_ALLOC_SIZE;
    dests = RENEW (dests, max_dests, dest_entry);
  }
  dests[number_dests].name = NEW (length, char);
  memcpy (dests[number_dests].name, name, length);
  dests[number_dests].length = length;
  dests[number_dests].array = array_ref;
  number_dests++;
#ifdef MEM_DEBUG
MEM_END
#endif
  return;
}

struct articles
{
  char *name;
  pdf_obj *info;
  pdf_obj *first;
  pdf_obj *last;
  pdf_obj *this;
};

typedef struct articles article_entry;
static article_entry articles[MAX_ARTICLES];
static unsigned long number_articles = 0;

static pdf_obj *articles_array;
static void start_articles (void)
{
  articles_array = pdf_new_array();
}

void pdf_doc_start_article (char *name, pdf_obj *info)
{
  if (number_articles >= MAX_ARTICLES) {
    ERROR ("pdf_doc_add_article:  Too many articles\n");
  }
  if (name == NULL || strlen (name) == 0)
    ERROR ("pdf_doc_start_article called null name");
  articles[number_articles].name = NEW (strlen(name)+1, char);
  strcpy (articles[number_articles].name, name);
  articles[number_articles].info = info;
  articles[number_articles].first = NULL;
  articles[number_articles].last = NULL;
  /* Start dictionary for this article even though we can't finish it
     until we get the first bead */
  articles[number_articles].this = pdf_new_dict();
  number_articles++;
  return;
}

void pdf_doc_add_bead (char *article_name, pdf_obj *partial_dict)
{
  /* partial_dict should have P (Page) and R (Rect) already filled in */
  /* See if the specified article exists */
  int i;
  for (i=0; i<number_articles; i++) {
    if (!strcmp (articles[i].name, article_name))
      break;
  }
  if (i == number_articles) {
    fprintf (stderr, "Bead specified thread that doesn't exist\n");
    return;
  }
  /* Is this the first bead? */
  if (articles[i].last == NULL) {
    articles[i].first = pdf_link_obj (partial_dict);
    /* Add pointer to its first object */ 
    pdf_add_dict (articles[i].this,
		  pdf_new_name ("F"),
		  pdf_ref_obj (articles[i].first));
    /* Next add pointer to its Info dictionary */
    pdf_add_dict (articles[i].this,
		  pdf_new_name ("I"),
		  pdf_ref_obj (articles[i].info));
    /* Point first bead to parent article */
    pdf_add_dict (partial_dict,
		  pdf_new_name ("T"),
		  pdf_ref_obj (articles[i].this));
    /* Ship it out and forget it */
    pdf_add_array (articles_array, pdf_ref_obj (articles[i].this));
    pdf_release_obj (articles[i].this);
    articles[i].this = NULL;
  } else {
    /* Link it in... */
    /* Point last object to this one */
    pdf_add_dict (articles[i].last,
		  pdf_new_name ("N"),
		  pdf_ref_obj (partial_dict));
    /* Point this one to last */
    pdf_add_dict (partial_dict,
		  pdf_new_name ("V"),
		  pdf_ref_obj (articles[i].last));
    pdf_release_obj (articles[i].last);
  }
  articles[i].last = partial_dict;
  if (this_page_beads == NULL)
    this_page_beads = pdf_new_array();
  pdf_add_array (this_page_beads,
		 pdf_ref_obj (partial_dict));
}

void finish_articles(void)
{
  int i;
  pdf_add_dict (catalog,
		pdf_new_name ("Threads"),
		pdf_ref_obj (articles_array));
  pdf_release_obj (articles_array);
  for (i=0; i<number_articles; i++) {
    if (articles[i].last == NULL) {
      fprintf (stderr, "Article started, but no beads\n");
      break;
    }
    /* Close the loop */
    pdf_add_dict (articles[i].last,
		  pdf_new_name ("N"),
		  pdf_ref_obj (articles[i].first));
    pdf_add_dict (articles[i].first,
		  pdf_new_name ("V"),
		  pdf_ref_obj (articles[i].last));
    pdf_release_obj (articles[i].first);
    pdf_release_obj (articles[i].last);
    pdf_release_obj (articles[i].info);
    RELEASE (articles[i].name);
  }
}

#ifdef HAVE_LIBPNG
static char thumbnail_opt = 0;
static char *thumb_basename = NULL;

void pdf_doc_enable_thumbnails(void)
{
  thumbnail_opt = 1;
}

#endif

void pdf_doc_finish_page ()
{
#ifdef MEM_DEBUG
MEM_START
#endif  
  if (debug) {
    fprintf (stderr, "(pdf_doc_finish_page)");
  }
  finish_pending_xobjects();
  /* Flush this page */
  /* Page_count is the index of the current page, starting at 1 */
  tmp1 = pdf_new_array ();
  pdf_add_array (tmp1, pdf_ref_obj (glob_page_bop));
  if (this_page_bop) {
    pdf_add_array (tmp1, pdf_ref_obj (this_page_bop));
  }
  pdf_add_array (tmp1, pdf_link_obj (coord_xform_ref));
  pdf_add_array (tmp1, pdf_ref_obj (this_page_contents));
  pdf_add_array (tmp1, pdf_ref_obj (glob_page_eop));
  pdf_add_dict (pages[page_count].page_dict,
		pdf_link_obj(contents_name), tmp1);
  /* We keep .page_dict open because we don't know the parent yet */
  if (this_page_bop != NULL) {
    pdf_add_stream (this_page_bop, "\n", 1);
    pdf_release_obj (this_page_bop);
    this_page_bop = NULL;
  }
  if (this_page_contents != NULL) {
    pdf_add_stream (this_page_contents, "\n", 1);
    pdf_release_obj (this_page_contents);
    this_page_contents = NULL;
  }
  if (this_page_annots != NULL) {
    pdf_add_dict (pages[page_count].page_dict,
		  pdf_link_obj(annots_name),
		  pdf_ref_obj (this_page_annots));
    pdf_release_obj (this_page_annots);
    this_page_annots = NULL;
  }
  if (this_page_beads != NULL) {
    pdf_add_dict (pages[page_count].page_dict,
		  pdf_link_obj (bead_name),
		  pdf_ref_obj (this_page_beads));
    pdf_release_obj (this_page_beads);
    this_page_beads = NULL;
  }
  if (this_page_fonts != NULL) {
    pdf_add_dict (current_page_resources, 
		  pdf_new_name ("Font"),
		  pdf_ref_obj (this_page_fonts));
    pdf_release_obj (this_page_fonts);
    this_page_fonts = NULL;
  }
  if (this_page_xobjects != NULL) {
    pdf_add_dict (current_page_resources,
		  pdf_new_name ("XObject"),
		  pdf_ref_obj (this_page_xobjects));
    pdf_release_obj (this_page_xobjects);
    this_page_xobjects = NULL;
  }
  if (current_page_resources != NULL) {
    pdf_release_obj (current_page_resources);
    current_page_resources = NULL;
  }
#ifdef HAVE_LIBPNG
  if (thumbnail_opt) {
    char *thumb_filename;
    pdf_obj *thumbnail;
    thumb_filename = NEW (strlen(thumb_basename)+7, char);
    sprintf (thumb_filename, "%s.%ld", thumb_basename,
	     page_count%99999+1L);
    thumbnail = do_thumbnail (thumb_filename);
    RELEASE (thumb_filename);
    if (thumbnail) 
      pdf_add_dict (pages[page_count].page_dict,
		    pdf_link_obj (thumb_name),
		    thumbnail);
  }
#endif
  page_count += 1;
#ifdef MEM_DEBUG
  MEM_END;
#endif  
}

pdf_obj *pdf_doc_current_page_resources (void)
{
  return current_page_resources;
}


static int highest_page_ref = 0;
pdf_obj *pdf_doc_ref_page (unsigned long page_no)
{
  if (debug)
    fprintf (stderr, "(doc_ref_page:page_no=%ld)", page_no);
  if (page_no >= max_pages) {
    resize_pages (page_no+PAGES_ALLOC_SIZE);
  }
  /* Has this page been referenced yet? */ 
  if (pages[page_no-1].page_dict == NULL) {
    /* If not, create it */
    pages[page_no-1].page_dict = pdf_new_dict ();
    /* and reference it */
    pages[page_no-1].page_ref = pdf_ref_obj (pages[page_no-1].page_dict);
  }
  if (page_no > highest_page_ref)
    highest_page_ref = page_no;
  return pdf_link_obj (pages[page_no-1].page_ref);
}

pdf_obj *pdf_doc_names (void)
{
  return names_dict;
}

pdf_obj *pdf_doc_page_tree (void)
{
  return page_tree;
}

pdf_obj *pdf_doc_catalog (void)
{
  return catalog;
}

pdf_obj *pdf_doc_this_page (void)
{
  return pages[page_count].page_dict;
}

pdf_obj *pdf_doc_this_page_ref (void)
{
  return pdf_doc_ref_page(page_count+1);
}

pdf_obj *pdf_doc_prev_page_ref (void)
{
  if (page_count < 1) {
    ERROR ("Reference to previous page, but no pages have been completed yet");
  }
  return pdf_doc_ref_page(page_count>0?page_count:1);
}

pdf_obj *pdf_doc_next_page_ref (void)
{
  return pdf_doc_ref_page(page_count+2);
}

void pdf_doc_new_page (void)
{
#ifdef MEM_DEBUG
MEM_START
#endif
  if (debug) {
    fprintf (stderr, "(pdf_doc_new_page)");
    fprintf (stderr, "page_count=%ld, max_pages=%ld\n", page_count,
	     max_pages);
  }
  /* See if we need more pages allocated yet */
  if (page_count >= max_pages) {
    resize_pages(max_pages+PAGES_ALLOC_SIZE);
  }
  /* Was this page already instantiated by a forward reference to it? */
  if (pages[page_count].page_ref == NULL) {
    /* If not, create it. */
    pages[page_count].page_dict = pdf_new_dict ();
    /* and reference it */
    pages[page_count].page_ref = pdf_ref_obj(pages[page_count].page_dict);
  }
  pdf_add_dict (pages[page_count].page_dict,
		pdf_link_obj(type_name), pdf_link_obj(page_name));
  /* start the contents stream for the new page */
  this_page_contents = pdf_new_stream(STREAM_COMPRESS);
  start_current_page_resources();
  pdf_add_dict (pages[page_count].page_dict,
		pdf_link_obj (resources_name),
		pdf_ref_obj (current_page_resources));
  /* Contents are still available as this_page_contents until next
     page is started */
  /* Even though the page is gone, a Reference to this page is kept
     until program ends */
#ifdef MEM_DEBUG
MEM_END
#endif
}

void pdf_doc_add_to_page (char *buffer, unsigned length)
{
  pdf_add_stream (this_page_contents, buffer, length);
}

void pdf_doc_init (char *filename) 
{
#ifdef MEM_DEBUG
  MEM_START
#endif
  if (debug) fprintf (stderr, "pdf_doc_init:\n");
  pdf_out_init (filename);
#ifdef HAVE_LIBPNG
  /* Create a default name for thumbnail image files */
  if (thumbnail_opt) {
    if (strlen(filename)>4 && !strncmp (".pdf", filename+strlen(filename)-4,4)) {
      thumb_basename = NEW (strlen(filename)+1-4, char);
      strncpy (thumb_basename, filename, strlen(filename)-4);
      thumb_basename[strlen(filename)-4] = 0;
    } else {
      thumb_basename = NEW (strlen(filename)+1, char);
      strcpy (thumb_basename, filename);
    }
  }
#endif /* HAVE_LIBPNG */
  make_short_cuts();
  create_docinfo ();
  create_catalog ();
#ifdef MEM_DEBUG
  MEM_END
#endif
}

void pdf_doc_creator (char *s)
{
  pdf_add_dict (docinfo, pdf_new_name ("Creator"),
		pdf_new_string (s, strlen(s)));
}

void pdf_doc_close ()
{
  if (debug) fprintf (stderr, "pdf_doc_finish:\n");
#ifdef HAVE_LIBPNG
  if (thumb_basename)
    RELEASE (thumb_basename);
#endif /* HAVE_LIBPNG */
  /* Following things were kept around so user can add dictionary
     items */
  finish_docinfo();
  finish_page_tree();
  /* Add names dict to catalog */
  finish_outline();
  finish_dests_tree();
  finish_articles();
  pdf_add_dict (catalog,
		pdf_new_name ("Names"),
		pdf_ref_obj (names_dict));
  pdf_release_obj (names_dict);
  pdf_release_obj (catalog);
  /* Do consistency check on forward references to pages */
  if (highest_page_ref > page_count) {
    unsigned long i;
    fprintf (stderr, "\nWarning:  Nonexistent page(s) referenced\n");
    fprintf (stderr, "          (PDF file may not work right)\n");
    for (i=page_count; i<highest_page_ref; i++) {
      if (pages[i].page_dict) {
	pdf_release_obj (pages[i].page_dict);
	pdf_release_obj (pages[i].page_ref);
      }
    }
  }
  pdf_finish_specials();
  release_short_cuts();
  pdf_out_flush ();
}

static pdf_obj *build_scale_array (double a, double b, double c,
				   double d, double e, double f)
{
  pdf_obj *result;
  result = pdf_new_array();
  pdf_add_array (result, pdf_new_number (a));
  pdf_add_array (result, pdf_new_number (b));
  pdf_add_array (result, pdf_new_number (c));
  pdf_add_array (result, pdf_new_number (d));
  pdf_add_array (result, pdf_new_number (ROUND(e,0.01)));
  pdf_add_array (result, pdf_new_number (ROUND(f,0.01)));
  return result;
}

/* All this routine does is give the form a name
   and add a unity scaling matrix. It fills
   in required fields.  The caller must initialize
   the stream */


void doc_make_form_xobj (pdf_obj *this_form_contents, pdf_obj *bbox,
			 double refptx, double refpty,
			 double xscale, double yscale,
			 pdf_obj *resources, char *form_name)
{
  pdf_obj *xobj_dict, *tmp1;
  xobj_dict = pdf_stream_dict (this_form_contents);
  
  pdf_add_dict (xobj_dict, pdf_new_name ("Name"), pdf_new_name(form_name));
  pdf_add_dict (xobj_dict, pdf_link_obj (type_name),
		pdf_new_name ("XObject"));
  pdf_add_dict (xobj_dict, pdf_new_name ("Subtype"),
		pdf_new_name ("Form"));
  pdf_add_dict (xobj_dict, pdf_new_name ("BBox"), bbox);
  pdf_add_dict (xobj_dict, pdf_new_name ("FormType"), 
		pdf_new_number(1.0));
  /* The reference point of an Xobject is at the lower left corner
     of the bounding box.  Since we would like to have an arbitrary
     reference point, we use a transformation matrix, translating
     the reference point to (0,0) */
  tmp1 = build_scale_array (xscale, 0, 0, yscale, -xscale*refptx, -yscale*refpty);
  pdf_add_dict (xobj_dict, pdf_new_name ("Matrix"), tmp1);
  pdf_add_dict (xobj_dict, pdf_link_obj (resources_name), resources);
  return;
}

struct resource_stack 
{
  pdf_obj *save_page_contents, *save_page_fonts;
  pdf_obj *save_page_xobjects, *save_page_resources;
  int xform_depth;
} res_stack[4];

static int xobjects_pending = 0;

/* begin_form_xobj creates an xobject with its "origin" at
   xpos and ypos that is clipped to the specified bbox. Note
   that the origin is not the lower left corner of the bbox */
pdf_obj *begin_form_xobj (double xpos, double ypos,
			  double bbllx, double bblly,
			  double bburx, double bbury, char *res_name)
{
  pdf_obj *bbox;
  if (xobjects_pending >= sizeof(res_stack)/sizeof(res_stack[0])) {
    fprintf (stderr, "\nForm XObjects nested too deeply.  Limit is %d\n",
	     sizeof(res_stack)/sizeof(res_stack[0]));
    return NULL;
  }
  /* This is a real hack.  We basically treat each xobj as a separate mini
     page unto itself.  Save all the page structures and reinitialize
     them when we finish this xobject. */
  res_stack[xobjects_pending].save_page_resources = current_page_resources;
  current_page_resources = NULL;
  res_stack[xobjects_pending].save_page_xobjects = this_page_xobjects;
  this_page_xobjects = NULL;
  res_stack[xobjects_pending].save_page_fonts = this_page_fonts;
  this_page_fonts = NULL;
  res_stack[xobjects_pending].save_page_contents = this_page_contents;
  this_page_contents = NULL;
  res_stack[xobjects_pending].xform_depth = dev_xform_depth();
  xobjects_pending += 1;
  start_current_page_resources(); /* Starts current_page_resources */
  this_page_contents = pdf_new_stream (STREAM_COMPRESS);
  /* Make a bounding box for this Xobject */
  /* Translate coordinate system so reference point of object 
     is at 0 */
  bbox = pdf_new_array ();
  pdf_add_array (bbox, pdf_new_number (ROUND(bbllx,0.01)));
  pdf_add_array (bbox, pdf_new_number (ROUND(bblly,0.01)));
  pdf_add_array (bbox, pdf_new_number (ROUND(bburx,0.01)));
  pdf_add_array (bbox, pdf_new_number (ROUND(bbury,0.01)));
  /* Resource is already made, so call doc_make_form_xobj() */
  doc_make_form_xobj (this_page_contents, bbox,
		      xpos, ypos, 1.0, 1.0,
		      pdf_ref_obj(current_page_resources), res_name);
  /* Make sure the object is self-contained by adding the
     current font to the object stream */
  dev_reselect_font();
  /* Likewise for color */
  dev_do_color();
  return pdf_link_obj (this_page_contents);
}

void end_form_xobj (void)
{
  if (xobjects_pending>0) {
    xobjects_pending -= 1;
    dev_close_all_xforms(res_stack[xobjects_pending].xform_depth);
    if (this_page_xobjects) {
      pdf_add_dict (current_page_resources, pdf_new_name ("XObject"),
		    pdf_ref_obj (this_page_xobjects));
      pdf_release_obj (this_page_xobjects);
    }
    if (this_page_fonts) {
      pdf_add_dict (current_page_resources, pdf_new_name ("Font"),
		    pdf_ref_obj (this_page_fonts));
      pdf_release_obj (this_page_fonts);
    }
    if (current_page_resources)
      pdf_release_obj (current_page_resources);
    if (this_page_contents)
      pdf_release_obj (this_page_contents);
    current_page_resources = res_stack[xobjects_pending].save_page_resources;
    this_page_xobjects = res_stack[xobjects_pending].save_page_xobjects;
    this_page_fonts = res_stack[xobjects_pending].save_page_fonts;
    this_page_contents = res_stack[xobjects_pending].save_page_contents;
    /* Must reselect the font again in case there was a font change in
       the object */
    dev_reselect_font();
    /* Must reselect color too */
    dev_do_color();
  } else{
    fprintf (stderr, "\nSpecial: exobj: Tried to close a nonexistent xobject\n");
  }
  return;
}

void finish_pending_xobjects (void)
{
  if (xobjects_pending) {
    fprintf (stderr, "\nFinishing a pending form XObject at end of page\n"); 
    while (xobjects_pending--) {
      end_form_xobj();
    }
  }
  return;
}

static struct
{
  pdf_obj *annot_dict;
  unsigned char dirty;
  double llx, lly, urx, ury;
} breaking_state = {NULL, 0};

void pdf_doc_set_box (void)
{
  breaking_state.llx = dev_page_width();
  breaking_state.urx = 0;
  breaking_state.lly = dev_page_height();
  breaking_state.ury = 0;
  breaking_state.dirty = 0;
  return;
}

void pdf_doc_begin_annot (pdf_obj *dict)
{
  breaking_state.annot_dict = dict;
  pdf_doc_set_box ();
  dev_tag_depth ();
  return;
}

void pdf_doc_end_annot (void)
{
  pdf_doc_flush_annot();
  breaking_state.annot_dict = NULL;
  dev_untag_depth ();
  return;
}

void pdf_doc_flush_annot (void)
{
  pdf_obj *rectangle, *new_dict;
  double grow;
  grow = pdf_special_tell_grow ();
  if (breaking_state.dirty) {
    rectangle = pdf_new_array ();
    pdf_add_array (rectangle, pdf_new_number(ROUND(breaking_state.llx-grow, 0.01)));
    pdf_add_array (rectangle, pdf_new_number(ROUND(breaking_state.lly-grow, 0.01)));
    pdf_add_array (rectangle, pdf_new_number(ROUND(breaking_state.urx+grow, 0.01)));
    pdf_add_array (rectangle, pdf_new_number(ROUND(breaking_state.ury+grow, 0.01)));
    new_dict = pdf_new_dict ();
    pdf_add_dict (new_dict, pdf_new_name ("Rect"),
		  rectangle);
    pdf_merge_dict (new_dict, breaking_state.annot_dict);
    pdf_doc_add_to_page_annots (pdf_ref_obj (new_dict));
    pdf_release_obj (new_dict);
  }
  pdf_doc_set_box();
  return;
}

void pdf_doc_expand_box (double llx, double lly, double urx, double
			 ury)
{
  breaking_state.llx = MIN (breaking_state.llx, llx);
  breaking_state.lly = MIN (breaking_state.lly, lly);
  breaking_state.urx = MAX (breaking_state.urx, urx);
  breaking_state.ury = MAX (breaking_state.ury, ury);
  breaking_state.dirty = 1;
  return;
}
