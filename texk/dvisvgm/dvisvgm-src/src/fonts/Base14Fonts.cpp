#include <algorithm>
#include "Base14Fonts.hpp"

using namespace std;

extern const MemoryFontData Dingbats_cff;
extern const MemoryFontData NimbusMonoPS_Bold_cff;
extern const MemoryFontData NimbusMonoPS_BoldItalic_cff;
extern const MemoryFontData NimbusMonoPS_Italic_cff;
extern const MemoryFontData NimbusMonoPS_Regular_cff;
extern const MemoryFontData NimbusRoman_Bold_cff;
extern const MemoryFontData NimbusRoman_BoldItalic_cff;
extern const MemoryFontData NimbusRoman_Italic_cff;
extern const MemoryFontData NimbusRoman_Regular_cff;
extern const MemoryFontData NimbusSans_Bold_cff;
extern const MemoryFontData NimbusSans_BoldItalic_cff;
extern const MemoryFontData NimbusSans_Italic_cff;
extern const MemoryFontData NimbusSans_Regular_cff;
extern const MemoryFontData StandardSymbolsPS_cff;

const MemoryFontData* find_base14_font (const string &fontname) {
	struct FontData {
		const char *name;
		const MemoryFontData *font;
	} names[14] = {
		{"Courier", &NimbusMonoPS_Regular_cff},
		{"Courier-Oblique", &NimbusMonoPS_Italic_cff},
		{"Courier-Bold", &NimbusMonoPS_Bold_cff},
		{"Courier-BoldOblique", &NimbusMonoPS_BoldItalic_cff},
		{"Helvetica", &NimbusSans_Regular_cff},
		{"Helvetica-Oblique", &NimbusSans_Italic_cff},
		{"Helvetica-Bold", &NimbusSans_Bold_cff},
		{"Helvetica-BoldOblique", &NimbusSans_BoldItalic_cff},
		{"Times-Roman", &NimbusRoman_Regular_cff},
		{"Times-Italic", &NimbusRoman_Italic_cff},
		{"Times-Bold", &NimbusRoman_Bold_cff},
		{"Times-BoldItalic", &NimbusRoman_BoldItalic_cff},
		{"Symbol", &StandardSymbolsPS_cff},
		{"ZapfDingbats", &Dingbats_cff}
	};
	auto it = find_if(begin(names), end(names), [&fontname](const FontData &data) {
		return data.name == fontname;
	});
	return it != end(names) ? it->font : nullptr;
}
