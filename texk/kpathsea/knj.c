/* knj.c: check for 2-Byte Kanji (CP 932, SJIS) codes.

   Copyright 2010, 2016, 2018 Akira Kakuto.
   Copyright 2013, 2016 TANAKA Takuji.

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
#include <kpathsea/debug.h>
#include <wchar.h>

static int
is_include_space(const char *s)
{
    char *p;
    p = strchr(s, ' ');
    if(p) return 1;
    p = strchr(s, '\t');
    if(p) return 1;
    return 0;
}

/*
  Get wide string from multibyte string.
*/
wchar_t *
get_wstring_from_mbstring(int cp, const char *mbstr, wchar_t *wstr)
{
  int len;

  len = MultiByteToWideChar(cp, 0, mbstr, -1, wstr, 0);
  if (len==0) {
    FATAL("cannot convert string to wide string");
  }
  if (wstr==NULL) {
    wstr = xmalloc(sizeof(wchar_t)*(len+1));
  }
  len = MultiByteToWideChar(cp, 0, mbstr, -1, wstr, len+1);
  if (len==0) {
    FATAL("cannot convert multibyte string to wide string");
  }
  return wstr;
}

/*
  Get multibyte string from wide string.
*/
char *
get_mbstring_from_wstring(int cp, const wchar_t *wstr, char *mbstr)
{
  int len;

  len = WideCharToMultiByte(cp, 0, wstr, -1, mbstr, 0, NULL, NULL);
  if (len==0) {
    FATAL("cannot convert string to multibyte string");
  }
  if (mbstr==NULL) {
    mbstr = xmalloc(len+1);
  }
  len = WideCharToMultiByte(cp, 0, wstr, -1, mbstr, len+1, NULL, NULL);
  if (len==0) {
    FATAL("cannot convert wide string to multibyte string");
  }
  return mbstr;
}

int
kpathsea_isknj(kpathsea kpse, int c)
{
  c &= 0xff;
  switch (kpse->Is_cp932_system) {
  case 932:
    return((c>=0x81 && c<=0x9f) || (c>=0xe0 && c<=0xfc));
  case 936:
    return(c>=0x81 && c<=0xfe);
  case 950:
    return((c>=0xa1 && c<=0xc6) || (c>=0xc9 && c<=0xf9));
  default:
    return(0);
  }
}

int
kpathsea_isknj2(kpathsea kpse, int c)
{
  c &= 0xff;
  switch (kpse->Is_cp932_system) {
  case 932:
    return(c>=0x40 && c<=0xfc && c!=0x7f);
  case 936:
    return(c>=0x40 && c<=0xfe && c!=0x7f);
  case 950:
    return((c>=0x40 && c<=0x7e) || (c>=0xa1 && c<=0xfe));
  default:
    return(0);
  }
}

/*
  xfopen by file system codepage
*/
FILE *
kpathsea_fsyscp_xfopen (kpathsea kpse, const char *filename, const char *mode)
{
    FILE *f;
    wchar_t *fnamew, modew[4];
    int i;
    unsigned char *fnn;
    unsigned char *p;
    size_t len;

    assert(filename && mode);
    len = strlen(filename);
/*
  Support very long input path name, longer than _MAX_PATH for
  Windows, if it really exists and input name is given in
  full-absolute path in a command line.
  /./ , /../, \.\, \..\ should be excluded. (2020/06/06)
*/
    fnn = xmalloc(len + 10);
    p = strstr(filename, ".\\");
    if (!p) {
       p = strstr(filename, "./");
    }
    if (!p && len > 2 && ((filename[0] == '/' && filename[1] == '/') ||
        (filename[0] == '\\' && filename[1] == '\\' &&
         filename[2] != '?'))) {
       filename += 2;
       strcpy (fnn, "\\\\?\\UNC\\");
       strcat (fnn, filename);
    } else if (!p && len > 2 && filename[1] == ':') {
       strcpy (fnn, "\\\\?\\");
       strcat (fnn, filename);
    } else {
       strcpy (fnn, filename);
    }
    for (p = fnn; *p; p++) {
      if (*p == '/')
         *p = '\\';
    }
    
    fnamew = get_wstring_from_mbstring(kpse->File_system_codepage, fnn, fnamew=NULL);
    for(i=0; (modew[i]=(wchar_t)mode[i]); i++) {} /* mode[i] must be ASCII */
    f = _wfopen(fnamew, modew);
    free (fnn);
    if (f == NULL)
        FATAL_PERROR(filename);
    if (KPATHSEA_DEBUG_P (KPSE_DEBUG_FOPEN)) {
        DEBUGF_START ();
        fprintf (stderr, "fsyscp_xfopen(%s [", filename);
        WriteConsoleW( GetStdHandle( STD_ERROR_HANDLE ), fnamew, wcslen( fnamew ), NULL, NULL );
#if defined(_WIN64)
        fprintf (stderr, "], %s) => 0x%I64x\n", mode, (unsigned __int64) f);
#else
        fprintf (stderr, "], %s) => 0x%lx\n", mode, (unsigned long) f);
#endif
        DEBUGF_END ();
    }
    free(fnamew);

    return f;
}

/*
  fopen by file system codepage
*/
FILE *
kpathsea_fsyscp_fopen (kpathsea kpse, const char *filename, const char *mode)
{
    FILE *f;
    wchar_t *fnamew, modew[4];
    int i;
    unsigned char *fnn;
    unsigned char *p;
    size_t len;

    assert(filename && mode);
    len = strlen(filename);
/*
  Support very long input path name, longer than _MAX_PATH for
  Windows, if it really exists and input name is given in
  full-absolute path in a command line.
  /./ , /../, \.\, \..\ should be excluded. (2020/06/06)
*/
    fnn = xmalloc(len + 10);
    p = strstr(filename, ".\\");
    if (!p) {
       p = strstr(filename, "./");
    }
    if (!p && len > 2 && ((filename[0] == '/' && filename[1] == '/') ||
        (filename[0] == '\\' && filename[1] == '\\' &&
         filename[2] != '?'))) {
       filename += 2;
       strcpy (fnn, "\\\\?\\UNC\\");
       strcat (fnn, filename);
    } else if (!p && len > 2 && filename[1] == ':') {
       strcpy (fnn, "\\\\?\\");
       strcat (fnn, filename);
    } else {
       strcpy (fnn, filename);
    }
    for (p = fnn; *p; p++) {
      if (*p == '/')
         *p = '\\';
    }

    fnamew = get_wstring_from_mbstring(kpse->File_system_codepage, fnn, fnamew=NULL);
    for(i=0; (modew[i]=(wchar_t)mode[i]); i++) {} /* mode[i] must be ASCII */
    f = _wfopen(fnamew, modew);
    free (fnn);
    if (f != NULL) {
        if (KPATHSEA_DEBUG_P (KPSE_DEBUG_FOPEN)) {
            DEBUGF_START ();
            fprintf (stderr, "fsyscp_fopen(%s [", filename);
            WriteConsoleW( GetStdHandle( STD_ERROR_HANDLE ), fnamew, wcslen( fnamew ), NULL, NULL );
#if defined(_WIN64)
            fprintf (stderr, "], %s) => 0x%I64x\n", mode, (unsigned __int64) f);
#else
            fprintf (stderr, "], %s) => 0x%lx\n", mode, (unsigned long) f);
#endif
            DEBUGF_END ();
        }
    }
    free(fnamew);

    return f;
}


FILE *
kpathsea_fsyscp_popen (kpathsea kpse, const char *command, const char *mode)
{
    FILE *f;
    wchar_t *commandw, modew[4];
    int i;

    assert(command && mode);

    if (is_include_space (command)) {
        const char *p;
        char *command2, *q;
        command2 = xmalloc (strlen (command) + 3);
        p = command;
        q = command2;
        *q++ = '\"';
        while (*p)
            *q++ = *p++;
        *q++ = '\"';
        *q = '\0';
        commandw = get_wstring_from_mbstring(kpse->File_system_codepage,command2, commandw=NULL);
        free (command2);
    } else {
        commandw = get_wstring_from_mbstring(kpse->File_system_codepage,command, commandw=NULL);
    }
    for(i=0; (modew[i]=(wchar_t)mode[i]); i++) {} /* mode[i] must be ASCII */
    f = _wpopen(commandw, modew);

    if (f != NULL) {
        if (KPATHSEA_DEBUG_P (KPSE_DEBUG_FOPEN)) {
            DEBUGF_START ();
            fprintf (stderr, "fsyscp_popen(%s [", command);
            WriteConsoleW( GetStdHandle( STD_ERROR_HANDLE ), commandw, wcslen( commandw ), NULL, NULL );
#if defined(_WIN64)
            fprintf (stderr, "], %s) => 0x%I64x\n", mode, (unsigned __int64) f);
#else
            fprintf (stderr, "], %s) => 0x%lx\n", mode, (unsigned long) f);
#endif
            DEBUGF_END ();
        }
    }

    free (commandw);
/* We use always binary mode on Windows */
    if(f) _setmode (fileno (f), _O_BINARY);

    return f;
}

int
kpathsea_get_command_line_args_utf8 (kpathsea kpse, const_string enc, int *p_ac, char ***p_av)
{
    int argc;
    string *argv;

    if (!enc || !strncmp(enc,"",1)) return 0;

#ifdef DEBUG
    fprintf(stderr, "command_line_encoding (%s)\n", enc);
#endif /* DEBUG */
    if (!(strncmp(enc,"utf8",5) && strncmp(enc,"utf-8",6))) {
      LPWSTR *argvw;
      INT argcw, i;
      string s;
#ifdef DEBUG
      DWORD ret;
      HANDLE hStderr;
      hStderr = GetStdHandle( STD_ERROR_HANDLE );
#endif /* DEBUG */
      kpse->File_system_codepage = CP_UTF8;
/*
  IS_KANJI() in the CP932-like system seems to be necessary to
  support non-ascii values for variables in the case of
  command_line_encoding = utf-8.
      kpse->Is_cp932_system = 0;
*/
      argvw = CommandLineToArgvW(GetCommandLineW(), &argcw);
      argc = argcw;
      argv = xmalloc(sizeof(char *)*(argcw+1));
      for (i=0; i<argcw; i++) {
        s = get_utf8_from_wstring(argvw[i], s=NULL);
        argv[i] = s;
#ifdef DEBUG
        fprintf(stderr, "Commandline arguments %d:(%s) [", i, argv[i]);
        WriteConsoleW( hStderr, argvw[i], wcslen(argvw[i]), &ret, NULL);
        fprintf(stderr, "]\n");
#endif /* DEBUG */
      }
      argv[argcw] = NULL;
      *p_ac = argc;
      *p_av = argv;
      return kpse->File_system_codepage;
    } else {
      return 0;
    }
}

/*
  spawnvp by file system codepage
*/
int
kpathsea_fsyscp_spawnvp (kpathsea kpse, int mode, const char *command, const char* const *argv)
{
    int ret;
    wchar_t *commandw, **argvw, **pw;
    int i;
    const char* const *p;

    assert(command && argv);
    for (i = 0, p = argv; *p; p++)
      i++;
    argvw = xcalloc (i + 3, sizeof (wchar_t *));
    commandw = get_wstring_from_mbstring(kpse->File_system_codepage, command, commandw=NULL);
    p = argv;
    pw = argvw;
    while (*p) {
      *pw = get_wstring_from_mbstring(kpse->File_system_codepage, *p, *pw=NULL);
      p++;
      pw++;
    }
    *pw = NULL;
    ret = _wspawnvp (mode, (const wchar_t *)commandw, (const wchar_t* const*) argvw);
    if(commandw) free(commandw);
    if (argvw) {
      pw = argvw;
      while (*pw) {
	free (*pw);
	pw++;
      }
      free (argvw);
    }

    return ret;
}

/*
  system by file system codepage
*/
int
kpathsea_fsyscp_system (kpathsea kpse, const char *cmd)
{
    const char *p;
    char  *q;
    char  *av[4];
    int   len, ret;
    int   spacep = 0;

    if (cmd == NULL)
      return 1;

    av[0] = xstrdup ("cmd.exe");
    av[1] = xstrdup ("/c");

    len = strlen (cmd) + 3;
    spacep = is_include_space (cmd);
    av[2] = xmalloc (len);
    q = av[2];
    if (spacep)
      *q++ = '"';
    for (p = cmd; *p; p++, q++) {
      if (*p == '\'')
        *q = '"';
      else
        *q = *p;
    }
    if (spacep)
      *q++ = '"';
    *q = '\0';
    av[3] = NULL;
    ret = kpathsea_fsyscp_spawnvp (kpse, _P_WAIT, av[0], (const char* const*) av);
    free (av[0]);
    free (av[1]);
    free (av[2]);
    return ret;
}

int
kpathsea_win32_getc(kpathsea kpse, FILE *fp)
{
    const int fd = fileno(fp);
    HANDLE hStdin;
    DWORD ret;
    wchar_t wc[3];
    char mbc[5];
    int j;

    if (!(fd == fileno(stdin) && _isatty(fd) && kpse->File_system_codepage == CP_UTF8))
        return getc(fp);

    if (kpse->getc_len == 0)
    {
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if (kpse->wcbuf) {
            wc[0] = kpse->wcbuf;
            kpse->wcbuf = L'\0';
        }
        else if (ReadConsoleW(hStdin, wc, 1, &ret, NULL) == 0)
            return EOF;
        if (0xd800<=wc[0] && wc[0]<0xdc00) {
            if (ReadConsoleW(hStdin, wc+1, 1, &ret, NULL) == 0)
                return EOF;
            if (0xdc00<=wc[1] && wc[1]<0xe000) {
                wc[2]=L'\0';
            } else {
                kpse->wcbuf=wc[1];
                wc[0]=0xfffd;    /* illegal surrogate pair */
                wc[1]=L'\0';
            }
        } else if (0xdc00<=wc[0] && wc[0]<0xe000) {
            wc[0]=0xfffd;        /* illegal surrogate pair */
            wc[1]=L'\0';
        } else {
            wc[1]=L'\0';
        }
        get_utf8_from_wstring(wc,mbc);
        j=strlen(mbc)-1;
        while(j>=0) {
            kpse->getc_buff[kpse->getc_len++]=(int)mbc[j--];
        }
    }
    return kpse->getc_buff[--kpse->getc_len];
}

int
kpathsea_win32_ungetc(kpathsea kpse, int c, FILE *fp)
{
    const int fd = fileno(fp);

    if (!(fd == fileno(stdin) && _isatty(fd) && kpse->File_system_codepage == CP_UTF8))
        return ungetc(c, fp);

    assert(kpse->getc_len < 4);
    return kpse->getc_buff[kpse->getc_len++] = c;
}

static int __win32_fputs(const char *str, HANDLE hStdout)
{
    DWORD ret;
    wchar_t *wstr;

    wstr = get_wstring_from_utf8(str, wstr=NULL);

    if (WriteConsoleW(hStdout, wstr, wcslen(wstr), &ret, NULL) == 0) {
        free(wstr);
        return EOF;
    }

    free(wstr);
    return ret;
}

int
kpathsea_win32_fputs(kpathsea kpse, const char *str, FILE *fp)
{
    const int fd = fileno(fp);
    HANDLE hStdout;

    if (!((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)
        && kpse->File_system_codepage == CP_UTF8))
        return fputs(str, fp);

    hStdout = (fd == fileno(stdout)) ?
        GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    return __win32_fputs(str, hStdout);
}

#define MAX_PROMPT_STR_SIZE 8192

int
kpathsea_win32_vfprintf(kpathsea kpse, FILE *fp, const char *format, va_list argp)
{
    const int fd = fileno(fp);
    HANDLE hStdout;
    char buff[MAX_PROMPT_STR_SIZE];
    int ret;

    if (!((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)
        && kpse->File_system_codepage == CP_UTF8))
        return vfprintf(fp, format, argp);

    hStdout = (fd == fileno(stdout)) ?
        GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    ret = _vsnprintf(buff, sizeof(buff), format, argp);
    if (__win32_fputs(buff, hStdout)==EOF) {
        return EOF;
    }
    return ret;
}

int
kpathsea_win32_puts(kpathsea kpse, const char *str)
{
    if (kpathsea_win32_fputs(kpse, str, stdout)==EOF) {
        return EOF;
    }
    return puts("");
}

int
kpathsea_win32_putc(kpathsea kpse, int c, FILE *fp)
{
    const int fd = fileno(fp);
    HANDLE hStdout;
    DWORD ret;
    wchar_t wstr[3];

    if (!((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)
        && kpse->File_system_codepage == CP_UTF8))
        return putc(c, fp);

    hStdout = (fd == fileno(stdout)) ?
        GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    c &= 0xff;

    if (c < 0x80) {
        kpse->st_str = kpse->st_buff;
        kpse->st_len = 1;
    }
    if (c < 0xc0) { /* ASCII or trailer */
        *(kpse->st_str)++ = c;
        kpse->st_len--;
        if (kpse->st_len == 0) {
            *(kpse->st_str) = '\0';
            get_wstring_from_utf8(kpse->st_buff, wstr);
            if (WriteConsoleW(hStdout, wstr, wcslen(wstr), &ret, NULL) == 0) {
                kpse->st_len = 0;
                return EOF;
            }
        }
        else if (kpse->st_len < 0) return EOF;
        return c;
    }
    else if (c < 0xc2) { kpse->st_len = 0; return EOF; }  /* illegal */
    else if (c < 0xe0) kpse->st_len = 2;
    else if (c < 0xf0) kpse->st_len = 3;
    else if (c < 0xf5) kpse->st_len = 4;
    else { kpse->st_len = 0; return EOF; }

    kpse->st_str = kpse->st_buff;
    *(kpse->st_str)++ = c;
    kpse->st_len--;
    return c;
}

int
kpathsea_IS_KANJI(kpathsea kpse, const char *p)
{
  int ret;

  ret = kpse->Is_cp932_system && kpathsea_isknj(kpse, *(p)) &&
        kpathsea_isknj2(kpse, *(p+1));
  return ret;
}

char *
kpathsea_get_fsyscp_from_wstring(kpathsea kpse, const wchar_t *w,char *mb)
{
  return get_mbstring_from_wstring(kpse->File_system_codepage, w, mb);
}

wchar_t *
kpathsea_get_wstring_from_fsyscp(kpathsea kpse, const char *mb,wchar_t *w)
{
  return get_wstring_from_mbstring(kpse->File_system_codepage, mb, w);
}

#if defined (KPSE_COMPAT_API)

int
isknj(int c)
{
  return kpathsea_isknj(kpse_def, c);
}

int
isknj2(int c)
{
  return kpathsea_isknj2(kpse_def, c);
}

FILE *
fsyscp_xfopen (const char *filename, const char *mode)
{
  return kpathsea_fsyscp_xfopen (kpse_def, filename, mode);
}

FILE *
fsyscp_fopen (const char *filename, const char *mode)
{
  return kpathsea_fsyscp_fopen (kpse_def, filename, mode);
}

FILE *
fsyscp_popen (const char *command, const char *mode)
{
  return kpathsea_fsyscp_popen (kpse_def, command, mode);
}

int
get_command_line_args_utf8 (const_string enc, int *p_ac, char ***p_av)
{
  return kpathsea_get_command_line_args_utf8 (kpse_def, enc, p_ac, p_av);
}

int
fsyscp_spawnvp (int mode, const char *command, const char* const *argv)
{
  return kpathsea_fsyscp_spawnvp (kpse_def, mode, command, argv);
}

int
fsyscp_system (const char *cmd)
{
  return kpathsea_fsyscp_system (kpse_def, cmd);
}

int
win32_getc(FILE *fp)
{
  return kpathsea_win32_getc(kpse_def, fp);
}

int
win32_ungetc(int c, FILE *fp)
{
  return kpathsea_win32_ungetc(kpse_def, c, fp);
}

int
win32_fputs(const char *str, FILE *fp)
{
  return kpathsea_win32_fputs(kpse_def, str, fp);
}

int
win32_vfprintf(FILE *fp, const char *format, va_list argp)
{
  return kpathsea_win32_vfprintf(kpse_def, fp, format, argp);
}

int
win32_puts(const char *str)
{
  return kpathsea_win32_puts(kpse_def, str);
}

int
win32_putc(int c, FILE *fp)
{
  return kpathsea_win32_putc(kpse_def, c, fp);
}

int
IS_KANJI(const char *p)
{
  return kpathsea_IS_KANJI(kpse_def, p);
}

char *
get_fsyscp_from_wstring(const wchar_t *w,char *mb)
{
  return kpathsea_get_fsyscp_from_wstring(kpse_def, w, mb);
}

wchar_t *
get_wstring_from_fsyscp(const char *mb,wchar_t *w)
{
  return kpathsea_get_wstring_from_fsyscp(kpse_def, mb, w);
}
#endif
