/*
 * minips.cpp
 * by pts@fazekas.hu at Sat Mar  9 21:33:35 CET 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "minips.hpp"
#include "error.hpp"
#include "gensio.hpp"
#if USE_DICT_MAPPING
#if OBJDEP
#  warning REQUIRES: mapping.o
#endif
#include "mapping.hpp"
#endif
#include <stdio.h> /* sscanf() */
#include <string.h> /* memset() */

static inline bool is_ps_white(char c) {
  return c=='\n' || c=='\r' || c=='\t' || c==' ' || c=='\f' || c=='\0';
}

static inline bool is_ps_name(char c) {
  /* Dat: we differ from PDF since we do not treat the hashmark (`#') special
   *      in names.
   * Dat: we differ from PostScript since we accept names =~ /[!-~]/
   */
  return c>='!' && c<='~'
      && c!='/' && c!='%' && c!='{' && c!='}' && c!='<' && c!='>'
      && c!='[' && c!=']' && c!='(' && c!=')';
  /* Dat: PS avoids: /{}<>()[]% \n\r\t\000\f\040 */
}

/** @param b: assume null-terminated @return true on erro
 * @return false on error
 */
static inline bool toInteger(SimBuffer::Flat const&b, signed long &ret) {
  int n=0; /* BUGFIX?? found by __CHECKER__ */
  // b.term0();
  return sscanf(b(), "%li%n", &ret, &n)<1 || b[n]!='\0';
}

static inline bool toHex(char const*s, unsigned long &ret) {
  int n=0;
  return sscanf(s, "%lx%n", &ret, &n)<1 || s[n]!='\0';
}

static inline bool toHex3(char const*s, char ret[3]) {
  unsigned long l;
  if (toHex(s, l)) return true;
  ret[0]=((l>>8)&15)*17; ret[1]=((l>>4)&15)*17; ret[2]=(l&15)*17;
  return false;
}

static inline bool toHex6(char const*s, char ret[3]) {
  unsigned long l;
  if (toHex(s, l)) return true;
  ret[0]=(l>>16)&255; ret[1]=(l>>8)&255; ret[2]=l&255;
  return false;
}

/** @param b: assume null-terminated @return true on error */
static inline bool toReal(SimBuffer::Flat const&b, double &ret) {
  int n;
  char c;
  // b.term0();
  /* Dat: glibc accepts "12e", "12E", "12e+" and "12E-" */
  return sscanf(b(), "%lf%n", &ret, &n)<1
      || (c=b[n-1])=='e' || c=='E' || c=='+' || c=='-' || b[n]!='\0';
}

/** This is not correct if blen cuts the real number into two strings.
 * @param b: assume null-terminated @return true on error
 */
static inline bool toReal(char const *b, slen_t blen, double &ret) {
  int n;
  char c;
  // b.term0();
  /* Dat: glibc accepts "12e", "12E", "12e+" and "12E-" */
  return sscanf(b, "%lf%n", &ret, &n)<1
      || (c=b[n-1])=='e' || c=='E' || c=='+' || c=='-' || (slen_t)n!=blen;
}

MiniPS::Tokenizer::Tokenizer(GenBuffer::Readable& in_): in(in_), ungot(NO_UNGOT) {
}
int MiniPS::Tokenizer::yylex() {
  int c=0; /* dummy initialization */
  bool hi;
  unsigned hv;
  slen_t nest, len;
  signed long l;
  double d;
  Real::metric_t metric;
  char saved;
  
  if (ungot==EOFF) return EOFF;
  if (ungot!=NO_UNGOT) { c=ungot; ungot=NO_UNGOT; goto again; }
 again_getcc:
  c=in.vi_getcc();
 again:
  switch (c) {
   case -1: eof:
    return ungot=EOFF;
   case '\n': case '\r': case '\t': case ' ': case '\f': case '\0':
    goto again_getcc;
   case '%': /* one-line comment */
    while ((c=in.vi_getcc())!='\n' && c!='\r' && c!=-1) ;
    if (c==-1) goto eof;
    goto again_getcc;
   case '{': case '[':
    return '[';
   case '}': case ']':
    return ']';
   case ')': goto err;
   case '>':
    if (in.vi_getcc()!='>') goto err;
    return '>';
   case '<':
    if ((c=in.vi_getcc())==-1) { uf_hex: Error::sev(Error::EERROR) << "miniPS: unfinished hexstr" << (Error*)0; }
    if (c=='<') return '<';
    if (c=='~') Error::sev(Error::EERROR) << "miniPS: a85str unsupported" << (Error*)0;
    tv.bb=&b; b.clear();
    hi=true;
    while (c!='>') {
      if ((hv=b.hexc2n(c))!=16) {
        if (hi) { b << (char)(hv<<4); hi=false; }
           else { b.end_()[-1]|=hv; hi=true; }
      } else if (!is_ps_white(c)) Error::sev(Error::EERROR) << "miniPS: syntax error in hexstr" << (Error*)0;
      if ((c=in.vi_getcc())==-1) goto uf_hex;
    }
    /* This is correct even if an odd number of hex digits have arrived */
    return '(';
   case '(':
    tv.bb=&b; b.clear();
    nest=1;
    while ((c=in.vi_getcc())!=-1) { redo:
      if (c==')' && --nest==0) return '(';
      if (c!='\\') { if (c=='(') nest++; b << (char)c; continue; }
      /* read a backslash */
      switch (c=in.vi_getcc()) {
       case -1: goto uf_str;
       case 'n': b << '\n'; break;
       case 'r': b << '\r'; break;
       case 't': b << '\t'; break;
       case 'b': b << '\010'; break; /* \b and \a conflict between -ansi and -traditional */
       case 'f': b << '\f'; break;
       default:
        if (c<'0' || c>'7') { b << (char)c; break; }
        hv=c-'0'; /* read at most 3 octal chars */
        if ((c=in.vi_getcc())==-1) goto uf_str;
        if (c<'0' || c>'7') { b << (char)hv; goto redo; }
        hv=8*hv+(c-'0');
        if ((c=in.vi_getcc())==-1) goto uf_str;
        if (c<'0' || c>'7') { b << (char)hv; goto redo; }
        b << (char)(8*hv+(c-'0'));
      } /* SWITCH */
    } /* WHILE */    
    uf_str: Error::sev(Error::EERROR) << "miniPS: unfinished str" << (Error*)0;
   case '/':
    /* fall-through, b will begin with '/' */
   default: /* /nametype, /integertype or /realtype */
    tv.bb=&b; b.clear();
    b.clear(); b << (char)c;
    while ((c=in.vi_getcc())!=-1 && is_ps_name(c)) b << (char)c;
    ungot=c==-1?EOFF:c;
    if (b[0]=='/') return '/';
    b.term0();
    /* Dat: we don't support base-n number such as `16#100' == 256 in PostScript */
    if (!toInteger(b, l)) { tv.i=l; return '1'; }
    /* Dat: call toInteger _before_ toReal */
    // if (!toReal(b, tv.d)) { fprintf(stderr,"%f;\n", tv.d); }
    /* assert(tv.bb!=NULLP); */
    len=b.getLength();
    if (!toReal(b, d)) { /* tv.bb is also valid */
      tv.r=new Real(d, b(), len);
      return '.';
    } 
    if (len>2 && (metric=Real::str2metric(b()+len-2))!=Real::ME_COUNT) {
      saved=b[len-2];
      b[len-2]='\0';
      if (!toReal(b, d)) {
        tv.r=new Real(d, b(), len-2);
        tv.r->setMetric(metric);
        return ':'; /* Real with metric */
      }
      b[len-2]=saved;
    }
    return 'E'; /* /nametype */
  }
 err:
  Error::sev(Error::EERROR) << "miniPS: syntax error" << (Error*)0;
  goto again_getcc; /* notreached */
}

/* --- */

#if 0
inline static unsigned typerr() { assert(0); return 0; }
#endif

unsigned MiniPS::getType(VALUE v) {
  return (v&1)!=0 ? T_INTEGER
       : v>Qmax_ ? RVALUE(v)->getType()
       : v==Qnull ? T_NULL+0/*avoid gcc-3.0 ld bug*/
       : T_BOOLEAN;
}
char const* MiniPS::getTypeStr(unsigned u) {
  static char const* strs[]= { (char const*)NULLP, "null", "boolean", "integer", "real", "string", "array", "dict", "name", "Ename", "void" };
  // return strs[getType(v)];
  return strs[u];
}
void MiniPS::delete0(VALUE v) {
  if (isDirect(v)) return;
  Value *vp=RVALUE(v);
  unsigned ty=vp->getType();
  if (ty==T_DICT) RDICT(v)->free();
  else if (ty==T_ARRAY) RARRAY(v)->free();
  else if (ty==T_VOID) ;
  else if (vp->hasPtr()) delete [] (char*)vp->begin_();
  delete vp; /* BUGFIX at Sat Sep  7 12:50:13 CEST 2002 */
}
void MiniPS::dump(VALUE v, unsigned indent) {
  Files::FILEW sout(stdout);
  dump(sout, v, indent);
}
void MiniPS::dump(GenBuffer::Writable& out_, VALUE v, unsigned indent) {
  if (v==Qnull) out_ << "null";
  else if (v==Qtrue) out_ << "true";
  else if (v==Qfalse) out_ << "false";
  else if ((v&1)!=0) out_ << (v/2); /* prints a signed integer */
  else {
    Value *vp=RVALUE(v);
    unsigned ty=vp->getType();
    if (ty==T_STRING) {
      SimBuffer::Static s((char*)vp->begin_(), vp->getLength());
      SimBuffer::B b;
      b.appendDumpPS(s, true);
      out_ << b;
    } else if (ty==T_SNAME || ty==T_ENAME) {
      out_.vi_write((char*)vp->begin_(), vp->getLength());
    } else if (ty==T_REAL) {
      RREAL(v)->dump(out_);
    } else if (ty==T_ARRAY) {
      if (!vp->isDumping()) { /* Imp: thread-safe locking */
        RARRAY(v)->dump(out_, indent);
      } else out_ << "[...]";
    } else if (ty==T_DICT) {
      if (!vp->isDumping()) { /* Imp: thread-safe locking */
        RDICT(v)->dump(out_, indent);
      } else out_ << "<<...>>";
    } else assert(0 && "unknown MiniPS type");
  }
}

/* --- */

/* Sat Sep  7 12:30:19 CEST 2002 */
const double MiniPS::Real::me_factor[MiniPS::Real::ME_COUNT]={
  1.0L, /* 1 bp = 1 bp (big point) */
  72.0L, /* 1 in = 72 bp (inch) */
  72.0L/72.27, /* 1 pt = 72/72.27 bp (point) */
  12.0L*72.0/72.27, /* 1 pc = 12*72/72.27 bp (pica) */
  1238.0L/1157.0*72.0/72.27, /* 1 dd = 1238/1157*72/72.27 bp (didot point) [about 1.06601110141206 bp] */
  12.0L*1238.0/1157.0*72.0/72.27, /* 1 cc = 12*1238/1157*72/72.27 bp (cicero) */
  72.0L/72.27/65536.0, /* 1 sp = 72/72.27/65536 bp (scaled point) */
  72.0L/2.54, /* 1 cm = 72/2.54 bp (centimeter) */
  7.2L/2.54, /* 1 mm = 7.2/2.54 bp (millimeter) */
};
/* Sat Sep  7 12:30:19 CEST 2002 */
char const* const MiniPS::Real::me_psfactor[MiniPS::Real::ME_COUNT]={
  "", /* 1 bp = 1 bp (big point) */
  " 72 mul", /* 1 in = 72 bp (inch) */
  " 72 mul 72.27 div", /* 1 pt = 72/72.27 bp (point) */
  " 864 mul 72.27 div", /* 1 pc = 12*72/72.27 bp (pica) */
  " 891.36 mul 836.164 div", /* 1 dd = 1238/1157*72/72.27 bp (didot point) [about 1.06601110141206 bp] */
  " 10696.32 mul 836.164 div", /* 1 cc = 12*1238/1157*72/72.27 bp (cicero) */
  " 0.72 mul 47362.8672 div", /* 1 sp = 72/72.27/65536 bp (scaled point) */
  " 72 mul 2.54 div", /* 1 cm = 72/2.54 bp (centimeter) */
  " 720 mul 254 div", /* 1 mm = 7.2/2.54 bp (millimeter) */
};
MiniPS::Real::Real(double d_, char const*ptr_, ii_t len_): d(d_), metric(0), dumpPS(false) {
  ty=T_REAL;
  char *p=new char[len_+1];
  memcpy(ptr=p, ptr_, len=len_);
  p[len_]='\0';
}
void MiniPS::Real::dump(GenBuffer::Writable &out_, bool dumpPS_force) {
  char buf[64]; /* Imp: should be enough?? */
  if (metric!=0 && (dumpPS_force || dumpPS)) {
    sprintf(buf, "%"PTS_CFG_PRINTFGLEN"g%s", d, me_psfactor[metric]);
  } else {
    sprintf(buf, "%"PTS_CFG_PRINTFGLEN"g", d*me_factor[metric]);
  }
  out_ << buf;
}
MiniPS::Real::metric_t MiniPS::Real::str2metric(char const str[2]) {
  switch (str[0]) {
   case 'b': if (str[1]=='p') return ME_bp;  break;
   case 'i': if (str[1]=='n') return ME_in;  break;
   case 'p': if (str[1]=='t') return ME_pt;
             if (str[1]=='c') return ME_pc;  break;
   case 'd': if (str[1]=='d') return ME_dd;  break;
   case 'c': if (str[1]=='c') return ME_cc;
             if (str[1]=='m') return ME_cm;  break;
   case 's': if (str[1]=='p') return ME_sp;  break;
   case 'm': if (str[1]=='m') return ME_mm;  break;
  }
  return ME_COUNT;
}
bool MiniPS::Real::isDimen(char const *str) {
  double d;
  slen_t len=strlen(str);
  if (!toReal(str, len, d)) return true;
  return len>2 && str2metric(str+len-2)!=ME_COUNT && !toReal(str, len-2, d);
}

MiniPS::String::String(char const*ptr_, ii_t len_) {
  char *p=new char[len_+1];
  memcpy(ptr=p, ptr_, len=len_);
  p[len_]='\0';
  ty=T_STRING;
}
void MiniPS::String::replace(char const*ap, slen_t alen, char const*bp, slen_t blen) {
  char *p=new char[alen+blen+1];
  memcpy(p,      ap, alen);
  memcpy(p+alen, bp, blen);
  p[alen+blen]='\0';
  delete [] (char*)ptr;
  ptr=p;
}

MiniPS::Sname::Sname(char const*ptr_, ii_t len_) {
  param_assert(len_>=1 && ptr_[0]=='/');
  char *p=new char[len_+1];
  memcpy(ptr=p, ptr_, len=len_);
  p[len_]='\0';
  ty=T_SNAME;
}
bool MiniPS::Sname::equals(Sname const&other) {
  return len==other.len && 0==memcmp(ptr, other.ptr, len);
}
bool MiniPS::Sname::equals(char const*other) {
  return 0==strcmp(1+(char*)ptr, other);
}

MiniPS::Ename::Ename(char const*ptr_, ii_t len_) {
  param_assert(len_>=1 && ptr_[0]!='/');
  char *p=new char[len_+1];
  memcpy(ptr=p, ptr_, len=len_);
  p[len_]='\0';
  ty=T_ENAME;
}
bool MiniPS::Ename::equals(Ename const&other) {
  return len==other.len && 0==memcmp(ptr, other.ptr, len);
}
bool MiniPS::Ename::equals(char const*other, slen_t otherlen) {
  return (slen_t)len==otherlen && 0==memcmp(ptr, other, otherlen);
}
bool MiniPS::Ename::equals(char const*other) {
  return 0==strcmp((char*)ptr, other);
}

MiniPS::Array::Array() {
  alloced=16;
  ptr=new VALUE[alloced=16];
  len=0;
  ty=T_ARRAY;
}
void MiniPS::Array::free() {
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) delete0(*p++);
  delete [] (VALUE*)ptr;
}
void MiniPS::Array::push(VALUE v) {
  if (len==alloced) extend(len+1);
  ((VALUE*)ptr)[len++]=v;
}
MiniPS::VALUE MiniPS::Array::get(ii_t index) {
  return (index<0 || index>=len) ? Qundef : ((VALUE*)ptr)[index];
}
void MiniPS::Array::set(ii_t index, VALUE val) {
  param_assert(index>=0 && index<len);
  MiniPS::delete0(((VALUE*)ptr)[index]);
  ((VALUE*)ptr)[index]=val;
}
void MiniPS::Array::dump(GenBuffer::Writable &out_, unsigned indent) {
  dumping=true;
  if (len==0) {
    out_ << "[]";
  } else if (len==1) {
    out_ << "[ ";
    MiniPS::dump(out_, ((VALUE*)ptr)[0], indent);
    out_ << " ]";
  } else {
    indent+=2;
    char *spaces=new char[indent];
    memset(spaces, ' ', indent);
    // spaces[indent]='\n';
    out_ << "[ % " << len << " elements\n";
    VALUE *p=(VALUE*)ptr, *pend=p+len;
    while (p!=pend) { 
      out_.vi_write(spaces, indent);
      MiniPS::dump(out_, *p++, indent);
      /*if(p!=pend)*/ out_ << "\n";
    }
    out_.vi_write(spaces, indent-=2);
    out_ << "]";
  }
  dumping=false;
}
void MiniPS::Array::extend(ii_t newlen) {
  if (newlen<=alloced) return;
  ii_t newalloced=alloced;
  assert(alloced>=0);
  while (newlen>newalloced) newalloced<<=1;
  VALUE *newptr=new VALUE[newalloced];
  memcpy(newptr, ptr, len*sizeof(VALUE));
  delete [] (VALUE*)ptr;
  ptr=newptr;
  alloced=newalloced;
  /* len remains unchanged */
}
void MiniPS::Array::getFirst(VALUE *&val) {
  if (len==0) { val=(VALUE*)NULLP; return; }
  val=(VALUE*)ptr;
}
void MiniPS::Array::getNext(VALUE *&val) {
  val++;
  if (len+(VALUE*)ptr==val) val=(VALUE*)NULLP;
}

#if USE_DICT_MAPPING
MiniPS::Dict::Dict() { /* Sun Mar 24 21:02:41 CET 2002 */
  ptr=(void*)new Mapping::H(sizeof(VALUE)+1);
  /* hash value format: a VALUE, and a flag (0 or 1) indicating touchedness */
  len=0; /* meaningless */
  ty=T_DICT;
}
void MiniPS::Dict::free() {
  char const*const* keyy; slen_t keylen; VALUE *val; bool touched;
  getFirst(keyy, keylen, val, touched);
  while (keyy!=(char const*const*)NULLP) {
    MiniPS::delete0(*val);
    getNext(keyy, keylen, val, touched);
  }
  delete (Mapping::H*)ptr;
}
void MiniPS::Dict::put(char const*key, VALUE val) {
  put(key,strlen(key),val);
}
MiniPS::VALUE MiniPS::Dict::push(char const*keys, slen_t keylen, VALUE val) {
  if (keys[0]=='/') { keys++; keylen--; }
  char *has=((Mapping::H*)ptr)->get(keys,keylen);
  VALUE ret=Qundef;
  if (has!=(char const*)NULLP) {
    memcpy(&ret, has, sizeof(VALUE));
    // printf("found=/%s.\n", keys);
    /* No delete0(); deliberately. */
    memcpy(has, &val, sizeof(VALUE)); has[sizeof(VALUE)]=0;
  } else {
    char tmp[sizeof(VALUE)+1];
    memcpy(tmp, &val, sizeof(VALUE));
    tmp[sizeof(VALUE)]=0;
    ((Mapping::H*)ptr)->set(keys,keylen,tmp);
  }
  return ret;
}
void MiniPS::Dict::put(char const*keys, slen_t keylen, VALUE val) {
  if (keys[0]=='/') { keys++; keylen--; }
  char *has=((Mapping::H*)ptr)->get(keys,keylen);
  if (has!=NULLP) {
    VALUE ret=Qundef; memcpy(&ret, has, sizeof(VALUE));
    MiniPS::delete0(ret);
    memcpy(has, &val, sizeof(VALUE)); has[sizeof(VALUE)]=0;
  } else {
    char tmp[sizeof(VALUE)+1];
    memcpy(tmp, &val, sizeof(VALUE));
    tmp[sizeof(VALUE)]=0;
    ((Mapping::H*)ptr)->set(keys,keylen,tmp);
  }
}
MiniPS::VALUE MiniPS::Dict::get(char const*keys, slen_t keylen) {
  if (keys[0]=='/') { keys++; keylen--; }
  char *has=((Mapping::H*)ptr)->get(keys,keylen);
  VALUE ret=Qundef; if (has!=NULLP) memcpy(&ret, has, sizeof(VALUE));
  return ret;
}
MiniPS::VALUE MiniPS::Dict::get1(char const*keys, slen_t keylen) {
  if (keys[0]=='/') { keys++; keylen--; }
  char *has=((Mapping::H*)ptr)->get(keys,keylen);
  VALUE ret=Qundef; if (has!=NULLP) { memcpy(&ret, has, sizeof(VALUE)); has[sizeof(VALUE)]=1; }
  return ret;
}
void MiniPS::Dict::untouch(char const*keys, slen_t keylen) {
  if (keys[0]=='/') { keys++; keylen--; }
  char *has=((Mapping::H*)ptr)->get(keys,keylen);
  if (has!=NULLP) has[sizeof(VALUE)]=0;
}
void MiniPS::Dict::getFirst(char const*const*& key, slen_t &keylen, VALUE *&val, bool &touched) {
  char *has;
  // key=(char const*const*)NULLP;return;
  ((Mapping::H*)ptr)->getFirst(key, keylen, has);
  if (key==(char const*const*)NULLP) return;
  val=PTS_align_cast(VALUE*,has);
  touched=has[sizeof(VALUE)]!=0;
}
void MiniPS::Dict::getNext (char const*const*& key, slen_t &keylen, VALUE *&val, bool &touched) {
  char *has;
  ((Mapping::H*)ptr)->getNext(key, keylen, has);
  if (key==(char const*const*)NULLP) return;
  val=PTS_align_cast(VALUE*,has);
  touched=has[sizeof(VALUE)]!=0;
}
void MiniPS::Dict::dump(GenBuffer::Writable &out_, unsigned indent, bool dump_delimiters) {
  dumping=true;
  slen_t len=((Mapping::H*)ptr)->getLength();
  if (len==0) {
    if (dump_delimiters) out_ << "<<>>";
  } else {
    char const*const* keyy; slen_t keylen; VALUE *val; bool touched;
    indent+=2;
    char *spaces=new char[indent];
    memset(spaces, ' ', indent);
    // spaces[indent]='\n';
    if (dump_delimiters) out_ << "<< % " << len << " key(s)\n";
    getFirst(keyy, keylen, val, touched);
    while (keyy!=(char const*const*)NULLP) {
      out_.vi_write(spaces, indent);
      out_.vi_putcc('/');
      out_.vi_write(*keyy, keylen); /* Imp: PDF #...-quoting */
      out_ << "  ";
      MiniPS::dump(out_, *val, indent);
      out_.vi_putcc('\n');
      getNext(keyy, keylen, val, touched);
    }
    if (dump_delimiters) { out_.vi_write(spaces, indent-=2); out_ << ">>"; }
  }
  dumping=false;
}
void MiniPS::Dict::extend(ii_t) {}

#else /* a MiniPS::Dict implementation with linear search */
MiniPS::Dict::Dict() {
  alloced=16;
  ptr=new VALUE[alloced=16];
  len=0;
  ty=T_DICT;
}
void MiniPS::Dict::free() {
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) delete0(*p++);
  delete [] (VALUE*)ptr;
}
void MiniPS::Dict::put(char const*key, VALUE val) {
  return put(key,strlen(key),val);
}
MiniPS::VALUE MiniPS::Dict::push(char const*keys, slen_t keylen, VALUE val) {
  // param_assert(key[0]=='/');
  if (keys[0]=='/') { keys++; keylen--; }
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) {
    if (MiniPS::RENAME(p[0]&~1)->equals(key,keylen)) {
      VALUE v=p[1];
      p[1]=val; return v;
    }
    p+=2;
  }
  if (len==alloced) extend(len+2);
  ((VALUE*)ptr)[len++]=(MiniPS::VALUE)new Ename(keys,keylen);
  ((VALUE*)ptr)[len++]=val;
  return Qundef;
}
void MiniPS::Dict::put(char const*keys, slen_t keylen, VALUE val) {
  // param_assert(key[0]=='/');
  if (keys[0]=='/') { keys++; keylen--; }
  //void MiniPS::Dict::put(VALUE key, VALUE val) {
  //param_assert(MiniPS::getType(key)==T_ENAME);
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) {
    if (MiniPS::RENAME(p[0]&~1)->equals(keys,keylen)) {
      MiniPS::delete0(p[1]);
      p[1]=val;
      return;
    }
    p+=2;
  }
  if (len==alloced) extend(len+2);
  ((VALUE*)ptr)[len++]=(MiniPS::VALUE)new Ename(keys,keylen);
  ((VALUE*)ptr)[len++]=val;
}
MiniPS::VALUE MiniPS::Dict::get(char const*key, slen_t keylen) {
  if (key[0]=='/') { key++; keylen--; }
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) {
    //printf("for=%s trying=%s.\n", key, MiniPS::RENAME(p[0]&~1)->begin_());
    if (MiniPS::RENAME(p[0]&~1)->equals(key, keylen)) return p[1];
    p+=2;
  }
  return Qundef;
}
MiniPS::VALUE MiniPS::Dict::get1(char const*key, slen_t keylen) {
  if (key[0]=='/') { key++; keylen--; }
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) {
    //printf("for=%s trying=%s.\n", key, MiniPS::RENAME(p[0]&~1)->begin_());
    if (MiniPS::RENAME(p[0]&~1)->equals(key,keylen)) {
      /* dirty, black magic */ p[0]|=1;
      return p[1];
    }
    p+=2;
  }
  return Qundef;
}
void MiniPS::Dict::untouch(char const*key, slen_t keylen) {
  if (key[0]=='/') { key++; keylen--; }
  VALUE *p=(VALUE*)ptr, *pend=p+len;
  while (p!=pend) {
    if (MiniPS::RENAME(p[0]&~1)->equals(key,keylen)) { p[0]&=~1; return; }
    p+=2;
  }
}
void MiniPS::Dict::getFirst(char const*const*& key, slen_t &keylen, VALUE *&val, bool &touched) {
  // assert(MiniPS::getType(((VALUE*)ptr)[0])==T_ENAME);
  if (len==0) { key=(char const*const*)NULLP; return; }
  assert(ptr!=NULLP);
  Ename *skey=(Ename*)(((VALUE*)ptr)[0]&~1);
  key=(char**)&skey->ptr;
  keylen=skey->len;
  val=((VALUE*)ptr)+1;
  touched=(((VALUE*)ptr)[0]&1)!=0;
}
void MiniPS::Dict::getNext (char const*const*& key, slen_t &keylen, VALUE *&val, bool &touched) {
  val+=2;
  if (len+(VALUE*)ptr==(VALUE*)val-1) { key=(char const*const*)NULLP; return; }
  Ename *skey=RENAME(val[-1]&~1);
  // assert(MiniPS::getType((VALUE)skey)==T_ENAME);
  key=(char**)&skey->ptr;
  keylen=skey->len;
  touched=(val[-1]&1)!=0;
}

#if 0 /* obsolete */
void MiniPS::Dict::getFirst(VALUE *&key, VALUE *&val) {
  if (len==0) { key=val=(VALUE*)NULLP; return; }
  assert(ptr!=NULLP);
  key=(VALUE*)ptr;
  val=key+1;
}
void MiniPS::Dict::getNext(VALUE *&key, VALUE *&val) {
  key+=2;
  if (len+(VALUE*)ptr==key) key=val=(VALUE*)NULLP;
                       else val=key+1;
}
#endif

void MiniPS::Dict::dump(GenBuffer::Writable &out_, unsigned indent, bool dump_delimiters) {
  assert(len>=0 && (len&1)==0);
  if (len==0) {
    if (dump_delimiters) out_ << "<<>>";
  } else {
    indent+=2;
    char *spaces=new char[indent];
    memset(spaces, ' ', indent);
    // spaces[indent]='\n';
    if (dump_delimiters) out_ << "<< % " << (len/2) << " key(s)\n";
    VALUE *p=(VALUE*)ptr, *pend=p+len;
    while (p!=pend) { 
      out_.vi_write(spaces, indent);
      MiniPS::dump(out_, *p++, indent);
      out_ << "  ";
      MiniPS::dump(out_, *p++, indent);
      /*if(p!=pend)*/ out_.vi_putcc('\n'); // out_ << "\n";
    }
    if (dump_delimiters) { out_.vi_write(spaces, indent-=2); out_ << ">>"; }
  }
}
void MiniPS::Dict::extend(ii_t newlen) {
  if (newlen<=alloced) return;
  ii_t newalloced=alloced;
  assert(alloced>=0);
  while (newlen>newalloced) newalloced<<=1;
  VALUE *newptr=new VALUE[newalloced];
  memcpy(newptr, ptr, len*sizeof(VALUE));
  delete [] (VALUE*)ptr;
  ptr=newptr;
  alloced=newalloced;
  /* len remains unchanged */
}
#endif

/* --- */

MiniPS::Parser::Parser(char const *filename_) {
  FILE *ff;
  ff=(filename_[0]=='-' && filename_[1]=='\0')? stdin: fopen(filename_, "r"); /* not "rb" */
  if (ff==NULLP) Error::sev(Error::EERROR) << "MiniPS::Parser: cannot open file: " << FNQ(filename_) << (Error*)0;
  f=(FILEP)ff;
  rd=new Files::FILER(ff);
  tok=new Tokenizer(*rd);
  master=(Parser*)NULLP;
  free_level=4;
  unread=Tokenizer::NO_UNGOT;
  depth=0;
  specRuns=(MiniPS::Dict*)NULLP;
  specRunsDelete=false;
}
MiniPS::Parser::Parser(FILEP f_) {
  f=f_;
  rd=new Files::FILER(PTS_align_cast(FILE*,f_));
  tok=new Tokenizer(*rd);
  master=(Parser*)NULLP;
  free_level=3;
  unread=Tokenizer::NO_UNGOT;
  depth=0;
  specRuns=(MiniPS::Dict*)NULLP;
  specRunsDelete=false;
}
MiniPS::Parser::Parser(GenBuffer::Readable *rd_) {
  f=(FILEP)NULLP;
  rd=rd_;
  tok=new Tokenizer(*rd);
  master=(Parser*)NULLP;
  free_level=2;
  unread=Tokenizer::NO_UNGOT;
  depth=0;
  specRuns=(MiniPS::Dict*)NULLP;
  specRunsDelete=false;
}
MiniPS::Parser::Parser(Tokenizer *tok_) {
  master=(Parser*)NULLP;
  f=(FILEP)NULLP;
  rd=(GenBuffer::Readable*)NULLP;
  tok=tok_;
  master=(Parser*)NULLP;
  free_level=0;
  unread=Tokenizer::NO_UNGOT;
  depth=0;
  specRuns=(MiniPS::Dict*)NULLP;
  specRunsDelete=false;
}
MiniPS::Parser::Parser(Parser *master_) {
  f=(FILEP)NULLP;
  rd=(GenBuffer::Readable*)NULLP;
  tok=(Tokenizer*)NULLP;
  master=master_;
  free_level=1;
  unread=Tokenizer::NO_UNGOT;
  depth=0;
  specRuns=(MiniPS::Dict*)NULLP;
  specRunsDelete=false;
}
MiniPS::Parser::~Parser() {
  /* We delete the master here! */
  if (master!=NULLP) delete master; /* recursive ~Parser() call */
  if (free_level>=2) delete tok;
  if (free_level>=3) delete rd;
  if (free_level>=4) fclose(PTS_align_cast(FILE*,f));
  if (specRunsDelete) delete0((VALUE)specRuns);
}
void MiniPS::Parser::addSpecRun(char const* filename_, GenBuffer::Readable *rd_) {
  if (specRuns==NULLP) {
    specRunsDelete=true;
    specRuns=new MiniPS::Dict();
  }
  specRuns->put(filename_, (MiniPS::VALUE)new MiniPS::Void(rd_));
}
void MiniPS::Parser::setSpecRuns(MiniPS::Dict *newSpecRuns) {
  if (newSpecRuns!=specRuns) {
    if (specRunsDelete) delete0((VALUE)specRuns);
    specRunsDelete=false;
    specRuns=newSpecRuns;
  }
}
void MiniPS::Parser::setDepth(unsigned depth_) {
  if (depth_>=MAX_DEPTH) Error::sev(Error::EERROR) << "MiniPS::Parser: `run' inclusion too deep" << (Error*)0;
  depth=depth_;
}
MiniPS::VALUE MiniPS::Parser::parse1(int closer, int sev) {
  char *beg=0; slen_t len=0; /* pacify g++-2.91 */
  Real::metric_t metric;  Real *r=0; /* pacify g++-2.91 */
  VALUE v, w;
  if (master!=NULLP) {
   from_master:
    /* vvv EOF_ALLOWED means: the master cannot close our open '>' or ']' */
    if ((v=master->parse1(EOF_ALLOWED, sev))!=Qundef) return v;
    delete master;
    master=(Parser*)NULLP;
    // fprintf(stderr, "closed master\n");
  }
  // return parse1_real(closer);

  int i=0;
  if (unread!=Tokenizer::NO_UNGOT) {
    i=unread;
    unread=Tokenizer::NO_UNGOT;
  } else i=tok->yylex();
  
  // fprintf(stderr, "i=%d i='%c'\n", i, i);

  switch (i) {
   case Tokenizer::EOFF: case ']': case '>':
    if (closer==i) return Qundef; /* EOF */
    Error::sev((Error::level_t)sev) << "MiniPS::Parser: premature EOF (early closer: " << (int)i << ')' << (Error*)0;
    return Qerror; /* parse error */
   case '(': {
     beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
     VALUE v=(VALUE)new String(beg, len); /* Imp: resolve memory leak here */
     i=tok->yylex();
     beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
     if (i!='E' || len!=3 || 0!=memcmp(beg,"run",3)) { unread=i; return v; }
     /* Process external file inclusion */
     assert(master==NULLP);
     /* Imp: prevent infinite recursion */
     if (specRuns!=NULLP && Qundef!=(w=specRuns->get(RSTRING(v)->begin_(), RSTRING(v)->getLength())))
       master=new Parser((GenBuffer::Readable*)RVOID(w)->getPtr());
       else master=new Parser(RSTRING(v)->getCstr());
     master->setDepth(depth+1);
     master->setSpecRuns(specRuns);
     goto from_master;
    }
   case '/':
    beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
    return (VALUE)new Sname(beg, len);
   case ':': /* Real with metric */
    return (VALUE)tok->lastTokVal().r;
   case '.':
    // fprintf(stderr, "d=%g\n", tok->lastTokVal().d);
    // fprintf(stderr, "b=(%s)\n", tok->lastTokVal().b());
    // assert(tok->lastTokVal().bb!=NULLP);
    // beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
    // r=new Real(tok->lastTokVal().d, beg, len);
    r=tok->lastTokVal().r;
    i=tok->yylex();
    beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
    if (i!='E' || len!=2 || (metric=Real::str2metric(beg))==Real::ME_COUNT) {
      unread=i;
    } else {
      r->setMetric(metric);
    }
    return (VALUE)r;
   case '1':
    i=tok->yylex();
    beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
    if (i!='E' || len!=2 || (metric=Real::str2metric(beg))==Real::ME_COUNT) {
      unread=i;
      return Qinteger(tok->lastTokVal().i);
    } else { /* integer with metric is auto-converted to Real */
      r=new Real(tok->lastTokVal().i, beg, len);
      r->setMetric(metric);
    }
    return (VALUE)r;
   case 'E': {
    beg=tok->lastTokVal().bb->begin_(); len=tok->lastTokVal().bb->getLength();
    // fprintf(stderr, "L=%d\n", bb->getLength());
    // assert(0);
    tok->lastTokVal().bb->term0();
    if (0==strcmp(beg,"true")) return Qtrue;
    if (0==strcmp(beg,"false")) return Qfalse;
    if (0==strcmp(beg,"null")) return Qnull;
    if (closer==EOF_ILLEGAL_POP && 0==strcmp(beg,"pop")) return Qpop;
    Error::sev((Error::level_t)sev) << "MiniPS::Parser: unknown Ename: " << (*tok->lastTokVal().bb) << (Error*)0;
    return Qerror;
    }
   case '[': {
    Array *ap=new Array();
    VALUE v;
    while (Qundef!=(v=parse1(']', sev))) { if (v==Qerror) return Qerror; ap->push(v); }
    return (VALUE)ap;
    }
   case '<': {
    Dict *ap=new Dict();
    VALUE key, val;
    while (1) {
      if (Qundef==(key=parse1('>', sev))) break;
      if (key==Qerror) return Qerror;
      if (getType(key)!=T_SNAME) {
        Error::sev(Error::EERROR) << "MiniPS::Parser: dict key must be a /name" << (Error*)0;
        return Qerror;
      }
      val=parse1(EOF_ILLEGAL_POP, sev); /* No EOF allowed here */
      if (val==Qerror) return Qerror;
      if (val!=Qpop) {
        // if (Qundef!=ap->push(RSNAME(key)->begin_(),RSNAME(key)->getLength(),val)) Error::sev(Error::EERROR) << "MiniPS::Parser: duplicate dict key" << (Error*)0;
        /* ^^^ should free if non-fatal error */
        if (Qundef!=(v=ap->push(RSNAME(key)->begin_(),RSNAME(key)->getLength(),val))) {
          Error::sev(Error::WARNING) << "MiniPS::Parser: overriding previous dict key: " << RSNAME(key)->begin_() << (Error*)0;
          MiniPS::delete0(v);
        }
      }
      MiniPS::delete0(key);
    }
    return (VALUE)ap;
    }
   default:
    assert(0);
  }
  return Qerror; /* NOTREACHED */
}

void MiniPS::scanf_dict(VALUE job, bool show_warnings, ...) {
  va_list ap;
  Dict *dict=RDICT(job);
  char *key;
  unsigned ty;
  char hex3[3];
  VALUE default_, *dst, got;
  if (getType(job)!=T_DICT) Error::sev(Error::EERROR) << "scanf_dict: dict expected" << (Error*)0;
  PTS_va_start(ap, show_warnings);
  //  "InputFile",  MiniPS::T_STRING, MiniPS::Qundef, &InputFile,
  //  "OutputFile", MiniPS::T_STRING, MiniPS::Qundef, &OutputFile,
  //  "Profile",    MiniPS::T_ARRAY,  MiniPS::Qundef, &Profiles,
  //  NULLP
  while (NULLP!=(key=va_arg(ap, char*))) {
    slen_t keylen=strlen(key);
    if (*key=='/') key++;
    ty=va_arg(ap, unsigned);
    default_=va_arg(ap, VALUE);
    dst=va_arg(ap, VALUE*);
    got=(show_warnings) ? dict->get1(key,keylen) : dict->get(key,keylen);
    if (got==Qundef) {
      got = (ty==S_SENUM) ? RDICT(default_)->get(" ",1) /* get the default value */
          : (ty==S_FUNC) ? ((VALUE(*)(VALUE))default_)(Qundef)
          : default_;
      if (got==Qundef) Error::sev(Error::EERROR) << "scanf_dict: required key missing: /" << key << (Error*)0;
      /* type of default value is unchecked deliberately */
    } else switch (ty) {
     case S_RGBSTR:
      /* Dat: red is: (\377\0\0), (#f00), (#ff0000) */
      if (getType(got)!=T_STRING || !(
             RSTRING(got)->getLength()==3 /* Imp: `transparent -red' shouldn't work */
          || RSTRING(got)->getLength()==4 && RSTRING(got)->begin_()[0]=='#' && !toHex3(RSTRING(got)->begin_()+1, hex3) && (got=(VALUE)new String(hex3, 3), true)
          || RSTRING(got)->getLength()==7 && RSTRING(got)->begin_()[0]=='#' && !toHex6(RSTRING(got)->begin_()+1, hex3) && (got=(VALUE)new String(hex3, 3), true)
          || RSTRING(got)->getLength()==6 && !toHex6(RSTRING(got)->begin_(), hex3) && (got=(VALUE)new String(hex3, 3), true)
         )) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be an RGB color triplet" << (Error*)0;
      break;
     case S_SENUM:
      if (getType(got)!=T_SNAME) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be an enum value (name)" << (Error*)0;
      got=RDICT(default_)->get(RSNAME(got)->begin_(),RSNAME(got)->getLength());
      if (got==Qundef) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be a valid enum value" << (Error*)0;
      break;
     case S_FUNC:
      got=((VALUE(*)(VALUE))default_)(got);
      if (got==Qundef) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " has invalid value" << (Error*)0;
      break;
     case S_UINTEGER:
      if ((got&1)==0 || got<Qinteger(0)) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be a non-negative integer" << (Error*)0;
      break;
     case S_ANY:
      break;
     case S_PINTEGER:
      if ((got&1)==0 || got<=Qinteger(0)) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be a positive integer" << (Error*)0;
      break;
     case S_NUMBER:
      if ((got&1)==0 && getType(got)!=T_REAL) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be real or integer" << (Error*)0;
      break;
     case S_PNUMBER:
      if ((got&1)==0 && getType(got)!=T_REAL) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be real or integer" << (Error*)0;
      if ((got&1)!=0 && got<=Qinteger(0)
       || getType(got)==T_REAL && RREAL(got)->getBp()<=0
         ) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must be positive" << (Error*)0;
      break;
     default:
      if (getType(got)!=ty) Error::sev(Error::EERROR) << "scanf_dict: key /" << key << " must have type " << getTypeStr(ty) << (Error*)0;
    }
    *dst=got;
  }
  va_end(ap);
  if (show_warnings) {
    // VALUE *keyy, *val;
    char const*const* keyy; slen_t keylen; VALUE *val; bool touched;
    dict->getFirst(keyy, keylen, val, touched);
    // fprintf(stderr, "> %p\n", keyy);
    PTS_va_start(ap, show_warnings);
    while (keyy!=(char const*const*)NULLP) {
      // fprintf(stderr, "untouch len=%u\n", keylen);
      // fprintf(stderr, "untouching key=(%s)\n", *keyy);
      if (!touched) Error::sev(Error::WARNING) << "scanf_dict: ignoring unknown key /" << SimBuffer::Static(*keyy,keylen) << (Error*)0;
               else dict->untouch(*keyy, keylen); /* undo get1 */
      dict->getNext(keyy, keylen, val, touched);
    }
    va_end(ap);
  }
}

void MiniPS::setDumpPS(MiniPS::VALUE v, bool g) {
  /* Sat Sep  7 13:18:35 CEST 2002 */
  if (getType(v)==T_REAL) RREAL(v)->setDumpPS(g);
}

bool MiniPS::isZero(MiniPS::VALUE v) {
  /* Sat Sep  7 15:12:54 CEST 2002 */
  switch (getType(v)) {
   case T_REAL: return RREAL(v)->getBp()==0;
   case T_INTEGER: return int2ii(v)==0;
  }
  Error::sev(Error::EERROR) << "isZero: number expected" << (Error*)0;
  return false; /* NOTREACHED */
}

bool MiniPS::isEq(MiniPS::VALUE v, double d) {
  double dif=0;
  switch (getType(v)) {
   case T_REAL: dif=RREAL(v)->getBp()-d; break;
   case T_INTEGER: dif=int2ii(v)-d; break;
   default: Error::sev(Error::EERROR) << "isEq: number expected" << (Error*)0;
  }
  if (dif<0.0) dif=-dif;
  /* fprintf(stderr,"dif=%g g=%d\n", dif, (dif<0.000001)); */
  return (dif<0.000001); /* Imp: ... */
}

void MiniPS::dumpScale(GenBuffer::Writable &out, VALUE v) {
  double d=0;
  switch (getType(v)) {
   case T_REAL: d=RREAL(v)->getBp(); break;
   case T_INTEGER:
    if (int2ii(v)%72==0) { out << (int2ii(v)/72); return; }
    d=int2ii(v); break;
   default: Error::sev(Error::EERROR) << "dumpScale: number expected" << (Error*)0;
  }
  char buf[64]; /* Dat: enough */
  sprintf(buf, "%"PTS_CFG_PRINTFGLEN"g", d/72.0);
  out << buf;
}

void MiniPS::dumpAdd3(GenBuffer::Writable &out, MiniPS::VALUE m, MiniPS::VALUE a, MiniPS::VALUE b, MiniPS::VALUE c, MiniPS::VALUE sub, unsigned rounding) {
  long ll;
  #if 1
    /* Sat Sep  7 15:30:28 CEST 2002 */
    bool no_real_real=true;
    double d=0, dd;
    long l=0;
    if (getType(m)==T_REAL && isEq(m,72)) /* Imp: not so exact comparison */
      m=Qinteger(72);
    MiniPS::VALUE t[5], *tt;
    t[0]=a; t[1]=m; t[2]=b; t[3]=c; t[4]=sub;
    for (tt=t;tt<t+5;tt++) switch (getType(*tt)) {
     case T_REAL:
      dd=RREAL(*tt)->getBp();
     doadd:
      if (no_real_real) {
        d=l;
        no_real_real=false;
      }
      if (tt==t+1) {
        if (dd==0.0 || d==0.0) { no_real_real=true; l=0; d=0.0; }
                          else d*=dd/72;
      } else if (tt==t+4) d-=dd;
      else d+=dd;
      break;
     case T_INTEGER:
      ll=int2ii(*tt);
      if (tt==t+1) {
        if (ll%72==0) l*=ll/72;
        else { dd=ll; goto doadd; }
      } else if (tt==t+4) l-=ll;
      else l+=ll;
      break;
     default: Error::sev(Error::EERROR) << "dumpAdd3: numbers expected" << (Error*)0;
    }
    if (no_real_real) { out << l; return; }
  #else
    /* Sat Sep  7 15:16:12 CEST 2002 */
    ...
  #endif
  if (rounding!=0) {
    ll=(long)d;
    if ((double)ll<d) ll++;
    assert((double)ll>=d); /* Imp: verify possible rounding errors */
    out << (rounding>=2 && ll<0 ? 0 : ll);
  } else {
    char buf[64]; /* Dat: enough */
    sprintf(buf, "%"PTS_CFG_PRINTFGLEN"g", d);
    out << buf;
  }
}

/* __END__ */
