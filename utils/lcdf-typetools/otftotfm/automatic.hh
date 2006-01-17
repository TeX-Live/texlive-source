#ifndef OTFTOTFM_AUTOMATIC_HH
#define OTFTOTFM_AUTOMATIC_HH
#include <lcdf/string.hh>
class ErrorHandler;

enum { O_ENCODING = 0, O_TFM, O_PL, O_VF, O_VPL, O_TYPE1, O_MAP, O_MAP_PARENT,
       NUMODIR };

extern bool automatic;
extern bool no_create;
String getodir(int o, ErrorHandler *);
bool setodir(int o, const String &);
bool set_vendor(const String &);
bool set_typeface(const String &, bool override);
bool set_map_file(const String &);
const char *odirname(int o);
void update_odir(int o, String file, ErrorHandler *);
String installed_type1(const String &otf_filename, const String &ps_fontname, bool allow_generate, ErrorHandler *);
String installed_type1_dotlessj(const String &otf_filename, const String &ps_fontname, bool allow_generate, ErrorHandler *);
int update_autofont_map(const String &fontname, String mapline, ErrorHandler *);
String locate_encoding(String encfile, ErrorHandler *, bool literal = false);

#endif
