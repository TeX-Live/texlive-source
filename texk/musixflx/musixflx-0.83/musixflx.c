#define VERSION "0.83"
#define MYVERSION "/T.63dt+jh.2\0" /* Revision jh.2  jh-2 */

/****************************************************************************
 Line breaking program for MusiXTeX.
 (c) Ross Mitchell 1992-1997 ross.mitchell@csiro.au


This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*****************************************************************************/

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TRUE  1
#define FALSE 0

/* FILENAME_MAX should be in stdio.h, but if your compiler loses,
   here's a replacement.  */
#ifndef FILENAME_MAX
#define FILENAME_MAX 512
#endif
#define FNL FILENAME_MAX

/* attention: every \changecontext reports a change of \sign@skip */

#define MAX_SIGNS    128  /* max signature changes  */
#define MAX_SECTIONS 128  /* max sections */
#define MAX_BARS    2048  /* max number of bars */

#define GETLINE fgets(linebuf, (int) sizeof(linebuf), infile)

int zbar[MAX_BARS], lr_repeat[MAX_BARS], raggedline[MAX_BARS],
    l_repeat[MAX_BARS], barno[MAX_BARS], autolines[MAX_SECTIONS],
    bars[MAX_SECTIONS], mulooseness[MAX_SECTIONS], signchange[MAX_SIGNS],
   /* Counters and storage needed to detect xbar sign changes.  jh-1 */
    xbar_count, xbar_flag, softspace_count, xbar[MAX_BARS],
    linegoal[MAX_SECTIONS], natural_lines /* jh-2 */;

double hardbarlength[MAX_BARS], softbarlength[MAX_BARS],
       width_leftrightrepeat[MAX_BARS], width_leftrepeat[MAX_BARS],
       eff_hardlength[MAX_SECTIONS], eff_softlength[MAX_SECTIONS],
       oldsignskip[MAX_SIGNS], signskip[MAX_SIGNS],
       futuresignskip[MAX_SIGNS], tempholdskip, hardsign /* jh-1 */;

FILE *infile, *outfile, *logfile;
char linebuf[128];

static void
error_exit(int error_number)
{
  switch (error_number){

  case 0:
    printf("\nFile error: Disk full ?\n"); exit(3);
  case 1:
    printf("\nUsage: musixflx filename [d|f|s] (optional for debug modes)\n");
    exit(3);
  case 2:
    printf("\nThis shouldn't happen ! Too less bars or \\mulooseness too large ?\n"); exit(3);
  case 3:
    printf("\nThis shouldn't happen ! Too less bars in section !\n"); exit(3);
  case 4:
    printf("\nMissing endmark ! Forgotten \\stop[end]piece ?\n"); exit(3);
  case 5:
    printf("\nDivision by zero ! Ask a wizard !\n"); exit(3);
  default:
    printf("!!! Can't go on !!!\n"); exit(3);}}

int
main(int argc, char **argv)
{
  register int i, j, bar;

  int junk= -9999,
      dbug= FALSE,
      dbug_lines= FALSE,
      dbug_logfile= FALSE,
      showresult= FALSE,
      detectraggedline= FALSE,
      currentline= 1,
      samechapter= TRUE,
      line_number= 0,
      chapterno= 1,

      detect_end, typ, jbar, sign, all_section, section, mark, lines,
      lastbarnumber, barsinline, lastbarno, firstbarno, line_in_section;

  double lthick=.4,
         flexit, linewidth, parindent, x, spc_factor, elemskip, cor_elemskip,
         beforerule, cor_beforerule, afterrule, cor_afterrule, lastbar,
         eff_linewidth, fill_length, hardlength, softlength, clefskip;

  char name_of_file [FNL], n_o_f [FNL], n_o_logfile[FNL], MusiXFlxVersion[6];

  char *p;

  printf("\n <<< musixflex %s%s >>>\n", VERSION, MYVERSION);
  printf("\n ... decoding command line\n");

  if (argc<2 || argc >3) error_exit(1);

/*********************************
 debugging
 d debug informations on screen
 f debug informations to file .mxl
 s show computed lines on screen
**********************************/

  if (argc==3){
    if (!strcmp("d",argv[2])) dbug=TRUE;
    if (!strcmp("m",argv[2])) dbug_lines=TRUE;
    if (!strcmp("f",argv[2])){dbug=TRUE; dbug_logfile=TRUE; dbug_lines=TRUE;}
    if (!strcmp("s",argv[2])) showresult=TRUE;}

/**************************************
 search for '.', cut string, add '.mx1'
***************************************/

  strcpy (name_of_file, argv[1]);

  p = strrchr(name_of_file, '.');
  if (p && !strchr(p + 1, '/')
#if defined(__MSDOS__) || defined(WIN32)
      && !strchr(p + 1, '\\')
#endif
      )
    *p = '\0';
  strcpy(n_o_f, name_of_file);
  strcat(name_of_file, ".mx1");

/*****************************************************
 Open the .mx1 file containing bar length information.
******************************************************/

  printf(" ... open <%s> for input\n", name_of_file);

  infile = fopen( name_of_file, "r");
  if (!infile){
    printf("\nFile not found: %s\n", name_of_file); exit(3);}

/***********************
 test versionnumber
 and stop if they differ
************************/

  strcpy(MusiXFlxVersion, VERSION);
  strcat(MusiXFlxVersion, "\n");
  printf(" ... testing versionnumber\n");
  ++currentline;
  if (GETLINE && strcmp(linebuf, MusiXFlxVersion)){
    printf("\n!!! Wrong version !!!\n\nMusiXTeX : %s",linebuf);
    printf("musixflx : %s\n",MusiXFlxVersion); error_exit(99);}

/*********************
 Open the output file.
**********************/

  strcpy (n_o_logfile, n_o_f);
  strcat (n_o_f, ".mx2");

  printf(" ... open <%s> for output\n",n_o_f);
  outfile = fopen( n_o_f, "w" );
  if (!outfile) {printf("\nCan't create: %s\n",n_o_f); exit(3);}

/******************
 Open the log file.
*******************/

  if (dbug_logfile){
    strcat (n_o_logfile, ".mxl");

    printf(" ... open <%s> for debugging\n",n_o_logfile);
    logfile = fopen( n_o_logfile, "w" );
    if (!logfile) {printf("\nCan't create: %s\n",n_o_logfile); exit(3);}
    fprintf(logfile,"Version %s", MusiXFlxVersion);}

/*******************
 skip startindicator
********************/

  if (GETLINE && strcmp(linebuf,"S\n")){
    printf("\nCorrupted <%s>\n", name_of_file); error_exit(99);}

/*****************************
 do...while loop for
 moretimes call of \startpiece
 >>>>>>>>>>>>>>>>>>>>>>>>>>>
******************************/

  GETLINE;
  do {

/****************
 reset all arrays
*****************/

  if (dbug) printf("\n------- Chapter %d -------\n\n", chapterno);
  if (dbug_logfile)
    fprintf(logfile, "\n------- Chapter %d -------\n\n", chapterno);

  for (i=0; i<MAX_SIGNS; ++i){
    signchange[i]=junk;
    oldsignskip[i]=0;
    futuresignskip[i]=0; /* jh-1 */
    signskip[i]=0;}

  for (i=0; i<MAX_BARS; ++i){
    hardbarlength[i]=0;
    softbarlength[i]=0;
    width_leftrightrepeat[i]=0;
    lr_repeat[i]=FALSE;
    width_leftrepeat[i]=0;
    xbar[i]=0;  /* jh-1 */
    l_repeat[i]=FALSE;
    zbar[i]=FALSE;
    raggedline[i]=FALSE;
    barno[i]=0;}

  for (i=0; i<MAX_SECTIONS; ++i){
    eff_hardlength[i]=0;
    eff_softlength[i]=0;
    bars[i]=0;
    autolines[i]=FALSE;
    linegoal[i]=0;  /* jh-2 */
    mulooseness[i]=0;}

/*****************************
 Read and decode header items:

 1. Linewidth;
 2. Parindent;
 3. Beforeruleskip;
 4. Afterruleskip;
 5. Elemskip;
 6. Clefskip;
 7. Signskip;
******************************/

           linewidth  = atof(linebuf); ++currentline;
  GETLINE; parindent  = atof(linebuf); ++currentline;
  GETLINE; beforerule = atof(linebuf); ++currentline;
  GETLINE; afterrule  = atof(linebuf); ++currentline;
  GETLINE; elemskip   = atof(linebuf); ++currentline;
  GETLINE; clefskip   = atof(linebuf); ++currentline;
  GETLINE; signskip[0]= atof(linebuf); ++currentline;

  futuresignskip[0] = signskip[0]; /* Initialize for xbar signs. jh-1 */

/******************************************************************
 Read the records specifying contributions to bar length.
 Lengths are of two types:
 (a) Hard or unscaleable, eg barlines, clef or meter changes (typ=0).
 (b) Soft or scaleable, eg noteboxes which scale with \elemskip (typ=1).
 Note that \afterruleskip and \beforerulskip are considered soft.
*******************************************************************/

  jbar=junk;
  i = 0;
  sign = 0;
  xbar_count = 0; /* Used to detect presence of xbars. jh-1 */
  xbar_flag = FALSE; /* True allows check for xbar. jh-1 */
  hardsign = 0; /* accumulator for hard skip from sign change. jh-1 */
  softspace_count = 0; /* Watch for softspace after bars are posted jh-1 */

  all_section = 0;

  printf(" ... reading\n");

  while ( samechapter && GETLINE )
 {
    ++currentline;
    switch (linebuf[0])
  {

/***************
 '\startpiece'
 stop reading,
 compute,
 write
 and start again
****************/

    case 'S':
      samechapter=FALSE;
      break;

/*********************************************************
 End of section. Action:
 Right justify the material ending at the previous bar.
 Read the number following the *, which is the 'looseness'
 parameter of the section just ended.
 Reset the bar test integer to JUNK in case the bar number
 was reset between sections.
***********************************************************/

    case '*':
      mulooseness[all_section]= (int) atol(strchr(linebuf,' '));
	  linegoal[all_section]=0;                          /* jh-2 + WS-1 */
	  sscanf(linebuf+1," %*d %d",linegoal+all_section); /* jh-2 + WS-1 */
      if (mulooseness[all_section] != 0 && linegoal[all_section] > 0 ) { 
       printf(
      "\\linegoal{%d} ignored because \\mulooseness not equal to zero\n"
	  "   for section %d in chapter %d\n",
        linegoal[all_section],all_section+1,chapterno);
        linegoal[all_section] = 0;  /* Reset to ignore it. jh-2 + WS-1 */
       }
      ++all_section;
      if (all_section>(MAX_SECTIONS-1)){
        printf("\nToo many sections, maximum number of sections: %d\n",
                 MAX_SECTIONS); exit(3);}
      jbar=junk;
      break;

/*********************************************************
 autolines. Action:
 Right justify the material ending at the previous bar.
 Set flag.
 Reset the bar test integer to JUNK in case the bar number
 was reset between sections.
***********************************************************/

    case 'a':
      autolines[all_section]=TRUE;
      ++all_section;
      if (all_section>(MAX_SECTIONS-1)){
        printf("\nToo many sections, maximum number of sections: %d\n",
                 MAX_SECTIONS); exit(3);}
      jbar=junk;
      break;

/******************************************************
 found a raggedline, let's set a flag
 I think, I'll hardly get a Nobel-Award for coding this
 What a pity! :-(
 but perhaps a Pulitzer-Award for my comments. :-)
*******************************************************/

    case 'r': raggedline[i+1]=TRUE; break;

/*************************************
 found \zbar, set flag and store barno
**************************************/

    case 'z': zbar[i]=TRUE;
      barno[i]= (int) atol(strrchr(linebuf, ' '));
      ++xbar_count; /* Help detect xbars, track zbar offsets. jh-1 */
      hardsign = 0; /* reset any sign change skip accumulated. jh-1 */
      softspace_count = 0; /* Reset for next bar. jh-1 */
      break;

/*****************************************
 found a leftrightrepeat
 set a flag and store the different widths
******************************************/

    case 'l':
      lr_repeat[i]=TRUE;
/*      printf("l found, i=%d\n",i); */
      width_leftrightrepeat[i]=atof(strchr(linebuf, ' '));
/*      printf("width_leftright=%f\n",width_leftrightrepeat[i]); */
      width_leftrepeat[i]=atof(strrchr(linebuf, ' '));
/*      printf("width_left=%f\n",width_leftrepeat[i]); */
      break;

/**************************
 found a leftrepeat
 set a flag and store width
***************************/

    case 'L':
      l_repeat[i]=TRUE;
      width_leftrepeat[i]=atof(strrchr(linebuf, ' '));
      break;

/***********
 store barno
************/

    case 'b':
      barno[i]= (int) atol(strrchr(linebuf, ' '));
      ++xbar_count;  /* Track possible xbars. jh-1 */
      xbar_flag = TRUE; /* Allow check of next line to detect xbar. jh-1 */
      hardsign = 0; /* Reset accumulated hard sign skip.  jh-1 */
      softspace_count = 0; /* Reset for next bar. jh-1 */
      break;

/**********************************
 enabling the use of 'hard' offsets
 advance current hardwith
 reduce current softwidth
***********************************/

    case 'h':
      x  = atof(strrchr(linebuf, ' '));
      softbarlength[i] -= x;
      hardbarlength[i] += x;
      eff_softlength[all_section] -= x;
      eff_hardlength[all_section] += x;
      break;

/***************************************************************
 This record began with 's' and specifies a key signature change
 store the signskip, s.b.
***************************************************************/

    case 's':  /* Changes to detect xbar and signchange interaction, jh-1 */

     tempholdskip=atof(strchr(linebuf,' ')); /* jh-1 */

     /* We might be in the middle of an xbar setup, and we only want to
       increment the sign pointer if this is the first sign change.  jh-1 */
     if (!(signchange[sign]==i)){ ++sign; /* first time for this bar set jh-1 */
       signchange[sign]=i;
       signskip[sign]=tempholdskip;
       oldsignskip[sign]=hardsign; /* Capture accumulated hard space. jh-1 */
       ++xbar[i];  /* Increment to detect xbars with sign changes. jh-1 */
       /* Housekeeping is done... Now, one more condition to check. If there
        has been any soft space since the bar was declared, then musixtex
        will NOT publish a sign change notice at the end of the line for
        this sign change when there is a line break in this bar.
        In that case, signal that this should be treated as an xbar, which
        will effectively suppress the transfer of hardspace for the sign
        change notice.  jh-1 */
        if (softspace_count > 0) xbar[i]=2; /* Suppress space move jh-1 */
      }
     /* Always update the futuresignskip value in case this is the last */
     futuresignskip[sign]=tempholdskip;

      break;

/* comment */

    case '%': break;

/********************************************************************
 This is an `ordinary' line, listing a contribution to the barlength.
*********************************************************************/

    default:
      if (!isdigit(linebuf[0])){
        printf("\nError in <%s> line %d \n", name_of_file, currentline);
        error_exit(99);}
      bar= (int) atol(linebuf);
      typ= (int) atol(strchr(linebuf, ' '));
      x=atof(strrchr(linebuf, ' '));

      if (typ) eff_softlength[all_section] += x;
      else eff_hardlength[all_section] += x;

/**************************************
 Increment bar number if the bar number
 read from the file has changed.
 Accumulate current bar length.
***************************************/

      if (bar>jbar){
        ++i;
        ++bars[all_section];
          if(i>MAX_BARS){
            printf("\nToo many bars, maximum number of bars: %d\n", MAX_BARS);
            exit(3);}}

     /* At this point, can check if this is an xbar... Only check once. jh-1 */
       if (xbar_flag){
          if ((xbar_count-bar-1)==0){ /* find an xbar.  jh-1 */
          --xbar_count; /* adjust offset to stay on track. */

       /* To handle the special conditions caused by possible xbars and
          sign changes, the xbar logic state has to be examined and changed
          only if this is the first xbar, and not for subsequent ones. jh-1 */
          if (xbar[i]==0){ /* then this is the first xbar in the set or
            there has already been a sign change. jh-1 */
	      xbar[i] = 1; /* jh-1 */
             }
       }}
       xbar_flag = FALSE; /* Reset to prevent checks until next bar. jh-1 */


      if (typ) { softbarlength[i] += x;
         /* Count softspace entries to help in sign/linebreak decisions jh-1 */
          ++softspace_count;
          hardsign = 0; /* safety - just be sure in case line break jh-1 */
         } else { /* jh-1 */
         hardbarlength[i] += x;
         hardsign += x; /* accumulate hardspace, there may be a sign change. */
        }
      jbar=bar;
     } /* eo switch */
   }   /* eo while  */

/**************************************************************
 Decrement the number of sections if the final section is void.
 This will be the usual case where the input file ends with
 an end of section record.
 If this record has been omitted, stop going on to avoid
 'You can't use \raise....'.
***************************************************************/

  printf(" ... compute\n");

  if (!bars[all_section])
    --all_section;
  else
     error_exit(4);

/********************************
 Summarize sectioning information.
*********************************/

  if (dbug){
    printf("\nNumber of sections        : %d\n\n",all_section+1);

    for (section=0; section<=all_section; ++section){
      if (autolines[section]) printf("---- autoline section ----\n");
      printf("Section                   : %d\n",section+1);
      printf("Number of bars in section : %d\n",bars[section]);
      printf("Length(hard) of section %d : %f\n",
              section+1, eff_hardlength[section]);
      printf("Length(soft) of section %d : %f\n",
              section+1, eff_softlength[section]);
      if (linegoal[section] != 0){ /* linegoal applies. jh-2 */
       printf("Section line goal was determined by \\linegoal value...\n");
       printf("Line Goal for section     : %d\n",linegoal[section]);
      } else { /* mulooseness applies. jh-2 */
      printf("Looseness of section      : %d\n",mulooseness[section]);}
      getchar();}}

  if (dbug_logfile){
    fprintf(logfile, "\nNumber of sections        : %d\n\n",
            all_section+1);

    for (section=0; section<=all_section; ++section){
      if (autolines[section])
        fprintf(logfile, "---- autoline section ----\n");
      fprintf(logfile, "Section                   : %d\n",
              section+1);
      fprintf(logfile, "Number of bars in section : %d\n",
              bars[section]);
      fprintf(logfile, "Length(hard) of section %d : %f\n",
              section+1, eff_hardlength[section]);
      fprintf(logfile, "Length(soft) of section %d : %f\n",
              section+1, eff_softlength[section]);
      if (linegoal[section] != 0){ /* line goal applies. jh-2 */
       fprintf(logfile,
       "Section line goal was determined by \\linegoal value...\n");
       fprintf(logfile, "Line Goal for section     : %d\n",linegoal[section]);
      } else { /* mulooseness applies.  jh-2 */
      fprintf(logfile, "Looseness of section      : %d\n",
              mulooseness[section]);}
     }}

/*************************************************
 Loop over the sections defined in the input file.
 Each section must be right justified.
 LAST is the absolute number of the last bar
   in the current section.
**************************************************/

  sign = 0;
  mark = 0;
  lastbarnumber = 0;

  for(section=0; section<=all_section; ++section){
    line_in_section=1;
    lastbarnumber += bars[section];

/*************************************
 Find number of lines to work towards.
**************************************/

    lines=((eff_hardlength[section]+eff_softlength[section]+parindent)/
          (linewidth-(clefskip+signskip[sign])))+.5;
    if (!lines) lines=1; /* safety */

    natural_lines = lines; /* Keep this for debug report. jh-2 */
    lines += mulooseness[section];
    if(mulooseness[section] != 0 && linegoal[section] > 0){ /* jh-2 */
    printf("Unexpected line goal reset occurred for section %d\n", section+1);
      linegoal[section] = 0; /* Zero it, Safety, should not happen. jh-2 */
     }
    if(linegoal[section] > 0) lines = linegoal[section]; /* jh-2 */
    
    if (lines<1){lines=1; printf("Don't stress \\mulooseness to much !!!\n");}

/**************************************
 autolinesflag set in current section ?
 iftrue force number of lines to 1
***************************************/

    if (autolines[section]) lines=1;

    if (dbug){
      printf("Section number           : %d\n", section+1);
      printf("Last bar in this section : %d\n", lastbarnumber);
      printf("Number of bars           : %d\n", bars[section]);
      printf("Natural number of lines  : %d\n", natural_lines); /* jh-2 */
      printf("Chosen  number of lines  : %d\n\n", lines);}

    if (dbug_logfile){
      fprintf(logfile, "Section number           : %d\n", section+1);
      fprintf(logfile, "Last bar in this section : %d\n", lastbarnumber);
      fprintf(logfile, "Number of bars           : %d\n", bars[section]);
      fprintf(logfile, "Natural number of lines  : %d\n",
              natural_lines); /* jh-2 */
      fprintf(logfile, "Chosen  number of lines  : %d\n\n", lines);}

    if (bars[section]<1) error_exit(3);

/**************************************************************
 fill_length is the length of 'bar' material (ie excluding
 signature space) required to fill the remainder
 of the piece. This value will not be exact if there are
 sign changes within the section. However,
 fill_length is used only to keep track of the mean scale factor
 for the remainder of the piece, as opposed to individual lines.

 Loop over lines, working out number of bars
 and revised \elemskip for each line.
 added correct computing of fill_length
****************************************************************/

    for (j=1; j<=lines; ++j, ++line_in_section) {
      ++line_number;
      fill_length=(lines-j+1)*(linewidth-(clefskip+signskip[sign]));

/****************************************************
 Work out mean element skip over remaining bars
 in the current section.
 EFFWID is the effective line width once
 key signature have been written.
 Set parindent to zero after it has been used for the
 first line of the first section.
****************************************************/

      if (!eff_softlength[section]) error_exit(5);
      spc_factor=(fill_length-eff_hardlength[section])/eff_softlength[section];

      if ((xbar[mark+1]>1) && (mark>0)) { 
        /* The bar is an bar+xbar with a sign change. jh-1 */
        eff_linewidth=linewidth-(clefskip+signskip[sign-1])-parindent;
      } else { /* This is a normal bar. jh-1 */
        eff_linewidth=linewidth-(clefskip+signskip[sign])-parindent;
      } /* jh-1 */
      signskip[sign] = futuresignskip[sign]; /* Supports xbar signs, jh-1 */

      parindent=0;

/****************************************************
 Fill the current line by adding bars until overflow.
*****************************************************/

      i=mark;
      firstbarno=barno[mark+1];
      hardlength= 0;
      softlength = 0;
      x = 0;
      lastbar = 0.0;
      detect_end= FALSE;

      while (x<eff_linewidth) {
        if (detect_end) break;
        ++i;

/*********************
 Check for raggedline.
**********************/

        if (raggedline[i]) detectraggedline=TRUE;

/*******************************************
 Check for key signature change at this bar.
********************************************/

        if (i==signchange[sign+1]) ++sign;

        lastbar = hardbarlength[i]+spc_factor*softbarlength[i];
        x += lastbar;

/*********************************************
 Enforce termination at last bar and last line
**********************************************/

        if (i==lastbarnumber) detect_end=TRUE;
        else if (line_in_section==lines){detect_end=FALSE; x=0;}

        hardlength += hardbarlength[i];
        softlength += softbarlength[i];
      }

/************************************************
 If the overhang is less than half the barlength,
 include the latest bar in the line,
 and shrink the line accordingly.
*************************************************/

      if ((x-eff_linewidth)<(lastbar/2)) {
        barsinline=i-mark;
        mark=i;
        lastbarno=barno[mark];

/********************************************
 last bar in line a zbar?
 if true -> add to the first bar in next line
 the amount of afterruleskip
*********************************************/

        if (zbar[mark]) {
          softbarlength[i+1] += afterrule;
          eff_softlength[section] += afterrule;
        }

/********************************************
 last bar in line a leftrightrepeat?
 if true -> reduce hardwidth of current line
            advance the hardwidth of next bar
            advance the softwidth of next bar
*********************************************/

        if (lr_repeat[mark]) {
/*          printf("mark=%d\n",mark);
            printf("width_leftright=%f\n",width_leftrightrepeat[i]);
            printf("width_left=%f\n",width_leftrepeat[i]);   */
          hardlength -= (width_leftrightrepeat[i]-width_leftrepeat[i]);
          eff_hardlength[section] +=
                        (width_leftrightrepeat[i]-width_leftrepeat[i]); 
          hardbarlength[i+1] += width_leftrepeat[i];
          softbarlength[i+1] += afterrule/2;
          eff_softlength[section] += afterrule/2;
          
        }

/********************************************
 last bar in line a leftrepeat?
 if true -> reduce hardwidth of current line
            advance the hardwidth of next bar
            advance the softwidth of next bar
*********************************************/

        if (l_repeat[mark]) {
          hardlength -= (width_leftrepeat[i]-lthick);
          hardbarlength[i+1] += width_leftrepeat[i];
          softbarlength[i+1] += afterrule/2;
          eff_softlength[section] += afterrule/2;
        }

        if (signchange[sign+1]==mark+1) { /* s.b. */
          ++sign;
          /* Because the bar is staying here in the line, we look ahead
             to see if the upcoming bar is a sign change, and adjust space
             to account for the complimentary sign change notice that will
             be posted at the end of this line.  However, if the upcoming
             sign change bar is really a bar+xbar set, where the sign change
             is buried in the xbar, then we don't do the move because the
             change notice really won't be posted in this line.  jh-1 */
          if (xbar[mark+1]<2) { /* okay to do the move.  jh-1 */
            hardlength += oldsignskip[sign];
            hardbarlength[mark+1] -= oldsignskip[sign];
          }
        }
      }

/*********************************************
 Exclude the latest bar, and stretch the line.
**********************************************/

      else {

        barsinline=i-1-mark;
        if (barsinline<1) error_exit(2);
        mark=i-1;
        lastbarno=barno[mark];
        hardlength -= hardbarlength[i];
        softlength -= softbarlength[i];

        if (zbar[mark]) softbarlength[i] += afterrule;

        if (lr_repeat[mark]) {
          hardlength -= (width_leftrightrepeat[i-1]-width_leftrepeat[i-1]);
          eff_hardlength[section] +=
                        (width_leftrightrepeat[i-1]-width_leftrepeat[i-1]);
          hardbarlength[i] += width_leftrepeat[i-1];
          softbarlength[i] += afterrule/2;
          eff_softlength[section] += afterrule/2;
        }

        if (l_repeat[mark]) {
          hardlength -= (width_leftrepeat[i-1]-lthick);
          hardbarlength[i] += width_leftrepeat[i-1];
          softbarlength[i] += afterrule/2;
          eff_softlength[section] += afterrule/2;
        }

/*********************************************************************
 Error (o/u-hbox) occurs only when signature change start in next line
 -> look for signature change in next line
 if true then advance the hardwidth of current line
              reduce next hard barlength by signature change
**********************************************************************/

        if (signchange[sign]==mark+1) {
          /* However, if the next bar is a bar+xbar set where the
             sign change comes from the xbar, then don't do this
             move, because the extra skip is not really there! jh-1 */
          if (xbar[mark+1]<2) { /* alright, do the move.   jh-1 */
            hardlength += oldsignskip[sign];
            hardbarlength[mark+1] -= oldsignskip[sign];
          }
        }
      }

/***********************************************
 Define a flex factor for this line as the ratio
 of soft part of the specified line width,
 to soft width in the approximate line.
************************************************/

      if (!softlength) error_exit(5);
      flexit=(eff_linewidth-hardlength)/softlength;
      if (detectraggedline) {flexit=1; detectraggedline=FALSE;}
      cor_elemskip   = elemskip   * flexit;
      cor_afterrule  = afterrule  * flexit;
      cor_beforerule = beforerule * flexit;

      if (dbug){
        printf("Line number             : %d\n",line_number);
        printf("Fill length             : %f\n",fill_length);
        printf("Effective length        : %f\n",
                eff_softlength[section]+eff_hardlength[section]);
        printf("Mean space factor       : %f\n",spc_factor);
        printf("Bars in line            : %d\n",barsinline);
        printf("Effective linewidth     : %f\n",eff_linewidth);
        printf("Uncorrected hard length : %f\n",hardlength);
        printf("Uncorrected soft length : %f\n",softlength);
        printf("Flex factor (soft)      : %f\n",flexit);
        printf("Corrected elemskip      : %f\n",cor_elemskip);
        printf("Corrected afterrule     : %f\n",cor_afterrule);
        printf("Corrected beforerule    : %f\n",cor_beforerule);
        getchar();}

      if (dbug_logfile){
        fprintf(logfile, "Line number             : %d\n",line_number);
        fprintf(logfile, "Fill length             : %f\n",fill_length);
        fprintf(logfile, "Effective length        : %f\n",
                eff_softlength[section]+eff_hardlength[section]);
        fprintf(logfile, "Mean space factor       : %f\n",spc_factor);
        fprintf(logfile, "Bars in line            : %d\n",barsinline);
        fprintf(logfile, "Effective linewidth     : %f\n",eff_linewidth);
        fprintf(logfile, "Uncorrected hard length : %f\n",hardlength);
        fprintf(logfile, "Uncorrected soft length : %f\n",softlength);
        fprintf(logfile, "Flex factor (soft)      : %f\n",flexit);
        fprintf(logfile, "Corrected elemskip      : %f\n",cor_elemskip);
        fprintf(logfile, "Corrected afterrule     : %f\n",cor_afterrule);
        fprintf(logfile, "Corrected beforerule    : %f\n",cor_beforerule);}

      eff_hardlength[section] -= hardlength;
      eff_softlength[section] -= softlength;
      fill_length -= eff_linewidth;

/**********************************
 Write a record to the output file.
***********************************/

      fprintf( outfile,
        "\\lineset{%3d}{%2d}{%8.5fpt}{%8.5fpt}{%8.5fpt}%% %d - %d\n",
        line_number, barsinline, cor_elemskip, cor_afterrule,
        cor_beforerule, firstbarno, lastbarno);

      if (showresult){
        printf(
        "\\lineset{%3d}{%2d}{%8.5fpt}{%8.5fpt}{%8.5fpt}%% %d - %d\n",
        line_number, barsinline, cor_elemskip, cor_afterrule,
        cor_beforerule, firstbarno, lastbarno);}

      if (dbug_lines) printf(" ... writing line : %d\r", line_number);
      if (dbug_logfile)
        fprintf(logfile, " ... writing line : %d\n", line_number);

    } /*eo for lines */
  } /* eo for sections */

  if (dbug_lines) printf("\n");

/*********
 <<<<<<<<<
 eo do
**********/

 samechapter=TRUE;
 ++chapterno;
 } while(GETLINE);

/*************
 closing files
**************/

  fclose(infile);
  if (dbug_logfile){
    fclose(logfile);
    if (!logfile) error_exit(0);}
  fclose(outfile);
  if (!outfile) error_exit(0);

  printf(" ... thats all, bye\n");
  return(0);
}

/*** eof ***/


