/*
 *   This is the main routine for the first (prescanning) pass.
 */
#ifndef XDVIPSK
#include "dvips.h" /* The copyright notice in that file is included too! */
#else
#include "xdvips.h" /* The copyright notice in that file is included too! */
#endif /* XDVIPSK */

/*
 *   The external declarations:
 */
#include "protos.h"
#ifdef XDVIPSK
#include "uthash.h"

charusetype_ref* charused_hash[RESHASHPRIME];
#endif /* XDVIPSK */

/*
 *   This routine handles the processing of the preamble in the dvi file.
 */
void
readpreamble(void)
{
   register int i;
   char *p;

   if (dvibyte()!=247) error("! Bad DVI file: first byte not preamble");
   if (dvibyte()!=2) error("! Bad DVI file: id byte not 2");
   num = signedquad();
   den = signedquad();
   if (overridemag > 0) signedquad();
   else if (overridemag < 0) mag = (mag * signedquad()) / 1000.0;
   else mag = signedquad();
   conv = (real) num * DPI * mag / ( den * 254000000.0 ); 
   vconv = (real) num * VDPI * mag / ( den * 254000000.0 ); 
   alpha = (((real)den / 7227.0) / 0x100000) * (25400000.0 / (real) num);
   fsizetol = 1 + (integer)(DPI/(72270.0 * conv));
   if (!pprescan) {
     for (i=dvibyte(),p=preamblecomment;i>0;i--,p++) *p=dvibyte();
     *p='\0';
     if (!quiet) {
        fprintf(stderr, "'");
#ifdef VMCMS /* IBM: VM/CMS */
        for(p=preamblecomment;*p;p++) putc(ascii2ebcdic[*p], stderr);
#else
#ifdef MVSXA /* IBM: MVS/XA */
        for(p=preamblecomment;*p;p++) putc(ascii2ebcdic[*p], stderr);
#else
        for(p=preamblecomment;*p;p++) putc_str(*p, stderr);
#endif  /* IBM: VM/CMS */
#endif
        fprintf_str(stderr, "' -> %s\n", oname);
      }
   } else
      skipover(dvibyte());
}

#ifdef XDVIPSK
charusetype_ref *lookup_charused(const char* ps_name, quarterword /* otftype_enum */ otftype)
{
    charusetype_ref *ptr;

    assert(ps_name);

    for (ptr = charused_hash[hash(ps_name)]; ptr != NULL; ptr = ptr->next)
        if (ptr->head && ptr->head->charused_ptr && ptr->head->charused_ptr->fd && ptr->head->charused_ptr->fd->resfont && ptr->head->charused_ptr->fd->resfont->PSname &&
                (strcmp(ptr->head->charused_ptr->fd->resfont->PSname, ps_name) == 0) && otftype_conforms(ptr->head->charused_ptr->fd->resfont->otftype, otftype))
            break;

    return(ptr);
}
#endif /* XDVIPSK */

/*
 *   Finally, here's our main prescan routine.
 */
static integer firstmatch = -1, lastmatch = -1;
void
#ifdef XDVIPSK
prescanpages(boolean parse_maps)
#else
prescanpages(void)
#endif /* XDVIPSK */
{
   register int cmd;
   short ret = 0;
   register integer thispageloc, thissecloc;
   register fontdesctype *f;
   register shalfword c;
   register long thissectionmem = 0;
   integer mpagenum;
   integer pageseq = 0;
   int ntfirst = notfirst;
#ifdef XDVIPSK
   chardesctype *current, *tmp;
#endif /* XDVIPSK */

#ifdef XDVIPSK
   firstmatch = -1;
   lastmatch = -1;
   pprescan = parse_maps;
#endif /* XDVIPSK */
   readpreamble();
/*
 *   Now we look for the first page to process.  If we get to the end of
 *   the file before the page, we complain (fatally).
 *   Incidentally, we don't use the DVI file's bop backpointer to skip
 *   over pages at high speed, because we want to look to for special
 *   header that might be in skipped pages.
 */
   while (1) {
      cmd = skipnop();
      if (cmd==248)
         error("! End of document before first specified page");
      if (cmd!=139)
         error("! Bad DVI file: expected bop");
      thispageloc = ftell(dvifile); /* the location FOLLOWING the bop */
#ifdef DEBUG
      if (dd(D_PAGE))
#ifdef SHORTINT
      fprintf(stderr,"bop at %ld\n", thispageloc);
#else   /* ~SHORTINT */
      fprintf(stderr,"bop at %d\n", (int)thispageloc);
#endif  /* ~SHORTINT */
#endif  /* DEBUG */
      pagenum = signedquad();
      pageseq++;
      mpagenum = abspage ? pageseq : pagenum;
      if (mpagenum == firstpage && ntfirst)
         firstmatch++;
      if (mpagenum == lastpage && notlast)
         lastmatch++;
      if (ntfirst && mpagenum == firstpage && firstmatch == firstseq)
         ntfirst = 0;
      if (ntfirst ||
          ((evenpages && (pagenum & 1)) || (oddpages && (pagenum & 1)==0) ||
           (pagelist && !InPageList(pagenum)))) {
         skipover(40);
         skippage();
      } else {
         if (notlast && mpagenum == lastpage)
            lastmatch--;
         break;
      }
   }
/*
 *   Here we scan for each of the sections.  First we initialize some of
 *   the variables we need.
 */
   while (maxpages > 0 && cmd != 248) {
      for (f=fonthead; f; f=f->next) {
         f->psname = 0;
#ifndef XDVIPSK
         if (f->loaded==1)
            for (c=255; c>=0; c--)
               f->chardesc[c].flags &= (STATUSFLAGS);
#else
		 if (f->loaded == 1) {
			 HASH_ITER(hh, f->chardesc_hh, current, tmp) {
				 current->flags &= (STATUSFLAGS);
			 }
		 }
#endif /* XDVIPSK */
      }
      fontmem = swmem - OVERCOST;
      if (fontmem <= 1000)
         error("! Too little VM in printer");

/*   The section begins at the bop command just before thispageloc (which may
 *   be a page that was aborted because the previous section overflowed memory).
 */
      pagecount = 0;
      fseek(dvifile, (long)thispageloc, 0);
      pagenum = signedquad();
      skipover(40);
      thissecloc = thispageloc;
/*
 *   Now we have the loop that actually scans the pages.  The scanpage routine
 *   returns 1 if the page scans okay; it returns 2 if the memory ran out
 *   before any pages were completed (in which case we'll try to carry on
 *   and hope for the best); it returns 0 if a page was aborted for lack
 *   of memory. After each page, we mark the characters seen on that page
 *   as seen for this section so that they will be downloaded.
 */
      ret = 0;
      while (maxpages>0) {
         if (!(evenpages && (pagenum & 1)) &&
             !(oddpages && (pagenum & 1)==0) &&
             !(pagelist && !InPageList(pagenum))) {
#ifdef XDVIPSK
            ret = scanpage(parse_maps);
#else
            ret = scanpage();
#endif /* XDVIPSK */
            if (ret == 0)
               break;
            pagecount++;
            maxpages--;
         } else
            skippage();
         thissectionmem = swmem - fontmem - OVERCOST;
         mpagenum = abspage ? pageseq : pagenum;
         pageseq++;
         if (mpagenum == lastpage && notlast)
            lastmatch++;
         if (notlast && mpagenum == lastpage && lastmatch == lastseq)
            maxpages = -1; /* we are done after this page. */
         if (reverse)
            thissecloc = thispageloc;
         for (f=fonthead; f; f=f->next)
            if (f->loaded==1) {
               if (f->psflag & THISPAGE)
                  f->psflag = PREVPAGE;
#ifndef XDVIPSK
               for (c=255; c>=0; c--)
                  if (f->chardesc[c].flags & THISPAGE)
                     f->chardesc[c].flags = PREVPAGE |
               (f->chardesc[c].flags & (STATUSFLAGS));
#else
			   HASH_ITER(hh, f->chardesc_hh, current, tmp) {
				   if (current->flags & THISPAGE)
					   current->flags = PREVPAGE |
					   (current->flags & (STATUSFLAGS));
			   }
#endif /* XDVIPSK */
            }
         cmd=skipnop();
         if (cmd==248) break;
         if (cmd!=139)
            error("! Bad DVI file: expected bop");
         thispageloc = ftell(dvifile);
#ifdef DEBUG
         if (dd(D_PAGE))
#ifdef SHORTINT
         fprintf(stderr,"bop at %ld\n", thispageloc);
#else   /* ~SHORTINT */
         fprintf(stderr,"bop at %d\n", (int)thispageloc);
#endif  /* ~SHORTINT */
#endif  /* DEBUG */
         pagenum = signedquad();
         skipover(40);
         if (ret==2 || (maxsecsize && pagecount >= maxsecsize))
            break;
      }
/*
 *   Now we have reached the end of a section for some reason.
 *   If there are any pages, we save the pagecount, section location,
 *   and continue.
 */
#ifdef XDVIPSK
      if (!parse_maps) {
#endif /* XDVIPSK */
         if (pagecount>0) {
            register int fc = 0;
            register sectiontype *sp;
            register charusetype *cp;

            totalpages += pagecount;
            for (f=fonthead; f; f=f->next)
               if (f->loaded==1 && f->psname)
                  fc++;
            sp = (sectiontype *)mymalloc((integer)(sizeof(sectiontype) +
               fc * sizeof(charusetype) + sizeof(fontdesctype *)));
            sp->bos = thissecloc;
            if (reverse) {
               sp->next = sections;
               sections = sp;
            } else {
               register sectiontype *p;

               sp->next = NULL;
               if (sections == NULL)
                  sections = sp;
               else {
                  for (p=sections; p->next != NULL; p = p->next);
                  p->next = sp;
               }
            }
            sp->numpages = pagecount;
#ifdef DEBUG
            if (dd(D_PAGE))
#ifdef SHORTINT
               fprintf(stderr,"Have a section: %ld pages at %ld fontmem %ld\n",
#else   /* ~SHORTINT */
               fprintf(stderr,"Have a section: %d pages at %d fontmem %d\n",
#endif  /* ~SHORTINT */
                  (integer)pagecount, (integer)thissecloc, (integer)thissectionmem);
#endif  /* DEBUG */
            cp = (charusetype *) (sp + 1);
            fc = 0;
            for (f=fonthead; f; f=f->next) {
               if (f->loaded==1 && f->psname) {
                  register halfword b, bit;

                  cp->psfused = (f->psflag & PREVPAGE);
                  f->psflag = 0;
                  cp->fd = f;
#ifdef XDVIPSK
                  if (f->resfont && f->resfont->PSname)
                  {
                      charusetype_ref *cu_ref;
                      charusetype_entry *cu_entry;
                      int hh;
                      cu_ref = lookup_charused(f->resfont->PSname, f->resfont->otftype);
                      if (cu_ref == NULL)
                      {
                          cu_ref = (charusetype_ref *)mymalloc((integer)sizeof(charusetype_ref));
                          assert(cu_ref);
                          memset(cu_ref, 0, sizeof(charusetype_ref));
                          hh = hash(f->resfont->PSname);
                          cu_ref->next = charused_hash[hh];
                          charused_hash[hh] = cu_ref;
                      }
                      cu_entry = (charusetype_entry *)mymalloc((integer)sizeof(charusetype_entry));
                      assert(cu_entry);
                      memset(cu_entry, 0, sizeof(charusetype_entry));
                      cu_entry->next = cu_ref->head;
                      cu_ref->head = cu_entry;
                      cu_entry->charused_ptr = cp;
                  }

                  memset(cp->bitmap, 0, sizeof(cp->bitmap));
                  if (f->resfont && f->resfont->otftype) {
                     HASH_ITER(hh, f->chardesc_hh, current, tmp) {
                        if (current->charcode == 120601) {
                           int xxx = 0;
                        }
                        if ((current->cid > 0) && (current->flags & PREVPAGE))
                           ADD_TO_USED_CHARS(cp->bitmap, current->cid);
                     }
                  }
                  else {
#endif /* XDVIPSK */
                     c = 0;
                     for (b=0; b<16; b++) {
                        cp->bitmap[b] = 0;
                        for (bit=32768; bit!=0; bit>>=1) {
#ifdef XDVIPSK
                           current = find_chardesc(f, c);
                           if (!current)
                           {
                               PRINTF_PR("Error: character %d not found in the font %s\n", c, f->name);
                           }
                           else
                              if (current->flags & PREVPAGE)
#else
                           if (f->chardesc[c].flags & PREVPAGE)
#endif /* XDVIPSK */
                              cp->bitmap[b] |= bit;
                           c++;
                        }
#ifdef XDVIPSK
                     }
#endif /* XDVIPSK */
                  }
                  cp++;
               }
            }
            cp->fd = NULL;
#ifdef XDVIPSK
         }
#endif /* XDVIPSK */
      }
   }
}
