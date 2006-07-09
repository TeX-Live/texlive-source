/* gensi.hpp -- generic char buffer and I/O facilities
 * by pts@fazekas.hu at Sun Feb 24 15:56:02 CET 2002
 */
/* Imp: get rid of gensio.o (Buffer::Writable::printf requires B...) */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef GENSI_HPP
#define GENSI_HPP 1

#include "config2.h"
#include <stdarg.h> /* va_list */

#if 'a'!=97 || '!'!=33
#error You need an ASCII system to compile this.
#endif

/** A very generic, abstract char (pure 8-bit octet) buffer class. This class
 * doesn't have any attributes.
 */
class GenBuffer {
 public:
  static inline unsigned hexc2n(char c) {
    return ((unsigned char)(c-'0')<=(unsigned char)('9'-'0')) ? c-'0' 
         : ((unsigned char)(c-'A')<=(unsigned char)('F'-'A')) ? c-'A'+10
         : ((unsigned char)(c-'a')<=(unsigned char)('f'-'a')) ? c-'a'+10
         : 16;
  }
  /** English ASCII letters [a-zA-Z] */
  static void tolower_memcpy(char *dst, char const*s, slen_t slen);
  /** English ASCII letters [a-zA-Z] */
  static void toupper_memcpy(char *dst, char const*s, slen_t slen);
  /** All letters treated as lower case. @return -1, 0 or 1 */
  static int nocase_memcmp(char const*a, char const *s, slen_t slen);
  static int nocase_strcmp(char const*a, char const *b);
  /** All letters treated as lower case. @return -1, 0 or 1 */
  static bool nocase_strbegins(char const*a, char const *with);
  static bool strbegins(char const*a, char const *with);
  static bool parseBool(char const *s, slen_t slen);

  /* Imp: convert usage of each_sub to first_sub+next_sub */
  typedef void (*block_sub_t)(char const*beg, slen_t len, void *data);
  typedef void (*block_char_t)(char c, void *data);
  struct Sub {
    char const*beg;
    slen_t len;
    void *data, *data2;
  };
  /** Writable. */
  struct SubW {
    char *beg;
    slen_t len;
    void *data, *data2;
  };
  virtual slen_t getLength() const =0;
  /** Iterates through each subrange of (this), and calls param `block'
   * for each range. Must call block with positive `len's, and -- as a final
   * call -- with len==0.
   * @param data arbitrary data, to be passed to param `block'
   */
  virtual void each_sub(block_sub_t block, void *data) const =0;
  /** Iterates through each character of (this), and calls param `block'
   * for each char. Implemented by calling (this).each_sub
   */
  /** Produces sub.len==0 if empty. */
  virtual void first_sub(Sub &sub) const =0;
  /** Produces sub.len==0 if no more subs.
   * @param sub fields `beg' and `len' are assumed to contain garbage (!)
   *            before invocation. Only `data' and `data2' can be relied upon.
   */
  virtual void next_sub(Sub &sub) const =0;
  inline void each_char(block_char_t block, void *data) const {
    void *t[2]= { (void*)data, (void*)block };
    each_sub(iter_char_sub, (void*)t);
  }
  inline bool isEmpty() const { return getLength()==0; }
  /** @return true iff not empty */
  inline virtual operator void*() const { return (void*)(getLength()!=0); }
  /** @return true iff not empty; pacify VC6.0 */
  // inline virtual operator bool() const { return (void*)(getLength()!=0); }
  // inline virtual operator bool() const { return getLength()!=0; }
  /** @return true iff empty */
  inline virtual bool operator!() const { return getLength()==0; }
  /** @return getLength() */
  virtual slen_t copyRange(char *to, slen_t cfrom, slen_t clen) const;
  /** @return true on conversion error */
  bool toBool(bool &dst);
  /** @return true on conversion error (overflow etc.) */
  bool toInteger(unsigned long &dst);
  /** @return true on conversion error (overflow etc.) */
  bool toInteger(signed long &dst);
  #if HAVE_LONG_LONG && NEED_LONG_LONG
    /** @return true on conversion error (overflow etc.) */
    bool toInteger(unsigned PTS_CFG_LONGEST &dst);
    /** @return true on conversion error (overflow etc.) */
    bool toInteger(signed PTS_CFG_LONGEST &dst);
  #endif
  inline bool toInteger(unsigned short &i) { unsigned long l; bool b=toInteger(l); i=l; return b; }
  inline bool toInteger(  signed short &i) {   signed long l; bool b=toInteger(l); i=l; return b; }
  inline bool toInteger(unsigned int   &i) { unsigned long l; bool b=toInteger(l); i=l; return b; }
  inline bool toInteger(  signed int   &i) {   signed long l; bool b=toInteger(l); i=l; return b; }
  /** Allocates a fresh new, null-terminated string. @return true */
  bool toCString(char *&dst);
  /** @return negative iff (this)<s2, positive iff (this)>s2 */
  virtual int cmp(GenBuffer const& s2) const;
  /** @return negative iff (this)<s2, positive iff (this)>s2 */
  virtual int cmp(char const* s2, slen_t len) const;
  /** Null-terminated, calls cmp(char const* s2, slen_t len). */
  int cmp(char const* s2) const;
  friend bool operator==(const GenBuffer& s1, const GenBuffer& s2);
  friend bool operator==(const char *s1, const GenBuffer& s2);
  friend bool operator==(const GenBuffer& s1, const char *s2);
  /* vvv G++ 2.91 doesn't autodetect these 15 operators :-( */
  friend bool operator!=(const GenBuffer& s1, const GenBuffer& s2);
  friend bool operator!=(const char *s1, const GenBuffer& s2);
  friend bool operator!=(const GenBuffer& s1, const char *s2);
  friend bool operator<=(const GenBuffer& s1, const GenBuffer& s2);
  friend bool operator<=(const char *s1, const GenBuffer& s2);
  friend bool operator<=(const GenBuffer& s1, const char *s2);
  friend bool operator<(const GenBuffer& s1, const GenBuffer& s2);
  friend bool operator<(const char *s1, const GenBuffer& s2);
  friend bool operator<(const GenBuffer& s1, const char *s2);
  friend bool operator>=(const GenBuffer& s1, const GenBuffer& s2);
  friend bool operator>=(const char *s1, const GenBuffer& s2);
  friend bool operator>=(const GenBuffer& s1, const char *s2);
  friend bool operator>(const GenBuffer& s1, const GenBuffer& s2);
  friend bool operator>(const char *s1, const GenBuffer& s2);
  friend bool operator>(const GenBuffer& s1, const char *s2);

  /** A very generic, abstract char (pure 8-bit octet) output (encoding
   * stream, filter) class. Writing always succeeds, all data gets written.
   * No attributes.
   */
  class Writable {
   public:
    virtual void vi_write(char const*, slen_t) =0;
    virtual void vi_putcc(char) =0;
    inline Writable &operator <<(GenBuffer const& b) {
      b.each_sub(iter_write_sub, this);
      return*this;
    }
    inline Writable &operator <<(char c) { vi_putcc(c); return*this; }
    /** Outputs a null-terminated, C string. Not `inline' because the use
     * of strlen().
     */
    Writable& operator <<(char const*);
    Writable& operator <<(void const*);
    inline Writable& operator <<(  signed short n) { write_num((signed long)n); return*this; }
    inline Writable& operator <<(  signed int   n) { write_num((signed long)n); return*this; }
    inline Writable& operator <<(  signed long  n) { write_num(n); return*this; }
    inline Writable& operator <<(unsigned short n) { write_num((unsigned long)n); return*this; }
    inline Writable& operator <<(unsigned int   n) { write_num((unsigned long)n); return*this; }
    inline Writable& operator <<(unsigned long  n) { write_num(n); return*this; }
    #if HAVE_LONG_LONG && NEED_LONG_LONG
      inline Writable &operator <<(signed long long n) { write_num(n); return*this; }
      inline Writable &operator <<(unsigned long long n) { write_num(n); return*this; }
    #endif
    inline Writable& operator <<(bool b) {
      if (b) vi_write("true", 4);
        else vi_write("false", 5);
      return*this;
    }

    /** appends at most `n' chars; uses SimBuffer::B::vformat as temp */
    virtual Writable& vformat(slen_t n, char const *fmt, va_list ap);
    /** appends; uses SimBuffer::B::vformat as temp */
    virtual Writable& vformat(char const *fmt, va_list ap);
    /** appends; calls vformat */
    Writable& format(slen_t n, char const *fmt, ...);
    /** appends; calls vformat */
    Writable& format(char const *fmt, ...);

    void write_num(signed long n);
    void write_num(unsigned long n);
    #if HAVE_LONG_LONG && NEED_LONG_LONG
      void write_num(signed PTS_CFG_LONGEST n);
      void write_num(unsigned PTS_CFG_LONGEST n);
    #endif
    /** @param zdigits specifies the exact number of digits to be appended.
     * Zeroes are prepended if necessary.
     */
    void write_num(unsigned long n, unsigned zdigits);
    #if HAVE_LONG_LONG && NEED_LONG_LONG
      void write_num(unsigned PTS_CFG_LONGEST n, unsigned zdigits);
    #endif
   protected:
    static void iter_write_sub(char const*beg, slen_t len, void *data);
  };
  
  /** Interface for a stream of characters. Similar to ISimplyiChar in
   * CHsplit. No attributes.
   */
  class Readable {
  public:
    /** @return -1 on EOF/error, a char (0..255) otherwise.
     */
    virtual int vi_getcc() =0;
    /** Does a single blocking read. Default functionality: calls vi_getcc()
     * repeatedly.
     * @return 0 on EOF/error, positive otherwise: number of characters
     * successfully read.
     */
    virtual slen_t vi_read(char *to_buf, slen_t max);
    /** Does multiple blocking reads, tries to fill `to_buf'. Calls vi_read()
     * repeatedly.
     */
    int readFill(char *to_buf, slen_t max);
    /** Returns a number not larger than the bytes available _altogether_
     * from this stream. The default implementation returns 0, which is
     * always safe. For regular files, the method should return the size
     * of the file (unless the size is too large to be represented as an
     * slen_t), for pipes and sockets, the method should return 0.
     */
    inline virtual slen_t vi_availh() { return 0; }
    inline bool equal_content(Readable &other);
    /** Does nothing or rewinds the stream to the beginning, so it can be
     * read again. The default implementation does nothing.
     */
    inline virtual void vi_rewind() {}
  };

 protected:
  static void iter_char_sub(char const*beg, slen_t len, void *data);
};

// vvv Doesn't work in gcc 2.95 since we have abstract methods :-(. Must be
//     declared inside class { ... };
///GenBuffer::Writable &operator <<(GenBuffer::Writable a, GenBuffer b);

/** Collection of some simplistic GenBuffer implementations. */
class SimBuffer {
 public:
  class B;
  /** A flat buffer, i.e occupying consecutive bytes in memory. This class is
   * useless by itself since memory management (allocation and deletion of the
   * memory area) isn't implemented. Example of virtual inheritance: needed
   * because of SimBuffer::B.
   */
  class Flat: virtual public GenBuffer {
   protected:
    friend class /*SimBuffer::*/B; /* can read `beg' and `len' */
    const char *beg;
    slen_t len;
   public:
    inline virtual slen_t getLength() const { return len; }
    virtual void each_sub(block_sub_t block, void *data) const {
      if (len!=0) block(beg,len,data);
      block(0,0,data);
    }
    inline virtual void first_sub(Sub &sub) const { sub.beg=beg; sub.len=len; };
    inline virtual void next_sub(Sub &sub) const { sub.len=0; }
    inline char const*getCString() const { return beg; }
    inline char const*operator()() const { return beg; }
    inline char const*begin_() const { return beg; }
    inline char const*end_() const { return beg+len; }
    // inline operator char const*() const { return beg; } /* would kill void* */
    /** @param idx would cause overload conflict if declared len_t. No range check */
    inline char const&operator[](slendiff_t idx) const { return beg[idx]; }
    /** @return true iff not empty */
    inline virtual operator void*() const { return (void*)(len!=0); }
    // inline operator bool() const { return len!=0; }
    /** @return true iff empty */
    inline virtual bool operator!() const { return len==0; }
    /**
     * This is deliberatly not an `operator char'
     * to avoid the ambiguity of implicit auto-conversion.
     * @return 1st char or 0.
     */
    inline void toChar(char &ret) const {
      ret= len==0 ? 0 : *beg;
    }
    /** Overridden. */
    virtual slen_t copyRange(char *to, slen_t cfrom, slen_t clen) const;
    /** @return getLength() if not found, offset otherwise */
    slen_t findLast(char const c) const;
    /** @return getLength() if not found, offset otherwise */
    slen_t findFirst(char const c) const;
    /** @return getLength() if not found, offset otherwise */
    slen_t findFirst(char const* s, slen_t slen) const;
    virtual int cmp(Flat const& s2) const;
    virtual int cmp(char const* s2, slen_t len) const;
    /** @param fallback default: '\0'
     * @return if idx is too large: `fallback', otherwise: the asked char
     */
    virtual inline char getAt0(slen_t idx, char fallback='\0') {
      return idx>=len ? fallback : beg[idx];
    }
    /* Dat: pacify VC6.0: use of undefined type 'SimBuffer' */
    friend /*SimBuffer::*/B operator+(const /*SimBuffer::*/Flat& s1, const /*SimBuffer::*/Flat& s2);
    friend /*SimBuffer::*/B operator+(const char *s1, const /*SimBuffer::*/Flat& s2);
    friend /*SimBuffer::*/B operator+(const /*SimBuffer::*/Flat& s1, const char *s2);
  };
  
  /** A statically allocated, read-only char buffer, probably inside the TEXT
   * section (program code). Needs no delete()ing. `Does' memory management by
   * not doing anything: statically allocated memory belongs to whole lifetime
   * of the process, so it doesn't have to be freed.
   */
  class Static: public Flat {
   public:
    Static(char const*);
    Static(char const*beg_,slen_t len_) { beg=beg_; len=len_; }
  };

  /** A flat buffer of fixed length. Not particularly useful, try
   * SimBuffer::B instead.
   */
  class Fixed: public Flat {
   public:
    inline Fixed(slen_t len_) { beg=new char[len_]; len=len_; }
    virtual inline ~Fixed() {
      delete [] const_cast<char*>(beg); /* Dat: const_cast: pacify VC6.0 */
    }
   private:
    /** Disable this. */
    inline Fixed& operator=(Fixed const&) {return*this;}
  };

#if 0
  /** Fixed-length, writable */
  class FixWrite: public GenBuffer {
   public:
  };
#endif

  /** Abstract class. Example of virtual inheritance, needed because of
   * SimBuffer::B.
   */
  class Appendable: virtual public GenBuffer, public GenBuffer::Writable {
   public:
    /** Makes room for `len' more chars at the end of the string, and returns
     * a pointer to the beginning of that location. Should be efficient.
     */
    virtual char *vi_mkend(slen_t) =0;
    /** Makes room for `len' more chars at the end of the string, and returns
     * a pointer to the beginning of that location. May be inefficient.
     */
    virtual char *vi_mkbeg(slen_t) =0;
    /** Use this instead of append(...). */
    virtual void vi_write(char const*, slen_t);
    inline virtual void vi_putcc(char c) { vi_mkend(1)[0]=c; }
    /** There is no append(...) method. Use vi_write() instead. */
    void prepend(char const*, slen_t);
  };
  
  /** A one-way linked list of flat strings. Quickest for long memory appends.
   * Does memory management.
   */
  class Linked: public Appendable {
   public:
    struct Node {
      char *beg;
      slen_t len;
      /** May be NULL. */
      Node *next;
    };
    Node *first, *last;
    inline Linked(): first(0), last(0) {}
    virtual ~Linked();
    Linked(GenBuffer const& other);
    Linked(char const*);
    Linked& operator=(GenBuffer const& other);
    Linked& operator=(Linked const& other);
    virtual slen_t getLength() const;
    virtual void each_sub(block_sub_t block, void *data) const;
    virtual void first_sub(Sub &sub) const;
    virtual void next_sub(Sub &sub) const;
    virtual char *vi_mkend(slen_t len);
    virtual char *vi_mkbeg(slen_t len);
  };

  /* Abstract class. */  
  class Resizable: public Appendable { public:
    /** Grows the string by the specified `left' and `right' amount on the
     * sides. The amounts may be positive, zero or negative. For a negative
     * amount, the `?beg' will be rendered invalid. For a nonnegative amount,
     * `?beg' will point to the beginning of the new, uninitialized part of the
     * buffer.
     */
    virtual void vi_grow2(slendiff_t left, slendiff_t right, char **lbeg, char **rbeg) =0;
    Resizable& operator=(GenBuffer const& other);
    inline void clearFree() { vi_grow2(0, -(slendiff_t)getLength(), 0, 0); }
    inline void forgetAll() { vi_grow2(0, -(slendiff_t)getLength(), 0, 0); }
    /** If howmuch>getLength(), then clears the string. */
    inline void forgetLast(slen_t howmuch) { vi_grow2(0, -(slendiff_t)howmuch, 0, 0); }
    inline void forgetFirst(slen_t howmuch) { vi_grow2(-(slendiff_t)howmuch, 0, 0, 0); }
    void keepLeft(slen_t howmuch);
    void keepRight(slen_t howmuch);
    void keepSubstr(slen_t from_offset, slen_t slen);
    inline virtual char *vi_mkend(slen_t howmuch) { char *s; vi_grow2(0, howmuch, 0, &s); return s; }
    inline virtual char *vi_mkbeg(slen_t howmuch) { char *s; vi_grow2(howmuch, 0, &s, 0); return s; }
  };
  
  /** A simple, non-shared, writable, flat memory buffer of bytes. Supports
   * fast appends (with rare memory-rallocations) by pre-allocating at most
   * twice as much memory. Prepends are
   * slow, because they always include memory allocation and copying.
   * Does memory management.
   * Imp: check when `len' overflows (i.e 2*alloced etc.)
   */
  class B: public Resizable, public Flat {
   /* BUGFIX at Tue Sep  3 18:04:34 CEST 2002:
    * original order was: public Flat, public Resizable, but I got the
    * error message from gcc-3.2: gensi.hpp:398: sorry, not implemented: adjusting pointers for covariant returns
    */
   protected:
    /** Number of bytes preallocated. */
    slen_t alloced;
    char small[8];
   public:
    /** Overridden. */
    virtual void vi_grow2(slendiff_t left, slendiff_t right, char **lbeg, char **rbeg);

    /** Destructor: must be virtual since we have virtual methods. */
    virtual ~B() {
      if (beg!=small) delete [] const_cast<char*>(beg);
      /* ^^^ Dat: const_cast: pacify VC6.0 */
    }
    /** Constructor: the empty buffer. */
    inline B(): alloced(sizeof(small)) { beg=small; len=0; }
    /** Constructor: copy data from a null-terminated C string. */
    B(char const*);
    /** Constructor: copy data from a memory buffer. */
    B(char const*,slen_t);
    /** Constructor: copy-constructor */
    B(B const&);
    /** Constructor: copy data from a Flat buffer. */
    B(Flat const&);
    /** Constructor: copy data from a Flat buffer, term0(). */
    B(Flat const&,int);
    /** Constructor: copy data from a GenBuffer. */
    B(GenBuffer const&);
    /** Constructor: copy (consume) data from a readable stream. */
    B(GenBuffer::Readable &);
    /** Constructor: concatenate two (2) memory buffers. */
    B(char const*,slen_t, char const*,slen_t);
    /** Constructor: concatenate two (2) memory buffers, term0(). */
    B(char const*,slen_t, char const*,slen_t,int);
    /** Constructor: concatenate two GenBuffers. */
    B(GenBuffer const&, GenBuffer const&);
    /** Constructor: concatenate three (3) memory buffers. */
    B(char const*,slen_t, char const*,slen_t, char const*,slen_t);
    /** Constructor: concatenate three (3) GenBuffers. */
    B(GenBuffer const&, GenBuffer const&, GenBuffer const&);
    /** Constructor: concatenate a flat buffer and a C string */
    B(Flat const&, char const*);
    /** Constructor: concatenate three ... */
    B(char const*, Flat const&, char const*);
    /** Constructor: a substring of a Flat buffer */
    B(Flat const&, slen_t from_offset, slen_t slen);
    
    /** Works even when other==&(this). @return (this) */
    B& operator=(/*SimBuffer::*/Flat const& other);
    /** C++ SUXX: type conversion (Flat& -> B&) doesn't work as expected.
     * Works even when other==&(this). @return (this)
     */
    B& operator=(/*SimBuffer::*/B const& other);
    /** @return (this) */
    B& operator=(char const*);
    /** Reads (consumes) the whole `stream', and appends the bytes to (this).
     * @return (this) declaring `operator<<' inside would ruin inherited `operator<<'s
     */
    /* GenBuffer::Writable& operator<<(GenBuffer::Readable &stream); */

    /** Pacify VC6.0 multiple inheritance*/
    inline virtual operator void*() const { return (void*)(len!=0); }
    /** Pacify VC6.0 multiple inheritance */
    inline virtual bool operator!() const { return len==0; }

    friend /*SimBuffer::*/B& operator<<(/*SimBuffer::*/B& self, GenBuffer::Readable &stream);
    /** Specific operators for faster implementation */
    B& operator<<(char c);
    B& operator<<(char const* s);
    /** Works even when other==&(this). @return (this) */
    B& operator<<(Flat const&);
    /* C++ inheritance SUXXXX: now I have to re-define _every_ operator<< in GenBuffer::Writable... */
    B& operator <<(void const*);
    inline B& operator <<(  signed short n) { write_num((signed long)n); return*this; }
    inline B& operator <<(  signed int   n) { write_num((signed long)n); return*this; }
    inline B& operator <<(  signed long  n) { write_num(n); return*this; }
    inline B& operator <<(unsigned short n) { write_num((unsigned long)n); return*this; }
    inline B& operator <<(unsigned int   n) { write_num((unsigned long)n); return*this; }
    inline B& operator <<(unsigned long  n) { write_num(n); return*this; }
    #if HAVE_LONG_LONG && NEED_LONG_LONG
      inline B &operator <<(signed long long n) { write_num(n); return*this; }
      inline B &operator <<(unsigned long long n) { write_num(n); return*this; }
    #endif
    inline B& operator <<(bool b) { GenBuffer::Writable::operator<<(b); return*this; }
    
#if 0
    friend SimBuffer::B& operator<<(SimBuffer::B& self, char const*s);
    friend SimBuffer::B& operator<<(SimBuffer::B& self, char c);
#endif
    
    inline char *begin_() const { return const_cast<char*>(beg); }
    inline char *end_  () const { return const_cast<char*>(beg)+len; }
    inline bool isFull() const { return len==alloced; }
    /** Ensures beg[len]=='\0'. @return (this) */
    B& term0();
    /** @param idx would cause overload conflict if declared len_t. No range check */
    inline char &operator[](slendiff_t idx) const { return const_cast<char*>(beg)[idx]; }
    /** @param lendiff if negative, then makes the buffer shorter */
    void grow_set0_by(slendiff_t lendiff);
    /** Grows the buffer if necessary, fills with '\0' */
    char getAt(slen_t idx);
    /** Doesn't free unnecessary memory. */
    inline void clear() { len=0; }
    /** Removes oldmuch chars from index first, and makes place for newmuch
     * chars there. Returns the beginning of the new place. Calls memmove().
     * Tue Jun 11 15:33:33 CEST 2002
     */
    char *substr_grow(slen_t first, slen_t oldmuch, slen_t newmuch);
    B substr(slen_t first, slen_t howmuch) const;
    B substr(slen_t first) const;
    B right(slen_t howmuch) const;
    B left(slen_t howmuch) const;
    static void space_pad_cpy(char *dst, char const*src, slen_t pad);

    /* vi_write() doesn't work if s is inside (this).
     * There is no append(...) method. Use vi_write() instead.
     */
    // void append(char const*s, const slen_t len_);
    virtual void vi_write(char const*, slen_t);

    /* Original: B& vformat(slen_t n, char const *fmt, va_list ap);
     * Pacify VC6.0: error C2555: 'SimBuffer::B::vformat' : overriding virtual function differs from 'GenBuffer::Writable::vformat' only by return type or calling convention
     */
    /** appends at most `n' chars, no trailing '\0'. This is different from
     * ANSI (old and C99) stdio.h, because those insert at most `n-1' chars
     * (not counting the '\0'), _and_ a trailing '\0'. Truncates the output
     * to `n' chars if it would be longer. (Truncation semantics changed at
     * Tue Jun 11 14:27:12 CEST 2002. Old: truncate to no chars if longer)
     */
    GenBuffer::Writable& vformat(slen_t n, char const *fmt, va_list ap);
    /** appends as many chars as requrested */
    GenBuffer::Writable& vformat(char const *fmt, va_list ap);
    /** appends; calls vformat(n); mandatorly non-inline because of ... */
    GenBuffer::Writable& format(slen_t n, char const *fmt, ...);
    /** appends; calls vformat(); mandatorly non-inline because of ... */
    GenBuffer::Writable& format(char const *fmt, ...);

    /*
     * Name: NonPathMeta
     * Input: any binary
     * Output: non-path characters pre-backslashed
     * Description: Places backslashes in front of non-path characters:
     *   [^-_./a-zA-Z0-9].
     * Compatibility: UNIX shells: sh (Bourne Shell), bash, ksh, zsh. Use this in
     *   a shell script to protect a string from word splitting, variable
     *   substitution and everything else. Note that there will be problems
     *   only with \0 (depends on the shell) and \n (will be simply removed by
     *   the shell!). See also Quote::QShell for full shell compatility.
     * Valid input type: binary
     * Valid input: any binary
     * On invalid input: impossible
     * Inverse of valid: lossy: Quote::UnMeta
     * Validity indicator: implemented
     * Output type: some binary
     * Direction: encode
     * Method: each_byte
     * Dependencies: -
     */
    B& appendNpmq(const Flat &other, bool dq=false);
    /** Quotes a filename (actually a pathname since it may include
     * (sub)directories) specified in param `other' to be passed to the
     * most common shell of the host operating system (/bin/sh, COMMAND.COM,
     * CMD.EXE etc.) as a separate command line argument for a command
     * invoked from the shell.
     *
     * Under UNIX, this differs from appendNpmq only when treating [\n\0].
     * Under Win32, the filename is surrounded by double quotes. Double quotes
     * inside the filename are skipped. Other systems than Win32 are treated
     * like UNIX.
     * @param preminus prefix filenames starting with `-' with `./' ?
     */
    B& appendFnq(const Flat &other, bool preminus=false);
    /**
     * Name: Quote::NonPathOctal; from quote.rb
     * Input: any binary
     * Output: non-path characters converted to octal
     * Description: Converts non-path characters ([^-_./a-zA-Z0-9]) in
     *   a string to their prebackslashed, 3-digit octal representation (i.e
     *   \123).
     * Compatibility: Ruby, ANSI C, K&R C, C++, Java (without \u....),
     *   TCL double quotes (without \u....), TCL unquoted strings (without
     *   \u....), Perl5, Pike, AWK, PostScript Level 1, bc. See also
     *   Quote::*Octal.
     * Valid input type: binary
     * Valid input: any binary
     * On invalid input: impossible
     * Inverse of valid: lossy: Quote::UnSlash
     * Validity indicator: implemented
     * Output type: \A[-\\._/a-zA-Z0-9]*\z
     * Direction: encode
     * Method: each_byte
     * Dependencies: -
     */
    B& appendDump(const Flat &other, bool dq=false);
    B& appendDump(const char c, bool dq=false);
    /**
     * Name: Quote::Unslash
     * Input: a double-quoted (backslashed) version of a string without 
     *   the double quotes themselves
     * Output: the original, unquoted (possibly binary) string
     * Description: Converts a string expressed inside double quotes of some
     *   programming language (e.g Ruby, C, Java, Perl, Ruby) to its original,
     *   unquoted state. Transformation is done only after backslashes. The
     *   following `common' transformations are supported: \0, \00, \000
     *   (octal), \a (alarm bell), \b (backslash), \e (escape), \f (form feed),
     *   \n (newline), \r (carriage return), \t (horizontal tab), \v (verical
     *   tab) \x61, \c[, \l (lowercase), \u (upper case), \NL (skip this),
     *   \", \\, \... .
     * Compatibility: Ruby, ANSI C, C++, Java (without \u....), TCL double
     *   quotes (without \u....), TCL unquoted strings, Perl5, Pike, AWK,
     *   PostScript Level 1, bc, PHP.
     *   See also Quote::UnSlashPHPC for full PHP:StripCSlashes() compatibility.
     *   See also Quote::UnSlashKnr. Compatible with PHP stripcslashes().
     *   See also Quote::UnSlashKnr. Differs from Quote::UnslashMiddle by not
     *   removing the double quotes from string edges.
     * Valid input type: binary
     * Valid input: any binary
     * On invalid input: impossible
     * Inverse of valid: lossy: Quote::NonPathOctal
     * Validity indicator: implemented
     * Output type: any binary
     * Direction: decode
     * Method: gsub
     * Dependencies: -
     *
     * @param iniq the char that surrounds the quoted param `other'
     * @param other a quoted string
     * @return an empty string if iniq<256 and param `other' not delimited by iniq
     */
    B& appendUnslash(const Flat &other, int iniq);
    /** Appends as a C (double-quoted) string. */
    B& appendDumpC  (const Flat &other, bool dq=false);
    /** Appends as a PostScript (paren-quoted) string. */
    B& appendDumpPS (const Flat &other, bool dq=false);
    /** Make `other' upper case (English), plus change all non-alpha chars
     * to underscore.
     */
    B& appendHppq(const Flat &other);

   protected:
    inline char *grow_by(slen_t howmuch) { char *s; vi_grow2(0, howmuch, 0, &s); return s; }
    /*SimBuffer::*/B& B_append(GenBuffer::Readable &stream);
#if 0
    SimBuffer::B& B_append(char c);
    SimBuffer::B& B_append(char const*s);
#endif
  };
};

inline SimBuffer::B& operator<<(SimBuffer::B& self, GenBuffer::Readable &stream) { return self.B_append(stream); }
#if 0
inline SimBuffer::B& operator<<(SimBuffer::B& self, char const*s) { return self.B_append(s); }
inline SimBuffer::B& operator<<(SimBuffer::B& self, char c) { return self.B_append(c); }
#endif

/** Shorthand synonym */
typedef SimBuffer::B Buffer;

inline bool operator ==(const GenBuffer& s1, const GenBuffer& s2) {
  return 0==s1.cmp(s2);
}
inline bool operator ==(const char *s1, const GenBuffer& s2) {
  return 0==s2.cmp(s1);
}
inline bool operator ==(const GenBuffer& s1, const char *s2) {
  return 0==s1.cmp(s2);
}
inline bool operator <(const GenBuffer& s1, const GenBuffer& s2) {
  return 0>s1.cmp(s2);
}
inline bool operator <(const char *s1, const GenBuffer& s2) {
  return 0<s2.cmp(s1);
}
inline bool operator <(const GenBuffer& s1, const char *s2) {
  return 0>s1.cmp(s2);
}
inline bool operator >(const GenBuffer& s1, const GenBuffer& s2) {
  return 0<s1.cmp(s2);
}
inline bool operator >(const char *s1, const GenBuffer& s2) {
  return 0>s2.cmp(s1);
}
inline bool operator >(const GenBuffer& s1, const char *s2) {
  return 0<s1.cmp(s2);
}
inline bool operator <=(const GenBuffer& s1, const GenBuffer& s2) {
  return 0>=s1.cmp(s2);
}
inline bool operator <=(const char *s1, const GenBuffer& s2) {
  return 0<=s2.cmp(s1);
}
inline bool operator <=(const GenBuffer& s1, const char *s2) {
  return 0>=s1.cmp(s2);
}
inline bool operator >=(const GenBuffer& s1, const GenBuffer& s2) {
  return 0<=s1.cmp(s2);
}
inline bool operator >=(const char *s1, const GenBuffer& s2) {
  return 0>=s2.cmp(s1);
}
inline bool operator >=(const GenBuffer& s1, const char *s2) {
  return 0<=s1.cmp(s2);
}
inline bool operator !=(const GenBuffer& s1, const GenBuffer& s2) {
  return 0!=s1.cmp(s2);
}
inline bool operator !=(const char *s1, const GenBuffer& s2) {
  return 0!=s2.cmp(s1);
}
inline bool operator !=(const GenBuffer& s1, const char *s2) {
  return 0!=s1.cmp(s2);
}

#endif
