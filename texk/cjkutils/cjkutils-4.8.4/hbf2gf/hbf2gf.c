#define banner  \
"hbf2gf (CJK ver. 4.8.4)" \
 \

#define TRUE 1
#define FALSE 0 \

#define STRING_LENGTH 255 \

#define FILE_NAME_LENGTH 1024 \
 \

#define VERSION  \
"\n" \
"Copyright (C) 1996-1999 Werner Lemberg.\n" \
"There is NO warranty.  You may redistribute this software\n" \
"under the terms of the GNU General Public License\n" \
"and the HBF library copyright.\n" \
"\n" \
"For more information about these matters, see the files\n" \
"named COPYING and hbf.c.\n" \
"\n" \

#define USAGE  \
"\n" \
"Usage: hbf2gf [-q] configuration_file[.cfg]\n" \
"       hbf2gf [options] font_name x_resolution [y_scale | y_resolution]\n" \
"       hbf2gf -t [-q] font_name\n" \
"\n" \
"  Convert a font in HBF format to TeX's GF resp. PK format.\n" \
"\n" \
"         -q             be silent\n" \
"         -p             don't produce a PL file\n" \
"         -g             don't produce a GF file\n" \
"         -n             use no resolution in extension (only `.gf')\n" \
"         -t             test for font_name (returns 0 on success)\n" \
"         --help         print this message and exit\n" \
"         --version      print version number and exit\n" \
"\n" \

#define GF_ID 131
#define PRE 247 \

#define header " hbf2gf output " \

#define BOC 67
#define BOC1 68
#define EOC 69 \

#define _2_16 65536.0
#define _2_20 1048576.0 \

#define XXX1 239
#define XXX2 240
#define XXX3 241
#define XXX4 242 \

#define YYY 243 \

#define POST 248 \

#define CHAR_LOC 245
#define CHAR_LOC0 246 \

#define POSTPOST 249
#define POSTPOST_ID 223 \

#define BLACK 1
#define WHITE 0 \

#define PIXEL_MAXVAL 255
#define SCALE 4096
#define HALFSCALE 2048 \

#define MAX_CHAR_SIZE 1023 \

#define PAINT_(x) (x) 
#define PAINT1 64
#define PAINT2 65
#define PAINT3 66 \

#define SKIP0 70
#define SKIP1 71
#define SKIP2 72
#define SKIP3 73 \

#define NEW_ROW_(x) ((x) +74)  \

#define NOOP 244 \

#define EXTENSION_LENGTH 8 \

#define GFTOPK_NAME "gftopk"
#define PLTOTF_NAME "pltotf"
#define OVP2OVF_NAME "ovp2ovf" \

#define PRINTER_MIN_RES_X 50
#define PRINTER_MIN_RES_Y 50 \

#define VALID_SUBCODE 1 \

/*4:*/
#line 191 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*10:*/
#line 408 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#ifdef HAVE_CONFIG_H
#include <c-auto.h> 
#endif
#include <ctype.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#ifdef TM_IN_SYS_TIME
#include <sys/time.h> 
#endif
#include "hbf.h"



/*:10*//*69:*/
#line 2473 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#if defined(HAVE_LIBKPATHSEA)
#include "kpathsea/kpathsea.h"

#elif defined(HAVE_EMTEXDIR)
#include "emtexdir.h"

#elif defined(HAVE_MIKTEX)
#include "miktex.h"
#endif

/*:69*/
#line 192 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*11:*/
#line 429 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_file(void);


/*:11*//*13:*/
#line 491 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_pre(void);


/*:13*//*16:*/
#line 579 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_data(void);


/*:16*//*20:*/
#line 690 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_post(void);


/*:20*//*25:*/
#line 834 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void fputl(long,FILE*);


/*:25*//*29:*/
#line 994 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void make_pixel_array(void);


/*:29*//*36:*/
#line 1222 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static
#ifdef __GNUC__
__inline__
#endif
void read_row(unsigned char*);


/*:36*//*39:*/
#line 1274 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static
#ifdef __GNUC__
__inline__
#endif
void write_row(unsigned char*);


/*:39*//*41:*/
#line 1329 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_coding(void);


/*:41*//*45:*/
#line 1490 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_pl(void);


/*:45*//*47:*/
#line 1587 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_ovp(void);


/*:47*//*50:*/
#line 1732 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_job(void);


/*:50*//*54:*/
#line 1981 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void read_config(void);


/*:54*//*64:*/
#line 2312 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static int fsearch(const char*);


/*:64*//*67:*/
#line 2451 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void config_error(const char*);


/*:67*//*71:*/
#line 2494 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static const char*TeX_search_version(void);


/*:71*//*74:*/
#line 2529 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#ifdef HAVE_EMTEXDIR
extern int setup_list(struct emtex_dir*,char*,const char*,unsigned);
static int dir_setup(struct emtex_dir*,const char*,const char*,unsigned);
static char*file_find(char*,struct emtex_dir*);
#endif


/*:74*//*79:*/
#line 2621 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static char*TeX_search_cfg_file(char*);
static char*TeX_search_hbf_file(char*);


/*:79*/
#line 193 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*2:*/
#line 127 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

int nmb_files= -1;
int unicode= FALSE;

int testing= FALSE;

int mf_like= FALSE;
int file_number= 0;

double x_resolution= 0.0;
double y_scale= 1.0;

int pk_files= TRUE;
int tfm_files= TRUE;
int long_extension= TRUE;
int quiet= FALSE;

char config_file[FILE_NAME_LENGTH+4+1];

char output_name[STRING_LENGTH+1];

FILE*config,*out;
HBF*hbf;

#ifdef msdos 
#define WRITE_BIN   "wb"
#define WRITE_TXT   "wt"
#define READ_BIN    "rb"
#define READ_TXT    "rt"
#else
#define WRITE_BIN   "w"
#define WRITE_TXT   "w"
#define READ_BIN    "r"
#define READ_TXT    "r"
#endif

int end_of_file= FALSE;


/*:2*//*15:*/
#line 538 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

long char_adr[256];
long*char_adr_p;

int pk_offset_x;


double tfm_offset_x;
int pk_offset_y;

double tfm_offset_y;

int input_size_x;
int input_size_y;
const char*font_encoding;
int pk_width;
int pk_output_size_x;

double tfm_output_size_x;
int pk_output_size_y;
double tfm_output_size_y;

double design_size= 10.0;
double target_size_x;
double target_size_y;
double magstep_x;
double magstep_y;
double slant;
int rotation;

double mag_x;
double mag_y;

int empty_char;
int last_char;

int dot_count;



/*:15*//*19:*/
#line 667 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

char coding[STRING_LENGTH+1];
char comment[STRING_LENGTH+1];

unsigned long checksum;

long pk_total_min_x;
long pk_total_max_x;
long pk_total_min_y;
long pk_total_max_y;

int dpi_x;
int dpi_y;
double ppp_x;
double ppp_y;


/*:19*//*27:*/
#line 871 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

HBF_CHAR code;
const unsigned char*bitmap;

const unsigned char*bP;

unsigned char out_char[MAX_CHAR_SIZE*MAX_CHAR_SIZE+1];

unsigned char*out_char_p;

unsigned char pixelrow[MAX_CHAR_SIZE];
unsigned char temp_pixelrow[MAX_CHAR_SIZE];
unsigned char new_pixelrow[MAX_CHAR_SIZE+1];

int curr_row;

long grayrow[MAX_CHAR_SIZE];

long s_mag_x,s_mag_y,s_slant;


/*:27*//*38:*/
#line 1269 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

int threshold= 128;


/*:38*//*49:*/
#line 1721 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

char job_extension[EXTENSION_LENGTH+1];
char rm_command[STRING_LENGTH+1];
char cp_command[STRING_LENGTH+1];
char pk_directory[STRING_LENGTH+1];
char tfm_directory[STRING_LENGTH+1];

int ofm_file= FALSE;


/*:49*//*53:*/
#line 1976 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

char Buffer[STRING_LENGTH+1];


/*:53*//*58:*/
#line 2094 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

int offset_x;
int offset_y;

HBF_CHAR user_min_char;
int have_min_char= FALSE;

/*:58*//*60:*/
#line 2251 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

HBF_CHAR min_char,max_char;


/*:60*//*62:*/
#line 2278 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

char b2_codes[256];
unsigned char min_2_byte,max_2_byte;
int nmb_2_bytes= 0;


/*:62*//*70:*/
#line 2485 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#if defined(HAVE_EMTEXDIR)
char emtex_version_string[]= "emTeXdir";
#elif !defined(HAVE_MIKTEX)
char no_version_string[]= "no search library";
#endif


/*:70*//*73:*/
#line 2522 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#ifdef HAVE_EMTEXDIR
struct emtex_dir cfg_path,hbf_path;
#endif


/*:73*//*76:*/
#line 2569 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#ifdef HAVE_EMTEXDIR
char name_buffer[FILE_NAME_LENGTH+1];
#endif


/*:76*/
#line 194 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"



int main(int argc,char*argv[])
{char*p;

/*78:*/
#line 2594 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#if defined(HAVE_LIBKPATHSEA)
kpse_set_program_name(argv[0],"hbf2gf");
kpse_init_prog("HBF2GF",300,"cx","cmr10");

#elif defined(HAVE_EMTEXDIR)
if(!dir_setup(&cfg_path,"HBFCFG",NULL,EDS_BANG))
{fprintf(stderr,
"Couldn't setup search path for configuration files\n");
exit(1);
}
if(!dir_setup(&hbf_path,"HBFONTS",NULL,EDS_BANG))
{fprintf(stderr,
"Couldn't setup search path for HBF header files\n");
exit(1);
}
#endif


/*:78*/
#line 200 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


/*7:*/
#line 307 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

if(argc==2)
{if(strcmp(argv[1],"--help")==0)
/*6:*/
#line 288 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{printf(USAGE);
exit(0);
}


/*:6*/
#line 310 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

else if(strcmp(argv[1],"--version")==0)
/*5:*/
#line 261 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{printf("\n");
printf(banner);
printf(" (%s)\n",TeX_search_version());
printf(VERSION);
exit(0);
}


/*:5*/
#line 312 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}

while(argc> 1)
{p= argv[1];
if(p[0]!='-')
break;
if(p[1]=='p')
tfm_files= FALSE;
else if(p[1]=='g')
pk_files= FALSE;
else if(p[1]=='n')
long_extension= FALSE;
else if(p[1]=='q')
quiet= TRUE;
else if(p[1]=='t')
testing= TRUE;

argv++;
argc--;
}

if(testing)
{if(argc!=2)
{fprintf(stderr,"Need exactly one parameter for `-t' option.\n");
fprintf(stderr,"Try `hbf2gf --help' for more information.\n");
exit(1);
}
}
else if(argc<2||argc> 4)
{fprintf(stderr,"Invalid number of parameters.\n");
fprintf(stderr,"Try `hbf2gf --help' for more information.\n");
exit(1);
}


/*:7*/
#line 202 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


if(!quiet)
printf("\n%s\n\n",banner);

strncpy(config_file,argv[1],FILE_NAME_LENGTH);
config_file[FILE_NAME_LENGTH]= '\0';

if(argc> 2||testing)
{int l= strlen(config_file);

if(l> 2)
config_file[l-2]= '\0';

else
{if(!quiet)
printf("`%s' can't be a subfont created by hbf2gf\n",
config_file);
exit(2);
}

mf_like= TRUE;
}

read_config();

if(mf_like)
/*8:*/
#line 352 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{if(unicode)
file_number= (int)strtol(&argv[1][strlen(argv[1])-2],
(char**)NULL,16);
else
file_number= atoi(&argv[1][strlen(argv[1])-2]);

x_resolution= atof(argv[2]);
if(x_resolution<PRINTER_MIN_RES_X)
{fprintf(stderr,"Invalid horizontal resolution\n");
exit(1);
}

if(argc> 3)
{y_scale= atof(argv[3]);
if(y_scale<0.01)
{fprintf(stderr,
"Invalid vertical scaling factor or resolution\n");
exit(1);
}
if(y_scale> 10.0)
y_scale= (double)x_resolution/y_scale;
}
}


/*:8*/
#line 229 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


/*28:*/
#line 903 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{int col,offset;

if(rotation)
{int tmp;


tmp= input_size_x;
input_size_x= input_size_y;
input_size_y= tmp;
}

if(mf_like)
{target_size_x= design_size*(x_resolution/dpi_x);
target_size_y= design_size*(x_resolution*y_scale/dpi_y);
}
else
target_size_x= target_size_y= design_size;
magstep_x= target_size_x/design_size;
magstep_y= target_size_y/design_size;

pk_offset_x= offset_x*magstep_x+0.5;
pk_offset_y= offset_y*magstep_y+0.5;
tfm_offset_x= offset_x/(dpi_x/72.27)/design_size;
tfm_offset_y= offset_y/(dpi_y/72.27)/design_size;

pk_width= input_size_x*mag_x*magstep_x+0.5;
pk_output_size_x= input_size_x*mag_x*magstep_x+
input_size_y*mag_y*magstep_y*slant+0.5;
pk_output_size_y= input_size_y*mag_y*magstep_y+0.5;
tfm_output_size_x= input_size_x*mag_x/
(dpi_x/72.27)/design_size;
tfm_output_size_y= input_size_y*mag_y/
(dpi_y/72.27)/design_size;
if(pk_output_size_x> MAX_CHAR_SIZE)
{fprintf(stderr,"Output character box width too big\n");
exit(1);
}
if(pk_output_size_y> MAX_CHAR_SIZE)
{fprintf(stderr,"Output character box height too big\n");
exit(1);
}

for(col= 0;col<input_size_x;++col)
grayrow[col]= HALFSCALE;

if(!mf_like)
code= (min_char&0xFF00)+min_2_byte;
else
{if((file_number<(unicode?0:1))||(file_number>=0x100))
{fprintf(stderr,"Invalid subfile number\n");
exit(1);
}

if(unicode)
{offset= 0;
code= file_number*0x100;
}
else
{offset= (file_number-1)*256%nmb_2_bytes;
code= (min_char&0xFF00)+min_2_byte+
(file_number-1)*256/nmb_2_bytes*0x100;
}

while(offset--)
while(!b2_codes[code++&0xFF])

;

if(code> max_char)
{fprintf(stderr,"Invalid subfile number\n");
exit(1);
}
}

s_mag_x= mag_x*magstep_x*SCALE;
s_mag_y= mag_y*magstep_y*SCALE;
s_slant= slant*SCALE;
}


/*:28*/
#line 231 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


/*9:*/
#line 387 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{int j,max_numb;

if(!mf_like)
{file_number= (unicode==TRUE?(min_char>>8):1);
if(nmb_files==-1)
max_numb= (unicode==TRUE?0x100:100);
else
max_numb= nmb_files;
}
else
max_numb= 1;

for(j= 0;(j<max_numb)&&!end_of_file;file_number++,j++)
write_file();

nmb_files= j;
}


/*:9*/
#line 233 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


if(tfm_files)
write_pl();
if(ofm_file)
write_ovp();
if(!mf_like)
write_job();

hbfClose(hbf);

exit(0);
return 0;
}


/*:4*//*12:*/
#line 437 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_file(void)
{char output_file[FILE_NAME_LENGTH+1];


if(pk_files)
{if(mf_like)
{if(unicode)
sprintf(output_file,"%s%02x.%.0igf",
output_name,file_number,
long_extension?(int)(x_resolution+0.5):0);
else
sprintf(output_file,"%s%02i.%.0igf",
output_name,file_number,
long_extension?(int)(x_resolution+0.5):0);
}
else
{if(unicode)
sprintf(output_file,"%s%02x.gf",output_name,file_number);
else
sprintf(output_file,"%s%02i.gf",output_name,file_number);
}
if(!(out= fopen(output_file,WRITE_BIN)))
{fprintf(stderr,"Couldn't open `%s'\n",output_file);
exit(1);
}
if(!quiet)
printf("Writing `%s' ",output_file);

write_pre();
write_data();
write_post();
fclose(out);

if(!quiet)
printf("\n");
}
else
write_data();
}


/*:12*//*14:*/
#line 498 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_pre(void)
{char out_s[40],s[20];
time_t secs_now;
struct tm*time_now;


strcpy(out_s,header);

secs_now= time(NULL);
time_now= localtime(&secs_now);
strftime(s,20,"%Y.%m.%d:%H.%M",time_now);
strcat(out_s,s);

fputc(PRE,out);
fputc(GF_ID,out);
fputc(strlen(out_s),out);
fputs(out_s,out);
}


/*:14*//*17:*/
#line 584 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_data(void)
{dot_count= 0;
char_adr_p= char_adr;

for(last_char= 0;(last_char<256)&&!end_of_file;last_char++)
/*18:*/
#line 609 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{if(dot_count++%10==0)
if(pk_files&&!quiet)
{printf(".");
fflush(stdout);
}

empty_char= FALSE;
make_pixel_array();
if(end_of_file)
return;

if(pk_files)
{*char_adr_p= ftell(out);
char_adr_p++;

if(empty_char)
{fputc(BOC1,out);
fputc((unsigned char)last_char,out);
fputc(0,out);
fputc(0,out);
fputc(0,out);
fputc(0,out);
fputc(EOC,out);
}
else
{fputc(BOC,out);
fputl(last_char,out);
fputl(-1L,out);
fputl(pk_offset_x,out);
fputl(pk_output_size_x+pk_offset_x,out);
fputl(pk_offset_y,out);
fputl(pk_output_size_y+pk_offset_y,out);

write_coding();

fputc(EOC,out);
}
}
}


/*:18*/
#line 590 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}


/*:17*//*21:*/
#line 695 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_post(void)
{long special_adr;
long post_adr;

long designsize= design_size*_2_20;

int pk_dx;
long tfm_width;

int i;
long temp;


ppp_x= dpi_x/72.27*magstep_x;
ppp_y= dpi_y/72.27*magstep_y;

pk_total_min_x= pk_offset_x;
pk_total_max_x= pk_output_size_x+2*pk_offset_x;
pk_total_min_y= pk_offset_y;
pk_total_max_y= pk_output_size_y+pk_offset_y;

pk_dx= pk_width+2*pk_offset_x;
tfm_width= (tfm_output_size_x+2*tfm_offset_x)*_2_20;


/*22:*/
#line 741 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

special_adr= ftell(out);

if(*coding)
{fputc(XXX1,out);
fputc(strlen(coding),out);
fputs(coding,out);
}

if(*comment)
{fputc(XXX1,out);
fputc(strlen(comment),out);
fputs(comment,out);
}


/*:22*/
#line 721 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*23:*/
#line 770 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

post_adr= ftell(out);
fputc(POST,out);
fputl(special_adr,out);

fputl(designsize,out);
fputl(checksum,out);
fputl(ppp_x*_2_16,out);
fputl(ppp_y*_2_16,out);
fputl(pk_total_min_x,out);
fputl(pk_total_max_x,out);
fputl(pk_total_min_y,out);
fputl(pk_total_max_y,out);

char_adr_p= char_adr;

if(pk_dx<256)
{for(i= 0;i<last_char;i++)
{fputc(CHAR_LOC0,out);
fputc(i,out);
fputc(pk_dx,out);
fputl(tfm_width,out);
fputl(*char_adr_p++,out);
}
}
else
{for(i= 0;i<last_char;i++)
{fputc(CHAR_LOC,out);
fputc(i,out);
fputl(pk_dx*_2_16,out);
fputl(0,out);
fputl(tfm_width,out);
fputl(*char_adr_p++,out);
}
}


/*:23*/
#line 722 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*24:*/
#line 821 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

fputc(POSTPOST,out);
fputl(post_adr,out);
fputc(GF_ID,out);
temp= ftell(out);
i= (int)(temp%4)+4;
while(i--)
fputc(POSTPOST_ID,out);


/*:24*/
#line 723 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}


/*:21*//*26:*/
#line 839 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void fputl(long num,FILE*f)
{fputc(num>>24,f);
fputc(num>>16,f);
fputc(num>>8,f);
fputc(num,f);
}


/*:26*//*30:*/
#line 999 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void make_pixel_array(void)
{unsigned char*prP;
unsigned char*temp_prP;
unsigned char*new_prP;
long*grP;

register unsigned char*xP;
register unsigned char*nxP;

register int row,col;
int rows_read= 0;
register int need_to_read_row= 1;

long frac_row_to_fill= SCALE;
long frac_row_left= s_mag_y;

int no_code= FALSE;


prP= pixelrow;
temp_prP= temp_pixelrow;
new_prP= new_pixelrow;
grP= grayrow;
out_char_p= out_char;

again:
if(b2_codes[code&0xFF])
{if(pk_files)
{bitmap= hbfGetBitmap(hbf,code);
bP= bitmap;


if(!bitmap)
empty_char= TRUE;
else
/*31:*/
#line 1058 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{if(pk_output_size_y==input_size_y)
temp_prP= prP;

curr_row= input_size_y-1;
for(row= 0;row<pk_output_size_y;++row)
{/*32:*/
#line 1072 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

if(pk_output_size_y==input_size_y)

read_row(prP);
else
{while(frac_row_left<frac_row_to_fill)
{if(need_to_read_row)
if(rows_read<input_size_y)
{read_row(prP);
++rows_read;
}

for(col= 0,xP= prP;col<input_size_x;++col,++xP)
grP[col]+= frac_row_left*(*xP);

frac_row_to_fill-= frac_row_left;
frac_row_left= s_mag_y;
need_to_read_row= 1;
}

/*33:*/
#line 1099 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

if(need_to_read_row)
if(rows_read<input_size_y)
{read_row(prP);
++rows_read;
need_to_read_row= 0;
}

for(col= 0,xP= prP,nxP= temp_prP;
col<input_size_x;++col,++xP,++nxP)
{register long g;


g= grP[col]+frac_row_to_fill*(*xP);
g/= SCALE;
if(g> PIXEL_MAXVAL)
g= PIXEL_MAXVAL;

*nxP= g;
grP[col]= HALFSCALE;
}

frac_row_left-= frac_row_to_fill;
if(frac_row_left==0)
{frac_row_left= s_mag_y;
need_to_read_row= 1;
}
frac_row_to_fill= SCALE;


/*:33*/
#line 1092 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}


/*:32*/
#line 1064 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*34:*/
#line 1140 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

if(pk_width==input_size_x&&s_slant==0)

write_row(temp_prP);
else
{register long g= HALFSCALE;
register long frac_col_to_fill= SCALE;
register long frac_col_left;
register int need_col= 0;


nxP= new_prP;

frac_col_left= (pk_output_size_y-row)*s_slant;
while(frac_col_left>=frac_col_to_fill)
{*(nxP++)= 0;
frac_col_left-= frac_col_to_fill;
}

if(frac_col_left> 0)
frac_col_to_fill-= frac_col_left;

for(col= 0,xP= temp_prP;col<input_size_x;++col,++xP)
{frac_col_left= s_mag_x;
while(frac_col_left>=frac_col_to_fill)
{if(need_col)
{++nxP;
g= HALFSCALE;
}

g+= frac_col_to_fill*(*xP);
g/= SCALE;
if(g> PIXEL_MAXVAL)
g= PIXEL_MAXVAL;

*nxP= g;
frac_col_left-= frac_col_to_fill;
frac_col_to_fill= SCALE;
need_col= 1;
}

if(frac_col_left> 0)
{if(need_col)
{++nxP;
g= HALFSCALE;
need_col= 0;
}

g+= frac_col_left*(*xP);
frac_col_to_fill-= frac_col_left;
}
}

/*35:*/
#line 1198 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

if(frac_col_to_fill> 0)
{--xP;
g+= frac_col_to_fill*(*xP);
}

if(!need_col)
{g/= SCALE;
if(g> PIXEL_MAXVAL)
g= PIXEL_MAXVAL;
*nxP= g;
}

*(++nxP)= 0;

write_row(new_prP);


/*:35*/
#line 1193 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}


/*:34*/
#line 1066 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}
}


/*:31*/
#line 1035 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

}
}
else
no_code= TRUE;

if((code&0xFF)==max_2_byte)
code+= 0xFF-(max_2_byte-min_2_byte);
if(code>=max_char)
{end_of_file= TRUE;
return;
}

code++;

if(no_code)
{no_code= FALSE;
goto again;
}
}


/*:30*//*37:*/
#line 1231 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static
#ifdef __GNUC__
__inline__
#endif
void read_row(unsigned char*pixelrow)
{register int col,bitshift,offset;
register unsigned char*xP;
register unsigned char item= 0;

if(rotation)
{bitshift= 7-(curr_row%8);
offset= (input_size_y+7)/8;
bP= bitmap+curr_row/8;
for(col= 0,xP= pixelrow;col<input_size_x;++col,++xP)
{*xP= ((*bP>>bitshift)&1)==1?PIXEL_MAXVAL:0;
bP+= offset;
}
curr_row--;
}
else
{bitshift= -1;
for(col= 0,xP= pixelrow;col<input_size_x;++col,++xP)
{if(bitshift==-1)
{item= *(bP++);
bitshift= 7;
}
*xP= ((item>>bitshift)&1)==1?PIXEL_MAXVAL:0;
--bitshift;
}
}
}


/*:37*//*40:*/
#line 1283 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static
#ifdef __GNUC__
__inline__
#endif
void write_row(unsigned char*pixelrow)
{register int col;
register unsigned char*xP;

for(col= 0,xP= pixelrow;col<pk_output_size_x;++col,++xP)
*(out_char_p++)= (*xP>=threshold)?1:0;

}


/*:40*//*42:*/
#line 1337 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_coding(void)
{register int count,skip;
register unsigned char paint;
register int x,y;
register unsigned char*cp;

x= 0;
y= 0;
cp= out_char+y*pk_output_size_x+x;
count= skip= 0;
paint= WHITE;
goto start;

while(y<pk_output_size_y)
{/*43:*/
#line 1361 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

count= 0;
x= 0;
cp= out_char+y*pk_output_size_x+x;

while(x<pk_output_size_x)
{if(*cp==paint)
count++;
else
{if(skip==0)
{if(count<=164)
fputc(NEW_ROW_(count),out);
else
{fputc(SKIP0,out);

if(count<256)
{fputc(PAINT1,out);
fputc(count,out);
}
else
{fputc(PAINT2,out);
fputc(count>>8,out);
fputc(count&0xFF,out);
}
}
}
else
{if(skip==1)
fputc(SKIP0,out);
else
{if(skip<256)
{fputc(SKIP1,out);
fputc(skip,out);
}
else
{fputc(SKIP2,out);
fputc(skip>>8,out);
fputc(skip&0xFF,out);
}
}
skip= 0;
if(count<64)
fputc(PAINT_(count),out);
else if(count<256)
{fputc(PAINT1,out);
fputc(count,out);
}
else
{fputc(PAINT2,out);
fputc(count>>8,out);
fputc(count&0xFF,out);
}
}
count= 0;
paint= BLACK;
break;
}
x++;
cp++;
}
if(x>=pk_output_size_x)
{skip++;
y++;
continue;
}


/*:43*/
#line 1352 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

start:
/*44:*/
#line 1429 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

while(x<pk_output_size_x)
{if(*cp==paint)
count++;
else
{if(count<64)
fputc(PAINT_(count),out);
else if(count<256)
{fputc(PAINT1,out);
fputc(count,out);
}
else
{fputc(PAINT2,out);
fputc(count>>8,out);
fputc(count&0xFF,out);
}
count= 1;
paint= BLACK-paint;
}
x++;
cp++;
}
if(paint==BLACK)
{if(count<64)
fputc(PAINT_(count),out);
else if(count<256)
{fputc(PAINT1,out);
fputc(count,out);
}
else
{fputc(PAINT2,out);
fputc(count>>8,out);
fputc(count&0xFF,out);
}
paint= WHITE;
}



/*:44*/
#line 1354 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

y++;
}
}


/*:42*//*46:*/
#line 1495 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_pl(void)
{int i,pos;
char output_file[FILE_NAME_LENGTH+1];
long t,sc;
char*s;
char tfm_header[]= "Created by hbf2gf";

file_number--;

if(mf_like)
{if(unicode)
sprintf(output_file,"%s%02x.pl",output_name,file_number);
else
sprintf(output_file,"%s%02i.pl",output_name,file_number);
}
else
sprintf(output_file,"%s.pl",output_name);

if(!(out= fopen(output_file,WRITE_TXT)))
{fprintf(stderr,"Couldn't open `%s'\n",output_file);
exit(1);
}
if(!quiet)
printf("\nWriting `%s'\n",output_file);

fprintf(out,
"\n(FAMILY %s%d)"
"\n(CODINGSCHEME CJK-%s)",
output_name,file_number,font_encoding);

fprintf(out,
"\n(DESIGNSIZE R %.6f)"
"\n(COMMENT DESIGNSIZE IS IN POINTS)"
"\n(COMMENT OTHER SIZES ARE MULTIPLES OF DESIGNSIZE)"
"\n(CHECKSUM O %lo)"
"\n(FONTDIMEN"
"\n   (SLANT R %.6f)"
"\n   (SPACE R 0.0)"
"\n   (STRETCH R 0.0)"
"\n   (SHRINK R 0.0)"
"\n   (XHEIGHT R 1.0)"
"\n   (QUAD R 1.0)"
"\n   (EXTRASPACE R 0.0)"
"\n   )",design_size,checksum,slant);

s= tfm_header;
i= strlen(s);
t= ((long)i)<<24;
sc= 16;
pos= 18;

fprintf(out,"\n");
while(i> 0)
{t|= ((long)(*(unsigned char*)s++))<<sc;
sc-= 8;
if(sc<0)
{fprintf(out,"\n(HEADER D %d O %lo)",pos,t);
t= 0;
sc= 24;
pos++;
}
i--;
}
if(t)
fprintf(out,"\n(HEADER D %d O %lo)",pos,t);
fprintf(out,"\n");

for(i= 0;i<256;i++)
{fprintf(out,
"\n(CHARACTER O %o"
"\n   (CHARWD R %.6f)"
"\n   (CHARHT R %.6f)"
"\n   (CHARDP R %.6f)"
"\n   (CHARIC R %.6f)"
"\n   )",
i,
tfm_output_size_x+2*tfm_offset_x,
tfm_output_size_y+tfm_offset_y,
-tfm_offset_y,
slant*(tfm_output_size_y+tfm_offset_y));
}

fclose(out);
}



/*:46*//*48:*/
#line 1592 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_ovp(void)
{int c,i,nmb_subfonts,remainder,count,pos;
char output_file[FILE_NAME_LENGTH+1];
long t,sc;
char*s;
char ofm_header[]= "Created by hbf2gf";

nmb_subfonts= ((max_char-(min_char&0xFF00))/256*nmb_2_bytes)
/256+1;
remainder= ((max_char-(min_char&0xFF00))/256*nmb_2_bytes)
%256;


for(count= 0;count<(max_char&0xFF);count++)
if(b2_codes[count])
remainder++;
if(remainder>=256)
nmb_subfonts++;

sprintf(output_file,"%s.ovp",output_name);

if(!(out= fopen(output_file,WRITE_TXT)))
{fprintf(stderr,"Couldn't open `%s'\n",output_file);
exit(1);
}
if(!quiet)
printf("\nWriting `%s'\n",output_file);

fprintf(out,
"\n(VTITLE Omega virtual font created by hbf2gf)"
"\n(DESIGNSIZE R %.6f)"
"\n(COMMENT DESIGNSIZE IS IN POINTS)"
"\n(COMMENT OTHER SIZES ARE MULTIPLES OF DESIGNSIZE)"
"\n(CHECKSUM O %lo)"
"\n(FONTDIMEN"
"\n   (SLANT R %.6f)"
"\n   (SPACE R 0.0)"
"\n   (STRETCH R 0.0)"
"\n   (SHRINK R 0.0)"
"\n   (XHEIGHT R 1.0)"
"\n   (QUAD R 1.0)"
"\n   (EXTRASPACE R 0.0)"
"\n   )",design_size,checksum,slant);

s= ofm_header;
i= strlen(s);
t= ((long)i)<<24;
sc= 16;
pos= 18;

fprintf(out,"\n");
while(i> 0)
{t|= ((long)(*(unsigned char*)s++))<<sc;
sc-= 8;
if(sc<0)
{fprintf(out,"\n(HEADER D %d O %lo)",pos,t);
t= 0;
sc= 24;
pos++;
}
i--;
}
if(t)
fprintf(out,"\n(HEADER D %d O %lo)",pos,t);
fprintf(out,"\n");

for(i= 0;i<nmb_subfonts;i++)
{fprintf(out,
"\n(MAPFONT D %i"
"\n   (FONTNAME %s%02i)"
"\n   (FONTCHECKSUM O %lo)"
"\n   (FONTAT R 1.0)"
"\n   (FONTDSIZE R %.6f)"
"\n   )",i,output_name,i+1,checksum,design_size);
}

for(c= min_char,i= 0,count= 0;c<=max_char;c++)
{if(b2_codes[c&0xFF]==VALID_SUBCODE)
{fprintf(out,
"\n(CHARACTER O %o"
"\n   (CHARWD R %.6f)"
"\n   (CHARHT R %.6f)"
"\n   (CHARDP R %.6f)"
"\n   (CHARIC R %.6f)"
"\n   (MAP"
"\n      (SELECTFONT D %i)"
"\n      (SETCHAR O %o)"
"\n      )"
"\n   )",
c,
tfm_output_size_x+2*tfm_offset_x,
tfm_output_size_y+tfm_offset_y,
-tfm_offset_y,
slant*(tfm_output_size_y+tfm_offset_y),
i,
count);

count++;
if(count==256)
{count= 0;
i++;
}
}
else
continue;
}

fclose(out);
}



/*:48*//*51:*/
#line 1737 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void write_job(void)
{FILE*out;
int i,j;
char buffer[FILE_NAME_LENGTH+1];


strcpy(buffer,output_name);
strcat(buffer,job_extension);
if(!(out= fopen(buffer,WRITE_TXT)))
{fprintf(stderr,"Couldn't open `%s'\n",buffer);
exit(1);
}
if(!quiet)
printf("\nWriting `%s'\n",buffer);

if(pk_files)
{if(unicode)
{for(i= (min_char>>8),j= 0;j<nmb_files;i++,j++)
fprintf(out,
"%s %s%02x.gf %s%s%02x.%.0ipk\n"
"%s %s%02x.gf\n",
GFTOPK_NAME,output_name,i,
pk_directory,output_name,i,
long_extension?(int)(dpi_x*magstep_x+0.5):0,
rm_command,output_name,i);
}
else
{for(i= 1;i<=nmb_files;i++)
fprintf(out,
"%s %s%02i.gf %s%s%02i.%.0ipk\n"
"%s %s%02i.gf\n",
GFTOPK_NAME,output_name,i,
pk_directory,output_name,i,
long_extension?(int)(dpi_x*magstep_x+0.5):0,
rm_command,output_name,i);
}
}

if(tfm_files)
{fprintf(out,
"\n"
"%s %s.pl %s.tfm\n"
"%s %s.pl\n"
"\n",
PLTOTF_NAME,output_name,output_name,
rm_command,output_name);

if(unicode)
{for(i= (min_char>>8),j= 0;j<nmb_files;i++,j++)
fprintf(out,
"%s %s.tfm %s%s%02x.tfm\n",
cp_command,output_name,
tfm_directory,output_name,i);
}
else
{for(i= 1;i<=nmb_files;i++)
fprintf(out,
"%s %s.tfm %s%s%02i.tfm\n",
cp_command,output_name,
tfm_directory,output_name,i);
}

fprintf(out,
"\n"
"%s %s.tfm",
rm_command,output_name);
}

if(ofm_file)
{fprintf(out,
"\n"
"%s %s.ovp %s.ovf %s.ofm\n"
"%s %s.ovp\n"
"\n",
OVP2OVF_NAME,output_name,output_name,output_name,
rm_command,output_name);
}

fclose(out);
}



/*:51*//*55:*/
#line 1989 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void read_config(void)
{HBF_BBOX*boxp;
char*real_config_file;


/*56:*/
#line 2041 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{int i,lastext= -1;

for(i= 0;config_file[i];i++)
if(config_file[i]=='.')
lastext= i;
else if(config_file[i]=='/'||
config_file[i]==':'||
config_file[i]=='\\')
lastext= -1;

if(lastext==-1)
strcat(config_file,".cfg");
}


/*:56*/
#line 1995 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


real_config_file= TeX_search_cfg_file(config_file);
if(!real_config_file)
{if(mf_like)
{if(!quiet)
printf("Couldn't find `%s'\n",config_file);
exit(2);
}
else
{fprintf(stderr,"Couldn't find `%s'\n",config_file);
exit(1);
}
}

if(!(config= fopen(real_config_file,READ_TXT)))
{if(!testing)
{fprintf(stderr,"Couldn't open `%s'\n",config_file);
exit(1);
}
else
{if(!quiet)
fprintf(stderr,"Couldn't find or open `%s'\n",config_file);
exit(2);
}
}

if(testing)
{if(!quiet)
printf("%s\n",real_config_file);
exit(0);
}

/*57:*/
#line 2058 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{char hbf_header[STRING_LENGTH+1];
char*real_hbf_header;

if(!fsearch("hbf_header"))
config_error("hbf_header");
else
strcpy(hbf_header,Buffer);

real_hbf_header= TeX_search_hbf_file(hbf_header);
if(!real_hbf_header)
{fprintf(stderr,"Couldn't find `%s'\n",hbf_header);
exit(1);
}

hbfDebug= 1;


if(!(hbf= hbfOpen(real_hbf_header)))
exit(1);

hbfDebug= 0;

boxp= hbfBitmapBBox(hbf);
input_size_x= boxp->hbf_height;
input_size_y= boxp->hbf_width;
font_encoding= hbfProperty(hbf,"HBF_CODE_SCHEME");

if(!fsearch("output_name"))
config_error("output_name");
else
strcpy(output_name,Buffer);
}


/*:57*/
#line 2028 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*59:*/
#line 2102 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{if(fsearch("nmb_files"))
nmb_files= atoi(Buffer);
if(fsearch("unicode"))
if(Buffer[0]=='y'||Buffer[0]=='Y')
unicode= TRUE;
if(fsearch("min_char"))
{user_min_char= strtoul(Buffer,(char**)NULL,0);
have_min_char= TRUE;
}

if(!mf_like)
{if(fsearch("pk_files"))
if(Buffer[0]=='n'||Buffer[0]=='N')
pk_files= FALSE;
if(fsearch("tfm_files"))
if(Buffer[0]=='n'||Buffer[0]=='N')
tfm_files= FALSE;
if(fsearch("ofm_file"))
if(Buffer[0]=='y'||Buffer[0]=='Y')
ofm_file= TRUE;
if(fsearch("long_extension"))
if(Buffer[0]=='n'||Buffer[0]=='N')
long_extension= FALSE;
}

if(fsearch("slant"))
slant= atof(Buffer);
if(slant<0.0||slant> 1.0)
{fprintf(stderr,"Invalid slant\n");
exit(1);
}
if(fsearch("rotation"))
if(Buffer[0]=='y'||Buffer[0]=='Y')
rotation= TRUE;

if(fsearch("mag_x"))
mag_x= atof(Buffer);
if(fsearch("mag_y"))
mag_y= atof(Buffer);
if(!mag_x&&!mag_y)
{mag_x= 1.0;
mag_y= 1.0;
}
if(mag_x&&!mag_y)
mag_y= mag_x;
if(mag_y&&!mag_x)
mag_x= mag_y;
if(mag_x<=0.0)
{fprintf(stderr,"Invalid horizontal magnification\n");
exit(1);
}
if(mag_y<=0.0)
{fprintf(stderr,"Invalid vertical magnification\n");
exit(1);
}

if(fsearch("dpi_x"))
dpi_x= atoi(Buffer);
if(fsearch("dpi_y"))
dpi_y= atoi(Buffer);
if(!dpi_x&&!dpi_y)
{dpi_x= 300;
dpi_y= 300;
}
if(dpi_x&&!dpi_y)
dpi_y= dpi_x;
if(dpi_y&&!dpi_x)
dpi_x= dpi_y;
if(dpi_x<=PRINTER_MIN_RES_X)
{fprintf(stderr,"Invalid horizontal printer resolution\n");
exit(1);
}
if(dpi_y<=PRINTER_MIN_RES_Y)
{fprintf(stderr,"Invalid vertical printer resolution\n");
exit(1);
}

if(fsearch("design_size"))
design_size= atof(Buffer);

if(fsearch("x_offset"))
offset_x= atoi(Buffer);
else
offset_x= rotation?0:(boxp->hbf_xDisplacement*mag_x+0.5);
if(fsearch("y_offset"))
offset_y= atoi(Buffer);
else
offset_y= rotation?0:(boxp->hbf_yDisplacement*mag_y+0.5);
if(!fsearch("comment"))
comment[0]= '\0';
else
strcpy(comment,Buffer);

if(fsearch("threshold"))
threshold= atoi(Buffer);
if(threshold<=0||threshold>=255)
{fprintf(stderr,"Invalid threshold\n");
exit(1);
}

if(!fsearch("checksum"))
checksum= 0;
else
checksum= strtoul(Buffer,(char**)NULL,0);

if(!fsearch("coding"))
coding[0]= '\0';
else
strcpy(coding,Buffer);

if(!fsearch("pk_directory"))
pk_directory[0]= '\0';
else
strcpy(pk_directory,Buffer);

if(!fsearch("tfm_directory"))
tfm_directory[0]= '\0';
else
strcpy(tfm_directory,Buffer);

if(fsearch("rm_command"))
strcpy(rm_command,Buffer);
else
strcpy(rm_command,"rm");

if(fsearch("cp_command"))
strcpy(cp_command,Buffer);
else
strcpy(cp_command,"cp");

if(!fsearch("job_extension"))
job_extension[0]= '\0';
else
{strncpy(job_extension,Buffer,EXTENSION_LENGTH);
job_extension[EXTENSION_LENGTH]= '\0';
}
}


/*:59*/
#line 2029 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


/*61:*/
#line 2256 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{const void*cp;
HBF_CHAR dummy;

cp= hbfGetCodeRange(hbf,NULL,&min_char,&max_char);
for(;cp!=NULL;cp= hbfGetCodeRange(hbf,cp,&dummy,&max_char))
;

if(have_min_char)
min_char= user_min_char;
}


/*:61*/
#line 2031 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

/*63:*/
#line 2285 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{const void*b2r;
unsigned char dummy;
int i;

for(i= 0;i<256;i++)
b2_codes[i]= 0;

b2r= hbfGetByte2Range(hbf,NULL,&min_2_byte,&max_2_byte);
dummy= min_2_byte;
for(;b2r!=NULL;b2r= hbfGetByte2Range(hbf,b2r,&dummy,&max_2_byte))
{for(i= dummy;i<=max_2_byte;i++)
b2_codes[i]= VALID_SUBCODE;
}

for(i= 0;i<256;i++)
if(b2_codes[i]==VALID_SUBCODE)
nmb_2_bytes++;
}


/*:63*/
#line 2032 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"


fclose(config);
}


/*:55*//*65:*/
#line 2317 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static int fsearch(const char*search_string)
{char*P,p;
const char*Q;
char temp_buffer[STRING_LENGTH+1];
char env_name[STRING_LENGTH+1];
char*env_p;
char*env_value;
char*Buf_p;
int Ch,ch,old_ch= '\n';
int count= STRING_LENGTH;

rewind(config);

do
{Q= search_string;
p= tolower((unsigned char)*Q);
Ch= fgetc(config);
ch= tolower(Ch);
while(!(ch==p&&old_ch=='\n')&&Ch!=EOF)


{old_ch= ch;
Ch= fgetc(config);
ch= tolower(Ch);
}

for(;;)
{if(*(++Q)=='\0')
if((Ch= fgetc(config))==' '||Ch=='\t')

goto success;
Ch= fgetc(config);
if(tolower(Ch)!=tolower((unsigned char)*Q))
break;
}
}
while(Ch!=EOF);

return 0;

success:
P= temp_buffer;

while((Ch= fgetc(config))==' '||Ch=='\t')

;
while(Ch!='\n'&&--count> 0&&Ch!=EOF)
{*P++= Ch;
Ch= fgetc(config);
}
*P= '\0';

if(*temp_buffer)
/*66:*/
#line 2396 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

{P= temp_buffer;
Buf_p= Buffer;
count= STRING_LENGTH-1;

while(*P&&count> 0)
{env_p= env_name;

if(*P=='$')
{P++;
if(*P=='$')
{*(Buf_p++)= *(P++);
count--;
continue;
}
while(*P=='{')
P++;
if(!(isalpha((unsigned char)*P)||*P=='_'))
{fprintf(stderr,
"Invalid environment variable name in configuration file\n");
exit(1);
}
*(env_p++)= *(P++);
while(*P)
{if(isalnum((unsigned char)*P)||*P=='_')
*(env_p++)= *(P++);
else
{while(*P=='}')
P++;
*env_p= '\0';
break;
}
}

env_value= getenv(env_name);
if(env_value)
{while(*env_value&&count> 0)
{*(Buf_p++)= *(env_value++);
count--;
}
}
}
else
{*(Buf_p++)= *(P++);
count--;
}
}
*Buf_p= '\0';
}


/*:66*/
#line 2371 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

else
*Buffer= '\0';

return(*Buffer)?1:0;
}


/*:65*//*68:*/
#line 2456 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static void config_error(const char*message)
{fprintf(stderr,"Couldn't find `%s' entry in configuration file\n",
message);
exit(1);
}



/*:68*//*72:*/
#line 2499 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

static const char*TeX_search_version(void)
{
#if defined(HAVE_LIBKPATHSEA)
return kpathsea_version_string;
#elif defined(HAVE_EMTEXDIR)
return emtex_version_string;
#elif defined(HAVE_MIKTEX)
char buf[200];

strcpy(buf,"MiKTeX ");
miktex_get_miktex_version_string_ex(buf+7,sizeof(buf)-7);

return buf;
#else
return no_version_string;
#endif
}


/*:72*//*75:*/
#line 2541 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#ifdef HAVE_EMTEXDIR
static int dir_setup(ed,env,dir,flags)
struct emtex_dir*ed;
const char*env;
const char*dir;
unsigned flags;

{const char*val;
char path[260];

ed->alloc= 0;
ed->used= 0;
ed->list= NULL;

if(env!=NULL&&(val= getenv(env))!=NULL)
return setup_list(ed,path,val,flags);
else
fprintf(stderr,
"Environment variable `%s' not set; use current directory\n",
env);

return TRUE;
}
#endif


/*:75*//*77:*/
#line 2576 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#ifdef HAVE_EMTEXDIR
static char*file_find(name,list)
char*name;
struct emtex_dir*list;

{if(emtex_dir_find(name_buffer,sizeof(name_buffer),list,name,EDF_CWD))
return name_buffer;

return NULL;
}
#endif


/*:77*//*80:*/
#line 2627 "/usr/local/home/wl/git/cjk/cjk-4.8.4/utils/hbf2gf/hbf2gf.w"

#if defined(HAVE_LIBKPATHSEA)
static char*TeX_search_cfg_file(char*name)
{
return kpse_find_file(name,kpse_program_text_format,TRUE);
}


static char*TeX_search_hbf_file(char*name)
{
return kpse_find_file(name,kpse_miscfonts_format,TRUE);
}


#elif defined(HAVE_EMTEXDIR)
static char*TeX_search_cfg_file(char*name)
{return file_find(name,&cfg_path);
}


static char*TeX_search_hbf_file(char*name)
{return file_find(name,&hbf_path);
}


#elif defined(HAVE_MIKTEX)
static char*TeX_search_cfg_file(char*name)
{char result[_MAX_PATH];

if(!miktex_find_input_file("hbf2gf",*name,result))
return 0;
return strdup(result);
}


static char*TeX_search_hbf_file(char*name)
{char result[_MAX_PATH];


if(!miktex_find_miscfont_file(*name,result))
return 0;
return strdup(result);
}


#else
static char*TeX_search_cfg_file(char*name)
{return name;
}


static char*TeX_search_hbf_file(char*name)
{return name;
}
#endif



/*:80*/
