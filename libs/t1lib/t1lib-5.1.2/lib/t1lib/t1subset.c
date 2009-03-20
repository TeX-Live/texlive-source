/*--------------------------------------------------------------------------
  ----- File:        t1subset.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions for subsetting type 1 fonts.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2005.
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
  
#define T1SUBSET_C


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
#include "../type1/paths.h"
#include "../type1/regions.h"


#include "t1types.h"
#include "t1extern.h"
#include "t1finfo.h"
#include "t1misc.h"
#include "t1base.h"
#include "t1delete.h"
#include "t1subset.h"

/* Segment header for pfb-files (reminder):

   Segment-header:

   Byte 1:    always 128 (0x80)
   
   Byte 2:    1 = ASCII text
              2 = Binary text
	      3 = End of file indicator (EOF)
   
   Byte 3:    least significant byte \
   Byte 4:                            \   Length of 
   Byte 5:                            /   data segment in bytes
   Byte 6:    most significant byte  /
  
 */
#define SEGMENT_ASCII       1
#define SEGMENT_BINARY      2
#define SEGMENT_EOF         3


extern psobj *StdEncArrayP;       /* For accessing StandardEncoding */

static char *charstringP;
static int  charstringL;


/* We define two lookup tables to make a fast conversion from
   binary bytes to ASCII-hex bytes. */
static unsigned char highHexByte[256]={
  0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
  0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,
  0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,
  0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,
  0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34,
  0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,
  0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,0x36,
  0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,0x37,
  0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,
  0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,
  0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
  0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,
  0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,0x43,
  0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
  0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,
  0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46
};
static unsigned char lowHexByte[256]={
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46
};
  

/* parameters for eexec encryption */
static unsigned short int eexec_r;
static unsigned short int eexec_c1=52845;
static unsigned short int eexec_c2=22719;

/* Eexec-encrption according to the Adobe Black Book */
static unsigned char EexecEncrypt( unsigned char plain) 
{
  
  unsigned char cipher;
  
  cipher = (plain ^ (eexec_r>>8));
  eexec_r = (cipher + eexec_r) * eexec_c1 + eexec_c2;
  return( cipher);
}



static int locateCharString( int FontID, char *charname) 
{

  int namelen;
  int dictlen;
  int i;
  psdict *CharStringsP;
  

  namelen=strlen( charname);

  CharStringsP=pFontBase->pFontArray[FontID].pType1Data->CharStringsP;
  
  dictlen=CharStringsP[0].key.len;
  for ( i=1; i<=dictlen; i++) {
    if (namelen==CharStringsP[i].key.len) {
      /* This could be the charstring in question */
      if (strncmp( charname,
		   CharStringsP[i].key.data.nameP,
		   namelen)==0) {
	/* It is the the desired charstring */
	charstringP=CharStringsP[i].value.data.valueP;
	charstringL=CharStringsP[i].value.len;
	return( i);
      }
    }
  }
  /* charstring not found ??? */
  return( 0);
  
}



static int make_pfb_segment_header( char *ptr, int type, unsigned long size) 
{
  ptr[0]=(char) 0x80;
  ptr[1]=type;
  if (type==3)
    return( 2);
  ptr[2]=(char)(size & 0xFF);
  ptr[3]=(char)((size>>8) & 0xFF);
  ptr[4]=(char)((size>>16) & 0xFF);
  ptr[5]=(char)((size>>24) & 0xFF);
  return( 6);
}




char *T1_SubsetFont( int FontID,
		     char *mask,
		     unsigned int flags,
		     int linewidth,
		     unsigned long maxblocksize,
		     unsigned long *bufsize)
{
  
  FILE *ifp;
  static char *filebuf=NULL;
  unsigned long filesize=0;
  int filesegs=0;
  
  char *outbuf=NULL;
  unsigned long outsize=0;
  char *encryptbuf=NULL;
  unsigned long encryptsize=0;
  int encryptsegs=0;
  char *trailerbuf=NULL;
  unsigned long trailersize=0;
  int trailersegs=0;
  char linebuf[1025];
  
  char *csdone;  /* stores which charstrings already have been written */
  int currstring_no=0;
  char *charnameP;
  char charnamebuf[257];
  unsigned char cipher;
  char rdstring[3];
  char ndstring[3];

  int retval=1;

  /* Indices. Could save a few but debugging becomes easier this way */
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  int m=0;
  int n=0;
  int o=0;
  int p=0;
  
  
  int notdefencoded=0;
  int stdenc=0;
  int reencode=0;
  int colcount=0;
  int tr_len=-1;
  int encrypt=1;      /* 1=ASCII-hex, 2=Binary, 0=None (for debugging) */
  int dindex=0;
  int nocharstrings=0;
  char encmask[256];  /* Mask after resolving composite characters */
  T1_COMP_CHAR_INFO* cci = NULL;

  /* variables for checking SEAC's */
  int qs_num = 0;
  unsigned char qs_piece1 = 0;
  unsigned char qs_piece2 = 0;
  
  
  /* Otherwise we would get invalid accesses later */
  if (T1_CheckForFontID(FontID)!=1) {
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  } 


  /* Check parameters */
  if ( (mask==NULL) || (bufsize==NULL) || (linewidth < 8) ||
       (linewidth > 1024) || (maxblocksize < 4) ) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  } 

  /* Reset resulting encoding mask */ 
  for ( j=0; j<256; j++) {
    encmask[j] = 0;
  }
  
  /* Build encmask, a mask, where all required elementary character
     definitions required are tagged. Font internal SEAC definitions
     are evaluated at first priority, and as a fallback, also user
     specified composite character data is taken into account.
  */
  for ( j=0; j<256; j++) {
    if ( mask[j] != 0 ) {
      /* In any case, tag slot j itself */
      encmask[j] |= 0x01;
      /* Now check against correctly defined SEAC (Standard
	 Encoding Accented Character). */
      if ( (qs_num = T1int_QuerySEAC( FontID, 
				      j,
				      &qs_piece1,
				      &qs_piece2
				      )) > 1 ) {
	/* We have a SEAC --> reserve 2 additional slots, 
	   the basechar and the accent. The index numbers in this case
	   always refer to to StandardEncoding, which is not necessarily
	   the font's current encoding. We indicate this by using bit 2
	   0x2 inverse sign. */
	encmask[qs_piece1] |= 0x2;
	encmask[qs_piece2] |= 0x2;
      }
      else {
	/* OK, it is no a SEAC, but we still check against AFM composite character
	   data definition. This is non-standard, but we give the user the chance
	   to access all single character definitions in order construct arbitrary
	   composite characters. */
	if ( cci != NULL ) {
	  T1_FreeCompCharData( cci);
	}
	cci = T1_GetCompCharData( FontID, j);
	
	if ( (cci != NULL) && (cci->numPieces > 1) ) {
	  /* Tag all components that are required to construct
	     the composite character j. */
	  for ( p=0; p<cci->numPieces; p++) {
	    encmask[cci->pieces[p].piece] |= 0x1;
	  }
	}
      }
    } /* if ( mask[j] != 0 ) */
  } /* for ...  mask */ 


  /* Inspect encmask, get number of atomic charstrings and check for
     ".notdef" being encoded. */
  for ( j=0; j<256; j++) {
    if ( (encmask[j] & 0x01) != 0 ) {
      ++nocharstrings;
      /* Obtain name from current encoding */
      charnameP=T1_GetCharName( FontID, j);
    }
    if ( (encmask[j] & 0x02) != 0 ) {
      ++nocharstrings;
      /* Obtain name from StandardEncoding */
      strncpy( charnamebuf, (StdEncArrayP[j]).data.nameP, (StdEncArrayP[j]).len);
      charnamebuf[StdEncArrayP[j].len] = '\0';
      charnameP=charnamebuf;
    }
    if ( encmask[j] != 0 ) {
      
      if ( strcmp(charnameP, ".notdef") == 0 ) {
	notdefencoded=1;
      }
    }
  }
  

  /* adjust encrypting type according to flags. Default is ASCII-hex
     encryption because the output may be verbatim inserted into a
     PostScript-file. */
  if (flags & T1_SUBSET_ENCRYPT_BINARY)
    encrypt=2;
  else if (flags & T1_SUBSET_ENCRYPT_NONE)
    encrypt=0;
  
  /* Open and initialize scanning machinery */
  T1io_reset();
  if ((ifp = (FILE *)T1Open(T1_GetFontFilePath( FontID), "rb"))==NULL) {
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(NULL);
  }

  if (encrypt==0)
    sprintf( err_warn_msg_buf, "Subsetting Font %d, flags=0x%X, non-encrypted debug output",
	     FontID, flags);
  else if (encrypt==1)
    sprintf( err_warn_msg_buf, "Subsetting Font %d, flags=0x%X, ASCII-hex output with linelength %d",
	     FontID, flags, linewidth);
  else if (encrypt==2)
    sprintf( err_warn_msg_buf, "Subsetting Font %d, flags=0x%X, Binary output with maximum blocksize %lu",
	     FontID, flags, maxblocksize);
  T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
	       T1LOG_DEBUG);
  
  /* Get size of file */
  filesize=T1GetFileSize( ifp);
  sprintf( err_warn_msg_buf, "Source file %s is %lu bytes long",
	   T1_GetFontFilePath( FontID), filesize);
  T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
	       T1LOG_DEBUG);
  if ((filebuf=(char *)calloc( filesize, sizeof(char)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return( NULL);
  }

  while (retval>0) {
    /* get a line from the file. We have to read enough characters to
       ensure that "eexec" does not get split between two reads.
       Otherwise, decryption would not be started. */
    retval=T1Gets(&(filebuf[i]), 1025, ifp);
    
    i+=retval;
    if ( (dindex==0) && (T1GetDecrypt()>0) ) {
      dindex=i-retval; /* from this point on we have decrypted bytes */
      sprintf( err_warn_msg_buf, "Human-readable header finished (%d data bytes)",
	       dindex);
      T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		   T1LOG_DEBUG);
    }
    
    /* Encoding handling follows.
       Note: If the font file defines StandardEncoding (as we will check shortly),
             but the font has been reencoded by t1lib, we enforce reencoding
	     in any case so that all characters are accessible eventually in
	     the subset.
     */
    if ( pFontBase->pFontArray[FontID].pFontEnc != NULL ) {
      T1_PrintLog( "T1_SubsetFont()", "Font is reencoded by t1lib, enforcing Reencoding of subset",
		   T1LOG_DEBUG);
      /* Set flag to enforce reenocde in case of StandardEncoding */
      flags |= T1_SUBSET_FORCE_REENCODE;
      /* Reset flag to enforce reenocde in case of StandardEncoding */
      flags &= ~T1_SUBSET_SKIP_REENCODE;
    }
    
    if (strstr( &(filebuf[i-retval]), "/Encoding")!=NULL) {
      if (strstr( &(filebuf[i-retval]), "StandardEncoding")!=NULL) {
	stdenc=1;

	if ((flags & T1_SUBSET_FORCE_REENCODE)!=0) {
	  /* we ignore the current line ... */
	  i-=retval;
	  /* and put the encoding header into the buffer */
	  i+=sprintf( &(filebuf[i]),
		      "/Encoding 256 array\n0 1 255 {1 index exch /.notdef put} for\n"
		      );
	  reencode=1;
	}
	else {
	  T1_PrintLog( "T1_SubsetFont()", "Leaving StandardEncoding untouched",
		       T1LOG_DEBUG);
	  reencode=0;
	}
      }
      else {
	/* The encoding is explicitly defined in the font file. We skip copying 
	   the whole definition because we will reencode the subset later, unless
	   reencoding should be skipped by means of the flag settings. If the font
	   has been reencoded by t1lib, we enforce reencoding in order to ensure
	   that all characters in the subset will be accessible (see above). */
	stdenc=0;
	retval=T1Gets(&(filebuf[i]), 1025, ifp);
	i+=retval;
	while (retval>0) {
	  /* get a line and check for end of encoding definition. */
	  retval=T1Gets(&(filebuf[i]), 1025, ifp);
	  if ((flags & T1_SUBSET_SKIP_REENCODE)!=0) { /* we store the encoding
							 defined here */
	    i+=retval;
	    reencode=0;
	  }
	  else {
	    reencode=1;
	  }
	  if ( (dindex==0) && (T1GetDecrypt()>0) ) {
	    dindex=i; /* from this point we have decrypted bytes */
	  }  
	  if (strstr( &(filebuf[i-retval]), "readonly def")!=NULL) {
	    break;
	  }
	}
	if (reencode==0)
	  T1_PrintLog( "T1_SubsetFont()", "Preserving FontSpecific Encoding",
		       T1LOG_DEBUG);
      }
      
      /* At this point, if required, the actual encoding definition
	 follows. */
      if ( reencode!=0) {
	k=0;

	/* Write actually required encoding slots. We only encode the
	   characters required from the current fonts encoding. Characters
	   from StandardEncoding required by composite chars are not encoded. */
	for ( j=0; j<256; j++) {
	  if ( (encmask[j] & 0x01) !=0 ) {
	    charnameP=T1_GetCharName( FontID, j);
	    i+=sprintf( &(filebuf[i]), "dup %d /%s put\n", j,
			charnameP);
	    k++;
	  }
	}

	/* finish encoding definition */
	i+=sprintf( &(filebuf[i]), "readonly def\n");
	sprintf( err_warn_msg_buf, "Encoded %d characters",
		 k);
	T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		     T1LOG_DEBUG);
	k=0;
      }
      
    } /* end of    if (...encoding handling...)    */

    /* Extract the names that are used for the charstring definitions.
       We will later need them! */
    if (strstr( &(filebuf[i-retval]), "/RD")!=NULL) {
      sprintf( rdstring, "RD");
    }
    if (strstr( &(filebuf[i-retval]), "/ND")!=NULL) {
      sprintf( ndstring, "ND");
    }
    if (strstr( &(filebuf[i-retval]), "/-|")!=NULL) {
      sprintf( rdstring, "-|");
    }
    if (strstr( &(filebuf[i-retval]), "/|-")!=NULL) {
      sprintf( ndstring, "|-");
    }
      
    if (strstr( &(filebuf[i-retval]), "/CharStrings")!=NULL) {
      /* replace dictionary with one of an appropriate size: */
      i -=retval;
      sprintf( err_warn_msg_buf, "Private dictionary finished (%u data bytes)",
	       i-dindex);
      T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		   T1LOG_DEBUG);

      /* if .notdef is not in the encoding mask, we have to reserve
	 room for the additional charstring .notdef. Note that still
	 nocharstrings is an upper bound estimation, which is reached
	 in cases where no characters are encoded more than one time. */
      if ( notdefencoded == 0 ) {
	nocharstrings++; 
      }
      
      i+=sprintf( &(filebuf[i]), "2 index /CharStrings %d dict dup begin\n",
		  nocharstrings);
      break;
    }
  } /* the portion until Charstrings-dict is now read in */
  
  
  /* We now have to write the CharStrings.
     Each charstring must be written once, even if the respective 
     character appears more than once in the encoding. So we set up
     table to remember which charstrings already have been written.

     Note: The indices returned by locateCharString() range from 1 to n, so that
     we have to decrement the index when filling the csdone array!
  */
  if (( csdone=(char *)calloc( pFontBase->pFontArray[FontID].pType1Data->CharStringsP[0].key.len,
			       sizeof(char)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    free( filebuf);
    T1Close( ifp);
    return( NULL);
  }
  
  /* The .notdef character is in force! */
  if ((currstring_no=locateCharString( FontID, ".notdef"))==0) {
    T1_errno=T1ERR_UNSPECIFIED;
    free( csdone);
    T1Close( ifp);
    return( NULL);
  }
  i+=sprintf( &(filebuf[i]), "/.notdef %d %s ", charstringL, rdstring);
  memcpy(&(filebuf[i]), charstringP, charstringL);
  i+=charstringL;
  i+=sprintf( &(filebuf[i]), " %s\n", ndstring);
  csdone[currstring_no-1]=1;
  /* Now, step through the specifier matrix and write only the
     necessary charstrings. */
  for ( j=0; j<256; j++) {
    if (encmask[j]!=0) {
      if ( (encmask[j] & 0x01) != 0 ) {
	/* Obtain name from current encoding */
	charnameP=T1_GetCharName( FontID, j);
      }
      else {
	/* Obtain name from StandardEncoding */
	strncpy( charnamebuf, (StdEncArrayP[j]).data.nameP, (StdEncArrayP[j]).len);
	charnamebuf[StdEncArrayP[j].len] = '\0';
	charnameP=charnamebuf;
      }
      if ((currstring_no=locateCharString( FontID, charnameP))==0) {
	/* Atomic character not found. This is mysterious, but causes no harm
	   because .notdef will be substituted */
	sprintf( err_warn_msg_buf, "Could not locate CS ""%s"" for index %d",
		 charnameP, j);
	T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		     T1LOG_WARNING);
	continue;
      }
      /* Process charstring only if it has not already been done */
      if (csdone[currstring_no-1]==0) {
	k=i;
	i+=sprintf( &(filebuf[i]), "/%s %d %s ", charnameP, charstringL, rdstring);
	memcpy(&(filebuf[i]), charstringP, charstringL);
	i+=charstringL;
	i+=sprintf( &(filebuf[i]), " %s\n", ndstring);
	csdone[currstring_no-1]=1;
	sprintf( err_warn_msg_buf,
		 "Processing of CS ""%s"" for index %d (EncMaskFlag=0x%X) successful (len=%d bytes, line=%d bytes)",
		 charnameP, j, encmask[j], charstringL, i-k);
	T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		     T1LOG_DEBUG);
      }
      else {
	sprintf( err_warn_msg_buf, "Skipped multiple processing of CS ""%s"" (index %d)",
		 charnameP, j);
	T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		     T1LOG_DEBUG);
      }
    }
  }

  /* Get rid of temporary data */
  if (csdone!=NULL) {
    free( csdone);
    csdone = NULL;
  }
  if ( cci != NULL ) {
    free( cci);
    cci = NULL;
  }

  /* All charstrings are written. Some PostScript code follows */
  i+=sprintf( &(filebuf[i]),
	      "end\nend\nreadonly put\nnoaccess put\ndup /FontName get exch definefont pop\nmark currentfile closefile\n");
  sprintf( err_warn_msg_buf,
	   "Charstrings and Font definition finished (%d data bytes, from which %d bytes will be encrypted)",
	   i, i-dindex);
  T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
	       T1LOG_DEBUG);
  
  /* we compute the size of the encrypted area. */
  if (encrypt==1) {
    encryptsize=i-dindex;
    encryptsize+=4;       /* the four random bytes */
    encryptsize*=2;       /* since we use ASCII-hex output */
    encryptsize+=(int)ceil((double)encryptsize/linewidth); /* we need a few newline characters
					     to adjust the output format */
  }
  else if (encrypt==2) { /* binary encryption. The whole data is contiguous
			    and only at the end of the data a newline is
			    added. */
    encryptsize=i-dindex+1;
    encryptsize+=4;       /* the four random bytes */
  }
  else { /*The case of no encryption does not produce
	   valid fonts, it is for debugging purposes only */
    encryptsize=i-dindex+1;
  }
  
  
  /* we add the size of the 512 zeros and the cleartomark */
  trailersize+=ZEROS;
  trailersize+=(int)ceil((double)ZEROS/linewidth);
  trailersize+=12;                /* cleartomark */

  /* Search for remaining PostScript code in the last 1024 bytes. This
     should suffice for any font. */
  tr_len=T1GetTrailer( linebuf, 1025, ifp);
  T1Close(ifp);  /* we do not need the file any longer */
  
  /* And we add the size of the trailer. A trailer only consisting of
     a newline is ignored because the newline has already been included
     in the cleartomark-string. */
  if ( !((tr_len==1) && ((linebuf[0]=='\n') || (linebuf[0]=='\r')) ) ) {
    trailersize+=tr_len;
  }
  
  
  if ((encryptbuf=(char *)calloc( encryptsize, 1))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    free( filebuf);
    return( NULL);
  }
  
  /* Allocate one byte in orer to be able to use sprintf() (which appends
     an ASCII-\0).*/
  if ((trailerbuf=(char *)calloc( trailersize+1, 1))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    free( filebuf);
    free( encryptbuf);
    return( NULL);
  }
  
  k=0;
  colcount=0;

  /* perform eexec-encryption */
  eexec_r=55665;
  if (encrypt==0) {
    for (j=dindex; j<i; j++) {
      encryptbuf[k++]=filebuf[j];
    }
  }
  else if ( encrypt==1) {
    /* consume four random bytes. We take the numbers '0', '1', '2'
       and '3'. The first cipher then is 217-dec (0xD9-hex), i.e., it
       is no white space character. The resulting font can thus be
       converted to pfb-format without the risk violating the Adobe spec.*/
    for (j=0; j<4; j++) { 
      cipher=EexecEncrypt( (unsigned char) j);  
      encryptbuf[k++]=highHexByte[cipher];
      if (++colcount==linewidth) {
	colcount=0;
	encryptbuf[k++]='\n';
      }
      encryptbuf[k++]=lowHexByte[cipher];
      if (++colcount==linewidth) {
	colcount=0;
	encryptbuf[k++]='\n';
      }
    }
    for (j=dindex; j<i; j++) {
      cipher=EexecEncrypt( (unsigned char) filebuf[j]);
      encryptbuf[k++]=highHexByte[cipher];
      if (++colcount==linewidth) {
	colcount=0;
	encryptbuf[k++]='\n';
      }
      encryptbuf[k++]=lowHexByte[cipher];
      if (++colcount==linewidth) {
	colcount=0;
	encryptbuf[k++]='\n';
      }
    }
    /* If necessary, append newline char */
    if (encryptbuf[k-1]!='\n') {
      encryptbuf[k++]='\n';
    }
  } /* if encrypt==1 */
  else { /* four random bytes ... */
    for (j=0; j<4; j++) { 
      encryptbuf[k++]=EexecEncrypt( (unsigned char) j);  
    }
    for (j=dindex; j<i; j++) { /* and encrypted charstrings */
      encryptbuf[k++]=EexecEncrypt( (unsigned char) filebuf[j]);  
    }
  } /* encrypt == 2 */
  /* Encryption finished */
  
  /* If necessary, append newline char */
  if (encryptbuf[k-1]!='\n') {
    encryptbuf[k++]='\n';
  }

  colcount=0;
  /* Append zeroes and cleartomark */
  for (j=0; j<ZEROS; j++) {
    trailerbuf[l++]='0';
    if (++colcount==linewidth) {
      colcount=0;
      trailerbuf[l++]='\n';
    }
  }
  /* If necessary, append newline char */
  if (trailerbuf[l-1]!='\n') {
    trailerbuf[l++]='\n';
  }
  l+=sprintf( &(trailerbuf[l]), "cleartomark\n");
  if ( !((tr_len==1) && ((linebuf[0]=='\n') || (linebuf[0]=='\r')) ) ) {
    sprintf( err_warn_msg_buf,
	     "Including additional PostScript trailer (%d bytes)",
	     tr_len);
    T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		 T1LOG_DEBUG);
    l+=sprintf( &(trailerbuf[l]), linebuf); /* contains the PostScript trailer */
  }
  
  /* compute size of output file */
  outsize=0;
  if (encrypt==2) { /* binary encryption, take care about pfb-blocks */ 
    /* each segment requires 6 bytes for the header */
    filesegs=(int)(ceil((double)dindex/maxblocksize));
    encryptsegs=(int)(ceil((double)k/maxblocksize));
    trailersegs=(int)(ceil((double)l/maxblocksize));
    /* trainling ASCII-part: */
    outsize +=dindex;
    /* binary encrypted data */
    outsize +=k;
    /* leading ASCII-part: */
    outsize +=l;
    /* segment headers */
    outsize +=6*(filesegs+encryptsegs+trailersegs);
    outsize +=2;  /* the EOF-marker segment */
  }
  else { /* ASCII-hex encryption or no encryption: no segmentation */
    outsize=dindex+k+l;
  }
  
  if ((outbuf=(char *)calloc( outsize, 1))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    free( filebuf);
    free( encryptbuf);
    free( trailerbuf);
    return( NULL);
  }



  /* The full bufsize is given as the unencrypted portion plus size
     of encryptbuf (which also inccorporates an unencrypted trailer).
     In addition, the segment headers have to be taken into account for
     pfb-format. */
  i=0;
  if (encrypt==2) { /* we have to add segment headers */
    T1_PrintLog( "T1_SubsetFont()", "Setting up segmented binary buffer (pfb-format)",
		 T1LOG_DEBUG);
    /* Clear text header */
    m=0;
    for ( j=0; j<filesegs; j++) {
      if (j==filesegs-1)
	n=dindex % maxblocksize;
      else
	n=maxblocksize;
      i +=make_pfb_segment_header( &(outbuf[i]), SEGMENT_ASCII, n);
      memcpy( &(outbuf[i]), &(filebuf[m]), n);
      i +=n;
      m +=n;
    }
    sprintf( err_warn_msg_buf,
	     "    Readable header (%d bytes: %d data bytes in %d ASCII-segments of maximum size %lu bytes)",
	     i, dindex, filesegs, maxblocksize);
    T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		 T1LOG_DEBUG);
    /* Binary data */
    m=0;
    o=i;
    for ( j=0; j<encryptsegs; j++) {
      if (j==encryptsegs-1)
	n=k % maxblocksize;
      else
	n=maxblocksize;
      i +=make_pfb_segment_header( &(outbuf[i]), SEGMENT_BINARY, n);
      memcpy( &(outbuf[i]), &(encryptbuf[m]), n);
      i +=n;
      m +=n;
    }
    sprintf( err_warn_msg_buf,
	     "    Binary data (%d bytes: %d data bytes in %d binary segments of maximum size %lu bytes)",
	     i-o, k, encryptsegs, maxblocksize);
    T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		 T1LOG_DEBUG);
    /* Readable ASCII-trailer */
    m=0;
    o=i;
    for ( j=0; j<trailersegs; j++) {
      if (j==trailersegs-1)
	n=l % maxblocksize;
      else
	n=maxblocksize;
      i +=make_pfb_segment_header( &(outbuf[i]), SEGMENT_ASCII, n);
      memcpy( &(outbuf[i]), &(trailerbuf[m]), n);
      i +=n;
      m +=n;
    }
    sprintf( err_warn_msg_buf,
	     "    Readable trailer (%d bytes: %d data bytes in %d ASCII-segments of maximum size %lu bytes)",
	     i-o, l, trailersegs, maxblocksize);
    T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		 T1LOG_DEBUG);
    i +=make_pfb_segment_header( &(outbuf[i]), SEGMENT_EOF, 0);
    sprintf( err_warn_msg_buf,
	     "    EOF-segment marker (2 bytes)");
    T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		 T1LOG_DEBUG);
    *bufsize=i;
  } /* end of pfb-setup */
  else { /* we continuously copy the data */
    if (encrypt==1)
      sprintf( err_warn_msg_buf,
	       "Dumping output buffer (%d bytes: %d readable ASCII, %d ASCII-hex in lines of width %d, %d readable ASCII)",
	       dindex+k+l, dindex, k, linewidth, l);
    else 
      sprintf( err_warn_msg_buf,
	       "Dumping output buffer (%d bytes: %d readable ASCII, %d non-encrypted binary, %d readable ASCII)",
	       dindex+k+l, dindex, k, l);
    T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
		 T1LOG_DEBUG);
    memcpy( outbuf, filebuf, dindex);
    memcpy( &(outbuf[dindex]), encryptbuf, k);
    memcpy( &(outbuf[dindex+k]), trailerbuf, l);
    *bufsize=dindex+k+l;
  }
  
  sprintf( err_warn_msg_buf,
	   "Output buffer is %ld bytes, original file size is %lu bytes",
	   *bufsize, filesize);
  T1_PrintLog( "T1_SubsetFont()", err_warn_msg_buf,
	       T1LOG_DEBUG);
  
  
  if (encryptbuf!=NULL)
    free(encryptbuf);
  if (trailerbuf!=NULL)
    free(trailerbuf);
  if (filebuf!=NULL)
    free( filebuf);

  return( outbuf);
  
}



/* This function returns the encrypted charstring of character
   charname of font FontID. The length is saved at len */
char *T1_GetCharString( int FontID, char *charname, int *len)
{

  static char *charstring=NULL;
  
  if (T1_CheckForFontID(FontID)!=1) {
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  } 
  
  if ( (charname==NULL) || (len==0) ) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  } 

  if ((locateCharString( FontID, charname))==0) {
    *len=0;
    T1_errno=T1ERR_UNSPECIFIED;
    return( NULL);
  }
  
  if (charstring!=NULL) {
    free( charstring);
    charstring=NULL;
  }
  if ((charstring=(char *)malloc( charstringL))==NULL) {
    *len=0;
    T1_errno=T1ERR_ALLOC_MEM;
    return( NULL);
  }

  memcpy( charstring, charstringP, charstringL);
  *len=charstringL;
  return( charstring);
  
}



/* For those who really need it, this function returns the value
   of lenIV from the Private dictionary. It specifies how many
   random leading bytes are used in Charstring encryption in the
   current font. */
int T1_GetlenIV( int FontID)
{
  
  if (T1_CheckForFontID(FontID)!=1) {
    T1_errno=T1ERR_INVALID_FONTID;
    return( -2);
  } 
  return( pFontBase->pFontArray[FontID].pType1Data->Private[LENIV].value.data.integer);

}
