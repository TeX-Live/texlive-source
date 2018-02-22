#ifndef OTFTOTFM_SECONDARY_HH
#define OTFTOTFM_SECONDARY_HH
#include <efont/otfcmap.hh>
#include <efont/cff.hh>
#include "setting.hh"
class Metrics;
class Transform;
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

    int nglyphs() const                 { return _nglyphs; }
    bool glyph_names(Vector<PermString> &) const;
    int glyphid(PermString) const;
    const Efont::CharstringProgram *program() const;
    int units_per_em() const {
        return program()->units_per_em();
    }

    bool is_fixed_pitch() const;
    double italic_angle() const;
    double x_height(const Transform& font_xform) const;

    void set_is_fixed_pitch(bool is_fixed_pitch) {
        _override_is_fixed_pitch = true;
        _is_fixed_pitch = is_fixed_pitch;
    }
    void set_italic_angle(double italic_angle) {
        _override_italic_angle = true;
        _italic_angle = italic_angle;
    }
    enum { x_height_auto = 0, x_height_explicit,
           x_height_os2, x_height_x };
    void set_x_height(int source, double x_height) {
        _override_x_height = source;
        _x_height = x_height;
    }

    String family_name() const;
    String postscript_name() const;

  private:

    int _nglyphs;
    mutable Vector<PermString> _glyph_names;
    mutable bool _got_glyph_names;
    mutable Vector<uint32_t> _unicodes;
    mutable Efont::TrueTypeBoundsCharstringProgram *_ttb_program;
    bool _override_is_fixed_pitch;
    bool _override_italic_angle;
    bool _is_fixed_pitch;
    uint8_t _override_x_height;
    double _italic_angle;
    double _x_height;

};

class Secondary { public:
    Secondary(const FontInfo& finfo)    : _finfo(finfo), _next(0) { }
    virtual ~Secondary();
    void set_next(Secondary *s)         { _next = s; }
    typedef Efont::OpenType::Glyph Glyph;
    bool encode_uni(int code, PermString name, const uint32_t *uni_begin, const uint32_t *uni_end, Metrics &metrics, ErrorHandler *errh);
    virtual bool encode_uni(int code, PermString name, uint32_t uni, Metrics &, ErrorHandler *);
    virtual int setting(uint32_t uni, Vector<Setting> &, Metrics &, ErrorHandler *);
  protected:
    const FontInfo& _finfo;

    class SettingSet { public:
        inline SettingSet(Secondary* s, Vector<Setting>& v, Metrics& m);
        inline SettingSet& kern(int kern_type);
        SettingSet& show(int uni);
        inline bool ok() const;
    private:
        Secondary* s_;
        Vector<Setting>& v_;
        int original_size_;
        Metrics& m_;
        int kern_type_;
        bool ok_;
    };
    inline SettingSet set(Vector<Setting>& v, Metrics& m);
    friend class SettingSet;
  private:
    Secondary *_next;
};

class T1Secondary : public Secondary { public:
    T1Secondary(const FontInfo &, const String &font_name, const String &otf_file_name);
    bool encode_uni(int code, PermString name, uint32_t uni, Metrics &, ErrorHandler *);
    int setting(uint32_t uni, Vector<Setting> &, Metrics &, ErrorHandler *);
  private:
    String _font_name;
    String _otf_file_name;
    int _units_per_em;
    int _xheight;
    int _spacewidth;
    enum { J_NODOT = -1031892 /* unlikely value */ };
    int dotlessj_font(Metrics &, ErrorHandler *, Glyph &dj_glyph);
};

bool char_bounds(double bounds[4], double& width, const FontInfo &,
                 const Transform &, uint32_t uni);

double char_one_bound(const FontInfo &, const Transform &,
                      int dimen, bool max, double best, int uni, ...);

inline Secondary::SettingSet::SettingSet(Secondary* s, Vector<Setting>& v, Metrics& m)
    : s_(s), v_(v), original_size_(v.size()), m_(m), kern_type_(Setting::KERN),
      ok_(true) {
}

inline Secondary::SettingSet& Secondary::SettingSet::kern(int kern_type) {
    kern_type_ = kern_type;
    return *this;
}

inline bool Secondary::SettingSet::ok() const {
    return ok_;
}

inline Secondary::SettingSet Secondary::set(Vector<Setting>& v, Metrics& m) {
    return SettingSet(this, v, m);
}

#endif
