% This file is part of HINT
% Copyright 2017-2021 Martin Ruckert, Hochschule Muenchen, Lothstrasse 64, 80336 Muenchen
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be
% included in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
% WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
% OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
% THE SOFTWARE.
%
% Except as contained in this notice, the name of the copyright holders shall
% not be used in advertising or otherwise to promote the sale, use or other
% dealings in this Software without prior written authorization from the
% copyright holders.

\input ../hint.sty
\input ../changefile.sty

%% defining how to display certain C identifiers
@s uint8_t int
@s uint16_t int
@s uint32_t int
@s uint64_t int
@s int8_t int
@s int16_t int
@s int32_t int
@s float64_t int
@s float32_t int
@s scaled int
@s pointer int
@s memory_word int
@s ASCII_code int
@s small_number int
@s info_t int
@s lig_t int
@s disc_t int
@s math_t int
@s glyph_t int
@s glue_t int
@s rule_t int
@s list_t int
@s kind_t int
@s image_t int
@s glue_ord int
@s four_quarters int
@s eight_bits int
@s internal_font_number int
@s explicit normal
@s str_number int
@s dimen_t int
@s loop else
@s kpse_file_format_type int
@s font_t int


\makeindex
\maketoc

%\makefigindex
\titletrue

\def\lastrevision{${}$Revision: 2515 ${}$}
\def\lastdate{${}$Date: 2021-09-23 17:59:58 +0200 (Thu, 23 Sep 2021) ${}$}
\eject
\input titlepage.tex


\frontmatter

@


\plainsection{Preface}
To be written

\vskip 1cm
\noindent {\it M\"unchen\hfil\break
August 20, 2018 \hfill Martin Ruckert}


\tableofcontent
%\thefigindex


\mainmatter

\section{Introduction}\label{intro}
Hi\TeX\ is a modified version of \TeX\ that replaces the \.{DVI}
(device independent) output format by the \HINT\ format.  The \HINT\ 
format, described in \cite{MR:format}, was designed with two
objectives: being able to support reflowing pages using the \TeX\
typesetting engine and making it simple to use it as output format of
the \TeX\ programm.  The present book tries to convince its readers
that the latter claim is true.  To this end, this book implements a
version of \TeX\ that produces \HINT\ output---to be precise: a short
format \HINT\ file.

The book is written as a literate program\cite{Knuth:lp} using ``The
\.{CWEB} System of Structured Documentation''\cite{Knuth:cweb}.  The
largest part of this program is, of course, \TeX\cite{Knuth:tex}
itself.  Therefore a significant part of Hi\TeX\ consists of
modifications of the \TeX\ source code, which itself is written as a
literate program using ``The \.{WEB} System of Structured
Documentation''\cite{Knuth:WEB}.  The tiny, but significant,
difference between \.{WEB}\index{WEB+{\tt WEB}}
and \.{CWEB}\index{CWEB+{\tt CWEB}} is the transition from
\Pascal\ to \CEE/ as target language. To bridge the gap between
\.{WEB} and \.{CWEB}, Hi\TeX\ is not based on the original \.{WEB}
implementation of \TeX\ but on the \.{CWEB} implementation of \TeX\
as described in~\cite{MR:webtocweb} and~\cite{MR:web2w}. 
Further, it does not use the plain {\tt ctex.w} translation of {\tt tex.web}
but the {\tt ktex.w} file which extends \TeX\ with
 the features of $\epsilon$-\TeX, the file searching
mechanisms of the {\tt kpathsearch} library, and the command line
conventions of \TeX\ Live.

To accomplish modifications of a \.{CWEB} file, the \.{CWEB} tools
support the use of ``change files''.  These change files are lists of
code changes optionally embellished with explanatory text.  Each code
change consists of two parts: a literal copy of the original source
code followed by the replacement text.  When a \.{CWEB} tool applies
such a change file to a cweb file, it will read both files
sequentially and applies the code changes in the order given: Whenever
it finds a section in the \.{CWEB} file that matches the first part of
the current code change, it will replace it by the second part of the
code change, and advance to the next code change.

The present book makes an attempt to present these code changes in
``literate programming style'' and had to overcome two obstacles:
First, in a literate program, the exposition determines the order of
appearance of the code sections; and second, the tools that generate
the documentation from a \.{CWEB} file are not able to generate
documentation for a change file.

The first problem can be solved by using the program {\tt
tie}\cite{kg:tie}\cite{jg:ctie}.  It allows splitting a single change
file into multiple change files, and while within each change file,
the order of changes is still determined by the original cweb file,
changes that belong together can be grouped into separate files.

The second problem is solved by a simple preprocessor, that converts
change files into cweb files which then can be converted into
\TeX. With the help of some modifications of the macros in {\tt
cwebmac.tex} these \TeX\ files are used to present change files in
this book.

Let's look at an examples to explain and illustrate the presentation
of code changes in this book: the {\tt display\_node.ch} change file.

\subsection{The Function |display_node|}
It is the purpose of the {\tt display\_node.ch} change file to make
the \TeX\ code contained in the section ``\<Display node |p|>''
available to other parts of Hi\TeX\ as a function. 
The function is then used to display debugging output.

This requires the following changes:

\changestyle{displaynode.tex}

As you can see, the two parts of a code change are enclosed in square brackets
where the opening top bracket is labeled {\it old\/} or {\it new\/}.
While a full understanding of these code changes requires a look at the original \TeX\ code,
at least they document what was done and why.

There is still one thing missing before we can use the |display_node| 
function in the {\tt hitex.c} file we are about to generate: 
we need an |extern| declaration of |display_node|.
In fact, the Hi\TeX\ program contains a whole bunch of functions 
that will need access to \TeX's internals and the most readable 
and convenient way to gain this access is a header file.

Generating such a header file can be accomplished by using the {\tt -h}
and {\tt -e} options of the {\tt web2w} program when converting the {\tt WEB}
source of \TeX\ to its {\tt cweb} source. We just need to modify the 
process slightly for our purposes.

{
\changestyle{types.tex}
}


\section{Modifying \TeX}\label{modifyingTeX}
In this section, we explain the different change files that modify \TeX.
Larger changes are accomplished by replacing entire functions.

\subsection{The |banner|}
Now that we are about to change the behaviour of \TeX\ significantly,
we have to give the program a new banner.

{
\changestyle{banner.tex}
}

\subsection{The |main| Program}
We add two function calls to \TeX's |main| program: 
|hint_open|, and |hint_close|.

{
\changestyle{main.tex}
}
The functions to open and close the \HINT\ file follow in 
section~\secref{output}. 


\subsection{The Command Line}
Hi\TeX\ has a few additional command line options that we define next.

{
\changestyle{command_line.tex}
}

Above we used the function |hint_debug_help|. It is defined as:

@<Hi\TeX\ routines@>=
#ifdef DEBUG 
void hint_debug_help(void)
{
fprintf(stderr,@/
  "To generate HINT format debug output use the option\n"
  " -debug=XX             "@/
@t\qquad@>"\t XX is a hexadecimal value. OR together these values:\n");@/
fprintf(stderr,"\t\t\t XX=%04X \t basic debugging\n", DBGBASIC);@/
fprintf(stderr,"\t\t\t XX=%04X \t tag debugging\n", DBGTAGS);@/
fprintf(stderr,"\t\t\t XX=%04X \t node debugging\n",DBGNODE);@/
fprintf(stderr,"\t\t\t XX=%04X \t definition debugging\n", DBGDEF);@/
fprintf(stderr,"\t\t\t XX=%04X \t directory debugging\n", DBGDIR);@/
fprintf(stderr,"\t\t\t XX=%04X \t range debugging\n",DBGRANGE);@/
fprintf(stderr,"\t\t\t XX=%04X \t float debugging\n", DBGFLOAT);@/
fprintf(stderr,"\t\t\t XX=%04X \t compression debugging\n", DBGCOMPRESS);@/
fprintf(stderr,"\t\t\t XX=%04X \t buffer debugging\n", DBGBUFFER);@/
fprintf(stderr,"\t\t\t XX=%04X \t TeX debugging\n", DBGTEX);@/
fprintf(stderr,"\t\t\t XX=%04X \t page debugging\n", DBGPAGE);@/
fprintf(stderr,"\t\t\t XX=%04X \t font debugging\n", DBGFONT);@/
exit(0);
}
#endif
@



\subsection{The Page Builder}\label{pagebuilder}
The point where Hi\TeX\ goes an entirely different path than \TeX\ is the page builder:
Instead of building a page, Hi\TeX\ writes a \HINT\ file.

{
\changestyle{build.tex}
}



\subsection{Adding new {\tt whatsit} Nodes}
\TeX\ has a mechanism to extend it: the {\tt whatsit} nodes.
For new concepts like baseline specifications, paragraphs, and boxes with unknown dimensions,
we define now special {\tt whatsit} nodes. The new node definitions are supplemented by
procedures to print them, copy them, delete them, and handle them in various contexts.

{
\changestyle{whatsit.tex}
}

\subsubsection{Freeing}
Because some of the new nodes occur at places where \TeX\ originaly only
handles box nodes,
there are many places in \TeX\ where we can no longer just say |free_node(b, box_node_size)|
to free a (box) node.
Instead, we have to use the more general routine  |flush_node_list|. 
This leads to a long series of simple replacements:

{
\changestyle{free.tex}
}

\subsubsection{Unpacking}
In the function |unpackage| the pointer |p| might now point to an
hset, vset, hpack or vpack node instead of a vbox or hbox node.
We have to adapte the following test to this new situation. 

{
\changestyle{unpackage.tex}
}

We continue to define auxiliar functions to create the new nodes.

\subsubsection{Creating}

The following functions create nodes for paragraphs, displayed equations, baseline skips,
hpack nodes, vpack nodes, hset nodes, vset nodes, and image nodes.

@<Hi\TeX\ routines@>=
pointer new_graf_node(void)
{ @+ pointer p;
  p=get_node(graf_node_size);
  type(p)=whatsit_node;
  subtype(p)=graf_node;
  graf_params(p)=null; 
  graf_list(p)=null; 
  return p;
}


pointer new_disp_node(void)
{ @+  pointer p;
  p=get_node(disp_node_size);
  type(p)=whatsit_node;
  subtype(p)=disp_node;
  display_params(p)=null; 
  display_formula(p)=null;
  display_eqno(p)=null; 
  return p;
}

pointer new_baseline_node(pointer bs, pointer ls, scaled lsl)
{  @+pointer p;
  p=get_node(baseline_node_size);
  type(p)=whatsit_node;
  subtype(p)=baseline_node;
  baseline_node_no(p)=hget_baseline_no(bs, ls, lsl);
  return p;
}

pointer new_pack_node(void)
{  @+pointer p;
  p=get_node(pack_node_size);
  type(p)=whatsit_node;
  subtype(p)=hpack_node;
  width(p)=depth(p)=height(p)=shift_amount(p)=0;
  pack_limit(p)=max_dimen;
  list_ptr(p)=null;
  return p;
}
pointer new_set_node(void)
{  @+pointer p;
  p=get_node(set_node_size);
  type(p)=whatsit_node;
  subtype(p)=hset_node;
  width(p)=depth(p)=height(p)=shift_amount(p)=set_stretch(p)=set_shrink(p)=set_extent(p)=0;
  list_ptr(p)=null;
  return p;
}




pointer new_image_node( str_number n, char *a, char *e)
{ pointer p;
  int i;
  char *fn;
  int l;

  p=get_node(image_node_size);type(p)=whatsit_node;subtype(p)=image_node;
  image_name(p)=n;
  image_area(p)=s_no(a);
  image_ext(p)=s_no(e);
  fn=hfile_name(n,a,e);
#if 0
  fn=kpse_find_tex(fn);
#endif
  i=hnew_file_section(fn);
#if 0
  free(fn);
#endif
  image_no(p)=i;
  image_width(p)=image_height(p)=image_stretch(p)=image_shrink(p)=0; 
  image_shrink_order(p)=image_stretch_order(p)=normal;@/
  return p;
}



@

\subsubsection{Parameter nodes}

Parameter nodes are added to the current list using the |add_par_node| function.
It should be possible to check the parameter values against those
stored in the definition section and remove the ones that
are unchanged. It would make the parameter lists shorter, saving
some time when setting and restoring them later.
There is probably not much savings in memory space, because
most of the times a reference number is found for the parameter list.
@<Create the parameter node@>=
  p=get_node(par_node_size);
  type(p)=whatsit_node;
  subtype(p)=par_node;
  par_type(p)=t;
  par_number(p)=n;
@

@<Initialize the parameter node@>=
  if (t==int_type) par_value(p).i=v;
  else if (t==dimen_type) par_value(p).sc=v;
  else  if (t==glue_type) 
  {@+ par_value(p).i=v;add_glue_ref(par_value(p).i); @+}
  else
  { free_node(p, par_node_size);
    QUIT("Undefined parameter type %d",t);
  }
@

@<Hi\TeX\ routines@>=
void add_par_node(uint8_t t, uint8_t n, int v)
{ @+pointer p;
  @<Create the parameter node@>@;
  @<Initialize the parameter node@>@;
  link(p)=link(temp_head);
  link(temp_head)=p;
}
@


\subsection{Extended Dimensions}
An extended dimension is a linar function of {\tt hsize} and {\tt vsize}, and
whenever \TeX\ works with a dimension, we want it to be able to deal with
an extended dimension. This implies many changes throughout \TeX's sources.
So let's get started.

{
\changestyle{xdimen.tex}
}




\subsection{Hyphenation}
While the breaking of a paragraph into lines must be postponed because
{\tt hsize} is not known, hyphenation should be done as part of Hi\TeX\
because we want to keep hyphenation out of the viewer.  Therefore
Hi\TeX\ will do hyphenation for all words within a paragraph. 

There is a fine point to observe here: \TeX\ will consider a word as
a candidate for automatic hyphenation only if the world ``follows'' after a
glue. (For the exact rules, see the \TeX book\cite{DK:texbook}, Appendix H.)
As a consequence, \TeX\ usually does not submit the first word of a 
paragraph to its hyphenation routine. 
Viewing paragraphs that start with a lengthy word on a narrow display
therefore often look more unsightly than necessary: the long word sticks out
into the right margin as much as it can. To remedy this situation,
Hi\TeX\ has a ``{\tt -f}'' option. If set Hi\TeX\ will deviate from
\TeX's rules and submit the first word of a paragraph to the hyphenation algorithm.

The next problem arises from \TeX's multipass approach to line breaking
and the attempt to have Hi\TeX\ choose exactly the same line breaks as 
\TeX\ does:
\TeX\ distingishes between discretionary breaks inserted by the author of a
text, and discretionary breaks discovered by the hyphenation routine.
The latter, called here ``automatic'', are used only in pass two and three
of the line breaking routine.

The change file that follows below contains mostly three types of changes:
\itemize
\item Hi\TeX\  restricts the |replace_count| to seven bit to make room for an
extra bit to mark automatic discretionary breaks.
Assignments to the replace count are therefore replaced by a macro that
protects the automatic bit.
\item The \TeX\ code for ``\<Try to hyphenate the following word>''
is packaged into a function |hyphenate_word|, so that it can be invoked as needed.
\item And calls of the function |line_break| are replaced by calls to |hline_break|
to have a hook for inserting Hi\TeX specific changes.
\enditemize

{
\changestyle{disc.tex}
}

The function |hline_break| follows:


@<Hi\TeX\ routines@>=
void hline_break(int final_widow_penalty)
{@+ bool auto_breaking; /*is node |cur_p| outside a formula?*/ 
  pointer r, s ; /*miscellaneous nodes of temporary interest*/ 
  pointer pp;
  bool par_shape_fix=false;
  bool first_word=true;
  if (DBGTEX&debugflags)
  { print_ln();print("Before hline_break:\n");
    breadth_max=200;
    depth_threshold=200;
    show_node_list(link(head));print_ln();
  }
  if (dimen_par_hfactor(hsize_code)==0 && dimen_par_vfactor(hsize_code)==0)
  { line_break(final_widow_penalty); /* the easy case */
    return;
  }
  /* Get ready to start line breaking */
  pp=new_graf_node();
  graf_penalty(pp)=final_widow_penalty;
  if(par_shape_ptr==null)
    graf_extent(pp)=new_xdimen(dimen_par(hsize_code),
      dimen_par_hfactor(hsize_code),dimen_par_vfactor(hsize_code));
  else 
    @<fix the use of parshape = 1 indent length@>@;
  link(temp_head)=link(head);
  if (is_char_node(tail))
  { tail_append(new_penalty(inf_penalty))@;
    tail_append(new_param_glue(par_fill_skip_code));
  }
  else if (type(tail)!=whatsit_node || subtype(tail)!=disp_node)
  { if (type(tail)!=glue_node) tail_append(new_penalty(inf_penalty))@;
    else
    {@+type(tail)=penalty_node;delete_glue_ref(glue_ptr(tail));
      flush_node_list(leader_ptr(tail));penalty(tail)=inf_penalty;
    } 
    link(tail)=new_param_glue(par_fill_skip_code);
  }
  DBG(DBGTEX,"\nCalling line_break:\n"
             "hang_indent=0x%08X hang_after=%d",hang_indent,hang_after);
  if (line_skip_limit!=0)
    DBG(DBGTEX," line_skip_limit=0x%08X",line_skip_limit);
  DBG(DBGTEX," prev_graf=0x%08X",prev_graf);

  init_cur_lang=prev_graf%0200000;
  init_l_hyf=prev_graf/020000000;
  init_r_hyf=(prev_graf/0200000)%0100;
  pop_nest();
  DBG(DBGTEX," prev_graf=0x%08X",prev_graf);

  /* Initialize for hyphenating...*/
#ifdef INIT
  if (trie_not_ready) init_trie();
#endif
  cur_lang=init_cur_lang;l_hyf=init_l_hyf;r_hyf=init_r_hyf;
  if (DBGTEX&debugflags)
  { print_ln();print("Before hyphenation:\n");
    breadth_max=200;
    depth_threshold=200;
    show_node_list(link(temp_head));print_ln();
  }
  auto_breaking=true;
  cur_p=temp_head;
  if (option_hyphen_first && is_char_node(link(cur_p))) 
  { hyphenate_word(); first_word=false; }
  cur_p=link(cur_p);
  while (cur_p!=null) 
  { /*Call |try_break| if |cur_p| is a legal breakpoint...*/
    if (is_char_node(cur_p))
	{ /* Advance |cur_p| to the node following the present string...*/
      do { 
        cur_p=link(cur_p);
      } while (is_char_node(cur_p));
	  if (cur_p==null) goto done5; /* mr: no glue and penalty at the end */
    }
    switch (type(cur_p)) 
	{ case whatsit_node:
	    adv_past(cur_p);
		break;
	  case glue_node:
     	if (auto_breaking) /* Try to hyphenate the following word*/
		  hyphenate_word();
        break;
	  case ligature_node: 
        break;
	  case disc_node: 
		/* Try to break after a discretionary fragment...*/
        r=replace_count(cur_p);s=link(cur_p);
        while (r > 0) 
        { decr(r);s=link(s);
        } 
          cur_p=s;
		goto done5;
	  case math_node: 
		auto_breaking=(subtype(cur_p)==after);
        break;
	  default: 
		break;
	}
        if (option_hyphen_first && first_word && is_char_node(link(cur_p))) 
        { hyphenate_word(); first_word=false; }
        cur_p=link(cur_p);
done5:;
  }
  if (DBGTEX&debugflags)
  { print_ln();print("After hline_break:\n");
    breadth_max=200;
    depth_threshold=200;
    show_node_list(link(temp_head));print_ln();
  }
  graf_list(pp)=link(temp_head);
  /* adding parameter nodes */
  link(temp_head)=null;

    add_par_node(int_type,pretolerance_code,pretolerance);
    add_par_node(int_type,tolerance_code,tolerance);
    add_par_node(dimen_type,emergency_stretch_code,emergency_stretch);

    add_par_node(int_type,line_penalty_code,line_penalty);
    add_par_node(int_type,hyphen_penalty_code,hyphen_penalty);
    add_par_node(int_type,ex_hyphen_penalty_code,ex_hyphen_penalty);
    add_par_node(int_type,club_penalty_code,club_penalty);
    add_par_node(int_type,widow_penalty_code,widow_penalty);
    add_par_node(int_type,broken_penalty_code,broken_penalty);
    add_par_node(int_type,inter_line_penalty_code,inter_line_penalty);
    add_par_node(int_type,double_hyphen_demerits_code,double_hyphen_demerits);
    add_par_node(int_type,final_hyphen_demerits_code,final_hyphen_demerits);
    add_par_node(int_type,adj_demerits_code,adj_demerits);
    add_par_node(int_type,looseness_code,looseness);

    if (par_shape_fix)
	{ add_par_node(int_type,hang_after_code,0);
          add_par_node(dimen_type,hang_indent_code,second_indent);
	}
    else
	{ add_par_node(int_type,hang_after_code,hang_after);
	  add_par_node(dimen_type,hang_indent_code, hang_indent);
	}

    add_par_node(dimen_type,line_skip_limit_code,line_skip_limit);
    add_par_node(glue_type,line_skip_code,line_skip);
    add_par_node(glue_type,baseline_skip_code,baseline_skip);

    add_par_node(glue_type,left_skip_code,left_skip);
    add_par_node(glue_type,right_skip_code,right_skip);
    add_par_node(glue_type,par_fill_skip_code,par_fill_skip);


  /* |par_shape| is not yet supported */
  graf_params(pp)=link(temp_head);
  link(temp_head)=null;
  append_to_vlist(pp); 
}
@

Currently Hi\TeX\ dos not implement the parshape feature of \TeX.
The implementation of {\tt \BS list} in \LaTeX\ does however depend
on a simple us of parshape where all lines have the same length
and indentation. We cover this special case be using a hanging
indentation and adjusting the paragraph width by the difference
of the normal {\tt \BS hsize} and the given length.

@<fix the use of parshape = 1 indent length@>=
{ last_special_line= info(par_shape_ptr)-1;
  if (last_special_line!=0)
    DBG(DBGTEX,"Warning parshape with n=%d not yet implemented",info(par_shape_ptr));
  second_width= mem[par_shape_ptr+2*(last_special_line+1)].sc; 
  second_indent= mem[par_shape_ptr+2*last_special_line+1].sc;
  
  graf_extent(pp)=new_xdimen(second_indent+second_width,
                             par_shape_hfactor,par_shape_vfactor);
  second_width=second_width+ round((double)par_shape_hfactor*hhsize/unity
               +(double)par_shape_vfactor*hvsize/unity);
  par_shape_fix=true;
}
@

\subsection{Baseline Skips}

\TeX\ will automatically insert a baseline skip between two boxes in a
vertical list.  The baseline skip is computed to make the distance
between the baselines of the two boxes exactly equal to the value of
{\tt \BS baselineskip}. And if that is not possible, because it would
make the distance between the descenders of the upper box and the
ascenders of the lower box smaller than {\tt \BS lineskiplimit}, it
will insert at least a small amount of glue of size {\tt \BS
lineskip}. As a further complication, if the depth of the upper box
has the special value |ignore_depth| the insertion of a baseline skip
is suppressed.  It is also common practice that authors manipulate the
values of {\tt \BS baselineskip}, {\tt \BS lineskiplimit}, {\tt \BS
lineskip}, and {\tt \BS prevdepth} to change the baseline calculations
of \TeX.

Of course a prerequisite of the whole computation of a baseline skip
is the knowledge of the depth of the upper box and the height of the
lower box. With the new types of boxes introduced by Hi\TeX\ neither
of them might be known. In these cases, the baseline skip computation
must be defered to the viewer by inserting a whatsit node of subtype
|baseline_node|.


{
\changestyle{baseline.tex}
}


\subsection{Displayed Formulas}

\TeX\ enters into math mode when it finds a math shift character ``\$''.
Then it calls |init_math| which checks for an aditional math shift 
character to call either
 \<Go into ordinary math mode> or \<Go into display math mode>.
We remove most of the differences from the latter because we will
treat both cases very similar here and will consider the necessary
differences in the \HINT\ viewer.

When the shift character that terminates math more is encountered,
\TeX\ calls |after_math| which ends with 
either \<Finish math in text> or \<Finish displayed math> .
We need to modify the latter, because positioning a displayed 
equation usually depends on {\tt hsize}, 
and must be postponed until the viewer knows its value.
So all Hi\TeX\ can do in this case is create a new whatsit node 
with subtype |disp_node| and 
insert it proceeding as if the formula had occured in ordinary math mode.


{
\changestyle{display.tex}
}




\subsection{Alignments}


{
\changestyle{align.tex}
}


\subsection{Inserts}
\TeX\ uses |vpack| to determine the natural height of inserted
material. In Hi\TeX, the function |vpack| will not always return
a |vlist_node| with a fixed height because the computation of the
height might fail. In the latter case, |vpack| will return a |vset_node|
or |vpack_node|. Therfore Hi\TeX\ will not use the |height| field of
a an |ins_node|.

As a result we have the following changes:


{
\changestyle{insert.tex}
}




\subsection{Implementing Hi\TeX\ Specific Primitives}\label{primitive}
Now we implement new \TeX\ primitives:
one to include images, a few more to define page teamplates,
and one to test whether the current instance of \TeX\ is
the new Hi\TeX. 
To implement a primitive, we call the |primitive| function. 
Since it needs the name of the new primitive as a \TeX\ string, 
we need to add the strings to the \TeX\ string pool.

Note that primitives are put into \TeX's memory when a \TeX\ format file is read.
So adding or changing the code below implies regenerating the format files.
To generate a format file run a command like 

{\tt\obeylines
hinitex 
**plain \BS dump
}

\noindent
Then find out where the \kpse\ library looks for format files
(for example by running

{\tt\obeylines
 kpsewhich latex.fmt
}

\noindent
and then place the new format file in the same directory.


{
\changestyle{primitive.tex}
}




\section{Hi\TeX}

\subsection{Images}
The handling of images is an integral part of Hi\TeX.  In
section~\secref{imageext} on page~\pageref{imageext} the {\tt image}
primitive was defined.  It requires a filename for the image and
allows the specification, of width, height, stretch and shrink of an
image. If either width or height is not given, Hi\TeX\ tries to
extract this information from the image file itself calling the
function |hget_image_information| with the pointer |p| to the image
node as parameter.

@<Hi\TeX\ routines@>=
void hget_image_information(pointer p)
{ char *fn;
  FILE *f;
  if (image_width(p)!=0 && image_height(p)!=0) return;
  fn=dir[image_no(p)].file_name;
  f=fopen(fn,"rb");
  if (f==NULL) QUIT("Unable to open image file %s.", fn);
  MESSAGE("(%s",fn);
  img_buf_size=0;
  if (!get_BMP_info(f,fn,p)&&!get_PNG_info(f,fn,p)&&!get_JPG_info(f,fn,p))
    QUIT("Unable to obtain width and height information for image %s",fn);
  fclose(f); 
  MESSAGE(" width= %fpt height= %fpt)",image_width(p)/(double)ONE,image_height(p)/(double)ONE);
}
@

When we have found the width and height of the stored image, we can
supplement the information given in the \TeX\ file. Occasionaly, the
image file will not specify the absolute dimensions of the image. In
this case, we can still compute the aspect ratio and supplement either
the width based on the height or vice versa.  This is accomplished by
calling the |set_image_dimensions| function. It returns true on
success and false otherwise.

@<Hi\TeX\ auxiliar routines@>=
static bool set_image_dimensions(pointer p, double w, double h, bool absolute)
{ if (image_width(p)!=0)
  { double aspect=h/w;
    image_height(p)=round(image_width(p)*aspect);
  }
  else if (image_height(p)!=0)
  { double aspect=w/h;
     image_width(p)=round(image_height(p)*aspect);
  }
  else 
  { if (!absolute) return false;
    image_width(p)=round(unity*w);
    image_height(p)=round(unity*h);
  }
  return true;
}
@
We call the following routines with the image buffer partly filled 
with the start of the image file.

@<Hi\TeX\ auxiliar routines@>=
#define IMG_BUF_MAX 54
#define IMG_HEAD_MAX 2
static unsigned char img_buf[IMG_BUF_MAX];
static size_t img_buf_size;
#define @[LittleEndian32(X)@]   (img_buf[(X)]+(img_buf[(X)+1]<<8)+\
                                (img_buf[(X)+2]<<16)+(img_buf[(X)+3]<<24))

#define @[BigEndian16(X)@]   (img_buf[(X)+1]+(img_buf[(X)]<<8))

#define @[BigEndian32(X)@]   (img_buf[(X)+3]+(img_buf[(X)+2]<<8)+\
                                (img_buf[(X)+1]<<16)+(img_buf[(X)]<<24))

#define Match2(X,A,B)  ((img_buf[(X)]==(A)) && (img_buf[(X)+1]==(B)))
#define Match4(X,A,B,C,D)  (Match2(X,A,B)&&Match2((X)+2,C,D))

#define @[GET_IMG_BUF(X)@] \
if (img_buf_size<X) \
  { size_t i=fread(img_buf+img_buf_size,1,(X)-img_buf_size,f); \
    if (i<0) QUIT("Unable to read image %s",fn); \
    else if (i==0) QUIT("Unable to read image header %s",fn); \
    else img_buf_size+=i; \
  }
@

\subsubsection{BMP}
We start with Windows Bitmaps. A Windows bitmap file usually has the extension {\tt .bmp}
but the better way to check for a Windows bitmap file ist to examine the first two byte
of the file: the ASCII codes for `B' and `M'. 
Once we have verified the file type, we find the width and height of the bitmap in pixels
at offsets |0x12| and |0x16| stored as little-endian 32 bit integers. At offsets |0x26| and |0x2A|,
we find the horizontal and vertical resolution in pixel per meter stored in the same format.
This is sufficient to compute the true width and height of the image in scaled points.
If either the width or the height was given in the \TeX\ file, we just compute the aspect ratio
and compute the missing value.

The Windows Bitmap format is easy to process but not very efficient. So the support for this
format in Hi\TeX\ is deprecated and will disappear. You should use one of the formats described next.

@<Hi\TeX\ auxiliar routines@>=
static bool get_BMP_info(FILE *f, char *fn, pointer p)
{ double w,h;
  double xppm,yppm;
  GET_IMG_BUF(2);
  if (!Match2(0,'B','M')) return false;
  GET_IMG_BUF(0x2E);
  w=(double)LittleEndian32(0x12); /*width in pixel*/
  h=(double)LittleEndian32(0x16); /*height in pixel*/
  xppm=(double)LittleEndian32(0x26); /* horizontal pixel per meter*/
  yppm=(double)LittleEndian32(0x2A); /* vertical pixel per meter*/
  return set_image_dimensions(p,(72.27*1000.0/25.4)*w/xppm,(72.27*1000.0/25.4)*h/yppm,true);
}
@

\subsubsection{PNG}
Now we repeat this process for image files in using the Portable Network Graphics\cite{png2nd} file
format. This file format is well suited to simple graphics that do not use color gradients.
These images usually have the extension {\tt .png} and start with an eight byte signature:
|0x89| followed by the ASCII Codes `P', `N', `G', followd by a carriage return (|0x0D| and line feed (|0x0A|),
an DOS end-of-file character (|0x1A|) and final line feed (|0x0A|).
After the signature follows a list of chunks. The first chunk is the image header chunk.
Each chunk starts with the size of the chunk stored as big-endian 32 bit integer, followed by the chunk name 
stored as four ASCII codes  followed by the chunk data and a CRC. The size as stored in the chunk 
does not include the size itself, the name or the CRC.
The first chunk is the IHDR chunk.
The chunk data of the IHDR chunk starts with the width and the height of the image in pixels
stored as 32 bit big-endian integers.

Finding the image resolution takes some more effort. The image resolution is stored in an optional chunk
named ``pHYs'' for the physical pixel dimensions. 
All we know is that this chunk, if it exists, will appear after the IHDR
chunk and before the (required) IDAT chunk. The pHYs chunk contains two 32 bit big-endian integers,
giving the horizontal and vertical pixels per unit, and a one byte unit specifier, which is either 0
for an undefined unit or 1 for the meter as unit. With an undefined unit only the aspect ratio
of the pixels anh hence the aspect ratio of the image can be determined.


@<Hi\TeX\ auxiliar routines@>=

static bool get_PNG_info(FILE *f, char *fn, pointer p)
{ int pos, size;
  double w,h;
  double xppu,yppu;
  int unit;
  GET_IMG_BUF(24);
  if (!Match4(0, 0x89, 'P', 'N', 'G') ||
      !Match4(4, 0x0D, 0x0A, 0x1A, 0x0A)) return false;
  size=BigEndian32(8);
  if (!Match4(12,'I', 'H', 'D', 'R')) return false;
  w=(double)BigEndian32(16);
  h=(double)BigEndian32(20);
  pos=20+size;
  while (true)
  { if (fseek(f,pos,SEEK_SET)!=0) return false;
    img_buf_size=0;
    GET_IMG_BUF(17);
    size=BigEndian32(0);
    if (Match4(4,'p', 'H', 'Y', 's'))
    { xppu =(double)BigEndian32(8);  
      yppu =(double)BigEndian32(12);
      unit=img_buf[16];
      if (unit==0)
        return set_image_dimensions(p,w/xppu,h/yppu,false);
      else if (unit==1)
        return set_image_dimensions(p,(72.27/0.0254)*w/xppu,(72.27/0.0254)*h/yppu,true);
      else
        return false;
    }
    else if  (Match4(4,'I', 'D', 'A', 'T'))
      return set_image_dimensions(p,w,h,false);
    else
      pos=pos+12+size;
  }
  return false;
}
@

\subsubsection{JPG}
For photographs, the JPEG File Interchange Format (JFIF)\cite{Ham92}\cite{JPEG93} is more appropriate.
JPEG files come with all sorts of file extensions like {\tt .jpg}, {\tt .jpeg}, or {\tt .jfif}.
We check the file siganture: it starts with the the SOI (Start of Image) marker |0xFF|, |0xD8|
followed by the JIFI-Tag. The JIFI-Tag starts with the segment marker APP0 (|0xFF|, |0xE0|) followed by the 
2 byte segment size, followed by the ASCII codes `J', `F', `I', `F' followed by a zero byte.
Next is a two byte version number which we do not read. 
Before the resolution proper there is a resolution unit indicator byte (0 = no units,
1 = dots per inch, 2 = dots per cm) and then comes the horizontal and vertical resolution both
as 16 Bit big-endian integers.
To find the actual width and height, we have to search for a start of frame marker (|0xFF|, |0xC0|+$n$ with $0\le n\le 15$). Which is followed by the 2 byte segment size, the 1 byte sample precission, the
2 byte height and the 2 byte width.


@<Hi\TeX\ auxiliar routines@>=

static bool get_JPG_info(FILE *f, char *fn, pointer p)
{ int pos, size;
  double w,h;
  double xppu,yppu;
  int unit;
  GET_IMG_BUF(18);

  if (!Match4(0, 0xFF,0xD8, 0xFF, 0xE0)) return false;
  size=BigEndian16(4);
  if (!Match4(6,'J', 'F', 'I', 'F')) return false;
  if (img_buf[10] != 0) return false; 
  unit=img_buf[13];
  xppu=(double)BigEndian16(14);
  yppu=(double)BigEndian16(16);
  pos=4+size;
  while (true)
  { if (fseek(f,pos,SEEK_SET)!=0) return false;
    img_buf_size=0;
    GET_IMG_BUF(10);
    if (img_buf[0] != 0xFF) return false; /* Not the start of a segment */
    if ( (img_buf[1]&0xF0) == 0xC0) /* Start of Frame */
    { h =(double)BigEndian16(5);  
      w =(double)BigEndian16(7);
      if (unit==0)
        return set_image_dimensions(p,w/xppu,h/yppu,false);
      else if (unit==1)
        return set_image_dimensions(p,72.27*w/xppu,72.27*h/yppu,true);
      else if (unit==2)
        return set_image_dimensions(p,(72.27/2.54)*w/xppu,(72.27/2.54)*h/yppu,true);
      else
        return false;
    }
    else
    { size=  BigEndian16(2);
      pos=pos+2+size;
    }
  }
  return false;
}
@

\subsubsection{SVG}
There is still one image format missing: scalable vector graphics.
In the moment, I tend not to include a further image format into
the definition of the \HINT\ file format but instead use the
PostScript subset that is used for Type 1 fonts to encode
vector graphics. Any \HINT\ viewer must support Type 1
PostScript fonts and hence it has already the necessary interpreter.
So it seems reasonable to put the burden of converting vector graphics
into a Type 1 PostScript font on the generator of \HINT\ files
and keep the \HINT\ viewer as small and simple as possible.


\subsection{Links, Labels, and Outlines}
The \HINT\ format knows about labels, links, and outlines.
When generating a short format \HINT\ file, links are part of
the content section, where as labels and outlines are found in
the definition section. Because labels are defined while
writing the content section, the writing of labels and outlines, which
reference the labels, must be postponed. For that reason,
we store information about labels and outlines in dynamic arrays,
and map labels, which are identified by a name or a number,
to their index using a dynamic hash table.

We start with two functions that allocate new entries in the
dynamic arrays increasing their size if necessary.
@<Hi\TeX\ auxiliar routines@>=
static int next_label(void)
{ static int label_no=-1;
  static int labels_allocated =0;
  label_no++;
  if (label_no>0xFFFF)
   overflow("labels",0xFFFF);
  if (label_no>=labels_allocated)
  { if (labels_allocated==0) 
    { labels_allocated=32; ALLOCATE(labels,labels_allocated,label_t); }
    else RESIZE(labels,labels_allocated,label_t);
  }
  max_ref[label_kind]=label_no;
  return label_no;
}

static int next_outline(void)
{ static int outlines_allocated =0;
  static int outline_no=-1;
  outline_no++;
  if (outline_no>0xFFFF)
   overflow("outlines",0xFFFF);
  if (outline_no>=outlines_allocated)
  { if (outlines_allocated==0) 
    { outlines_allocated=32; ALLOCATE(outlines,outlines_allocated,outline_t); }
    else RESIZE(outlines,outlines_allocated,outline_t);
  }
  max_outline=outline_no;
  return outline_no;
}
@

While processing the content nodes, access to the labels is provided either
by name or by number through a hash table. We store tabel entries in linked
lists starting with a reasonably sized table of pointers. This keeps
the fixed costs low and guards against overflow and rapidly increasing
inefficiency. We start with a function to insert a new entry into
the hash table.

@<Hi\TeX\ auxiliar routines@>=
typedef struct hash_entry 
{int num; char *nom; uint16_t n; struct hash_entry *next;} hash_entry_t;
#define LABEL_HASH 1009 /* MIX a prime number */
static hash_entry_t *label_hash[LABEL_HASH]={NULL};

static int insert_hash(int h, int num, char *nom)
{ hash_entry_t *e;
  ALLOCATE(e,1,hash_entry_t);
  e->n= next_label();
  if (nom!=NULL) e->nom=strdup(nom);
  else e->num=num;
  e->next= label_hash[h];
  label_hash[h]=e;
  if (e->nom!=NULL)
    DBG(DBGLABEL,"Creating new label *%d: name='%s'\n",e->n,e->nom);
  else
    DBG(DBGLABEL,"Creating new label *%d: num=%d\n",e->n,e->num); 
  return e->n;
}
@

There are two cases: finding a label by name or by number.
We start with the simpler case where the number is given.
The process is straigth forward:

@<Hi\TeX\ auxiliar routines@>=
static int find_label_by_number(int p)
{ unsigned int h=(unsigned int)p%LABEL_HASH;
  hash_entry_t *e= label_hash[h];
  while (e!=NULL)
    if (e->nom==NULL && e->num==p) return e->n;
    else e=e->next;
  return insert_hash(h,p,NULL);
}
@

To look up a label by its name as given by a token list,
we prepare ourselfs by implementing two functions:
one to extract the character codes from the token list
forming the ``name''
and one to compute the hash value for a name.
The routine to find the label by name is then equivalent to the
routine we have just seen. Given a pointer |p| to
either a label, a link, or an outline node, the  last function
returns the correct label reference.
Currently, we limit label names to at most 255 byte not counting the
zero byte.

@<Hi\TeX\ auxiliar routines@>=
static char *tokens_to_name(pointer p)
{ static char s[256]; 
  int i=0;
  bool skip_space=0;
  while (i<255 && p!=0)@/
  { int m = info(p)/0400;@+
    int c = info(p)%0400;
    if (m==spacer && ! skip_space) @/
    { s[i++]=' '; skip_space=true;@+}
    else if ((m==letter || m==other_char) && ' '< c && c < 0x7F)@/ 
    {@+ s[i++]=c; skip_space=false;@+}
    p=link(p);
  }
  s[i]=0;
  return s;
}

static unsigned int name_hash(char *s)
{ unsigned int h=0;
  while (*s!=0)
    h=(h<<2)+*(s++); 
  return h;
}

static int find_label_by_name(pointer p)
{ char *s=tokens_to_name(link(p));
  unsigned int h=name_hash(s)%LABEL_HASH;
  hash_entry_t *e= label_hash[h];
  while (e!=NULL)
    if (e->nom!=NULL && strcmp(e->nom,s)==0) return e->n;
    else e=e->next;
  return insert_hash(h,0,s);
}
@

We combine both ways of finding a label reference in the following function:

@<Hi\TeX\ auxiliar routines@>=
static int find_label(pointer p)
{@+ if (label_has_name(p)) return find_label_by_name(label_ptr(p));
  else return find_label_by_number(label_ptr(p)); 
}
@
  
After these preparations, we can implement the functions needed
when labels, links, and outlines are delivered to the page builder.

We start with looking at the labels:
When a labels is defined, the current position is recorded. 
Further labels are linked together in order of descending positions,
to allow the efficient adjustment of label positions when
moving lists.

@<Hi\TeX\ auxiliar routines@>=
static void new_label(pointer p)
{ int n=find_label(p);
  if (n!=zero_label_no && labels[n].where!=LABEL_UNDEF)
  { MESSAGE("WARNING: Ignoring duplicate definition of label ");
    if (label_has_name(p)) MESSAGE("name %s\n",tokens_to_name(link(label_ptr(p))));
    else  MESSAGE("num %d\n",label_ptr(p));
  }
  else
  { labels[n].where=label_where(p);
    labels[n].pos=hpos-hstart;
    labels[n].pos0=hpos0-hstart;
    labels[n].next=first_label;
    first_label=n;
    DBG(DBGLABEL,"Defining label *%d: pos=0x%x\n",n,labels[n].pos);
  }
}
@

When a link node is written to the output, we can check
that start links and end links properly match.

@<Hi\TeX\ auxiliar routines@>=
static int last_link=-1;
static int new_start_link(pointer p)
{ int n=find_label(p);
  if (last_link>=0)
    fatal_error("Missing end link before start link");
  labels[n].used=true;
  last_link=n;
  DBG(DBGLABEL,"New link to label *%d\n",n);
  return n;
}

static int new_end_link(void)
{ int n;
  if (last_link<0)
    fatal_error("Missing start link before end link");
  n=last_link;
  last_link=-1;
  return n;
}
@

For outline nodes, we use the next two functions.
The node list representing the title can be an arbitrary
list in horizontal mode.
In general, the front end should be able to render such a
horizontal list, but at least it should be able to extract
the UTF8 characters codes and display those.

@<Hi\TeX\ auxiliar routines@>=
static void new_outline(pointer p)
{ int r=find_label(p);
  int m=next_outline();
  list_t l;
  uint32_t pos;
  pos=hpos-hstart;
  l.k=list_kind; /* this eventually should be |text_kind| */
  hout_list_node(outline_ptr(p),pos,&l);
  hset_outline(m,r,outline_depth(p),pos);
  DBG(DBGLABEL,"New outline for label *%d\n",r);
}
@

One last function is needed, which better schould be in the {\tt htex.w}
file. It is called when the |outline_group| ends that was started
when scanning the {\tt\BS HINToutline} primitive.
@<Hi\TeX\ routines@>=
void hfinish_outline_group(void)
{ pointer s=link(head);
  unsave();
  pop_nest();
  outline_ptr(tail)=s;
} 
@

\subsection{The New Page Builder}\label{buildpage}
Here is the new |build_page| routine of Hi\TeX:

@<Hi\TeX\ routines@>=
void build_page(void)
{ static bool initial=true;
  if(link(contrib_head)==null||output_active)return;
  do 
  { pointer p= link(contrib_head);
    pointer q=null; /* for output nodes */
    pointer *t; /*the tail of the output nodes*/
    @<Record the bottom mark@>@;
    @<Suppress empty pages if requested@>@;
    link(contrib_head)= link(p);link(p)= null;
    if (link(contrib_head)==null)
    { if(nest_ptr==0) tail= contrib_head;
      else contrib_tail= contrib_head;
    }
    update_last_values(p);
    @<Freeze the page specs if called for@>@;
    page_goal=0x3fffffff; /* maximum dimension */
    t=collect_output(&p,&q);    
    if (p!=null) 
    { hpos0=hpos; hout_node(p); }
recycle_p:
    flush_node_list(p);
    if (q!=null)
      @<Fire up the output routine for |q|@>@; 
  } while(link(contrib_head)!=null);
  DBG(DBGBUFFER,"after build page dyn_used= %d\n", dyn_used);
}
@

@<Freeze the page specs if called for@>=
if (page_contents<box_there) 
{ switch(type(p))
  { case whatsit_node:
    if (subtype(p)==baseline_node) goto recycle_p;
    else if (subtype(p)!=hset_node && subtype(p)!=vset_node &&
	subtype(p)!=hpack_node && subtype(p)!=vpack_node &&
        subtype(p)!=graf_node &&  subtype(p)!=disp_node &&
	subtype(p)!=image_node && subtype(p)!=align_node)
        break; /* else fall through */
    case hlist_node: case vlist_node: case rule_node: 
      if (page_contents==empty)
      { freeze_page_specs(box_there);
        hfix_defaults(); 
      }
      else page_contents=box_there;
      break;
    case ins_node:
      if (page_contents==empty)
      { freeze_page_specs(inserts_only);
        hfix_defaults(); 
      }
      break;
    case kern_node:
    case penalty_node:
    case glue_node: goto recycle_p;
    default:
      break;
  }
}
@

Users of \TeX\ often force the generation of empty pages for example to start
a new chapter on a right hand page with an odd page number. 
This makes sense for a printed book but not for a screen reader where
there are no page numbers nor right or left hand pages.
Using a screen reader, empty pages are just annoying.
The common way to achive an empty page is the use of {\tt \BS eject}
followed by a an empty box, a fill glue, and another  {\tt \BS eject}.

The following code tries to detect such a sequence of nodes and will eliminate
them if requested. To do so, we delay the output of nodes after
an eject penalty until either something gets printed on the page or
another eject penalty comes along. To override the delayed output,
a penalty less or equal to a double |eject_penalty| can be used.
The function |its_all_over| (see section~\secref{itsallover}) 
is an example for such a use.


@<Suppress empty pages if requested@>=
if (option_no_empty_page &&
    ((type(p)==penalty_node && 
      penalty(p)<=eject_penalty && penalty(p)>2*(eject_penalty)) ||
     (page_contents==empty && !is_visible(p)))) 
{ pointer q= link(p);  
  while (true)
  { if (q==null) return;
    else if (is_visible(q)) break;
    else if (type(q)==penalty_node && penalty(q)<=eject_penalty)
    { while (p!=q)
      { pointer r=p;
        DBG(DBGPAGE,"Eliminating node (%d,%d)\n", 
          type(p), type(p)==penalty_node?penalty(p):subtype(p));
        p=link(p);
        link(r)=null;
        flush_node_list(r);
      }
      link(contrib_head)= p;
      DBG(DBGPAGE,"Eliminating empty page done\n");
      if (penalty(q)<=2*(eject_penalty)) break; 
    }
    q=link(q);
  }
}   
@
It remains to test a node for visibility. This is a quick (and dirty) test
because the test will not look inside boxes; it simply tests whether 
the list pointer is |null|. We consider an |open_node|, |write_node|,
|close_node|, |label_node|, or |outline_node| a as visible, 
because deleting them could cause unwanted
side effects. Possibly it would be better to regard them as invisible,
but still pass them on to the rest of the output routine.
 
@<Hi\TeX\ auxiliar routines@>=
static bool is_visible(pointer p)
{ switch (type(p))
  { case penalty_node: 
    case kern_node:
    case glue_node:
    case mark_node:
      return false;
    case ins_node:
      return ins_ptr(p)!=null;
    case adjust_node:
      return adjust_ptr(p)!=null;
    case hlist_node:
    case vlist_node:
      return list_ptr(p)!=null;
    case whatsit_node:
      if (subtype(p)==image_node || subtype(p)==align_node || subtype(p)==disp_node ||
          subtype(p)==open_node ||subtype(p)==write_node ||subtype(p)==close_node ||
          subtype(p)==label_node || subtype(p)==outline_node )
        return true;
      else if (subtype(p)==hset_node || subtype(p)==vset_node ||
	       subtype(p)==hpack_node || subtype(p)==vpack_node)
        return list_ptr(p)!=null;
      else if (subtype(p)==graf_node)
        return graf_list(p)!=null;
      else
        return false;
    default: return true;
  }
}
@

An important feature of the new routine is the call to
|hfix_defaults|.  It occurs when the first ``visible mark'' is placed
on the page. At that point we record the current values of \TeX's
parameters which we will use to generate the definition section of the
\HINT\ file.  It is still possible to specify alternative values for
these parameters by using parameter lists but only at an additional
cost in space and time.

Furthermore, this is the point where we freeze the definition of
|hsize| and |vsize|. The current values will be regarded as the sizes
as recommended by the author.

From then on |hsize| and |vsize| are replaced by the equivalent
extended dimensions and any attempt to modify them on the global level
will be ignored. |hhsize| and |hvsize| will contain the sizes that a
regular \TeX\ engine would use.
 
We also compute the total page size from the page template defined
last.


@<Compute the page size@>=
{ pointer p;
  p=link(setpage_head);
  if (p==null)
  { scaled margin;
    if (hhsize<hvsize) margin=hhsize; else margin=hvsize;
    margin = margin/6 -6*unity;
    if (margin<0) margin=0;
    page_h=hhsize+2*margin;
    page_v=hvsize+2*margin;
  }
  else
  { pointer x;
    x=setpage_height(p);
    page_v=xdimen_width(x)
     +round(((double)xdimen_hfactor(x)*hhsize+(double)xdimen_vfactor(x)*hvsize)/unity);
    x=setpage_width(p);
    page_h=xdimen_width(x)
     +round(((double)xdimen_hfactor(x)*hhsize+(double)xdimen_vfactor(x)*hvsize)/unity);
  }
}
@

@<Hi\TeX\ variables@>=
static scaled page_h, page_v;
@

@<Switch |hsize| and |vsize| to extended dimensions@>=
  hsize=0; vsize=0;
  dimen_par_hfactor(hsize_code)= unity;
  dimen_par_vfactor(vsize_code)= unity;
@

There is one point where we can not simpy forego the 
output routine: \.{\\write} commands. Unless the \.{\\write} is
decorated with an \.{\\immediate}, the whatsit node generated from it
will lay dormant in the contribution list and then the page until
the output routine passes it inside the finished page to the |ship_out|
routine where it will come to life and write its token list out.
The whatsit nodes from \.{\\openout} and \.{\\closeout} commands
behave similary.

It is not possible to ignore the output routine
because the output routine may change the environment in which the
token list of a \.{\\write} will be expanded. 
For example \LaTeX\ redefines \.{\\protect} to be \.{\\noexpand}.
As a consequence we have to implement a simplified version
of \TeX's usual process to fire up the output routine.

The |collect_output| routine takes a node list |p|,
removes the output nodes and appends them to |q|, with |q|
always pointing to the tail pointer.

@<Hi\TeX\ auxiliar routines@>=
static pointer *collect_output(pointer *p, pointer *q)
{ while (*p!=null)
  { @<Collect output nodes from |*p|@>@;
    p=&(link(*p));
  }
  return q;
}
@

\TeX\ does not permit output nodes in leaders, so we dont check them;
further we do not check the pre- and post-break lists of 
discretionary breaks.

@<Collect output nodes from |*p|@>=
if (!is_char_node(*p))
{ pointer r=*p;
  switch (type(r))
  { case whatsit_node:
      switch (subtype(r))  
      { case open_node: case write_node: case close_node:
        { *p=link(r); link(r)=null; *q=r; q=&(link(r)); 
          if (*p==null) return q;
        }
          break;
        case graf_node: q=collect_output(&graf_list(r),q); 
          break;
        case disp_node:
          if (display_left(r)) q=collect_output(&display_eqno(r),q); 
          q=collect_output(&display_formula(r),q);
          if (!display_left(r)) q=collect_output(&display_eqno(r),q); 
          break;
        case hset_node: case vset_node: case hpack_node: case vpack_node:
          q=collect_output(&list_ptr(r),q);
          break;
        case align_node:
          q=collect_output(&align_list(r),q);
          break;
        default: break;
      }
      break;
    case hlist_node: case vlist_node:
      q=collect_output(&list_ptr(r),q);
      break;
    case ins_node:
      q=collect_output(&ins_ptr(r),q);
      break;
    case adjust_node:
      q=collect_output(&adjust_ptr(r),q);
      break;
    default: break;
  }
}
@



@<Fire up the output routine for |q|@>=
{ pointer r=new_null_box();type(r)=vlist_node;
  subtype(r)=0;shift_amount(r)=0;height(r)=hvsize;
  list_ptr(r)=q;
  *t=new_glue(ss_glue);
  flush_node_list(box(255)); /* just in case \dots */
  box(255)=r;
  if (output_routine!=null)
  {@+output_active=true;
    if (bot_mark!=null)
    {@+if (top_mark!=null) delete_token_ref(top_mark);
      top_mark=bot_mark;add_token_ref(top_mark);
      if (first_mark!=null) delete_token_ref(first_mark);
      first_mark=bot_mark;add_token_ref(first_mark);
    }
    push_nest();mode=-vmode;prev_depth=ignore_depth;mode_line=-line;
    begin_token_list(output_routine, output_text);
    new_save_level(output_group);normal_paragraph();
    scan_left_brace();
    return;
  }
  else
  {
    ship_out(box(255)); box(255)=null;
  }
}
@

The |ship_out| routine just calls |execute_output|.
Because the output routine might have added plenty
of decorations around the list of output nodes,
we have to find them again.

@<Hi\TeX\ routines@>=
void execute_output(pointer p)
{@+while (p!=null)
  { @<Execute output nodes from |p|@>@;
    p=link(p);
  }
}
@

@<Execute output nodes from |p|@>=
if (!is_char_node(p))
  switch (type(p))
  { case whatsit_node:
      switch (subtype(p))  
      { case open_node: case write_node: case close_node:
          out_what(p);
          break;
        case graf_node: execute_output(graf_list(p)); 
          break;
        case disp_node:
          if (display_left(p)) execute_output(display_eqno(p)); 
          execute_output(display_formula(p));
          if (!display_left(p)) execute_output(display_eqno(p)); 
          break;
        case hset_node: case vset_node: case hpack_node: case vpack_node:
          execute_output(list_ptr(p));
          break;
        case align_node:
          execute_output(align_list(p));
          break;
        default: break;
      }
      break;
    case hlist_node: case vlist_node:
      execute_output(list_ptr(p));
      break;
    case ins_node:
      execute_output(ins_ptr(p));
      break;
    case adjust_node:
      execute_output(adjust_ptr(p));
      break;
    default: break;
  }
@

Invoking the user's output routine is a risky endeavor
if marks are not initialized properly. In our case
we will have always |top_mark| equal to |first_mark| and
|bot_mark|.

@<Record the bottom mark@>=
if (type(p)==mark_node)
{ if (bot_mark!=null) delete_token_ref(bot_mark);
  bot_mark=mark_ptr(p);add_token_ref(bot_mark);
}
@


\subsection{Replacing {\tt hpack} and {\tt vpack}}


@<Hi\TeX\ routines@>=

pointer hpack(pointer p,scaled w, scaled hf, scaled vf, small_number m)
{
  pointer r; /*the box node that will be returned*/ 
  pointer prev_p; /*trails behind |p|*/
  scaled h,d,x; /*height, depth, and natural width*/ 
  scaled s; /*shift amount*/ 
  pointer g; /*points to a glue specification*/ 
  glue_ord sto, sho; /*order of infinity*/ 
  internal_font_number f; /*the font in a |char_node|*/ 
  four_quarters i;  /*font information about a |char_node|*/ 
  eight_bits hd; /*height and depth indices for a character*/ 
  last_badness= 0;r= get_node(box_node_size);type(r)= hlist_node;
  subtype(r)= min_quarterword;shift_amount(r)= 0;
  prev_p= r+list_offset;link(prev_p)= p;
  h= 0; d= 0; x= 0;
  total_stretch[normal]= 0;total_shrink[normal]= 0;
  total_stretch[fil]= 0;total_shrink[fil]= 0;
  total_stretch[fill]= 0;total_shrink[fill]= 0;
  total_stretch[filll]= 0;total_shrink[filll]= 0;
  while(p!=null)
    {
    reswitch:
      while(is_char_node(p))
	{ f= font(p);i= char_info(f, character(p));hd= height_depth(i);
	  x= x+char_width(f, i);
	  s= char_height(f, hd);if (s> h)h= s;
	  s= char_depth(f, hd);if (s> d)d= s;
	  p= link(p);
	}
      if (p!=null)
	{ switch(type(p)){
	  case hlist_node:case vlist_node:case rule_node:case unset_node:
	    { x= x+width(p);
	      if (type(p)>=rule_node)s= 0;else s= shift_amount(p);
	      if (height(p)-s> h)h= height(p)-s;
	      if (depth(p)+s> d)d= depth(p)+s;
	    }
	    break;
	  case ins_node:case mark_node:case adjust_node:if (adjust_tail!=null)
	      { while(link(prev_p)!=p)prev_p= link(prev_p);
		if (type(p)==adjust_node)
                  { link(adjust_tail)= adjust_ptr(p);
		    while(link(adjust_tail)!=null)adjust_tail= link(adjust_tail);
		    p= link(p);free_node(link(prev_p),small_node_size);
		  }
		else
		  { link(adjust_tail)= p;adjust_tail= p;p= link(p);
		  }
		link(prev_p)= p;p= prev_p;
	      }
	    break;
	  case whatsit_node:
            if (subtype(p)==graf_node)
			  goto repack;
			else if (subtype(p)==disp_node )
			  goto repack;
			else if (subtype(p)==vpack_node )
			  goto repack;
			else if (subtype(p)==hpack_node )
			  goto repack;
			else if (subtype(p)==hset_node )
			  goto repack;
			else if (subtype(p)==vset_node )
			  goto repack;
			else if (subtype(p)==stream_node )
			  goto repack;
			else if (subtype(p)==image_node)
			{ glue_ord o;
			  if (image_height(p)> h) h= image_height(p);
                          x= x+image_width(p);
			  o= image_stretch_order(p);total_stretch[o]= total_stretch[o]+image_stretch(p);
	                  o= image_shrink_order(p);total_shrink[o]= total_shrink[o]+image_shrink(p);
			}
            break;
		break;
	  case glue_node:
	    { glue_ord o;
		  g= glue_ptr(p);x= x+width(g);
	      o= stretch_order(g);total_stretch[o]= total_stretch[o]+stretch(g);
	      o= shrink_order(g);total_shrink[o]= total_shrink[o]+shrink(g);
	      if (subtype(p)>=a_leaders)
		{ g= leader_ptr(p);
		  if (height(g)> h)h= height(g);
		  if (depth(g)> d)d= depth(g);
		}
	    }
	    break;
	  case kern_node:case math_node:x= x+width(p);break;
	  case ligature_node:
	    { mem[lig_trick]= mem[lig_char(p)];link(lig_trick)= link(p);
	      p= lig_trick;goto reswitch;
	    }
	  default:do_nothing;
	  }
	  p= link(p);
	}
    }
  if (adjust_tail!=null) link(adjust_tail)= null;
  height(r)= h;depth(r)= d;
      if (total_stretch[filll]!=0)sto= filll;
      else if (total_stretch[fill]!=0)sto= fill;
      else if (total_stretch[fil]!=0)sto= fil;
      else sto= normal;
 
	  if (total_shrink[filll]!=0)sho= filll;
      else if (total_shrink[fill]!=0)sho= fill;
      else if (total_shrink[fil]!=0)sho= fil;
      else sho= normal;

  if (hf!=0 || vf!=0 )  /* convert to a hset node */
	{ pointer q;
	  q=new_set_node();
	  subtype(q)=hset_node;
      height(q)=h;
	  depth(q)=d;
	  width(q)=x; /* the natural width */
	  shift_amount(q)=shift_amount(r);
	  list_ptr(q)=list_ptr(r);
	  list_ptr(r)=null;
      free_node(r, box_node_size);
      if (m==exactly)
	    set_extent(q)=new_xdimen(w,hf,vf);
	  else
	    set_extent(q)=new_xdimen(x+w,hf,vf);
      set_stretch_order(q)=sto;
      set_shrink_order(q)=sho;
      set_stretch(q)=total_stretch[sto];
      set_shrink(q)=total_shrink[sho];
	  return q;
	}




 if (m==additional) w= x+w;
 width(r)= w;x= w-x; /*now |x| is the excess to be made up*/ 
    
  if (x==0)
    { glue_sign(r)= normal; glue_order(r)= normal;
      set_glue_ratio_zero(glue_set(r));
      goto end;
    }
  else if (x> 0)
    {
      glue_order(r)= sto;glue_sign(r)= stretching;
      if (total_stretch[sto]!=0)glue_set(r)= unfloat(x/(double)total_stretch[sto]);
      else
	{ glue_sign(r)= normal;
	  set_glue_ratio_zero(glue_set(r));
	}
      if (sto==normal)
	{ if (list_ptr(r)!=null)
	    { last_badness= badness(x,total_stretch[normal]);
	      if (last_badness> hbadness)
		{ print_ln();
		  if (last_badness> 100)
		    print_nl("Underfull");else print_nl("Loose");
		  print(" \\hbox (badness ");print_int(last_badness);
		  goto common_ending;
		}
	    }
	}
      goto end;
    }
  else
    {
      glue_order(r)= sho;glue_sign(r)= shrinking;
      if (total_shrink[sho]!=0)
	glue_set(r)= unfloat((-x)/(double)total_shrink[sho]);
      else
	{ glue_sign(r)= normal;
	  set_glue_ratio_zero(glue_set(r));
	}
      if ((total_shrink[sho]<-x)&&(sho==normal)&&(list_ptr(r)!=null))
	{ last_badness= 1000000;
	  set_glue_ratio_one(glue_set(r));
	  if ((-x-total_shrink[normal]> hfuzz)||(hbadness<100))
	    { if ((overfull_rule> 0)&&(-x-total_shrink[normal]> hfuzz))
		{ while(link(prev_p)!=null)prev_p= link(prev_p);
		  link(prev_p)= new_rule();
		  width(link(prev_p))= overfull_rule;
		}
	      print_ln();print_nl("Overfull \\hbox (");
	      print_scaled(-x-total_shrink[normal]);print("pt too wide");
	      goto common_ending;
	    }
	}
      else if (sho==normal)
	{ if (list_ptr(r)!=null)
	    { last_badness= badness(-x,total_shrink[normal]);
	      if (last_badness> hbadness)
		{ print_ln();print_nl("Tight \\hbox (badness ");print_int(last_badness);
		  goto common_ending;
		}
	    }
	}
      goto end;
    }
 common_ending:
  if (pack_begin_line!=0)
	{ if (pack_begin_line> 0)print(") in paragraph at lines ");
	  else print(") in alignment at lines ");
	  print_int(abs(pack_begin_line));
	  print("--");
	}
  else print(") detected at line ");
      print_int(line);
  print_ln();
  font_in_short_display= null_font;short_display(list_ptr(r));print_ln();
  begin_diagnostic();show_box(r);end_diagnostic(true);
 end:return r;

  
repack:
  {  /* convert the box to a |hpack_node| */
	  pointer q;
	  q=new_pack_node();
	  height(q)=h;
	  depth(q)=d;
	  width(q)=x;
	  subtype(q)=hpack_node;
      list_ptr(q)=list_ptr(r);
	  list_ptr(r)=null;
      free_node(r, box_node_size);
	  pack_limit(q)=max_dimen; /* no limit, not used */
	  pack_m(q)=m;
	  pack_extent(q)=new_xdimen(w,hf,vf);
	  return q;
  }
}

@

@<Hi\TeX\ routines@>=
pointer vpackage(pointer p, scaled h, scaled hf, scaled vf, small_number m, scaled l)
{ pointer r; /*the box node that will be returned*/ 
  scaled w,d,x; /*width, depth, and natural height*/ 
  scaled s=0; /*shift amount*/ 
  pointer g; /*points to a glue specification*/ 
  glue_ord sho, sto; /*order of infinity*/ 
  last_badness= 0; r= get_node(box_node_size); type(r)= vlist_node;
  subtype(r)= min_quarterword; shift_amount(r)= 0;
  list_ptr(r)= p;
  w= 0;
  d= 0;x= 0;
  total_stretch[normal]= 0;total_shrink[normal]= 0;
  total_stretch[fil]= 0;total_shrink[fil]= 0;
  total_stretch[fill]= 0;total_shrink[fill]= 0;
  total_stretch[filll]= 0;total_shrink[filll]= 0;
  while(p!=null)
    { if (is_char_node(p)) 
	    confusion("vpack");
      else 
	switch(type(p))
          { case hlist_node:case vlist_node:case rule_node:case unset_node:
              x= x+d+height(p);d= depth(p);
              if (type(p)>=rule_node) s= 0;
		      else s= shift_amount(p);
              if (width(p)+s> w) w= width(p)+s; 
              break;
          case whatsit_node:
            if (subtype(p)==graf_node)
			  goto repack;
			else if (subtype(p)==disp_node )
			  goto repack;
			else if (subtype(p)==vpack_node )
			  goto repack;
			else if (subtype(p)==hpack_node )
			  goto repack;
			else if (subtype(p)==hset_node )
			  goto repack;
			else if (subtype(p)==vset_node )
			  goto repack;
			else if (subtype(p)==stream_node )
			  goto repack;
			else if (subtype(p)==image_node)
			{ glue_ord o;
			  if (image_width(p)> w) w= image_width(p);
			  x= x+d+image_height(p);d=0;
			  o= image_stretch_order(p);total_stretch[o]= total_stretch[o]+image_stretch(p);
	          o= image_shrink_order(p);total_shrink[o]= total_shrink[o]+image_shrink(p);
			}
             break;
          case glue_node:
            { glue_ord o;
			  x= x+d;d= 0;
              g= glue_ptr(p);x= x+width(g);
              o= stretch_order(g); total_stretch[o]= total_stretch[o]+stretch(g);
              o= shrink_order(g); total_shrink[o]= total_shrink[o]+shrink(g);
              if (subtype(p)>=a_leaders)
                { g= leader_ptr(p);
                  if (width(g)> w) w= width(g);
                }
            }
            break;
          case kern_node:
            x= x+d+width(p);d= 0;
            break;
          default:do_nothing;
          }
      p= link(p);
    }
  width(r)= w;   


    if (total_stretch[filll]!=0) sto= filll;
    else if (total_stretch[fill]!=0) sto= fill;
    else if (total_stretch[fil]!=0) sto= fil;
    else sto= normal;

    if (total_shrink[filll]!=0) sho= filll;
    else if (total_shrink[fill]!=0) sho= fill;
    else if (total_shrink[fil]!=0) sho= fil;
    else sho= normal;

    if (hf!=0 || vf!=0) /* convert to a vset node */
	{ pointer q;
	  q=new_set_node();
	  subtype(q)=vset_node;
	  width(q)=w;
      if (d> l)
      { x= x+d-l;depth(r)= l;
      }
      else depth(r)= d;
	  height(q)=x;
	  depth(q)=d;
	  shift_amount(q)=shift_amount(r);
	  list_ptr(q)=list_ptr(r);
	  list_ptr(r)=null;
      free_node(r, box_node_size);
      if (m==exactly)
	    set_extent(q)=new_xdimen(h,hf,vf);
	  else
	    set_extent(q)=new_xdimen(x+h,hf,vf);
      set_stretch_order(q)=sto;
      set_shrink_order(q)=sho;
      set_stretch(q)=total_stretch[sto];
      set_shrink(q)=total_shrink[sho];
	  return q;
	}

   if (d> l)
      { x= x+d-l;depth(r)= l;
      }
      else depth(r)= d;
      if (m==additional) 
	    h= x+h;
      height(r)= h; x= h-x; /*now |x| is the excess to be made up*/ 
if (x==0)
    { glue_sign(r)= normal; glue_order(r)= normal;
      set_glue_ratio_zero(glue_set(r));
      goto end;
    }
 else if (x> 0)
	  { glue_order(r)= sto;glue_sign(r)= stretching;
        if (total_stretch[sto]!=0)glue_set(r)= unfloat(x/(double)total_stretch[sto]);
        else
	    { glue_sign(r)= normal;
	      set_glue_ratio_zero(glue_set(r));
	    }
        if (sto==normal)
		{ if (list_ptr(r)!=null)
		  { last_badness= badness(x,total_stretch[normal]);
			if (last_badness> vbadness)
			  { print_ln();
			    if (last_badness> 100)print_nl("Underfull");else print_nl("Loose");
			    print(" \\vbox (badness ");print_int(last_badness);
			    goto common_ending;
			  }
		  }
		}
        goto end;
	  }
  else /* if (x<0) */
    {
      glue_order(r)= sho;glue_sign(r)= shrinking;
      if (total_shrink[sho]!=0)glue_set(r)= unfloat((-x)/(double)total_shrink[sho]);
      else
	{ glue_sign(r)= normal;
	  set_glue_ratio_zero(glue_set(r));
	}
      if ((total_shrink[sho]<-x)&&(sho==normal)&&(list_ptr(r)!=null))
	{ last_badness= 1000000;
	  set_glue_ratio_one(glue_set(r));
	  if ((-x-total_shrink[normal]> vfuzz)||(vbadness<100))
	    { print_ln();print_nl("Overfull \\vbox (");
	      print_scaled(-x-total_shrink[normal]);print("pt too high");
	      goto common_ending;
	    }
	}
      else if (sho==normal)
	  { if (list_ptr(r)!=null)
			   { last_badness= badness(-x,total_shrink[normal]);
			     if (last_badness> vbadness)
			       { print_ln();print_nl("Tight \\vbox (badness ");print_int(last_badness);
				 goto common_ending;
			       }
			   }
	  }
      goto end;
    }

 
 common_ending:
   if (pack_begin_line!=0)
	  { print(") in alignment at lines ");
	    print_int(abs(pack_begin_line));
	    print("--");
	  }
   else 
		print(") detected at line ");
   print_int(line);
   print_ln();

   begin_diagnostic();show_box(r);end_diagnostic(true);
 end: 
   return r;


repack:
  {  /* convert the box to a |vpack_node| */
	  pointer q;
	  q=new_pack_node();
	  subtype(q)=vpack_node;
	  height(q)=x;
	  depth(q)=d;
	  width(q)=w;
      list_ptr(q)=list_ptr(r);
	  list_ptr(r)=null;
      free_node(r, box_node_size);
	  pack_limit(q)=l;
	  pack_m(q)=m;
      pack_extent(q)=new_xdimen(h,hf,vf);
	  return q;
  }
}

@

\subsection{Streams}
\HINT\ stream numbers start at 0 for the main text and continue
upwards. \TeX, on the other hand, numbers insertions starting with 
{\tt box255} for the main text and continues downwards. Some mapping is
needed, and we use the array |insert2stream| to map \TeX's insert
numbers to \HINT\ stream numbers.
The predefined stream for the main content has stream number 0.

@<Hi\TeX\ variables@>=
int insert2stream[0x100]={0};
@

The following function returns the stream number for a given insert number $i$
with $255>|i|\ge 0$. A new stream number is allocated if necessary.
Note that no overflow test is necessary since \TeX\ allocates less
than 233 inserts.
The initial value of |max_ref[stream_kind]| is 0 and therefore
stream number 0, reserved for the main content
is never allocated. Stream definitions might also be loaded
as part of a format file. Then the maximum stream number ist stored in |max_stream|.
So if we do not find a stream number
in the |insert2stream| array, we scan the stream definitions
once and cache the associations found there.

@<Hi\TeX\ routines@>=
int hget_stream_no(int i)
{ static bool init=false;
  int s;
  if (i==0) return 0; 
  s=insert2stream[i];
  if (s!=0) return s;
  if (!init)
  { pointer t,s;
    for (t=link(setpage_head); t!=null; t=link(t))
      for(s=setpage_streams(t); s!=null; s=link(s))
        insert2stream[setstream_insertion(s)]=setstream_number(s);
    max_ref[stream_kind]=max_stream;
    init=true;
  }
  s=insert2stream[i];
  if (s==0)
    s=insert2stream[i]=max_ref[stream_kind]=++max_stream;
  return s;
}
@

\subsection{Stream Definitions}

A stream definition is stored as a whatsit node with subtype |setstream_node| as
defined in section~\secref{whatsit}.
Given a pointer |p| to such a node, here are the macros used to access the data stored there:
\itemize
\item |setstream_number(p)| the \HINT\ stream number $n$.
\item |setstream_insertion(p)| the corresponding \TeX\ insertion number $i$.
\item |setstream_max(p)| the maximum height $x$: This extended dimension is the maximum size 
          per page for this insertion.
\item |setstream_mag(p)| the magnification factor $f$:
          Inserting a box of height $h$ will contribute $h*f/1000$
          to the main page.
\item |setstream_prefered(p)| the prefered stream  $p$:
          If $p\ge0$ we move the insert to stream $p$ if possible.
\item |setstream_next(p)| the next stream $n$:
          If $n\ge0$ we move the insert to stream $n$ if it can not be
          accomodated otherwise.
\item |setstream_ratio(p)| the split ratio $r$:
          If $r>0$ split the final contribution of this streams between
          stream $p$ and $n$  in the ratio $r/1000$ for $p$ and $1-r/1000$ for $n$
          before contributing streams $p$ and $r$ to the page.
\item |setstream_before(p)|  the ``before'' list $b$:
          For a nonempty stream the material that is added before the stream content.
\item |setstream_after(p)| the ``after'' list  $a$:
          For a nonempty stream, material that is added after the stream conten.
\item |setstream_topskip(p)| the top skip glue $t$: This glue is inserted between
          the $b$ list and the stream content and ajusted for the height for the first box
          of the stream content.
\item |setstream_width(p)| the width $w$:
          This extended dimension is the width used for example
          to break paragraphs in the stream content into lines.
\item |setstream_height(p)| a glue specification $h$ reflecting the total height,
          strechability and shrinkability of the material in lists $a$ and $b$.
\enditemize

Currently Hi\TeX\ handles only normal streams. First or last streams will come later.

The stream definition nodes are created and initialized with the following function:
@<Hi\TeX\ routines@>=
pointer new_setstream_node(uint8_t n)
{ pointer p=get_node(setstream_node_size);
  type(p)=whatsit_node;subtype(p)=setstream_node;
  setstream_insertion(p)=n;
  setstream_number(p)=hget_stream_no(n);
  setstream_mag(p)=1000;
  setstream_prefered(p)=255;
  setstream_next(p)=255;
  setstream_ratio(p)=0;
  setstream_max(p)=new_xdimen(0,0,ONE);
  setstream_width(p)=new_xdimen(0,ONE,0);
  setstream_topskip(p)=zero_glue; add_glue_ref(zero_glue);
  setstream_height(p)=zero_glue; add_glue_ref(zero_glue);
  setstream_before(p)=null;
  setstream_after(p)=null;

  return p;
}
@


The prefered stream, the next stream, and the split ratio are scanned as part of the
{\tt \BS setstream} primitive as described in section~\secref{primitive}.
When \TeX\ finds the right brace that terminates the stream definition,
it calls |handle_right_brace|. Then it is time to obtain the remaining parts of the
stream definition.
For insertion class $i$,
we can extract the maximum height $x$ of the insertions from 
the corresponding {\tt dimen$i$} register 
the magnification factor $f$ from the {\tt count$i$} register,
and the total height $h$ from the {\tt skip$i$} register.
The width $w$ is taken from {\tt \BS hsize} and the
topskip $t$ from  {\tt \BS topskip}. 

@<Hi\TeX\ routines@>=
void hfinish_stream_group(void)
{  pointer s;
   end_graf();
   s=hget_current_stream();
   if (s!=null) 
   { pointer t;
     uint8_t i;
     i= setstream_insertion(s);
     setstream_mag(s)=count(i);
     setstream_width(s)=new_xdimen(dimen_par(hsize_code),
     dimen_par_hfactor(hsize_code),dimen_par_vfactor(hsize_code));
     t=zero_glue;add_glue_ref(t);delete_glue_ref(setstream_topskip(s)); setstream_topskip(s)=t;
     t=skip(i);add_glue_ref(t);delete_glue_ref(setstream_height(s)); setstream_height(s)=t;
     setstream_max(s)=new_xdimen(dimen(i),
     dimen_hfactor(i),dimen_vfactor(i));
   }
   unsave();
   flush_node_list(link(head));
   pop_nest();
} 
@

The before list $b$ and the after list $a$ are defined using the
{\tt \BS before} and {\tt \BS after} primitives. When the corresponding list
has ended with a right brace, \TeX\ calls |handle_right_brace| and we can store
the lists.

@<Hi\TeX\ routines@>=
void hfinish_stream_before_group(void)
{ pointer s;
  end_graf();
  s=hget_current_stream();
  if (s!=null)
    setstream_before(s)=link(head);
  unsave();
  pop_nest();
} 

void hfinish_stream_after_group(void)
{ pointer s;
  end_graf();
  s=hget_current_stream();
  if (s!=null)
    setstream_after(s)=link(head);
  unsave();
  pop_nest();
}
@

\subsection{Page Template Definitions}

The data describing a page template is stored in a whatsit node with subtype
|setpage_node| as defined insection~\secref{whatsit}.
Given a pointer |p| to such a node, here are the macros used to access the data stored there:
\itemize
\item |setpage_name(p)|: The name of the page template
      can be used in the user interface of a \HINT\ viewer.
\item |setpage_number(p)|: The number of the page template that is used in the \HINT\ 
      file to reference this page template.
\item |setpage_id(p)|: The number of the page template that is used in \TeX\
      to reference this page template.
\item |setpage_priority(p)|: The priority helps in selecting a page template.
\item |setpage_topskip(p)|: The topskip glue is added at the top of a page and 
      adjusted by the height of the first bos on the page.
\item |setpage_height(p)|: The height of the full page including the margins. 
\item |setpage_width(p)|:  The width of the full page including the margins. 
\item |setpage_depth(p)|:  The maximum depth of the page content. If the last box is deeper
than this maximum, the difference is subtracted from the height of the page body.
\item |setpage_list(p)|: The list that defines the page template. After the page builder
has completed a page this list is scanned and page body and nonempty streams
are added at the corresponding insertion points.
\item |setpage_streams(p)|: The list of stream definitions that
belong to this page template.
\enditemize

To allow \TeX\ to use arbitrary numbers between 1 and 255 for the page templates
while in \HINT\ the numbers of page templates are best consecutive 
from 1 to |max_ref[page_kind]==max_page|, we let \TeX\ assign an id and generate
the template number. Because templates might be in format files, the variable
|max_page| will hold the true number.

The function |new_setpage_node| is called with the page template id
$0<|i|<256$ and a string number for the name |n|.
It allocates and initializes a node if necessary and moves it to the front of the 
list of templates.

@<Hi\TeX\ routines@>=
pointer new_setpage_node(uint8_t i, str_number n)
{ pointer p, prev_p;
  prev_p=setpage_head;
  for (p=link(prev_p); p!=null; prev_p=p,p=link(p))
    if (setpage_id(p)==i) break;
  if (p==null)
    @<allocate a new |setpage_node| |p|@>@;
  else 
    link(prev_p)=link(p);
  link(p)=link(setpage_head);
  link(setpage_head)=p;
  return p;
}   
@
    
@<allocate a new |setpage_node| |p|@>=
{ p=get_node(setpage_node_size);type(p)=whatsit_node;subtype(p)=setpage_node;
  setpage_number(p)=max_ref[page_kind]=++max_page;
  setpage_id(p)=i;
  setpage_name(p)=n;
  setpage_priority(p)=1;
  setpage_topskip(p)=zero_glue; add_glue_ref(zero_glue);
  setpage_height(p)=new_xdimen(0,0,ONE);
  setpage_width(p)=new_xdimen(0,ONE,0);
  setpage_depth(p)=max_depth;
  setpage_list(p)=null;
  setpage_streams(p)=null;
}
@

The default values are replaced by parameters given to the {\tt\BS setpage}
primitive (see section~\secref{pageext}) and by the current values of certain
\TeX\ registers when finishing the page template.
page template itself,

@<Hi\TeX\ routines@>=
void hfinish_page_group(void)
{ uint8_t k;
  pointer p,q,r;
  end_graf();
  p=hget_current_page();
  if (p!=null)
  { delete_glue_ref(setpage_topskip(p));
    setpage_topskip(p)=top_skip;add_glue_ref(top_skip);
    setpage_depth(p)=max_depth;
    flush_node_list(setpage_list(p));
    setpage_list(p)=link(head);
  }
  unsave();
  pop_nest();
} 
@
@<Hi\TeX\ auxiliar routines@>=

static pointer hget_current_page(void)
{ pointer p=link(setpage_head);
  if (p==null)
    print_err("end of output group without setpage node"); 
  return p;
}

static pointer hget_current_stream(void)
{ pointer p,s;
  p=hget_current_page();
  if (p==null) return null;
  s=setpage_streams(p);
  if (s==null)
    print_err("end of setstream group without setstream node");
  return s;
}
@



\section{\HINT\ Output}

\subsection{Initialization}
\noindent

@<Hi\TeX\ routines@>=
static void hout_init(void)
{
  new_directory(dir_entries); 
  new_output_buffers();
  max_section_no=2;
  hdef_init();
  hput_content_start();
  @<insert an initial language node@>@;
}

extern int option_global;
void hint_open(void)
{ if (job_name==0) open_log_file();
  pack_job_name(".hnt");
  while (!(hout=open_out((char *)name_of_file+1,"wb")))
    prompt_file_name("file name for output",".hnt");
  hlog=stderr;
  option_global=true;
  hout_init();
}
@

\subsection{Termination}

@<Hi\TeX\ routines@>=
#define HITEX_VERSION "1.1"
static void  hput_definitions();
static void hout_terminate(void)
{ hput_content_end();
  hput_definitions();
  hput_directory(); 
  hput_hint("created by HiTeX Version " HITEX_VERSION);
}
	
void hint_close(void)
{ hout_terminate();
  if (hout!=NULL)
  fclose(hout);
  hout=NULL;
}
@


\subsection{\HINT\ Directory}
There is not much to do here: some code to find a new or existing directory entry,
a variable to hold the number of directory entries allocated,
a function to allocate a new file section, and an auxiliar function to
convert \TeX's file names to ordinary \CEE/ strings.

@<Find an existing directory entry@>=
for (i=3; i<= max_section_no;i++)
  if (dir[i].file_name!=NULL && strcmp(dir[i].file_name,file_name)==0)
    return i;
@

@<Allocate a new directory entry@>=
  i = max_section_no;
  i++;
  if (i>0xFFFF) QUIT("Too many file sections");
  if (i>=dir_entries) 
    RESIZE(dir,dir_entries,entry_t);
  max_section_no=i;
  if (max_section_no>0xFFFF) QUIT("Too many sections");
  dir[i].section_no=i;
@

@<Hi\TeX\ macros@>=
#define @[RESIZE(P,S,T)@]	      \
{ int _n = (S)*1.4142136 +0.5;        \
  if (_n<32) _n=32;                   \
  { REALLOCATE(P,_n,T);             \
    memset((P)+(S),0,(_n-(S))*sizeof(T));         \
    (S)=_n;                           \
  }                                   \
}
@

@<Hi\TeX\ variables@>=
static int dir_entries=4;
@

@<Hi\TeX\ auxiliar routines@>=
static uint16_t hnew_file_section(char *file_name)
{ uint16_t i;
  @<Find an existing directory entry@>@;
  @<Allocate a new directory entry@>@;
  dir[i].file_name=strdup(file_name);
  return i;
}
@

The following function uses \TeX's function |pack_file_name|
to create a new filename from a name |n|, a direcory or ``area'' |a|,
and an extension |e|. \TeX\ will truncate the new filename
to |file_name_size| characters without warning. The new function
will take a |name_length| equal to | file_name_size| as an
indication that truncation has taken place and terminates the
program. The return value converts a \Pascal\ array, starting with index 1,
into a \CEE/ array starting with index 0.

@<Hi\TeX\ auxiliar routines@>=
static char *hfile_name(str_number n, char *a, char *e)
{ pack_file_name(n,a,e);
  if (name_length>=file_name_size)
   QUIT("File name too long %d >= %d",name_length,file_name_size);
  return (char *)name_of_file+1;
}

@

\section{\HINT\ Definitions}\label{definitions}
Definitions are used for two reasons: they provide default values for the parameters
that drive \TeX's algorithms running in the \HINT\ viewer, and they provide a compact notation
for \HINT\ content nodes.

To find the optimal coding for a \HINT\ file, a global knowledge of the \HINT\ file is necessary. 
This would require a two pass process:
in the first pass Hi\TeX\ could gather statistics on the use of parameter values and content
nodes as a basis for making definitions and in the second pass it could encode the content using
these definitions. I consider it, however, more reasonable to write such a two pass optimizer
as a separate program which can be used on any \HINT\ file. 
Hence Hi\TeX\ uses a much simpler one pass approach:
\itemize

\item Hi\TeX\ generates definitions for \TeX-parameters using the
 values they have when the first non discardable item appears in
 |build_page|. This is usually the case after initial style files have
 been processed and we can expect that they set useful default values.

The procedure that generates these definitions is called |hfix_defaults|:

@<Hi\TeX\ auxiliar routines@>=

void hfix_defaults(void)
{ @+int i;
  DBG(DBGDEF,"Freezing HINT file defaults\n");
  @<Compute the page size@>@;
  @<Fix definitions for integer parameters@>@;
  @<Fix definitions for dimension parameters@>@;
  @<Fix definitions for glue parameters@>@;
  @<Fix definitions of page templates@>@;
}

@

\item Hi\TeX\ generates definitions to be used in content nodes on the fly:
Whenever a routine outputs an item for which a definition might be available,
it calls a {\it hget\_\dots\_no} function. This function returns, if possible,
the reference number of a suitable definition.
If no definition is available, the function will try to allocate a new one,
only if all reference numbers from 0 to |0xFF| are already in use, a $-1$ is
returned to indicate failure.

There are two possible problems with this approach: We might miss a
very common item because it occurs for the first time late in the
input when all reference numbers are already in use. For example an
extensive index might repeat a certain pattern for each entry.  And second, we
might make a definition for an item that occurs only once. Taken
together the definition plus the reference to it requires more space
than the same item without a definition.

We can hope that the first effect does not occur too often, especially if the
\TeX\ file is short, and we know that the second effect is limited by the
total number of definitions we can make plus four byte of overhead per instance.
\enditemize

Here we initialize the necessary data structures for definitions.
@<Hi\TeX\ auxiliar routines@>=
static void hdef_init(void)
{@+ int i; 
  @<Switch |hsize| and |vsize| to extended dimensions@>@;
  @<Initialize definitions for extended dimensions@>@;
  @<Initialize definitions for baseline skips@>@;
  @<Initialize definitions for fonts@>@;
  @<Initialize definitions for labels@>@;
#if 0
  overfull_rule=0;    /* no overfull rules please */
#endif
}
@

After all definitions are ready, we write them using the function
|hput_definitions|.  When we output the definitions, 
we have to make sure to define references before we use them.
This is achived by using a specific ordering of the
definitions in the function |hput_definitions| and by preventing
the allocation of new definitions as soon as the output of the definition
section has started. The latter has the aditional benefit that the
maximum values do no longer change.

@<Hi\TeX\ routines@>=
static void  hput_definitions()
/* write the definitions into the definitions buffer */
{  int i;
   uint32_t d, m, s;
   hput_definitions_start();
   hput_max_definitions();
   @<Output language definitions@>@;
   @<Output font definitions@>@;
   @<Output integer definitions@>@;
   @<Output dimension definitions@>@;
   @<Output extended dimension definitions@>@;
   @<Output glue definitions@>@;
   @<Output baseline skip definitions@>@;
   @<Output parameter list definitions@>@;
   @<Output discretionary break definitions@>@;
   @<Output page template definitions@>@;
   hput_definitions_end();
   hput_range_defs(); /* expects the definitions section to be ended */
   hput_label_defs();
}
@


In the following, we present for each node type the code to generate
the definitions, using a common schema: We define a data structure
called {\it\dots\_defined}, to hold the definitions; we define, if
applicable, the \TeX-parameters; we add an {\it hget\_\dots\_no}
function to allocate new definitions; and we finish with the code to
output the collected definitions.

Lets start with the most simple case: integers.

\subsection{Integers}
\subsubsection{Data}
The data structure to hold the integer definitions is a simple array with |0x100| entries.
A more complex data structure, for example a hash table, could speed up searching for
existing definitions but lets keep things simple for now.

@<Hi\TeX\ variables@>=
static int32_t int_defined[0x100]={0};
@
\subsubsection{Mapping}
Before we can generate definitions for \TeX-parameters, we have to map \TeX's 
parameter numbers to \HINT\ definition numbers. While it seems more convenient here
to have the reverse mapping, we need the mapping only once to record parameter definitions,
but we will need it repeatedly in the function |hdef_param_node| and the overhead here does
not warant having the mapping in both directions.

@<Hi\TeX\ variables@>=
static const int hmap_int[] ={@/
pretolerance_no,  /* |pretolerance_code| 0 */
tolerance_no,  /* |tolerance_code| 1 */ 
line_penalty_no,  /* |line_penalty_code| 2 */ 
hyphen_penalty_no,  /* |hyphen_penalty_code| 3 */ 
ex_hyphen_penalty_no,  /* |ex_hyphen_penalty_code| 4 */ 
club_penalty_no,  /* |club_penalty_code| 5 */ 
widow_penalty_no,  /* |widow_penalty_code| 6 */ 
display_widow_penalty_no,  /* |display_widow_penalty_code| 7 */ 
broken_penalty_no,  /* |broken_penalty_code| 8 */ 
-1,  /* |bin_op_penalty_code| 9 */ 
-1,  /* |rel_penalty_code| 10 */ 
pre_display_penalty_no,  /* |pre_display_penalty_code| 11  */ 
post_display_penalty_no,  /* |post_display_penalty_code| 12  */ 
inter_line_penalty_no,  /* |inter_line_penalty_code| 13 */ 
double_hyphen_demerits_no,  /* |double_hyphen_demerits_code| 14 */ 
final_hyphen_demerits_no,  /* |final_hyphen_demerits_code| 15 */ 
adj_demerits_no,  /* |adj_demerits_code| 16 */ 
-1,  /* |mag_code| 17 */ 
-1,  /* |delimiter_factor_code| 18 */ 
looseness_no,  /* |looseness_code| 19 */ 
time_no,  /* |time_code| 20 */ 
day_no,  /* |day_code| 21 */ 
month_no,  /* |month_code| 22 */ 
year_no,  /* |year_code| 23 */ 
-1,  /* |show_box_breadth_code| 24 */ 
-1,  /* |show_box_depth_code| 25 */ 
-1,  /* |hbadness_code| 26 */ 
-1,  /* |vbadness_code| 27 */ 
-1,  /* |pausing_code| 28 */ 
-1,  /* |tracing_online_code| 29 */ 
-1,  /* |tracing_macros_code| 30 */ 
-1,  /* |tracing_stats_code| 31 */ 
-1,  /* |tracing_paragraphs_code| 32 */ 
-1,  /* |tracing_pages_code| 33 */ 
-1,  /* |tracing_output_code| 34 */ 
-1,  /* |tracing_lost_chars_code| 35 */ 
-1,  /* |tracing_commands_code| 36 */ 
-1,  /* |tracing_restores_code| 37 */ 
-1,  /* |uc_hyph_code| 38 */ 
-1,  /* |output_penalty_code| 39 */ 
-1,  /* |max_dead_cycles_code| 40 */ 
hang_after_no,  /* |hang_after_code| 41*/
floating_penalty_no  /* |floating_penalty_code|	42*/
};


@
\subsubsection{Parameters}
Now we can generate the definitions for integer parameters:

@<Fix definitions for integer parameters@>=
  int_defined[zero_int_no]=0;
  for (i=pretolerance_code; i<=hang_after_code;i++)
    if ( hmap_int[i]>=0) int_defined[hmap_int[i]]=int_par(i);
  max_ref[int_kind]=MAX_INT_DEFAULT;
@


\subsubsection{Allocation}
The function |hget_int_no| tries to allocate a predefined integer number; 
if not successful, it returns $-1$.

@<Hi\TeX\ auxiliar routines@>=
static int hget_int_no(int32_t n)
{ int i;
  int m =max_ref[int_kind];
  for (i=0; i<=m; i++)
    if (n== int_defined[i]) return i;
  if (m<0xFF && section_no==2)
    { m=++max_ref[int_kind]; int_defined[m]=n; return m; }
  else
    return -1;
}
@

\subsubsection{Output}
Before we give the code to output an integer definition, we declare a macro that
is usefull for all the definitions. |HPUTDEF| takes a function |F| and a reference number |R|.
It is assumed that |F| writes a definition into the output and returns a tag. The macro
will then add the reference number and both tags to the output.
@<Hi\TeX\ macros@>=
#define HPUTDEF(F,R)            \
  { uint32_t _p;                \
    uint8_t _t;                 \
    HPUTNODE; /* allocate */    \
    _p=hpos-hstart;             \
    HPUT8(0);  /* tag */        \
    HPUT8(R); /* reference */   \
    _t=F;                       \
    hstart[_p]=_t; DBGTAG(_t,hstart+_p);      \
    DBGTAG(_t,hpos); HPUT8(_t); \
  }
@

Definitions are written to the output only if they differ from Hi\TeX's built in defaults.
@<Output integer definitions@>=
  DBG(DBGDEF,"Maximum int reference: %d\n",max_ref[int_kind]);
  for (i=max_fixed[int_kind]+1;i<=max_default[int_kind]; i++)
    { if (int_defined[i]!=int_defaults[i])@/
        HPUTDEF(hput_int(int_defined[i]),i);
    }
  for (;i<=max_ref[int_kind]; i++)@/
         HPUTDEF(hput_int(int_defined[i]),i);
@


\subsection{Dimensions}
We proceed as we did for integers, starting with the array that holds the defined dimensions.
\subsubsection{Data}
@<Hi\TeX\ variables@>=
static scaled dimen_defined[0x100]={0};
@
\subsubsection{Mapping}
@<Hi\TeX\ variables@>=
static const int hmap_dimen[] ={@/
  -1, /* |par_indent_code| 0 */
  -1,  /* |math_surround_code| 1 */ 
  line_skip_limit_no,  /* |line_skip_limit_code| 2 */ 
  hsize_dimen_no,   /* |hsize_code| 3 */
  vsize_dimen_no,  /* |vsize_code| 4 */
  max_depth_no,  /* |max_depth_code| 5 */
  split_max_depth_no, /* |split_max_depth_code| 6 */
  -1, /* |box_max_depth_code| 7 */
  -1,  /* |hfuzz_code| 8 */
  -1, /* |vfuzz_code| 9 */ 
  -1, /* |delimiter_shortfall_code| 10 */
  -1, /* |null_delimiter_space_code| 11 */
  -1, /* |script_space_code| 12 */
  -1, /* |pre_display_size_code| 13 */
  -1, /* |display_width_code| 14 */
  -1, /* |display_indent_code| 15 */
  -1, /* |overfull_rule_code| 16 */
  hang_indent_no,  /* |hang_indent_code| 17 */
  -1, /* |h_offset_code| 18 */
  -1,  /* |v_offset_code| 19 */
  emergency_stretch_no /* |emergency_stretch_code| 20 */
};
@

\subsubsection{Parameters}
@<Fix definitions for dimension parameters@>=
  dimen_defined[zero_dimen_no]=0;
  for (i=par_indent_code; i<=emergency_stretch_code;i++)
    if ( hmap_dimen[i]>=0) dimen_defined[hmap_dimen[i]]=dimen_par(i);
  dimen_defined[hsize_dimen_no]=page_h;
  dimen_defined[vsize_dimen_no]=page_v;
  dimen_defined[quad_no]=quad(cur_font);
  dimen_defined[math_quad_no]=math_quad(text_size);
  max_ref[dimen_kind]=MAX_DIMEN_DEFAULT;
@

\subsubsection{Allocation}
@<Hi\TeX\ auxiliar routines@>=
static int hget_dimen_no(scaled s)
/* tries to allocate  a predefined dimension number in the range 0 to 0xFF 
   if not successful return -1 */
{ int i;
  int m =max_ref[dimen_kind];
  for (i=0; i<=m; i++)
    if (s== dimen_defined[i]) return i;
  if (m<0xFF && section_no==2)
    { m=++max_ref[dimen_kind]; dimen_defined[m]=s; return m; }
  else
    return -1;
}
@

\subsubsection{Output}
@<Output dimension definitions@>=
  DBG(DBGDEF,"Maximum dimen reference: %d\n",max_ref[dimen_kind]);
  for (i=max_fixed[dimen_kind]+1;i<=max_default[dimen_kind]; i++)
    { if (dimen_defined[i]!=dimen_defaults[i])
        HPUTDEF(hput_dimen(dimen_defined[i]),i);
    }
  for (;i<=max_ref[dimen_kind]; i++)
         HPUTDEF(hput_dimen(dimen_defined[i]),i);
@


\subsection{Extended Dimensions}

\subsubsection{Data}
@<Hi\TeX\ variables@>=
static struct {
scaled w,h,v; } xdimen_defined[0x100];
@

\subsubsection{Initialization}
@<Initialize definitions for extended dimensions@>=
  for (i=0; i<=max_fixed[xdimen_kind]; i++)
  { xdimen_defined[i].w = xdimen_defaults[i].w;
    xdimen_defined[i].h = ONE*xdimen_defaults[i].h;
    xdimen_defined[i].v = ONE*xdimen_defaults[i].v;
  }
@

\subsubsection{Allocation}
To obtain a reference number for an extended dimension, we search the 
array and if no match was found, we allocate a new entry, 
reallocating the array if needed.
We use the variable |rover| to mark the place where the 
last entry was inserted, because
quite often we repeatedly search for the same values.

@<Hi\TeX\ auxiliar routines@>=
int hget_xdimen_no(pointer p)
{ int i;
  for (i=0;i<=max_ref[xdimen_kind];i++)
  { if (xdimen_defined[i].w== xdimen_width(p) &&
        xdimen_defined[i].h== xdimen_hfactor(p) &&
        xdimen_defined[i].v== xdimen_vfactor(p))
       return i;
  }
  if (section_no!=2) return -1;
  if (i>=0x100) return -1;
  max_ref[xdimen_kind]=i;
  xdimen_defined[i].w= xdimen_width(p);
  xdimen_defined[i].h= xdimen_hfactor(p);
  xdimen_defined[i].v= xdimen_vfactor(p);
  return i;
}
@

@<Hi\TeX\ routines@>=
pointer new_xdimen(scaled w, scaled h, scaled v)
{ pointer p=get_node(xdimen_node_size);
  type(p)=whatsit_node;subtype(p)=xdimen_node;
  xdimen_width(p)=w;
  xdimen_hfactor(p)=h;
  xdimen_vfactor(p)=v;
  return p;
}
@

\subsubsection{Output}
@<Output extended dimension definitions@>=
  DBG(DBGDEF,"Maximum xdimen reference: %d\n",max_ref[xdimen_kind]);
  for (i=max_fixed[xdimen_kind]+1;i<=max_default[xdimen_kind]; i++)
  { xdimen_t x;
    x.w=xdimen_defined[i].w;
    x.h=xdimen_defined[i].h/(double)ONE;
    x.v=xdimen_defined[i].v/(double)ONE;

    if (x.w!=xdimen_defaults[i].w ||
        x.h!=xdimen_defaults[i].h ||
        x.v!=xdimen_defaults[i].v)
        HPUTDEF(hput_xdimen(&x),i);
  }
  for (;i<=max_ref[xdimen_kind]; i++)
  { xdimen_t x;
    x.w=xdimen_defined[i].w;
    x.h=xdimen_defined[i].h/(double)ONE;
    x.v=xdimen_defined[i].v/(double)ONE;
    HPUTDEF(hput_xdimen(&x),i);
  }
@


@
\subsection{Glues}
In general there are two choices on how to store a definition: We can use the data structures used by \TeX\
or we can use the data structures defined by \HINT. If we are lucky, both of them are the same
as we have seen for integers and dimensions. For extended dimensions, we had to use the \HINT\ data type
|xdimen_t| because \TeX\ has no corresponding data type and uses only reference numbers.
In the case of glue, we definitely have a choice. We decide to use \TeX's pointers to glue specifications
in the hope to save some work when comparing glues for equality, because \TeX\ already reuses
glue specifications and often a simple comparison of pointers might suffice.
\subsubsection{Data}
@<Hi\TeX\ variables@>=
static pointer glue_defined[0x100]; 
@
\subsubsection{Mapping}
@<Hi\TeX\ variables@>=
static int hmap_glue[] ={

line_skip_no,  /* |line_skip_code| 0 */ 
baseline_skip_no,  /* |baseline_skip_code| 1 */ 
-1,  /* |par_skip_code| 2 */ 
above_display_skip_no,  /* |above_display_skip_code| 3 */ 
below_display_skip_no,  /* |below_display_skip_code| 4 */ 
above_display_short_skip_no,  /* |above_display_short_skip_code| 5 */ 
below_display_short_skip_no,  /* |below_display_short_skip_code| 6 */ 
left_skip_no,  /* |left_skip_code| 7 */ 
right_skip_no,  /* |right_skip_code| 8 */ 
top_skip_no,  /* |top_skip_code| 9 */ 
split_top_skip_no,  /* |split_top_skip_code| 10 */ 
tab_skip_no,  /* |tab_skip_code| 11 */ 
-1,  /* |space_skip_code| 12 */ 
-1,  /* |xspace_skip_code| 13 */ 
par_fill_skip_no  /* |par_fill_skip_code| 14 */ 
};
@
\subsubsection{Parameters}
@<Fix definitions for glue parameters@>=
  glue_defined[zero_skip_no]=zero_glue; incr(glue_ref_count(zero_glue));
  for (i=line_skip_code; i<=par_fill_skip_code;i++)
    if ( hmap_glue[i]>=0)
    { glue_defined[hmap_glue[i]]=glue_par(i); incr(glue_ref_count(glue_par(i)));}
  max_ref[glue_kind]=MAX_GLUE_DEFAULT;
@


\subsubsection{Allocation}
Next we define some auxiliar routines to compare glues for equality and to convert glues beween the different representations.

@<Hi\TeX\ auxiliar routines@>=
int glue_spec_equal(pointer p, pointer q)
{ return (width(q)==width(p) && stretch(q)==stretch(p) && shrink(q)==shrink(p) &&
        (stretch_order(q)==stretch_order(p) || stretch(q)==0) &&
		  (shrink_order(q)==shrink_order(p)|| shrink(q)==0));
}

int glue_equal(pointer p, pointer q)
{ return p==q || glue_spec_equal(p,q);
}

int glue_t_equal(glue_t *p, glue_t *q)
{ return(p->w.w==q->w.w && p->w.h==q->w.h && p->w.v==q->w.v && 
          p->p.f== q->p.f && p->m.f==q->m.f &&
          (p->p.o==q->p.o || p->p.f==0.0) &&
	  (p->m.o==q->m.o || q->m.f==0.0));
}

@

To find a matching glue we make two passes over the defined glues:
on the first pass we just compare pointers and on the second pass
we also compare values. An alternative approach to speed up searching
is used in section~\secref{paramlist} below.

@<Hi\TeX\ auxiliar routines@>=
static int hget_glue_no(pointer p)
{ static int rover=0;
  int i;
  if (p==zero_glue) return zero_skip_no;
  for(i=0; i<= max_ref[glue_kind] ; i++)
  { if (p==glue_defined[rover]) return rover;
    else if (rover==0) 
      rover=max_ref[glue_kind];
    else      
      rover--;
  }
  for(i=0; i<= max_ref[glue_kind] ; i++)
  { pointer q=glue_defined[rover];
    if (glue_spec_equal(p,q))
      return rover;
    else if (rover==0) 
      rover=max_ref[glue_kind];
    else      
      rover--;
  }
  if (max_ref[glue_kind]<0xFF && section_no==2) 
  { rover=++max_ref[glue_kind];
    glue_defined[rover]=p;
    incr(glue_ref_count(p));
    DBG(DBGDEF,"Defining new glue %d\n",rover);
    return rover;
  }
  else
    return -1;
}
@
\subsubsection{Output}
@<Output glue definitions@>=
  DBG(DBGDEF,"Maximum glue reference: %d\n",max_ref[glue_kind]);
  for (i=max_fixed[glue_kind]+1;i<=max_default[glue_kind]; i++)
    { glue_t g;
      to_glue_t(glue_defined[i],&g);
     if (!glue_t_equal(&g,&glue_defaults[i]))
        HPUTDEF(hput_glue(&g),i);
    }
  for (;i<=max_ref[glue_kind]; i++)
           HPUTDEF(hout_glue_spec(glue_defined[i]),i);
@

The above code uses the following conversion routine.
While \HINT\ supports glue that depends on {\tt hsize} and {\tt vsize},
this is currently not supported by Hi\TeX. 
Future versions of Hi\TeX\ should extend glue spec nodes (and kern nodes)
by fields for |hfactor| and |vfactor| which are zero by default.
This would leave most parts of \TeX\ unchanged.
As a work-around one can combine a box with an extended dimension with 
a regular glue or kern. 
% Care should be taken for the statically allocated glue specs
% deallocation of glue specs is relatively simple
% extended glue and kern values can be restricted to non math mode
@<Hi\TeX\ auxiliar routines@>=
void to_glue_t(pointer p, glue_t *g)
{ g->w.w=width(p);
  g->w.h=g->w.v=0.0;
  g->p.f=stretch(p)/(double)ONE; g->p.o= stretch_order(p);
  g->m.f=shrink(p)/(double)ONE; g->m.o= shrink_order(p);
}
@

\subsection{Baseline Skips}
TeX's baseline nodes just store a baseline skip reference number.
We have seen this sitation before when dealing with extended dimensions
and the solution here is the same: a dynamicaly allocated array.
\subsubsection{Data}
@<Hi\TeX\ variables@>=
typedef struct {
	pointer ls, bs; /* line skip and baselineskip gluespecs */
	scaled lsl; /* lineskip limit */
} bl_defined_t;

static bl_defined_t *bl_defined=NULL;
static int bl_used=0,bl_allocated=0;
@

\subsubsection{Initialization}
The zero baseline skip is predefined which prevents an ambiguous info value of zero
in a baseline node.

@<Initialize definitions for baseline skips@>=
  bl_allocated=8;
  ALLOCATE(bl_defined,bl_allocated,bl_defined_t);
  bl_defined[zero_baseline_no].bs=zero_glue; incr(glue_ref_count(zero_glue));
  bl_defined[zero_baseline_no].ls=zero_glue; incr(glue_ref_count(zero_glue));
  bl_defined[zero_baseline_no].lsl=0;
  bl_used= MAX_BASELINE_DEFAULT+1;
  max_ref[baseline_kind]= MAX_BASELINE_DEFAULT;
@
\subsubsection{Allocation}
@<Hi\TeX\ auxiliar routines@>=
int hget_baseline_no(pointer bs, pointer ls, scaled lsl)
{ 
  static int rover=0;
  int i;
  for(i=0; i< bl_used; i++) /* search for an existing spec */
    { bl_defined_t *q=&(bl_defined[rover]);
    if (glue_equal(bs,q->bs) &&  glue_equal(ls,q->ls) && lsl==q->lsl)
      return rover;
    else if (rover==0) 
      rover=bl_used-1;
    else
      rover--;
  }
  if (bl_used>=bl_allocated)
    RESIZE(bl_defined,bl_allocated, bl_defined_t);
  rover=bl_used++;
  if (rover<0x100 && section_no==2) max_ref[baseline_kind]=rover;
  if (glue_equal(bs,zero_glue))
  {  bl_defined[rover].bs=zero_glue; incr(glue_ref_count(zero_glue)); }
  else
  {  bl_defined[rover].bs=bs; incr(glue_ref_count(bs));}
  if (glue_equal(ls,zero_glue))
  {  bl_defined[rover].ls=zero_glue; incr(glue_ref_count(zero_glue)); }
  else
  { bl_defined[rover].ls=ls; incr(glue_ref_count(ls));}
  bl_defined[rover].lsl=lsl;
  return rover;
}

@
\subsubsection{Output}

The following routine does not allocate a new glue definition, because the
baselinedefinitions are output after the glue definitions. This is not perfect.
@<Hi\TeX\ auxiliar routines@>=
static uint8_t hout_glue_spec(pointer p);
uint8_t hout_baselinespec(int n)
{ info_t i=b000;
  pointer p;
  scaled s;
  s=bl_defined[n].lsl;
  if (s!=0) {HPUT32(s); i|=b001;}
  p=bl_defined[n].bs;
  if (p!=zero_glue) 
{ uint8_t *pos;
  uint8_t tag;
  HPUTNODE; /* allocate */
  pos=hpos;  
  hpos++;   /* tag */   
  tag=hout_glue_spec(p);
  *pos=tag; DBGTAG(tag,pos);   
  DBGTAG(tag,hpos); HPUT8(tag);
  i|=b100;
}
  p=bl_defined[n].ls;
  if (p!=zero_glue) 
{ uint8_t *pos;
  uint8_t tag;
  HPUTNODE; /* allocate */
  pos=hpos;  
  hpos++;   /* tag */   
  tag=hout_glue_spec(p);
  *pos=tag; DBGTAG(tag,pos);   
  DBGTAG(tag,hpos); HPUT8(tag);
  i|=b010;
}
  return TAG(baseline_kind,i);
}
@

@<Output baseline skip definitions@>=
  DBG(DBGDEF,"Defining %d baseline skips\n",max_ref[baseline_kind]);
  for (i=1;i<=max_ref[baseline_kind]; i++)
  { uint32_t   pos=hpos-hstart;
    uint8_t tag;
    hpos++; /* space for the tag */
    HPUT8(i); /* reference */
    tag=hout_baselinespec(i);
    hstart[pos]=tag;
    HPUT8(tag);
  }
@
\subsubsection{Printing}
The following function is needed in Hi\TeX\ to produce debugging output if needed.
@<Hi\TeX\ routines@>=
void print_baseline_skip(int i)
{ if (0<=i && i < bl_used)
  { print_spec(bl_defined[i].bs,0); print_char(',');
    print_spec(bl_defined[i].ls,0); print_char(',');
    print_scaled(bl_defined[i].lsl);
  }
  else
    print("unknown");
}
@



\subsection{Discretionary breaks}
\subsubsection{Data}
For discretionary breaks, we use again the pointer representation.
@<Hi\TeX\ variables@>=
static pointer dc_defined[0x100];
@
\subsubsection{Allocation}
There are no predefined discretionary breaks and so we start with 
three auxiliar functions and the
function to get a ``disc'' number.

The first two routines are used to compare discretionary breaks
in order to reuse already defined disc numbers.
The pre and post break lists must consist entirely of character, 
kern, box, rule, and ligature nodes. 
Unfortunately a box node might contain all kinds of nodes
and its content might be huge and deeply nested. 
The following routine will not make a complete comparison but will give
up if the box content is ``too complex''.

@<Hi\TeX\ auxiliar routines@>=
static bool list_equal(pointer p, pointer q);
static bool node_equal(pointer p, pointer q)
{ if (is_char_node(p) && is_char_node(q) &&
        font(p)==font(q) && character(p)==character(q))
    return true;
  if (!is_char_node(p) && !is_char_node(q))
  { if (type(p)!=type(q)) return false;
    if (type(p)==kern_node && 
        subtype(p)==subtype(q) && width(p)==width(q))
      return true;
    if (type(p)==ligature_node &&
        character(lig_char(p)) == character(lig_char(q)) &&
        font(lig_char(p)) == font(lig_char(q)))
      return true;
    if (type(p)==rule_node  && 
        width(p)==width(q) &&  height(p)==height(q) &&  depth(p)==depth(q))
      return true;
    if ((type(p)==hlist_node || type(p)==vlist_node) &&
        width(p)==width(q) &&  height(p)==height(q) &&  depth(p)==depth(q) &&
        shift_amount(p)==shift_amount(q) &&
        glue_sign(p) ==  glue_sign(q) &&
        glue_order(p) ==  glue_order(q) &&
        glue_set(p) == glue_set(q) &&
        list_equal(list_ptr(p),list_ptr(q)))
      return true;
  }
  return false;
}

static bool list_equal(pointer p, pointer q)
{@+while (true)
  { if (p==q) return true;
    if (p==null || q==null) return false;
    if (!node_equal(p,q)) return false;
    p=link(p);q=link(q);
  }     
}

static pointer copy_disc_node(pointer p)
{ pointer q;
  q=get_node(small_node_size);
  pre_break(q)=copy_node_list(pre_break(p));
  post_break(q)=copy_node_list(post_break(p));
  type(q)=type(p);
  subtype(q)=subtype(p); /* replace count and explicit bit */
  return q;
}
@

@<Hi\TeX\ routines@>=
int hget_disc_no(pointer p)
{ 
  static int rover=0;
  int i;
  for(i=0; i<= max_ref[disc_kind]; i++) 
    { pointer q=dc_defined[rover];
  if ( is_auto_disc(p)==is_auto_disc(q) && replace_count(p)==replace_count(q) &&
       list_equal(pre_break(p),pre_break(q)) &&
       list_equal(post_break(p),post_break(q)))
      return rover;
    else if (rover==0) 
      rover=max_ref[disc_kind];
    else
      rover--;
  }
  if (max_ref[disc_kind]>=0xFF || section_no!=2) return -1;

  rover=++max_ref[disc_kind];
  dc_defined[rover]=copy_disc_node(p);
  @<Allocate font numbers for glyphs in the pre- and post-break lists@>@;
  return rover;
}
@

When we allocate disc numbers we might have fonts inside the pre-
or post-break list, that never show up anywhere else in the content.
These fonts would then be undefined once we start the definition section.
So we have to make sure, all necessary fonts get defined.

@<Allocate font numbers for glyphs in the pre- and post-break lists@>=
ensure_font_no(pre_break(p));
ensure_font_no(post_break(p));
@

\subsubsection{Output}
@<Output discretionary break definitions@>=
  DBG(DBGDEF,"Maximum disc reference: %d\n",max_ref[disc_kind]);
  for (i=0;i<=max_ref[disc_kind]; i++)
           HPUTDEF(hout_disc(dc_defined[i]),i);
@



\subsection{Parameter Lists}\label{paramlist}
\subsubsection{Data}
We store predefined parameter lists in a hash table in order to speed up
finding existing parameter lists. The parameter list itself is stored as
a byte sequence using the short \HINT\ file format.
We link the table entries in order of increasing reference numbers to be able 
to output them in a more ``orderly'' fashion.

@<Hi\TeX\ variables@>=

#define PLH_SIZE 313 /* a prime number $\approx2^8\times 1.2$. */

struct {int l; /* link */
  uint32_t h; /* hash */
  uint32_t n; /* number */
  uint32_t s; /* size */
  uint8_t *p; /* pointer */} pl_defined[PLH_SIZE]={{0}};
int pl_head=-1, *pl_tail=&pl_head;
@

\subsubsection{Allocation}
Next we define three short auxiliar routines and the |hget_param_list_no| function.

@<Hi\TeX\ routines@>=

static uint32_t  hparam_list_hash(list_t *l)
{ uint32_t h=0;
  uint32_t i;
  for (i=0;i<l->s;i++)
    h=3*h+hstart[l->p+i];
  return i;
}

static bool pl_equal(list_t *l, uint8_t *p)
{ uint8_t *q=hstart+l->p;
  uint32_t i;
  for (i=0; i<l->s; i++)
    if (q[i]!=p[i]) return false;
  return true;
}

static void pl_copy(list_t *l, uint8_t *p)
{ uint8_t *q=hstart+l->p;
  memcpy(p,q,l->s);
}

int hget_param_list_no(list_t *l)
{ uint32_t h;
  int i;
  if (l->s<=0) return -1;
  h= hparam_list_hash(l);
  i = h%PLH_SIZE;
  while (pl_defined[i].p!=NULL)
  { if (pl_defined[i].h==h && pl_equal(l,pl_defined[i].p))
      return pl_defined[i].n;
    i=i+199; /* some other prime */
    if (i>=PLH_SIZE) i=i-PLH_SIZE;
  }
  if (max_ref[param_kind]>=0xFF || section_no!=2) return -1;
  pl_defined[i].n=++max_ref[param_kind];
  *pl_tail=i; pl_tail=&(pl_defined[i].l);
  pl_defined[i].l=-1;
  pl_defined[i].h=h;
  pl_defined[i].s=l->s;
  ALLOCATE(pl_defined[i].p,l->s,uint8_t);
  pl_copy(l,pl_defined[i].p);
  return pl_defined[i].n;
}
@

\subsubsection{Output}
To output parameter lists, we need a function to output a parameter node:

@<Hi\TeX\ routines@>=
void hdef_param_node(int ptype, int pnumber,int pvalue)
{
  if (ptype==int_type)
  { if (pvalue==int_defined[hmap_int[pnumber]]) return;
    else HPUTDEF(hput_int(pvalue),hmap_int[pnumber]);
  }
  else if (ptype==dimen_type)
  { if (pvalue==dimen_defined[hmap_dimen[pnumber]]) return;
    else HPUTDEF(hput_dimen(pvalue),hmap_dimen[pnumber]);
  }
  else if (ptype==glue_type)
    { if (glue_equal(pvalue,glue_defined[hmap_glue[pnumber]])) return;
    else HPUTDEF(hout_glue_spec((pointer)pvalue),hmap_glue[pnumber]);
  }
  else QUIT("Unexpected parameter type %d",ptype);
}
@


Now we use the linked list starting with |pl_head| to output the predefined
parameter lists sorted by their reference number.

 @<Output parameter list definitions@>=
  DBG(DBGDEF,"Defining %d parameter lists\n",max_ref[param_kind]+1);
  for (i=pl_head;i>=0;i=pl_defined[i].l)
  { int j;
    DBG(DBGDEF,"Defining parameter list %d, size 0x%x\n",i,pl_defined[i].s);
    j=hsize_bytes(pl_defined[i].s);
    HPUTX(1+1+j+1+pl_defined[i].s+1+j+1);
    HPUTTAG(param_kind,j+1);
    HPUT8(pl_defined[i].n);
    hput_list_size(pl_defined[i].s,j);
    HPUT8(0x100-j);
    memcpy(hpos,pl_defined[i].p,pl_defined[i].s);
    hpos=hpos+pl_defined[i].s;
    HPUT8(0x100-j);
    hput_list_size(pl_defined[i].s,j);
    HPUTTAG(param_kind,j+1);
  }
@

\subsection{Fonts}

It seems I need: (see email by Karl)


|kpse_find_file(name,kpse_fontmap_format,false);|
with name "ps2pk.map" or "psfonts.map" or "ttfonts.map" or cmfonts.map
to get from the tfm name the postscript name.
then I get the |psfont_name| ususaly its the same font name
with .pfb appended
so I can skip it
%#ifdef HAVE_KPSE_ENC_FORMATS
%  enc_file=kpse_find_file(encoding,kpse_enc_format,false);
%#else
%  enc_file=kpse_find_file(encoding,kpse_tex_ps_header_format,false);
%#endif
%reading the encoding is found in dvipng/dvipnd-src/enc.c



\subsubsection{Data}
To store a font definition, we define the data type |font_t|
and an array |hfonts| of pointers indexed by \HINT\ font numbers.
To map \HINT\ font numbers to \TeX\ font numbers, the |font_t| contains
the |i| field; to map \TeX\ font numbers to \HINT\ font numbers,
we use the array |hmap_font|.

@<Hi\TeX\ variables@>=
#define MAX_FONTS 0x100

typedef struct {
  uint8_t i; /* the \TeX\  font number */
  pointer g; /* space glue */
  pointer h; /* default hyphen */
  pointer p[MAX_FONT_PARAMS]; /* font parameters */
  uint16_t m; /* section number of font metric file */
  uint16_t y; /* section number of font glyph file */
} font_t;

static font_t *hfonts[MAX_FONTS]={NULL}; 
static int hmap_font[MAX_FONTS]; 
@



@<Initialize definitions for fonts@>=
  for (i=0;i<0x100;i++) hmap_font[i]=-1;
  max_ref[font_kind]=-1;
@

\subsubsection{Allocation}
Allocation of a |font_t| record takes place when we translate a \TeX\ font
number to a \HINT\ font number using the function |hget_font_no|, and while
doing so discover that the corresponding \HINT\ font number does not yet exist.
Because the |font_t| structure must be initialized after allocating it,
we start with some auxiliar routines for that purpose.

@<Hi\TeX\ auxiliar routines@>=
static pointer find_space_glue(internal_font_number f)
{@+font_index @!main_k;
   pointer main_p=font_glue[f];
if (main_p==null) 
  {@+main_p=new_spec(zero_glue);main_k=param_base[f]+space_code;
  width(main_p)=font_info[main_k].sc; /*that's |space(f)|*/ 
  stretch(main_p)=font_info[main_k+1].sc; /*and |space_stretch(f)|*/ 
  shrink(main_p)=font_info[main_k+2].sc; /*and |space_shrink(f)|*/ 
  font_glue[f]=main_p;
  }
  return main_p; 
}
static pointer hget_font_space(uint8_t f)
{ pointer p;
  if (space_skip==zero_glue) 
	p = find_space_glue(f);
  else 
    p=glue_par(space_skip_code);
  add_glue_ref(p);
  return p;
}


static pointer hget_font_hyphen(uint8_t f)
{ pointer p;
  int c;
  p=new_disc();
  c= hyphen_char[f];
  if (c >= 0 && c < 256) pre_break(p)=new_character(f, c);
  return p;
}


static void hdef_font_params(pointer p[MAX_FONT_PARAMS])
{ /* used only for texts */
}
@
\goodbreak

In the following code, |f| is a \TeX\ internal font number
and |g| is the corresponding \HINT\ font number.
\TeX's null-font, a kind of undefined font containing no characters
is replaced by \HINT's font number zero. Actually the nullfont should
never appear in the output, but if it does so, either an error message
or a more sensible replacement font might be in order.

@<Hi\TeX\ auxiliar routines@>=

static char *hfind_glyphs(char *filename)
{ char *fname=NULL;
  kpse_glyph_file_type file_ret;
  fname=kpse_find_file(filename,kpse_type1_format,true);
  if (fname==NULL) fname=kpse_find_file(filename,kpse_truetype_format,true);
  if (fname==NULL) fname=kpse_find_file(filename,kpse_opentype_format,true);
  if (fname==NULL) fname = kpse_find_glyph(filename, option_dpi,kpse_pk_format, &file_ret);
  if (fname==NULL) 
   fprintf(stderr,"Unable to find glyph data for font %s\n",filename),exit(1);
  return fname;   
}

static uint8_t hget_font_no(uint8_t f)

{ int g;
  char *n,*fn;
  int l;
  if (f==0) 
  { DBG(DBGFONT,"TeX nullfont -> 0\n");
    return 0;@+
  }
  g=hmap_font[f];
  DBG(DBGFONT,"Mapping TeX font %d->%d\n",f,g);
  if (g>=0) return g;
  DBG(DBGDEF,"New TeX font %d\n",f);
  if (max_ref[font_kind]>=0x100)
    QUIT("too many fonts in use");
  g = ++(max_ref[font_kind]);
  ALLOCATE(hfonts[g],1,font_t);
  hfonts[g]->i=f;
  hmap_font[f]=g;
  hfonts[g]->g=hget_font_space(f);
  hfonts[g]->h=hget_font_hyphen(f);
  pack_file_name(font_name[f], "",".tfm");
  n = kpse_find_tfm((char*)name_of_file+1);
  if (n==NULL) 
    QUIT("Unable to find .tfm file for font %s",(char*)name_of_file+1);
  hfonts[g]->m= hnew_file_section(n);
  free(n);
  pack_file_name(font_name[f], "","");
  n= hfind_glyphs((char*)name_of_file+1);
  if (n==NULL) 
    QUIT("Unable to find glyph file for font %s",(char*)name_of_file+1);
  hfonts[g]->y= hnew_file_section(n);
  free(n);
  return g;
}
@

Surprisingly, not all characters that occur in a \HINT\ file are inside the
content section; some characters might hide in the definition section
inside the pre- or post-break list of a predefined discretionary break.
To make sure that the fonts necessary for these characters are included
in the final \HINT\ file, we check these lists to make sure all \TeX\ font
numbers have a corresponting \HINT\ font number.

@<Hi\TeX\ auxiliar routines@>=
static void ensure_font_no(pointer p)
{ while (p!=null)
  { if (is_char_node(p)) 
      hget_font_no(font(p));
    else if (type(p)==hlist_node||type(p)==vlist_node)
      ensure_font_no(list_ptr(p));
    p=link(p);
  }
}
@

\subsubsection{Output}
@<Output font definitions@>=
{ int f;
    DBG(DBGDEF,"Defining %d fonts\n",max_ref[font_kind]+1);
    for (f=0;f<=max_ref[font_kind];f++)
    { font_t *hf=hfonts[f];
      internal_font_number g=hf->i;
      uint32_t pos=hpos-hstart;
      info_t i= b000;
      DBG(DBGDEF,"Defining font %d size 0x%x\n",f,font_size[g]);
      hpos++; HPUTNODE;  /* space for the tag and the node */
      HPUT8(f); /* reference */
      hout_string(font_id_text(g));
      if(font_size[g]>0) HPUT32(font_size[g]);
      else  HPUT32(font_dsize[g]);
      HPUT16(hf->m);HPUT16(hf->y);
      DBG(DBGDEF,"Defining font space\n");
      HPUTCONTENT(hout_glue_spec,hf->g);
      DBG(DBGDEF,"Defining font hyphen\n");
      HPUTCONTENT(hout_disc,hf->h);
      hdef_font_params(hf->p);
      DBG(DBGDEF,"End of font %d\n",f);
      hput_tags(pos,TAG(font_kind,i));
    }
}
@

We used the following function to write a \TeX\ string to the \HINT\ file:
@<Hi\TeX\ auxiliar routines@>=

void hout_string(int s)
{ pool_pointer j;
  uint8_t c;
  j= str_start[s];
  while(j<str_start[s+1])
  { c= so(str_pool[j++]);
    if (c=='%' || c < 0x20 ||c >= 0x7F)
    { char str[4];
      snprintf(str,4,"%%%02X",c);/* convert to printable ASCII */
      HPUTX(3);
      HPUT8(str[0]); HPUT8(str[1]); HPUT8(str[2]);
    }
    else 
    { HPUTX(1);
      HPUT8(c);
    }
  }
  HPUT8(0);
}
@


We used the following macro to add tags around the font glue and the font hyphen:


@<Hi\TeX\ macros@>=

#define HPUTCONTENT(F,D)        \
  { uint8_t *_p;                \
    uint8_t _t;                 \
    HPUTNODE; /* allocate */    \
    _p=hpos++; /* tag */        \
    _t=F(D);                    \
    *_p=_t; DBGTAG(_t,_p);      \
    DBGTAG(_t,hpos); HPUT8(_t); \
  }
@

\subsection{Labels}
The only label that must exist always is the zero label used
to mark the ``home'' position of a document.

\subsubsection{Initialization}
We allocate the zero label with the first call to |next_label|
and initialize is with the value from |label_defaults|.
We then make sure it can be found under the name ``HINT.home''.

@<Initialize definitions for labels@>=
{ char nom[]="HINT.home";
  unsigned int h=name_hash(nom)%LABEL_HASH;
  int i=insert_hash(h,0,nom);
  if (i!=zero_label_no) 
    QUIT("Trying to allocate the zero label, got %d",i);
  labels[zero_label_no]=label_defaults[zero_label_no];
  labels[zero_label_no].next=first_label;
  first_label=zero_label_no;
  DBG(DBGLABEL,"Defining zero label: pos=0x%x\n",labels[zero_label_no].pos);
}
@ 

\subsection{Page Templates}

Once we start producing content nodes, we update the maximum numbers
of page templates and streams from |max_page| and |max_stream|.
These values might have changed because the templates were stored in a
format file.

@<Fix definitions of page templates@>=
max_ref[page_kind]=max_page;
max_ref[stream_kind]=max_stream;
@


\subsubsection{Output}

As part of a page template, we will see stream insertion nodes.
When we encounter an |stream_node| inside a template definition, 
we output a stream insertion point.

@<cases to output whatsit content nodes@>=
     case stream_node:
        HPUT8(setstream_number(p));
        tag=TAG(stream_kind,b100);
        break;
@


@<Output page template definitions@>=
 DBG(DBGDEF,"Maximum page template reference: %d\n",max_page);
 { pointer t;
   for (t=link(setpage_head);t!=null;t=link(t))
   { uint32_t pos=hpos-hstart;
     DBG(DBGDEF,"Defining page template %d\n",setpage_number(i));@/
     hpos++; HPUTNODE;  /* space for the tag and the node */
     HPUT8(setpage_number(t)); 
     hout_string(setpage_name(t));
     HPUT8(setpage_priority(t));
     hout_glue_node(setpage_topskip(t)); 
     hput_dimen(setpage_depth(t));
     hout_xdimen_node(setpage_height(t));
     hout_xdimen_node(setpage_width(t));
     hout_list_node2(setpage_list(t));
     @<output stream definitions@>@;
     hput_tags(pos,TAG(page_kind,0));
   }
}
@
As part of the output of page template definitions, we output
stream definitions:

@<output stream definitions@>=
{ pointer p,q;
  p =  setpage_streams(t);
  while (p!=null)
  { uint8_t n;
    n=setstream_number(p);
    DBG(DBGDEF,"Defining stream %d at 0x%tx\n",n,hpos-hstart);
    HPUTTAG(stream_kind,b100);
    HPUT8(n);
    hout_xdimen_node(setstream_max(p)); /* maximum height */
    HPUT16(setstream_mag(p)); /* factor */
    HPUT8(setstream_prefered(p)); /* prefered */
    HPUT8(setstream_next(p)); /* next */
    HPUT16(setstream_ratio(p)); /* ratio */
    q=setstream_before(p);setstream_before(p)=null;
    hout_list_node2(q);flush_node_list(q);
    hout_xdimen_node(setstream_width(p));
    q=setstream_topskip(p);
    hout_glue_node(q); delete_glue_ref(q);
    q=setstream_after(p);setstream_after(p)=null;
    hout_list_node2(q);flush_node_list(q);
    q=setstream_height(p);
    hout_glue_node(q); delete_glue_ref(q); 
    HPUTTAG(stream_kind,b100);  
    p=link(p);
  }
}
@



\section{\HINT\ Content}\label{output}
\TeX\ puts content nodes on the contribution list and once in a while calls |build_page| to
move nodes from the contribution list to the current page. Hi\TeX\ has a special version of
|build_page| that will simply remove nodes from the contribution list and passe them 
to the function |hout_node|.

\noindent
@<Hi\TeX\ routines@>=
static void hout_node(pointer p)
{ uint32_t pos=hpos-hstart;
  uint8_t tag;
  HPUTNODE;
  hpos++;
  if(is_char_node(p))
    @<output a character node@>@;
  else switch(type(p)) @/
  { @<cases to output content nodes@>@t\1@>@;
    default:
        MESSAGE("\nOutput of node type=%d subtype=%d not implemented\n", type(p),subtype(p));
        display_node(p);
        MESSAGE("End of node");
        hpos--;
        return;@t\2@>@;
  }
  hput_tags(pos,tag);
}
@
@<Hi\TeX\ function declarations@>=
static void hout_node(pointer p);
@

To output \HINT\ nodes, we use the functions defined in {\tt hput.c} from the {\tt shrink} program
(see~\cite{MR:format}).


Let's start with character nodes.

\subsection{Characters}
The processing of a character node consist of three steps: checking for definitions, converting the
\TeX\ node pointed to by |p| to a \HINT\ data type, here a |glyph_t|, and using the
corresponding {\tt hput\_\dots} function to output the node and return the |tag|. 
In the following, we will see the same approach in many
small variations for all kinds of nodes.

@<output a character node@>=
{ glyph_t g;
  g.f=hget_font_no(font(p));
  g.c = character(p);
  tag=hput_glyph(&g);
}
@

\subsection{Penalties}
Integer nodes, which as content nodes are used for penalties, come next.
Except for the embedding between |case| and |break|, the
processing of penalty nodes follows the same pattern we have just seen.
 @<cases to output content nodes@>=
   case penalty_node:
     { int n,i;
       i = penalty(p);
       if (i>10000) i=10000;
       else if (i<-10000) i=-10000;
       n=hget_int_no(i);
       if (n<0) tag=hput_int(i);
       else  { HPUT8(n); tag=TAG(penalty_kind,0);} 
     }
     break;
@

\subsection{Kerns}
The kern nodes of \TeX\ contain a single dimension and a flag to mark ``explicit'' kerns.
 @<cases to output content nodes@>=
   case kern_node:
     { int n;
       n=hget_dimen_no(width(p));
       if (n<0)
       { kern_t k;
         k.x=(subtype(p)==explicit);
         k.d.w=width(p);
         k.d.h=k.d.v=0.0;
         tag=hput_kern(&k);
       }
       else 
       { HPUT8(n);
         if (subtype(p)==explicit) tag=TAG(kern_kind,b100); else tag=TAG(kern_kind,b000);
       }
     }
     break;
@

\subsection{Extended Dimensions}
Extended dimensions do not consitute content on their own, but nodes
containing an extended dimension are part of other nodes. Here we
define an auxiliar function that checks for a predefined extended
dimension and if found outputs the reference number and returns false;
otherwise it outputs the extended dimension and returns true.


@<Hi\TeX\ auxiliar routines@>=
void hout_xdimen_node(pointer p)
{ xdimen_t x;
   x.w=xdimen_width(p);
   x.h=xdimen_hfactor(p)/(double)ONE;
   x.v=xdimen_vfactor(p)/(double)ONE; 
   hput_xdimen_node(&x); 
}
bool hout_xdimen(pointer p)
{int n = hget_xdimen_no(p);
 if (n>=0) { HPUT8(n); return false; @+}
 else
 { hout_xdimen_node(p); return true;@+}
}

@

\subsection{Languages}
We map the language numbers of \TeX\ to \HINT\ language numbers
using the |hlanguage| array.
@<Hi\TeX\ variables@>=
static struct {
  uint8_t n;
  str_number s;
} hlanguage[0x100];
@

For any language number of \TeX, the following function returns
the corresponding \HINT\ language number.
Since \TeX\ knowns about a maximum of 255 languages, there is
no need for overflow checking. The next function writes a language
node to the output stream.

@<Hi\TeX\ auxiliar routines@>=
uint8_t hget_language_no(uint8_t n)
{ int i;
  for (i=0;i<=max_ref[language_kind]; i++)
    if (hlanguage[i].n==n) return i;
  i=++max_ref[language_kind];
  hlanguage[i].n=n;
  hlanguage[i].s=0; /* language unknown*/
  return i;
}

uint8_t hout_language(uint8_t n)
{ n=hget_language_no(n);
  if (n<7) return TAG(language_kind,n+1);
  else 
  { HPUT8(n); return TAG(language_kind,0); }
}
@

After these preparations, the output of a language node is
simple:

@<cases to output whatsit content nodes@>=
case language_node:
  tag=hout_language(what_lang(p));
  break;
@

Normaly \TeX\ does not produce an initial language node and
then the language in the \HINT\ file would not be known until 
it changes for the first time.

@<insert an initial language node@>=
{ uint32_t pos = hpos-hstart;
  hpos++;
  hput_tags(pos,hout_language(language));
}
@

\TeX\ offers currently no simple way to obtain a standardized
language identifier for the current language. So if the
string number of the language is zero, we output the string |"unknown"|;
if somehow the language is known, we output the corresponding string
from \TeX's string pool.

@<Output language definitions@>=
  DBG(DBGDEF,"Maximum language reference: %d\n",max_ref[language_kind]);
  for (i=max_fixed[language_kind]+1;i<=max_ref[language_kind]; i++)
  {  HPUTNODE; 
     HPUT8(TAG(language_kind,0));
     HPUT8(i);
     if (hlanguage[i].s==0)
       hput_string("unknown");
     else
       hout_string(hlanguage[i].s);
     HPUT8(TAG(language_kind,0));
  } 
@



\subsection{Mathematics}
\TeX's math nodes have an optional width---a copy of the mathsurround parameter---while
\HINT\ math nodes do not. Therefore we have to add an explicit kern node if
the width is nonzero. We add it before a ``math on'' node or after a ``math off''
to get the same behavior in respect to line breaking.

@<cases to output content nodes@>=
   case math_node:
     { kern_t k;
       k.x=true;
       k.d.w=width(p);
       k.d.h=k.d.v=0.0;
       if (subtype(p)==before)
       { tag=TAG(math_kind,b111);
         if (width(p)!=0)
 	 { hput_tags(pos,hput_kern(&k));
           pos=hpos-hstart;
           HPUTNODE;
           hpos++;
         }
       }
       else
       { tag=TAG(math_kind,b011);
         if (width(p)!=0)
 	 { hput_tags(pos,tag);
           pos=hpos-hstart;
           HPUTNODE;
           hpos++;
           tag=hput_kern(&k);
         }
       }
     }
     break;
@

\subsection{Glue and Leaders}
Because glue specifications and glue nodes are sometimes part of other
nodes, we start with three auxiliar functions: The first simply
converts a Hi\TeX\ glue node into a \HINT\ |glue_t|, outputs it and
returns the tag; the second checks for predefined glues, and the third
outputs a complete glue node including tags.
@<Hi\TeX\ auxiliar routines@>=

static uint8_t hout_glue_spec(pointer p)
{ @+glue_t g;
  to_glue_t(p,&g);
  return hput_glue(&g);@+
} 

static uint8_t hout_glue(pointer p)
{ int n;
  n = hget_glue_no(p);
  if (n<0)
    return hout_glue_spec(p);
  else 
    {@+ HPUT8(n); return TAG(glue_kind,0);@+}
}

static void hout_glue_node(pointer p)
{ uint8_t *pos;
  uint8_t tag;
  HPUTNODE; /* allocate */
  pos=hpos;  
  hpos++;   /* tag */   
  tag=hout_glue(p);
  *pos=tag; DBGTAG(tag,pos);   
  DBGTAG(tag,hpos); HPUT8(tag);
}
@

Since \TeX\ implements leaders as a kind of glue, we have one case statement covering glue and leaders.

@<cases to output content nodes@>=
  case glue_node:
     if (subtype(p)< cond_math_glue) /* normal glue */
     tag= hout_glue(glue_ptr(p));
     else if (a_leaders<=subtype(p) && subtype(p)<=x_leaders) /*leaders */
     { hout_glue_node(glue_ptr(p));
       { bool outer_doing_leaders=doing_leaders;
         doing_leaders=true;
         hout_node(leader_ptr(p));
         doing_leaders=outer_doing_leaders;
       }
       tag=TAG(leaders_kind,b100|(subtype(p)-a_leaders+1));
     }
     else 
       QUIT("glue subtype %d not implemented\n", subtype(p));
     break;
@

\subsection{Discretionary breaks}
Discretionary breaks are needed in font descriptions (see section~\secref{fontdef}).
Therefore we define a function that converts \TeX's |disc_node| pointers
to \HINT's |disc_t|, outputs the discretionary break, and returns the tag.

@<Hi\TeX\ auxiliar routines@>=

uint8_t hout_disc(pointer p)
{ disc_t h;
  h.x=!is_auto_disc(p);
  h.r=replace_count(p);
  if (h.x) h.r|=0x80;
  if (h.r!=0) HPUT8(h.r);
  if (pre_break(p)==null && post_break(p)==null)
    h.p.s=h.q.s=0;
  else
  { uint32_t lpos;
    lpos=hpos-hstart;
    h.p.k=list_kind;
    hout_list_node(pre_break(p),lpos,&(h.p));
    if (post_break(p)==null)
      h.q.s=0;
    else
    { uint32_t lpos;
      lpos=hpos-hstart;
      h.q.k=list_kind;
      hout_list_node(post_break(p),lpos,&(h.q));
    }
  }
  return hput_disc(&h);
}

@


@<cases to output content nodes@>=
   case disc_node:
     { int n;
       n=hget_disc_no(p);
       if (n<0)
         tag=hout_disc(p);
        else  { HPUT8(n); tag=TAG(disc_kind,0);} 
     }
     break;
@

\subsection{Ligatures}
The subtype giving information on left and right boundary characters
is ignored since the \HINT\ viewer will not do ligature or kerning
programms and neither attempt hyphenation.

@<cases to output content nodes@>=
   case ligature_node:
     { lig_t l;
       pointer q;
       l.f=hget_font_no(font(lig_char(p)));
       HPUT8(l.f);
       l.l.p=hpos-hstart;
       hput_utf8(qo(character(lig_char(p))));
       q=lig_ptr(p);
       while(q> null)
	 { hput_utf8(qo(character(q)));
         q=link(q);
       }
       l.l.s=(hpos-hstart)-l.l.p; 
       tag=hput_ligature(&l);
     }
     break;
@
\subsection{Rules}
@<cases to output content nodes@>=
   case rule_node:
     { rule_t r;
	if (is_running(height(p))) r.h=RUNNING_DIMEN; else r.h=height(p);
	if (is_running(depth(p)))  r.d=RUNNING_DIMEN; else r.d=depth(p);
	if (is_running(width(p)))  r.w=RUNNING_DIMEN; else r.w=width(p);
        tag=hput_rule(&r);       
     }
     break;
@

\subsection{Boxes}
@<cases to output content nodes@>=
    case hlist_node: 
    case vlist_node:
        if (type(p)==hlist_node) tag=TAG(hbox_kind,0);
	else tag=TAG(vbox_kind,0);
        tag |= hput_box_dimen(height(p),depth(p),width(p));
        tag |= hput_box_shift(shift_amount(p));
        tag |= hput_box_glue_set((glue_sign(p)==stretching)?+1:-1,glue_set(p),glue_order(p));
	hout_list_node2(list_ptr(p));
      break;
 @

\subsection{Adjustments}
@<cases to output content nodes@>=
   case adjust_node:
     hout_list_node2(adjust_ptr(p));
     tag=TAG(adjust_kind,1);
     break; 
@


\subsection{Insertions}
\TeX's insertions are mapped to \HINT\ streams.

@<cases to output content nodes@>=
case ins_node:
@<output stream content@>@;
break;
@

Here we consider stream content and come back to stream 
definitions in section~\secref{streamdef}.
In a \HINT\ stream content node the stream 
parameters |floating_penalty|, |split_max_depth|, and
|split_top_skip| are optional. If omitted, the defaults from the stream definition are
used. This is probably also for \TeX\ the most common situation.
It is, however, possible to supply more than one page template with different defaults
and while not very common, \TeX\ might change the parameters at any time.
Because we dont know which is the current page template,
it is not posible to compare the current parameter values against the defaults,
and we have to supply all the parameters always.
In a future version, we might have a \TeX\ primitive that allows us to
signal ``use the defaults''.

@<output stream content@>=
{ int k,n;
  uint32_t pos;
  list_t l;
  info_t i=b000;
  k=subtype(p);
  n=hget_stream_no(k);
  HPUT8(n);
  link(temp_head)=null;
  add_par_node(int_type,floating_penalty_code,float_cost(p));
  add_par_node(dimen_type,split_max_depth_code,depth(p));
  add_par_node(glue_type,split_top_skip_code,split_top_ptr(p));
  pos=hpos-hstart;
  l.k=param_kind;
  n=hout_param_list(link(temp_head),pos,&l);
  flush_node_list(link(temp_head));@+ link(temp_head)=null;
  if (n>=0) HPUT8(n); else i=b010;
  hout_list_node2(ins_ptr(p));
  tag=TAG(stream_kind,i);
}
@


\subsection{Marks}
We currently ignore Marks.

@<cases to output content nodes@>=
    case mark_node: hpos--; return;
@

\subsection{Whatsit Nodes}\label{whatsit}
We have added custom whatsit nodes and now we switch based on the subtype.

@<cases to output content nodes@>=
  case whatsit_node:
    switch(subtype(p))
    { @<cases to output whatsit content nodes@>@;
      default:
        MESSAGE("\nOutput of whatsit nodes subtype=%d not implemented\n", subtype(p));
        display_node(p);
        MESSAGE("End of node");
        hpos--;
        return;
    }
    break;
@

For \TeX's whatsit nodes that handle output files, no code is generated;
hence, we call |out_what| and simply remove the tag byte that is already 
in the output.
When the \.{\\write} node arrives here, it is normaly handled
in |hlist_out| or |vlist_out| in an environment determined by
the output routine. For example \LaTeX\ redefines \.{\\protect}
as \.{\\noexpand} and these redefinitions need to be made
before calling |out_what| which expands the token list.
We should therefore add the definitions contained in the output routine
to mimic expanding inside an output routine.


@<cases to output whatsit content nodes@>=
     case open_node:
     case write_node:
     case close_node: out_what(p);
     case special_node: hpos--; return;
@ 

\subsection{Paragraphs}
When we output a paragraph node, we have to consider a special case:
The parameter list is given by a reference number but the extended dimension
needs an |xdimen| node. In this case the reference number for the parameter
list comes first, while otherwise the extended dimension would come first.
To determine whether ther is a reference number for the parameter list,
the function |hout_param_list| is writing the parameter list to the output.
\noindent
@<cases to output whatsit content nodes@>=
case graf_node:
      { uint32_t pos, xpos, xsize;
        list_t l;
        pointer q;
        int n,m;
        info_t i=b000;
        q=graf_extent(p);
        n=hget_xdimen_no(q);
        if (n>=0) HPUT8(n); 
	else 
        { xpos=hpos-hstart; hout_xdimen_node(p); xsize=(hpos-hstart)-xpos; i|=b100; }
        pos=hpos-hstart;
        l.k=param_kind;
	m=hout_param_list(graf_params(p),pos,&l);
        if (m>=0) 
        { if (i&b100)
          { HPUTX(1);
            memmove(hstart+xpos+1,hstart+xpos,xsize);
	    hpos++;
            hstart[xpos]=m;
          }
          else 
            HPUT8(m); 
        }
        else i|=b010;
        pos=hpos-hstart;
        l.k=list_kind;
        hout_list_node(graf_list(p),pos,&l);
        tag=TAG(par_kind,i);
      }
    break;
@

\subsection{Baseline Skips}
@<cases to output whatsit content nodes@>=
    case baseline_node:
      { int n;
        n= baseline_node_no(p);
        if (n>0xFF) tag=hout_baselinespec(n);
	else 
        { HPUT8(n);
          tag=TAG(baseline_kind,b000);
        }
      }
      break;
  @

\subsection{Displayed Equations}
@<cases to output whatsit content nodes@>=
      case disp_node:
	{ uint32_t pos;
          list_t l;
          int n;
          info_t i=b000;
          pos=hpos-hstart;
          l.k=param_kind;
	  n=hout_param_list(display_params(p),pos,&l);
          if (n>=0) HPUT8(n); else i|=b100;
          if (display_eqno(p)!=null && display_left(p))
	  { hout_node(display_eqno(p)); i|=b010; }
          pos=hpos-hstart;
          l.k=list_kind;
          hout_list_node(display_formula(p),pos,&l);
          if (display_eqno(p)!=null && !display_left(p))
	  { hout_node(display_eqno(p)); i|=b001; }
          tag=TAG(math_kind,i);
          /* the |display_no_bs(p)| tells whether the baseline skip is ignored */ 
	}
         break; 
@
\subsection{Extended Boxes}
When we output an extended box, we have to consider a special case: the page templates.
Page templates are boxes that contain insertion points. These insertion points look
like regular insertions but with an empty content list. As a result the |hpack| and
|vpackage| routines might belive that they can compute the dimensions of the box
conntent when in fact they can not. 


@<cases to output whatsit content nodes@>=
   case hset_node:
   case vset_node:
        { kind_t k= subtype(p)==hset_node?hset_kind:vset_kind;
          info_t i=b000;
          stretch_t s;
          int n=set_extent(p);
          i|=hput_box_dimen(height(p),depth(p),width(p));
          i|=hput_box_shift(shift_amount(p));
          s.f=set_stretch(p)/(double)ONE; s.o=set_stretch_order(p);
          hput_stretch(&s);
          s.f=set_shrink(p)/(double)ONE; s.o=set_shrink_order(p);
          hput_stretch(&s);
          if (hout_xdimen(n)) i|=b001;
	  hout_list_node2(list_ptr(p)); 
          tag=TAG(k,i);
        }
        break;
@

@<cases to output whatsit content nodes@>=
      case hpack_node:
      case vpack_node:
        { kind_t k= (subtype(p)==hpack_node?hpack_kind:vpack_kind);
          info_t i=b000;
          int n=pack_extent(p);
          if (pack_m(p)==additional) i|=b001;
          if (shift_amount(p)!=0) { HPUT32(shift_amount(p)); i|=b010; }
          if (k==vpack_kind) HPUT32(pack_limit(p));
          if (hout_xdimen(n)) i|=b100;
	  hout_list_node2(list_ptr(p));
          tag=TAG(k,i);
        }
        break;
@

\subsection{Extended Alignments}
@<cases to output whatsit content nodes@>=
case align_node:
  { info_t i=b000;
    if (align_m(p)==additional) i|=b001;
    if (align_v(p)) i|=b010;
    if (hout_xdimen(align_extent(p))) i|=b100;
    hout_preamble(align_preamble(p));
    hout_align_list(align_list(p),align_v(p)); 
    tag=TAG(table_kind,i);
    }
break;        
@

In the preamble, we remove the unset nodes and retain only the list of tabskip glues.
@<Hi\TeX\ auxiliar routines@>=
void hout_preamble(pointer p)
{ pointer q,r;
  DBG(DBGBASIC,"Writing Preamble\n");
  q=p;
  if (q!=null) r=link(q); else r=null;
  while (r!=null)
  { if (type(r)==unset_node)
      { link(q)=link(r);
        link(r)=null; flush_node_list(r);
      }
      else
        q=r;
      r=link(q);
  }
  hout_list_node2(p); 
  DBG(DBGBASIC,"End Preamble\n");
}
@

In the |align_list| we have to convert the unset nodes back to box nodes or extended box nodes
packaged inside an item node.
When the viewer reads an item node, it will package the extended boxes to their natural size.
This is the size that is needed to compute the maximum width of a column.

@<Hi\TeX\ auxiliar routines@>=

static void hout_item(pointer p, uint8_t t, uint8_t s)
{ info_t i=b000;
  uint8_t n;
   n=span_count(p)+1;
  DBG(DBGBASIC,"Writing Item %d/%d->%d/%d\n",type(p),n,t,s);
  display_node(p);
  if (n==0) QUIT("Span count of item must be positive");
  if (n<7) i=n; else i=7;
  HPUTTAG(item_kind,i);
  if (i==7) HPUT8(n);
  type(p)=t; subtype(p)=s;
  hout_node(p);
  HPUTTAG(item_kind,i);
  DBG(DBGBASIC,"End Item\n");
}


static void hout_item_list(pointer p, bool v)
{ list_t l;
  uint32_t pos;
  DBG(DBGBASIC,"Writing Item List\n");
  l.k=list_kind; 
  HPUTTAG(item_kind,b000);
  pos=hpos-hstart;
  HPUTX(2);
  HPUT8(0); /* space for the list tag */
  HPUT8(0); /* space for the list size */
  l.p=hpos-hstart;
  while(p> mem_min)
  { if (is_char_node(p))   hout_node(p);
    else if (type(p)==unset_node) hout_item(p,v?vlist_node:hlist_node,0);
    else if (type(p)==unset_set_node) hout_item(p,whatsit_node,v?vset_node:hset_node);
    else if (type(p)==unset_pack_node) hout_item(p,whatsit_node,v?vpack_node:hpack_node);
    else hout_node(p);
    p=link(p);
  }
  l.s=(hpos-hstart)-l.p;
  hput_tags(pos,hput_list(pos+1,&l));
  HPUTTAG(item_kind,b000);
  DBG(DBGBASIC,"End Item List\n");
}

void hout_align_list(pointer p, bool v)
{ list_t l; 
  uint32_t pos;
  DBG(DBGBASIC,"Writing Align List\n");
  l.k=list_kind; 
  pos=hpos-hstart;
  HPUTX(2);
  HPUT8(0); /* space for the tag */
  HPUT8(0); /* space for the list size */
  l.p=pos+2;
  while(p> mem_min)
  { if (!is_char_node(p) && (type(p)==unset_node||type(p)==unset_set_node||type(p)==unset_pack_node))
      hout_item_list(list_ptr(p),v);
    else
      hout_node(p);
    p=link(p);
  }
  l.s=(hpos-hstart)-l.p;
  hput_tags(pos,hput_list(pos+1,&l));
  DBG(DBGBASIC,"End Align List\n");
}
@

Inside the alignment list we will find various types of unset nodes, we convert them back
to regular nodes and put them inside an item node.

@<cases to output content nodes@>=
case unset_node: 
case unset_set_node: 
case unset_pack_node: 


@ 

\subsection{Lists}
Two functions are provided here: |hout_list| will write a list given by the pointer |p| to the
output at the current position |hpos|. After the list has finished, it will move the list, if necessary,
and add the size information so that the final list will be at position |pos|;
|hout_list_node| uses |hout_list| but adds the tags to form a complete node.


@<Hi\TeX\ routines@>=

static uint8_t hout_list(pointer p, uint32_t pos, list_t *l)
{ l->p=hpos-hstart;
  while(p> mem_min)
  { hout_node(p);
    p=link(p);
  }
  l->s=(hpos-hstart)-l->p;
  return hput_list(pos,l);
}

static void hout_list_node(pointer p, uint32_t pos, list_t *l)
/* p is a pointer to a node list, 
   output the node list at position pos (thats where the tag goes)
   using l->k as list kind,
   and set l->p and l->s.
*/
{ 
  hpos=hstart+pos;
  HPUTX(3);
  HPUT8(0); /* space for the tag */
  HPUT8(0); /* space for the list size */
  HPUT8(0); /* space for the size boundary byte */
  hput_tags(pos,hout_list(p,pos+1,l));
}


static void hout_list_node2(pointer p)
{ list_t l;
  uint32_t pos;
  pos=hpos-hstart;
  l.k=list_kind;
  hout_list_node(p,pos,&l);
}
@

@<Hi\TeX\ function declarations@>=
static void hout_list_node(pointer p, uint32_t pos, list_t *l);
static void hout_list_node2(pointer p);
static uint8_t hout_list(pointer p, uint32_t pos, list_t *l);
@

\subsection{Parameter Lists}

The next function is like |hout_list_node| but restricted to parameter nodes.

@<Hi\TeX\ routines@>=

static int hout_param_list(pointer p, uint32_t pos, list_t *l)
/* p is a pointer to a param node list,
   either find a reference number to a predefined parameter list 
   and return the reference number or 
   output the node list at position pos (thats where the tag goes)
   and set l->k, l->p and l->s,
   and return -1;
*/
{ int n;
  hpos=hstart+pos;
  if (p==null) 
  {HPUTX(2); hpos++;hput_tags(pos,TAG(param_kind,1)); l->s=0; return -1;}
  HPUTX(3);
  HPUT8(0); /* space for the tag */
  HPUT8(0); /* space for the list size */
  HPUT8(0); /* space for the size boundary byte*/
  l->p=hpos-hstart;
  while(p> mem_min)
  { hdef_param_node(par_type(p),par_number(p),par_value(p).i);
    p=link(p);
  }
  l->s=(hpos-hstart)-l->p;
  n=hget_param_list_no(l);
  if (n>=0)
    hpos=hstart+pos;  
  else 
    hput_tags(pos,hput_list(pos+1,l));
  return n;
}
@

@<Hi\TeX\ function declarations@>=
static int hout_param_list(pointer p, uint32_t pos, list_t *l);
@

\subsection{Labels, Links, and Outlines}
\indent
@<cases to output whatsit content nodes@>=
case label_node:  hpos--; new_label(p); return;
case start_link_node:
{ info_t i;
  int n=new_start_link(p);
  i=b010;
  if (n>0xFF) { i|=b001; HPUT16(n);@+} @+else HPUT8(n);
  tag= TAG(link_kind,i);
}
break;
case end_link_node:
{ info_t i;
  int n=new_end_link();
  i=b000;
  if (n>0xFF) { i|=b001; HPUT16(n);@+} @+else HPUT8(n);
  tag= TAG(link_kind,i);
}
break;
case outline_node: hpos--; new_outline(p);  return;
@

The routines to put labels and outlines into the definition section
are defined in {\tt hput.c}.

\subsection{Images}
\indent
@<cases to output whatsit content nodes@>=
     case image_node:
        { image_t i;
          i.n=image_no(p);
          i.w=image_width(p);
	  i.h=image_height(p);
	  i.p.f=image_stretch(p)/(double)ONE;
	  i.p.o=image_stretch_order(p);
	  i.m.f=image_shrink(p)/(double)ONE;
	  i.m.o=image_shrink_order(p);
          tag=hput_image(&i);
	}
        break;
@



\subsection{Text}
The routines in this section are not yet ready.

@<Hi\TeX\ routines@>=

#if 0
static void hchange_text_font(internal_font_number f)
{ uint8_t g;   
  if (f!=hfont) 
  { g=get_font_no(f);
    if (g<8) 
	  hputcc(FONT0_CHAR+g);
	else 
	{ hputcc(FONTN_CHAR);
	  hputcc(g);
	}
	hfont=f;
  }
}

static void hprint_text_char(pointer p)
{ uint8_t f,c; 
  f = font(p);
  c = character(p);
  hchange_text_font(f);
  if (c<=SPACE_CHAR) hputcc(ESC_CHAR);
  hputcc(c);
}


static void hprint_text_node(pointer p)
{ switch(type(p))
  { case hlist_node: 
      /* this used to be the |par_indent| case */
      goto nodex;
    case glue_node:
	  if (subtype(p)>= cond_math_glue) goto nodex;
	  else 
      { pointer q=glue_ptr(p);
		int i;
        if (glue_equal(f_space_glue[hfont],q)) 
	    { hputc(SPACE_CHAR); return; }
		if (glue_equal(f_xspace_glue[hfont],q)) 
	    { hputcc(XSPACE_CHAR); return; }
		if (f_1_glue[hfont]==0 && (subtype(p)-1==space_skip_code))
		{ pointer r=glue_par(subtype(p)-1);
          add_glue_ref(r);
          f_1_glue[hfont]=r;
		}
		if (f_1_glue[hfont]!=0 && glue_equal(f_1_glue[hfont],q)) 
	    { hputcc(GLUE1_CHAR); return; }
		if (f_2_glue[hfont]==0 && (subtype(p)-1==space_skip_code || subtype(p)-1==xspace_skip_code)) 
		{ pointer r=glue_par(subtype(p)-1);
          add_glue_ref(r);
          f_2_glue[hfont]=r;
		}
		if (f_2_glue[hfont]!=0 && glue_equal(f_2_glue[hfont],q)) 
	    { hputcc(GLUE2_CHAR); return; }
		if (f_3_glue[hfont]==0) 
		{ f_3_glue[hfont]=q;
		  add_glue_ref(q);
		}
		if (f_3_glue[hfont]!=0 && glue_equal(f_3_glue[hfont],q)) 
	    { hputcc(GLUE3_CHAR); return; }
        i = hget_glue_no(q);
        if (i>=0) 
		{ hputcc(GLUEN_CHAR); hputcc(i); return; }
      }
      break;     
	case ligature_node:
	{ int n;
	  pointer q;
	  for (n=0,q=lig_ptr(p); n<5 && q!=null; n++,q=link(q)) continue;
	  if (n==2) hputcc(LIG2_CHAR);
	  else if (n==3) hputcc(LIG3_CHAR);
	  else if (n==0) hputcc(LIG0_CHAR);
	  else goto nodex;
      hprint_text_char(lig_char(p));
	  for (q=lig_ptr(p);q!=null; q=link(q)) hprint_text_char(q);
      return;
	}
    case disc_node:
      if (post_break(p)==null && pre_break(p)!=null && replace_count(p)==0)
	  { pointer q;
	    q=pre_break(p);
	    if (is_char_node(q) && link(q)==null && font(q)==hfont && character(q)== hyphen_char[hfont])
	    { if (is_auto_disc(p)) hputcc(DISC1_CHAR); 
		  else hputcc(DISC2_CHAR); 
		  return; 
		}
	  }
	  else if (post_break(p)==null && pre_break(p)==null && replace_count(p)==0 && !is_auto_disc(p))
	  { hputcc(DISC3_CHAR); return; }
      break;
	case math_node:
      if(width(p)!=0) goto  nodex;
      if(subtype(p)==before) hputcc(MATHON_CHAR);
      else hputcc(MATHOFF_CHAR);
      return;
	default:
	  break;
  }
nodex:
  hout_node(p);
}

static void hprint_text(pointer p)
{ internal_font_number f=hfont;
  nesting++;
  hprint_nesting();
  hprintf("<text ");
  while(p> mem_min)
  { if (is_char_node(p))
      hprint_text_char(p);
	else
      hprint_text_node(p);
    p=link(p);
  }
  hchange_text_font(f);
  hprintf(">\n");
  nesting--;
}
#endif

@


\section{\HINT\ Limitations}
\itemize
\item Kerns and glues using a width that depends on \.{\\hsize} or
      \.{\\vsize} are not yet supported.
\item Tables where the width of a column depends on \.{\\hsize} or
      \.{\\vsize} are not tested and probably not yet supported.
\item The encoding of horizontal lists as texts is not yet supported,
      but it would make the \HINT\ file shorter and much better to read
      when stretched into long \HINT\ format.
\enditemize


\appendix

\section{Source Files}

\subsection{Basic types}
To define basic types we use the file {\tt bastypes.h} from~\cite{MR:format}.

\subsection{Hi\TeX\ routines: \tt hitex.c}
\indent
@p
#include <stdio.h>
#include <kpathsea/kpathsea.h>
#include "basetypes.h"
#include "error.h"
#include "hformat.h"
#include "hput.h"
#include "htex.h"
#include "hitex.h"

/* from ktex.ch */

@<Hi\TeX\ macros@>@;
@<Hi\TeX\ variables@>@;
@<Hi\TeX\ function declarations@>@;

@<Hi\TeX\ auxiliar routines@>@;

@<Hi\TeX\ routines@>@;

@


\subsection{Hi\TeX\ prototypes: \tt hitex.h}
@(hitex.h@>=
extern void hint_open(void);
extern void hint_close(void);@#

extern pointer new_xdimen(scaled w, scaled h, scaled v);
extern void hget_image_information(pointer p);
extern pointer new_baseline_node(pointer bs, pointer ls, scaled lsl);
extern pointer new_set_node(void);
extern void hline_break(int final_widow_penalty);
extern pointer new_disp_node(void);
extern void add_par_node(eight_bits t, eight_bits n, int v);
extern pointer new_image_node(str_number n, char *a, char *e);
extern pointer  new_setpage_node(eight_bits k, str_number n);
extern int hget_stream_no(int i);
extern pointer new_setstream_node(eight_bits n);
extern void hfinish_stream_group(void);
extern void hfinish_page_group(void);
extern void hfinish_stream_before_group(void);
extern void hfinish_stream_after_group(void);
extern void hfinish_outline_group(void);
extern void execute_output(pointer p);
extern void hint_debug_help(void);
@

\crosssections

\plainsection{References}

{\baselineskip=11pt
\def\bfblrm{\small\rm}%
\def\bblem{\small\it}%
\bibliography{../hint}
\bibliographystyle{plain}
}

\plainsection{Index}
{
\def\_{{\tt \UL}} % underline in a string
\catcode`\_=\active \let_=\_ % underline is a letter
\input hitex.ind
}

  \write\cont{} % ensure that the contents file isn't empty
%  \write\cont{\catcode `\noexpand\@=12\relax}   % \makeatother
  \closeout\cont% the contents information has been fully gathered
