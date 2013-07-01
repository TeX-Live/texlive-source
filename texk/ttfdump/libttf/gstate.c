/* gstate.c -- manage graphics state
 * Copyright (C) 1996 Li-Da Lho, All right reserved.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: gstate.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

GraphicsState gsdefault;

void ttfInitGraphicsState(TTFontPtr font)
{
#if 0
    font->gstate = GS_DEFAULT;
    font->gstate_init = GS_DEFAULT;
#endif

#if 0
    ttfExecuteInstruction(font, font->fpgm);
    ttfExecuteInstruction(font, font->prep);
#endif
}

void ttfInitStack(TTFontPtr font)
{
    font->vm.sp = 0;
    font->vm.Stack = XCALLOC (font->maxp->maxStackElements, LONG);
}

void ttfInitStorageArea(TTFontPtr font)
{
    font->vm.StorageArea = XCALLOC (font->maxp->maxStorage, LONG);
}
