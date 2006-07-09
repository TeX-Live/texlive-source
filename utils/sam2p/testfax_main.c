/*
 * testfax_main.c -- CCITTFax(En|De)code usage sample
 * by pts@fazekas.hu at Sat Jul  6 20:03:35 CEST 2002
 * Sun Jul  7 22:25:55 CEST 2002
 *
 * Note that this sample is incomplete since the parameters (such as /K)
 * cannot be set from the command line.
 */

#include "pts_fax.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#ifdef __GNUC__
#pragma implementation
#endif

#if OBJDEP
#  warning PROVIDES: testfax_main
#endif

static void* gsfax_xalloc(unsigned n) {
  void *ret;
  if ((ret=malloc(n))==0) abort();
  return ret;
}
static void gsfax_free(void *ptr) {
  free(ptr);
}
static void gsfax_memset(void *s, int c, unsigned n) {
  /*return*/ memset(s,c,n);
}
static void gsfax_memcpy(void *dest, const void *src, unsigned n) {
  /*return*/ memcpy(dest, src, n);
}



#if 0
CCITTFaxEncode:

bool Uncompressed=false;
int  K=0;
bool EndOfLine=false;
bool EncodedByteAlign=false;
int  Columns=1728;
int  Rows=0; // Decode only
bool EndOfBlock=true;
bool BlackIs1=false;
int  DamagedRowsBeforeError=0; // Decode only
bool FirstBitLowOrder=false; // GS only
int  DecodedByteAlign=1; // GS only
bool CloseSource;
bool CloseTarget;
#endif

/* GS expected
 * min_left=0
 * bits_left=32
 *
 */

static void work(FILE *f, stream_template const*template_, stream_state *state) {
  unsigned got, pog;
  stream_cursor_read r;
  stream_cursor_write w;
  unsigned char rbuf[4096], wbuf[1000], *hard, *rlimit;
  bool last;

  #if __CHECKER__
    memset(&r, 0, sizeof(r));
    memset(&w, 0, sizeof(w));
  #endif

  template_->init(state);
  
  r.ptr=rlimit=rbuf-1;
  hard=rbuf+sizeof(rbuf)-1;
  assert(hard-r.ptr>=(int)template_->min_in_size);
  last=false;


  while (1) {
    assert(r.ptr==rbuf-1);
    if (last==false) {
      if (0==(got=fread(rlimit+1, 1, hard-rlimit, f))) last=true;
      rlimit+=got;
    }
    /* if (r.ptr==rlimit) break; */
    w.ptr=wbuf-1;
    w.limit=wbuf+sizeof(wbuf)-1;
    assert(w.limit-w.ptr>=(int)template_->min_out_size);
    r.limit=rlimit;
    pog=template_->process(state, &r, &w, last);
    fprintf(stderr, "pog=%d write=%d last=%d\n", pog, w.ptr-(wbuf-1), last);
    if ((int)pog==PTSFAX_ERRC) {
      fprintf(stderr, "syntax error!\n");
      abort();
    }
    /* assert(last || r.ptr>=rbuf); */
    fwrite(wbuf, 1, w.ptr-(wbuf-1), stdout);
    if (last && (pog==0 || (int)pog==PTSFAX_EOFC)) break;
    if (r.ptr!=rbuf-1) {
      fprintf(stderr, "limit=%d\n", rlimit-r.ptr);
      memmove(rbuf, r.ptr+1, rlimit-r.ptr);
      rlimit=rbuf-1+(rlimit-r.ptr);
      r.ptr=rbuf-1;
    }
  }

  template_->release(state);
}

int main(int argc, char **argv) {
  FILE *f;

  (void)argc;
  (void)argv;
/*
-* Define a limit on the Rows parameter, close to max_int. *-
#define cf_max_height 32000
    if (code >= 0 &&
	(state.K < -cf_max_height || state.K > cf_max_height ||
	 state.Columns < 0 || state.Columns > cfe_max_width ||
	 state.Rows < 0 || state.Rows > cf_max_height ||
	 state.DamagedRowsBeforeError < 0 ||
	 state.DamagedRowsBeforeError > cf_max_height ||
	 state.DecodedByteAlign < 1 || state.DecodedByteAlign > 16 ||
	 (state.DecodedByteAlign & (state.DecodedByteAlign - 1)) != 0)
	)
*/

  f=stdin;
  /* f=fopen("stdpre.x","rb"); */
  /* f=fopen("216","rb"); */
  assert(f!=0);

  if (0) {
  #if USE_BUILTIN_FAXE
  #if OBJDEP
  #  warning REQUIRES: pts_faxe
  #endif
  } else if (argc==2 && 0==strcmp(argv[1], "encode")) {
    stream_CFE_state sCFEs;
    sCFEs.memset_=gsfax_memset;
    sCFEs.xalloc_=gsfax_xalloc;
    sCFEs.free_=gsfax_free;
    sCFEs.memcpy_=gsfax_memcpy;
    s_CFE_template.set_defaults((stream_state*)&sCFEs);
    /* sCFEs.K=-1; ... */
    work(f, (stream_template const*)&s_CFE_template, (stream_state*)&sCFEs);
  #endif
  #if USE_BUILTIN_FAXD
  #if OBJDEP
  #  warning REQUIRES: pts_faxd
  #endif
  } else if (argc==2 && 0==strcmp(argv[1], "decode")) { 
    stream_CFD_state sCFDs;
    sCFDs.memset_=gsfax_memset;
    sCFDs.xalloc_=gsfax_xalloc;
    sCFDs.free_=gsfax_free;
    sCFDs.memcpy_=gsfax_memcpy;
    s_CFD_template.set_defaults((stream_state*)&sCFDs);
    /* sCFEs.K=-1; ... */
    work(f, (stream_template const*)&s_CFD_template, (stream_state*)&sCFDs);
  #endif
  } else {
    fprintf(stderr, "Usage: %s encode|decode <INFILE >OUTFILE\n", argv[0]);
    return 2;
  }
  fclose(f);

  return 0;
}
