/*
 * mapping.cpp -- associative array implementations
 * by pts@fazekas.hu at Fri Mar 15 21:13:47 CET 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "mapping.hpp"
#include "error.hpp"
#include <string.h>
#include <stdlib.h> /* abort() */

/* --- */

bool Mapping::Gen::update(char const*key, slen_t keylen, char const*data) {
  char *found_data=get(key, keylen);
  if (found_data==NULLP) return true;
  memcpy(found_data, data, datalen);
  return false;
}

/* --- */

bool Mapping::DoubleHash::obj_assert() {
  assert(len <= used);
  assert(minlen <= len);
  assert(used <= maxused);
  assert(maxused < alloced);
  assert(alloced < (slen_t)-2);
  /* Imp: re-count used and len */
  return true;
}

bool Mapping::DoubleHash::set     (char const*  key, slen_t  keylen, char const*data) {
  assert(obj_assert());
  slen_t h1=vi_h1(key,keylen);
  assert(h1<alloced);
  Ary *p=ary+h1;
  if (p->keylen==NEVER_USED) { added:
    /* Dat: we shouldn't stop on p->keylen==DELETED */
    memcpy(p->keydata=new char[keylen+datalen], data, datalen);
    memcpy(p->keydata+=datalen, key, p->keylen=keylen); len++;
    //if (p->keylen==NEVER_USED && used++==maxused) { p->keylen=keylen; rehash(); }
    //                                         else { p->keylen=keylen; }
    if (used++==maxused) rehash();
    assert(obj_assert());
    return true; /* new value added */
  }
  if (p->keylen==keylen && 0==memcmp(p->keydata, key, keylen)) { updated:
    memcpy(p->keydata-datalen, data, datalen);
    return false; /* value updated */
  }
  /* Not found for the 1st try. We have to walk. */
  slen_t h2=vi_h2(key,keylen);
  assert(1<=h2 && h1<alloced);
  while (1) { /* Dat: maxlen < alloced, so this can safely be an infinite loop. */
    if (h1>=h2) { h1-=h2; p-=h2; }
           else { h1+=alloced-h2; p+=alloced-h2; }
    if (p->keylen==NEVER_USED) goto added;
    if (p->keylen==keylen && 0==memcmp(p->keydata, key, keylen)) goto updated;
  }
}
char*Mapping::DoubleHash::get     (char const*  key, slen_t  keylen) {
  assert(obj_assert());
  slen_t h1=vi_h1(key,keylen);
  assert(h1<alloced);
  Ary *p=ary+h1;
  if (p->keylen==NEVER_USED) return (char*)NULLP;
  if (p->keylen==keylen && 0==memcmp(p->keydata, key, keylen)) return p->keydata-datalen;
  /* Not found for the 1st try. We have to walk. */
  slen_t h2=vi_h2(key,keylen);
  assert(1<=h2 && h1<alloced);
  /* Dat: assert(lnko(...)) better in vi_h2 */
  while (1) { /* Dat: maxlen < alloced, so this can safely be an infinite loop. */
    if (h1>=h2) { h1-=h2; p-=h2; }
           else { h1+=alloced-h2; p+=alloced-h2; }
    if (p->keylen==NEVER_USED) return (char*)NULLP;
    if (p->keylen==keylen && 0==memcmp(p->keydata, key, keylen)) return p->keydata-datalen;
  }
}
bool Mapping::DoubleHash::deletee (char const*  key, slen_t  keylen) {
  /* We must not set `p->keylen=NEVER_USED' in this method because this would
   * ruin searches jumping on `p', but not coming from `p+h2'. So we just set
   * `p->keylen=DELETED'.
   */
  assert(obj_assert());
  slen_t h1=vi_h1(key,keylen);
  assert(h1<alloced);
  Ary *p=ary+h1;
  if (p->keylen==NEVER_USED) return true; /* key to be deleted does not exist */
  if (p->keylen==keylen && 0==memcmp(p->keydata, key, keylen)) { found:
    vi_dtor(p->keydata-datalen);
    delete [] (p->keydata-datalen);
    p->keylen=(slen_t)DELETED;
    // p->keydata=(char*)NULLP;
    if (len--==minlen) rehash();
    assert(obj_assert());
    return false; /* key-value pair deleted */
  }
  /* Not found for the 1st try. We have to walk. */
  slen_t h2=vi_h2(key,keylen);
  assert(1<=h2 && h1<alloced);
  while (1) { /* Dat: maxlen < alloced, so this can safely be an infinite loop. */
    if (h1>=h2) { h1-=h2; p-=h2; }
           else { h1+=alloced-h2; p+=alloced-h2; }
    if (p->keylen==(slen_t)NEVER_USED) return true; /* key to be deleted does not exist */
    if (p->keylen==keylen && 0==memcmp(p->keydata, key, keylen)) goto found;
  }
}
void Mapping::DoubleHash::getFirst(char const*const*& key, slen_t &keylen, char *& data) {
  assert(obj_assert());
  Ary *p=ary, *pend=p+alloced;
  while (p!=pend && p->keylen>=(slen_t)-2) p++;
  if (p==pend) { key=(char const*const*)NULLP; }
          else { key=&p->keydata; data=p->keydata-datalen; keylen=p->keylen; }
}
void Mapping::DoubleHash::getNext (char const*const*& key, slen_t &keylen, char *& data) {
  assert(obj_assert());
  /* vvv Dat: this operation is safe despite of the fact that it increases
   *          signedness off pointer target type ((char*) -> (Ary*)).
   */
  Ary *p=PTS_align_cast(Ary*,
   ( (char*)const_cast<char**>(key)
     - ((char*)&((Ary*)0)->keydata-(char*)0) )), *pend=ary+alloced;
  p++;
  while (p!=pend && p->keylen>=(slen_t)-2) p++;
  if (p==pend) { key=(char const*const*)NULLP; }
          else { key=&p->keydata; data=p->keydata-datalen; keylen=p->keylen; }
}
void Mapping::DoubleHash::rehash() {
  unsigned old_scale=scale;
  slen_t old_alloced=alloced;
#ifndef NDEBUG
  slen_t old_len=len;
  slen_t old_used=used;
#endif
  Ary *old_ary=ary;
  // printf("rehash minlen=%u len=%u used=%u maxused=%u alloced=%u\n", minlen, len, used, maxused, alloced);
  vi_scale();
  // printf("scaled minlen=%u len=%u used=%u maxused=%u alloced=%u\n", minlen, len, used, maxused, alloced);
  // assert( minlen <= len);
  if (scale==old_scale && used<=maxused) { assert(obj_assert()); return; }
  Ary *pp=old_ary, *ppend=pp+old_alloced;
  slen_t calclen=0, calcused=0;
  ary=new Ary[alloced];
  // printf("alloced=%u\n", alloced);
  memset(ary, '\377', sizeof(Ary)*alloced); /* fill with NEVER_USED */
  /* insert the old values to the new hash */
  for (; pp!=ppend; pp++) {
    if (pp->keylen!=NEVER_USED) calcused++;
    if (pp->keylen<(slen_t)-2) {
      // printf("%u %u\n", (unsigned char)pp->keydata[0], (unsigned char)pp->keydata[1]);
      calclen++;
      slen_t h1=vi_h1(pp->keydata,pp->keylen);
      assert(h1<alloced);
      Ary *p=ary+h1;
      assert(p->keylen!=DELETED);
      if (p->keylen==NEVER_USED) { found:
        p->keylen=pp->keylen;
        p->keydata=pp->keydata;
        continue;
      }
      assert(!(p->keylen==pp->keylen && 0==memcmp(p->keydata, pp->keydata, pp->keylen)) && "dupicate key");
      /* Not found for the 1st try. We have to walk. */
      slen_t h2=vi_h2(pp->keydata,pp->keylen);
      assert(1<=h2 && h1<alloced);
      while (1) { /* Dat: maxlen < alloced, so this can safely be an infinite loop. */
        if (h1>=h2) { h1-=h2; p-=h2; }
               else { h1+=alloced-h2; p+=alloced-h2; }
        assert(p->keylen!=DELETED);
        if (p->keylen==NEVER_USED) goto found;
        assert(!(p->keylen==pp->keylen && 0==memcmp(p->keydata, pp->keydata, pp->keylen)) && "dupicate key");
      } /* WHILE */
    }
  } /* NEXT */
  used=len=calclen;
  assert(calclen==old_len);
  assert(calcused==old_used);
  assert(old_len==len);
  assert(old_len==used);
  assert(obj_assert());
  delete [] old_ary;
}
void Mapping::DoubleHash::clear() {
  assert(obj_assert());
  Ary *pp=ary, *ppend=pp+alloced;
  for (; pp!=ppend; pp++) if (pp->keylen<(slen_t)-2) {
    vi_dtor(pp->keydata-datalen); /* this is quite fast */
    delete [] (pp->keydata-datalen); /* this is really slow for millions of values */
    pp->keylen=(slen_t)DELETED;
    len--;
  }
  assert(len==0);
  if (minlen!=0) rehash();
}

/* --- */

static slen_t const d15_primes[]= {
  0, 10, 13, 23, 37, 59, 89, 137, 211, 317, 479, 719, 1087, 1637, 2459, 3691,
  5557, 8353, 12539, 18839, 28277, 42433, 63659,
  95507UL, 143261UL, 214913UL, 322397UL, 483611UL, 725423UL, 1088159UL, 1632259UL, 2448389UL,
  3672593UL, 5508913UL, 8263373UL, 12395069UL, 18592631UL, 27888947UL, 41833427UL,
  62750147UL, 94125247UL, 141187901UL, 211781873UL, 317672813UL, 476509223UL,
  714763843UL, 1072145771UL, 1608218669UL, 2412328031UL, 3618492049UL,
#if SIZEOF_SLEN_T>=8  
  5427738097UL, 8141607167UL, 12212410753UL, 18318616157UL, 27477924239UL,
  41216886467UL, 61825329719UL, 92737994593UL, 139106991917UL, 208660487887UL,
  312990731839UL, 469486097807UL, 704229146717UL, 1056343720093UL,
  1584515580187UL, 2376773370313UL, 3565160055487UL, 5347740083243UL,
  8021610124867UL, 12032415187319UL, 18048622780987UL, 27072934171487UL,
  40609401257291UL, 60914101885951UL, 91371152828947UL, 137056729243439UL,
  205585093865189UL, 308377640797829UL, 462566461196803UL,
  693849691795229UL, 1040774537692907UL, 1561161806539393UL,
  2341742709809177UL, 3512614064713777UL, 5268921097070759UL,
  7903381645606193UL, 11855072468409349UL, 17782608702614033UL,
  26673913053921061UL, 40010869580881603UL, 60016304371322423UL,
  90024456556983643UL, 135036684835475519UL, 202555027253213279UL,
  303832540879819943UL, 455748811319729951UL, 683623216979594929UL,
  1025434825469392417UL, 1538152238204088631UL, 2307228357306132983UL,
  3460842535959199481UL, 5191263803938799269UL,
#endif
  0
};

void Mapping::DoubleHash15::vi_scale() {
#if 0
  // <lloced=63659 used=59671 maxused=59670
  // alloced=1637 used=59671 maxused=1534

  // <lloced=1637 used=1535 maxused=1534
  // |lloced=2459 used=1535 maxused=1534
  // |lloced=1637 used=1535 maxused=1534
  // alloced=1637 used=1535 maxused=1534


  printf("<lloced=%u used=%u maxused=%u\n", alloced, used, maxused);
  if (len<minlen) { again:
    /* assert(used==minlen-1); */
    assert(scale>=1);
    alloced=(d15_primes+2)[--scale];
    if (scale>1) minlen=(d15_primes+2)[scale-2];
    else if (scale==1) minlen=10;
    else minlen=0;
    if (len<minlen) goto again;
  } else if (used>maxused) {
    assert(used==maxused+1);
    if ((alloced=(d15_primes+2)[++scale])==0) abort(); /* Imp: better overflow reporting */
    if (scale>1) minlen=(d15_primes+2)[scale-2];
    else if (scale==1) minlen=10;
    else minlen=0;
    // if (len<minlen) minlen=len;
  } else assert(0);
  printf("|lloced=%u used=%u maxused=%u\n", alloced, used, maxused);
  if (alloced<=4000U) maxused=(alloced*15)>>4;
                 else maxused=(alloced>>4)*15;
  if (used>maxused) printf("alloced=%u used=%u maxused=%u\n", alloced, used, maxused);
#endif

  /* Dat: we respect only `len', not used. */
  /* Imp: make calculation of scale relative to previous one */
  
  scale=0; while (1) {
    // printf("xscale=%u\n", scale);
    if (0==(alloced=(d15_primes+2)[scale])) { assert(0); abort(); } /* Imp: better overflow reporting */
    if (alloced<=4000U) maxused=(alloced*15)>>4;
                   else maxused=(alloced>>4)*15; /* avoid overflows */
    if (len<=maxused) break; /* _not_ used<=maxused, because of rehash() */
    scale++;
  }
  minlen=(d15_primes)[scale];
  // printf("ret alloced=%u used=%u maxused=%u\n", alloced, used, maxused);
}
slen_t Mapping::DoubleHash15::vi_h1(register char const*key, register slen_t keylen) {
  register slen_t hval=0, m=alloced;
  while (keylen--!=0) hval=((hval<<8)+*key++)%m;
  /* Imp: make this accurate and faster (the `%' operation is rather slow) */
  return hval;
}
slen_t Mapping::DoubleHash15::vi_h2(char const*key, slen_t keylen) {
  register slen_t hval=0, m=alloced-1;
  while (keylen--!=0) hval=((hval<<8)+*key++)%m;
  /* Imp: make this accurate and faster (the `%' operation is rather slow) */
  return hval+1;
}
void Mapping::DoubleHash15::vi_dtor(char *) {}

Mapping::DoubleHash15::DoubleHash15(slen_t datalen_) {
  ary=new Ary[alloced=(d15_primes+2)[scale=0]];
  memset(ary, '\377', sizeof(Ary)*alloced); /* fill with NEVER_USED */
  datalen=datalen_;
  len=used=minlen=0;
  maxused=(alloced>>4)*15;
}
Mapping::DoubleHash15::~DoubleHash15() {
  clear();
  delete [] ary;
}

/* __END__ */
