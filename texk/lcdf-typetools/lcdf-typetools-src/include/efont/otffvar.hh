// -*- related-file-name: "../../libefont/otffvar.cc" -*-
#ifndef EFONT_OTFFVAR_HH
#define EFONT_OTFFVAR_HH
#include <efont/otf.hh>
#include <lcdf/error.hh>
#include <utility>
namespace Efont { namespace OpenType {
class Axis;
class FvarInstance;

class Fvar { public:
    Fvar(Data);
    // default destructor

    inline int naxes() const;
    inline Axis axis(int i) const;
    inline int ninstances() const;
    inline FvarInstance instance(int i) const;

  private:
    const unsigned char* _d;
    const unsigned char* _id;

    enum { HEADER_SIZE = 16, AXIS_SIZE = 20,
           X_AXISOFF = 4, X_AXISCOUNT = 8, X_AXISSIZE = 10,
           X_INSTANCECOUNT = 12, X_INSTANCESIZE = 14 };

};

class Axis { public:
    inline Axis(const unsigned char* d) : _d(d) {}

    inline Tag tag() const;
    inline double min_value() const;
    inline double default_value() const;
    inline double max_value() const;
    inline uint16_t flags() const;
    inline int nameid() const;

  private:
    const unsigned char* _d;
};

class FvarInstance { public:
    inline FvarInstance(const unsigned char* d, int naxes) : _d(d), _naxes(naxes) {}

    inline int nameid() const;
    inline double coord(int) const;

  private:
    const unsigned char* _d;
    const int _naxes;
};


inline int Fvar::naxes() const {
    return Data::u16_aligned(_d + X_AXISCOUNT);
}

inline Axis Fvar::axis(int i) const {
    assert(i >= 0 && i < naxes());
    return Axis(_d + Data::u16_aligned(_d + X_AXISOFF) + i * Data::u16_aligned(_d + X_AXISSIZE));
}

inline int Fvar::ninstances() const {
    return _id ? Data::u16_aligned(_d + X_INSTANCECOUNT) : 0;
}

inline FvarInstance Fvar::instance(int i) const {
    assert(i >= 0 && i < ninstances());
    return FvarInstance(_id + i * Data::u16_aligned(_d + X_INSTANCESIZE), naxes());
}


inline Tag Axis::tag() const {
    return Tag(Data::u32_aligned16(_d));
}

inline double Axis::min_value() const {
    return Data::fixed_aligned16(_d + 4);
}

inline double Axis::default_value() const {
    return Data::fixed_aligned16(_d + 8);
}

inline double Axis::max_value() const {
    return Data::fixed_aligned16(_d + 12);
}

inline uint16_t Axis::flags() const {
    return Data::u16_aligned(_d + 16);
}

inline int Axis::nameid() const {
    return Data::u16_aligned(_d + 18);
}


inline int FvarInstance::nameid() const {
    return Data::u16_aligned(_d);
}

inline double FvarInstance::coord(int ax) const {
    assert(ax >= 0 && ax < _naxes);
    return Data::fixed_aligned16(_d + 4 + ax * 4);
}

}}
#endif
