/* $XConsortium: fontfcn.c,v 1.8 92/03/27 18:15:45 eswu Exp $ */
/* Copyright International Business Machines,Corp. 1991
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is
 * hereby granted, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation,
 * and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * IBM PROVIDES THIS SOFTWARE "AS IS", WITHOUT ANY WARRANTIES
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT
 * LIMITED TO ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS.  THE ENTIRE RISK AS TO THE QUALITY AND
 * PERFORMANCE OF THE SOFTWARE, INCLUDING ANY DUTY TO SUPPORT
 * OR MAINTAIN, BELONGS TO THE LICENSEE.  SHOULD ANY PORTION OF
 * THE SOFTWARE PROVE DEFECTIVE, THE LICENSEE (NOT IBM) ASSUMES
 * THE ENTIRE COST OF ALL SERVICING, REPAIR AND CORRECTION.  IN
 * NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* Author: Katherine A. Hitchcock    IBM Almaden Research Laboratory */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "t1imager.h" 
#include "util.h"
#include "fontfcn.h"
#include "fontmisc.h"
#include "paths_rmz.h" 

#include "../t1lib/parseAFM.h" 
#include "../t1lib/t1types.h"
#include "../t1lib/t1extern.h"
#include "../t1lib/t1misc.h"
#include "../t1lib/t1base.h"
#include "../t1lib/t1finfo.h"

/* Note: The argument decodeonly is used to make Type1Char() decode only
         such that later certain characterictics of the pass can be queried
	 (here, information about the parts of a seac).
*/
extern xobject Type1Char(psfont *env, struct XYspace *S,
			 psobj *charstrP, psobj *subrsP,
			 psobj *osubrsP,
			 struct blues_struct *bluesP,
			 int *modeP, char *name,
			 float strokewidth,
			 int decodeonly);
extern xobject Type1Line(psfont *env, struct XYspace *S,
			 float line_position,
			 float line_thickness,
			 float line_length,
			 float strokewidth);
extern int T1int_Type1QuerySEAC( unsigned char* base,
				 unsigned char* accent);
void objFormatName(psobj *objP, int length, char *valueP);
  
extern void T1io_reset( void);

#define BEZIERTYPE 0x10+0x02
#define LINETYPE   0x10+0x00
#define MOVETYPE   0x10+0x05

#if 1
struct region {
  XOBJ_COMMON           /* xobject common data define 3-26-91 PNM    */
  /* type = REGIONTYPE                         */
  struct fractpoint origin;    /* beginning handle:  X,Y origin of region      */
  struct fractpoint ending;    /* ending handle:  X,Y change after painting region */
  pel xmin,ymin;        /* minimum X,Y of region                        */
  pel xmax,ymax;        /* mat1_mum X,Y of region                        */
  struct edgelist *anchor;  /* list of edges that bound the region      */
  struct picture *thresholded;  /* region defined by thresholded picture*/
  /*
    Note that the ending handle and the bounding box values are stored
    relative to 'origin'.
    
    The above elements describe a region.  The following elements are
    scratchpad areas used while the region is being built:
  */
  fractpel lastdy;      /* direction of last segment                    */
  fractpel firstx,firsty;    /* starting point of current edge          */
  fractpel edgexmin,edgexmax;  /* x extent of current edge              */
  struct edgelist *lastedge,*firstedge;  /* last and first edges in subpath */
  pel *edge;            /* pointer to array of X values for edge        */
  fractpel edgeYstop;   /* Y value where 'edges' array ends             */
  int (*newedgefcn)();  /* function to use when building a new edge     */
  struct strokeinfo *strokeinfo;  /* scratchpad info during stroking only */
} ;
struct edgelist {
  XOBJ_COMMON          /* xobject common data define 3-26-91 PNM        */
  /* type = EDGETYPE                               */
  struct edgelist *link;  /* pointer to next in linked list             */
  struct edgelist *subpath;  /* informational link for "same subpath"   */
  pel xmin,xmax;        /* range of edge in X                           */
  pel ymin,ymax;        /* range of edge in Y                           */
  pel *xvalues;         /* pointer to ymax-ymin X values                */
};
#endif

/***================================================================***/
/*   GLOBALS                                                          */
/***================================================================***/
static char CurCharName[257]="";
static char BaseCharName[257]="";
char CurFontName[MAXPATHLEN+1];
char *CurFontEnv;
char *vm_base = NULL;

static char notdef[]=".notdef";


/* the following is inserted by RMz for VM checking and reallocating: */
char *vm_used = NULL;
extern int vm_init_count;
extern int vm_init_amount;

static psfont *FontP = NULL;
psfont TheCurrentFont;
 
 
/***================================================================***/
/*   SearchDict - look for  name                                      */
/*              - compare for match on len and string                 */
/*                return 0 - not found.                               */
/*                return n - nth element in dictionary.               */
/***================================================================***/
int SearchDictName(dictP,keyP)
 psdict *dictP;
 psobj  *keyP;
{
  int i,n;
 
 
  n =  dictP[0].key.len;
  for (i=1;i<=n;i++) {          /* scan the intire dictionary */
    if (
        (dictP[i].key.len  == keyP->len )
        &&
        (strncmp(dictP[i].key.data.valueP,
                 keyP->data.valueP,
                 keyP->len) == 0
        )
       ) return(i);
  }
  return(0);
}
/***================================================================***/
/* assignment of &TheCurrentFont removed by RMz:
 */
boolean initFont()
{
  if (!(vm_init())) return(FALSE);
  vm_base = vm_next_byte();
  strcpy(CurFontName, "");    /* iniitialize to none */
  FontP->vm_start = vm_next_byte();
  FontP->FontFileName.len = 0;
  FontP->FontFileName.data.valueP = CurFontName;
  return(TRUE);
}
/***================================================================***/
int resetFont(env)
char *env;
{
 
  vm_next =  FontP->vm_start;
  vm_free = vm_size - ( vm_next - vm_base);
  FontP->Subrs.len = 0;
  FontP->Subrs.data.stringP = NULL;
  FontP->CharStringsP = NULL;
  FontP->Private = NULL;
  FontP->fontInfoP = NULL;
  FontP->BluesP = NULL;
  /* This will load the font into the FontP */
  strncpy(CurFontName,env, MAXPATHLEN);
  CurFontName[MAXPATHLEN] = '\0';
  FontP->FontFileName.len = strlen(CurFontName);
  FontP->FontFileName.data.nameP = CurFontName;
  T1io_reset();

  return(0);
  
}
/***================================================================***/
/* Read font used to attempt to load the font and, upon failure, 
   try a second time with twice as much memory.  Unfortunately, if
   it's a really complex font, simply using 2*vm_size may be insufficient.
   I've modified it so that the program will try progressively larger
   amounts of memory until it really runs out or the font loads
   successfully. (ndw)
*/
int readFont(env)
char *env;
{
  int rcode;
  /* int memscale = 2; */ /* initially, try twice just like we used to ... */
 
  /* restore the virtual memory and eliminate old font */
  
  resetFont(env);
  /* This will load the font into the FontP */
 
  rcode = scan_font(FontP);
  return(rcode);
}


static int isCompositeChar( int FontID,
			    char *charname) 
{
  int i;
  FontInfo *pAFMData;
  
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    /* No AFM data present */
    return( -1);
  }

  pAFMData=pFontBase->pFontArray[FontID].pAFMData;
  for ( i=0; i<pAFMData->numOfComps; i++) {
    if (strcmp( pAFMData->ccd[i].ccName, charname)==0)
      return( i);
  }
  
  return( -1);
  
}



/* dump a description of path elements to stdout */
static T1_PATHPOINT getDisplacement( struct segment *path)
{

  register struct segment *ipath;
  register struct beziersegment *ibpath;
  T1_PATHPOINT point={0,0};
  
  /* Step through the path list */
  ipath=(struct segment *)path;
  
  do {
    if (ipath->type==LINETYPE) {
      point.x+=ipath->dest.x;
      point.y+=ipath->dest.y;
    }
    else if (ipath->type==MOVETYPE) {
      point.x+=ipath->dest.x;
      point.y+=ipath->dest.y;
    }
    else if (ipath->type==BEZIERTYPE) {
      ibpath=(struct beziersegment *)ipath;
      point.x+=ibpath->dest.x;
      point.y+=ibpath->dest.y;
    }
    ipath=ipath->link;
  } while (ipath!=NULL);
  return( point);
  
}



/***================================================================***/
/* RMz: instead of code, which is a character pointer to the name
        of the character, we use "ev" which is a pointer to a desired
	encoding vector (or NULL if font-internal encoding should be
	used) and "index" as an index into the desired encoding vector!
	The user thus has the opportunity of supplying whatever encoding
	he wants. Font_Ptr is the pointer to the local psfont-structure. 
	*/

xobject fontfcnB(int FontID, int modflag,
		 struct XYspace *S, char **ev,
		 unsigned char index, int *mode,
		 psfont *Font_Ptr,
		 int do_raster,
		 float strokewidth)
{
 
  psobj *charnameP; /* points to psobj that is name of character*/
  FontInfo *pAFMData=NULL;
  int i=-1;
  int j=0;
  int numPieces=1;
  int N;
  T1_PATHPOINT currdisp;
  int basechar;
  
  psdict *CharStringsDictP; /* dictionary with char strings     */
  psobj   CodeName;   /* used to store the translation of the name*/
  psobj  *SubrsArrayP;
  psobj  *theStringP;
  int localmode=0;
  
  struct segment *charpath=NULL;   /* the path for this character   */           
  struct segment *tmppath1=NULL;
  struct segment *tmppath2=NULL;
  struct segment *tmppath3=NULL;
  struct segment *tmppath4=NULL;
  
   
  /* set the global font pointer to the address of already allocated
     structure and setup pointers*/
  FontP=Font_Ptr;
  CharStringsDictP =  FontP->CharStringsP;
  SubrsArrayP = &(FontP->Subrs);
  charnameP = &CodeName;

  if (ev==NULL){  /* font-internal encoding should be used */
    charnameP->len = FontP->fontInfoP[ENCODING].value.data.arrayP[index].len;
    charnameP->data.stringP = (unsigned char *) FontP->fontInfoP[ENCODING].value.data.arrayP[index].data.arrayP;
  }
  else{           /* some user-supplied encoding is to be used */
    charnameP->len = strlen(ev[index]);
    charnameP->data.stringP = (unsigned char *) ev[index];
  }
  strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
  CurCharName[charnameP->len]='\0';
  
 
  /* search the chars string for this charname as key */
  basechar = SearchDictName(CharStringsDictP,charnameP);
  if (basechar<=0) {
    /* Check first, whether a char in question is a composite char */
    if ((i=isCompositeChar( FontID, CurCharName))>-1) {
      /* i is now the index of the composite char definitions
	 (starting at 0). At this point it is clear that AFM-info
	 must be present -> fetch first component of composite char. */
      pAFMData=pFontBase->pFontArray[FontID].pAFMData;
      charnameP->len=strlen( pAFMData->ccd[i].pieces[0].pccName);
      charnameP->data.stringP=(unsigned char*)pAFMData->ccd[i].pieces[0].pccName;
      numPieces=pAFMData->ccd[i].numOfPieces;
      
      if ((basechar=SearchDictName(CharStringsDictP,charnameP))<=0) {
	/* this is bad, AFM-file and font file do not match. This 
	   will most probably lead to errors or inconsistencies later.
	   However, we substitute .notdef and inform the user via
	   logfile and T1_errno. */
	sprintf( err_warn_msg_buf,
		 "Charstring \"%s\" needed to construct composite char \"%s\" not defined (FontID=%d)",
		 pAFMData->ccd[i].pieces[0].pccName,
		 pAFMData->ccd[i].ccName, FontID);
	T1_PrintLog( "fontfcnB():", err_warn_msg_buf, T1LOG_WARNING);
	T1_errno=T1ERR_COMPOSITE_CHAR;
      }
    }
  }
  
  if (basechar<=0) { /* This  means the requested char is unknown or the
			base char of a composite is not found ->
			we substitute .notdef */
    charnameP = &CodeName;
    charnameP->len = 7;
    charnameP->data.stringP = (unsigned char *) notdef;
    basechar = SearchDictName(CharStringsDictP,charnameP);
    localmode=FF_NOTDEF_SUBST;
    /* Font must be completely damaged if it doesn't define a .notdef */
    if (basechar<=0) {
      *mode=FF_PARSE_ERROR;
      return(NULL);
    }
  } /* if (basechar<=0) */
  /* basechar is now the index of the base character in the CharStrings
     dictionary */

  /* we provide the Type1Char() procedure with the name of the character
     to rasterize for debugging purposes */
  strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
  CurCharName[charnameP->len]='\0';
  /* get CharString and character path */
  theStringP = &(CharStringsDictP[basechar].value);
  tmppath2 = (struct segment *) Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
					  FontP->BluesP,mode,CurCharName,strokewidth,0);
  /* if Type1Char reported an error, then return */
  if ( *mode == FF_PARSE_ERROR || *mode==FF_PATH_ERROR)
    return(NULL);
  
  /* Defer rastering to later, we first have to handle the composite
     symbols */
  for (j=1; j<numPieces; j++) {
    /* get composite symbol name */
    charnameP->len=strlen( pAFMData->ccd[i].pieces[j].pccName);
    charnameP->data.stringP=(unsigned char*)pAFMData->ccd[i].pieces[j].pccName;
    /* get CharString definition */
    if ((N=SearchDictName(CharStringsDictP,charnameP))<=0) {
      /* handling of errors, see comments above ... */
      sprintf( err_warn_msg_buf,
	       "Charstring \"%s\" needed to construct composite char \"%s\" not defined (FontID=%d)",
	       pAFMData->ccd[i].pieces[j].pccName,
	       pAFMData->ccd[i].ccName, FontID);
      T1_PrintLog( "fontfcnB():", err_warn_msg_buf, T1LOG_WARNING);
      charnameP = &CodeName;
      charnameP->len = 7;
      charnameP->data.stringP = (unsigned char *) notdef;
      N = SearchDictName(CharStringsDictP,charnameP);
      localmode=FF_NOTDEF_SUBST;
      /* damaged Font */
      if (N<=0) {
	*mode=FF_PARSE_ERROR;
	if (charpath!=NULL) {
	  KillPath( charpath);
	}
	return(NULL);
      }
    }
    theStringP = &(CharStringsDictP[N].value);
    tmppath1=(struct segment *)ILoc(S,
				    pAFMData->ccd[i].pieces[j].deltax,
				    pAFMData->ccd[i].pieces[j].deltay);
    
    strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
    CurCharName[charnameP->len]='\0';
    charpath=(struct segment *)Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
					 FontP->BluesP,mode,CurCharName,strokewidth,0);
    /* return if Type1Char reports an error */
    if ( *mode == FF_PARSE_ERROR || *mode==FF_PATH_ERROR)
      return(NULL);
    /* get escapement of current symbol */
    currdisp=getDisplacement( charpath);
    /* concat displacement and symbol path */
    charpath=(struct segment *)Join(tmppath1,charpath);
    /* for composite symbols we have to step back the char escapement.
       this is, in order to be able to use accents that cause a
       non zero displacement of the current point! We further have to
       step back the displacement from composite char data. */
    tmppath1=(struct segment *)t1_PathSegment( MOVETYPE, -currdisp.x, -currdisp.y);
    tmppath3=(struct segment *)ILoc(S,
				    -pAFMData->ccd[i].pieces[j].deltax,
				    -pAFMData->ccd[i].pieces[j].deltay);
    tmppath3=(struct segment *)Join(tmppath1,tmppath3);
    /* create path, or, respectively, append to existing path */
    if (tmppath4==NULL) {
      tmppath4=(struct segment *)Join(charpath,tmppath3);
    }
    else {
      charpath=(struct segment *)Join(charpath,tmppath3);
      tmppath4=(struct segment *)Join(tmppath4,charpath);
    }
  }

  /* concat composite symbols and base char */
  if (tmppath4==NULL) { /* no previous composite symbols */
    charpath=tmppath2; /* a simple char */
  }
  else { 
    charpath=(struct segment *)Join(tmppath4,tmppath2);
  }
  
  
  if (do_raster) { 
    /* fill with winding rule unless path was requested */
    if (*mode != FF_PATH) {
      charpath =  (struct segment *)Interior(charpath,WINDINGRULE+CONTINUITY);
    }
  }

  if (*mode==0)
    *mode=localmode;
  
  return((xobject) charpath);
}




/***================================================================***/
/*   fontfcnA(env, mode)                                              */
/*                                                                    */
/*          env is a pointer to a string that contains the fontname.  */
/*                                                                    */
/*     1) initialize the font     - global indicates it has been done */
/*     2) load the font                                               */
/*                                                                    */
/* This function has been modified by RMz. It now takes a pointer which
   already contains the address of a valid type1 font structure as the
   third argument. The value of this pointer is first handed to FontP
   so that most other routines may be used without changes */

#define MAXTRIAL              4

/***================================================================***/
Bool fontfcnA(env,mode,Font_Ptr)
char *env;
int  *mode;
psfont *Font_Ptr;

{
  int i, result;
  
  /* set the global font pointer to the address of already allocated
     structure */
  FontP=Font_Ptr;

  InitImager();

  /* Read the font program. */
  for (i=1; i<MAXTRIAL; i++){
    vm_init_count=0;
    /* We allocate larger chunks (4*65536 Bytes) in order to reduce load
       time for large fonts by initially requesting somewhat more
       memory. */
    vm_init_amount=MAX_STRING_LEN * 4 * i;
    if (!(initFont())) {
      /* we are really out of memory, not simulated! */
      *mode = SCAN_OUT_OF_MEMORY;
      return(FALSE);
    }
    /* Try to read font into memory */
    if ((result=readFont(env))==0){
      /* In order to get the amount of memory that was really used */      
      vm_used=vm_next_byte();
      return(TRUE);
    }
    else{
      /* VM did not suffice, free it and try again with larger
	 value: */
      free(vm_base);
    }
  }
  /* Font could not be loaded: */
  *mode = result;
  return(FALSE);

}


/***================================================================***/
/*   QueryFontLib(env, infoName,infoValue,rcodeP)                     */
/*                                                                    */
/*          env is a pointer to a string that contains the fontname.  */
/*                                                                    */
/*     1) initialize the font     - global indicates it has been done */
/*     2) load the font                                               */
/*     3) use the font to call getInfo for that value.                */
/***================================================================***/

void QueryFontLib(env,infoName,infoValue,rcodeP)
char *env;
char *infoName;
pointer infoValue;    /* parameter returned here    */
int  *rcodeP;
{

  int rc,N,i;
  psdict *dictP;
  psobj  nameObj;
  psobj  *valueP;
 
  /* Has the FontP initialized?  If not, then   */
  /* Initialize  */
  if (FontP == NULL) {
    InitImager();
    if (!(initFont())) {
      *rcodeP = 1;
      return;
    }
  }
  /* if the env is null, then use font already loaded */
  /* if the not same font name, reset and load next font */
  if ( (env) && (strcmp(env,CurFontName) != 0 ) ) {
    /* restore the virtual memory and eliminate old font */
    rc = readFont(env);
    if (rc != 0 ) {
      strcpy(CurFontName, "");    /* no font loaded */
      *rcodeP = 1;
      return;
    }
  }
  dictP = FontP->fontInfoP;
  objFormatName(&nameObj,strlen(infoName),infoName);
  N = SearchDictName(dictP,&nameObj);
  /* if found */
  if ( N > 0 ) {
    *rcodeP = 0;
    switch (dictP[N].value.type) {
       case OBJ_ARRAY:
         valueP = dictP[N].value.data.arrayP;
         if (strcmp(infoName,"FontMatrix") == 0) {
           /* 6 elments, return them as floats      */
           for (i=0;i<6;i++) {
             if (valueP->type == OBJ_INTEGER )
               ((float *)infoValue)[i] = valueP->data.integer;
             else
               ((float *)infoValue)[i] = valueP->data.real;
            valueP++;
           }
         }
         if (strcmp(infoName,"FontBBox") == 0) {
           /* 4 elments for Bounding Box.  all integers   */
           for (i=0;i<4;i++) {
             ((int *)infoValue)[i] = valueP->data.integer;
             valueP++;
           }
         break;
       case OBJ_INTEGER:
       case OBJ_BOOLEAN:
         *((int *)infoValue) = dictP[N].value.data.integer;
         break;
       case OBJ_REAL:
         *((float *)infoValue) = dictP[N].value.data.real;
         break;
       case OBJ_NAME:
       case OBJ_STRING:
         *((char **)infoValue) =  dictP[N].value.data.valueP;
         break;
       default:
         *rcodeP = 1;
         break;
     }
   }
  }
  else *rcodeP = 1;
}


/***================================================================***/
/* RMz: instead of code, which is a character pointer to the name
        of the character, we use "ev" which is a pointer to a desired
	encoding vector (or NULL if font-internal encoding should be
	used) and "index" as an index into the desired encoding vector!
	The user thus has the opportunity of supplying whatever encoding
	he wants. Font_Ptr is the pointer to the local psfont-structure. 
	*/
xobject fontfcnB_string( int FontID, int modflag,
			 struct XYspace *S, char **ev,
			 unsigned char *string, int no_chars,
			 int *mode, psfont *Font_Ptr,
			 int *kern_pairs, long spacewidth,
			 int do_raster,
			 float strokewidth)
{
 
  psobj *charnameP; /* points to psobj that is name of character*/
  FontInfo *pAFMData=NULL;
  int i=-1;
  int j=0;
  int k=0;
  long acc_width=0;
  int numPieces=1;
  int N;
  T1_PATHPOINT currdisp;
  int basechar;
  
  psdict *CharStringsDictP; /* dictionary with char strings     */
  psobj   CodeName;   /* used to store the translation of the name*/
  psobj  *SubrsArrayP;
  psobj  *theStringP;
  int localmode=0;
  
  struct segment *charpath=NULL;   /* the path for this character   */           
  struct segment *tmppath1=NULL;
  struct segment *tmppath2=NULL;
  struct segment *tmppath3=NULL;
  struct segment *tmppath4=NULL;
  struct segment *tmppath5=NULL;
  
   
  /* set the global font pointer to the address of already allocated
     structure and setup pointers*/
  FontP=Font_Ptr;
  CharStringsDictP =  FontP->CharStringsP;
  SubrsArrayP = &(FontP->Subrs);
  charnameP = &CodeName;

  
  /* In the following for-loop, all characters are processed, one after
     the other. Between them, the amount of kerning is inserted.
     The number of path variables used is somewhat numerous. We use the
     follwing conventions:

     charpath:  the overall path of the string.
     tmppath5:  the overall path of one component (possibly a composite symbol)
     tmppath2:  the path of a simple char or base char of a composite
     tmppath4:  the path of all "accents" of a composite symbol
  */
  for (k=0; k<no_chars;k++) {
    if (ev==NULL){  /* font-internal encoding should be used */
      charnameP = &CodeName;
      charnameP->len = FontP->fontInfoP[ENCODING].value.data.arrayP[string[k]].len;
      charnameP->data.stringP = (unsigned char *) FontP->fontInfoP[ENCODING].value.data.arrayP[string[k]].data.arrayP;
    }
    else {           /* some user-supplied encoding is to be used */
      charnameP = &CodeName;
      charnameP->len = strlen(ev[string[k]]);
      charnameP->data.stringP = (unsigned char*) ev[string[k]];
    }
    
    /* Spacing is to be under users control: => if space is the charname, don't
       raster it. Rather, generate a horizontal movement of spacewidth: */
    if (strcmp((char *)charnameP->data.stringP, "space")==0){
      tmppath5=(struct segment *)ILoc(S, spacewidth,0);
      acc_width += spacewidth;
    }
    else {
      /* here a character or composite character is to be constructed */
      strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
      CurCharName[charnameP->len]='\0';
      
      /* search the CharString for this charname as key */
      basechar = SearchDictName(CharStringsDictP,charnameP);
      if (basechar<=0) {
	/* Check first, whether a char in question is a composite char */
	if ((i=isCompositeChar( FontID, CurCharName))>-1) {
	  /* i is now the index of the composite char definitions
	     (starting at 0). At this point it is clear that AFM-info
	     must be present -> fetch first component of composite char. */
	  pAFMData=pFontBase->pFontArray[FontID].pAFMData;
	  charnameP->len=strlen( pAFMData->ccd[i].pieces[0].pccName);
	  charnameP->data.stringP=(unsigned char*)pAFMData->ccd[i].pieces[0].pccName;
	  numPieces=pAFMData->ccd[i].numOfPieces;
	  
	  if ((basechar=SearchDictName(CharStringsDictP,charnameP))<=0) {
	    /* this is bad, AFM-file and font file do not match. This 
	       will most probably lead to errors or inconsistencies later.
	       However, we substitute .notdef and inform the user via
	       logfile and T1_errno. */
	    sprintf( err_warn_msg_buf,
		     "Charstring \"%s\" needed to construct composite char \"%s\" not defined (FontID=%d)",
		     pAFMData->ccd[i].pieces[0].pccName,
		     pAFMData->ccd[i].ccName, FontID);
	    T1_PrintLog( "fontfcnB():", err_warn_msg_buf, T1LOG_WARNING);
	    T1_errno=T1ERR_COMPOSITE_CHAR;
	  }
	}
      }
      
      if (basechar<=0) { /* This  means the requested char is unknown or the
			    base char of a composite is not found ->
			    we substitute .notdef */
	charnameP = &CodeName;
	charnameP->len = 7;
	charnameP->data.stringP = (unsigned char *) notdef;
	basechar = SearchDictName(CharStringsDictP,charnameP);
	localmode=FF_NOTDEF_SUBST;
	/* Font must be completely damaged if it doesn't define a .notdef */
	if (basechar<=0) {
	  *mode=FF_PARSE_ERROR;
	  return(NULL);
	}
      } /* if (basechar<=0) */
      /* basechar is now the index of the base character in the CharStrings
	 dictionary */
      
      /* we provide the Type1Char() procedure with the name of the character
	 to rasterize for debugging purposes */
      strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
      CurCharName[charnameP->len]='\0';
      /* get CharString and character path */
      theStringP = &(CharStringsDictP[basechar].value);
      tmppath2 = (struct segment *) Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
					      FontP->BluesP,mode,CurCharName,strokewidth,0);
      strcpy( BaseCharName, CurCharName);
      /* if Type1Char reports an error, clean up and return */
      if ( *mode == FF_PARSE_ERROR || *mode==FF_PATH_ERROR) {
	if (charpath!=NULL) {
	  KillPath( charpath);
	}
	if (tmppath1!=NULL) {
	  KillPath( tmppath1);
	}
	if (tmppath2!=NULL) {
	  KillPath( tmppath2);
	}
	if (tmppath3!=NULL) {
	  KillPath( tmppath3);
	}
	if (tmppath4!=NULL) {
	  KillPath( tmppath4);
	}
	if (tmppath5!=NULL) {
	  KillPath( tmppath5);
	}
	return(NULL);
      }
	
      /* Defer rastering to later, we first have to handle the composite
	 symbols */
      for (j=1; j<numPieces; j++) {
	/* get composite symbol name */
	charnameP->len=strlen( pAFMData->ccd[i].pieces[j].pccName);
	charnameP->data.stringP=(unsigned char*)pAFMData->ccd[i].pieces[j].pccName;
	/* get CharString definition */
	if ((N=SearchDictName(CharStringsDictP,charnameP))<=0) {
	  /* handling of errors, see comments above ... */
	  sprintf( err_warn_msg_buf,
		   "Charstring \"%s\" needed to construct composite char \"%s\" not defined (FontID=%d)",
		   pAFMData->ccd[i].pieces[j].pccName,
		   pAFMData->ccd[i].ccName, FontID);
	  T1_PrintLog( "fontfcnB():", err_warn_msg_buf, T1LOG_WARNING);
	  charnameP = &CodeName;
	  charnameP->len = 7;
	  charnameP->data.stringP = (unsigned char *) notdef;
	  N = SearchDictName(CharStringsDictP,charnameP);
	  localmode=FF_NOTDEF_SUBST;
	  /* an undefined .notdef is fatal -> clean up and return */
	  if (N<=0) {
	    *mode=FF_PARSE_ERROR;
	    if (charpath!=NULL) {
	      KillPath( charpath);
	    }
	    if (tmppath1!=NULL) {
	      KillPath( tmppath1);
	    }
	    if (tmppath2!=NULL) {
	      KillPath( tmppath2);
	    }
	    if (tmppath3!=NULL) {
	      KillPath( tmppath3);
	    }
	    if (tmppath4!=NULL) {
	      KillPath( tmppath4);
	    }
	    if (tmppath5!=NULL) {
	      KillPath( tmppath5);
	    }
	    return(NULL);
	  }
	}
	theStringP = &(CharStringsDictP[N].value);
	tmppath1=(struct segment *)ILoc(S,
					pAFMData->ccd[i].pieces[j].deltax,
					pAFMData->ccd[i].pieces[j].deltay);
    
	strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
	CurCharName[charnameP->len]='\0';
	tmppath5=(struct segment *)Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
					     FontP->BluesP,mode,CurCharName,strokewidth,0);
	/* return if Type1Char reports an error */
	if ( *mode == FF_PARSE_ERROR || *mode==FF_PATH_ERROR)
	  return(NULL);
	/* get escapement of current symbol */
	currdisp=getDisplacement( tmppath5);
	/* concat displacement and symbol path */
	tmppath5=(struct segment *)Join(tmppath1,tmppath5);
	/* for composite symbols we have to step back the char escapement.
	   this is, in order to be able to use accents that cause a
	   non zero displacement of the current point! We further have to
	   step back the displacement from composite char data. */
	tmppath1=(struct segment *)t1_PathSegment( MOVETYPE, -currdisp.x, -currdisp.y);
	tmppath3=(struct segment *)ILoc(S,
					-pAFMData->ccd[i].pieces[j].deltax,
					-pAFMData->ccd[i].pieces[j].deltay);
	tmppath3=(struct segment *)Join(tmppath1,tmppath3);
	/* create path, or, respectively, append to existing path */
	if (tmppath4==NULL) {
	  tmppath4=(struct segment *)Join(tmppath5,tmppath3);
	}
	else {
	  tmppath5=(struct segment *)Join(tmppath5,tmppath3);
	  tmppath4=(struct segment *)Join(tmppath4,tmppath5);
	}
      }
      
      /* concat composite symbols and base char. We use tmppath5 to store
	 the path of the resulting (possibly composite) character. */
      if (tmppath4==NULL) { /* no previous composite symbols */
	tmppath5=tmppath2; /* a simple char */
      }
      else { 
	tmppath5=(struct segment *)Join(tmppath4,tmppath2);
      }
      

      /* Accumulate displacement, but be careful: In case of composite
	 characters, we have to take the escapement of the base char only
	 into account, because accents do not cause spacing. The path is
	 constructed in a way that this automatically matches.
      */
      if (numPieces>1) { /* composite character */
	acc_width +=pFontBase->pFontArray[FontID].pAFMData->ccd[-(pFontBase->pFontArray[FontID].pEncMap[string[k]]+1)].wx;
      }
      else { /* ordinary character */
	acc_width +=pFontBase->pFontArray[FontID].pAFMData->cmi[pFontBase->pFontArray[FontID].pEncMap[string[k]]-1].wx;
      }
      
    } /* else (if (char==space) */

    /* character path is now stored in tmppath5. It may be a composite character.
       Insert kerning amount, if it is not the last character of the string. */
    if (k<no_chars-1){
      tmppath2=(struct segment *)ILoc(S,kern_pairs[k],0); 
      tmppath5=(struct segment *)Join(tmppath5,tmppath2);
      acc_width += kern_pairs[k];
    }
    if (charpath!=NULL){
      charpath=(struct segment *)Join(charpath,tmppath5);
    }
    else{
      charpath=(struct segment *)tmppath5;
    }
    /* reset the temporary paths so that constructing composite
       characters wiil continue to work properly in the next interation. */
    tmppath1=NULL;
    tmppath2=NULL;
    tmppath3=NULL;
    tmppath4=NULL;
    tmppath5=NULL;
    /* reset composition parameters */
    i=-1;
    numPieces=1;
    
  } /* for (k<no_chars) */
  
  
  /* Take care for underlining and such */
  if (modflag & T1_UNDERLINE){
    tmppath2=(struct segment *)Type1Line(FontP,S,
					 pFontBase->pFontArray[FontID].UndrLnPos,
					 pFontBase->pFontArray[FontID].UndrLnThick,
					 (float) acc_width,strokewidth);
    charpath=(struct segment *)Join(charpath,tmppath2);
  }
  if (modflag & T1_OVERLINE){
    tmppath2=(struct segment *)Type1Line(FontP,S,
					 pFontBase->pFontArray[FontID].OvrLnPos,
					 pFontBase->pFontArray[FontID].OvrLnThick,
					 (float) acc_width,strokewidth);
    charpath=(struct segment *)Join(charpath,tmppath2);
  }
  if (modflag & T1_OVERSTRIKE){
    tmppath2=(struct segment *)Type1Line(FontP,S,
					 pFontBase->pFontArray[FontID].OvrStrkPos,
					 pFontBase->pFontArray[FontID].OvrStrkThick,
					 (float) acc_width,strokewidth);
    charpath=(struct segment *)Join(charpath,tmppath2);
  }
  
  /*
  printf("charpath->type: %x\n",charpath->type);
  printf("path1->type: %x\n",path1->type);
  printf("path2->type: %x\n",path2->type);
  */

  /* if Type1Char reported an error, then return */

  if ( *mode == FF_PARSE_ERROR)  return(NULL);
  if ( *mode == FF_PATH_ERROR)  return(NULL);
  if (do_raster) { 
    /* fill with winding rule unless path was requested */
    if (*mode != FF_PATH) {
      charpath = (struct segment *) Interior((path) charpath,WINDINGRULE+CONTINUITY);
    }
  }
  
  if (*mode==0)
    *mode=localmode;

  return((path)charpath);
}


/* This special variant is for generating character bitmaps from
   charactername */
xobject fontfcnB_ByName( int FontID, int modflag,
			 struct XYspace *S,
			 unsigned char *charname,
			 int *mode, psfont *Font_Ptr,
			 int do_raster)
{
 
  psobj *charnameP; /* points to psobj that is name of character*/
  FontInfo *pAFMData=NULL;
  int i=-1;
  int j=0;
  int numPieces=1;
  int N;
  T1_PATHPOINT currdisp;
  int basechar;
  
  psdict *CharStringsDictP; /* dictionary with char strings     */
  psobj   CodeName;   /* used to store the translation of the name*/
  psobj  *SubrsArrayP;
  psobj  *theStringP;
  int localmode=0;
  
  struct segment *charpath=NULL;   /* the path for this character   */           
  struct segment *tmppath1=NULL;
  struct segment *tmppath2=NULL;
  struct segment *tmppath3=NULL;
  struct segment *tmppath4=NULL;
  
   
  /* set the global font pointer to the address of already allocated
     structure and setup pointers*/
  FontP=Font_Ptr;
  CharStringsDictP =  FontP->CharStringsP;
  SubrsArrayP = &(FontP->Subrs);
  charnameP = &CodeName;

  charnameP->len = strlen((char*)charname);
  charnameP->data.stringP = charname;

  strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
  CurCharName[charnameP->len]='\0';
  
 
  /* search the chars string for this charname as key */
  basechar = SearchDictName(CharStringsDictP,charnameP);
  if (basechar<=0) {
    /* Check first, whether a char in question is a composite char */
    if ((i=isCompositeChar( FontID, CurCharName))>-1) {
      /* i is now the index of the composite char definitions
	 (starting at 0). At this point it is clear that AFM-info
	 must be present -> fetch first component of composite char. */
      pAFMData=pFontBase->pFontArray[FontID].pAFMData;
      charnameP->len=strlen( pAFMData->ccd[i].pieces[0].pccName);
      charnameP->data.stringP=(unsigned char*)pAFMData->ccd[i].pieces[0].pccName;
      numPieces=pAFMData->ccd[i].numOfPieces;
      
      if ((basechar=SearchDictName(CharStringsDictP,charnameP))<=0) {
	/* this is bad, AFM-file and font file do not match. This 
	   will most probably lead to errors or inconsistencies later.
	   However, we substitute .notdef and inform the user via
	   logfile and T1_errno. */
	sprintf( err_warn_msg_buf,
		 "Charstring \"%s\" needed to construct composite char \"%s\" not defined (FontID=%d)",
		 pAFMData->ccd[i].pieces[0].pccName,
		 pAFMData->ccd[i].ccName, FontID);
	T1_PrintLog( "fontfcnB():", err_warn_msg_buf, T1LOG_WARNING);
	T1_errno=T1ERR_COMPOSITE_CHAR;
      }
    }
  }
  
  if (basechar<=0) { /* This  means the requested char is unknown or the
			base char of a composite is not found ->
			we substitute .notdef */
    charnameP = &CodeName;
    charnameP->len = 7;
    charnameP->data.stringP = (unsigned char *) notdef;
    basechar = SearchDictName(CharStringsDictP,charnameP);
    localmode=FF_NOTDEF_SUBST;
    /* Font must be completely damaged if it doesn't define a .notdef */
    if (basechar<=0) {
      *mode=FF_PARSE_ERROR;
      return(NULL);
    }
  } /* if (basechar<=0) */
  /* basechar is now the index of the base character in the CharStrings
     dictionary */

  /* we provide the Type1Char() procedure with the name of the character
     to rasterize for debugging purposes */
  strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
  CurCharName[charnameP->len]='\0';
  /* get CharString and character path */
  theStringP = &(CharStringsDictP[basechar].value);
  tmppath2 = (struct segment *) Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
					  FontP->BluesP,mode,CurCharName, 0.0f, 0);
  /* if Type1Char reported an error, then return */
  if ( *mode == FF_PARSE_ERROR || *mode==FF_PATH_ERROR)
    return(NULL);
  
  /* Defer rastering to later, we first have to handle the composite
     symbols */
  for (j=1; j<numPieces; j++) {
    /* get composite symbol name */
    charnameP->len=strlen( pAFMData->ccd[i].pieces[j].pccName);
    charnameP->data.stringP=(unsigned char*)pAFMData->ccd[i].pieces[j].pccName;
    /* get CharString definition */
    if ((N=SearchDictName(CharStringsDictP,charnameP))<=0) {
      /* handling of errors, see comments above ... */
      sprintf( err_warn_msg_buf,
	       "Charstring \"%s\" needed to construct composite char \"%s\" not defined (FontID=%d)",
	       pAFMData->ccd[i].pieces[j].pccName,
	       pAFMData->ccd[i].ccName, FontID);
      T1_PrintLog( "fontfcnB():", err_warn_msg_buf, T1LOG_WARNING);
      charnameP = &CodeName;
      charnameP->len = 7;
      charnameP->data.stringP = (unsigned char *) notdef;
      N = SearchDictName(CharStringsDictP,charnameP);
      localmode=FF_NOTDEF_SUBST;
      /* damaged Font */
      if (N<=0) {
	*mode=FF_PARSE_ERROR;
	if (charpath!=NULL) {
	  KillPath( charpath);
	}
	return(NULL);
      }
    }
    theStringP = &(CharStringsDictP[N].value);
    tmppath1=(struct segment *)ILoc(S,
				    pAFMData->ccd[i].pieces[j].deltax,
				    pAFMData->ccd[i].pieces[j].deltay);
    
    strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
    CurCharName[charnameP->len]='\0';
    charpath=(struct segment *)Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
					 FontP->BluesP,mode,CurCharName,0.0f,0);
    /* return if Type1Char reports an error */
    if ( *mode == FF_PARSE_ERROR || *mode==FF_PATH_ERROR)
      return(NULL);
    /* get escapement of current symbol */
    currdisp=getDisplacement( charpath);
    /* concat displacement and symbol path */
    charpath=(struct segment *)Join(tmppath1,charpath);
    /* for composite symbols we have to step back the char escapement.
       this is, in order to be able to use accents that cause a
       non zero displacement of the current point! We further have to
       step back the displacement from composite char data. */
    tmppath1=(struct segment *)t1_PathSegment( MOVETYPE, -currdisp.x, -currdisp.y);
    tmppath3=(struct segment *)ILoc(S,
				    -pAFMData->ccd[i].pieces[j].deltax,
				    -pAFMData->ccd[i].pieces[j].deltay);
    tmppath3=(struct segment *)Join(tmppath1,tmppath3);
    /* create path, or, respectively, append to existing path */
    if (tmppath4==NULL) {
      tmppath4=(struct segment *)Join(charpath,tmppath3);
    }
    else {
      charpath=(struct segment *)Join(charpath,tmppath3);
      tmppath4=(struct segment *)Join(tmppath4,charpath);
    }
  }

  /* concat composite symbols and base char */
  if (tmppath4==NULL) { /* no previous composite symbols */
    charpath=tmppath2; /* a simple char */
  }
  else { 
    charpath=(struct segment *)Join(tmppath4,tmppath2);
  }
  
  
  if (do_raster) { 
    /* fill with winding rule unless path was requested */
    if (*mode != FF_PATH) {
      charpath =  (struct segment *)Interior(charpath,WINDINGRULE+CONTINUITY);
    }
  }

  if (*mode==0)
    *mode=localmode;
  
  return((xobject) charpath);

}


xobject fontfcnRect( float width,
		     float height,
		     struct XYspace* S,
		     int *mode,
		     int do_raster,
		     float strokewidth)
{
  struct segment *charpath = NULL;   /* the path for this character (rectangle)  */           
  
  charpath = (struct segment *) Type1Line( NULL, S,
					   0.5f * height,    /* position */
					   height,           /* thickness */
					   -width,            /* width */
					   strokewidth       /* strokewidth */
					   );
  
  if (do_raster) { 
    /* fill with winding rule unless path was requested */
    if (*mode != FF_PATH) {
      charpath =  (struct segment *)Interior(charpath,WINDINGRULE+CONTINUITY);
    }
  }

  return((xobject) charpath);
  
}



/* T1int_QuerySEAC(): Query for Type of character definition of index "index".

   Returns: 0     if charstring for currenc[index] not defined
            1     if charstring defines a self-containing character
	    2     if charstring references other definition by means of
	          the SEAC directive.
*/
int T1int_QuerySEAC( int FontID, 
		     unsigned char index,
		     unsigned char* basepiece,
		     unsigned char* accpiece
		     )
{
  
  psobj *charnameP; /* points to psobj that is name of character*/
  int thischar;
  int mode = 0;
  
  psdict *CharStringsDictP; /* dictionary with char strings     */
  psobj   CodeName;   /* used to store the translation of the name*/
  psobj  *SubrsArrayP;
  psobj  *theStringP;
  char **ev;
  struct XYspace *S;
  struct segment *path=NULL;   /* the path for this character   */           
  
   
  /* set the global font pointer to the address of already allocated
     structure and setup pointers*/
  FontP=pFontBase->pFontArray[FontID].pType1Data;
  CharStringsDictP =  FontP->CharStringsP;
  SubrsArrayP = &(FontP->Subrs);
  charnameP = &CodeName;

  /* get encoding */
  ev=pFontBase->pFontArray[FontID].pFontEnc;

  if (ev==NULL){  /* font-internal encoding should be used */
    charnameP->len = FontP->fontInfoP[ENCODING].value.data.arrayP[index].len;
    charnameP->data.stringP = (unsigned char *) FontP->fontInfoP[ENCODING].value.data.arrayP[index].data.arrayP;
  }
  else{           /* some user-supplied encoding is to be used */
    charnameP->len = strlen(ev[index]);
    charnameP->data.stringP = (unsigned char *) ev[index];
  }
  strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
  CurCharName[charnameP->len]='\0';
  
 
  /* search the chars string for this charname as key */
  thischar = SearchDictName(CharStringsDictP,charnameP);
  /* thischar is now the index of the base character in the CharStrings
     dictionary */

  if ( thischar <= 0 ) {
    /* CharString not defined, return */
    return 0;
  }

  /* Setup NULL-space, not needed when paths aren't created */
  S = NULL; 
      
  
  /* we provide the Type1Char() procedure with the name of the character
     to rasterize for debugging purposes */
  strncpy( (char *)CurCharName, (char *)charnameP->data.stringP, charnameP->len);
  CurCharName[charnameP->len]='\0';
  /* get CharString and character path */
  theStringP = &(CharStringsDictP[thischar].value);
  
  path = (struct segment *) Type1Char(FontP,S,theStringP,SubrsArrayP,NULL,
				      FontP->BluesP,&mode,CurCharName,0, 1);

  /* if Type1Char reported an error, then return */
  if ( mode == FF_PARSE_ERROR || mode==FF_PATH_ERROR) {
    return -1;
  }

  if ( T1int_Type1QuerySEAC( basepiece, accpiece) > 0 ) {
    return 2;
  }

  return 1;

}
