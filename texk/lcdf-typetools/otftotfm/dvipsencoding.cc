/* dvipsencoding.{cc,hh} -- store a DVIPS encoding
 *
 * Copyright (c) 2003-2008 Eddie Kohler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "dvipsencoding.hh"
#include "metrics.hh"
#include "secondary.hh"
#include <lcdf/error.hh>
#include <lcdf/straccum.hh>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <algorithm>
#include "util.hh"

static String::Initializer initializer;
enum { GLYPHLIST_MORE = 0x40000000,
       U_EMPTYSLOT = 0xD801, U_ALTSELECTOR = 0xD802 };
static HashMap<String, int> glyphlist(-1);
static PermString::Initializer perm_initializer;
PermString DvipsEncoding::dot_notdef(".notdef");

#define NEXT_GLYPH_NAME(gn)	("/" + (gn))

int
DvipsEncoding::add_glyphlist(String text)
{
    // XXX ignores glyph names that map to multiple characters
    const char *data = text.c_str();
    int pos = 0, len = text.length();
    while (1) {
	// move to first nonblank
	while (pos < len && isspace((unsigned char) data[pos]))
	    pos++;
	// parse line
	if (pos >= len)
	    return 0;
	else if (data[pos] != '#') {
	    int first = pos;
	    for (; pos < len && !isspace((unsigned char) data[pos]) && data[pos] != ';'; pos++)
		/* nada */;
	    String glyph_name = text.substring(first, pos - first);
	    int value;
	    char *next;
	  read_uni:
	    if (first == pos
		|| pos + 1 >= len
		|| (data[pos] != ';' && data[pos] != ',')
		|| !isxdigit((unsigned char) data[pos+1])
		|| ((value = strtol(data + pos + 1, &next, 16)),
		    (!isspace((unsigned char) *next) && *next && *next != ';' && *next != ',')))
		return -1;
	    while (*next == ' ' || *next == '\t')
		next++;
	    if (*next == '\r' || *next == '\n')
		glyphlist.insert(glyph_name, value);
	    else if (*next == ';' || *next == ',')
		glyphlist.insert(glyph_name, value | GLYPHLIST_MORE);
	    else
		while (*next != '\r' && *next != '\n' && *next != ';' && *next != ',')
		    next++;
	    pos = next - data;
	    if (*next == ';' || *next == ',') {	// read another possibility
		glyph_name = NEXT_GLYPH_NAME(glyph_name);
		// XXX given "DDDD,EEEE EEEE,FFFF", will not store "FFFF"
		goto read_uni;
	    }
	} else
	    while (pos < len && data[pos] != '\n' && data[pos] != '\r')
		pos++;
    }
}

void
DvipsEncoding::glyphname_unicode(String gn, Vector<int> &unis, bool *more)
{    
    if (more)
	*more = false;
    
    // first, drop all characters to the right of the first dot
    String::iterator dot = std::find(gn.begin(), gn.end(), '.');
    if (dot < gn.end())
	gn = gn.substring(gn.begin(), dot);

    // then, separate into components
    while (gn) {
	String::iterator underscore = std::find(gn.begin(), gn.end(), '_');
	String component = gn.substring(gn.begin(), underscore);
	gn = gn.substring(underscore + 1, gn.end());

	// check glyphlist
	int value = glyphlist[component];
	uint32_t uval;
	if (value >= 0) {
	    unis.push_back(value & ~GLYPHLIST_MORE);
	    if (more && (value & GLYPHLIST_MORE) && !gn)
		*more = true;
	} else if (component.length() >= 7
		   && (component.length() % 4) == 3
		   && memcmp(component.data(), "uni", 3) == 0) {
	    int old_size = unis.size();
	    for (const char* s = component.begin() + 3; s < component.end(); s += 4)
		if (parse_unicode_number(s, s + 4, -1, uval))
		    unis.push_back(uval);
		else {
		    unis.resize(old_size);
		    break;
		}
	} else if (component.length() >= 5
		   && component.length() <= 7
		   && component[0] == 'u'
		   && parse_unicode_number(component.begin() + 1, component.end(), -1, uval))
	    unis.push_back(uval);
    }
}

int
DvipsEncoding::glyphname_unicode(const String &gn, bool *more)
{
    Vector<int> unis;
    glyphname_unicode(gn, unis, more);
    return (unis.size() == 1 ? unis[0] : -1);
}


DvipsEncoding::DvipsEncoding()
    : _boundary_char(-1), _altselector_char(-1), _unicoding_map(-1),
      _warn_missing(false)
{
}

void
DvipsEncoding::encode(int e, PermString what)
{
    if (e >= _e.size())
	_e.resize(e + 1, dot_notdef);
    _e[e] = what;
}

int
DvipsEncoding::encoding_of(PermString what, bool encoding_required)
{
    int slot = -1;
    for (int i = 0; i < _e.size(); i++)
	if (_e[i] == what) {
	    slot = i;
	    goto use_slot;
	} else if (!_e[i] || _e[i] == dot_notdef)
	    slot = i;
    if (what == "||")
	return _boundary_char;
    else if (!encoding_required || slot < 0)
	return -1;
  use_slot:
    if (encoding_required) {
	if (slot >= _encoding_required.size())
	    _encoding_required.resize(slot + 1, false);
	_encoding_required[slot] = true;
	this->encode(slot, what);
    }
    return slot;
}

static String
tokenize(const String &s, int &pos_in, int &line)
{
    const char *data = s.data();
    int len = s.length();
    int pos = pos_in;
    while (1) {
	// skip whitespace
	while (pos < len && isspace((unsigned char) data[pos])) {
	    if (data[pos] == '\n')
		line++;
	    else if (data[pos] == '\r' && (pos + 1 == len || data[pos+1] != '\n'))
		line++;
	    pos++;
	}
	
	if (pos >= len) {
	    pos_in = len;
	    return String();
	} else if (data[pos] == '%') {
	    for (pos++; pos < len && data[pos] != '\n' && data[pos] != '\r'; pos++)
		/* nada */;
	} else if (data[pos] == '[' || data[pos] == ']' || data[pos] == '{' || data[pos] == '}') {
	    pos_in = pos + 1;
	    return s.substring(pos, 1);
	} else if (data[pos] == '(') {
	    int first = pos, nest = 0;
	    for (pos++; pos < len && (data[pos] != ')' || nest); pos++)
		switch (data[pos]) {
		  case '(': nest++; break;
		  case ')': nest--; break;
		  case '\\':
		    if (pos + 1 < len)
			pos++;
		    break;
		  case '\n': line++; break;
		  case '\r':
		    if (pos + 1 == len || data[pos+1] != '\n')
			line++;
		    break;
		}
	    pos_in = (pos < len ? pos + 1 : len);
	    return s.substring(first, pos_in - first);
	} else {
	    int first = pos;
	    while (pos < len && data[pos] == '/')
		pos++;
	    while (pos < len && data[pos] != '/' && !isspace((unsigned char) data[pos]) && data[pos] != '[' && data[pos] != ']' && data[pos] != '%' && data[pos] != '(' && data[pos] != '{' && data[pos] != '}')
		pos++;
	    pos_in = pos;
	    return s.substring(first, pos - first);
	}
    }
}


static String
comment_tokenize(const String &s, int &pos_in, int &line)
{
    const char *data = s.data();
    int len = s.length();
    int pos = pos_in;
    while (1) {
	while (pos < len && data[pos] != '%' && data[pos] != '(') {
	    if (data[pos] == '\n')
		line++;
	    else if (data[pos] == '\r' && (pos + 1 == len || data[pos+1] != '\n'))
		line++;
	    pos++;
	}
	
	if (pos >= len) {
	    pos_in = len;
	    return String();
	} else if (data[pos] == '%') {
	    for (pos++; pos < len && (data[pos] == ' ' || data[pos] == '\t'); pos++)
		/* nada */;
	    int first = pos;
	    for (; pos < len && data[pos] != '\n' && data[pos] != '\r'; pos++)
		/* nada */;
	    pos_in = pos;
	    if (pos > first)
		return s.substring(first, pos - first);
	} else {
	    int nest = 0;
	    for (pos++; pos < len && (data[pos] != ')' || nest); pos++)
		switch (data[pos]) {
		  case '(': nest++; break;
		  case ')': nest--; break;
		  case '\\':
		    if (pos + 1 < len)
			pos++;
		    break;
		  case '\n': line++; break;
		  case '\r':
		    if (pos + 1 == len || data[pos+1] != '\n')
			line++;
		    break;
		}
	}
    }
}

static bool
retokenize_isword(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
	|| (c >= '0' && c <= '9') || c == '.' || c == '_';
}


static struct { const char *s; int v; } ligkern_ops[] = {
    { "=:", DvipsEncoding::JL_LIG }, { "|=:", DvipsEncoding::JL_CLIG },
    { "|=:>", DvipsEncoding::JL_CLIG_S }, { "=:|", DvipsEncoding::JL_LIGC },
    { "=:|>", DvipsEncoding::JL_LIGC_S }, { "|=:|", DvipsEncoding::JL_CLIGC },
    { "|=:>", DvipsEncoding::JL_CLIGC_S }, { "|=:|>>", DvipsEncoding::JL_CLIGC_SS },
    { "{}", DvipsEncoding::JT_KERN }, { "{K}", DvipsEncoding::JT_KERN },
    { "{L}", DvipsEncoding::JT_LIG }, { "{LK}", DvipsEncoding::JT_NOLIGKERN },
    { "{KL}", DvipsEncoding::JT_NOLIGKERN }, { "{k}", DvipsEncoding::JT_KERN },
    { "{l}", DvipsEncoding::JT_LIG }, { "{lk}", DvipsEncoding::JT_NOLIGKERN },
    { "{kl}", DvipsEncoding::JT_NOLIGKERN },
    // some encodings have @{@} instead of {}
    { "@{@}", DvipsEncoding::JT_KERN },
    { 0, 0 }
};

static int
find_ligkern_op(const String &s)
{
    for (int i = 0; ligkern_ops[i].s; i++)
	if (ligkern_ops[i].s == s)
	    return ligkern_ops[i].v;
    return 0;
}

inline bool
operator==(const DvipsEncoding::Ligature& l1, const DvipsEncoding::Ligature& l2)
{
    return l1.c1 == l2.c1 && l1.c2 == l2.c2;
}

void
DvipsEncoding::add_ligkern(const Ligature &l, int override)
{
    Ligature *old = std::find(_lig.begin(), _lig.end(), l);
    if (old == _lig.end())
	_lig.push_back(l);
    else {
	if ((l.join & JT_KERN) && (override > 0 || !(old->join & JT_KERN))) {
	    old->join |= JT_KERN;
	    old->k = l.k;
	}
	if ((l.join & JT_LIG) && (override > 0 || !(old->join & JT_LIG))) {
	    old->join = (old->join & JT_KERN) | (l.join & JT_LIGALL);
	    old->d = l.d;
	}
    }
}

int
DvipsEncoding::parse_ligkern_words(Vector<String> &v, int override, ErrorHandler *errh)
{
    _file_had_ligkern = true;
    int op;
    long l;
    char *endptr;
    if (v.size() == 3) {
	// empty string fails
	if (!v[0])
	    return -1;
	// boundary char setting
	if (v[0] == "||" && v[1] == "=") {
	    char *endptr;
	    if (override > 0 || _boundary_char < 0)
		_boundary_char = strtol(v[2].c_str(), &endptr, 10);
	    if (*endptr == 0 && _boundary_char < _e.size())
		return 0;
	    else
		return errh->error("parse error in boundary character assignment");
	}
	// altselector char setting
	if (v[0] == "^^" && v[1] == "=") {
	    char *endptr;
	    if (override > 0 || _altselector_char < 0)
		_altselector_char = strtol(v[2].c_str(), &endptr, 10);
	    if (*endptr == 0 && _altselector_char < _e.size())
		return 0;
	    else
		return errh->error("parse error in altselector character assignment");
	}
	// encoding
	l = strtol(v[0].c_str(), &endptr, 0);
	if (endptr == v[0].end() && v[1] == "=") {
	    if (l >= 0 && l < 256) {
		if (override > 0 || !_e[l])
		    encode(l, v[2]);
		return 0;
	    } else
		return errh->error("encoding value '%d' out of range", l);
	}
	
	// kern operation
	if (v[1].length() >= 3 && v[1][0] == '{' && v[1].back() == '}') {
	    String middle = v[1].substring(1, v[1].length() - 2);
	    l = strtol(middle.c_str(), &endptr, 0);
	    if (endptr == middle.end()) {
		op = JT_KERN;
		goto found_kernop;
	    }
	}
	op = find_ligkern_op(v[1]);
	if (!op || (op & JT_ADDLIG))
	    return -1;
      found_kernop:
	int av = (v[0] == "*" ? J_ALL : encoding_of(v[0]));
	if (av < 0)
	    return errh->warning("'%s' has no encoding, ignoring '%s'", v[0].c_str(), v[1].c_str());
	int bv = (v[2] == "*" ? J_ALL : encoding_of(v[2]));
	if (bv < 0)
	    return errh->warning("'%s' has no encoding, ignoring '%s'", v[2].c_str(), v[1].c_str());
	if ((op & JT_KERN) && l && (av == J_ALL || bv == J_ALL))
	    return errh->warning("'%s %s %s' illegal, only {0} works with *", v[0].c_str(), v[1].c_str(), v[2].c_str());
	Ligature lig = { av, bv, op, l, 0 };
	add_ligkern(lig, override);
	return 0;

    } else if (v.size() == 4 && ((op = find_ligkern_op(v[2])) & JT_ADDLIG)) {
	int av = encoding_of(v[0], override > 0);
	if (av < 0)
	    return (override > 0 ? errh->warning("'%s' has no encoding, ignoring ligature", v[0].c_str()) : -1);
	int bv = encoding_of(v[1], override > 0);
	if (bv < 0)
	    return (override > 0 ? errh->warning("'%s' has no encoding, ignoring ligature", v[1].c_str()) : -1);
	int cv = encoding_of(v[3], override > 0);
	if (cv < 0)
	    return (override > 0 ? errh->warning("'%s' has no encoding, ignoring ligature", v[3].c_str()) : -1);
	Ligature lig = { av, bv, op, 0, cv };
	add_ligkern(lig, override);
	return 0;
	
    } else
	return -EPARSE;
}

int
DvipsEncoding::parse_position_words(Vector<String> &v, int override, ErrorHandler *errh)
{
    if (v.size() != 4)
	return -EPARSE;

    int c = encoding_of(v[0]);
    if (c < 0)
	return (override > 0 ? errh->warning("'%s' has no encoding, ignoring positioning", v[0].c_str()) : -1);
    
    char *endptr;
    int pdx, pdy, adx;
    if (!v[1] || !v[2] || !v[3]
	|| (pdx = strtol(v[1].c_str(), &endptr, 10), *endptr)
	|| (pdy = strtol(v[2].c_str(), &endptr, 10), *endptr)
	|| (adx = strtol(v[3].c_str(), &endptr, 10), *endptr))
	return errh->error("parse error in POSITION");

    Ligature l = { c, pdx, pdy, adx, 0 };
    Ligature *old = std::find(_pos.begin(), _pos.end(), l);
    if (old == _pos.end())
	_pos.push_back(l);
    else if (override > 0)
	*old = l;
    return 0;
}

int
DvipsEncoding::parse_unicoding_words(Vector<String> &v, int override, ErrorHandler *errh)
{
    int av;
    if (v.size() < 2 || (v[1] != "=" && v[1] != "=:" && v[1] != ":="))
	return -EPARSE;
    else if (v[0] == "||" || (av = encoding_of(v[0])) < 0)
	return errh->warning("'%s' has no encoding, ignoring UNICODING", v[0].c_str());

    int original_size = _unicoding.size();
    
    if (v.size() == 2 || (v.size() == 3 && v[2] == dot_notdef))
	/* no warnings to delete a glyph */;
    else {
	for (int i = 2; i < v.size(); i++) {
	    bool more;		// some care to get all possibilities
	    int uni = glyphname_unicode(v[i], &more);
	    if (uni < 0) {
		errh->warning("can't map '%s' to Unicode", v[i].c_str());
		if (i == 2)
		    errh->warning("target '%s' will be deleted from encoding", v[0].c_str());
	    } else {
		_unicoding.push_back(uni);
		while (more) {
		    v[i] = NEXT_GLYPH_NAME(v[i]);
		    if ((uni = glyphname_unicode(v[i], &more)) >= 0)
			_unicoding.push_back(uni);
		}
	    }
	}
    }
    
    _unicoding.push_back(-1);
    if (override > 0 || _unicoding_map[v[0]] < 0)
	_unicoding_map.insert(v[0], original_size);
    return 0;
}

const DvipsEncoding::WordType DvipsEncoding::word_types[] = {
    { "LIGKERN", &DvipsEncoding::parse_ligkern_words },
    { "POSITION", &DvipsEncoding::parse_position_words },
    { "UNICODING", &DvipsEncoding::parse_unicoding_words }
};

void
DvipsEncoding::parse_word_group(Vector<String> &words, int override, int wt, ErrorHandler *errh)
{
    if (words.size() > 0) {
	int (DvipsEncoding::*method)(Vector<String> &, int, ErrorHandler *) = word_types[wt].parsefunc;
	if ((this->*method)(words, override, errh) == -EPARSE) {
	    Vector<String> rewords;
	    for (String *sp = words.begin(); sp != words.end(); sp++) {
		const char *s = sp->begin(), *ends = sp->end();
		while (s != ends) {
		    const char *word = s;
		    if (*s == '{') {
			for (s++; s != ends && *s != '}'; s++)
			    /* nada */;
			if (s != ends)
			    s++;
		    } else {
			bool x = retokenize_isword(*s);
			for (s++; s != ends && x == retokenize_isword(*s); s++)
			    /* nada */;
		    }
		    rewords.push_back(sp->substring(word, s));
		}
	    }
	    if ((this->*method)(rewords, override, errh) == -EPARSE)
		errh->error("parse error in %s", word_types[wt].name);
		
	}
	words.clear();
    }
}

int
DvipsEncoding::parse_words(const String &s, int override, int wt, ErrorHandler *errh)
{
    Vector<String> words;
    const char *data = s.data();
    const char *end = s.end();
    while (data < end) {
	while (data < end && isspace((unsigned char) *data))
	    data++;
	const char *first = data;
	while (data < end && !isspace((unsigned char) *data) && *data != ';')
	    data++;
	if (data == first) {
	    data++;		// step past semicolon (or harmlessly past EOS)
	    parse_word_group(words, override, wt, errh);
	} else
	    words.push_back(s.substring(first, data));
    }
    parse_word_group(words, override, wt, errh);
    return 0;
}

static String
landmark(const String &filename, int line)
{
    return filename + String::stable_string(":", 1) + String(line);
}

static String
trim_space(const String &s, int pos)
{
    while (pos < s.length() && isspace((unsigned char) s[pos]))
	pos++;
    int epos = s.length();
    for (int x = 0; x < 2; x++) {
	while (epos > pos && isspace((unsigned char) s[epos - 1]))
	    epos--;
	if (epos == pos || s[epos - 1] != ';')
	    break;
	epos--;
    }
    return s.substring(pos, epos - pos);
}

int
DvipsEncoding::parse(String filename, bool ignore_ligkern, bool ignore_other, ErrorHandler *errh)
{
    int before = errh->nerrors();
    String s = read_file(filename, errh);
    if (errh->nerrors() != before)
	return -1;
    _filename = filename;
    _file_had_ligkern = false;
    filename = printable_filename(filename);
    int pos = 0, line = 1;

    // parse text
    String token = tokenize(s, pos, line);
    if (!token || token[0] != '/')
	return errh->lerror(landmark(filename, line), "parse error, expected name");
    _name = token.substring(1);
    _initial_comment = s.substring(0, pos - token.length());

    if (tokenize(s, pos, line) != "[")
	return errh->lerror(landmark(filename, line), "parse error, expected [");

    while ((token = tokenize(s, pos, line)) && token[0] == '/')
	_e.push_back(token.substring(1));

    _final_text = token + s.substring(pos);

    // now parse comments
    pos = 0, line = 1;
    Vector<String> words;
    LandmarkErrorHandler lerrh(errh, "");
    while ((token = comment_tokenize(s, pos, line)))
	if (token.length() >= 8
	    && memcmp(token.data(), "LIGKERN", 7) == 0
	    && isspace((unsigned char) token[7])
	    && !ignore_ligkern) {
	    lerrh.set_landmark(landmark(filename, line));
	    parse_words(token.substring(8), 1, WT_LIGKERN, &lerrh);
	    
	} else if (token.length() >= 9
		   && memcmp(token.data(), "LIGKERNX", 8) == 0
		   && isspace((unsigned char) token[8])
		   && !ignore_ligkern) {
	    lerrh.set_landmark(landmark(filename, line));
	    parse_words(token.substring(9), 1, WT_LIGKERN, &lerrh);
	    
	} else if (token.length() >= 10
		   && memcmp(token.data(), "UNICODING", 9) == 0
		   && isspace((unsigned char) token[9])
		   && !ignore_other) {
	    lerrh.set_landmark(landmark(filename, line));
	    parse_words(token.substring(10), 1, WT_UNICODING, &lerrh);
	    
	} else if (token.length() >= 9
		   && memcmp(token.data(), "POSITION", 8) == 0
		   && isspace((unsigned char) token[8])
		   && !ignore_other) {
	    lerrh.set_landmark(landmark(filename, line));
	    parse_words(token.substring(9), 1, WT_POSITION, &lerrh);
	    
	} else if (token.length() >= 13
		   && memcmp(token.data(), "CODINGSCHEME", 12) == 0
		   && isspace((unsigned char) token[12])
		   && !ignore_other) {
	    _coding_scheme = trim_space(token, 13);
	    if (_coding_scheme.length() > 39)
		lerrh.lwarning(landmark(filename, line), "only first 39 chars of CODINGSCHEME are significant");
	    if (std::find(_coding_scheme.begin(), _coding_scheme.end(), '(') < _coding_scheme.end()
		|| std::find(_coding_scheme.begin(), _coding_scheme.end(), ')') < _coding_scheme.end()) {
		lerrh.lerror(landmark(filename, line), "CODINGSCHEME cannot contain parentheses");
		_coding_scheme = String();
	    }

	} else if (token.length() >= 11
		   && memcmp(token.data(), "WARNMISSING", 11) == 0
		   && (token.length() == 11 || isspace((unsigned char) token[11]))
		   && !ignore_other) {
	    String value = trim_space(token, 11);
	    if (value == "1" || value == "yes" || value == "true" || !value)
		_warn_missing = true;
	    else if (value == "0" || value == "no" || value == "false")
		_warn_missing = false;
	    else
		lerrh.lerror(landmark(filename, line), "WARNMISSING command not understood");
	}

    return 0;
}

int
DvipsEncoding::parse_ligkern(const String &ligkern_text, int override, ErrorHandler *errh)
{
    return parse_words(ligkern_text, override, WT_LIGKERN, errh);
}

int
DvipsEncoding::parse_position(const String &position_text, int override, ErrorHandler *errh)
{
    return parse_words(position_text, override, WT_POSITION, errh);
}

int
DvipsEncoding::parse_unicoding(const String &unicoding_text, int override, ErrorHandler *errh)
{
    return parse_words(unicoding_text, override, WT_UNICODING, errh);
}

void
DvipsEncoding::bad_codepoint(int code, Metrics &metrics, Vector<String> &unencoded)
{
    for (int i = 0; i < _lig.size(); i++) {
	Ligature &l = _lig[i];
	if (l.c1 == code || l.c2 == code)
	    l.join = 0;
	else if ((l.join & JT_ADDLIG) && l.d == code)
	    l.join &= ~JT_LIGALL;
    }

    if (_warn_missing) {
	Vector<uint32_t> unicodes;
	bool unicodes_explicit = x_unicodes(_e[code], unicodes);
	if (!unicodes_explicit || unicodes.size() > 0) {
	    Vector<Setting> v;
	    v.push_back(Setting(Setting::RULE, 500, 500));
	    v.push_back(Setting(Setting::SPECIAL, String("Warning: missing glyph '") + _e[code] + "'"));
	    metrics.encode_virtual(code, _e[code], 0, v, true);
	    unencoded.push_back(_e[code]);
	}
    }
}

static inline Efont::OpenType::Glyph
map_uni(uint32_t uni, const Efont::OpenType::Cmap &cmap, const Metrics &m)
{
    if (uni == U_EMPTYSLOT)
	return m.emptyslot_glyph();
    else
	return cmap.map_uni(uni);
}

bool
DvipsEncoding::x_unicodes(PermString chname, Vector<uint32_t> &unicodes) const
{
    int i = _unicoding_map[chname];
    if (i >= 0) {
	for (; _unicoding[i] >= 0; i++)
	    unicodes.push_back(_unicoding[i]);
	return true;
    } else {
	bool more;
	if ((i = glyphname_unicode(chname, &more)) >= 0)
	    unicodes.push_back(i);
	if (more) {		// might be multiple possibilities
	    String gn = chname;
	    do {
		gn = NEXT_GLYPH_NAME(gn);
		if ((i = glyphname_unicode(gn, &more)) >= 0)
		    unicodes.push_back(i);
	    } while (more);
	}
	return false;
    }
}


void
DvipsEncoding::make_metrics(Metrics &metrics, const FontInfo &finfo, Secondary *secondary, bool literal, ErrorHandler *errh)
{
    // first pass: without secondaries
    for (int code = 0; code < _e.size(); code++) {
	PermString chname = _e[code];

	// common case: skip .notdef
	if (chname == dot_notdef)
	    continue;

	// find all Unicodes
	Vector<uint32_t> unicodes;
	(void) x_unicodes(chname, unicodes);

	// find first Unicode supported by the font
	Efont::OpenType::Glyph glyph = 0;
	uint32_t glyph_uni = (unicodes.size() ? unicodes[0] : 0);
	for (uint32_t *u = unicodes.begin(); u < unicodes.end() && !glyph; u++)
	    if ((glyph = map_uni(*u, *finfo.cmap, metrics)) > 0)
		glyph_uni = *u;

	// find named glyph, if any
	Efont::OpenType::Glyph named_glyph = finfo.glyphid(chname);
#if 0
	// 2.May.2008: ff, fi, fl, ffi, and ffl might map to f_f, f_i, f_l,
	// f_f_i, and f_f_l
	if (!named_glyph && chname.length() > 0 && chname.length() <= 3
	    && chname[0] == 'f') {
	    if (chname.equals("ff", 2))
		named_glyph = finfo.glyphid("f_f");
	    else if (chname.equals("fi", 2))
		named_glyph = finfo.glyphid("f_i");
	    else if (chname.equals("fl", 2))
		named_glyph = finfo.glyphid("f_l");
	    else if (chname.equals("ffi", 2))
		named_glyph = finfo.glyphid("f_f_i");
	    else if (chname.equals("ffl", 2))
		named_glyph = finfo.glyphid("f_f_l");
	}
#endif
	// do not use a Unicode-mapped glyph if literal
	if (literal)
	    glyph = named_glyph;
	
	// If we found a glyph, maybe use its named_glyph variant.
	if (glyph > 0 && named_glyph > 0
	    && std::find(chname.begin(), chname.end(), '.') < chname.end())
	    glyph = named_glyph;

	// assign slot
	if (glyph > 0)
	    metrics.encode(code, glyph_uni, glyph);
    }

    // second pass: with secondaries
    for (int code = 0; code < _e.size(); code++) {
	// skip already-encoded characters and .notdef
	if (literal || metrics.glyph(code) > 0 || _e[code] == dot_notdef)
	    continue;
	
	PermString chname = _e[code];

	// find all Unicodes
	Vector<uint32_t> unicodes;
	bool unicodes_explicit = x_unicodes(chname, unicodes);

	// find named glyph, if any
	Efont::OpenType::Glyph named_glyph = finfo.glyphid(chname);

	// 1. We were not able to find the glyph using Unicode.
	// 2. There might be a named_glyph.
	// May need to try secondaries later.  Store this slot.
	// Try secondaries, if there's explicit unicoding or no named_glyph.
	if (unicodes_explicit || named_glyph <= 0)
	    for (uint32_t *u = unicodes.begin(); u < unicodes.end(); u++)
		if (secondary->encode_uni(code, chname, *u, metrics, errh))
		    goto encoded;

	// 1. We were not able to find the glyph using Unicode or secondaries.
	// 2. There might be a named_glyph.
	// Use named glyph, if any.  Special case for "UNICODING foo =: ;",
	// which should turn off the character (even if a named_glyph exists),
	// UNLESS the glyph was explicitly requested.
	if (named_glyph > 0
	    && (!unicodes_explicit || unicodes.size() > 0
		|| (_encoding_required.size() > code && _encoding_required[code])))
	    metrics.encode(code, unicodes.size() ? unicodes[0] : 0, named_glyph);

      encoded:
	/* all set */;
    }

    // add altselector
    if (_altselector_char >= 0 && _altselector_char < _e.size()) {
	metrics.add_altselector_code(_altselector_char, 0);
	if (metrics.glyph(_altselector_char) <= 0 && !literal)
	    (void) secondary->encode_uni(_altselector_char, "altselector", U_ALTSELECTOR, metrics, errh);
    }

    // final pass: complain
    Vector<String> unencoded;
    for (int code = 0; code < _e.size(); code++)
	if (_e[code] != dot_notdef && metrics.glyph(code) <= 0)
	    bad_codepoint(code, metrics, unencoded);

    if (unencoded.size() == 1) {
	errh->warning("'%s' glyph not found in font", unencoded[0].c_str());
	errh->message("(This glyph will appear as a blot and cause warnings if used.)");
    } else if (unencoded.size() > 1) {
	std::sort(unencoded.begin(), unencoded.end());
	StringAccum sa;
	for (const String* a = unencoded.begin(); a < unencoded.end(); a++)
	    sa << *a << ' ';
	sa.pop_back();
	sa.append_break_lines(sa.take_string(), 68, "  ");
	sa.pop_back();
	errh->warning("%d glyphs not found in font:", unencoded.size());
	errh->message("%s\n(These glyphs will appear as blots and cause warnings if used.)", sa.c_str());
    }

    metrics.set_coding_scheme(_coding_scheme);
}

void
DvipsEncoding::make_base_mappings(Vector<int> &mappings, const FontInfo &finfo)
{
    mappings.clear();
    for (int code = 0; code < _e.size(); code++) {
	PermString chname = _e[code];

	// common case: skip .notdef
	if (chname == dot_notdef)
	    continue;

	// find named glyph
	Efont::OpenType::Glyph named_glyph = finfo.glyphid(chname);
	if (named_glyph > 0) {
	    if (mappings.size() <= named_glyph)
		mappings.resize(named_glyph + 1, -1);
	    mappings[named_glyph] = code;
	}
    }
}


void
DvipsEncoding::apply_ligkern_lig(Metrics &metrics, ErrorHandler *errh) const
{
    assert((int)J_ALL == (int)Metrics::CODE_ALL);
    for (const Ligature *l = _lig.begin(); l < _lig.end(); l++) {
	if (l->c1 < 0 || l->c2 < 0 || l->join < 0 || !(l->join & JT_LIG))
	    continue;
	metrics.remove_ligatures(l->c1, l->c2);
	if (!(l->join & JT_ADDLIG))
	    /* nada */;
	else if ((l->join & JT_LIGALL) == JL_LIG)
	    metrics.add_ligature(l->c1, l->c2, l->d);
	else if ((l->join & JT_LIGALL) == JL_LIGC)
	    metrics.add_ligature(l->c1, l->c2, metrics.pair_code(l->d, l->c2));
	else if ((l->join & JT_LIGALL) == JL_CLIG)
	    metrics.add_ligature(l->c1, l->c2, metrics.pair_code(l->c1, l->d));
	else {
	    static int complex_join_warning = 0;
	    if (!complex_join_warning) {
		errh->warning("complex LIGKERN ligature removed (I only support '=:', '=:|', and '|=:')");
		complex_join_warning = 1;
	    }
	}
    }
}

void
DvipsEncoding::apply_ligkern_kern(Metrics &metrics, ErrorHandler *) const
{
    assert((int)J_ALL == (int)Metrics::CODE_ALL);
    for (const Ligature *l = _lig.begin(); l < _lig.end(); l++)
	if (l->c1 >= 0 && l->c2 >= 0 && (l->join & JT_KERN))
	    metrics.set_kern(l->c1, l->c2, l->k);
}

void
DvipsEncoding::apply_position(Metrics &metrics, ErrorHandler *) const
{
    for (const Ligature *l = _pos.begin(); l < _pos.end(); l++)
	if (l->c1 >= 0)
	    metrics.add_single_positioning(l->c1, l->c2, l->join, l->k);
}
