#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lzmadec.h>
#include "../lzma_version.h"

#ifdef WIN32
#include <fcntl.h>
#endif

#define CHUNKSIZE_IN 4096
#define CHUNKSIZE_OUT (1024*512)

int main(int argc, char **argv)
{
	int ret;
	lzmadec_stream strm;
	size_t write_size;
	FILE *file_in = stdin;
	FILE *file_out = stdout;
	unsigned char *buffer_in = malloc (CHUNKSIZE_IN);
	unsigned char *buffer_out = malloc (CHUNKSIZE_OUT);

	/* Check the command line arguments. */
	if (argc > 1 && 0 == strcmp (argv[1], "--help")) {
		printf (/* Breaking the indentation style */
"\nLZMAdec - a small LZMA decoder\n\n"
"Usage: %s [--help]\n\n"
"The compressed data is read from stdin and uncompressed to stdout.\n\n"
"LZMA SDK version %s - %s\n"
"LZMA Utils version %s - %s\n"
"\nLZMAdec is free software licensed under the GNU LGPL.\n\n",
			argv[0],
			LZMA_SDK_VERSION_STRING, LZMA_SDK_COPYRIGHT_STRING,
			LZMA_UTILS_VERSION_STRING, LZMA_UTILS_COPYRIGHT_STRING);
		return 0;
	}

#ifdef WIN32
	setmode(fileno(stdout), O_BINARY);
	setmode(fileno(stdin), O_BINARY);
#endif

	if (buffer_in == NULL || buffer_out == NULL) {
		fprintf (stderr, "%s: Not enough memory.\n", argv[0]);
		return 5;
	}

	/* Other initializations */
	strm.lzma_alloc = NULL;
	strm.lzma_free = NULL;
	strm.opaque = NULL;
	strm.avail_in = 0;
	strm.next_in = NULL;
	if (lzmadec_init (&strm) != LZMADEC_OK) {
		fprintf (stderr, "Not enough memory.\n");
		return 2;
	}

	/* Decode */
	while (1) {
		if (strm.avail_in == 0) {
			strm.next_in = buffer_in;
			strm.avail_in = fread (buffer_in, sizeof (unsigned char),
					CHUNKSIZE_IN, file_in);
		}
		strm.next_out = buffer_out;
		strm.avail_out = CHUNKSIZE_OUT;
		ret = lzmadec_decode (&strm, strm.avail_in == 0);
		if (ret != LZMADEC_OK && ret != LZMADEC_STREAM_END)
			return 1;
		write_size = CHUNKSIZE_OUT - strm.avail_out;
		if (write_size != (fwrite (buffer_out, sizeof (unsigned char),
				write_size, file_out)))
			return 2;
		if (ret == LZMADEC_STREAM_END) {
			lzmadec_end (&strm);
			return 0;
		}
	}
}
