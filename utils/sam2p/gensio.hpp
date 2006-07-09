/* gensio.hpp -- generic char buffer and I/O facilities
 * by pts@fazekas.hu at Tue Feb 26 13:30:02 CET 2002
 */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef GENSIO_HPP
#define GENSIO_HPP 1

#include "config2.h"
#include <stdarg.h> /* va_list */
#include "gensi.hpp"
#include <stdio.h>

/* Naming conventions: *Encode is a specific, well-known PostScript or PDF
 * encoding filter documented by Adobe, *Encoder is something more general.
 */

/** Writing 0 bytes for vi_write is interpreted as a close() operation.
 * Constructor must not write anything to the underlying (lower-level)
 * Encoder. Implementors must redefine vi_write(). Encoders may be stacked
 * atop of each other.
 */
class Encoder: public GenBuffer::Writable { public:
  /** Default: calls vi_write. */
  virtual void vi_putcc(char c);
  /** vi_write() must be called with positive slen_t for normal writing, and
   * vi_write(?,0); must be called to signal EOF. After that, it is prohibited
   * to call vi_write() either way.
   */
  virtual void vi_write(char const*buf, slen_t len) =0;
  /** Copies all data (till EOF) from stream `f' to (this). */
  static void writeFrom(GenBuffer::Writable& out, FILE *f);
  /** Copies all data (till EOF) from stream `f' to (this). */
  static void writeFrom(GenBuffer::Writable& out, GenBuffer::Readable &in);
}; /* class Encoder */

/** Implementors must redefine vi_read(), and they may redefine vi_getcc().
 * A .vi_read(?,0) has special meaning in a Decoder (but not in a normal
 * Readable).
 */
class Decoder: public GenBuffer::Readable { public:
  /** Calls vi_read(&ret,1). Note that this is the inverse of
   * GenBuffer::Readable, because there vi_read() calls vi_getcc(). Decoders
   * may be stacked atop of each other.
   */
  virtual int vi_getcc();
  /** vi_read() must be called with positive slen_t for normal reading, and
   * vi_read(?,0); must be called to signal that the caller would not read from
   * the Decoder again, so the decoder is allowed to release the resources
   * used. After that, it is prohibited to call vi_read() either way.
   */
  virtual slen_t vi_read(char *to_buf, slen_t max) =0;
}; /* class Decoder */

class DecoderTeller: public Decoder { public:
  /** Equivalent of ftell() */
  virtual long vi_tell() const=0;
};

class Filter { public:
  /** Starts a child process (with popen(pipe_tmpl,"wb")), pipes data to it,
   * writes output to a temporary file. After _everything_ is written, the
   * temporary file is read and fed to out_.
   */
  class PipeE: public Encoder {
   public:
    PipeE(GenBuffer::Writable &out_, char const*pipe_tmpl, slendiff_t i=0);
    virtual ~PipeE();
    virtual void vi_write(char const*buf, slen_t len);
   protected:
    FILE *p;
    SimBuffer::B tmpname;
    GenBuffer::Writable &out;
    SimBuffer::B tmpename, redir_cmd;
    /** Temporary source file name `%S', forces system() instead of popen() */
    SimBuffer::B tmpsname;
    /** vi_check() is called by this->vi_write(...) (and also when Broken Pipe)
     * to check whether the encoding succeeded. vi_check() should raise an Error
     * if it detects failure or simply return if it detects success. Default
     * implementation: empty body which simly returns.
     */
    virtual void vi_check();
   protected:
    /** Copies the rest of (seekable) file `f' to `out' (subsequent filters).
     * `f' is initially positioned at the beginning. Must call fclose().
     * vi_copy() should raise Error::...s. The default implementation
     * just copies the data bytes verbatim. `out.vi_write(0,0);' will be called
     * by vi_write().
     */
    virtual void vi_copy(FILE *f);
  };
  
  /** Gobbles all data written to it, just like /dev/null */
  class NullE: public Encoder {
    inline virtual void vi_putcc(char) {}
    inline virtual void vi_write(char const*, slen_t) {}
  };
  
  class VerbatimE: public Encoder {
   public:
    inline VerbatimE(GenBuffer::Writable& out_): out(&out_) {}
    inline virtual void vi_write(char const*buf, slen_t len) { out->vi_write(buf,len); }
    inline void setOut(GenBuffer::Writable& out_) { out=&out_; }
    inline GenBuffer::Writable& getOut() const { return *out; }
   protected:
    GenBuffer::Writable* out;
  };

  class VerbatimCountE: public Encoder {
   public:
    inline VerbatimCountE(GenBuffer::Writable& out_): out(&out_), count(0) {}
    inline virtual void vi_write(char const*buf, slen_t len) { out->vi_write(buf,len); count+=len; }
    inline void setOut(GenBuffer::Writable& out_) { out=&out_; }
    inline GenBuffer::Writable& getOut() const { return *out; }
    inline slen_t getCount() const { return count; }
   protected:
    GenBuffer::Writable* out;
    /** Number of bytes already written */
    slen_t count;
  };

  class FILEE: public Encoder {
   public:
    inline FILEE(FILE *f_,bool closep_): f(f_), closep(closep_) {}
    FILEE(char const* filename);
    inline virtual void vi_putcc(char c) { MACRO_PUTC(c,f); }
    virtual void vi_write(char const*buf, slen_t len);
    void close();
   protected:
    FILE *f;
    bool closep;
  };

  class PipeD: public Decoder { public:
    PipeD(GenBuffer::Readable &in_, char const*pipe_tmpl, slendiff_t i=0);
    virtual ~PipeD();
    /** Equivalent to vi_read(&ret,1). */
    virtual int vi_getcc();
    /** Upon the first non-zero call, opens `p', calls vi_precopy() and
     * closes `in'. Upon all non-zero calls,
     * reads the temporary file with fread(). Upon vi_read(?,0), removes the
     * the temporary files.
     */
    virtual slen_t vi_read(char *to_buf, slen_t max);
   protected:
    /** 0: never-read (initial state), 1: vi_precopy() already called, 2: EOF reached, `in' closed */
    int state;
    /** opened with popen(?,"w") in state:0->1, then with fopen(?,"rb") in state==1 */
    FILE *p;
    /** Temporary destination file name `%D' */
    SimBuffer::B tmpname;
    GenBuffer::Readable &in;
    SimBuffer::B tmpename, redir_cmd;
    /** Temporary source file name `%S', forces system() instead of popen() */
    SimBuffer::B tmpsname;
    
    /** Copies the whole `in' to writable pipe `p'. `p' will be closed by the
     * caller; in.read(0,0) will be called by the caller.
     * vi_precopy() should raise Error::...s. The default implementation
     * just copies the data bytes verbatim.
     */
    virtual void vi_precopy();
    /** vi_check() is called by this->vi_write(...) (and also when Broken Pipe)
     * to check whether the encoding succeeded. vi_check() should raise an Error
     * if it detects failure or simply return if it detects success. Default
     * implementation: empty body which simly returns.
     */
    virtual void vi_check();
   private:
    void do_close();
  };

  class VerbatimD: public Decoder {
   public:
    inline VerbatimD(GenBuffer::Readable& in_): in(in_) {}
    /** Works fine even if len==0. */
    inline virtual slen_t vi_read(char *buf, slen_t len) { return in.vi_read(buf,len); }
   protected:
    GenBuffer::Readable& in;
  };

  class FILED: public DecoderTeller {
   public:
    inline FILED(FILE *f_,bool closep_): f(f_), closep(closep_) {}
    FILED(char const* filename);
    inline virtual ~FILED() { close(); }
    inline virtual int vi_getcc() { return MACRO_GETC(f); }
    virtual slen_t vi_read(char *buf, slen_t len);
    void close();
    inline virtual long vi_tell() const { return ftell(f); }
   protected:
    FILE *f;
    bool closep;
  };

  /**
   * Sat Apr 19 12:05:49 CEST 2003
   * Always opens the file in binary mode.
   * First reads from the unget buffer, then reads from a FILE*. A typical
   * usage is:
   *   { Filter::UngetFILED f("in.txt"); // fopen("in.txt","rb"), true);
   *     f.getUnget() << "Prepend this in front of first line.\n";
   *     int c;
   *     while ((c=f.vi_getcc()!=-1)) putchar(c);
   *   }
   */
  class UngetFILED: public DecoderTeller {
   public:
    GenBuffer::Writable& getUnget() { return unget; }
    BEGIN_STATIC_ENUM(unsigned char, closeMode_t)
      CM_closep=1,
      CM_unlinkp=2,
      CM_keep_stdinp=4, /* CM_unlinkp, but keep STDIN open */
      CM_seekablep=8, /* it is sure that this->f is seekable */
      CM_MAX=4
    END_STATIC_ENUM()
    inline UngetFILED(FILE *f_, closeMode_t closep_): f(f_), closeMode(closep_), ftell_at(0), ofs(0) {}
    UngetFILED(char const* filename_, FILE* stdin_f=(FILE*)NULLP, closeMode_t closeMode_=CM_closep);
    inline virtual ~UngetFILED() { close(); }
    virtual int vi_getcc();
    virtual slen_t vi_read(char *buf, slen_t len);
    /** Appends a line to buf, including delimiter unless EOF.
     * @param delimiter: a char or negative to read to EOF
     */ 
    void appendLine(GenBuffer::Writable &buf, int delimiter='\n');
    void close();
    // void checkFILE();
    inline virtual long vi_tell() const { return ftell_at; } // return ftell_add+ofs+(f!=NULLP ? ftell(f) : 0); }
    /**
     * @return equivalent to getc(f) if this->f is not seekable, but returns
     *   -2 if this->f is seekable.
     */
    int getc_seekable();
    /**
     * Actively tests whether FILE* is seekable. Doesn't work for regular files
     * of 0 bytes.
     */
    bool isSeekable();
    /** Reading (this) and reading the returned FILE* will be equivalent
     * (unless this->getUnget() is used later).
     * Creates a temporary file if this->unget is not empty.
     * @param seekable_p if true, the returned FILE* must be seekable. Possibly
     *   creates a temporary file.
     */
    FILE* getFILE(bool seekable_p);
    /** Implies a getFILE() */
    void seek(long abs_ofs);
    /** Tries to do an fseek(f, -slen, SEEK_CUR). On failure, appends to unget.
     * The user should only unread() data obtained from vi_read().
     */
    void unread(char const *s, slen_t slen);
    inline char const* getFilename() const { return filename; }
    inline char const* getFilenameDefault(char const *def) const { return filename==NULLP ? def : filename; }
    inline bool hadError() const { return f!=NULLP && ferror(f); }
   protected:
    FILE *f;
    unsigned char closeMode;
    slen_t ftell_at;
    slen_t ofs;
    SimBuffer::B unget;
    char const* filename;
  };

  /** Reads from a memory of a GenBuffer via first_sub and next_sub. The
   * GenBuffer should not be changed during the read. The GenBuffer isn't
   * delete()d by (this)
   */
  class BufR: public GenBuffer::Readable {
   public:
    BufR(GenBuffer const& buf_);
    virtual int vi_getcc();
    virtual slen_t vi_read(char *to_buf, slen_t max);
    virtual void vi_rewind();
   protected:
    GenBuffer const* bufp;
    GenBuffer::Sub sub;
  };

  /** Reads from a consecutive memory area, which won't be
   * delete()d by (this)
   */
  class FlatD: public DecoderTeller /*GenBuffer::Readable*/ {
   public:
    FlatD(char const* s_, slen_t slen_);
    FlatD(char const* s_);
    virtual int vi_getcc();
    virtual slen_t vi_read(char *to_buf, slen_t max);
    virtual void vi_rewind();
    inline int getcc() { return slen!=0 ? (slen--, *(unsigned char const*)s++) : -1; }
    inline virtual long vi_tell() const { return s-sbeg; }
    inline long tell() const { return s-sbeg; }
   protected:
    char const *s, *sbeg;
    slen_t slen;
  };
}; /* class Filter */

class Files {
 public:
  /** Formerly `class WritableFILE' */
  class FILEW: public GenBuffer::Writable {
   public:
    inline FILEW(FILE *f_): f(f_) {}
    inline virtual void vi_putcc(char c) { MACRO_PUTC(c,f); }
    inline virtual void vi_write(char const*buf, slen_t len) { fwrite(buf, 1, len, f); }
    virtual GenBuffer::Writable& vformat(slen_t n, char const *fmt, va_list ap);
    /** appends; uses SimBuffer::B as temp */
    virtual GenBuffer::Writable& vformat(char const *fmt, va_list ap);
    inline void close() { fclose(f); }
    inline void setF(FILE *f_) { f=f_; }
   private:
    FILE *f;
  };

  /** Formerly `class ReadableFILE'. Doesn't close the its FILE* automatically. */
  class FILER: public GenBuffer::Readable {
   public:
    inline FILER(FILE *f_): f(f_) {}
    inline virtual int vi_getcc() { return MACRO_GETC(f); }
    inline virtual slen_t vi_read(char *to_buf, slen_t max) { return fread(to_buf, 1, max, f); }
    inline void close() { fclose(f); }
    inline virtual void vi_rewind() { rewind(f); }
   private:
    FILE *f;
  };

  /** @return the beginning of the last substring in param filename that does
   * not contain '/' (dir separator)
   */
  static char const* only_fext(char const*filename);

  /** true iff temporary files should be removed at program finish end */
  static bool tmpRemove;
  /** @param fname must start with '/' (dir separator)
   * @return true if file successfully created
   */
  static FILE *try_dir(SimBuffer::B &dir, SimBuffer::B const&fname, char const*s1, char const*s2, char const*open_mode="wb");
  /* The file will be opened for writing only. It won't be registered for
   * for automatic removal.
   * @param dir `dir' is empty: appends a unique filename for a temporary
   *        file. Otherwise: returns a unique filename in the specified directory.
   *        Creates the new file with 0 size.
   * @param extension NULLP or a string specifying the extension of the file
   *        to create (should beginn with ".")
   * @param open_mode "w", "wb" or "wb+"
   * @return FILE* opened for writing for success, NULLP on failure
   * --return true on success, false on failure
   */
  static FILE *open_tmpnam(SimBuffer::B &dir, char const*open_mode="wb", char const*extension=(char const*)NULLP);
  // static FILE *open_tmpnam(SimBuffer::B &dir, bool binary_p=true, char const*extension=(char const*)NULLP);
  static bool find_tmpnam(SimBuffer::B &dir);
  /** Calls lstat().
   * @return (slen_t)-1 on error, the size otherwise
   */
  static slen_t statSize(char const* filename);
  /** Ensures that file will be removed (if possible...) when the process
   * terminates. Copies the string filename.
   */
  static void tmpRemoveCleanup(char const* filename);
  /** Ensures that file will be removed (if possible...) when the process
   * terminates. Copies the string filename. The file will be removed iff
   * (*p!=NULLP) when the cleanup handler runs. The cleanup handler fclose()s
   * the file before removing it.
   */
  static void tmpRemoveCleanup(char const* filename, FILE**p);
  /** Removes the file/entity if exists.
   * @return 0 on success (file hadn't existed, a directory component
   *   hadn't existed, or the file has been successfully removed),
   *   1 otherwise
   */
  static int removeIf(char const *filename);

  /** Sat Sep  7 20:58:54 CEST 2002 */
  static void doSignalCleanup();

  /** Set the specified file descriptor to binary mode. Useful for stdin (0),
   * stdout (1), stderr (2) on Win32 and DOS systems. On these systems, this
   * call has the effect of `fopen(..., "rb");' On UNIX, file
   * descriptors are always binary. 
   */
  #if HAVE_DOS_BINARY
    static void set_binary_mode(int fd, bool binary);
  #else
    static inline void set_binary_mode(int,bool) {}
  #endif

  /** Like the system(3) call, but it is able to run a string containing
   * multiple lines of commands. On Win32, it creates a batch file if
   * necessary.
   */
  static int system3(char const *commands);
};

#endif
