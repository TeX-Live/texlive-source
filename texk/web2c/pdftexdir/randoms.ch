% randoms.ch
% Copyright (c) 2005 Han Th\^e\llap{\raise 0.5ex\hbox{\'{}}} Th\`anh, <thanh@pdftex.org>
%
% This file is part of pdfTeX.
%
% pdfTeX is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% pdfTeX is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with pdfTeX; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%
% $Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftex.ch#163 $
%
% This is a WEB change file for pseudo-random numbers in pdftex 1.30 and above.
%
% There are four new primitives:
%
% \pdfuniformdeviate <count>
%   Generates a uniformly distributed random integer value
%   between 0 (inclusive) and <count> (exclusive).
%   This primitive expands to a list of tokens.
%
% \pdfnormaldeviate
%   Expands to a random integer value with a mean of 0 and a 
%   unit of 65536. 
%   This primitive expands to a list of tokens.
%
% \pdfrandomseed 
%   You can use \the\pdfrandomseed to query the current seed value,
%   so you can e.g. the value to the log file.
%
%   The initial value of the seed is derived from the system time, 
%   and is not more than 1,000,999,999 (this ensures that the value
%   can be used with commands like \count).
%
% \pdfsetrandomseed <count>
%   This sets the random seed to a specific value, allowing you
%   to re-play sequences of semi-randoms at a later moment. 
%
% Most of the actual code is taken from metapost, and originally
% written by Knuth, for Metafont. Glue to make it work in TeX is
% by me. If you find an error, it is bound to be in my code,
% not Knuth's :-)
% 
% Taco Hoekwater (taco@metatex.org), june 27, 2005. No restrictions.

@x
@* \[8] Packed data.
@y
@* \[7b] Random numbers.

\font\tenlogo=logo10 % font used for the METAFONT logo
\def\MP{{\tenlogo META}\-{\tenlogo POST}}

This section is (almost) straight from MetaPost. I had to change
the types (use |integer| instead of |fraction|), but that should
not have any influence on the actual calculations (the original
comments refer to quantities like |fraction_four| ($2^{30}$), and 
that is the same as the numeric representation of |maxdimen|).

I've copied the low-level variables and routines that are needed, but 
only those (e.g. |m_log|), not the accompanying ones like |m_exp|. Most
of the following low-level numeric routines are only needed within the
calculation of |norm_rand|. I've been forced to rename |make_fraction|
to |make_frac| because TeX already has a routine by that name with
a wholly different function (it creates a |fraction_noad| for math
typesetting) -- Taco

And now let's complete our collection of numeric utility routines
by considering random number generation.
\MP\ generates pseudo-random numbers with the additive scheme recommended
in Section 3.6 of {\sl The Art of Computer Programming}; however, the
results are random fractions between 0 and |fraction_one-1|, inclusive.

There's an auxiliary array |randoms| that contains 55 pseudo-random
fractions. Using the recurrence $x_n=(x_{n-55}-x_{n-31})\bmod 2^{28}$,
we generate batches of 55 new $x_n$'s at a time by calling |new_randoms|.
The global variable |j_random| tells which element has most recently
been consumed.

@<Glob...@>=
@!randoms:array[0..54] of integer; {the last 55 random values generated}
@!j_random:0..54; {the number of unused |randoms|}
@!random_seed:scaled; {the default random seed}

@ A small bit of metafont is needed.

@d fraction_half==@'1000000000 {$2^{27}$, represents 0.50000000}
@d fraction_one==@'2000000000 {$2^{28}$, represents 1.00000000}
@d fraction_four==@'10000000000 {$2^{30}$, represents 4.00000000}
@d el_gordo == @'17777777777 {$2^{31}-1$, the largest value that \MP\ likes}
@d halfp(#)==(#) div 2
@d double(#) == #:=#+# {multiply a variable by two}

@ The |make_frac| routine produces the |fraction| equivalent of
|p/q|, given integers |p| and~|q|; it computes the integer
$f=\lfloor2^{28}p/q+{1\over2}\rfloor$, when $p$ and $q$ are
positive. If |p| and |q| are both of the same scaled type |t|,
the ``type relation'' |make_frac(t,t)=fraction| is valid;
and it's also possible to use the subroutine ``backwards,'' using
the relation |make_frac(t,fraction)=t| between scaled types.

If the result would have magnitude $2^{31}$ or more, |make_frac|
sets |arith_error:=true|. Most of \MP's internal computations have
been designed to avoid this sort of error.

If this subroutine were programmed in assembly language on a typical
machine, we could simply compute |(@t$2^{28}$@>*p)div q|, since a
double-precision product can often be input to a fixed-point division
instruction. But when we are restricted to \PASCAL\ arithmetic it
is necessary either to resort to multiple-precision maneuvering
or to use a simple but slow iteration. The multiple-precision technique
would be about three times faster than the code adopted here, but it
would be comparatively long and tricky, involving about sixteen
additional multiplications and divisions.

This operation is part of \MP's ``inner loop''; indeed, it will
consume nearly 10\pct! of the running time (exclusive of input and output)
if the code below is left unchanged. A machine-dependent recoding
will therefore make \MP\ run faster. The present implementation
is highly portable, but slow; it avoids multiplication and division
except in the initial stage. System wizards should be careful to
replace it with a routine that is guaranteed to produce identical
results in all cases.
@^system dependencies@>

As noted below, a few more routines should also be replaced by machine-dependent
code, for efficiency. But when a procedure is not part of the ``inner loop,''
such changes aren't advisable; simplicity and robustness are
preferable to trickery, unless the cost is too high.
@^inner loop@>

@p function make_frac(@!p,@!q:integer):integer;
var @!f:integer; {the fraction bits, with a leading 1 bit}
@!n:integer; {the integer part of $\vert p/q\vert$}
@!negative:boolean; {should the result be negated?}
@!be_careful:integer; {disables certain compiler optimizations}
begin if p>=0 then negative:=false
else  begin negate(p); negative:=true;
  end;
if q<=0 then
  begin debug if q=0 then confusion("/");@;@+gubed@;@/
@:this can't happen /}{\quad \./@>
  negate(q); negative:=not negative;
  end;
n:=p div q; p:=p mod q;
if n>=8 then
  begin arith_error:=true;
  if negative then make_frac:=-el_gordo@+else make_frac:=el_gordo;
  end
else  begin n:=(n-1)*fraction_one;
  @<Compute $f=\lfloor 2^{28}(1+p/q)+{1\over2}\rfloor$@>;
  if negative then make_frac:=-(f+n)@+else make_frac:=f+n;
  end;
end;

@ The |repeat| loop here preserves the following invariant relations
between |f|, |p|, and~|q|:
(i)~|0<=p<q|; (ii)~$fq+p=2^k(q+p_0)$, where $k$ is an integer and
$p_0$ is the original value of~$p$.

Notice that the computation specifies
|(p-q)+p| instead of |(p+p)-q|, because the latter could overflow.
Let us hope that optimizing compilers do not miss this point; a
special variable |be_careful| is used to emphasize the necessary
order of computation. Optimizing compilers should keep |be_careful|
in a register, not store it in memory.
@^inner loop@>

@<Compute $f=\lfloor 2^{28}(1+p/q)+{1\over2}\rfloor$@>=
f:=1;
repeat be_careful:=p-q; p:=be_careful+p;
if p>=0 then f:=f+f+1
else  begin double(f); p:=p+q;
  end;
until f>=fraction_one;
be_careful:=p-q;
if be_careful+p>=0 then incr(f)

@ 

@p function take_frac(@!q:integer;@!f:integer):integer;
var @!p:integer; {the fraction so far}
@!negative:boolean; {should the result be negated?}
@!n:integer; {additional multiple of $q$}
@!be_careful:integer; {disables certain compiler optimizations}
begin @<Reduce to the case that |f>=0| and |q>0|@>;
if f<fraction_one then n:=0
else  begin n:=f div fraction_one; f:=f mod fraction_one;
  if q<=el_gordo div n then n:=n*q
  else  begin arith_error:=true; n:=el_gordo;
    end;
  end;
f:=f+fraction_one;
@<Compute $p=\lfloor qf/2^{28}+{1\over2}\rfloor-q$@>;
be_careful:=n-el_gordo;
if be_careful+p>0 then
  begin arith_error:=true; n:=el_gordo-p;
  end;
if negative then take_frac:=-(n+p)
else take_frac:=n+p;
end;

@ @<Reduce to the case that |f>=0| and |q>0|@>=
if f>=0 then negative:=false
else  begin negate(f); negative:=true;
  end;
if q<0 then
  begin negate(q); negative:=not negative;
  end;

@ The invariant relations in this case are (i)~$\lfloor(qf+p)/2^k\rfloor
=\lfloor qf_0/2^{28}+{1\over2}\rfloor$, where $k$ is an integer and
$f_0$ is the original value of~$f$; (ii)~$2^k\L f<2^{k+1}$.
@^inner loop@>

@<Compute $p=\lfloor qf/2^{28}+{1\over2}\rfloor-q$@>=
p:=fraction_half; {that's $2^{27}$; the invariants hold now with $k=28$}
if q<fraction_four then
  repeat if odd(f) then p:=halfp(p+q)@+else p:=halfp(p);
  f:=halfp(f);
  until f=1
else  repeat if odd(f) then p:=p+halfp(q-p)@+else p:=halfp(p);
  f:=halfp(f);
  until f=1

@ The subroutines for logarithm and exponential involve two tables.
The first is simple: |two_to_the[k]| equals $2^k$. The second involves
a bit more calculation, which the author claims to have done correctly:
|spec_log[k]| is $2^{27}$ times $\ln\bigl(1/(1-2^{-k})\bigr)=
2^{-k}+{1\over2}2^{-2k}+{1\over3}2^{-3k}+\cdots\,$, rounded to the
nearest integer.

@<Glob...@>=
@!two_to_the:array[0..30] of integer; {powers of two}
@!spec_log:array[1..28] of integer; {special logarithms}


@ @<Set init...@>=
two_to_the[0]:=1;
for k:=1 to 30 do two_to_the[k]:=2*two_to_the[k-1];
spec_log[1]:=93032640;
spec_log[2]:=38612034;
spec_log[3]:=17922280;
spec_log[4]:=8662214;
spec_log[5]:=4261238;
spec_log[6]:=2113709;
spec_log[7]:=1052693;
spec_log[8]:=525315;
spec_log[9]:=262400;
spec_log[10]:=131136;
spec_log[11]:=65552;
spec_log[12]:=32772;
spec_log[13]:=16385;
for k:=14 to 27 do spec_log[k]:=two_to_the[27-k];
spec_log[28]:=1;

@ 

@p function m_log(@!x:integer):integer;
var @!y,@!z:integer; {auxiliary registers}
@!k:integer; {iteration counter}
begin if x<=0 then @<Handle non-positive logarithm@>
else  begin y:=1302456956+4-100; {$14\times2^{27}\ln2\approx1302456956.421063$}
  z:=27595+6553600; {and $2^{16}\times .421063\approx 27595$}
  while x<fraction_four do
    begin double(x); y:=y-93032639; z:=z-48782;
    end; {$2^{27}\ln2\approx 93032639.74436163$
      and $2^{16}\times.74436163\approx 48782$}
  y:=y+(z div unity); k:=2;
  while x>fraction_four+4 do
    @<Increase |k| until |x| can be multiplied by a
      factor of $2^{-k}$, and adjust $y$ accordingly@>;
  m_log:=y div 8;
  end;
end;

@ @<Increase |k| until |x| can...@>=
begin z:=((x-1) div two_to_the[k])+1; {$z=\lceil x/2^k\rceil$}
while x<fraction_four+z do
  begin z:=halfp(z+1); k:=k+1;
  end;
y:=y+spec_log[k]; x:=x-z;
end

@ @<Handle non-positive logarithm@>=
begin print_err("Logarithm of ");
@.Logarithm...replaced by 0@>
print_scaled(x); print(" has been replaced by 0");
help2("Since I don't take logs of non-positive numbers,")@/
  ("I'm zeroing this one. Proceed, with fingers crossed.");
error; m_log:=0;
end

@ The following somewhat different subroutine tests rigorously if $ab$ is
greater than, equal to, or less than~$cd$,
given integers $(a,b,c,d)$. In most cases a quick decision is reached.
The result is $+1$, 0, or~$-1$ in the three respective cases.

@d return_sign(#)==begin ab_vs_cd:=#; return;
  end

@p function ab_vs_cd(@!a,b,c,d:integer):integer;
label exit;
var @!q,@!r:integer; {temporary registers}
begin @<Reduce to the case that |a,c>=0|, |b,d>0|@>;
loop@+  begin q := a div d; r := c div b;
  if q<>r then
    if q>r then return_sign(1)@+else return_sign(-1);
  q := a mod d; r := c mod b;
  if r=0 then
    if q=0 then return_sign(0)@+else return_sign(1);
  if q=0 then return_sign(-1);
  a:=b; b:=q; c:=d; d:=r;
  end; {now |a>d>0| and |c>b>0|}
exit:end;

@ @<Reduce to the case that |a...@>=
if a<0 then
  begin negate(a); negate(b);
  end;
if c<0 then
  begin negate(c); negate(d);
  end;
if d<=0 then
  begin if b>=0 then
    if ((a=0)or(b=0))and((c=0)or(d=0)) then return_sign(0)
    else return_sign(1);
  if d=0 then
    if a=0 then return_sign(0)@+else return_sign(-1);
  q:=a; a:=c; c:=q; q:=-b; b:=-d; d:=q;
  end
else if b<=0 then
  begin if b<0 then if a>0 then return_sign(-1);
  if c=0 then return_sign(0) else return_sign(-1);
  end

@ To consume a random integer, the program below will say `|next_random|'
and then it will fetch |randoms[j_random]|.

@d next_random==if j_random=0 then new_randoms
  else decr(j_random)

@p procedure new_randoms;
var @!k:0..54; {index into |randoms|}
@!x:integer; {accumulator}
begin for k:=0 to 23 do
  begin x:=randoms[k]-randoms[k+31];
  if x<0 then x:=x+fraction_one;
  randoms[k]:=x;
  end;
for k:=24 to 54 do
  begin x:=randoms[k]-randoms[k-24];
  if x<0 then x:=x+fraction_one;
  randoms[k]:=x;
  end;
j_random:=54;
end;

@ To initialize the |randoms| table, we call the following routine.

@p procedure init_randoms(@!seed:integer);
var @!j,@!jj,@!k:integer; {more or less random integers}
@!i:0..54; {index into |randoms|}
begin j:=abs(seed);
while j>=fraction_one do j:=halfp(j);
k:=1;
for i:=0 to 54 do
  begin jj:=k; k:=j-k; j:=jj;
  if k<0 then k:=k+fraction_one;
  randoms[(i*21)mod 55]:=j;
  end;
new_randoms; new_randoms; new_randoms; {``warm up'' the array}
end;

@ To produce a uniform random number in the range |0<=u<x| or |0>=u>x|
or |0=u=x|, given a |scaled| value~|x|, we proceed as shown here.

Note that the call of |take_frac| will produce the values 0 and~|x|
with about half the probability that it will produce any other particular
values between 0 and~|x|, because it rounds its answers.

@p function unif_rand(@!x:integer):integer;
var @!y:integer; {trial value}
begin next_random; y:=take_frac(abs(x),randoms[j_random]);
if y=abs(x) then unif_rand:=0
else if x>0 then unif_rand:=y
else unif_rand:=-y;
end;

@ Finally, a normal deviate with mean zero and unit standard deviation
can readily be obtained with the ratio method (Algorithm 3.4.1R in
{\sl The Art of Computer Programming\/}).

@p function norm_rand:integer;
var @!x,@!u,@!l:integer; {what the book would call $2^{16}X$, $2^{28}U$,
  and $-2^{24}\ln U$}
begin repeat
  repeat next_random;
  x:=take_frac(112429,randoms[j_random]-fraction_half);
    {$2^{16}\sqrt{8/e}\approx 112428.82793$}
  next_random; u:=randoms[j_random];
  until abs(x)<u;
x:=make_frac(x,u);
l:=139548960-m_log(u); {$2^{24}\cdot12\ln2\approx139548959.6165$}
until ab_vs_cd(1024,l,x,x)>=0;
norm_rand:=x;
end;

@* \[8] Packed data.
@z

@x l.388
@d pdftex_last_item_codes     = pdftex_first_rint_code + 11 {end of \pdfTeX's command codes}
@y
@d random_seed_code           = pdftex_first_rint_code + 12 {code for \.{\\pdfrandomseed}}
@d pdftex_last_item_codes     = pdftex_first_rint_code + 12 {end of \pdfTeX's command codes}
@z

@x  l.417
primitive("pdfelapsedtime",last_item,elapsed_time_code);
@!@:elapsed_time_}{\.{\\pdfelapsedtime} primitive@>
@y
primitive("pdfelapsedtime",last_item,elapsed_time_code);
@!@:elapsed_time_}{\.{\\pdfelapsedtime} primitive@>
primitive("pdfrandomseed",last_item,random_seed_code);
@!@:random_seed_}{\.{\\pdfrandomseed} primitive@>
@z

@x l.434
  elapsed_time_code: print_esc("pdfelapsedtime");
@y
  elapsed_time_code: print_esc("pdfelapsedtime");
  random_seed_code: print_esc("pdfrandomseed");
@z

@x l.461
  elapsed_time_code: cur_val := get_microinterval;
@y
  elapsed_time_code: cur_val := get_microinterval;
  random_seed_code:  cur_val := random_seed;
@z

@x
@d pdftex_convert_codes     = pdftex_first_expand_code + 21 {end of \pdfTeX's command codes}
@y
@d uniform_deviate_code     = pdftex_first_expand_code + 21 {end of \pdfTeX's command codes}
@d normal_deviate_code      = pdftex_first_expand_code + 22 {end of \pdfTeX's command codes}
@d pdftex_convert_codes     = pdftex_first_expand_code + 23 {end of \pdfTeX's command codes}
@z

@x
primitive("jobname",convert,job_name_code);@/
@y
primitive("pdfuniformdeviate",convert,uniform_deviate_code);@/
@!@:uniform_deviate_}{\.{\\pdfuniformdeviate} primitive@>
primitive("pdfnormaldeviate",convert,normal_deviate_code);@/
@!@:normal_deviate_}{\.{\\pdfnormaldeviate} primitive@>
primitive("jobname",convert,job_name_code);@/
@z

@x
  othercases print_esc("jobname")
@y
  uniform_deviate_code:     print_esc("pdfuniformdeviate");
  normal_deviate_code:      print_esc("pdfnormaldeviate");
  othercases print_esc("jobname")
@z

@x
pdf_strcmp_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    compare_strings;
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
  end;
job_name_code: if job_name=0 then open_log_file;
@y
pdf_strcmp_code:
  begin
    save_scanner_status := scanner_status;
    save_warning_index := warning_index;
    save_def_ref := def_ref;
    compare_strings;
    def_ref := save_def_ref;
    warning_index := save_warning_index;
    scanner_status := save_scanner_status;
  end;
job_name_code: if job_name=0 then open_log_file;
uniform_deviate_code:     scan_int;
normal_deviate_code:      do_nothing;
@z

@x
job_name_code: print(job_name);
@y
uniform_deviate_code:     print_int(unif_rand(cur_val));
normal_deviate_code:      print_int(norm_rand);
job_name_code: print(job_name);
@z


@x
@<Compute the magic offset@>;
@y
random_seed :=(microseconds*1000)+(epochseconds mod 1000000);@/
init_randoms(random_seed);@/
@<Compute the magic offset@>;
@z

@x l. 4562
@d pdftex_last_extension_code  == pdftex_first_extension_code + 25
@y
@d set_random_seed_code        == pdftex_first_extension_code + 26
@d pdftex_last_extension_code  == pdftex_first_extension_code + 26
@z

@x l.4625
primitive("pdfresettimer",extension,reset_timer_code);@/
@!@:reset_timer_}{\.{\\pdfresettimer} primitive@>
@y
primitive("pdfresettimer",extension,reset_timer_code);@/
@!@:reset_timer_}{\.{\\pdfresettimer} primitive@>
primitive("pdfsetrandomseed",extension,set_random_seed_code);@/
@!@:set_random_seed_code}{\.{\\pdfsetrandomseed} primitive@>
@z

@x
  reset_timer_code: print_esc("pdfresettimer");
@y
  reset_timer_code: print_esc("pdfresettimer");
  set_random_seed_code: print_esc("pdfsetrandomseed");
@z

@x
reset_timer_code: @<Implement \.{\\pdfresettimer}@>;
@y
reset_timer_code: @<Implement \.{\\pdfresettimer}@>;
set_random_seed_code: @<Implement \.{\\pdfsetrandomseed}@>;
@z

@x
@ @<Implement \.{\\pdfresettimer}@>=
@y
@ Negative random seed values are silently converted to positive ones

@<Implement \.{\\pdfsetrandomseed}@>=
begin
  scan_int;
  if cur_val<0 then negate(cur_val); 
  random_seed := cur_val;
  init_randoms(random_seed);
end

@ @<Implement \.{\\pdfresettimer}@>=
@z

