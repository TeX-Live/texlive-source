
#ifndef _OLDNAMES_H_
#define _OLDNAMES_H_

/* These have to be defined because our compilers treat __STDC__ as being
   defined (most of them anyway). */
/* Non-ANSI names for compatibility. Those symbols are defined
   in oldnames.lib .
   However, even if it is less safe, it might be better to just
   #define the oldnames to the new names. Use the next macro
   to change the behaviour.
*/

#if __STDC__

/* from fcntl.h */

#ifndef O_RDONLY
#define O_RDONLY _O_RDONLY
#endif
#ifndef O_WRONLY
#define O_WRONLY _O_WRONLY
#endif
#ifndef O_RDWR
#define O_RDWR _O_RDWR
#endif
#ifndef O_APPEND
#define O_APPEND _O_APPEND
#endif
#ifndef O_CREAT
#define O_CREAT _O_CREAT
#endif
#ifndef O_TRUNC
#define O_TRUNC _O_TRUNC
#endif
#ifndef O_EXCL
#define O_EXCL _O_EXCL
#endif
#ifndef O_TEXT
#define O_TEXT _O_TEXT
#endif
#ifndef O_BINARY
#define O_BINARY _O_BINARY
#endif
#ifndef O_RAW
#define O_RAW           _O_BINARY
#endif
#ifndef O_TEMPORARY
#define O_TEMPORARY     _O_TEMPORARY
#endif
#ifndef O_NOINHERIT
#define O_NOINHERIT     _O_NOINHERIT
#endif
#ifndef O_SEQUENTIAL
#define O_SEQUENTIAL    _O_SEQUENTIAL
#endif
#ifndef O_RANDOM
#define O_RANDOM        _O_RANDOM
#endif


#include <sys/types.h>

#ifdef  __cplusplus
extern "C" {
#endif

struct stat {
        _dev_t st_dev;
        _ino_t st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        _dev_t st_rdev;
        _off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };

/* from io.h */
#if OLDNAMES_WITH_MACROS
#define access _access
#define chmod _chmod
#define chsize _chsize
#define close _close
#define creat _creat
#define dup _dup
#define dup2 _dup2
#if 0
/* This one conflicts with texk/web2/lib/eofeoln.c */
#define eof _eof
#endif
#define filelength _filelength
#define isatty _isatty
#define locking _locking
#define lseek _lseek
#define mktemp _mktemp
#define open _open
#define read _read
#define setmode _setmode
#define sopen _sopen
#define tell _tell
#define umask _umask
#define unlink _unlink
#define write _write
#else
_CRTIMP int __cdecl access(const char *, int);
_CRTIMP int __cdecl chmod(const char *, int);
_CRTIMP int __cdecl chsize(int, long);
_CRTIMP int __cdecl close(int);
_CRTIMP int __cdecl creat(const char *, int);
_CRTIMP int __cdecl dup(int);
_CRTIMP int __cdecl dup2(int, int);
#if 0
_CRTIMP int __cdecl eof(int);
#endif
_CRTIMP long __cdecl filelength(int);
_CRTIMP int __cdecl isatty(int);
_CRTIMP int __cdecl locking(int, int, long);
_CRTIMP long __cdecl lseek(int, long, int);
_CRTIMP char * __cdecl mktemp(char *);
_CRTIMP int __cdecl open(const char *, int, ...);
_CRTIMP int __cdecl read(int, void *, unsigned int);
_CRTIMP int __cdecl setmode(int, int);
_CRTIMP int __cdecl sopen(const char *, int, int, ...);
_CRTIMP long __cdecl tell(int);
_CRTIMP int __cdecl umask(int);
_CRTIMP int __cdecl unlink(const char *);
_CRTIMP int __cdecl write(int, const void *, unsigned int);
#endif

/* from conio.h */
#if OLDNAMES_WITH_MACROS
#define cgets _cgets
#define cprintf _cprintf
#define cputs _cputs
#define cscanf _cscanf
#define inp _inp
#define inpw _inpw
#define getch _getch
#define getche _getche
#define kbhit _kbhit
#define outp _outp
#define outpw _outpw
#define putch _putch
#define ungetch _ungetch
#else
_CRTIMP char * __cdecl cgets(char *);
_CRTIMP int __cdecl cprintf(const char *, ...);
_CRTIMP int __cdecl cputs(const char *);
_CRTIMP int __cdecl cscanf(const char *, ...);
int __cdecl inp(unsigned short);
unsigned short __cdecl inpw(unsigned short);
_CRTIMP int __cdecl getch(void);
_CRTIMP int __cdecl getche(void);
_CRTIMP int __cdecl kbhit(void);
int __cdecl outp(unsigned short, int);
unsigned short __cdecl outpw(unsigned short, unsigned short);
_CRTIMP int __cdecl putch(int);
_CRTIMP int __cdecl ungetch(int);
#endif

/* from malloc.h */
#define alloca     _alloca

/* from direct.h */
#if OLDNAMES_WITH_MACROS
#define chdir _chdir
#define getcwd _getcwd
#define mkdir _mkdir
#define rmdir _rmdir
#else
_CRTIMP int __cdecl chdir(const char *);
_CRTIMP char * __cdecl getcwd(char *, int);
_CRTIMP int __cdecl mkdir(const char *);
_CRTIMP int __cdecl rmdir(const char *);
#endif

/* from time.h */
#define CLK_TCK  CLOCKS_PER_SEC

#if OLDNAMES_WITH_MACROS
#define daylight _daylight
#define timezone _timezone
#define tzname _tzname

#define tzset _tzset
#else
_CRTIMP extern int daylight;
_CRTIMP extern long timezone;
_CRTIMP extern char * tzname[2];

#define tzset _tzset
#endif

/* from process.h */
#define P_WAIT          _P_WAIT
#define P_NOWAIT        _P_NOWAIT
#define P_OVERLAY       _P_OVERLAY
#define OLD_P_OVERLAY   _OLD_P_OVERLAY
#define P_NOWAITO       _P_NOWAITO
#define P_DETACH        _P_DETACH
#define WAIT_CHILD      _WAIT_CHILD
#define WAIT_GRANDCHILD _WAIT_GRANDCHILD

#if OLD_P_OVERLAY
#define cwait _cwait
#define execl _execl
#define execle _execle
#define execlp _execlp
#define execlpe _execlpe
#define execv _execv
#define execve _execve
#define execvp _execvp
#define execvpe _execvpe
#define spawnl _spawnl
#define spawnle _spawnle
#define spawnlp _spawnlp
#define spawnlpe _spawnlpe
#define spawnv _spawnv
#define spawnve _spawnve
#define spawnvp _spawnvp
#define spawnvpe _spawnvpe

#define getpid _getpid
#else
_CRTIMP int __cdecl cwait(int *, int, int);
_CRTIMP int __cdecl execl(const char *, const char *, ...);
_CRTIMP int __cdecl execle(const char *, const char *, ...);
_CRTIMP int __cdecl execlp(const char *, const char *, ...);
_CRTIMP int __cdecl execlpe(const char *, const char *, ...);
_CRTIMP int __cdecl execv(const char *, const char * const *);
_CRTIMP int __cdecl execve(const char *, const char * const *, const char * const *);
_CRTIMP int __cdecl execvp(const char *, const char * const *);
_CRTIMP int __cdecl execvpe(const char *, const char * const *, const char * const *);
_CRTIMP int __cdecl spawnl(int, const char *, const char *, ...);
_CRTIMP int __cdecl spawnle(int, const char *, const char *, ...);
_CRTIMP int __cdecl spawnlp(int, const char *, const char *, ...);
_CRTIMP int __cdecl spawnlpe(int, const char *, const char *, ...);
_CRTIMP int __cdecl spawnv(int, const char *, const char * const *);
_CRTIMP int __cdecl spawnve(int, const char *, const char * const *,
        const char * const *);
_CRTIMP int __cdecl spawnvp(int, const char *, const char * const *);
_CRTIMP int __cdecl spawnvpe(int, const char *, const char * const *,
        const char * const *);

_CRTIMP int __cdecl getpid(void);
#endif

/* from stdio.h */
#define P_tmpdir  _P_tmpdir
#define SYS_OPEN  _SYS_OPEN

#if OLDNAMES_WITH_MACROS
#define fcloseall _fcloseall
#define fdopen _fdopen
#define fgetchar _fgetchar
#define fileno _fileno
#define flushall _flushall
#define fputchar _fputchar
#define getw _getw
#define putw _putw
#define rmtmp _rmtmp
#define tempnam _tempnam
#define unlink _unlink
#else
_CRTIMP int __cdecl fcloseall(void);
_CRTIMP FILE * __cdecl fdopen(int, const char *);
_CRTIMP int __cdecl fgetchar(void);
_CRTIMP int __cdecl fileno(FILE *);
_CRTIMP int __cdecl flushall(void);
_CRTIMP int __cdecl fputchar(int);
_CRTIMP int __cdecl getw(FILE *);
_CRTIMP int __cdecl putw(int, FILE *);
_CRTIMP int __cdecl rmtmp(void);
_CRTIMP char * __cdecl tempnam(const char *, const char *);
_CRTIMP int __cdecl unlink(const char *);
#endif

/* from stdlib.h */

#define sys_errlist _sys_errlist
#define sys_nerr    _sys_nerr
#define environ     _environ

#define onexit_t _onexit_t

#if OLDNAMES_WITH_MACROS
#define ecvt _ecvt
#define fcvt _fcvt
#define gcvt _gcvt
#define itoa _itoa
#define ltoa _ltoa
#define onexit _onexit
#define putenv _putenv
#define swab _swab
#define ultoa _ultoa
#else
_CRTIMP char * __cdecl ecvt(double, int, int *, int *);
_CRTIMP char * __cdecl fcvt(double, int, int *, int *);
_CRTIMP char * __cdecl gcvt(double, int, char *);
_CRTIMP char * __cdecl itoa(int, char *, int);
_CRTIMP char * __cdecl ltoa(long, char *, int);
        onexit_t __cdecl onexit(onexit_t);
_CRTIMP int    __cdecl putenv(const char *);
_CRTIMP void   __cdecl swab(char *, char *, int);
_CRTIMP char * __cdecl ultoa(unsigned long, char *, int);
#endif

/* from sys/stat.h */
#ifndef S_IFMT
#define S_IFMT _S_IFMT
#endif
#ifndef S_IFDIR
#define S_IFDIR _S_IFDIR
#endif
#ifndef S_IFCHR
#define S_IFCHR _S_IFCHR
#endif
#ifndef S_IFIFO
#define S_IFIFO _S_IFIFO
#endif
#ifndef S_IFREG
#define S_IFREG _S_IFREG
#endif
#ifndef S_IREAD
#define S_IREAD _S_IREAD
#endif
#ifndef S_IWRITE
#define S_IWRITE _S_IWRITE
#endif
#ifndef S_IEXEC
#define S_IEXEC  _S_IEXEC
#endif
#ifndef S_IXUSR
#define S_IXUSR _S_IEXEC
#endif
#ifndef S_IXGRP
#define S_IXGRP _S_IEXEC
#endif
#ifndef S_IXOTH
#define S_IXOTH _S_IEXEC
#endif
#ifndef S_IRUSR
#define S_IRUSR _S_IREAD
#endif
#ifndef S_IRGRP
#define S_IRGRP _S_IREAD
#endif
#ifndef S_IROTH
#define S_IROTH _S_IREAD
#endif
#ifndef S_IWUSR
#define S_IWUSR _S_IWRITE
#endif
#ifndef S_IWGRP
#define S_IWGRP _S_IWRITE
#endif
#ifndef S_IWOTH
#define S_IWOTH _S_IWRITE
#endif

#if OLDNAMES_WITH_MACROS
#define fstat _fstat
#else
_CRTIMP int __cdecl fstat(int, struct stat *);
#endif

/* Alternatively, we could always use the MS one.
   Which one is the most buggy ? :-> */
#if REPLACE_LIBC_FUNCTIONS
# define stat(p, s) win32_stat(p, s)
# define system(p) win32_system(p, 0)
# define popen(cmd, mode) win32_popen(cmd, mode)
# define pclose(file) win32_pclose(file)
#else
# define popen(cmd, mode) _popen(cmd, mode)
# define pclose(file) _pclose(file)
_CRTIMP int __cdecl system(const char *);
_CRTIMP FILE * __cdecl _popen(const char *, const char *);
_CRTIMP int __cdecl _pclose(FILE *);
# if OLDNAMES_WITH_MACROS
#  define stat _stat
# else
_CRTIMP int __cdecl stat(const char *, struct stat *);
# endif
#endif

/* from sys/types.h */
typedef long off_t;

/* from string.h */
#if OLDNAMES_WITH_MACROS
#define memccpy _memccpy
#define memicmp _memicmp
#define strcmpi _strcmpi
#define stricmp _stricmp
#define strdup _strdup
#define strlwr _strlwr
#define strnicmp _strnicmp
#define strnset _strnset
#define strrev _strrev
#define strset _strset
#define strupr _strupr
#else
_CRTIMP void * __cdecl memccpy(void *, const void *, int, unsigned int);
_CRTIMP int __cdecl memicmp(const void *, const void *, unsigned int);
_CRTIMP int __cdecl strcmpi(const char *, const char *);
_CRTIMP int __cdecl stricmp(const char *, const char *);
_CRTIMP char * __cdecl strdup(const char *);
_CRTIMP char * __cdecl strlwr(char *);
_CRTIMP int __cdecl strnicmp(const char *, const char *, size_t);
_CRTIMP char * __cdecl strnset(char *, int, size_t);
_CRTIMP char * __cdecl strrev(char *);
        char * __cdecl strset(char *, int);
_CRTIMP char * __cdecl strupr(char *);
#endif

/* from ctype.h */
#if !defined(_CTYPE_DEFINED) && !defined(OLDNAMES_WITH_MACROS)
_CRTIMP int __cdecl isascii(int);
_CRTIMP int __cdecl toascii(int);
_CRTIMP int __cdecl iscsymf(int);
_CRTIMP int __cdecl iscsym(int);
#else
#define isascii __isascii
#define toascii __toascii
#define iscsymf __iscsymf
#define iscsym  __iscsym
#endif

/* from sys/time.h */
/* Non-ANSI name for compatibility */

struct timeb {
        time_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };

#if OLDNAMES_WITH_MACROS
#define ftime _ftime
#else
_CRTIMP void __cdecl ftime(struct timeb *);
#endif

/* from sys/utime.h */

struct utimbuf {
        time_t actime;          /* access time */
        time_t modtime;         /* modification time */
        };

#if OLDNAMES_WITH_MACROS
#define utime _utime
#else
_CRTIMP int __cdecl utime(const char *, struct utimbuf *);
#endif

#ifdef  __cplusplus
}
#endif

#endif /* __STDC__ */

#endif /* _OLDNAMES_H_ */
