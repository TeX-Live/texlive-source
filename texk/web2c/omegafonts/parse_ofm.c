/* parse_ofm.h: Checking an existent OFM file 

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
Copyright (C) 2005, 2006 Roozbeh Pournader

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
#include "header_routines.h"
#include "manifests.h"
#include "char_routines.h"
#include "ligkern_routines.h"
#include "out_ofm.h"
#include "extra_routines.h"
#include "param_routines.h"
#include "dimen_routines.h"
#include "error_routines.h"
#include "print_routines.h"
#include "out_routines.h"
#include "font_routines.h"
#include "parse_ofm.h"
#include "omfonts.h"
#include "manifests.h"

unsigned top_char, top_width, top_height, top_depth, top_italic;
unsigned start_ptr, check_sum_pos, design_size_pos, scheme_pos;
unsigned seven_bit_flag_pos;
unsigned random_word_pos, header_length;

unsigned ivalues_start,fvalues_start,mvalues_start,
    rules_start,glues_start,penalties_start;
unsigned ivalues_base,fvalues_base,mvalues_base,
    rules_base,glues_base,penalties_base;
unsigned char_base,width_base,height_base,depth_base,italic_base;
unsigned lig_kern_base,kern_base,exten_base,param_base;
  /* base addresses for the subfiles */
unsigned char_start;
unsigned bytes_per_entry;

unsigned lf;
boolean ofm_on;
unsigned start_pos;
unsigned family_pos;
unsigned ofm_ptr;
unsigned ncw;
unsigned ncl;
unsigned nce;
unsigned nco;
unsigned npc;

static void
eval_two_bytes(unsigned *pos) {
  if (ofm[ofm_ptr]>127)
      fatal_error_0("One of the subfile sizes (2) is negative");
  *pos = ofm[ofm_ptr]*0x100 + ofm[ofm_ptr+1];
  ofm_ptr += 2;
}

static void
eval_four_bytes(unsigned *pos) {
  if (ofm[ofm_ptr]>127)
      fatal_error_0("One of the subfile sizes (4) is negative");
  *pos = ofm[ofm_ptr]*0x1000000 + ofm[ofm_ptr+1]*0x10000 +
         ofm[ofm_ptr+2]*0x100   + ofm[ofm_ptr+3];
  ofm_ptr += 4;
}

static void ofm_organize(void);
static void ofm_read_simple(void);
static void ofm_read_rest(void);

boolean changed = FALSE;

void
parse_ofm(boolean read_ovf)
{
    ofm_organize();
    if (read_ovf == TRUE) input_ovf_file();
    ofm_read_simple();
    if (read_ovf == TRUE) input_ovf_fonts();
    ofm_read_rest();
    if (read_ovf == TRUE) input_ovf_chars();
    print_characters(read_ovf);
    if (changed) {
        left();
        out("COMMENT THE OFM FILE WAS BAD, SO THE DATA HAS BEEN CHANGED!");
        right();
    }
}

/* parse_ofm ensures that all of the header entries are consistent.
   The file is assumed to have been read in already, with the
   contents in the ofm array (length_ofm bytes).
*/

void
ofm_organize(void)
{
    unsigned copies,i;

    ofm_on = false; ofm_level = OFM_NOLEVEL; lf = 0; lh = 0;
    nco = 0; ncw = 0; npc = 0;
    bc = 0; ec = 0; nw = 0; nh = 0; nd = 0; ni = 0;
    nl = 0; nk = 0; ne = 0; np = 0;
    nki = 0; nwi = 0; nkf = 0; nwf = 0;
    nkm = 0; nwm = 0; 
    nkr = 0; nwr = 0; nkg = 0; nwg = 0;
    nkp = 0; nwp = 0; font_dir = 0;

    if (length_ofm < 8)
        fatal_error_0("File too short");
    lf = ofm[0]*256 + ofm[1];
    if (lf==0) { /* This is not a TFM file, it is an OFM-file */
        ofm_on = TRUE;

        /* next line edited by Thomas Esser, based on a patch sent
         * by Hossein Movahhedian. The old code had " + 1"
         * instead of "+ OFM_LEVEL0" and caused lots of segfaults */
        ofm_level = ofm[2]*0x100 + ofm[3] + OFM_LEVEL0;
        if (ofm[4]>127)
            fatal_error_0("File length is negative");
        lf = ofm[4]*0x1000000 + ofm[5]*0x10000 + ofm[6]*0x100 + ofm[7];
    } else {
        ofm_on = FALSE;
        ofm_level  = OFM_TFM;
    }
    if ((lf*4)!=length_ofm)
       fatal_error_2("Stated (%d) and actual (%d) file length do not match",
                      lf*4, length_ofm);
    
    switch(ofm_level) {
        case OFM_TFM: { start_pos = 2; check_sum_pos = 24;  break; }
        case OFM_LEVEL0: { start_pos = 8; check_sum_pos = 56;  break; }
        case OFM_LEVEL1: { start_pos = 8; check_sum_pos = 116; break; }
        default: { fatal_error_1("OFMLEVEL %d not supported", ofm_level-OFM_LEVEL0);
                   break; }
    }
    design_size_pos = check_sum_pos+4;
    scheme_pos = design_size_pos+4;
    family_pos = scheme_pos+40;
    random_word_pos = family_pos+20;

    ofm_ptr = start_pos;

    if (ofm_on==FALSE) {
        eval_two_bytes(&lh);
        eval_two_bytes(&bc);
        eval_two_bytes(&ec);
        eval_two_bytes(&nw);
        eval_two_bytes(&nh);
        eval_two_bytes(&nd);
        eval_two_bytes(&ni);
        eval_two_bytes(&nl);
        eval_two_bytes(&nk);
        eval_two_bytes(&ne);
        eval_two_bytes(&np);
        ncw = (ec-bc+1);
        ncl = nl;
        nce = ne;
        header_length = 6;
        top_char = 255;
        top_width = 255;
        top_height = 15;
        top_depth = 15;
        top_italic = 63;
    } else {
        eval_four_bytes(&lh);
        eval_four_bytes(&bc);
        eval_four_bytes(&ec);
        eval_four_bytes(&nw);
        eval_four_bytes(&nh);
        eval_four_bytes(&nd);
        eval_four_bytes(&ni);
        eval_four_bytes(&nl);
        eval_four_bytes(&nk);
        eval_four_bytes(&ne);
        eval_four_bytes(&np);
        eval_four_bytes(&font_dir);
        top_char = 0xffffff;
        top_width = 65535;
        top_height = 255;
        top_depth = 255;
        top_italic = 255;
        ncl = nl*2;
        nce = ne*2;
        if (ofm_level==OFM_LEVEL0) {
            header_length = 14;
            ncw = 2*(ec-bc+1);
        } else {
            header_length = 29;
            eval_four_bytes(&nco);
            eval_four_bytes(&ncw);
            eval_four_bytes(&npc);
            eval_four_bytes(&nki); /* Kinds of font ivalues   */
            eval_four_bytes(&nwi); /* Words of font ivalues   */
            eval_four_bytes(&nkf); /* Kinds of font fvalues   */
            eval_four_bytes(&nwf); /* Words of font fvalues   */
            eval_four_bytes(&nkm); /* Kinds of font mvalues   */
            eval_four_bytes(&nwm); /* Words of font mvalues   */
            eval_four_bytes(&nkr); /* Kinds of font rules     */
            eval_four_bytes(&nwr); /* Words of font rules     */
            eval_four_bytes(&nkg); /* Kinds of font glues     */
            eval_four_bytes(&nwg); /* Words of font glues     */
            eval_four_bytes(&nkp); /* Kinds of font penalties */
            eval_four_bytes(&nwp); /* Words of font penalties */
        }
    }
    if (lf != (header_length+lh+ncw+nw+nh+nd+ni+ncl+nk+nce+np+
               nki+nwi+nkf+nwf+nkm+nwm+nkr+nwr+nkg+nwg+nkp+nwp))
        fatal_error_0("Subfile sizes do not add up to the stated total");
    if (lh < 2)
        fatal_error_1("The header length is only %d", lh);
    if ((bc > (ec+1)) || (ec > top_char))
        fatal_error_2("The character code range %d .. %d is illegal", bc, ec);
    if ((nw==0) || (nh==0) || (nd==0) || (ni==0))
        fatal_error_0("Incomplete subfiles for character dimensions");
    ivalues_start   = header_length+lh;
    fvalues_start   = ivalues_start+nki;
    mvalues_start   = fvalues_start+nkf;
    rules_start     = mvalues_start+nkm;
    glues_start     = rules_start+nkr;
    penalties_start = glues_start+nkg;
    ivalues_base    = penalties_start+nkp;
    fvalues_base    = ivalues_base+nwi;
    mvalues_base    = fvalues_base+nwf;
    rules_base      = mvalues_base+nwm;
    glues_base      = rules_base+nwr;
    penalties_base  = glues_base+nwg;
    char_base       = penalties_base+nwp;
    bytes_per_entry = (12 + 2*npc) / 4 * 4;
    init_planes();
    no_labels = 0;
    switch (ofm_level) {
    case OFM_TFM: {
      for(i=bc; i<=ec; i++) {
        init_character(i,NULL);
        char_start = 4*char_base+4*(i-bc);
        current_character->index_indices[C_WD] =
           ofm[char_start] & 0xff;
        current_character->index_indices[C_HT] =
           (ofm[char_start+1] & 0xf0) >> 4;
        current_character->index_indices[C_DP] =
	   ofm[char_start+1] & 0xf;
        current_character->index_indices[C_IC] =
           (ofm[char_start+2] & 0xfc) >> 2;
        current_character->tag = 
           ofm[char_start+2] & 0x3;
        if (current_character->tag == TAG_LIG) no_labels++;
        current_character->remainder = 
           ofm[char_start+3] & 0xff;
      }
      break;
    }
    case OFM_LEVEL0: {
      for(i=bc; i<=ec; i++) {
        init_character(i,NULL);
        char_start = 4*char_base+8*(i-bc);
        current_character->index_indices[C_WD] =
           ((ofm[char_start] & 0xff) << 8) |
           (ofm[char_start+1] & 0xff);
        current_character->index_indices[C_HT] =
           ofm[char_start+2] & 0xff;
        current_character->index_indices[C_DP] =
	   ofm[char_start+3] & 0xff;
        current_character->index_indices[C_IC] =
           ofm[char_start+4] & 0xff;
        current_character->tag = 
           ofm[char_start+5] & 0x3;
        if (current_character->tag == TAG_LIG) no_labels++;
        current_character->remainder = 
           ((ofm[char_start+6] & 0xff) << 8) |
           (ofm[char_start+7] & 0xff);
      }
      break;
    }
    case OFM_LEVEL1: {
      char_start = 4*char_base;
      i = bc;
      while (i<=ec) {
        init_character(i,NULL);
        current_character->index_indices[C_WD] =
           ((ofm[char_start] & 0xff) << 8) |
           (ofm[char_start+1] & 0xff);
        current_character->index_indices[C_HT] =
           ofm[char_start+2] & 0xff;
        current_character->index_indices[C_DP] =
	   ofm[char_start+3] & 0xff;
        current_character->index_indices[C_IC] =
           ofm[char_start+4] & 0xff;
        current_character->tag = 
           ofm[char_start+5] & 0x3;
        if (current_character->tag == TAG_LIG) no_labels++;
        current_character->remainder = 
           ((ofm[char_start+6] & 0xff) << 8) |
           (ofm[char_start+7] & 0xff);
        copies = 256*ofm[char_start+8]+ofm[char_start+9];
        /* Handle character params */
        copy_characters(i, copies);
        i += copies + 1;
        char_start += bytes_per_entry;
      }
      if (char_start != (4*(char_base+ncw)))
          fatal_error_0("Length of char info table does not "
                        "correspond to specification");
      break;
    }
    default: {
      fatal_error_1("Inappropriate font level (%d)", ofm_level-OFM_LEVEL0);
    }
    }
    width_base    = char_base+ncw;
    height_base   = width_base+nw;
    depth_base    = height_base+nh;
    italic_base   = depth_base+nd;
    lig_kern_base = italic_base+ni;
    kern_base     = lig_kern_base+ncl;
    exten_base    = kern_base+nk;
    param_base    = exten_base+nce-1;
}

void
ofm_read_simple(void)
{

    if (ofm_on==TRUE) {
        print_ofm_level(ofm_level-OFM_LEVEL0);
        print_font_dir(font_dir);
    }
    header = (char *) ofm+check_sum_pos;
    retrieve_header();
    print_family();
    print_face();
    print_coding_scheme();
    print_design_size();
    out("(COMMENT DESIGNSIZE IS IN POINTS)"); out_ln();
    out("(COMMENT OTHER SIZES ARE MULTIPLES OF DESIGNSIZE)"); out_ln();
    print_check_sum();
    if ((seven_bit == TRUE) || (ofm_level != OFM_TFM))
      print_seven_bit_safe_flag();
    retrieve_parameters(ofm+(4*param_base));
    print_parameters();
}

void
ofm_read_rest(void)
{
    retrieve_dimen_tables();
    if (verbose_option==TRUE) print_dimen_tables();
    retrieve_ligkern_table(ofm+(4*lig_kern_base), ofm+(4*kern_base));
    adjust_labels(FALSE);
    if (verbose_option==TRUE) print_labels();
    print_ligkern_table();
    retrieve_exten_table(ofm+(4*exten_base));
    if (verbose_option==TRUE) print_extens();
}
