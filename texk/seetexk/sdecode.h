/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Specials consist of keyword/value groupings, with the simple syntax
 * suggested by the BNF
 *
 *	list ::=	kv | list ',' kv
 *	kv ::=		word opt_eq words
 *	opt_eq ::=	'=' | <empty>
 *	words ::=	word | words ' ' word
 *
 * `Words' include any character that is not a space, comma, or quote,
 * and can include these by using quoting.  Inside and outside quotes,
 * `\"' produces `"'; `\\' produces `\'; and `\,' produces `,'.
 *
 * No single word can exceed WDMAX characters (after quotes are interpreted).
 * WDMAX is defined in sdecode.c, and is typically large (>= 1024).
 *
 * Each special is typed according to its arguments, which are collected
 * and decoded, and passed to the special function as ordinary parameters.
 * All possible argument-types must be known to the special decoder.
 * The function also receives its own name (from the table) as its
 * first argument.
 */

/* value argument types */
enum sd_args {
	sda_none,	/* no arguments, just a flag */
	sda_s,		/* one string argument (`foo abc') */
	sda_d,		/* one decimal (i32) argument (`foo 2') */
	sda_f,		/* one floating (double) argument (`foo 3.14') */
	sda_dd,		/* two decimal arguments (`foo 3 4') */
	sda_ff,		/* two floating arguments (`foo .3 .4') */
	sda_ddddff,	/* four decimal args, then 2 floating args */
	sda_nd,		/* array n of decimal arguments */
	sda_nx,		/* array n of hex arguments (`x 1a00 d02f ...') */
	sda_rest	/* fn(len1, ptr, len2); uses up rest of input */
};

struct sdecode {
	char	*sd_name;	/* keyword to match */
	enum sd_args sd_args;	/* arguments to collect */
	void	(*sd_fn)();	/* actual arguments depend on sd_args */
};

void	SDecode(/* FILE *, i32, struct sdecode *, size_t */);
void	SDsetclass(/* char *spaces, char *semis */);

#define	SDsize(tab) ((int)(sizeof(tab) / sizeof(*tab)))
