// -*- related-file-name: "../../libefont/otfdata.cc" -*-
#ifndef EFONT_OTFDATA_HH
#define EFONT_OTFDATA_HH
#include <lcdf/string.hh>
#include <lcdf/vector.hh>
#include <lcdf/inttypes.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if NEED_ARPA_INET_H
# include <arpa/inet.h>
#endif
#if HAVE_BYTEORDER_H
# include <byteorder.h>
#elif HAVE_NETINET_IN_H
# include <netinet/in.h>
#elif HAVE_SYS_PARAM_H
# include <sys/param.h>
#elif !defined(WIN32)
# error "configury disaster! Report this error to ekohler@gmail.com"
#endif
#ifdef WIN32
# ifdef __MSC_VER
#  pragma warning (disable: 4290)
# endif
# include <winsock2.h>
#endif
class ErrorHandler;
namespace Efont { namespace OpenType {

#if HAVE_PRECONDITION_CHECKING
#define efont_precondition(x) assert((x))
#else
#define efont_precondition(x)
#endif

class Error {
  public:
    String description;
    Error()                             : description(String::make_stable("unspecified error")) { }
    Error(const String &d)              : description(d) { }
};

class Bounds : public Error {
  public:
    Bounds()                            : Error(String::make_stable("bounds error")) { }
};

class Format : public Error {
  public:
    Format(const String& name)          : Error(name + " format error") { }
    Format(const String& name, const String& type)
                                        : Error(name + " " + type + " format error") { }
};

class BlankTable : public Format {
  public:
    BlankTable(const String& name)      : Format(name, "blank table") { }
};

class Data {
  public:
    Data()                              { }
    Data(const String& str)             : _str(str) { _str.align(2); }
    // default copy constructor
    // default destructor

    const String& string() const        { return _str; }
    operator const String&() const      { return _str; }

    operator String::unspecified_bool_type() const { return _str; }
    const uint8_t *udata() const        { return _str.udata(); }
    int length() const                  { return _str.length(); }

    static inline uint16_t u16(const unsigned char* s);
    static inline uint16_t u16_aligned(const unsigned char* s);
    static inline int16_t s16(const unsigned char* s);
    static inline int16_t s16_aligned(const unsigned char* s);
    static inline uint32_t u32(const unsigned char* s);
    static inline uint32_t u32_aligned16(const unsigned char* s);
    static inline uint32_t u32_aligned(const unsigned char* s);
    static inline int32_t s32(const unsigned char* s);
    static inline int32_t s32_aligned16(const unsigned char* s);
    static inline int32_t s32_aligned(const unsigned char* s);

    inline uint8_t operator[](unsigned offset) const throw (Bounds);
    inline uint16_t u16(unsigned offset) const throw (Bounds);
    inline int16_t s16(unsigned offset) const throw (Bounds);
    inline uint32_t u32(unsigned offset) const throw (Bounds);
    inline int32_t s32(unsigned offset) const throw (Bounds);
    inline uint8_t operator[](int offset) const throw (Bounds);
    inline uint16_t u16(int offset) const throw (Bounds);
    inline int16_t s16(int offset) const throw (Bounds);
    inline uint32_t u32(int offset) const throw (Bounds);
    inline int32_t s32(int offset) const throw (Bounds);

    Data subtable(unsigned offset) const throw (Bounds);
    Data offset_subtable(unsigned offset_offset) const throw (Bounds);
    inline Data substring(int left, int len = -1) const throw ();

    void align_long()                   { _str.align(4); }

  private:

    String _str;

};

inline uint8_t Data::operator[](unsigned offset) const throw (Bounds) {
    if (offset >= static_cast<unsigned>(_str.length()))
        throw Bounds();
    else
        return _str[offset];
}

inline uint16_t Data::u16(const unsigned char* s) {
#if __x86__ || __x86_64__ || HAVE_INDIFFERENT_ALIGNMENT
    return ntohs(*reinterpret_cast<const uint16_t*>(s));
#else
    return (s[0] << 8) + s[1];
#endif
}

inline uint16_t Data::u16_aligned(const unsigned char* s) {
    efont_precondition((reinterpret_cast<uintptr_t>(s) & 1) == 0);
    return ntohs(*reinterpret_cast<const uint16_t*>(s));
}

inline int16_t Data::s16(const unsigned char* s) {
#if __x86__ || __x86_64__ || HAVE_INDIFFERENT_ALIGNMENT
    return ntohs(*reinterpret_cast<const int16_t*>(s));
#else
    return (int16_t) ((s[0] << 8) + s[1]);
#endif
}

inline int16_t Data::s16_aligned(const unsigned char* s) {
    efont_precondition((reinterpret_cast<uintptr_t>(s) & 1) == 0);
    return ntohs(*reinterpret_cast<const int16_t*>(s));
}

inline uint32_t Data::u32(const unsigned char* s) {
#if __x86__ || __x86_64__ || HAVE_INDIFFERENT_ALIGNMENT
    return ntohl(*reinterpret_cast<const uint32_t*>(s));
#else
    return (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3];
#endif
}

inline uint32_t Data::u32_aligned16(const unsigned char* s) {
#if __x86__ || __x86_64__ || HAVE_INDIFFERENT_ALIGNMENT
    return ntohl(*reinterpret_cast<const uint32_t*>(s));
#else
    efont_precondition((reinterpret_cast<uintptr_t>(s) & 1) == 0);
    return (u16_aligned(s) << 16) + u16_aligned(s + 2);
#endif
}

inline uint32_t Data::u32_aligned(const unsigned char* s) {
    efont_precondition((reinterpret_cast<uintptr_t>(s) & 3) == 0);
    return ntohl(*reinterpret_cast<const uint32_t*>(s));
}

inline int32_t Data::s32(const unsigned char* s) {
#if __x86__ || __x86_64__ || HAVE_INDIFFERENT_ALIGNMENT
    return ntohl(*reinterpret_cast<const int32_t*>(s));
#else
    return (int32_t) ((s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3]);
#endif
}

inline int32_t Data::s32_aligned16(const unsigned char* s) {
#if __x86__ || __x86_64__ || HAVE_INDIFFERENT_ALIGNMENT
    return ntohl(*reinterpret_cast<const int32_t*>(s));
#else
    efont_precondition((reinterpret_cast<uintptr_t>(s) & 1) == 0);
    return (int32_t) ((u16_aligned(s) << 16) + u16_aligned(s + 2));
#endif
}

inline int32_t Data::s32_aligned(const unsigned char* s) {
    efont_precondition((reinterpret_cast<uintptr_t>(s) & 3) == 0);
    return ntohl(*reinterpret_cast<const int32_t*>(s));
}

inline uint16_t Data::u16(unsigned offset) const throw (Bounds) {
    if (offset + 1 >= static_cast<unsigned>(_str.length()) || offset + 1 == 0)
        throw Bounds();
    else
        return u16_aligned(_str.udata() + offset);
}

inline int16_t Data::s16(unsigned offset) const throw (Bounds) {
    if (offset + 1 >= static_cast<unsigned>(_str.length()) || offset + 1 == 0)
        throw Bounds();
    else
        return s16_aligned(_str.udata() + offset);
}

inline uint32_t Data::u32(unsigned offset) const throw (Bounds) {
    if (offset + 3 >= static_cast<unsigned>(_str.length()) || offset + 3 < 3)
        throw Bounds();
    else
        return u32_aligned16(_str.udata() + offset);
}

inline int32_t Data::s32(unsigned offset) const throw (Bounds) {
    if (offset + 3 >= static_cast<unsigned>(_str.length()) || offset + 3 < 3)
        throw Bounds();
    else
        return s32_aligned16(_str.udata() + offset);
}

inline uint8_t Data::operator[](int offset) const throw (Bounds) {
    return (*this)[unsigned(offset)];
}

inline uint16_t Data::u16(int offset) const throw (Bounds) {
    return u16(unsigned(offset));
}

inline int16_t Data::s16(int offset) const throw (Bounds) {
    return s16(unsigned(offset));
}

inline uint32_t Data::u32(int offset) const throw (Bounds) {
    return u32(unsigned(offset));
}

inline int32_t Data::s32(int offset) const throw (Bounds) {
    return s32(unsigned(offset));
}

inline Data Data::substring(int left, int len) const throw () {
    return Data(_str.substring(left, len));
}

} // namespace Efont::OpenType
} // namespace Efont
#endif
