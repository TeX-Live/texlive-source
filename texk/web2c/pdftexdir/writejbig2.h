/***********************************************************************
Copyright (c) 2002-2006 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

pdfTeX is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with pdfTeX; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA 02111-1307 USA

This is experimental JBIG2 image support to pdfTeX. JBIG2 image decoding
is part of Adobe PDF-1.4, and requires Acroread 5.0 or later.

$Id: writejbig2.h,v 1.40 2006/05/31 19:18:51 hahe Exp hahe $
***********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "ptexlib.h"
#include "ptexmac.h"
#include "image.h"

/* 7.3 Segment types */
#define M_SymbolDictionary 0
#define M_IntermediateTextRegion 4
#define M_ImmediateTextRegion 6
#define M_ImmediateLosslessTextRegion 7
#define M_PatternDictionary 16
#define M_IntermediateHalftoneRegion 20
#define M_ImmediateHalftoneRegion 22
#define M_ImmediateLosslessHalftoneRegion 23
#define M_IntermediateGenericRegion 36
#define M_ImmediateGenericRegion 38
#define M_ImmediateLosslessGenericRegion 39
#define M_IntermediateGenericRefinementRegion 40
#define M_ImmediateGenericRefinementRegion 42
#define M_ImmediateLosslessGenericRefinementRegion 43
#define M_PageInformation 48
#define M_EndOfPage 49
#define M_EndOfStripe 50
#define M_EndOfFile 51
#define M_Profiles 52
#define M_Tables 53
#define M_Extension 62

/**********************************************************************/

typedef enum { INITIAL, HAVEINFO, WRITEPDF } PHASE;

typedef struct _LITEM {
    struct _LITEM *prev;
    struct _LITEM *next;
    void *d;                    /* data */
} LITEM;

typedef struct _LIST {
    LITEM *first;
    LITEM *last;
    struct avl_table *tree;
} LIST;

typedef struct _SEGINFO {
    unsigned long segnum;
    boolean isrefered;
    boolean refers;
    unsigned int seghdrflags;   /* set by readseghdr() */
    boolean pageassocsizeflag;  /* set by readseghdr() */
    unsigned int reftosegcount; /* set by readseghdr() */
    unsigned int countofrefered;        /* set by readseghdr() */
    unsigned int fieldlen;      /* set by readseghdr() */
    unsigned int segnumwidth;   /* set by readseghdr() */
    long segpage;               /* set by readseghdr() */
    unsigned long segdatalen;   /* set by readseghdr() */
    unsigned long hdrstart;     /* set by readseghdr() */
    unsigned long hdrend;       /* set by readseghdr() */
    unsigned long datastart;
    unsigned long dataend;
    boolean endofstripeflag;    /* set by checkseghdrflags() */
    boolean endofpageflag;      /* set by checkseghdrflags() */
    boolean pageinfoflag;       /* set by checkseghdrflags() */
    boolean endoffileflag;      /* set by checkseghdrflags() */
} SEGINFO;

typedef struct _PAGEINFO {
    LIST segments;              /* segments associated with page */
    unsigned long pagenum;
    unsigned int width;
    unsigned int height;
    unsigned int xres;
    unsigned int yres;
    unsigned int pagesegmentflags;
    unsigned int stripinginfo;
    unsigned int stripedheight;
} PAGEINFO;

typedef struct _FILEINFO {
    FILE *file;
    char *filename;
    long filesize;
    LIST pages;                 /* not including page0 */
    LIST page0;
    unsigned int filehdrflags;  /* set by readfilehdr() */
    boolean sequentialaccess;   /* set by readfilehdr() */
    unsigned long numofpages;   /* set by readfilehdr() */
    unsigned long streamstart;  /* set by get_jbig2_info() */
    unsigned long pdfpage0objnum;
    PHASE phase;
} FILEINFO;

/**********************************************************************/

static int comp_file_entry(const void *, const void *, void *);
static int comp_page_entry(const void *, const void *, void *);
static int comp_segment_entry(const void *, const void *, void *);
int ygetc(FILE *);
FILEINFO *new_fileinfo();
PAGEINFO *new_pageinfo();
void initseginfo(SEGINFO *);
void initlinkedlist(LIST *);
LIST *litem_append(LIST *);
void pages_maketree(LIST *);
void segments_maketree(LIST *);
PAGEINFO *find_pageinfo(LIST *, unsigned long);
SEGINFO *find_seginfo(LIST *, unsigned long);
unsigned int read2bytes(FILE *);
unsigned long read4bytes(FILE *);
unsigned long getstreamlen(LITEM *, boolean);
void readfilehdr(FILEINFO *);
boolean readseghdr(FILEINFO *, SEGINFO *);
void writeseghdr(FILEINFO *, SEGINFO *);
void checkseghdr(FILEINFO *, SEGINFO *);
void checkseghdrflags(SEGINFO * sip);
void markpage0seg(FILEINFO *, unsigned long);
unsigned long findstreamstart(FILEINFO *);
void rd_jbig2_info(FILEINFO *);
void wr_jbig2(FILEINFO *, unsigned long);
void read_jbig2_info(integer);
void write_jbig2(integer);
void flushjbig2page0objects();

/**********************************************************************/
