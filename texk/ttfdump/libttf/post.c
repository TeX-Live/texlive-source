#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: post.c,v 1.3 1998/06/06 12:55:53 werner Exp $	 */

static void ttfLoadPOST(FILE *fp, POSTPtr post, ULONG offset);

void ttfInitPOST(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('p', 'o', 's', 't');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag, font)) != NULL)
	{
	    font->post = XCALLOC1 (POST);
	    ttfLoadPOST(font->fp, font->post, ptd->offset);
	}
}

static void ttfLoadPOST(FILE *fp, POSTPtr post, ULONG offset)
{
    USHORT i,numGlyphs;
    xfseek(fp, offset, SEEK_SET, "ttfLoadPOST");

    post->format = ttfGetFixed(fp);
    post->italicAngle = ttfGetFixed(fp);
    post->underlinePosition = ttfGetFWord(fp);
    post->underlineThickness = ttfGetFWord(fp);
    post->isFixedPitch = ttfGetULONG(fp);
    post->minMemType42 = ttfGetULONG(fp);
    post->maxMemType42 = ttfGetULONG(fp);
    post->minMemType1 = ttfGetULONG(fp);
    post->maxMemType1 = ttfGetULONG(fp);

    switch (post->format)
	{
	case 0x00020000:
	    post->name.format20 = XCALLOC1 (Format20);
	    post->name.format20->numGlyphs = numGlyphs = ttfGetUSHORT(fp);
	    post->name.format20->glyphNameIndex = ttfMakeUSHORT (numGlyphs, fp);

	    post->name.format20->GlyphName = XCALLOC (numGlyphs, CHAR *);
	    for (i=0;i<numGlyphs;i++)
		{
		    unsigned char len;
		    if (post->name.format20->glyphNameIndex[i] <= 257)
			  {
			      /* do nothing for standard Mac glyf name */
			  }
		      else if (post->name.format20->glyphNameIndex[i] <= 32767)
			{
			    /* non-standard glyf name is stored as a Pascal 
			     * string in the file i.e.  
			     * the first byte is the length of the string 
			     * but the string is not ended with a null 
			     * character */
			    len = (unsigned char) ttfGetCHAR(fp);
			    post->name.format20->GlyphName[i] = XTALLOC (len+1, CHAR);
			    if (len)
				fread(post->name.format20->GlyphName[i],
				      sizeof(CHAR), len, fp);
			    post->name.format20->GlyphName[i][len] = 0x0;
			}
		}
	    break;
	case 0x00028000:                        /* 2.5 in 16.16 format */
	    /* not implemented yet */
	    break;
	default:
	    /* do nothing */ ;
	}
}

void ttfPrintPOST(FILE *fp,POSTPtr post)
{
    int b[2],b1[2];
    USHORT i,numGlyphs;
    
    FixedSplit(post->format, b);
    FixedSplit(post->italicAngle, b1);
    fprintf(fp,"'post' Table - PostScript\n");
    fprintf(fp,"-------------------------\n");
    fprintf(fp,"\t 'post' format:\t\t %d.%d\n", b[1], b[0]);
    fprintf(fp,"\t italicAngle:\t\t %d.%d\n", b1[1], b1[0]);
    fprintf(fp,"\t underlinePosition:\t %d\n", post->underlinePosition);
    fprintf(fp,"\t underlineThichness:\t %d\n", post->underlineThickness);
    fprintf(fp,"\t isFixedPitch:\t\t %ud\n", post->isFixedPitch);
    fprintf(fp,"\t minMemType42:\t\t %ud\n", post->minMemType42);
    fprintf(fp,"\t maxMemType42:\t\t %ud\n", post->maxMemType42);
    fprintf(fp,"\t minMemType1:\t\t %ud\n", post->minMemType1);
    fprintf(fp,"\t maxMemType1:\t\t %ud\n", post->maxMemType1);

    switch (post->format)
	{
	  case 0x00020000:
	      numGlyphs = post->name.format20->numGlyphs;
	      fprintf(fp, "\t Format 2.0:  Non-Standard (for PostScript) TrueType Glyph Set.\n");
	      fprintf(fp,"\t\t numGlyphs:\t %d\n",
		      post->name.format20->numGlyphs);
	      for (i=0;i<numGlyphs;i++)
		  {
		      if (post->name.format20->glyphNameIndex[i] < 257)
			  {
			      fprintf(fp,"\t\t Glyf %3d  -> Mac Glyph # %3d\n", i,
				      post->name.format20->glyphNameIndex[i]);
			  }
		      else if (post->name.format20->glyphNameIndex[i] < 32767)
			  {
			      fprintf(fp,"\t\t Glyf %3d  -> PSGlyf Name # %3d, '%s'\n",
				      i, post->name.format20->glyphNameIndex[i] - 257,
				      post->name.format20->GlyphName[i]);
			  }
		  }
	    break;
	case 0x00020005:
	    /* not implemented yet */
	    break;
	default:
	    /* do nothing */;
	}
}

void ttfFreePOST(POSTPtr post)
{
    USHORT i, numGlyphs;

    if (!post)
	return;

    switch (post->format)
	{
	case 0x00020000:
	    numGlyphs = post->name.format20->numGlyphs;
	    for (i=0;i<numGlyphs;i++)
		{
		   if (post->name.format20->glyphNameIndex[i] > 257)
		       free(post->name.format20->GlyphName[i]);
		}
	    free(post->name.format20->glyphNameIndex);
	    free(post->name.format20->GlyphName);
	    free(post->name.format20);
	    break;
	case 0x00020005:
	    /* not implemented yet */
	    break;
	default:
	    /* do nothing */;
	}
    free(post);
}
