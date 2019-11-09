/* mfluaextra.h: banner etc. for MFLua.

   This is included by MFLua, from mfluaextra.c
*/

#include <mfluadir/mflua_version.h> /* for MFLUA_VERSION and BANNER */

/*#define BANNER "This is MFLua, Version 2.7182818-" MFLUA_VERSION*/
#define COPYRIGHT_HOLDER "L. Scarso"
#define AUTHOR NULL
#define PROGRAM_HELP MFLUAHELP
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
#define INI_PROGRAM "inimflua"
#define VIR_PROGRAM "virmflua"
#define MFLua

