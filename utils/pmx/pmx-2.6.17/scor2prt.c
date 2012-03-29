/* scor2prt.f -- translated by f2c (version 20031025).
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

/* Common Block Declarations */

struct all_1_ {
    integer noinow, iorig[24], noinst;
    logical insetup, replacing;
    integer instnum[24];
    logical botv[24];
    integer nvi[24], nsyst, nvnow;
};
struct all_2_ {
    integer noinow, iorig[24], noinst;
    logical insetup, replacing;
    integer instnum[24];
    logical botv[24];
    integer nvi[24], nsyst, nvnow;
};

#define all_1 (*(struct all_1_ *) &all_)
#define all_2 (*(struct all_2_ *) &all_)

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


    clefpend = FALSE_;
    all_1.insetup = TRUE_;
    all_1.replacing = FALSE_;
    frstln = TRUE_;
    lenhold = 0;
    *(unsigned char *)sq = '\\';
    s_wsle(&io___6);
    do_lio(&c__9, &c__1, "This is scor2prt for PMX 2.615, 24 July 2011", (
	    ftnlen)44);
    e_wsle();
    numargs = iargc_();
    if (numargs == 0) {
	s_wsle(&io___8);
	do_lio(&c__9, &c__1, "You could have entered a jobname on the comman"
		"d line,", (ftnlen)53);
	e_wsle();
	s_wsle(&io___9);
	do_lio(&c__9, &c__1, "      but you may enter one now:", (ftnlen)32);
	e_wsle();
	s_rsfe(&io___10);
	do_fio(&c__1, jobname, (ftnlen)27);
	e_rsfe();
	numargs = 1;
    } else {
/*        call getarg(1,jobname,idum) ! May need to replace this w/ next line */
	getarg_(&c__1, jobname, (ftnlen)27);
    }
    ljob = lenstr_(jobname, &c__27, (ftnlen)27);
    if (ljob == 0) {
	s_wsle(&io___13);
	do_lio(&c__9, &c__1, "No was jobname entered. Restart and try again.",
		 (ftnlen)46);
	e_wsle();
	s_stop("", (ftnlen)0);
    }

/*  Strip ".pmx" if necessary */

/* Computing MAX */
    i__1 = i_indx(jobname, ".pmx", (ftnlen)27, (ftnlen)4), i__2 = i_indx(
	    jobname, ".PMX", (ftnlen)27, (ftnlen)4);
    ndxpmx = max(i__1,i__2);
    if (ndxpmx > 0) {
	s_copy(jobname, jobname, (ftnlen)27, ndxpmx - 1);
	ljob += -4;
    }

/*  Check for existence of input file */

/* Writing concatenation */
    i__3[0] = ljob, a__1[0] = jobname;
    i__3[1] = 4, a__1[1] = ".pmx";
    s_cat(infileq, a__1, i__3, &c__2, (ftnlen)27);
    ioin__1.inerr = 0;
    ioin__1.infilen = 27;
    ioin__1.infile = infileq;
    ioin__1.inex = &fexist;
    ioin__1.inopen = 0;
    ioin__1.innum = 0;
    ioin__1.innamed = 0;
    ioin__1.inname = 0;
    ioin__1.inacc = 0;
    ioin__1.inseq = 0;
    ioin__1.indir = 0;
    ioin__1.infmt = 0;
    ioin__1.inform = 0;
    ioin__1.inunf = 0;
    ioin__1.inrecl = 0;
    ioin__1.innrec = 0;
    ioin__1.inblank = 0;
    f_inqu(&ioin__1);
    if (! fexist) {
	s_wsle(&io___17);
/* Writing concatenation */
	i__3[0] = 17, a__1[0] = "Cannot find file ";
	i__3[1] = 27, a__1[1] = infileq;
	s_cat(ch__1, a__1, i__3, &c__2, (ftnlen)44);
	do_lio(&c__9, &c__1, ch__1, (ftnlen)44);
	e_wsle();
	s_stop("", (ftnlen)0);
    }
    o__1.oerr = 0;
    o__1.ounit = 10;
    o__1.ofnmlen = ljob + 4;
/* Writing concatenation */
    i__3[0] = ljob, a__1[0] = jobname;
    i__3[1] = 4, a__1[1] = ".pmx";
    s_cat(ch__2, a__1, i__3, &c__2, (ftnlen)31);
    o__1.ofnm = ch__2;
    o__1.orl = 0;
    o__1.osta = 0;
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);

/* Open all instrument files now for allparts stuff.  Later disgard those >nv */

    for (iv = 1; iv <= 24; ++iv) {
	all_1.iorig[iv - 1] = iv;
	o__1.oerr = 0;
	o__1.ounit = iv + 10;
	o__1.ofnm = 0;
	o__1.orl = 0;
	o__1.osta = "SCRATCH";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
	ludpfn[iv - 1] = 0;
/* L19: */
    }
    s_rsfe(&io___20);
    do_fio(&c__1, line, (ftnlen)128);
    e_rsfe();
    chkcom_(line, (ftnlen)128);
    if (s_cmp(line, "---", (ftnlen)3, (ftnlen)3) == 0) {
	allparts_(line, &c__128, (ftnlen)128);
L31:
	s_rsfe(&io___22);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	if (s_cmp(line, "---", (ftnlen)3, (ftnlen)3) != 0) {
	    allparts_(line, &c__128, (ftnlen)128);
	    goto L31;
	}
	allparts_(line, &c__128, (ftnlen)128);
	s_rsfe(&io___23);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	chkcom_(line, (ftnlen)128);
    }
    iccount = 0;
    nv = readin_(line, &iccount, &c__1, (ftnlen)128) + .1f;
    all_1.noinst = readin_(line, &iccount, &c__2, (ftnlen)128) + .1f;
    if (all_1.noinst > 0) {
	all_1.nvi[0] = nv - all_1.noinst + 1;
    } else {
	all_1.noinst = 1 - all_1.noinst;
	i__1 = all_1.noinst;
	for (iinst = 1; iinst <= i__1; ++iinst) {
	    all_1.nvi[iinst - 1] = readin_(line, &iccount, &c_n1, (ftnlen)128)
		     + .1f;
/* L21: */
	}
    }
    all_1.noinow = all_1.noinst;
    insnow = 1;

/*  ivlast is last iv in current inst.  instnum(iv) is iinst for current voice. */

    ivlast = all_1.nvi[0];
    i__1 = nv;
    for (iv = 1; iv <= i__1; ++iv) {
	all_1.instnum[iv - 1] = insnow;
	if (iv == ivlast) {
	    if (iv < nv) {
		all_1.botv[iv] = TRUE_;
	    }

/*  The previous stmt will set botv true only for bot voice of iinst>1.  It is */
/*  used when writing termrpts, but the one in voice one is handled differently, */
/*  so botv(1) is left .false. */

	    if (insnow < all_1.noinst) {
		++insnow;
		ivlast += all_1.nvi[insnow - 1];
	    }
	}
/* L22: */
    }
    mtrnuml = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
    mtrdenl = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
    mtrnmp = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
    mtrdnp = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
    xmtrnum0 = readin_(line, &iccount, &c__0, (ftnlen)128);
    isig = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
    npages = readin_(line, &iccount, &c__3, (ftnlen)128) + .1f;
    all_1.nsyst = readin_(line, &iccount, &c__0, (ftnlen)128) + .1f;
    musicsize = readin_(line, &iccount, &c__4, (ftnlen)128) + .1f;
    fracindent = readin_(line, &iccount, &c__5, (ftnlen)128);
    if (npages == 0) {
	s_wsle(&io___38);
	do_lio(&c__9, &c__1, "You entered npages=0, which means nsyst is not"
		" the total number", (ftnlen)63);
	e_wsle();
	s_wsle(&io___39);
	do_lio(&c__9, &c__1, "of systems.  Scor2prt has to know the total nu"
		"mber of systems.", (ftnlen)62);
	e_wsle();
	s_wsle(&io___40);
	do_lio(&c__9, &c__1, "Please set npages and nsyst to their real valu"
		"es.", (ftnlen)49);
	e_wsle();
	s_stop("", (ftnlen)0);
    }

/*  Must leave insetup=.true. else could bypass ALL instrument names. */

    s_rsfe(&io___41);
    do_fio(&c__1, line, (ftnlen)128);
    e_rsfe();
    chkcom_(line, (ftnlen)128);
    al__1.aerr = 0;
    al__1.aunit = 10;
    f_back(&al__1);

/*  Normally this puts pointer at start of line with 1st inst name */
/*  Check if prior line was "%%" */

    al__1.aerr = 0;
    al__1.aunit = 10;
    f_back(&al__1);
    s_rsfe(&io___42);
    do_fio(&c__1, line, (ftnlen)128);
    e_rsfe();
    if (s_cmp(line, "%%", (ftnlen)2, (ftnlen)2) == 0) {
	al__1.aerr = 0;
	al__1.aunit = 10;
	f_back(&al__1);
    }
    i__1 = all_1.noinst;
    for (iv = 1; iv <= i__1; ++iv) {
	gotname = FALSE_;
L16:
	s_rsfe(&io___44);
	do_fio(&c__1, instrum + (iv - 1 << 7), (ftnlen)128);
	e_rsfe();
	if (s_cmp(instrum + (iv - 1 << 7), "%%", (ftnlen)2, (ftnlen)2) == 0) {
	    s_rsfe(&io___46);
	    do_fio(&c__1, line, (ftnlen)128);
	    e_rsfe();
	    goto L16;
	} else if (*(unsigned char *)&instrum[(iv - 1) * 128] == '%') {
	    ivq = *(unsigned char *)&instrum[(iv - 1 << 7) + 1] - 48;
	    if (ivq != iv) {

/*  It's really a comment.  Copy to parts, then get another trial name. */

		allparts_(instrum + (iv - 1 << 7), &c__128, (ftnlen)128);
		goto L16;
	    } else {
		s_copy(line, instrum + ((iv - 1 << 7) + 2), (ftnlen)128, (
			ftnlen)126);
		s_copy(instrum + (iv - 1 << 7), line, (ftnlen)128, (ftnlen)
			128);
		gotname = TRUE_;
	    }
	} else {
	    gotname = TRUE_;
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
	if (! gotname) {
	    s_wsle(&io___48);
	    do_lio(&c__9, &c__1, "You must provide a replacement instrument "
		    "name", (ftnlen)46);
	    e_wsle();
	    s_stop("", (ftnlen)0);
	}
	io___49.ciunit = iv + 10;
	s_wsfe(&io___49);
	do_fio(&c__1, " ", (ftnlen)1);
	e_wsfe();
/* L14: */
    }
    all_1.replacing = FALSE_;
    all_1.nvnow = nv;

/*  Clef string:  Note insetup is still T, so "%%" will be treated specially */

    s_rsfe(&io___50);
    do_fio(&c__1, line, (ftnlen)128);
    e_rsfe();
    chkcom_(line, (ftnlen)128);
    if (all_1.replacing) {

/*  If here, we have next line after "%%", containing score's clef string */
/*  Assume all clefs are handled with instrument comments. */

	s_rsfe(&io___51);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	chkcom_(line, (ftnlen)128);
	al__1.aerr = 0;
	al__1.aunit = 10;
	f_back(&al__1);
    } else {

/*  If here, line has the clef string in it.  Handle the old way */

	kvstart = 1;
	kvend = all_1.nvi[0];
	i__1 = all_1.noinst;
	for (inst = 1; inst <= i__1; ++inst) {
	    ci__1.cierr = 0;
	    ci__1.ciunit = inst + 10;
/* Writing concatenation */
	    i__4[0] = 2, a__2[0] = "(a";
	    *(unsigned char *)&ch__4[0] = all_1.nvi[inst - 1] + 48;
	    i__4[1] = 1, a__2[1] = ch__4;
	    i__4[2] = 1, a__2[2] = ")";
	    ci__1.cifmt = (s_cat(ch__3, a__2, i__4, &c__3, (ftnlen)4), ch__3);
	    s_wsfe(&ci__1);
	    do_fio(&c__1, line + (kvstart - 1), kvend - (kvstart - 1));
	    e_wsfe();
	    if (inst < all_1.noinst) {
		kvstart = kvend + 1;
		kvend = kvstart + all_1.nvi[inst] - 1;
	    }
/* L2: */
	}
    }
    all_1.replacing = FALSE_;
    all_1.insetup = FALSE_;

/*  *****NOTE*****This comment applies to stuff done earlier! */
/*  Before starting the big loop, copy initial instnum and staffnum stuff */
/*  into working values.  Latter may change if noinst changes.  Also make */
/*  list of current inst nums relative to original ones.  In addition to those */
/*  below, must redo instnum(iv) and botv(iv) when we change noinst. */

/*  Path string:  ASSUME THIS WILL NEVER BE ALTERED IN PARTS! */

L18:
    s_rsfe(&io___55);
    do_fio(&c__1, line, (ftnlen)128);
    e_rsfe();
    if (*(unsigned char *)line == '%') {
	allparts_(line, &c__128, (ftnlen)128);
	goto L18;
    }
    allparts_(line, &c__128, (ftnlen)128);

/*  Write instrument names.  Will be blank if later part of a score. */

    if (*(unsigned char *)&instrum[0] != ' ') {
	i__1 = all_1.noinst;
	for (iv = 1; iv <= i__1; ++iv) {
	    len = lenstr_(instrum + (iv - 1 << 7), &c__79, (ftnlen)128);
	    io___57.ciunit = iv + 10;
	    s_wsfe(&io___57);
	    do_fio(&c__1, "Ti", (ftnlen)2);
	    do_fio(&c__1, instrum + (iv - 1 << 7), len);
	    e_wsfe();
/* L3: */
	}
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

    iv = 1;
    iinst = 1;
    termrpt = FALSE_;
L4:
    i__1 = s_rsfe(&io___59);
    if (i__1 != 0) {
	goto L999;
    }
    i__1 = do_fio(&c__1, line, (ftnlen)128);
    if (i__1 != 0) {
	goto L999;
    }
    i__1 = e_rsfe();
    if (i__1 != 0) {
	goto L999;
    }
    lenline = lenstr_(line, &c__128, (ftnlen)128);
    if (lenline == 0) {
	goto L4;
    }
    zapbl_(line, &c__128, (ftnlen)128);
    chkcom_(line, (ftnlen)128);
    lenline = lenstr_(line, &c__128, (ftnlen)128);
    if (lenline == 0) {
	goto L4;
    }
    if (*(unsigned char *)line == 'T') {
	allparts_(line, &c__128, (ftnlen)128);
	s_rsfe(&io___61);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	allparts_(line, &c__128, (ftnlen)128);
	goto L4;
    } else /* if(complicated condition) */ {
/* Writing concatenation */
	i__3[0] = 1, a__1[0] = sq;
	i__3[1] = 1, a__1[1] = sq;
	s_cat(ch__5, a__1, i__3, &c__2, (ftnlen)2);
	if (s_cmp(line, ch__5, (ftnlen)2, (ftnlen)2) == 0) {
	    allparts_(line, &c__128, (ftnlen)128);
	    goto L4;
	} else if (i_indx("hl", line, (ftnlen)2, (ftnlen)1) > 0 && i_indx(
		" +-", line + 1, (ftnlen)3, (ftnlen)1) > 0) {
	    allparts_(line, &c__128, (ftnlen)128);
	    s_rsfe(&io___62);
	    do_fio(&c__1, line, (ftnlen)128);
	    e_rsfe();
	    allparts_(line, &c__128, (ftnlen)128);
	    goto L4;
	} else if (iv == 1) {
	    for (ia = 1; ia <= 10; ++ia) {
L24:
		idxa = ntindex_(line, achar + (ia - 1), (ftnlen)128, (ftnlen)
			1);
		isachar = idxa > 0;
		if (idxa > 1) {
		    i__1 = idxa - 2;
		    isachar = s_cmp(line + i__1, " ", idxa - 1 - i__1, (
			    ftnlen)1) == 0;
		}

/*                   1   2   3   4   5   6   7   8   9   10 */
/*      data achar /'P','m','V','R','A','h','w','K','M','I'/ */

		if (ia == 9) {
		    i__1 = idxa;
		    isachar = isachar && s_cmp(line + i__1, "S", idxa + 1 - 
			    i__1, (ftnlen)1) == 0;
		}
		if (isachar) {

/*  Check whether character is inside a quoted string by counting */
/*  how many quotes precede it in the line */

		    oddquotesbefore_(line, &idxa, &yesodd, (ftnlen)128);
		    if (yesodd) {
			isachar = FALSE_;
		    }
		}
		if (isachar) {

/*  Find next blank */

		    for (ib = idxa + 1; ib <= 128; ++ib) {
			if (*(unsigned char *)&line[ib - 1] == ' ') {
			    goto L7;
			}
/* L6: */
		    }
		    s_wsle(&io___68);
		    do_lio(&c__9, &c__1, "Problem with \"V,R,m,P,A,h,MS, o"
			    "r w\"", (ftnlen)35);
		    e_wsle();
		    s_wsle(&io___69);
		    do_lio(&c__9, &c__1, "Send files to Dr. Don at dsimons a"
			    "t roadrunner dot com", (ftnlen)54);
		    e_wsle();
		    s_stop("1", (ftnlen)1);
L7:

/*  Next blank is at position ib.  Later, if ia=1, must check for Pc"  " ; */
/*    i.e., look for '"' between P and blank */

		    if (ia == 4) {

/*  Check for terminal repeat.  Note if there's a term rpt, there can't be any */
/*  others.  Also, must process repeats LAST, after m's and 'V's */

			for (ic = ib + 1; ic <= 128; ++ic) {

/*  If any subsequent character on this line is neither blank nor "/", get out */

			    if (i_indx(" /", line + (ic - 1), (ftnlen)2, (
				    ftnlen)1) == 0) {
				goto L9;
			    }
			    if (*(unsigned char *)&line[ic - 1] == '/') {
				termrpt = TRUE_;
				i__1 = ib - 3;
				s_copy(termsym, line + i__1, (ftnlen)2, ib - 
					1 - i__1);

/*  Process the line as if there were no "R" */

				goto L10;
			    }
/* L8: */
			}

/* +++ 060812 */
/*  If here, all chars after "R" symbol are blanks, so process the line normally, */
/*    but only IF next line is not the M-Tx line " /" */

			s_rsfe(&io___72);
			do_fio(&c__1, templine, (ftnlen)128);
			e_rsfe();
			if (s_cmp(templine, " /", (ftnlen)2, (ftnlen)2) != 0) 
				{
			    al__1.aerr = 0;
			    al__1.aunit = 10;
			    f_back(&al__1);
/* and flow out */
			} else {

/*  We have the M-Tx case where one line ends with R? and next is " /". Add / to the line, */
/*    and proceed as above */

/* Writing concatenation */
			    i__3[0] = ib, a__1[0] = line;
			    i__3[1] = 1, a__1[1] = "/";
			    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
			    lenline += 2;
			    termrpt = TRUE_;
			    i__1 = ib - 3;
			    s_copy(termsym, line + i__1, (ftnlen)2, ib - 1 - 
				    i__1);
			    goto L10;
			}
/* +++ 060812 */

		    } else if (ia == 1) {
			idxq = ntindex_(line, "\"", (ftnlen)128, (ftnlen)1);
			if (idxq > idxa && idxq < ib) {

/*  Quote is between P and next blank.  Find 2nd quote, starting at the blank. */

			    idxq2 = ib - 1 + ntindex_(line, "\"", (ftnlen)128,
				     (ftnlen)1);
			    i__1 = idxq2;
			    if (idxq == 0 || s_cmp(line + i__1, " ", idxq2 + 
				    1 - i__1, (ftnlen)1) != 0) {
				s_wsle(&io___76);
				e_wsle();
				s_wsle(&io___77);
				do_lio(&c__9, &c__1, "Error copying P with q"
					"uotes, idxq2:", (ftnlen)35);
				do_lio(&c__3, &c__1, (char *)&idxq2, (ftnlen)
					sizeof(integer));
				e_wsle();
				s_wsle(&io___78);
				do_lio(&c__9, &c__1, line, (ftnlen)60);
				e_wsle();
				s_stop("1", (ftnlen)1);
			    }
			    ib = idxq2 + 1;
			}

/*  Do not transfer P into parts. */

			goto L12;
		    } else if (ia == 9) {

/*  Start Saving a macro. After leaving here, a symbol will be sent to all parts, */
/*  If all on this line, set ib to end and exit normally. */

			i__1 = ib;
			ndxm = i_indx(line + i__1, "M", 128 - i__1, (ftnlen)1)
				;
			i__1 = ib + ndxm - 2;
			if (ndxm > 0 && s_cmp(line + i__1, " ", ib + ndxm - 1 
				- i__1, (ftnlen)1) == 0) {

/*  Macro ends on this line */

			    ib = ib + ndxm + 1;
			} else {

/*  Save leading part of current line */

			    lenhold = idxa - 1;
			    if (lenhold > 0) {
				s_copy(holdln, line, (ftnlen)128, lenhold);
			    }

/*  Transfer rest of line */

			    i__1 = 129 - idxa;
			    allparts_(line + (idxa - 1), &i__1, 128 - (idxa - 
				    1));

/*  Read next line */

L20:
			    s_rsfe(&io___81);
			    do_fio(&c__1, line, (ftnlen)128);
			    e_rsfe();

/*  Check for comment, transfer and loop if so */

/*                if (line(1:1) .eq.'%') then */
L23:
			    if (*(unsigned char *)line == '%') {
/*                  call allparts(line,128) */
/*                  go to 20 */
				chkcom_(line, (ftnlen)128);
				goto L23;
			    }

/*  Look for terminal ' M' */

			    if (*(unsigned char *)line == 'M') {
				ndxm = 1;
			    } else {
				ndxm = i_indx(line, " M", (ftnlen)128, (
					ftnlen)2);
				if (ndxm > 0) {
				    ++ndxm;
				}
			    }
			    if (ndxm > 0) {

/*  Set parameters, exit normally (but later check for leading part of 1st line */

				idxa = 1;
				ib = ndxm + 1;
			    } else {

/*  No "M", transfer entire line, loop */

				allparts_(line, &c__128, (ftnlen)128);
				goto L20;
			    }
			}
		    } else if (ia == 10) {

/*  Do not transfer MIDI command into parts */

			goto L12;
		    } else if (ia == 5) {

/*  First check for "AS", but S may come after other "A" options */

			i__1 = idxa;
			idxs = i_indx(line + i__1, "S", ib - i__1, (ftnlen)1);
			if (idxs > 0) {

/*  Get rid of the string. First check if that's all there is in A. */

			    if (ib - idxa == nv + 2) {
				goto L12;
			    }
			    i__1 = idxa + idxs + nv;
/* Writing concatenation */
			    i__3[0] = idxa + idxs - 1, a__1[0] = line;
			    i__3[1] = ib - i__1, a__1[1] = line + i__1;
			    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
			}

/*  Check for user-defined part file name. Must start in column 1 and have */
/*    AN[n]"userfilename". */
/*    idxa is position of "A" */
/*    ib is position of the next blank after "A" */
/*    Don't allow any blanks in user */

			i__1 = idxa;
			if (s_cmp(line + i__1, "N", idxa + 1 - i__1, (ftnlen)
				1) != 0) {
			    goto L9;
			}
/* bail out */
			if (idxa != 1) {
			    s_wsle(&io___83);
			    e_wsle();
			    s_wsle(&io___84);
			    do_lio(&c__9, &c__1, "You entered \"AN...\" some"
				    "where beyond first column; stopping.", (
				    ftnlen)60);
			    e_wsle();
			    s_stop("1", (ftnlen)1);
			}

/*  pmxa already checked for valid one- or 2-digit number, so get it */

			if (*(unsigned char *)&line[3] == '"') {

/*  Single digit instrument number */

			    s_rsfi(&io___85);
			    do_fio(&c__1, (char *)&iudpfn, (ftnlen)sizeof(
				    integer));
			    e_rsfi();
			    idxstartname = 5;
			} else {
			    s_rsfi(&io___88);
			    do_fio(&c__1, (char *)&iudpfn, (ftnlen)sizeof(
				    integer));
			    e_rsfi();
			    idxstartname = 6;
			}
			ludpfn[iudpfn - 1] = i_indx(line + (idxstartname - 1),
				 "\"", 128 - (idxstartname - 1), (ftnlen)1) - 
				1;
			if (ludpfn[iudpfn - 1] < 0) {
			    s_wsle(&io___89);
			    e_wsle();
			    s_wsle(&io___90);
			    do_lio(&c__9, &c__1, "User-defined part file nam"
				    "e must be in quotes", (ftnlen)45);
			    e_wsle();
			    s_stop("1", (ftnlen)1);
			}
			idxendname = idxstartname + ludpfn[iudpfn - 1] - 1;
			s_copy(udpfnq + (iudpfn - 1 << 7), line + (
				idxstartname - 1), (ftnlen)128, idxendname - (
				idxstartname - 1));

/*  Get a new line! */

			goto L4;
		    } else if (ia == 8) {

/* Key change/transposition. */
/* If not instrument specific, copy to all parts */

			i__1 = idxa;
			if (s_cmp(line + i__1, "i", idxa + 1 - i__1, (ftnlen)
				1) != 0) {
			    goto L9;
			}

/* Instrument-wise key/transposition(s): Ki[nn][+/-][dd}[+/-][dd]... */

			ibb = idxa + 2;
/* Starts on digit after 'i' */
L40:
			ici__1.icierr = 0;
			ici__1.iciend = 0;
			ici__1.icirnum = 1;
			ici__1.icirlen = 1;
			ici__1.iciunit = line + (ibb - 1);
			ici__1.icifmt = "(i1)";
			s_rsfi(&ici__1);
			do_fio(&c__1, (char *)&iiinst, (ftnlen)sizeof(integer)
				);
			e_rsfi();
/* 1st digit of iinst */
			i__1 = ibb;
			itemp = i_indx("01234567890", line + i__1, (ftnlen)11,
				 ibb + 1 - i__1);
			if (itemp > 0) {
			    ++ibb;
			    iiinst = iiinst * 10 + itemp - 1;
			}
			++ibb;
/* now at first +/-. Need end of 2nd number */
			i__1 = ibb;
			itemp = i_indx(line + i__1, "i", ib - i__1, (ftnlen)1)
				;
/* Rel pos'n of next 'i' */
			if (itemp > 0) {
			    iend = ibb + itemp - 1;
			} else {
			    iend = ib - 1;
			}
			io___97.ciunit = all_1.iorig[iiinst - 1] + 10;
			s_wsfe(&io___97);
/* Writing concatenation */
			i__3[0] = 3, a__1[0] = "Ki1";
			i__3[1] = iend - (ibb - 1), a__1[1] = line + (ibb - 1)
				;
			s_cat(ch__6, a__1, i__3, &c__2, (ftnlen)131);
			do_fio(&c__1, ch__6, iend - (ibb - 1) + 3);
			e_wsfe();
			if (itemp > 0) {
			    ibb = iend + 2;
			    goto L40;
			}
			goto L12;
/* Remove K command from string, go to next ia */
		    }
L9:
		    i__1 = ib - idxa;
		    allparts_(line + (idxa - 1), &i__1, ib - 1 - (idxa - 1));
L12:

/*  Remove the string from line */

		    if (idxa == 1) {
			s_copy(line, line + (ib - 1), (ftnlen)128, 128 - (ib 
				- 1));
		    } else {
/* Writing concatenation */
			i__3[0] = idxa - 1, a__1[0] = line;
			i__3[1] = 128 - (ib - 1), a__1[1] = line + (ib - 1);
			s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
		    }
		    lenline = lenstr_(line, &c__128, (ftnlen)128);

/*  Loop if only blanks are left */

		    if (lenline == 0) {
			goto L4;
		    }

/*  Must check for multiple "I" commands, so go to just after start of ia loop */

		    if (ia == 10) {
			goto L24;
		    }

/*  Tack on front part from 1st line of saved macro */

		    if (lenhold > 0) {
/* Writing concatenation */
			i__4[0] = lenhold, a__2[0] = holdln;
			i__4[1] = 1, a__2[1] = " ";
			i__4[2] = lenline, a__2[2] = line;
			s_cat(line, a__2, i__4, &c__3, (ftnlen)128);
			lenhold = 0;
		    }
		}
/* L5: */
	    }
	}
    }

/*  Now a special loop to deal with 'X'.  If it was %[n]X..., will have been */
/*  copied into part [n] already.  If no "B" or "P", remove.  If "P", just */
/*  remove the "P" so pmxa/b will process.  If "B". do nothing. */

L10:
    nchk = 1;
L13:
    ntinx = nchk - 1 + ntindex_(line + (nchk - 1), "X", 128 - (nchk - 1), (
	    ftnlen)1);
    if (ntinx > nchk - 1) {

/*  There is a non-TeX 'X' at ntinx.  Loop if neither 1st nor after a blank. */

	if (ntinx > 1) {
	    i__1 = ntinx - 2;
	    if (s_cmp(line + i__1, " ", ntinx - 1 - i__1, (ftnlen)1) != 0) {

/*  The X is not 1st char of PMX command.  Advance starting point, loop. */

		nchk = ntinx + 1;
		goto L13;
	    }
	}

/*  We now know the X at ntinx starts a PMX command.  Find next blank */

	i__1 = ntinx;
	ib = ntinx + i_indx(line + i__1, " ", 128 - i__1, (ftnlen)1);

/*  There must be a blank to right of "X", so ib>ntinx */

/*        locp = nchk-1+index(line(nchk:ib),'P') */
	i__1 = ntinx;
	locp = ntinx + i_indx(line + i__1, "P", ib - i__1, (ftnlen)1);

/*  Did not need to use ntindex because we already know bounds of PMX command. */

/*        if (locp .gt. nchk-1) then */
	if (locp > ntinx) {

/*  Strip out the 'P' */

	    s_copy(templine, line, (ftnlen)128, locp - 1);
	    i__1 = locp;
/* Writing concatenation */
	    i__3[0] = locp - 1, a__1[0] = templine;
	    i__3[1] = lenline - i__1, a__1[1] = line + i__1;
	    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
	    --lenline;
	    --ib;
	}
	if (i_indx(line + (ntinx - 1), ":", ib - (ntinx - 1), (ftnlen)1) > 0 
		|| i_indx(line + (ntinx - 1), "S", ib - (ntinx - 1), (ftnlen)
		1) > 0 || i_indx(line + (ntinx - 1), "B", ib - (ntinx - 1), (
		ftnlen)1) > 0 || locp > ntinx) {

/*  The X command is a shift, "Both", or "Part".  Do not remove. */

	    nchk = ib + 1;
	    goto L13;
	}

/*  Remove the X command. */

	if (ntinx == 1) {
	    if (ib < lenline) {
		i__1 = ib;
		s_copy(line, line + i__1, (ftnlen)128, lenline - i__1);
	    } else {

/*  line contains ONLY the "X" command, so get a new line */

		goto L4;
	    }
	} else {
	    s_copy(templine, line, (ftnlen)128, ntinx - 1);
	    if (ib < lenline) {
		i__1 = ib;
/* Writing concatenation */
		i__3[0] = ntinx - 1, a__1[0] = templine;
		i__3[1] = lenline - i__1, a__1[1] = line + i__1;
		s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
	    } else {
		s_copy(line, templine, (ftnlen)128, ntinx - 1);
	    }
	}

/*  Recompute lenline */

	lenline = lenstr_(line, &c__128, (ftnlen)128);

/*  Resume checking after location of removed command. */

	nchk = ntinx;
	goto L13;
    }

/*  End of loop for X-checks */

    oneof2 = ntindex_(line, "//", (ftnlen)128, (ftnlen)2) > 0;
    if (termrpt && all_1.botv[iv - 1] && frstln && *(unsigned char *)&line[
	    lenline - 1] == '/') {

/*  Must add a terminal repeat before the slash */

	if (oneof2) {
	    --lenline;
	}
	if (lenline > 1) {
	    io___102.ciunit = all_1.iorig[iinst - 1] + 10;
	    s_wsfe(&io___102);
	    do_fio(&c__1, line, lenline - 1);
	    e_wsfe();
	}
	if (! oneof2) {
/* Writing concatenation */
	    i__3[0] = 2, a__1[0] = termsym;
	    i__3[1] = 2, a__1[1] = " /";
	    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
	    lenline = 4;
	} else {
/* Writing concatenation */
	    i__3[0] = 2, a__1[0] = termsym;
	    i__3[1] = 3, a__1[1] = " //";
	    s_cat(line, a__1, i__3, &c__2, (ftnlen)128);
	    lenline = 5;
	}
    }
    if (termrpt && frstln && *(unsigned char *)&line[lenline - 1] == '/' && 
	    iv == all_1.nvnow) {
	termrpt = FALSE_;
    }
    io___103.ciunit = all_1.iorig[iinst - 1] + 10;
    s_wsfe(&io___103);
    do_fio(&c__1, line, lenline);
    e_wsfe();
    if (oneof2) {
	frstln = FALSE_;
    } else if (! frstln) {
	frstln = TRUE_;
    }
/*      if (ntindex(line,'/').gt.0 .and. index(line,'//').eq.0) then */
    if (ntindex_(line, "/", (ftnlen)128, (ftnlen)1) > 0 && ntindex_(line, 
	    "//", (ftnlen)128, (ftnlen)2) == 0) {
	iv = iv % all_1.nvnow + 1;
	iinst = all_1.instnum[iv - 1];
    }
    goto L4;
L999:
    cl__1.cerr = 0;
    cl__1.cunit = 10;
    cl__1.csta = 0;
    f_clos(&cl__1);

/*  In the mbrest checks, must run through ALL noinst files (not just noinow) */

    i__1 = all_1.noinst;
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
	if (all_1.nvi[iinst - 1] == 1) {
	    mbrests_(&iinst, jobname, &ljob, &ludpfn[iinst - 1], udpfnq + (
		    iinst - 1 << 7), (ftnlen)27, (ftnlen)128);
	} else {

/*  Send a signal with ljob to bypass most mbrest processing */

	    i__2 = -ljob;
	    mbrests_(&iinst, jobname, &i__2, &ludpfn[iinst - 1], udpfnq + (
		    iinst - 1 << 7), (ftnlen)27, (ftnlen)128);
	}
/* L11: */
    }
    return 0;
} /* MAIN__ */

integer lenstr_(char *string, integer *n, ftnlen string_len)
{
    /* System generated locals */
    integer ret_val;

    for (ret_val = *n; ret_val >= 1; --ret_val) {
	if (*(unsigned char *)&string[ret_val - 1] != ' ') {
	    return ret_val;
	}
/* L1: */
    }
    ret_val = 0;
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


    len = lenstr_(string, n, string_len);
    if (len == 0) {
	len = 1;
	s_copy(string, " ", string_len, (ftnlen)1);
    }
    i__1 = all_1.noinow;
    for (iinst = 1; iinst <= i__1; ++iinst) {
	io___106.ciunit = all_1.iorig[iinst - 1] + 10;
	s_wsfe(&io___106);
	do_fio(&c__1, string, len);
	e_wsfe();
/* L1: */
    }
    return 0;
} /* allparts_ */

/* Subroutine */ int mbrests_(integer *iv, char *jobname, integer *ljob, 
	integer *ludpfn, char *udpfnq, ftnlen jobname_len, ftnlen udpfnq_len)
{
    /* System generated locals */
    address a__1[2], a__2[3];
    integer i__1[2], i__2[3], i__3, i__4;
    real r__1;
    char ch__1[1], ch__2[18], ch__3[4], ch__4[2], ch__5[10];
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


    type4 = FALSE_;
    *(unsigned char *)sq = '\\';
    alldone = FALSE_;
    al__1.aerr = 0;
    al__1.aunit = *iv + 10;
    f_rew(&al__1);
/*      open(20,file=jobname(1:abs(ljob))//char(48+iv)//'.pmx') */
    if (*iv < 10) {
	*(unsigned char *)partq = (char) (*iv + 48);
	lpart = 1;

/*  5/16/10 Fix for >20 parts */

/*      else */
/*        partq = '1'//char(38+iv) */
/*        lpart = 2 */
    } else {
	lpart = 2;
	if (*iv < 20) {
/* Writing concatenation */
	    i__1[0] = 1, a__1[0] = "1";
	    *(unsigned char *)&ch__1[0] = *iv + 38;
	    i__1[1] = 1, a__1[1] = ch__1;
	    s_cat(partq, a__1, i__1, &c__2, (ftnlen)2);
	} else {
/* Writing concatenation */
	    i__1[0] = 1, a__1[0] = "2";
	    *(unsigned char *)&ch__1[0] = *iv + 28;
	    i__1[1] = 1, a__1[1] = ch__1;
	    s_cat(partq, a__1, i__1, &c__2, (ftnlen)2);
	}
    }
/*      if (ludpfn .eq. 0) then */
    o__1.oerr = 0;
    o__1.ounit = 40;
    o__1.ofnmlen = abs(*ljob) + lpart + 4;
/* Writing concatenation */
    i__2[0] = abs(*ljob), a__2[0] = jobname;
    i__2[1] = lpart, a__2[1] = partq;
    i__2[2] = 4, a__2[2] = ".pmx";
    s_cat(ch__2, a__2, i__2, &c__3, (ftnlen)18);
    o__1.ofnm = ch__2;
    o__1.orl = 0;
    o__1.osta = 0;
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
/*      else */
/*        open(30,file=udpfnq(1:ludpfn)//'.pmx') */
/*      end if */
    for (i__ = 1; i__ <= 10000; ++i__) {
	io___113.ciunit = *iv + 10;
	s_rsfe(&io___113);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	if (*(unsigned char *)&line[0] == '%' || s_cmp(line, "---", (ftnlen)3,
		 (ftnlen)3) == 0 || type4) {
	    len = lenstr_(line, &c__128, (ftnlen)128);
	    if (len > 0) {
		s_wsfe(&io___116);
		do_fio(&c__1, line, len);
		e_wsfe();
	    } else {
		s_wsfe(&io___117);
		do_fio(&c__1, " ", (ftnlen)1);
		e_wsfe();
	    }
	    if (s_cmp(line, "---", (ftnlen)3, (ftnlen)3) == 0) {
		type4 = ! type4;
	    }
	} else {
	    goto L11;
	}
/* L10: */
    }
    s_wsle(&io___118);
    do_lio(&c__9, &c__1, "You should not be here in scor2prt.  Call Dr. Don", 
	    (ftnlen)49);
    e_wsle();
    s_stop("", (ftnlen)0);
L11:

/*  Finished reading opening type4 TeX and comments.  Next line to be read */
/*  will contain the first of the input numbers */

    dosetup_(iv, line, &mtrnum, &mtrden, (ftnlen)128);
    for (i__ = 1; i__ <= 10000; ++i__) {
L13:
	io___121.ciunit = *iv + 10;
	i__3 = s_rsfe(&io___121);
	if (i__3 != 0) {
	    goto L999;
	}
	i__3 = do_fio(&c__1, line, (ftnlen)128);
	if (i__3 != 0) {
	    goto L999;
	}
	i__3 = e_rsfe();
	if (i__3 != 0) {
	    goto L999;
	}
L7:
	len = lenstr_(line, &c__128, (ftnlen)128);

/*  Pass-through if instrumnet has >1 voice. */

	if (*ljob < 0) {
	    goto L2;
	}
	if (i_indx("TtTiTch+h-h l ", line, (ftnlen)14, (ftnlen)2) > 0) {

/*  Traps titles, instruments, composers, headers, lower strings.  Read 2 lines. */

	    s_wsfe(&io___122);
	    do_fio(&c__1, line, len);
	    e_wsfe();
	    io___123.ciunit = *iv + 10;
	    s_rsfe(&io___123);
	    do_fio(&c__1, line, (ftnlen)128);
	    e_rsfe();
	    len = lenstr_(line, &c__128, (ftnlen)128);
	    goto L2;
	}
	if (i__ == 1 || i__ > 5 && *(unsigned char *)&line[0] == 'm') {

/*  NOTE! The above test is truly bogus. */

	    if (*(unsigned char *)&line[0] == '%') {
		s_wsfe(&io___124);
		do_fio(&c__1, line, len);
		e_wsfe();
		goto L13;
	    }
	    if (i__ == 1) {
/*            read(line(1),'(10x,2i5)')mtrnum,mtrden */
	    } else {

/*  Check for slashes (new meter change syntax) */

		idxs = i_indx(line, "/", (ftnlen)128, (ftnlen)1);
		idxb = i_indx(line, " ", (ftnlen)128, (ftnlen)1);
		newmtr = idxs > 0 && (idxb == 0 || idxs < idxb);
		if (! newmtr) {

/*  Old way, no slashes, uses 'o' for lonesome '1' */

		    icden = 3;
		    if (*(unsigned char *)&line[1] == 'o') {
			mtrnum = 1;
		    } else {
			mtrnum = *(unsigned char *)&line[1] - 48;
			if (mtrnum == 1) {
			    icden = 4;
			    mtrnum = *(unsigned char *)&line[2] - 38;
			}
		    }
		    mtrden = *(unsigned char *)&line[icden - 1] - 48;
		} else {

/*  New way with slashes: idxs is index of 1st slash! */

		    ici__1.icierr = 0;
		    ici__1.iciend = 0;
		    ici__1.icirnum = 1;
		    ici__1.icirlen = idxs - 2;
		    ici__1.iciunit = line + 1;
/* Writing concatenation */
		    i__2[0] = 2, a__2[0] = "(i";
		    *(unsigned char *)&ch__1[0] = idxs + 46;
		    i__2[1] = 1, a__2[1] = ch__1;
		    i__2[2] = 1, a__2[2] = ")";
		    ici__1.icifmt = (s_cat(ch__3, a__2, i__2, &c__3, (ftnlen)
			    4), ch__3);
		    s_rsfi(&ici__1);
		    do_fio(&c__1, (char *)&mtrnum, (ftnlen)sizeof(integer));
		    e_rsfi();
		    i__3 = idxs;
		    idxb = i_indx(line + i__3, "/", 128 - i__3, (ftnlen)1);
		    i__3 = idxs;
		    ici__1.icierr = 0;
		    ici__1.iciend = 0;
		    ici__1.icirnum = 1;
		    ici__1.icirlen = idxs + idxb - 1 - i__3;
		    ici__1.iciunit = line + i__3;
/* Writing concatenation */
		    i__2[0] = 2, a__2[0] = "(i";
		    *(unsigned char *)&ch__1[0] = idxb + 47;
		    i__2[1] = 1, a__2[1] = ch__1;
		    i__2[2] = 1, a__2[2] = ")";
		    ici__1.icifmt = (s_cat(ch__3, a__2, i__2, &c__3, (ftnlen)
			    4), ch__3);
		    s_rsfi(&ici__1);
		    do_fio(&c__1, (char *)&mtrden, (ftnlen)sizeof(integer));
		    e_rsfi();
		}
	    }
	    lenbeat = ifnodur_(&mtrden, "x", (ftnlen)1);
	    lenmult = 1;
	    if (mtrden == 2) {
		lenbeat = 16;
		lenmult = 2;
	    }
	    lenbar = lenmult * mtrnum * lenbeat;
	    fwbrsym_(&lenbar, &nwbrs, wbrsym, &lwbrs, (ftnlen)3);
	}
	ip1 = 0;
	s_copy(line1, line, (ftnlen)128, (ftnlen)128);
	i__3 = nwbrs;
	for (iw = 0; iw <= i__3; ++iw) {
	    if (iw > 0) {
		idx = ntindex_(line1, wbrsym + (iw - 1) * 3, (ftnlen)128, 
			lwbrs);
		if (idx > 0) {

/*  Check for blank or shifted rest, discount it if it's there */

		    i__4 = idx + lwbrs - 1;
		    if (s_cmp(line1 + i__4, " ", idx + lwbrs - i__4, (ftnlen)
			    1) != 0) {
			idx = 0;
		    }
		}
	    } else {
		idx = ntindex_(line1, "rp", (ftnlen)128, (ftnlen)2);

/*  Check for raised rest */

		if (idx > 0) {
		    i__4 = idx + 1;
		    if (s_cmp(line1 + i__4, " ", idx + 2 - i__4, (ftnlen)1) !=
			     0) {
			idx = 0;
		    }
		}
	    }
	    if (idx > 0) {
		if (ip1 == 0) {
		    ip1 = idx;
		} else {
		    ip1 = min(ip1,idx);
		}
	    }
/* L3: */
	}
/* Writing concatenation */
	i__1[0] = 1, a__1[0] = sq;
	i__1[1] = 1, a__1[1] = sq;
	s_cat(ch__4, a__1, i__1, &c__2, (ftnlen)2);
	if (i__ < 5 || *(unsigned char *)&line[0] == '%' || s_cmp(line, ch__4,
		 (ftnlen)2, (ftnlen)2) == 0 || ip1 == 0) {
	    goto L2;
	}

/*  Switch to multibar rest search mode!!!  Start forward in line(1) */

	rpfirst = s_cmp(line1 + (ip1 - 1), "rp", (ftnlen)2, (ftnlen)2) == 0;
	iline = 1;
	nmbr = 1;
	if (rpfirst) {
	    lwbrsx = 2;
	} else {
	    lwbrsx = lwbrs;
	}
	ipe = ip1 + lwbrsx - 1;
L4:
	if (ipe == len) {

/*  Need a new line */

	    ++iline;
L6:
	    io___144.ciunit = *iv + 10;
	    i__3 = s_rsfe(&io___144);
	    if (i__3 != 0) {
		goto L998;
	    }
	    i__3 = do_fio(&c__1, line + (iline - 1 << 7), (ftnlen)128);
	    if (i__3 != 0) {
		goto L998;
	    }
	    i__3 = e_rsfe();
	    if (i__3 != 0) {
		goto L998;
	    }
	    len = lenstr_(line + (iline - 1 << 7), &c__128, (ftnlen)128);
	    if (*(unsigned char *)&line[(iline - 1) * 128] == '%') {
		s_wsfe(&io___145);
		do_fio(&c__1, "% Following comment has been moved forward", (
			ftnlen)42);
		e_wsfe();
		s_wsfe(&io___146);
		do_fio(&c__1, line + (iline - 1 << 7), len);
		e_wsfe();
		goto L6;
	    }
	    ipe = 0;
	    goto L4;
L998:

/*  No more input left */

	    s_wsle(&io___147);
	    do_lio(&c__9, &c__1, "All done!", (ftnlen)9);
	    e_wsle();
	    alldone = TRUE_;
	    ipe = 0;
	    --iline;
	    len = lenstr_(line + (iline - 1 << 7), &c__128, (ftnlen)128);
	    goto L4;
	} else {
	    if (alldone) {
		*(unsigned char *)sym = ' ';
	    } else {

/*  ipe<len here, so it's ok to get a symbol */

		nextsym_(line + (iline - 1 << 7), &len, &ipe, &ipenew, sym, &
			lsym, (ftnlen)128, (ftnlen)80);
	    }

/*  Check for end of block or bar line symbol */

	    if (i_indx("/|", sym, (ftnlen)2, (ftnlen)1) > 0) {
		ipe = ipenew;
		goto L4;
	    } else {
		wbrest = FALSE_;
		if (alldone) {
		    goto L12;
		}
		i__3 = nwbrs;
		for (iw = 1; iw <= i__3; ++iw) {
		    wbrest = wbrest || s_cmp(sym, wbrsym + (iw - 1) * 3, lsym,
			     lwbrs) == 0;
/* L5: */
		}
		wbrest = wbrest || s_cmp(sym, "r", lsym, (ftnlen)1) == 0 && 
			lwbrs == 2 || s_cmp(sym, "rd", lsym, (ftnlen)2) == 0 
			&& lwbrs == 3 || s_cmp(sym, "rp", lsym, (ftnlen)2) == 
			0 || s_cmp(sym, "r", lsym, (ftnlen)1) == 0 && rpfirst;
L12:
		if (wbrest) {
		    ipe = ipenew;
		    ++nmbr;
		    goto L4;
		} else {

/*  AHA! Failed prev. test, so last symbol was *not* mbr. */
/*  It must be saved, and its starting position is ipenew-lsym+1 */

		    if (nmbr > 1) {

/*  Write stuff up to start of mbr */

			if (ip1 > 1) {
			    s_wsfe(&io___152);
			    do_fio(&c__1, line, ip1 - 1);
			    e_wsfe();
			}

/*  Insert mbr symbol.  Always end with a slash just in case next sym must be */
/*  at start of block.  May think this causes undefined octaves, but */
/*  probably not since it's a single voice. */

			r__1 = nmbr + .01f;
			ndig = (integer) r_lg10(&r__1) + 1;
			s_wsle(&io___154);
			do_lio(&c__9, &c__1, "Inserting rm, iv,nmbr:", (
				ftnlen)22);
			do_lio(&c__3, &c__1, (char *)&(*iv), (ftnlen)sizeof(
				integer));
			do_lio(&c__3, &c__1, (char *)&nmbr, (ftnlen)sizeof(
				integer));
			e_wsle();
			ci__1.cierr = 0;
			ci__1.ciunit = 40;
/* Writing concatenation */
			i__2[0] = 5, a__2[0] = "(a2,i";
			*(unsigned char *)&ch__1[0] = ndig + 48;
			i__2[1] = 1, a__2[1] = ch__1;
			i__2[2] = 4, a__2[2] = ",a2)";
			ci__1.cifmt = (s_cat(ch__5, a__2, i__2, &c__3, (
				ftnlen)10), ch__5);
			s_wsfe(&ci__1);
			do_fio(&c__1, "rm", (ftnlen)2);
			do_fio(&c__1, (char *)&nmbr, (ftnlen)sizeof(integer));
			do_fio(&c__1, " /", (ftnlen)2);
			e_wsfe();
			if (alldone) {
			    goto L999;
			}
			ipc = ipenew - lsym + 1;
			s_copy(line, line + ((iline - 1 << 7) + (ipc - 1)), (
				ftnlen)128, len - (ipc - 1));
		    } else {

/*  Write old stuff up to end of original lonesome wbr, save the rest. */
/*  4 cases:  (wbr /) , (wbr line-end) , (wbr followed by other non-/ symbols) , */
/*      alldone. */
/*  In 1st 2 will have gotten some other lines, so write all up to one b4 last */
/*  non-comment line; then revert to normal mode on that.  In 3rd case must */
/*  split line. */

			if (alldone) {
			    s_wsfe(&io___156);
			    do_fio(&c__1, line, len);
			    e_wsfe();
			    goto L999;
			} else if (iline > 1) {
			    i__3 = iline - 1;
			    for (il = 1; il <= i__3; ++il) {
				len = lenstr_(line + (il - 1 << 7), &c__128, (
					ftnlen)128);
				s_wsfe(&io___158);
				do_fio(&c__1, line + (il - 1 << 7), len);
				e_wsfe();
/* L9: */
			    }
			    s_copy(line, line + (iline - 1 << 7), (ftnlen)128,
				     (ftnlen)128);
			} else {

/*  Since iline = 1 the wbr is not the last sym, so must split */

			    s_wsfe(&io___159);
			    do_fio(&c__1, line, ip1 + lwbrsx - 1);
			    e_wsfe();
			    i__3 = ip1 + lwbrsx;
			    s_copy(line, line + i__3, (ftnlen)128, len - i__3)
				    ;
			}
		    }

/*  Exit multibar mode */

		    goto L7;
		}
	    }
	}
L2:
	if (len > 0) {
	    s_wsfe(&io___160);
	    do_fio(&c__1, line, len);
	    e_wsfe();
	} else {
	    s_wsfe(&io___161);
	    do_fio(&c__1, " ", (ftnlen)1);
	    e_wsfe();
	}
/* L1: */
    }
L999:
    cl__1.cerr = 0;
    cl__1.cunit = *iv + 10;
    cl__1.csta = 0;
    f_clos(&cl__1);
    cl__1.cerr = 0;
    cl__1.cunit = 40;
    cl__1.csta = 0;
    f_clos(&cl__1);
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


    if (*idur == 6) {
	ret_val = 1;
    } else if (*idur == 3) {
	ret_val = 2;
    } else if (*idur == 1 || *idur == 16) {
	ret_val = 4;
    } else if (*idur == 8) {
	ret_val = 8;
    } else if (*idur == 4) {
	ret_val = 16;
    } else if (*idur == 2) {
	ret_val = 32;
    } else if (*idur == 0) {
	ret_val = 64;
    } else {
	s_wsle(&io___162);
	do_lio(&c__9, &c__1, "You entered an invalid note-length value", (
		ftnlen)40);
	e_wsle();
	s_stop("", (ftnlen)0);
    }
    if (*(unsigned char *)dotq == 'd') {
	ret_val = ret_val * 3 / 2;
    }
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


    /* Parameter adjustments */
    wbrsym -= 3;

    /* Function Body */
    *nwbrs = 1;
    *lwbrs = 2;
    if (*lenbar == 16) {
	s_copy(wbrsym + 3, "r4", (ftnlen)3, (ftnlen)2);
    } else if (*lenbar == 32) {
	s_copy(wbrsym + 3, "r2", (ftnlen)3, (ftnlen)2);
    } else if (*lenbar == 64) {
	s_copy(wbrsym + 3, "r0", (ftnlen)3, (ftnlen)2);
    } else if (*lenbar == 8) {
	s_copy(wbrsym + 3, "r8", (ftnlen)3, (ftnlen)2);
    } else if (*lenbar == 128) {
	s_copy(wbrsym + 3, "r9", (ftnlen)3, (ftnlen)2);
    } else {
	*nwbrs = 2;
	*lwbrs = 3;
	if (*lenbar == 24) {
	    s_copy(wbrsym + 3, "rd4", (ftnlen)3, (ftnlen)3);
	    s_copy(wbrsym + 6, "r4d", (ftnlen)3, (ftnlen)3);
	} else if (*lenbar == 48) {
	    s_copy(wbrsym + 3, "rd2", (ftnlen)3, (ftnlen)3);
	    s_copy(wbrsym + 6, "r2d", (ftnlen)3, (ftnlen)3);
	} else if (*lenbar == 96) {
	    s_copy(wbrsym + 3, "rd0", (ftnlen)3, (ftnlen)3);
	    s_copy(wbrsym + 6, "r0d", (ftnlen)3, (ftnlen)3);
	} else {
	    s_wsfe(&io___163);
	    do_fio(&c__1, (char *)&(*lenbar), (ftnlen)sizeof(integer));
	    e_wsfe();
	}
    }
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

    if (*ipeold >= *len) {
	s_wsle(&io___164);
	do_lio(&c__9, &c__1, "Called nextsym with ipstart>=len ", (ftnlen)33);
	e_wsle();
	s_wsle(&io___165);
	do_lio(&c__9, &c__1, "Send files to Dr. Don at dsimons@logicon.com", (
		ftnlen)44);
	e_wsle();
	s_stop("", (ftnlen)0);
    }
    i__1 = *len;
    for (ip = *ipeold + 1; ip <= i__1; ++ip) {
	if (*(unsigned char *)&line[ip - 1] != ' ') {

/*  symbol starts here (ip).  We're committed to exit the loop. */

	    if (ip < *len) {
		i__2 = *len;
		for (iip = ip + 1; iip <= i__2; ++iip) {
		    if (*(unsigned char *)&line[iip - 1] != ' ') {
			goto L2;
		    }

/*  iip is the space after the symbol */

		    *ipenew = iip - 1;
		    *lsym = *ipenew - ip + 1;
		    s_copy(sym, line + (ip - 1), (ftnlen)80, *ipenew - (ip - 
			    1));
		    return 0;
L2:
		    ;
		}

/*  Have len>=2 and ends on len */

		*ipenew = *len;
		*lsym = *ipenew - ip + 1;
		s_copy(sym, line + (ip - 1), (ftnlen)80, *ipenew - (ip - 1));
		return 0;
	    } else {

/*  ip = len */

		*ipenew = *len;
		*lsym = 1;
		s_copy(sym, line + (ip - 1), (ftnlen)80, (ftnlen)1);
		return 0;
	    }
	}
/* L1: */
    }
    s_wsle(&io___168);
    do_lio(&c__9, &c__1, "Error #3.  Send files to Dr. Don at dsimons@logico"
	    "n.com", (ftnlen)55);
    e_wsle();
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

    s_copy(tline, line, (ftnlen)128, (ftnlen)128);
    ndxs2 = i_indx(tline, s2q, (ftnlen)128, s2q_len);

/*  Return point below for rechecks after zapping D"  " */

L2:
    ndxbs = i_indx(tline, "\\", (ftnlen)128, (ftnlen)1);
    if (ndxbs > 0) {

/* Special check in case \ is inside D"..." */

	ndxdq1 = i_indx(tline, "D\"", (ftnlen)128, (ftnlen)2);

/* If the following test fails, flow out of if block; else loop up to 2. */

	if (ndxdq1 > 0) {

/* Find end of D"..." */

	    i__1 = ndxdq1 + 1;
	    ndxdq2 = ndxdq1 + 1 + i_indx(tline + i__1, "\"", 128 - i__1, (
		    ftnlen)1);
	    if (ndxdq2 == ndxdq1 + 1) {
		s_wsle(&io___174);
		do_lio(&c__9, &c__1, "Something is really wierd here", (
			ftnlen)30);
		e_wsle();
		s_stop("", (ftnlen)0);
	    }
	    s_copy(tline, tline, (ftnlen)128, ndxdq1 - 1);
	    i__1 = ndxdq2;
	    for (ic = ndxdq1; ic <= i__1; ++ic) {
/* Writing concatenation */
		i__2[0] = ic - 1, a__1[0] = tline;
		i__2[1] = 1, a__1[1] = " ";
		s_cat(tline, a__1, i__2, &c__2, (ftnlen)128);
/* L3: */
	    }
	    i__1 = ndxdq2;
/* Writing concatenation */
	    i__2[0] = ndxdq2, a__1[0] = tline;
	    i__2[1] = 128 - i__1, a__1[1] = line + i__1;
	    s_cat(tline, a__1, i__2, &c__2, (ftnlen)128);
	    goto L2;
	}
    }
    if (ndxbs == 0 || ndxs2 < ndxbs) {
	ret_val = ndxs2;
/*     print*,'No bs, or char is left of 1st bs, ntindex:',ntindex */
    } else {

/*  There are both bs and s2q, and bs is to the left of sq2. So check bs's to */
/*  right of first: End is '\ ', start is ' \' */

	len = lenstr_(tline, &c__128, (ftnlen)128);
	intex = TRUE_;
/*     print*,'intex+>',intex */
	i__1 = len;
	for (ic = ndxbs + 1; ic <= i__1; ++ic) {
	    if (ic == ndxs2) {
		if (intex) {
		    ret_val = 0;
		    i__3 = ic;
		    ndxs2 = i_indx(tline + i__3, s2q, len - i__3, s2q_len) + 
			    ic;
/*     print*,'ndxs2 =>',ndxs2 */
		} else {
		    ret_val = ndxs2;
		    return ret_val;
		}
/*     print*,'Internal exit, intex, ntindex:',intex,ntindex */
	    } else /* if(complicated condition) */ {
		i__3 = ic;
		if (intex && s_cmp(tline + i__3, "\\ ", ic + 2 - i__3, (
			ftnlen)2) == 0) {
		    intex = FALSE_;
/*     print*,'intex+>',intex */
		} else /* if(complicated condition) */ {
		    i__3 = ic;
		    if (! intex && s_cmp(tline + i__3, " \\", ic + 2 - i__3, (
			    ftnlen)2) == 0) {
			intex = TRUE_;
/*     print*,'intex+>',intex */
		    }
		}
	    }
/* L1: */
	}
/*     print*,'Out end of loop 1' */
    }
/*     print*,'Exiting ntindex at the end???' */
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

    if (*iccount == 128) {
	s_rsfe(&io___178);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	*iccount = 0;
    }
    ++(*iccount);
    *(unsigned char *)charq = *(unsigned char *)&line[*iccount - 1];
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

L4:
    if (*iccount == 128) {
	s_rsfe(&io___179);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	if (all_1.replacing) {
	    all_1.replacing = FALSE_;
	}
	chkcom_(line, (ftnlen)128);
	*iccount = 0;
    }
    ++(*iccount);

/*  Find next non-blank or end of line */

    for (*iccount = *iccount; *iccount <= 127; ++(*iccount)) {
	if (*(unsigned char *)&line[*iccount - 1] != ' ') {
	    goto L3;
	}
/* L2: */
    }

/*  If here, need to get a new line */

    *iccount = 128;
    goto L4;
L3:

/*  iccount now points to start of number to read */

    i1 = *iccount;
L5:
    getchar_(line, iccount, durq, (ftnlen)128, (ftnlen)1);

/*  Remember that getchar first increments iccount, *then* reads a character. */

    if (i_indx("0123456789.-", durq, (ftnlen)12, (ftnlen)1) > 0) {
	goto L5;
    }
    i2 = *iccount - 1;
    if (i2 < i1) {
	s_wsle(&io___183);
/* Writing concatenation */
	i__1[0] = 7, a__1[0] = "Found \"";
	i__1[1] = 1, a__1[1] = durq;
	i__1[2] = 19, a__1[2] = "\" instead of number";
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)27);
	do_lio(&c__9, &c__1, ch__1, (ftnlen)27);
	e_wsle();
	s_stop("1", (ftnlen)1);
    }
    icf = i2 - i1 + 49;
    ici__1.icierr = 0;
    ici__1.iciend = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = i2 - (i1 - 1);
    ici__1.iciunit = line + (i1 - 1);
/* Writing concatenation */
    i__1[0] = 2, a__1[0] = "(f";
    *(unsigned char *)&ch__3[0] = icf;
    i__1[1] = 1, a__1[1] = ch__3;
    i__1[2] = 3, a__1[2] = ".0)";
    ici__1.icifmt = (s_cat(ch__2, a__1, i__1, &c__3, (ftnlen)6), ch__2);
    s_rsfi(&ici__1);
    do_fio(&c__1, (char *)&ret_val, (ftnlen)sizeof(real));
    e_rsfi();
    if (! all_1.replacing) {
	if (*iread == 0) {
	    i__2 = i2 - i1 + 1;
	    allparts_(line + (i1 - 1), &i__2, i2 - (i1 - 1));
	} else if (*iread == 1) {
	    allparts_("-999", &c__4, (ftnlen)4);
	} else if (*iread == 2) {
	    allparts_("1", &c__1, (ftnlen)1);
	} else if (*iread == 3) {
	    allparts_("-998", &c__4, (ftnlen)4);
	} else if (*iread == 4) {
	    allparts_("20", &c__2, (ftnlen)2);
	} else if (*iread == 5) {
	    allparts_(".05", &c__3, (ftnlen)3);
	} else if (*iread != -1) {
	    s_wsle(&io___185);
	    do_lio(&c__9, &c__1, "Error with iread in readin", (ftnlen)26);
	    e_wsle();
	    s_stop("", (ftnlen)0);
	}
    }
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

L1:
    if (*(unsigned char *)line != '%') {
	return 0;
    }

/*  If here, line has some sort of comment */

    if (*(unsigned char *)&line[1] == '%') {
	if (! all_1.insetup) {

/*  Suck up a line, then flow out of "if" block to get another and loop */

	    s_rsfe(&io___186);
	    do_fio(&c__1, line, (ftnlen)128);
	    e_rsfe();
/* ++VV */

/*  UNLESS (a) it has a score-only "M" and changes # of inst's. */

	    if (i_indx(line, "M", (ftnlen)128, (ftnlen)1) > 0) {
		idxl = i_indx(line, "L", (ftnlen)128, (ftnlen)1);
		idxm = i_indx(line, "M", (ftnlen)128, (ftnlen)1);
		idxn = i_indx(line, "n", (ftnlen)128, (ftnlen)1);
		idxb = i_indx(line, " ", (ftnlen)128, (ftnlen)1);
		if (idxl < idxm && idxm < idxn && (idxb == 0 || idxn < idxb)) 
			{
		    i__1 = idxn;
		    all_1.noinow = *(unsigned char *)&line[i__1] - 48;
		    clefpend = TRUE_;

/*  Next noinow digits are original inst. #'s of new inst. set.  Next noinow */
/*  char's after that are clefs */

		    all_1.nvnow = 0;
		    i__1 = all_1.noinow;
		    for (j = 1; j <= i__1; ++j) {
			i__2 = idxn + 1 + j - 1;
			all_1.iorig[j - 1] = *(unsigned char *)&line[i__2] - 
				48;
			iposc0 = idxn + 1 + all_1.noinow;
			i__2 = all_1.nvi[all_1.iorig[j - 1] - 1];
			for (k = 1; k <= i__2; ++k) {
			    ++all_1.nvnow;
/*                  clefq(nvnow) = line(iposc0+nvnow:iposc0+nvnow) */
			    all_1.instnum[all_1.nvnow - 1] = j;
			    all_1.botv[all_1.nvnow - 1] = k == 1 && j != 1;
/* L25: */
			}
/* L24: */
		    }
		}
	    }

/*  or if it's "h" or "l", need to suck up one more line */

	    if (*(unsigned char *)line == 'h' && i_indx("+- ", line + 1, (
		    ftnlen)3, (ftnlen)1) > 0 || *(unsigned char *)line == 'T' 
		    || s_cmp(line, "l ", (ftnlen)2, (ftnlen)2) == 0) {
		s_rsfe(&io___195);
		do_fio(&c__1, line, (ftnlen)128);
		e_rsfe();
	    }

/*  4/29/00 check for T string also */

	} else {

/*  In setup mode. Set flag, flow out and do use following line */

	    all_1.replacing = TRUE_;
	}
    } else if (*(unsigned char *)&line[1] == '!') {

/*  Copy to all parts */

	allparts_(line + 2, &c__125, (ftnlen)126);
    } else {

/*  Get value of hex integer 1,2,...,9,a,b,c in 2nd position, zero otherwise */
/* c  Get value of extended hex integer 1,2,...,9,a,b,c,...,o in 2nd position, zero otherwise */

	ivq = i_indx("123456789abcdefghijklmno", line + 1, (ftnlen)24, (
		ftnlen)1);

/*  Only treat as part-specific pmx line if number .le. noinst */

	if (ivq < 1 || ivq > all_1.noinst) {

/*  Simple comment. */

	    allparts_(line, &c__128, (ftnlen)128);
	} else {

/*  Instrument comment, copy only to part */

	    lenline = lenstr_(line, &c__128, (ftnlen)128);
	    if (lenline > 2) {
		io___198.ciunit = ivq + 10;
		s_wsfe(&io___198);
		do_fio(&c__1, line + 2, lenline - 2);
		e_wsfe();
	    } else {

/*  Transferring blank line */

		io___199.ciunit = ivq + 10;
		s_wsfe(&io___199);
		do_fio(&c__1, " ", (ftnlen)1);
		e_wsfe();
	    }
	}
    }
    i__1 = s_rsfe(&io___200);
    if (i__1 != 0) {
	goto L2;
    }
    i__1 = do_fio(&c__1, line, (ftnlen)128);
    if (i__1 != 0) {
	goto L2;
    }
    i__1 = e_rsfe();
    if (i__1 != 0) {
	goto L2;
    }
    zapbl_(line, &c__128, (ftnlen)128);
    goto L1;
L2:
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

    iccount = 0;
    for (iset = 1; iset <= 12; ++iset) {
	partnum_(iv, &iccount, line, &xdata, (ftnlen)128);
	if (iset == 2) {
	    if (xdata > 0.f) {
		s_wsfe(&io___204);
		i__1 = (integer) (xdata + .1f);
		do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
		e_wsfe();
	    } else {
		noi = -xdata + .1f;
		s_wsfe(&io___206);
		do_fio(&c__1, (char *)&noi, (ftnlen)sizeof(integer));
		e_wsfe();
		i__1 = noi;
		for (ioi = 1; ioi <= i__1; ++ioi) {
		    partnum_(iv, &iccount, line, &xdata, (ftnlen)128);
		    s_wsfe(&io___208);
		    i__2 = (integer) (xdata + .1f);
		    do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
		    e_wsfe();
/* L2: */
		}
	    }
/*        else if (iset.ne.8 .and. xdata.lt.0) then */
	} else if (iset != 8 && iset != 5 && xdata < 0.f) {

/*  Must be either nv or npages */

	    if ((integer) (-xdata + .1f) == 999) {

/*  It's nv */

		s_wsfe(&io___209);
		do_fio(&c__1, (char *)&all_2.nvi[*iv - 1], (ftnlen)sizeof(
			integer));
		e_wsfe();
	    } else {

/*  npages must be computed */

		s_wsfe(&io___210);
		i__1 = (all_2.nsyst - 1) / 12 + 1;
		do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
		e_wsfe();
	    }
	} else if (iset != 7 && iset != 12) {

/*  write integer */

	    s_wsfe(&io___211);
	    i__1 = i_nint(&xdata);
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    e_wsfe();
	} else {

/*  write floating number */

	    s_wsfe(&io___212);
	    do_fio(&c__1, (char *)&xdata, (ftnlen)sizeof(real));
	    e_wsfe();
	}
	if (iset == 3) {
	    *mtrnum = i_nint(&xdata);
	} else if (iset == 4) {
	    *mtrden = i_nint(&xdata);
	}
/* L1: */
    }
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

L2:
    if (*iccount == 128) {
	io___213.ciunit = *iv + 10;
	s_rsfe(&io___213);
	do_fio(&c__1, line, (ftnlen)128);
	e_rsfe();
	if (*(unsigned char *)line == '%') {
	    len = lenstr_(line, &c__128, (ftnlen)128);
	    s_wsfe(&io___215);
	    do_fio(&c__1, line, len);
	    e_wsfe();
	    goto L2;
	}
	*iccount = 0;
    }
    ++(*iccount);

/*  Find next non-blank or end of line */

    for (*iccount = *iccount; *iccount <= 127; ++(*iccount)) {
	if (*(unsigned char *)&line[*iccount - 1] != ' ') {
	    goto L3;
	}
/* L4: */
    }

/*  If here, iccount=128 and need to get a new line */

    goto L2;
L3:

/*  iccount now points to start of number to read */

    i1 = *iccount;
L5:
    getchar_(line, iccount, durq, (ftnlen)128, (ftnlen)1);

/*  Remember that getchar first increments iccount, *then* reads a character. */

    if (i_indx("0123456789.-", durq, (ftnlen)12, (ftnlen)1) > 0) {
	goto L5;
    }
    i2 = *iccount - 1;
    if (i2 < i1) {
	s_wsle(&io___219);
/* Writing concatenation */
	i__1[0] = 7, a__1[0] = "Found \"";
	i__1[1] = 1, a__1[1] = durq;
	i__1[2] = 19, a__1[2] = "\" instead of number";
	s_cat(ch__1, a__1, i__1, &c__3, (ftnlen)27);
	do_lio(&c__9, &c__1, ch__1, (ftnlen)27);
	e_wsle();
	s_stop("1", (ftnlen)1);
    }
    icf = i2 - i1 + 49;
    ici__1.icierr = 0;
    ici__1.iciend = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = i2 - (i1 - 1);
    ici__1.iciunit = line + (i1 - 1);
/* Writing concatenation */
    i__1[0] = 2, a__1[0] = "(f";
    *(unsigned char *)&ch__3[0] = icf;
    i__1[1] = 1, a__1[1] = ch__3;
    i__1[2] = 3, a__1[2] = ".0)";
    ici__1.icifmt = (s_cat(ch__2, a__1, i__1, &c__3, (ftnlen)6), ch__2);
    s_rsfi(&ici__1);
    do_fio(&c__1, (char *)&(*xdata), (ftnlen)sizeof(real));
    e_rsfi();
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

    i__1 = *len;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*(unsigned char *)&string[i__ - 1] == ' ') {
	    goto L1;
	}
	if (i__ == 1) {
	    return 0;
	}
	goto L2;
L1:
	;
    }

/*  If line is all blank, leave it alone */

    return 0;
L2:
    s_copy(string, string + (i__ - 1), string_len, *len - (i__ - 1));
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

    numdqs = 0;
    i__1 = *indx - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (*(unsigned char *)&lineq[i__ - 1] == '"') {
	    ++numdqs;
	}
/* L1: */
    }
    *yesodd = numdqs % 2 == 1;
    return 0;
} /* oddquotesbefore_ */

