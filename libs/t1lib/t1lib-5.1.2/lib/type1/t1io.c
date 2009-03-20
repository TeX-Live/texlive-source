/* $XConsortium: t1io.c,v 1.4 91/10/10 11:19:41 rws Exp $ */
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
 *
 * Author: Carol H. Thompson  IBM Almaden Research Center
 */
/*******************************************************************
*  I/O package for Type 1 font reading
********************************************************************/
 
#ifndef STATIC
#define STATIC static
#endif
 
#if defined(_MSC_VER)
# include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
#endif
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "t1stdio.h"
#include "t1hdigit.h"

/* we define this to switch to decrypt-debugging mode. The stream of
   decrypted bytes will be written to stdout! This contains binary
   charstring data */
/* #define DEBUG_DECRYPTION */
/* #define DEBUG_PFB_BLOCKS  */

/* Constants and variables used in the decryption */
#define c1 ((unsigned short)52845)
#define c2 ((unsigned short)22719)
static unsigned short r;
static int asc, Decrypt;
static int extrach;
static int haveextrach;

static int starthex80=0;
static long pfbblocklen=0;
static long accu=0;
static unsigned long bytecnt=0;
static int eexec_startOK=0;
static int eexec_endOK=0;
static int in_eexec=0;

 
/* Our single FILE structure and buffer for this package */
STATIC F_FILE TheFile;
STATIC unsigned char TheBuffer[F_BUFSIZ];
 
/* Our routines */
F_FILE *T1Open(), *T1eexec();
int T1Close(F_FILE *);
int T1Read(), T1Getc(F_FILE *), T1Ungetc(int,F_FILE *);
void T1io_reset(void);
STATIC int T1Decrypt(), T1Fill();
 
/* -------------------------------------------------------------- */
/*ARGSUSED*/
F_FILE *T1Open(fn, mode)
  char *fn;    /* Pointer to filename */
  char *mode;  /* Pointer to open mode string */
{
  F_FILE *of = &TheFile;
  char c;
  
 
  Decrypt = 0;
  eexec_startOK=0;
  eexec_endOK=0;
 
#ifndef O_BINARY
#  define O_BINARY 0x0
#endif
 
  /* We know we are only reading */
  if ((of->fd=open(fn, O_RDONLY | O_BINARY)) < 0) return NULL;

  /* We check for pfa/pfb file */
  if (read( of->fd, &c, 1)!=1) {
    close( of->fd);
    return(NULL);
  }
  else
    if (c==(char)0x80){
      starthex80=1;
    }
  lseek( of->fd, 0, SEEK_SET);
  
  /* Initialize the buffer information of our file descriptor */
  of->b_base = TheBuffer;
  of->b_size = F_BUFSIZ;
  of->b_ptr = NULL;
  of->b_cnt = 0;
  of->flags = 0;
  of->error = 0;
  haveextrach = 0;
  return &TheFile;
} /* end Open */
 
/* -------------------------------------------------------------- */
int T1Getc(f)        /* Read one character */
  F_FILE *f;         /* Stream descriptor */
{
  if (f->b_base == NULL) return EOF;  /* already closed */
 
  if (f->flags & UNGOTTENC) { /* there is an ungotten c */
    f->flags &= ~UNGOTTENC;
    return (int) f->ungotc;
  }
 
  if (f->b_cnt == 0)  /* Buffer needs to be (re)filled */
    f->b_cnt = T1Fill(f);
  if (f->b_cnt > 0) return (f->b_cnt--, (int) *(f->b_ptr++));
  else {
    f->flags |= FIOEOF;
    return EOF;
  }
} /* end Getc */
 
/*  This function is added by RMz:
    T1Gets(): Read a line of the file and save it to string. At most,
    (size-1) bytes are read. The user *must* ensure (by making size large
    enough) that "eexec" does not get split between two calls because
    in this case, eexec-decryption does not set in.
    ------------------------------------------------------------ */
int T1Gets(char *string,
	   int size,
	   F_FILE *f) /* Read a line */
{
  int i=0;
  char *eexecP;
  
  if (string == NULL) {
    return( i);
  }
  if (f->b_base == NULL)
    return( i);  /* already closed */
  if (size<2)   /* no bytes to be read. For size = 1 we only had
		   room for the \0-character. */
    return( i);
  
  if (f->flags & UNGOTTENC) { /* there is an ungotten c */
    f->flags &= ~UNGOTTENC;
    string[i++]=f->ungotc;
    size--;
  }

  size--; /* we have to leave room for one \0-character */
  
  while ( size>0) {
    if (f->b_cnt == 0) { /* Buffer needs to be (re)filled */
      f->b_cnt = T1Fill(f);
    }
    if (f->b_cnt == 0) { /* no more bytes available. Put \0-char
			    and return. */
      if ( i==0) { /* we did not already store one single char to string */
	f->flags |= FIOEOF;
	return( i);
      }
      else {
	f->flags |= FIOEOF;
	string[i]='\0';
	return( i);
      }
    }

    /* do not skip white space as required by Adobe spec, because
       I have found fonts where the first encrypted byte was of
       white space type. */
    if ( (eexec_startOK==1) && (eexec_endOK==1)) {
      T1eexec( f);
      eexec_startOK=0;
      eexec_endOK=0;
      in_eexec=1;
      /* we are now in the encrypted portion. */
    }
    string[i]=*(f->b_ptr);
    
    /* Check whether eexec appears in the string just setup */
    if ( (Decrypt==0) &&
	 ((eexecP=strstr( string, "eexec"))!=NULL) ) {
      /* if eexec is an isolated token, start decryption */
      if ( (eexec_startOK==1) &&
	   (isspace( (int)string[i])!=0) ) {
	eexec_endOK=1;
      }
      if ( (eexec_startOK==0) &&
	   (isspace( (int)string[i-5])!=0) ) {
	eexec_startOK=1;
      }
    }
    i++;
    /* Under UNIX, '\n' is the accepted newline. For pfb-files it is also
       common to use '\r' as the newline indicator. I have, however, never
       seen a pfb-file which uses the sequence '\r''\n' as a newline
       indicator, as known from DOS. So we don't take care for this case
       and simply map both single characters \r and \n into \n. Of course,
       this can only be done in the ASCII section of the font.

       2002-10-26: Well, life life is teaching me better: There *are* fonts
       out there, ASCII encoded pfa's, that use the crappy DOSian 0x0d 0x0a
       sequence as line separation. In order to make it still work, we absorb
       the byte 0x0a. Failure to do so result in decryption failure. The
       workaround is implemented T1eexec():
       
    */
    if ( *(f->b_ptr)=='\n' || *(f->b_ptr)=='\r') {
      if (in_eexec==0)
	string[i-1]='\n';  
      string[i]='\0'; 
      f->b_cnt--;
      f->b_ptr++;
      return( i);
    }
    
    f->b_cnt--;
    f->b_ptr++;
    size--;
  } /* end of while (size>0) */
  
  string[i]='\0'; /* finish string */
  return( i);
  
} /* end of T1Gets() */



int T1GetDecrypt( void) 
{
  return( in_eexec);
}


/* Return the optional contents after the final cleartomark token.
   There might appear some PostScript code which is not important
   for t1lib, but which becomes important if subsetted fonts are
   embedded in PostScript files. */
int T1GetTrailer(char *string,
		 int size,
		 F_FILE *f)
{
  unsigned long off_save;
  char *buf;
  char *ctmP;
  int i=0, j;
  int datasize;
  int len;
  
  datasize=size;
  
  off_save=lseek( f->fd, 0, SEEK_CUR);
  if ((buf=(char *)malloc( size+1))==NULL ) {
    return( -1);
  }
  lseek( f->fd, -size, SEEK_END);
  read(f->fd, buf, size);
  buf[size]='\0';   /* to be ablo perform a strstr() on this memory */
  
  i=datasize;
  j=datasize-11;   /* length of "cleartomark" plus terminating white
		      space or newline */
   
  while ((j--)>-1) {
    if ((unsigned char)buf[i]==0x80) {
      datasize=i; /* we skip the segment marker of pfb-files */
    }
    if ((ctmP=strstr( &(buf[j]), "cleartomark"))!=NULL) {
      /* buf[i-1] now is the first character after cleartomark. Advance now
	 to the next non white character of EOF. */
      len = datasize - i;
      while ( (isspace( (int)(buf[i-1])) != 0) &&
	      (i < datasize) ) {
	++i;
      }
      memcpy( string, &(buf[i-1]), len);
      string[len]='\0';
      lseek( f->fd, off_save, SEEK_SET);
      free( buf);
      return len;
    }
    i--;
  }
  lseek( f->fd, off_save, SEEK_SET);
  free( buf);
  return( -1);
}



unsigned long T1GetFileSize( F_FILE *f) 
{
  unsigned long off_save;
  unsigned long filesize;
  
  off_save=lseek( f->fd, 0, SEEK_CUR);
  filesize=lseek( f->fd, 0, SEEK_END);
  lseek( f->fd, off_save, SEEK_SET);
  return( filesize);
}



/* -------------------------------------------------------------- */
int T1Ungetc(c, f)   /* Put back one character */
  int c;
  F_FILE *f;         /* Stream descriptor */
{
  if (c != EOF) {
    f->ungotc = c;
    f->flags |= UNGOTTENC;  /* set flag */
    f->flags &= ~FIOEOF;    /* reset EOF */
  }
  return c;
} /* end Ungetc */
 
/* -------------------------------------------------------------- */
int T1Read(buffP, size, n, f)  /* Read n items into caller's buffer */
  char *buffP;       /* Buffer to be filled */
  int   size;        /* Size of each item */
  int   n;           /* Number of items to read */
  F_FILE *f;         /* Stream descriptor */
{
  int bytelen, cnt, i;
  F_char *p = (F_char *)buffP;
  int  icnt;         /* Number of characters to read */
 
  if (f->b_base == NULL) return 0;  /* closed */
  icnt = (size!=1)?n*size:n;  /* Number of bytes we want */
 
  if (f->flags & UNGOTTENC) { /* there is an ungotten c */
    f->flags &= ~UNGOTTENC;
    *(p++) = f->ungotc;
    icnt--; bytelen = 1;
  }
  else bytelen = 0;
 
  while (icnt > 0) {
    /* First use any bytes we have buffered in the stream buffer */
    if ((cnt=f->b_cnt) > 0) {
      if (cnt > icnt) cnt = icnt;
      for (i=0; i<cnt; i++) *(p++) = *(f->b_ptr++);
      f->b_cnt -= cnt;
      icnt -= cnt;
      bytelen += cnt;
    }
 
    if ((icnt == 0) || (f->flags & FIOEOF)) break;
 
    f->b_cnt = T1Fill(f);
  }
  return ((size!=1)?bytelen/size:bytelen);
} /* end Read */
 
/* -------------------------------------------------------------- */
int T1Close(f)       /* Close the file */
  F_FILE *f;         /* Stream descriptor */
{
  if (f->b_base == NULL) return 0;  /* already closed */
  f->b_base = NULL;  /* no valid stream */
  return close(f->fd);
} /* end Close */
 

/* -------------------------------------------------------------- */
F_FILE *T1eexec(f)   /* Initialization */
  F_FILE *f;         /* Stream descriptor */
{
  int i;
  int H;
  
  unsigned char *p;
  int testchar;
  unsigned char randomP[8];
 
  r = 55665;  /* initial key */
  asc = 1;    /* indicate ASCII form */

#ifdef DEBUG_DECRYPTION
  printf("T1eexec(1): first 20 bytes=%.20s, b_cnt=%d\n", f->b_ptr, f->b_cnt);
#endif

  /* As the very first action we check the first byte against 0x0a.
     This mmight happen in context with the T1gets() function for
     pfa files that use DOSian linefeed style. If that character appears
     here, we absorb it (see also T1Gets()!). 
  */
  if ( ( testchar = T1Getc( f)) != 0x0a )
    T1Ungetc( testchar, f);
    
  /* Consume the 4 random bytes, determining if we are also to
     ASCIIDecodeHex as we process our input.  (See pages 63-64
     of the Adobe Type 1 Font Format book.)  */

  /* Skipping over initial white space chars has been removed since
     it could lead to unprocessable pfb-fonts if accindentally the
     first cipher text byte was of the class HWHITE_SPACE.
     Instead, we just read ahead, this should suffice for any
     Type 1 font program. (RMz, 08/02/1998) */

  /* If ASCII, the next 7 chars are guaranteed consecutive */
  randomP[0] = getc(f);  /* store first non white space char */
  fread(randomP+1, 1, 3, f);  /* read 3 more, for a total of 4 */
  /* store first four chars */
  for (i=0,p=randomP; i<4; i++) {  /* Check 4 valid ASCIIEncode chars */
    if (HighHexP[*p++] > LAST_HDIGIT) {  /* non-ASCII byte */
      asc = 0;
      break;
    }
  }
  if (asc) {  /* ASCII form, convert first eight bytes to binary */
    fread(randomP+4, 1, 4, f);  /* Need four more */
    for (i=0,p=randomP; i<4; i++) {  /* Convert */
      H = HighHexP[*p++];
      randomP[i] = H | LowHexP[*p++];
    }
  }
  
  /* Adjust our key */
  for (i=0,p=randomP; i<4; i++) {
    r = (*p++ + r) * c1 + c2;
  }

  /* Decrypt the remaining buffered bytes */
  f->b_cnt = T1Decrypt(f->b_ptr, f->b_cnt);
  Decrypt = 1;
  
#ifdef DEBUG_DECRYPTION
  printf("T1eexec(2): first 120 bytes=%.120s, b_cnt=%d\n", f->b_ptr, f->b_cnt);
#endif
  
  return (feof(f))?NULL:f;
} /* end eexec */
 
/* -------------------------------------------------------------- */
STATIC int T1Decrypt(p, len)
  unsigned char *p;
  int len;
{
  int n;
  int H=0, L=0;
  unsigned char *inp = p;
  unsigned char *tblP;
 
#ifdef DEBUG_DECRYPTION
  printf("T1_Decrypt(): called with len=%d\n",len);
#endif
  if (asc) {
    if (haveextrach) {
      H = extrach;
      tblP = LowHexP;
    }
    else tblP = HighHexP;
    for (n=0; len>0; len--) {
      L = tblP[*inp++];
#ifdef DEBUG_DECRYPTION
      printf("L=0x%X, %d, inp=%c (%d)\n", L,L, *(inp-1), *(inp-1));
#endif
      if (L == HWHITE_SPACE) {
#ifdef DEBUG_DECRYPTION	
	printf("continue\n");
#endif
	continue;
      }
      if (L > LAST_HDIGIT) {
#ifdef DEBUG_DECRYPTION
	printf("L=0x%X, --> break\n", L);
#endif
	break;
      }
      
      if (tblP == HighHexP) { /* Got first hexit value */
        H = L;
        tblP = LowHexP;
      } else { /* Got second hexit value; compute value and store it */
        n++;
        tblP = HighHexP;
        H |= L;
        /* H is an int, 0 <= H <= 255, so all of this will work */
        *p++ = H ^ (r >> 8);
        r = (H + r) * c1 + c2;
      }
    }
    if (tblP != HighHexP) {  /* We had an odd number of hexits */
      extrach = H;
      haveextrach = 1;
    } else haveextrach = 0;
#ifdef DEBUG_DECRYPTION
    printf("T1_Decrypt(): Decrypted %d bytes\n",n);
#endif
    return n;
  } else {
    for (n = len; n>0; n--) {
      H = *inp++;
      *p++ = H ^ (r >> 8);
      r = (H + r) * c1 + c2;
    }
    return len;
  }
} /* end Decrypt */
 
/* -------------------------------------------------------------- */
/* This function has been adapted to support pfb-files with multiple
   data segments */
STATIC int T1Fill(f) /* Refill stream buffer */
  F_FILE *f;         /* Stream descriptor */
{
  int rc,i;
  static unsigned char hdr_buf[6];

  if (starthex80){ /* we have a pfb-file -> be aware of pfb-blocks */
    if ( pfbblocklen-accu >= F_BUFSIZ){
      /* fill the buffer */
      rc = read(f->fd, f->b_base, F_BUFSIZ);
      bytecnt+=rc;
      accu +=rc;
    }
    else{
      if (pfbblocklen-accu>0){
	/* read the remaining of the pfb-block ... */
	rc = read(f->fd, f->b_base, pfbblocklen-accu);
	bytecnt +=rc;
	accu +=rc;
	/* ... and examine the next header */
	i=read(f->fd, hdr_buf, 6);
	bytecnt +=i;
	pfbblocklen=0;
	pfbblocklen += hdr_buf[2]&0xFF  ;
	pfbblocklen += (hdr_buf[3] & 0xFF)  <<8;
	pfbblocklen += (hdr_buf[4] & 0xFF)  <<16;
	pfbblocklen += (hdr_buf[5] & 0xFF)  <<24;
#ifdef DEBUG_PFB_BLOCKS	
	printf("t1io: New segment, length=%d, type=%d\n",
	       pfbblocklen, hdr_buf[1]);
#endif	
	accu=0;
      }
      else{
	/* We are at the beginning of a new block ->
	   examine header */
	i=read(f->fd, hdr_buf, 6);
	pfbblocklen=0;
	pfbblocklen += hdr_buf[2]&0xFF  ;
	pfbblocklen += (hdr_buf[3] & 0xFF)  <<8;
	pfbblocklen += (hdr_buf[4] & 0xFF)  <<16;
	pfbblocklen += (hdr_buf[5] & 0xFF)  <<24;
#ifdef DEBUG_PFB_BLOCKS	
	printf("t1io: New segment, length=%d, type=%d\n",
	       pfbblocklen, hdr_buf[1]);
#endif
	accu=0;
	/* header read, now fill the buffer */
	if (pfbblocklen-accu >= F_BUFSIZ){
	  rc = read(f->fd, f->b_base, F_BUFSIZ);
	  accu +=rc;
	}
	else{
	  /* we have the unusual case that the pfb-block size is
	     shorter than F_BUFSIZ -> Read this block only */
	  rc = read(f->fd, f->b_base, pfbblocklen);
	  accu +=rc;
	}
      }
    }
  }
  else{
    /* We have a pfa-file -> read straight ahead and fill buffer */
    rc = read(f->fd, f->b_base, F_BUFSIZ);
  }
  
  /* propagate any error or eof to current file */
  if (rc <= 0) {
    if (rc == 0)    /* means EOF */
      f->flags |= FIOEOF;
    else {
      f->error = (short)-rc;
      f->flags |= FIOERROR;
      rc = 0;
    }
  }

  f->b_ptr = f->b_base;
#ifdef DEBUG_DECRYPTION
  printf("T1_Fill(): read %d bytes\n", rc);
#endif
  
  if (Decrypt){
    rc = T1Decrypt(f->b_base, rc);
#ifdef DEBUG_DECRYPTION
    printf("T1_Fill(): decrypted %d bytes\n", rc);
#endif
  }
  
  return rc;
} /* end Fill */


void T1io_reset(void)
{
  pfbblocklen=0;
  accu=0;
  starthex80=0;
  eexec_startOK=0;
  eexec_endOK=0;
  in_eexec=0;
}



