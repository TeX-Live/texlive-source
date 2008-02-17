// -*- related-file-name: "../../liblcdf/permstr.cc" -*-
#ifndef LCDF_PERMSTR_HH
#define LCDF_PERMSTR_HH
#include <assert.h>
#include <stddef.h>
#include <stdarg.h>
#include <lcdf/inttypes.h>
class PermString;
inline bool operator==(PermString, PermString);
inline bool operator!=(PermString, PermString);

class PermString { struct Doodad; public:
  
    typedef Doodad *Capsule;
    // Declare a PermString::Initializer in any file in which you declare
    // static global PermStrings.
    struct Initializer { Initializer(); };
  
    PermString()			: _rep(zero_char_doodad.data) { }
    explicit PermString(char c);
    inline PermString(const char*);
    inline PermString(const char*, int);
    inline PermString(const char*, const char*);

    inline operator bool() const;
    inline bool operator!() const;
    
    inline int length() const;
    char operator[](int) const;
    const char* begin() const		{ return _rep; }
    inline const char* end() const;

    friend inline bool operator==(PermString, PermString);
    friend inline bool operator!=(PermString, PermString);
  
    const char* c_str() const		{ return _rep; }
  
    inline Capsule capsule() const;
    inline static PermString decapsule(Capsule c);
  
    friend PermString permprintf(const char*, ...);
    friend PermString vpermprintf(const char*, va_list);
    friend PermString permcat(PermString, PermString);
    friend PermString permcat(PermString, PermString, PermString);  

  private:
  
    struct Doodad {
	Doodad* next;
	int length;
	char data[2];
    };
  
    const char* _rep;
  
    PermString(Doodad* d)		: _rep(d->data) { }
    void initialize(const char*, int);
    Doodad* doodad() const { return (Doodad*)(_rep - offsetof(Doodad, data)); }
  
    friend struct PermString::Initializer;
    static void static_initialize();

    enum { NHASH = 1024 };	// must be power of 2
    static Doodad zero_char_doodad, one_char_doodad[256], *buckets[NHASH];

};


inline
PermString::PermString(const char* s)
{
    initialize(s, -1);
}

inline
PermString::PermString(const char* s, int len)
{
    initialize(s, len);
}

inline
PermString::PermString(const char* begin, const char* end)
{
    assert(end);
    initialize(begin, end > begin ? end - begin : 0);
}

inline
PermString::operator bool() const
{
    return _rep != zero_char_doodad.data;
}

inline bool
PermString::operator!() const
{
    return _rep == zero_char_doodad.data;
}

inline int
PermString::length() const
{
    return doodad()->length;
}

inline const char*
PermString::end() const
{
    return _rep + doodad()->length;
}

inline bool
operator==(PermString a, PermString b)
{
    return a._rep == b._rep;
}

bool operator==(PermString, const char*);

inline bool
operator==(const char* a, PermString b)
{
    return b == a;
}

inline bool
operator!=(PermString a, PermString b)
{
    return a._rep != b._rep;
}

inline bool
operator!=(PermString a, const char* b)
{
    return !(a == b);
}

inline bool
operator!=(const char* a, PermString b)
{
    return !(b == a);
}

inline PermString::Capsule
PermString::capsule() const
{
    return doodad();
}

inline PermString
PermString::decapsule(Capsule c)
{
    return PermString(c);
}

inline unsigned
hashcode(PermString s)
{
    return (uintptr_t)(s.c_str());
}

PermString permprintf(const char*, ...);
PermString vpermprintf(const char*, va_list);
PermString permcat(PermString, PermString);
PermString permcat(PermString, PermString, PermString);  

#endif
