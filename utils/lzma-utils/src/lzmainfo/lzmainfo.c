/******************************************************************************

    Simple tool to view LZMA header information. Uses liblzmadec.

    Copyright (C) 2005 Lasse Collin <lasse.collin@tukaani.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

******************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
extern int errno
#endif

#include "../lzma_version.h"

/* This can be compiled without liblzmadec's stdio support. */
#define LZMADEC_NO_STDIO
#include <lzmadec.h>


/* Extremely simple and limited logarithm function */
static uint_fast32_t
Log2 (uint_fast32_t n)
{
	uint_fast32_t e;
	for (e = 0; n > 1; e++, n /= 2);
	return (e);
}

static int
lzmainfo (FILE *file, const char *name, const char *argv0)
{
	lzmadec_info info;
	uint8_t buf[LZMADEC_MINIMUM_SIZE];
	/* Read the first bytes of the file. */
	if (LZMADEC_MINIMUM_SIZE != fread (buf, sizeof (uint8_t),
			LZMADEC_MINIMUM_SIZE, file)) {
		fprintf (stderr, "%s: %s: Too small to be an LZMA "
				"encoded file.\n", argv0, name);
		return 1; /* Error */
	} else if (LZMADEC_OK != lzmadec_buffer_info (
			&info, buf, LZMADEC_MINIMUM_SIZE)) {
		fprintf (stderr, "%s: %s: Invalid LZMA header.\n",
				argv0, name);
		return 1;
	} else {
		if (file != stdin)
			printf ("%s\n", name);
		printf ("Uncompressed size:             ");
		if (info.is_streamed)
			printf ("Unknown");
		else
			printf ("%llu MB (%llu bytes)",
					(info.uncompressed_size + 512 * 1024)
					/ (1024 * 1024),
					info.uncompressed_size);
		printf ("\nDictionary size:               "
				"%u MB (2^%u bytes)\n"
				"Literal context bits (lc):     %d\n"
				"Literal pos bits (lp):         %d\n"
				"Number of pos bits (pb):       %d\n",
				(info.dictionary_size + 512 * 1024)
				/ (1024 * 1024),
				Log2 (info.dictionary_size),
				(int)info.lc,
				(int)info.lp,
				(int)info.pb);
	}
	fclose (file);
	return 0;
}

int
main (int argc, char **argv)
{
	FILE *file;
	int i;
	int ret = 0;

	/* Check the command line arguments. */
	if (argc < 2) {
		/* No arguments, reading standard input. */
		ret = lzmainfo (stdin, "(standard input)", argv[0]);
		return ret;
	} else if (0 == strcmp (argv[1], "--help")) {
		printf (/* Breaking the indentation style */
"\nLZMAinfo - Show information stored in the LZMA file header\n\n"
"Usage: lzmainfo [--help | filename.lzma [filename2.lzma ...]]\n\n"
"If no filename is specified lzmainfo reads stdin.\n"
"The information is always printed to stdout.\n\n"
"LZMA SDK version %s - %s\n"
"LZMA utils version %s - %s\n"
"\nLZMAinfo is free software licensed under the GNU LGPL.\n\n",
			LZMA_SDK_VERSION_STRING, LZMA_SDK_COPYRIGHT_STRING,
			LZMA_UTILS_VERSION_STRING, LZMA_UTILS_COPYRIGHT_STRING);
		return 0;
	}

	/* Show information about files listed on the command line. */
	printf ("\n");
	for (i = 1; i < argc; i++) {
		file = fopen (argv[i], "rb");
		if (file == NULL) {
			fprintf (stderr, "%s: %s: %s\n", argv[0], argv[i],
					strerror (errno));
			continue;
		}
		ret |= lzmainfo (file, argv[i], argv[0]);
		printf ("\n");
	}
	return ret;
}
