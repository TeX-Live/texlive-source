#ifndef OTFTOTFM_SECONDARY_HH
#define OTFTOTFM_SECONDARY_HH
#include <efont/otfcmap.hh>
#include <efont/cff.hh>
class Metrics;
class Transform;
struct Setting;

struct FontInfo {
    const Efont::OpenType::Font *otf;
    const Efont::Cff::Font *cff;
    const Efont::OpenType::Cmap *cmap;
};

class Secondary { public:
    Secondary()				: _next(0) { }
    virtual ~Secondary();
    void set_next(Secondary *s)		{ _next = s; }
    typedef Efont::OpenType::Glyph Glyph;
    virtual bool encode_uni(int code, PermString name, uint32_t uni, Metrics &, ErrorHandler *);
    virtual bool setting(uint32_t uni, Vector<Setting> &, Metrics &, ErrorHandler *);
  private:
    Secondary *_next;
};

class T1Secondary : public Secondary { public:
    T1Secondary(const FontInfo &, const String &font_name, const String &otf_file_name);
    bool encode_uni(int code, PermString name, uint32_t uni, Metrics &, ErrorHandler *);
    bool setting(uint32_t uni, Vector<Setting> &, Metrics &, ErrorHandler *);
  private:
    const FontInfo &_finfo;
    String _font_name;
    String _otf_file_name;
    int _xheight;
    int _spacewidth;
    bool char_setting(Vector<Setting> &, Metrics &, int uni, ...);
    enum { J_NODOT = -1031892 /* unlikely value */ };
    int dotlessj_font(Metrics &, ErrorHandler *, Glyph &dj_glyph);
};

bool char_bounds(int bounds[4], int &width, const FontInfo &,
		 const Transform &, uint32_t uni);

int char_one_bound(const FontInfo &, const Transform &,
		   int dimen, bool max, int best, uint32_t uni, ...);

#endif
