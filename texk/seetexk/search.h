/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/* search structures and routines for 32-bit key, arbitrary data */

struct search {
	unsigned s_dsize;	/* data object size (includes key size) */
	unsigned s_space;	/* space left (in terms of objects) */
	unsigned s_n;		/* number of objects in the table */
	char	*s_data;	/* data area */
};

/* returns a pointer to the search table (for future search/installs) */
struct	search *SCreate();	/* create a search table */

/* returns a pointer to the data object found or created */
char	*SSearch();		/* search for a data object */

/* clears a search table (wipes out all entries) */
#define	SClear(s)	((s)->s_space += (s)->s_n, (s)->s_n = 0)

/* the third argument to SSearch controls operation as follows: */
#define	S_LOOKUP	0x00	/* pseudo flag */
#define	S_CREATE	0x01	/* create object if not found */
#define	S_EXCL		0x02	/* complain if already exists */

/* in addition, it is modified before return to hold status: */
#define	S_COLL		0x04	/* collision (occurs iff S_EXCL set) */
#define	S_FOUND		0x08	/* found (occurs iff existed already) */
#define	S_NEW		0x10	/* created (occurs iff S_CREATE && !S_EXCL) */
#define	S_ERROR		0x20	/* problem creating (out of memory) */

void SEnumerate();
