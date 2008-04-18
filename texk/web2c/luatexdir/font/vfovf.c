/*
Copyright (c) 1996-2006 Taco Hoekwater <taco@luatex.org>

This file is part of LuaTeX.

LuaTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

LuaTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LuaTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: vfovf.c 1169 2008-04-15 14:57:56Z oneiros $
*/

#include "ptexlib.h"

#include "luatex-api.h"

/* this is a hack! */
#define font_max 5000

#define set_char_0 0 /* typeset character 0 and move right */
#define set1 128 /* typeset a character and move right */
#define set2 129 /* typeset a character and move right */
#define set3 130 /* typeset a character and move right */
#define set4 131 /* typeset a character and move right */
#define set_rule 132 /* typeset a rule and move right */
#define put1  133 /* typeset a character without moving */
#define put2  134 /* typeset a character without moving */
#define put3  135 /* typeset a character without moving */
#define put4  136 /* typeset a character without moving */
#define put_rule 137 /* typeset a rule */
#define nop 138 /* no operation */
#define bop 139 /* beginning of page */
#define eop 140 /* ending of page */
#define push 141 /* save the current positions */
#define pop 142 /* restore previous positions */
#define right1  143 /* move right */
#define right2  144 /* move right */
#define right3  145 /* move right */
#define right4  146 /* move right, 4 bytes */
#define w0 147 /* move right by |w| */
#define w1 148 /* move right and set |w| */
#define w2 149 /* move right and set |w| */
#define w3 150 /* move right and set |w| */
#define w4 151 /* move right and set |w| */
#define x0 152 /* move right by |x| */
#define x1 153 /* move right and set |x| */
#define x2 154 /* move right and set |x| */
#define x3 155 /* move right and set |x| */
#define x4 156 /* move right and set |x| */
#define down1 157 /* move down */
#define down2 158 /* move down */
#define down3 159 /* move down */
#define down4 160 /* move down, 4 bytes */
#define y0 161 /* move down by |y| */
#define y1 162 /* move down and set |y| */
#define y2 163 /* move down and set |y| */
#define y3 164 /* move down and set |y| */
#define y4 165 /* move down and set |y| */
#define z0 166 /* move down by |z| */
#define z1 167 /* move down and set |z| */
#define z2 168 /* move down and set |z| */
#define z3 169 /* move down and set |z| */
#define z4 170 /* move down and set |z| */
#define fnt_num_0 171 /* set current font to 0 */
#define fnt1 235 /* set current font */
#define fnt2 236 /* set current font */
#define fnt3 237 /* set current font */
#define fnt4 238 /* set current font */
#define xxx1 239 /* extension to DVI  primitives */
#define xxx2 240 /* extension to DVI  primitives */
#define xxx3 241 /* extension to DVI  primitives */
#define xxx4 242 /* potentially long extension to DVI primitives */
#define fnt_def1 243 /* define the meaning of a font number */
#define pre 247 /* preamble */
#define post 248 /* postamble beginning */
#define post_post 249 /* postamble ending */
#define yyy1 250 /* PDF literal text */
#define yyy2 251 /* PDF literal text */
#define yyy3 252 /* PDF literal text */
#define yyy4 253 /* PDF literal text */

#define null_font 0

#define long_char 242 /* \.{VF} command for general character packet */
#define vf_id 202 /* identifies \.{VF} files */

/* go out \.{VF} processing with an error message */
#define bad_vf(a) { xfree(vf_buffer); print_nlp();	\
    print_string("Error in processing VF font (");	\
    print_string(font_name(f));				\
    print_string(".vf): ");				\
    print_string(a);					\
    print_string(", virtual font will be ignored");	\
    print_ln();	 return; }

#define lua_bad_vf(a) { xfree(vf_buffer);		\
		       lua_settop(L,s_top);		\
		       lua_pushnil(L);			\
		       lua_pushstring(L,a);		\
		       return 2; }

#define tmp_b0  tmp_w.qqqq.b0
#define tmp_b1  tmp_w.qqqq.b1
#define tmp_b2  tmp_w.qqqq.b2
#define tmp_b3  tmp_w.qqqq.b3
#define tmp_int tmp_w.cint

#define vf_stack_size 100  /* \.{DVI} files shouldn't |push| beyond this depth */

typedef unsigned char vf_stack_index ;  /* an index into the stack */

typedef struct vf_stack_record {
    scaled stack_w, stack_x, stack_y, stack_z;
} vf_stack_record;

boolean auto_expand_vf(internal_font_number f); /* forward */

/* get a byte from\.{VF} file */

#define vf_byte(a) { real_eight_bits vf_tmp_b;		\
    if (vf_cur>=vf_size) {				\
      pdftex_fail("unexpected eof on virtual font");	\
    }							\
    vf_tmp_b = vf_buffer[vf_cur++] ; a = vf_tmp_b; }


#define vf_replace_z() {			\
    vf_alpha=16;				\
  while (vf_z>=040000000) {			\
    vf_z= vf_z / 2;				\
    vf_alpha += vf_alpha;			\
  }						\
  vf_beta=256 / vf_alpha;			\
  vf_alpha=vf_alpha*vf_z; }

/* read |k| bytes as an integer from \.{VF} file */

#define vf_read(k,l) {  integer itmp = 0, dtmp = k, jtmp = 0;		\
    while (dtmp > 0) {							\
      vf_byte(jtmp);							\
      if ((dtmp==k) && jtmp>127)					\
	jtmp = jtmp - 256;						\
      itmp  = itmp*256 + jtmp;						\
      decr(dtmp);							\
    }									\
    l = itmp; }

#define vf_read_u(k,l) {  unsigned int dtmp=k, itmp = 0, jtmp = 0;	\
    while (dtmp-- > 0) {						\
      vf_byte(jtmp);						\
      itmp  = itmp*256 + jtmp;					\
    }								\
    l = itmp; }

void
pdf_check_vf_cur_val (void) {
 internal_font_number f;
 f = cur_val;
 if (font_type(f) == virtual_font_type)
   pdf_error(maketexstring("font"), maketexstring("command cannot be used with virtual font"));
}

static void
vf_local_font_warning(internal_font_number f, internal_font_number k, char *s, integer a, integer b) {
  print_nlp();
  print_string(s);
  print_string(" in local font ");
  print_string(font_name(k));
  print_string(" (");
  print_int(b);
  print_string(" != ");
  print_int(a);
  print_string(") in virtual font ");
  print_string(font_name(f));
  print_string(".vf ignored.");
}


/* process a local font in \.{VF} file */

internal_font_number 
vf_def_font(internal_font_number f,unsigned char *vf_buffer, integer *vf_cr) {
    internal_font_number k;
    str_number s;
    scaled ds,fs;
    four_quarters cs;
    memory_word tmp_w; /* accumulator */
    integer junk;
    unsigned long checksum;
    cs.b0 = vf_buffer[(*vf_cr)]; 
    cs.b1 = vf_buffer[(*vf_cr)+1]; 
    cs.b2 = vf_buffer[(*vf_cr)+2]; 
    cs.b3 = vf_buffer[(*vf_cr)+3]; (*vf_cr)+=4;
    checksum = cs.b0*256*256*256 +  cs.b1*256*256 + cs.b2*256 +  cs.b3;
    k =  vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    if (k>127) k-=256;
    k = k*256+ vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    k = k*256+ vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    k = k*256+ vf_buffer[(*vf_cr)] ; (*vf_cr)++;

    fs = sqxfw(k, font_size(f));

    k =  vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    if (k>127) k-=256;
    k = k*256+ vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    k = k*256+ vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    k = k*256+ vf_buffer[(*vf_cr)] ; (*vf_cr)++;

    ds = k / 16;


    tmp_b0 = vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    tmp_b1 = vf_buffer[(*vf_cr)] ; (*vf_cr)++;
    while (tmp_b0 > 0) {
      tmp_b0--;
      (*vf_cr)++; /* skip the font path */
    }
    string_room(tmp_b1);
    while (tmp_b1 > 0) {
      tmp_b1--;
      junk = vf_buffer[(*vf_cr)] ; (*vf_cr)++;
      append_pool_char(junk);
    }
    s = make_string();
    k = tfm_lookup(s, fs);
    if (k == null_font) 
      k = read_font_info(get_nullcs(), s, get_nullstr(), fs, -1);
    if (k != null_font) {
      if (checksum!=0 && font_checksum(k)!=0 && checksum!= font_checksum(k))
	vf_local_font_warning(f, k, "checksum mismatch",checksum,font_checksum(k));
      if (ds != font_dsize(k))
	vf_local_font_warning(f, k, "design size mismatch",ds,font_dsize(k));
    }
    if (pdf_font_expand_ratio(f) != 0)
      set_expand_params(k, pdf_font_auto_expand(f),
						pdf_font_expand_ratio(pdf_font_stretch(f)),
						-pdf_font_expand_ratio(pdf_font_shrink(f)),
						pdf_font_step(f), pdf_font_expand_ratio(f));
    return k;
}


int
open_vf_file (char *fn, unsigned char **vbuffer, integer *vsize) {
  boolean res; /* was the callback successful? */
  integer callback_id;
  boolean file_read; /* was |vf_file| successfully read? */
  FILE *vf_file;
  char *fnam = NULL;




  namelength = strlen(fn);
  nameoffile = xmalloc(namelength+2);
  strcpy((char *)(nameoffile+1),fn);

  callback_id=callback_defined(find_vf_file_callback);
  if (callback_id>0) {
    res = run_callback(callback_id,"S->S",fn, &fnam);
    if (res && (fnam!=NULL) && (strlen(fnam)>0)) {
      /* @<Fixup |nameoffile| after callback@>; */
      free(nameoffile);
      namelength = strlen(fnam);
      nameoffile = xmalloc(namelength+2);
      strcpy((char *)(nameoffile+1),fnam);
    } else {
      return 0;
    }
  }
  callback_id=callback_defined(read_vf_file_callback);
  if (callback_id>0) {
    file_read = false;
    res = run_callback(callback_id,"S->bSd",stringcast(nameoffile+1),
		       &file_read, vbuffer,vsize);
    if (res && file_read && (*vsize>0)) {
      return 1; 
    }
    if (!file_read)
      return 0;    /* -1 */
  } else {
    if (ovf_b_open_in(vf_file) || vf_b_open_in(vf_file)) {
      res = read_vf_file(vf_file,vbuffer,vsize);
      b_close(vf_file);
      if (res) {
	return 1;
      }
    } else {
      return 0; /* -1 */
    }
  }
  return 0;
}


/*
  @ The |do_vf| procedure attempts to read the \.{VF} file for a font, and sets
  |font_type()| to |real_font_type| if the \.{VF} file could not be found
  or loaded, otherwise sets |font_type()| to |virtual_font_type|.  At this
  time, |tmp_f| is the internal font number of the current \.{TFM} font.  To
  process font definitions in virtual font we call |vf_def_font|.
*/

#define append_packet(k) vpackets[vf_np++] = k 

/* life is easier if all internal font commands are fnt4 and
   all character commands are set4 or put4 */

#define append_fnt_set(k) {			\
    assert(k>0)	;				\
    append_packet(packet_font_code);		\
    append_four(k); } 

#define append_four(k) {			\
    append_packet((k&0xFF000000)>>24);		\
    append_packet((k&0x00FF0000)>>16);		\
    append_packet((k&0x0000FF00)>>8);		\
    append_packet((k&0x000000FF));  } 

/* some of these things happen twice, adding a define is simplest */

#define test_checksum()  { vf_byte(tmp_b0); vf_byte(tmp_b1);		\
    vf_byte(tmp_b2); vf_byte(tmp_b3);					\
    if (((tmp_b0 != 0) || (tmp_b1 != 0) || (tmp_b2 != 0) || (tmp_b3 != 0)) && \
	((font_check_0(f) != 0) || (font_check_1(f) != 0) ||		\
	 (font_check_2(f) != 0) || (font_check_3(f) != 0)) &&		\
	((tmp_b0 != font_check_0(f)) || (tmp_b1 != font_check_1(f)) ||	\
	 (tmp_b2 != font_check_2(f)) || (tmp_b3 != font_check_3(f)))) {	\
      print_nlp();							\
      print_string("checksum mismatch in font ");			\
      print_string(font_name(f));					\
      print_string(".vf ignored "); } }

#define test_dsize() {  int read_tmp; vf_read(4,read_tmp);		\
    if ((read_tmp / 16) != font_dsize(f)) {				\
      print_nlp();							\
      print_string("design size mismatch in font ");			\
      print_string(font_name(f));					\
      print_string(".vf ignored");					\
    } }


int count_packet_bytes (real_eight_bits *vf_buf, int cur_bute, int count) {
  int k = 0;
  int ff = 0;
  int acc = 0;
  unsigned int cmd = 0;
  unsigned int d = 0;
  while (k<count) {
	cmd = vf_buf[cur_bute+k]; k++;
	if ((cmd >= set_char_0) && (cmd < set1)) {
	  if (ff==0) { ff = 1; acc+=5;  }
	  acc+=5;
	} else if ((fnt_num_0 <= cmd) && (cmd <= fnt_num_0 + 63)) {
	  ff = 1;
	  acc+=5;
	} else { 
	  switch (cmd) {
	  case fnt1:     acc+=5; k+=1; ff=1 ; break;
	  case fnt2:     acc+=5; k+=2; ff=1 ; break;
	  case fnt3:     acc+=5; k+=3; ff=1 ; break;
	  case fnt4:     acc+=5; k+=4; ff=1 ; break;
	  case set_rule: acc+=9; k+=8; break;
	  case put_rule: acc+=11; k+=8; break; 
	  case set1:     acc+=5; k+=1; if (ff==0) { ff = 1; acc+=5;  } break;
	  case set2:     acc+=5; k+=2; if (ff==0) { ff = 1; acc+=5;  } break;
	  case set3:     acc+=5; k+=3; if (ff==0) { ff = 1; acc+=5;  } break;
	  case set4:     acc+=5; k+=4; if (ff==0) { ff = 1; acc+=5;  } break;
	  case put1:     acc+=7; k+=1; if (ff==0) { ff = 1; acc+=5;  } break;
	  case put2:     acc+=7; k+=2; if (ff==0) { ff = 1; acc+=5;  } break;
	  case put3:     acc+=7; k+=3; if (ff==0) { ff = 1; acc+=5;  } break;
	  case put4:     acc+=7; k+=4; if (ff==0) { ff = 1; acc+=5;  } break;
	  case right1:   acc+=5; k+=1; break;
	  case right2:   acc+=5; k+=2; break;
	  case right3:   acc+=5; k+=3; break;
	  case right4:   acc+=5; k+=4; break;
	  case w1:       acc+=5; k+=1; break;
	  case w2:       acc+=5; k+=2; break;
	  case w3:       acc+=5; k+=3; break;
	  case w4:       acc+=5; k+=4; break;
	  case x1:       acc+=5; k+=1; break;
	  case x2:       acc+=5; k+=2; break; 
	  case x3:       acc+=5; k+=3; break; 
	  case x4:       acc+=5; k+=4; break;
	  case down1:    acc+=5; k+=1; break;  
	  case down2:    acc+=5; k+=2; break;  
	  case down3:    acc+=5; k+=3; break;  
	  case down4:    acc+=5; k+=4; break;
	  case y1:       acc+=5; k+=1; break;
	  case y2:       acc+=5; k+=2; break; 
	  case y3:       acc+=5; k+=3; break; 
	  case y4:       acc+=5; k+=4; break;
	  case z1:       acc+=5; k+=1; break;
	  case z2:       acc+=5; k+=2; break; 
	  case z3:       acc+=5; k+=3; break; 
	  case z4:       acc+=5; k+=4; break;
	  case xxx1: 
		d = vf_buf[cur_bute+k]; k++;
		k += d; acc += 5 + d;
		break;
	  case xxx2: 
		d = vf_buf[cur_bute+k]; k++;
		d = d*256 + vf_buf[cur_bute+k]; k++;
		k += d; acc += 5 + d;
		break;
	  case xxx3: 
		d = vf_buf[cur_bute+k]; k++;
		d = d*256 + vf_buf[cur_bute+k]; k++;
		d = d*256 + vf_buf[cur_bute+k]; k++;
		k += d; acc += 5 + d;
		break;
	  case xxx4:
		d = vf_buf[cur_bute+k]; k++;
		d = d*256 + vf_buf[cur_bute+k]; k++;
		d = d*256 + vf_buf[cur_bute+k]; k++;
		d = d*256 + vf_buf[cur_bute+k]; k++;
		k += d; acc += 5 + d;
	  break;
	  case w0:      acc+=5; break;
	  case x0:      acc+=5; break;
	  case y0:      acc+=5; break;
	  case z0:      acc+=5; break;
	  case nop:	    break;
	  case push:    acc+=1; break;
	  case pop:     acc+=1; break;
	  }
	}
  }
  return (acc+1);
}

void 
do_vf(internal_font_number f) {
  integer k, n, i;
  unsigned cmd;
  scaled x,y,w,z,h,v;
  integer cc, cmd_length, packet_length;
  charinfo *co;
  scaled tfm_width;
  integer save_cur_byte;
  vf_stack_index stack_level;
  integer vf_z; /* multiplier */
  integer vf_alpha; /* correction for negative values */
  char vf_beta; /* divisor */
  integer vf_np; 
  real_eight_bits *vpackets;
  memory_word tmp_w; /* accumulator */
  vf_stack_record vf_stack[256];
  integer junk;
  unsigned char *vf_buffer;
  integer vf_size; 
  integer vf_cur;
  integer *vf_local_fnts = NULL; /* external font ids */
  integer *vf_real_fnts = NULL; /* internal font ids */
  unsigned vf_nf = 0; /* local font counter */

  if (font_type(f)!= unknown_font_type)
    return;
  set_font_type(f,real_font_type);
  if (auto_expand_vf(f))
    return; /* auto-expanded virtual font */
  stack_level = 0;
  /* @<Open |vf_file|, return if not found@>; */
  vf_cur=0; vf_buffer=NULL; vf_size=0;
  if (!open_vf_file(font_name(f), &vf_buffer, &vf_size))
    return;
  /* @<Process the preamble@>;@/ */
  set_font_type(f,virtual_font_type);
  vf_byte(k);
  if (k != pre) 
    bad_vf("PRE command expected");
  vf_byte(k);
  if (k != vf_id)
    bad_vf("wrong id byte");
  vf_byte(cmd_length);
  for (k = 1;k<= cmd_length;k++)
    vf_byte(junk);
  test_checksum();
  test_dsize();
  /* update_terminal;*/
  vf_z = font_size(f);
  vf_replace_z();
  /* @<Process the font definitions@>;@/ */
  /* scan forward to find the number of internal fonts */
  vf_nf = 0;
  save_cur_byte = vf_cur;
  vf_byte(cmd);
  while ((cmd >= fnt_def1) && (cmd <= (fnt_def1 + 3))) {
    vf_read((cmd - fnt_def1 + 1),junk);
    vf_read(4,junk);
    vf_read(4,junk);
    vf_read(4,junk);
    vf_byte(k);
    vf_byte(junk);
    k += junk;
    while (k-- > 0) {  vf_byte(junk); } 
    incr(vf_nf);
    vf_byte(cmd);
  }
  vf_cur = save_cur_byte;
  vf_byte(cmd);
  /* malloc and fill the local font arrays */
  if (vf_nf>0) {
    i = vf_nf*sizeof(integer);
    vf_local_fnts= xmalloc(i);
    memset(vf_local_fnts,0,i);
    vf_real_fnts= xmalloc(i);
    memset(vf_real_fnts,0,i);
    vf_nf = 0;
    while ((cmd >= fnt_def1) && (cmd <= (fnt_def1 + 3))) {
      vf_read_u((cmd - fnt_def1 + 1),vf_local_fnts[vf_nf]);
      vf_real_fnts[vf_nf] = vf_def_font(f, vf_buffer, &vf_cur);
      incr(vf_nf);    
      vf_byte(cmd);
    }
  }


  while (cmd <= long_char) {
    /* @<Build a character packet@>;@/ */
	vf_np = 0;
    if (cmd == long_char) {
      vf_read_u(4,packet_length);
      vf_read_u(4,cc);
      if (!char_exists(f,cc)) {
        bad_vf("invalid character code");
      }
      vf_read(4,k);
      tfm_width = sqxfw(k, font_size(f));
    } else {
      packet_length = cmd;
      vf_byte(cc);
      if (!char_exists(f,cc)) {
        bad_vf("invalid character code");
      }
      vf_read(3,k);
      tfm_width = sqxfw(k, font_size(f));
    }
    if (packet_length < 0)
      bad_vf("negative packet length");
    if (tfm_width != char_width(f,cc)) {
      /* precisely 'one off' errors are rampant */
      if (abs(tfm_width - char_width(f,cc))>1) {
		print_nlp();
		print_string("character width mismatch in font ");
		print_string(font_name(f));
		print_string(".vf ignored");
      }
    }
    k  = count_packet_bytes(vf_buffer,vf_cur,packet_length);
    vpackets = xmalloc(k+1);
    co = get_charinfo(f,cc);
    k = 0; 
    w = 0; x = 0; y = 0; z = 0;
    while (packet_length > 0) {
      vf_byte(cmd);
      decr(packet_length);

      if ((cmd >= set_char_0) && (cmd < set1)) {
		if (k == 0) {
		  k = vf_real_fnts[0];
		  append_fnt_set(k);
		}
		append_packet(packet_char_code);
		append_four(cmd);
		cmd_length = 0;
		cmd = nop;
		
      } else if (((fnt_num_0 <= cmd) && (cmd <= fnt_num_0 + 63)) ||
				 ((fnt1 <= cmd) && (cmd <= fnt1 + 3))) {
		if (cmd >= fnt1) {
		  vf_read_u((cmd - fnt1 + 1),k);
		  packet_length -= (cmd - fnt1 + 1);
		} else {
		  k = cmd - fnt_num_0;
		}

		/*  change from local to external font id */
		n = 0;
		while ((n < vf_nf) && (vf_local_fnts[n] != k)) 
		  n++;
		if (n==vf_nf)
		  bad_vf("undefined local font");

		k = vf_real_fnts[n];
		append_fnt_set(k);
		cmd_length = 0;
		cmd = nop;
      } else { 
		switch (cmd) {
		case set_rule: 
		  vf_read(4,h); vf_read(4,v);
		  append_packet(packet_rule_code);
		  append_four(h);
		  append_four(v);
		  packet_length -= 8;
		  break;
		case put_rule: 
		  vf_read(4,h); vf_read(4,v);
		  append_packet(packet_push_code);
		  append_packet(packet_rule_code);
		  append_four(h);
		  append_four(v);
		  append_packet(packet_pop_code);
		  packet_length -= 8;
		  break;
		case set1: 
		case set2: 
		case set3: 
		case set4:
		  if (k == 0) {
		    k = vf_real_fnts[0];
		    append_fnt_set(k);
		  }
		  vf_read_u((cmd - set1 + 1),i);
		  append_packet(packet_char_code);
		  append_four(i);
		  packet_length -= (cmd - set1 + 1);
		  break;
		case put1: 
		case put2: 
		case put3: 
		case put4:
		  if (k == 0) {
			k = vf_real_fnts[0];
			append_fnt_set(k);
		  }
		  vf_read_u((cmd - put1 + 1),i);
		  append_packet(packet_push_code);
		  append_packet(packet_char_code);
		  append_four(i);
		  append_packet(packet_pop_code);
		  packet_length -= (cmd - put1 + 1);
		  break;
		case right1: 
		case right2: 
		case right3: 
		case right4: 
		  vf_read((cmd - right1 + 1),i);
		  append_packet(packet_right_code);
		  append_four(i);
		  packet_length -= (cmd - right1 + 1);
		  break;
		case w1: 
		case w2: 
		case w3: 
		case w4:
		  vf_read((cmd - w1 + 1),w);
		  append_packet(packet_right_code);
		  append_four(w);
		  packet_length -= (cmd - w1 + 1);
		  break;
		case x1:
		case x2: 
		case x3: 
		case x4:
		  vf_read((cmd - x1 + 1),x);
		  append_packet(packet_right_code);
		  append_four(x);
		  packet_length -= (cmd - x1 + 1);
		  break;
		case down1:  
		case down2:  
		case down3:  
		case down4:
		  vf_read((cmd - down1 + 1),i);
		  append_packet(packet_down_code);
		  append_four(i);
		  packet_length -= (cmd - down1 + 1);
		  break;
		case y1: 
		case y2: 
		case y3: 
		case y4:
		  vf_read((cmd - y1 + 1),y);
		  append_packet(packet_down_code);
		  append_four(y);
		  packet_length -= (cmd - y1 + 1);
		  break;
		case z1: 
		case z2: 
		case z3: 
		case z4:
		  vf_read((cmd - z1 + 1),z);
		  append_packet(packet_down_code);
		  append_four(z);
		  packet_length -= (cmd - z1 + 1);
		  break;
		case xxx1: 
		case xxx2: 
		case xxx3: 
		case xxx4:
		  vf_read_u((cmd - xxx1 + 1),cmd_length);
		  packet_length -= (cmd - xxx1 + 1);
		  if (cmd_length <= 0)
			bad_vf("special of negative length");
		  packet_length -= cmd_length;

		  append_packet(packet_special_code);
		  append_four(cmd_length);
		  while (cmd_length > 0) {
			cmd_length--;
			vf_byte(i);
			append_packet(i);
		  }
		  append_packet(0);
		  break;
		case w0: 
		  append_packet(packet_right_code);
		  append_four(w);
		  break;
		case x0: 
		  append_packet(packet_right_code);
		  append_four(x);
		  break;
		case y0: 
		  append_packet(packet_down_code);
		  append_four(y);
		  break;
		case z0: 
		  append_packet(packet_down_code);
		  append_four(z);
		  break;
		case nop:
		  break;
		case push: 
		  if (stack_level == vf_stack_size) {
			overflow_string("virtual font stack size", vf_stack_size);
		  } else {
			vf_stack[stack_level].stack_w = w;
			vf_stack[stack_level].stack_x = x;
			vf_stack[stack_level].stack_y = y;
			vf_stack[stack_level].stack_z = z;
			incr(stack_level);
			append_packet(packet_push_code);
		  }
		  break;
		case pop: 
		  if (stack_level == 0) {
			bad_vf("more POPs than PUSHs in character");
		  } else {
			decr(stack_level);
			w = vf_stack[stack_level].stack_w;
			x = vf_stack[stack_level].stack_x;
			y = vf_stack[stack_level].stack_y;
			z = vf_stack[stack_level].stack_z;
			append_packet(packet_pop_code);
		  }
		  break;
		default:
		  bad_vf("improver DVI command");
		}
      }
    }
    /* signal end of packet */
    append_packet(packet_end_code); 

    if (stack_level != 0)
      bad_vf("more PUSHs than POPs in character packet");
    if (packet_length != 0) 
      bad_vf("invalid packet length or DVI command in packet");
    /* @<Store the packet being built@>; */
    set_charinfo_packets(co,vpackets);
    vf_byte(cmd);
  }
  if (cmd != post)
    bad_vf("POST command expected");

  xfree(vf_buffer);
}

#define make_command0(N,K) {			\
    lua_newtable(L);				\
    lua_pushstring(L, N);			\
    lua_rawseti(L,-2, 1);			\
    lua_rawseti(L,-2, K);			\
    K++; }

#define make_command1(N,V,K) {			\
    lua_newtable(L);				\
    lua_pushstring(L, N);			\
    lua_rawseti(L,-2, 1);			\
    lua_pushnumber(L, V);			\
    lua_rawseti(L,-2, 2);			\
    lua_rawseti(L,-2, K);			\
    K++; }

#define make_command2(N,V,W,K) {		\
    lua_newtable(L);				\
    lua_pushstring(L, N);			\
    lua_rawseti(L,-2, 1);			\
    lua_pushnumber(L, V);			\
    lua_rawseti(L,-2, 2);			\
    lua_pushnumber(L, W);			\
    lua_rawseti(L,-2, 3);			\
    lua_rawseti(L,-2, K);			\
    K++; }

#define make_commands(N,S,V,K) {		\
    lua_newtable(L);				\
    lua_pushstring(L, N);			\
    lua_rawseti(L,-2, 1);			\
    lua_pushlstring(L, S, V);			\
    lua_rawseti(L,-2, 2);			\
    lua_rawseti(L,-2, K);			\
    K++; }


int 
make_vf_table(lua_State *L, char *cnom, scaled atsize) {
  integer cmd, k, i;
  integer cc, cmd_length, packet_length;
  scaled tfm_width;
  vf_stack_index stack_level;
  integer vf_z; /* multiplier */
  integer vf_alpha; /* correction for negative values */
  char vf_beta; /* divisor */
  char *s;
  scaled h,v;
  scaled w, x, y, z;
  integer s_top; /* lua stack */
  integer vf_nf ; /* local font counter */
  scaled ds,fs;
  four_quarters cs;
  memory_word tmp_w; /* accumulator */
  vf_stack_record vf_stack[256];
  unsigned char *vf_buffer;
  integer vf_size; 
  integer vf_cur;


  stack_level = 0;
  /* @<Open |vf_file|, return if not found@>; */
  vf_cur=0; vf_buffer=NULL; vf_size=0;  
  if (!open_vf_file(cnom, &vf_buffer, &vf_size)) {
    lua_pushnil(L);
    return 1;
  }

  /* start by creating a table */
  s_top = lua_gettop(L);
  lua_newtable(L);

  /* @<Process the preamble@>;@/ */
  vf_byte(k);
  if (k != pre) 
    lua_bad_vf("PRE command expected");
  vf_byte(k);
  if (k != vf_id)
    lua_bad_vf("wrong id byte");
  vf_byte(cmd_length);

  s = xmalloc(cmd_length);
  for (k = 1;k<= cmd_length;k++)
    vf_byte(s[(k-1)]);

  lua_pushlstring(L,s,cmd_length);
  free(s);
  lua_setfield(L,-2,"header");

  vf_byte(cs.b0); vf_byte(cs.b1); vf_byte(cs.b2); vf_byte(cs.b3); 
  lua_pushnumber(L, (unsigned long)(cs.b0<<24)+(cs.b1<<16)+(cs.b2<<8)+cs.b3 );
  lua_setfield(L,-2,"checksum");
  
  vf_read(4,k);
  ds = k /16;
  lua_pushinteger(L,ds);
  lua_setfield(L,-2,"designsize");


  lua_pushstring(L,cnom);
  lua_setfield(L,-2,"name");

  lua_pushinteger(L,atsize);
  lua_setfield(L,-2,"size");


  /* update_terminal;*/
  vf_z = atsize;
  vf_replace_z();
  /* @<Process the font definitions@>;@/ */
  vf_byte(cmd);
  lua_newtable(L);  

  i = 1;
  while ((cmd >= fnt_def1) && (cmd <= fnt_def1 + 3)) {

    lua_newtable(L);      
    vf_read_u((cmd - fnt_def1 + 1),vf_nf);
    vf_nf++;
    /* checksum */
    vf_byte(cs.b0); vf_byte(cs.b1); vf_byte(cs.b2); vf_byte(cs.b3); 

    vf_read(4,k);
    fs = sqxfw(k, atsize);
	lua_pushstring(L,"size");
    lua_pushnumber(L,fs);
    lua_rawset(L,-3);
    
    vf_read(4,k);
    ds = k / 16; /* dsize, not used */
    
    vf_byte(tmp_b0);
    vf_byte(tmp_b1);
    while (tmp_b0 > 0) {  tmp_b0--; vf_byte(k); } /* skip the font path */
    
    s = xmalloc(tmp_b1+1); k = 0;
    while (tmp_b1-- > 0)
      vf_byte(s[k++]);    
    s[k]=0;
	lua_pushstring(L,"name");
    lua_pushstring(L,xstrdup(s));
    free(s);
    lua_rawset(L,-3);
    
    lua_rawseti(L,-2,vf_nf);
    i++;   
    vf_byte(cmd);
  }

  if (i>1) {
    lua_setfield(L,-2,"fonts");
  } else {
    lua_pop(L,1);
  }

  lua_newtable(L); /* 'characters' */
  while (cmd <= long_char) {
    /* @<Build a character packet@>;@/ */
    if (cmd == long_char) {
      vf_read_u(4,packet_length);
      vf_read_u(4,cc);
      vf_read(4,tfm_width);
    } else {
      packet_length = cmd;
      vf_byte(cc);
      vf_read(3,tfm_width);
    }
    if (packet_length < 0)
      lua_bad_vf("negative packet length");

    lua_newtable(L); /* for this character */

    lua_pushnumber(L,tfm_width);
    lua_setfield(L,-2,"width");

    lua_newtable(L); /* for 'commands' */

    k = 1; vf_nf = 0;
    w = 0; x = 0; y = 0; z = 0;
    while (packet_length > 0) {
      vf_byte(cmd);
      decr(packet_length);
	  
      if ((cmd >= set_char_0) && (cmd < set1)) {
		if (vf_nf == 0) {
		  vf_nf  = 1;
		  make_command1("font",vf_nf,k);
		}
		make_command1("char",cmd,k);
		
      } else if (((fnt_num_0 <= cmd) && (cmd <= fnt_num_0 + 63)) ||
		 ((fnt1 <= cmd) && (cmd <= fnt1 + 3))) {
		if (cmd >= fnt1) {
		  vf_read_u((cmd - fnt1 + 1),vf_nf); vf_nf++;
		  packet_length -= (cmd - fnt1 + 1);
		} else {
		  vf_nf = cmd - fnt_num_0 + 1;
		}
		make_command1("font",vf_nf,k);
      } else { 
	switch (cmd) {
	case set_rule: 
	  vf_read(4,h); vf_read(4,v);
	  make_command2("rule",sqxfw(h,atsize),sqxfw(v,atsize),k);
	  packet_length -= 8;
	  break;
	case put_rule: 
	  vf_read(4,h); vf_read(4,v);
	  make_command0("push",k);
	  make_command2("rule",sqxfw(h,atsize),sqxfw(v,atsize),k);
	  make_command0("pop",k);
	  packet_length -= 8;
	  break;
	case set1: 
	case set2: 
	case set3: 
	case set4:
	  if (vf_nf == 0) {
	    vf_nf  = 1;
	    make_command1("font",vf_nf,k);
	  }
	  vf_read_u((cmd - set1 + 1),i);
	  make_command1("char",i,k);
	  packet_length -= (cmd - set1 + 1);
	  break;
	case put1: 
	case put2: 
	case put3: 
	case put4:
	  if (vf_nf == 0) {
	    vf_nf  = 1;
	    make_command1("font",vf_nf,k);
	  }
	  vf_read_u((cmd - put1 + 1),i);
	  make_command0("push",k);
	  make_command1("char",i,k);
	  make_command0("pop",k);
	  packet_length -= (cmd - put1 + 1);
	  break;
	case right1: 
	case right2: 
	case right3: 
	case right4: 
	  vf_read((cmd - right1 + 1),i);
	  make_command1("right",sqxfw(i,atsize),k);
	  packet_length -= (cmd - right1 + 1);
	  break;
	case w1: 
	case w2: 
	case w3: 
	case w4:
	  vf_read((cmd - w1 + 1),w);
	  make_command1("right",sqxfw(w,atsize),k);
	  packet_length -= (cmd - w1 + 1);
	  break;
	case x1:
	case x2: 
	case x3: 
	case x4:
	  vf_read((cmd - x1 + 1),x);	  
	  make_command1("right",sqxfw(x,atsize),k);
	  packet_length -= (cmd - x1 + 1);
	  break;
	case down1:  
	case down2:  
	case down3:  
	case down4:  
	  vf_read((cmd - down1 + 1),i);
	  make_command1("down",sqxfw(i,atsize),k);
	  packet_length -= (cmd - down1 + 1);
	  break;
	case y1: 
	case y2: 
	case y3: 
	case y4:
	  vf_read((cmd - y1 + 1),y);
	  make_command1("down",sqxfw(y,atsize),k);
	  packet_length -= (cmd - y1 + 1);
	  break;
	case z1: 
	case z2: 
	case z3: 
	case z4:
	  vf_read((cmd - z1 + 1),z);
	  make_command1("down",sqxfw(z,atsize),k);
	  packet_length -= (cmd - z1 + 1);
	  break;
	case xxx1: 
	case xxx2: 
	case xxx3: 
	case xxx4:
	  vf_read_u((cmd - xxx1 + 1),cmd_length);
	  packet_length -= (cmd - xxx1 + 1);
	  if (cmd_length <= 0)
	    lua_bad_vf("special of negative length");
	  packet_length -= cmd_length;

	  s = xmalloc(cmd_length+1); i = 0;
	  while (cmd_length > 0) {
	    cmd_length--;
	    vf_byte(s[i]);
	    i++;
	  }
	  s[i]=0;
	  make_commands("special",strdup(s),i,k);
	  free(s);
	  break;
	case w0: 
	  make_command1("right",sqxfw(w,atsize),k);
	  break;
	case x0: 
	  make_command1("right",sqxfw(x,atsize),k);
	  break;
	case y0: 
	  make_command1("down",sqxfw(y,atsize),k);
	  break;
	case z0: 
	  make_command1("down",sqxfw(z,atsize),k);
	  break;
	case nop:
	  break;
	case push: 
	  if (stack_level == vf_stack_size) {
	    overflow_string("virtual font stack size", vf_stack_size);
	  } else {
	    vf_stack[stack_level].stack_w = w;
	    vf_stack[stack_level].stack_x = x;
	    vf_stack[stack_level].stack_y = y;
	    vf_stack[stack_level].stack_z = z;
	    incr(stack_level);
	    make_command0("push",k);
	  }
	  break;
	case pop: 
	  if (stack_level == 0) {
	    lua_bad_vf("more POPs than PUSHs in character");
	  } else {
	    decr(stack_level);
	    w = vf_stack[stack_level].stack_w;
	    x = vf_stack[stack_level].stack_x;
	    y = vf_stack[stack_level].stack_y;
	    z = vf_stack[stack_level].stack_z;
	    make_command0("pop",k);
	  }
	  break;
	default:
	  lua_bad_vf("improver DVI command");
	}
      }
    }
    /* signal end of packet */
    lua_setfield(L,-2,"commands");

    if (stack_level != 0)
      lua_bad_vf("more PUSHs than POPs in character packet");
    if (packet_length != 0) 
      lua_bad_vf("invalid packet length or DVI command in packet");

    lua_rawseti(L,-2,cc);

    vf_byte(cmd);
  }
  lua_setfield(L,-2,"characters");

  if (cmd != post)
    lua_bad_vf("POST command expected");
  xfree(vf_buffer);
  return 1;
}


extern void replace_packet_fonts(internal_font_number f, integer *old_fontid, integer *new_fontid, int count) ;
extern integer *packet_local_fonts(internal_font_number f, integer *num);
extern internal_font_number auto_expand_font (internal_font_number f, integer e) ;


/* This function is called from |do_vf|, and fixes up the virtual data 
   inside an auto-expanded virtual font */

boolean 
auto_expand_vf(internal_font_number f) {

  internal_font_number bf;
  integer e, k;
  integer *vf_old_fonts, *vf_new_fonts;
  integer num = 0;

  if ((! pdf_font_auto_expand(f)) || (pdf_font_blink(f) == null_font))
    return false ; /* not an auto-expanded font */
  bf = pdf_font_blink(f);
  
  if (font_type(bf) != virtual_font_type)
    return false; /* not a virtual font */

  e = pdf_font_expand_ratio(f);

  vf_old_fonts = packet_local_fonts(bf,&num);
  if (num>0) {
    vf_new_fonts = xmalloc(num*sizeof(integer));
    for (k=0;k<num;k++) {
      vf_new_fonts[k] = auto_expand_font(vf_old_fonts[k], e);
      copy_expand_params(vf_new_fonts[k], vf_old_fonts[k], e);
    }
    replace_packet_fonts(f,vf_old_fonts,vf_new_fonts,num);
    xfree(vf_new_fonts);
    xfree(vf_old_fonts);
  }
  set_font_type(f,virtual_font_type);
  return true;
}

/* this function is not always safe, because it needs makecstring() */
str_number 
expand_font_name (internal_font_number f, integer e) {
  int old_setting;
  old_setting=selector; 
  selector=new_string;
  print_string(font_name(f));
  if (e > 0) {
	print_char('+');
  }
  print_int(e);
  selector=old_setting;
  return make_string();
}


internal_font_number
auto_expand_font (internal_font_number f, integer e) {
  internal_font_number k;
  kerninfo *krn;
  charinfo *co, *ci;
  char *fn;
  integer i;
  scaled w;
  k = copy_font(f);
  i = strlen(font_name(f))+12;
  fn = xmalloc(i);
  snprintf(fn,i,"%s%s%d",font_name(f),(e>0 ? "+" : ""), (int)e);
  set_font_name(k,fn);
  for (i = font_bc(k);i<=font_ec(k);i++) {	
    if (char_exists(k,i)) {
      co = get_charinfo(k,i);
      w = round_xn_over_d(char_width(f,i),  1000 + e, 1000);
	  set_charinfo_width (co, w);
      w = round_xn_over_d(char_italic(f,i), 1000 + e, 1000);
      set_charinfo_italic(co, w);
      krn = get_charinfo_kerns(co);
      if (krn != NULL) {
		while (!kern_end((*krn))) {
		  kern_kern(*krn) = round_xn_over_d(kern_kern(*krn), 1000 + e, 1000);
		  krn++;
		}
      }
    }
  }
  return k;
}

void 
vf_expand_local_fonts(internal_font_number f) {
  internal_font_number lf;
  integer k, num;
  integer *vf_old_fonts;
  pdfassert(font_type(f) == virtual_font_type);
  num  = 0;
  vf_old_fonts = packet_local_fonts(f,&num);
  if (num>0) {
    for (k = 0;k<num;k++) {
      lf = vf_old_fonts[k];
      set_expand_params(lf, pdf_font_auto_expand(f),
						pdf_font_expand_ratio(pdf_font_stretch(f)),
						-pdf_font_expand_ratio(pdf_font_shrink(f)),
						pdf_font_step(f), pdf_font_expand_ratio(f));
      if (font_type(lf) == virtual_font_type)
	vf_expand_local_fonts(lf);
    }
    xfree(vf_old_fonts);
  }
}

internal_font_number 
letter_space_font (halfword u, internal_font_number f, integer e) {
  internal_font_number k;
  scaled  w, r;

  char *new_font_name;
  integer vf_z;
  integer vf_alpha;
  integer vf_beta;
  memory_word tmp_w; /* accumulator */


  /* read a new font and expand the character widths */
  k = read_font_info(u, tex_font_name(f), get_nullstr(), font_size(f), font_natural_dir(f));
  set_no_ligatures(k); /* disable ligatures for letter-spaced fonts */
/*
  for (i = 0;i <= font_widths(k);i++) {
    set_font_width(k,i,font_width(f,i)+round_xn_over_d(quad(k), e, 1000));
  }
*/
  /* append eg '+100ls' to font name */
  new_font_name = xmalloc(strlen(font_name(k)) + 8); /* |abs(e) <= 1000| */
  if (e > 0) {
    sprintf(new_font_name,"%s+%ils",font_name(k),(int)e);
  } else {
    /* minus from %i */
    sprintf(new_font_name,"%s%ils",font_name(k),(int)e);
  }
  set_font_name(k, new_font_name);
  
  /* create the corresponding virtual font */
  set_font_type(k,virtual_font_type);

  vf_z = font_size(f);
  vf_replace_z();
  w = round_xn_over_d(quad(f), e, 2000);
  if (w > 0) {
    tmp_b0 = 0;
  } else {
    tmp_b0 = 255;
    w = vf_alpha + w;
  }
  r = w*vf_beta;
  tmp_b1 = r / vf_z;
  r = r % vf_z;
  if (r == 0) {
    tmp_b2 = 0;
  } else {
    r = r * 256;
    tmp_b2 = r / vf_z;
    r = r % vf_z;
  }
  if (r == 0) {
    tmp_b3 = 0;
  } else {
    r = r * 256;
    tmp_b3 = r / vf_z;
  }
  /*
  vf_packet_base[k] = new_vf_packet(k);

  for (c=font_bc(k);c<=font_ec(k);c++) {
    string_room(17); 
    append_fnt_set(f);
    append_packet(right1 + 3);
    append_packet(tmp_b0);
    append_packet(tmp_b1);
    append_packet(tmp_b2);
    append_packet(tmp_b3);

    append_packet(set1);
    append_packet(c);
      
    append_packet(right1 + 3);
    append_packet(tmp_b0);
    append_packet(tmp_b1);
    append_packet(tmp_b2);
    append_packet(tmp_b3);
    s = make_string();
    store_packet(k, c, s);
    flush_str(s);
  }
*/
  return k;
}


