#define BUG_ADDRESS "tex-k@tug.org"

#if defined(WIN32) && defined(KPATHSEA)
#undef fopen
#undef vfprintf
#define fopen    fsyscp_fopen
#define vfprintf win32_vfprintf
#endif

