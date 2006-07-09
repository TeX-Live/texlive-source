/* minips.hpp -- mini-PostScript parser and structure builder
 * by pts@fazekas.hu at Sat Mar  9 21:33:04 CET 2002
 */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef MINIPS_HPP
#define MINIPS_HPP 1

#include "config2.h"
#include "gensi.hpp"
#include "error.hpp"

class MiniPS {
 public:
  #if SIZEOF_VOID_P <= SIZEOF_INT
    typedef signed int ii_t;
  #elif SIZEOF_VOID_P <= SIZEOF_LONG
    typedef signed long ii_t;
  #elif SIZEOF_VOID_P <= SIZEOF_CFG_LONGEST
    typedef signed PTS_CFG_LONGEST ii_t;
  #else
    #error No integral data type to hold a ptr.
  #endif

  class Real;

  /*union*/ struct TokVal {
    SimBuffer::B *bb;
    ii_t i;
    /* double d; */
    /*MiniPS::*/Real *r;
  };
  /** minips tokenizer */
  class Tokenizer {
   public:
    BEGIN_STATIC_ENUM1(int) EOFF=-1, NO_UNGOT=-2 END_STATIC_ENUM()
    Tokenizer(GenBuffer::Readable& in_);
    /** Reads and returns next token.
     * Token types are named for their first character:
     * '1': integertype: 31 bits signed (-1073741824 .. 1073741823) is
     *      guaranteed. VALUED
     * '.': realtype (NOT implemented), double guaranteed. VALUED
     * 'E': Ename (name without a slash). VALUED
     * '/': Sname (name beginning with a slash). VALUED
     * '(': stringtype (also with `<hexadecimal>') VALUED
     * '[': beginning-of-array (also with `{')
     * ']': end-of-array (also with '}')
     * '<': beginning-of-dict (`<<')
     * '>': end-of-dict (`>>')
     * -1:  EOF
     */
    int yylex();
    inline TokVal const& lastTokVal() const { return tv; }
   protected:
    /** Data for last token read. */
    TokVal tv;
    SimBuffer::B b;
    GenBuffer::Readable& in;
    /* NO_UNGOT for nothing, EOFF for EOF, 0..255 otherwise */
    int ungot;
  };
  
  /* This is somewhat similar to Ruby */
  typedef ii_t VALUE;
  /** Qundef is the undefined hash key or array elements. It is an _invalid_ VALUE! */
  BEGIN_STATIC_ENUM1(VALUE) Qfalse=0, Qtrue=2, Qnull=4, Qundef=6, Qpop=8, Qerror=10, Qmax_=10 END_STATIC_ENUM()
  
  BEGIN_STATIC_ENUM1(unsigned) T_NULL=1, T_BOOLEAN=2, T_INTEGER=3, T_REAL=4,
    T_STRING=5, T_ARRAY=6, T_DICT=7, T_SNAME=8, T_ENAME=9, T_VOID=10,
    S_SENUM=20, /* must be a dict-member; meta-type used by scanf_dict() */
    S_FUNC=21, /* call a function to determine; meta-type used by scanf_dict() */
    S_UINTEGER=22, /* non-negative integer; meta-type used by scanf_dict() */
    S_ANY=23, /* anything; meta-type used by scanf_dict() */
    S_PINTEGER=24, /* positive integer; meta-type used by scanf_dict() */
    S_RGBSTR=25, /* an optional 3-byte string, representing an RGB color triplet */
    S_NUMBER=26, /* real or integer */
    S_PNUMBER=27 /* positive real or integer */
  END_STATIC_ENUM()

  /** No virtual methods because of special types. MiniPS composite values
   * really _contain_ their components; one Value has exactly one reference:
   * its container component.
   */
  class Value { public:
    inline ii_t getLength() const { return len; }
    inline ii_t getType() const { return ty; }
    inline bool hasPtr() const { return ptr!=NULLP; }
    inline bool isDumping() const { return dumping; }
    inline char const* getCstr() const { return (char const*)ptr; }
    inline char* begin_() const { return (char*)ptr; }
    inline char const* operator()() const { return (char const*)ptr; }
   protected:
    ii_t len; void *ptr; unsigned char ty; bool dumping;
  };
  /** ptr contains a `void*'. Won't be freed. */
  class Void: public Value { public:
    inline Void(void *ptr_) { ptr=ptr_; ty=T_VOID; }
    inline void *getPtr() const { return ptr; }
  };
  /** Always null-terminated. */
  class String: public Value { public:
    /** Copies from ptr_ */
    String(char const*ptr_, ii_t len_);
    /** Replaces (this) with a copy of (a).(b) */
    void replace(char const*ap, slen_t alen, char const*bp, slen_t blen);
  };
  class Sname: public Value { public:
    /** ptr_ must begin with '/' */
    Sname(char const*ptr_, ii_t len_);
    bool equals(Sname const&other);
    bool equals(char const*other);
  };
  class Ename: public Value { public:
    Ename(char const*ptr_, ii_t len_);
    bool equals(Ename const&other);
    bool equals(char const*other);
    bool equals(char const*other, slen_t otherlen);
  };
  class Real: public Value { public:
    typedef unsigned char metric_t;
    inline Real(double d_): d(d_), metric(0), dumpPS(false) { ty=T_REAL; ptr=(char*)NULLP; }
    /** Also supply a string representation of the value (to avoid possible
     * loss of precision when converting string -> double -> string).
     */
    Real(double d_, char const*ptr_, ii_t len_);
    // inline bool isZero() const { return d==0.0; }
    inline double getBp() const { return d*me_factor[metric]; }
    inline void setDumpPS(bool g) { dumpPS=g; }
    inline void setMetric(metric_t metric_) { metric=metric_; }
    /** Return true iff the specified null-terminated string is a valid
     * dimen.
     */
    static bool isDimen(char const *);
    void dump(GenBuffer::Writable &out_, bool dumpPS_force=false);
    /** @return ME_count on invalid */
    static metric_t str2metric(char const str[2]);
    BEGIN_STATIC_ENUM1(metric_t)
     ME_bp=0, /* 1 bp = 1 bp (big point) */
     ME_in=1, /* 1 in = 72 bp (inch) */
     ME_pt=2, /* 1 pt = 72/72.27 bp (point) */
     ME_pc=3, /* 1 pc = 12*72/72.27 bp (pica) */
     ME_dd=4, /* 1 dd = 1238/1157*72/72.27 bp (didot point) [about 1.06601110141206 bp] */
     ME_cc=5, /* 1 cc = 12*1238/1157*72/72.27 bp (cicero) */
     ME_sp=6, /* 1 sp = 72/72.27/65536 bp (scaled point) */
     ME_cm=7, /* 1 cm = 72/2.54 bp (centimeter) */
     ME_mm=8, /* 1 mm = 7.2/2.54 bp (millimeter) */
     ME_COUNT=9
   END_STATIC_ENUM()
   protected:
    double d;
    /* vvv metric added at Sat Sep  7 12:26:08 CEST 2002 */
    metric_t metric;
    /** Allow PostScript operators such as `div' to appear in the dump */
    bool dumpPS;
    /** Factor to convert to bp */
    static const double me_factor[ME_COUNT];
    /** PostScript code to do multiplication by me_factor */
    static char const* const me_psfactor[ME_COUNT];
  };
  class Array: public Value { public:
    Array();
    void free();
    void dump(GenBuffer::Writable &out_, unsigned indent);
    void push(VALUE v);
    VALUE get(ii_t index);
    /** Cannot extend. Calls delete0() when overwriting an element */
    void set(ii_t index, VALUE val);
    /** val: out */
    void getFirst(VALUE *&val);
    /** val: in_out */
    void getNext(VALUE *&val);
   protected:
    ii_t alloced;
    void extend(ii_t newlen);
  };
  /** Keys must be Snames here! (i.e no integer keys allowed). The current
   * implementation does a linear string search :-(. Dat: might not work
   * on extremely long keys if slen_t can hold a larger integer than
   * ii_t.
   */
  class Dict: public Value { public:
    Dict();
    void free();
    void dump(GenBuffer::Writable &out_, unsigned indent, bool dump_delimiters=true);
    /** @return val or Qundef */
    VALUE get(char const*key, slen_t keylen);
    /** A hack: get and touch. */
    VALUE get1(char const*key, slen_t keylen);
    void untouch(char const*key, slen_t keylen);
    /** Can extend. */
    void put(char const*key, VALUE val);
    /** Calls delete0() when overwriting an element */
    void put(char const*key, slen_t keylen, VALUE val);
    /** @return old value for key `key', or Qundef */
    VALUE push(char const*key, slen_t keylen, VALUE val);
    /** key: out, val: out */
    void getFirst(char const*const*& key, slen_t &keylen, VALUE *&val, bool &touched);
    /** key: in_out, val: in_out */
    void getNext (char const*const*& key, slen_t &keylen, VALUE *&val, bool &touched);
    // void getFirst(VALUE *&key, VALUE *&val);
    // void getNext(VALUE *&key, VALUE *&val);
   protected:
    ii_t alloced;
    void extend(ii_t newlen);
  };

  static inline Value*  RVALUE(VALUE v) { return static_cast<Value*>((void*)v); }
  static inline Void*   RVOID(VALUE v)  { return static_cast<Void*>((void*)v); }
  static inline Array*  RARRAY(VALUE v) { return static_cast<Array*>((void*)v); }
  static inline String* RSTRING(VALUE v){ return static_cast<String*>((void*)v); }
  static inline Dict*   RDICT(VALUE v)  { return static_cast<Dict*>((void*)v); }
  static inline Real*   RREAL(VALUE v)  { return static_cast<Real*>((void*)v); }
  static inline Sname*  RSNAME(VALUE v) { return static_cast<Sname*>((void*)v); }
  static inline Ename*  RENAME(VALUE v) { return static_cast<Ename*>((void*)v); }
  static inline bool isDirect(VALUE v) { return (v&1)!=0 || v<=Qmax_; }
  /** T_NULL .. T_DICT */
  static unsigned getType(VALUE v);
  /** "null" .. "dict", "name" (T_SNAME), "ename" (T_ENAME, non-std-PS) */
  static char const* getTypeStr(unsigned u);
  static void delete0(VALUE v);
  /** The current implementation dumps dict keys in order of insertion, but
   * this is very likely to change soon to an _arbitrary_ order.
   */
  static void dump(GenBuffer::Writable& out_, VALUE v, unsigned indent=0);
  static void dump(VALUE v, unsigned indent=0);
  static inline VALUE Qinteger(ii_t v) { return (v<<1)+1; }
  static inline ii_t int2ii(VALUE v) { return v>>1; }
  /** Fortunate coincidence that 2x+1>0 <=> x>0 */
  static inline bool isPositive(VALUE v) { return v>0; }
  static inline VALUE undef2null(VALUE v) { return v==Qundef ? Qnull : v; }
  // static SimBuffer::B scale72(VALUE v, double d);
  
  class Parser {
    /** Define this to avoid including <stdio.h> */
    typedef class _anon_filet_ {} *FILEP;
   public:
    BEGIN_STATIC_ENUM1(int) EOF_ALLOWED=Tokenizer::EOFF, EOF_ILLEGAL=-2, EOF_ILLEGAL_POP=-3 END_STATIC_ENUM()
    /** Maximum depth of `run' file inclusions. */
    BEGIN_STATIC_ENUM1(unsigned) MAX_DEPTH=17 END_STATIC_ENUM()
    Parser(char const *filename_);
    /** The caller is responsible for closing the FILE* */
    Parser(FILEP f_);
    Parser(GenBuffer::Readable *rd_);
    Parser(Tokenizer *tok_);
    ~Parser();
    /** Allocates and returns. Qundef is returned on EOF
     * @param closer: EOF_ILLEGAL, EOF_ALLOWED, '>' or ']'
     */
    VALUE parse1(int closer=EOF_ILLEGAL, int sev=Error::EERROR);
    void setDepth(unsigned depth_);
    /** Sets special filename for the `run' operator. `run' will read that
     * special file from param `rd_', and then it will call rd_->vi_rewind().
     * Example usage: .addSpecRun("%stdin", new Files::FileR(stdin));
     */
    void addSpecRun(char const* filename_, GenBuffer::Readable *rd_);
    /*MiniPS::*/Dict *getSpecRuns() const { return specRuns; }
    /** Does not copy the dict, but sets the pointer. */
    void setSpecRuns(/*MiniPS::*/Dict *);
   protected:
    Parser(Parser *master_);
    // VALUE parse1_real(int closer);
    /* 0=nothing, 1=master 2=tok, 3=tok+rd, 4=tok+rd+f */
    unsigned free_level;
    Parser *master;
    Tokenizer *tok;
    GenBuffer::Readable *rd;
    FILEP f;
    int unread;
    unsigned depth;
    /*MiniPS::*/Dict *specRuns;
    /** Should MiniPS::delete0() be called on specRuns upon destruction of
     * (this)?
     */
    bool specRunsDelete;
  };

  /** Assumes that param `job' is a dict, extracts its elements into ...,
   * emits errors for elements (not found and having default==Qundef), emits
   * warnings for elements found in `job', but undescribed in `...' (only
   * if show_warnings==true). Example:
   *
   * MiniPS::scanf_dict(job, true,
   *   "InputFile",  MiniPS::T_STRING, MiniPS::Qundef, &InputFile,
   *   "OutputFile", MiniPS::T_STRING, MiniPS::Qundef, &OutputFile,
   *   "Profiles",   MiniPS::T_ARRAY,  MiniPS::Qundef, &Profiles
   *   NULLP
   * );
   */
  static void scanf_dict(VALUE job, bool show_warnings, ...);
  static void setDumpPS(VALUE v, bool g);
  /** @param v must be T_REAL or T_INTEGER */
  static bool isZero(VALUE v);
  /** @param v must be T_REAL or T_INTEGER
   * @return true iff v==i
   */
  static bool isEq(VALUE v, double d);
  /** Dumps the human-readable real or integer value of the sum (m/72)*a+b+c-sub
   * to `out'.
   * @param rounding 0: nothing. 1: round the sum _up_ to integers. 2:
   *        round the sum _up_ to non-negative integers
   * @param a must be T_REAL or T_INTEGER
   * @param b must be T_REAL or T_INTEGER
   * @param c must be T_REAL or T_INTEGER
   * @param sub must be T_REAL or T_INTEGER
   */
  static void dumpAdd3(GenBuffer::Writable &out, VALUE m, VALUE a, VALUE b, VALUE c, VALUE sub, unsigned rounding=0);
  static void dumpScale(GenBuffer::Writable &out, VALUE v);
};

/* Fri Aug 16 17:07:14 CEST 2002 */
#if 0 /* doesn't work because MiniPS::VALUE is really an `int', so there will be an ambiguous overload */
inline GenBuffer::Writable& operator<<(GenBuffer::Writable& out_, MiniPS::VALUE v) {
  MiniPS::dump(out_, v);
  return out_;
}
#endif
inline GenBuffer::Writable& operator<<(GenBuffer::Writable& out_, MiniPS::Value *v) {
  MiniPS::dump(out_, (MiniPS::VALUE)v);
  return out_;
}

// GenBuffer::Writable& operator<<(GenBuffer::Writable& out_, MiniPS::Value *v) {

#endif
