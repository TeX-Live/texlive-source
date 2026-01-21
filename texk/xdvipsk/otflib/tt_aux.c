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

#include <config.h>

#include "system.h"
#include "mem.h"
#include "error.h"
#include "numbers.h"

#include "tt_aux.h"

static int always_embed = 0;

ULONG ttc_read_offset (sfnt *sfont, int ttc_idx)
{
  ULONG offset = 0, num_dirs = 0;
  
  if (sfont == NULL || sfont->stream == NULL)
    ERROR("file not opened");

  if (sfont->type != SFNT_TYPE_TTC)
    ERROR("ttc_read_offset(): invalid font type");

  sfnt_seek_set (sfont, 4); /* skip version tag */

  /* version = */ sfnt_get_ulong(sfont);
  num_dirs = sfnt_get_ulong(sfont);
  if (ttc_idx < 0 || ttc_idx > num_dirs - 1)
    ERROR("Invalid TTC index number");

  sfnt_seek_set (sfont, 12 + ttc_idx * 4);
  offset = sfnt_get_ulong (sfont);

  return offset;
}
