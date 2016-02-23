/* unix-to-macintosh.c  24.2.1996

 * Macintosh THINK-C file system and user interface
 * library for use by dvi2tty, disdvi, and rtf2LaTeX.
 *
 * Written and copyright (c) 1996 by Alex Viskovatoff
 * (av@mixcom.com). Permission is granted to distribute
 * and modify this file.
 */

#include <MacHeaders>
#include <string.h>
#include "macintosh.h"


static char home_pathname_buf[PATHNAME_LENGTH];

static char *make_pathname(char *q,long dir_id, int vol_num)
{
	CInfoPBRec	directory;
	Str31		dir_name;
	int			n;
	directory.dirInfo.ioDrDirID = dir_id;
	directory.dirInfo.ioVRefNum = vol_num;
	directory.dirInfo.ioFDirIndex = -1;
	directory.dirInfo.ioNamePtr = dir_name;
	for (;;directory.dirInfo.ioDrDirID = directory.dirInfo.ioDrParID) {
		*q-- = ':';
		PBGetCatInfoSync(&directory);
		for (n=*dir_name; n; n--) *q-- = (char) dir_name[n];
		if (directory.dirInfo.ioDrDirID == 2) break;
	}
	return q+1;
}	

char *get_home_dir()
{
	FCBPBRec	home_dir;
	Str31		dir_name;
	Str31		apName;
	int			pathRefNum;
	Handle		apParam;
	char 	   *p = home_pathname_buf+PATHNAME_LENGTH-1;
	GetAppParms(&apName,&pathRefNum,&apParam);
	home_dir.ioRefNum = pathRefNum;
	home_dir.ioFCBIndx = 0;
	home_dir.ioNamePtr = dir_name;
	PBGetFCBInfoSync(&home_dir);
	*p-- = 0;
	return make_pathname(p,home_dir.ioFCBParID,home_dir.ioFCBVRefNum);
}

pascal unsigned char not_rtf_file_p(void *parms)
{
	StringPtr name = ((IOParam*)parms)->ioNamePtr;
	char *p, *q;
	static char extension[] = ".rtf";
	int i;
	if (name[0] < 5) return TRUE;
	for (i=4, p = (char*) name + name[0] - 3, q=extension; i; i--)
		if (*p++ != *q++) return TRUE;
	return FALSE;
}

void set_creator(const unsigned char *outfilename)
{
	FInfo	finfo;
	Str255	filename;
	StringHandle h;
	strcpy((char*)filename,(char*)outfilename);
	CtoPstr(filename);
	if (!GetFInfo(filename,0,&finfo)) {
        finfo.fdCreator = *((OSType*)*(h=GetString(128)));
        ReleaseResource(h);
	SetFInfo(filename,0,&finfo);
	}
}



/* The rest of file is a modification of the file ccomand.c (part of
 * Think C 5.0), with the function ccommand renamed to "docommand",
 * and changed to take three more arguments: a filter procedure to use
 * when displaying files in the open input file dialog, the length of
 * a list of signatures of file types that should be displayed in that
 * dialog, and a pointer to this list. Furthermore, the file creator
 * signature for the output file, which is of type 'TEXT', is now set
 * by a call to the new function set_creator. The signature is stored
 * as a string resource, and so can be modified by means of ResEdit.
 * (In the distribution, the signature used is '*TEX', that of Textures.)
 */


/*
 *  ccommand.c
 *
 *  Copyright (c) 1991 Symantec Corporation.  All rights reserved.
 *
 *  This routine should be called from "main" as follows:
 *
 *		main(int argc, char **argv)
 *		{
 *			argc = ccommand(&argv);
 *			...
 *
 *  A dialog is presented allowing entry of command line arguments
 *  and redirection of stdin/stdout.
 *
 */

#include <MacHeaders>

#include "stdlib.h"
#include "errno.h"
#include "console.h"
#include "ansi_private.h"

#define NARGS		25

static char *argv[NARGS+1];
static char argbuf[256];

static void create_dialog(void);
static Handle ditem(int);
static void radio(int, int, int);
static pascal void drawRing(DialogPtr, short);

static Point getWhere = { 90, 82 };
static Point putWhere = { 106, 104 };

static void setfile(char *, SFReply *);

static void init_command_line(char *, char *);
static int parse(char *, char *);

enum {
	cmdLine = 3,
	inCon, inFile,
	outCon, outFile, outEcho, outPrint,
	okRing,
	labelIn, labelOut, labelLine
};

static struct {
	short			count;
	struct {
		Handle			h;
		Rect			box;
		char			kind;
	}				item[13];
} itemList = { 12,

		/*  OK  */
	0, { 176, 115, 196, 175 }, ctrlItem+btnCtrl,

		/*  Cancel  */
	0, { 176, 225, 196, 285 }, ctrlItem+btnCtrl,
	
		/*  command line  */
	0, { 141, 34, 157, 376 }, editText+itemDisable,
	
		/*  (stdin) console  */
	0, { 34, 30, 50, 180 }, ctrlItem+radCtrl,
	
		/*  (stdin) file  */
	0, { 54, 30, 70, 180 }, ctrlItem+radCtrl,
	
		/*  (stdout) console  */
	0, { 34, 230, 50, 380 }, ctrlItem+radCtrl,
	
		/*  (stdout) file  */
	0, { 54, 230, 70, 380 }, ctrlItem+radCtrl,
	
		/*  (stdout) console+file  */
	0, { 74, 230, 90, 380 }, ctrlItem+radCtrl,
	
		/*  (stdout) console+printer  */
	0, { 94, 230, 110, 380 }, ctrlItem+radCtrl,

		/*  ring around OK button  */
	(Handle) drawRing, { 172, 111, 200, 179 }, userItem+itemDisable,
	
		/*  "Standard Input:"  */
	0, { 10, 20, 26, 170 }, statText+itemDisable,
	
		/*  "Standard Output:"  */
	0, { 10, 220, 26, 370 }, statText+itemDisable,
	
		/*  "Command Line:"  */
	0, { 114, 20, 130, 170 }, statText+itemDisable,
};

static DialogPtr dp;
static Rect bounds = { 60, 51, 270, 461 };
static int inChoice = inCon, outChoice = outCon;


/*
 *  ccommand - process "command line"
 *
 */

int
docommand(char ***av, ProcPtr file_filter, int num_types, SFTypeList type_list)
{
	short i, argc;
	SFReply input, output, scratch;
	char buf[256];
	
		/*  present dialog  */
		
	cshow(stdin);
	create_dialog();
	init_command_line(buf, argbuf);
	SetCtlValue(ditem(inCon), 1);
	SetCtlValue(ditem(outCon), 1);
	ShowWindow(dp);
	
		/*  engage in dialog  */
		
	do {
		ModalDialog(0, &i);
		switch (i) {
			case cancel:
				abort();
			case inFile:
				SFGetFile(getWhere, "", file_filter, num_types, type_list, NULL, &scratch);
				if (!scratch.good)
					break;
				input = scratch;
				/* ... */
			case inCon:
				radio(inChoice = i, inCon, 2);
				break;
			case outFile:
			case outEcho:
				SFPutFile(putWhere, "", "", NULL, &scratch);
				if (!scratch.good)
					break;
				output = scratch;
				/* ... */
			case outCon:
			case outPrint:
				radio(outChoice = i, outCon, 4);
				break;
		}
	} while (i != ok);
	
		/*  parse command line  */
		
	GetIText(ditem(cmdLine), argbuf);
	sprintf(buf, "%#s", argbuf);
	argc = parse(buf, argbuf);
	*av = argv;
	DisposDialog(dp);
	
		/*  redirect stdout  */
		
	if (outChoice == outPrint)
		cecho2printer(stdout);
	else if (outChoice != outCon) {
		setfile((char *) scratch.fName, &output);
		if (outChoice == outFile)
			freopen((char *) scratch.fName, "w", stdout);
		else cecho2file((char *) scratch.fName, 0, stdout);
		set_creator(scratch.fName);              /*** Added by av ***/
	}
	
		/*  redirect stdin  */
		
	if (inChoice == inFile) {
		setfile((char *) scratch.fName, &input);
		freopen((char *) scratch.fName, "r", stdin);
	}
	
		/*  done  */
	
	errno = 0;
	return(argc);
}


/* ---------- dialog utilities ---------- */


/*
 *  create_dialog - build dialog in memory
 *
 */

static void
create_dialog(void)
{
	Handle items;
	
	asm {
		lea		itemList,a0
		move.l	#sizeof itemList,d0
		_PtrToHand
		move.l	a0,items
	}
	dp = NewDialog(0, &bounds, "", 0, 1, (Ptr) -1, 0, 0, items);
	SetCTitle(ditem(ok), "\pOK");
	SetCTitle(ditem(cancel), "\pCancel");
	SetCTitle(ditem(inCon), "\pconsole");
	SetCTitle(ditem(inFile), "\pfile");
	SetCTitle(ditem(outCon), "\pconsole");
	SetCTitle(ditem(outFile), "\pfile");
	SetCTitle(ditem(outEcho), "\pconsole+file");
	SetCTitle(ditem(outPrint), "\pconsole+printer");
	SetIText(ditem(labelIn), "\pStandard Input:");
	SetIText(ditem(labelOut), "\pStandard Output:");
	SetIText(ditem(labelLine), "\pCommand Line:");
}


/*
 *  ditem - return item handle
 *
 */

static Handle
ditem(int i)
{
	short kind;
	Handle item;
	Rect box;
	
	GetDItem(dp, i, &kind, &item, &box);
	return(item);
}


/*
 *  radio - adjust a cluster of radio buttons
 *
 */

static void
radio(int i, int j, int n)
{
	for (; n--; j++)
		SetCtlValue(ditem(j), i == j);
}


/*
 *  drawRing - (user-item proc) draw ring around OK button
 *
 */

static pascal void
drawRing(DialogPtr dp, short i)
{
	PenNormal();
	PenSize(3, 3);
	FrameRoundRect(&itemList.item[okRing-1].box, 16, 16);
	PenNormal();
}


/* ---------- file utilities ---------- */


/*
 *  setfile - prepare to open file
 *
 */

static void
setfile(char *buf, SFReply *reply)
{
	IOParam pb;
	
	pb.ioNamePtr = 0;
	pb.ioVRefNum = reply->vRefNum;
	PBSetVolSync(&pb);
	sprintf(buf, "%#s", reply->fName);
}


/* ---------- string utilities ---------- */


/*
 *  init_command_line - prepare initial command line
 *
 *  The command line is preset to show the name of the program.
 *  The program name is quoted as necessary.
 *
 */

static void
init_command_line(char *buf1, char *buf2)
{
	register char *s, *t = buf2;
	int c, space = 0, dquote = 0, squote = 0, quote = 0;
	
	sprintf(s = buf1, "%#s", CurApName);
	while (c = *s++) {
		if (c == ' ')
			space = 1;
		else if (c == '"')
			dquote = 1;
		else if (c == '\'')
			squote = 1;
	}
	if (space || dquote || squote)
		*t++ = quote = dquote && !squote ? '\'' : '"';
	for (s = buf1; c = *s++; *t++ = c) {
		if (c == quote || c == '\\')
			*t++ = '\\';
	}
	if (quote)
		*t++ = quote;
	*t++ = ' ';
	*t++ = 0;
	SetIText(ditem(cmdLine), __c2p(buf2, buf1));
	SelIText(dp, cmdLine, 9999, 9999);
}


/*
 *  parse - divide command line into "words"
 *
 *  Words are delimited by one or more spaces.  Any characters within
 *  matching single (') or double (") quotes are taken literally.  Any
 *  character preceded by a backslash (\) is taken literally.
 *
 */

static int
parse(char *s, char *t)
{
	int c, quote = 0, argc = 0;
		
	while (c = *s++) {
		if (c == ' ')
			continue;
		if (argc < NARGS)
			argv[argc++] = t;
		do {
			if (c == '\\' && *s)
				c = *s++;
			else if (c == '"' || c == '\'') {
				if (!quote) {
					quote = c;
					continue;
				}
				if (c == quote) {
					quote = 0;
					continue;
				}
			}
			*t++ = c;
		} while (*s && ((c = *s++) != ' ' || quote));
		*t++ = 0;
	}
	return(argc);
}
