/*--------------------------------------------------------------------------
  ----- File:        t1delete.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2002-12-02
  ----- Description: This file is part of the t1-library. It contains
                     functions for giving free previously allocated
		     memory areas and similar things.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2002. 
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  
#define T1DELETE_C


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(_MSC_VER)
# include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>


#include "../type1/types.h"
#include "parseAFM.h" 
#include "../type1/objects.h" 
#include "../type1/spaces.h"  
#include "../type1/util.h" 
#include "../type1/fontfcn.h"

#include "t1types.h"
#include "t1extern.h"
#include "t1delete.h"
#include "t1load.h"
#include "t1finfo.h"
#include "t1base.h"



/* T1_DeleteSize(): Gives back all the memory allocated for size to the
   system. If size is somewhere in the middle of a linked list of sizes,
   it further takes care that the remaining list is linked in a proper
   way. Function returns 0 if successful and otherwise -1*/
int T1_DeleteSize( int FontID, float size)
{
  int i, j;
  FONTSIZEDEPS *ptr, *next_ptr, *prev_ptr;
  int jobs=0;
  int antialias;
  int level[4]={0,T1_AA_NONE,T1_AA_LOW,T1_AA_HIGH};
  

  for ( j=0; j<4; j++){
    antialias=level[j];
    /* Check if size exists; if not, return 1 */
    if ((ptr=T1int_QueryFontSize( FontID, size, antialias))!=NULL){
      /* We have to remove a size-> */
      jobs++;
      /* Get pointers to structure which is before/after  the structure
	 to be deleted 	 in the linked list and properly relink
	 structures */
      next_ptr=((FONTSIZEDEPS *)ptr)->pNextFontSizeDeps;
      prev_ptr=((FONTSIZEDEPS *)ptr)->pPrevFontSizeDeps;

      if ((prev_ptr==NULL)&&(next_ptr==NULL)){
	/* There's only one single size, no relink is necessary
	   => reset the initial pointer to indicate that no size
	   dependent data is available */
	pFontBase->pFontArray[FontID].pFontSizeDeps=NULL;
      }
      else{
	if (prev_ptr!=NULL)
	  /* We are at the first size of the linked list and
	     there are still some sizes left after removing the
	     current */
	  prev_ptr->pNextFontSizeDeps=next_ptr;
	else
	  pFontBase->pFontArray[FontID].pFontSizeDeps=next_ptr;
	if (next_ptr!=NULL)
	  /* We are at the end of an list of at least two sizes: */
	  next_ptr->pPrevFontSizeDeps=prev_ptr;
      }
      
      /* Now, that the list is properly linked, free the memory used by size: */
      /* Free the bitmaps memory: */
      for (i=0; i<256; i++)
	if (ptr->pFontCache[i].bits)
	  free(ptr->pFontCache[i].bits);

      /* Free memory for glyphs: */
      free(ptr->pFontCache);
      /* Free the structure itself: */
      free(ptr);
      /* Print log: */
      sprintf( err_warn_msg_buf, "Size %f deleted for FontID %d (antialias=%d)",
	       size, FontID, antialias);
      T1_PrintLog( "T1_DeleteSize()", err_warn_msg_buf, T1LOG_STATISTIC);
    }
  }

  /* Return the appropriate value */
  if (jobs==0)
    return(-1);
  else  
    return(0);
  
}


/* T1_DeleteAllSizes(): Gives back all the memory allocated for all sizes
   to the system. Function returns the number of removed sizes or -1 if an
   error ocurred. */
int T1_DeleteAllSizes( int FontID)
{
  int sizecount;
  float currsize;
  
  FONTSIZEDEPS *ptr;
  
  if (T1_CheckForFontID(FontID)!=1)
    return(-1);
  
  /* Start deleting at the end of the linked list: */ 
  sizecount=0;
  if ((ptr=T1int_GetLastFontSize( FontID))==NULL){
    /* There has not been any size dependent data: */
    return(0);
  }

  while (((ptr=T1int_GetLastFontSize(FontID)) != NULL)){
    currsize=ptr->size;
    T1_DeleteSize( FontID, currsize);
    sizecount++;
    
  }

  return(sizecount);
}


/* T1_FreeGlyph(): Gives the memory used by a glyph back to the system. */
int T1_FreeGlyph( GLYPH *glyph)
{
  if (glyph!=NULL) {
    if (glyph->bits!=NULL) {
      free(glyph->bits);
    }
    free(glyph);
  }
  return(0);
}


/* T1_FreeCompCharData(): Return emory used by a composite character
   data information structure to the system */
int T1_FreeCompCharData( T1_COMP_CHAR_INFO *cci)
{

  if (cci!=NULL) {
    if (cci->pieces!=NULL) {
      free( cci->pieces);
    }
    free( cci);
  }
  return( 0);
}


/* T1_DeleteFont(): Gives all memory used by a font back to the system.
   If the font is successfully deinstalled 0 is returned. A positive value
   indicates an error. */
int T1_DeleteFont( int FontID)
{

  int result;

  
  if (T1_CheckForFontID(FontID)==-1){  /* Invalid ID */
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);   
  }
  
  if (T1_CheckForFontID(FontID)==0)   /* Font is not loaded */
    return(0);   

  /* Memory freeing must be done hierachical, start with size dependent
     data: */
  result=T1_DeleteAllSizes(FontID);

  /* Next we delete the AFM-mapping tables */
  if (pFontBase->pFontArray[FontID].pEncMap!=NULL)
    free( pFontBase->pFontArray[FontID].pEncMap);
  if (pFontBase->pFontArray[FontID].pKernMap!=NULL)
    free( pFontBase->pFontArray[FontID].pKernMap);

  /* We do not touch the file name because this is only to be done by
     T1_CloseLib(): */
  
  /* The Type-1 area and tyhe AFM-area may only be free'ed, if the
     font is a "physical" font and if its reference counter is 1, or
     if it is a logical font. Otherwise, other logical font use this
     physical one and it may not be free'ed.  In this case, return the
     number of logical fonts which refer to this physical font. */
  if ((pFontBase->pFontArray[FontID].physical==1)&&
      (pFontBase->pFontArray[FontID].refcount==1)){
    /* Now handle the type 1 data: */
    if (pFontBase->pFontArray[FontID].pType1Data!=NULL){
      /* First: VM, which includes CharStrings, Private, .... */
      free(pFontBase->pFontArray[FontID].vm_base); 
      /* .. then the struct itself: */
      free(pFontBase->pFontArray[FontID].pType1Data);
      pFontBase->pFontArray[FontID].pType1Data=NULL;
    }
    
    /* afm-data is yet there -> */
    if (pFontBase->pFontArray[FontID].pAFMData!=NULL){
      result=FreeAFMData(pFontBase->pFontArray[FontID].pAFMData);
      pFontBase->pFontArray[FontID].pAFMData=NULL;
    }
  }
  else{
    if (pFontBase->pFontArray[FontID].physical==1){
      /* font is physical and is referred to by other fonts ->
	 Do nothing further and return number of references: */

      return(pFontBase->pFontArray[FontID].refcount - 1);
    }
  }
  
  /* If we get here and the font is logical, we have to
     decrement the refcount of the referred physical font */
  if (pFontBase->pFontArray[FontID].physical==0){
    pFontBase->pFontArray[pFontBase->pFontArray[FontID].refcount].refcount--;
  }
  
  
  /* Set remaining area explicitly to 0 (all but pFontFileName and
     pAfmFileName!) */
  pFontBase->pFontArray[FontID].pAFMData=NULL;
  pFontBase->pFontArray[FontID].pType1Data=NULL;
  pFontBase->pFontArray[FontID].pEncMap=NULL;
  pFontBase->pFontArray[FontID].pKernMap=NULL;
  pFontBase->pFontArray[FontID].pFontEnc=NULL;
  pFontBase->pFontArray[FontID].pFontSizeDeps=NULL;
  pFontBase->pFontArray[FontID].vm_base=NULL;
  pFontBase->pFontArray[FontID].FontMatrix[0]=0.0;
  pFontBase->pFontArray[FontID].FontMatrix[1]=0.0;
  pFontBase->pFontArray[FontID].FontMatrix[2]=0.0;
  pFontBase->pFontArray[FontID].FontMatrix[3]=0.0;
  pFontBase->pFontArray[FontID].FontTransform[0]=0.0;
  pFontBase->pFontArray[FontID].FontTransform[1]=0.0;
  pFontBase->pFontArray[FontID].FontTransform[2]=0.0;
  pFontBase->pFontArray[FontID].FontTransform[3]=0.0;
  pFontBase->pFontArray[FontID].slant=0.0;
  pFontBase->pFontArray[FontID].extend=0.0;
  pFontBase->pFontArray[FontID].UndrLnPos=0.0;
  pFontBase->pFontArray[FontID].UndrLnThick=0.0;
  pFontBase->pFontArray[FontID].OvrLnPos=0.0;
  pFontBase->pFontArray[FontID].OvrLnThick=0.0;
  pFontBase->pFontArray[FontID].OvrStrkPos=0.0;
  pFontBase->pFontArray[FontID].OvrStrkThick=0.0;
  pFontBase->pFontArray[FontID].physical=0;
  pFontBase->pFontArray[FontID].refcount=0; 
  pFontBase->pFontArray[FontID].space_position=0; 
  pFontBase->pFontArray[FontID].info_flags=0; 

  return(0);
  
}



/* FreeAFMData(): Give all memory used by afm-Information back to the
   system. */
int FreeAFMData( FontInfo *pAFMData)
{
  if (pAFMData != NULL){
    if (pAFMData->gfi != NULL){
      free(pAFMData->gfi->afmVersion); pAFMData->gfi->afmVersion = NULL;
      free(pAFMData->gfi->fontName); pAFMData->gfi->fontName = NULL;
      free(pAFMData->gfi->fullName); pAFMData->gfi->fullName = NULL;
      free(pAFMData->gfi->familyName); pAFMData->gfi->familyName = NULL;
      free(pAFMData->gfi->weight); pAFMData->gfi->weight = NULL;
      free(pAFMData->gfi->version); pAFMData->gfi->version = NULL;
      free(pAFMData->gfi->notice); pAFMData->gfi->notice = NULL;
      free(pAFMData->gfi->encodingScheme); pAFMData->gfi->encodingScheme = NULL;
      free(pAFMData->gfi); pAFMData->gfi = NULL;
    }

    if (pAFMData->cwi != NULL){
      free(pAFMData->cwi); pAFMData->cwi = NULL; 
    }
    
    if (pAFMData->cmi != NULL){ 
      int i = 0;
      CharMetricInfo *temp = pAFMData->cmi;
      Ligature *node = temp->ligs;
      for (i = 0; i < pAFMData->numOfChars; ++i){
	for (node = temp->ligs; node != NULL; node = node->next){
	  free(node->succ); node->succ = NULL;
	  free(node->lig); node->lig = NULL;
	}
	free(temp->name); temp->name = NULL;
	temp++;
      }
      free(pAFMData->cmi); pAFMData->cmi = NULL;
    }

    if (pAFMData->tkd != NULL){
      free(pAFMData->tkd); pAFMData->tkd = NULL;
    }
      
    if (pAFMData->pkd != NULL){
      int i = 0;
      for (i = 0; i < pAFMData->numOfPairs; ++i){
	free(pAFMData->pkd[i].name1); pAFMData->pkd[i].name1 = NULL;
	free(pAFMData->pkd[i].name2); pAFMData->pkd[i].name2 = NULL;
      }
      free(pAFMData->pkd); pAFMData->pkd = NULL;
    }

    if (pAFMData->ccd != NULL){
      int i = 0, j = 0;
      CompCharData *ccd = pAFMData->ccd;
      for (i = 0; i < pAFMData->numOfComps; ++i){
	for (j = 0; j < ccd[i].numOfPieces; ++j){
	  free(ccd[i].pieces[j].pccName); 
	  ccd[i].pieces[j].pccName = NULL;
	}
	free(ccd[i].ccName); ccd[i].ccName = NULL;
      }
      free(pAFMData->ccd); pAFMData->ccd = NULL;
    }
    free(pAFMData);
  }
  return(0);
  
} 

