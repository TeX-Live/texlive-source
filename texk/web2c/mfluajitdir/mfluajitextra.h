/* mfluajitextra.h: banner etc. for MFLuaJIT.

   This is included by MFLuaJIT, from mfluajitextra.c
*/

#include <mfluadir/mflua_version.h> /* for MFLUA_VERSION */
#ifdef  BANNER 
#undef  BANNER
#define BANNER "This is MFLuaJIT, Version 2.7182818-" MFLUA_VERSION
#endif 
#define COPYRIGHT_HOLDER "L. Scarso"
#define AUTHOR NULL
#define PROGRAM_HELP MFLUAJITHELP
#define BUG_ADDRESS "luigi.scarso@gmail.com"
#define DUMP_VAR MFbasedefault
#define DUMP_LENGTH_VAR basedefaultlength
#define DUMP_OPTION "base"
#ifdef DOS
#define DUMP_EXT ".bas"
#else
#define DUMP_EXT ".base"
#endif
#define INPUT_FORMAT kpse_mf_format
#define INI_PROGRAM "inimfluajit"
#define VIR_PROGRAM "virmfluajit"
#define MFLuaJIT