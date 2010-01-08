/* header_routines.c: Headers of font metric files.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#include "cpascal.h"
#include "list_routines.h"
#include "manifests.h"
#include "header_routines.h"
#include "print_routines.h"
#include "error_routines.h"
#include "out_ofm.h"
#include "omfonts.h"

#define LOC_CHECK_SUM     	0
#define LOC_DESIGN_SIZE   	4
#define LOC_CODING_SCHEME 	8
#define LOC_FAMILY        	(LOC_CODING_SCHEME+LEN_CODING_SCHEME+1)
#define LOC_SEVEN_FLAG    	(LOC_FAMILY+LEN_FAMILY+1)
#define LOC_FACE          	(LOC_SEVEN_FLAG+3)

#define HEADER_MIN          	 18
#define HEADER_MAX          	255


string header;
av_list header_list = NULL;
unsigned header_max = HEADER_MIN-1;

unsigned check_sum;
boolean check_sum_specified;

fix design_size;
boolean design_size_specified;

fix design_units;
boolean design_units_specified;

string coding_scheme;
boolean coding_scheme_specified;

string family;
boolean family_specified;

unsigned face;
boolean face_specified;

unsigned ofm_level;
boolean ofm_level_specified;

unsigned font_dir;
boolean font_dir_specified;

boolean seven_bit;
boolean seven_bit_specified;
boolean seven_bit_calculated;

unsigned font_type = FT_VANILLA;

unsigned lh;

void
store_header_int(unsigned loc, unsigned val)
{
    header[loc]   = 0xff & (val >> 24) ;
    header[loc+1] = 0xff & (val >> 16) ;
    header[loc+2] = 0xff & (val >>  8) ;
    header[loc+3] = 0xff & val ;
}

void
store_header_byte(unsigned loc, unsigned val)
{
    header[loc] = 0xff & val ;
}

void
retrieve_header_int(unsigned loc, unsigned *where)
{
    string ptr = header+loc;

    *where = ((* ptr    & 0xff) << 24) |
             ((*(ptr+1) & 0xff) << 16) |
             ((*(ptr+2) & 0xff) <<  8) |
              (*(ptr+3) & 0xff) ;

}

void
retrieve_header_byte(unsigned loc, unsigned char *where)
{
    *where = header[loc];
}

/* HEADER */

static void
init_header_word(void)
{
header_list = NULL;
header_max = HEADER_MIN-1;
font_type = FT_VANILLA;
}

void
set_header_word(unsigned index, unsigned val)
{
    av_list L1, L2;

    if (index > HEADER_MAX) {
	warning_0("HEADER index must be at most 255; ignored");
        return;
    }
    if (index < HEADER_MIN) {
	warning_0("HEADER index must be at least 18; ignored");
        return;
    }
    L1 = header_list;
    if (L1 == NULL) {
	header_list = av_list1(index, val);
	header_max = index;
    } else {
	L2 = L1->ptr;
	while ((L2 != NULL) && (lattr(L2) <= index)) {
	    L1 = L2;
	    L2 = L2->ptr;
	}
	if (index < lattr(L1)) {
	    header_list = av_list1(index, val);
	    header_list->ptr = L1;
        } else if (index == lattr(L1)) {
	    warning_1("HEADER index (%d) previously defined; "
                      "old value ignored", index);
	    lval(L1)  = val;
	} else {
	    L2 = av_list1(index, val);
	    L2->ptr = L1->ptr;
	    if (L1->ptr==NULL) header_max = index;
	    L1->ptr = L2;
	}
    }
}

static void
store_header_word(void)
{
    av_list L = header_list;
    unsigned ctr = HEADER_MIN;

    header = (string) xmalloc(4*(header_max+1));
    while (L != NULL) {
	while (ctr<lattr(L)) {
	    store_header_int(ctr*4,0);
	    ctr++;
	}
	store_header_int(ctr*4,lval(L));
	ctr++; L = L->ptr;
    }
}

static void
retrieve_header_word(void)
{
    unsigned j = HEADER_MIN, value, header_no=lh;

    header_list = NULL; 
    while (j <= header_no) {
	retrieve_header_int(j*4, &value);
	if (value != 0) {
	    set_header_word(j, value);
	}
        j++;
    }
}

void
print_header(void)
{
    av_list L = header_list;

    print_check_sum();
    print_design_size();
    print_coding_scheme();
    print_family();
    print_face();
    print_seven_bit_safe_flag();
    while (L!=NULL) {
	print_header_word(lattr(L),lval(L));
	L = L->ptr;
    }
}

/* CHECKSUM */

void
init_check_sum(void)
{
    check_sum = 0;
    check_sum_specified = FALSE;
}

void
set_check_sum(unsigned cs)
{
    check_sum = cs;
    check_sum_specified = TRUE;
}

void
store_check_sum(void)
{

    calculate_check_sum();
    store_header_int(LOC_CHECK_SUM, check_sum);
}

void
retrieve_check_sum(void)
{
    retrieve_header_int(LOC_CHECK_SUM, &check_sum);
}

void
calculate_check_sum(void)
{
/* 
    if (check_sum_specified == FALSE) {
	not_yet_done("checksum calculation");
    }
*/
}


/* DESIGNSIZE */

void
init_design_size(void)
{
    design_size = 10*UNITY;
    design_size_specified = FALSE;
}

void
set_design_size(fix ds)
{
    if (design_size_specified==TRUE)
	warning_0("DESIGNSIZE previously defined; old value ignored");
    if (ds<1) {
        warning_0("DESIGNSIZE value must be at least 1; set to 10");
        design_size = 10*UNITY;
    } else {
        design_size = ds;
    }
    design_size_specified = TRUE;
}

void
store_design_size(void)
{
    store_header_int(LOC_DESIGN_SIZE, design_size);
}

void
retrieve_design_size(void)
{
    retrieve_header_int(LOC_DESIGN_SIZE, (unsigned *) &design_size);
}


/* DESIGNUNITS */

void
init_design_units(void)
{
    design_units = UNITY;
    design_units_specified = FALSE;
}

void
set_design_units(fix du)
{
    if (design_units_specified==TRUE)
	warning_0("DESIGNUNITS previously defined; old value ignored");
    if (du<=0) {
        warning_0("DESIGNUNITS value must be positive; set to 1");
        design_units = UNITY;
    } else {
        design_units = du;
    }
    design_units_specified = TRUE;
}


/* CODINGSCHEME */

void
init_coding_scheme(void)
{
    coding_scheme = xstrdup("UNSPECIFIED");
    coding_scheme_specified = FALSE;
}

void
set_coding_scheme(string sval)
{
    if (coding_scheme_specified==TRUE) {
	warning_0("CODINGSCHEME previously defined; old value ignored");
    }
    free(coding_scheme);
    coding_scheme = sval;
    
    if (!strncmp(coding_scheme, "TEX MATH SY", 11) ||
        !strncmp(coding_scheme, "TeX math sy", 11))
        font_type = FT_MATHSY;
    else if (!strncmp(coding_scheme, "TEX MATH EX", 11) ||
        !strncmp(coding_scheme, "TeX math ex", 11))
        font_type = FT_MATHEX;
    else font_type = FT_VANILLA;

    coding_scheme_specified = TRUE;
}

void
store_coding_scheme(void)
{
    register unsigned i=0, j=LOC_CODING_SCHEME, len=strlen(coding_scheme);

    header[j] = len;
    for (j++; i<len; i++,j++) header[j] = coding_scheme[i];
    for (; j<(LOC_CODING_SCHEME+LEN_CODING_SCHEME); j++)
        header[j] = '\0';;
}


void
retrieve_coding_scheme(void)
{
    register unsigned i=0, j=LOC_CODING_SCHEME, len=header[LOC_CODING_SCHEME];

    coding_scheme = (string) xmalloc(len+1);
    for (j++; i<len; i++,j++) {
       coding_scheme[i] = header[j];
       if ((text_format == TEXT_CODE_UPPER) &&
           (coding_scheme[i] >= 'a') &&
           (coding_scheme[i] <= 'z')) {
          coding_scheme[i] += 'A' - 'a';
       }
    }
    coding_scheme[i] = '\0';
    if (!strncmp(coding_scheme, "TEX MATH SY", 11) ||
        !strncmp(coding_scheme, "TeX math sy", 11))
        font_type = FT_MATHSY;
    else if (!strncmp(coding_scheme, "TEX MATH EX", 11) ||
        !strncmp(coding_scheme, "TeX math ex", 11))
        font_type = FT_MATHEX;
    else font_type = FT_VANILLA;
}

/* FAMILY */

void
init_family(void)
{
    family = xstrdup("UNSPECIFIED");
    family_specified = FALSE;
}

void
set_family(string sval)
{
    if (family_specified==TRUE) {
	warning_0("FAMILY previously defined; old value ignored");
    }
    free(family);
    family = sval;
    family_specified = TRUE;
}

void
store_family(void)
{
    register unsigned i=0, j=LOC_FAMILY, len=strlen(family);

    if (len>LEN_FAMILY) internal_error_1("store_family (len=%d)", len);
    header[j] = len;
    for (j++; i<len; i++,j++) header[j] = family[i];
    for (; j<=(LOC_FAMILY+LEN_FAMILY); j++)
        header[j] = '\0';;
}


void
retrieve_family(void)
{
    register unsigned i=0, j=LOC_FAMILY, len=header[LOC_FAMILY];

    family = (string) xmalloc(len+1);
    for (j++; i<len; i++,j++) {
       family[i] = header[j];
       if ((text_format == TEXT_CODE_UPPER) &&
           (family[i] >= 'a') &&
           (family[i] <= 'z')) {
          family[i] += 'A' - 'a';
       }
    }
    family[i] = '\0';
}

/* FACE */

static void
init_face(void)
{
    face = 0;
    face_specified = FALSE;
}

void
set_face(unsigned f)
{
    if (face_specified==TRUE)
	warning_0("FACE previously defined; old value ignored");
    if (face>255) {
        warning_0("FACE value must be less than 256");
    } else {
        face = f;
    }
    face_specified = TRUE;
}

void
store_face(void)
{
    store_header_byte(LOC_FACE, face);
}

void
retrieve_face(void)
{
    unsigned char face_byte;
    retrieve_header_byte(LOC_FACE, &face_byte);
    face = face_byte;
}

/* OFMLEVEL */

static void
init_ofm_level(void)
{
    ofm_level = OFM_TFM;
    ofm_level_specified = FALSE;
}

void
set_ofm_level(unsigned level)
{
    if (ofm_level_specified==TRUE)
        warning_0("OFMLEVEL previously defined; old value ignored");
    /* Fix from Joel Riou <joel.riou@normalesup.org> */
    if ((level != 0) && (level != 1)) {
        warning_0("OFMLEVEL value must be D 0 or D 1");
    } else {
        ofm_level = level ? OFM_LEVEL1 : OFM_LEVEL0;
    }
    ofm_level_specified = TRUE;
}

/* FONTDIR */

#if 0
/* Not yet used */
static void
init_font_dir(void)
{
    font_dir = DIR_ORD+DIR_TL;
    font_dir_specified = FALSE;
}
#endif

void
set_font_dir(unsigned direction)
{
    if (font_dir_specified==TRUE)
        warning_0("FONTDIR previously defined; old value ignored");
    if ((direction < DIR_MIN) || (direction > DIR_MAX)) {
        warning_0("bad FONTDIR value; ignored");
    } else {
        font_dir = direction;
    }
    font_dir_specified = TRUE;
}

/* SEVENBITSAFEFLAG */

static void
init_seven_bit_safe_flag(void)
{
    seven_bit = 0;
    seven_bit_calculated = 1;
    seven_bit_specified = FALSE;
}

void
set_seven_bit_safe_flag(unsigned f)
{
    if (seven_bit_specified==TRUE)
        warning_0("SEVENBITSAFEFLAG previously defined; "
                  "old value ignored");
    if ((f!=FALSE) && (f!=TRUE)) {
        internal_error_1("set_seven_bit_safe_flag (f=%d)", f);
    }
    seven_bit = f;
    seven_bit_specified = TRUE;
}

void
store_seven_bit_safe_flag(void)
{
    store_header_byte(LOC_SEVEN_FLAG, seven_bit ? 0x80 : 0);
}

void
retrieve_seven_bit_safe_flag(void)
{
    unsigned char seven_bit_byte;
    retrieve_header_byte(LOC_SEVEN_FLAG, &seven_bit_byte);
    seven_bit = (seven_bit_byte != 0);
}

void
calculate_seven_bit_safe_flag(void)
{
    if ((seven_bit_specified == TRUE) && (seven_bit == 1) &&
        (seven_bit_calculated == 0)) {
        warning_0("SEVENBITSAFEFLAG value specified TRUE; "
                  "really FALSE");
    }
    seven_bit = seven_bit_calculated;
}

void
init_header(void)
{
    init_ofm_level();
    init_header_word();
    init_check_sum();
    init_design_size();
    init_design_units();
    init_coding_scheme();
    init_family();
    init_face();
    init_seven_bit_safe_flag();
}

void
store_header(void)
{

    store_header_word();
    store_check_sum();
    store_design_size();
    store_coding_scheme();
    store_family();
    store_face();
    store_seven_bit_safe_flag();

/*
    { int i;
    for (i=0; i<(4*(header_max+1)); i++) {
        if (!(i%4)) fprintf(stdout, "\n");
	fprintf(stdout, "%d: %c (%d, %2x) ",
                i, header[i], header[i], header[i]);
    }
    fprintf(stdout, "\n");
*/
    if (header_list!=NULL) {
        av_list L1 = header_list, L2;
        while (L1 != NULL) {
            L2 = L1->ptr;
            free(L1);
            L1 = L2;
        }
    }
    lh = header_max;
    header_list = NULL;
    header_max = HEADER_MIN-1;

    retrieve_header();
}

void
retrieve_header(void)
{
    retrieve_header_word();
    retrieve_check_sum();
    retrieve_design_size();
    retrieve_coding_scheme();
    retrieve_family();
    retrieve_face();
    retrieve_seven_bit_safe_flag();
}

void
output_ofm_header(void)
{
    unsigned i=0, j, k1, k2;

    av_list L = header_list;

    out_ofm_4(check_sum); i++;
    out_ofm_4(design_size); i++;
    k1 = strlen(coding_scheme);
    out_ofm(k1);
    for (j=0; j<k1; j++) out_ofm(coding_scheme[j]);
    for (j=k1; j<LEN_CODING_SCHEME; j++) out_ofm(0); 
    k2 = strlen(family);
    out_ofm(k2);
    for (j=0; j<k2; j++) out_ofm(family[j]);
    for (j=k2; j<LEN_FAMILY; j++) out_ofm(0); 
    if ((ofm_level==OFM_TFM) && (seven_bit_specified==TRUE))
      out_ofm(seven_bit ? 0x80 : 0);
    else
      out_ofm(0);
    out_ofm(0); out_ofm(0); out_ofm(face);
    i = 1 + LOC_SEVEN_FLAG / 4;
    lh = header_max + 1;
    while(L != NULL) {
       j=lattr(L);
       while (i<j) {
           out_ofm_4(0);
           i++;
       }
       out_ofm_4(lval(L));
       L = L->ptr; i++;
    }
    while (i<lh) {out_ofm_4(0); i++;}
}
