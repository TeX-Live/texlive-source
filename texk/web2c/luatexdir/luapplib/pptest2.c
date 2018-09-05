
#include <stdio.h>
#include <assert.h>
#include "ppapi.h"

static const char * get_file_name (const char *path)
{
  const char *fn, *p;
  for (fn = p = path; *p != '\0'; ++p)
    if (*p == '\\' || *p == '/')
      fn = p + 1;
  return fn;
}

static void box_info (ppdict *pagedict, FILE *fh)
{
  const char *boxes[] = {"MediaBox", "CropBox", "BleedBox", "TrimBox", "ArtBox"};
  pprect rect;
  size_t i;
  for (i = 0; i < sizeof(boxes) / sizeof(const char *); ++i)
    if (ppdict_get_box(pagedict, boxes[i], &rect))
      fprintf(fh, "%%%% %s [%f %f %f %f]\n", boxes[i], rect.lx, rect.ly, rect.rx, rect.ry);
}

static int usage (const char *argv0)
{
	printf("pplib " pplib_version ", " pplib_author "\n");
	printf("usage: %s file1.pdf file2.pdf ...\n", argv0);
	return 0;
}

#define OUTDIR "."

static void log_callback (const char *message, void *alien)
{
	fprintf((FILE *)alien, "\nooops: %s\n", message);
}

int main (int argc, const char **argv)
{
  const char *filepath, *filename;
  int a;
  ppdoc *pdf;
  ppref *pageref;
  ppdict *pagedict;
  int pageno;
  char outname[1024];
  FILE *fh;
  ppstream *stream;
  uint8_t *data;
  size_t size;
  ppcontext *context;
  ppobj *obj;
  ppname op;
  size_t operators;

  if (argc < 2)
    return usage(argv[0]);
  ppstream_init_buffers();
  pplog_callback(log_callback, stderr);
  context = ppcontext_new();
  for (a = 1; a < argc; ++a)
  {
    filepath = argv[a];
    printf("loading %s... ", filepath);
    pdf = ppdoc_load(filepath);
    if (pdf == NULL)
    {
      printf("failed\n");
      continue;
    }
    printf("done.\n");
    switch (ppdoc_crypt_status(pdf))
    {
    	case PPCRYPT_NONE:
    	case PPCRYPT_DONE:
    		break;
    	case PPCRYPT_PASS:
    		if (ppdoc_crypt_pass(pdf, "dummy", 5, NULL, 0) == PPCRYPT_DONE || ppdoc_crypt_pass(pdf, NULL, 0, "dummy", 5) == PPCRYPT_DONE)
    		  break;
        printf("sorry, password needed\n");
        ppdoc_free(pdf);
        continue;
    	case PPCRYPT_FAIL:
    		printf("sorry, encryption failed\n");
    		ppdoc_free(pdf);
    		continue;
    }
    filename = get_file_name(filepath);
    sprintf(outname, OUTDIR "/%s.out", filename);
    fh = fopen(outname, "wb");
    if (fh == NULL)
    {
      printf("can't open %s for writing\n", outname);
      continue;
    }
    for (pageref = ppdoc_first_page(pdf), pageno = 1;
         pageref != NULL;
         pageref = ppdoc_next_page(pdf), ++pageno)
    {
      pagedict = pageref->object.dict;
      /* decompress contents data */
      fprintf(fh, "%%%% PAGE %d\n", pageno);
      box_info(pagedict, fh);
      for (stream = ppcontents_first(pagedict);
           stream != NULL;
           stream = ppcontents_next(pagedict, stream))
      {
        for (data = ppstream_first(stream, &size, 1);
             data != NULL;
             data = ppstream_next(stream, &size))
          fwrite(data, size, 1, fh);
        ppstream_done(stream);
      }
      /* now parse contents */
      for (stream = ppcontents_first(pagedict);
           stream != NULL;
           stream = ppcontents_next(pagedict, stream))
      {
        operators = 0;
        for (obj = ppcontents_first_op(context, stream, &size, &op);
             obj != NULL;
             obj = ppcontents_next_op(context, stream, &size, &op))
          ++operators;
        fprintf(fh, "%%%% OPERATORS count " PPSIZEF "\n", operators);
        ppstream_done(stream);
        //obj = ppcontents_parse(context, stream, &size);
        //fprintf(fh, "%%%% items count " PPSIZEF "\n", size);
        fprintf(fh, "\n");
      }
      ppcontext_done(context);
    }
    fclose(fh);
    ppdoc_free(pdf);
  }
  ppcontext_free(context);
	ppstream_free_buffers();
  return 0;
}
