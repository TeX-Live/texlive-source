/* color.c */

/************************************************************************

  Part of the dvipng distribution

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program. If not, see
  <http://www.gnu.org/licenses/>.

  Copyright (C) 2002-2008 Jan-Åke Larsson

************************************************************************/

#include "dvipng.h"
#if HAVE_ALLOCA_H
# include <alloca.h>
#endif

/*
 * Color. We delete and recreate the gdImage for each new page. This
 * means that the stack must contain rgb value not color index.
 * Besides, the current antialiasing implementation needs rgb anyway.
*/

struct colorname {
  struct colorname *next;
  char*             color;
  char              name[1];
} *dvipsnam=NULL,*svgnam=NULL,*xcolor=NULL;


void initcolor() 
{
   csp = 1;
   cstack[0].red=255; 
   cstack[0].green=255; 
   cstack[0].blue=255; 
   cstack[1].red=0; 
   cstack[1].green=0; 
   cstack[1].blue=0; 
}

struct colorname * NewColor(char* name, int nname,
	      char* model, int nmodel,
	      char* values, int nvalues)
{
  struct colorname *tmp = 
    malloc(sizeof(struct colorname)+3+nname+nmodel+nvalues);
  if (tmp==NULL) 
    Fatal("Cannot allocate space for color name");
  tmp->color=tmp->name+nname+1;
  strncpy(tmp->name,name,nname);
  tmp->name[nname]='\0';
  strncpy(tmp->color,model,nmodel);
  tmp->color[nmodel]=' ';
  strncpy(tmp->color+nmodel+1,values,nvalues);
  tmp->color[nmodel+nvalues+1]='\0';
  model=tmp->color;
  while(*model!='\0') { 
    if (*model==',') 
      *model=' '; 
    model++;
  }
  DEBUG_PRINT(DEBUG_COLOR,("\n  COLOR NAME:\t'%s' '%s'",
			   tmp->name,tmp->color)); 
  return(tmp);
}

#define FINDWORD(s) while(s<max && \
                  (*s=='{'||*s==' '||*s=='%'||*s=='\n'||*s==';')) s++
#define FINDARG(s) while(s<max && *s!='{') s++; FINDWORD(s)
#define FINDMODELEND(s,n) n=0; while(s<max && *s!='}') { s++; n++; }
#define FINDNAMEEND(s,n) n=0; while(s<max && *s!='}' && *s!=',') { s++; n++; }
#define FINDVALEND(s,n) n=0; while(s<max && *s!='}' && *s!=';') { s++; n++; }
#define BLANKCOMMAS(s) 

struct colorname* LoadColornameFile(char* filename, bool should_exist)
{
  struct colorname *list=NULL,*tmp=NULL; 
  char *filepath,*pos,*max;
  char *name,*values,*model;
  int nname,nvalues,nmodel;
  struct filemmap fmmap;

  TEMPSTR(filepath,kpse_find_file(filename,kpse_tex_format,false));
  if (filepath == NULL) {
    if (should_exist)
      Warning("color name file '%s' could not be found",filename);
    return NULL;
  }
  DEBUG_PRINT(DEBUG_COLOR,("\n  OPEN COLOR NAMES:\t'%s'", filepath));
  if (MmapFile(filepath,&fmmap)) return NULL;
  pos=fmmap.data;
  max=fmmap.data+fmmap.size;
  while (pos<max && *pos!='\\') pos++;
  while(pos+9<max && strncmp(pos,"\\endinput",9)!=0) {
    if (pos+17<max && strncmp(pos,"\\DefineNamedColor",17)==0) {
      model=NULL;
      nname=nmodel=nvalues=0;
      FINDARG(pos);             /* skip first argument */
      FINDARG(pos);             /* find second argument: color name */
      name=pos;
      FINDNAMEEND(pos,nname);
      FINDARG(pos);             /* find third argument: color model */
      model=pos;
      FINDMODELEND(pos,nmodel);
      FINDARG(pos);             /* find fourth argument: color values */
      values=pos;
      FINDVALEND(pos,nvalues);
      tmp=NewColor(name,nname,model,nmodel,values,nvalues);
      tmp->next=list;
      list=tmp;
    } else if (pos+15<max && strncmp(pos,"\\definecolorset",15)==0) {
      char *model;
      FINDARG(pos);             /* find first argument: color model */
      model=pos;
      FINDMODELEND(pos,nmodel);
      FINDARG(pos);             /* skip second argument */
      FINDARG(pos);             /* skip third argument */
      FINDARG(pos);             /* find fourth argument: names, values */
      while(pos<max && *pos!='}'){
	name=pos;
	FINDNAMEEND(pos,nname);
	pos++;
	values=pos;
	FINDVALEND(pos,nvalues);
	tmp=NewColor(name,nname,model,nmodel,values,nvalues);
	tmp->next=list;
	list=tmp;
	FINDWORD(pos);
      }
    } else {
      pos++;
      while (pos<max && *pos!='\\') pos++;
    }
  }
  UnMmapFile(&fmmap);
  return(list);
}

void ClearColorNames(void)
{
  struct colorname *tmp=dvipsnam,*next=NULL;
  while (tmp!=NULL) {
    next=tmp->next;
    free(tmp);
    tmp=next;
  }
  dvipsnam=NULL;
  tmp=svgnam;
  while (tmp!=NULL) {
    next=tmp->next;
    free(tmp);
    tmp=next;
  }
  svgnam=NULL;
  tmp=xcolor;
  while (tmp!=NULL) {
    next=tmp->next;
    free(tmp);
    tmp=next;
  }
  xcolor=NULL;
}

#define FTO255(a) ((int) (255*a+0.5))
#define WARN_IF_FAILED(a,b) if (a==b) { page_flags |= PAGE_GAVE_WARN; \
  Warning("missing color-specification value, treated as zero"); }

#define SKIPSPACES(s) while(s && *s==' ' && *s!='\0') s++
#define NEXTFLOAT255(c) FTO255(strtod(c,&end)); WARN_IF_FAILED(c,end); c=end
#define NEXTFLOAT(c) strtod(c,&end); WARN_IF_FAILED(c,end); c=end
#define NEXTINT(c) strtol(c,&end,10); WARN_IF_FAILED(c,end); c=end
#define NEXTHEX(c) strtol(c,&end,16); WARN_IF_FAILED(c,end); c=end

void stringrgb(char* color,int *r,int *g,int *b)
{
  char* end;

  DEBUG_PRINT(DEBUG_COLOR,("\n  COLOR SPEC:\t'%s' (",color));
  SKIPSPACES(color);
  if (strcmp(color,"Black")==0) {
    *r = *g = *b = 0;
  } else if (strcmp(color,"White")==0) {
    *r = *g = *b = 255;
  } else if (strncmp(color,"gray ",5)==0) {
    color+=5;
    *r = *g = *b = NEXTFLOAT255(color);
  } else if (strncmp(color,"rgb ",4)==0) {
    color+=4;
    *r = NEXTFLOAT255(color);
    *g = NEXTFLOAT255(color);
    *b = NEXTFLOAT255(color);
   } else if (strncmp(color,"Gray ",5)==0) {
    color+=5;
    *r = *g = *b = NEXTINT(color);
  } else if (strncmp(color,"RGB ",4)==0) {
    color+=4;
    *r = NEXTINT(color);
    *g = NEXTINT(color);
    *b = NEXTINT(color);
  } else if (strncmp(color,"HTML ",5)==0) {
    color+=5;
    *b = NEXTHEX(color);
    *r = *b/65536;
    *g = *b/256;
    *b -= *g*256;
    *g -= *r*256;
  } else if (strncmp(color,"cmy ",4)==0
	     || strncmp(color,"cmyk ",5)==0) {
    int c,m,y,k;
    color+=3;
    k=(*color=='k');
    if (k)
      color++;
    c = NEXTFLOAT255(color);
    m = NEXTFLOAT255(color);
    y = NEXTFLOAT255(color);
    if (k)
      k = NEXTFLOAT255(color);
    *r = c+k<255 ? 255-(c+k) : 0;
    *g = m+k<255 ? 255-(m+k) : 0;
    *b = y+k<255 ? 255-(y+k) : 0;
  } else if (strncmp(color,"hsb ",4)==0 
	     || strncmp(color,"HSB ",4)==0) {
    /* The hsb and HSB models really need more presicion. 
       Use double and convert back*/
    double hu,sa,br,f,R,G,B;
    int i;
    if (*color=='h') {
      color+=4;
      hu = NEXTFLOAT(color);
      sa = NEXTFLOAT(color);
      br = NEXTFLOAT(color);
    } else {
      color+=4;
      hu = (float)NEXTINT(color); hu /= 255;
      sa = (float)NEXTINT(color); sa /= 255;
      br = (float)NEXTINT(color); br /= 255;
    }
    i=6*hu;
    f=6*hu-i;
    switch(i) {
    case 0: 
      R = br*(1-sa*0);     G = br*(1-sa*(1-f)); B = br*(1-sa*1); break;
    case 1: 
      R = br*(1-sa*f);     G = br*(1-sa*0);     B = br*(1-sa*1); break;
    case 2: 
      R = br*(1-sa*1);     G = br*(1-sa*0);     B = br*(1-sa*(1-f)); break;
    case 3: 
      R = br*(1-sa*1);     G = br*(1-sa*f);     B = br*(1-sa*0); break;
    case 4: 
      R = br*(1-sa*(1-f)); G = br*(1-sa*1);     B = br*(1-sa*0); break;
    case 5: 
      R = br*(1-sa*0);     G = br*(1-sa*1);     B = br*(1-sa*f); break;
    default: 
      R = br*(1-sa*0);     G = br*(1-sa*1);     B = br*(1-sa*1);
    }
    *r=FTO255(R);
    *g=FTO255(G);
    *b=FTO255(B);
  } else {
    /* Model not found, probably a color name */
    struct colorname *tmp;

    if (dvipsnam==NULL) 
      dvipsnam=LoadColornameFile("dvipsnam.def",true);
    tmp=dvipsnam;
    while(tmp!=NULL && tmp->name!=NULL && strcmp(color,tmp->name)) 
      tmp=tmp->next;
    if (tmp==NULL) {
      if (svgnam==NULL) 
	svgnam=LoadColornameFile("svgnam.def",false);
      tmp=svgnam;
      while(tmp!=NULL && tmp->name!=NULL && strcmp(color,tmp->name)) 
	tmp=tmp->next;
      if (tmp==NULL) {
	if (xcolor==NULL) 
	  xcolor=LoadColornameFile("xcolor.sty",false);
	tmp=xcolor;
	while(tmp!=NULL && tmp->name!=NULL && strcmp(color,tmp->name)) 
	  tmp=tmp->next;
      }
    }
    if (tmp!=NULL) {
      /* Found: one-level recursion */
      DEBUG_PRINT(DEBUG_COLOR,("\n    ---RECURSION--- "))
      stringrgb(tmp->color,r,g,b);
    } else {
      /* Not found, warn */
      page_flags |= PAGE_GAVE_WARN;
      Warning("unimplemented color specification '%s'",color);
    }
  }
  DEBUG_PRINT(DEBUG_COLOR,("%d %d %d) ",*r,*g,*b))
}

void background(char* p)
{
  stringrgb(p, &cstack[0].red, &cstack[0].green, &cstack[0].blue);
  DEBUG_PRINT(DEBUG_COLOR,("\n  BACKGROUND:\t(%d %d %d) ",
			   cstack[0].red, cstack[0].green, cstack[0].blue));
}

void pushcolor(char * p)
{
  if ( ++csp == STACK_SIZE )
    Fatal("out of color stack space") ;
  stringrgb(p, &cstack[csp].red, &cstack[csp].green, &cstack[csp].blue);
  DEBUG_PRINT(DEBUG_COLOR,("\n  COLOR PUSH:\t(%d %d %d) ",
			   cstack[csp].red, cstack[csp].green, cstack[csp].blue))
}

void popcolor()
{
  if (csp > 1) csp--; /* Last color is global */
  DEBUG_PRINT(DEBUG_COLOR,("\n  COLOR POP\t"))
}

void resetcolorstack(char * p)
{
  if ( csp > 1 )
    Warning("global color change within nested colors");
  csp=0;
  pushcolor(p) ;
  DEBUG_PRINT(DEBUG_COLOR,("\n  RESET COLOR:\tbottom of stack:"))
}

void StoreColorStack(struct page_list *tpagep)
{
  int i=0;

  DEBUG_PRINT(DEBUG_COLOR,("\n  STORE COLOR STACK:\t %d ", csp));
  tpagep->csp=csp;
  while ( i <= csp ) {
    DEBUG_PRINT(DEBUG_COLOR,("\n  COLOR STACK:\t %d (%d %d %d) ",i,
			     cstack[i].red, cstack[i].green, cstack[i].blue));
    tpagep->cstack[i].red = cstack[i].red;
    tpagep->cstack[i].green = cstack[i].green;
    tpagep->cstack[i].blue = cstack[i].blue;
    i++;
  }
}

void ReadColorStack(struct page_list *tpagep)
{
  int i=0;

  DEBUG_PRINT(DEBUG_COLOR,("\n  READ COLOR STACK:\t %d ", tpagep->csp));
  csp=tpagep->csp;
  while ( i <= tpagep->csp ) {
    DEBUG_PRINT(DEBUG_COLOR,("\n  COLOR STACK:\t %d (%d %d %d) ",i,
			     cstack[i].red, cstack[i].green, cstack[i].blue));
    cstack[i].red = tpagep->cstack[i].red;
    cstack[i].green = tpagep->cstack[i].green;
    cstack[i].blue = tpagep->cstack[i].blue;
    i++;
  }
}

void StoreBackgroundColor(struct page_list *tpagep)
{
  /* Background color changes affect the _whole_ page */
  tpagep->cstack[0].red = cstack[0].red;
  tpagep->cstack[0].green = cstack[0].green;
  tpagep->cstack[0].blue = cstack[0].blue;
}
