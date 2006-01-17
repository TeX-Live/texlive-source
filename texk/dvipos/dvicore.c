/*  $Header
    
    dvipos-20030225

    Copyright (C) 2003 by Jin-Hwan <chofchof@ktug.or.kr>
    
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

#include "dvicore.h"
#include "tfm.h"

/* Global variables */
int dvi_pages = 0;
extern FILE *tuo_file;

/* Internal variables */
static FILE *dvi_file = NULL;
static SIGNED_QUAD dvi_filesize;
static SIGNED_QUAD post_location, dvi_location, *page_location = NULL;
static int current_page;

#define DVI_MAX_STACK_DEPTH 256
static int dvi_stack_depth = 0;
static struct dvi_register {
  SIGNED_QUAD h, v, w, x, y, z, d;
} dvi_state, dvi_stack[DVI_MAX_STACK_DEPTH];

static int dvi_current_font = -1, num_dvi_fonts = 0, max_dvi_fonts = 0;
static struct dvi_font {
  SIGNED_QUAD size, id; int tfm_id, used; char *name;
} *dvi_fonts = NULL;

static SIGNED_QUAD numerator, denominator, mag, max_v, max_h;
static double tfm_conv, true_conv, conv, resolution = 300.0;
static SIGNED_PAIR max_s, total_pages;

/* Internal buffer */
#ifdef DEBUG
#define DVI_BUFFER_SIZE 1024
static int dvi_buffer_len = 0;
static unsigned char dvi_buffer[DVI_BUFFER_SIZE];
#endif

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

  if (b0 == 0) return result;
  else if (b0 == 255) return (result - alpha);

  fprintf(stderr, "[fatal] sqxfw(): TFM file is bad.\n");
  exit(1);
}

static UNSIGNED_BYTE get_unsigned_byte (void)
{
#ifdef DEBUG
  dvi_location++;
#endif
  return (UNSIGNED_BYTE)(fgetc(dvi_file) & 0xFF);
}

static SIGNED_BYTE get_signed_byte (void)
{
  register UNSIGNED_BYTE byte = get_unsigned_byte();
  return (SIGNED_BYTE)(byte & 0x80 ? byte - 0x100 : byte);
}

static UNSIGNED_PAIR get_unsigned_pair (void)
{
  register UNSIGNED_PAIR pair = get_unsigned_byte();
  /* Read the second byte */
  pair = pair << 8; pair += get_unsigned_byte();
  return pair;
}

static SIGNED_PAIR get_signed_pair (void)
{
  register UNSIGNED_PAIR pair;
  register UNSIGNED_BYTE byte = get_unsigned_byte();
  pair = (byte & 0x80 ? byte - 0x100 : byte);
  /* Read the second byte */
  pair = pair << 8; pair += get_unsigned_byte();
  return (SIGNED_PAIR)pair;
}

static UNSIGNED_TRIPLE get_unsigned_triple (void)
{
  register UNSIGNED_TRIPLE triple = get_unsigned_byte();
  /* Read the second, the third byte */
  triple = triple << 8; triple += get_unsigned_byte();
  triple = triple << 8; triple += get_unsigned_byte();
  return triple;
}

static SIGNED_TRIPLE get_signed_triple (void)
{
  register UNSIGNED_TRIPLE triple;
  /* Read the first byte and check the sign */
  register UNSIGNED_BYTE byte = get_unsigned_byte();
  triple = (byte & 0x80 ? byte - 0x100 : byte);
  /* Read the second, the third byte */
  triple = triple << 8; triple += get_unsigned_byte();
  triple = triple << 8; triple += get_unsigned_byte();
  return (SIGNED_TRIPLE)triple;
}

static UNSIGNED_QUAD get_unsigned_quad (void)
{
  register UNSIGNED_QUAD quad = get_unsigned_byte();
  /* Read the second, the third, and the fourth byte */
  quad = quad << 8; quad += get_unsigned_byte();
  quad = quad << 8; quad += get_unsigned_byte();
  quad = quad << 8; quad += get_unsigned_byte();
  return quad;
}

static SIGNED_QUAD get_signed_quad (void)
{
  register UNSIGNED_QUAD quad;
  /* Read the first byte and check the sign */
  register UNSIGNED_BYTE byte = get_unsigned_byte();
  quad = (byte & 0x80 ? byte - 0x100 : byte);
  /* Read the second, the third, and the fourth byte */
  quad = quad << 8; quad += get_unsigned_byte();
  quad = quad << 8; quad += get_unsigned_byte();
  quad = quad << 8; quad += get_unsigned_byte();
  return (SIGNED_QUAD)quad;
}

static void dvi_clear_state (void)
{
  dvi_state.h = 0; dvi_state.v = 0; dvi_state.w = 0;
  dvi_state.x = 0; dvi_state.y = 0; dvi_state.z = 0;
  dvi_state.d = 0; /* direction for ASCII pTeX */
  dvi_stack_depth = 0; dvi_current_font = -1;
}

static void do_space (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD x;

  switch (opcode) {
  case RIGHT1:
    x = get_signed_byte(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: right1 %ld ", dvi_location-1, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case RIGHT2:
    x = get_signed_pair(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: right2 %ld ", dvi_location-2, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case RIGHT3:
    x = get_signed_triple(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: right3 %ld ", dvi_location-3, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case RIGHT4:
    x = get_signed_quad(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: right4 %ld ", dvi_location-4, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case W0:
    x = dvi_state.w;
#ifdef DEBUG
    fprintf(stderr, "%ld: w0 %ld ", dvi_location, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case W1:
    x = dvi_state.w = get_signed_byte(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: w1 %ld ", dvi_location-1, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case W2:
    x = dvi_state.w = get_signed_pair(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: w2 %ld ", dvi_location-2, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case W3:
    x = dvi_state.w = get_signed_triple(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: w3 %ld ", dvi_location-3, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case W4:
    x = dvi_state.w = get_signed_quad(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: w4 %ld ", dvi_location-4, x);
    dvi_buffer[dvi_buffer_len++] = ' ';
#endif
    break;
  case X0:
    x = dvi_state.x;
#ifdef DEBUG
    fprintf(stderr, "%ld: x0 %ld ", dvi_location, x);
#endif
    break;
  case X1:
    x = dvi_state.x = get_signed_byte(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: x1 %ld ", dvi_location-1, x);
#endif
    break;
  case X2:
    x = dvi_state.x = get_signed_pair(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: x2 %ld ", dvi_location-2, x);
#endif
    break;
  case X3:
    x = dvi_state.x = get_signed_triple(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: x3 %ld ", dvi_location-3, x);
#endif
    break;
  case X4:
    x = dvi_state.x = get_signed_quad(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: x4 %ld ", dvi_location-4, x);
#endif
    break;
  }

  if (!dvi_state.d) {
#ifdef DEBUG
    fprintf(stderr, "h:=%ld+%ld=%ld, hh:=xx\n", dvi_state.h, x, dvi_state.h+x);
#endif
    dvi_state.h += x;
  } else {
#ifdef DEBUG
    fprintf(stderr, "v:=%ld+%ld=%ld, vv:=xx\n", dvi_state.v, x, dvi_state.v+x);
#endif
    dvi_state.v += x;
  }
}

static void do_vmove (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD y;

  switch (opcode) {
  case DOWN1:
    y = get_signed_byte(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: down1 %ld ", dvi_location-1, y);
#endif
    break;
  case DOWN2:
    y = get_signed_pair(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: down2 %ld ", dvi_location-2, y);
#endif
    break;
  case DOWN3:
    y = get_signed_triple(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: down3 %ld ", dvi_location-3, y);
#endif
    break;
  case DOWN4:
    y = get_signed_quad(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: down4 %ld ", dvi_location-4, y);
#endif
    break;
  case Y0:
    y = dvi_state.y;
#ifdef DEBUG
    fprintf(stderr, "%ld: y0 %ld ", dvi_location, y);
#endif
    break;
  case Y1:
    y = dvi_state.y = get_signed_byte(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: y1 %ld ", dvi_location-1, y);
#endif
    break;
  case Y2:
    y = dvi_state.y = get_signed_pair(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: y2 %ld ", dvi_location-2, y);
#endif
    break;
  case Y3:
    y = dvi_state.y = get_signed_triple(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: y3 %ld ", dvi_location-3, y);
#endif
    break;
  case Y4:
    y = dvi_state.y = get_signed_quad(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: y4 %ld ", dvi_location-4, y);
#endif
    break;
  case Z0:
    y = dvi_state.z;
#ifdef DEBUG
    fprintf(stderr, "%ld: z0 %ld ", dvi_location, y);
#endif
    break;
  case Z1:
    y = dvi_state.z = get_signed_byte(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: z1 %ld ", dvi_location-1, y);
#endif
    break;
  case Z2:
    y = dvi_state.z = get_signed_pair(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: z2 %ld ", dvi_location-2, y);
#endif
    break;
  case Z3:
    y = dvi_state.z = get_signed_triple(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: z3 %ld ", dvi_location-3, y);
#endif
    break;
  case Z4:
    y = dvi_state.z = get_signed_quad(); 
#ifdef DEBUG
    fprintf(stderr, "%ld: z4 %ld ", dvi_location-4, y);
#endif
    break;
  }

  if (!dvi_state.d) {
#ifdef DEBUG
    fprintf(stderr, "v:=%ld+%ld=%ld, vv:=xx\n", dvi_state.v, y, dvi_state.v+y);
#endif
    dvi_state.v += y;
  } else {
#ifdef DEBUG
    fprintf(stderr, "h:=%ld-%ld=%ld, hh:=xx\n", dvi_state.h, y, dvi_state.h-y);
#endif
    dvi_state.h -= y;
  }
}

static void do_rule (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD width, height;

  switch (opcode) {
  case SET_RULE:
#ifdef DEBUG
    fprintf(stderr, "%ld: setrule ", dvi_location);
#endif
    break;
  case PUT_RULE:
#ifdef DEBUG
    fprintf(stderr, "%ld: putrule ", dvi_location);
#endif
    break;
  }

  height = get_signed_quad(); width = get_signed_quad();
#ifdef DEBUG
  fprintf(stderr, "height %ld width %ld ", height, width);
#endif

  /*
   * SET_RULE: Typeset a solid black rectangle of height |a| and width |b|,
   * with its bottom left corner at |(h,v)|. Then set |h:=h+b|. If either
   * |a<=0| or |b<=0|, nothing should be typeset. Note that if |b<0|, the
   * value of |h| will decrease even though nothing else happens.
   */
  if (opcode == SET_RULE) {
    dvi_state.h += width;
#ifdef DEBUG
    fprintf(stderr, "h:=%ld+%ld=%ld, hh:=xx\n", dvi_state.h, width, dvi_state.h+width);
#endif
  }
}

static void do_set (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD ch, width;
  struct dvi_font *fnt;

  switch (opcode) {
  case SET1:
    ch = get_unsigned_byte();
#ifdef DEBUG
    fprintf(stderr, "%ld: set1 %ld ", dvi_location-1, ch);
#endif
    break;
  case SET2:
    ch = get_unsigned_pair();
#ifdef DEBUG
    fprintf(stderr, "%ld: set2 %ld ", dvi_location-2, ch);
#endif
    break;
  case SET3:
    ch = get_unsigned_triple();
#ifdef DEBUG
    fprintf(stderr, "%ld: set3 %ld ", dvi_location-3, ch);
#endif
    break;
  case SET4:
    ch = get_signed_quad();
#ifdef DEBUG
    fprintf(stderr, "%ld: set4 %ld ", dvi_location-4, ch);
#endif
    break;
  }

  if (dvi_current_font < 0) {
    fprintf(stderr, "[fatal] dvi_set(): No font selected.\n");
    exit(1);
  }

  fnt = dvi_fonts + dvi_current_font;
  width = sqxfw(fnt->size, tfm_get_fw_width(fnt->tfm_id, ch));

  if (!dvi_state.d) {
#ifdef DEBUG
    fprintf(stderr, "h:=%ld+%ld=%ld, hh:=xx\n", dvi_state.h, width, dvi_state.h+width);
#endif
    dvi_state.h += width;
  } else {
#ifdef DEBUG
    fprintf(stderr, "v:=%ld+%ld=%ld, vv:=xx\n", dvi_state.v, width, dvi_state.v+width);
#endif
    dvi_state.v += width;
  }
}

static void do_put (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD ch;

  switch (opcode) {
  case PUT1:
    ch = get_unsigned_byte();
#ifdef DEBUG
    fprintf(stderr, "%ld: put1 %ld ", dvi_location-1, ch);
#endif
    break;
  case PUT2:
    ch = get_unsigned_pair();
#ifdef DEBUG
    fprintf(stderr, "%ld: put2 %ld ", dvi_location-2, ch);
#endif
    break;
  case PUT3:
    ch = get_unsigned_triple();
#ifdef DEBUG
    fprintf(stderr, "%ld: put3 %ld ", dvi_location-3, ch);
#endif
    break;
  case PUT4:
    ch = get_signed_quad();
#ifdef DEBUG
    fprintf(stderr, "%ld: put4 %ld ", dvi_location-4, ch);
#endif
    break;
  }
}

static void do_push (void) 
{
  if (dvi_stack_depth < DVI_MAX_STACK_DEPTH)
    dvi_stack[dvi_stack_depth++] = dvi_state;
  else {
    fprintf(stderr, "[fatal] do_push(): DVI stack exceeded.\n");
    exit(1);
  }
#ifdef DEBUG
  fprintf(stderr, "%ld: push\nlevel %d:(h=%ld,v=%ld,w=%ld,x=%ld,y=%ld,z=%ld,hh=xx,vv=xx)\n", dvi_location, dvi_stack_depth-1, dvi_state.h, dvi_state.v, dvi_state.w, dvi_state.x, dvi_state.y, dvi_state.z);
#endif
}

static void do_pop (void)
{
  if (dvi_stack_depth > 0)
    dvi_state = dvi_stack[--dvi_stack_depth];
  else {
    fprintf(stderr, "[fatal] do_pop(): Tried to pop an empty stack.\n");
    exit(1);
  }
#ifdef DEBUG
  if (dvi_buffer_len > 0) {
    dvi_buffer[dvi_buffer_len] = 0;
    fprintf(stderr, "[%s]\n", dvi_buffer);
    dvi_buffer_len = 0;
  }
  fprintf(stderr, "%ld: pop\nlevel %d:(h=%ld,v=%ld,w=%ld,x=%ld,y=%ld,z=%ld,hh=xx,vv=xx)\n", dvi_location, dvi_stack_depth, dvi_state.h, dvi_state.v, dvi_state.w, dvi_state.x, dvi_state.y, dvi_state.z);
#endif
}

static void do_fnt_num (UNSIGNED_BYTE opcode)
{
  register int i;
  SIGNED_QUAD id;

  if (opcode >= FNT_NUM_0 && opcode <= FNT_NUM_63) {
    id = opcode - FNT_NUM_0;
#ifdef DEBUG
    fprintf(stderr, "%ld: fntnum%d ", dvi_location, id);
#endif
  } else switch (opcode) {
    case FNT1:
      id = get_unsigned_byte();
#ifdef DEBUG
      fprintf(stderr, "%ld: fnt1 %ld ", dvi_location-1, id);
#endif
      break;
    case FNT2:
      id = get_unsigned_pair();
#ifdef DEBUG
      fprintf(stderr, "%ld: fnt2 %ld ", dvi_location-2, id);
#endif
      break;
  case FNT3:
      id = get_unsigned_triple();
#ifdef DEBUG
      fprintf(stderr, "%ld: fnt3 %ld ", dvi_location-3, id);
#endif
      break;
  case FNT4:
      id = get_signed_quad();
#ifdef DEBUG
      fprintf(stderr, "%ld: fnt4 %ld ", dvi_location-4, id);
#endif
      break;
    }

  for (i = 0; i < num_dvi_fonts; i++)
    if (dvi_fonts[i].id == id) break;

  if (i == num_dvi_fonts) {
    fprintf(stderr, "[fatal] do_fnt_num(): Tried to select a font (ID %ld) that hasn't been defined.\n", id);
    exit(1);
  }

#ifdef DEBUG
  fprintf(stderr, "current font is %s\n", dvi_fonts[i].name);
#endif

  if (!dvi_fonts[i].used) {
    dvi_fonts[i].tfm_id = tfm_open(dvi_fonts[i].name, 1);
    dvi_fonts[i].used = 1;
  }

  dvi_current_font = i;
}

static void do_setchar (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD width;
  struct dvi_font *fnt;
 
  if (dvi_current_font < 0) {
    fprintf(stderr, "[fatal] dvi_string(): No font selected.\n");
    exit(1);
  }

  fnt = dvi_fonts + dvi_current_font;
  width = tfm_get_fw_width(fnt->tfm_id, opcode-SET_CHAR_0);
  width = sqxfw(fnt->size, width);

#ifdef DEBUG
  fprintf(stderr, "%ld: setchar%d ", dvi_location, opcode-SET_CHAR_0);
#endif
  if (!dvi_state.d) {
#ifdef DEBUG
    fprintf(stderr, "h:=%ld+%ld=%ld, hh:=xx\n", dvi_state.h, width, dvi_state.h+width);
#endif
    dvi_state.h += width;
  } else {
#ifdef DEBUG
    fprintf(stderr, "v:=%ld+%ld=%ld, vv:=xx\n", dvi_state.v, width, dvi_state.v+width);
#endif
    dvi_state.v += width;
  }
#ifdef DEBUG
  dvi_buffer[dvi_buffer_len++] = opcode - SET_CHAR_0;
#endif
}

static void do_pos_special (unsigned char *buffer, SIGNED_QUAD buffer_len)
{
  unsigned char *cmd, *ident, *p;
  SIGNED_QUAD x, y, w, h, d, list;

  for (p = buffer; p - buffer < buffer_len && isspace(*p); p++)
    ; /* skip white chars */
  if (strncmp(p, "pos:", 4)) return;

  for (cmd = p; p - buffer < buffer_len && !isspace(*p); p++)
    ; /* retrieve POS command */

  for (*p++ = 0; p - buffer < buffer_len && isspace(*p); p++)
    ; /* skip white chars */

  if (*p == '"') p++;
  for (ident = p; p - buffer < buffer_len && !isspace(*p); p++)
    ; /* retrieve POS identification */
  if (*(p-1) == '"') *(p-1) = 0; else *p = 0;

  for (p++; p - buffer < buffer_len && isspace(*p); p++)
    ; /* skip white chars */
  if (p - buffer < buffer_len)
    sscanf(p, "%ld %ld %ld %ld", &w, &h, &d, &list);

  x = dvi_state.h + denominator / 100;
  y = max_v - dvi_state.v;

  if (strcmp(cmd, "pos:pxy") == 0)
    fprintf(tuo_file, "\\pospxy{%s}{%d}{%ld}{%ld}\n", ident, current_page, x, y);
  else if (strcmp(cmd, "pos:pxywhd") == 0)
    fprintf(tuo_file, "\\pospxywhd{%s}{%d}{%ld}{%ld}{%ld}{%ld}{%ld}\n", ident, current_page, x, y, w, h, d);
  else if (strcmp(cmd, "pos:pxyplus") == 0)
    fprintf(tuo_file, "\\pospxyplus{%s}{%d}{%ld}{%ld}{%ld}{%ld}{%ld}{%ld}\n", ident, current_page, x, y, w, h, d, list);
}

static void do_xxx (UNSIGNED_BYTE opcode)
{
  SIGNED_QUAD size;
  unsigned char *sp_buf;

#ifdef DEBUG
  if (dvi_buffer_len > 0) {
    dvi_buffer[dvi_buffer_len] = 0;
    fprintf(stderr, "[%s]\n", dvi_buffer);
    dvi_buffer_len = 0;
  }
#endif

  switch (opcode) {
  case XXX1:
    size = get_unsigned_byte();
#ifdef DEBUG
    fprintf(stderr, "%ld: xxx ", dvi_location-1);
#endif
    break;
  case XXX2:
    size = get_unsigned_pair();
#ifdef DEBUG
    fprintf(stderr, "%ld: xxx ", dvi_location-2);
#endif
    break;
  case XXX3:
    size = get_unsigned_triple();
#ifdef DEBUG
    fprintf(stderr, "%ld: xxx ", dvi_location-3);
#endif
    break;
  case XXX4:
    size = get_signed_quad();
#ifdef DEBUG
    fprintf(stderr, "%ld: xxx ", dvi_location-4);
#endif
    break;
  }

  sp_buf = (unsigned char *)calloc(size+1, sizeof(unsigned char));
  if (fread(sp_buf, sizeof(unsigned char), size, dvi_file) != size) {
    fprintf(stderr, "[fatal] do_xxx(): Failed to read the special commands with size %d.\n", size);
    exit(1);
  }

#ifdef DEBUG
  fprintf(stderr, "'%s'\n", sp_buf);
  dvi_location += size;
#endif

  do_pos_special(sp_buf, size); /* defined in dvispec.c */

  free(sp_buf);
}

static void find_post_opcode (void)
{
  UNSIGNED_BYTE opcode;
  SIGNED_QUAD current_location;

  /* Calculate the file size of the DVI file */
  fseek(dvi_file, 0L, SEEK_END);
  dvi_filesize = current_location = ftell(dvi_file);
  rewind(dvi_file);
 
  /* Scan backwards through PADDING */  
  do {
    current_location--;
    fseek(dvi_file, current_location, SEEK_SET);
  } while((opcode = get_unsigned_byte()) == PADDING && current_location > 0);

  /* current_location now points to last non padding character or beginning of file */
  if (dvi_filesize - current_location < 4 || current_location == 0 ||
      !(opcode == DVI_ID || opcode == DVIV_ID)) {
    fprintf(stderr, "[fatal] Invalid signature: (%d).\n", opcode);
    exit(1);
  } 

  current_location -= 5;
  fseek(dvi_file, current_location, SEEK_SET);
  if ((opcode = get_unsigned_byte()) != POST_POST) {
     fprintf (stderr, "[fatal] Found (%d) where POST_POST should be.\n", opcode);
     exit(1);
  }

  current_location = get_signed_quad();
  fseek(dvi_file, current_location, SEEK_SET);
  if ((opcode = get_unsigned_byte()) != POST) {
     fprintf (stderr, "[fatal] Found (%d) where POST should be\n", opcode);
     exit(1);
  }

  post_location = current_location;
}

/*
 * The last page in a DVI file is followed by `post'; this command
 * introduces the postamble, which summarizes important facts that TeX has
 * accumulated about the file, making it possible to print subsets of the data
 * with reasonable efficiency. The postamble has the form
 *
 * post p[4] num[4] den[4] mag[4] l[4] u[4] s[2] t[2]
 * (font definitions)
 * post_post q[4] i[1] 223's[>=4]
 *
 * Here p is a pointer to the final bop in the file. The next three
 * parameters, num, den, and mag, are duplicates of the quantities that
 * appeared in the preamble.
 */


static void read_dvi_info (void)
{
#ifdef DEBUG
  fprintf(stderr, "Options selected:  Starting page = *\n  Maximum number of pages = 1000000\n  Output level = 4 (the works)\n  Resolution = %g\n", resolution);
#endif

  /* Skip the first five bytes; post and p[4] */
  fseek(dvi_file, post_location+5, SEEK_SET);

  if ((numerator = get_signed_quad()) <= 0) {
    fprintf(stderr, "Bad DVI file: numerator is %d!\n", numerator);
    exit(1);
  }

  if ((denominator = get_signed_quad()) <= 0) {
    fprintf(stderr, "Bad DVI file: denominator is %d!\n", denominator);
    exit(1);
  }

#ifdef DEBUG
  fprintf(stderr, "numerator/denomination=%ld/%ld\n", numerator, denominator);
#endif

  if ((mag = get_signed_quad()) <= 0) {
    fprintf(stderr, "Bad DVI file: magnification is %d!\n", mag);
    exit(1);
  }

  tfm_conv = ((double)25400000.0/numerator)*(denominator/(double)473628672)/16.0;
  true_conv = (numerator/(double)254000.0)*(resolution/denominator);
  conv = true_conv*(mag/(double)1000.0);

#ifdef DEBUG
  fprintf(stderr, "magnification=%ld; %17.8f pixels per DVI unit\n", mag, conv);
  fprintf(stderr, "Postamble start at byte %ld.\n", post_location);
#endif

  max_v = get_signed_quad();
  max_h = get_signed_quad();
  if ((max_s = get_signed_pair()) > DVI_MAX_STACK_DEPTH) {
    fprintf(stderr, "[fatal] read_dvi_info(): maxstackdepth %d exceeds DVI_MAX_STACK_DEPTH %d\n", max_s, DVI_MAX_STACK_DEPTH);
    exit(1);
  }
  total_pages = get_signed_pair();

#ifdef DEUBG
  fprintf(stderr, "maxv=%ld, maxh=%ld, maxstackdepth=%d, totalpages=%d\n",
    max_v, max_h, max_s, total_pages);
#endif
}

static void read_dvi_comment (void)
{
  static char dvi_comment[257];
  fseek(dvi_file, 14, SEEK_SET);
  dvi_comment[fread(dvi_comment, sizeof(char), get_unsigned_byte(), dvi_file)] = 0;
#ifdef DEBUG
  fprintf(stderr, "'%s'\n", dvi_comment);
#endif
}

static void find_page_location (void) 
{
  register int i;

  /* Read the total page number */
  fseek(dvi_file, post_location+27, SEEK_SET);
  if ((dvi_pages = get_unsigned_pair()) == 0) {
    fprintf(stderr, "[fatal] Total page number is zero.\n");
    exit(1);
  }

  /* Read the location of each page */
  page_location = (SIGNED_QUAD *)calloc(dvi_pages, sizeof(SIGNED_QUAD));
  fseek(dvi_file, post_location+1, SEEK_SET);
  page_location[dvi_pages-1] = get_signed_quad();
  if (page_location[dvi_pages-1] + 41 > dvi_filesize) {
    fprintf(stderr, "[fatal] The location of the page %d was broken.\n", dvi_pages);
    exit(1);
  }
  for (i = dvi_pages-2; i >= 0; i--) {
    fseek(dvi_file, page_location[i+1] + 41, SEEK_SET);
    page_location[i] = get_signed_quad();
    if (page_location[i] + 41 > dvi_filesize) {
      fprintf(stderr, "[fatal] The location of the page %d was broken.\n", i+1);
      exit(1);
    }
  }
}

static void read_dvi_fonts (void)
{
  UNSIGNED_BYTE opcode;
  SIGNED_QUAD id, size;

  int dir_len, name_len;
  char *dir, *name;

  fseek(dvi_file, post_location+29, SEEK_SET);

  while ((opcode = get_unsigned_byte()) != POST_POST) {

    switch (opcode) {
      case FNT_DEF1: id = get_unsigned_byte(); break;
      case FNT_DEF2: id = get_unsigned_pair(); break;
      case FNT_DEF3: id = get_unsigned_triple(); break;
      case FNT_DEF4: id = get_signed_quad(); break;
      default:
	fprintf(stderr, "[fatal] read_dvi_fonts(): Unexpected opcode (%ld).\n", opcode);
        exit(1);
    }

    /* Retrieve a font record */
    if (num_dvi_fonts >= max_dvi_fonts) {
      max_dvi_fonts += MAX_FONTS_STEP;
      dvi_fonts = (struct dvi_font *)realloc(dvi_fonts, max_dvi_fonts * sizeof(struct dvi_font));
    }

    get_signed_quad(); /* checksum */
    size = get_signed_quad(); /* scale factor */
    get_signed_quad(); /* designed size */

    dir_len = get_unsigned_byte();
    name_len = get_unsigned_byte();

    /* Retrieve the directory name and check the size and then discard it */
    dir = (char *)calloc(dir_len+1, sizeof(char));
    if (fread(dir, sizeof(char), dir_len, dvi_file) != dir_len) {
      fprintf(stderr, "[fatal] read_dvi_fonts(): Failed to retrieve a directory name.\n");
      exit(1);
    }
    free(dir);

    /* Retrieve the font name */
    name = (char *)calloc(name_len+1, sizeof(char));
    if (fread(name, sizeof(char), name_len, dvi_file) != name_len) {
      fprintf(stderr, "[fatal] read_dvi_fonts(): Failed to retrieve a font name.\n");
      exit(1);
    }
    name[name_len] = 0;

#ifdef DEBUG
    fprintf(stderr, "Font %d: %s---loaded at size %ld DVI units\n", id, name, size);
#endif
    dvi_fonts[num_dvi_fonts].name = name;
    dvi_fonts[num_dvi_fonts].size = size;
    dvi_fonts[num_dvi_fonts].id = id;
    dvi_fonts[num_dvi_fonts].used = 0;
    num_dvi_fonts++;
  }
}

static void do_bop (void)
{
  register int i;
#ifdef DEBUG
  fprintf(stderr, "\n%ld: beginning of page %d\n", dvi_location, current_page);
#endif
  for (i = 0; i < 11; i++) get_signed_quad();
  dvi_clear_state();
}

static void do_eop (void)
{
#ifdef DEBUG
  fprintf(stderr, "%ld: eop\n", dvi_location);
#endif
}

static void do_dir (void)
{
  dvi_state.d = get_unsigned_byte();
#ifdef DEBUG
  fprintf(stderr, "%ld: dir %d\n", dvi_location-1, dvi_state.d);
#endif
}

static void do_fnt_def (UNSIGNED_BYTE opcode)
{
  register int len;
  SIGNED_QUAD id;
  char *fnt_buf;

#ifdef DEBUG
  if (dvi_buffer_len > 0) {
    dvi_buffer[dvi_buffer_len] = 0;
    fprintf(stderr, "[%s]\n", dvi_buffer);
    dvi_buffer_len = 0;
  }
#endif

  switch (opcode) {
  case FNT_DEF1:
    id = get_unsigned_byte();
#ifdef DEBUG
    fprintf(stderr, "%ld: fntdef1 %ld: ", dvi_location-1, id);
#endif
    break;
  case FNT_DEF2:
    id = get_unsigned_pair();
#ifdef DEBUG
    fprintf(stderr, "%ld: fntdef2 %ld: ", dvi_location-2, id);
#endif
    break;
  case FNT_DEF3:
    id = get_unsigned_triple();
#ifdef DEBUG
    fprintf(stderr, "%ld: fntdef3 %ld: ", dvi_location-3, id);
#endif
    break;
  case FNT_DEF4:
    id = get_signed_quad();
#ifdef DEBUG
    fprintf(stderr, "%ld: fntdef4 %ld: ", dvi_location-4, id);
#endif
    break;
  }
    
  /* the checksum that TeX found in the TFM file for this font */
  get_signed_quad();
  /* a fixed-point scale factor */
  get_signed_quad();
  /* the "design size" */
  get_signed_quad();
  /* the length of the "area" or directory */
  len = get_unsigned_byte();
  /* the length of the font name itself */
  len += get_unsigned_byte();

  /* the external name of the font */
  fnt_buf = (char *)calloc(len+1, sizeof(char));
  len = fread(fnt_buf, sizeof(char), len, dvi_file);
  fnt_buf[len] = 0;
#ifdef DEBUG
  fprintf(stderr, "%s\n", fnt_buf);
  dvi_location += len;
#endif
}

void dvi_init (char *filename)
{
  /* Open the input DVI file */
  if ((dvi_file = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "[fatal] Could not open %s.", filename);
    exit(1);
  }
  /* Find the post opcode */
  find_post_opcode();
  read_dvi_info();
  find_page_location();
  read_dvi_comment();
  read_dvi_fonts();
  dvi_clear_state();
}

void dvi_close (void)
{
  register int i;
  tfm_close_all();
  fclose(dvi_file);
  free(page_location);
  for (i = 0; i < num_dvi_fonts; i++) free(dvi_fonts[i].name);
  if (dvi_fonts) free(dvi_fonts);
}


void dvi_do_page (int n)
{
#ifdef DEBUG
  dvi_location = page_location[n] - 1;
#endif

  current_page = n + 1;

  /* Position to beginning of page */
  fseek(dvi_file, page_location[n], SEEK_SET);

  dvi_clear_state();

  for (;;) { /* until finding EOP opcode */
    UNSIGNED_BYTE opcode = get_unsigned_byte();

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
    case NOP: break;
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
      fprintf(stderr, "[fatal] Unexpected opcode (%d).", opcode);
      exit(1);
    }
  }
}

