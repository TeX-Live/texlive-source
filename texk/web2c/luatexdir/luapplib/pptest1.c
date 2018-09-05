
#include <stdio.h>
#include "ppapi.h"

static const char * sizenum (size_t s)
{
  static char buffer[32];
  if (s < 1000)
    sprintf(buffer, "%uB", (unsigned)s);
  else if (s < 1000000)
    sprintf(buffer, "%.2fkB", (double)(s) / 1000);
  else
    sprintf(buffer, "%.2fMB", (double)(s) / 1000000);
  return buffer;
}

static const char * crypt_info (ppdoc *pdf)
{
	switch (ppdoc_crypt_status(pdf))
	{
		case PPCRYPT_NONE:
			return "none";
		case PPCRYPT_DONE:
			return "empty password";
		case PPCRYPT_PASS:
			return "nonempty password";
		default:
			break;
	}
	return "this shouldn't happen";
}

static void print_info (ppdoc *pdf)
{
  ppdict *info;
  ppstring creator, producer;
  size_t memused, memwaste;

  if ((info = ppdoc_info(pdf)) != NULL)
  {
    if ((creator = ppdict_rget_string(info, "Creator")) != NULL)
      printf("  creator: %s\n", ppstring_decoded(creator));
    if ((producer = ppdict_rget_string(info, "Producer")) != NULL)
      printf("  producer: %s\n", ppstring_decoded(producer));
  }
  printf("  version: %s\n", ppdoc_version_string(pdf));
  printf("  protection: %s\n", crypt_info(pdf));
  printf("  filesize: %s\n", sizenum(ppdoc_file_size(pdf)));
  printf("  objects: " PPUINTF "\n", ppdoc_objects(pdf));
  printf("  pagecount: " PPUINTF "\n", ppdoc_page_count(pdf));
  memused = ppdoc_memory(pdf, &memwaste);
  printf("  memused: %s\n", sizenum(memused));
  printf("  memwaste: %s\n", sizenum(memwaste));
}

static int usage (const char *argv0)
{
	printf("pplib " pplib_version ", " pplib_author "\n");
	printf("usage: %s file1.pdf file2.pdf ...\n", argv0);
	return 0;
}

int main (int argc, const char **argv)
{
  const char *filepath;
  int a;
  ppdoc *pdf;

  if (argc < 2)
    return usage(argv[0]);
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
    print_info(pdf);
    ppdoc_free(pdf);
  }
  return 0;
}
