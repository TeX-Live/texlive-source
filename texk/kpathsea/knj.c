/* knj.c: check for 2-Byte Kanji (CP 932, SJIS) codes.

   Copyright 2010, 2016 Akira Kakuto.
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

int is_cp932_system, file_system_codepage;

int isknj(int c)
{
  c &= 0xff;
  switch (is_cp932_system) {
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

int isknj2(int c)
{
  c &= 0xff;
  switch (is_cp932_system) {
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

/*
  xfopen by file system codepage
*/
FILE *
fsyscp_xfopen (const char *filename, const char *mode)
{
    FILE *f;
    wchar_t *fnamew, modew[4];
    int i;
#if defined (KPSE_COMPAT_API)
    kpathsea kpse;
#endif
    assert(filename && mode);

    fnamew = get_wstring_from_fsyscp(filename, fnamew=NULL);
    for(i=0; (modew[i]=(wchar_t)mode[i]); i++) {} /* mode[i] must be ASCII */
    f = _wfopen(fnamew, modew);
    if (f == NULL)
        FATAL_PERROR(filename);
#if defined (KPSE_COMPAT_API)
    kpse = kpse_def;
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
#endif
    free(fnamew);

    return f;
}

/*
  fopen by file system codepage
*/
FILE *
fsyscp_fopen (const char *filename, const char *mode)
{
    FILE *f;
    wchar_t *fnamew, modew[4];
    int i;
#if defined (KPSE_COMPAT_API)
    kpathsea kpse;
#endif
    assert(filename && mode);

    fnamew = get_wstring_from_fsyscp(filename, fnamew=NULL);
    for(i=0; (modew[i]=(wchar_t)mode[i]); i++) {} /* mode[i] must be ASCII */
    f = _wfopen(fnamew, modew);
#if defined (KPSE_COMPAT_API)
    if (f != NULL) {
        kpse = kpse_def;
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
#endif
    free(fnamew);

    return f;
}

/*
  popen by file system codepage
*/
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

FILE *
fsyscp_popen (const char *command, const char *mode)
{
    FILE *f;
    wchar_t *commandw, modew[4];
    int i;
#if defined (KPSE_COMPAT_API)
    kpathsea kpse;
#endif
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
        commandw = get_wstring_from_fsyscp(command2, commandw=NULL);
        free (command2);
    } else {
        commandw = get_wstring_from_fsyscp(command, commandw=NULL);
    }
    for(i=0; (modew[i]=(wchar_t)mode[i]); i++) {} /* mode[i] must be ASCII */
    f = _wpopen(commandw, modew);
#if defined (KPSE_COMPAT_API)
    if (f != NULL) {
        kpse = kpse_def;
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
#endif
    free (commandw);
/* We use always binary mode on Windows */
    if(f) _setmode (fileno (f), _O_BINARY);

    return f;
}

int
get_command_line_args_utf8 (const_string enc, int *p_ac, char ***p_av)
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
      file_system_codepage = CP_UTF8;
      is_cp932_system = 0;
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
      return file_system_codepage;
    } else {
      return 0;
    }
}

/*
  spawnvp by file system codepage
*/
int
fsyscp_spawnvp (int mode, const char *command, const char* const *argv)
{
    int ret;
    wchar_t *commandw, **argvw, **pw;
    int i;
    const char* const *p;

    assert(command && argv);
    for (i = 0, p = argv; *p; p++)
      i++;
    argvw = xcalloc (i + 3, sizeof (wchar_t *));
    commandw = get_wstring_from_fsyscp(command, commandw=NULL);
    p = argv;
    pw = argvw;
    while (*p) {
      *pw = get_wstring_from_fsyscp(*p, *pw=NULL);
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
fsyscp_system (const char *cmd)
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
    ret = fsyscp_spawnvp (_P_WAIT, av[0], (const char* const*) av);
    free (av[0]);
    free (av[1]);
    free (av[2]);
    return ret;
}

static int getc_len;
static int getc_buff[4];

int win32_getc(FILE *fp)
{
    const int fd = fileno(fp);
    HANDLE hStdin;
    DWORD ret;
    wchar_t wc[3];
    char mbc[5];
    int j;
    static wchar_t wcbuf = L'\0';

    if (!(fd == fileno(stdin) && _isatty(fd) && file_system_codepage == CP_UTF8))
        return getc(fp);

    if (getc_len == 0)
    {
        hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if (wcbuf) {
            wc[0] = wcbuf;
            wcbuf = L'\0';
        }
        else if (ReadConsoleW(hStdin, wc, 1, &ret, NULL) == 0)
            return EOF;
        if (0xd800<=wc[0] && wc[0]<0xdc00) {
            if (ReadConsoleW(hStdin, wc+1, 1, &ret, NULL) == 0)
                return EOF;
            if (0xdc00<=wc[1] && wc[1]<0xe000) {
                wc[2]=L'\0';
            } else {
                wcbuf=wc[1];
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
            getc_buff[getc_len++]=(int)mbc[j--];
        }
    }
    return getc_buff[--getc_len];
}

int win32_ungetc(int c, FILE *fp)
{
    const int fd = fileno(fp);

    if (!(fd == fileno(stdin) && _isatty(fd) && file_system_codepage == CP_UTF8))
        return ungetc(c, fp);

    assert(getc_len < 4);
    return getc_buff[getc_len++] = c;
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

int win32_fputs(const char *str, FILE *fp)
{
    const int fd = fileno(fp);
    HANDLE hStdout;

    if (!((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)
        && file_system_codepage == CP_UTF8))
        return fputs(str, fp);

    hStdout = (fd == fileno(stdout)) ?
        GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    return __win32_fputs(str, hStdout);
}

#define MAX_PROMPT_STR_SIZE 8192

int win32_vfprintf(FILE *fp, const char *format, va_list argp)
{
    const int fd = fileno(fp);
    HANDLE hStdout;
    char buff[MAX_PROMPT_STR_SIZE];
    int ret;

    if (!((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)
        && file_system_codepage == CP_UTF8))
        return vfprintf(fp, format, argp);

    hStdout = (fd == fileno(stdout)) ?
        GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    ret = _vsnprintf(buff, sizeof(buff), format, argp);
    if (__win32_fputs(buff, hStdout)==EOF) {
        return EOF;
    }
    return ret;
}

int win32_puts(const char *str)
{
    if (win32_fputs(str, stdout)==EOF) {
        return EOF;
    }
    return puts("");
}

int win32_putc(int c, FILE *fp)
{
    const int fd = fileno(fp);
    HANDLE hStdout;
    DWORD ret;
    wchar_t wstr[3];
    static int len = 0;
    static char buff[5], *str;

    if (!((fd == fileno(stdout) || fd == fileno(stderr)) && _isatty(fd)
        && file_system_codepage == CP_UTF8))
        return putc(c, fp);

    hStdout = (fd == fileno(stdout)) ?
        GetStdHandle(STD_OUTPUT_HANDLE) : GetStdHandle(STD_ERROR_HANDLE);

    c &= 0xff;

    if (c < 0x80) {
        str = buff;
        len = 1;
    }
    if (c < 0xc0) { /* ASCII or trailer */
        *str++ = c;
        len--;
        if (len == 0) {
            *str = '\0';
            get_wstring_from_utf8(buff, wstr);
            if (WriteConsoleW(hStdout, wstr, wcslen(wstr), &ret, NULL) == 0) {
                len = 0;
                return EOF;
            }
        }
        else if (len < 0) return EOF;
        return c;
    }
    else if (c < 0xc2) { len = 0; return EOF; }  /* illegal */
    else if (c < 0xe0) len = 2;
    else if (c < 0xf0) len = 3;
    else if (c < 0xf5) len = 4;
    else { len = 0; return EOF; }

    str = buff;
    *str++ = c;
    len--;
    return c;
}
