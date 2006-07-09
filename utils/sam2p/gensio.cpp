/*
 * gensio.cpp -- IO-specific methods
 * by pts@fazekas.hu at Tue Feb 26 13:28:12 CET 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#if 0
extern "C" int errno;
/* OK: autodetect with autoconf */
extern "C" int _l_stat(const char *file_name, struct stat *buf);
/* OK: Imp: not in ANSI C, but we cannot emulate it! */
extern "C" int _v_s_n_printf ( char *str, size_t n, const char *format, va_list ap );
#else
#undef __STRICT_ANSI__ /* for __MINGW32__ */
#define _BSD_SOURCE 1 /* vsnprintf(); may be emulated with fixup_vsnprintf() */
#ifndef __APPLE__ /* SUXX: Max OS X has #ifndef _POSIX_SOURCE around lstat() :-( */
#define _POSIX_SOURCE 1 /* also popen() */
#endif
#define _POSIX_C_SOURCE 2 /* also popen() */
#define _XOPEN_SOURCE_EXTENDED 1 /* Digital UNIX lstat */
#ifndef _XPG4_2
#define _XPG4_2 1 /* SunOS 5.7 lstat() */
#endif
#undef  _XOPEN_SOURCE /* pacify gcc-3.1 */
#define _XOPEN_SOURCE 1 /* popen() on Digital UNIX */
#endif

#include "gensio.hpp"
#include "error.hpp"
#include <string.h> /* strlen() */
#include <stdarg.h> /* va_list */
#if _MSC_VER > 1000
// extern "C" int getpid(void);
#  include <process.h>
#else
#  include <unistd.h> /* getpid() */
#endif
#include <sys/stat.h> /* struct stat */
#include <stdlib.h> /* getenv() */
#include <errno.h>
#include <signal.h> /* signal() */ /* Imp: use sigaction */
#if HAVE_DOS_BINARY
#undef __STRICT_ANSI__
#include <fcntl.h> /* O_BINARY */
#include <io.h> /* setmode() */
#endif

#define USGE(a,b) ((unsigned char)(a))>=((unsigned char)(b))

#if HAVE_PTS_VSNPRINTF /* Both old and c99 work OK */
#  define VSNPRINTF vsnprintf
#else
#  if OBJDEP
#    warning REQUIRES: snprintf.o
#  endif
#  include "snprintf.h"
#  define VSNPRINTF fixup_vsnprintf /* Tested, C99. */
#endif

static void cleanup(int) {
  Error::cexit(126);
}

void Files::doSignalCleanup() {
  signal(SIGINT, cleanup);
  signal(SIGTERM, cleanup);
#ifdef SIGHUP
  signal(SIGHUP, SIG_IGN);
#endif
  /* Dat: don't do cleanup for SIGQUIT */
}

GenBuffer::Writable& SimBuffer::B::vformat(slen_t n, char const *fmt, va_list ap) {
  /* Imp: test this code in various systems and architectures. */
  /* Dat: vsnprintf semantics are verified in configure AC_PTS_HAVE_VSNPRINTF,
   * and a replacement vsnprintf is provided in case of problems. We don't
   * depend on HAVE_PTS_VSNPRINTF_C99, because C99-vsnprintf is a run-time
   * property.
   *
   * C99 vsnprintf semantics: vsnprintf() always returns a non-negative value:
   *   the number of characters (trailing \0 not included) that would have been
   *   printed if there were enough space. Only the first maxlen (@param)
   *   characters may be modified. The output is terminated by \0 iff maxlen!=0.
   *   NULL @param dststr is OK iff maxlen==0. Since glibc 2.1.
   * old vsnprintf semantics: vsnprintf() returns a non-negative value or -1:
   *   0 if maxlen==0, otherwise: -1 if maxlen is shorter than all the characters
   *   plus the trailing \0, otherwise: the number of characters (trailing \0 not
   *   included) that were printed. Only the first maxlen (@param)
   *   characters may be modified. The output is terminated by \0 iff maxlen!=0.
   *   NULL @param dststr is OK iff maxlen==0.
   */
  if (n>0) { /* avoid problems with old-style vsnprintf */
    char *s; vi_grow2(0, n+1, 0, &s); len-=n+1; /* +1: sprintf appends '\0' */
    const_cast<char*>(beg)[len]='\0'; /* failsafe sentinel */
    slen_t did=VSNPRINTF(s, n+1, fmt, ap);
    if (did>n) did=n;
    /* ^^^ Dat: always true: (unsigned)-1>n, so this works with both old and c99 */
    /* Now: did contains the # chars to append, without trailing '\0' */
    /* Dat: we cannot check for -1, because `did' is unsigned */
    /* Dat: trailer '\0' doesn't count into `did' */
    len+=did;
  }
  return *this;
}
GenBuffer::Writable& SimBuffer::B::vformat(char const *fmt, va_list ap) {
  char dummy, *s;
  slen_t did=VSNPRINTF(&dummy, 1, fmt, ap), n;
  if (did>0) { /* skip if nothing to be appended */
    /* vvv Dat: we cannot check for -1, because `did' is unsigned */
    if ((did+1)!=(slen_t)0) { /* C99 semantics; quick shortcut */
      vi_grow2(0, (n=did)+1, 0, &s); len-=n+1;
      ASSERT_SIDE2(VSNPRINTF(s, n+1, fmt, ap), * 1U==did);
    } else { /* old semantics: grow the buffer incrementally */
      if ((n=strlen(fmt))<16) n=16; /* initial guess */
      while (1) {
        vi_grow2(0, n+1, 0, &s); len-=n+1; /* +1: sprintf appends '\0' */
        const_cast<char*>(beg)[len]='\0'; /* failsafe sentinel */
        did=VSNPRINTF(s, n+1, fmt, ap);
        if ((did+1)!=(slen_t)0) {
          assert(did!=0); /* 0 is caught early in this function */
          assert(did<=n); /* non-C99 semantics */
          break;
        }
        n<<=1;
      }
    }
    len+=did;
  }
  return *this;
}

GenBuffer::Writable& SimBuffer::B::format(slen_t n, char const *fmt, ...) {
  va_list ap;
  PTS_va_start(ap, fmt);
  vformat(n, fmt, ap);
  va_end(ap);
  return *this;
}
GenBuffer::Writable& SimBuffer::B::format(char const *fmt, ...) {
  va_list ap;
  PTS_va_start(ap, fmt);
  vformat(fmt, ap);
  va_end(ap);
  return *this;
}

/* --- */

GenBuffer::Writable& Files::FILEW::vformat(slen_t n, char const *fmt, va_list ap) {
  /* Dat: no vfnprintf :-( */
  SimBuffer::B buf;
  buf.vformat(n, fmt, ap);
  fwrite(buf(), 1, buf.getLength(), f);
  return*this;
}
GenBuffer::Writable& Files::FILEW::vformat(char const *fmt, va_list ap) {
  vfprintf(f, fmt, ap);
  return*this;
}

/* --- */

/** Must be <=32767. Should be a power of two. */
static const slen_t BUFLEN=4096;

void Encoder::vi_putcc(char c) { vi_write(&c, 1); }
int Decoder::vi_getcc() { char ret; return vi_read(&ret, 1)==1 ? (unsigned char)ret : -1; }
void Encoder::writeFrom(GenBuffer::Writable& out, FILE *f) {
  char *buf=new char[BUFLEN];
  int wr;
  while (1) {
    if ((wr=fread(buf, 1, BUFLEN, f))<1) break;
    out.vi_write(buf, wr);
  }
  delete [] buf;
}
void Encoder::writeFrom(GenBuffer::Writable& out, GenBuffer::Readable& in) {
  char *buf=new char[BUFLEN];
  int wr;
  while (1) {
    if ((wr=in.vi_read(buf, BUFLEN))<1) break;
    out.vi_write(buf, wr);
  }
  delete [] buf;
}

/* --- */

Filter::FILEE::FILEE(char const* filename) {
  if (NULLP==(f=fopen(filename,"wb"))) Error::sev(Error::EERROR) << "Filter::FILEE: error open4write: " << FNQ2(filename,strlen(filename)) << (Error*)0;
  closep=true;
}
void Filter::FILEE::vi_write(char const*buf, slen_t len) {
  if (len==0) close(); else fwrite(buf, 1, len, f);
}
void Filter::FILEE::close() {
  if (closep) { fclose(f); f=(FILE*)NULLP; closep=false; }
}

/* --- */
       
static FILE* fopenErr(char const* filename, char const* errhead) {
  FILE *f;
  if (NULLP==(f=fopen(filename,"rb")))
    Error::sev(Error::EERROR) << errhead << ": error open4read: " << FNQ2(filename,strlen(filename)) << (Error*)0;
  return f;
}

Filter::FILED::FILED(char const* filename) {
  f=fopenErr(filename, "Filter::FileD");
  closep=true;
}
slen_t Filter::FILED::vi_read(char *buf, slen_t len) {
  if (len==0) { close(); return 0; }
  return fread(buf, 1, len, f);
}
void Filter::FILED::close() {
  if (closep) { fclose(f); f=(FILE*)NULLP; closep=false; }
}

/* --- */

Filter::UngetFILED::UngetFILED(char const* filename_, FILE *stdin_f, closeMode_t closeMode_) {
  if (stdin_f!=NULLP && (filename_==NULLP || (filename_[0]=='-' && filename_[1]=='\0'))) {
    f=stdin_f;
    Files::set_binary_mode(fileno(f), true);
    closeMode_&=~CM_unlinkp;
    if (0!=(closeMode_&CM_keep_stdinp)) closeMode_&=~CM_closep;
    filename_=(char const*)NULLP; /* BUGFIX at Tue Jan  4 23:45:31 CET 2005 */
  } else {
    f=fopenErr(filename_, "Filter::UngetFileD");
  }
  if (filename_!=NULLP) strcpy(const_cast<char*>(filename=new char[strlen(filename_)+1]), filename_);
                   else filename=(char*)NULLP;
  /* fprintf(stderr,"filename:%s\n",filename); */
  closeMode=closeMode_;
  ftell_at=0;
  ofs=0;
}
// Filter::UngetFILED::checkFILE() { }
slen_t Filter::UngetFILED::vi_read(char *buf, slen_t len) {
  slen_t delta;
  if (len==0) {
    close(); return 0;
  } else if (unget.getLength()==0) { delta=0; do_read_f:
    delta+=(f==NULLP ? 0 : fread(buf, 1, len, f));
    ftell_at+=delta;
    return delta;
    // delta+=fread(buf, 1, len, f);  // write(1, buf, delta);  // return delta;
  } else if (ofs+len<=unget.getLength()) {
    // printf("\nul=%d ft=%ld\n", unget.getLength(), ftell(f)); fflush(stdout);
    memcpy(buf, unget()+ofs, len); /* Dat: don't remove from unget yet */
    ftell_at+=len;
    // write(1, buf, len);
    if ((ofs+=len)==unget.getLength()) { unget.forgetAll(); ofs=0; }
    return len;
  } else {
    // printf("\num=%d ft=%d\n", unget.getLength(), ftell(f)); fflush(stdout);
    delta=unget.getLength()-ofs; /* BUGFIX at Sat Apr 19 17:15:50 CEST 2003 */
    memcpy(buf, unget()+ofs, delta);
    // write(1, buf, delta);
    unget.forgetAll(); ofs=0;
    buf+=delta;
    len-=delta;
    goto do_read_f;
  }
}
void Filter::UngetFILED::close() {
  if (0!=(closeMode&CM_closep)) { fclose(f); f=(FILE*)NULLP; closeMode&=~CM_closep; }
  unget.forgetAll(); ofs=0;
  if (filename!=NULLP) {
    if (0!=(closeMode&CM_unlinkp)) { remove(filename); }
    delete [] filename;
  }
}

int Filter::UngetFILED::vi_getcc() {
  if (unget.getLength()==0) { do_getc:
    int i=-1;
    if (f!=NULLP && (i=MACRO_GETC(f))!=-1) ftell_at++;
    return i;
  }
  if (ofs==unget.getLength()) { ofs=0; unget.forgetAll(); goto do_getc; }
  ftell_at++;
  return unget[ofs++];
}
int Filter::UngetFILED::getc_seekable() {
  /* Glibc stdio doesn't allow fseek() even if the seek would go into the
   * read buffer, and fseek(f, 0, SEEK_CUR) fails for unseekable files. Fine.
   */
  int c=MACRO_GETC(f);
  return 0==fseek(f, -1L, SEEK_CUR) ? -2 : c;
}
bool Filter::UngetFILED::isSeekable() {
  long pos, posend;
  if (f==NULLP || 0!=(closeMode&CM_seekablep)) return true;
  // return false;
  clearerr(f); /* clears both the EOF and error indicators */
  if (-1L==(pos=ftell(f)) /* sanity checks on ftell() and fseek() */
   || 0!=fseek(f, 0L, SEEK_CUR)
   || pos!=ftell(f)
   || 0!=fseek(f, 0L, SEEK_END)
   || (posend=ftell(f))==0 || posend<pos
   || 0!=fseek(f, 0L, SEEK_SET)
   || 0!=ftell(f)
   || 0!=fseek(f, pos, SEEK_SET)
   || pos!=ftell(f)
   ) return false;
  int c;
  if ((c=getc_seekable())==-2 && pos==ftell(f)) {
    // if (0!=fseek(f, -1L, SEEK_CUR) || pos!=ftell(f))
    //  Error::sev(Error::EERROR) << "Filter::UngetFILED: cannot seek back" << (Error*)0;
    return true;
  }
  unget << (char)c; /* not seekable, must unget the test character just read */
  return false;
}
FILE* Filter::UngetFILED::getFILE(bool seekable_p) {
  FILE *tf;
  if (!unget.isEmpty() || (seekable_p && !isSeekable())) { do_temp:
    /* must create a temporary file */
    SimBuffer::B tmpnam;
    if (filename==NULLP) Error::sev(Error::NOTICE) << "Filter::UngetFILED" << ": using temp for" << " `-' (stdin)" << (Error*)0;
		    else Error::sev(Error::NOTICE) << "Filter::UngetFILED" << ": using temp for" << ": " << FNQ2(filename,strlen(filename)) << (Error*)0;
    if (NULLP==(tf=Files::open_tmpnam(tmpnam, "wb+"))) {
      if (filename==NULLP) Error::sev(Error::EERROR) << "Filter::UngetFILED" << ": cannot open temp file for" << " `-' (stdin)" << (Error*)0;
                      else Error::sev(Error::EERROR) << "Filter::UngetFILED" << ": cannot open temp file for" << ": " << FNQ2(filename,strlen(filename)) << (Error*)0;
    }
    tmpnam.term0();
    /* vvv change filename, so CM_unlinkp can work */
    if (filename!=NULLP) {
      if (0!=(closeMode&CM_unlinkp)) { remove(filename); }
      delete [] filename;
    }
    strcpy(const_cast<char*>(filename=new char[tmpnam.getLength()+1]), tmpnam());
    Files::tmpRemoveCleanup(filename);
    if (unget.getLength()-ofs==fwrite(unget()+ofs, 1, unget.getLength()-ofs, tf)) {
      static const slen_t BUFSIZE=4096; /* BUGFIX at Sat Apr 19 15:43:59 CEST 2003 */
      char *buf=new char[BUFSIZE];
      unsigned got;
      // fprintf(stderr, "ftf=%ld ofs=%d\n", ftell(f), ofs);
      while ((0<(got=fread(buf, 1, BUFSIZE, f)))
             && got==fwrite(buf, 1, got, tf)) {}
      // fprintf(stderr,"got=%d ftell=%d\n", got, ftell(tf));
      delete [] buf;
    }
    unget.forgetAll(); ofs=0;
    fflush(tf); rewind(tf);
    if (ferror(tf) || ferror(f))
      Error::sev(Error::EERROR) << "Filter::UngetFILED" << ": cannot write temp file" << (Error*)0;
    if (0!=(closeMode&CM_closep)) fclose(f);
    closeMode|=CM_closep|CM_unlinkp; /* close and unlink the temporary file */
    /* ^^^ Imp: verify VC++ compilation, +others */
    f=tf;
  } else if (f==NULLP) { /* no real file open */
    #if OS_COTY==COTY_UNIX
      tf=fopen("/dev/null","rb");
    #else
      #if OS_COPTY==COTY_WIN9X || OS_COTY==COTY_WINNT
        tf=fopen("nul","rb");
      #else
        tf=(FILE*)NULLP;
      #endif
    #endif
    if (tf==NULLP) goto do_temp; /* perhaps inside chroot() */
    close();
    closeMode|=CM_closep|CM_unlinkp; /* close and unlink the temporary file */
    f=tf;
  }
  closeMode|=CM_seekablep;
  return f;
}
void Filter::UngetFILED::seek(long abs_ofs) {
  if (abs_ofs==vi_tell()) return;
  (void) getFILE(true); /* ensure seekability */
  if (0!=fseek(f, abs_ofs, SEEK_SET))
    Error::sev(Error::EERROR) << "Filter::UngetFILED" << ": cannot seek" << (Error*)0;
  assert(unget.isEmpty());
  assert(ofs==0);
  ftell_at=abs_ofs;
}
void Filter::UngetFILED::unread(char const *s, slen_t slen) {
  ftell_at-=slen;
  if (slen==0) {
  } else if (slen<=ofs) {
    memcpy(const_cast<char*>(unget()+(ofs-=slen)), s, slen);
  } else {
    slen-=ofs;
    ofs=0;
    if (!unget.isEmpty() || 0!=fseek(f, -slen, SEEK_CUR)) {
      assert(unget.isEmpty()); // !!
      unget.vi_write(s, slen); /* complete garbage unless unget was empty */
      assert(unget.getLength());
    }
  }
  // fprintf(stderr, "%d..\n", unget.getLength());
}
void Filter::UngetFILED::appendLine(GenBuffer::Writable &buf, int delimiter) {
  // fprintf(stderr, "this=%p %d (%p)\n", this, unget.getLength(), unget());
  unget.term0();
  if (delimiter<0) {
    char rbuf[4096];
    slen_t got;
    /* vvv Imp: less memory copying, less stack usage? */
    while (0!=(got=vi_read(rbuf, sizeof(rbuf)))) buf.vi_write(rbuf, got);
  } else if (unget.getLength()==0) { do_getc:
    int i;
    if (f!=NULLP) {
      while ((i=MACRO_GETC(f))>=0 && i!=delimiter) { buf.vi_putcc(i); ftell_at++; }
      if (i>=0) buf.vi_putcc(i);
    }
  } else {
    char const *p=unget()+ofs, *p0=p, *pend=unget.end_();
    assert(ofs<=unget.getLength());
    while (p!=pend && *p!=delimiter) p++;
    ftell_at+=p-p0;
    if (p==pend) { buf.vi_write(p0, p-p0); ofs=0; unget.forgetAll(); goto do_getc; }
    ftell_at++; p++; /* found delimiter in `unget' */
    buf.vi_write(p0, p-p0);
    ofs+=p-p0;
  }
}

/* --- */

Filter::PipeE::PipeE(GenBuffer::Writable &out_, char const*pipe_tmpl, slendiff_t i): tmpname(), out(out_), tmpename() {
  /* <code similarity: Filter::PipeE::PipeE and Filter::PipeD::PipeD> */
  param_assert(pipe_tmpl!=(char const*)NULLP);
  SimBuffer::B *pp;
  char const*s=pipe_tmpl;
  lex: while (s[0]!='\0') { /* Interate throuh the template, substitute temporary filenames */
    if (*s++=='%') switch (*s++) {
     case '\0': case '%':
      redir_cmd << '%';
      break;
     case 'i': /* the optional integer passed in param `i' */
      redir_cmd << i;
      break;
     case '*': /* the optional unsafe string passed in param `i' */
      redir_cmd << (char const*)i;
      break;
     case 'd': case 'D': /* temporary file for encoded data output */
      pp=&tmpname;
     put:
      // if (*pp) Error::sev(Error::EERROR) << "Filter::PipeE" << ": multiple %escape" << (Error*)0;
      /* ^^^ multiple %escape is now a supported feature */
      if (!*pp && !Files::find_tmpnam(*pp)) Error::sev(Error::EERROR) << "Filter::PipeE" << ": tmpnam() failed" << (Error*)0;
      assert(! !*pp); /* pacify VC6.0 */
      // *pp << ext;
      pp->term0();
      if ((unsigned char)(s[-1]-'A')<(unsigned char)('Z'-'A'))
        redir_cmd.appendFnq(*pp, /*preminus:*/ true); /* Capital letter: quote from the shell */
        else redir_cmd << *pp;
      break;
     case 'e': case 'E': /* temporary file for error messages */
      pp=&tmpename;
      goto put;
     case 's': case 'S': /* temporary source file */
      pp=&tmpsname;
      goto put;
     default:
      Error::sev(Error::EERROR) << "Filter::PipeE" << ": invalid %escape in pipe_tmpl" << (Error*)0;
    } else redir_cmd << s[-1];
  }
  #if 0
    if (!tmpname) Error::sev(Error::EERROR) << "Filter::PipeE" << ": no outname (%D) in cmd: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
  #else
    /* Append quoted file redirect to command, if missing */
    if (!tmpname) { s=" >%D"; goto lex; }
  #endif
  #if !HAVE_PTS_POPEN
    if (!tmpsname) { s=" <%S"; goto lex; }
  #endif
  // tmpname="tmp.name";
  redir_cmd.term0();
  if (tmpname) { Files::tmpRemoveCleanup(tmpname()); remove(tmpname()); } /* already term0() */
  /* ^^^ Dat: remove() here introduces a race condition, but helps early error detection */
  if (tmpename) Files::tmpRemoveCleanup(tmpename()); /* already term0() */
  if (tmpsname) Files::tmpRemoveCleanup(tmpsname()); /* already term0() */
  /* </code similarity: Filter::PipeE::PipeE and Filter::PipeD::PipeD> */
  
  // fprintf(stderr, "rc: (%s)\n", redir_cmd());

 #if HAVE_PTS_POPEN
  if (!tmpsname) {
    if (NULLP==(p=popen(redir_cmd(), "w"CFG_PTS_POPEN_B))) Error::sev(Error::EERROR) << "Filter::PipeE" << ": popen() failed: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
    signal(SIGPIPE, SIG_IGN); /* Don't abort process with SIGPIPE signals if child cannot read our data */
  } else {
 #else
  if (1) {
 #endif
   #if !HAVE_system_in_stdlib
    Error::sev(Error::EERROR) << "Filter::PipeE" << ": no system() on this system" << (Error*)0;
   #else
    if (NULLP==(p=fopen(tmpsname(), "wb"))) Error::sev(Error::EERROR) << "Filter::PipeD" << ": fopen(w) failed: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
   #endif
  }
}
void Filter::PipeE::vi_copy(FILE *f) {
  writeFrom(out, f);
  if (ferror(f)) Error::sev(Error::EERROR) << "Filter::PipeE: vi_copy() failed" << (Error*)0;
  fclose(f);
}
void Filter::PipeE::vi_write(char const*buf, slen_t len) {
  assert(p!=NULLP);
  int wr;
  if (len==0) { /* EOF */
    if (tmpsname) {
     #if HAVE_system_in_stdlib
      fclose(p);
      if (0!=(Files::system3(redir_cmd()))) Error::sev(Error::EERROR) << "Filter::PipeE" << ": system() failed: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
      remove(tmpsname());
     #endif /* Dat: else is not required; would be unreachable code. */
    } else {
     #if HAVE_PTS_POPEN
      if (0!=pclose(p)) Error::sev(Error::EERROR) << "Filter::PipeE" << ": pclose() failed; error in external prg" << (Error*)0;
     #endif
    }
    vi_check();
    p=(FILE*)NULLP;
    FILE *f=fopen(tmpname(),"rb");
    if (NULLP==f) Error::sev(Error::EERROR) << "Filter::PipeE" <<": fopen() after pclose() failed: " << tmpname << (Error*)0;
    vi_copy(f);
    // if (ferror(f)) Error::sev(Error::EERROR) << "Filter::Pipe: fread() tmpfile failed" << (Error*)0;
    // fclose(f);
    /* ^^^ interacts badly when Image::load() is called inside vi_copy(),
     * Image::load() calls fclose()
     */
    if (tmpname ) remove(tmpname ());
    if (tmpename) remove(tmpename());
    if (tmpsname) remove(tmpsname());
    out.vi_write(0,0); /* Signal EOF to subsequent filters. */
  } else {
    while (len!=0) {
      wr=fwrite(buf, 1, len>0x4000?0x4000:len, p);
//      assert(!ferror(p));
      if (ferror(p)) {
        vi_check(); /* Give a chance to report a better error message when Broken File. */
        Error::sev(Error::EERROR) << "Filter::PipeE" << ": pipe write failed" << (Error*)0;
      }
      buf+=wr; len-=wr;
    }
  }
}
Filter::PipeE::~PipeE() {}
void Filter::PipeE::vi_check() {}

/* --- */

Filter::PipeD::PipeD(GenBuffer::Readable &in_, char const*pipe_tmpl, slendiff_t i): state(0), in(in_) {
  /* <code similarity: Filter::PipeE::PipeE and Filter::PipeD::PipeD> */
  param_assert(pipe_tmpl!=(char const*)NULLP);
  SimBuffer::B *pp=(SimBuffer::B*)NULLP;
  char const*s=pipe_tmpl;
  lex: while (s[0]!='\0') { /* Interate throuh the template, substitute temporary filenames */
    if (*s++=='%') switch (*s++) {
     case '\0': case '%':
      redir_cmd << '%';
      break;
     case 'i': /* the optional integer passed in param `i' */
      redir_cmd << i;
      break;
     case '*': /* the optional unsafe string passed in param `i' */
      redir_cmd << (char const*)i;
      break;
     case 'd': case 'D': /* temporary file for encoded data output */
      pp=&tmpname;
     put:
      // if (*pp) Error::sev(Error::EERROR) << "Filter::PipeD: multiple %escape" << (Error*)0;
      /* ^^^ multiple %escape is now a supported feature */
      if (!*pp && !Files::find_tmpnam(*pp)) Error::sev(Error::EERROR) << "Filter::PipeD" << ": tmpnam() failed" << (Error*)0;
      assert(*pp);
      pp->term0();
      if ((unsigned char)(s[-1]-'A')<(unsigned char)('Z'-'A'))
        redir_cmd.appendFnq(*pp); /* Capital letter: quote from the shell */
        else redir_cmd << *pp;
      break;
     case 'e': case 'E': /* temporary file for error messages */
      pp=&tmpename;
      goto put;
     case 's': case 'S': /* temporary source file */
      pp=&tmpsname;
      goto put;
     /* OK: implement temporary file for input, option to suppress popen() */
     default:
      Error::sev(Error::EERROR) << "Filter::PipeD: invalid %escape in pipe_tmpl" << (Error*)0;
    } else redir_cmd << s[-1];
  }
  #if 0
    if (!tmpname) Error::sev(Error::EERROR) << "Filter::PipeD" << ": no outname (%D) in cmd: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
  #else
    /* Append quoted file redirect to command, if missing */
    if (!tmpname) { s=" >%D"; goto lex; }
  #endif
  #if !HAVE_PTS_POPEN
    if (!tmpsname) { s=" <%S"; goto lex; }
  #endif
  // tmpname="tmp.name";
  redir_cmd.term0();
  if (tmpname)  Files::tmpRemoveCleanup(tmpname ()); /* already term0() */
  if (tmpename) Files::tmpRemoveCleanup(tmpename()); /* already term0() */
  if (tmpsname) { Files::tmpRemoveCleanup(tmpsname()); remove(tmpsname()); } /* already term0() */
  /* ^^^ Dat: remove() here introduces a race condition, but helps early error detection */
  /* </code similarity: Filter::PipeE::PipeE and Filter::PipeD::PipeD> */
}
slen_t Filter::PipeD::vi_read(char *tobuf, slen_t tolen) {
  assert(!(tolen!=0 && state==2));
  if (state==2) return 0; /* Should really never happen. */
  /* Normal read operation with tolen>0; OR tolen==0 */
  if (state==0) { /* Read the whole stream from `in', write it to `tmpsname' */
   #if HAVE_PTS_POPEN
    if (!tmpsname) {
      if (NULLP==(p=popen(redir_cmd(), "w"CFG_PTS_POPEN_B))) Error::sev(Error::EERROR) << "Filter::PipeD" << ": popen() failed: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
      signal(SIGPIPE, SIG_IGN); /* Don't abort process with SIGPIPE signals if child cannot read our data */
      vi_precopy();
      in.vi_read(0,0);
      if (0!=pclose(p)) Error::sev(Error::EERROR) << "Filter::PipeD" << ": pclose() failed; error in external prg" << (Error*)0;
    } else {
   #else
    if (1) {
   #endif
     #if !HAVE_system_in_stdlib
      Error::sev(Error::EERROR) << "Filter::PipeD" << ": no system() on this system" << (Error*)0;
     #else
      if (NULLP==(p=fopen(tmpsname(), "wb"))) Error::sev(Error::EERROR) << "Filter::PipeD" << ": fopen(w) failed: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
      vi_precopy();
      in.vi_read(0,0);
      fclose(p);
      if (0!=(Files::system3(redir_cmd()))) Error::sev(Error::EERROR) << "Filter::PipeD" << ": system() failed: " << (SimBuffer::B().appendDumpC(redir_cmd)) << (Error*)0;
      remove(tmpsname());
     #endif
    }
    vi_check();
    if (NULLP==(p=fopen(tmpname(),"rb"))) Error::sev(Error::EERROR) << "Filter::PipeD" << ": fopen() after pclose() failed: " << tmpname << (Error*)0;
    state=1;
  } /* IF state==0 */
  assert(state==1);
  if (tolen==0 || 0==(tolen=fread(tobuf, 1, tolen, p))) do_close();
  // putchar('{'); fwrite(tobuf, 1, tolen, stdout); putchar('}');
  return tolen;
}
void Filter::PipeD::do_close() {
  fclose(p); p=(FILE*)NULLP;
  if (tmpname ) remove(tmpname ());
  if (tmpename) remove(tmpename());
  if (tmpsname) remove(tmpsname());
  state=2;
}
void Filter::PipeD::vi_precopy() {
  char *buf0=new char[BUFLEN], *buf;
  slen_t len, wr;
  while (0!=(len=in.vi_read(buf0, BUFLEN))) {
    // printf("[%s]\n", buf0);
    for (buf=buf0; len!=0; buf+=wr, len-=wr) {
      wr=fwrite(buf, 1, len>0x4000?0x4000:len, p);
      if (ferror(p)) {
        vi_check(); /* Give a chance to report a better error message when Broken File. */
        Error::sev(Error::EERROR) << "Filter::PipeD" << ": pipe write failed" << (Error*)0;
      }
    }
  }
  delete [] buf0;
}
int Filter::PipeD::vi_getcc() {
  char ret; int i;
  // fprintf(stderr,"state=%u\n", state);
  switch (state) {
   case 0: return vi_read(&ret, 1)==1 ? (unsigned char)ret : -1;
   case 1: if (-1==(i=MACRO_GETC(p))) do_close(); return i;
   /* case: 2: fall-through */
  }
  return -1;
}
void Filter::PipeD::vi_check() {}
Filter::PipeD::~PipeD() { if (state!=2) vi_read(0,0); }

Filter::BufR::BufR(GenBuffer const& buf_): bufp(&buf_) {
  buf_.first_sub(sub);
}
int Filter::BufR::vi_getcc() {
  if (bufp==(GenBuffer const*)NULLP) return -1; /* cast: pacify VC6.0 */
  if (sub.len==0) {
    bufp->next_sub(sub);
    if (sub.len==0) { bufp=(GenBuffer const*)NULLP; return -1; }
  }
  sub.len--; return *sub.beg++;
}
slen_t Filter::BufR::vi_read(char *to_buf, slen_t max) {
  if (max==0 || bufp==(GenBuffer const*)NULLP) return 0;
  if (sub.len==0) {
    bufp->next_sub(sub);
    if (sub.len==0) { bufp=(GenBuffer const*)NULLP; return 0; }
  }
  if (max<sub.len) {
    memcpy(to_buf, sub.beg, max);
    sub.len-=max; sub.beg+=max;
    return max;
  }
  max=sub.len; sub.len=0;
  memcpy(to_buf, sub.beg, max);
  return max;
}
void Filter::BufR::vi_rewind() { bufp->first_sub(sub); }

Filter::FlatD::FlatD(char const* s_, slen_t slen_): s(s_), sbeg(s_), slen(slen_) {}
Filter::FlatD::FlatD(char const* s_): s(s_), sbeg(s_), slen(strlen(s_)) {}
void Filter::FlatD::vi_rewind() { s=sbeg; }
int Filter::FlatD::vi_getcc() {
  if (slen==0) return -1;
  slen--; return *(unsigned char const*)s++;
}
slen_t Filter::FlatD::vi_read(char *to_buf, slen_t max) {
  if (max>slen) max=slen;
  memcpy(to_buf, s, max);
  s+=max; slen-=max;
  return max;
}

/* --- */


#if HAVE_lstat_in_sys_stat
#  define PTS_lstat lstat
#else
#  define PTS_lstat stat
#endif

/** @param fname must start with '/' (dir separator)
 * @return true if file successfully created
 */
FILE *Files::try_dir(SimBuffer::B &dir, SimBuffer::B const&fname, char const*s1, char const*s2, char const*open_mode) {
  if (dir.isEmpty() && s1==(char const*)NULLP) return (FILE*)NULLP;
  SimBuffer::B full(s1!=(char const*)NULLP?s1:dir(),
                    s1!=(char const*)NULLP?strlen(s1):dir.getLength(),
		    s2!=(char const*)NULLP?s2:"",
		    s2!=(char const*)NULLP?strlen(s2):0, fname(), fname.getLength());
  full.term0();
  struct stat st;
  FILE *f;
  /* Imp: avoid race conditions with other processes pretending to be us... */
  if (-1!=PTS_lstat(full(), &st)
   || (0==(f=fopen(full(), open_mode)))
   || ferror(f)
     ) return (FILE*)NULLP;
  dir=full;
  return f;
}

#if OS_COTY==COTY_WIN9X || OS_COTY==COTY_WINNT
#  define DIR_SEP "\\"
#else
#  define DIR_SEP "/"
#endif

FILE *Files::open_tmpnam(SimBuffer::B &dir, char const*open_mode, char const*extension) {
  /* Imp: verify / on Win32... */
  /* Imp: ensure uniqueness on NFS */
  /* Imp: short file names */
  static unsigned PTS_INT32_T counter=0;
  assert(Error::tmpargv0!=(char const*)NULLP);
  SimBuffer::B fname(DIR_SEP "tmp_", 5, Error::tmpargv0,strlen(Error::tmpargv0));
  /* ^^^ Dat: we need DIR_SEP here, because the name of the tmp file may be
   * passed to Win32 COMMAND.COM, which interprets "/" as a switch
   */
  long pid=getpid();
  if (pid<0 && pid>-(1<<24)) pid=-pid;
  fname << '_' << pid << '_' << counter++;
  if (extension) fname << extension;
  fname.term0();
  FILE *f=(FILE*)NULLP;
  // char const* open_mode=binary_p ? "wb" : "w"; /* Dat: "bw" is bad */
  (void)( ((FILE*)NULLP!=(f=try_dir(dir, fname, 0, 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, PTS_CFG_P_TMPDIR, 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, getenv("TMPDIR"), 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, getenv("TMP"), 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, getenv("TEMP"), 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "/tmp", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, getenv("WINBOOTDIR"), "//temp", open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, getenv("WINDIR"), "//temp", open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "c:/temp", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "c:/windows/temp", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "c:/winnt/temp", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "c:/tmp", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, ".", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "..", 0, open_mode))) ||
          ((FILE*)NULLP!=(f=try_dir(dir, fname, "../..", 0, open_mode))) );
  return f;
}

bool Files::find_tmpnam(SimBuffer::B &dir)  {
  FILE *f=open_tmpnam(dir);
  if (f!=NULL) { fclose(f); return true; }
  return false;
}

bool Files::tmpRemove=true;

static int cleanup_remove(Error::Cleanup *cleanup) {
  if (Files::tmpRemove) return Files::removeIf(cleanup->getBuf());
  Error::sev(Error::WARNING) << "keeping tmp file: " << cleanup->getBuf() << (Error*)0;
  return 0;
}

void Files::tmpRemoveCleanup(char const* filename) {
  Error::newCleanup(cleanup_remove, 0, filename);
}

static int cleanup_remove_cond(Error::Cleanup *cleanup) {
  if (*(FILE**)cleanup->data!=NULLP) {
    fclose(*(FILE**)cleanup->data);
    if (Files::tmpRemove) return Files::removeIf(cleanup->getBuf());
    Error::sev(Error::WARNING) << "keeping tmp2 file: " << cleanup->getBuf() << (Error*)0;
  }
  return 0;
}

void Files::tmpRemoveCleanup(char const* filename, FILE**p) {
  param_assert(p!=NULLP);
  Error::newCleanup(cleanup_remove_cond, (void*)p, filename);
}

int Files::removeIf(char const* filename) {
  if (0==remove(filename) || errno==ENOENT) return 0;
  return 1;
}

slen_t Files::statSize(char const* filename) {
  struct stat st;
  if (-1==PTS_lstat(filename, &st)) return (slen_t)-1;
  return st.st_size;
}

/* Tue Jul  2 10:57:21 CEST 2002 */
char const* Files::only_fext(char const*filename) {
  char const *ret;
  if (OS_COTY==COTY_WINNT || OS_COTY==COTY_WIN9X) {
    if ((USGE('z'-'a',filename[0]-'a') || USGE('Z'-'A',filename[0]-'A'))
     && filename[1]==':'
       ) filename+=2; /* strip drive letter */
    ret=filename;
    while (*filename!='\0') {
      if (*filename=='/' || *filename=='\\') ret=++filename;
                                        else filename++;
    }
  } else { /* Everything else is treated as UNIX */
    ret=filename;
    while (filename[0]!='\0') if (*filename++=='/') ret=filename;
  }
  return ret;
}

#if HAVE_DOS_BINARY
void Files::set_binary_mode(int fd, bool binary) {
  /* Wed Dec 11 18:17:30 CET 2002 */
  setmode(fd, binary ? O_BINARY : O_TEXT);
}
#endif

int Files::system3(char const *commands) {
  #if OS_COTY==COTY_WIN9X || OS_COTY==COTY_WINNT
    char const *p;
    p=commands; while (*p!='\0' && *p!='\n') p++;
    if (*p=='\0') return system(commands); /* no newline -- simple run */
    SimBuffer::B tmpnam;
    FILE *f=Files::open_tmpnam(tmpnam, /*binary_p:*/false, ".bat");
    tmpnam.term0();
    Files::tmpRemoveCleanup(tmpnam());
    fprintf(f, "@echo off\n%s\n", commands);
    if (ferror(f)) Error::sev(Error::EERROR) << "system3: write to tmp .bat file: " << tmpnam << (Error*)NULLP;
    fclose(f);
    // printf("(%s)\n", tmpnam()); system("bash");
    // int ret=system(("sh "+tmpnam)());
    int ret=system(tmpnam());
    remove(tmpnam());
    return ret;
  #else
    return system(commands);
  #endif
}

/* __END__ */
