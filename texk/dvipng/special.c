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

  Copyright (C) 2002-2006 Jan-Åke Larsson

************************************************************************/

#include "dvipng.h"
#include <fcntl.h>
#if HAVE_ALLOCA_H
# include <alloca.h>
#endif

#define SKIPSPACES(s) while(s && *s==' ' && *s!='\0') s++

gdImagePtr
ps2png(const char *psfile, char *device, int hresolution, int vresolution, 
       int llx, int lly, int urx, int ury)
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
#define GS_PATH szGsPath
  int fd;
#endif
  FILE *psstream=NULL, *pngstream=NULL;
  char resolution[STRSIZE]; 
  /*   char devicesize[STRSIZE];  */
  gdImagePtr psimage=NULL;
  static char* showpage="";

  sprintf(resolution, "-r%dx%d",hresolution,vresolution);
  /* Future extension for \rotatebox
  status=sprintf(devicesize, "-g%dx%d",
		 //(int)((sin(atan(1.0))+1)*
		 (urx - llx)*hresolution/72,//), 
		 //(int)((sin(atan(1.0))+1)*
		 (ury - lly)*vresolution/72);//);
  */
  DEBUG_PRINT(DEBUG_GS,
	      ("\n  GS CALL:\t%s %s %s %s %s %s %s %s %s %s %s",/* %s", */
	       GS_PATH, device, resolution, /*devicesize,*/
	       "-dBATCH", "-dNOPAUSE", "-q", "-sOutputFile=-", 
	       "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	       (flags & NO_GSSAFER) ? "-": "-dSAFER", 
	       (flags & NO_GSSAFER) ? "": "- "));
#ifndef MIKTEX
  if (pipe(downpipe) || pipe(uppipe)) return(NULL);
  /* Ready to fork */
  pid = fork ();
  if (pid == 0) { /* Child process.  Execute gs. */       
    close(downpipe[1]);
    dup2(downpipe[0], STDIN_FILENO);
    close(downpipe[0]);
    close(uppipe[0]);
    dup2(uppipe[1], STDOUT_FILENO);
    close(uppipe[1]);
    execlp(GS_PATH, GS_PATH, device, resolution, /*devicesize,*/
	   "-dBATCH", "-dNOPAUSE", "-q", "-sOutputFile=-", 
	   "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	   (flags & NO_GSSAFER) ? "-": "-dSAFER", 
	   (flags & NO_GSSAFER) ? NULL: "-",
	   NULL);
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
  sprintf(szCommandLine,"\"%s\" %s %s %s %s %s %s %s %s %s %s",/* %s",*/
	  szGsPath, device, resolution, /*devicesize,*/
	  "-dBATCH", "-dNOPAUSE", "-q", "-sOutputFile=-", 
	  "-dTextAlphaBits=4", "-dGraphicsAlphaBits=4",
	  (flags & NO_GSSAFER) ? "-": "-dSAFER", 
	  (flags & NO_GSSAFER) ? "": "-");
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
    DEBUG_PRINT(DEBUG_GS,("\n  PS CODE:\t(%s) run %s quit ", psfile, showpage));
    fprintf(psstream, "(%s) run %s quit ", psfile, showpage);
    fclose(psstream);
  }
  if (pngstream) {
    psimage = gdImageCreateFromPng(pngstream);
    fclose(pngstream);
  }
#ifdef MIKTEX
  CloseHandle(pi.hProcess);
#endif
  if (psimage == NULL) {
    DEBUG_PRINT(DEBUG_GS,("\n  GS OUTPUT:\tNO IMAGE "));
    if (!(*showpage)) {
      showpage="showpage";
      DEBUG_PRINT(DEBUG_GS,("(will try adding \"%s\") ",showpage));
      psimage=ps2png(psfile, device, hresolution, vresolution, llx, lly, urx, ury);
      showpage="";
    }
#ifdef DEBUG
  } else {
    DEBUG_PRINT(DEBUG_GS,("\n  GS OUTPUT:\t%dx%d image ",
			  gdImageSX(psimage),gdImageSY(psimage)));
#endif
  }
  return psimage;
}


gdImagePtr readcache(char* psname)
{
  char *cachename = NULL, *cachefile, *separator;
  gdImagePtr cacheimage=NULL;

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
      DEBUG_PRINT(DEBUG_DVI,("\n  READING CACHED IMAGE \t%s", cachefile));
      cacheimage = gdImageCreateFromPng(cachefilep);
      fclose(cachefilep);
    }
    free(cachefile);
  }
  return(cacheimage);
}

void storecache(char* psname, gdImagePtr psimage)
{
  char *cachename = NULL, *separator;

  cachename = alloca(sizeof(char)*(strlen(psname+5)));
  if (cachename==NULL) 
    Fatal("cannot allocate space for cached image filename");
  strcpy(cachename,psname);
  separator = strrchr(cachename,'.');
  if (separator!=NULL)
    *separator='\0';
  strcat(cachename,".png");
  
  if (psimage != NULL) {
    FILE* cachefilep = fopen(cachename,"wb");
    if (cachefilep!=NULL) {
      gdImagePng(psimage,cachefilep);
      fclose(cachefilep);
    } else
      Warning("Unable to cache %s as PNG", psname );
  }
}



/*-->SetSpecial*/
/*********************************************************************/
/****************************  SetSpecial  ***************************/
/*********************************************************************/

void SetSpecial(char * special, int32_t length, int32_t hh, int32_t vv)
/* interpret a \special command, made up of keyword=value pairs */
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

  /******************* Image inclusion ********************/
  if (strncmp(buffer,"PSfile=",7)==0) { /* PSfile */
    char* psname = buffer+7,*psfile;
    int llx=0,lly=0,urx=0,ury=0,rwi=0,rhi=0;
    bool clip=false;
    int hresolution,vresolution;
    int pngheight,pngwidth;

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
      else if (strncmp(buffer,"clip",4)==0) {clip = true; buffer=buffer+4;}
      while (*buffer && *buffer!=' ') buffer++;
      SKIPSPACES(buffer);
    }
    
    /* Calculate resolution, and use our base resolution as a fallback. */
    /* The factor 10 is magic, the dvips graphicx driver needs this.    */
    hresolution = ((dpi*rwi+urx-llx-1)/(urx - llx)+9)/10;
    vresolution = ((dpi*rhi+ury-lly-1)/(ury - lly)+9)/10;
    if (vresolution==0) vresolution = hresolution;
    if (hresolution==0) hresolution = vresolution;
    if (hresolution==0) hresolution = vresolution = dpi;
      
    /* Convert from postscript 72 dpi resolution to our given resolution */
    pngwidth  = (dpi*rwi+719)/720; /* +719: round up */
    pngheight = (dpi*rhi+719)/720;
    if (pngwidth==0)  
      pngwidth  = ((dpi*rhi*(urx-llx)+ury-lly-1)/(ury-lly)+719)/720;
    if (pngheight==0) 
      pngheight = ((dpi*rwi*(ury-lly)+urx-llx-1)/(urx-llx)+719)/720;
    if (pngheight==0) {
      pngwidth  = (dpi*(urx-llx)+71)/72;
      pngheight = (dpi*(ury-lly)+71)/72;
    }    
    if (page_imagep != NULL) { /* Draw into image */
      char *psfile;
      gdImagePtr psimage=NULL;
      FILE* psstream;

      TEMPSTR(psfile,kpse_find_file(psname,kpse_pict_format,0));
      if (psfile == NULL) {
	Warning("Image file %s not found, image will be left blank", psname );
	flags |= PAGE_GAVE_WARN;
	return;
      } 
      /* Test access permission even for EPS files */
      psstream=fopen(psfile,"rb");
      if (psstream == NULL) {
	Warning("Cannot access image file %s, image will be left blank", 
		psname );
	flags |= PAGE_GAVE_WARN;
	return;
      } 
      Message(BE_NONQUIET," <%s",psname);
      switch (getc(psstream)) {
      case 0x89: /* PNG magic: "\211PNG\r\n\032\n" */
	DEBUG_PRINT(DEBUG_DVI,("\n  INCLUDE PNG \t%s",psfile));
	fseek(psstream,0,SEEK_SET);
	psimage=gdImageCreateFromPng(psstream);
	fclose(psstream);
	break;
      case 'G': /* GIF magic: "GIF87" or "GIF89" */
	DEBUG_PRINT(DEBUG_DVI,("\n  INCLUDE GIF \t%s",psfile));
#ifdef HAVE_GDIMAGEGIF
	fseek(psstream,0,SEEK_SET);
	psimage=gdImageCreateFromGif(psstream);
#else
	DEBUG_PRINT(DEBUG_DVI,(" (NO GIF DECODER)"));
#endif
	fclose(psstream);
	break;
      case 0xff: /* JPEG magic: 0xffd8 */
	DEBUG_PRINT(DEBUG_DVI,("\n  INCLUDE JPEG \t%s",psfile));
#ifdef HAVE_GDIMAGECREATETRUECOLOR
	fseek(psstream,0,SEEK_SET);
	psimage=gdImageCreateFromJpeg(psstream);
#else
	DEBUG_PRINT(DEBUG_DVI,(" (NO JPEG DECODER)"));
#endif
	fclose(psstream);
	break;
      default:  /* Default, PostScript magic: "%!PS-Adobe" */
	fclose(psstream);
	if (flags & NO_GHOSTSCRIPT) { 
	  Warning("GhostScript calls disallowed by --noghostscript", psfile );
	  flags |= PAGE_GAVE_WARN;
	} else {
	  gdImagePtr cacheimage=NULL;
	  if (flags & CACHE_IMAGES) 
	    cacheimage=psimage=readcache(psname);
	  /* Use alpha blending, and render transparent postscript
	     images. The alpha blending works correctly only from
	     libgd 2.0.12 upwards */
#ifdef HAVE_GDIMAGECREATETRUECOLOR
	  if (!page_imagep->trueColor)
	    Warning("Palette output, opaque image inclusion");
#ifdef HAVE_GDIMAGEPNGEX
	  else if (psimage==NULL) {
	    DEBUG_PRINT(DEBUG_DVI,("\n  RENDER PNGALPHA POSTSCRIPT \t%s",
				   psfile));
	    if (clip) {
	      DEBUG_PRINT(DEBUG_DVI,(", CLIPPED TO BBOX"));
	      psimage = ps2png(psfile, "-sDEVICE=pngalpha", 
			       hresolution, vresolution, 
			       llx, lly, urx, ury);
	    } else {
	      /* Render across the whole image */ 
	      DEBUG_PRINT(DEBUG_DVI,
			  ("\n  EXPAND BBOX \t%d %d %d %d -> %d %d %d %d",
			   llx,lly,urx,ury,
			   llx-(hh+1)*72/hresolution,
			   lly-(gdImageSY(page_imagep)-vv-1)*72/vresolution,
			   llx+(gdImageSX(page_imagep)-hh)*72/hresolution,
			   lly+(vv+1)*72/vresolution));
	      psimage = ps2png(psfile, "-sDEVICE=pngalpha", 
			       hresolution, vresolution,
			       llx-(hh+1)*72/hresolution,
			       lly-(gdImageSY(page_imagep)-vv-1)*72/vresolution,
			       llx+(gdImageSX(page_imagep)-hh)*72/hresolution,
			       lly+(vv+1)*72/vresolution);
	      if (psimage!=NULL) {
		hh=0;
		vv=gdImageSY(page_imagep)-1;
		pngwidth=gdImageSX(psimage);
		pngheight=gdImageSY(psimage);
	      }
	    }
	    if (psimage==NULL)
	      Warning("No GhostScript pngalpha output, opaque image inclusion");
	  }
#endif
#endif
	  if (psimage==NULL) {
	    /* png256 gives inferior result */
	    DEBUG_PRINT(DEBUG_DVI,("\n  RENDER POSTSCRIPT \t%s", psfile));
	    psimage = ps2png(psfile, "-sDEVICE=png16m",
			     hresolution, vresolution, 
			     llx, lly, urx, ury);
	    flags |= PAGE_GAVE_WARN;
	  }
	  if (flags & CACHE_IMAGES && cacheimage==NULL) 
	    storecache(psname,psimage); 
	}
      }
      if (psimage!=NULL) {
	/* Rescale, but ignore (one-pixel) rounding errors */
	if (gdImageSX(psimage)!=pngwidth 
	    && gdImageSX(psimage)!=pngwidth+1
	    && gdImageSY(psimage)!=pngheight
	    && gdImageSY(psimage)!=pngheight+1) {
	  gdImagePtr scaledimage;

	  DEBUG_PRINT(DEBUG_DVI,
		      ("\n  RESCALE INCLUDED BITMAP \t%s (%d,%d) -> (%d,%d)",
		       psfile,
		       gdImageSX(psimage),gdImageSY(psimage),
		       pngwidth,pngheight));
#ifdef HAVE_GDIMAGECREATETRUECOLOR
	  scaledimage=gdImageCreateTrueColor(pngwidth,pngheight);
	  gdImageCopyResampled(scaledimage,psimage,0,0,0,0,
			       pngwidth,pngheight,
			       gdImageSX(psimage),gdImageSY(psimage));
#else
	  scaledimage=gdImageCreate(pngwidth,pngheight);
	  gdImageCopyResized(scaledimage,psimage,0,0,0,0,
			     pngwidth,pngheight,
			     gdImageSX(psimage),gdImageSY(psimage));
#endif
	  gdImageDestroy(psimage);
	  psimage=scaledimage;
	}
	DEBUG_PRINT(DEBUG_DVI,
		    ("\n  GRAPHIC(X|S) INCLUDE \t%s (%d,%d) res %dx%d at (%d,%d)",
		     psname,gdImageSX(psimage),gdImageSY(psimage),
		     hresolution,vresolution,hh,vv));
#ifdef HAVE_GDIMAGECREATETRUECOLOR
	if (psimage->trueColor && !page_imagep->trueColor)
	  gdImageTrueColorToPalette(psimage,0,256);
#endif
#ifdef HAVE_GDIMAGEPNGEX
	gdImageAlphaBlending(page_imagep,1);
#else
	Warning("Using libgd < 2.0.12, opaque image inclusion");
	flags |= PAGE_GAVE_WARN;
#endif
	gdImageCopy(page_imagep, psimage, 
		    hh, vv-gdImageSY(psimage)+1,
		    0,0,
		    gdImageSX(psimage),gdImageSY(psimage));
#ifdef HAVE_GDIMAGEPNGEX
	gdImageAlphaBlending(page_imagep,0);
#endif
	gdImageDestroy(psimage);
      } else {
	Warning("Unable to load %s, image will be left blank",psfile );
	flags |= PAGE_GAVE_WARN;
      } 
      Message(BE_NONQUIET,">");
    } else { /* Don't draw */
      flags |= PAGE_TRUECOLOR;
      DEBUG_PRINT(DEBUG_DVI,
		  ("\n  GRAPHIC(X|S) INCLUDE \t%s (%d,%d) res %dx%d at (%d,%d)",
		   psname,pngheight,pngwidth,
		   hresolution,vresolution,hh,vv));
      min(x_min,hh);
      min(y_min,vv-pngheight+1);
      max(x_max,hh+pngwidth);
      max(y_max,vv+1);
    }
    return;
  }

  if (strncmp(buffer,"!/preview@version(",18)==0) { 
    buffer+=18;
    length-=18;
    while (length>0 && buffer[length]!=')') 
      length--;
    if (page_imagep==NULL) 
      Message(BE_NONQUIET," (preview-latex version %.*s)",length,buffer);
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
      Message(BE_VERBOSE," (preview-latex beginning-of-page-hook detected)");
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
