// -*- related-file-name: "../include/lcdf/string.hh" -*-
/*
 * string.{cc,hh} -- a String class with shared substrings
 * Eddie Kohler
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 * Copyright (c) 2001-2010 Eddie Kohler
 * Copyright (c) 2008 Meraki, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <lcdf/string.hh>
#include <lcdf/straccum.hh>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <lcdf/inttypes.h>

/** @file string.hh
 * @brief The LCDF String class.
 */

/** @class String
 * @brief A string of characters.
 *
 * The String class represents a string of characters.  Strings may be
 * constructed from C strings, characters, numbers, and so forth.  They may
 * also be added together.  The underlying character arrays are dynamically
 * allocated; String operations allocate and free memory as needed.  A String
 * and its substrings generally share memory.  Accessing a character by index
 * takes O(1) time; so does creating a substring.
 *
 * <h3>Initialization</h3>
 *
 * The String implementation must be explicitly initialized before use; see
 * static_initialize().  Explicit initialization is used because static
 * constructors and other automatic initialization tricks don't work in the
 * kernel.  However, at user level, you can declare a String::Initializer
 * object to initialize the library.
 *
 * <h3>Out-of-memory strings</h3>
 *
 * When there is not enough memory to create a particular string, a special
 * "out-of-memory" string is returned instead.  Out-of-memory strings are
 * contagious: the result of any concatenation operation involving an
 * out-of-memory string is another out-of-memory string.  Thus, the final
 * result of a series of String operations will be an out-of-memory string,
 * even if the out-of-memory condition occurs in the middle.
 *
 * Out-of-memory strings have zero characters, but they aren't equal to other
 * empty strings.  If @a s is a normal String (even an empty string), and @a
 * oom is an out-of-memory string, then @a s @< @a oom.
 *
 * All out-of-memory strings are equal and share the same data(), which is
 * different from the data() of any other string.  See
 * String::out_of_memory_data().  The String::make_out_of_memory() function
 * returns an out-of-memory string.
 */

const char String::null_string_data = 0;
const char String::oom_string_data = 0;
const char String::bool_data[] = "true\0false";

String::memo_t String::null_memo = {
    2, 0, 0, const_cast<char *>(&null_string_data)
};
String::memo_t String::permanent_memo = {
    1, 0, 0, const_cast<char *>(&null_string_data)
};
String::memo_t String::oom_memo = {
    2, 0, 0, const_cast<char *>(&oom_string_data)
};

const String::rep_t String::null_string_rep = {
    &null_string_data, 0, &null_memo
};
const String::rep_t String::oom_string_rep = {
    &oom_string_data, 0, &oom_memo
};

/** @cond never */
String::memo_t *
String::create_memo(char *data, int dirty, int capacity)
{
    assert(capacity >= dirty);
    memo_t *memo = new memo_t;
    if (memo) {
	if (data)
	    memo->real_data = data;
	else if (!(memo->real_data = new char[capacity])) {
	    delete memo;
	    return 0;
	}
	memo->capacity = capacity;
	memo->dirty = dirty;
	memo->refcount = (data ? 0 : 1);
    }
    return memo;
}

void
String::delete_memo(memo_t *memo)
{
    if (memo->capacity) {
	assert(memo->capacity >= memo->dirty);
	delete[] memo->real_data;
    }
    delete memo;
}
/** @endcond never */


String::String(int i)
{
    char buf[128];
    sprintf(buf, "%d", i);
    assign(buf, -1, false);
}

String::String(unsigned u)
{
    char buf[128];
    sprintf(buf, "%u", u);
    assign(buf, -1, false);
}

String::String(long i)
{
    char buf[128];
    sprintf(buf, "%ld", i);
    assign(buf, -1, false);
}

String::String(unsigned long u)
{
    char buf[128];
    sprintf(buf, "%lu", u);
    assign(buf, -1, false);
}

String::String(double d)
{
    char buf[128];
    int len = sprintf(buf, "%.12g", d);
    assign(buf, len, false);
}

String
String::make_claim(char *str, int len, int capacity)
{
    assert(str && len > 0 && capacity >= len);
    if (memo_t *new_memo = create_memo(str, len, capacity))
	return String(str, len, new_memo);
    else
	return String(&oom_string_data, 0, &oom_memo);
}

String
String::make_stable(const char *s, int len)
{
    if (len < 0)
	len = (s ? strlen(s) : 0);
    if (len == 0)
	return String();
    else
	return String(s, len, &permanent_memo);
}

String
String::make_garbage(int len)
{
    String s;
    s.append_garbage(len);
    return s;
}

String
String::make_fill(int c, int len)
{
    String s;
    s.append_fill(c, len);
    return s;
}

void
String::assign_out_of_memory()
{
    if (_r.memo)
	deref();
    _r.memo = &oom_memo;
    _r.data = _r.memo->real_data;
    _r.length = 0;
    ++_r.memo->refcount;
}

void
String::assign(const char *str, int len, bool need_deref)
{
    if (!str) {
	assert(len <= 0);
	len = 0;
    } else if (len < 0)
	len = strlen(str);

    // need to start with dereference
    if (need_deref) {
	if (str >= _r.memo->real_data
	    && str + len <= _r.memo->real_data + _r.memo->capacity) {
	    // Be careful about "String s = ...; s = s.c_str();"
	    _r.data = str;
	    _r.length = len;
	    return;
	} else
	    deref();
    }

    if (len == 0) {
	_r.memo = (str == &oom_string_data ? &oom_memo : &null_memo);
	++_r.memo->refcount;

    } else {
	// Make 'capacity' a multiple of 16 characters and bigger than 'len'.
	int capacity = (len + 16) & ~15;
	_r.memo = create_memo(0, len, capacity);
	if (!_r.memo) {
	    assign_out_of_memory();
	    return;
	}
	memcpy(_r.memo->real_data, str, len);
    }

    _r.data = _r.memo->real_data;
    _r.length = len;
}

char *
String::append_garbage(int len)
{
    // Appending anything to "out of memory" leaves it as "out of memory"
    if (len <= 0 || _r.memo == &oom_memo)
	return 0;

    // If we can, append into unused space. First, we check that there's
    // enough unused space for 'len' characters to fit; then, we check
    // that the unused space immediately follows the data in '*this'.
    uint32_t dirty = _r.memo->dirty;
    if (_r.memo->capacity > dirty + len) {
	char *real_dirty = _r.memo->real_data + dirty;
	if (real_dirty == _r.data + _r.length) {
	    _r.length += len;
	    _r.memo->dirty = dirty + len;
	    assert(_r.memo->dirty < _r.memo->capacity);
	    return real_dirty;
	}
    }

    // Now we have to make new space. Make sure the new capacity is a
    // multiple of 16 characters and that it is at least 16. But for large
    // strings, allocate a power of 2, since power-of-2 sizes minimize waste
    // in frequently-used allocators, like Linux kmalloc.
    int new_capacity = (_r.length + len < 1024 ? (_r.length + 16) & ~15 : 1024);
    while (new_capacity < _r.length + len)
	new_capacity *= 2;

    memo_t *new_memo = create_memo(0, _r.length + len, new_capacity);
    if (!new_memo) {
	assign_out_of_memory();
	return 0;
    }

    char *new_data = new_memo->real_data;
    memcpy(new_data, _r.data, _r.length);

    deref();
    _r.data = new_data;
    new_data += _r.length;	// now new_data points to the garbage
    _r.length += len;
    _r.memo = new_memo;
    return new_data;
}

void
String::append(const char *s, int len)
{
    if (!s) {
	assert(len <= 0);
	len = 0;
    } else if (len < 0)
	len = strlen(s);

    if (s == &oom_string_data)
	// Appending "out of memory" to a regular string makes it "out of
	// memory"
	assign_out_of_memory();
    else if (len == 0)
	/* do nothing */;
    else if (!(s >= _r.memo->real_data
	       && s + len <= _r.memo->real_data + _r.memo->capacity)) {
	if (char *space = append_garbage(len))
	    memcpy(space, s, len);
    } else {
	String preserve_s(*this);
	if (char *space = append_garbage(len))
	    memcpy(space, s, len);
    }
}

void
String::append_fill(int c, int len)
{
    assert(len >= 0);
    if (char *space = append_garbage(len))
	memset(space, c, len);
}

char *
String::mutable_data()
{
    // If _memo has a capacity (it's not one of the special strings) and it's
    // uniquely referenced, return _data right away.
    if (_r.memo->capacity && _r.memo->refcount == 1)
	return const_cast<char *>(_r.data);

    // Otherwise, make a copy of it. Rely on: deref() doesn't change _data or
    // _length; and if _capacity == 0, then deref() doesn't free _real_data.
    assert(!_r.memo->capacity || _r.memo->refcount > 1);
    deref();
    assign(_r.data, _r.length, false);
    return const_cast<char *>(_r.data);
}

char *
String::mutable_c_str()
{
    (void) mutable_data();
    (void) c_str();
    return const_cast<char *>(_r.data);
}

const char *
String::c_str() const
{
    // If _memo has no capacity, then this is one of the special strings (null
    // or PermString). We are guaranteed, in these strings, that
    // _data[_length] exists. We can return _data immediately if we have a
    // '\0' in the right place.
    if (!_r.memo->capacity && _r.data[_r.length] == '\0')
	return _r.data;

    // Otherwise, this invariant must hold (there's more real data in _memo
    // than in our substring).
    assert(!_r.memo->capacity
	   || _r.memo->real_data + _r.memo->dirty >= _r.data + _r.length);

    // Has the character after our substring been set?
    uint32_t dirty = _r.memo->dirty;
    if (_r.memo->real_data + dirty == _r.data + _r.length) {
	if (_r.memo->capacity > dirty) {
	    _r.memo->dirty = dirty + 1;
	    // Character after our substring has not been set. Change it to
	    // '\0'. This case will never occur on special strings.
	    char *real_data = const_cast<char *>(_r.data);
	    real_data[_r.length] = '\0';
	    return _r.data;
	}

    } else {
	// Character after our substring has been set. OK to return _data if
	// it is already '\0'.
	if (_r.data[_r.length] == '\0')
	    return _r.data;
    }

    // If we get here, we must make a copy of our portion of the string.
    {
	String s(_r.data, _r.length);
	deref();
	assign(s);
    }

    char *real_data = const_cast<char *>(_r.data);
    real_data[_r.length] = '\0';
    ++_r.memo->dirty;		// include '\0' in used portion of _memo
    return _r.data;
}

String
String::substring(int pos, int len) const
{
    if (pos < 0)
	pos += _r.length;

    int pos2;
    if (len < 0)
	pos2 = _r.length + len;
    else if (pos >= 0 && len >= _r.length) // avoid integer overflow
	pos2 = _r.length;
    else
	pos2 = pos + len;

    if (pos < 0)
	pos = 0;
    if (pos2 > _r.length)
	pos2 = _r.length;

    if (pos >= pos2)
	return String();
    else
	return String(_r.data + pos, pos2 - pos, _r.memo);
}

int
String::find_left(char c, int start) const
{
    if (start < 0)
	start = 0;
    for (int i = start; i < _r.length; i++)
	if (_r.data[i] == c)
	    return i;
    return -1;
}

int
String::find_left(const String &str, int start) const
{
    if (start < 0)
	start = 0;
    if (start >= length())
	return -1;
    if (!str.length())
	return 0;
    int first_c = (unsigned char)str[0];
    int pos = start, max_pos = length() - str.length();
    for (pos = find_left(first_c, pos); pos >= 0 && pos <= max_pos;
	 pos = find_left(first_c, pos + 1))
	if (!memcmp(_r.data + pos, str._r.data, str.length()))
	    return pos;
    return -1;
}

int
String::find_right(char c, int start) const
{
    if (start >= _r.length)
	start = _r.length - 1;
    for (int i = start; i >= 0; i--)
	if (_r.data[i] == c)
	    return i;
    return -1;
}

static String
hard_lower(const String &s, int pos)
{
    String new_s(s.data(), s.length());
    char *x = const_cast<char *>(new_s.data()); // know it's mutable
    int len = s.length();
    for (; pos < len; pos++)
	x[pos] = tolower((unsigned char) x[pos]);
    return new_s;
}

String
String::lower() const
{
    // avoid copies
    for (int i = 0; i < _r.length; i++)
	if (_r.data[i] >= 'A' && _r.data[i] <= 'Z')
	    return hard_lower(*this, i);
    return *this;
}

static String
hard_upper(const String &s, int pos)
{
    String new_s(s.data(), s.length());
    char *x = const_cast<char *>(new_s.data()); // know it's mutable
    int len = s.length();
    for (; pos < len; pos++)
	x[pos] = toupper((unsigned char) x[pos]);
    return new_s;
}

String
String::upper() const
{
    // avoid copies
    for (int i = 0; i < _r.length; i++)
	if (_r.data[i] >= 'a' && _r.data[i] <= 'z')
	    return hard_upper(*this, i);
    return *this;
}

static String
hard_printable(const String &s, int pos)
{
    StringAccum sa(s.length() * 2);
    sa.append(s.data(), pos);
    const unsigned char *x = reinterpret_cast<const unsigned char *>(s.data());
    int len = s.length();
    for (; pos < len; pos++) {
	if (x[pos] >= 32 && x[pos] < 127)
	    sa << x[pos];
	else if (x[pos] < 32)
	    sa << '^' << (unsigned char)(x[pos] + 64);
	else if (char *buf = sa.extend(4, 1))
	    sprintf(buf, "\\%03o", x[pos]);
    }
    return sa.take_string();
}

String
String::printable() const
{
    // avoid copies
    for (int i = 0; i < _r.length; i++)
	if (_r.data[i] < 32 || _r.data[i] > 126)
	    return hard_printable(*this, i);
    return *this;
}

hashcode_t
String::hashcode(const char *begin, const char *end)
{
    if (end <= begin)
	return 0;

    uint32_t hash = end - begin;
    int rem = hash & 3;
    end -= rem;
    uint32_t last16;

#if !HAVE_INDIFFERENT_ALIGNMENT
    if (!(reinterpret_cast<uintptr_t>(begin) & 1)) {
#endif
#define get16(p) (*reinterpret_cast<const uint16_t *>((p)))
	for (; begin != end; begin += 4) {
	    hash += get16(begin);
	    uint32_t tmp = (get16(begin + 2) << 11) ^ hash;
	    hash = (hash << 16) ^ tmp;
	    hash += hash >> 11;
	}
	if (rem >= 2) {
	    last16 = get16(begin);
	    goto rem2;
	}
#undef get16
#if !HAVE_INDIFFERENT_ALIGNMENT
    } else {
# if !__i386__
#  define get16(p) (((unsigned char) (p)[0] << 8) + (unsigned char) (p)[1])
# else
#  define get16(p) ((unsigned char) (p)[0] + ((unsigned char) (p)[1] << 8))
# endif
	// should be exactly the same as the code above
	for (; begin != end; begin += 4) {
	    hash += get16(begin);
	    uint32_t tmp = (get16(begin + 2) << 11) ^ hash;
	    hash = (hash << 16) ^ tmp;
	    hash += hash >> 11;
	}
	if (rem >= 2) {
	    last16 = get16(begin);
	    goto rem2;
	}
# undef get16
    }
#endif

    /* Handle end cases */
    if (0) {			// weird organization avoids uninitialized
      rem2:			// variable warnings
	if (rem == 3) {
	    hash += last16;
	    hash ^= hash << 16;
	    hash ^= ((unsigned char) begin[2]) << 18;
	    hash += hash >> 11;
	} else {
	    hash += last16;
	    hash ^= hash << 11;
	    hash += hash >> 17;
	}
    } else if (rem == 1) {
	hash += (unsigned char) *begin;
	hash ^= hash << 10;
	hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

#if 0
// 11.Apr.2008 -- This old hash function was swapped out in favor of
// SuperFastHash, above.
hashcode_t
String::hashcode() const
{
    int l = length();
    const char *d = data();
    if (!l)
	return 0;
    else if (l == 1)
	return d[0] | (d[0] << 8);
    else if (l < 4)
	return d[0] + (d[1] << 3) + (l << 12);
    else
	return d[0] + (d[1] << 8) + (d[2] << 16) + (d[3] << 24)
	    + (l << 12) + (d[l-1] << 10);
}
#endif

bool
String::equals(const char *s, int len) const
{
    // It'd be nice to make "out-of-memory" strings compare unequal to
    // anything, even themselves, but this would be a bad idea for Strings
    // used as (for example) keys in hashtables. Instead, "out-of-memory"
    // strings compare unequal to other null strings, but equal to each other.
    if (len < 0)
	len = strlen(s);
    if (_r.length != len)
	return false;
    else if (_r.data == s)
	return true;
    else if (len == 0)
	return (s != &oom_string_data && _r.memo != &oom_memo);
    else
	return memcmp(_r.data, s, len) == 0;
}

bool
String::starts_with(const char *s, int len) const
{
    // See note on equals() re: "out-of-memory" strings.
    if (len < 0)
	len = strlen(s);
    if (_r.length < len)
	return false;
    else if (_r.data == s)
	return true;
    else if (len == 0)
	return (s != &oom_string_data && _r.memo != &oom_memo);
    else
	return memcmp(_r.data, s, len) == 0;
}

int
String::compare(const char *s, int len) const
{
    if (len < 0)
	len = strlen(s);
    if (_r.data == s)
	return _r.length - len;
    else if (_r.memo == &oom_memo)
	return 1;
    else if (s == &oom_string_data)
	return -1;
    else if (_r.length == len)
	return memcmp(_r.data, s, len);
    else if (_r.length < len) {
	int v = memcmp(_r.data, s, _r.length);
	return (v ? v : -1);
    } else {
	int v = memcmp(_r.data, s, len);
	return (v ? v : 1);
    }
}

void
String::align(int n)
{
    int offset = reinterpret_cast<uintptr_t>(_r.data) % n;
    if (offset) {
	String s;
	s.append_garbage(_r.length + n + 1);
	offset = reinterpret_cast<uintptr_t>(s._r.data) % n;
	memcpy((char *)s._r.data + n - offset, _r.data, _r.length);
	s._r.data += n - offset;
	s._r.length = _r.length;
	*this = s;
    }
}
