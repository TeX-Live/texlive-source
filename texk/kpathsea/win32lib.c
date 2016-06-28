/* win32lib.c: bits and pieces for win32 and msvc.

   Copyright 2006, 2011-2016 Akira Kakuto.
   Copyright 1996, 1997, 1998, 1999 Fabrice Popineau.

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
#include <kpathsea/c-stat.h>

static int is_include_space(const char *s)
{
  char *p;
  p = strchr(s, ' ');
  if(p) return 1;
  p = strchr(s, '\t');
  if(p) return 1;
  return 0;
}

double win32_floor (double x)
{
  return floor (x);
}

FILE * win32_popen (const char *cmd, const char *fmode)
{
  char mode[3];
  char *p, *q;
  const char *cmd2;
  FILE *ret;

  mode[0] = fmode[0];
  mode[1] = 'b';
  mode[2] = '\0';

  if (is_include_space (cmd)) {
    cmd2 = xmalloc (strlen (cmd) + 3);
    q = (char *)cmd2;
    p = (char *)cmd;
    *q++= '\"';
    while(*p)
      *q++ = *p++;
    *q++ = '\"';
    *q = '\0';
    ret = _popen (cmd2, mode);
    free ((char *)cmd2);
    return ret;
  } else {
    return _popen (cmd, mode);
  }
}

int win32_pclose (FILE *f)
{
  return _pclose (f);
}

/* large file support */

__int64
xftell64 (FILE *f, const char *filename)
{
#if _MSC_VER > 1200
    __int64 where;
    where = _ftelli64(f);
    if (where < (__int64)0)
        FATAL_PERROR(filename);
#else
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
#endif
  return where;
}

void
xfseek64 (FILE *f, __int64 offset, int wherefrom,  const char *filename)
{
#if _MSC_VER > 1200
    if (_fseeki64(f, offset, wherefrom) < (__int64)0)
        FATAL_PERROR(filename);
#else
  if(wherefrom == SEEK_CUR) {
    offset += xftell64(f, filename);
    wherefrom = SEEK_SET;
  }
  fflush(f);
  if (_lseeki64(fileno(f), offset, wherefrom) < (__int64)0)
    FATAL_PERROR(filename);
#endif
}


/* special TeXLive Ghostscript */

static int is_dir (char *buff)
{
  struct stat stats;

  return stat (buff, &stats) == 0 && S_ISDIR (stats.st_mode);
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
  nptr = kpse_var_value("TEXLIVE_WINDOWS_EXTERNAL_GS");
  if (nptr == NULL || !strcmp(nptr, "0") || !strcmp(nptr, "n") || !strcmp(nptr, "f")) {
    if (nptr)
      free (nptr);
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
  } else {
    free (nptr);
  }
}

/*
 path name in char *input is changed into the long
 name format and returned in char *buff.
 return value: 0 if failed
               1 if succeeded
*/

int kpathsea_getlongpath(kpathsea kpse, char *buff, char *input, int len)
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
         if (kpathsea_IS_KANJI(kpse, p)) {
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
         if (kpathsea_IS_KANJI(kpse, p)) {
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
      if(kpathsea_IS_KANJI(kpse, p)) {
         p++;
         continue;
      }
      if(*p == '\\') {
         *p = '\0';
         if((*(p-2) == '\\' || p-1 == q) && *(p-1) == '.') {
            cnt += 2;
            if(cnt > len) return 0;
            strcat(buff, "./");
         } else if((*(p-3) == '\\' || p-2 == q) && *(p-2) == '.' && *(p-1) == '.') {
            cnt += 3;
            if(cnt > len) return 0;
            strcat(buff, "../");
         } else {
            if((hnd = FindFirstFile(q, &ffd)) == INVALID_HANDLE_VALUE) {
               return 0;
            }
            FindClose(hnd);
            cnt += strlen(ffd.cFileName);
            cnt++;
            if(cnt > len) return 0;
            strcat(buff, ffd.cFileName);
            strcat(buff, "/");
         }
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

char * kpathsea_get_home_directory(kpathsea kpse)
{
  char *p;
  char *home = getenv("HOME");
  if(!home)
    home = getenv("USERPROFILE");
  if(home) {
    home = xstrdup(home);
    for(p = home; *p; p++) {
      if(kpathsea_IS_KANJI(kpse, p)) {
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

   home = kpathsea_get_home_directory(kpse);
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
   strcpy (kpse->the_passwd.pw_dir, kpathsea_get_home_directory(kpse));
   strcpy (kpse->the_passwd.pw_shell, getenv ("SHELL"));
}

/* win32_system */
int win32_system(const char *cmd)
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
  ret = _spawnvp(_P_WAIT, av[0], av);
  free(av[0]);
  free(av[1]);
  free(av[2]);
  return ret;
}

#if defined (KPSE_COMPAT_API)
int getlongpath(char *buff, char *input, int len)
{
  return kpathsea_getlongpath(kpse_def, buff, input, len);
}

char * get_home_directory(void)
{
  return kpathsea_get_home_directory(kpse_def);
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
