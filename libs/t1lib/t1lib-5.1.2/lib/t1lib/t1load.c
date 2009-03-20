/*--------------------------------------------------------------------------
  ----- File:        t1load.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2007-12-23
  ----- Description: This file is part of the t1-library. It contains
                     functions for loading fonts  and for managing size
		     dependent data.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2007. 
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
  
#define T1LOAD_C

#define ANSI_REALLOC_VM  

/* Note: On some systems, like e.g. my Linux box, realloc() frequently returns
         the identical pointer, if the memory chunk is *decreased* in size. As
	 a consequence, pointer shifting (as implemented below) would never
	 actually appear. The following definition enforces pointer shifting,
	 and hence allows to check pointer shifting on every system. Do not
	 activate this, it is meant for testing only!
*/
/* #define ANSI_REALLOC_ENFORCE_POINTERSHIFTING */

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
#include <string.h>

#include "../type1/ffilest.h"
#include "../type1/types.h"
#include "parseAFM.h" 
#include "../type1/objects.h"
#include "../type1/spaces.h"
#include "../type1/util.h"
#include "../type1/fontfcn.h"
#include "../type1/blues.h"
#include "../type1/paths.h"
#include "../type1/regions.h"


#include "t1types.h"
#include "t1extern.h"
#include "t1load.h"
#include "t1env.h"
#include "t1set.h"
#include "t1base.h"
#include "t1finfo.h"
#include "t1afmtool.h"



extern psobj *StdEncArrayP;       /* For checking of a fonts encoding */
extern char not_def[];            /* for checking the ".notdef"-string */



/* T1_LoadFont(FontID): Loads a Type1 font into memory and allocates all
   memory, necessary for this. */

int T1_LoadFont( int FontID)
{
  int i, j, k, l, m, n;
  char *FileName, *FileNamePath;
  int mode;  /* This is used by the type1-library for error reporting */   
  char *charname;
  
  /* The following vars are used for reallocation of VM */
  long tmp_size;
  float ascender;
#ifdef ANSI_REALLOC_VM
  unsigned long shift;
  unsigned long ldummy;
  char *tmp_ptr;
#endif

  struct region *area;
  struct XYspace *S;    

  /* These are for constructing the kerning lookup table: */
  PairKernData *pkd;
  METRICS_ENTRY *kern_tbl;
  int char1, char2;
  
  
  if (T1_CheckForInit()){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }
  

  i=T1_CheckForFontID(FontID);
  if (i==1)
    return(0);      /* Font already loaded */
  if (i==-1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);     /* illegal FontID */
  }
  
  /* Allocate memory for ps_font structure: */
  if ((pFontBase->pFontArray[FontID].pType1Data=(psfont *)malloc(sizeof(psfont)))==NULL){
    T1_PrintLog( "T1_LoadFont()", "Failed to allocate memory for psfont-struct (FontID=%d)",
		 T1LOG_ERROR, FontID);
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }

  /* Check for valid filename */
  if ((FileName=T1_GetFontFileName(FontID))==NULL){
    T1_PrintLog( "T1_LoadFont()", "No font file name for font %d", T1LOG_ERROR, FontID);
    return(-1);
  }
  
  /* Fetch the full path of type1 font file */
  if ((FileNamePath=intT1_Env_GetCompletePath( FileName,
					 T1_PFAB_ptr))==NULL){
    T1_PrintLog( "T1_LoadFont()", "Couldn't locate font file for font %d in %s",
		 T1LOG_ERROR, FontID, T1_GetFileSearchPath(T1_PFAB_PATH));
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(-1);
  }
  
  /* And load all PostScript information into memory */
  if (fontfcnA( FileNamePath, &mode,
		pFontBase->pFontArray[FontID].pType1Data) == FALSE){
    T1_PrintLog( "T1_LoadFont()", "Loading font with ID = %d failed! (mode = %d)",
		 T1LOG_ERROR, FontID, mode);
    free(FileNamePath);
    pFontBase->pFontArray[FontID].pType1Data=NULL;
    T1_errno=mode;
    return(-1);
  }
  free(FileNamePath);

  
  /* Set some default for FontBBox and Encoding if the font does not provide
     correct data. Strictly taken, these fonts do not adhere to the Type1
     specification. However, it is easy to work around and find reasonable
     defaults. This solution has been proposed by the Debian community (see
     http://bugs.debian.org/313236). */
  /* 1. FontBBox. We set default values of 0 which is recommended by Adobe
     in cases where the font does not make use of the SEAC primitive. Later on,
     if AFM fallback info is computed, these settings might be overwritten with
     meaningful values. */
  if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP == NULL) {
    if ((pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP =  
	 (psobj *)vm_alloc(4 * sizeof(psobj))) == NULL) {
      T1_PrintLog( "T1_LoadFont()", "Error allocating memory for fontbbox objects (FontID=%d)", 
		   T1LOG_ERROR, FontID);
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    for (n = 0; n < 4; n++) {
      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[n].type = OBJ_INTEGER;
      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[n].len = 0;
      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[n].data.integer = 0;
    }
    T1_PrintLog( "T1_LoadFont()", "Missing FontBBox, adding a trivial one in order to avoid crashes (FontID=%d)", 
		 T1LOG_WARNING, FontID);
  }
  /* 2. Encoding. In this case, we simply fallback to Standard Encoding. */
  if (pFontBase->pFontArray[FontID].pFontEnc == NULL && 
      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP == NULL) {
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.valueP = (char *) StdEncArrayP;
    T1_PrintLog( "T1_LoadFont()", "Missing, invalid  or undefined Encoding, setting up Standard Encoding in order to avoid crashes (FontID=%d)", 
		 T1LOG_WARNING, FontID);
  }
  
  
  /* Store the base address of virtual memory and realloc in order not
     to waste too much memory: */
  pFontBase->pFontArray[FontID].vm_base=vm_base; 
#ifdef ANSI_REALLOC_VM
  /* We first get the size of pointers on the current system */
  /* Get size of VM, ... */
  tmp_size=((unsigned long)vm_used - (unsigned long)vm_base); 
  /* ... realloc to that size ... */
#ifdef ANSI_REALLOC_ENFORCE_POINTERSHIFTING
  tmp_ptr=(char *)malloc( tmp_size);
  memcpy( tmp_ptr, vm_base, tmp_size);
#else
  tmp_ptr=(char *)realloc(vm_base,  tmp_size);
#endif
  /* ... and shift all pointers refering to that area */
  if (tmp_ptr > vm_base){
    shift= (unsigned long)tmp_ptr - (unsigned long)vm_base;
    sprintf( err_warn_msg_buf,
	     "Old VM at 0x%lX, new VM at 0x%lX, shifting up by %lu",
	     (unsigned long)vm_base, (unsigned long)tmp_ptr, tmp_size);
    T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
    
    /* We start by shifting the topmost pointers: */
    pFontBase->pFontArray[FontID].vm_base=tmp_ptr;
    
    ldummy=(long)(pFontBase->pFontArray[FontID].pType1Data->vm_start);
    ldummy +=shift;
    pFontBase->pFontArray[FontID].pType1Data->vm_start=(char *)ldummy;
    
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP;
    ldummy +=shift;
    pFontBase->pFontArray[FontID].pType1Data->CharStringsP=(psdict *)ldummy;
    
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Private;
    ldummy +=shift;
    pFontBase->pFontArray[FontID].pType1Data->Private=(psdict *)ldummy;
    
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP;
    ldummy +=shift;
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP=(psdict *)ldummy;
    
    ldummy=(long)(pFontBase->pFontArray[FontID].pType1Data->BluesP);
    ldummy +=shift;
    pFontBase->pFontArray[FontID].pType1Data->BluesP=(struct blues_struct *)ldummy;
    
    /* We now have to care for correcting all pointers which are in the VM
       and refer to some place in the VM! Note: Instead of selecting the
       appropriate pointer-elements of the union we simply shift the
       unspecified pointer "valueP".
       Note: The filename entry does not need to be modified since it does not
       need to be shifted since it points to memory managed by t1lib.
       */
    /* FontInfo-dictionary: All name-pointers and the pointers to all array
       types have to be shifted: */
    i=pFontBase->pFontArray[FontID].pType1Data->fontInfoP[0].key.len;
    for (j=1; j<=i; j++){
      if ((pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_ARRAY) ||
	  (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_STRING) ||
	  (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_NAME) ||
	  (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_FILE)){
	ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP;
	ldummy +=shift;
	pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP=(char *)ldummy;
      }
      /* The encoding needs special treatment: */
      if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_ENCODING){
	/* If builtin StandardEncoding is used, we do nothing here. Standard Encoding
	   is now located once for all fonts on the heap. For font-specific encodings
	   we have to move all pointers appropriately, because this is entirely located
	   in VM */
	if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.arrayP
	    == StdEncArrayP){ /* Font uses builtin StandardEncoding */
	  ;
	} 
	else{ /* Font-specific encoding */ 
	  ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP;
	  ldummy +=shift;
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP=(char *)ldummy;
	  for (k=0; k<256; k++){
	    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.arrayP[k].data.arrayP;
	    /* The ".notdef" is also static and may not be shifted (Thanks, Derek ;) */
	    if (ldummy != (unsigned long)not_def) {
	      ldummy +=shift;
	      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.arrayP[k].data.arrayP=(struct ps_obj *)ldummy;
	    }
	  }
	}
      } /* end of encoding-handling */
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].key.data.valueP;
      ldummy +=shift;
      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].key.data.valueP=(char *)ldummy;
    } /* fontinfo-dict done */
    
    /* Private-dictionary: All name-pointers and the pointers to all array
       types have to be shifted: */
    i=pFontBase->pFontArray[FontID].pType1Data->Private[0].key.len;
    for (j=1; j<=i; j++){
      if ((pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_ARRAY) ||
	  (pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_STRING) ||
	  (pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_NAME) ||
	  (pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_FILE)){
	ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Private[j].value.data.valueP;
	ldummy +=shift;
	pFontBase->pFontArray[FontID].pType1Data->Private[j].value.data.valueP=(char *)ldummy;
      }
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Private[j].key.data.valueP;
      ldummy +=shift;
      pFontBase->pFontArray[FontID].pType1Data->Private[j].key.data.valueP=(char *)ldummy;
    }
    
    /* BluesP: The entry "next" is the only pointer in blues_struct. Although it is
       not used anywhere we should shift it for correctness reasons (in case its not
       NULL)! */
    if (pFontBase->pFontArray[FontID].pType1Data->BluesP->next != NULL){
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->BluesP->next;
      ldummy +=shift;
      pFontBase->pFontArray[FontID].pType1Data->BluesP->next=(struct blues_struct *)ldummy;
    }
    
    /* The CharStrings-dictionary: Every namepointer and its corresponding
       charstring has to be shifted: */
    i=pFontBase->pFontArray[FontID].pType1Data->CharStringsP[0].key.len;
    for (j=1; j<=i; j++){
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].value.data.valueP;
      ldummy +=shift;
      pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].value.data.valueP=(char *)ldummy;
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].key.data.valueP;
      ldummy +=shift;
      pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].key.data.valueP=(char *)ldummy;
    }
    
    /* The Subroutines have also to be reorganized: */
    i=pFontBase->pFontArray[FontID].pType1Data->Subrs.len;
    /* First, shift pointer to array-start and after that the pointers to
       each command string: */
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP;
    ldummy +=shift;
    pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP=(struct ps_obj *)ldummy;
    for (j=0; j<i; j++) {
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP[j].data.valueP;
      ldummy +=shift;
      pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP[j].data.valueP=(char *)ldummy;
    }
  } /* end of if( tmp_ptr > vm_base ) */
  else if ( vm_base > tmp_ptr){
    shift= (unsigned long)vm_base - (unsigned long)tmp_ptr;
    sprintf( err_warn_msg_buf,
	     "Old VM at 0x%lX, new VM at 0x%lX, shifting down by %lu",
	     (unsigned long)vm_base, (unsigned long)tmp_ptr, tmp_size);
    T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
    
    /* We start by shifting the topmost pointers: */
    pFontBase->pFontArray[FontID].vm_base=tmp_ptr;
    
    ldummy=(long)(pFontBase->pFontArray[FontID].pType1Data->vm_start);
    ldummy -=shift;
    pFontBase->pFontArray[FontID].pType1Data->vm_start=(char *)ldummy;
    
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP;
    ldummy -=shift;
    pFontBase->pFontArray[FontID].pType1Data->CharStringsP=(psdict *)ldummy;
    
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Private;
    ldummy -=shift;
    pFontBase->pFontArray[FontID].pType1Data->Private=(psdict *)ldummy;
    
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP;
    ldummy -=shift;
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP=(psdict *)ldummy;
    
    ldummy=(long)(pFontBase->pFontArray[FontID].pType1Data->BluesP);
    ldummy -=shift;
    pFontBase->pFontArray[FontID].pType1Data->BluesP=(struct blues_struct *)ldummy;
    
    /* We now have to care for correcting all pointers which are in the VM
       and refer to some place in the VM! Note: Instead of selecting the
       appropriate pointer-elements of the union we simply shift the
       unspecified pointer "valueP".
       Note: The filename entry does not need to be modified since it does not
       need to be shifted since it points to memory managed by t1lib.
       */
    /* FontInfo-dictionary: All name-pointers and the pointers to all array
       types have to be shifted: */
    i=pFontBase->pFontArray[FontID].pType1Data->fontInfoP[0].key.len;
    for (j=1; j<=i; j++){
      if ((pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_ARRAY) ||
	  (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_STRING) ||
	  (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_NAME) ||
	  (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_FILE)){
	ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP;
	ldummy -=shift;
	pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP=(char *)ldummy;
      }
      /* The encoding needs special treatment: */
      if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.type==OBJ_ENCODING){
	/* If builtin StandardEncoding is used, we do nothing here. Standard Encoding
	   is now located once for all fonts on the heap. For font-specific encodings
	   we have to move all pointers appropriately, because this is entirely located
	   in VM */
	if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.arrayP
	    == StdEncArrayP){ /* Font uses builtin StandardEncoding */
	  ;
	} 
	else{ /* Font-specific encoding */ 
	  ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP;
	  ldummy -=shift;
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.valueP=(char *)ldummy;
	  for (k=0; k<256; k++){
	    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.arrayP[k].data.arrayP;
	    /* The ".notdef" is also static and may not be shifted (Thanks, Derek ;) */
	    if (ldummy != (unsigned long)not_def) {
	      ldummy -=shift;
	      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].value.data.arrayP[k].data.arrayP=(struct ps_obj *)ldummy;
	    }
	  }
	}
      } /* end of encoding-handling */
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].key.data.valueP;
      ldummy -=shift;
      pFontBase->pFontArray[FontID].pType1Data->fontInfoP[j].key.data.valueP=(char *)ldummy;
    } /* fontinfo-dict done */
    
    /* Private-dictionary: All name-pointers and the pointers to all array
       types have to be shifted: */
    i=pFontBase->pFontArray[FontID].pType1Data->Private[0].key.len;
    for (j=1; j<=i; j++){
      if ((pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_ARRAY) ||
	  (pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_STRING) ||
	  (pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_NAME) ||
	  (pFontBase->pFontArray[FontID].pType1Data->Private[j].value.type==OBJ_FILE)){
	ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Private[j].value.data.valueP;
	ldummy -=shift;
	pFontBase->pFontArray[FontID].pType1Data->Private[j].value.data.valueP=(char *)ldummy;
      }
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Private[j].key.data.valueP;
      ldummy -=shift;
      pFontBase->pFontArray[FontID].pType1Data->Private[j].key.data.valueP=(char *)ldummy;
    }
    
    /* BluesP: The entry "next" is the only pointer in blues_struct. Although it is
       not used anywhere we should shift it for correctness reasons (in case its not
       NULL)! */
    if (pFontBase->pFontArray[FontID].pType1Data->BluesP->next != NULL){
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->BluesP->next;
      ldummy -=shift;
      pFontBase->pFontArray[FontID].pType1Data->BluesP->next=(struct blues_struct *)ldummy;
    }
    
    /* The CharStrings-dictionary: Every namepointer and its corresponding
       charstring has to be shifted: */
    i=pFontBase->pFontArray[FontID].pType1Data->CharStringsP[0].key.len;
    for (j=1; j<=i; j++){
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].value.data.valueP;
      ldummy -=shift;
      pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].value.data.valueP=(char *)ldummy;
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].key.data.valueP;
      ldummy -=shift;
      pFontBase->pFontArray[FontID].pType1Data->CharStringsP[j].key.data.valueP=(char *)ldummy;
    }
    
    /* The Subroutines have also to be reorganized: */
    i=pFontBase->pFontArray[FontID].pType1Data->Subrs.len;
    /* First, shift pointer to array-start and after that the pointers to
       each command string: */
    ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP;
    ldummy -=shift;
    pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP=(struct ps_obj *)ldummy;
    for (j=0; j<i; j++) {
      ldummy=(long)pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP[j].data.valueP;
      ldummy -=shift;
      pFontBase->pFontArray[FontID].pType1Data->Subrs.data.arrayP[j].data.valueP=(char *)ldummy;
    }
  } /* end of if( vm_base > tmp_ptr ) */
  else{ /* VM addess has not changed during reallocation */
    sprintf( err_warn_msg_buf,
	     "Old VM and new VM at 0x%lX, no pointer-shifting",
	     (unsigned long)vm_base);
    T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
  }
#ifdef ANSI_REALLOC_ENFORCE_POINTERSHIFTING
  /* If pointer shifting had been enforced by allocating from
     scratch using malloc() free the previous vm. */
  free( vm_base);
#endif
#endif
    
  /* Generate a message how much VM the current font consumes */
  sprintf( err_warn_msg_buf,
	   "VM for Font %d: %d bytes", FontID, (int) tmp_size);
  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_STATISTIC);

  
  /* Set the matrix for common transformations to "no transformations" */
  pFontBase->pFontArray[FontID].FontTransform[0]=1.0;
  pFontBase->pFontArray[FontID].FontTransform[1]=0.0;
  pFontBase->pFontArray[FontID].FontTransform[2]=0.0;
  pFontBase->pFontArray[FontID].FontTransform[3]=1.0;

  /* Now, that the font has been loaded into memory, try to find the
     FontMatrix in the font info dictionary. If it exists, load it into
     our local fontmatrix, otherwise use a default matrix which scales to
     1/1000 (since font outlines  are defined in a 1000 point space)
     and does no further transformations. */
  if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTMATRIX].value.data.arrayP == NULL){
    pFontBase->pFontArray[FontID].FontMatrix[0]=0.001;
    pFontBase->pFontArray[FontID].FontMatrix[1]=0.0;
    pFontBase->pFontArray[FontID].FontMatrix[2]=0.0;
    pFontBase->pFontArray[FontID].FontMatrix[3]=0.001;
  }
  else{
    pFontBase->pFontArray[FontID].FontMatrix[0]= (double)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTMATRIX].value.data.arrayP[0].data.real;
    pFontBase->pFontArray[FontID].FontMatrix[1]= (double)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTMATRIX].value.data.arrayP[1].data.real;
    pFontBase->pFontArray[FontID].FontMatrix[2]= (double)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTMATRIX].value.data.arrayP[2].data.real;
    pFontBase->pFontArray[FontID].FontMatrix[3]= (double)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTMATRIX].value.data.arrayP[3].data.real;
  }

  /* Set the default values for transformation: */
  pFontBase->pFontArray[FontID].slant=0.0;
  pFontBase->pFontArray[FontID].extend=1.0;

  
  /* Now try to load afm-structures from corresponding .afm-file (if
     not suppressed by the user). */
  if ((pFontBase->t1lib_flags & T1_NO_AFM)!=0) {
    pFontBase->pFontArray[FontID].pAFMData = NULL;
    T1_PrintLog( "T1_LoadFont()",
		 "Suppressing AFM data handling on user request",
		 T1LOG_STATISTIC);
  }
  else {
    if ((i=openFontMetricsFile( FontID, 0))){
      /* Try a fallback, opening sloppy: */
      if ((i=openFontMetricsFile( FontID, 1))) {
	sprintf( err_warn_msg_buf,
		 "Alert: Error (%d) sloppy-processing afm-file for Font %d!",
		 i ,FontID);
	T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_STATISTIC);
	if ((pFontBase->pFontArray[FontID].pAFMData=
	     T1_GenerateAFMFallbackInfo(FontID))==NULL){
	  sprintf( err_warn_msg_buf,
		   "Ultimately failed to generate metrics information Font %d!",
		   FontID);
	  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_WARNING);
	}
	else {
	  pFontBase->pFontArray[FontID].info_flags |=AFM_SELFGEN_SUCCESS;
	  T1_PrintLog( "T1_LoadFont()",
		       "Generating AFM-information from fontfile successful!",
		       T1LOG_STATISTIC);
	}
      }
      else {
	pFontBase->pFontArray[FontID].info_flags |=AFM_SLOPPY_SUCCESS;
	sprintf( err_warn_msg_buf,
		 "Alert: Limited afm-information for Font %d",FontID);
	T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_STATISTIC);
      } 
    }
    else {
      pFontBase->pFontArray[FontID].info_flags |=AFM_SUCCESS;
    }
  }
  
  
  /* Now, set Encodingvector entry to default if the font's
     internal encoding is "StandardEncoding".
     */
  if (pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP
      == StdEncArrayP) {
    pFontBase->pFontArray[FontID].info_flags |=USES_STANDARD_ENCODING;
    pFontBase->pFontArray[FontID].pFontEnc=pFontBase->default_enc;
    sprintf( err_warn_msg_buf,
	     "Font %d reencoded to default",FontID);
    T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
  }
  else {
    sprintf( err_warn_msg_buf,
	     "Font %d not reencoded to default",FontID);
    T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
    pFontBase->pFontArray[FontID].pFontEnc = NULL;
  }

  
  /* If AFM-Info available we try to speed up some things: */
  if (pFontBase->pFontArray[FontID].pAFMData != NULL) {
    /* We have to fill the array that maps the current encodings' indices to the
       indices used in afm file. The interpretation has been changed in
       in t1lib-1.2. We now use positive values for indexing into the charmetrics
       array and negative values for indexing into the composite character array.
       an index of zero indicates that no metrics are defined for this character.
       This may happen because (a) not all AFM-files define metrics for the .notdef
       character, and (b) because font and AFM-file do not match. */
    if ((pFontBase->pFontArray[FontID].pEncMap=
	 (int *)calloc(256,sizeof(int)))==NULL) {
      sprintf( err_warn_msg_buf, "Error allocating memory for encoding map (FontID=%d)",
	       FontID);
      T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf,
		 T1LOG_WARNING);
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    for (i=0; i<256; i++) {
      charname=T1_GetCharName( FontID, i);
      /* in a first loop check for ordinary characters */
      for ( j=0; j<pFontBase->pFontArray[FontID].pAFMData->numOfChars; j++) {
	if (strcmp( charname,
		    pFontBase->pFontArray[FontID].pAFMData->cmi[j].name)==0) {
	  pFontBase->pFontArray[FontID].pEncMap[i]=j+1; /* index 0 is reserved! */
	  continue;
	}
      }
      /* if nothing has been found, check for composite characters */ 
      for ( j=0; j<pFontBase->pFontArray[FontID].pAFMData->numOfComps; j++) {
	if (strcmp( charname,
		    pFontBase->pFontArray[FontID].pAFMData->ccd[j].ccName)==0) {
	  pFontBase->pFontArray[FontID].pEncMap[i]=-(j+1); /* index 0 is reserved! */
	  continue;
	}
      }
    }
    
    /* For composite characters, we still have to compute the width and bbox */
    for ( j=0; j<pFontBase->pFontArray[FontID].pAFMData->numOfComps; j++) {
      /*and bounding box by ourselves. First, set up an identity charspace
	matrix and then generate an edgelist for the composite character at
	size 1000bp using no transformation and current encoding. Note: This
	action is only required when loading a font at first time, but not
	when reencoding a font. */
      S=(struct XYspace *)IDENTITY;
      S=(struct XYspace *)Permanent
	(Transform(S, pFontBase->pFontArray[FontID].FontTransform[0],
		   pFontBase->pFontArray[FontID].FontTransform[1],
		   pFontBase->pFontArray[FontID].FontTransform[2],
		   pFontBase->pFontArray[FontID].FontTransform[3]));
      
      area=fontfcnB_ByName( FontID, 0, S,
			    pFontBase->pFontArray[FontID].pAFMData->ccd[j].ccName,
			    &mode, pFontBase->pFontArray[FontID].pType1Data,
			    DO_RASTER);
      /* Store bounding box ... */
      pFontBase->pFontArray[FontID].pAFMData->ccd[j].charBBox.llx=area->xmin;
      pFontBase->pFontArray[FontID].pAFMData->ccd[j].charBBox.urx=area->xmax;
      pFontBase->pFontArray[FontID].pAFMData->ccd[j].charBBox.lly=area->ymin;
      pFontBase->pFontArray[FontID].pAFMData->ccd[j].charBBox.ury=area->ymax;
      /* ... and character width. This should be the width of the base character
	 of the composite! */
      pFontBase->pFontArray[FontID].pAFMData->ccd[j].wx=NEARESTPEL(area->ending.x);
      /* clean up. */
      KillRegion (area);
      if (S!=NULL) {
	KillSpace (S);
	S=NULL;
      }
    }
    /* We now create an encoding-specific kerning table which will speed up
       looking for kerning pairs! */
    pFontBase->pFontArray[FontID].KernMapSize=0;
    /* First, get number of defined kerning pairs: */
    k=pFontBase->pFontArray[FontID].pAFMData->numOfPairs;
    if (k>0){ /* i.e., there are any pairs */
      /* OK, it does not suffice to alloc numOfPairs METRICS_ENTRYs, because
	 a given character might be encoded at several locations and kerning
	 should still work. As a worst case estimation, we allocate 256^2
	 and realloc later. */ 
      if ((pFontBase->pFontArray[FontID].pKernMap=
	   (METRICS_ENTRY *)malloc( (256*256) *sizeof( METRICS_ENTRY)))==NULL){
	sprintf( err_warn_msg_buf, "Error allocating memory for metrics map (FontID=%d)",
		 FontID);
	T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf,
		     T1LOG_WARNING);
	T1_errno=T1ERR_ALLOC_MEM;
	return(-1);
      }
      kern_tbl=pFontBase->pFontArray[FontID].pKernMap;
      pkd=pFontBase->pFontArray[FontID].pAFMData->pkd;
      j=0;
      for ( i=0; i<k; i++) {
	/* We do not check T1_GetEncodingIndices() against the return value
	   NULL because we just loading the font in question: */
	l=0;
	while ((char1=(T1_GetEncodingIndices( FontID, pkd[i].name1))[l++])!=-1) {
	  /* pair could be relevant in current encoding */
	  m=0;
	  while ((char2=(T1_GetEncodingIndices( FontID, pkd[i].name2))[m++])!=-1) {
	    /* Since we get here we have a relevant pair -->
	       Put char1 in higher byte and char2 in LSB: */
	    kern_tbl[j].chars=(char1 << 8) | char2;
	    /* We only make use of horizontal kerning */
	    kern_tbl[j].hkern=pkd[i].xamt;
	    j++;
	  } /* while (char2) */
	} /* while (char1) */
      } /* for */
      /* We are done, realloc memory: */
      kern_tbl=(METRICS_ENTRY*) realloc( kern_tbl, j*sizeof(METRICS_ENTRY));
      /* We now sort the kerning array with respect to char indices */
      qsort( kern_tbl, (size_t) j, sizeof(METRICS_ENTRY),
	     &cmp_METRICS_ENTRY );
      /* Finally write back pointer for the case that realloc changed the
	 pointer */
      pFontBase->pFontArray[FontID].pKernMap=kern_tbl;
      pFontBase->pFontArray[FontID].KernMapSize=j;
    }
    else
      pFontBase->pFontArray[FontID].pKernMap=NULL;
  }
  else { /* no AFM data */
    pFontBase->pFontArray[FontID].pKernMap=NULL;
    pFontBase->pFontArray[FontID].pEncMap=NULL;
  }
  /* End of "if (AFM-info ..)" */
  
  
  /* We have just loaded a physical font into memory, thus .... */
  pFontBase->pFontArray[FontID].physical=1;

  /* Set reference-counter to 1: */
  pFontBase->pFontArray[FontID].refcount=1;

  /* Get the index into encoding vector where the space character is
     found. If not encoded, set space_position to -1. */
  pFontBase->pFontArray[FontID].space_position=-1;
  i=0;
  if (pFontBase->pFontArray[FontID].pFontEnc) { /* external default encoding */
    while (i<256) {
      if (strcmp( (char *)pFontBase->pFontArray[FontID].pFontEnc[i],
		  "space")==0){
	/* space found at position i: */
	pFontBase->pFontArray[FontID].space_position=i;
	break;
      }
      i++;
    }
  }
  else { /* internal encoding */
    while (i<256) {
      if (strcmp( (char *)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[i].data.arrayP,
		  "space")==0){
	/* space found at position i: */
	pFontBase->pFontArray[FontID].space_position=i;
	break;
      }
      i++;
    }
  }
  

  /* Set the lining rule parameters to default values */
  pFontBase->pFontArray[FontID].UndrLnPos=
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[UNDERLINEPOSITION].value.data.real;
  pFontBase->pFontArray[FontID].UndrLnThick=
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[UNDERLINETHICKNESS].value.data.real;

  /* We have to set the value for the typographic ascender. If possible,
     we get it from the afm-File. But be aware this value might be undefined!
     This value should in any acse explicitly be set later by the user! */
  if (pFontBase->pFontArray[FontID].pAFMData!=NULL &&
      pFontBase->pFontArray[FontID].pAFMData->gfi!=NULL) {
    ascender=(float) pFontBase->pFontArray[FontID].pAFMData->gfi->ascender;
  }
  else {
    ascender=(float) T1_GetCharBBox( FontID, T1_GetEncodingIndex( FontID, "d")).ury;
  }
  
  pFontBase->pFontArray[FontID].OvrLnPos=ascender
    + (float) abs( (double)pFontBase->pFontArray[FontID].UndrLnPos);
  pFontBase->pFontArray[FontID].OvrStrkPos=ascender / 2.0;
  pFontBase->pFontArray[FontID].OvrLnThick=pFontBase->pFontArray[FontID].UndrLnThick;
  pFontBase->pFontArray[FontID].OvrStrkThick=pFontBase->pFontArray[FontID].UndrLnThick;

  
  /* Finally, set the font size dependencies pointer to NULL since we can
     assume, that at load time of a font, no size specific data of this
     font is available.
     */
  
  pFontBase->pFontArray[FontID].pFontSizeDeps=NULL;

  /* If wanted, some debugging information is put into logfile */
  sprintf( err_warn_msg_buf, "Pointer vm_base: 0x%lX",
	   (long)pFontBase->pFontArray[FontID].vm_base);
  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "Pointer vm_start: 0x%lX",
	   (long)pFontBase->pFontArray[FontID].pType1Data->vm_start);
  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "Pointer CharStringsP: 0x%lX",
	   (long)pFontBase->pFontArray[FontID].pType1Data->CharStringsP);
  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "Pointer Private: 0x%lX",
	   (long)pFontBase->pFontArray[FontID].pType1Data->Private);
  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "Pointer fontInfoP: 0x%lX",
	   (long)pFontBase->pFontArray[FontID].pType1Data->fontInfoP);
  T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf, T1LOG_DEBUG);

  return(0);
}
  

/* openFontMetricsFile( FontID, open_sloppy): Gets the fontfilename
   corresponding to FontID, opens the corresponding afm-file and fills
   the data structures. return-value is the value returned by the
   T1lib_parseFile() function. If open_sloppy is set, the minimum
   information needed is read from AFM file. This can be considered a
   fallback for problematic AFM files. */
static int openFontMetricsFile( int FontID, int open_sloppy)
{
  char *FontFileName;
  char *AFMFileName;
  char *afm_name;
  char *AFMFileNamePath;
  
  int i, j;
  FILE *metricsfile;
  

  afm_name=T1_GetAfmFileName(FontID);
  
  if (afm_name!=NULL) { /* We have name explicitly specified */
    /* It needs to be freeable */
    if ((AFMFileName=
	 (char *)malloc( (strlen(afm_name)+1)*sizeof( char)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return( -6);
    }
    strcpy( AFMFileName, afm_name);
  }
  else {
    FontFileName=T1_GetFontFileName( FontID);
    i=strlen(FontFileName);
    j=i;
    AFMFileName=(char *)malloc( i+5);
    strcpy( AFMFileName, FontFileName);
    while ( AFMFileName[i] != '.'){
      if (i==0) break;
      else i--;
    }
    if (i==0){
      /* We have a filename without extension -> append extension */
      AFMFileName[j]='.';
      AFMFileName[j+1]='a';
      AFMFileName[j+2]='f';
      AFMFileName[j+3]='m';
      AFMFileName[j+4]='\0';
    }
    else{
      /* we found a '.' -> replace extension */
      AFMFileName[i+1]='a';
      AFMFileName[i+2]='f';
      AFMFileName[i+3]='m';
      AFMFileName[i+4]='\0';
    }
  }
  
  /* Get full path of the afm file (The case of a full path name
     name specification is valid */
  AFMFileNamePath=intT1_Env_GetCompletePath( AFMFileName, T1_AFM_ptr);
  free( AFMFileName);
  
  /* open afm-file: */
  if (AFMFileNamePath!=NULL){
    if ((metricsfile=fopen(AFMFileNamePath,"rb"))==NULL){
      free(AFMFileNamePath);
      return(-4);
    }
    else {
      free(AFMFileNamePath);
    }
  }
  else{
    return( -5);
  }
  
  /* Call procedure to read afm-file and store the data formatted.
     Flags used here: P_M  All Metrics Information
                      P_P  Pair Kerning Information
		      P_C  Composite Character Data (since t1lib V.1.2)
     The P_G flag to get global font information should not be used
     if not absolutely needed. When parsing an unknown keyword, which
     may be harmless, the T1lib_parseFile function returns the error code
     -1 (parseError). On the other hand, all other really relevant
     data may habe been parsed and stored correctly. In such a case,
     There's no way to make a serious decision whether an error has
     occured or not.
     */
  if (open_sloppy!=0)
    i=T1lib_parseFile( (FILE *) metricsfile,
		       (FontInfo **) &(FontBase.pFontArray[FontID].pAFMData),
		       P_M );
  else
    i=T1lib_parseFile( (FILE *) metricsfile,
		       (FontInfo **) &(FontBase.pFontArray[FontID].pAFMData),
		       P_G | P_M | P_P | P_C );
  fclose(metricsfile);
  return(i);
}



/* T1int_CreateNewFontSize( FontID, size): Create a new size "size" of font
   "FontID" and allocate all data necessary for this. The data
   structure is connected to the linked list of FontSizeDeps for this
   font. Returns a pointer to the newly created FontSizeDeps-struct
   if all went correct and NULL otherwise.
   Since of version 0.3 a member antialias has been added to the
   FONTSIZEDEPS structure! This can be:

   0:     bitmaps are stored in this struct
   1:     non-antialiased bytemaps are stored in this struct
   2:     low-antialiased bytemaps are stored in this struct
   4:     high-antialiased bytemaps are stored in this struct
   */
FONTSIZEDEPS *T1int_CreateNewFontSize( int FontID, float size, int aa)
{

  FONTSIZEDEPS *pFontSizeDeps, *pPrev;
  

  /* First, get to the last font size in the linked list for this font.
     The following routine returns the address of the last struct in the
     linked list of FONTSIZEDEPS or NULL if none exists. */
  pFontSizeDeps=T1int_GetLastFontSize( FontID);
  pPrev=pFontSizeDeps;
  
  
  if (pFontSizeDeps==NULL){
    /* Allocate memory for first FontSizeDeps-structure: */
    if ((pFontBase->pFontArray[FontID].pFontSizeDeps=(FONTSIZEDEPS *)malloc(sizeof(FONTSIZEDEPS)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    pFontSizeDeps=pFontBase->pFontArray[FontID].pFontSizeDeps;
  }
  else{
    /* A valid address of an existing structure was found */
    if ((pFontSizeDeps->pNextFontSizeDeps=(FONTSIZEDEPS *)malloc(sizeof(FONTSIZEDEPS)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    pFontSizeDeps=pFontSizeDeps->pNextFontSizeDeps;
  }

  /* The pointer to the previous struct */
  pFontSizeDeps->pPrevFontSizeDeps=pPrev;
  /* Put the size into this structure */
  pFontSizeDeps->size=size;
  /* Set the antialias mark: */
  pFontSizeDeps->antialias=aa;
  
  /* Just the current becomes now the last item in the linked list: */
  pFontSizeDeps->pNextFontSizeDeps=NULL;
  /* Setup CharSpaceMatrix for this font: */
  pFontSizeDeps->pCharSpaceLocal=(struct XYspace *) IDENTITY;
  /* Apply transformation with font matrix: */
  pFontSizeDeps->pCharSpaceLocal=(struct XYspace *)
    Transform(pFontSizeDeps->pCharSpaceLocal,
	      pFontBase->pFontArray[FontID].FontMatrix[0],
	      pFontBase->pFontArray[FontID].FontMatrix[1],
	      pFontBase->pFontArray[FontID].FontMatrix[2],
	      pFontBase->pFontArray[FontID].FontMatrix[3]);
  /* Apply a further transformation (optionally): */
  pFontSizeDeps->pCharSpaceLocal=(struct XYspace *)
    Transform(pFontSizeDeps->pCharSpaceLocal,
	      pFontBase->pFontArray[FontID].FontTransform[0],
	      pFontBase->pFontArray[FontID].FontTransform[1],
	      pFontBase->pFontArray[FontID].FontTransform[2],
	      pFontBase->pFontArray[FontID].FontTransform[3]);
  /* Apply desired scaling factor, and make it Permanent */
  pFontSizeDeps->pCharSpaceLocal=(struct XYspace *) Permanent
    (Scale(pFontSizeDeps->pCharSpaceLocal, size, size));
							 
  /* We should now allocate memory for the glyph area of the font
     cache: */
  if ((pFontSizeDeps->pFontCache=(GLYPH *)calloc(256,sizeof(GLYPH)))
      ==NULL)
    return(NULL);

  sprintf( err_warn_msg_buf, "New Size %f created for FontID %d (antialias=%d)",
	   pFontSizeDeps->size, FontID, pFontSizeDeps->antialias);
  T1_PrintLog( "CreateNewFontSize()", err_warn_msg_buf, T1LOG_STATISTIC);
  /* We are done */
  return(pFontSizeDeps);
  
}

  


/* T1_QueryFontSize( FontID, size, aa): Search if a requested size of font
   FontID is already existing. If so, it returns a pointer to the
   respective FontSizeDeps-structure,  otherwise NULL is returned: */
FONTSIZEDEPS *T1int_QueryFontSize( int FontID, float size, int aa)
{
  
  FONTSIZEDEPS *link_ptr;


  /* There's not yet one size: */
  if (pFontBase->pFontArray[FontID].pFontSizeDeps == NULL)
    return(pFontBase->pFontArray[FontID].pFontSizeDeps);
  

  /* There's already existing one or more size */
  link_ptr=pFontBase->pFontArray[FontID].pFontSizeDeps;
  
  while (((link_ptr->size != size)||(link_ptr->antialias != aa))
	 &&(link_ptr->pNextFontSizeDeps != NULL))
    link_ptr=link_ptr->pNextFontSizeDeps;
  
  if ((link_ptr->size != size)||(link_ptr->antialias != aa))
    return( NULL);     /* requested size/aa-combination  was not found */
  else
    return(link_ptr); /* return pointer to requested struct */
  
}

/* FONTSIZEDEPS *T1int_GetLastFontSize( FontID): Get the address of the
   last struct in the linked list of FontSizeDeps or NULL if there is
   no existing size dependent data. */
FONTSIZEDEPS *T1int_GetLastFontSize( int FontID)
{
  FONTSIZEDEPS *link_ptr, *result_ptr;
  

  /* There's not yet one size: */
  if (pFontBase->pFontArray[FontID].pFontSizeDeps == NULL)
    return((FONTSIZEDEPS *) (pFontBase->pFontArray[FontID].pFontSizeDeps));
  
  
  /* There's already existing one or more size */
  link_ptr=pFontBase->pFontArray[FontID].pFontSizeDeps;
  
  while (link_ptr != NULL){
    result_ptr=link_ptr;
    link_ptr=link_ptr->pNextFontSizeDeps;
  }

  return((FONTSIZEDEPS *)(result_ptr));
}


/* A function for comparing METRICS_ENTRY structs */
static int cmp_METRICS_ENTRY( const void *entry1, const void *entry2)
{
  if (((METRICS_ENTRY *)entry1)->chars <
      ((METRICS_ENTRY *)entry2)->chars)
    return(-1);
  if (((METRICS_ENTRY *)entry1)->chars >
      ((METRICS_ENTRY *)entry2)->chars)
    return(1);
  return(0); /* This should not happen */
}
    
    
