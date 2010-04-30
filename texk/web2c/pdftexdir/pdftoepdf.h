/*
Copyright (c) 1996-2008 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with pdfTeX; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef PDFTOEPDF_H
#  define PDFTOEPDF_H

extern "C" {

#include "openbsd-compat.h"

#include <kpathsea/c-auto.h>
#include <kpathsea/c-proto.h>
#include <kpathsea/lib.h>

#define KPATHSEA_CONFIG_H       /* avoid including other kpathsea header files */
                                /* from web2c/config.h */

#ifdef CONFIG_H                 /* CONFIG_H has been defined by some xpdf */
#  undef CONFIG_H               /* header file */
#endif

#include <c-auto.h>       /* define SIZEOF_LONG */
#include <config.h>       /* define type integer */

#include <pdftexdir/ptexmac.h>

#include "pdftex-common.h"
}

#endif /* PDFTOEPDF_H */
