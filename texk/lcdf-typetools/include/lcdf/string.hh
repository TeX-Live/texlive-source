// -*- related-file-name: "../../liblcdf/string.cc" -*-
#ifndef LCDF_STRING_HH
#define LCDF_STRING_HH
#ifdef HAVE_PERMSTRING
# include <lcdf/permstr.hh>
#endif
#include <assert.h>
class StringAccum;

class String { public:
  
    // Call static_initialize() before any function which might deal with
    // Strings, and declare a String::Initializer in any file in which you
    // declare static global Strings.
    struct Initializer { Initializer(); };
    friend struct String::Initializer;
    static void static_initialize();
    static void static_cleanup();
  
    inline String();
    inline String(const String &s);
    String(const char *cc)		{ assign(cc, -1); }
    String(const char *cc, int l)	{ assign(cc, l); }
#ifdef HAVE_PERMSTRING
    inline String(PermString p);
#endif
    explicit inline String(char);
    explicit inline String(unsigned char);
    explicit String(int);
    explicit String(unsigned);
    explicit String(long);
    explicit String(unsigned long);
    explicit String(double);
    inline ~String();
  
    static const String &null_string()	{ return *null_string_p; }
    static const String &out_of_memory_string() { return *oom_string_p; }
    static const char *out_of_memory_data() { return &oom_string_data; }
    static String garbage_string(int n); // n garbage characters
    static String fill_string(int c, int n); // n copies of c
    static String stable_string(const char *, int = -1); // stable read-only memory

    bool out_of_memory() const		{ return _data == &oom_string_data; }

    int length() const			{ return _length; }
    const char *data() const		{ return _data; }
    const unsigned char *udata() const	{ return reinterpret_cast<const unsigned char *>(_data); }
    
    typedef const char *const_iterator;
    typedef const_iterator iterator;
    const_iterator begin() const	{ return _data; }
    const_iterator end() const		{ return _data + _length; }
    
    char *mutable_data();
    char *mutable_c_str();
    unsigned char *mutable_udata()	{ return reinterpret_cast<unsigned char *>(mutable_data()); }
  
    operator bool() const		{ return _length != 0; }
    bool operator!() const		{ return _length == 0; }

#ifdef HAVE_PERMSTRING
    operator PermString() const		{ return PermString(_data, _length); }
#endif
  
    const char *c_str() const;		// pointer returned is semi-transient
  
    char operator[](int i) const	{ return _data[i]; }
    char back() const			{ return _data[_length - 1]; }
    int find_left(int c, int start = 0) const;
    int find_left(const String &s, int start = 0) const;
    int find_right(int c, int start = 0x7FFFFFFF) const;
  
    bool equals(const char *, int) const;
    // bool operator==(const String &, const String &);
    // bool operator==(const String &, const char *);
    // bool operator==(const char *, const String &);
    // bool operator!=(const String &, const String &);
    // bool operator!=(const String &, const char *);
    // bool operator!=(const char *, const String &);

    int compare(const char *, int) const;
    int compare(const String &x) const	{ return compare(x._data, x._length); }
    static inline int compare(const String &a, const String &b);
    // bool operator<(const String &, const String &);
    // bool operator<=(const String &, const String &);
    // bool operator>(const String &, const String &);
    // bool operator>=(const String &, const String &);
  
    String substring(const char *begin, const char *end) const;
    String substring(int pos, int len) const;
    String substring(int pos) const	{ return substring(pos, _length); }

    String lower() const;
    String upper() const;
    String printable() const;
  
    inline String &operator=(const String &);
    inline String &operator=(const char *);
#ifdef HAVE_PERMSTRING
    inline String &operator=(PermString);
#endif

    void append(const char *, int len);
    void append_fill(int c, int len);
    char *append_garbage(int len);
    inline String &operator+=(const String &);
    inline String &operator+=(const char *);
    inline String &operator+=(char);
#ifdef HAVE_PERMSTRING
    inline String &operator+=(PermString p);
#endif

    // String operator+(String, const String &);
    // String operator+(String, const char *);
    // String operator+(const char *, const String &);
    // String operator+(String, PermString);
    // String operator+(PermString, String);
    // String operator+(PermString, const char *);
    // String operator+(const char *, PermString);
    // String operator+(PermString, PermString);
    // String operator+(String, char);

    void align(int);
  
  private:
   
    struct Memo {
	int _refcount;
	int _capacity;
	int _dirty;
	char *_real_data;
    
	inline Memo();
	inline Memo(char *, int, int);
	Memo(int, int);
	~Memo();
    };
  
    mutable const char *_data;	// mutable for c_str()
    mutable int _length;
    mutable Memo *_memo;
  
    inline String(const char *, int, Memo *);
  
    inline void assign(const String &) const;
    void assign(const char *, int);
#ifdef HAVE_PERMSTRING
    inline void assign(PermString) const;
#endif
    inline void deref() const;
    void make_out_of_memory();
  
    static Memo *null_memo;
    static Memo *permanent_memo;
    static Memo *oom_memo;
    static String *null_string_p;
    static String *oom_string_p;
    static const char oom_string_data;

    static String claim_string(char *, int, int); // claim memory

    friend class StringAccum;
  
};


inline
String::String(const char *data, int length, Memo *memo)
    : _data(data), _length(length), _memo(memo)
{
    _memo->_refcount++;
}

inline void
String::assign(const String &s) const
{
    _data = s._data;
    _length = s._length;
    _memo = s._memo;
    _memo->_refcount++;
}

inline void
String::deref() const
{
    if (--_memo->_refcount == 0)
	delete _memo;
}

inline
String::String()
    : _data(null_memo->_real_data), _length(0), _memo(null_memo)
{
    _memo->_refcount++;
}

inline
String::String(char c)
{
    assign(&c, 1);
}

inline
String::String(unsigned char c)
{
    assign(reinterpret_cast<char *>(&c), 1);
}

inline
String::String(const String &s)
{
    assign(s);
}

inline
String::~String()
{
    deref();
}

inline String
String::substring(const char *begin, const char *end) const
{
    if (begin < end && begin >= _data && end <= _data + _length)
	return String(begin, end - begin, _memo);
    else
	return String();
}

inline int
String::compare(const String &a, const String &b)
{
    return a.compare(b);
}

inline bool
operator==(const String &s1, const String &s2)
{
    return s1.equals(s2.data(), s2.length());
}

inline bool
operator==(const char *cc1, const String &s2)
{
    return s2.equals(cc1, -1);
}

inline bool
operator==(const String &s1, const char *cc2)
{
    return s1.equals(cc2, -1);
}

inline bool
operator!=(const String &s1, const String &s2)
{
    return !s1.equals(s2.data(), s2.length());
}

inline bool
operator!=(const char *cc1, const String &s2)
{
    return !s2.equals(cc1, -1);
}

inline bool
operator!=(const String &s1, const char *cc2)
{
    return !s1.equals(cc2, -1);
}

inline bool
operator<(const String &s1, const String &s2)
{
    return s1.compare(s2.data(), s2.length()) < 0;
}

inline bool
operator<=(const String &s1, const String &s2)
{
    return s1.compare(s2.data(), s2.length()) <= 0;
}

inline bool
operator>(const String &s1, const String &s2)
{
    return s1.compare(s2.data(), s2.length()) > 0;
}

inline bool
operator>=(const String &s1, const String &s2)
{
    return s1.compare(s2.data(), s2.length()) >= 0;
}


inline String &
String::operator=(const String &s)
{
    if (&s != this) {
	deref();
	assign(s);
    }
    return *this;
}

inline String &
String::operator=(const char *cc)
{
    deref();
    assign(cc, -1);
    return *this;
}

inline String &
String::operator+=(const String &s)
{
    append(s._data, s._length);
    return *this;
}

inline String &
String::operator+=(const char *cc)
{
    append(cc, -1);
    return *this;
}

inline String &
String::operator+=(char c)
{
    append_fill(c, 1);
    return *this;
}

inline String
operator+(String s1, const String &s2)
{
    s1 += s2;
    return s1;
}

inline String
operator+(String s1, const char *cc2)
{
    s1.append(cc2, -1);
    return s1;
} 

inline String
operator+(const char *cc1, const String &s2)
{
    String s1(cc1);
    s1 += s2;
    return s1;
}

inline String
operator+(String s1, char c2)
{
    s1.append(&c2, 1);
    return s1;
} 

#ifdef HAVE_PERMSTRING

inline void
String::assign(PermString p) const
{
    _data = p.c_str();
    _length = p.length();
    _memo = permanent_memo;
    _memo->_refcount++;
}

inline
String::String(PermString p)
{
    assign(p);
}

inline bool
operator==(PermString p1, const String &s2)
{
    return s2.equals(p1.c_str(), p1.length());
}

inline bool
operator==(const String &s1, PermString p2)
{
    return s1.equals(p2.c_str(), p2.length());
}

inline bool
operator!=(PermString p1, const String &s2)
{
    return !s2.equals(p1.c_str(), p1.length());
}

inline bool
operator!=(const String &s1, PermString p2)
{
    return !s1.equals(p2.c_str(), p2.length());
}

inline String &
String::operator=(PermString p)
{
    deref();
    assign(p);
    return *this;
}

inline String &
String::operator+=(PermString p)
{
    append(p.c_str(), p.length());
    return *this;
}

inline String
operator+(String s1, PermString p2)
{
    s1.append(p2.c_str(), p2.length());
    return s1;
} 

inline String
operator+(PermString p1, String s2)
{
    String s1(p1);
    return s1 + s2;
} 

inline String
operator+(PermString p1, const char *cc2)
{
    String s1(p1);
    return s1 + cc2;
} 

inline String
operator+(const char *cc1, PermString p2)
{
    String s1(cc1);
    return s1 + p2;
} 

inline String
operator+(PermString p1, PermString p2)
{
    String s1(p1);
    return s1 + p2;
}

#endif

unsigned hashcode(const String &);

#endif
