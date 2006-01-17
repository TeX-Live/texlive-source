/* pserror.h
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * Header file for external functions in pserror.c
 */

#include <stdarg.h>

/* message flags */
#define MESSAGE_NL	1	/* Newline before message if necessary */
#define MESSAGE_PROGRAM	2	/* announce program name */
#define MESSAGE_EXIT	4	/* do not return */

/* message types */
#define FATAL		(MESSAGE_EXIT|MESSAGE_PROGRAM|MESSAGE_NL)
#define WARN		(MESSAGE_NL|MESSAGE_PROGRAM)
#define LOG		0

extern void message(int flags, char *format, ...) ;
