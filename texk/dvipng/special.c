/* special.c */

/************************************************************************

  Part of the dvipng distribution

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA.

  Copyright (C) 2002-2005 Jan-Åke Larsson

************************************************************************/

#include "dvipng.h"
#include <fcntl.h>
#if HAVE_ALLOCA_H
# include <alloca.h>
#endif

#define SKIPSPACES(s) while(s && *s==' ' && *s!='\0') s++

gdImagePtr
ps2png(const char *psfile, int hresolution, int vresolution, 
       int urx, int ury, int llx, int lly)
{
#ifndef MIKTEX
  int downpipe[2], uppipe[2];
  pid_t pid;
#else
  HANDLE hPngStream;
  HANDLE hPsStream;
  HANDLE hStdErr;
  PROCESS_INFORMATION pi;
  _TCHAR szCommandLine[2048];
  _TCHAR szGsPath[_MAX_PATH];
  int fd;
#endif
  FILE *psstream=NULL, *pngstream=NULL;
  char resolution[STRSIZE]; 
  /*   char devicesize[STRSIZE];  */
  /* For some reason, png256 gives inferior result */
  char *device="-sDEVICE=png16m";  
  gdImagePtr psimage=NULL;
  static bool showpage;

  sprintf(resolution, "-r%dx%d",hresolution,vresolution);
  /* Future extension for \rotatebox
  status=sprintf(devicesize, "-g%dx%d",
		 //(int)((sin(atan(1.0))+1)*
		 (urx - llx)*hresolution/72,//), 
		 //(int)((sin(atan(1.0))+1)*
		 (ury - lly)*vresolution/72);//);
  */
  /* png16m being the default, this code is not needed
   * #ifdef HAVE_GDIMAGECREATETRUECOLOR
   * if (flags & RENDER_TRUECOLOR) 
   * device="-sDEVICE=png16m";
   * #endif  
   */

#ifndef MIKTEX
  if (pipe(downpipe) || pipe(uppipe)) return(NULL);
  /* Ready to fork */
  pid = fork ();
  if (pid == 0) { /* Child process.  Execute gs. */       
    close(downpipe[1]);
    dup2(downpipe[0], STDIN_FILENO);
    close(downpipe[0]);
#ifdef DEBUG
    if (flags & NO_GSSAFER) {
      DEBUG_PRINT(DEBUG_GS,
		  ("\n  GS CALL:\t%s %s %s %s %s %s %s %s %s %s ",/* %s", */
		   GS_PATH, device, resolution, /* devicesize, */
		   "-dBATCH", "-dNOPAUSE", "-q", 
		   "-sOutputFile=-", 
		   "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
		   "-"));
    } else {
      DEBUG_PRINT(DEBUG_GS,
		  ("\n  GS CALL:\t%s %s %s %s %s %s %s %s %s %s %s ",/* %s", */
		   GS_PATH, device, resolution, /*devicesize,*/
		   "-dBATCH", "-dNOPAUSE", "-dSAFER", "-q", 
		   "-sOutputFile=-", 
		   "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
		   "-"));
    }
#endif
    close(uppipe[0]);
    dup2(uppipe[1], STDOUT_FILENO);
    close(uppipe[1]);
    if (flags & NO_GSSAFER) 
      execl (GS_PATH, GS_PATH, device, resolution, /*devicesize,*/
	     "-dBATCH", "-dNOPAUSE", "-q", 
	     "-sOutputFile=-", 
	     "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	     "-",NULL);
    else
      execl (GS_PATH, GS_PATH, device, resolution, /*devicesize,*/
	     "-dBATCH", "-dNOPAUSE", "-dSAFER", "-q", 
	     "-sOutputFile=-", 
	     "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	     "-",NULL);
    _exit (EXIT_FAILURE);
  }
  /* Parent process. */
  
  close(downpipe[0]);
  psstream=fdopen(downpipe[1],"wb");
  if (psstream == NULL) 
    close(downpipe[1]);
  close(uppipe[1]);
  pngstream=fdopen(uppipe[0],"rb");
  if (pngstream == NULL) 
    close(uppipe[0]);
#else /* MIKTEX */
  if (! miktex_find_miktex_executable("mgs.exe", szGsPath)) {
      Warning("Ghostscript could not be found");
      return(NULL);
  }
  if (flags & NO_GSSAFER) {
    DEBUG_PRINT(DEBUG_GS,
		("\n  GS CALL:\t%s %s %s %s %s %s %s %s %s %s ",/* %s",*/
		 szGsPath, device, resolution, /*devicesize,*/
		 "-dBATCH", "-dNOPAUSE", "-q", 
		 "-sOutputFile=-", 
		 "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
		 "-"));
    sprintf(szCommandLine,"\"%s\" %s %s %s %s %s %s %s %s %s",/* %s",*/
	    szGsPath, device, resolution, /*devicesize,*/
	    "-dBATCH", "-dNOPAUSE", "-q", 
	    "-sOutputFile=-", 
	    "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	    "-");
  } else {
    DEBUG_PRINT(DEBUG_GS,
		("\n  GS CALL:\t%s %s %s %s %s %s %s %s %s %s %s ",/* %s",*/
		 szGsPath, device, resolution, /*devicesize,*/
		 "-dBATCH", "-dNOPAUSE", "-dSAFER", "-q", 
		 "-sOutputFile=-", 
		 "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
		 "-"));
    sprintf(szCommandLine,"\"%s\" %s %s %s %s %s %s %s %s %s %s",/* %s",*/
	    szGsPath, device, resolution, /*devicesize,*/
	    "-dBATCH", "-dNOPAUSE", "-dSAFER", "-q", 
	    "-sOutputFile=-", 
	    "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	    "-");
  }
  if (! miktex_start_process_3(szCommandLine, &pi, INVALID_HANDLE_VALUE,
			       &hPsStream, &hPngStream, &hStdErr, 0)) {
      Warning("Ghostscript could not be started");
      return(NULL);
  }
  CloseHandle (pi.hThread);
  fd = _open_osfhandle((intptr_t)hPsStream, _O_WRONLY);
  if (fd >= 0) { 
    psstream = _tfdopen(fd, "wb");
    if (psstream == NULL) 
      _close (fd);
  }
  fd = _open_osfhandle((intptr_t)hPngStream, _O_RDONLY);
  if (fd >= 0) {
    pngstream = _tfdopen(fd, "rb");
    if (pngstream == NULL) 
      _close (fd);
  }
#endif 
  if (psstream) {
    DEBUG_PRINT(DEBUG_GS,("\n  PS CODE:\t<</PageSize[%d %d]/PageOffset[%d %d[1 1 dtransform exch]{0 ge{neg}if exch}forall]>>setpagedevice",
			  urx - llx, ury - lly,llx,lly));
    fprintf(psstream, "<</PageSize[%d %d]/PageOffset[%d %d[1 1 dtransform exch]{0 ge{neg}if exch}forall]>>setpagedevice\n",
	    urx - llx, ury - lly,llx,lly);
    if ( cstack[0].red < 255 || cstack[0].green < 255 || cstack[0].blue < 255 ) {
      DEBUG_PRINT(DEBUG_GS,("\n  PS CODE:\tgsave %f %f %f setrgbcolor clippath fill grestore",
			    cstack[0].red/255.0, cstack[0].green/255.0, cstack[0].blue/255.0));
      fprintf(psstream, "gsave %f %f %f setrgbcolor clippath fill grestore",
	      cstack[0].red/255.0, cstack[0].green/255.0, cstack[0].blue/255.0);
    }
    DEBUG_PRINT(DEBUG_GS,("\n  PS CODE:\t(%s) run", psfile));
    fprintf(psstream, "(%s) run\n", psfile);
    if (showpage) {
      DEBUG_PRINT(DEBUG_GS,("\n  PS CODE:\tshowpage"));
      fprintf(psstream, "showpage\n");
    }
    DEBUG_PRINT(DEBUG_GS,("\n  PS CODE:\tquit"));
    fprintf(psstream, "quit\n");
    fclose(psstream);
  }
  if (pngstream) {
    psimage = gdImageCreateFromPng(pngstream);
    fclose(pngstream);
  }
#ifdef MIKTEX
  CloseHandle(pi.hProcess);
#endif
#ifdef HAVE_GDIMAGETRUECOLORTOPALETTE
#ifdef HAVE_GDIMAGECREATETRUECOLOR
  if (!flags & RENDER_TRUECOLOR)
#endif
    gdImageTrueColorToPalette(psimage,0,256);
#endif
  if (psimage == NULL) {
    DEBUG_PRINT(DEBUG_GS,("\n  GS OUTPUT:\tNO IMAGE "));
    if (!showpage) {
      showpage=true;
      DEBUG_PRINT(DEBUG_GS,("(will try adding \"showpage\") "));
      psimage=ps2png(psfile, hresolution, vresolution, urx, ury, llx, lly);
      showpage=false;
    }
#ifdef DEBUG
  } else {
    DEBUG_PRINT(DEBUG_GS,("\n  GS OUTPUT:\t%dx%d image ",
			  gdImageSX(psimage),gdImageSY(psimage)));
#endif
  }
  return psimage;
}

/*-->SetSpecial*/
/*********************************************************************/
/****************************  SetSpecial  ***************************/
/*********************************************************************/

void SetSpecial(char * special, int32_t length, int32_t hh, int32_t vv)
/* interpret a \special command, made up of keyword=value pairs */
/* Color specials only for now. Warn otherwise. */
{
  char *buffer;

  DEBUG_PRINT(DEBUG_DVI,(" '%.*s'",length,special));

  buffer = alloca(sizeof(char)*(length+1));
  if (buffer==NULL) 
    Fatal("cannot allocate space for special string");

  strncpy(buffer,special,length);
  buffer[length]='\0';

  SKIPSPACES(buffer);
  /********************** Color specials ***********************/
  if (strncmp(buffer,"background ",11)==0) {
    background(buffer+11);
    return;
  }
  if (strncmp(buffer,"color ",6)==0) {
    buffer+=6;
    SKIPSPACES(buffer);
    if (strncmp(buffer,"push ",5)==0) {
      pushcolor(buffer+5);
    } else {
      if (strcmp(buffer,"pop")==0)
	popcolor();
      else 
	resetcolorstack(buffer);
    }
    return;
  }

  /******************* Postscript inclusion ********************/
  if (strncmp(buffer,"PSfile=",7)==0) { /* PSfile */
    char* psname = buffer+7,*psfile;
    int llx=0,lly=0,urx=0,ury=0,rwi=0,rhi=0;
    int hresolution,vresolution;

    /* Remove quotation marks around filename */
    if (*psname=='"') {
      char* tmp;
      psname++;
      tmp=strrchr(psname,'"');
      if (tmp!=NULL) {
	*tmp='\0';
	buffer=tmp+1;
      } else
	buffer=NULL;
    }
    TEMPSTR(psfile,kpse_find_file(psname,kpse_pict_format,0));
    
    /* Retrieve parameters */
    SKIPSPACES(buffer);
    while(buffer && *buffer) {
      if (strncmp(buffer,"llx=",4)==0) llx = strtol(buffer+4,&buffer,10);
      else if (strncmp(buffer,"lly=",4)==0) lly = strtol(buffer+4,&buffer,10);
      else if (strncmp(buffer,"urx=",4)==0) urx = strtol(buffer+4,&buffer,10);
      else if (strncmp(buffer,"ury=",4)==0) ury = strtol(buffer+4,&buffer,10);
      else if (strncmp(buffer,"rwi=",4)==0) rwi = strtol(buffer+4,&buffer,10);
      else if (strncmp(buffer,"rhi=",4)==0) rhi = strtol(buffer+4,&buffer,10);
      else while (*buffer && *buffer!=' ') buffer++;
      SKIPSPACES(buffer);
    }
    
    /* Calculate resolution, and use our base resolution as a fallback. */
    /* The factor 10 is magic, the dvips graphicx driver needs this.    */
    hresolution = dpi*rwi/(urx - llx)/10;
    vresolution = dpi*rhi/(ury - lly)/10;
    if (vresolution==0) vresolution = hresolution;
    if (hresolution==0) hresolution = vresolution;
    if (hresolution==0) hresolution = vresolution = dpi;
    
    if (page_imagep != NULL) { /* Draw into image */
      char* psfile;
      gdImagePtr psimage=NULL;

      /*---------- Cache ----------*/
      char* cachename = NULL;
      gdImagePtr cacheimage=NULL;

      TEMPSTR(psfile,kpse_find_file(psname,kpse_pict_format,0));
      if (flags & CACHE_IMAGES) { /* Find cached image, if it exists */
	char *cachefile,*separator;

	cachename = alloca(sizeof(char)*(strlen(psname+5)));
	if (cachename==NULL) 
	  Fatal("cannot allocate space for cached image filename");
	strcpy(cachename,psname);
	separator = strrchr(cachename,'.');
	if (separator!=NULL)
	  *separator='\0';
	strcat(cachename,".png");
	cachefile = kpse_find_file(cachename,kpse_pict_format,0);
	if (cachefile!=NULL) {
	  FILE* cachefilep = fopen(cachefile,"rb");

	  if (cachefilep!=NULL) {
	    cacheimage = gdImageCreateFromPng(cachefilep);
	    fclose(cachefilep);
	  }
	  free(cachefile);
	}
	psimage = cacheimage;
      }
      /*---------- End Cache ----------*/
      Message(BE_NONQUIET,"<%s",psname);
      if (psimage==NULL) {
	/* No cached image, convert postscript */
	if (psfile == NULL) {
	  Warning("PS file %s not found, image will be left blank", psname );
	  flags |= PAGE_GAVE_WARN;
	} else if (flags & NO_GHOSTSCRIPT) {
	    Warning("GhostScript calls disallowed by --noghostscript", 
		    psfile );
	    flags |= PAGE_GAVE_WARN;
	} else {
	  psimage = ps2png(psfile, hresolution, vresolution, 
			   urx, ury, llx, lly);
	  if ( psimage == NULL ) {
	    Warning("Unable to convert %s to PNG, image will be left blank", 
		    psfile );
	    flags |= PAGE_GAVE_WARN;
	  }
	}
      }
      /*---------- Store Cache ----------*/
      if (flags & CACHE_IMAGES && cachename !=NULL && 
	  cacheimage==NULL && psimage != NULL) {
	/* Cache image not found, save converted postscript */
	FILE* cachefilep = fopen(cachename,"wb");
	if (cachefilep!=NULL) {
	  gdImagePng(psimage,cachefilep);
	  fclose(cachefilep);
	} else
	  Warning("Unable to cache %s as PNG", psfile );
      } 
      /*---------- End Store Cache ----------*/
      if (psimage!=NULL) {
	DEBUG_PRINT(DEBUG_DVI,
		    ("\n  PS-PNG INCLUDE \t%s (%d,%d) res %dx%d at (%d,%d)",
		     psfile,
		     gdImageSX(psimage),gdImageSY(psimage),
		     hresolution,vresolution,
		     hh, vv));
	gdImageCopy(page_imagep, psimage, 
		    hh, vv-gdImageSY(psimage),
		    0,0,
		    gdImageSX(psimage),gdImageSY(psimage));
	gdImageDestroy(psimage);
      }
      Message(BE_NONQUIET,">");
    } else { /* Don't draw */
      int pngheight,pngwidth;
      
      /* Convert from postscript 72 dpi resolution to our given resolution */
      pngheight = (vresolution*(ury - lly)+71)/72; /* +71: do 'ceil' */
      pngwidth  = (hresolution*(urx - llx)+71)/72;
      DEBUG_PRINT(DEBUG_DVI,("\n  PS-PNG INCLUDE \t(%d,%d)", 
		   pngwidth,pngheight));
      min(x_min,hh);
      min(y_min,vv-pngheight);
      max(x_max,hh+pngwidth);
      max(y_max,vv);
    }
    return;
  }

  /* preview-latex' tightpage option */
  if (strncmp(buffer,"!/preview@tightpage",19)==0) { 
    buffer+=19;
    SKIPSPACES(buffer);
    if (strncmp(buffer,"true",4)==0) {
      if (page_imagep==NULL) 
	Message(BE_NONQUIET," (preview-latex tightpage option detected, will use its bounding box)");
      flags |= PREVIEW_LATEX_TIGHTPAGE;
      return;
    }
  }
  if (strncmp(buffer,"!userdict",9)==0 
      && strstr(buffer+10,"7{currentfile token not{stop}if 65781.76 div")!=NULL) {
    if (page_imagep==NULL && ~flags & PREVIEW_LATEX_TIGHTPAGE) 
      Message(BE_NONQUIET," (preview-latex <= 0.9.1 tightpage option detected, will use its bounding box)");
    flags |= PREVIEW_LATEX_TIGHTPAGE;
    return;
  }

  /* preview-latex' dvips bop-hook redefinition */
  if (strncmp(buffer,"!userdict",9)==0 
      && strstr(buffer+10,"preview-bop-")!=NULL) {
    if (page_imagep==NULL) 
      Message(BE_NONQUIET," (preview-latex beginning-of-page-hook detected)");
    return;
  }

  if (strncmp(buffer,"ps::",4)==0) {
    /* Hokay, decode bounding box */
    dviunits adj_llx,adj_lly,adj_urx,adj_ury,ht,dp,wd;
    adj_llx = strtol(buffer+4,&buffer,10);
    adj_lly = strtol(buffer,&buffer,10);
    adj_urx = strtol(buffer,&buffer,10);
    adj_ury = strtol(buffer,&buffer,10);
    ht = strtol(buffer,&buffer,10);
    dp = strtol(buffer,&buffer,10);
    wd = strtol(buffer,&buffer,10);
    if (wd>0) {
      x_offset_tightpage = 
	(-adj_llx+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
      x_width_tightpage  = x_offset_tightpage
	+(wd+adj_urx+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
    } else {
      x_offset_tightpage = 
	(-wd+adj_urx+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
      x_width_tightpage  = x_offset_tightpage
	+(-adj_llx+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
    }
    /* y-offset = height - 1 */
    y_offset_tightpage = 
      (((ht>0)?ht:0)+adj_ury+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor-1;
    y_width_tightpage  = y_offset_tightpage+1
      +(((dp>0)?dp:0)-adj_lly+dvi->conv*shrinkfactor-1)/dvi->conv/shrinkfactor;
    return;
  }

  if (strncmp(buffer,"papersize=",10)==0) { /* papersize spec, ignored */
    return;
  }
  if (strncmp(buffer,"header=",7)==0 || buffer[0]=='!') { /* header, ignored */
    if ( page_imagep != NULL )
      Warning("at (%ld,%ld) ignored header \\special{%.*s}",
	      hh, vv, length,special);
    return;
  }
  if (strncmp(buffer,"src:",4)==0) { /* source special */
    if ( page_imagep != NULL )
      Message(BE_NONQUIET," at (%ld,%ld) source \\special{%.*s}",
	      hh, vv, length,special);
    return;
  }
  if ( page_imagep != NULL ) {
    Warning("at (%ld,%ld) unimplemented \\special{%.*s}",
	    hh, vv, length,special);
    flags |= PAGE_GAVE_WARN;
  }
}
