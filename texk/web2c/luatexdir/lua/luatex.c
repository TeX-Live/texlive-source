/* $Id: luatex.c 1013 2008-02-14 00:09:02Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>
#include <zlib.h>

/* do this aleph stuff here, for now */

void
b_test_in(void)
{
  string fname = kpse_find_file ((char *)(nameoffile + 1), 
				   kpse_program_binary_format, true);

    if (fname) {
      libcfree(nameoffile);
      nameoffile = xmalloc(2+strlen(fname));
      namelength = strlen(fname);
      strcpy((char *)nameoffile+1, fname);
    }
    else {
      libcfree(nameoffile);
      nameoffile = xmalloc(2);
      namelength = 0;
      nameoffile[0] = 0;
      nameoffile[1] = 0;
    }
}
 

int **ocp_tables;
static int ocp_entries = 0;

void
allocate_ocp_table P2C(int, ocp_number, int, ocp_size)
{
    int i;
    if (ocp_entries==0) {
      ocp_tables = (int **) xmalloc(256*sizeof(int**));
      ocp_entries=256;
    } else if ((ocp_number==256)&&(ocp_entries==256)) {
      ocp_tables = xrealloc(ocp_tables, 65536);
      ocp_entries=65536;
    }
    ocp_tables[ocp_number] =
       (int *) xmalloc((1+ocp_size)*sizeof(int));
    ocp_tables[ocp_number][0] = ocp_size;
    for (i=1; i<=ocp_size; i++) {
        ocp_tables[ocp_number][i]  = 0;
    }
}

void
dump_ocp_table P1C(int, ocp_number)
{
    dump_things(ocp_tables[ocp_number][0], ocp_tables[ocp_number][0]+1);
}

void
undump_ocp_table P1C(int, ocp_number)
{
    int sizeword;
    if (ocp_entries==0) {
      ocp_tables = (int **) xmalloc(256*sizeof(int**));
      ocp_entries=256;
    } else if ((ocp_number==256)&&(ocp_entries==256)) {
      ocp_tables = xrealloc(ocp_tables, 65536);
      ocp_entries=65536;
    }
    undump_things(sizeword,1);
    ocp_tables[ocp_number] =
        (int *) xmalloc((1+sizeword)*sizeof(int));
    ocp_tables[ocp_number][0] = sizeword;
    undump_things(ocp_tables[ocp_number][1], sizeword);
}


void
run_external_ocp P1C(string, external_ocp_name)
{
  char *in_file_name;
  char *out_file_name;
  FILE *in_file;
  FILE *out_file;
  char command_line[400];
  int i;
  unsigned c;
  int c_in;
#ifdef WIN32
  char *tempenv;

#define null_string(s) ((s == NULL) || (*s == '\0'))

  tempenv = getenv("TMPDIR");
  if (null_string(tempenv))
    tempenv = getenv("TEMP");
  if (null_string(tempenv))
    tempenv = getenv("TMP");
  if (null_string(tempenv))
    tempenv = "c:/tmp";	/* "/tmp" is not good if we are on a CD-ROM */
  in_file_name = concat(tempenv, "/__aleph__in__XXXXXX");
  mktemp(in_file_name);
#else
  in_file_name = strdup("/tmp/__aleph__in__XXXXXX");
  mkstemp(in_file_name);
#endif /* WIN32 */

  in_file = fopen(in_file_name, FOPEN_WBIN_MODE);
  
  for (i=1; i<=otp_input_end; i++) {
      c = otp_input_buf[i];
      if (c>0xffff) {
          fprintf(stderr, "Aleph does not currently support 31-bit chars\n");
          exit(1);
      }
      if (c>0x4000000) {
          fputc(0xfc | ((c>>30) & 0x1), in_file);
          fputc(0x80 | ((c>>24) & 0x3f), in_file);
          fputc(0x80 | ((c>>18) & 0x3f), in_file);
          fputc(0x80 | ((c>>12) & 0x3f), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c>0x200000) {
          fputc(0xf8 | ((c>>24) & 0x3), in_file);
          fputc(0x80 | ((c>>18) & 0x3f), in_file);
          fputc(0x80 | ((c>>12) & 0x3f), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c>0x10000) {
          fputc(0xf0 | ((c>>18) & 0x7), in_file);
          fputc(0x80 | ((c>>12) & 0x3f), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c>0x800) {
          fputc(0xe0 | ((c>>12) & 0xf), in_file);
          fputc(0x80 | ((c>>6) & 0x3f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else if (c>0x80) {
          fputc(0xc0 | ((c>>6) & 0x1f), in_file);
          fputc(0x80 | (c & 0x3f), in_file);
      } else {
          fputc(c & 0x7f, in_file);
      }
  }
  fclose(in_file);
  
#define advance_cin if ((c_in = fgetc(out_file)) == -1) { \
                         fprintf(stderr, "File contains bad char\n"); \
                         goto end_of_while; \
                    }
                     
#ifdef WIN32
  out_file_name = concat(tempenv, "/__aleph__out__XXXXXX");
  mktemp(out_file_name);
#else
  out_file_name = strdup("/tmp/__aleph__out__XXXXXX");
  mkstemp(out_file_name);
#endif

  sprintf(command_line, "%s <%s >%s\n",
          external_ocp_name+1, in_file_name, out_file_name);
  system(command_line);
  out_file = fopen(out_file_name, FOPEN_RBIN_MODE);
  otp_output_end = 0;
  otp_output_buf[otp_output_end] = 0;
  while ((c_in = fgetc(out_file)) != -1) {
     if (c_in>=0xfc) {
         c = (c_in & 0x1)   << 30;
         {advance_cin}
         c |= (c_in & 0x3f) << 24;
         {advance_cin}
         c |= (c_in & 0x3f) << 18;
         {advance_cin}
         c |= (c_in & 0x3f) << 12;
         {advance_cin}
         c |= (c_in & 0x3f) << 6;
         {advance_cin}
         c |= c_in & 0x3f;
     } else if (c_in>=0xf8) {
         c = (c_in & 0x3) << 24;
         {advance_cin}
         c |= (c_in & 0x3f) << 18;
         {advance_cin}
         c |= (c_in & 0x3f) << 12;
         {advance_cin}
         c |= (c_in & 0x3f) << 6;
         {advance_cin}
         c |= c_in & 0x3f;
     } else if (c_in>=0xf0) {
         c = (c_in & 0x7) << 18;
         {advance_cin}
         c |= (c_in & 0x3f) << 12;
         {advance_cin}
         c |= (c_in & 0x3f) << 6;
         {advance_cin}
         c |= c_in & 0x3f;
     } else if (c_in>=0xe0) {
         c = (c_in & 0xf) << 12;
         {advance_cin}
         c |= (c_in & 0x3f) << 6;
         {advance_cin}
         c |= c_in & 0x3f;
     } else if (c_in>=0x80) {
         c = (c_in & 0x1f) << 6;
         {advance_cin}
         c |= c_in & 0x3f;
     } else {
         c = c_in & 0x7f;
     }
     otp_output_buf[++otp_output_end] = c;
  }

end_of_while:
  remove(in_file_name);
  remove(out_file_name);
}

/* Read and write dump files through zlib */

/* Earlier versions recast *f from FILE * to gzFile, but there is
 * no guarantee that these have the same size, so a static variable 
 * is needed.
 */

static gzFile gz_fmtfile = NULL;

void
do_zdump (char *p,  int item_size,  int nitems, FILE *out_file)
{
  int err;
  if (nitems==0)
	return;
  if (gzwrite (gz_fmtfile,(void *)p, item_size*nitems) != item_size*nitems)
    {
      fprintf (stderr, "! Could not write %d %d-byte item(s): %s.\n",
               nitems, item_size, gzerror(gz_fmtfile,&err));
      uexit (1);
    }
}

void
do_zundump (char *p,  int item_size,  int nitems, FILE *in_file)
{
  int err;
  if (nitems==0)
	return;
  if (gzread (gz_fmtfile,(void *)p, item_size*nitems) <= 0) 
	{
	  fprintf (stderr, "Could not undump %d %d-byte item(s): %s.\n",
			   nitems, item_size, gzerror(gz_fmtfile,&err));
	  uexit (1);
	}
}

#define COMPRESSION "R3"

boolean 
zopen_w_input (FILE **f, int format, const_string fopen_mode) {
  int callbackid;
  int res;
  char *fnam;
  callbackid = callback_defined(find_format_file_callback);
  if (callbackid>0) {
	res = run_callback(callbackid,"S->S",(nameoffile+1),&fnam);
	if (res && fnam && strlen(fnam)>0) {
	  xfree (nameoffile);
	  nameoffile = xmalloc (strlen(fnam)+2);
	  memcpy((nameoffile+1),fnam,strlen(fnam));
	  *(nameoffile+strlen(fnam)+1)=0;
	  *f = xfopen(fnam,fopen_mode);
	  if (*f == NULL) {
		return 0;
	  }
	} else {
	  return 0;
	}
  } else {
	res = open_input(f,format,fopen_mode);
  }
  if (res) {
	gz_fmtfile = gzdopen(fileno(*f),"rb" COMPRESSION);
  }
  return res;
}

boolean 
zopen_w_output (FILE **f, const_string fopen_mode) {
  int res = 1;
  if (luainit) {
    *f = fopen((nameoffile+1),fopen_mode);
    if (*f == NULL) {
      return 0;
    }
  } else {
    res =  open_output(f,fopen_mode);
  }
  if (res) {
	gz_fmtfile = gzdopen(fileno(*f),"wb" COMPRESSION);
  }
  return res;
}

void 
zwclose (FILE *f) { 
  gzclose(gz_fmtfile); 
}

/* create the dvi or pdf file */

int
open_outfile(FILE **f, char *name, char *mode) {
  FILE *res;
  res = fopen(name,mode);
  if (res != NULL) {
	*f = res;
	return 1;
  }
  return 0;
}


/* the caller sets tfm_buffer=NULL and tfm_size=0 */

int
readbinfile (FILE *f, unsigned char **tfm_buffer, integer *tfm_size) {
  void *buf;
  int size;
  if (fseek(f, 0, SEEK_END)==0) {
    size = ftell(f);
    if (size>0) {
      buf = xmalloc(size); 
      if(fseek(f, 0, SEEK_SET)==0) {
	if(fread((void *)buf,size,1,f)==1) {
	  *tfm_buffer=(unsigned char *)buf;
	  *tfm_size=(integer)size;
	  return 1;
	} 
      }
    }
  } /* seek failed, or zero-sized file */
  return 0;
}

