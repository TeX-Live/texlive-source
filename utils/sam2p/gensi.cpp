/*
 * gensi.cpp
 * by pts@fazekas.hu at Tue Feb 26 13:28:12 CET 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "gensi.hpp"
#include <string.h> /* strlen() */
// #include <stdarg.h> /* va_list */

void GenBuffer::iter_char_sub(char const*beg, slen_t len, void *data) {
  while (len--!=0) (1[(block_char_t**)data][0])(*beg++, 0[(void**)data]);
}

struct copydata_t {
  char *to;
  slen_t cfrom;
  slen_t clen;
  slen_t sumlen;
};
#define CD static_cast<copydata_t*>(data)
static void iter_copy_sub(char const*beg, slen_t len, void *data) {
  slen_t i;
  CD->sumlen+=len;
  if (CD->clen==0) return;
  i=CD->cfrom;
  if (i>=len) { CD->cfrom-=len; return; }
  if (i>0) { CD->cfrom=0; beg+=i; len-=i; }
  if (len>=CD->clen) { memcpy(CD->to, beg, CD->clen); CD->clen=0; return; }
  memcpy(CD->to, beg, len);
  CD->to+=len; CD->clen-=len;
}
// #include <stdio.h>
slen_t GenBuffer::copyRange(char *to, slen_t cfrom, slen_t clen) const {
  if (clen==0) return getLength();
  copydata_t cd= { to, cfrom, clen, 0 };
  each_sub(iter_copy_sub, &cd);
#if 1
  while (cd.clen--!=0) *cd.to++='\0'; /* padding */
#else
  fprintf(stderr,"cd.clen=%d\n", cd.clen);
  while (cd.clen--!=0) {
    fprintf(stderr,"padded.\n");
    *cd.to++='\0'; /* padding */
  }
#endif
  return cd.sumlen;
}

#if HAVE_LONG_LONG && NEED_LONG_LONG
#  define LONGEXT PTS_CFG_LONGEST
#else
#  define LONGEXT long
#endif

/* Imp: ensure reentrace. Maybe vi_write wants to output a number which would
 *      overwrite ours...
 */
static char numtmp[sizeof(LONGEXT)*3+2];

bool GenBuffer::toBool(bool &dst) {
  /* on  1 true  yes ja   igen igaz  be oui vrai: 1tyjibov */
  /* off 0 false no  nein nem  hamis ki non --  : 0fnhk */
  slen_t len=copyRange(numtmp, 0, 3);
  numtmp[0]|=32; numtmp[1]|=32; numtmp[2]|=32; /* poor man's uppercase */
  dst=true;
  if (len==0) return true;
  if ((numtmp[0]=='o' && numtmp[1]=='f' && numtmp[2]=='f')
   || numtmp[0]=='0' || numtmp[0]=='f' || numtmp[0]=='n' || numtmp[0]=='h'
   || numtmp[0]=='k') dst=false;
  else if ((numtmp[0]<'0' || numtmp[0]>'9') && (numtmp[0]<'a' || numtmp[0]>'z')) return true;
  return false;
}

// #include <stdio.h>

bool GenBuffer::toInteger(unsigned long &dst) {
  /* Imp: several bases (2, 8, 10 and 16), ignore _too_long_ */
  /* Imp: check for overflow! */
  slen_t len=copyRange(numtmp, 0, sizeof(numtmp));
  // fprintf(stderr,"len=%d\n", len);
  if (len>=sizeof(numtmp)) return true; /* too long */
  /* ASSERT(numtmp null-terminated) */
  char *p=numtmp;
  if (*p=='+') p++;
  unsigned long i=0;
  while (1) {
    // fprintf(stderr,"toInteger'%c'\n", *p);
    if (*p<'0' || *p>'9') break;
    i=10*i+(*p-'0');
    p++;
  }
  dst=i;
  return *p!='\0'; /* a non-digit arrived */
}
bool GenBuffer::toInteger(signed long &dst) {
  /* Imp: several bases (2, 8, 10 and 16), ignore _too_long_ */
  slen_t len=copyRange(numtmp, 0, sizeof(numtmp));
  if (len>=sizeof(numtmp)) return true; /* too long */
  /* ASSERT(numtmp null-terminated) */
  char *p=numtmp;
  bool neg=false;
  if (*p=='+') p++;
  else if (*p=='-') { neg=true; p++; }
  unsigned long i=0;
  while (1) {
    if (*p<'0' || *p>'9') break;
    i=10*i+(*p-'0');
    p++;
  }
  dst=neg?-(long)i:i;
  return *p!='\0'; /* a non-digit arrived */
}
#if HAVE_LONG_LONG && NEED_LONG_LONG
bool GenBuffer::toInteger(unsigned PTS_CFG_LONGEST &dst) {
  /* Imp: several bases (2, 8, 10 and 16), ignore _too_long_ */
  slen_t len=copyRange(numtmp, 0, sizeof(numtmp));
  if (len>=sizeof(numtmp)) return true; /* too long */
  /* ASSERT(numtmp null-terminated) */
  char *p=numtmp;
  if (*p=='+') p++;
  unsigned PTS_CFG_LONGEST i=0;
  while (1) {
    if (*p<'0' || *p>'9') break;
    i=10*i+(*p-'0');
    p++;
  }
  dst=i;
  return *p!='\0'; /* a non-digit arrived */
}
bool GenBuffer::toInteger(signed PTS_CFG_LONGEST &dst) {
  /* Imp: several bases (2, 8, 10 and 16), ignore _too_long_ */
  slen_t len=copyRange(numtmp, 0, sizeof(numtmp));
  if (len>=sizeof(numtmp)) return true; /* too long */
  /* ASSERT(numtmp null-terminated) */
  char *p=numtmp;
  bool neg=false;
  if (*p=='+') p++;
  else if (*p=='-') { neg=true; p++; }
  unsigned PTS_CFG_LONGEST i=0;
  while (1) {
    if (*p<'0' || *p>'9') break;
    i=10*i+(*p-'0');
    p++;
  }
  dst=neg?-i:i;
  return *p!='\0'; /* a non-digit arrived */
}
#endif
bool GenBuffer::toCString(char *&dst) {
  slen_t len=getLength();
  dst=new char[len+1];
  copyRange(dst, 0, len+1); /* copies the terminating '\0' automatically. */
  return false;
}

int GenBuffer::cmp(GenBuffer const& s2) const {
  Sub u1, u2;
  slen_t m;
  int i;
  first_sub(u1);
  s2.first_sub(u2);
  while (1) {
    if (u1.len==0 && u2.len==0) return 0;  /* (*this) == s2 */
    else if (u1.len==0) return -1;         /* (*this) < s2  */
    else if (u2.len==0) return  1;         /* (*this) > s2  */
    m=(u1.len<u2.len)?u1.len:u2.len;
    if (0!=(i=memcmp(u1.beg,u2.beg,m))) return i;
    if (0==(u1.len-=m))    next_sub(u1); else u1.beg+=m;
    if (0==(u2.len-=m)) s2.next_sub(u2); else u2.beg+=m;
  }
}
  
int GenBuffer::cmp(char const* u2beg, slen_t u2len) const {
  Sub u1;
  slen_t m;
  int i;
  first_sub(u1);
  while (1) {
    if (u1.len==0 && u2len==0) return 0;   /* (*this) == s2 */
    else if (u1.len==0) return -1;         /* (*this) < s2  */
    else if (u2len ==0) return  1;         /* (*this) > s2  */
    m=(u1.len<u2len)?u1.len:u2len;
    if (0!=(i=memcmp(u1.beg,u2beg,m))) return i;
    if (0==(u1.len-=m))    next_sub(u1); else u1.beg+=m;
    u2len-=m; u2beg+=m;
  }
}

int GenBuffer::cmp(char const* u2beg) const {
  return cmp(u2beg, strlen(u2beg));
}

/* --- */

bool GenBuffer::Readable::equal_content(GenBuffer::Readable &other) {
  int i1, i2;
  while (((i1=vi_getcc())&255) == ((i2=other.vi_getcc())&255) && i1!=-1) ;
  return i1==i2;
}

slen_t GenBuffer::Readable::vi_read(char *to_buf, slen_t max) {
  register char *p=to_buf;
  char *end=to_buf+max;
  int c;
  while (p!=end && 0<=(c=vi_getcc())) *p++=c;
  return p-to_buf;
}

int GenBuffer::Readable::readFill(char *to_buf, slen_t max) {
  slen_t got, sum=0;
  while (max>0 && 0<(got=vi_read(to_buf, max))) { to_buf+=got; sum+=got; max-=got; }
  return sum;
}

/* --- */

void GenBuffer::Writable::iter_write_sub(char const*beg, slen_t len, void *data) {
  if (len!=0) static_cast<GenBuffer::Writable*>(data)->vi_write(beg, len);
}

GenBuffer::Writable& GenBuffer::Writable::operator <<(char const*cstr) {
  assert(cstr!=0);
  vi_write(cstr, strlen(cstr));
  return*this;
}

GenBuffer::Writable& GenBuffer::Writable::operator <<(void const*ptr) {
  if (ptr==0) vi_write("(null)", 6);
  else {
    vi_write("(0d", 3);
    /* Imp: hexadecimal pointer output */
    *this << (PTS_INTP_T)ptr;
    vi_putcc(')');
  }
  return*this;
}

void GenBuffer::Writable::write_num(unsigned long n, unsigned zdigits) {
  if (zdigits>=sizeof(numtmp)) {
    memset(numtmp,'0',sizeof(numtmp));
    while (zdigits>2*sizeof(numtmp)) {
      vi_write(numtmp, sizeof(numtmp));
      zdigits-=sizeof(numtmp);
    }
    vi_write(numtmp, zdigits-sizeof(numtmp));
    zdigits=sizeof(numtmp);
  }
  char *j=numtmp+sizeof(numtmp), *jend=j-zdigits;
  while (j!=jend) { *--j='0'+n%10; n/=10; }
  vi_write(j, zdigits);
}
#if HAVE_LONG_LONG && NEED_LONG_LONG
void GenBuffer::Writable::write_num(unsigned PTS_CFG_LONGEST n, unsigned zdigits) {
  if (zdigits>=sizeof(numtmp)) {
    memset(numtmp,'0',sizeof(numtmp));
    while (zdigits>2*sizeof(numtmp)) {
      vi_write(numtmp, sizeof(numtmp));
      zdigits-=sizeof(numtmp);
    }
    vi_write(numtmp, zdigits-sizeof(numtmp));
    zdigits=sizeof(numtmp);
  }
  char *j=numtmp+sizeof(numtmp), *jend=j-zdigits;
  while (j!=jend) { *--j='0'+n%10; n/=10; }
  vi_write(j, zdigits);
}
#endif

void GenBuffer::Writable::write_num(unsigned long n) {
  char *j=numtmp+sizeof(numtmp);
  do *--j='0'+n%10; while ((n/=10)!=0);
  vi_write(j, numtmp+sizeof(numtmp)-j);
}
void GenBuffer::Writable::write_num(signed long nn) {
  register unsigned long n;
  char *j=numtmp+sizeof(numtmp);
  if (nn<0) {
    n=-nn; do *--j='0'+n%10; while ((n/=10)!=0);
    *--j='-';
  } else {
    n=nn; do *--j='0'+n%10; while ((n/=10)!=0);
  }
  vi_write(j, numtmp+sizeof(numtmp)-j);
}
#if HAVE_LONG_LONG && NEED_LONG_LONG
void GenBuffer::Writable::write_num(unsigned PTS_CFG_LONGEST n) {
  char *j=numtmp+sizeof(numtmp);
  do *--j='0'+n%10; while ((n/=10)!=0);
  vi_write(j, numtmp+sizeof(numtmp)-j);
}
void GenBuffer::Writable::write_num(signed PTS_CFG_LONGEST nn) {
  register unsigned PTS_CFG_LONGEST n;
  char *j=numtmp+sizeof(numtmp);
  if (nn<0) {
    n=-nn; do *--j='0'+n%10; while ((n/=10)!=0);
    *--j='-';
  } else {
    n=nn;
    do *--j='0'+n%10; while ((n/=10)!=0);
  }
  vi_write(j, numtmp+sizeof(numtmp)-j);
}
#endif

GenBuffer::Writable& GenBuffer::Writable::vformat(slen_t n, char const *fmt, va_list ap) {
  SimBuffer::B buf;
  buf.vformat(n, fmt, ap);
  vi_write(buf(), buf.getLength());
  return*this;
}
GenBuffer::Writable& GenBuffer::Writable::vformat(char const *fmt, va_list ap) {
  SimBuffer::B buf;
  buf.vformat(fmt, ap);
  vi_write(buf(), buf.getLength());
  return*this;
}
GenBuffer::Writable& GenBuffer::Writable::format(slen_t n, char const *fmt, ...) {
  va_list ap;
  PTS_va_start(ap, fmt);
  vformat(n, fmt, ap);
  va_end(ap);
  return *this;
}
GenBuffer::Writable& GenBuffer::Writable::format(char const *fmt, ...) {
  va_list ap;
  PTS_va_start(ap, fmt);
  vformat(fmt, ap);
  va_end(ap);
  return *this;
}

/* --- */

slen_t SimBuffer::Flat::copyRange(char *to, slen_t cfrom, slen_t clen) const {
  if (cfrom<len) { /* added padding BUGFIX at Fri Mar  7 20:30:07 CET 2003 */
    slen_t dlen;
    memcpy(to, beg+cfrom, dlen=cfrom+clen>len ? len-cfrom : clen);
    to+=dlen;
  }
  while (clen--!=0) *to++='\0'; /* padding */
  return len;
}

slen_t SimBuffer::Flat::findLast(char const c) const {
  char const*p;
  for (p=beg+len;p>=beg;p--) if (*p==c) return p-beg;
  return len;
}

slen_t SimBuffer::Flat::findFirst(char const c) const {
  char const*p, *end=beg+len;
  for (p=beg;p!=end;p++) if (*p==c) return p-beg;
  return len;
}

slen_t SimBuffer::Flat::findFirst(char const* s, slen_t slen) const {
  if (slen==0) return 0; /* found */
  if (slen>len) return len; /* not found */
  char const c=*s;
  char const*p=beg, *end=beg+len-slen+1;
  if (slen==1) {
    for (;p!=end;p++) if (*p==c) return p-beg;
  } else {
    for (;p!=end;p++) if (*p==c && 0==memcmp(p,s,slen)) return p-beg;
  }
  return len;
}

int SimBuffer::Flat::cmp(SimBuffer::Flat const& s2) const {
  return memcmp(beg, s2.beg, len<s2.len?len:s2.len) || (len>s2.len)-(len<s2.len);
}

int SimBuffer::Flat::cmp(char const* s2beg, slen_t s2len) const {
  return memcmp(beg, s2beg, len<s2len?len:s2len) || (len>s2len)-(len<s2len);
}

/* --- */

SimBuffer::Static::Static(char const*cstr) { beg=cstr; len=strlen(cstr); }

/* --- */

void SimBuffer::Appendable::vi_write(char const*str, slen_t slen) {
  if (slen>0) {
    char *p=vi_mkend(slen);
    memcpy(p, str, slen);
  }
}
void SimBuffer::Appendable::prepend(char const*str, slen_t slen) {
  if (slen>0) {
    char *p=vi_mkbeg(slen);
    memcpy(p, str, slen);
  }
}

/* inlined.
void SimBuffer::Appendable::vi_putcc(char c) {
  vi_mkend(1)[0]=c;
}*/

/* --- */

SimBuffer::Linked::~Linked() {
  Node *n;
  while (first!=0) {
    if (first->beg!=(char*)(first+1)) delete [] first->beg;
    n=first->next; delete [] first; first=n;
  }
}

slen_t SimBuffer::Linked::getLength() const {
  slen_t len=0;
  Node *n=first;
  while (n!=0) { len+=n->len; n=n->next; }
  return len;
}

SimBuffer::Linked::Linked(GenBuffer const& other) {
  slen_t len=other.getLength();
  Node *n=static_cast<Node*>(static_cast<void*>(new char[sizeof(Node)+len]));
  n->beg=(char*)(n+1);
  other.copyRange(n->beg, 0, len);
  n->len=len;
  n->next=0;
  first=last=n;
}
SimBuffer::Linked::Linked(char const*str) {
  slen_t len=strlen(str);
  Node *n=static_cast<Node*>(static_cast<void*>(new char[sizeof(Node)+len]));
  n->beg=(char*)(n+1);
  memcpy(n->beg, str, len);
  n->len=len;
  n->next=0;
  first=last=n;
}
SimBuffer::Linked& SimBuffer::Linked::operator=(GenBuffer const& other) {
  slen_t len=other.getLength();
  Node *n=static_cast<Node*>(static_cast<void*>(new char[sizeof(Node)+len]));
  n->beg=(char*)(n+1);
  other.copyRange(n->beg, 0, len);
  n->len=len;
  n->next=0;
  first=last=n;
  return *this;
}
SimBuffer::Linked& SimBuffer::Linked::operator=(SimBuffer::Linked const& other) {
  /* Imp: avoid code repeat */
  slen_t len=other.getLength();
  Node *n=static_cast<Node*>(static_cast<void*>(new char[sizeof(Node)+len]));
  n->beg=(char*)(n+1);
  other.copyRange(n->beg, 0, len);
  n->len=len;
  n->next=0;
  first=last=n;
  return *this;
}

void SimBuffer::Linked::each_sub(GenBuffer::block_sub_t block, void *data) const {
  Node *n=first;
  while (n!=0) { block(n->beg, n->len, data); n=n->next; }
}
void SimBuffer::Linked::first_sub(Sub &sub) const {
  if (first!=0) {
    sub.data=first->next;
    sub.beg=first->beg;
    sub.len=first->len;
  } else sub.len=0;
}
void SimBuffer::Linked::next_sub(Sub &sub) const {
  if (sub.data!=0) {
    sub.beg=static_cast<Node*>(sub.data)->beg;
    sub.len=static_cast<Node*>(sub.data)->len;
    sub.data=static_cast<Node*>(sub.data)->next;
  } else sub.len=0;
}
char *SimBuffer::Linked::vi_mkend(slen_t len) {
  Node *n=static_cast<Node*>(static_cast<void*>(new char[sizeof(Node)+len]));
  n->beg=(char*)(n+1);
  n->len=len;
  n->next=0;
  if (last==0) first=last=n; 
          else { last->next=n; last=n; }
  return n->beg;          
}
char *SimBuffer::Linked::vi_mkbeg(slen_t len) {
  Node *n=static_cast<Node*>(static_cast<void*>(new char[sizeof(Node)+len]));
  n->beg=(char*)(n+1);
  n->len=len;
  n->next=first;
  first=n;
  if (last==0) last=n;
  return n->beg;
}

/* --- */

SimBuffer::Resizable& SimBuffer::Resizable::operator=(GenBuffer const& s2) {
  vi_grow2(0, s2.getLength()-getLength(), 0, 0);
  assert(getLength()==s2.getLength());

  Sub u1, u2;
  slen_t m;
  first_sub(u1);
  s2.first_sub(u2);
  assert(!((u1.len==0) ^ (u2.len==0))); /* s1 and s2 end in the same time */
  while (u1.len!=0) {
    m=(u1.len<u2.len)?u1.len:u2.len;
    memcpy(const_cast<char*>(u1.beg), u2.beg, m);
    if (0==(u1.len-=m))    next_sub(u1); else u1.beg+=m;
    if (0==(u2.len-=m)) s2.next_sub(u2); else u2.beg+=m;
    assert(!((u1.len==0) ^ (u2.len==0))); /* s1 and s2 end in the same time */
  }
  return*this;
}

//void SimBuffer::Resizable::clear() { /* Inlined. */
//  vi_grow2(0, -getLength(), 0, 0);
//}

void SimBuffer::Resizable::keepLeft(slen_t howmuch) {
  slen_t len=getLength();
  vi_grow2(0, len>howmuch?0-len+howmuch:-(slendiff_t)len, 0, 0);
  /* ^^^ BUGFIX at Tue Jun 11 19:57:03 CEST 2002 */
}
void SimBuffer::Resizable::keepRight(slen_t howmuch) {
  slen_t len=getLength();
  vi_grow2(len>howmuch?0-len+howmuch:-(slendiff_t)len, 0, 0, 0);
  /* ^^^ BUGFIX at Tue Jun 11 19:57:03 CEST 2002 */
}
void SimBuffer::Resizable::keepSubstr(slen_t from_offset, slen_t slen) {
  slen_t len=getLength();
  if (from_offset>=len) vi_grow2(0, -(slendiff_t)len, 0, 0);
  else if (from_offset+slen>=len) vi_grow2(-(slendiff_t)from_offset, 0, 0, 0);
  else vi_grow2(-(slendiff_t)from_offset, len-from_offset-slen, 0, 0);
}

/* --- */

void SimBuffer::B::vi_grow2(slendiff_t left, slendiff_t right, char **lbeg, char **rbeg) {
  assert(alloced>=len);
  char *origbeg=const_cast<char*>(beg);
  if (left<0)  { if (len<=(slen_t)-left) len=0; else beg-=left; left=0; } /* ! */
  if (right<0) { if (len<=(slen_t)-right)len=0; else len+=right;right=0; }
  /* ^^^ BUGFIX at Tue Jun 11 16:07:56 CEST 2002 */
  assert(left>=0);
  assert(right>=0);
  slen_t newlen=left+right+len;
  assert(newlen>=len);
  char *newbeg;
  assert(alloced>=sizeof(small)/1);
  if (beg==small) {
    assert(alloced==sizeof(small));
    if (newlen>sizeof(small)) {
      assert(newlen>len);
      newbeg=new char[alloced=2*newlen];
      memcpy(newbeg+left, beg, len);
      beg=newbeg;
    }
  } else { /* beg!=small */
    // assert(len>=alloced/2); /* -- may not always be true, especially not after appending a `long' */
    if (newlen<alloced/2) { /* shrink */
      // assert(newlen<=len); /* filled-expectations are true _after_ vi_grow2, not before */
      if (newlen>sizeof(small)) {
        newbeg=new char[alloced=newlen];
        memcpy(newbeg+left, beg, len);
      } else {
        memcpy((newbeg=small)+left, beg, len);
        delete [] origbeg;
        alloced=sizeof(small);
      }
      beg=newbeg;
    } else if (newlen>alloced) { /* grow */
      assert(newlen>sizeof(small));
      assert(newlen>len);
      newbeg=new char[alloced=2*newlen];
      memcpy(newbeg+left, beg, len);
      delete [] origbeg;
      beg=newbeg;
    } else if (beg!=origbeg) { /* called with negative `left' @param */
      assert(left==0);
      memmove(origbeg, beg, len); /* Slow, may move the whole buffer. */
    }
  }
  // fprintf(stderr, "newlen=%u\n", newlen);
  len=newlen;
  if (lbeg) *lbeg=const_cast<char*>(beg);
  if (rbeg) *rbeg=const_cast<char*>(beg+newlen-right);
  assert(alloced==sizeof(small) || (alloced>sizeof(small) && len>=alloced/2));
  assert(alloced>=len);
}

SimBuffer::B::B(char const* cstr): alloced(sizeof(small)) {
  beg=small;
  slen_t len_=strlen(cstr);
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), cstr, len);
}

SimBuffer::B::B(char const* str, slen_t len_): alloced(sizeof(small)) {
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), str, len);
}

SimBuffer::B::B(SimBuffer::B const& other): GenBuffer(), SimBuffer::Resizable(), SimBuffer::Flat(), alloced(sizeof(small)) {
  beg=small;
  if (other.len>sizeof(small)) { len=0; vi_grow2(0, other.len, 0, 0); }
                     else len=other.len;
  assert(len==other.len);
  memcpy(const_cast<char*>(beg), other.beg, len);
}

SimBuffer::B::B(SimBuffer::Flat const& other): alloced(sizeof(small)) {
  beg=small;
  if (other.len>sizeof(small)) { len=0; vi_grow2(0, other.len, 0, 0); }
                     else len=other.len;
  assert(len==other.len);
  memcpy(const_cast<char*>(beg), other.beg, len);
}

SimBuffer::B::B(SimBuffer::Flat const& other,int): alloced(sizeof(small)) {
  beg=small;
  if (other.len>=sizeof(small)) { len=0; vi_grow2(0, other.len+1, 0, 0); len--; }
                     else len=other.len;
  assert(len==other.len);
  memcpy(const_cast<char*>(beg), other.beg, len);
  const_cast<char*>(beg)[len]='\0';
}

SimBuffer::B::B(SimBuffer::Flat const& other, slen_t from_offset, slen_t len_): alloced(sizeof(small)) {
  /* substr */
  beg=small;
  if (from_offset<other.len) { 
    if (from_offset+len_>other.len) len_=other.len-from_offset;
    if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                       else len=len_;
    assert(len==len_);
    memcpy(const_cast<char*>(beg), other.beg+from_offset, len);
  } else len=0;
}

SimBuffer::B::B(GenBuffer const& other): alloced(sizeof(small)) {
  slen_t len_=other.getLength();
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  other.copyRange(const_cast<char*>(beg), 0, len_);
}

/** Constructor: copy (consume) data from a readable stream. */
SimBuffer::B::B(GenBuffer::Readable &other): alloced(sizeof(small)) {
  beg=small;
  len=0;
#if 000
  operator <<(other);
#else
  B_append(other);
#endif  
}

SimBuffer::B::B(char const* as,slen_t al, char const* bs,slen_t bl): alloced(sizeof(small)) {
  slen_t len_=al+bl;
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), as, al);
  memcpy(const_cast<char*>(beg)+al, bs, bl);
}

SimBuffer::B::B(char const* as,slen_t al, char const* bs,slen_t bl,int): alloced(sizeof(small)) {
  slen_t len_=al+bl;
  beg=small;
  if (len_>=sizeof(small)) { len=0; vi_grow2(0, len_+1, 0, 0); len--; }
                      else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), as, al);
  memcpy(const_cast<char*>(beg)+al, bs, bl);
  const_cast<char*>(beg)[al+bl]='\0';
}

SimBuffer::B::B(GenBuffer const& a, GenBuffer const& b): alloced(sizeof(small)) {
  slen_t al=a.getLength();
  slen_t bl=b.getLength();
  slen_t len_=al+bl;
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  a.copyRange(const_cast<char*>(beg), 0, al);
  b.copyRange(const_cast<char*>(beg)+al, 0, bl);
}

SimBuffer::B::B(GenBuffer const& a, GenBuffer const& b, GenBuffer const& c): alloced(sizeof(small)) {
  slen_t al=a.getLength();
  slen_t bl=b.getLength();
  slen_t cl=c.getLength();
  slen_t len_=al+bl;
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  a.copyRange(const_cast<char*>(beg), 0, al);
  b.copyRange(const_cast<char*>(beg)+al, 0, bl);
  c.copyRange(const_cast<char*>(beg)+al+bl, 0, cl);
}

SimBuffer::B::B(char const* as,slen_t al, char const* bs,slen_t bl, char const* cs,slen_t cl): alloced(sizeof(small)) {
  slen_t len_=al+bl+cl;
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), as, al);
  memcpy(const_cast<char*>(beg)+al, bs, bl);
  memcpy(const_cast<char*>(beg)+al+bl, cs, cl);
}

SimBuffer::B::B(Flat const&b, char const*cs): alloced(sizeof(small)) {
  slen_t cl=strlen(cs);
  slen_t len_=b.len+cl;
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), b.beg, b.len);
  memcpy(const_cast<char*>(beg)+b.len, cs, cl);
}

SimBuffer::B::B(char const*as, Flat const&b, char const*cs): alloced(sizeof(small)) {
  slen_t al=strlen(as);
  slen_t cl=strlen(cs);
  slen_t len_=al+b.len+cl;
  beg=small;
  if (len_>sizeof(small)) { len=0; vi_grow2(0, len_, 0, 0); }
                     else len=len_;
  assert(len==len_);
  memcpy(const_cast<char*>(beg), as, al);
  memcpy(const_cast<char*>(beg)+al, b.beg, b.len);
  memcpy(const_cast<char*>(beg)+al+b.len, cs, cl);
}

/* SimBuffer::B& operator<<(SimBuffer::B& self, GenBuffer::Readable &stream); */
SimBuffer::B& SimBuffer::B::B_append(GenBuffer::Readable &stream) {
  assert(alloced>=len);
  slen_t oldlen, ask;
  ask=stream.vi_availh();
  if (ask>0) { oldlen=len; vi_grow2(0, ask+1, 0, 0); len=oldlen; }
  if (alloced!=len) len+=stream.readFill(const_cast<char*>(beg+len), alloced-len);
  while (alloced==len) { /* more data to be read */
    oldlen=len; vi_grow2(0, alloced, 0, 0); len=oldlen;
    stream.readFill(const_cast<char*>(beg+len), alloced-len);
  }
  return *this;
}

SimBuffer::B& SimBuffer::B::term0() {
  if (len==alloced) { vi_grow2(0,1,0,0); len--; }
  const_cast<char*>(beg)[len]='\0';
  return *this;
}

void SimBuffer::B::grow_set0_by(slendiff_t lendiff) {
  char *rbeg;
  vi_grow2(0, lendiff, 0, &rbeg);
  if (lendiff>0) memset(rbeg, '\0', lendiff);
}

char SimBuffer::B::getAt(slen_t idx) {
  if (idx<len) return beg[idx];
  /* grow_set0_by(idx+1-len); */
  char *rbeg;
  vi_grow2(0, idx-len+1, 0, &rbeg);
  memset(rbeg, '\0', idx-len+1);
  return '\0';
}

SimBuffer::B& SimBuffer::B::operator=(SimBuffer::Flat const& other) {
  if (&other!=this) {
    len=0;
    vi_grow2(0, other.len, 0, 0);
    memcpy(const_cast<char*>(beg), other.beg, len);
  }
  return*this;
}

SimBuffer::B& SimBuffer::B::operator=(SimBuffer::B const& other) {
  if (&other!=this) {
    len=0;
    vi_grow2(0, other.len, 0, 0);
    memcpy(const_cast<char*>(beg), other.beg, len);
  }
  return*this;
}

SimBuffer::B& SimBuffer::B::operator=(char const* cstr) {
  slen_t slen=strlen(cstr);
  len=0;
  vi_grow2(0, slen, 0, 0);
  memcpy(const_cast<char*>(beg), cstr, slen);
  return*this;
}

SimBuffer::B SimBuffer::B::substr(slen_t first, slen_t howmuch) const {
  return SimBuffer::B(*this, first, howmuch);
}
SimBuffer::B SimBuffer::B::substr(slen_t first) const {
  /* Actually, this will probably be shorter than end-begin */
  return SimBuffer::B(*this, first, len);
}
SimBuffer::B SimBuffer::B::right(slen_t howmuch) const {
  return SimBuffer::B(*this, len<=howmuch?0:len-howmuch, howmuch);
}
SimBuffer::B SimBuffer::B::left(slen_t howmuch) const {
  return SimBuffer::B(*this, 0, howmuch);
}

#if 0
void SimBuffer::B::append(char const*s, const slen_t len_) {
  char *rbeg;
  vi_grow2(0, len_, 0, &rbeg);
  memcpy(rbeg, s, len_);
}
#endif

SimBuffer::B operator+(const SimBuffer::Flat& s1, const SimBuffer::Flat& s2) {
  return SimBuffer::B(s1.beg, s1.len, s2.beg, s2.len);
}
SimBuffer::B operator+(const char *s1, const SimBuffer::Flat& s2) {
  return SimBuffer::B(s1, strlen(s1), s2.beg, s2.len);
}
SimBuffer::B operator+(const SimBuffer::Flat& s1, const char *s2) {
  return SimBuffer::B(s1.beg, s1.len, s2, strlen(s2));
}

SimBuffer::B& SimBuffer::B::operator<<(SimBuffer::Flat const& other) {
  char *d;
  vi_grow2(0, other.len, 0, &d);
  memcpy(d,other.beg,other.len);
  return*this;
}

SimBuffer::B& SimBuffer::B::operator<<(char c) {
  if (len==alloced) { vi_grow2(0, 1, 0, 0); const_cast<char*>(beg)[len-1]=c; }
               else const_cast<char*>(beg)[len++]=c;
  return*this;
}
SimBuffer::B& SimBuffer::B::operator<<(char const *s) {
  char *d;
  slen_t slen=strlen(s);
  vi_grow2(0, slen, 0, &d);
  memcpy(d,s,slen);
  return*this;
}
void SimBuffer::B::vi_write(char const*str, slen_t slen) {
  if (slen>0) {
    char *p; vi_grow2(0, slen, 0, &p);
    memcpy(p, str, slen);
  }
}

#define USGE(a,b) ((unsigned char)(a))>=((unsigned char)(b))
static inline bool is_path(char const c) {
 return c=='-' || c=='.' || c=='_' || c=='/'
     || USGE('z'-'a',c-'a')
     || USGE('Z'-'A',c-'A')
     || USGE('9'-'0',c-'0');
}
static inline bool is_safe_c(char const c) {
  return c!='\\' && c!='\'' && c!='\"' && USGE('~'-' ',c-' ');
}
static inline bool is_safe_ps(char const c) {
  return c!='\\' && c!='('  && c!=')'  && USGE('~'-' ',c-' ');
}

SimBuffer::B& SimBuffer::B::appendDump(const char c, bool dq) {
  char t[7]; register char *p=t;
  if (dq) *p++='\'';
  if (is_path(c)) {
    *p++=c;
  } else {
    *p++='\\';
    *p++=('0'+((c>>6)&3));
    *p++=('0'+((c>>3)&7));
    *p++=('0'+(c&7));
  }
  if (dq) *p++='\'';
  vi_write(t, p-t);
  return*this;
}


SimBuffer::B& SimBuffer::B::appendDump(const SimBuffer::Flat &other, bool dq) {
  slen_t rlen=dq?2:0;
  register char c; register char const*p;
  char const *pend; char *dst;
  for (p=other.beg,pend=p+other.len; p!=pend; p++) rlen+=is_path(*p)?1:4;
  vi_grow2(0, rlen, 0, &dst);
  if (dq) *dst++='"';
  for (p=other.beg,pend=p+other.len; p!=pend; p++) {
    if (is_path(c=*p)) {
      *dst++=c;
    } else {
      *dst++='\\';
      *dst++=('0'+((c>>6)&3));
      *dst++=('0'+((c>>3)&7));
      *dst++=('0'+(c&7));
    }
  }
  if (dq) *dst++='"';
  assert(dst==end_());
  return*this;
}

SimBuffer::B& SimBuffer::B::appendNpmq(const SimBuffer::Flat &other, bool dq) {
  slen_t rlen=dq?2:0;
  register char c; register char const*p;
  char const *pend; char *dst;
  for (p=other.beg,pend=p+other.len; p!=pend; p++) rlen+=is_path(*p)?1:2;
  vi_grow2(0, rlen, 0, &dst);
  if (dq) *dst++='"';
  for (p=other.beg,pend=p+other.len; p!=pend; p++) {
    if (is_path(c=*p)) {
      *dst++=c;
    } else {
      *dst++='\\';
      *dst++=c;
    }
  }
  if (dq) *dst++='"';
  assert(dst==end_());
  return*this;
}

SimBuffer::B& SimBuffer::B::appendDumpC  (const SimBuffer::Flat &other, bool dq) {
  slen_t rlen=dq?2:0;
  register char c; register char const*p;
  char const *pend; char *dst;
  for (p=other.beg,pend=p+other.len; p!=pend; p++) rlen+=is_safe_c(*p)?1:4;
  vi_grow2(0, rlen, 0, &dst);
  if (dq) *dst++='"';
  for (p=other.beg,pend=p+other.len; p!=pend; p++) {
    if (is_safe_c(c=*p)) {
      *dst++=c;
    } else {
      *dst++='\\';
      *dst++=('0'+((c>>6)&3));
      *dst++=('0'+((c>>3)&7));
      *dst++=('0'+(c&7));
    }
  }
  if (dq) *dst++='"';
  assert(dst==end_());
  return*this;
}

SimBuffer::B& SimBuffer::B::appendFnq(const SimBuffer::Flat &other, bool preminus) {
  slen_t rlen=0;
  register char c; register char const*p;
  char const *pend; char *dst;
  if (OS_COTY==COTY_WINNT || OS_COTY==COTY_WIN9X) {
    for (p=other.beg,pend=p+other.len; p!=pend; p++) {
      if ('\0'==(c=*p) || c=='"') break;
      rlen++;
    }
    if (preminus && rlen!=0 && other.beg[0]=='-') rlen+=2; /* .\ */
    vi_grow2(0, rlen+2, 0, &dst);
    *dst++='"'; /* Dat: "ab"c" ""def" is perfectly legal and parses to: `abc def' */
    p=other.beg;
    if (preminus && other.beg[0]=='-') { *dst++='.'; *dst++='\\'; }
    for (p=other.beg,pend=p+other.len; p!=pend; p++) {
      if ('\0'==(c=*p) || c=='"') break;
      *dst++=c;
    }
    *dst++='"';
  } else { /* Everything else is treated as UNIX */
    for (p=other.beg,pend=p+other.len; p!=pend; p++) {
      if ('\0'==(c=*p)) break;
      rlen+=is_path(c)?1: c=='\n'?3:2;
    }
    if (preminus && rlen!=0 && other.beg[0]=='-') rlen+=2; /* ./ */
    vi_grow2(0, rlen, 0, &dst);
    if (preminus && other.beg[0]=='-') { *dst++='.'; *dst++='/'; }
    for (p=other.beg,pend=p+other.len; p!=pend; p++) {
      if ('\0'==(c=*p)) break;
      if (is_path(c)) *dst++=c;
      else if (c=='\n') { *dst++='"'; *dst++='\n'; *dst++='"'; }
      else { *dst++='\\'; *dst++=c; }
    }
  } /* IF OS_COTY... */
  assert(dst==end_());
  return*this;
}

SimBuffer::B& SimBuffer::B::appendDumpPS  (const SimBuffer::Flat &other, bool dq) {
  slen_t rlen=dq?2:0;
  register char c; register char const*p;
  char const *pend;
  for (p=other.beg,pend=p+other.len; p!=pend; p++) rlen+=is_safe_ps(*p)?1:4;
  char *dst; vi_grow2(0, rlen, 0, &dst);
  if (dq) *dst++='(';
  for (p=other.beg,pend=p+other.len; p!=pend; p++) {
    if (is_safe_ps(c=*p)) {
      *dst++=c;
    } else {
      *dst++='\\';
      *dst++=('0'+((c>>6)&3));
      *dst++=('0'+((c>>3)&7));
      *dst++=('0'+(c&7));
    }
  }
  if (dq) *dst++=')';
  assert(dst==end_());
  return*this;
}

SimBuffer::B& SimBuffer::B::appendHppq(const SimBuffer::Flat &other) {
  vi_grow2(0, other.len, 0, 0);
  char *pend=const_cast<char*>(beg)+len;
  register char c, *p=pend-other.len;
  memcpy(p, other.beg, other.len);
  for (;p!=pend;pend++) {
    c=*p;
    *p++=(
      (c>='a' && c<='z') ? (char)(c-'a'+'A') :
      (c>='A' && c<='Z') ? c :
                           '_'
    );
  }
  return *this;
}

SimBuffer::B& SimBuffer::B::appendUnslash(const SimBuffer::Flat &other, int iniq) {
  slen_t rlen=0;
  slen_t left=other.len;
  register char c; register char const*p=other.beg;
  if (iniq<=256) {
    if (left<2 || *p!=iniq || p[left-1]!=iniq) return*this;
    /* ^^^ return empty string */
    p++; left-=2;
  }
  while (0!=left) { /* Calculate lengths */
    c=*p++;
    if (c!='\\' || left==1) { rlen++; left--; continue; }
    c=*p++;
    if (c>='0' && c<='7') {
      rlen++;
      if (left>=3 && p[0]>='0' && p[0]<='7') {
        if (left>=4 && p[1]>='0' && p[1]<='7') { p+=2; left-=4; }
                                          else { p+=1; left-=3; }
      } else left-=2;
    } else if ((c=='x' || c=='X') && left>=3 && 16!=(hexc2n(p[0]))) {
      rlen++;
      if (left>=4 && 16!=(hexc2n(p[1]))) { p+=2; left-=4; }
                                    else { p+=1; left-=2; }
    } else if ((c=='c' || c=='C') && left>=4 && (p[0]=='-' || p[0]=='[')) {
      rlen++; left-=4; p+=2;
    } else if (c=='l' && left>=3) {
      rlen++; left-=3; p++;
    } else if (c=='u' && left>=3) {
      rlen++; left-=3; p++;
    } else if (c=='\n') {
      left-=2;
    } else { /* backslash is used for escaping a single char */
      rlen++; left-=2;
    }
  }
  
  char *dst; vi_grow2(0, rlen, 0, &dst);
  unsigned tmp1, tmp2;
  left=other.len; p=other.beg;
  if (iniq<=256) {
    assert(!(left<2 || *p!=iniq || p[left-1]!=iniq));
    p++; left-=2;
  }
  while (0!=left) {
    c=*p++;
    if (c!='\\' || left==1) { *dst++=(c); left--; continue; }
    c=*p++;
    if (c>='0' && c<='7') {
      if (left>=3 && p[0]>='0' && p[0]<='7') {
        if (left>=4 && p[1]>='0' && p[1]<='7') {
          *dst++=((char)((c<<6)+(p[0]<<3)+p[1]-'0'*73));
          p+=2; left-=4;
        } else {
          *dst++=((char)((c<<3)+p[0]-'0'*9));
          p+=1; left-=3;
        }
      } else {
        *dst++=((char)(c-'0'));
        left-=2;
      }
    } else if ((c=='x' || c=='X') && left>=3 && 16!=(tmp1=hexc2n(p[0]))) {
      if (left>=4 && 16!=(tmp2=hexc2n(p[1]))) {
        *dst++=((char)((tmp1<<4)+tmp2));
        p+=2; left-=4;
      } else {
        *dst++=((char)tmp1);
        p+=1; left-=2;
      }
    } else if ((c=='c' || c=='C') && left>=4 && (p[0]=='-' || p[0]=='[')) {
      *dst++=((char)(p[1]>='a' && p[1]<='z' ? (p[1]+'A'-'a')^64 : p[1]^64));
      left-=4; p+=2;
    } else if (c=='l' && left>=3) {
      *dst++=((char)(p[0]>='A' && p[0]<='Z' ? p[0]+'a'-'A' : p[0]));
      left-=3; p++;
    } else if (c=='u' && left>=3) {
      *dst++=((char)(p[0]>='a' && p[0]<='z' ? p[0]+'A'-'a' : p[0]));
      left-=3; p++;
    } else if (c=='\n') {
      left-=2;
    } else { /* backslash is used for escaping a single char */
           if (c=='a') c=007; // \x07 (alarm bell)
      else if (c=='b') c=010; // \x08 (backspace) (_not_ alarm bell)
      else if (c=='e') c=033; // \x1B (escape)
      else if (c=='f') c=014; // \x0C (form feed)
      else if (c=='n') c=012; // \x0A (newline, line feed)
      else if (c=='r') c=015; // \x0D (carriage return)
      else if (c=='t') c=011; // \x09 (horizontal tab)
      else if (c=='v') c=013; // \x0B (vertical tab)
      *dst++=(c); left-=2;
      // if (0!=left--) { *dst++=(*p++); left--; } /* already escaped 1 */
    }
  }
  return*this;  
}

void SimBuffer::B::space_pad_cpy(char *dst, char const*src, slen_t pad) {
  while (pad!=0 && src[0]!='\0') { *dst++=*src++; pad--; }
  while (pad--!=0) *dst++=' ';
}

// #include <stdio.h>

char *SimBuffer::B::substr_grow(slen_t first, slen_t oldmuch, slen_t newmuch) {
  slen_t idx=first;
  if (first<len) {
    if (first+oldmuch>len) oldmuch=len-first;
    if (newmuch<oldmuch) { first+=newmuch; oldmuch-=newmuch; newmuch=0; }
    else if (newmuch>oldmuch) { first+=oldmuch; newmuch-=oldmuch; oldmuch=0; }
    else return const_cast<char*>(beg)+first;
  } else {
    len=first=idx=oldmuch=0;
    if (newmuch==0) { vi_grow2(0,0,0,0); return const_cast<char*>(beg); }
  }
  // fprintf(stderr, "newmuch=%u oldmuch=%u len=%u\n", newmuch, oldmuch, len);
  if (newmuch>0) {
    vi_grow2(0,newmuch,0,0);
    char *p=const_cast<char*>(beg)+first; /* after vi_grow2() */
    memmove(p+newmuch, p, len-first-newmuch);
  } else if (oldmuch>0) {
    char *p=const_cast<char*>(beg)+first; /* before vi_grow2() */
    memmove(p, p+oldmuch, len-first-oldmuch);
    vi_grow2(0,-(slendiff_t)oldmuch,0,0);
  } else assert(0);
  // fprintf(stderr, "len=%u oldmuch=%u\n", len, oldmuch);
  return const_cast<char*>(beg)+idx;
}

/* --- Tue Jul  2 10:47:14 CEST 2002 */

void GenBuffer::tolower_memcpy(char *dst, char const*s, slen_t slen) {
  while (slen--!=0) *dst++=USGE('Z'-'A',*s-'A') ? *s+++'a'-'A' : *s++;
}
void GenBuffer::toupper_memcpy(char *dst, char const*s, slen_t slen) {
  while (slen--!=0) *dst++=USGE('z'-'a',*s-'a') ? *s+++'A'-'a' : *s++;
}

int GenBuffer::nocase_memcmp(char const*a, char const *s, slen_t slen) {
  int i;
  while (slen--!=0) {
    i=(USGE('Z'-'A',*a-'A') ? *a+++'a'-'A' : *a++)
     -(USGE('Z'-'A',*s-'A') ? *s+++'a'-'A' : *s++);
    if (i>0) return 1;
    if (i<0) return -1;
  }
  return 0;
}
int GenBuffer::nocase_strcmp(char const*a, char const*b) {
  slen_t alen=strlen(a), blen=strlen(b), min=alen<blen?alen:blen;
  int ret=nocase_memcmp(a, b, min);
  return ret!=0 || alen==blen ? ret : alen<blen ? -1 : 1;
}

bool GenBuffer::parseBool(char const *s, slen_t slen) {
  if (slen==0) return true;
  char c=s[0];
  if (slen==2 && (c=='o' || c=='O') && (s[1]=='n' || s[1]=='N')
   || slen==3 && (c=='o' || c=='O') && (s[1]=='u' || s[1]=='U')
     ) return true;
  /* on  true  yes ja   igen oui enable  1 true  vrai? right sure allowed
   * off false no  nein nem  non disable 0 false faux? wrong nope disallowed
   */
  return c!='f' && c!='n' && c!='d' && c!='w' && c!='0'
      && c!='F' && c!='N' && c!='D' && c!='W';
}

bool GenBuffer::strbegins(char const*a, char const *with) {
  while (*with!='\0') if (*a++!=*with++) return false;
  return true;
}

bool GenBuffer::nocase_strbegins(char const*a, char const *with) {
  while (*with!='\0')
    if ((USGE('Z'-'A',*a-'A') ? *a+++'a'-'A' : *a++)
      !=(USGE('Z'-'A',*with-'A') ? *with+++'a'-'A' : *with++)
       ) return false;
  return true;
}

/* __END__ */
