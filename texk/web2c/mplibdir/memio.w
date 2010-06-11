% $Id: psout.w 616 2008-07-09 14:05:55Z taco $
%
% Copyright 2008-2009 Taco Hoekwater.
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU Lesser General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU Lesser General Public License for more details.
%
% You should have received a copy of the GNU Lesser General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% TeX is a trademark of the American Mathematical Society.
% METAFONT is a trademark of Addison-Wesley Publishing Company.
% PostScript is a trademark of Adobe Systems Incorporated.

% Here is TeX material that gets inserted after \input webmac

\font\tenlogo=logo10 % font used for the METAFONT logo
\def\MP{{\tenlogo META}\-{\tenlogo POST}}
\def\title{MetaPost MEM reading and writing}
\def\topofcontents{\hsize 5.5in
  \vglue -30pt plus 1fil minus 1.5in
  \def\?##1]{\hbox to 1in{\hfil##1.\ }}
  }
\def\botofcontents{\vskip 0pt plus 1fil minus 1.5in}
\pdfoutput=1
\pageno=3

@ As usual, need true and false.

@d true 1
@d false 0
@d null 0 /* the null pointer */
@d incr(A)   (A)=(A)+1 /* increase a variable by unity */
@d decr(A)   (A)=(A)-1 /* decrease a variable by unity */
@d qo(A) (A) /* to read eight bits from a quarterword */
@d qi(A) (quarterword)(A) /* to store eight bits in a quarterword */
@d max_quarterword 0x3FFF /* largest allowable value in a |quarterword| */
@d max_halfword 0xFFFFFFF /* largest allowable value in a |halfword| */
@d max_str_ref 127 /* ``infinite'' number of references */


@c
#include <w2c/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mplib.h"
#include "mpmp.h"
static void swap_items (char *p,  int nitems,  size_t size);
/* If we have these macros, use them, as they provide a better guide to
   the endianess when cross-compiling. */
#if defined (BYTE_ORDER) && defined (BIG_ENDIAN) && defined (LITTLE_ENDIAN)
#ifdef WORDS_BIGENDIAN
#undef WORDS_BIGENDIAN
#endif
#if BYTE_ORDER == BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif
#endif
/* More of the same, but now NeXT-specific. */
#ifdef NeXT
#ifdef WORDS_BIGENDIAN
#undef WORDS_BIGENDIAN
#endif
#ifdef __BIG_ENDIAN__
#define WORDS_BIGENDIAN
#endif
#endif
#include "mpmemio.h" /* internal header */

@ @(mpmemio.h@>=
extern void mp_store_mem_file (MP mp);

@ @c void mp_store_mem_file (MP mp) {
  integer k;  /* all-purpose index */
  pointer p,q; /* all-purpose pointers */
  integer x; /* something to dump */
  four_quarters w; /* four ASCII codes */
  memory_word WW;
  @<Create the |mem_ident|, open the mem file,
    and inform the user that dumping has begun@>;
  @<Dump constants for consistency check@>;
  @<Dump the string pool@>;
  @<Dump the dynamic memory@>;
  @<Dump the table of equivalents and the hash table@>;
  @<Dump a few more things and the closing check word@>;
  @<Close the mem file@>;
}

@ Corresponding to the procedure that dumps a mem file, we also have a function
that reads~one~in. The function returns |false| if the dumped mem is
incompatible with the present \MP\ table sizes, etc.

@d too_small(A) { wake_up_terminal;
  wterm_ln("---! Must increase the "); wterm((A));
@.Must increase the x@>
  goto OFF_BASE;
  }

@(mpmemio.h@>=
extern boolean mp_load_mem_file (MP mp);

@ @c
boolean mp_load_mem_file (MP mp) {
  integer k; /* all-purpose index */
  pointer p,q; /* all-purpose pointers */
  integer x; /* something undumped */
  str_number s; /* some temporary string */
  four_quarters w; /* four ASCII codes */
  memory_word WW;
  @<Undump the string pool@>;
  @<Undump the dynamic memory@>;
  @<Undump the table of equivalents and the hash table@>;
  @<Undump a few more things and the closing check word@>;
  return true; /* it worked! */
OFF_BASE: 
  wake_up_terminal;
  wterm_ln("(Fatal mem file error; I'm stymied)\n");
@.Fatal mem file error@>
   return false;
}

@ If we want mem files to be shareable between different endianness architectures,
we have to swap some of the items so that everything turns out right.
Code borrowed from |texmfmp.c|.

Make the NITEMS items pointed at by P, each of size SIZE, be the
   opposite-endianness of whatever they are now.  */

@d SWAP(x, y) temp = (x); (x) = (y); (y) = temp

@c
static void
swap_items (char *p,  int nitems,  size_t size)
{
  char temp;
#if !defined (WORDS_BIGENDIAN)
  /* Since `size' does not change, we can write a while loop for each
     case, and avoid testing `size' for each time.  */
  switch (size)
    {
    /* 16-byte items happen on the DEC Alpha machine when we are not
       doing sharable memory dumps.  */
    case 16:
      while (nitems--)
        {
          SWAP (p[0], p[15]);
          SWAP (p[1], p[14]);
          SWAP (p[2], p[13]);
          SWAP (p[3], p[12]);
          SWAP (p[4], p[11]);
          SWAP (p[5], p[10]);
          SWAP (p[6], p[9]);
          SWAP (p[7], p[8]);
          p += size;
        }
      break;

    case 8:
      while (nitems--)
        {
          SWAP (p[0], p[7]);
          SWAP (p[1], p[6]);
          SWAP (p[2], p[5]);
          SWAP (p[3], p[4]);
          p += size;
        }
      break;

    case 4:
      while (nitems--)
        {
          SWAP (p[0], p[3]);
          SWAP (p[1], p[2]);
          p += size;
        }
      break;

    case 2:
      while (nitems--)
        {
          SWAP (p[0], p[1]);
          p += size;
        }
      break;

    case 1:
      /* Nothing to do.  */
      break;

    default:
      fprintf (stderr, "Can't swap a %d-byte item for (un)dumping", (int)size);
      exit(1);
  }
#endif
}

@ Mem files consist of |memory_word| items, and we use the following
macros to dump words of different types:

@d dump_wd(A)   { WW=(A);       swap_items((char *)&WW,1,sizeof(WW)); (mp->write_binary_file)(mp,mp->mem_file,&WW,sizeof(WW)); }
@d dump_int(A)  { int cint=(A); swap_items((char *)&cint,1,sizeof(cint)); (mp->write_binary_file)(mp,mp->mem_file,&cint,sizeof(cint)); }
@d dump_hh(A)   { WW.hh=(A);    swap_items((char *)&WW,1,sizeof(WW)); (mp->write_binary_file)(mp,mp->mem_file,&WW,sizeof(WW)); }
@d dump_qqqq(A) { WW.qqqq=(A);  swap_items((char *)&WW,1,sizeof(WW)); (mp->write_binary_file)(mp,mp->mem_file,&WW,sizeof(WW)); }
@d dump_string(A) { dump_int((int)(strlen(A)+1));
                    (mp->write_binary_file)(mp,mp->mem_file,A,strlen(A)+1); }

@ The inverse macros are slightly more complicated, since we need to check
the range of the values we are reading in. We say `|undump(a)(b)(x)|' to
read an integer value |x| that is supposed to be in the range |a<=x<=b|.

@d mgeti(A) do {
  size_t wanted = sizeof(A);
  void *A_ptr = &A;
  (mp->read_binary_file)(mp, mp->mem_file,&A_ptr,&wanted);
  if (wanted!=sizeof(A)) goto OFF_BASE;
  swap_items(A_ptr,1,wanted); 
} while (0)

@d mgetw(A) do {
  size_t wanted = sizeof(A);
  void *A_ptr = &A;
  (mp->read_binary_file)(mp, mp->mem_file,&A_ptr,&wanted);
  if (wanted!=sizeof(A)) goto OFF_BASE;
  swap_items(A_ptr,1,wanted); 
} while (0)

@d undump_wd(A)   { mgetw(WW); A=WW; }
@d undump_int(A)  { int cint; mgeti(cint); A=cint; }
@d undump_hh(A)   { mgetw(WW); A=WW.hh; }
@d undump_qqqq(A) { mgetw(WW); A=WW.qqqq; }
@d undump_strings(A,B,C) { 
   undump_int(x); if ( (x<(A)) || (x>(B)) ) goto OFF_BASE; else C=str(x); }
@d undump(A,B,C) { undump_int(x); 
                   if ( (x<(A)) || (x>(int)(B)) ) goto OFF_BASE; else C=x; }
@d undump_size(A,B,C,D) { undump_int(x);
                          if (x<(A)) goto OFF_BASE; 
                          if (x>(B)) too_small((C)); else D=x; }
@d undump_string(A) { 
  size_t the_wanted; 
  void *the_string;
  integer XX=0; 
  undump_int(XX);
  the_wanted = (size_t)XX;
  the_string = mp_xmalloc(mp,(size_t)XX,1);
  (mp->read_binary_file)(mp,mp->mem_file,&the_string,&the_wanted);
  A = (char *)the_string;
  if (the_wanted!=(size_t)XX) goto OFF_BASE;
}

@ The next few sections of the program should make it clear how we use the
dump/undump macros.

@<Dump constants for consistency check@>=
x = metapost_magic; dump_int(x);
dump_int(mp->mem_top);
dump_int((integer)mp->hash_size);
dump_int(mp->hash_prime)
dump_int(mp->param_size);
dump_int(mp->max_in_open);

@ Sections of a \.{WEB} program that are ``commented out'' still contribute
strings to the string pool; therefore \.{INIMP} and \MP\ will have
the same strings. (And it is, of course, a good thing that they do.)
@.WEB@>
@^string pool@>

@(mpmemio.h@>=
extern int mp_undump_constants (MP mp);

@ @c
int mp_undump_constants (MP mp) {
  integer x;
  undump_int(x); 
  if (x!=metapost_magic) return x;
  undump_int(x); mp->mem_top = x;
  undump_int(x); mp->hash_size = (unsigned)x;
  undump_int(x); mp->hash_prime = x;
  undump_int(x); mp->param_size = x;
  undump_int(x); mp->max_in_open = x;
  return metapost_magic;
  OFF_BASE:
    return -1;
}

@ We do string pool compaction to avoid dumping unused strings.

@d dump_four_ASCII 
  w.b0=qi(mp->str_pool[k]); w.b1=qi(mp->str_pool[k+1]);
  w.b2=qi(mp->str_pool[k+2]); w.b3=qi(mp->str_pool[k+3]);
  dump_qqqq(w)

@<Dump the string pool@>=
mp_do_compaction(mp, mp->pool_size);
dump_int(mp->pool_ptr);
dump_int(mp->max_str_ptr);
dump_int(mp->str_ptr);
k=0;
while ( (mp->next_str[k]==k+1) && (k<=mp->max_str_ptr) ) 
  k++;
dump_int(k);
while ( k<=mp->max_str_ptr ) { 
  dump_int(mp->next_str[k]); incr(k);
}
k=0;
while (1)  { 
  dump_int(mp->str_start[k]); /* TODO: valgrind warning here */
  if ( k==mp->str_ptr ) {
    break;
  } else { 
    k=mp->next_str[k]; 
  }
}
k=0;
while (k+4<mp->pool_ptr ) {
  dump_four_ASCII; k=k+4; 
}
k=mp->pool_ptr-4; dump_four_ASCII;
mp_print_ln(mp); mp_print(mp, "at most "); mp_print_int(mp, mp->max_str_ptr);
mp_print(mp, " strings of total length ");
mp_print_int(mp, mp->pool_ptr)

@ @d undump_four_ASCII 
  undump_qqqq(w);
  mp->str_pool[k]=(ASCII_code)qo(w.b0); mp->str_pool[k+1]=(ASCII_code)qo(w.b1);
  mp->str_pool[k+2]=(ASCII_code)qo(w.b2); mp->str_pool[k+3]=(ASCII_code)qo(w.b3)

@<Undump the string pool@>=
undump_int(mp->pool_ptr);
mp_reallocate_pool(mp, mp->pool_ptr) ;
undump_int(mp->max_str_ptr);
mp_reallocate_strings (mp,mp->max_str_ptr) ;
undump(0,mp->max_str_ptr,mp->str_ptr);
undump(0,mp->max_str_ptr+1,s);
for (k=0;k<=s-1;k++) 
  mp->next_str[k]=k+1;
for (k=s;k<=mp->max_str_ptr;k++) 
  undump(s+1,mp->max_str_ptr+1,mp->next_str[k]);
mp->fixed_str_use=0;
k=0;
while (1) { 
  undump(0,mp->pool_ptr,mp->str_start[k]);
  if ( k==mp->str_ptr ) break;
  mp->str_ref[k]=max_str_ref;
  incr(mp->fixed_str_use);
  mp->last_fixed_str=k; k=mp->next_str[k];
}
k=0;
while ( k+4<mp->pool_ptr ) { 
  undump_four_ASCII; k=k+4;
}
k=mp->pool_ptr-4; undump_four_ASCII;
mp->init_str_use=mp->fixed_str_use; mp->init_pool_ptr=mp->pool_ptr;
mp->max_pool_ptr=mp->pool_ptr;
mp->strs_used_up=mp->fixed_str_use;
mp->pool_in_use=mp->str_start[mp->str_ptr]; mp->strs_in_use=mp->fixed_str_use;
mp->max_pl_used=mp->pool_in_use; mp->max_strs_used=mp->strs_in_use;
mp->pact_count=0; mp->pact_chars=0; mp->pact_strs=0;

@ By sorting the list of available spaces in the variable-size portion of
|mem|, we are usually able to get by without having to dump very much
of the dynamic memory.

We recompute |var_used| and |dyn_used|, so that \.{INIMP} dumps valid
information even when it has not been gathering statistics.

@<Dump the dynamic memory@>=
mp_sort_avail(mp); mp->var_used=0;
dump_int(mp->lo_mem_max); dump_int(mp->rover);
p=0; q=mp->rover; x=0;
do {  
  for (k=p;k<= q+1;k++) 
    dump_wd(mp->mem[k]);
  x=x+q+2-p; mp->var_used=mp->var_used+q-p;
  p=q+node_size(q); q=rmp_link(q);
} while (q!=mp->rover);
mp->var_used=mp->var_used+mp->lo_mem_max-p; 
mp->dyn_used=mp->mem_end+1-mp->hi_mem_min;
for (k=p;k<= mp->lo_mem_max;k++ ) 
  dump_wd(mp->mem[k]);
x=x+mp->lo_mem_max+1-p;
dump_int(mp->hi_mem_min); dump_int(mp->avail);
for (k=mp->hi_mem_min;k<=mp->mem_end;k++ ) 
  dump_wd(mp->mem[k]);
x=x+mp->mem_end+1-mp->hi_mem_min;
p=mp->avail;
while ( p!=null ) { 
  decr(mp->dyn_used); p=mp_link(p);
}
dump_int(mp->var_used); dump_int(mp->dyn_used);
mp_print_ln(mp); mp_print_int(mp, x);
mp_print(mp, " memory locations dumped; current usage is ");
mp_print_int(mp, mp->var_used); mp_print_char(mp, xord('&')); mp_print_int(mp, mp->dyn_used)

@ @<Undump the dynamic memory@>=
undump(lo_mem_stat_max+1000,hi_mem_stat_min-1,mp->lo_mem_max);
undump(lo_mem_stat_max+1,mp->lo_mem_max,mp->rover);
p=0; q=mp->rover;
do {  
  for (k=p;k<= q+1; k++) 
    undump_wd(mp->mem[k]);
  p=q+node_size(q);
  if ( (p>mp->lo_mem_max)||((q>=rmp_link(q))&&(rmp_link(q)!=mp->rover)) ) 
    goto OFF_BASE;
  q=rmp_link(q);
} while (q!=mp->rover);
for (k=p;k<=mp->lo_mem_max;k++ ) 
  undump_wd(mp->mem[k]);
undump(mp->lo_mem_max+1,hi_mem_stat_min,mp->hi_mem_min);
undump(null,mp->mem_top,mp->avail); mp->mem_end=mp->mem_top;
mp->last_pending=spec_head;
for (k=mp->hi_mem_min;k<= mp->mem_end;k++) 
  undump_wd(mp->mem[k]);
undump_int(mp->var_used); undump_int(mp->dyn_used)

@ A different scheme is used to compress the hash table, since its lower region
is usually sparse. When |text(p)<>0| for |p<=hash_used|, we output three
words: |p|, |hash[p]|, and |eqtb[p]|. The hash table is, of course, densely
packed for |p>=hash_used|, so the remaining entries are output in~a~block.

@<Dump the table of equivalents and the hash table@>=
dump_int(mp->hash_used); 
mp->st_count=frozen_inaccessible-1-mp->hash_used;
for (p=1;p<=mp->hash_used;p++) {
  if ( text(p)!=0 ) {
     dump_int(p); dump_hh(mp->hash[p]); dump_hh(mp->eqtb[p]); incr(mp->st_count);
  }
}
for (p=mp->hash_used+1;p<=(int)hash_end;p++) {
  dump_hh(mp->hash[p]); dump_hh(mp->eqtb[p]);
}
dump_int(mp->st_count);
mp_print_ln(mp); mp_print_int(mp, mp->st_count); mp_print(mp, " symbolic tokens")

@ @<Undump the table of equivalents and the hash table@>=
undump(1,frozen_inaccessible,mp->hash_used); 
p=0;
do {  
  undump(p+1,mp->hash_used,p); 
  undump_hh(mp->hash[p]); undump_hh(mp->eqtb[p]);
} while (p!=mp->hash_used);
for (p=mp->hash_used+1;p<=(int)hash_end;p++ )  { 
  undump_hh(mp->hash[p]); undump_hh(mp->eqtb[p]);
}
undump_int(mp->st_count)

@ We have already printed a lot of statistics, so we set |mp_tracing_stats:=0|
to prevent them appearing again.

@<Dump a few more things and the closing check word@>=
dump_int(mp->max_internal);
dump_int(mp->int_ptr);
for (k=1;k<= mp->int_ptr;k++ ) { 
  dump_int(mp->internal[k]); 
  dump_int(mp->int_type[k]); 
  dump_string(mp->int_name[k]);
}
dump_int(mp->start_sym); 
dump_int(mp->interaction); 
dump_string(mp->mem_ident);
dump_int(mp->bg_loc); dump_int(mp->eg_loc); dump_int(mp->serial_no); dump_int(69073);
mp->internal[mp_tracing_stats]=0

@ @<Undump a few more things and the closing check word@>=
undump_int(x);
if (x>mp->max_internal) mp_grow_internals(mp,x);
undump_int(mp->int_ptr);
for (k=1;k<= mp->int_ptr;k++) { 
  undump_int(mp->internal[k]);
  undump_int(mp->int_type[k]); 
  undump_string(mp->int_name[k]);
}
undump(0,frozen_inaccessible,mp->start_sym);
if (mp->interaction==mp_unspecified_mode) {
  undump(mp_unspecified_mode,mp_error_stop_mode,mp->interaction);
} else {
  undump(mp_unspecified_mode,mp_error_stop_mode,x);
}
undump_string(mp->mem_ident);
undump(1,hash_end,mp->bg_loc);
undump(1,hash_end,mp->eg_loc);
undump_int(mp->serial_no);
undump_int(x); 
if (x!=69073) goto OFF_BASE

@ @<Create the |mem_ident|...@>=
{ 
  char *tmp = mp_xmalloc(mp,11,1);
  mp_xfree(mp->mem_ident);
  mp->mem_ident = mp_xmalloc(mp,256,1);
  mp_snprintf(tmp,11,"%04d.%02d.%02d",
          (int)mp_round_unscaled(mp, mp->internal[mp_year]),
          (int)mp_round_unscaled(mp, mp->internal[mp_month]),
          (int)mp_round_unscaled(mp, mp->internal[mp_day]));
  mp_snprintf(mp->mem_ident,256," (mem=%s %s)",mp->job_name, tmp);
  mp_xfree(tmp);
  mp_pack_job_name(mp, ".mem");
  while (! mp_w_open_out(mp, &mp->mem_file) )
    mp_prompt_file_name(mp, "mem file name", ".mem");
  mp_print_nl(mp, "Beginning to dump on file ");
@.Beginning to dump...@>
  mp_print(mp, mp->name_of_file); 
  mp_print_nl(mp, mp->mem_ident);
}

@ @<Close the mem file@>=
(mp->close_file)(mp,mp->mem_file)
