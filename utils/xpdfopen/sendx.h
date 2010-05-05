/*
 * sendx.h: declare prototypes for functions defined in sendx.c
 *
 * Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#ifndef SENDX_H
#define SENDX_H

extern int sendx_string(const char * string, const char * wname);
extern int sendx_token(const char * string, const char * wname);
extern int sendx_alt_token(const char * string, const char * wname);
extern int sendx_controlalt_token(const char * string, const char * wname);
extern int sendx_control_token(const char * string, const char * wname);
extern int set_focus(const char * wname);
extern int reset_focus(void);

#endif
