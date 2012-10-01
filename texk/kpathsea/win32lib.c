/* win32lib.c: bits and pieces for win32 and msvc.

   Copyright 1996, 1997. 1998, 1999 Fabrice Popineau.
   Copyright 2006, 2011, 2012 Akira Kakuto.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/concatn.h>
#include <kpathsea/variable.h>

FILE * __cdecl kpathsea_win32_popen (kpathsea kpse, const char *cmd, const char *fmode)
{
  char mode[3];
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  FILE *f = NULL;
  int fno, i;
  HANDLE child_in, child_out;
  HANDLE father_in, father_out;
  HANDLE father_in_dup, father_out_dup;
  HANDLE current_in, current_out;
  HANDLE current_pid;
  char *env_path;
  int binary_mode;
  char *new_cmd, *app_name = NULL;
  char *p, *q;
  struct _popen_elt *new_process;
  char pname[MAXPATHLEN], *fp;
  char *suffixes[] = { ".bat", ".cmd", ".com", ".exe", NULL };
  char **s;
  BOOL go_on;

  /* We always use binary mode */

  mode[0] = fmode[0];
  mode[1] = 'b';
  mode[2] = '\0';

  /* We should look for the application name along the PATH,
     and decide to prepend "%COMSPEC% /c " or not to the command line.
     Do nothing for the moment. */

  /* Another way to do that would be to try CreateProcess first without
     invoking cmd, and look at the error code. If it fails because of
     command not found, try to prepend "cmd /c" to the cmd line.
     */
  /* Look for the application name */
  for (p = (char *)cmd; *p && isspace(*p); p++);
  if (*p == '"') {
    q = ++p;
    while(*p && *p != '"') p++;
    if (*p == '\0') {
      fprintf(stderr, "popen: malformed command (\" not terminated)\n");
      return NULL;
    }
  }
  else
    for (q = p; *p && !isspace(*p); p++);
  /* q points to the beginning of appname, p to the last + 1 char */
  if ((app_name = malloc(p - q + 1)) == NULL) {
    fprintf(stderr, "xpopen: malloc(app_name) failed.\n");
    return NULL;
  }
  strncpy(app_name, q, p - q );
  app_name[p - q] = '\0';
  pname[0] = '\0';
#ifdef TRACE
  fprintf(stderr, "popen: app_name = %s\n", app_name);
#endif

  {
    char *tmp = getenv("PATH");
    env_path = xmalloc(strlen(tmp) + 3);
    strcpy(env_path, tmp);
    strcat(env_path, ";.");
  }

  /* Looking for appname on the path */
  for (s = suffixes, go_on = TRUE; go_on; *s++) {
    if (SearchPath(env_path,    /* Address of search path */
                   app_name,    /* Address of filename */
                   *s,          /* Address of extension */
                   MAXPATHLEN,  /* Size of destination buffer */
                   pname,       /* Address of destination buffer */
                   &fp)         /* File part of app_name */
      != 0 && *s) { /* added *s in order not to allow no suffix.
                         --ak 2009/10/24 */
#ifdef TRACE
      fprintf(stderr, "%s found with suffix %s\nin %s\n", app_name, *s, pname);
#endif
      new_cmd = xstrdup(cmd);
      free(app_name);
      app_name = xstrdup(pname);
      break;
    }
    go_on = (*s != NULL);
  }
  if (go_on == FALSE) {
    /* the app_name was not found */
#ifdef TRACE
    fprintf(stderr, "%s not found, concatenating comspec\n", app_name);
#endif
    new_cmd = concatn("cmd.exe", " /c ", cmd, NULL);
    free(app_name);
    app_name = NULL;
  }
  if (env_path) free(env_path);

#ifdef TRACE
  fprintf(stderr, "popen: app_name = %s\n", app_name);
  fprintf(stderr, "popen: cmd_line = %s\n", new_cmd);
#endif
  current_in = GetStdHandle(STD_INPUT_HANDLE);
  current_out = GetStdHandle(STD_OUTPUT_HANDLE);
  current_pid = GetCurrentProcess();
  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;

  if (strchr(mode, 'b'))
    binary_mode = _O_BINARY;
  else
    binary_mode = _O_TEXT;

  /* Opening the pipe for writing */
  if (strchr(mode, 'w')) {
    binary_mode |= _O_WRONLY;
    if (CreatePipe(&child_in, &father_out, &sa, 0) == FALSE) {
      fprintf(stderr, "popen: error CreatePipe\n");
      return NULL;
    }

    si.hStdInput = child_in;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    if (DuplicateHandle(current_pid, father_out,
                        current_pid, &father_out_dup,
                        0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) {
      fprintf(stderr, "popen: error DuplicateHandle father_out\n");
      return NULL;
    }
    CloseHandle(father_out);
    fno = _open_osfhandle((long)father_out_dup, binary_mode);
    f = _fdopen(fno, mode);
    i = setvbuf( f, NULL, _IONBF, 0 );
  }
  /* Opening the pipe for reading */
  else if (strchr(mode, 'r')) {
    binary_mode |= _O_RDONLY;
    if (CreatePipe(&father_in, &child_out, &sa, 0) == FALSE) {
      fprintf(stderr, "popen: error CreatePipe\n");
      return NULL;
    }

    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = child_out;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (DuplicateHandle(current_pid, father_in,
                        current_pid, &father_in_dup,
                        0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) {
      fprintf(stderr, "popen: error DuplicateHandle father_in\n");
      return NULL;
    }
    CloseHandle(father_in);
    fno = _open_osfhandle((long)father_in_dup, binary_mode);
    f = _fdopen(fno, mode);
    i = setvbuf( f, NULL, _IONBF, 0 );
  }
  else {
    fprintf(stderr, "popen: invalid mode %s\n", mode);
    return NULL;
  }

  /* creating child process */
  if (CreateProcess(app_name,   /* pointer to name of executable module */
                    new_cmd,    /* pointer to command line string */
                    NULL,       /* pointer to process security attributes */
                    NULL,       /* pointer to thread security attributes */
                    TRUE,       /* handle inheritance flag */
                    0,          /* creation flags, do not touch this again ! (16/06/98) */
                    NULL,       /* pointer to environment */
                    NULL,       /* pointer to current directory */
                    &si,        /* pointer to STARTUPINFO */
                    &pi         /* pointer to PROCESS_INFORMATION */
                  ) == FALSE) {
    fprintf(stderr, "popen: CreateProcess %x\n", GetLastError());
    return NULL;
  }

   /* Only the process handle is needed */
  if (CloseHandle(pi.hThread) == FALSE) {
    fprintf(stderr, "popen: error closing thread handle\n");
    return NULL;
  }

  if (new_cmd) free(new_cmd);
  if (app_name) free(app_name);

  /* Add the pair (f, pi.hProcess) to the list */
  if ((new_process = malloc(sizeof(struct _popen_elt))) == NULL) {
    fprintf (stderr, "popen: malloc(new_process) error\n");
    return NULL;
  }
  /* Saving the FILE * pointer, access key for retrieving the process
     handle later on */
  new_process->f = f;
  /* Closing the unnecessary part of the pipe */
  if (strchr(mode, 'r')) {
    CloseHandle(child_out);
  }
  else if (strchr(mode, 'w')) {
    CloseHandle(child_in);
  }
  /* Saving the process handle */
  new_process->hp = pi.hProcess;
  /* Linking it to the list of popen() processes */
  new_process->next = kpse->_popen_list;
  kpse->_popen_list = new_process;

  return f;

}

int __cdecl kpathsea_win32_pclose (kpathsea kpse, FILE *f)
{
  struct _popen_elt *p, *q;
  int exit_code;

  /* Look for f is the access key in the linked list */
  for (q = NULL, p = kpse->_popen_list;
       p != &kpse->_z_p_open && p->f != f;
       q = p, p = p->next);

  if (p == &kpse->_z_p_open) {
    fprintf(stderr, "pclose: error, file not found.");
    return -1;
  }

  /* Closing the FILE pointer */
  fclose(f);

  /* Waiting for the process to terminate */
  if (WaitForSingleObject(p->hp, INFINITE) != WAIT_OBJECT_0) {
    fprintf(stderr, "pclose: error, process still active\n");
    return -1;
  }

  /* retrieving the exit code */
  if (GetExitCodeProcess(p->hp, &exit_code) == 0) {
    fprintf(stderr, "pclose: can't get process exit code\n");
    return -1;
  }

  /* Closing the process handle, this will cause the system to
     remove the process from memory */
  if (CloseHandle(p->hp) == FALSE) {
    fprintf(stderr, "pclose: error closing process handle\n");
    return -1;
  }

  /* remove the elt from the list */
  if (q != NULL)
    q->next = p->next;
  else
    kpse->_popen_list = p->next;
  free(p);

  return exit_code;
}

/* large file support */

__int64
xftell64 (FILE *f, const char *filename)
{
  __int64 where, filepos;
  int fd;

  fd = fileno(f);
  if(f->_cnt < 0)
    f->_cnt = 0;
  if((filepos = _lseeki64(fd, (__int64)0, SEEK_CUR)) < (__int64)0) {
    FATAL_PERROR(filename);
    return (__int64)(-1);
  }
  if(filepos == (__int64)0)
    where = (__int64)(f->_ptr - f->_base);
  else
    where = filepos - f->_cnt;
  return where;
}

void
xfseek64 (FILE *f, __int64 offset, int wherefrom,  const char *filename)
{
  if(wherefrom == SEEK_CUR) {
    offset += xftell64(f, filename);
    wherefrom = SEEK_SET;
  }
  fflush(f);
  if (_lseeki64(fileno(f), offset, wherefrom) < (__int64)0)
    FATAL_PERROR(filename);
}


/* special TeXLive Ghostscript */

static int is_dir (char *buff)
{
  HANDLE h;
  WIN32_FIND_DATA w32fd;

  if (((h = FindFirstFile (buff, &w32fd))
       != INVALID_HANDLE_VALUE) &&
      (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
    FindClose (h);
    return (1);
  } else {
    FindClose (h);
    return (0);
  }
}

/*
   TeXlive uses its own gs in
   $SELFAUTOPARENT/tlpkg/tlgs
*/
void texlive_gs_init(void)
{
  char *nptr, *path;
  char tlgsbindir[512];
  char tlgslibdir[512];
  nptr = kpse_var_value("SELFAUTOPARENT");
  if (nptr) {
    strcpy(tlgsbindir, nptr);
    strcat(tlgsbindir,"/tlpkg/tlgs");
    if(is_dir(tlgsbindir)) {
      strcpy(tlgslibdir, tlgsbindir);
      strcat(tlgslibdir, "/lib;");
      strcat(tlgslibdir, tlgsbindir);
      strcat(tlgslibdir, "/fonts");
      strcat(tlgsbindir, "/bin;");
      free(nptr);
      for(nptr = tlgsbindir; *nptr; nptr++) {
        if(*nptr == '/') *nptr = '\\';
      }
      nptr = getenv("PATH");
      path = (char *)malloc(strlen(nptr) + strlen(tlgsbindir) + 6);
      strcpy(path, tlgsbindir);
      strcat(path, nptr);
      xputenv("PATH", path);
      xputenv("GS_LIB", tlgslibdir);
    }
  }
}

/*
 path name in char *input is changed into the long
 name format and returned in char *buff.
 return value: 0 if failed
               1 if succeeded
*/

int getlongpath(char *buff, char *input, int len)
{
   HANDLE hnd;
   WIN32_FIND_DATA ffd;
   int  cnt = 0;
   char *p, *q, *r;

   buff[0] = '\0';
/*
temporarily change directory separators into back slashs
*/
   for(p = input; *p; p++) {
      if(*p == '/')
         *p = '\\';
   }

   p = q = input;
   r = buff;

/*
UNC name
*/
   if(q[0] == '\\' && q[1] == '\\') {
      cnt += 2;
      if(cnt > len) return 0;
      buff[0] = '/';
      buff[1] = '/';
      p += 2;
      r += 2;
      while(*p != '\\' && *p) {
         if (IS_KANJI(p)) {
            cnt++;
            if(cnt > len) return 0;
            *r++ = *p++;
         }
         cnt++;
         if(cnt > len) return 0;
         *r++ = *p++;
      }
      cnt++;
      if(cnt > len) return 0;
      *r++ = '/';
      if(*p) p++;
      while(*p != '\\' && *p) {
         if (IS_KANJI(p)) {
            cnt++;
            if(cnt > len) return 0;
            *r++ = *p++;
         }
         cnt++;
         if(cnt > len) return 0;
         *r++ = *p++;
      }
      cnt++;
      if(cnt > len) return 0;
      *r++ = '/';
      *r= '\0';
      if(*p) p++;
/*
drive name
*/
   } else if(isalpha(q[0]) && q[1] == ':' && q[2] == '\\') {
      *r++ = q[0];
      *r++ = ':';
      *r++ = '/';
      *r = '\0';
      p += 3;
      cnt += 3;
      if(cnt > len) return 0;
   }

   for( ; *p; p++) {
      if(IS_KANJI(p)) {
         p++;
         continue;
      }
      if(*p == '\\') {
         *p = '\0';
         if((hnd = FindFirstFile(q, &ffd)) == INVALID_HANDLE_VALUE) {
            return 0;
         }
         FindClose(hnd);
         cnt += strlen(ffd.cFileName);
         cnt++;
         if(cnt > len) return 0;
         strcat(buff, ffd.cFileName);
         strcat(buff, "/");
         *p = '\\';
      }
   }

/*
file itself
*/
   if((hnd = FindFirstFile(q, &ffd)) == INVALID_HANDLE_VALUE) {
      return 0;
   }
   FindClose(hnd);
   cnt += strlen(ffd.cFileName);
   if(cnt > len) return 0;
   strcat(buff, ffd.cFileName);
   return 1;
}

/* user info */

/* Adapted for XEmacs by David Hobley <david@spook-le0.cia.com.au> */
/* Sync'ed with Emacs 19.34.6 by Marc Paquette <marcpa@cam.org> */
/* Adapted to fpTeX 0.4 by Fabrice Popineau <Fabrice.Popineau@supelec.fr> */

char * get_home_directory()
{
  char *p;
  char *home = getenv("HOME");
  if(!home)
    home = getenv("USERPROFILE");
  if(home) {
    home = xstrdup(home);
    for(p = home; *p; p++) {
      if(IS_KANJI(p)) {
        p++;
        continue;
      }
      if(*p == '\\')
        *p = '/';
    }
  }
  return home;
}

int
kpathsea_getuid (kpathsea kpse)
{
  return kpse->the_passwd.pw_uid;
}

int
kpathsea_getgid (kpathsea kpse)
{
  return kpse->the_passwd.pw_gid;
}

struct passwd *
kpathsea_getpwuid (kpathsea kpse, int uid)
{
  if (uid == kpse->the_passwd.pw_uid)
    return &kpse->the_passwd;
  return NULL;
}

struct passwd *
kpathsea_getpwnam (kpathsea kpse, char *name)
{
  struct passwd *pw;

  pw = kpathsea_getpwuid (kpse, kpathsea_getuid (kpse));
  if (!pw)
    return pw;

  if (stricmp (name, pw->pw_name))
    return NULL;

  return pw;
}

void
kpathsea_init_user_info (kpathsea kpse)
{
   char  *home;
   DWORD nSize = 256;

   if (!GetUserName (kpse->the_passwd.pw_name, &nSize))
      strcpy (kpse->the_passwd.pw_name, "unknown");
   kpse->the_passwd.pw_uid = 123;
   kpse->the_passwd.pw_gid = 123;

   /* Ensure HOME and SHELL are defined. */

   home = get_home_directory();
   if (home) {
      putenv(concat("HOME=", home));
   }
   else {
      putenv ("HOME=c:/");
   }

   if (getenv ("SHELL") == NULL)
      putenv ((GetVersion () & 0x80000000) ? "SHELL=command" : "SHELL=cmd");

   {
   /* If TEXMFTEMP is defined, then use it as the TEMP and TMP variables. */
      char *p;
      if ((p = getenv("TEXMFTEMP")) != NULL) {
         putenv(concat("TEMP=", p));
         putenv(concat("TMP=", p));
      }
   }

   /* Set dir and shell from environment variables. */
   strcpy (kpse->the_passwd.pw_dir, get_home_directory());
   strcpy (kpse->the_passwd.pw_shell, getenv ("SHELL"));
}

/* lookcmd */

BOOL
look_for_cmd(const char *cmd, char **app, char **new)
{
  char *env_path;
  char *p, *q;
  char pname[MAXPATHLEN], *fp;
  char *suffixes[] = { ".bat", ".cmd", ".com", ".exe", NULL };
  char **s;
  char *app_name, *new_cmd;

  BOOL go_on;

  *new = *app = NULL;
  new_cmd = app_name = NULL;

  /* We should look for the application name along the PATH,
     and decide to prepend "%COMSPEC% /c " or not to the command line.
     Do nothing for the moment. */

  /* Another way to do that would be to try CreateProcess first without
     invoking cmd, and look at the error code. If it fails because of
     command not found, try to prepend "cmd /c" to the cmd line.
  */

  /* Look for the application name */
  for (p = (char *)cmd; *p && isspace(*p); p++);
  if (*p == '"') {
    q = ++p;
    while(*p && *p != '"') p++;
    if (*p == '\0') {
      fprintf(stderr, "Look_for_cmd: malformed command (\" not terminated)\n");
      return FALSE;
    }
  }
  else
    for (q = p; *p && !isspace(*p); p++);
  /* q points to the beginning of appname, p to the last + 1 char */
  if ((app_name = malloc(p - q + 1)) == NULL) {
    fprintf(stderr, "Look_for_cmd: malloc(app_name) failed.\n");
    return FALSE;
  }
  strncpy(app_name, q, p - q );
  app_name[p - q] = '\0';
  pname[0] = '\0';
#ifdef TRACE
  fprintf(stderr, "popen: app_name = %s\n", app_name);
#endif

  {
    char *tmp = getenv("PATH");
    env_path = xmalloc(strlen(tmp) + 3);
    strcpy(env_path, tmp);
    strcat(env_path, ";.");
  }

  /* Looking for appname on the path */
  for (s = suffixes, go_on = TRUE; go_on; *s++) {
    if (SearchPath(env_path,    /* Address of search path */
                   app_name,    /* Address of filename */
                   *s,          /* Address of extension */
                   MAXPATHLEN,  /* Size of destination buffer */
                   pname,       /* Address of destination buffer */
                   &fp)         /* File part of app_name */
        != 0 && *s) { /* added *s in order not to allow no suffix.
                         --ak 2009/07/08 */
#ifdef TRACE
      fprintf(stderr, "%s found with suffix %s\nin %s\n", app_name, *s, pname);
#endif
      new_cmd = xstrdup(cmd);
      free(app_name);
      app_name = xstrdup(pname);
      break;
    }
    go_on = (*s != NULL);
  }
  if (go_on == FALSE) {
    /* the app_name was not found */
#ifdef TRACE
    fprintf(stderr, "%s not found, concatenating comspec\n", app_name);
#endif
    new_cmd = concatn("cmd.exe", " /c ", cmd, NULL);
    free(app_name);
    app_name = NULL;
  }
  if (env_path) free(env_path);

  *new = new_cmd;
  *app = app_name;

  return TRUE;

}

/*
  Command parser. Borrowed from DJGPP.
 */

typedef enum {
  EMPTY,
  WORDARG,
  REDIR_INPUT,
  REDIR_APPEND,
  REDIR_OUTPUT,
  PIPE,
  SEMICOLON,
  UNMATCHED_QUOTE,
  EOL
} cmd_sym_t;

/* Return a copy of a word between BEG and (excluding) END with all
   quoting characters removed from it.  */

static char *
kpathsea__unquote (kpathsea kpse, char *to, const char *beg, const char *end)
{
  const char *s = beg;
  char *d = to;
  int quote = 0;

  while (s < end)
    {
      switch (*s)
        {
        case '"':
        case '\'':
          if (!quote)
            quote = *s;
          else if (quote == *s)
            quote = 0;
          s++;
          break;
        case '\\':
          if (s[1] == '"' || s[1] == '\''
              || (s[1] == ';'
                  && (kpse->__system_allow_multiple_cmds)))
            s++;
          /* Fall-through.  */
        default:
          *d++ = *s++;
          break;
        }
    }

  *d = 0;
  return to;
}

/* A poor-man's lexical analyzer for simplified command processing.

   It only knows about these:

     redirection and pipe symbols
     semi-colon `;' (that possibly ends a command)
     argument quoting rules with quotes and `\'
     whitespace delimiters of words (except in quoted args)

   Returns the type of next symbol and pointers to its first and (one
   after) the last characters.

   Only `get_sym' and `unquote' should know about quoting rules.  */

static cmd_sym_t
kpathsea_get_sym (kpathsea kpse, char *s, char **beg, char **end)
{
  int in_a_word = 0;

  while (isspace (*s))
    s++;

  *beg = s;

  do {
    *end = s + 1;

    if (in_a_word
        && (!*s || strchr ("<>| \t\n", *s)
            || ((kpse->__system_allow_multiple_cmds) && *s == ';')))
      {
        --*end;
        return WORDARG;
      }

    switch (*s)
      {
      case '<':
        return REDIR_INPUT;
      case '>':
        if (**end == '>')
          {
            ++*end;
            return REDIR_APPEND;
          }
        return REDIR_OUTPUT;
      case '|':
        return PIPE;
      case ';':
        if (kpse->__system_allow_multiple_cmds)
          return SEMICOLON;
        else
          in_a_word = 1;
        break;
      case '\0':
        --*end;
        return EOL;
      case '\\':
        if (s[1] == '"' || s[1] == '\''
            || (s[1] == ';' && (kpse->__system_allow_multiple_cmds)))
          s++;
        in_a_word = 1;
        break;
      case '\'':
      case '"':
        {
          char quote = *s++;

          while (*s && *s != quote)
            {
              if (*s++ == '\\' && (*s == '"' || *s == '\''))
                s++;
            }
          *end = s;
          if (!*s)
            return UNMATCHED_QUOTE;
          in_a_word = 1;
          break;
        }
      default:
        in_a_word = 1;
        break;
      }

    s++;

  } while (1);
}

/*
  What we allow :
  [cmd] [arg1] ... [argn] < [redinput] | [cmd2] | ... | [cmdn] > [redoutput]
*/
void *kpathsea_parse_cmdline(kpathsea kpse, char *line, char **input,
                             char **output, int *is_cmd_pipe)
{
  BOOL again, needcmd = TRUE, bSuccess = TRUE, append_out = FALSE;
  char *beg = line, *end, *new_end;
  cmd_sym_t token = EMPTY, prev_token = EMPTY;
  int ncmd = 0, narg = 1;
  char **fp;
  char ***cmd;
  char *dummy_input;                    /* So that we could pass NULL */
  char *dummy_output;                   /* instead of a real ??put */

  if (input == NULL) input = &dummy_input;
  if (output == NULL) output = &dummy_output;

  *input = NULL;
  *output = NULL;
  cmd = xmalloc(MAX_PIPES*sizeof(char **));
  cmd[ncmd] = NULL;
#ifdef TRACE
  fprintf(stderr, "line = %s\n", line);
#endif
  do {
    again = FALSE;
    prev_token = token;
    token = kpathsea_get_sym (kpse, beg, &beg, &end);  /* get next symbol */
#ifdef TRACE
    fprintf(stderr, "token = %s\n", beg);
#endif  
    switch (token) {
    case WORDARG:
      if (prev_token == REDIR_INPUT
          || prev_token == REDIR_OUTPUT) {
        fprintf(stderr, "Ambigous input/output redirect.");
        bSuccess = FALSE;
        goto leave;
      }
      /* First word we see is the program to run.  */
      if (needcmd) {
        narg = 1;
        cmd[ncmd] = xmalloc(narg * sizeof(char *));
        cmd[ncmd][narg - 1] = xmalloc(end - beg + 1);
        kpathsea__unquote (kpse, cmd[ncmd][narg - 1], beg, end);
        /* unquote and copy to prog */
        if (cmd[ncmd][narg - 1][0] == '(') {
          fprintf(stderr, "parse_cmdline(%s): Parenthesized groups not allowed.\n", line);
          bSuccess = FALSE;
          goto leave;
        }
        needcmd = FALSE;
      }
      else {
        narg++;
        cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
        cmd[ncmd][narg - 1] = xmalloc(end - beg + 1);
        kpathsea__unquote (kpse, cmd[ncmd][narg - 1], beg, end);
        /* unquote and copy to prog */
      }
      beg = end; /* go forward */
      again = TRUE;
      break;

    case REDIR_INPUT:
    case REDIR_OUTPUT:
    case REDIR_APPEND:
      if (token == REDIR_INPUT) {
        if (*input) {
          fprintf(stderr, "Ambiguous input redirect.");
          errno = EINVAL;
          bSuccess = FALSE;
          goto leave;
        }
        fp = input;
      }
      else if (token == REDIR_OUTPUT || token == REDIR_APPEND) {
        if (*output) {
          fprintf(stderr, "Ambiguous output redirect.");
          errno = EINVAL;
          bSuccess = FALSE;
          goto leave;
        }
        fp = output;
        if (token == REDIR_APPEND)
          append_out = TRUE;
      }
      if (kpathsea_get_sym (kpse, end, &end, &new_end) != WORDARG) {
        fprintf(stderr, "Target of redirect is not a filename.");
        errno = EINVAL;
        bSuccess = FALSE;
        goto leave;
      }
      *fp = (char *)xmalloc (new_end - end + 1);
      memcpy (*fp, end, new_end - end);
      (*fp)[new_end - end] = '\0';
      beg = new_end;
      again = TRUE;
      break;
    case PIPE:
      *is_cmd_pipe = 1; /* --ak 2009/07/08 */
      if (*output) {
        fprintf(stderr, "Ambiguous output redirect.");
        errno = EINVAL;
        bSuccess = FALSE;
        goto leave;
      }
      narg++;
      cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
      cmd[ncmd][narg - 1] = NULL;
      ncmd++;
      needcmd = TRUE;
      beg = end;
      again = TRUE;
      break;
    case SEMICOLON:
    case EOL:
      if (needcmd) {
        fprintf(stderr, "No command name seen.");
        errno = EINVAL;
        bSuccess = FALSE;
        goto leave;
      }
      narg++;
      cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
      cmd[ncmd][narg - 1] = NULL;
      ncmd++;
      cmd[ncmd] = NULL;
      again = FALSE;
      break;
        
    case UNMATCHED_QUOTE:
      fprintf(stderr, "Unmatched quote character.");
      errno = EINVAL;
      bSuccess = FALSE;
      goto leave;
    default:
      fprintf(stderr, "I cannot grok this.");
      errno = EINVAL;
      bSuccess = FALSE;
      goto leave;

    }

  }     while (again);

 leave:
  if (!bSuccess) {
    int i;
    char **p;
    /* Need to free everything that was allocated */
    for (i = 0; i < ncmd; i++) {
      for (p = cmd[i]; *p; p++)
        free(*p);
      free(cmd[i]);
    }
    if (cmd[ncmd]) {
      for (i = 0; i < narg; i++)
        free(cmd[ncmd][i]);
      free(cmd[ncmd]);
    }
    free(cmd);
    *cmd = NULL;
  }
  return cmd;
}

static int need_qq(char c)
{
  int ret;
  ret = ((c=='&') ||
         (c=='(') ||
         (c==')') ||
         (c=='[') ||
         (c==']') ||
         (c=='{') ||
         (c=='}') ||
         (c=='^') ||
         (c==';') ||
         (c=='!') ||
         (c=='\'') ||
         (c=='+') ||
         (c=='`') ||
         (c=='~') ||
         (c=='%') ||
         (c=='@'));
  return ret;
}

static char *
quote_elt(char *elt)
{
  char *p;
  int qok;

  if(!strcmp(elt,"|") ||
     !strcmp(elt,"||") ||
     !strcmp(elt,"&") ||
     !strcmp(elt,"&&") ||
     !strcmp(elt,";")) {
    qok = 0;
  } else {
    qok = 1;
  }
  for (p = elt; *p; p++)
    if (isspace(*p) || (need_qq(*p) && qok))
      return concat3("\"", elt, "\"");

  return xstrdup(elt);
}

static char *
quote_args(char **argv)
{
  int i;
  char *line = NULL, *new_line;
  char *new_argv;

  if (!argv)
    return line;

  line = quote_elt(argv[0]);
  for (i = 1; argv[i]; i++) {
    new_argv = quote_elt(argv[i]);
    new_line = concat3(line, " ", new_argv);
    free(line);
    free(new_argv);
    line = new_line;
  }

  return line;
}

char *
build_cmdline(char ***cmd, char *input, char *output)
{
  int ncmd;
  char *line = NULL, *new_line;

  if (!cmd)
    return line;

  line = quote_args(cmd[0]);
  if (input) {
    new_line = concat3(line, "<", quote_elt(input));
    free(line);
    line = new_line;
  }
  for(ncmd = 1; cmd[ncmd]; ncmd++) {
    new_line = concat3(line, " | ", quote_args(cmd[ncmd]));
    free(line);
    line = new_line;
  }

  if (output) {
    new_line = concat3(line, ">", quote_elt(output));
    free(line);
    line = new_line;
  }

  return line;
}

/* win32_system */
static int is_include_space(const char *s)
{
  char *p;
  p = strchr(s, ' ');
  if(p) return 1;
  p = strchr(s, '\t');
  if(p) return 1;
  return 0;
}

int __cdecl win32_system(const char *cmd)
{
  const char *p;
  char  *q;
  char  *av[4];
  int   len, ret;
  int   spacep = 0;

  if(cmd == NULL)
    return 1;

  av[0] = xstrdup("cmd.exe");
  av[1] = xstrdup("/c");

  len = strlen(cmd) + 3;
  spacep = is_include_space(cmd);
  av[2] = malloc(len);
  q = av[2];
  if(spacep)
    *q++ = '"';
  for(p = cmd; *p; p++, q++) {
    if(*p == '\'')
      *q = '"';
    else
      *q = *p;
  }
  if(spacep)
    *q++ = '"';
  *q = '\0';
  av[3] = NULL;
  ret = spawnvp(_P_WAIT, av[0], av);
  free(av[0]);
  free(av[1]);
  free(av[2]);
  return ret;
}

#if defined (KPSE_COMPAT_API)
FILE * __cdecl win32_popen (const char *cmd, const char *mode)
{
   return kpathsea_win32_popen(kpse_def, cmd, mode);
}

int __cdecl win32_pclose (FILE *f)
{
   return kpathsea_win32_pclose(kpse_def, f);
}

int 
getuid (void) 
{ 
  return kpathsea_getuid(kpse_def);
}

int 
geteuid (void) 
{ 
  /* I could imagine arguing for checking to see whether the user is
     in the Administrators group and returning a UID of 0 for that
     case, but I don't know how wise that would be in the long run.  */
  return getuid (); 
}

int
getgid (void) 
{ 
  return kpathsea_getgid (kpse_def);
}

int 
getegid (void) 
{ 
  return getgid ();
}

struct passwd *
getpwuid (int uid)
{
  return kpathsea_getpwuid (kpse_def, uid);
}

struct passwd *
getpwnam (char *name)
{
  return kpathsea_getpwnam (kpse_def, name);
}

#endif /* KPSE_COMPAT_API */
