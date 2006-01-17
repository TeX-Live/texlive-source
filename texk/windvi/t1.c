/*========================================================================*\

Copyright (c) 1999  Nicolai Langfeldt

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL NICOLAI LANGFELDT, PAUL VOJTA, OR ANYONE ELSE BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Names:
  janl: Nicolai Langfeldt (janl@math.uio.no)
  rmz: Rainer Menzner (Rainer.Menzner@neuroinformatik.ruhr-uni-bochum.de)

History:
  18/02/1999, janl: Initial version, first public alpha release of xdvi-t1.
  22/02/1999, rmz: Changes to make scaling and antialiasing work better.
  24/02/1999, janl: Read afm files to get alignments right.  type1 char
  	widhts are to coarse.
  08/05/1999, janl: Update to t1lib-0.9, add code to read more generic
  	encoding maps (encoding.map, not *.enc)
  10/05/1999, janl: New font loading code and data struct for defered
        font loading.  Significant reduction in startup time realized.

Bugs:
  - Every kpse_find_file call presents a memory leak.  The pathname
    buffer is not freed.

\*========================================================================*/

#define HAVE_BOOLEAN 1

#include "xdvi-config.h"

#if T1

#define PRIVATE static
#define PUBLIC

#if defined(HAVE_STRING_H)
#include <string.h>
#else
extern char *strtok P2H(char *, const char *);
#endif

#include "dvi.h"
#include <kpathsea/tex-file.h>

#include <t1lib.h>
#include <t1libw32.h>

/* Datastructures:

   encodings[n]: Loaded from map file
   [0] = 8r,	<vector>
   [1] = 8c,	<vector>

   The vectors are demand-loaded in 'load_encoded_font'

*/

typedef struct encoding {
  char *enc;		/* This may be NULL.  From dvips maps we get 
			   encodings loaded by filename, and no name given */
  char *file;
  char **vector;
} encoding;

PRIVATE encoding *encodings;	/* Dynamic array */
#define ENCSTART 16		/* Start size.  Normaly there will not
				   be even this many. */
#define ENCGROW 16		/* Grow by, when full */
PRIVATE int maxenc=ENCSTART-1;	/* Current size of array */
PRIVATE int enclidx=-1;		/* Current last index in array */

/*

   t1fonts[n]: Built as xdvi loads t1 fonts
   idx	 file			short	t1id	loaded
   [0] = /path/cmr10.pfb,	cmr10	0	1
   [1] = /path/prmr.pfa,	prmr	1	0
   [2] = /path/pcrr8a.pfa,	pcrr8a	2	0

   This array enumerates the loaded t1 fonts by full path name.  The
   refered to t1id is the t1lib font id.  The font will not be
   extended/slanted/encoded.  The loaded field indicates if
   T1_LoadFont has been called for the font yet.  texfonts that need a
   modified version of the t1 font must use T1_CopyFont to copy the
   raw font and obtain the id of a new t1 font which can then be
   modified.  As described in the t1 docs a font must be loaded before
   it can be copied.

   Fonts that are copies of other fonts have NULL filenames.  If any
   font files are to be reused based on the file name the raw font
   must be reused, not a copy.

   The short field contains the font as it was identified in the
   fontmap.  This is used to save searching; if two fonts are
   identified the same way in the fontmap they the same font.

*/

typedef struct t1font {
  char	*file;
  char	*shortname;
  int	t1id;
  int	loaded;
} t1font;

PRIVATE t1font *t1fonts;	/* Dynamic array */
#define T1START	2048		/* Japanese, Chineese, Korean and others
				   use a lot of fonts */
#define T1GROW  1024
PRIVATE int maxt1=T1START-1;
PRIVATE int t1lidx=-1;

/*
   fontmaps[n]: Loaded from map file, and extended with 'implied'
	fonts as needed.
   idx	 texname enc	exten.	slant	filena.	t1libid	pathn.	tfmidx
   [0] = pcrr8rn 0	750	0	pcrr8a	2	...	2
   [1] = putro8r 0	0	0	putro8r 3	...	3

   The first 5 fields of this table are loaded from the font map
   files, or filled out with default values when implied fonts are
   set-up.  The t1libid is -1 until the font is loaded or copied as
   described under t1fonts.  Once the dvi file reveals that the font
   is needed it is located on disk to ensure it exists, and the
   pathname field is filled out.  The t1 font is not loaded, or
   copied, before it's needed because set_t1_char has been called to
   draw a glyph from the font.  The late loading is necessiated by the
   high cost of loading fonts.  If the font loading is concentrated at
   startup of the program the startup-time becomes ecessively high due
   to the high number of fonts used by any normaly LaTeX-document.

   0 extention and slant means no extention/slant.  If the input
   values are decimal (less than 10) they're multiplied by 1000 and
   10000 respectively to obtain fixed-point integer values.  Integer
   values have the advandage of being testable for exact match (== 0)
   on all architectures (it's true!  Not all architectures can do
   (reliable) exact matching of floatingpoint values)

*/

typedef struct fontmap {
  char *texname;
  int  enc;		/* Index in encoding array */
  int  extention;	/* Fixed point, *1000 */
  int  slant;		/* Fixed point, *10000, some font slantings have 4 
			   significant digits, all after the decimalpoint */
  char *filename;	/* Name of the t1 font file as given in map/dvi */

  int  t1libid;		/* The t1lib id of this font, or -1 if not set up */
  char *pathname;	/* Full path of the font, once needed and located */
  int  tfmidx;		/* Index in tfminfo array, or -1 */
} fontmap;

PRIVATE fontmap *fontmaps;
#define FNTMAPSTART 2048
#define FNTMAPGROW 1024
PRIVATE int maxmap=FNTMAPSTART-1;
PRIVATE int maplidx=-1;

/* 
   widthinfo[n]: TFM widht info, demand loaded as the fonts are used.
   idx	 texname	widths
   [0] = cmr10		...
   [1] = ptmr8r		...
   [2] = pcrr8rn	...
   [3] = putro8r	...

   The widthinfo is loaded from the font tfm file since the type1
   width information isn't precise enough to avoid accumulating
   rounding errors that make things visibly unaligned.  This is
   esp. noticeable in tables; the vline column separators become
   unaligned.

   For other than "design" sizes the widths are scaled up or down.

*/

typedef struct tfminfos {
  char *texname;
  long designsize;
  long widths[256];
} tfminfos;

PRIVATE tfminfos *tfminfo;
#define TEXWSTART 2048
#define TEXWGROW 1024
PRIVATE int maxw=TEXWSTART-1;
PRIVATE int wlidx=-1;

PRIVATE int size_devisor;

/* For file reading */
#define BUFFER_SIZE 512

/* This is the conversion factor from TeXs pt to TeXs bp (big point).
   Maximum machine precision is needed to keep it accurate. */
PRIVATE double bp_fac=72.0/72.27;

/* Try to convert from PS charspace units to DVI units with minimal
   loss of significant digits */
#define t1_dvi_conv(x)	((((int) (x)) << 16)/1000)
/* Convert from DVI units to TeX pt */
#define dvi_pt_conv(x)	(((long) (x)) >> 16)
/* Experiment with preserving significant digits by using floating point.
   No win observed. */
#define dvi_pt_fconv(x)	((x) / 65536.0)

/* Convert from TFM units to DVI units */
#define tfm_dvi_conv(x) (((int) (x)) >> 1)

/* ********************** FONT AND ENCODING LOADING ******************** */

PRIVATE char **load_vector P1C(char *,enc)

{
  char *filename;
  char **vector=NULL;

  filename=kpse_find_file(enc,kpse_program_text_format,0);
  
  if (filename!=NULL) {
    vector=T1_LoadEncoding(filename);
    if (vector==NULL) {
      fprintf(stderr,"%s: Unable to load encoding vector %s from %s\n",
	      enc,filename);
      perror("xdvi");
      exit(1);
    }
    fprintf(stderr,"Encoding %s loaded from %s at 0x%lx\n",
	    enc,filename,vector);
  } else
    fprintf(stderr,"Unable to load encoding vector %s\n",enc);

  return vector;
}


PRIVATE int add_tfm P1C(char *,texname)

{
  /* Load font metrics if not already loaded.  Return index into
     fontmetrics array, or -1 if it failed */

  int i;

  for (i=0;i<=wlidx;i++) 
    if (strcmp(tfminfo[i].texname,texname)==0)
      return i;

  if (wlidx==maxw) {
    maxw += TEXWGROW;
    tfminfo=realloc(tfminfo,sizeof(*tfminfo)*maxw);
    if (tfminfo==NULL) {
      fprintf(stderr,"%s: Error: Out of memory.\n");
      exit(1);
    }
  }

  wlidx++;

  tfminfo[wlidx].texname=texname;

  if (!tfmload(texname,&tfminfo[wlidx].designsize,tfminfo[wlidx].widths)) {
      fprintf(stderr,"%s: Could not find/load tfm file for %s. "
	      "%s: Expect ugly output.\n",texname);

    wlidx--;
    return -1;
  }

  return wlidx;
}


PRIVATE int add_t1font P2C(char *,fontname, char *,filename)

{
  /* Add t1 font to list, or not if it's already there.  Return the
     t1lib font id. The filename argument is optional, but is assumed
     to be a full path if it is given */

  int id,i,free_it=0;
  char *path;

  /* Already set up by that name? */
  for (i=0;i<=t1lidx;i++) {
    if (strcmp(t1fonts[i].shortname,fontname)==0 ||
	strcmp(t1fonts[i].file,fontname)==0) {
#if debug
      fprintf(stderr,"Type1 font %s already loaded from %s\n",
	      fontname, filename);
#endif
      return t1fonts[i].t1id;
    }
  }
  
  /* Insert and set up new t1 font */

  if (t1lidx==maxt1) {
    /* Make room */
    maxt1 += T1GROW;
    t1fonts=realloc(t1fonts,sizeof(*t1fonts)*maxt1);
    if (t1fonts==NULL) {
      fprintf(stderr,"%s: Error: Out of memory\n");
      exit(1);
    }
    fprintf(stderr,"Enlarged t1 table from %d to %d entries\n",
	    t1lidx, maxt1);
  }

  t1lidx++;

  if (filename == NULL) {
    /* We don't know the full path name yet, find it */
    fprintf(stderr,"Looking for type1: %s\n",fontname);
    path = kpse_find_file(fontname,kpse_type1_format,0);
    if (path == NULL) return -1; /* xdvi will use substitution font */
    free_it=1;
  } else {
    path=filename;
  }

  t1fonts[t1lidx].file=strdup(path);
  t1fonts[t1lidx].t1id=id=T1_AddFont(path);
  t1fonts[t1lidx].shortname=strdup(fontname);
  t1fonts[t1lidx].loaded=0;

  if (free_it) 
    free(path);

  fprintf(stderr,"%s set-up as t1lib font %d\n", filename, id);

#if USE_AFM
  /* Set the afm filename.  Always before loading.  If no afm file
     found t1lib will use fallback code */
  path=kpse_find_file(fontname,kpse_afm_format,0);
  if (path!=NULL) {
    T1_SetAfmFileName(id,path);
    free(path);
  }
#endif

  return id;
}


PRIVATE int find_texfont P1C(char *,texname)

{
  /* Find fontmap index of texfont */
  int i;

  for (i=0;i<maplidx;i++) {
    if (strcmp(fontmaps[i].texname,texname)==0) {
#if debug
      fprintf(stderr,"Type1 font already loaded: %s\n",texname);
#endif
      return i;
    }
  }
  return -1;
}


PRIVATE int setup_encoded_T1_font P6C(char *,texname, char *,alias, 
				      char *,filename, 
				      int, enc, int, ext, int, sl)


{
  /* xdvi T1 Font loading is done in two steps:

     1. At xdvi startup two things happen:

        a. The fontmaps are read.

	b. the dvi file is (partialy) scanned, and fonts are set up.
	   In the case of t1 fonts we only set up the data structures,
	   they are not actually loaded until they are used.

     2. At the time a T1 font is used it is loaded, encoded, extended
        and slanted as prescribed.

     This procedure takes care of step 1a.  It locates the font and
     sets up the data-structures so it may be assumed, by xdvi, to be
     loaded.  Return the fontmaps array index.

     The 'texname' param should be the texname of the font, such as
     ptmr8r or cmr10.

     The 'alias' param should be the file name of the font if it is
     different from the texname.  This is often the case with fonts
     defined in fontmaps.

     If, for some reason, the full filename of the font has already
     been looked up before we get here it is passed in the filename
     param so we don't have to look it up again.

     Implied encodings are not handled here.

     REMEMBER: THIS PROC IS CALLED FOR EACH FONT IN THE FONTMAPS, WE
     CANNOT DO ANY EXPENSIVE OPERATIONS HERE!!!

 */

  int idx=-1;
  
  /* Already setup by that name? */
  idx=find_texfont(texname);
  
  if (idx!=-1) return idx;

  /* Not set up.  Do it. */

  if (maplidx==maxmap) {
    maxmap += FNTMAPGROW;
    /* Hope realloc isn't destructive anywhere :-) */
    fontmaps=realloc(fontmaps,sizeof(*fontmaps)*maxmap);
    if (fontmaps==NULL) {
      fprintf(stderr,"%s: Error: No memory for fontmap.  Buy some RAM?\n");
      exit(1);
    }
    fprintf(stderr,"Enlarged the fontmap from %d to %d entries\n",
	    maplidx,maxmap);
  }

  maplidx++;

  if (alias==NULL)
    alias=texname;

  fontmaps[maplidx].texname=strdup(texname);
  fontmaps[maplidx].enc=enc;
  fontmaps[maplidx].extention=ext;
  fontmaps[maplidx].slant=sl;

  fontmaps[maplidx].filename=strdup(alias);
  fontmaps[maplidx].t1libid=-1;
  if (filename!=NULL) {
    fontmaps[maplidx].pathname=strdup(filename);
  } else {
    fontmaps[maplidx].pathname=NULL;
  }
  fontmaps[maplidx].tfmidx=-1;

  if (fontmaps[maplidx].texname==NULL || fontmaps[maplidx].filename==NULL) {
    fprintf(stderr, "%s: Error: Dang, can't find any free memory!\n");
    exit(1);
  }

  return maplidx;
}


PRIVATE void load_font_now P1C(int, idx)

{
  /* At this point xdvi needs to draw a glyph from this font.  But
     first it must be loaded/copied and modified if needed. */

  int t1idx;	/* The fontmap entry number */
  int t1id;	/* The id of the unmodified font */
  int cid;	/* The id of the copied font */
  int enc,sl,ext;

  t1idx=add_t1font(fontmaps[idx].filename,fontmaps[idx].pathname);

  t1id=fontmaps[idx].t1libid=t1fonts[t1idx].t1id;

  if (!t1fonts[t1idx].loaded && T1_LoadFont(t1id)==-1) {
    fprintf(stderr,"Could not load T1 font %s from %s alias %s\n",
	    fontmaps[idx].texname, fontmaps[idx].filename,
	    fontmaps[idx].pathname);
    exit(1);
  }

  fontmaps[idx].tfmidx=add_tfm(fontmaps[idx].texname);

  t1fonts[t1idx].loaded=1;

  /* If there is nothing further to do, just return */
  enc=fontmaps[idx].enc;
  ext=fontmaps[idx].extention;
  sl=fontmaps[idx].slant;

  if (enc==-1 && ext==0 && sl==0)
    return;

  /* The fontmap entry speaks of a modified font.  Copy it first. */
  cid = T1_CopyFont(t1id);

  fprintf(stderr,"t1lib font %d copied to %d to be modified: enc: %d, sl: "
	  "%d ext: %d\n",t1id,cid,enc,sl,ext);

  fontmaps[idx].t1libid=cid;
  
  if (enc!=-1) {
    /* Demand load vector */
    if (encodings[enc].vector==NULL)
      encodings[enc].vector=load_vector(encodings[enc].file);

    if (encodings[enc].vector==NULL) {
      fprintf(stderr,"xdvik: Could not load encoding file %s for vector %s.\n"
	      "The font '%s' is affected by this.\n",
	      encodings[enc].file,encodings[enc].enc,fontmaps[idx].texname);
    } else {
      if (T1_ReencodeFont(cid,encodings[enc].vector)!=0) {
	fprintf(stderr,"%s: WARNING: Re-encoding of %s did not work.\n",
		fontmaps[idx].texname);
      }
    }
  }

  if (ext)
    T1_ExtendFont(cid,ext/1000.0);

  if (sl) 
    T1_SlantFont(cid,sl/10000.0);
  
  return;
}


PUBLIC int find_T1_font P1C(char *,texname)

{
  /* Step 1b in the scenario above.  xdvi knows that this font is
     needed.  Check if it is available.  But do not load yet.
     Return the fontmap index at which the font was found */

  int idx;	/* Iterator, t1 font id */
  char *filename;
  int fl,el;	/* Fontname length, encoding name length */
  int encoded=-1;
  char *mname;	/* Modified font name */

  encoded=0;

  /* First: Check the maps */
  idx=find_texfont(texname);

  if (idx!=-1) {
    if (fontmaps[idx].pathname==NULL) {
      filename=kpse_find_file(fontmaps[idx].filename,kpse_type1_format,0);
      /* It should have been on disk according to the map, but never
         fear, xdvi will try to find it other ways. */
      if (filename==NULL) {
	fprintf(stderr,"%s: Warning: Font map calls for %s, but it was "
		"not found\n",fontmaps[idx].filename);
	return -1;
      }
      fontmaps[idx].pathname=filename;
    }
    return idx;
  }
    
  /* Second: the bare name */
  filename=kpse_find_file(texname,kpse_type1_format,0);

  if (filename!=NULL)
    return setup_encoded_T1_font(texname, NULL, filename, -1, 0, 0);

  /* Third: Implied encoding? */
  fl=strlen(texname);

  for (idx=0; idx<=enclidx; idx++) {

    if (encodings[idx].enc==NULL)
      continue;

    el=strlen(encodings[idx].enc);
    
    if (strcmp(texname+(fl-el),encodings[idx].enc)==0) {
      /* Found a matching encoding */
      fprintf(stderr,"Encoding match: %s - %s\n",texname,encodings[idx].enc);
      mname=malloc(fl+2);
      strcpy(mname,texname);
      mname[fl-el]='\0';
      /* If we had 'ptmr8r' the we now look for 'ptmr' */
      filename=kpse_find_file(mname, kpse_type1_format,0);
      if (filename==NULL) {
	/* No? Look for ptmr8a. 8a postfix is oft used on raw fonts */
	strcat(mname,"8a");
	filename=kpse_find_file(mname,kpse_type1_format,0);
      }
      if (filename!=NULL)
	return setup_encoded_T1_font(texname, mname, filename, idx, 0, 0);

      free(mname);
      /* Now we have tried everything.  Time to give up. */
      return -1;

    } /* If (strcmp...) */
  } /* for */

  return -1;
}

/* ************************* CONFIG FILE READING ************************ */


PRIVATE int new_encoding P2C(char *,enc, char *,file)

{
  /* (Possebly) new encoding entered from .cfg file, or from dvips
     map.  When entered from dvips map the enc is null/anonymous */

  int i;

  /* Shirley!  You're jesting! */
  if (file==NULL) return -1;

  if (enc==NULL) {
    /* Enter by file name.  First check if the file has been entered
       already. */
    for (i=0; i<=enclidx; i++) {
      if (strcmp(encodings[i].file,file)==0)
	return i;
    }
  } else {
    /* Enter by encoding name.  Check if already loaded first. */
    for (i=0; i<=enclidx; i++) {
      if (encodings[i].enc!=NULL && strcmp(encodings[i].enc,enc)==0)
	return i;
    }
  }
  
  /* Bonafide new encoding */

  /* Any space left? */
  if (enclidx==maxenc) {
    maxenc += ENCGROW;
    encodings=realloc(encodings,sizeof(*encodings)*maxenc);
    if (encodings==NULL) {
      fprintf(stderr,"%s: Error: No memory for encoding map.?\n");
      exit(1);
    }
    fprintf(stderr,"Enlarged encoding map from %d to %d entries\n",
	    enclidx,maxenc);
  }

  enclidx++;
#if debug
  fprintf(stderr,"New encoding #%d: '%s' -> '%s'\n",enclidx,enc,file);
#endif
  encodings[enclidx].enc=strdup(enc);
  encodings[enclidx].file=strdup(file);
  encodings[enclidx].vector=NULL;	/* Demand load later */

  if (encodings[enclidx].enc==NULL || encodings[enclidx].file==NULL) {
    fprintf(stderr,"%s: Ran out memory\n");
    exit(0);
  }
	  
  return enclidx;
}


PUBLIC void add_T1_mapentry P4C(char *,name, char *,file, char *,vec, 
			      char *,spec)

{
  /* This is called from getpsinfo, once for each fontmap line.  We
     will dutifully enter the information into our fontmap table */

  static char delim[]="\t ";
  char *last, *current;
  float number;
  int extend=0;
  int slant=0;

#if 0
  fprintf(stderr,"Map entry: %s -> %s(%s) Spec: %s\n",name,file,vec,spec);
#endif

  if (spec!=NULL) {
    /* Try to analyze the postscript string.  We recognize two things:
       "n ExtendFont" and "m SlantFont".  n can be a decimal number in
       which case it's an extention factor, or a integer, in which
       case it's a charspace unit? In any case 850 = .85 */

    last=strtok(spec,delim);
    current=strtok(NULL,delim);
    while (current!=NULL) {
      if (strcmp(current,"ExtendFont")==0) {
	sscanf(last,"%f",&number);
	if (number<10.0)
	  extend = number * 1000.0;
	else 
	  extend = number;
#if 0
	fprintf(stderr,"EXTEND: %d\n",extend);
#endif
      } else if (strcmp(current,"SlantFont")==0) {
	sscanf(last,"%f",&number);
	slant = number * 10000.0;
#if 0
	fprintf(stderr,"SLANT: %d\n",slant);
#endif
      }
      last=current;
      current=strtok(NULL,delim);
    }
  }

  setup_encoded_T1_font(name,file,NULL,new_encoding(NULL,vec),extend,slant);

  if (fontmaps[maplidx].texname==NULL || fontmaps[maplidx].filename==NULL) {
    fprintf(stderr, "%s: Error: Dang, can't find any free memory!\n");
    exit(1);
  }
}


PRIVATE void read_enc_map P1C(char *,file)

     /* Read encoding map file */

{
  char *filename;
  char *enc, *name, *f;
  char *buffer;
  FILE *mapfile;
  int len;
  int i;
  static const char delim[]="\t \n\r";

  filename=kpse_find_file(file,kpse_program_text_format,1);
  if (filename==NULL) {
    fprintf(stderr,"%s: Warning: Unable to find %s\n",file);
    exit(1);
  }

  mapfile = fopen(filename,"r");

  if (mapfile==NULL) {
    fprintf(stderr,"%s: Unable to open %s for reading: ",filename);
    perror("");
    exit(1);
  }

#if debug
  fprintf(stderr,"Reading encoding map file %s\n",filename);
#endif

  buffer=malloc(BUFFER_SIZE);
  if (buffer==NULL) {
    fprintf(stderr,"%s: Out of memory error.\n");
    exit(1);
  }

  while (fgets(buffer, BUFFER_SIZE, mapfile)!=NULL) {
    len=strlen(buffer);
    if (buffer[len-1]!='\n') {
      fprintf(stderr,"%s: Error: Line longer than %d in %s. %d\n",
	      BUFFER_SIZE,filename,(int)buffer[len]);
      exit(1);
    }

    /* char *enc, *name, *f; */
    enc=buffer;

    while (enc[0]!='\0' && (enc[0]==' ' || enc[0]=='\t')) enc++;

    /* % in first column is a correct comment */
    if (enc[0]=='%' || enc[0]=='\0' || enc[0]=='\n') 
      continue;

    enc=strtok(enc,delim);
    name=strtok(NULL,delim);
    f=strtok(NULL,delim);
    i=new_encoding(enc,f);
    fprintf(stderr,"Encoding[%d]: '%s' = '%s' -> '%s'\n", i,enc,name,f);
  }

  fclose(mapfile);

  free(buffer);
  free(filename);
}


PRIVATE void read_cfg_file P1C(char *,file)

     /* Read config file. */

{
  char *filename;
  FILE *mapfile;
  int len;
  char *keyword;
  char *f;
  char *buffer;
  static const char delim[]="\t \n\r";

  if (file==NULL)
    file="xdvi.cfg";

  filename=kpse_find_file(file,kpse_program_text_format,1);
  if (filename==NULL) {
    fprintf(stderr,"%s: Warning: unable to find %s\n",file);
    return;
  }

  mapfile = fopen(filename,"r");

  if (mapfile==NULL) {
    fprintf(stderr,"%s: Error: Unable to open %s for reading: ",filename);
    perror("");
    exit(1);
  }

  fprintf(stderr,"Reading cfg file %s\n",filename);

  buffer=malloc(BUFFER_SIZE);
  if (buffer==NULL) {
    fprintf(stderr,"%s: Out of memory error.\n");
    exit(1);
  }

  while (fgets(buffer, BUFFER_SIZE, mapfile)!=NULL) {
    len=strlen(buffer);
    if (buffer[len-1]!='\n') {
      fprintf(stderr, "%s: Error: Line longer than %d in %s. %d\n",
	      BUFFER_SIZE,filename,(int)buffer[len]);
      exit(1);
    }

    keyword=buffer;

    /* Skip leading whitespace */
    while (keyword[0]!='\0' && (keyword[0]==' ' || keyword[0]=='\t')) 
      keyword++;

    /* % in first column is a correct comment */
    if (keyword[0]=='%' || keyword[0]=='\0' || keyword[0]=='\n') 
      continue;

    keyword=strtok(keyword,delim);

    if (strcmp(keyword,"dvipsmap")==0) {
      f=strtok(NULL,delim);
#if debug
      fprintf(stderr,"DVIPSMAP: '%s'\n",f);
#endif
      if (!getpsinfo(f)) {
	fprintf(stderr,"Could not load dvips map %s\n",f);
	exit(1);
      }
    } else if (strcmp(keyword,"encmap")==0) {
      f=strtok(NULL,delim);
#if debug
      fprintf(stderr,"Encoding map: '%s'\n",f);
#endif
      read_enc_map(f);
    } else {
      fprintf(stderr,"%s: Error: Unknown keyword: '%s' in %s\n",
	      keyword,filename);
      exit(1);
    }
  }

  fclose(mapfile);

  free(buffer);
  free(filename);

  return;
}


/* **************************** GLYPH DRAWING *************************** */


/* Set character# ch */

#ifdef TEXXET
PUBLIC void set_t1_char P2C(wide_ubyte, cmd, wide_ubyte, ch)
#else
PUBLIC long set_t1_char P1C(wide_ubyte, ch)
#endif

{
  GC gc;
  int xpos = PXL_H - currwin.base_x ;
  /* + 1 here, seems t1lib and xdvi rounds the baseline differently */
  int ypos = PXL_V - currwin.base_y + 1;
  int t1libid;

  /* Side by side comparison of cmr10 and ecrm1000 suggests that t1lib
     renders the fonts a touch too large, and a factor of 0.96 was
     arrived at as a reasonable correction for this.  

     Comparing the test file in gv at large sizes indicates that the
     t1 cmr10 font is a hair taller than ecrm1000 also as seen by gv.
     But, the size difference in xdvi was bigger.

     Another related problem is that there are some gaps in math modes
     tall braces '{', (see for example the amstex users guide).  When
     the factor is applied these gaps are more pronounced and there
     are more of them, compared to when the factor is not applied.
     The gaps does not show up in the fullsize window, and when the
     0.96 factor is not applied it only seems to show up in the main
     window when the shrink factor is even.

     Also, the size difference is in no way obvious unless you're
     looking for it.

     All in all the factor has been droped pending further experience
     with t1 fonts in xdvi.

   */

  float size=
    dvi_pt_conv(currinf.fontp->scale*(pixels_per_inch/size_devisor))/72.27;

  int id=currinf.fontp->t1id;
  int dvi_h_sav;

  long gw ;

  /* size=size*0.96; */

  if (fontmaps[id].t1libid==-1)
    load_font_now(id);

  t1libid=fontmaps[id].t1libid;

#if 0
  fprintf(stderr,"Setting '%c' of %d, at %ld(%.2fpt), shrinkage is %d\n",
	  ch, t1libid, currinf.fontp->scale, 
	  dvi_pt_fconv(currinf.fontp->scale),shrink_factor);
#endif

  if (fontmaps[id].tfmidx==-1)
    /* Fallback when tfm data is missing.  Not precise enough for
       tables with vertical lines. */
    gw = t1_dvi_conv(T1_GetCharWidth(t1libid, ch)*size)*
      pixels_per_inch/72;
  else
    /* Use the widht from the tfm file */
    gw = tfminfo[fontmaps[id].tfmidx].widths[ch] * currinf.fontp->dimconv;

#ifdef HTEX
  gc = HTeXreflevel > 0? highGC: foreGC;
#else
  gc = foreGC;
#endif

#ifdef TEXXET
  dvi_h_sav = DVI_H;
  if (currinf.dir < 0) DVI_H -= gw;

  /* FIXME: There is something about scan_frame in set_char, I have no
     idea what it does, and dvi-draw does not export it either. */
#endif

#ifdef GREY  
  if (use_grey && !(shrink_factor==1)) {
    T1_AASetCharX(currwin.win, gc, T1_OPAQUE, xpos, ypos, 
		  t1libid, ch, size/(double) shrink_factor, NULL);
  } else
#endif
    {
      T1_SetCharX(currwin.win, gc, T1_OPAQUE, xpos, ypos, t1libid,
		  ch, size/(double) shrink_factor, NULL);
    }

#ifndef TEXXET
  return gw;
#else
# ifdef Omega
  if ((cmd == PUT1) || (cmd == PUT2))
# else
  if (cmd == PUT1)
# endif
    DVI_H = dvi_h_sav;
  else
    if (currinf.dir > 0) 
      DVI_H += gw;
#endif
}


PUBLIC void read_T1_char P2C(struct font *,fontp, ubyte, ch)

{
  /* Should never be called */
  fprintf(stderr,"BUG: asked to load %c\n",ch);
}


PUBLIC void init_t1 P1C(void,)

{
  /* Initialize t1lib for drawing marks on screen */

  if ((T1_InitLib(LOGFILE|
		  IGNORE_CONFIGFILE|
		  IGNORE_FONTDATABASE|
		  T1_NO_AFM)==NULL)) {
    fprintf(stderr, "%s: Error: Initialization of t1lib failed\n");
    exit(1);
  }
  T1_SetLogLevel(T1LOG_DEBUG);

  T1_SetDeviceResolutions((float)pixels_per_inch,(float)pixels_per_inch);

  /* Thanks to my wife for figuring this out.  My mind was putty after
     struggeling with too many kinds of units and factors for too
     long: */
  size_devisor=pixels_per_inch/75;

  T1_SetX11Params(DISP, our_visual, our_depth, our_colormap);
  /* T1_AA_LOW seems better than T1_AA_HIGH */
  T1_AASetLevel(T1_AA_HIGH);
  /* T1_AASetLevel(T1_AA_LOW);*/
  T1_AASetBitsPerPixel(our_depth);

  encodings=malloc(sizeof(*encodings)*ENCSTART);
  fontmaps=malloc(sizeof(*fontmaps)*FNTMAPSTART);
  t1fonts=malloc(sizeof(*t1fonts)*T1START);
  tfminfo=malloc(sizeof(*tfminfo)*TEXWSTART);

  if (encodings==NULL || fontmaps==NULL || t1fonts==NULL) {
    fprintf(stderr,"%s: Error: No memory for datastructures.\n");
    exit(1);
  }

  /* Malloc mem before reading config files! */
  read_cfg_file(NULL);
}

#endif /* t1 */
