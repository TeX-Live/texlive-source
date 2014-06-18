 
/*
 * Copyright (C) Yves Arrouye <Yves.Arrouye@marin.fdn.fr>, 1996.
 *
 * Use under the GPL version 2. You are not allowed to remove this
 * copyright notice.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "dimen.h"

static struct {
    const char* name;
    float factor;
} units[] = {
    { "in",	1. },
    { "ft",	12. },
    { "pt",	1. / 72. },
    { "m", 	100. / 2.54 },
    { "dm",	10. / 2.54 },
    { "cm", 	1. / 2.54 },
    { "mm",	.1 / 2.54 },
    { 0 }
};

float unitfactor(const char* unit)
{
    int i;

    for (i = 0; units[i].name; ++i) {
	if (!strcasecmp(units[i].name, unit)) {
	    return units[i].factor;
	}
    }

    return 0;
}

int psdimension(const char* what, int* dim)
{
    const char* unit;
    int dot = 0;

    if (!what || !*what) return -1;

    if (*(unit = what) == '-') ++unit;

    for (; isdigit((unsigned char)*unit) || (*unit == '.' && !dot++); ++unit);

    if (*unit && !isalpha((unsigned char)*unit)) {
	return -1;
    } else {
	double base = atof(what);
	double factor = unitfactor(unit);

	if (factor) {
	    *dim = base * factor * 72;
	    return 0;
	}

	return 1;
    }
}

