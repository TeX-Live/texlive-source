
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

#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>

#include "paper.h"

struct paper {
    const char* name;
    double pswidth, psheight;
};

/* This table will be searched in order: please put typical paper sizes
   at the beginning of it. */

/* The paper names have been got from gs 3.68 gs_stadt.ps file, with
   some personal additions. */

static struct paper papers[] = {
#include "paperspecs.h"
    { 0 }
};

int paperinit(void) {
    return 0;
}

int paperdone(void) {
    return 0;
}

const char* papername(const struct paper* spaper)
{
    return spaper->name;
}

double paperpswidth(const struct paper* spaper)
{
    return spaper->pswidth;
}

double paperpsheight(const struct paper* spaper)
{
    return spaper->psheight;
}

const struct paper* paperfirst(void) {
    return papers;
}

const struct paper* paperlast(void) {
    static const struct paper* lastpaper = 0;
   
    const struct paper* next = papers;
    while (next->name) {
	lastpaper = next, ++next;
    }

    return lastpaper;
}

const struct paper* papernext(const struct paper* spaper)
{
    return (++spaper)->name ? spaper : 0;
}

const struct paper* paperprev(const struct paper* spaper)
{
    return spaper == papers ? 0 : --spaper;
}

const char* defaultpapersizefile(void) {
    return NULL;
}

const char* systempapersizefile(void) {
    return NULL;
}

const char* defaultpapername(void) {
    return PAPERSIZE;
}

char* systempapername(void) {
    char* paperstr;
    char* paperenv;
    const char* paperdef;
    const struct paper* pp;

    paperenv = getenv(PAPERSIZEVAR);
    if ((paperenv != NULL) && (strchr(paperenv, '/') != NULL)) {
        paperenv = NULL;
    }

    if (paperenv) {
        paperstr = malloc((strlen(paperenv) + 1) * sizeof(char));	
	
	if (! paperstr) return 0;
	
	if ((pp = paperinfo(paperenv)))
	    return strcpy(paperstr, pp->name);
	else
	    return strcpy(paperstr, paperenv);
    }

    paperdef = defaultpapername();
    paperstr = malloc((strlen(paperdef) + 1) * sizeof(char));
    
    if (paperstr) 
        return strcpy(paperstr, paperdef); 
    else
        return 0;
}

const struct paper* paperinfo(const char* paper)
{
    const struct paper* pp;

    for (pp = paperfirst(); pp; pp = papernext(pp)) {
	if (!strcasecmp(pp->name, paper)) {
	    return pp;
	}
    }

    return 0;
}

const struct paper* paperwithsize(double pswidth, double psheight)
{
    const struct paper* pp;

    for (pp = paperfirst(); pp; pp = papernext(pp)) {
	if (pp->pswidth == pswidth
	    && pp->psheight == psheight) {
	    return pp;
	}
    }

    return 0;
}

