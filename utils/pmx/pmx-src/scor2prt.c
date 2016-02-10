#line 1 ""
/*  -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

#line 1 ""
/* Common Block Declarations */

struct all_1_ {
    integer noinow, iorig[24], noinst;
    logical insetup, replacing;
    integer instnum[24];
    logical botv[24];
    integer nvi[24], nsyst, nvnow;
};

#define all_1 (*(struct all_1_ *) &all_)

/* Initialized data */

struct {
    integer e_1;
    integer fill_2[24];
    integer e_3;
    integer fill_4[26];
    logical e_5[24];
    integer e_6[24];
    integer fill_7[2];
    } all_ = { 24, {0}, 24, {0}, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, 
	    FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, 
	    FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, FALSE_, 
	    FALSE_, FALSE_, FALSE_, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };


/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__27 = 27;
static integer c__2 = 2;
static integer c__128 = 128;
static integer c_n1 = -1;
static integer c__0 = 0;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__5 = 5;
static integer c__79 = 79;
static integer c__125 = 125;

/* ccccccccccccccccccccccccc */
/* c */
/* c  scor2prt 7/24/11 for PMX 2.615 */
/* ccccccccccccccccccccccccc */
/* This program, developed by Don Simons (dsimons@roadrunner.com), is */
/* part of the PMX distribution, PMX is a preprocessor for MusiXTeX. In concert */
/* with MusiXTeX and TeX, the purpose of PMX is to allow the user to create */
/* high-quality typeset musical scores by including a sequence of PMX commands */
/* in an ASCII input file. Scor2prt is an auxiliary program that creates PMX */
/* input files for individual parts from the PMX input file for the score. */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */


/* 4/9/12 */
/*   Add logical declarations to subroutine dosetup */
/* 8/8/11 */
/*   Copy & mod Ki into parts when after beginning. */
/* 7/18/11 */
/*   Start to fix up for AS..., also Ki was done a little earlier. */
/* 7/5/10 */
/*   Modify Manual to explain extended hex numbering for part-specific comments */
/* 5/16/10 */
/*   Extend part-specific comments for up to 24 parts (subroutine chkcom) */
/*     Allow files 11-34 for parts */
/*     Change file 30 to 40 */
/* 6/1/08 es2p */
/*   noimax = 24 */

/* To do */
/*  Override fracindent, musicsize? */
/*  Deal with midbar R? */
/* 2/29/04 */
/*  Check for special character being in quoted string. */
/* 10/20/02 PMX 2.407 */
/*  Interpret AN[n]"[used-defined-part-file-name]" */
/* 1/21/02 */
/*  Deals with rm in multiple parts. */
/* s2p15 */
/*  Get right non-tex index even if there's "\" inside D"..." */
/* s2p14 */
/*  10/29/00 fix transfer to parts of negative mtrdnp */
/*  4/29/00 screen for "%%" followed by "T" */
/*  Restore change from "Version 2.1a, 21 Dec" */
/* s2p13 */
/*  Allow whole-bar double-whole rests. */
/*  Bypass MIDI commands "I..." achar(10)="I" */
/* s2p12 */
/*  Let %ablabla ... %cblabla represent pmx input for parts 10-12.  But %?blabla */
/*    only taken as such if ? represents a hex digit .le. noinst, otherwise it's */
/*    a simple comment.  This lessens incompatibility. */
/*  In subroutine mbrest, properly open parts 10-12 if needed */
/* s2p11 */
/*  Ignore leading blanks */
/*  Fix undefined linelength in mbrest at very end of comments. */
/* s2p10 */
/*  Fix non-transfer of P..c" " */
/*  Allow "%%" and "%"n anywhere */
/* Version 1.43 */
/*  Fix bug with P in TeX string. */
/*  Ignore shifted whole-bar rests when consolidating whole-bar rests */
/*  Copy type 4 TeX into all parts. */
/*  Deal with XB and XP. */
/*  Permit transfer of blank line into parts */
/*  Change staves/inst in 'M' command. */
/*  Arbitrary staves/inst. */
/*  Recognize m1/2/3/4 syntax. */
/*  Enable comment and one-voice syntax in instrument names */

/* Changes since 1.1 */
/*  Deal with saved macros. */
/*  Revise setup readin, to admit comments. */
/*  Do not copy 'X' into parts */

/* ccccccccccccccccccccccccccccccccccccccccccccccc */
/* Main program */ int MAIN__(void)
{
    /* Initialized data */

    static char achar[1*10] = "P" "m" "V" "R" "A" "h" "w" "K" "M" "I";

    /* System generated locals */
    address a__1[2], a__2[3];
    integer i__1, i__2, i__3[2], i__4[3];
    char ch__1[44], ch__2[31], ch__3[4], ch__4[1], ch__5[2], ch__6[131];
    cilist ci__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;
    alist al__1;
    inlist ioin__1;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void), s_rsfe(cilist *), do_fio(integer *, char *, ftnlen),
	     e_rsfe(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer i_indx(char *, char *, ftnlen, ftnlen);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen), s_cat(char *,
	     char **, integer *, integer *, ftnlen);
    integer f_inqu(inlist *), f_open(olist *), s_cmp(char *, char *, ftnlen, 
	    ftnlen), f_back(alist *), s_wsfe(cilist *), e_wsfe(void), s_rsfi(
	    icilist *), e_rsfi(void), f_clos(cllist *);

    /* Local variables */
    extern /* Subroutine */ int oddquotesbefore_(char *, integer *, logical *,
	     ftnlen);
    static logical clefpend;
    static real xmtrnum0;
    static char templine[128];
    extern /* Subroutine */ int allparts_(char *, integer *, ftnlen);
    static integer musicsize, ia, ib, ic, iv;
    static char sq[1];
    static integer nv, idxendname;
    static real fracindent;
    static integer ibb, len, ivq, iend, nchk, idxa, ljob;
    static char line[128];
    static integer isig, locp, idxq, ndxm, idxs, inst, idxq2, idxstartname;
    extern integer iargc_(void);
    static integer kvend;
    extern /* Subroutine */ int zapbl_(char *, integer *, ftnlen);
    static integer itemp, iinst, ntinx;
    static logical oneof2;
    extern doublereal readin_(char *, integer *, integer *, ftnlen);
    extern /* Subroutine */ int chkcom_(char *, ftnlen), getarg_(integer *, 
	    char *, ftnlen);
    static integer npages;
    static char holdln[128];
    static integer iudpfn;
    static logical yesodd;
    static integer ludpfn[24];
    static char udpfnq[128*24];
    static integer iiinst;
    static logical fexist;
    static integer ivlast, mtrdnp;
    extern integer lenstr_(char *, integer *, ftnlen);
    static logical frstln;
    static integer insnow, ndxpmx, mtrnmp;
    static logical isachar;
    static char jobname[27];
    static integer lenhold, lenline;
    static char infileq[27];
    static logical gotname;
    static integer iccount, mtrdenl;
    extern integer ntindex_(char *, char *, ftnlen, ftnlen);
    static integer numargs;
    extern /* Subroutine */ int mbrests_(integer *, char *, integer *, 
	    integer *, char *, ftnlen, ftnlen);
    static logical termrpt;
    static integer mtrnuml;
    static char termsym[2], instrum[128*24];
    static integer kvstart;

    /* Fortran I/O blocks */
    static cilist io___6 = { 0, 6, 0, 0, 0 };
    static cilist io___8 = { 0, 6, 0, 0, 0 };
    static cilist io___9 = { 0, 6, 0, 0, 0 };
    static cilist io___10 = { 0, 5, 0, "(a)", 0 };
    static cilist io___13 = { 0, 6, 0, 0, 0 };
    static cilist io___17 = { 0, 6, 0, 0, 0 };
    static cilist io___20 = { 0, 10, 0, "(a)", 0 };
    static cilist io___22 = { 0, 10, 0, "(a)", 0 };
    static cilist io___23 = { 0, 10, 0, "(a)", 0 };
    static cilist io___38 = { 0, 6, 0, 0, 0 };
    static cilist io___39 = { 0, 6, 0, 0, 0 };
    static cilist io___40 = { 0, 6, 0, 0, 0 };
    static cilist io___41 = { 0, 10, 0, "(a)", 0 };
    static cilist io___42 = { 0, 10, 0, "(a)", 0 };
    static cilist io___44 = { 0, 10, 0, "(a)", 0 };
    static cilist io___46 = { 0, 10, 0, "(a)", 0 };
    static cilist io___48 = { 0, 6, 0, 0, 0 };
    static cilist io___49 = { 0, 0, 0, "(a)", 0 };
    static cilist io___50 = { 0, 10, 0, "(a)", 0 };
    static cilist io___51 = { 0, 10, 0, "(a)", 0 };
    static cilist io___55 = { 0, 10, 0, "(a)", 0 };
    static cilist io___57 = { 0, 0, 0, "(a2/a)", 0 };
    static cilist io___59 = { 0, 10, 1, "(a)", 0 };
    static cilist io___61 = { 0, 10, 0, "(a)", 0 };
    static cilist io___62 = { 0, 10, 0, "(a)", 0 };
    static cilist io___68 = { 0, 6, 0, 0, 0 };
    static cilist io___69 = { 0, 6, 0, 0, 0 };
    static cilist io___72 = { 0, 10, 0, "(a)", 0 };
    static cilist io___76 = { 0, 6, 0, 0, 0 };
    static cilist io___77 = { 0, 6, 0, 0, 0 };
    static cilist io___78 = { 0, 6, 0, 0, 0 };
    static cilist io___81 = { 0, 10, 0, "(a)", 0 };
    static cilist io___83 = { 0, 6, 0, 0, 0 };
    static cilist io___84 = { 0, 6, 0, 0, 0 };
    static icilist io___85 = { 0, line+2, 0, "(i1)", 1, 1 };
    static icilist io___88 = { 0, line+2, 0, "(i2)", 2, 1 };
    static cilist io___89 = { 0, 6, 0, 0, 0 };
    static cilist io___90 = { 0, 6, 0, 0, 0 };
    static cilist io___97 = { 0, 0, 0, "(a)", 0 };
    static cilist io___102 = { 0, 0, 0, "(a)", 0 };
    static cilist io___103 = { 0, 0, 0, "(a)", 0 };


#line 99 ""
    clefpend = FALSE_;
#line 100 ""
    all_1.insetup = TRUE_;
#line 101 ""
    all_1.replacing = FALSE_;
#line 102 ""
    frstln = TRUE_;
#line 103 ""
    lenhold = 0;
#line 104 ""
    *(unsigned char *)sq = '\\';
#line 105 ""
    s_wsle(&io___6);
#line 105 ""
    do_lio(&c__9, &c__1, "This is scor2prt for PMX 2.7, 2 April 2013", (
	    ftnlen)42);
#line 105 ""
    e_wsle();
#line 106 ""
    numargs = iargc_();
#line 107 ""
    if (numargs == 0) {
#line 108 ""
	s_wsle(&io___8);
#line 108 ""
	do_lio(&c__9, &c__1, "You could have entered a jobname on the comman"\
		"d line,", (ftnlen)53);
#line 108 ""
	e_wsle();
#line 109 ""
	s_wsle(&io___9);
#line 109 ""
	do_lio(&c__9, &c__1, "      but you may enter one now:", (ftnlen)32);
#line 109 ""
	e_wsle();
#line 110 ""
	s_rsfe(&io___10);
#line 110 ""
	do_fio(&c__1, jobname, (ftnlen)27);
#line 110 ""
	e_rsfe();
#line 111 ""
	numargs = 1;
#line 112 ""
    } else {
/*       call getarg(1,jobname,idum) ! May need to replace this w/ next line */
#line 114 ""
	getarg_(&c__1, jobname, (ftnlen)27);
#line 115 ""
    }
#line 116 ""
    ljob = lenstr_(jobname, &c__27, (ftnlen)27);
#line 117 ""
    if (ljob == 0) {
#line 118 ""
	s_wsle(&io___13);
#line 118 ""
	do_lio(&c__9, &c__1, "No was jobname entered. Restart and try again.",
		 (ftnlen)46);
#line 118 ""
	e_wsle();
#line 119 ""
	s_stop("", (ftnlen)0);
#line 120 ""
    }

/*  Strip ".pmx" if necessary */

/* Computing MAX */
#line 124 ""
    i__1 = i_indx(jobname, ".pmx", (ftnlen)27, (ftnlen)4), i__2 = i_indx(
	    jobname, ".PMX", (ftnlen)27, (ftnlen)4);
#line 124 ""
    ndxpmx = max(i__1,i__2);
#line 125 ""
    if (ndxpmx > 0) {
#line 126 ""
	s_copy(jobname, jobname, (ftnlen)27, ndxpmx - 1);
#line 127 ""
	ljob += -4;
#line 128 ""
    }

/*  Check for existence of input file */

/* Writing concatenation */
#line 132 ""
    i__3[0] = ljob, a__1[0] = jobname;
#line 132 ""
    i__3[1] = 4, a__1[1] = ".pmx";
#line 132 ""
    s_cat(infileq, a__1, i__3, &c__2, (ftnlen)27);
#line 133 ""
    ioin__1.inerr = 0;
#line 133 ""
    ioin__1.infilen = 27;
#line 133 ""
    ioin__1.infile = infileq;
#line 133 ""
    ioin__1.inex = &fexist;
#line 133 ""
    ioin__1.inopen = 0;
#line 133 ""
    ioin__1.innum = 0;
#line 133 ""
    ioin__1.innamed = 0;
#line 133 ""
    ioin__1.inname = 0;
#line 133 ""
    ioin__1.inacc = 0;
#line 133 ""
    ioin__1.inseq = 0;
#line 133 ""
    ioin__1.indir = 0;
#line 133 ""
    ioin__1.infmt = 0;
#line 133 ""
    ioin__1.inform = 0;
#line 133 ""
    ioin__1.inunf = 0;
#line 133 ""
    ioin__1.inrecl = 0;
#line 133 ""
    ioin__1.innrec = 0;
#line 133 ""
    ioin__1.inblank = 0;
#line 133 ""
    f_inqu(&ioin__1);
#line 134 ""
    if (! fexist) {
#line 135 ""
	s_wsle(&io___17);
/* Writing concatenation */
#line 135 ""
	i__3[0] = 17, a__1[0] = "Cannot find file ";
#line 135 ""
	i__3[1] = 27, a__1[1] = infileq;
#line 135 ""
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)44);
#line 135 ""
	do_lio(&c__9, &c__1, ch__1, (ftnlen)44);
#line 135 ""
	e_wsle();
#line 136 ""
	s_stop("", (ftnlen)0);
#line 137 ""
    }
#line 138 ""
    o__1.oerr = 0;
#line 138 ""
    o__1.ounit = 10;
#line 138 ""
    o__1.ofnmlen = ljob + 4;
/* Writing concatenation */
#line 138 ""
    i__3[0] = ljob, a__1[0] = jobname;
#line 138 ""
    i__3[1] = 4, a__1[1] = ".pmx";
#line 138 ""
    s_cat(ch__2, a__1, i__3, &c__2, (ftnlen)31);
#line 138 ""
    o__1.ofnm = ch__2;
#line 138 ""
    o__1.orl = 0;
#line 138 ""
    o__1.osta = 0;
#line 138 ""
    o__1.oacc = 0;
#line 138 ""
    o__1.ofm = 0;
#line 138 ""
    o__1.oblnk = 0;
#line 138 ""
    f_open(&o__1);

/* Open all instrument files now for allparts stuff.  Later disgard those >nv */

#line 142 ""
    for (iv = 1; iv <= 24; ++iv) {
#line 143 ""
	all_1.iorig[iv - 1] = iv;
#line 144 ""
	o__1.oerr = 0;
#line 144 ""
	o__1.ounit = iv + 10;
#line 144 ""
	o__1.ofnm = 0;
#line 144 ""
	o__1.orl = 0;
#line 144 ""
	o__1.osta = "SCRATCH";
#line 144 ""
	o__1.oacc = 0;
#line 144 ""
	o__1.ofm = 0;
#line 144 ""
	o__1.oblnk = 0;
#line 144 ""
	f_open(&o__1);
#line 145 ""
	ludpfn[iv - 1] = 0;
#line 146 ""
/* L19: */
#line 146 ""
    }
#line 147 ""
    s_rsfe(&io___20);
#line 147 ""
    do_fio(&c__1, line, (ftnlen)128);
#line 147 ""
    e_rsfe();
#line 148 ""
    chkcom_(line, (ftnlen)128);
#line 149 ""
    if (s_cmp(line, "---", (ftnlen)3, (ftnlen)3) == 0) {
#line 150 ""
	allparts_(line, &c__128, (ftnlen)128);
#line 151 ""
L31:
#line 151 ""
	s_rsfe(&io___22);
#line 151 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 151 ""
	e_rsfe();
#line 152 ""
	if (s_cmp(line, "---", (ftnlen)3, (ftnlen)3) != 0) {
#line 153 ""
	    allparts_(line, &c__128, (ftnlen)128);
#line 154 ""
	    goto L31;
#line 155 ""
	}
#line 156 ""
	allparts_(line, &c__128, (ftnlen)128);
#line 157 ""
	s_rsfe(&io___23);
#line 157 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 157 ""
	e_rsfe();
#line 158 ""
	chkcom_(line, (ftnlen)128);
#line 159 ""
    }
#line 160 ""
    iccount = 0;
#line 161 ""
    nv = readin_(line, &iccount, &c__1, (ftnlen)128) + .1f;
#line 162 ""
    all_1.noinst = readin_(line, &iccount, &c__2, (ftnlen)128) + .1f;
#line 163 ""
    if (all_1.noinst > 0) {
#line 164 ""
	all_1.nvi[0] = nv - all_1.noinst + 1;
#line 165 ""
    } else {
#line 166 ""
	all_1.noinst = 1 - all_1.noinst;
#line 167 ""
	i__1 = all_1.noinst;
#line 167 ""
	for (iinst = 1; iinst <= i__1; ++iinst) {
#line 168 ""
	    all_1.nvi[iinst - 1] = readin_(line, &iccount, &c_n1, (ftnlen)128)
		     + .1f;
#line 169 ""
/* L21: */
#line 169 ""
	}
#line 170 ""
    }
#line 171 ""
    all_1.noinow = all_1.noinst;
#line 172 ""
    insnow = 1;

/*  ivlast is last iv in current inst.  instnum(iv) is iinst for current voice. */

#line 176 ""
    ivlast = all_1.nvi[0];
#line 177 ""
    i__1 = nv;
#line 177 ""
    for (iv = 1; iv <= i__1; ++iv) {
#line 178 ""
	all_1.instnum[iv - 1] = insnow;
#line 179 ""
	if (iv == ivlast) {
#line 180 ""
	    if (iv < nv) {
#line 180 ""
		all_1.botv[iv] = TRUE_;
#line 180 ""
	    }

/*  The previous stmt will set botv true only for bot voice of iinst>1.  It is */
/*  used when writing termrpts, but the one in voice one is handled differently, */
/*  so botv(1) is left .false. */

#line 186 ""
	    if (insnow < all_1.noinst) {
#line 187 ""
		++insnow;
#line 188 ""
		ivlast += all_1.nvi[insnow - 1];
#line 189 ""
	    }
#line 190 ""
	}
#line 191 ""
/* L22: */
#line 191 ""
    }
#line 192 ""
    mtrnuml = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
#line 193 ""
    mtrdenl = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
#line 194 ""
    mtrnmp = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
#line 195 ""
    mtrdnp = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
#line 196 ""
    xmtrnum0 = readin_(line, &iccount, &c__0, (ftnlen)128);
#line 197 ""
    isig = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
#line 198 ""
    npages = readin_(line, &iccount, &c__3, (ftnlen)128) + .1f;
#line 199 ""
    all_1.nsyst = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
#line 200 ""
    musicsize = readin_(line, &iccount, &c__4, (ftnlen)128) + .1f;
#line 201 ""
    fracindent = readin_(line, &iccount, &c__5, (ftnlen)128);
#line 202 ""
    if (npages == 0) {
#line 203 ""
	s_wsle(&io___38);
#line 203 ""
	do_lio(&c__9, &c__1, "You entered npages=0, which means nsyst is not"\
		" the total number", (ftnlen)63);
#line 203 ""
	e_wsle();
#line 205 ""
	s_wsle(&io___39);
#line 205 ""
	do_lio(&c__9, &c__1, "of systems.  Scor2prt has to know the total nu"\
		"mber of systems.", (ftnlen)62);
#line 205 ""
	e_wsle();
#line 207 ""
	s_wsle(&io___40);
#line 207 ""
	do_lio(&c__9, &c__1, "Please set npages and nsyst to their real valu"\
		"es.", (ftnlen)49);
#line 207 ""
	e_wsle();
#line 209 ""
	s_stop("", (ftnlen)0);
#line 210 ""
    }

/*  Must leave insetup=.true. else could bypass ALL instrument names. */

#line 214 ""
    s_rsfe(&io___41);
#line 214 ""
    do_fio(&c__1, line, (ftnlen)128);
#line 214 ""
    e_rsfe();
#line 215 ""
    chkcom_(line, (ftnlen)128);
#line 216 ""
    al__1.aerr = 0;
#line 216 ""
    al__1.aunit = 10;
#line 216 ""
    f_back(&al__1);

/*  Normally this puts pointer at start of line with 1st inst name */
/*  Check if prior line was "%%" */

#line 221 ""
    al__1.aerr = 0;
#line 221 ""
    al__1.aunit = 10;
#line 221 ""
    f_back(&al__1);
#line 222 ""
    s_rsfe(&io___42);
#line 222 ""
    do_fio(&c__1, line, (ftnlen)128);
#line 222 ""
    e_rsfe();
#line 223 ""
    if (s_cmp(line, "%%", (ftnlen)2, (ftnlen)2) == 0) {
#line 223 ""
	al__1.aerr = 0;
#line 223 ""
	al__1.aunit = 10;
#line 223 ""
	f_back(&al__1);
#line 223 ""
    }
#line 224 ""
    i__1 = all_1.noinst;
#line 224 ""
    for (iv = 1; iv <= i__1; ++iv) {
#line 225 ""
	gotname = FALSE_;
#line 226 ""
L16:
#line 226 ""
	s_rsfe(&io___44);
#line 226 ""
	do_fio(&c__1, instrum + (iv - 1 << 7), (ftnlen)128);
#line 226 ""
	e_rsfe();
#line 227 ""
	if (s_cmp(instrum + (iv - 1 << 7), "%%", (ftnlen)2, (ftnlen)2) == 0) {
#line 228 ""
	    s_rsfe(&io___46);
#line 228 ""
	    do_fio(&c__1, line, (ftnlen)128);
#line 228 ""
	    e_rsfe();
#line 229 ""
	    goto L16;
#line 230 ""
	} else if (*(unsigned char *)&instrum[(iv - 1) * 128] == '%') {
#line 231 ""
	    ivq = *(unsigned char *)&instrum[(iv - 1 << 7) + 1] - 48;
#line 232 ""
	    if (ivq != iv) {

/*  It's really a comment.  Copy to parts, then get another trial name. */

#line 236 ""
		allparts_(instrum + (iv - 1 << 7), &c__128, (ftnlen)128);
#line 237 ""
		goto L16;
#line 238 ""
	    } else {
#line 239 ""
		s_copy(line, instrum + ((iv - 1 << 7) + 2), (ftnlen)128, (
			ftnlen)126);
#line 240 ""
		s_copy(instrum + (iv - 1 << 7), line, (ftnlen)128, (ftnlen)
			128);
#line 241 ""
		gotname = TRUE_;
#line 242 ""
	    }
#line 243 ""
	} else {
#line 244 ""
	    gotname = TRUE_;
#line 245 ""
	}
/* c */
/* c  The following checks for macro that write original C-clef as part of */
/* c  instrument name.  See pmx.tex */
/* c */
/*        if (index(instrum(iv),'namewpc') .eq. 0) then */
/*          write(10+iv,'(a)')' ' */
/*        else */
/*          inm1 = index(instrum(iv),'{')+1 */
/*          inm2 = index(instrum(iv),'}')-1 */
/*          read(instrum(iv)(inm2+2:inm2+8),'(i1,4x,2i1)')ilev,iy1,iy2 */
/*          write(10+iv,'(a)')sq//'namewpc{}'//char(ilev+48)//'{20}'// */
/*     *      char(iy1+49)//char(iy2+49) */
/*          instrum(iv) = instrum(iv)(inm1:inm2) */
/*        end if */
#line 260 ""
	if (! gotname) {
#line 261 ""
	    s_wsle(&io___48);
#line 261 ""
	    do_lio(&c__9, &c__1, "You must provide a replacement instrument "\
		    "name", (ftnlen)46);
#line 261 ""
	    e_wsle();
#line 262 ""
	    s_stop("", (ftnlen)0);
#line 263 ""
	}
#line 264 ""
	io___49.ciunit = iv + 10;
#line 264 ""
	s_wsfe(&io___49);
#line 264 ""
	do_fio(&c__1, " ", (ftnlen)1);
#line 264 ""
	e_wsfe();
#line 265 ""
/* L14: */
#line 265 ""
    }
#line 266 ""
    all_1.replacing = FALSE_;
#line 267 ""
    all_1.nvnow = nv;

/*  Clef string:  Note insetup is still T, so "%%" will be treated specially */

#line 271 ""
    s_rsfe(&io___50);
#line 271 ""
    do_fio(&c__1, line, (ftnlen)128);
#line 271 ""
    e_rsfe();
#line 272 ""
    chkcom_(line, (ftnlen)128);
#line 273 ""
    if (all_1.replacing) {

/*  If here, we have next line after "%%", containing score's clef string */
/*  Assume all clefs are handled with instrument comments. */

#line 278 ""
	s_rsfe(&io___51);
#line 278 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 278 ""
	e_rsfe();
#line 279 ""
	chkcom_(line, (ftnlen)128);
#line 280 ""
	al__1.aerr = 0;
#line 280 ""
	al__1.aunit = 10;
#line 280 ""
	f_back(&al__1);
#line 281 ""
    } else {

/*  If here, line has the clef string in it.  Handle the old way */

#line 285 ""
	kvstart = 1;
#line 286 ""
	kvend = all_1.nvi[0];
#line 287 ""
	i__1 = all_1.noinst;
#line 287 ""
	for (inst = 1; inst <= i__1; ++inst) {
#line 288 ""
	    ci__1.cierr = 0;
#line 288 ""
	    ci__1.ciunit = inst + 10;
/* Writing concatenation */
#line 288 ""
	    i__4[0] = 2, a__2[0] = "(a";
#line 288 ""
	    *(unsigned char *)&ch__4[0] = all_1.nvi[inst - 1] + 48;
#line 288 ""
	    i__4[1] = 1, a__2[1] = ch__4;
#line 288 ""
	    i__4[2] = 1, a__2[2] = ")";
#line 288 ""
	    ci__1.cifmt = (s_cat(ch__3, a__2, i__4, &c__3, (ftnlen)4), ch__3);
#line 288 ""
	    s_wsfe(&ci__1);
#line 288 ""
	    do_fio(&c__1, line + (kvstart - 1), kvend - (kvstart - 1));
#line 288 ""
	    e_wsfe();
#line 290 ""
	    if (inst < all_1.noinst) {
#line 291 ""
		kvstart = kvend + 1;
#line 292 ""
		kvend = kvstart + all_1.nvi[inst] - 1;
#line 293 ""
	    }
#line 294 ""
/* L2: */
#line 294 ""
	}
#line 295 ""
    }
#line 296 ""
    all_1.replacing = FALSE_;
#line 297 ""
    all_1.insetup = FALSE_;

/*  *****NOTE*****This comment applies to stuff done earlier! */
/*  Before starting the big loop, copy initial instnum and staffnum stuff */
/*  into working values.  Latter may change if noinst changes.  Also make */
/*  list of current inst nums relative to original ones.  In addition to those */
/*  below, must redo instnum(iv) and botv(iv) when we change noinst. */

/*  Path string:  ASSUME THIS WILL NEVER BE ALTERED IN PARTS! */

#line 307 ""
L18:
#line 307 ""
    s_rsfe(&io___55);
#line 307 ""
    do_fio(&c__1, line, (ftnlen)128);
#line 307 ""
    e_rsfe();
#line 308 ""
    if (*(unsigned char *)line == '%') {
#line 309 ""
	allparts_(line, &c__128, (ftnlen)128);
#line 310 ""
	goto L18;
#line 311 ""
    }
#line 312 ""
    allparts_(line, &c__128, (ftnlen)128);

/*  Write instrument names.  Will be blank if later part of a score. */

#line 316 ""
    if (*(unsigned char *)&instrum[0] != ' ') {
#line 317 ""
	i__1 = all_1.noinst;
#line 317 ""
	for (iv = 1; iv <= i__1; ++iv) {
#line 318 ""
	    len = lenstr_(instrum + (iv - 1 << 7), &c__79, (ftnlen)128);
#line 319 ""
	    io___57.ciunit = iv + 10;
#line 319 ""
	    s_wsfe(&io___57);
#line 319 ""
	    do_fio(&c__1, "Ti", (ftnlen)2);
#line 319 ""
	    do_fio(&c__1, instrum + (iv - 1 << 7), len);
#line 319 ""
	    e_wsfe();
#line 320 ""
/* L3: */
#line 320 ""
	}
#line 321 ""
    }

/*  The big loop.  Except for '%%', put all comment lines in all parts. */
/*  Unless preceeded by '%%', put all type 2 or 3 TeX Strings in all parts */
/*  If a line starts with %!, put the rest of it in each part. */
/*  If a line starts with %[n], put the rest of it in part [n]. */
/*  Check for Tt, Tc, Voltas, Repeats, headers, lower texts, meter changes. */
/*     Assume they only come at top of block, except terminal repeat needs */
/*     special handling. */
/*  Check for "P"; ignore in parts. */
/*  Check for consecutive full-bar rests; if found, replace with rm[nn] */

#line 333 ""
    iv = 1;
#line 334 ""
    iinst = 1;
#line 335 ""
    termrpt = FALSE_;
#line 336 ""
L4:
#line 336 ""
    i__1 = s_rsfe(&io___59);
#line 336 ""
    if (i__1 != 0) {
#line 336 ""
	goto L999;
#line 336 ""
    }
#line 336 ""
    i__1 = do_fio(&c__1, line, (ftnlen)128);
#line 336 ""
    if (i__1 != 0) {
#line 336 ""
	goto L999;
#line 336 ""
    }
#line 336 ""
    i__1 = e_rsfe();
#line 336 ""
    if (i__1 != 0) {
#line 336 ""
	goto L999;
#line 336 ""
    }
#line 337 ""
    lenline = lenstr_(line, &c__128, (ftnlen)128);
#line 338 ""
    if (lenline == 0) {
#line 338 ""
	goto L4;
#line 338 ""
    }
#line 339 ""
    zapbl_(line, &c__128, (ftnlen)128);
#line 340 ""
    chkcom_(line, (ftnlen)128);
#line 341 ""
    lenline = lenstr_(line, &c__128, (ftnlen)128);
#line 342 ""
    if (lenline == 0) {
#line 342 ""
	goto L4;
#line 342 ""
    }
#line 343 ""
    if (*(unsigned char *)line == 'T') {
#line 344 ""
	allparts_(line, &c__128, (ftnlen)128);
#line 345 ""
	s_rsfe(&io___61);
#line 345 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 345 ""
	e_rsfe();
#line 346 ""
	allparts_(line, &c__128, (ftnlen)128);
#line 347 ""
	goto L4;
#line 348 ""
    } else /* if(complicated condition) */ {
/* Writing concatenation */
#line 348 ""
	i__3[0] = 1, a__1[0] = sq;
#line 348 ""
	i__3[1] = 1, a__1[1] = sq;
#line 348 ""
	s_cat(ch__5, a__1, i__3, &c__2, (ftnlen)2);
#line 348 ""
	if (s_cmp(line, ch__5, (ftnlen)2, (ftnlen)2) == 0) {
#line 349 ""
	    allparts_(line, &c__128, (ftnlen)128);
#line 350 ""
	    goto L4;
#line 351 ""
	} else if (i_indx("hl", line, (ftnlen)2, (ftnlen)1) > 0 && i_indx(
		" +-", line + 1, (ftnlen)3, (ftnlen)1) > 0) {
#line 353 ""
	    allparts_(line, &c__128, (ftnlen)128);
#line 354 ""
	    s_rsfe(&io___62);
#line 354 ""
	    do_fio(&c__1, line, (ftnlen)128);
#line 354 ""
	    e_rsfe();
#line 355 ""
	    allparts_(line, &c__128, (ftnlen)128);
#line 356 ""
	    goto L4;
#line 357 ""
	} else if (iv == 1) {
#line 358 ""
	    for (ia = 1; ia <= 10; ++ia) {
#line 359 ""
L24:
#line 360 ""
		idxa = ntindex_(line, achar + (ia - 1), (ftnlen)128, (ftnlen)
			1);
#line 361 ""
		isachar = idxa > 0;
#line 362 ""
		if (idxa > 1) {
#line 362 ""
		    i__1 = idxa - 2;
#line 362 ""
		    isachar = s_cmp(line + i__1, " ", idxa - 1 - i__1, (
			    ftnlen)1) == 0;
#line 362 ""
		}

/*                   1   2   3   4   5   6   7   8   9   10 */
/*      data achar /'P','m','V','R','A','h','w','K','M','I'/ */

#line 367 ""
		if (ia == 9) {
#line 367 ""
		    i__1 = idxa;
#line 367 ""
		    isachar = isachar && s_cmp(line + i__1, "S", idxa + 1 - 
			    i__1, (ftnlen)1) == 0;
#line 367 ""
		}
#line 369 ""
		if (isachar) {

/*  Check whether character is inside a quoted string by counting */
/*  how many quotes precede it in the line */

#line 375 ""
		    oddquotesbefore_(line, &idxa, &yesodd, (ftnlen)128);
#line 376 ""
		    if (yesodd) {
#line 376 ""
			isachar = FALSE_;
#line 376 ""
		    }
#line 377 ""
		}
#line 378 ""
		if (isachar) {

/*  Find next blank */

#line 382 ""
		    for (ib = idxa + 1; ib <= 128; ++ib) {
#line 383 ""
			if (*(unsigned char *)&line[ib - 1] == ' ') {
#line 383 ""
			    goto L7;
#line 383 ""
			}
#line 384 ""
/* L6: */
#line 384 ""
		    }
#line 385 ""
		    s_wsle(&io___68);
#line 385 ""
		    do_lio(&c__9, &c__1, "Problem with \"V,R,m,P,A,h,MS, o"\
			    "r w\"", (ftnlen)35);
#line 385 ""
		    e_wsle();
#line 386 ""
		    s_wsle(&io___69);
#line 386 ""
		    do_lio(&c__9, &c__1, "Send files to Dr. Don at dsimons a"\
			    "t roadrunner dot com", (ftnlen)54);
#line 386 ""
		    e_wsle();
#line 388 ""
		    s_stop("1", (ftnlen)1);
#line 389 ""
L7:

/*  Next blank is at position ib.  Later, if ia=1, must check for Pc"  " ; */
/*    i.e., look for '"' between P and blank */

#line 394 ""
		    if (ia == 4) {

/*  Check for terminal repeat.  Note if there's a term rpt, there can't be any */
/*  others.  Also, must process repeats LAST, after m's and 'V's */

#line 399 ""
			for (ic = ib + 1; ic <= 128; ++ic) {

/*  If any subsequent character on this line is neither blank nor "/", get out */

#line 403 ""
			    if (i_indx(" /", line + (ic - 1), (ftnlen)2, (
				    ftnlen)1) == 0) {
#line 403 ""
				goto L9;
#line 403 ""
			    }
#line 404 ""
			    if (*(unsigned char *)&line[ic - 1] == '/') {
#line 405 ""
				termrpt = TRUE_;
#line 406 ""
				i__1 = ib - 3;
#line 406 ""
				s_copy(termsym, line + i__1, (ftnlen)2, ib - 
					1 - i__1);

/*  Process the line as if there were no "R" */

#line 410 ""
				goto L10;
#line 411 ""
			    }
#line 412 ""
/* L8: */
#line 412 ""
			}

/* +++ 060812 */
/*  If here, all chars after "R" symbol are blanks, so process the line normally, */
/*    but only IF next line is not the M-Tx line " /" */

#line 418 ""
			s_rsfe(&io___72);
#line 418 ""
			do_fio(&c__1, templine, (ftnlen)128);
#line 418 ""
			e_rsfe();
#line 419 ""
			if (s_cmp(templine, " /", (ftnlen)2, (ftnlen)2) != 0) 
				{
#line 420 ""
			    al__1.aerr = 0;
#line 420 ""
			    al__1.aunit = 10;
#line 420 ""
			    f_back(&al__1);
/* and flow out */
#line 421 ""
			} else {

/*  We have the M-Tx case where one line ends with R? and next is " /". Add / to the line, */
/*    and proceed as above */

/* Writing concatenation */
#line 426 ""
			    i__3[0] = ib, a__1[0] = line;
#line 426 ""
			    i__3[1] = 1, a__1[1] = "/";
#line 426 ""
			    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 427 ""
			    lenline += 2;
#line 428 ""
			    termrpt = TRUE_;
#line 429 ""
			    i__1 = ib - 3;
#line 429 ""
			    s_copy(termsym, line + i__1, (ftnlen)2, ib - 1 - 
				    i__1);
#line 430 ""
			    goto L10;
#line 431 ""
			}
/* +++ 060812 */

#line 434 ""
		    } else if (ia == 1) {
#line 435 ""
			idxq = ntindex_(line, "\"", (ftnlen)128, (ftnlen)1);
#line 436 ""
			if (idxq > idxa && idxq < ib) {

/*  Quote is between P and next blank.  Find 2nd quote, starting at the blank. */

#line 440 ""
			    idxq2 = ib - 1 + ntindex_(line, "\"", (ftnlen)128,
				     (ftnlen)1);
#line 441 ""
			    i__1 = idxq2;
#line 441 ""
			    if (idxq == 0 || s_cmp(line + i__1, " ", idxq2 + 
				    1 - i__1, (ftnlen)1) != 0) {
#line 442 ""
				s_wsle(&io___76);
#line 442 ""
				e_wsle();
#line 443 ""
				s_wsle(&io___77);
#line 443 ""
				do_lio(&c__9, &c__1, "Error copying P with q"\
					"uotes, idxq2:", (ftnlen)35);
#line 443 ""
				do_lio(&c__3, &c__1, (char *)&idxq2, (ftnlen)
					sizeof(integer));
#line 443 ""
				e_wsle();
#line 444 ""
				s_wsle(&io___78);
#line 444 ""
				do_lio(&c__9, &c__1, line, (ftnlen)60);
#line 444 ""
				e_wsle();
#line 445 ""
				s_stop("1", (ftnlen)1);
#line 446 ""
			    }
#line 447 ""
			    ib = idxq2 + 1;
#line 448 ""
			}

/*  Do not transfer P into parts. */

#line 452 ""
			goto L12;
#line 453 ""
		    } else if (ia == 9) {

/*  Start Saving a macro. After leaving here, a symbol will be sent to all parts, */
/*  If all on this line, set ib to end and exit normally. */

#line 458 ""
			i__1 = ib;
#line 458 ""
			ndxm = i_indx(line + i__1, "M", 128 - i__1, (ftnlen)1)
				;
#line 459 ""
			i__1 = ib + ndxm - 2;
#line 459 ""
			if (ndxm > 0 && s_cmp(line + i__1, " ", ib + ndxm - 1 
				- i__1, (ftnlen)1) == 0) {

/*  Macro ends on this line */

#line 463 ""
			    ib = ib + ndxm + 1;
#line 464 ""
			} else {

/*  Save leading part of current line */

#line 468 ""
			    lenhold = idxa - 1;
#line 469 ""
			    if (lenhold > 0) {
#line 469 ""
				s_copy(holdln, line, (ftnlen)128, lenhold);
#line 469 ""
			    }

/*  Transfer rest of line */

#line 473 ""
			    i__1 = 129 - idxa;
#line 473 ""
			    allparts_(line + (idxa - 1), &i__1, 128 - (idxa - 
				    1));

/*  Read next line */

#line 477 ""
L20:
#line 477 ""
			    s_rsfe(&io___81);
#line 477 ""
			    do_fio(&c__1, line, (ftnlen)128);
#line 477 ""
			    e_rsfe();

/*  Check for comment, transfer and loop if so */

/*                if (line(1:1) .eq.'%') then */
#line 482 ""
L23:
#line 482 ""
			    if (*(unsigned char *)line == '%') {
/*                  call allparts(line,128) */
/*                  go to 20 */
#line 485 ""
				chkcom_(line, (ftnlen)128);
#line 486 ""
				goto L23;
#line 487 ""
			    }

/*  Look for terminal ' M' */

#line 491 ""
			    if (*(unsigned char *)line == 'M') {
#line 492 ""
				ndxm = 1;
#line 493 ""
			    } else {
#line 494 ""
				ndxm = i_indx(line, " M", (ftnlen)128, (
					ftnlen)2);
#line 495 ""
				if (ndxm > 0) {
#line 495 ""
				    ++ndxm;
#line 495 ""
				}
#line 496 ""
			    }
#line 497 ""
			    if (ndxm > 0) {

/*  Set parameters, exit normally (but later check for leading part of 1st line */

#line 501 ""
				idxa = 1;
#line 502 ""
				ib = ndxm + 1;
#line 503 ""
			    } else {

/*  No "M", transfer entire line, loop */

#line 507 ""
				allparts_(line, &c__128, (ftnlen)128);
#line 508 ""
				goto L20;
#line 509 ""
			    }
#line 510 ""
			}
#line 511 ""
		    } else if (ia == 10) {

/*  Do not transfer MIDI command into parts */

#line 515 ""
			goto L12;
#line 516 ""
		    } else if (ia == 5) {

/*  First check for "AS", but S may come after other "A" options */

#line 520 ""
			i__1 = idxa;
#line 520 ""
			idxs = i_indx(line + i__1, "S", ib - i__1, (ftnlen)1);
#line 521 ""
			if (idxs > 0) {

/*  Get rid of the string. First check if that's all there is in A. */

#line 525 ""
			    if (ib - idxa == nv + 2) {
#line 525 ""
				goto L12;
#line 525 ""
			    }
#line 526 ""
			    i__1 = idxa + idxs + nv;
/* Writing concatenation */
#line 526 ""
			    i__3[0] = idxa + idxs - 1, a__1[0] = line;
#line 526 ""
			    i__3[1] = ib - i__1, a__1[1] = line + i__1;
#line 526 ""
			    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 527 ""
			}

/*  Check for user-defined part file name. Must start in column 1 and have */
/*    AN[n]"userfilename". */
/*    idxa is position of "A" */
/*    ib is position of the next blank after "A" */
/*    Don't allow any blanks in user */

#line 535 ""
			i__1 = idxa;
#line 535 ""
			if (s_cmp(line + i__1, "N", idxa + 1 - i__1, (ftnlen)
				1) != 0) {
#line 535 ""
			    goto L9;
#line 535 ""
			}
/* bail out */
#line 536 ""
			if (idxa != 1) {
#line 537 ""
			    s_wsle(&io___83);
#line 537 ""
			    e_wsle();
#line 538 ""
			    s_wsle(&io___84);
#line 538 ""
			    do_lio(&c__9, &c__1, "You entered \"AN...\" some"\
				    "where beyond first column; stopping.", (
				    ftnlen)60);
#line 538 ""
			    e_wsle();
#line 540 ""
			    s_stop("1", (ftnlen)1);
#line 541 ""
			}

/*  pmxa already checked for valid one- or 2-digit number, so get it */

#line 545 ""
			if (*(unsigned char *)&line[3] == '"') {

/*  Single digit instrument number */

#line 549 ""
			    s_rsfi(&io___85);
#line 549 ""
			    do_fio(&c__1, (char *)&iudpfn, (ftnlen)sizeof(
				    integer));
#line 549 ""
			    e_rsfi();
#line 550 ""
			    idxstartname = 5;
#line 551 ""
			} else {
#line 552 ""
			    s_rsfi(&io___88);
#line 552 ""
			    do_fio(&c__1, (char *)&iudpfn, (ftnlen)sizeof(
				    integer));
#line 552 ""
			    e_rsfi();
#line 553 ""
			    idxstartname = 6;
#line 554 ""
			}
#line 555 ""
			ludpfn[iudpfn - 1] = i_indx(line + (idxstartname - 1),
				 "\"", 128 - (idxstartname - 1), (ftnlen)1) - 
				1;
#line 556 ""
			if (ludpfn[iudpfn - 1] < 0) {
#line 557 ""
			    s_wsle(&io___89);
#line 557 ""
			    e_wsle();
#line 558 ""
			    s_wsle(&io___90);
#line 558 ""
			    do_lio(&c__9, &c__1, "User-defined part file nam"\
				    "e must be in quotes", (ftnlen)45);
#line 558 ""
			    e_wsle();
#line 559 ""
			    s_stop("1", (ftnlen)1);
#line 560 ""
			}
#line 561 ""
			idxendname = idxstartname + ludpfn[iudpfn - 1] - 1;
#line 562 ""
			s_copy(udpfnq + (iudpfn - 1 << 7), line + (
				idxstartname - 1), (ftnlen)128, idxendname - (
				idxstartname - 1));

/*  Get a new line! */

#line 566 ""
			goto L4;
#line 567 ""
		    } else if (ia == 8) {

/* Key change/transposition. */
/* If not instrument specific, copy to all parts */

#line 572 ""
			i__1 = idxa;
#line 572 ""
			if (s_cmp(line + i__1, "i", idxa + 1 - i__1, (ftnlen)
				1) != 0) {
#line 572 ""
			    goto L9;
#line 572 ""
			}

/* Instrument-wise key/transposition(s): Ki[nn][+/-][dd}[+/-][dd]... */

#line 576 ""
			ibb = idxa + 2;
/* Starts on digit after 'i' */
#line 577 ""
L40:
#line 578 ""
			ici__1.icierr = 0;
#line 578 ""
			ici__1.iciend = 0;
#line 578 ""
			ici__1.icirnum = 1;
#line 578 ""
			ici__1.icirlen = 1;
#line 578 ""
			ici__1.iciunit = line + (ibb - 1);
#line 578 ""
			ici__1.icifmt = "(i1)";
#line 578 ""
			s_rsfi(&ici__1);
#line 578 ""
			do_fio(&c__1, (char *)&iiinst, (ftnlen)sizeof(integer)
				);
#line 578 ""
			e_rsfi();
/* 1st digit of iinst */
#line 579 ""
			i__1 = ibb;
#line 579 ""
			itemp = i_indx("01234567890", line + i__1, (ftnlen)11,
				 ibb + 1 - i__1);
#line 580 ""
			if (itemp > 0) {
#line 581 ""
			    ++ibb;
#line 582 ""
			    iiinst = iiinst * 10 + itemp - 1;
#line 583 ""
			}
#line 584 ""
			++ibb;
/* now at first +/-. Need end of 2nd number */
#line 585 ""
			i__1 = ibb;
#line 585 ""
			itemp = i_indx(line + i__1, "i", ib - i__1, (ftnlen)1)
				;
/* Rel pos'n of next 'i' */
#line 586 ""
			if (itemp > 0) {
#line 587 ""
			    iend = ibb + itemp - 1;
#line 588 ""
			} else {
#line 589 ""
			    iend = ib - 1;
#line 590 ""
			}
#line 591 ""
			io___97.ciunit = all_1.iorig[iiinst - 1] + 10;
#line 591 ""
			s_wsfe(&io___97);
/* Writing concatenation */
#line 591 ""
			i__3[0] = 3, a__1[0] = "Ki1";
#line 591 ""
			i__3[1] = iend - (ibb - 1), a__1[1] = line + (ibb - 1)
				;
#line 591 ""
			s_cat(ch__6, a__1, i__3, &c__2, (ftnlen)131);
#line 591 ""
			do_fio(&c__1, ch__6, iend - (ibb - 1) + 3);
#line 591 ""
			e_wsfe();
#line 592 ""
			if (itemp > 0) {
#line 593 ""
			    ibb = iend + 2;
#line 594 ""
			    goto L40;
#line 595 ""
			}
#line 596 ""
			goto L12;
/* Remove K command from string, go to next ia */
#line 597 ""
		    }
#line 598 ""
L9:
#line 599 ""
		    i__1 = ib - idxa;
#line 599 ""
		    allparts_(line + (idxa - 1), &i__1, ib - 1 - (idxa - 1));
#line 600 ""
L12:

/*  Remove the string from line */

#line 604 ""
		    if (idxa == 1) {
#line 605 ""
			s_copy(line, line + (ib - 1), (ftnlen)128, 128 - (ib 
				- 1));
#line 606 ""
		    } else {
/* Writing concatenation */
#line 607 ""
			i__3[0] = idxa - 1, a__1[0] = line;
#line 607 ""
			i__3[1] = 128 - (ib - 1), a__1[1] = line + (ib - 1);
#line 607 ""
			s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 608 ""
		    }
#line 609 ""
		    lenline = lenstr_(line, &c__128, (ftnlen)128);

/*  Loop if only blanks are left */

#line 613 ""
		    if (lenline == 0) {
#line 613 ""
			goto L4;
#line 613 ""
		    }

/*  Must check for multiple "I" commands, so go to just after start of ia loop */

#line 617 ""
		    if (ia == 10) {
#line 617 ""
			goto L24;
#line 617 ""
		    }

/*  Tack on front part from 1st line of saved macro */

#line 621 ""
		    if (lenhold > 0) {
/* Writing concatenation */
#line 622 ""
			i__4[0] = lenhold, a__2[0] = holdln;
#line 622 ""
			i__4[1] = 1, a__2[1] = " ";
#line 622 ""
			i__4[2] = lenline, a__2[2] = line;
#line 622 ""
			s_cat(line, a__2, i__4, &c__3, (ftnlen)128);
#line 623 ""
			lenhold = 0;
#line 624 ""
		    }
#line 625 ""
		}
#line 626 ""
/* L5: */
#line 626 ""
	    }
#line 627 ""
	}
#line 627 ""
    }

/*  Now a special loop to deal with 'X'.  If it was %[n]X..., will have been */
/*  copied into part [n] already.  If no "B" or "P", remove.  If "P", just */
/*  remove the "P" so pmxa/b will process.  If "B". do nothing. */

#line 633 ""
L10:
#line 634 ""
    nchk = 1;
#line 635 ""
L13:
#line 635 ""
    ntinx = nchk - 1 + ntindex_(line + (nchk - 1), "X", 128 - (nchk - 1), (
	    ftnlen)1);
#line 636 ""
    if (ntinx > nchk - 1) {

/*  There is a non-TeX 'X' at ntinx.  Loop if neither 1st nor after a blank. */

#line 640 ""
	if (ntinx > 1) {
#line 641 ""
	    i__1 = ntinx - 2;
#line 641 ""
	    if (s_cmp(line + i__1, " ", ntinx - 1 - i__1, (ftnlen)1) != 0) {

/*  The X is not 1st char of PMX command.  Advance starting point, loop. */

#line 645 ""
		nchk = ntinx + 1;
#line 646 ""
		goto L13;
#line 647 ""
	    }
#line 648 ""
	}

/*  We now know the X at ntinx starts a PMX command.  Find next blank */

#line 652 ""
	i__1 = ntinx;
#line 652 ""
	ib = ntinx + i_indx(line + i__1, " ", 128 - i__1, (ftnlen)1);

/*  There must be a blank to right of "X", so ib>ntinx */

/*        locp = nchk-1+index(line(nchk:ib),'P') */
#line 657 ""
	i__1 = ntinx;
#line 657 ""
	locp = ntinx + i_indx(line + i__1, "P", ib - i__1, (ftnlen)1);

/*  Did not need to use ntindex because we already know bounds of PMX command. */

/*        if (locp .gt. nchk-1) then */
#line 662 ""
	if (locp > ntinx) {

/*  Strip out the 'P' */

#line 666 ""
	    s_copy(templine, line, (ftnlen)128, locp - 1);
#line 667 ""
	    i__1 = locp;
/* Writing concatenation */
#line 667 ""
	    i__3[0] = locp - 1, a__1[0] = templine;
#line 667 ""
	    i__3[1] = lenline - i__1, a__1[1] = line + i__1;
#line 667 ""
	    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 668 ""
	    --lenline;
#line 669 ""
	    --ib;
#line 670 ""
	}
#line 671 ""
	if (i_indx(line + (ntinx - 1), ":", ib - (ntinx - 1), (ftnlen)1) > 0 
		|| i_indx(line + (ntinx - 1), "S", ib - (ntinx - 1), (ftnlen)
		1) > 0 || i_indx(line + (ntinx - 1), "B", ib - (ntinx - 1), (
		ftnlen)1) > 0 || locp > ntinx) {

/*  The X command is a shift, "Both", or "Part".  Do not remove. */

#line 677 ""
	    nchk = ib + 1;
#line 678 ""
	    goto L13;
#line 679 ""
	}

/*  Remove the X command. */

#line 683 ""
	if (ntinx == 1) {
#line 684 ""
	    if (ib < lenline) {
#line 685 ""
		i__1 = ib;
#line 685 ""
		s_copy(line, line + i__1, (ftnlen)128, lenline - i__1);
#line 686 ""
	    } else {

/*  line contains ONLY the "X" command, so get a new line */

#line 690 ""
		goto L4;
#line 691 ""
	    }
#line 692 ""
	} else {
#line 693 ""
	    s_copy(templine, line, (ftnlen)128, ntinx - 1);
#line 694 ""
	    if (ib < lenline) {
#line 695 ""
		i__1 = ib;
/* Writing concatenation */
#line 695 ""
		i__3[0] = ntinx - 1, a__1[0] = templine;
#line 695 ""
		i__3[1] = lenline - i__1, a__1[1] = line + i__1;
#line 695 ""
		s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 696 ""
	    } else {
#line 697 ""
		s_copy(line, templine, (ftnlen)128, ntinx - 1);
#line 698 ""
	    }
#line 699 ""
	}

/*  Recompute lenline */

#line 703 ""
	lenline = lenstr_(line, &c__128, (ftnlen)128);

/*  Resume checking after location of removed command. */

#line 707 ""
	nchk = ntinx;
#line 708 ""
	goto L13;
#line 709 ""
    }

/*  End of loop for X-checks */

#line 713 ""
    oneof2 = ntindex_(line, "//", (ftnlen)128, (ftnlen)2) > 0;
#line 714 ""
    if (termrpt && all_1.botv[iv - 1] && frstln && *(unsigned char *)&line[
	    lenline - 1] == '/') {

/*  Must add a terminal repeat before the slash */

#line 719 ""
	if (oneof2) {
#line 719 ""
	    --lenline;
#line 719 ""
	}
#line 720 ""
	if (lenline > 1) {
#line 720 ""
	    io___102.ciunit = all_1.iorig[iinst - 1] + 10;
#line 720 ""
	    s_wsfe(&io___102);
#line 720 ""
	    do_fio(&c__1, line, lenline - 1);
#line 720 ""
	    e_wsfe();
#line 720 ""
	}
#line 721 ""
	if (! oneof2) {
/* Writing concatenation */
#line 722 ""
	    i__3[0] = 2, a__1[0] = termsym;
#line 722 ""
	    i__3[1] = 2, a__1[1] = " /";
#line 722 ""
	    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 723 ""
	    lenline = 4;
#line 724 ""
	} else {
/* Writing concatenation */
#line 725 ""
	    i__3[0] = 2, a__1[0] = termsym;
#line 725 ""
	    i__3[1] = 3, a__1[1] = " //";
#line 725 ""
	    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
#line 726 ""
	    lenline = 5;
#line 727 ""
	}
#line 728 ""
    }
#line 729 ""
    if (termrpt && frstln && *(unsigned char *)&line[lenline - 1] == '/' && 
	    iv == all_1.nvnow) {
#line 729 ""
	termrpt = FALSE_;
#line 729 ""
    }
#line 731 ""
    io___103.ciunit = all_1.iorig[iinst - 1] + 10;
#line 731 ""
    s_wsfe(&io___103);
#line 731 ""
    do_fio(&c__1, line, lenline);
#line 731 ""
    e_wsfe();
#line 732 ""
    if (oneof2) {
#line 733 ""
	frstln = FALSE_;
#line 734 ""
    } else if (! frstln) {
#line 735 ""
	frstln = TRUE_;
#line 736 ""
    }
/*      if (ntindex(line,'/').gt.0 .and. index(line,'//').eq.0) then */
#line 738 ""
    if (ntindex_(line, "/", (ftnlen)128, (ftnlen)1) > 0 && ntindex_(line, 
	    "//", (ftnlen)128, (ftnlen)2) == 0) {
#line 739 ""
	iv = iv % all_1.nvnow + 1;
#line 740 ""
	iinst = all_1.instnum[iv - 1];
#line 741 ""
    }
#line 742 ""
    goto L4;
#line 743 ""
L999:
#line 744 ""
    cl__1.cerr = 0;
#line 744 ""
    cl__1.cunit = 10;
#line 744 ""
    cl__1.csta = 0;
#line 744 ""
    f_clos(&cl__1);

/*  In the mbrest checks, must run through ALL noinst files (not just noinow) */

#line 748 ""
    i__1 = all_1.noinst;
#line 748 ""
    for (iinst = 1; iinst <= i__1; ++iinst) {
/* cc+++ */
/* c */
/* c  Temporarily transfer entire scratch file to real file */
/* c */
/*        rewind(10+iinst) */
/*        open(40,file='s2pout'//char(48+iinst)//'.pmx') */
/*        do 50 m = 1 , 10000 */
/*          read(10+iinst,'(a)',end=51)line */
/*          lenline = lenstr(line,128) */
/*          if (lenline .ge. 1) then */
/*            write(40,'(a)')line(1:lenline) */
/*          else */
/*            write(40,'(a)')' ' */
/*          end if */
/* 50      continue */
/* 51      continue */
/*        close(40) */
/* cc+++ */
#line 767 ""
	if (all_1.nvi[iinst - 1] == 1) {
#line 768 ""
	    mbrests_(&iinst, jobname, &ljob, &ludpfn[iinst - 1], udpfnq + (
		    iinst - 1 << 7), (ftnlen)27, (ftnlen)128);
#line 769 ""
	} else {

/*  Send a signal with ljob to bypass most mbrest processing */

#line 773 ""
	    i__2 = -ljob;
#line 773 ""
	    mbrests_(&iinst, jobname, &i__2, &ludpfn[iinst - 1], udpfnq + (
		    iinst - 1 << 7), (ftnlen)27, (ftnlen)128);
#line 774 ""
	}
#line 775 ""
/* L11: */
#line 775 ""
    }
#line 776 ""
    return 0;
} /* MAIN__ */

integer lenstr_(char *string, integer *n, ftnlen string_len)
{
    /* System generated locals */
    integer ret_val;

#line 779 ""
    for (ret_val = *n; ret_val >= 1; --ret_val) {
#line 780 ""
	if (*(unsigned char *)&string[ret_val - 1] != ' ') {
#line 780 ""
	    return ret_val;
#line 780 ""
	}
#line 781 ""
/* L1: */
#line 781 ""
    }
#line 782 ""
    ret_val = 0;
#line 783 ""
    return ret_val;
} /* lenstr_ */

/* Subroutine */ int allparts_(char *string, integer *n, ftnlen string_len)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    static integer len, iinst;
    extern integer lenstr_(char *, integer *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___106 = { 0, 0, 0, "(a)", 0 };


#line 791 ""
    len = lenstr_(string, n, string_len);
#line 792 ""
    if (len == 0) {
#line 793 ""
	len = 1;
#line 794 ""
	s_copy(string, " ", string_len, (ftnlen)1);
#line 795 ""
    }
#line 796 ""
    i__1 = all_1.noinow;
#line 796 ""
    for (iinst = 1; iinst <= i__1; ++iinst) {
#line 797 ""
	io___106.ciunit = all_1.iorig[iinst - 1] + 10;
#line 797 ""
	s_wsfe(&io___106);
#line 797 ""
	do_fio(&c__1, string, len);
#line 797 ""
	e_wsfe();
#line 798 ""
/* L1: */
#line 798 ""
    }
#line 799 ""
    return 0;
} /* allparts_ */

/* Subroutine */ int mbrests_(integer *iv, char *jobname, integer *ljob, 
	integer *ludpfn, char *udpfnq, ftnlen jobname_len, ftnlen udpfnq_len)
{
    /* System generated locals */
    address a__1[2], a__2[3];
    integer i__1[2], i__2[3], i__3, i__4;
    real r__1;
    char ch__1[1], ch__2[18], ch__3[132], ch__4[4], ch__5[2], ch__6[10];
    cilist ci__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;
    alist al__1;

    /* Builtin functions */
    integer f_rew(alist *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    integer f_open(olist *), s_rsfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_rsfe(void), s_cmp(char *, char *, ftnlen, ftnlen), 
	    s_wsfe(cilist *), e_wsfe(void), s_wsle(cilist *), do_lio(integer *
	    , integer *, char *, ftnlen), e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer i_indx(char *, char *, ftnlen, ftnlen), s_rsfi(icilist *), e_rsfi(
	    void);
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    double r_lg10(real *);
    integer f_clos(cllist *);

    /* Local variables */
    static integer i__, il, iw;
    static char sq[1];
    static integer ip1, ipc, ipe, len, idx;
    static char sym[80];
    static integer ndig;
    static char line[128*50];
    static integer idxb, nmbr, idxs, lsym;
    static char line1[128];
    static logical type4;
    static integer icden, iline, lpart;
    static char partq[2];
    static integer lwbrs, nwbrs, lenbar, ipenew, mtrden;
    static logical wbrest;
    extern integer lenstr_(char *, integer *, ftnlen);
    static logical newmtr;
    static integer lwbrsx, mtrnum;
    static char wbrsym[3*2];
    static integer lenbeat;
    static logical alldone;
    extern integer ifnodur_(integer *, char *, ftnlen), ntindex_(char *, char 
	    *, ftnlen, ftnlen);
    static integer lenmult;
    extern /* Subroutine */ int dosetup_(integer *, char *, integer *, 
	    integer *, ftnlen);
    static logical rpfirst;
    extern /* Subroutine */ int fwbrsym_(integer *, integer *, char *, 
	    integer *, ftnlen), nextsym_(char *, integer *, integer *, 
	    integer *, char *, integer *, ftnlen, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___113 = { 0, 0, 0, "(a)", 0 };
    static cilist io___116 = { 0, 40, 0, "(a)", 0 };
    static cilist io___117 = { 0, 40, 0, "(a)", 0 };
    static cilist io___118 = { 0, 6, 0, 0, 0 };
    static cilist io___121 = { 0, 0, 1, "(a)", 0 };
    static cilist io___122 = { 0, 40, 0, "(a)", 0 };
    static cilist io___123 = { 0, 0, 0, "(a)", 0 };
    static cilist io___124 = { 0, 40, 0, "(a)", 0 };
    static cilist io___144 = { 0, 0, 1, "(a)", 0 };
    static cilist io___145 = { 0, 40, 0, "(a)", 0 };
    static cilist io___146 = { 0, 40, 0, "(a)", 0 };
    static cilist io___147 = { 0, 6, 0, 0, 0 };
    static cilist io___152 = { 0, 40, 0, "(a)", 0 };
    static cilist io___154 = { 0, 6, 0, 0, 0 };
    static cilist io___156 = { 0, 40, 0, "(a)", 0 };
    static cilist io___158 = { 0, 40, 0, "(a)", 0 };
    static cilist io___159 = { 0, 40, 0, "(a)", 0 };
    static cilist io___160 = { 0, 40, 0, "(a)", 0 };
    static cilist io___161 = { 0, 40, 0, "(a)", 0 };


#line 809 ""
    type4 = FALSE_;
#line 810 ""
    *(unsigned char *)sq = '\\';
#line 811 ""
    alldone = FALSE_;
#line 812 ""
    al__1.aerr = 0;
#line 812 ""
    al__1.aunit = *iv + 10;
#line 812 ""
    f_rew(&al__1);
#line 813 ""
    if (*iv < 10) {
#line 814 ""
	*(unsigned char *)partq = (char) (*iv + 48);
#line 815 ""
	lpart = 1;
#line 816 ""
    } else {
#line 817 ""
	lpart = 2;
#line 818 ""
	if (*iv < 20) {
/* Writing concatenation */
#line 819 ""
	    i__1[0] = 1, a__1[0] = "1";
#line 819 ""
	    *(unsigned char *)&ch__1[0] = *iv + 38;
#line 819 ""
	    i__1[1] = 1, a__1[1] = ch__1;
#line 819 ""
	    s_cat(partq, a__1, i__1, &c__2, (ftnlen)2);
#line 820 ""
	} else {
/* Writing concatenation */
#line 821 ""
	    i__1[0] = 1, a__1[0] = "2";
#line 821 ""
	    *(unsigned char *)&ch__1[0] = *iv + 28;
#line 821 ""
	    i__1[1] = 1, a__1[1] = ch__1;
#line 821 ""
	    s_cat(partq, a__1, i__1, &c__2, (ftnlen)2);
#line 822 ""
	}
#line 823 ""
    }
/* 130327 */
/*      open(40,file=jobname(1:abs(ljob))//partq(1:lpart)//'.pmx') */
#line 826 ""
    if (*ludpfn == 0) {
#line 827 ""
	o__1.oerr = 0;
#line 827 ""
	o__1.ounit = 40;
#line 827 ""
	o__1.ofnmlen = abs(*ljob) + lpart + 4;
/* Writing concatenation */
#line 827 ""
	i__2[0] = abs(*ljob), a__2[0] = jobname;
#line 827 ""
	i__2[1] = lpart, a__2[1] = partq;
#line 827 ""
	i__2[2] = 4, a__2[2] = ".pmx";
#line 827 ""
	s_cat(ch__2, a__2, i__2, &c__3, (ftnlen)18);
#line 827 ""
	o__1.ofnm = ch__2;
#line 827 ""
	o__1.orl = 0;
#line 827 ""
	o__1.osta = 0;
#line 827 ""
	o__1.oacc = 0;
#line 827 ""
	o__1.ofm = 0;
#line 827 ""
	o__1.oblnk = 0;
#line 827 ""
	f_open(&o__1);
#line 828 ""
    } else {
#line 829 ""
	o__1.oerr = 0;
#line 829 ""
	o__1.ounit = 40;
#line 829 ""
	o__1.ofnmlen = *ludpfn + 4;
/* Writing concatenation */
#line 829 ""
	i__1[0] = *ludpfn, a__1[0] = udpfnq;
#line 829 ""
	i__1[1] = 4, a__1[1] = ".pmx";
#line 829 ""
	s_cat(ch__3, a__1, i__1, &c__2, (ftnlen)132);
#line 829 ""
	o__1.ofnm = ch__3;
#line 829 ""
	o__1.orl = 0;
#line 829 ""
	o__1.osta = 0;
#line 829 ""
	o__1.oacc = 0;
#line 829 ""
	o__1.ofm = 0;
#line 829 ""
	o__1.oblnk = 0;
#line 829 ""
	f_open(&o__1);
#line 830 ""
    }
#line 831 ""
    for (i__ = 1; i__ <= 10000; ++i__) {
#line 832 ""
	io___113.ciunit = *iv + 10;
#line 832 ""
	s_rsfe(&io___113);
#line 832 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 832 ""
	e_rsfe();
#line 833 ""
	if (*(unsigned char *)&line[0] == '%' || s_cmp(line, "---", (ftnlen)3,
		 (ftnlen)3) == 0 || type4) {
#line 835 ""
	    len = lenstr_(line, &c__128, (ftnlen)128);
#line 836 ""
	    if (len > 0) {
#line 837 ""
		s_wsfe(&io___116);
#line 837 ""
		do_fio(&c__1, line, len);
#line 837 ""
		e_wsfe();
#line 838 ""
	    } else {
#line 839 ""
		s_wsfe(&io___117);
#line 839 ""
		do_fio(&c__1, " ", (ftnlen)1);
#line 839 ""
		e_wsfe();
#line 840 ""
	    }
#line 841 ""
	    if (s_cmp(line, "---", (ftnlen)3, (ftnlen)3) == 0) {
#line 841 ""
		type4 = ! type4;
#line 841 ""
	    }
#line 842 ""
	} else {
#line 843 ""
	    goto L11;
#line 844 ""
	}
#line 845 ""
/* L10: */
#line 845 ""
    }
#line 846 ""
    s_wsle(&io___118);
#line 846 ""
    do_lio(&c__9, &c__1, "You should not be here in scor2prt.  Call Dr. Don", 
	    (ftnlen)49);
#line 846 ""
    e_wsle();
#line 847 ""
    s_stop("", (ftnlen)0);
#line 848 ""
L11:

/*  Finished reading opening type4 TeX and comments.  Next line to be read */
/*  will contain the first of the input numbers */

#line 853 ""
    dosetup_(iv, line, &mtrnum, &mtrden, (ftnlen)128);
#line 854 ""
    for (i__ = 1; i__ <= 10000; ++i__) {
#line 855 ""
L13:
#line 855 ""
	io___121.ciunit = *iv + 10;
#line 855 ""
	i__3 = s_rsfe(&io___121);
#line 855 ""
	if (i__3 != 0) {
#line 855 ""
	    goto L999;
#line 855 ""
	}
#line 855 ""
	i__3 = do_fio(&c__1, line, (ftnlen)128);
#line 855 ""
	if (i__3 != 0) {
#line 855 ""
	    goto L999;
#line 855 ""
	}
#line 855 ""
	i__3 = e_rsfe();
#line 855 ""
	if (i__3 != 0) {
#line 855 ""
	    goto L999;
#line 855 ""
	}
#line 856 ""
L7:
#line 856 ""
	len = lenstr_(line, &c__128, (ftnlen)128);

/*  Pass-through (and copy into part file) if instrumnet has >1 voice. */

#line 860 ""
	if (*ljob < 0) {
#line 860 ""
	    goto L2;
#line 860 ""
	}
#line 861 ""
	if (i_indx("TtTiTch+h-h l ", line, (ftnlen)14, (ftnlen)2) > 0) {

/*  Traps titles, instruments, composers, headers, lower strings.  Read 2 lines. */

#line 865 ""
	    s_wsfe(&io___122);
#line 865 ""
	    do_fio(&c__1, line, len);
#line 865 ""
	    e_wsfe();
#line 866 ""
	    io___123.ciunit = *iv + 10;
#line 866 ""
	    s_rsfe(&io___123);
#line 866 ""
	    do_fio(&c__1, line, (ftnlen)128);
#line 866 ""
	    e_rsfe();
#line 867 ""
	    len = lenstr_(line, &c__128, (ftnlen)128);
#line 868 ""
	    goto L2;
#line 869 ""
	}
#line 870 ""
	if (i__ == 1 || i__ > 5 && *(unsigned char *)&line[0] == 'm') {

/*  Either just starting, or a new meter is defined. */
/*  NOTE! The above test may be bogus. */

#line 875 ""
	    if (*(unsigned char *)&line[0] == '%') {
#line 876 ""
		s_wsfe(&io___124);
#line 876 ""
		do_fio(&c__1, line, len);
#line 876 ""
		e_wsfe();
#line 877 ""
		goto L13;
#line 878 ""
	    }
#line 879 ""
	    if (i__ != 1) {

/*  New meter. Check for slashes (new meter change syntax) */

#line 883 ""
		idxs = i_indx(line, "/", (ftnlen)128, (ftnlen)1);
#line 884 ""
		idxb = i_indx(line, " ", (ftnlen)128, (ftnlen)1);
#line 885 ""
		newmtr = idxs > 0 && (idxb == 0 || idxs < idxb);
#line 886 ""
		if (! newmtr) {

/*  Old way, no slashes, uses 'o' for lonesome '1' */

#line 890 ""
		    icden = 3;
#line 891 ""
		    if (*(unsigned char *)&line[1] == 'o') {
#line 892 ""
			mtrnum = 1;
#line 893 ""
		    } else {
#line 894 ""
			mtrnum = *(unsigned char *)&line[1] - 48;
#line 895 ""
			if (mtrnum == 1) {
#line 896 ""
			    icden = 4;
#line 897 ""
			    mtrnum = *(unsigned char *)&line[2] - 38;
#line 898 ""
			}
#line 899 ""
		    }
#line 900 ""
		    mtrden = *(unsigned char *)&line[icden - 1] - 48;
#line 901 ""
		} else {

/*  New way with slashes: idxs is index of 1st slash! */

#line 905 ""
		    ici__1.icierr = 0;
#line 905 ""
		    ici__1.iciend = 0;
#line 905 ""
		    ici__1.icirnum = 1;
#line 905 ""
		    ici__1.icirlen = idxs - 2;
#line 905 ""
		    ici__1.iciunit = line + 1;
/* Writing concatenation */
#line 905 ""
		    i__2[0] = 2, a__2[0] = "(i";
#line 905 ""
		    *(unsigned char *)&ch__1[0] = idxs + 46;
#line 905 ""
		    i__2[1] = 1, a__2[1] = ch__1;
#line 905 ""
		    i__2[2] = 1, a__2[2] = ")";
#line 905 ""
		    ici__1.icifmt = (s_cat(ch__4, a__2, i__2, &c__3, (ftnlen)
			    4), ch__4);
#line 905 ""
		    s_rsfi(&ici__1);
#line 905 ""
		    do_fio(&c__1, (char *)&mtrnum, (ftnlen)sizeof(integer));
#line 905 ""
		    e_rsfi();
#line 906 ""
		    i__3 = idxs;
#line 906 ""
		    idxb = i_indx(line + i__3, "/", 128 - i__3, (ftnlen)1);
#line 907 ""
		    i__3 = idxs;
#line 907 ""
		    ici__1.icierr = 0;
#line 907 ""
		    ici__1.iciend = 0;
#line 907 ""
		    ici__1.icirnum = 1;
#line 907 ""
		    ici__1.icirlen = idxs + idxb - 1 - i__3;
#line 907 ""
		    ici__1.iciunit = line + i__3;
/* Writing concatenation */
#line 907 ""
		    i__2[0] = 2, a__2[0] = "(i";
#line 907 ""
		    *(unsigned char *)&ch__1[0] = idxb + 47;
#line 907 ""
		    i__2[1] = 1, a__2[1] = ch__1;
#line 907 ""
		    i__2[2] = 1, a__2[2] = ")";
#line 907 ""
		    ici__1.icifmt = (s_cat(ch__4, a__2, i__2, &c__3, (ftnlen)
			    4), ch__4);
#line 907 ""
		    s_rsfi(&ici__1);
#line 907 ""
		    do_fio(&c__1, (char *)&mtrden, (ftnlen)sizeof(integer));
#line 907 ""
		    e_rsfi();
#line 909 ""
		}
#line 910 ""
	    }
#line 911 ""
	    lenbeat = ifnodur_(&mtrden, "x", (ftnlen)1);
#line 912 ""
	    lenmult = 1;
#line 913 ""
	    if (mtrden == 2) {
#line 914 ""
		lenbeat = 16;
#line 915 ""
		lenmult = 2;
#line 916 ""
	    }
#line 917 ""
	    lenbar = lenmult * mtrnum * lenbeat;
#line 918 ""
	    fwbrsym_(&lenbar, &nwbrs, wbrsym, &lwbrs, (ftnlen)3);
#line 919 ""
	}

/* Finished setting up meter stuff and defining whole-bar rest symbols */

#line 923 ""
	ip1 = 0;
#line 924 ""
	s_copy(line1, line, (ftnlen)128, (ftnlen)128);
#line 925 ""
	i__3 = nwbrs;
#line 925 ""
	for (iw = 0; iw <= i__3; ++iw) {
#line 926 ""
	    if (iw > 0) {
#line 927 ""
		idx = ntindex_(line1, wbrsym + (iw - 1) * 3, (ftnlen)128, 
			lwbrs);
#line 928 ""
		if (idx > 0) {

/*  Check for blank or shifted rest, discount it if it's there */

#line 932 ""
		    i__4 = idx + lwbrs - 1;
#line 932 ""
		    if (s_cmp(line1 + i__4, " ", idx + lwbrs - i__4, (ftnlen)
			    1) != 0) {
#line 932 ""
			idx = 0;
#line 932 ""
		    }
#line 933 ""
		}
#line 934 ""
	    } else {
#line 935 ""
		idx = ntindex_(line1, "rp", (ftnlen)128, (ftnlen)2);

/*  Check for raised rest */

#line 939 ""
		if (idx > 0) {
#line 940 ""
		    i__4 = idx + 1;
#line 940 ""
		    if (s_cmp(line1 + i__4, " ", idx + 2 - i__4, (ftnlen)1) !=
			     0) {
#line 940 ""
			idx = 0;
#line 940 ""
		    }
#line 941 ""
		}
#line 942 ""
	    }
#line 943 ""
	    if (idx > 0) {
#line 944 ""
		if (ip1 == 0) {
#line 945 ""
		    ip1 = idx;
#line 946 ""
		} else {
#line 947 ""
		    ip1 = min(ip1,idx);
/* Maybe allows e.g. r0 rp ... */
#line 948 ""
		}
#line 949 ""
	    }
#line 950 ""
/* L3: */
#line 950 ""
	}
/* Writing concatenation */
#line 951 ""
	i__1[0] = 1, a__1[0] = sq;
#line 951 ""
	i__1[1] = 1, a__1[1] = sq;
#line 951 ""
	s_cat(ch__5, a__1, i__1, &c__2, (ftnlen)2);
#line 951 ""
	if (i__ < 5 || *(unsigned char *)&line[0] == '%' || s_cmp(line, ch__5,
		 (ftnlen)2, (ftnlen)2) == 0 || ip1 == 0) {
#line 951 ""
	    goto L2;
#line 951 ""
	}

/*  Switch to multibar rest search mode!!!  Start forward in line(1) */

#line 956 ""
	rpfirst = s_cmp(line1 + (ip1 - 1), "rp", (ftnlen)2, (ftnlen)2) == 0;
#line 957 ""
	iline = 1;
#line 958 ""
	nmbr = 1;
#line 959 ""
	if (rpfirst) {
#line 960 ""
	    lwbrsx = 2;
#line 961 ""
	} else {
#line 962 ""
	    lwbrsx = lwbrs;
#line 963 ""
	}
#line 964 ""
	ipe = ip1 + lwbrsx - 1;
/* ip at end of 1st wbrsym */
#line 965 ""
L4:
#line 965 ""
	if (ipe == len) {

/*  Need a new line */

#line 969 ""
	    ++iline;
#line 970 ""
L6:
#line 970 ""
	    io___144.ciunit = *iv + 10;
#line 970 ""
	    i__3 = s_rsfe(&io___144);
#line 970 ""
	    if (i__3 != 0) {
#line 970 ""
		goto L998;
#line 970 ""
	    }
#line 970 ""
	    i__3 = do_fio(&c__1, line + (iline - 1 << 7), (ftnlen)128);
#line 970 ""
	    if (i__3 != 0) {
#line 970 ""
		goto L998;
#line 970 ""
	    }
#line 970 ""
	    i__3 = e_rsfe();
#line 970 ""
	    if (i__3 != 0) {
#line 970 ""
		goto L998;
#line 970 ""
	    }
#line 971 ""
	    len = lenstr_(line + (iline - 1 << 7), &c__128, (ftnlen)128);
#line 972 ""
	    if (*(unsigned char *)&line[(iline - 1) * 128] == '%') {
#line 973 ""
		s_wsfe(&io___145);
#line 973 ""
		do_fio(&c__1, "% Following comment has been moved forward", (
			ftnlen)42);
#line 973 ""
		e_wsfe();
#line 974 ""
		s_wsfe(&io___146);
#line 974 ""
		do_fio(&c__1, line + (iline - 1 << 7), len);
#line 974 ""
		e_wsfe();
#line 975 ""
		goto L6;
#line 976 ""
	    }
#line 977 ""
	    ipe = 0;
#line 978 ""
	    goto L4;
#line 979 ""
L998:

/*  No more input left */

#line 983 ""
	    s_wsle(&io___147);
#line 983 ""
	    do_lio(&c__9, &c__1, "All done!", (ftnlen)9);
#line 983 ""
	    e_wsle();
#line 984 ""
	    alldone = TRUE_;
#line 985 ""
	    ipe = 0;
#line 986 ""
	    --iline;
#line 987 ""
	    len = lenstr_(line + (iline - 1 << 7), &c__128, (ftnlen)128);
#line 988 ""
	    goto L4;
#line 989 ""
	} else {
#line 990 ""
	    if (alldone) {
#line 991 ""
		*(unsigned char *)sym = ' ';
#line 992 ""
	    } else {

/*  ipe<len here, so it's ok to get a symbol */

#line 996 ""
		nextsym_(line + (iline - 1 << 7), &len, &ipe, &ipenew, sym, &
			lsym, (ftnlen)128, (ftnlen)80);
#line 997 ""
	    }

/*  Check for end of block or bar line symbol */

#line 1001 ""
	    if (i_indx("/|", sym, (ftnlen)2, (ftnlen)1) > 0) {
#line 1002 ""
		ipe = ipenew;
#line 1003 ""
		goto L4;
#line 1004 ""
	    } else {
#line 1005 ""
		wbrest = FALSE_;
#line 1006 ""
		if (alldone) {
#line 1006 ""
		    goto L12;
#line 1006 ""
		}
#line 1007 ""
		i__3 = nwbrs;
#line 1007 ""
		for (iw = 1; iw <= i__3; ++iw) {
#line 1008 ""
		    wbrest = wbrest || s_cmp(sym, wbrsym + (iw - 1) * 3, lsym,
			     lwbrs) == 0;
#line 1009 ""
/* L5: */
#line 1009 ""
		}
#line 1010 ""
		wbrest = wbrest || s_cmp(sym, "r", lsym, (ftnlen)1) == 0 && 
			lwbrs == 2 || s_cmp(sym, "rd", lsym, (ftnlen)2) == 0 
			&& lwbrs == 3 || s_cmp(sym, "rp", lsym, (ftnlen)2) == 
			0 || s_cmp(sym, "r", lsym, (ftnlen)1) == 0 && rpfirst;
#line 1014 ""
L12:
#line 1014 ""
		if (wbrest) {
#line 1015 ""
		    ipe = ipenew;
#line 1016 ""
		    ++nmbr;
#line 1017 ""
		    goto L4;
#line 1018 ""
		} else {

/*  AHA! Failed prev. test, so last symbol was *not* mbr. */
/*  It must be saved, and its starting position is ipenew-lsym+1 */

#line 1023 ""
		    if (nmbr > 1) {

/*  Write stuff up to start of mbr */

#line 1027 ""
			if (ip1 > 1) {
#line 1027 ""
			    s_wsfe(&io___152);
#line 1027 ""
			    do_fio(&c__1, line, ip1 - 1);
#line 1027 ""
			    e_wsfe();
#line 1027 ""
			}

/*  Insert mbr symbol.  Always end with a slash just in case next sym must be */
/*  at start of block.  May think this causes undefined octaves, but */
/*  probably not since it's a single voice. */

#line 1033 ""
			r__1 = nmbr + .01f;
#line 1033 ""
			ndig = (integer) r_lg10(&r__1) + 1;
#line 1034 ""
			s_wsle(&io___154);
#line 1034 ""
			do_lio(&c__9, &c__1, "Inserting rm, iv,nmbr:", (
				ftnlen)22);
#line 1034 ""
			do_lio(&c__3, &c__1, (char *)&(*iv), (ftnlen)sizeof(
				integer));
#line 1034 ""
			do_lio(&c__3, &c__1, (char *)&nmbr, (ftnlen)sizeof(
				integer));
#line 1034 ""
			e_wsle();
#line 1035 ""
			ci__1.cierr = 0;
#line 1035 ""
			ci__1.ciunit = 40;
/* Writing concatenation */
#line 1035 ""
			i__2[0] = 5, a__2[0] = "(a2,i";
#line 1035 ""
			*(unsigned char *)&ch__1[0] = ndig + 48;
#line 1035 ""
			i__2[1] = 1, a__2[1] = ch__1;
#line 1035 ""
			i__2[2] = 4, a__2[2] = ",a2)";
#line 1035 ""
			ci__1.cifmt = (s_cat(ch__6, a__2, i__2, &c__3, (
				ftnlen)10), ch__6);
#line 1035 ""
			s_wsfe(&ci__1);
#line 1035 ""
			do_fio(&c__1, "rm", (ftnlen)2);
#line 1035 ""
			do_fio(&c__1, (char *)&nmbr, (ftnlen)sizeof(integer));
#line 1035 ""
			do_fio(&c__1, " /", (ftnlen)2);
#line 1035 ""
			e_wsfe();
#line 1036 ""
			if (alldone) {
#line 1036 ""
			    goto L999;
#line 1036 ""
			}
#line 1037 ""
			ipc = ipenew - lsym + 1;
#line 1038 ""
			s_copy(line, line + ((iline - 1 << 7) + (ipc - 1)), (
				ftnlen)128, len - (ipc - 1));
#line 1039 ""
		    } else {

/*  Write old stuff up to end of original lonesome wbr, save the rest. */
/*  4 cases:  (wbr /) , (wbr line-end) , (wbr followed by other non-/ symbols) , */
/*      alldone. */
/*  In 1st 2 will have gotten some other lines, so write all up to one b4 last */
/*  non-comment line; then revert to normal mode on that.  In 3rd case must */
/*  split line. */

#line 1048 ""
			if (alldone) {
#line 1049 ""
			    s_wsfe(&io___156);
#line 1049 ""
			    do_fio(&c__1, line, len);
#line 1049 ""
			    e_wsfe();
#line 1050 ""
			    goto L999;
#line 1051 ""
			} else if (iline > 1) {
#line 1052 ""
			    i__3 = iline - 1;
#line 1052 ""
			    for (il = 1; il <= i__3; ++il) {
#line 1053 ""
				len = lenstr_(line + (il - 1 << 7), &c__128, (
					ftnlen)128);
#line 1054 ""
				s_wsfe(&io___158);
#line 1054 ""
				do_fio(&c__1, line + (il - 1 << 7), len);
#line 1054 ""
				e_wsfe();
#line 1055 ""
/* L9: */
#line 1055 ""
			    }
#line 1056 ""
			    s_copy(line, line + (iline - 1 << 7), (ftnlen)128,
				     (ftnlen)128);
#line 1057 ""
			} else {

/*  Since iline = 1 the wbr is not the last sym, so must split */

#line 1061 ""
			    s_wsfe(&io___159);
#line 1061 ""
			    do_fio(&c__1, line, ip1 + lwbrsx - 1);
#line 1061 ""
			    e_wsfe();
#line 1062 ""
			    i__3 = ip1 + lwbrsx;
#line 1062 ""
			    s_copy(line, line + i__3, (ftnlen)128, len - i__3)
				    ;
#line 1063 ""
			}
#line 1064 ""
		    }

/*  Exit multibar mode */

#line 1068 ""
		    goto L7;
#line 1069 ""
		}
#line 1070 ""
	    }
#line 1071 ""
	}
#line 1072 ""
L2:
#line 1073 ""
	if (len > 0) {
#line 1074 ""
	    s_wsfe(&io___160);
#line 1074 ""
	    do_fio(&c__1, line, len);
#line 1074 ""
	    e_wsfe();
#line 1075 ""
	} else {
#line 1076 ""
	    s_wsfe(&io___161);
#line 1076 ""
	    do_fio(&c__1, " ", (ftnlen)1);
#line 1076 ""
	    e_wsfe();
#line 1077 ""
	}
#line 1078 ""
/* L1: */
#line 1078 ""
    }
#line 1079 ""
L999:
#line 1080 ""
    cl__1.cerr = 0;
#line 1080 ""
    cl__1.cunit = *iv + 10;
#line 1080 ""
    cl__1.csta = 0;
#line 1080 ""
    f_clos(&cl__1);
#line 1081 ""
    cl__1.cerr = 0;
#line 1081 ""
    cl__1.cunit = 40;
#line 1081 ""
    cl__1.csta = 0;
#line 1081 ""
    f_clos(&cl__1);
#line 1082 ""
    return 0;
} /* mbrests_ */

integer ifnodur_(integer *idur, char *dotq, ftnlen dotq_len)
{
    /* System generated locals */
    integer ret_val;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___162 = { 0, 6, 0, 0, 0 };


#line 1086 ""
    if (*idur == 6) {
#line 1087 ""
	ret_val = 1;
#line 1088 ""
    } else if (*idur == 3) {
#line 1089 ""
	ret_val = 2;
#line 1090 ""
    } else if (*idur == 1 || *idur == 16) {
#line 1091 ""
	ret_val = 4;
#line 1092 ""
    } else if (*idur == 8) {
#line 1093 ""
	ret_val = 8;
#line 1094 ""
    } else if (*idur == 4) {
#line 1095 ""
	ret_val = 16;
#line 1096 ""
    } else if (*idur == 2) {
#line 1097 ""
	ret_val = 32;
#line 1098 ""
    } else if (*idur == 0) {
#line 1099 ""
	ret_val = 64;
#line 1100 ""
    } else {
#line 1101 ""
	s_wsle(&io___162);
#line 1101 ""
	do_lio(&c__9, &c__1, "You entered an invalid note-length value", (
		ftnlen)40);
#line 1101 ""
	e_wsle();
#line 1102 ""
	s_stop("", (ftnlen)0);
#line 1103 ""
    }
#line 1104 ""
    if (*(unsigned char *)dotq == 'd') {
#line 1104 ""
	ret_val = ret_val * 3 / 2;
#line 1104 ""
    }
#line 1105 ""
    return ret_val;
} /* ifnodur_ */

/* Subroutine */ int fwbrsym_(integer *lenbar, integer *nwbrs, char *wbrsym, 
	integer *lwbrs, ftnlen wbrsym_len)
{
    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Fortran I/O blocks */
    static cilist io___163 = { 0, 6, 0, "(33H Any whole-bar rests of duratio"
	    "n ,i3,                  26H/64 will not be recognized)", 0 };


#line 1109 ""
    /* Parameter adjustments */
#line 1109 ""
    wbrsym -= 3;
#line 1109 ""

#line 1109 ""
    /* Function Body */
#line 1109 ""
    *nwbrs = 1;
#line 1110 ""
    *lwbrs = 2;
#line 1111 ""
    if (*lenbar == 16) {
#line 1112 ""
	s_copy(wbrsym + 3, "r4", (ftnlen)3, (ftnlen)2);
#line 1113 ""
    } else if (*lenbar == 32) {
#line 1114 ""
	s_copy(wbrsym + 3, "r2", (ftnlen)3, (ftnlen)2);
#line 1115 ""
    } else if (*lenbar == 64) {
#line 1116 ""
	s_copy(wbrsym + 3, "r0", (ftnlen)3, (ftnlen)2);
#line 1117 ""
    } else if (*lenbar == 8) {
#line 1118 ""
	s_copy(wbrsym + 3, "r8", (ftnlen)3, (ftnlen)2);
#line 1119 ""
    } else if (*lenbar == 128) {
#line 1120 ""
	s_copy(wbrsym + 3, "r9", (ftnlen)3, (ftnlen)2);
#line 1121 ""
    } else {
#line 1122 ""
	*nwbrs = 2;
#line 1123 ""
	*lwbrs = 3;
#line 1124 ""
	if (*lenbar == 24) {
#line 1125 ""
	    s_copy(wbrsym + 3, "rd4", (ftnlen)3, (ftnlen)3);
#line 1126 ""
	    s_copy(wbrsym + 6, "r4d", (ftnlen)3, (ftnlen)3);
#line 1127 ""
	} else if (*lenbar == 48) {
#line 1128 ""
	    s_copy(wbrsym + 3, "rd2", (ftnlen)3, (ftnlen)3);
#line 1129 ""
	    s_copy(wbrsym + 6, "r2d", (ftnlen)3, (ftnlen)3);
#line 1130 ""
	} else if (*lenbar == 96) {
#line 1131 ""
	    s_copy(wbrsym + 3, "rd0", (ftnlen)3, (ftnlen)3);
#line 1132 ""
	    s_copy(wbrsym + 6, "r0d", (ftnlen)3, (ftnlen)3);
#line 1133 ""
	} else {
#line 1134 ""
	    s_wsfe(&io___163);
#line 1134 ""
	    do_fio(&c__1, (char *)&(*lenbar), (ftnlen)sizeof(integer));
#line 1134 ""
	    e_wsfe();
#line 1136 ""
	}
#line 1137 ""
    }
#line 1138 ""
    return 0;
} /* fwbrsym_ */

/* Subroutine */ int nextsym_(char *line, integer *len, integer *ipeold, 
	integer *ipenew, char *sym, integer *lsym, ftnlen line_len, ftnlen 
	sym_len)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen), s_copy(char *, char *, 
	    ftnlen, ftnlen);

    /* Local variables */
    static integer ip, iip;

    /* Fortran I/O blocks */
    static cilist io___164 = { 0, 6, 0, 0, 0 };
    static cilist io___165 = { 0, 6, 0, 0, 0 };
    static cilist io___168 = { 0, 6, 0, 0, 0 };



/*  Know its the last symbol if on return ipenew = len!.  So should never */
/*    be called when ipstart=len. */

#line 1147 ""
    if (*ipeold >= *len) {
#line 1148 ""
	s_wsle(&io___164);
#line 1148 ""
	do_lio(&c__9, &c__1, "Called nextsym with ipstart>=len ", (ftnlen)33);
#line 1148 ""
	e_wsle();
#line 1149 ""
	s_wsle(&io___165);
#line 1149 ""
	do_lio(&c__9, &c__1, "Send files to Dr. Don at dsimons@logicon.com", (
		ftnlen)44);
#line 1149 ""
	e_wsle();
#line 1150 ""
	s_stop("", (ftnlen)0);
#line 1151 ""
    }
#line 1152 ""
    i__1 = *len;
#line 1152 ""
    for (ip = *ipeold + 1; ip <= i__1; ++ip) {
#line 1153 ""
	if (*(unsigned char *)&line[ip - 1] != ' ') {

/*  symbol starts here (ip).  We're committed to exit the loop. */

#line 1157 ""
	    if (ip < *len) {
#line 1158 ""
		i__2 = *len;
#line 1158 ""
		for (iip = ip + 1; iip <= i__2; ++iip) {
#line 1159 ""
		    if (*(unsigned char *)&line[iip - 1] != ' ') {
#line 1159 ""
			goto L2;
#line 1159 ""
		    }

/*  iip is the space after the symbol */

#line 1163 ""
		    *ipenew = iip - 1;
#line 1164 ""
		    *lsym = *ipenew - ip + 1;
#line 1165 ""
		    s_copy(sym, line + (ip - 1), (ftnlen)80, *ipenew - (ip - 
			    1));
#line 1166 ""
		    return 0;
#line 1167 ""
L2:
#line 1167 ""
		    ;
#line 1167 ""
		}

/*  Have len>=2 and ends on len */

#line 1171 ""
		*ipenew = *len;
#line 1172 ""
		*lsym = *ipenew - ip + 1;
#line 1173 ""
		s_copy(sym, line + (ip - 1), (ftnlen)80, *ipenew - (ip - 1));
#line 1174 ""
		return 0;
#line 1175 ""
	    } else {

/*  ip = len */

#line 1179 ""
		*ipenew = *len;
#line 1180 ""
		*lsym = 1;
#line 1181 ""
		s_copy(sym, line + (ip - 1), (ftnlen)80, (ftnlen)1);
#line 1182 ""
		return 0;
#line 1183 ""
	    }
#line 1184 ""
	}
#line 1185 ""
/* L1: */
#line 1185 ""
    }
#line 1186 ""
    s_wsle(&io___168);
#line 1186 ""
    do_lio(&c__9, &c__1, "Error #3.  Send files to Dr. Don at dsimons@logico"\
	    "n.com", (ftnlen)55);
#line 1186 ""
    e_wsle();
#line 1187 ""
    return 0;
} /* nextsym_ */

integer ntindex_(char *line, char *s2q, ftnlen line_len, ftnlen s2q_len)
{
    /* System generated locals */
    address a__1[2];
    integer ret_val, i__1, i__2[2], i__3;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);
    integer i_indx(char *, char *, ftnlen, ftnlen), s_wsle(cilist *), do_lio(
	    integer *, integer *, char *, ftnlen), e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen), s_cat(char *, char **, 
	    integer *, integer *, ftnlen);
    integer s_cmp(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer ic, len, ndxs2;
    static char tline[128];
    static integer ndxbs;
    static logical intex;
    static integer ndxdq1, ndxdq2;
    extern integer lenstr_(char *, integer *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___174 = { 0, 6, 0, 0, 0 };



/*  Returns index(line,s2q) if NOT in TeX string, 0 otherwise */


/*     print*,'Starting ntindex.  s2q:',s2q,', line(1:79) is below' */
/*     print*,line(1:79) */


/*  Use a temporary string to store the input and test, so can zap D"..." */

#line 1202 ""
    s_copy(tline, line, (ftnlen)128, (ftnlen)128);
#line 1203 ""
    ndxs2 = i_indx(tline, s2q, (ftnlen)128, s2q_len);

/*  Return point below for rechecks after zapping D"  " */

#line 1207 ""
L2:
#line 1208 ""
    ndxbs = i_indx(tline, "\\", (ftnlen)128, (ftnlen)1);
#line 1209 ""
    if (ndxbs > 0) {

/* Special check in case \ is inside D"..." */

#line 1213 ""
	ndxdq1 = i_indx(tline, "D\"", (ftnlen)128, (ftnlen)2);

/* If the following test fails, flow out of if block; else loop up to 2. */

#line 1217 ""
	if (ndxdq1 > 0) {

/* Find end of D"..." */

#line 1221 ""
	    i__1 = ndxdq1 + 1;
#line 1221 ""
	    ndxdq2 = ndxdq1 + 1 + i_indx(tline + i__1, "\"", 128 - i__1, (
		    ftnlen)1);
#line 1222 ""
	    if (ndxdq2 == ndxdq1 + 1) {
#line 1223 ""
		s_wsle(&io___174);
#line 1223 ""
		do_lio(&c__9, &c__1, "Something is really wierd here", (
			ftnlen)30);
#line 1223 ""
		e_wsle();
#line 1224 ""
		s_stop("", (ftnlen)0);
#line 1225 ""
	    }
#line 1226 ""
	    s_copy(tline, tline, (ftnlen)128, ndxdq1 - 1);
#line 1227 ""
	    i__1 = ndxdq2;
#line 1227 ""
	    for (ic = ndxdq1; ic <= i__1; ++ic) {
/* Writing concatenation */
#line 1228 ""
		i__2[0] = ic - 1, a__1[0] = tline;
#line 1228 ""
		i__2[1] = 1, a__1[1] = " ";
#line 1228 ""
		s_cat(tline, a__1, i__2, &c__2, (ftnlen)128);
#line 1229 ""
/* L3: */
#line 1229 ""
	    }
#line 1230 ""
	    i__1 = ndxdq2;
/* Writing concatenation */
#line 1230 ""
	    i__2[0] = ndxdq2, a__1[0] = tline;
#line 1230 ""
	    i__2[1] = 128 - i__1, a__1[1] = line + i__1;
#line 1230 ""
	    s_cat(tline, a__1, i__2, &c__2, (ftnlen)128);
#line 1231 ""
	    goto L2;
#line 1232 ""
	}
#line 1233 ""
    }
#line 1234 ""
    if (ndxbs == 0 || ndxs2 < ndxbs) {
#line 1235 ""
	ret_val = ndxs2;
/*     print*,'No bs, or char is left of 1st bs, ntindex:',ntindex */
#line 1237 ""
    } else {

/*  There are both bs and s2q, and bs is to the left of sq2. So check bs's to */
/*  right of first: End is '\ ', start is ' \' */

#line 1242 ""
	len = lenstr_(tline, &c__128, (ftnlen)128);
#line 1243 ""
	intex = TRUE_;
/*     print*,'intex+>',intex */
#line 1245 ""
	i__1 = len;
#line 1245 ""
	for (ic = ndxbs + 1; ic <= i__1; ++ic) {
#line 1246 ""
	    if (ic == ndxs2) {
#line 1247 ""
		if (intex) {
#line 1248 ""
		    ret_val = 0;
#line 1249 ""
		    i__3 = ic;
#line 1249 ""
		    ndxs2 = i_indx(tline + i__3, s2q, len - i__3, s2q_len) + 
			    ic;
/*     print*,'ndxs2 =>',ndxs2 */
#line 1251 ""
		} else {
#line 1252 ""
		    ret_val = ndxs2;
#line 1253 ""
		    return ret_val;
#line 1254 ""
		}
/*     print*,'Internal exit, intex, ntindex:',intex,ntindex */
#line 1256 ""
	    } else /* if(complicated condition) */ {
#line 1256 ""
		i__3 = ic;
#line 1256 ""
		if (intex && s_cmp(tline + i__3, "\\ ", ic + 2 - i__3, (
			ftnlen)2) == 0) {
#line 1257 ""
		    intex = FALSE_;
/*     print*,'intex+>',intex */
#line 1259 ""
		} else /* if(complicated condition) */ {
#line 1259 ""
		    i__3 = ic;
#line 1259 ""
		    if (! intex && s_cmp(tline + i__3, " \\", ic + 2 - i__3, (
			    ftnlen)2) == 0) {
#line 1261 ""
			intex = TRUE_;
/*     print*,'intex+>',intex */
#line 1263 ""
		    }
#line 1263 ""
		}
#line 1263 ""
	    }
#line 1264 ""
/* L1: */
#line 1264 ""
	}
/*     print*,'Out end of loop 1' */
#line 1266 ""
    }
/*     print*,'Exiting ntindex at the end???' */
#line 1268 ""
    return ret_val;
} /* ntindex_ */

/* Subroutine */ int getchar_(char *line, integer *iccount, char *charq, 
	ftnlen line_len, ftnlen charq_len)
{
    /* Builtin functions */
    integer s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), e_rsfe(void);

    /* Fortran I/O blocks */
    static cilist io___178 = { 0, 10, 0, "(a)", 0 };



/*  Gets the next character out of line*128.  If pointer iccount=128 on entry, */
/*  then reads in a new line.  Resets iccount to position of the new character. */

#line 1277 ""
    if (*iccount == 128) {
#line 1278 ""
	s_rsfe(&io___178);
#line 1278 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 1278 ""
	e_rsfe();
#line 1279 ""
	*iccount = 0;
#line 1280 ""
    }
#line 1281 ""
    ++(*iccount);
#line 1282 ""
    *(unsigned char *)charq = *(unsigned char *)&line[*iccount - 1];
#line 1283 ""
    return 0;
} /* getchar_ */

doublereal readin_(char *line, integer *iccount, integer *iread, ftnlen 
	line_len)
{
    /* System generated locals */
    address a__1[3];
    integer i__1[3], i__2;
    real ret_val;
    char ch__1[27], ch__2[6], ch__3[1];
    icilist ici__1;

    /* Builtin functions */
    integer s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), e_rsfe(void),
	     i_indx(char *, char *, ftnlen, ftnlen), s_wsle(cilist *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    integer do_lio(integer *, integer *, char *, ftnlen), e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer s_rsfi(icilist *), e_rsfi(void);

    /* Local variables */
    extern /* Subroutine */ int allparts_(char *, integer *, ftnlen);
    static integer i1, i2, icf;
    static char durq[1];
    extern /* Subroutine */ int chkcom_(char *, ftnlen), getchar_(char *, 
	    integer *, char *, ftnlen, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___179 = { 0, 10, 0, "(a)", 0 };
    static cilist io___183 = { 0, 6, 0, 0, 0 };
    static cilist io___185 = { 0, 6, 0, 0, 0 };



/*  Reads a piece of setup data from line, gets a new line from */
/*  file 10 (jobname.pmx) if needed, Transfers comment lines into all parts. */

/*  iread controls copying of values into scratch files for parts, but only */
/*  if not replacing. */

/*  iread input  value written */
/*   -1   nvi      nothing (only used when noinst<0 initially) */
/*   0   various   value read */
/*   1    nv       -1 , replace later with nvi(i) */
/*   2    noinst    1 */
/*   3    np       -2 , replace later with (nsyst-1)/12+1 */
/*   4  musicsize   20 */
/*   5  fracondent  0.05 */

#line 1308 ""
L4:
#line 1308 ""
    if (*iccount == 128) {
#line 1309 ""
	s_rsfe(&io___179);
#line 1309 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 1309 ""
	e_rsfe();
#line 1310 ""
	if (all_1.replacing) {
#line 1310 ""
	    all_1.replacing = FALSE_;
#line 1310 ""
	}
#line 1311 ""
	chkcom_(line, (ftnlen)128);
#line 1312 ""
	*iccount = 0;
#line 1313 ""
    }
#line 1314 ""
    ++(*iccount);

/*  Find next non-blank or end of line */

#line 1318 ""
    for (*iccount = *iccount; *iccount <= 127; ++(*iccount)) {
#line 1319 ""
	if (*(unsigned char *)&line[*iccount - 1] != ' ') {
#line 1319 ""
	    goto L3;
#line 1319 ""
	}
#line 1320 ""
/* L2: */
#line 1320 ""
    }

/*  If here, need to get a new line */

#line 1324 ""
    *iccount = 128;
#line 1325 ""
    goto L4;
#line 1326 ""
L3:

/*  iccount now points to start of number to read */

#line 1330 ""
    i1 = *iccount;
#line 1331 ""
L5:
#line 1331 ""
    getchar_(line, iccount, durq, (ftnlen)128, (ftnlen)1);

/*  Remember that getchar first increments iccount, *then* reads a character. */

#line 1335 ""
    if (i_indx("0123456789.-", durq, (ftnlen)12, (ftnlen)1) > 0) {
#line 1335 ""
	goto L5;
#line 1335 ""
    }
#line 1336 ""
    i2 = *iccount - 1;
#line 1337 ""
    if (i2 < i1) {
#line 1338 ""
	s_wsle(&io___183);
/* Writing concatenation */
#line 1338 ""
	i__1[0] = 7, a__1[0] = "Found \"";
#line 1338 ""
	i__1[1] = 1, a__1[1] = durq;
#line 1338 ""
	i__1[2] = 19, a__1[2] = "\" instead of number";
#line 1338 ""
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)27);
#line 1338 ""
	do_lio(&c__9, &c__1, ch__1, (ftnlen)27);
#line 1338 ""
	e_wsle();
#line 1339 ""
	s_stop("1", (ftnlen)1);
#line 1340 ""
    }
#line 1341 ""
    icf = i2 - i1 + 49;
#line 1342 ""
    ici__1.icierr = 0;
#line 1342 ""
    ici__1.iciend = 0;
#line 1342 ""
    ici__1.icirnum = 1;
#line 1342 ""
    ici__1.icirlen = i2 - (i1 - 1);
#line 1342 ""
    ici__1.iciunit = line + (i1 - 1);
/* Writing concatenation */
#line 1342 ""
    i__1[0] = 2, a__1[0] = "(f";
#line 1342 ""
    *(unsigned char *)&ch__3[0] = icf;
#line 1342 ""
    i__1[1] = 1, a__1[1] = ch__3;
#line 1342 ""
    i__1[2] = 3, a__1[2] = ".0)";
#line 1342 ""
    ici__1.icifmt = (s_cat(ch__2, a__1, i__1, &c__3, (ftnlen)6), ch__2);
#line 1342 ""
    s_rsfi(&ici__1);
#line 1342 ""
    do_fio(&c__1, (char *)&ret_val, (ftnlen)sizeof(real));
#line 1342 ""
    e_rsfi();
#line 1343 ""
    if (! all_1.replacing) {
#line 1344 ""
	if (*iread == 0) {
#line 1345 ""
	    i__2 = i2 - i1 + 1;
#line 1345 ""
	    allparts_(line + (i1 - 1), &i__2, i2 - (i1 - 1));
#line 1346 ""
	} else if (*iread == 1) {
#line 1347 ""
	    allparts_("-999", &c__4, (ftnlen)4);
#line 1348 ""
	} else if (*iread == 2) {
#line 1349 ""
	    allparts_("1", &c__1, (ftnlen)1);
#line 1350 ""
	} else if (*iread == 3) {
#line 1351 ""
	    allparts_("-998", &c__4, (ftnlen)4);
#line 1352 ""
	} else if (*iread == 4) {
#line 1353 ""
	    allparts_("20", &c__2, (ftnlen)2);
#line 1354 ""
	} else if (*iread == 5) {
#line 1355 ""
	    allparts_(".05", &c__3, (ftnlen)3);
#line 1356 ""
	} else if (*iread != -1) {
#line 1357 ""
	    s_wsle(&io___185);
#line 1357 ""
	    do_lio(&c__9, &c__1, "Error with iread in readin", (ftnlen)26);
#line 1357 ""
	    e_wsle();
#line 1358 ""
	    s_stop("", (ftnlen)0);
#line 1359 ""
	}
#line 1360 ""
    }
#line 1361 ""
    return ret_val;
} /* readin_ */

/* Subroutine */ int chkcom_(char *line, ftnlen line_len)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), e_rsfe(void),
	     i_indx(char *, char *, ftnlen, ftnlen), s_cmp(char *, char *, 
	    ftnlen, ftnlen), s_wsfe(cilist *), e_wsfe(void);

    /* Local variables */
    static logical clefpend;
    extern /* Subroutine */ int allparts_(char *, integer *, ftnlen);
    static integer j, k, ivq, idxb, idxl, idxm, idxn;
    extern /* Subroutine */ int zapbl_(char *, integer *, ftnlen);
    static integer iposc0;
    extern integer lenstr_(char *, integer *, ftnlen);
    static integer lenline;

    /* Fortran I/O blocks */
    static cilist io___186 = { 0, 10, 0, "(a)", 0 };
    static cilist io___195 = { 0, 10, 0, "(a)", 0 };
    static cilist io___198 = { 0, 0, 0, "(a)", 0 };
    static cilist io___199 = { 0, 0, 0, "(a)", 0 };
    static cilist io___200 = { 0, 10, 1, "(a)", 0 };



/*  Assume that line has just been read. No need to change iccount since we only */
/*  process full lines. */

#line 1373 ""
L1:
#line 1373 ""
    if (*(unsigned char *)line != '%') {
#line 1373 ""
	return 0;
#line 1373 ""
    }

/*  If here, line has some sort of comment */

#line 1377 ""
    if (*(unsigned char *)&line[1] == '%') {
#line 1378 ""
	if (! all_1.insetup) {

/*  Suck up a line, then flow out of "if" block to get another and loop */

#line 1382 ""
	    s_rsfe(&io___186);
#line 1382 ""
	    do_fio(&c__1, line, (ftnlen)128);
#line 1382 ""
	    e_rsfe();
/* ++VV */

/*  UNLESS (a) it has a score-only "M" and changes # of inst's. */

#line 1388 ""
	    if (i_indx(line, "M", (ftnlen)128, (ftnlen)1) > 0) {
#line 1389 ""
		idxl = i_indx(line, "L", (ftnlen)128, (ftnlen)1);
#line 1390 ""
		idxm = i_indx(line, "M", (ftnlen)128, (ftnlen)1);
#line 1391 ""
		idxn = i_indx(line, "n", (ftnlen)128, (ftnlen)1);
#line 1392 ""
		idxb = i_indx(line, " ", (ftnlen)128, (ftnlen)1);
#line 1393 ""
		if (idxl < idxm && idxm < idxn && (idxb == 0 || idxn < idxb)) 
			{
#line 1395 ""
		    i__1 = idxn;
#line 1395 ""
		    all_1.noinow = *(unsigned char *)&line[i__1] - 48;
#line 1396 ""
		    clefpend = TRUE_;

/*  Next noinow digits are original inst. #'s of new inst. set.  Next noinow */
/*  char's after that are clefs */

#line 1401 ""
		    all_1.nvnow = 0;
#line 1402 ""
		    i__1 = all_1.noinow;
#line 1402 ""
		    for (j = 1; j <= i__1; ++j) {
#line 1403 ""
			i__2 = idxn + 1 + j - 1;
#line 1403 ""
			all_1.iorig[j - 1] = *(unsigned char *)&line[i__2] - 
				48;
#line 1404 ""
			iposc0 = idxn + 1 + all_1.noinow;
#line 1405 ""
			i__2 = all_1.nvi[all_1.iorig[j - 1] - 1];
#line 1405 ""
			for (k = 1; k <= i__2; ++k) {
#line 1406 ""
			    ++all_1.nvnow;
/*                  clefq(nvnow) = line(iposc0+nvnow:iposc0+nvnow) */
#line 1408 ""
			    all_1.instnum[all_1.nvnow - 1] = j;
#line 1409 ""
			    all_1.botv[all_1.nvnow - 1] = k == 1 && j != 1;
#line 1410 ""
/* L25: */
#line 1410 ""
			}
#line 1411 ""
/* L24: */
#line 1411 ""
		    }
#line 1412 ""
		}
#line 1413 ""
	    }

/*  or if it's "h" or "l", need to suck up one more line */

#line 1417 ""
	    if (*(unsigned char *)line == 'h' && i_indx("+- ", line + 1, (
		    ftnlen)3, (ftnlen)1) > 0 || *(unsigned char *)line == 'T' 
		    || s_cmp(line, "l ", (ftnlen)2, (ftnlen)2) == 0) {
#line 1417 ""
		s_rsfe(&io___195);
#line 1417 ""
		do_fio(&c__1, line, (ftnlen)128);
#line 1417 ""
		e_rsfe();
#line 1417 ""
	    }

/*  4/29/00 check for T string also */

#line 1423 ""
	} else {

/*  In setup mode. Set flag, flow out and do use following line */

#line 1427 ""
	    all_1.replacing = TRUE_;
#line 1428 ""
	}
#line 1429 ""
    } else if (*(unsigned char *)&line[1] == '!') {

/*  Copy to all parts */

#line 1433 ""
	allparts_(line + 2, &c__125, (ftnlen)126);
#line 1434 ""
    } else {

/*  Get value of hex integer 1,2,...,9,a,b,c in 2nd position, zero otherwise */
/* c  Get value of extended hex integer 1,2,...,9,a,b,c,...,o in 2nd position, zero otherwise */

#line 1439 ""
	ivq = i_indx("123456789abcdefghijklmno", line + 1, (ftnlen)24, (
		ftnlen)1);

/*  Only treat as part-specific pmx line if number .le. noinst */

#line 1443 ""
	if (ivq < 1 || ivq > all_1.noinst) {

/*  Simple comment. */

#line 1447 ""
	    allparts_(line, &c__128, (ftnlen)128);
#line 1448 ""
	} else {

/*  Instrument comment, copy only to part */

#line 1452 ""
	    lenline = lenstr_(line, &c__128, (ftnlen)128);
#line 1453 ""
	    if (lenline > 2) {
#line 1454 ""
		io___198.ciunit = ivq + 10;
#line 1454 ""
		s_wsfe(&io___198);
#line 1454 ""
		do_fio(&c__1, line + 2, lenline - 2);
#line 1454 ""
		e_wsfe();
#line 1455 ""
	    } else {

/*  Transferring blank line */

#line 1459 ""
		io___199.ciunit = ivq + 10;
#line 1459 ""
		s_wsfe(&io___199);
#line 1459 ""
		do_fio(&c__1, " ", (ftnlen)1);
#line 1459 ""
		e_wsfe();
#line 1460 ""
	    }
#line 1461 ""
	}
#line 1462 ""
    }
#line 1463 ""
    i__1 = s_rsfe(&io___200);
#line 1463 ""
    if (i__1 != 0) {
#line 1463 ""
	goto L2;
#line 1463 ""
    }
#line 1463 ""
    i__1 = do_fio(&c__1, line, (ftnlen)128);
#line 1463 ""
    if (i__1 != 0) {
#line 1463 ""
	goto L2;
#line 1463 ""
    }
#line 1463 ""
    i__1 = e_rsfe();
#line 1463 ""
    if (i__1 != 0) {
#line 1463 ""
	goto L2;
#line 1463 ""
    }
#line 1464 ""
    zapbl_(line, &c__128, (ftnlen)128);
#line 1465 ""
    goto L1;
#line 1466 ""
L2:
#line 1467 ""
    return 0;
} /* chkcom_ */

/* Subroutine */ int dosetup_(integer *iv, char *line, integer *mtrnum, 
	integer *mtrden, ftnlen line_len)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void),
	     i_nint(real *);

    /* Local variables */
    static integer ioi, noi, iset;
    static real xdata;
    static integer iccount;
    extern /* Subroutine */ int partnum_(integer *, integer *, char *, real *,
	     ftnlen);

    /* Fortran I/O blocks */
    static cilist io___204 = { 0, 40, 0, "(i5)", 0 };
    static cilist io___206 = { 0, 40, 0, "(i5)", 0 };
    static cilist io___208 = { 0, 40, 0, "(i5)", 0 };
    static cilist io___209 = { 0, 40, 0, "(i5)", 0 };
    static cilist io___210 = { 0, 40, 0, "(i5)", 0 };
    static cilist io___211 = { 0, 40, 0, "(i5)", 0 };
    static cilist io___212 = { 0, 40, 0, "(f5.2)", 0 };



/*  Transfers setup data from scratch file to real one for one instrument */
/*  Data may be mixed with comments, but on entry 1st item is a number. */
/*  Write a comment when encountered, as it comes. */
/*  Write numbers one per line. */
/*  Three input data require special handling: */
/*    If not already replaced, i.e., if negative, then */
/*      iset(1) (nv) will be replaced with nvi(i) */
/*      iset(9) (npages) will be replaced with (nsyst-1)/12+1 */
/*    iset(2), if negative, will be followed by extra numbers to be transf. */

#line 1486 ""
    iccount = 0;
#line 1487 ""
    for (iset = 1; iset <= 12; ++iset) {
#line 1488 ""
	partnum_(iv, &iccount, line, &xdata, (ftnlen)128);
#line 1489 ""
	if (iset == 2) {
#line 1490 ""
	    if (xdata > 0.f) {
#line 1491 ""
		s_wsfe(&io___204);
#line 1491 ""
		i__1 = (integer) (xdata + .1f);
#line 1491 ""
		do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
#line 1491 ""
		e_wsfe();
#line 1492 ""
	    } else {
#line 1493 ""
		noi = -xdata + .1f;
#line 1494 ""
		s_wsfe(&io___206);
#line 1494 ""
		do_fio(&c__1, (char *)&noi, (ftnlen)sizeof(integer));
#line 1494 ""
		e_wsfe();
#line 1495 ""
		i__1 = noi;
#line 1495 ""
		for (ioi = 1; ioi <= i__1; ++ioi) {
#line 1496 ""
		    partnum_(iv, &iccount, line, &xdata, (ftnlen)128);
#line 1497 ""
		    s_wsfe(&io___208);
#line 1497 ""
		    i__2 = (integer) (xdata + .1f);
#line 1497 ""
		    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
#line 1497 ""
		    e_wsfe();
#line 1498 ""
/* L2: */
#line 1498 ""
		}
#line 1499 ""
	    }
/*        else if (iset.ne.8 .and. xdata.lt.0) then */
#line 1501 ""
	} else if (iset != 8 && iset != 5 && xdata < 0.f) {

/*  Must be either nv or npages */

#line 1505 ""
	    if ((integer) (-xdata + .1f) == 999) {

/*  It's nv */

#line 1509 ""
		s_wsfe(&io___209);
#line 1509 ""
		do_fio(&c__1, (char *)&all_1.nvi[*iv - 1], (ftnlen)sizeof(
			integer));
#line 1509 ""
		e_wsfe();
#line 1510 ""
	    } else {

/*  npages must be computed */

#line 1514 ""
		s_wsfe(&io___210);
#line 1514 ""
		i__1 = (all_1.nsyst - 1) / 12 + 1;
#line 1514 ""
		do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
#line 1514 ""
		e_wsfe();
#line 1515 ""
	    }
#line 1516 ""
	} else if (iset != 7 && iset != 12) {

/*  write integer */

#line 1520 ""
	    s_wsfe(&io___211);
#line 1520 ""
	    i__1 = i_nint(&xdata);
#line 1520 ""
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
#line 1520 ""
	    e_wsfe();
#line 1521 ""
	} else {

/*  write floating number */

#line 1525 ""
	    s_wsfe(&io___212);
#line 1525 ""
	    do_fio(&c__1, (char *)&xdata, (ftnlen)sizeof(real));
#line 1525 ""
	    e_wsfe();
#line 1526 ""
	}
#line 1527 ""
	if (iset == 3) {
#line 1528 ""
	    *mtrnum = i_nint(&xdata);
#line 1529 ""
	} else if (iset == 4) {
#line 1530 ""
	    *mtrden = i_nint(&xdata);
#line 1531 ""
	}
#line 1532 ""
/* L1: */
#line 1532 ""
    }
#line 1533 ""
    return 0;
} /* dosetup_ */

/* Subroutine */ int partnum_(integer *iv, integer *iccount, char *line, real 
	*xdata, ftnlen line_len)
{
    /* System generated locals */
    address a__1[3];
    integer i__1[3];
    char ch__1[27], ch__2[6], ch__3[1];
    icilist ici__1;

    /* Builtin functions */
    integer s_rsfe(cilist *), do_fio(integer *, char *, ftnlen), e_rsfe(void),
	     s_wsfe(cilist *), e_wsfe(void), i_indx(char *, char *, ftnlen, 
	    ftnlen), s_wsle(cilist *);
    /* Subroutine */ int s_cat(char *, char **, integer *, integer *, ftnlen);
    integer do_lio(integer *, integer *, char *, ftnlen), e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer s_rsfi(icilist *), e_rsfi(void);

    /* Local variables */
    static integer i1, i2, icf, len;
    static char durq[1];
    extern integer lenstr_(char *, integer *, ftnlen);
    extern /* Subroutine */ int getchar_(char *, integer *, char *, ftnlen, 
	    ftnlen);

    /* Fortran I/O blocks */
    static cilist io___213 = { 0, 0, 0, "(a)", 0 };
    static cilist io___215 = { 0, 40, 0, "(a)", 0 };
    static cilist io___219 = { 0, 6, 0, 0, 0 };



/*  Simplified number parsing.  Only looks for comment lines and numbers. */

#line 1541 ""
L2:
#line 1541 ""
    if (*iccount == 128) {
#line 1542 ""
	io___213.ciunit = *iv + 10;
#line 1542 ""
	s_rsfe(&io___213);
#line 1542 ""
	do_fio(&c__1, line, (ftnlen)128);
#line 1542 ""
	e_rsfe();
#line 1543 ""
	if (*(unsigned char *)line == '%') {
#line 1544 ""
	    len = lenstr_(line, &c__128, (ftnlen)128);
#line 1545 ""
	    s_wsfe(&io___215);
#line 1545 ""
	    do_fio(&c__1, line, len);
#line 1545 ""
	    e_wsfe();
#line 1546 ""
	    goto L2;
#line 1547 ""
	}
#line 1548 ""
	*iccount = 0;
#line 1549 ""
    }
#line 1550 ""
    ++(*iccount);

/*  Find next non-blank or end of line */

#line 1554 ""
    for (*iccount = *iccount; *iccount <= 127; ++(*iccount)) {
#line 1555 ""
	if (*(unsigned char *)&line[*iccount - 1] != ' ') {
#line 1555 ""
	    goto L3;
#line 1555 ""
	}
#line 1556 ""
/* L4: */
#line 1556 ""
    }

/*  If here, iccount=128 and need to get a new line */

#line 1560 ""
    goto L2;
#line 1561 ""
L3:

/*  iccount now points to start of number to read */

#line 1565 ""
    i1 = *iccount;
#line 1566 ""
L5:
#line 1566 ""
    getchar_(line, iccount, durq, (ftnlen)128, (ftnlen)1);

/*  Remember that getchar first increments iccount, *then* reads a character. */

#line 1570 ""
    if (i_indx("0123456789.-", durq, (ftnlen)12, (ftnlen)1) > 0) {
#line 1570 ""
	goto L5;
#line 1570 ""
    }
#line 1571 ""
    i2 = *iccount - 1;
#line 1572 ""
    if (i2 < i1) {
#line 1573 ""
	s_wsle(&io___219);
/* Writing concatenation */
#line 1573 ""
	i__1[0] = 7, a__1[0] = "Found \"";
#line 1573 ""
	i__1[1] = 1, a__1[1] = durq;
#line 1573 ""
	i__1[2] = 19, a__1[2] = "\" instead of number";
#line 1573 ""
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)27);
#line 1573 ""
	do_lio(&c__9, &c__1, ch__1, (ftnlen)27);
#line 1573 ""
	e_wsle();
#line 1574 ""
	s_stop("1", (ftnlen)1);
#line 1575 ""
    }
#line 1576 ""
    icf = i2 - i1 + 49;
#line 1577 ""
    ici__1.icierr = 0;
#line 1577 ""
    ici__1.iciend = 0;
#line 1577 ""
    ici__1.icirnum = 1;
#line 1577 ""
    ici__1.icirlen = i2 - (i1 - 1);
#line 1577 ""
    ici__1.iciunit = line + (i1 - 1);
/* Writing concatenation */
#line 1577 ""
    i__1[0] = 2, a__1[0] = "(f";
#line 1577 ""
    *(unsigned char *)&ch__3[0] = icf;
#line 1577 ""
    i__1[1] = 1, a__1[1] = ch__3;
#line 1577 ""
    i__1[2] = 3, a__1[2] = ".0)";
#line 1577 ""
    ici__1.icifmt = (s_cat(ch__2, a__1, i__1, &c__3, (ftnlen)6), ch__2);
#line 1577 ""
    s_rsfi(&ici__1);
#line 1577 ""
    do_fio(&c__1, (char *)&(*xdata), (ftnlen)sizeof(real));
#line 1577 ""
    e_rsfi();
#line 1578 ""
    return 0;
} /* partnum_ */


/* Subroutine */ int zapbl_(char *string, integer *len, ftnlen string_len)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    /* Subroutine */ int s_copy(char *, char *, ftnlen, ftnlen);

    /* Local variables */
    static integer i__;

#line 1590 ""
    i__1 = *len;
#line 1590 ""
    for (i__ = 1; i__ <= i__1; ++i__) {
#line 1591 ""
	if (*(unsigned char *)&string[i__ - 1] == ' ') {
#line 1591 ""
	    goto L1;
#line 1591 ""
	}
#line 1592 ""
	if (i__ == 1) {
#line 1592 ""
	    return 0;
#line 1592 ""
	}
#line 1593 ""
	goto L2;
#line 1594 ""
L1:
#line 1594 ""
	;
#line 1594 ""
    }

/*  If line is all blank, leave it alone */

#line 1598 ""
    return 0;
#line 1599 ""
L2:
#line 1600 ""
    s_copy(string, string + (i__ - 1), string_len, *len - (i__ - 1));
#line 1601 ""
    return 0;
} /* zapbl_ */

/* Subroutine */ int oddquotesbefore_(char *lineq, integer *indx, logical *
	yesodd, ftnlen lineq_len)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, numdqs;


/*  This counts number of double quotes in lineq up to position indx-1, then */
/*    sets yesodd according to whether number is odd or even */

#line 1610 ""
    numdqs = 0;
#line 1611 ""
    i__1 = *indx - 1;
#line 1611 ""
    for (i__ = 1; i__ <= i__1; ++i__) {
#line 1612 ""
	if (*(unsigned char *)&lineq[i__ - 1] == '"') {
#line 1612 ""
	    ++numdqs;
#line 1612 ""
	}
#line 1613 ""
/* L1: */
#line 1613 ""
    }
#line 1614 ""
    *yesodd = numdqs % 2 == 1;
#line 1615 ""
    return 0;
} /* oddquotesbefore_ */

