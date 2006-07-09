/* mapping.hpp -- an associative array that maps strings into arbitrary fixed-length values
 * by pts@fazekas.hu at Sat Mar 23 16:01:55 CET 2002
 */

#ifdef __GNUC__
#pragma interface
#endif

#ifndef MAPPING_HPP
#define MAPPING_HPP 1

#include "config2.h"
#include "gensi.hpp"

class Mapping {
  /** A mapping is an assicative array whose keys are arbitrary binary strings
   * and its values are arbirary fixed-length data. The Mapping class is
   * abstract. The longest key which can be stored in a Mapping::Gen has
   * length `(slen_t)-3'. The caller must verify that she doesn't try to
   * call us with a longer string.
   */
  class Gen {
   public:
    /** Returns the length of one data value (same for all values, determined
     * at construction of Gen).
     */
    inline slen_t getDataLen() const { return datalen; }
    /** Returns the number of keys currently in this Mapping. */
    inline slen_t getLength() const { return len; }
    /** Returns the number of key positions allocated for this Mapping. This
     * number may or may not have meaning, depending on the implementation.
     */
    inline slen_t getAlloced() const { return len; }
    /**
     * Updates or extends the mapping with the specified key/value pair.
     * @param data must point to an area of length getDataLen(). That
     *        area will be copied (memcpy()) by set().
     * @return true if key previously hasn't existed
     */
    virtual bool set(char const*key, slen_t keylen, char const*data) =0;
    /** @return the address of the data that corresponds to param
     * `key', or NULLP if `key' was not found. The returned value is valid
     * until the next modification (.set(), deletee() methods).
     */
    virtual char* get(char const*key, slen_t keylen) =0;
    /**
     * Updates mapping with the specified key/value pair. Leaves it unchanged
     * if the specified key doesn't exist. Calls get() and memcpy() to do the
     * job.
     * @param data must point to an area of length getDataLen(). That
     *        area will be copied by set().
     * @return true if key previously hasn't existed
     */
    bool update(char const*key, slen_t keylen, char const*data);
    
    /** Calls get to do the job. */
    inline bool exists(char const*key, slen_t keylen) { return NULLP!=get(key, keylen); }
    
    /**
     * Deletes the specified `key' from the mapping.
     * @param data must point to an area of length getDataLen(). That
     *        area will be copied by set().
     * @return true if key previously hasn't existed
     */
    virtual bool deletee(char const*key, slen_t keylen) =0;
    
    /** The user must ensure that she isn't updating the mapping while walking.
     * @param key (out) is set to NULLP on empty mapping.
     */
    virtual void getFirst(char const*const*& key, slen_t &keylen, char *& data) =0;
    /** The user must ensure that she isn't updating the mapping while walking.
     * @param key (out) is set to NULLP if there are no more elements
     */
    virtual void getNext (char const*const*& key, slen_t &keylen, char *& data) =0;

   protected:
    slen_t datalen, len, alloced;
  };
  
  /** Double hashing. Still abstract. */
  class DoubleHash: public Gen {
   public:
    /** If it modifies `scale', then it must modify alloced, may modify minlen
     * and maxused, and must not modify anything else. If it does not modify
     * `scale', it must not modify anything else. After it returns,
     * obj_assert() must hold. Must not copy or (re)allocate memory,
     * rehashing is not done by vi_scale(). rehash() calls vi_scale(), and
     * rehash() does the real reallocation.
     */
    virtual void vi_scale() =0;
    /** First hash function.  Must return a value in 0..alloced-1 */
    virtual slen_t vi_h1(char const*key, slen_t keylen) =0;
    /** Second hash function. Must return a value in 1..alloced-1, which is
     * realatively prime to the value returned by vi_h2().
     */
    virtual slen_t vi_h2(char const*key, slen_t keylen) =0;
    /** Destruct and free resources used by data. Called by deletee(). */
    virtual void vi_dtor(char *data) =0;
    
    /** Called by set() and deletee() ?? */
    virtual bool set     (char const*  key, slen_t  keylen, char const*data);
    virtual char*get     (char const*  key, slen_t  keylen);
    virtual bool deletee (char const*  key, slen_t  keylen);
    virtual void getFirst(char const*const*&key, slen_t &keylen, char *& data);
    virtual void getNext (char const*const*&key, slen_t &keylen, char *& data);
    /** Delete everything. */
    void clear();
   protected:
    void rehash();
    /** @return true */
    bool obj_assert();
    /** `keylen==(slen_t)-1' indicates a place never used,
     *  `keylen==(slen_t)-2' indicates a deleted place
     * Before changing the value of NEVER_USED, please update the memset(...)
     * in rehash().
     */
    BEGIN_STATIC_ENUM1(slen_t) NEVER_USED=(slen_t)-1, DELETED=(slen_t)-2 END_STATIC_ENUM()
    // static const slen_t NEVER_USED=(slen_t)-1, DELETED=(slen_t)-2;
    struct Ary {
      slen_t keylen;
      /** key is located at keydata, data is located keydata-datalen */
      char *keydata;
    } *ary;
    /* The minimum number of keys before rehashing. */
    slen_t minlen;
    /* The maximum number of keys before rehashing. */
    slen_t maxused;
    /* The number of used places. A place is used unless it is NEVER_USED */
    slen_t used;
    /** Used by the implementor of vi_scale. */
    unsigned scale;
  };
  
  /** Simple prime modulus double hashing with a factor of around 1.5
   * between `alloced' scales. This class is not abstract anymore.
   */
  class DoubleHash15: public DoubleHash {
   public:
    DoubleHash15(slen_t datalen_);
    virtual ~DoubleHash15();
    virtual void vi_scale();
    /** @return key % Prime */
    virtual slen_t vi_h1(char const*key, slen_t keylen);
    /** @return (key % (Prime-1))+1 */
    virtual slen_t vi_h2(char const*key, slen_t keylen);
    /** No-op. */
    virtual void vi_dtor(char *data);
  };
 public:  
  typedef DoubleHash15 H;
};

#endif
