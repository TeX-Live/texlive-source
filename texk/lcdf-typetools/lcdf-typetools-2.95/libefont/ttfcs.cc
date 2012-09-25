// -*- related-file-name: "../include/efont/ttfcs.hh" -*-

/* ttfcs.{cc,hh} -- TrueType "charstring" emulation
 *
 * Copyright (c) 2006-2012 Eddie Kohler
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
#include <efont/ttfcs.hh>
#include <efont/ttfhead.hh>
#include <efont/t1csgen.hh>
#include <efont/otfpost.hh>
#include <efont/otfcmap.hh>
#include <lcdf/hashmap.hh>
namespace Efont {

TrueTypeBoundsCharstringProgram::TrueTypeBoundsCharstringProgram(const OpenType::Font *otf)
    : _otf(otf), _nglyphs(-1), _loca_long(false), _units_per_em(1024),
      _loca(otf->table("loca")), _glyf(otf->table("glyf")),
      _hmtx(otf->table("hmtx")), _got_glyph_names(false), _got_unicodes(false)
{
    OpenType::Data maxp(otf->table("maxp"));
    if (maxp.length() >= 6)
	_nglyphs = maxp.u16(4);

    OpenType::Head head(otf->table("head"), 0);
    if (head.ok()) {
	_loca_long = head.index_to_loc_format() != 0;
	_units_per_em = head.units_per_em();
    }
    if (_loca_long)
	_loca.align_long();
    int loca_onesize = (_loca_long ? 4 : 2);
    if (_nglyphs >= _loca.length() / loca_onesize)
	_nglyphs = (_loca.length() / loca_onesize) - 1;

    // horizontal metrics
    OpenType::Data hhea(_otf->table("hhea"));
    // HHEA format:
    // 0	Fixed	Table version number  	0x00010000 for version 1.0.
    // 4	FWORD	Ascender
    // 6	FWORD	Descender
    // 8	FWORD	LineGap
    // 10	UFWORD	advanceWidthMax
    // 12	FWORD	minLeftSideBearing
    // 14	FWORD	minRightSideBearing
    // 16	FWORD	xMaxExtent
    // 18	SHORT	caretSlopeRise
    // 20	SHORT	caretSlopeRun
    // 22	SHORT	caretOffset
    // 24	SHORT	(reserved)
    // 26	SHORT	(reserved)
    // 28	SHORT	(reserved)
    // 30	SHORT	(reserved)
    // 32	SHORT	metricDataFormat
    // 34	USHORT	numberOfHMetrics
    if (hhea.length() >= 36
	&& hhea.u32(0) == 0x10000)
	_nhmtx = hhea.u16(34);
    if (_nhmtx * 4 > _hmtx.length())
	_nhmtx = _hmtx.length() / 4;
}

TrueTypeBoundsCharstringProgram::~TrueTypeBoundsCharstringProgram()
{
    for (Charstring **cs = _charstrings.begin(); cs < _charstrings.end(); cs++)
	delete *cs;
}

int
TrueTypeBoundsCharstringProgram::units_per_em() const
{
    return _units_per_em;
}

int
TrueTypeBoundsCharstringProgram::nglyphs() const
{
    return _nglyphs;
}

PermString
TrueTypeBoundsCharstringProgram::glyph_name(int gi) const
{
    // generate glyph names based on what pdftex can understand
    if (gi == 0)
	return PermString(".notdef");

    // try 'post' table glyph names
    if (!_got_glyph_names) {
	OpenType::Post post(_otf->table("post"));
	if (post.ok())
	    post.glyph_names(_glyph_names);
	_got_glyph_names = true;
    }
    if (gi >= 0 && gi < _glyph_names.size())
	return _glyph_names[gi];

    // try 'uniXXXX' names
    if (!_got_unicodes) {
	OpenType::Cmap cmap(_otf->table("cmap"));
	if (cmap.ok())
	    cmap.unmap_all(_unicodes);
	// make sure we only use uniquely identified glyphs
	HashMap<uint32_t, int> unicode_usage(-1);
	for (int g = 0; g < _unicodes.size(); g++)
	    if (_unicodes[g]) {
		int i = unicode_usage.find_force(_unicodes[g], g);
		if (i != g)
		    _unicodes[i] = _unicodes[g] = 0;
	    }
	_got_unicodes = true;
    }

    if (gi >= 0 && gi < _unicodes.size() && _unicodes[gi] > 0 && _unicodes[gi] <= 0xFFFF) {
	char buf[10];
	sprintf(buf, "uni%04X", _unicodes[gi]);
	return PermString(buf);
    } else
	return permprintf("index%d", gi);
}

void
TrueTypeBoundsCharstringProgram::glyph_names(Vector<PermString> &gn) const
{
    gn.clear();
    for (int gi = 0; gi < _nglyphs; gi++)
	gn.push_back(glyph_name(gi));
}

Charstring *
TrueTypeBoundsCharstringProgram::glyph(int gi) const
{
    if (gi < 0 || gi >= _nglyphs)
	return 0;
    if (_charstrings.size() <= gi)
	_charstrings.resize(gi + 1, (Charstring *) 0);
    if (!_charstrings[gi]) {
	// calculate glyf offsets
	uint32_t offset, end_offset;
	if (_loca_long) {
	    offset = _loca.u32(gi * 4);
	    end_offset = _loca.u32(gi * 4 + 4);
	} else {
	    offset = _loca.u16(gi * 2) * 2;
	    end_offset = _loca.u16(gi * 2 + 2) * 2;
	}

	// fetch bounding box from glyf
	int ncontours, xmin, ymin, xmax, ymax;
	if (offset != end_offset) {
	    if (offset > end_offset || offset + 10 > end_offset
		|| end_offset > (uint32_t) _glyf.length())
		return 0;

	    ncontours = _glyf.s16(offset);
	    xmin = _glyf.s16(offset + 2);
	    ymin = _glyf.s16(offset + 4);
	    xmax = _glyf.s16(offset + 6);
	    ymax = _glyf.s16(offset + 8);
	} else
	    ncontours = xmin = ymin = xmax = ymax = 0;

	// fetch horizontal metrics
	int advance_width, lsb;
	if (gi >= _nhmtx) {
	    advance_width = (_nhmtx ? _hmtx.u16((_nhmtx - 1) * 4) : 0);
	    int hmtx_offset = _nhmtx * 4 + (gi - _nhmtx) * 2;
	    lsb = (hmtx_offset + 2 <= _hmtx.length() ? _hmtx.s16(hmtx_offset) : 0);
	} else {
	    advance_width = _hmtx.u16(gi * 4);
	    lsb = _hmtx.s16(gi * 4 + 2);
	}

	// make charstring
	Type1CharstringGen gen;
	if (ncontours == 0) {
	    gen.gen_number(0, 'X');
	    gen.gen_number(advance_width);
	    gen.gen_command(Charstring::cHsbw);
	} else {
	    gen.gen_number(lsb, 'X');
	    gen.gen_number(advance_width);
	    gen.gen_command(Charstring::cHsbw);
	    gen.gen_moveto(Point(xmin, ymin), false, false);
	    if (xmax != xmin || ymax == ymin)
		gen.gen_number(xmax - xmin, 'x');
	    if (ymax != ymin)
		gen.gen_number(ymax - ymin, 'y');
	    gen.gen_command(ymax == ymin ? Charstring::cHlineto
			    : (xmax == xmin ? Charstring::cVlineto
			       : Charstring::cRlineto));
	    gen.gen_command(Charstring::cClosepath);
	}
	gen.gen_command(Charstring::cEndchar);

	_charstrings[gi] = gen.output();
    }
    return _charstrings[gi];
}

}
