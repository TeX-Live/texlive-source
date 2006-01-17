/* 
   winhtex.h : prototype file
   Time-stamp: "99/07/22 20:24:32 popineau"
   
   Copyright (C) 1999
      Fabrice Popineau <Fabrice.Popineau@supelec.fr>

   This file is part of Windvi.

   Windvi is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   Windvi is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Windvi; if not, write to the Free Software Foundation, 59
   Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _WINHTEX_H_
#define _WINHTEX_H_

#define NO  0
#define YES 1

#define PARSE_ALL 1

extern BOOL HTURL_isAbsolute(const char *url);
extern int invokviewer(char *filename);
extern char * HTParse (const char *aName, const char *relatedName, int wanted);
extern void htex_cleanup(int arg);
extern int www_fetch(char *url, char *savefile);
#endif

