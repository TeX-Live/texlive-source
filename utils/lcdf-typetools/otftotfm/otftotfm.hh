#ifndef OTFTOTFM_OTFTOTFM_HH
#define OTFTOTFM_OTFTOTFM_HH

String suffix_font_name(const String &font_name, const String &suffix);

void output_metrics(Metrics &metrics, const String &ps_name, int boundary_char,
	const FontInfo &finfo,
	const String &encoding_name, const String &encoding_file,
	const String &font_name,
	void (*dvips_include)(const String &ps_name, StringAccum &, ErrorHandler *),
	ErrorHandler *errh);

int font_x_height(const FontInfo &, const Transform &);
int font_cap_height(const FontInfo &, const Transform &);
int font_ascender(const FontInfo &, const Transform &);
double font_slant(const FontInfo &);

#endif
