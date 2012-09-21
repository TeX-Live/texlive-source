/*  
    
    This is dvipdfmx, an eXtended version of dvipdfm by Mark A. Wicks.

    Copyright (C) 2002-2012 by Jin-Hwan Cho and Shunsaku Hirata,
    the dvipdfmx project team.
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

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

#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "mem.h"
#include "error.h"

#include "dpxfile.h"

#include "pdfparse.h"

#include "pdfdoc.h"

#include "mpost.h"

#include "pdfximage.h"
#include "pdfdraw.h"
#include "pdfcolor.h"
#include "pdfdev.h"

#include "specials.h"
#include "spc_util.h"

#include "spc_dvips.h"


static int    block_pending = 0;
static double pending_x     = 0.0;
static double pending_y     = 0.0;
static int    position_set  = 0;

static char *
parse_filename (const char **pp, const char *endptr)
{
  char  *r;
  const char *q = NULL, *p = *pp;
  char   qchar;
  int    n;

  if (!p || p >= endptr)
    return  NULL;
  else if (*p == '\"' || *p == '\'')
    qchar = *p++;
  else {
    qchar = ' ';
  }
  for (n = 0, q = p; p < endptr && *p != qchar; n++, p++);
  if (qchar != ' ') {
    if (*p != qchar)
      return  NULL;
    p++;
  }
  if (!q || n == 0)
    return  NULL;

#if  0
  {
    int  i;
    for (i = 0; i < n && isprint(q[i]); i++);
    if (i != n) {
      WARN("Non printable char in filename string...");
    }
  }
#endif

  r = NEW(n + 1, char);
  memcpy(r, q, n); r[n] = '\0';

  *pp = p;
  return  r;
}

/* =filename ... */
static int
spc_handler_ps_file (struct spc_env *spe, struct spc_arg *args)
{
  int            form_id;
  char          *filename;
  transform_info ti;

  ASSERT(spe && args);

  skip_white(&args->curptr, args->endptr);
  if (args->curptr + 1 >= args->endptr ||
      args->curptr[0] != '=') {
    spc_warn(spe, "No filename specified for PSfile special.");
    return  -1;
  }
  args->curptr++;

  filename = parse_filename(&args->curptr, args->endptr);
  if (!filename) {
    spc_warn(spe, "No filename specified for PSfile special.");
    return  -1;
  }

  transform_info_clear(&ti);
  if (spc_util_read_dimtrns(spe, &ti, args, NULL, 1) < 0) {
    RELEASE(filename);
    return  -1;
  }

  form_id = pdf_ximage_findresource(filename, 1, NULL);
  if (form_id < 0) {
    spc_warn(spe, "Failed to read image file: %s", filename);
    RELEASE(filename);
    return  -1;
  }
  RELEASE(filename);

  pdf_dev_put_image(form_id, &ti, spe->x_user, spe->y_user);

  return  0;
}

/* This isn't correct implementation but dvipdfm supports... */
static int
spc_handler_ps_plotfile (struct spc_env *spe, struct spc_arg *args)
{
  int            error = 0;
  int            form_id;
  char          *filename;
  transform_info p;

  ASSERT(spe && args);

  spc_warn(spe, "\"ps: plotfile\" found (not properly implemented)");

  skip_white(&args->curptr, args->endptr);
  filename = parse_filename(&args->curptr, args->endptr);
  if (!filename) {
    spc_warn(spe, "Expecting filename but not found...");
    return -1;
  }

  form_id = pdf_ximage_findresource(filename, 1, NULL);
  if (form_id < 0) {
    spc_warn(spe, "Could not open PS file: %s", filename);
    error = -1;
  } else {
    transform_info_clear(&p);
    p.matrix.d = -1.0; /* xscale = 1.0, yscale = -1.0 */
#if 0
    /* I don't know how to treat this... */
    pdf_dev_put_image(form_id, &p,
		      block_pending ? pending_x : spe->x_user,
		      block_pending ? pending_y : spe->y_user);
#endif
    pdf_dev_put_image(form_id, &p, 0, 0);
  }
  RELEASE(filename);

  return  error;
}

static int
spc_handler_ps_literal (struct spc_env *spe, struct spc_arg *args)
{
  int     error = 0;
  int     st_depth, gs_depth;
  double  x_user, y_user;

  ASSERT(spe && args && args->curptr <= args->endptr);

  if (args->curptr + strlen(":[begin]") <= args->endptr &&
      !strncmp(args->curptr, ":[begin]", strlen(":[begin]"))) {
    block_pending++;
    position_set = 1;

    x_user = pending_x = spe->x_user;
    y_user = pending_y = spe->y_user;
    args->curptr += strlen(":[begin]");
  } else if (args->curptr + strlen(":[end]") <= args->endptr &&
	     !strncmp(args->curptr, ":[end]", strlen(":[end]"))) {
    if (block_pending <= 0) {
      spc_warn(spe, "No corresponding ::[begin] found.");
      return -1;
    }
    block_pending--;

    position_set = 0;

    x_user = pending_x;
    y_user = pending_y;
    args->curptr += strlen(":[end]");
  } else if (args->curptr < args->endptr &&
	     args->curptr[0] == ':') {
    x_user = position_set ? pending_x : spe->x_user;
    y_user = position_set ? pending_y : spe->y_user;
    args->curptr++;
  } else {
    position_set = 1;
    x_user = pending_x = spe->x_user;
    y_user = pending_y = spe->y_user;
  }

  skip_white(&args->curptr, args->endptr);
  if (args->curptr < args->endptr) {

    st_depth = mps_stack_depth();
    gs_depth = pdf_dev_current_depth();

    error = mps_exec_inline(&args->curptr,
			    args->endptr,
			    x_user, y_user);
    if (error) {
      spc_warn(spe, "Interpreting PS code failed!!! Output might be broken!!!");
      pdf_dev_grestore_to(gs_depth);
    } else if (st_depth != mps_stack_depth()) {
      spc_warn(spe, "Stack not empty after execution of inline PostScript code.");
      spc_warn(spe, ">> Your macro package makes some assumption on internal behaviour of DVI drivers.");
      spc_warn(spe, ">> It may not compatible with dvipdfmx.");
    }
  }

  return  error;
}

static int
spc_handler_ps_default (struct spc_env *spe, struct spc_arg *args)
{
  int  error;
  int  st_depth, gs_depth;

  ASSERT(spe && args);

  pdf_dev_gsave();

  st_depth = mps_stack_depth();
  gs_depth = pdf_dev_current_depth();

  {
    pdf_tmatrix M;
    M.a = M.d = 1.0; M.b = M.c = 0.0; M.e = spe->x_user; M.f = spe->y_user;
    pdf_dev_concat(&M);
  error = mps_exec_inline(&args->curptr,
			  args->endptr,
			  spe->x_user, spe->y_user);
    M.e = -spe->x_user; M.f = -spe->y_user;
    pdf_dev_concat(&M);
  }
  if (error)
    spc_warn(spe, "Interpreting PS code failed!!! Output might be broken!!!");
  else {
    if (st_depth != mps_stack_depth()) {
      spc_warn(spe, "Stack not empty after execution of inline PostScript code.");
      spc_warn(spe, ">> Your macro package makes some assumption on internal behaviour of DVI drivers.");
      spc_warn(spe, ">> It may not compatible with dvipdfmx.");
    }
  }

  pdf_dev_grestore_to(gs_depth);
  pdf_dev_grestore();

  return  error;
}

static struct spc_handler dvips_handlers[] = {
  {"PSfile",        spc_handler_ps_file},
  {"psfile",        spc_handler_ps_file},
  {"ps: plotfile ", spc_handler_ps_plotfile},
  {"PS: plotfile ", spc_handler_ps_plotfile},
  {"PS:",           spc_handler_ps_literal},
  {"ps:",           spc_handler_ps_literal},
  {"\" ",           spc_handler_ps_default}
};

int
spc_dvips_at_end_page (void)
{
  mps_eop_cleanup();
  return  0;
}

int
spc_dvips_check_special (const char *buf, long len)
{
  const char *p, *endptr;
  int   i;

  p      = buf;
  endptr = p + len;

  skip_white(&p, endptr);
  if (p >= endptr)
    return  0;

  len = (long) (endptr - p);
  for (i = 0;
       i < sizeof(dvips_handlers)/sizeof(struct spc_handler); i++) {
    if (len >= strlen(dvips_handlers[i].key) &&
        !memcmp(p, dvips_handlers[i].key,
                strlen(dvips_handlers[i].key))) {
      return  1;
    }
  }

  return  0;
}

int 
spc_dvips_setup_handler (struct spc_handler *handle,
			 struct spc_env *spe, struct spc_arg *args)
{
  const char *key;
  int   i, keylen;

  ASSERT(handle && spe && args);

  skip_white(&args->curptr, args->endptr);

  key = args->curptr;
  while (args->curptr < args->endptr &&
	 isalpha(args->curptr[0])) {
    args->curptr++;
  }
  /* Test for "ps:". The "ps::" special is subsumed under this case.  */
  if (args->curptr < args->endptr &&
      args->curptr[0] == ':') {
    args->curptr++;
    if (args->curptr+strlen(" plotfile ") <= args->endptr &&
	!strncmp(args->curptr, " plotfile ", strlen(" plotfile "))) {
      args->curptr += strlen(" plotfile ");
      }
  } else if (args->curptr+1 < args->endptr &&
             args->curptr[0] == '"' && args->curptr[1] == ' ') {
    args->curptr += 2;
  }

  keylen = (int) (args->curptr - key);
  if (keylen < 1) {
    spc_warn(spe, "Not ps: special???");
    return  -1;
  }

  for (i = 0;
       i < sizeof(dvips_handlers) / sizeof(struct spc_handler); i++) {
    if (keylen == strlen(dvips_handlers[i].key) &&
	!strncmp(key, dvips_handlers[i].key, keylen)) {

      skip_white(&args->curptr, args->endptr);

      args->command = dvips_handlers[i].key;

      handle->key  = "ps:";
      handle->exec = dvips_handlers[i].exec;

      return  0;
    }
  }

  return  -1;
}

