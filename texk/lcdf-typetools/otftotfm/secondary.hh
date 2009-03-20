#ifndef OTFTOTFM_SECONDARY_HH
#define OTFTOTFM_SECONDARY_HH
#include <efont/otfcmap.hh>
#include <efont/cff.hh>
class Metrics;
class Transform;
struct Setting;
namespace Efont { class TrueTypeBoundsCharstringProgram; }

struct FontInfo {

    const Efont::OpenType::Font *otf;
    const Efont::OpenType::Cmap *cmap;

    Efont::Cff *cff_file;
    const Efont::Cff::Font *cff;

    const Efont::OpenType::Post *post;
    const Efont::OpenType::Name *name;

    FontInfo(const Efont::OpenType::Font *otf, ErrorHandler *);
    ~FontInfo();

    bool ok() const;

    int nglyphs() const			{ return _nglyphs; }
    bool glyph_names(Vector<PermString> &) const;
    int glyphid(PermString) const;
    const Efont::CharstringProgram *program() const;

    bool is_fixed_pitch() const;
    double italic_angle() const;

    String family_name() const;
    String postscript_name() const;

  private:

    int _nglyphs;
    mutable Vector<PermString> _glyph_names;
    mutable bool _got_glyph_names;
    mutable Vector<uint32_t> _unicodes;
    mutable Efont::TrueTypeBoundsCharstringProgram *_ttb_program;

};

class Secondary { public:
    Secondary()				: _next(0) { }
    virtual ~Secondary();
    void set_next(Secondary *s)		{ _next = s; }
    typedef Efont::OpenType::Glyph Glyph;
    virtual bool encode_uni(int code, PermString name, uint32_t uni, Metrics &, ErrorHandler *);
    virtual int setting(uint32_t uni, Vector<Setting> &, Metrics &, ErrorHandler *);
  private:
    Secondary *_next;
};

class T1Secondary : public Secondary { public:
    T1Secondary(const FontInfo &, const String &font_name, const String &otf_file_name);
    bool encode_uni(int code, PermString name, uint32_t uni, Metrics &, ErrorHandler *);
    int setting(uint32_t uni, Vector<Setting> &, Metrics &, ErrorHandler *);
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
