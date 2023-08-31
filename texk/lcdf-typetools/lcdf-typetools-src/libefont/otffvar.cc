// -*- related-file-name: "../include/efont/otffvar.hh" -*-

/* otffvar.{cc,hh} -- OpenType fvar table
 *
 * Copyright (c) 2002-2023 Eddie Kohler
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
#include <efont/otffvar.hh>
#include <lcdf/error.hh>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <efont/otfdata.hh>     // for ntohl()

namespace Efont { namespace OpenType {

Fvar::Fvar(Data d)
    : _id(nullptr)
{
    d.align_long();
    _d = d.udata();
    // USHORT   majorVersion
    // USHORT   minorVersion
    // OFFSET16 axesArrayOffset
    // USHORT   reserved
    // USHORT   axisCount
    // USHORT   axisSize
    // USHORT   instanceCount
    // USHORT   instanceSize
    if (d.length() == 0)
        throw BlankTable("fvar");
    if (d.length() < HEADER_SIZE
        || d.u16(0) != 1)
        throw Format("fvar");
    int axoff = d.u16(X_AXISOFF), nax = d.u16(X_AXISCOUNT),
        axsz = d.u16(X_AXISSIZE);
    if (axoff < HEADER_SIZE
        || (axoff % 2) != 0
        || nax == 0
        || axsz < AXIS_SIZE
        || (axsz % 2) != 0
        || axoff + nax * axsz > d.length())
        throw Format("fvar");
    int nin = d.u16(X_INSTANCECOUNT), insz = d.u16(X_INSTANCESIZE);
    if (insz >= 4 + nax * 4
        && (insz % 2) == 0
        && axoff + axsz * nax + insz * nin <= d.length())
        _id = _d + axoff + axsz * nax;
}

}}
