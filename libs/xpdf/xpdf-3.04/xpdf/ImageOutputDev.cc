//========================================================================
//
// ImageOutputDev.cc
//
// Copyright 1998-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include "gmem.h"
#include "config.h"
#include "Error.h"
#include "GfxState.h"
#include "Object.h"
#include "Stream.h"
#include "ImageOutputDev.h"

ImageOutputDev::ImageOutputDev(char *fileRootA, GBool dumpJPEGA) {
  fileRoot = copyString(fileRootA);
  fileName = (char *)gmalloc((int)strlen(fileRoot) + 20);
  dumpJPEG = dumpJPEGA;
  imgNum = 0;
  ok = gTrue;
}

ImageOutputDev::~ImageOutputDev() {
  gfree(fileName);
  gfree(fileRoot);
}

void ImageOutputDev::tilingPatternFill(GfxState *state, Gfx *gfx,
				       Object *strRef,
				       int paintType, Dict *resDict,
				       double *mat, double *bbox,
				       int x0, int y0, int x1, int y1,
				       double xStep, double yStep) {
  // do nothing -- this avoids the potentially slow loop in Gfx.cc
}

void ImageOutputDev::drawImageMask(GfxState *state, Object *ref, Stream *str,
				   int width, int height, GBool invert,
				   GBool inlineImg, GBool interpolate) {
  FILE *f;
  char buf[4096];
  int size, n, i;

  // dump JPEG file
  if (dumpJPEG && str->getKind() == strDCT && !inlineImg) {

    // open the image file
    sprintf(fileName, "%s-%04d.jpg", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }

    // initialize stream
    str = ((DCTStream *)str)->getRawStream();
    str->reset();

    // copy the stream
    while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, n, f);
    }

    str->close();
    fclose(f);

  // dump PBM file
  } else {

    // open the image file and write the PBM header
    sprintf(fileName, "%s-%04d.pbm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P4\n");
    fprintf(f, "%d %d\n", width, height);

    // initialize stream
    str->reset();

    // copy the stream
    size = height * ((width + 7) / 8);
    while (size > 0) {
      i = size < (int)sizeof(buf) ? size : (int)sizeof(buf);
      n = str->getBlock(buf, i);
      fwrite(buf, 1, n, f);
      if (n < i) {
	break;
      }
      size -= n;
    }

    str->close();
    fclose(f);
  }
}

void ImageOutputDev::drawImage(GfxState *state, Object *ref, Stream *str,
			       int width, int height,
			       GfxImageColorMap *colorMap,
			       int *maskColors, GBool inlineImg,
			       GBool interpolate) {
  FILE *f;
  ImageStream *imgStr;
  Guchar *p;
  GfxRGB rgb;
  int x, y;
  char buf[4096];
  int size, n, i;

  // dump JPEG file
  if (dumpJPEG && str->getKind() == strDCT &&
      (colorMap->getNumPixelComps() == 1 ||
       colorMap->getNumPixelComps() == 3) &&
      !inlineImg) {

    // open the image file
    sprintf(fileName, "%s-%04d.jpg", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }

    // initialize stream
    str = ((DCTStream *)str)->getRawStream();
    str->reset();

    // copy the stream
    while ((n = str->getBlock(buf, sizeof(buf))) > 0) {
      fwrite(buf, 1, n, f);
    }

    str->close();
    fclose(f);

  // dump PBM file
  } else if (colorMap->getNumPixelComps() == 1 &&
	     colorMap->getBits() == 1) {

    // open the image file and write the PBM header
    sprintf(fileName, "%s-%04d.pbm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P4\n");
    fprintf(f, "%d %d\n", width, height);

    // initialize stream
    str->reset();

    // copy the stream
    size = height * ((width + 7) / 8);
    while (size > 0) {
      i = size < (int)sizeof(buf) ? size : (int)sizeof(buf);
      n = str->getBlock(buf, i);
      fwrite(buf, 1, n, f);
      if (n < i) {
	break;
      }
      size -= n;
    }

    str->close();
    fclose(f);

  // dump PPM file
  } else {

    // open the image file and write the PPM header
    sprintf(fileName, "%s-%04d.ppm", fileRoot, imgNum);
    ++imgNum;
    if (!(f = fopen(fileName, "wb"))) {
      error(errIO, -1, "Couldn't open image file '{0:s}'", fileName);
      return;
    }
    fprintf(f, "P6\n");
    fprintf(f, "%d %d\n", width, height);
    fprintf(f, "255\n");

    // initialize stream
    imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(),
			     colorMap->getBits());
    imgStr->reset();

    // for each line...
    for (y = 0; y < height; ++y) {

      // write the line
      if ((p = imgStr->getLine())) {
	for (x = 0; x < width; ++x) {
	  colorMap->getRGB(p, &rgb);
	  fputc(colToByte(rgb.r), f);
	  fputc(colToByte(rgb.g), f);
	  fputc(colToByte(rgb.b), f);
	  p += colorMap->getNumPixelComps();
	}
      } else {
	for (x = 0; x < width; ++x) {
	  fputc(0, f);
	  fputc(0, f);
	  fputc(0, f);
	}
      }
    }

    imgStr->close();
    delete imgStr;

    fclose(f);
  }
}

void ImageOutputDev::drawMaskedImage(GfxState *state, Object *ref, Stream *str,
				     int width, int height,
				     GfxImageColorMap *colorMap,
				     Stream *maskStr,
				     int maskWidth, int maskHeight,
				     GBool maskInvert, GBool interpolate) {
  drawImage(state, ref, str, width, height, colorMap,
	    NULL, gFalse, interpolate);
  drawImageMask(state, ref, maskStr, maskWidth, maskHeight, maskInvert,
		gFalse, interpolate);
}

void ImageOutputDev::drawSoftMaskedImage(GfxState *state, Object *ref,
					 Stream *str,
					 int width, int height,
					 GfxImageColorMap *colorMap,
					 Stream *maskStr,
					 int maskWidth, int maskHeight,
					 GfxImageColorMap *maskColorMap,
					 GBool interpolate) {
  drawImage(state, ref, str, width, height, colorMap,
	    NULL, gFalse, interpolate);
  drawImage(state, ref, maskStr, maskWidth, maskHeight, maskColorMap,
	    NULL, gFalse, interpolate);
}
