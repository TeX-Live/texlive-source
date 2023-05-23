
#include <stdio.h>
#include <assert.h>
#include "ppapi.h"

/*
static const char * get_file_name (const char *path)
{
  const char *fn, *p;
  for (fn = p = path; *p != '\0'; ++p)
    if (*p == '\\' || *p == '/')
      fn = p + 1;
  return fn;
}
*/

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
  printf("       %s file.pdf -u userpassword\n", argv0);
  printf("       %s file.pdf -o ownerpassword\n", argv0);
  printf("       %s file.pdf -p bothpasswords\n", argv0);
  return 0;
}

static void log_callback (const char *message, void *alien)
{
  fprintf((FILE *)alien, "\nooops: %s\n", message);
}

static const char * get_next_argument (const char *opt, int *a, int argc, const char **argv)
{
  const char *next;
  if ((*a) + 2 < argc)
  {
    next = argv[*a + 1];
    if (strcmp(next, opt) == 0)
    {
      *a += 2;
      return argv[*a];
    }
  }
  return NULL;
}

int main (int argc, const char **argv)
{
  const char *filepath, *password;
  int a;
  ppdoc *pdf;
  ppcrypt_status cryptstatus;
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
  ppname *op;
  size_t operators;

  if (argc < 2)
    return usage(argv[0]);
  ppstream_init_buffers();
  pplog_callback(log_callback, stderr);
  context = ppcontext_new();
  for (a = 1; a < argc; ++a)
  {
    /* load */
    filepath = argv[a];
    printf("loading %s... ", filepath);
    pdf = ppdoc_load(filepath);
    if (pdf == NULL)
    {
      printf("failed\n");
      continue;
    }
    printf("done\n");

    /* decrypt */
    if ((password = get_next_argument("-u", &a, argc, argv)) != NULL)
      cryptstatus = ppdoc_crypt_pass(pdf, password, strlen(password), NULL, 0);
    else if ((password = get_next_argument("-o", &a, argc, argv)) != NULL)
      cryptstatus = ppdoc_crypt_pass(pdf, NULL, 0, password, strlen(password));
    else if ((password = get_next_argument("-p", &a, argc, argv)) != NULL)
      cryptstatus = ppdoc_crypt_pass(pdf, password, strlen(password), password, strlen(password));
    else
      cryptstatus = ppdoc_crypt_status(pdf);
    switch (cryptstatus)
    {
      case PPCRYPT_NONE:
        break;
      case PPCRYPT_DONE:
        printf("opened with password '%s'\n", password != NULL ? password : "");
        break;
      case PPCRYPT_PASS:
        printf("invalid password\n");
        ppdoc_free(pdf);
        continue;
      case PPCRYPT_FAIL:
        printf("invalid encryption\n");
        ppdoc_free(pdf);
        continue;
    }

    /* process */
    sprintf(outname, "%s.out", filepath);
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
        fprintf(fh, "%%%% OPERATORS count %lu\n", (unsigned long)operators);
        ppstream_done(stream);
        //obj = ppcontents_parse(context, stream, &size);
        //fprintf(fh, "%%%% items count %lu\n", (unsigned long)size);
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
