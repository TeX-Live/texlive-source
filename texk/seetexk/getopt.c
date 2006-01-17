/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * getopt - get option letter from argv
 * (From Henry Spencer @ U of Toronto Zoology, slightly edited)
 */

#include <stdio.h>

char	*optarg;	/* Global argument pointer. */
int	optind;		/* Global argv index. */

static char *scan;	/* Private scan pointer. */
#if defined( WIN32 ) || defined( _AMIGA )
#include <string.h>
#define index strchr
#else
extern char *index();
#endif

int
getopt(argc, argv, optstring)
	register int argc;
	register char **argv;
	char *optstring;
{
	register int c;
	register char *place;

	optarg = NULL;
	if (scan == NULL || *scan == 0) {
		if (optind == 0)
			optind++;
		if (optind >= argc || argv[optind][0] != '-' ||
		    argv[optind][1] == 0)
			return (EOF);
		if (strcmp(argv[optind], "--") == 0) {
			optind++;
			return (EOF);
		}
		scan = argv[optind] + 1;
		optind++;
	}
	c = *scan++;
	place = index(optstring, c);

	if (place == NULL || c == ':') {
		fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
		return ('?');
	}
	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else {
			if (optind >= argc) {
				fprintf(stderr,
					"%s: missing argument after -%c\n",
					argv[0], c);
				return ('?');
			}
			optarg = argv[optind];
			optind++;
		}
	}
	return (c);
}
