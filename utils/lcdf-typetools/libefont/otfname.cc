// -*- related-file-name: "../include/efont/otfname.hh" -*-

/* otfname.{cc,hh} -- OpenType name table
 *
 * Copyright (c) 2003-2004 Eddie Kohler
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
#include <efont/otfname.hh>
#include <lcdf/error.hh>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define USHORT_AT(d)		(ntohs(*(const uint16_t *)(d)))

namespace Efont { namespace OpenType {

Name::Name(const String &s, ErrorHandler *errh)
    : _str(s)
{
    _str.align(2);
    _error = parse_header(errh ? errh : ErrorHandler::silent_handler());
}

int
Name::parse_header(ErrorHandler *errh)
{
    // HEADER FORMAT:
    // USHORT	version
    // USHORT	numTables
    int len = _str.length();
    const uint8_t *data = _str.udata();
    if (HEADER_SIZE > len)
	return errh->error("OTF name too small for header"), -EFAULT;
    if (!(data[0] == '\000' && data[1] == '\000'))
	return errh->error("bad name version number"), -ERANGE;
    int count = USHORT_AT(data + 2);
    if (HEADER_SIZE + count*NAMEREC_SIZE > len)
	return errh->error("OTF name too small for header"), -EFAULT;
    return 0;
}

String
Name::name(const_iterator i) const
{
    if (i < end()) {
	int stringOffset = USHORT_AT(_str.data() + 4);
	int length = USHORT_AT(reinterpret_cast<const uint8_t *>(i) + 8);
	int offset = USHORT_AT(reinterpret_cast<const uint8_t *>(i) + 10);
	if (stringOffset + offset + length <= _str.length())
	    return _str.substring(stringOffset + offset, length);
    }
    return String();
}

}}
