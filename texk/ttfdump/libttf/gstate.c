/* gstate.c -- manage graphics state
 * Copyright (C) 1996 Li-Da Lho, All right reserved.
 */
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: gstate.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: gstate.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

GraphicsState gsdefault;

void ttfInitGraphicsState(TTFontPtr font)
{
    /*font->gstate = GS_DEFAULT;
    font->gstate_init = GS_DEFAULT;*/

    /*ttfExecuteInstruction(font, font->fpgm);
    ttfExecuteInstruction(font, font->prep);*/
}

void ttfInitStack(TTFontPtr font)
{
    font->vm.sp = 0;
    font->vm.Stack = (LONG *) calloc (font->maxp->maxStackElements, sizeof(LONG));
    if (font->vm.Stack == NULL)
	ttfError("Out of memory\n");
}

void ttfInitStorageArea(TTFontPtr font)
{
    font->vm.StorageArea = (LONG *) calloc (font->maxp->maxStorage, sizeof(LONG));
    if (font->vm.StorageArea == NULL)
	ttfError("Out of memory\n");
}
