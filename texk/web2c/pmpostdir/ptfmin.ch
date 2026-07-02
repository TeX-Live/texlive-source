@x
#include <w2c/config.h>
@y
#include <kpathsea/kpathsea.h>
#include <w2c/config.h>
@z

%
% local variable "nt" && "ct"
@x
  halfword lf,tfm_lh,bc,ec,nw,nh,nd; /* subfile size parameters */
@y
  halfword lf,tfm_lh,bc,ec,nw,nh,nd,nt; /* subfile size parameters */
@z

@x
  int h_and_d; /* height and depth indices being unpacked */
@y
  int h_and_d; /* height and depth indices being unpacked */
  halfword ct; /* char and type */
  halfword id; /* JFM font id */
@z

@x
@<Read the character data and the width, height, and depth tables and
  |goto done|@>
@y
@<Read the character type table@>;
@<Read the character data and the width, height, and depth tables and
  |goto done|@>
@z

@x
@d read_two(A) { (A)=tfbyte;
@y
@d read_pair(A) { (A)=tfbyte;
  tfget; (A)=(A)*0400+tfbyte;
}
@d read_triplekanji(A) { (A)=tfbyte;
  tfget; (A)=(A)*0400+tfbyte;
  tfget; (A)=(A)+tfbyte*0200000;
}
@d read_two(A) { (A)=tfbyte;
@z

%
% routine to process JFM file format
@x
@<Read the \.{TFM} size fields@>=
tfget; read_two(lf);
@y
@d yoko_jfm_id   11 /* `yoko-kumi' fonts */
@d tate_jfm_id   9  /* `tate-kumi' fonts */
@d font_jfm_p(A) (mp->font_id[(A)]!=0)
@d incr(A)   (A)=(A)+1 /* increase a variable by unity */
@<Read the \.{TFM} size fields@>=
tfget; read_two(lf);
if ( (lf==yoko_jfm_id) || (lf==tate_jfm_id) ) {
  id=lf;
  tfget; read_two(nt);
  tfget; read_two(lf);
} else {
  id=0; nt=0;
};
@z

@x
whd_size=(size_t)((ec+1-bc)+nw+nh+nd);
@y
whd_size=(size_t)((ec+1-bc)+nt+nw+nh+nd);
@z

%
% reserve space for character type table
@x
mp->char_base[n]=(int)(mp->next_fmem-(size_t)bc);
mp->width_base[n]=(int)(mp->next_fmem+(size_t)(ec-bc)+1);
@y
mp->font_id[n]=id;
mp->font_nt[n]=nt;
mp->ctype_base[n]=mp->next_fmem;
mp->char_base[n]=(int)(mp->next_fmem+nt-(size_t)bc);
mp->width_base[n]=(int)(mp->next_fmem+nt+(size_t)(ec-bc)+1);
@z

%
% read character type table
%
@x
tf_ignore(4*(tfm_lh-2))
@y
tf_ignore(4*(tfm_lh-2))

@ @<Read the character type table@>=
ii=mp->ctype_base[n]+nt;
i=mp->ctype_base[n];
while ( i<ii ) {
  tfget; read_triplekanji(ct);  /* allow character codes up to 0xffffff */
  mp->font_info[i].hh.LH=ct;
  tfget; mp->font_info[i].hh.RH=tfbyte;
  incr(i);
}
@z

@x
mp_pack_file_name(mp, mp->cur_name,mp->cur_area,mp->cur_ext);
mp->tfm_infile = (mp->open_file)(mp, mp->name_of_file, "r",mp_filetype_metrics);
@y
mp_pack_file_name(mp, mp->cur_name,mp->cur_area,mp->cur_ext);
{
  char *fulln;
  fulln = kpse_find_file(fname, kpse_tfm_format, 1);
  mp->tfm_infile = (mp->open_file)(mp, fulln, "r",mp_filetype_metrics);
  if(fulln) mp_xfree(fulln);
}
@z
