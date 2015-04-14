/* alephbis.c: C routines to support external OCPs
 * based on omegabis.c from the Omega project

This file is part of the Aleph project

Copyright (C) 1994--2001, 2014 John Plaice and Yannis Haralambous
Copyright (C) 2002 Behdad Esfahbod
Copyright (C) 2002, 2005, 2006 Roozbeh Pournader
Copyright (C) 2004 the Aleph team

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#define EXTERN extern
#include "alephd.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#undef read

#if 1

void
runexternalocp (string external_ocp_name)
{
  char *in_file_name;
  char *out_file_name;
  FILE *in_file;
  FILE *out_file;
  char command_line[400];
  int i;
  unsigned c;
  int c_in;
#ifdef WIN32
  const char *tempenv;

#define null_string(s) ((s == NULL) || (*s == '\0'))

  tempenv = getenv("TMPDIR");
  if (null_string(tempenv))
    tempenv = getenv("TEMP");
  if (null_string(tempenv))
    tempenv = getenv("TMP");
  if (null_string(tempenv))
    tempenv = "c:/tmp";	/* "/tmp" is not good if we are on a CD-ROM */
  in_file_name = concat(tempenv, "/__aleph__in__XXXXXX");
  mktemp(in_file_name);
  in_file = fopen(in_file_name, FOPEN_WBIN_MODE);
#else
#if HAVE_MKSTEMP
  int in_file_fd;
  int out_file_fd;

  in_file_name = xstrdup("/tmp/__aleph__in__XXXXXX");
  in_file_fd = mkstemp(in_file_name);
  in_file = fdopen(in_file_fd, FOPEN_WBIN_MODE);
#else
#if HAVE_MKTEMP
  in_file_name = xstrdup("/tmp/__aleph__in__XXXXXX");
  mktemp(in_file_name);
#else
  in_file_name = xstrdup(tmpnam(NULL));
#endif /* HAVE_MKTEMP */
  in_file = fopen(in_file_name, FOPEN_WBIN_MODE);
#endif /* HAVE_MKSTEMP */

#endif /* WIN32 */

  if (in_file == NULL)
    fprintf(stderr, "aleph: error opening file: %s\n", strerror(errno));
  
  for (i=1; i<=otpinputend; i++) {
      c = otpinputbuf[i];
      if (c>0xffff) {
          fprintf(stderr, "aleph: 31-bit chars not supported, goodbye.\n");
          exit(1);
      }
      if (c<0x80) {
          fputc(c & 0x7f, in_file);
      } else if (c<0x800) {
          fputc(0xc0 | ((c>>6) & 0x1f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c<0x10000) {
          fputc(0xe0 | ((c>>12) & 0xf), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c<0x200000) {
          fputc(0xf0 | ((c>>18) & 0x7), in_file);
          fputc(0x80 | ((c>>12) & 0x3f), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c<0x4000000) {
          fputc(0xf8 | ((c>>24) & 0x3), in_file);
          fputc(0x80 | ((c>>18) & 0x3f), in_file);
          fputc(0x80 | ((c>>12) & 0x3f), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else { /* c>=0x4000000 */
          fputc(0xfc | ((c>>30) & 0x1), in_file);
          fputc(0x80 | ((c>>24) & 0x3f), in_file);
          fputc(0x80 | ((c>>18) & 0x3f), in_file);
          fputc(0x80 | ((c>>12) & 0x3f), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      }
  }
  fclose(in_file);
  
#define advance_cin do { if ((c_in = fgetc(out_file)) == -1) { \
                         fprintf(stderr, "File contains bad char\n"); \
                         goto end_of_while; \
                    } } while (0)
                     
#ifdef WIN32
  out_file_name = concat(tempenv, "/__aleph__out__XXXXXX");
  mktemp(out_file_name);
  out_file = fopen(out_file_name, FOPEN_RBIN_MODE);
#else

#if HAVE_MKSTEMP
  out_file_name = xstrdup("/tmp/__aleph__out__XXXXXX");
  out_file_fd = mkstemp(out_file_name);
  out_file = fdopen(out_file_fd, FOPEN_RBIN_MODE);
#else
#if HAVE_MKTEMP
  out_file_name = xstrdup("/tmp/__aleph__out__XXXXXX");
  mktemp(out_file_name);
#else
  out_file_name = xstrdup(tmpnam(NULL));
#endif /* HAVE_MKTEMP */
  out_file = fopen(out_file_name, FOPEN_RBIN_MODE);
#endif /* HAVE_MKSTEMP */

#endif /* WIN32 */
 
  if (out_file == NULL)
    fprintf(stderr, "aleph: error opening file: %s\n", strerror(errno));
  
  if (strlen(external_ocp_name+1) + strlen(in_file_name)
      + strlen(out_file_name) + 15 > sizeof(command_line)) { /* random 15 */
    fprintf(stderr, "aleph: command line would be too long (%d): %s %s %s\n",
            (int) sizeof(command_line), 
            external_ocp_name+1, in_file_name, out_file_name);
    exit(1);
  }
  
  if (strchr(external_ocp_name+1, '\'')) {
    fprintf(stderr, "aleph: single quote not allowed in ocp name: %s\n",
            external_ocp_name+1);
    exit(1);
  }
  if (strchr(in_file_name, '\'')) {
    fprintf(stderr, "aleph: single quote not allowed in in file name: %s\n",
            in_file_name);
    exit(1);
  }
  if (strchr(out_file_name, '\'')) {
    fprintf(stderr, "aleph: single quote not allowed in out file name: %s\n",
            out_file_name);
    exit(1);
  }

  /* ok, we've done some basic safety checks. */
  sprintf(command_line, "'%s' <'%s' >'%s'\n",
          external_ocp_name+1, in_file_name, out_file_name);
  system(command_line);
  otpoutputend = 0;
  otpoutputbuf[otpoutputend] = 0;
  while ((c_in = fgetc(out_file)) != -1) {
     if (c_in<0x80) {
         c = c_in & 0x7f;
     } else if (c_in<0xe0) {
         c = (c_in & 0x1f) << 6;
         advance_cin;
         c |= c_in & 0x3f;
     } else if (c_in<=0xf0) {
         c = (c_in & 0xf) << 12;
         advance_cin;
         c |= (c_in & 0x3f) << 6;
         advance_cin;
         c |= c_in & 0x3f;
     } else if (c_in<0xf8) {
         c = (c_in & 0x7) << 18;
         advance_cin;
         c |= (c_in & 0x3f) << 12;
         advance_cin;
         c |= (c_in & 0x3f) << 6;
         advance_cin;
         c |= c_in & 0x3f;
     } else if (c_in<0xfc) {
         c = (c_in & 0x3) << 24;
         advance_cin;
         c |= (c_in & 0x3f) << 18;
         advance_cin;
         c |= (c_in & 0x3f) << 12;
         advance_cin;
         c |= (c_in & 0x3f) << 6;
         advance_cin;
         c |= c_in & 0x3f;
     } else { /* c>=0xfc */
         c = (c_in & 0x1)   << 30;
         advance_cin;
         c |= (c_in & 0x3f) << 24;
         advance_cin;
         c |= (c_in & 0x3f) << 18;
         advance_cin;
         c |= (c_in & 0x3f) << 12;
         advance_cin;
         c |= (c_in & 0x3f) << 6;
         advance_cin;
         c |= c_in & 0x3f;
     }
     otpoutputbuf[++otpoutputend] = c;
  }

end_of_while:
  fclose(out_file);
  remove(in_file_name);
  remove(out_file_name);
  free(in_file_name);
  free(out_file_name);
}

#else /* 0 */

void
runexternalocp (string external_ocp_name)
{
  int outpipes[2], inpipes[2];
  char *outbuf;
  char *inbuf;
  int n;
  int chars_read_in, chars_to_go_out;
  int myerrno;

#ifdef WIN32
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  HANDLE hIn, hOut, hPipeIn, hPipeOut;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  DWORD ret = 0;

  /* Make pipes to send data from the parent to the child.  The parent
     writes to outpipes[0], and the child reads from outpipes[1].  */
  _pipe (outpipes, 0, _O_BINARY);
  /* Make pipes to send data from the child to the parent.  The child
     writes to inpipes[0], and the parent reads from inpipes[1].  */
  _pipe (inpipes, 0, _O_BINARY);

  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOW;
  si.hStdInput = _get_osfhandle(outpipes[0]);
  si.hStdOutput = _get_osfhandle(inpipes[1]);
  si.hStdError = _get_osfhandle(_fileno(stderr));

  /* Close unnecessary pipes.  */
  close (outpipes[1]);
  close (inpipes[0]);

  if (CreateProcess(external_ocp_name+1,
		    NULL, /* Use lpApplicationName */
		    NULL,
		    NULL,
		    TRUE, /* bInheritHandles */
		    0,
		    NULL,
		    NULL,
		    &si,
		    &pi) == 0) {
    fprintf(stderr, "Failed to create process for %s (Error %d).\n", external_ocp_name+1, GetLastError());
    return;
  }
  
#else /* ! WIN32 */

  /* Make pipes to send data from the parent to the child.  The parent
     writes to outpipes[0], and the child reads from outpipes[1].  */
  pipe (outpipes);
  /* Make pipes to send data from the child to the parent.  The child
     writes to inpipes[0], and the parent reads from inpipes[1].  */
  pipe (inpipes);

  /* For a child process.  */
  if (fork () == 0)
    {
      /* This part is executed by the child process.  It translates
         lower case letters to upper case.  */

      char *prog = external_ocp_name+1;
      char *args[] = {external_ocp_name+1, NULL};

      /* Close unnecessary pipes.  They are for the parent.  */
      close (outpipes[1]);
      close (inpipes[0]);

      /* Connect pipes to stdin and stdout.  */
      dup2 (outpipes[0], 0);
      dup2 (inpipes[1], 1);

      /* Overlays a new process image on an old process. */
      execv (prog, args);

      /* We should never reach here. */
    }
  else
    {
      /* Close unnecessary pipes.  They are for the child.  */
      close (outpipes[0]);
      close (inpipes[1]);

#endif /* WIN32 */

/* Here is the interesting part */
      outbuf = ((char *) otpinputbuf)+2;
      inbuf = ((char *) otpoutputbuf)+2;
      chars_to_go_out = 2*otpinputend;
      chars_read_in = 0;
      while ((n = write (outpipes[1], outbuf, chars_to_go_out))>0) {
fprintf(stderr, "Wrote (1) %d characters\n", n);
        outbuf+=n;
        chars_to_go_out-=n;
        if (chars_to_go_out==0) goto done_writing;
/*
        n = read (inpipes[0], inbuf, 1024);
fprintf(stderr, "Read (1) %d characters\n", n);
        inbuf+=n;
        chars_read_in+=n;
*/
      }
fprintf(stderr, "Wrote (2) %d characters\n", n);

done_writing:
      close (outpipes[1]);
      while ((n = read (inpipes[0], inbuf, 1024)) > 0) {
fprintf(stderr, "Read (2) %d characters\n", n);
        inbuf+=n;
        chars_read_in+=n;
      }
fprintf(stderr, "Read (3) %d characters\n", n);
      otpoutputend = chars_read_in / 2;

      close (inpipes[0]);
#ifndef WIN32
    }
#endif
}

#endif /* 0 */
