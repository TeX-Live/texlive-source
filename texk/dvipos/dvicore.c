/*  dvipos-20070107

    Copyright (C) 2003 by Jin-Hwan <chofchof@ktug.or.kr>
    
    Includes two small fixes by Sanjoy Mahajan <sanjoy@mit.edu>.  See
    the ChangeLog.

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

#include "utils.h"
#include "dvicore.h"
#include "tfm.h"

#include <math.h>
#include <time.h>

/* Global variables */
int dvi_pages = 0;

/* Internal variables */
static FILE *dvi_file;
static SIGNED_QUAD dvi_fsize, dvi_location;
static SIGNED_QUAD post_location, post_post_location, *page_location = NULL;
static SIGNED_QUAD prev_page_location = -1;
static int current_page, new_dvi_pages = 0;

#define DVI_MAX_STACK_DEPTH 256
static int dvi_stack_depth = 0;
static struct dvi_register {
  SIGNED_QUAD h, v, w, x, y, z, d, hh, vv;
} dvi_state, dvi_stack[DVI_MAX_STACK_DEPTH];

static int cur_font = -1, num_dvi_fonts = 0, max_dvi_fonts = 0;
static struct dvi_font {
  SIGNED_QUAD size, id; /* size == scaled_size */
  int tfm_id, used; char *name;
} *dvi_fonts = NULL;

static SIGNED_QUAD numerator, denominator, dvi_mag, new_mag, mag;
static double tfm_conv, true_conv, conv, resolution;

static UNSIGNED_BYTE id;
static SIGNED_QUAD max_v, max_h, max_v_so_far, max_h_so_far;
static SIGNED_PAIR max_s, total_pages;
static SIGNED_PAIR max_s_so_far, page_count;

typedef struct a_bbox {
  SIGNED_QUAD h1, v1, h2, v2, fb, cb; /* first and current baseline */
  int type, lev_s;
  char *tag;
  struct a_bbox *next;
} BBOX;
static BBOX page_bbox;

static char comment[257];

/* Internal buffer */
#define DVI_BUFFER_SIZE 1024
static int dvi_buffer_len = 0;
static unsigned char dvi_buffer[DVI_BUFFER_SIZE];

static void calc_bbox (SIGNED_QUAD width, SIGNED_QUAD height, SIGNED_QUAD depth)
{
  BBOX *bb = &page_bbox;

  if (do_smashchars) height = depth = 0;

  while (bb) {
    if (bb->fb == -WEB_INFINITY) bb->fb = dvi_state.v;
    bb->cb = dvi_state.v;
    if (bb->h1 > dvi_state.h) bb->h1 = dvi_state.h;
    if (bb->v1 > dvi_state.v - height) bb->v1 = dvi_state.v - height;
    if (bb->h2 < dvi_state.h + width)  bb->h2 = dvi_state.h + width;
    if (bb->v2 < dvi_state.v + depth)  bb->v2 = dvi_state.v + depth;
    bb = bb->next;
  }
}

#define PAGE_TYPE  0
#define BOX_TYPE   1
#define LINES_TYPE 2
#define LINES_TYPE_SUSPEND 3

static void draw_frame (BBOX *bb)
{
  SIGNED_QUAD width, height;

  width  = bb->h2 - bb->h1 + 2 * rule_width;
  height = bb->v2 - bb->v1 + 2 * rule_width;

  /* color special */
  put_unsigned_byte(XXX1, frmfp);
  put_unsigned_byte(20, frmfp);
  switch (bb->type) {
    case BOX_TYPE:
      fwrite("color push rgb 0 1 0", 1, 20, frmfp);
      break;
    case LINES_TYPE:
      fwrite("color push rgb 0 0 1", 1, 20, frmfp);
      break;
    case PAGE_TYPE:
    default:
      fwrite("color push rgb 1 0 0", 1, 20, frmfp);
      break;
  }
  dbg_location += 20;

  /* push the current state */
  put_unsigned_byte(PUSH, frmfp);

  /* move to ll corner plus (-rule_width,rule_width) */
  put_unsigned_byte(RIGHT4, frmfp);
  put_signed_quad(-dvi_state.h + bb->h1 - rule_width, frmfp);
  put_unsigned_byte(DOWN4, frmfp);
  put_signed_quad(-dvi_state.v + bb->v2 + rule_width, frmfp);

  /* draw a horizontal line connecting ll corner to lr corner */
  put_unsigned_byte(PUT_RULE, frmfp);
  put_signed_quad(rule_width, frmfp);
  put_signed_quad(width, frmfp);

  /* draw a vertical line connecting ll corner to ul corner */
  put_unsigned_byte(PUT_RULE, frmfp);
  put_signed_quad(height, frmfp);
  put_signed_quad(rule_width, frmfp);

  /* push the location of ll corner */
  put_unsigned_byte(PUSH, frmfp);

  /* move to ul corner plus (-rule_width,0) */
  put_unsigned_byte(DOWN4, frmfp);
  put_signed_quad(-bb->v2 + bb->v1 - rule_width, frmfp);

  /* draw a horizontal line connecting ul corner to ur corner */
  put_unsigned_byte(PUT_RULE, frmfp);
  put_signed_quad(rule_width, frmfp);
  put_signed_quad(width, frmfp);

  /* restore the location of ll corner */
  put_unsigned_byte(POP, frmfp);

  /* move to lr corner plus (0,rule_width) */
  put_unsigned_byte(RIGHT4, frmfp);
  put_signed_quad(-bb->h1 + bb->h2 + rule_width, frmfp);

  /* draw a vertical line connecting ll corner to ul corner */
  put_unsigned_byte(PUT_RULE, frmfp);
  put_signed_quad(height, frmfp);
  put_signed_quad(rule_width, frmfp);

  /* restore the original location */
  put_unsigned_byte(POP, frmfp);

  /* restore color special */
  put_unsigned_byte(XXX1, frmfp);
  put_unsigned_byte(9, frmfp);
  fwrite("color pop", 1, 9, frmfp);
  dbg_location += 9;
}

static void reset_bbox (BBOX *bb)
{
  bb->h1 = bb->v1 = WEB_INFINITY;
  bb->h2 = bb->v2 = -WEB_INFINITY;
  bb->fb = bb->cb = -WEB_INFINITY;
}

static void clear_bbox (int init)
{
  /* calculate bounding box of each page */
  reset_bbox(&page_bbox);
  page_bbox.type = PAGE_TYPE;
  if (init) {
    page_bbox.tag = xstrdup("pagebb");
    page_bbox.next = NULL;
  }
}

static void new_bbox (char *tag, int type)
{
  BBOX *bb = &page_bbox;

  while (bb->next) {
//    if (strcmp(tag, (bb->next)->tag) == 0) {
//      msg_out(M_WARNING, "Warning: Found an entry with the same tag (%s).\n", tag);
//      return;
//    }
    bb = bb->next;
  }

  /* allocate a new entry */
  bb = bb->next = (BBOX *)xmalloc(sizeof(BBOX));
  bb->tag = xstrdup(tag);
  bb->next = NULL;
  bb->lev_s = dvi_stack_depth;
  bb->type = type;

  reset_bbox(bb);

  if (bbxfp && bb->type == LINES_TYPE)
    fprintf(bbxfp, "\n## KEY & TAG\nbeglines & \"%s\"\n", bb->tag);
}

static void write_bbox (BBOX *bb)
{
  if (bbxfp) {
    fprintf(bbxfp, "\"%ld.%04lX & ",
      (bb->fb / 65536), (bb->fb < 0 ? -1 : 1) * (bb->fb % 65536));
    fprintf(bbxfp, "\"%ld.%04lX \"%ld.%04lX \"%ld.%04lX \"%ld.%04lX & ",
      (bb->v1 / 65536), (bb->v1 < 0 ? -1 : 1) * (bb->v1 % 65536),
      (bb->h1 / 65536), (bb->h1 < 0 ? -1 : 1) * (bb->h1 % 65536),
      (bb->v2 / 65536), (bb->v2 < 0 ? -1 : 1) * (bb->v2 % 65536),
      (bb->h2 / 65536), (bb->h2 < 0 ? -1 : 1) * (bb->h2 % 65536));
    fprintf(bbxfp, "\"%ld.%04lX\n",
      (bb->cb / 65536), (bb->cb < 0 ? -1 : 1) * (bb->cb % 65536));
/*
    fprintf(bbxfp, "\"%ld & ", bb->fb);
    fprintf(bbxfp, "\"%ld \"%ld \"%ld \"%ld & ", bb->v1, bb->h1, bb->v2, bb->h2);
    fprintf(bbxfp, "\"%ld\n", bb->cb);
*/
  }
}

static void flush_bbox (BBOX *bb)
{
  if (bb == NULL) return;
  if (frmfp) draw_frame(bb);
  if (bbxfp) {
    switch (bb->type) {
    case PAGE_TYPE:
      fprintf(bbxfp, "\n## KEY & PAGENO & ENTRY-V & TOP-V LEFT-H BOTTOM-V RIGHT-H & EXIT-V\n");
      fprintf(bbxfp, "%s & %d & ", "pagebb", current_page);
      write_bbox(bb);
      break;
    case BOX_TYPE:
      fprintf(bbxfp, "\n## KEY & TAG & ENTRY-V & TOP-V LEFT-H BOTTOM-V RIGHT-H & EXIT-V\n");
      fprintf(bbxfp, "%s & \"%s\" & ", "box", bb->tag);
      write_bbox(bb);
      break;
    case LINES_TYPE:
      fprintf(bbxfp, "  %s & ", "line");
      write_bbox(bb);
      break;
    case LINES_TYPE_SUSPEND:
      bb->type = LINES_TYPE;
      fprintf(bbxfp, "\n[resume]lines & \"%s\"\n", bb->tag);
      fprintf(bbxfp, "  %s & ", "line");
      write_bbox(bb);
    default:
      break;
    }
  }
}

static void close_bbox (char *tag)
{
  BBOX *obb = &page_bbox, *nbb = obb->next;

  /* find the entry with the same tag */
  while (nbb)
    if (strcmp(nbb->tag, tag) == 0) {
      flush_bbox(nbb);
      if (bbxfp && nbb->type == LINES_TYPE)
        fprintf(bbxfp, "endlines & \"%s\"\n", nbb->tag);
      free(nbb->tag);
      obb->next = (nbb->next ? nbb->next : NULL);
      free(nbb);
      break;
    } else {
      if (bbxfp && nbb->type == LINES_TYPE) {
        fprintf(bbxfp, "[suspend]lines & \"%s\"\n", nbb->tag);
        nbb->type = LINES_TYPE_SUSPEND;
      }
      obb = nbb;
      nbb = nbb->next;
    }
}

static UNSIGNED_BYTE dvi_unsigned_byte (void)
{
  dvi_location++;
  return get_unsigned_byte(dvi_file);
}

static SIGNED_BYTE dvi_signed_byte (void)
{
  dvi_location++;
  return get_signed_byte(dvi_file);
}

static UNSIGNED_PAIR dvi_unsigned_pair (void)
{
  dvi_location += 2;
  return get_unsigned_pair(dvi_file);
}

static SIGNED_PAIR dvi_signed_pair (void)
{
  dvi_location += 2;
  return get_signed_pair(dvi_file);
}

static UNSIGNED_PAIR dvi_unsigned_triple (void)
{
  dvi_location += 3;
  return get_unsigned_triple(dvi_file);
}

static SIGNED_PAIR dvi_signed_triple (void)
{
  dvi_location += 3;
  return get_signed_triple(dvi_file);
}

#if 0
/* Not used */
static UNSIGNED_PAIR dvi_unsigned_quad (void)
{
  dvi_location += 4;
  return get_unsigned_quad(dvi_file);
}
#endif

static SIGNED_PAIR dvi_signed_quad (void)
{
  dvi_location += 4;
  return get_signed_quad(dvi_file);
}

static SIGNED_QUAD sqxfw (SIGNED_QUAD z, SIGNED_QUAD b)
{
  SIGNED_QUAD alpha, beta, result;
  UNSIGNED_BYTE b0, b1, b2, b3;

  alpha = 16;
  while (z >= 0x800000L) {
    z = z / 2;
    alpha = alpha + alpha;
  }
  beta = 256 / alpha;
  alpha = alpha * z;

  b0 = (b >> 24) & 0xFF; b1 = (b >> 16) & 0xFF;
  b2 = (b >> 8) & 0xFF; b3 = b & 0xFF;

  result = (((((b3 * z) / 0x100) + (b2 * z)) / 0x100) + (b1 * z)) / beta;

  if (b0 == 255) result -= alpha;
  else if (b0 != 0)
    msg_out(M_FAIL, "[fatal] sqxfw(): TFM file is bad.\n");

  return result;
}

static SIGNED_QUAD xround (double p)
{
  if (p < 0)
    return (SIGNED_QUAD)ceil(p - .5);
  else
    return (SIGNED_QUAD)floor(p + .5);
}

#define LINE_LENGTH 79
/* 69. The flush_text procedure will empty the buffer if there is something
 *     in it. */
static void flush_text (void)
{
  if (dvi_buffer_len > 0) {
    dvi_buffer[dvi_buffer_len] = 0;
    msg_out(M_DEBUG, "[%s]\n", dvi_buffer);
    dvi_buffer_len = 0;
  }
}

/* 70. And the out_text procedure puts something in it. */
static void out_text (char c)
{
  if (dvi_buffer_len == LINE_LENGTH - 2) flush_text();
  dvi_buffer[dvi_buffer_len++] = c;
}

#define WEB_INFINITY 0x7FFFFFFFL //017777777777
#define MAX_DRIFT 2
#define PIXEL_ROUND(p) xround(conv*(p))

static void move_right (SIGNED_QUAD q)
{
  SIGNED_QUAD h, hh, hhh;

  h = dvi_state.h; hh = dvi_state.hh;

  if (h > 0 && q > 0 && h > WEB_INFINITY - q) {
    msg_out(M_DEBUG, " arithmetic overflow! parameter changed from '%ld' to '%ld'\n", q, WEB_INFINITY - h);
    q = WEB_INFINITY - h;
  }
  if (h < 0 && q < 0 && -h > q + WEB_INFINITY) {
    msg_out(M_DEBUG, " arithmetic overflow! parameter changed from '%ld' to '%ld'\n", q, (-h) - WEB_INFINITY);
    q = (-h) - WEB_INFINITY;
  }
  hhh = PIXEL_ROUND(h + q);
  if (labs(hhh - hh) > MAX_DRIFT) {
    if (hhh > hh) hh = hhh - MAX_DRIFT;
    else hh = hhh + MAX_DRIFT;
  }
  msg_out(M_DEBUG, " h:=%ld", h);
  if (q >= 0) msg_out(M_DEBUG, "+");
  msg_out(M_DEBUG, "%ld=%ld, hh:=%ld", q, h + q, hh);
  h = h + q;
  if (labs(h) > max_h_so_far) {
    if (labs(h) > max_h + 99) {
      msg_out(M_DEBUG, " warning: |h|>%ld!", max_h);
      max_h = labs(h);
    }
    max_h_so_far = labs(h);
  }
  msg_out(M_DEBUG, " \n");
  dvi_state.h = h; dvi_state.hh = hh;
}

/* 92. <Finish a command that sets v = v + p, then goto done> */
static void move_down (SIGNED_QUAD p)
{
  SIGNED_QUAD v, vv, vvv;

  v = dvi_state.v; vv = dvi_state.vv;

  if (v > 0 && p > 0 && v > WEB_INFINITY - p) {
    msg_out(M_DEBUG, " arithmetic overflow! parameter changed from '%ld' to '%ld'\n", p, WEB_INFINITY - v);
    p = WEB_INFINITY - v;
  }
  if (v < 0 && p < 0 && -v > p + WEB_INFINITY) {
    msg_out(M_DEBUG, " arithmetic overflow! parameter changed from '%ld' to '%ld'\n", p, (-v) - WEB_INFINITY);
    p = (-v) - WEB_INFINITY;
  }
  vvv = PIXEL_ROUND(v + p);
  if (labs(vvv - vv) > MAX_DRIFT) {
    if (vvv > vv) vv = vvv - MAX_DRIFT;
    else vv = vvv + MAX_DRIFT;
  }
  msg_out(M_DEBUG, " v:=%ld", v);
  if (p >= 0) msg_out(M_DEBUG, "+");
  msg_out(M_DEBUG, "%ld=%ld, vv:=%ld", p, v + p, vv);
  v = v + p;
  if (labs(v) > max_v_so_far) {
    if (labs(v) > max_v + 99) {
      msg_out(M_DEBUG, " warning: |v|>%ld!", max_v);
      max_v = labs(v);
    }
    max_v_so_far = labs(v);
  }
  msg_out(M_DEBUG, " \n");
  dvi_state.v = v; dvi_state.vv = vv;
}

/* 84. Rounding to the nearest pixel is best done in the manner shown here,
 * so as to be inoffensive to the eye: When the horizontal motion is small,
 * like a kern, hh changes by rounding the kern; but when the motion is
 * large, hh changes by rounding the true position h so that accumulated
 * rounding errors disappear. We allow a larger space in the negative
 * direction than in the positive one, because \TeX{} makes comparatively
 * large backspaces when it positions accents. */
static void out_space (SIGNED_QUAD p)
{
  SIGNED_QUAD font_space;
  
  font_space = (cur_font >= 0 ? dvi_fonts[cur_font].size / 6 : 0);

  if (p >= font_space || p <= -4 * font_space) {
    out_text(' ');
    dvi_state.hh = PIXEL_ROUND(dvi_state.h + p);
  } else
    dvi_state.hh += PIXEL_ROUND(p);

  move_right(p);
}

/* 85. Vertical motion is done similarly, but with the threshold between
 * ``small'' and ``large'' increased by a factor of five. The idea is to
 * make fractions like ``1\over2'' round consistently, but to absorb
 * accumulated rounding errors in the baseline-skip moves. */
static void out_vmove (SIGNED_QUAD p)
{
  SIGNED_QUAD font_space;
  
  font_space = (cur_font >= 0 ? dvi_fonts[cur_font].size / 6 : 0);

  if (labs(p) >= 5 * font_space)
    dvi_state.vv = PIXEL_ROUND(dvi_state.v + p);
  else
    dvi_state.vv += PIXEL_ROUND(p);

  move_down(p);
}

static void do_space (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD x = 0;	/* avoid uninitialized warning */

  msg_out(M_DEBUG, "%ld: ", dvi_location);
  switch (opcode) {
  case RIGHT1:
    x = dvi_signed_byte(); 
    if (frmfp) put_signed_byte(x, frmfp);
    msg_out(M_DEBUG, "right1");
    break;
  case RIGHT2:
    x = dvi_signed_pair(); 
    if (frmfp) put_signed_pair(x, frmfp);
    msg_out(M_DEBUG, "right2");
    break;
  case RIGHT3:
    x = dvi_signed_triple(); 
    if (frmfp) put_signed_triple(x, frmfp);
    msg_out(M_DEBUG, "right3");
    break;
  case RIGHT4:
    x = dvi_signed_quad(); 
    if (frmfp) put_signed_quad(x, frmfp);
    msg_out(M_DEBUG, "right4");
    break;
  case W0:
    x = dvi_state.w;
    msg_out(M_DEBUG, "w0");
    break;
  case W1:
    x = dvi_state.w = dvi_signed_byte(); 
    if (frmfp) put_signed_byte(x, frmfp);
    msg_out(M_DEBUG, "w1");
    break;
  case W2:
    x = dvi_state.w = dvi_signed_pair(); 
    if (frmfp) put_signed_pair(x, frmfp);
    msg_out(M_DEBUG, "w2");
    break;
  case W3:
    x = dvi_state.w = dvi_signed_triple(); 
    if (frmfp) put_signed_triple(x, frmfp);
    msg_out(M_DEBUG, "w3");
    break;
  case W4:
    x = dvi_state.w = dvi_signed_quad(); 
    if (frmfp) put_signed_quad(x, frmfp);
    msg_out(M_DEBUG, "w4");
    break;
  case X0:
    x = dvi_state.x;
    msg_out(M_DEBUG, "x0");
    break;
  case X1:
    x = dvi_state.x = dvi_signed_byte(); 
    if (frmfp) put_signed_byte(x, frmfp);
    msg_out(M_DEBUG, "x1");
    break;
  case X2:
    x = dvi_state.x = dvi_signed_pair(); 
    if (frmfp) put_signed_pair(x, frmfp);
    msg_out(M_DEBUG, "x2");
    break;
  case X3:
    x = dvi_state.x = dvi_signed_triple(); 
    if (frmfp) put_signed_triple(x, frmfp);
    msg_out(M_DEBUG, "x3");
    break;
  case X4:
    x = dvi_state.x = dvi_signed_quad(); 
    if (frmfp) put_signed_quad(x, frmfp);
    msg_out(M_DEBUG, "x4");
    break;
  }
  msg_out(M_DEBUG, " %ld", x);

  if (!dvi_state.d)
    out_space(x);
  else
    out_vmove(x);
}

static void do_vmove (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD y = 0;	/* avoid uninitialized warning */

  flush_text();
  msg_out(M_DEBUG, "%ld: ", dvi_location);
  switch (opcode) {
  case DOWN1:
    y = dvi_signed_byte(); 
    if (frmfp) put_signed_byte(y, frmfp);
    msg_out(M_DEBUG, "down1");
    break;
  case DOWN2:
    y = dvi_signed_pair(); 
    if (frmfp) put_signed_pair(y, frmfp);
    msg_out(M_DEBUG, "down2");
    break;
  case DOWN3:
    y = dvi_signed_triple(); 
    if (frmfp) put_signed_triple(y, frmfp);
    msg_out(M_DEBUG, "down3");
    break;
  case DOWN4:
    y = dvi_signed_quad(); 
    if (frmfp) put_signed_quad(y, frmfp);
    msg_out(M_DEBUG, "down4");
    break;
  case Y0:
    y = dvi_state.y;
    msg_out(M_DEBUG, "y0");
    break;
  case Y1:
    y = dvi_state.y = dvi_signed_byte(); 
    if (frmfp) put_signed_byte(y, frmfp);
    msg_out(M_DEBUG, "y1");
    break;
  case Y2:
    y = dvi_state.y = dvi_signed_pair(); 
    if (frmfp) put_signed_pair(y, frmfp);
    msg_out(M_DEBUG, "y2");
    break;
  case Y3:
    y = dvi_state.y = dvi_signed_triple(); 
    if (frmfp) put_signed_triple(y, frmfp);
    msg_out(M_DEBUG, "y3");
    break;
  case Y4:
    y = dvi_state.y = dvi_signed_quad(); 
    if (frmfp) put_signed_quad(y, frmfp);
    msg_out(M_DEBUG, "y4");
    break;
  case Z0:
    y = dvi_state.z;
    msg_out(M_DEBUG, "z0");
    break;
  case Z1:
    y = dvi_state.z = dvi_signed_byte(); 
    if (frmfp) put_signed_byte(y, frmfp);
    msg_out(M_DEBUG, "z1");
    break;
  case Z2:
    y = dvi_state.z = dvi_signed_pair(); 
    if (frmfp) put_signed_pair(y, frmfp);
    msg_out(M_DEBUG, "z2");
    break;
  case Z3:
    y = dvi_state.z = dvi_signed_triple(); 
    if (frmfp) put_signed_triple(y, frmfp);
    msg_out(M_DEBUG, "z3");
    break;
  case Z4:
    y = dvi_state.z = dvi_signed_quad(); 
    if (frmfp) put_signed_pair(y, frmfp);
    msg_out(M_DEBUG, "z4");
    break;
  }
  msg_out(M_DEBUG, " %ld", y);

  if (!dvi_state.d)
    out_vmove(y);
  else
    out_space(-y);
}

#define RULE_PIXELS(p) (SIGNED_QUAD)ceil(conv*(p))

/* 90. <Finish a command that either sets or puts a rule, then goto
 * move_right or done> */
static void do_rule (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD width, height;

  flush_text();
  switch (opcode) {
  case SET_RULE:
    msg_out(M_DEBUG, "%ld: setrule", dvi_location);
    break;
  case PUT_RULE:
    msg_out(M_DEBUG, "%ld: putrule", dvi_location);
    break;
  }

  height = dvi_signed_quad(); width = dvi_signed_quad();
  if (frmfp) {
    put_signed_quad(height, frmfp); put_signed_quad(width, frmfp);
  }
  msg_out(M_DEBUG, " height %ld width %ld", height, width);
  if (height <= 0 || width <= 0)
    msg_out(M_DEBUG, " (invisible) \n");
  else {
    msg_out(M_DEBUG, " %ldx%ld pixels \n", RULE_PIXELS(height), RULE_PIXELS(width));
    calc_bbox(width, height, 0);
  }

  /*
   * SET_RULE: Typeset a solid black rectangle of height |a| and width |b|,
   * with its bottom left corner at |(h,v)|. Then set |h:=h+b|. If either
   * |a<=0| or |b<=0|, nothing should be typeset. Note that if |b<0|, the
   * value of |h| will decrease even though nothing else happens.
   */
  if (opcode == SET_RULE) {
    dvi_state.hh += RULE_PIXELS(width);
    move_right(width);
  }
}

/* 89. <Finish a command that either sets or puts a character, then
 * goto move_right or done> */
static void fin_set (UNSIGNED_BYTE p, int move)
{
  SIGNED_QUAD width, height, depth;
 
  if (cur_font < 0)
    msg_out(M_FAIL, "[fatal] fin_set(): No font selected.\n");

  width = sqxfw(dvi_fonts[cur_font].size, tfm_get_fw_width(dvi_fonts[cur_font].tfm_id, p));
  height = sqxfw(dvi_fonts[cur_font].size, tfm_get_fw_height(dvi_fonts[cur_font].tfm_id, p));
  depth = sqxfw(dvi_fonts[cur_font].size, tfm_get_fw_depth(dvi_fonts[cur_font].tfm_id, p));

  calc_bbox(width, height, depth);

  if (!move) return;

  if (!dvi_state.d) {
    dvi_state.hh += PIXEL_ROUND(width);
    move_right(width);
  } else {
    dvi_state.vv += PIXEL_ROUND(width);
    move_down(width);
  }
}

/* 88. Translate a set_char command. */
static void do_setchar (UNSIGNED_BYTE opcode)
{
  if (opcode > 32 && opcode < 127) /* from '!' to '~' */
    out_text((char)(opcode - SET_CHAR_0));
  else
    flush_text();
  msg_out(M_DEBUG, "%ld: setchar%d", dvi_location, opcode - SET_CHAR_0);

  fin_set(opcode - SET_CHAR_0, 1);
}

/* 89. <Finish a command that either sets or puts a character, then
 * goto move_right or done> */
static void do_set (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD ch = 0;	/* avoid uninitialized warning */

  flush_text();
  msg_out(M_DEBUG, "%ld: ", dvi_location);

  switch (opcode) {
  case SET1:
    ch = dvi_unsigned_byte();
    if (frmfp) put_unsigned_byte(ch, frmfp);
    break;
  case SET2:
    ch = dvi_unsigned_pair();
    if (frmfp) put_unsigned_pair(ch, frmfp);
    break;
  case SET3:
    ch = dvi_unsigned_triple();
    if (frmfp) put_unsigned_triple(ch, frmfp);
    break;
  case SET4:
    ch = dvi_signed_quad();
    if (frmfp) put_signed_quad(ch, frmfp);
    break;
  }
  msg_out(M_DEBUG, " set%c %ld ", '1' + (opcode - SET1), ch);

  fin_set(ch, 1);
}

/* 89. <Finish a command that either sets or puts a character, then
 * goto move_right or done> */
static void do_put (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD ch = 0;	/* avoid uninitialized warning */

  flush_text();
  msg_out(M_DEBUG, "%ld: ", dvi_location);
  switch (opcode) {
  case PUT1:
    ch = dvi_unsigned_byte();
    if (frmfp) put_unsigned_byte(ch, frmfp);
    break;
  case PUT2:
    ch = dvi_unsigned_pair();
    if (frmfp) put_unsigned_pair(ch, frmfp);
    break;
  case PUT3:
    ch = dvi_unsigned_triple();
    if (frmfp) put_unsigned_triple(ch, frmfp);
    break;
  case PUT4:
    ch = dvi_signed_quad();
    if (frmfp) put_signed_quad(ch, frmfp);
    break;
  }
  msg_out(M_DEBUG, " put%c %ld ", '1' + (opcode - PUT1), ch);

  fin_set(ch, 0);
}

static void show_state (void)
{
  msg_out(M_DEBUG, "level %d:(h=%ld,v=%ld,w=%ld,x=%ld,y=%ld,z=%ld,hh=%ld,vv=%ld) \n", dvi_stack_depth, dvi_state.h, dvi_state.v, dvi_state.w, dvi_state.x, dvi_state.y, dvi_state.z, dvi_state.hh, dvi_state.vv);
}

static void do_push (void) 
{
  flush_text();
  msg_out(M_DEBUG, "%ld: push \n", dvi_location);
  show_state();
  if (dvi_stack_depth < DVI_MAX_STACK_DEPTH)
    dvi_stack[dvi_stack_depth++] = dvi_state;
  else
    msg_out(M_FAIL, "[fatal] do_push(): DVI stack exceeded.\n");
}

static void do_pop (void)
{
  BBOX *bb;

  if (dvi_stack_depth > 0)
    dvi_state = dvi_stack[--dvi_stack_depth];
  else
    msg_out(M_FAIL, "[fatal] do_pop(): Tried to pop an empty stack.\n");
  flush_text();
  msg_out(M_DEBUG, "%ld: pop \n", dvi_location);
  show_state();

  /* reset bounding boxes */
  bb = page_bbox.next;
  while (bb) {
    if (bb->lev_s == dvi_stack_depth + 1 && bb->type == LINES_TYPE) {
      flush_bbox(bb);
      reset_bbox(bb);
    }
    bb = bb->next;
  }
}

static void do_fnt_num (UNSIGNED_BYTE opcode)
{
  register int i;
  SIGNED_QUAD id = 0;	/* avoid uninitialized warning */

  flush_text();
  msg_out(M_DEBUG, "%ld: ", dvi_location);
  if (opcode >= FNT_NUM_0 && opcode <= FNT_NUM_63) {
    id = opcode - FNT_NUM_0;
    msg_out(M_DEBUG, "fntnum%ld ", id);
  } else {
    switch (opcode) {
    case FNT1:
      id = dvi_unsigned_byte();
      if (frmfp) put_unsigned_byte(id, frmfp);
      break;
    case FNT2:
      id = dvi_unsigned_pair();
      if (frmfp) put_unsigned_pair(id, frmfp);
      break;
    case FNT3:
      id = dvi_unsigned_triple();
      if (frmfp) put_unsigned_triple(id, frmfp);
      break;
    case FNT4:
      id = dvi_signed_quad();
      if (frmfp) put_signed_quad(id, frmfp);
      break;
    }
    msg_out(M_DEBUG, "fnt%c %ld ", '1' + (opcode - FNT1), id);
  }

  for (i = 0; i < num_dvi_fonts; i++)
    if (dvi_fonts[i].id == id) break;

  if (i == num_dvi_fonts)
    msg_out(M_FAIL, "[fatal] do_fnt_num(): Tried to select a font (ID %ld) that hasn't been defined.\n", id);

  msg_out(M_DEBUG, "current font is %s \n", dvi_fonts[i].name);

  if (!dvi_fonts[i].used) {
    dvi_fonts[i].tfm_id = tfm_open(dvi_fonts[i].name, 1);
    dvi_fonts[i].used = 1;
  }

  cur_font = i;
}

static void do_pos_special (unsigned char *buffer, SIGNED_QUAD buffer_len)
{
  unsigned char *cmd, *tag, *p;
  SIGNED_QUAD x, y;
  SIGNED_QUAD w = 0, h = 0, d = 0;	/* avoid uninitialized warning */
  double w_pt, h_pt, d_pt;	/* in pt */
  int parsed = 0;		/* how many conversions the sscanf managed */

  struct  /* see \doregisterparoptions in ConTeXt's core-pos.tex */
  {
    double hsize, leftskip, rightskip, hangindent, parindent;
    SIGNED_QUAD hangafter;
  } list;

  for (p = buffer; p - buffer < buffer_len && isspace(*p); p++)
    ; /* skip white chars */
  if (strncmp((char *)p, "pos:", 4)) return;

  for (cmd = p; p - buffer < buffer_len && !isspace(*p); p++)
    ; /* retrieve POS command */

  for (*p++ = 0; p - buffer < buffer_len && isspace(*p); p++)
    ; /* skip white chars */

  /* retrieve POS identification */
  if (*p == '"')
    for (tag = ++p; p - buffer < buffer_len && *p != '"'; p++);
  else
    for (tag = p; p - buffer < buffer_len && !isspace(*p); p++);
  *p = 0;

  for (p++; p - buffer < buffer_len && isspace(*p); p++)
    ; /* skip white chars */
  if (p - buffer < buffer_len)
    {
      /* hangafter is a number, not a dimension, hence its %ld */
      parsed = sscanf((char *)p, "%lfpt %lfpt %lfpt %lfpt,%lfpt,%lfpt,%lfpt,%ld,%lfpt",
		      &w_pt, &h_pt, &d_pt,
		      &list.hsize, &list.leftskip, &list.rightskip,
		      &list.hangindent, &list.hangafter, &list.parindent);
      /* convert pt to sp.  Forget about rounding (add 0.5 if you want it). */
      w = w_pt * 65536;
      h = h_pt * 65536;
      d = d_pt * 65536;
    }

  x = dvi_state.h + denominator / 100;
  y = max_v - dvi_state.v;

  if (strcmp((char *)cmd, "pos:pxy") == 0)
    fprintf(outfp, "\\pospxy{%s}{%d}{%ldsp}{%ldsp}\n", tag, current_page, x, y);
  else if (strcmp((char *)cmd, "pos:pxywhd") == 0)
    fprintf(outfp, "\\pospxywhd{%s}{%d}{%ldsp}{%ldsp}{%ldsp}{%ldsp}{%ldsp}\n", tag, current_page, x, y, w, h, d);
  else if (strcmp((char *)cmd, "pos:pxyplus") == 0) {
    if (parsed < 9)
      fprintf(stderr, "dvipos: only %d conversions for \\pospxyplus but 9 are needed\nBeward: Coordinates in the output may therefore be junk.  Continuing anyway...", parsed);
    fprintf(outfp, "\\pospxyplus{%s}{%d}{%ldsp}{%ldsp}{%ldsp}{%ldsp}{%ldsp}", tag, current_page, x, y, w, h, d);
    fprintf(outfp, "{%lfpt,%lfpt,%lfpt,%lfpt,%ld,%lfpt}\n",
	    list.hsize, list.leftskip, list.rightskip,
	    list.hangindent, list.hangafter, list.parindent);
  }
  else if (strcmp((char *)cmd, "pos:begbox") == 0)
    new_bbox((char *)tag, BOX_TYPE);
  else if (strcmp((char *)cmd, "pos:endbox") == 0)
    close_bbox((char *)tag);
  else if (strcmp((char *)cmd, "pos:beglines") == 0)
    new_bbox((char *)tag, LINES_TYPE);
  else if (strcmp((char *)cmd, "pos:endlines") == 0)
    close_bbox((char *)tag);
}

static void do_xxx (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD size = 0;	/* avoid uninitialized warning */
  unsigned char *sp_buf;

  flush_text();
  msg_out(M_DEBUG, "%ld: xxx ", dvi_location);
  switch (opcode) {
  case XXX1:
    size = dvi_unsigned_byte();
    if (frmfp) put_unsigned_byte(size, frmfp);
    break;
  case XXX2:
    size = dvi_unsigned_pair();
    if (frmfp) put_unsigned_pair(size, frmfp);
    break;
  case XXX3:
    size = dvi_unsigned_triple();
    if (frmfp) put_unsigned_triple(size, frmfp);
    break;
  case XXX4:
    size = dvi_signed_quad();
    if (frmfp) put_signed_quad(size, frmfp);
    break;
  }

  sp_buf = (unsigned char *)calloc(size+1, sizeof(unsigned char));
  if (fread(sp_buf, sizeof(unsigned char), size, dvi_file) != size)
    msg_out(M_FAIL, "[fatal] do_xxx(): Failed to read the special commands with size %d.\n", size);
  if (frmfp) {
    fwrite(sp_buf, sizeof(unsigned char), size, frmfp);
    dbg_location += size;
  }

  msg_out(M_DEBUG, "'%s' \n", sp_buf);
  dvi_location += size;

  do_pos_special(sp_buf, size); /* defined in dvispec.c */

  free(sp_buf);
}

static void dvi_clear_state (void)
{
  cur_font = -1; /* set current font undefined */
  dvi_stack_depth = 0;
  dvi_state.h = 0; dvi_state.v = 0; dvi_state.w = 0;
  dvi_state.x = 0; dvi_state.y = 0; dvi_state.z = 0;
  dvi_state.hh = 0; dvi_state.vv = 0;
  dvi_state.d = 0; /* direction for ASCII pTeX */
}

static void do_bop (void)
{
  int i;

  msg_out(M_DEBUG, " \n%ld: beginning of page %d \n", dvi_location, current_page);
  if (frmfp) {
    for (i = 0; i < 10; i++) put_signed_quad(dvi_signed_quad(), frmfp);
    /* the previous page location */
    dvi_signed_quad();
    put_signed_quad(prev_page_location, frmfp);
    prev_page_location = dbg_location - 45;
  } else
    for (i = 0; i < 11; i++) dvi_signed_quad();
  dvi_clear_state();
  clear_bbox(0);
}

static void do_eop (void)
{
  BBOX *bb;

  flush_text();
  msg_out(M_DEBUG, "%ld: eop ", dvi_location);
  msg_out(M_DEBUG, "[%ld %ld %ld %ld]", page_bbox.h1, page_bbox.v1, page_bbox.h2, page_bbox.v2);

  /* reset bounding boxes */
  bb = page_bbox.next;
  while (bb) {
    reset_bbox(bb);
    bb = bb->next;
  }

  flush_bbox(&page_bbox);
  if (frmfp) put_unsigned_byte(EOP, frmfp);
}

static void do_dir (void)
{
  dvi_state.d = dvi_unsigned_byte();
  if (frmfp) put_unsigned_byte(dvi_state.d, frmfp);
  msg_out(M_DEBUG, "%ld: dir %d \n", dvi_location-1, dvi_state.d);
}

static void do_fnt_def (UNSIGNED_BYTE opcode)
{
  register int dlen, flen;
  SIGNED_QUAD id;
  char *fnt_buf;

  flush_text();
  switch (opcode) {
  case FNT_DEF1:
    id = dvi_unsigned_byte();
    if (frmfp) put_unsigned_byte(id, frmfp);
    msg_out(M_DEBUG, "%ld: fntdef1 %ld: ", dvi_location-1, id);
    break;
  case FNT_DEF2:
    id = dvi_unsigned_pair();
    if (frmfp) put_unsigned_pair(id, frmfp);
    msg_out(M_DEBUG, "%ld: fntdef2 %ld: ", dvi_location-2, id);
    break;
  case FNT_DEF3:
    id = dvi_unsigned_triple();
    if (frmfp) put_unsigned_triple(id, frmfp);
    msg_out(M_DEBUG, "%ld: fntdef3 %ld: ", dvi_location-3, id);
    break;
  case FNT_DEF4:
    id = dvi_signed_quad();
    if (frmfp) put_signed_quad(id, frmfp);
    msg_out(M_DEBUG, "%ld: fntdef4 %ld: ", dvi_location-4, id);
    break;
  }
    
  /* the checksum that TeX found in the TFM file for this font */
  if (frmfp) put_signed_quad(dvi_signed_quad(), frmfp);
  else dvi_signed_quad();
  /* a fixed-point scale factor */
  if (frmfp) put_signed_quad(dvi_signed_quad(), frmfp);
  else dvi_signed_quad();
  /* the "design size" */
  if (frmfp) put_signed_quad(dvi_signed_quad(), frmfp);
  else dvi_signed_quad();
  /* the length of the "area" or directory */
  dlen = dvi_unsigned_byte();
  if (frmfp) put_unsigned_byte(dlen, frmfp);
  /* the length of the font name itself */
  flen = dvi_unsigned_byte();
  if (frmfp) put_unsigned_byte(flen, frmfp);

  /* the external name of the font */
  fnt_buf = (char *)calloc(dlen+flen+1, sizeof(char));
  flen = fread(fnt_buf, sizeof(char), dlen+flen, dvi_file);
  if (frmfp) {
    fwrite(fnt_buf, sizeof(char), flen, frmfp);
    dbg_location += flen;
  }
  fnt_buf[flen] = 0;
  msg_out(M_DEBUG, "%s \n", fnt_buf);
  dvi_location += flen;
  free(fnt_buf);
}

/* 110. The conversion factor \emph{conv} is figured as follows: There are
 * exactly $n/d$ decimicrons per DVI unit, and 254000 decimicrons per inch,
 * and \emph{resolution} pixels per inch. Then we have to adjust this by
 * the stated amount of magnification. */
static void compute_conversion_factors (void)
{
  if ((numerator = dvi_signed_quad()) <= 0)
    msg_out(M_FAIL, "Bad DVI file: numerator is %d!\n", numerator);

  if ((denominator = dvi_signed_quad()) <= 0)
    msg_out(M_FAIL, "Bad DVI file: denominator is %d!\n", denominator);

  msg_out(M_DEBUG, "numerator/denominator=%ld/%ld\n", numerator, denominator);

  tfm_conv = ((double)25400000.0/numerator) * (denominator/(double)473628672)/16.0;
  conv = (numerator/(double)254000.0) * (resolution/denominator);

  dvi_mag = mag = dvi_signed_quad();
  if (new_mag > 0)
    mag = new_mag; /* override the existing maginification */
  else if (mag <= 0)
    msg_out(M_FAIL, "Bad DVI file: magnification is %d!\n", mag);

  true_conv = conv;
  conv = true_conv * (mag/(double)1000.0);

  msg_out(M_DEBUG, "magnification=%ld; %16.8f pixels per DVI unit\n", mag, conv);
}

/* 109. A DVI-reading program that reads the postamble first need not look
 * at the preamble; but DVItype looks at the preamble in order to do error
 * checking, and to display the introductory comment. */
static void process_preamble (void)
{
  fseek(dvi_file, 0L, SEEK_SET);

  /* fetch the first byte */
  if (dvi_unsigned_byte() != PRE)
    msg_out(M_FAIL, "First byte isn't start of preamble!\n");
  /* fetch the identification byte */
  if ((id = dvi_unsigned_byte()) != DVI_ID && id != DVIV_ID)
    msg_out(M_FAIL, "identification in byte 1 should be %d or %d.\n", DVI_ID, DVIV_ID);

  compute_conversion_factors();

  /* fetch the introductory comment */
  comment[fread(comment, sizeof(char), dvi_unsigned_byte(), dvi_file)] = 0;
  msg_out(M_DEBUG, "'%s'\n", comment);
}

/* 19. The last page in a DVI file is followed by `\emph{post}'; this command
 * introduces the postamble, which summarizes important facts that \TeX{} has
 * accumulated about the file, making it possible to print subsets of the data
 * with reasonable efficiency. The postamble has the form
 *
 *   post p[4] num[4] den[4] mag[4] l[4] u[4] s[2] t[2]
 *   <font definitions>
 *   post_post q[4] i[1] 223's[>=4]
 *
 * Here $p$ is a pointer to the final bop in the file. The next three
 * parameters, $num$, $den$, and $mag$, are duplicates of the quantities that
 * appeared in the preamble.
 */

/* 103. Reading the postamble.  Now imagine that we are reading the DVI file
 * and positioned just four bytes after the \emph{post} command. That,
 * in fact, is the situation, when the following part of DVItype is called
 * upon to read, translate, and check the rest of the postamble. */

/* 20. The curious way to finish off a DVI file makes it feasible for
 * DVI-reading programs to find the postamble first, on most computers,
 * even though \TeX{} wants to write the postamble last. Most operating
 * systems permit random access to individual words or bytes of a file,
 * so the DVI reader can start at the end and skip backwards over the
 * 223's until finding the identification byte. Then it can back up four
 * bytes, read $q$, and move to byte $q$ of the file. This byte should,
 * of course, contain the value 248 (\emph{post}); now the postamble can
 * be read, so the DVI reader discovers all the information needed for
 * typesetting the pages. Note that it is also possible to skip through
 * the DVI file at reasonably high speed to locate a particular page,
 * if that proves desirable. This saves a lot of time, since DVI files
 * used in production jobs tend to be large. */
static void read_postamble (void)
{
  UNSIGNED_BYTE opcode;
  SIGNED_QUAD current_location = dvi_fsize;

  /* scan backwards through PADDING */  
  do {
    current_location--;
    fseek(dvi_file, current_location, SEEK_SET);
  } while((opcode = dvi_unsigned_byte()) == PADDING && current_location > 0);

  /* current_location now points to the identification byte */
  if (dvi_fsize <= current_location + 4)
    msg_out(M_FAIL, "Bad DVI file: not enough signature bytes at end of file (%ld)\n", dvi_fsize - current_location - 1);

  post_post_location = current_location - 5;
  fseek(dvi_file, post_post_location, SEEK_SET);
  if ((opcode = dvi_unsigned_byte()) != POST_POST)
     msg_out(M_FAIL, "[fatal] Found (%d) where POST_POST should be.\n", opcode);

  /* now points to the pointer $q$ to the \emph{post} command */
  post_location = dvi_signed_quad();
  fseek(dvi_file, post_location, SEEK_SET);
  if ((opcode = dvi_unsigned_byte()) != POST)
     msg_out(M_FAIL, "[fatal] Found (%d) where POST should be\n", opcode);

  msg_out(M_DEBUG, "Postamble starts at byte %ld.\n", post_location);

  dvi_signed_quad(); /* skip the first four bytes; p[4] */

  if (dvi_signed_quad() != numerator)
    msg_out(M_FAIL, "Bad DVI file: numerator doesn't match the preamble!\n");

  if (dvi_signed_quad() != denominator)
    msg_out(M_FAIL, "Bad DVI file: denominator doesn't match the preamble!\n");

  if (dvi_signed_quad() != mag && new_mag == 0)
    msg_out(M_FAIL, "Bad DVI file: magnification doesn't match the preamble!\n");

  max_v = dvi_signed_quad();
  max_h = dvi_signed_quad();
  msg_out(M_DEBUG, "maxv=%ld, maxh=%ld, ", max_v, max_h);

  if ((max_s = dvi_signed_pair()) > DVI_MAX_STACK_DEPTH)
    msg_out(M_FAIL, "[fatal] maxstackdepth %d exceeds DVI_MAX_STACK_DEPTH %d\n", max_s, DVI_MAX_STACK_DEPTH);
  total_pages = dvi_signed_pair();
  msg_out(M_DEBUG, "maxstackdepth=%d, totalpages=%d\n", max_s, total_pages);
}

static void find_page_location (void) 
{
  int i;

  /* Read the total page number */
  fseek(dvi_file, post_location+27, SEEK_SET);
  if ((dvi_pages = dvi_unsigned_pair()) == 0)
    msg_out(M_FAIL, "[fatal] Total page number is zero.\n");

  /* Read the location of each page */
  page_location = (SIGNED_QUAD *)calloc(dvi_pages, sizeof(SIGNED_QUAD));
  fseek(dvi_file, post_location+1, SEEK_SET);
  page_location[dvi_pages-1] = dvi_signed_quad();
  if (page_location[dvi_pages-1] + 41 > dvi_fsize)
    msg_out(M_FAIL, "[fatal] The location of the page %d was broken.\n", dvi_pages);
  for (i = dvi_pages-2; i >= 0; i--) {
    fseek(dvi_file, page_location[i+1] + 41, SEEK_SET);
    page_location[i] = dvi_signed_quad();
    if (page_location[i] + 41 > dvi_fsize)
      msg_out(M_FAIL, "[fatal] The location of the page %d was broken.\n", i+1);
  }
}

/* 59. The following subroutine does the necessary things when a fnt_def
 * command is being processed. */
static void define_font (SIGNED_QUAD e)
{
  SIGNED_QUAD q, d, m;
  int p, n;
  char *name;

  if (num_dvi_fonts >= max_dvi_fonts) {
    max_dvi_fonts += MAX_FONTS_STEP;
    dvi_fonts = (struct dvi_font *)realloc(dvi_fonts, max_dvi_fonts * sizeof(struct dvi_font));
  }

      dvi_signed_quad(); /* font_check_sum */
  q = dvi_signed_quad(); /* font_scaled_size */
  d = dvi_signed_quad(); /* font_design_size */

  if (q <= 0 || d <= 0) m = 1000;
  else m = xround((1000.0*conv*q)/(true_conv*d));

  p = dvi_unsigned_byte();
  n = dvi_unsigned_byte();

  msg_out(M_DEBUG, "Font %ld: ", e);

  /* Retrieve the directory name and the font name */
  name = (char *)calloc(p+n+1, sizeof(char));
  if (p + n > 0) {
    if (fread(name, sizeof(char), p+n, dvi_file) != p+n)
      msg_out(M_FAIL, "[fatal] Failed to retrieve a font name.\n");
  }
  name[p+n] = 0;

  msg_out(M_DEBUG, "%s", (p + n > 0 ? name : "null font name!"));

  if (m != 1000)
    msg_out(M_DEBUG, " scaled %ld", m);

  dvi_fonts[num_dvi_fonts].name = name;
  dvi_fonts[num_dvi_fonts].size = q; /* scaled_size */
  dvi_fonts[num_dvi_fonts].id = e;
  dvi_fonts[num_dvi_fonts].used = 0;

  msg_out(M_DEBUG, "---loaded at size %ld DVI units \n", q);
  d = xround((100.0*conv*q)/(true_conv*d));
  if (d != 100)
    msg_out(M_DEBUG, " (this font is magnified %ld%c) \n", d, '%');

  num_dvi_fonts++;
}

static void process_fonts (void)
{
  UNSIGNED_BYTE opcode;
//  fseek(dvi_file, post_location+29, SEEK_SET);
  while ((opcode = dvi_unsigned_byte()) != POST_POST)
    switch (opcode) {
      case FNT_DEF1: define_font(dvi_unsigned_byte()); break;
      case FNT_DEF2: define_font(dvi_unsigned_pair()); break;
      case FNT_DEF3: define_font(dvi_unsigned_triple()); break;
      case FNT_DEF4: define_font(dvi_signed_quad()); break;
      default: msg_out(M_FAIL, "Bad DVI file: byte %ld is not postpost!\n", dvi_location);
    }
}

void dvi_init (int m, int r)
{
  time_t tm = time(NULL);
  /* get all variables initialized */
  dvi_file = infp;
  new_mag = (SIGNED_QUAD)m;
  resolution = (double)r;

  /* 73. Three characteristics of the pages (their max_v, max_h, and max_s)
   * are specified in the postamble, and a warning message is printed if
   * their limits are exceeded. Actually max_v is set to the maximum height
   * plus depth of a page, and max_h to the maximum width, for purposes of
   * page layout. Since characters can legally be set outside of the page
   * boundaries, it is not an error when max_v or max_h is exceeded. But
   * max_s should not be exceeded.
   *   The postamble also specifies the total number of pages; DVItype
   * checks to see if this total is accurate. */
//  max_v = max_h = 017777777777 - 99;
//  max_s = stack_size + 1;
  max_v_so_far = max_h_so_far = max_s_so_far = page_count = 0;

  /* Calculate the file size of the DVI file */
  fseek(dvi_file, 0L, SEEK_END);
  dvi_fsize = ftell(dvi_file);
  rewind(dvi_file);

  /* process the preamble */
  process_preamble();

  /* reading the postamble */
  read_postamble();

  /* process the font definitions of the postamble */
  process_fonts();

  /* Find the post opcode */
  find_page_location();

  dvi_clear_state(); clear_bbox(1);

  if (bbxfp) {
    fprintf(bbxfp, "%%!! DVIpos, version (%s), output file\n", VERSION);
    fprintf(bbxfp, "%% This_position_filename: \"%s\"\n", bbxfname);
    fprintf(bbxfp, "%% Command_line_to_dvipos:\n");
    fprintf(bbxfp, "%%+ <THE_COMMAND_LINE>\n");
    fprintf(bbxfp, "%%+ <THE_COMMAND_LINE CONTINUED>\n");
    fprintf(bbxfp, "%% Processing_data_and_time: %s", ctime(&tm));
    fprintf(bbxfp, "%% Input_filename: \"%s\"\n", infname);
    if (frmfp)
      fprintf(bbxfp, "%% DVI_debug_output_filename: \"%s\"\n", frmfname);
    fprintf(bbxfp, "%% DVI_standard: %d\n", id);
    fprintf(bbxfp, "%% DVI_mag: %ld\n", dvi_mag);
    fprintf(bbxfp, "%% DVI_mag_requested: %ld\n", mag);
  }

  if (frmfp) {
    int count = 15 + strlen(comment);
    char *buf = (char *)xmalloc(count);
    fseek(dvi_file, 0L, SEEK_SET);
    fread(buf, 1, count, dvi_file);
    sput_signed_quad(buf+10, mag);
    fwrite(buf, 1, count, frmfp);
    dbg_location += count;
    free(buf);
  }
}

void dvi_close (void)
{
  int i;

  if (frmfp) {
    char *buf;
    int count = post_post_location - post_location - 29;
    SIGNED_QUAD new_post_location = dbg_location; /* new post_location */
    put_unsigned_byte(POST, frmfp);
    put_signed_quad(prev_page_location, frmfp);
    put_signed_quad(numerator, frmfp);
    put_signed_quad(denominator, frmfp);
    put_signed_quad(mag, frmfp);
    put_signed_quad(max_v_so_far, frmfp);
    put_signed_quad(max_h_so_far, frmfp);
    put_unsigned_pair(max_s, frmfp);
    put_unsigned_pair(new_dvi_pages, frmfp);
    buf = (char *)xmalloc(count);
    fseek(dvi_file, post_location + 29, SEEK_SET);
    fread(buf, 1, count, dvi_file);
    fwrite(buf, 1, count, frmfp);
    free(buf);
    dbg_location += count;
    put_unsigned_byte(POST_POST, frmfp);
    put_signed_quad(new_post_location, frmfp);
    put_unsigned_byte(id, frmfp);
    for (i = 0; i < 3; i++)
      put_unsigned_byte(223, frmfp);
    while (dbg_location % 4 != 3)
      put_unsigned_byte(223, frmfp);
  }

  tfm_close_all();
  fclose(dvi_file);
  free(page_location);
  for (i = 0; i < num_dvi_fonts; i++) free(dvi_fonts[i].name);
  if (dvi_fonts) free(dvi_fonts);
}

/* 79. Here is the overall setup. */
void dvi_do_page (int n)
{
  dvi_location = page_location[n-1] - 1;
  current_page = n;
  new_dvi_pages++;

  /* Position to beginning of page */
  fseek(dvi_file, page_location[n-1], SEEK_SET);

  /* set current font undefined; initialize the state variables */
  dvi_clear_state();

  /* 80. Translate the next command in the DVI file until finding EOP. */
  for (;;) {
    UNSIGNED_BYTE opcode = dvi_unsigned_byte();

    if (frmfp && opcode != EOP) put_unsigned_byte(opcode, frmfp);

    /* SET_CHAR_0 - SET_CHAR_127 */
    if (opcode >= SET_CHAR_0 && opcode <= SET_CHAR_127) {
      do_setchar(opcode);
      continue;
    }

    /* FNT_NUM_0 - FNT_NUM_63 */
    if (opcode >= FNT_NUM_0 && opcode <= FNT_NUM_63) {
      do_fnt_num(opcode);
      continue;
    }

    switch (opcode) {
    case SET1:
    case SET2:
    case SET3:
    case SET4: do_set(opcode); break;
    case PUT1:
    case PUT2:
    case PUT3:
    case PUT4: do_put(opcode); break;
    case SET_RULE:
    case PUT_RULE: do_rule(opcode); break;
    case NOP: msg_out(M_DEBUG, "%ld: nop \n", dvi_location); break;
    case BOP: do_bop(); break;
    case EOP: do_eop(); return;
    case PUSH: do_push(); break;
    case POP: do_pop(); break;
    case RIGHT1:
    case RIGHT2:
    case RIGHT3:
    case RIGHT4:
    case W0:
    case W1:
    case W2:
    case W3:
    case W4:
    case X0:
    case X1:
    case X2:
    case X3:
    case X4: do_space(opcode); break;
    case DOWN1:
    case DOWN2:
    case DOWN3:
    case DOWN4:
    case Y0:
    case Y1:
    case Y2:
    case Y3:
    case Y4:
    case Z0:
    case Z1:
    case Z2:
    case Z3:
    case Z4: do_vmove(opcode); break;
    case FNT1:
    case FNT2:
    case FNT3:
    case FNT4: do_fnt_num(opcode); break;
    case XXX1:
    case XXX2: /* Not used in general */
    case XXX3: /* Not used in general */
    case XXX4: do_xxx(opcode); break;
    case FNT_DEF1:
    case FNT_DEF2:
    case FNT_DEF3:
    case FNT_DEF4: do_fnt_def(opcode); break;
    case DIR: do_dir(); break;
    case PRE:
    case POST:
    case POST_POST:
    default:
      msg_out(M_FAIL, "[fatal] Unexpected opcode (%d).", opcode);
    }
  }
}
