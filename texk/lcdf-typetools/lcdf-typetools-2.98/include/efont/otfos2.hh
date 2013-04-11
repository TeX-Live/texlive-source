// -*- related-file-name: "../../libefont/otfos2.cc" -*-
#ifndef EFONT_OTFOS2_HH
#define EFONT_OTFOS2_HH
#include <efont/otf.hh>
#include <efont/otfdata.hh>
#include <lcdf/error.hh>
namespace Efont { namespace OpenType {

class Os2 { public:

    Os2(const Data &, ErrorHandler * = 0);
    // default destructor

    bool ok() const			{ return _error >= 0; }
    int error() const			{ return _error; }

    enum Offsets { O_AVGCHARWIDTH = 2, O_SUBSCRIPTXSIZE = 10,
		   O_SUBSCRIPTYSIZE = 12, O_SUBSCRIPTXOFFSET = 14,
		   O_SUBSCRIPTYOFFSET = 16, O_SUPERSCRIPTXSIZE = 18,
		   O_SUPERSCRIPTYSIZE = 20, O_SUPERSCRIPTXOFFSET = 22,
		   O_SUPERSCRIPTYOFFSET = 24, O_STRIKEOUTSIZE = 26,
		   O_STRIKEOUTPOSITION = 28, O_VENDORID = 58,
		   O_TYPOASCENDER = 68, O_TYPODESCENDER = 70,
		   O_TYPOLINEGAP = 72, O_XHEIGHT = 86, O_CAPHEIGHT = 88 };
    enum { HEADER_SIZE = 2 };

    inline int16_t typo_ascender() const throw (Bounds);
    inline int16_t typo_descender() const throw (Bounds);
    inline int16_t typo_line_gap() const throw (Bounds);
    inline int16_t x_height() const throw (Bounds);
    inline int16_t cap_height() const throw (Bounds);
    inline String vendor_id() const throw ();

  private:

    Data _data;
    int _error;

    int parse_header(ErrorHandler *);

};


inline int16_t Os2::typo_ascender() const throw (Bounds)
{
    return _data.s16(O_TYPOASCENDER);
}

inline int16_t Os2::typo_descender() const throw (Bounds)
{
    return _data.s16(O_TYPODESCENDER);
}

inline int16_t Os2::typo_line_gap() const throw (Bounds)
{
    return _data.s16(O_TYPOLINEGAP);
}

inline int16_t Os2::x_height() const throw (Bounds)
{
    return _data.s16(O_XHEIGHT);
}

inline int16_t Os2::cap_height() const throw (Bounds)
{
    return _data.s16(O_CAPHEIGHT);
}

inline String Os2::vendor_id() const throw ()
{
    return _data.substring(O_VENDORID, 4);
}

}}
#endif
