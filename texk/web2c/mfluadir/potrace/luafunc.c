/* From potracelib_demo.c */

#include "luafunc.h"



/* ---------------------------------------------------------------------- */
/* auxiliary bitmap functions */


/* macros for writing individual bitmap pixels */
#define BM_WORDSIZE ((int)sizeof(potrace_word))
#define BM_WORDBITS (8*BM_WORDSIZE)
#define BM_HIBIT (((potrace_word)1)<<(BM_WORDBITS-1))
#define bm_scanline(bm, y) ((bm)->map + (y)*(bm)->dy)
#define bm_index(bm, x, y) (&bm_scanline(bm, y)[(x)/BM_WORDBITS])
#define bm_mask(x) (BM_HIBIT >> ((x) & (BM_WORDBITS-1)))
#define bm_range(x, a) ((int)(x) >= 0 && (int)(x) < (a))
#define bm_safe(bm, x, y) (bm_range(x, (bm)->w) && bm_range(y, (bm)->h))
#define BM_USET(bm, x, y) (*bm_index(bm, x, y) |= bm_mask(x))
#define BM_UCLR(bm, x, y) (*bm_index(bm, x, y) &= ~bm_mask(x))
#define BM_UPUT(bm, x, y, b) ((b) ? BM_USET(bm, x, y) : BM_UCLR(bm, x, y))
#define BM_PUT(bm, x, y, b) (bm_safe(bm, x, y) ? BM_UPUT(bm, x, y, b) : 0)

/* return new un-initialized bitmap. NULL with errno on error */
static potrace_bitmap_t *bm_new(int w, int h) {
  potrace_bitmap_t *bm;
  int dy = (w + BM_WORDBITS - 1) / BM_WORDBITS;

  bm = (potrace_bitmap_t *) malloc(sizeof(potrace_bitmap_t));
  if (!bm) {
    return NULL;
  }
  bm->w = w;
  bm->h = h;
  bm->dy = dy;
  bm->map = (potrace_word *) calloc(h, dy * BM_WORDSIZE);
  if (!bm->map) {
    free(bm);
    return NULL;
  }
  return bm;
}

/* free a bitmap */
static void bm_free(potrace_bitmap_t *bm) {
  if (bm != NULL) {
    free(bm->map);
  }
  free(bm);
}

//int potrace_getMFoutlines(const uint8_t *raster,int w, int h) {
int potrace_getMFoutlines(const uint8_t *raster, int w, int h, MFoutlines_param_t opt_param,potrace_state_t **trace) {
  int x, y;
  potrace_bitmap_t *bm;
  potrace_param_t *param;
  potrace_state_t *st;
  int W,X;
  uint8_t data,pix;

  /* create a bitmap */
  bm = bm_new(w, h);
  if (!bm) {
    fprintf(stderr, "! Error allocating bitmap: %s\n", strerror(errno)); 
    return 1;
  }

  /* fill the bitmap  */
  W= (w%8)==0 ?(w/8):(w/8+1);
  for (y=0; y<h; y++) {
    for (X=0;X<W; X++) {
      data = raster[W*y+X];
      pix = (data & (1<<7))==0 ? 0:1;x = 0+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<6))==0 ? 0:1;x = 1+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<5))==0 ? 0:1;x = 2+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<4))==0 ? 0:1;x = 3+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<3))==0 ? 0:1;x = 4+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<2))==0 ? 0:1;x = 5+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<1))==0 ? 0:1;x = 6+X*8;BM_PUT(bm, x, y, pix);
      pix = (data & (1<<0))==0 ? 0:1;x = 7+X*8;BM_PUT(bm, x, y, pix);
    }
  }

  /* set tracing parameters, starting from defaults */
  if (opt_param.potrace_param == NULL) {
    param = potrace_param_default();printf("opt_param.potrace_param == NULL\n");
    if (!param) {
     fprintf(stderr, "! Error allocating parameters: %s\n", strerror(errno)); 
     return 1;
    }
    param->turdsize = 0;
  } else {
    param = opt_param.potrace_param;
  }
  /*printf("turdsize=%d;turnpolicy=%d;alphamax=%f;opticurve=%d;opttolerance=%f\n",param->turdsize,param->turnpolicy,param->alphamax,param->opticurve,param->opttolerance);*/

  /* trace the bitmap */
  st = potrace_trace(param, bm);
  if (!st || st->status != POTRACE_STATUS_OK) {
    fprintf(stderr, "! Error tracing bitmap: %s\n", strerror(errno));
    bm_free(bm);
    return 1;
  }
  bm_free(bm);
  *trace = st ;
  return 0;
}