/*
 *  ChkTeX v1.5, operating system specific code for ChkTeX.
 *  Copyright (C) 1995-96 Jens T. Berger Thielemann
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Contact the author at:
 *		Jens Berger
 *		Spektrumvn. 4
 *		N-0666 Oslo
 *		Norway
 *		E-mail: <jensthi@ifi.uio.no>
 *
 *
 */


/*
 * Some functions which have to be made different from OS to OS,
 * unfortunately...:\
 *
 */

#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#include <kpathsea/expand.h>
#include <kpathsea/tex-file.h>
#endif

#include "ChkTeX.h"
#include "OpSys.h"
#include "Utility.h"

#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#ifdef HAVE_STAT_H
#  include <stat.h>
#endif

#if HAVE_DIRENT_H
#  include <dirent.h>
#  define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#  define dirent direct
#  define NAMLEN(dirent) (dirent)->d_namlen
#  if HAVE_SYS_NDIR_H
#    include <sys/ndir.h>
#  endif
#  if HAVE_SYS_DIR_H
#    include <sys/dir.h>
#  endif
#  if HAVE_NDIR_H
#    include <ndir.h>
#  endif
#endif

#if defined(HAVE_OPENDIR) && defined(HAVE_CLOSEDIR) && \
    defined(HAVE_READDIR) && defined(HAVE_STAT) && \
    defined(S_IFDIR) && defined(SLASH)
#  define USE_RECURSE 1
#else
#  define USE_RECURSE 0
#endif

#if defined(HAVE_LIBTERMCAP) || defined(HAVE_LIBTERMLIB)
#  define USE_TERMCAP 1
#endif


#ifdef USE_TERMCAP
#  ifdef HAVE_TERMCAP_H
#    include <termcap.h>
#  elif HAVE_TERMLIB_H
#    include <termlib.h>
#  else
int tgetent (char *BUFFER, char *TERMTYPE);
char *tgetstr (char *NAME, char **AREA);
#  endif
static char term_buffer[2048];
#endif

#ifdef AMIGA
#  define __USE_SYSBASE
#  include <dos.h>
#  include <dos/dos.h>
#  include <dos/dosasl.h>
#  include <proto/dos.h>
#  include <proto/exec.h>
#  include <signal.h>
#endif

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */


#ifdef AMIGA
const char
	VersString [] = "$VER: ChkTeX 1.5 " __AMIGADATE__
	" Copyright (c) 1995-96 Jens T. Berger Thielemann "
        "<jensthi@ifi.uio.no>",
	__stdiowin [] = "CON:0/10/640/180/ChkTeX",
	__stdiov37 [] = "/AUTO/CLOSE/WAIT";

const ULONG __MemPoolPuddleSize = 16384,
            __MemPoolThreshSize = 8192;

static struct AnchorPath	*AnchorPath = NULL;

static void	KillAnchorPath(void);
static STRPTR	InitAnchorPath(STRPTR String);
#  define V37 (DOSBase->dl_lib.lib_Version > 36)
#endif


/*
 * This is the name of the global resource file.
 */

#ifndef DATADIR
#  ifdef AMIGA
#    define DATADIR  "S:"
#  elif defined(__unix__)
#    define DATADIR "/usr/local/lib/"
#  elif defined(__MSDOS__)
#    define DATADIR "\\emtex\\data\\"
#  else
#    define DATADIR
#  endif
#endif
#define RCBASENAME              "chktexrc"

#ifdef __MSDOS__
#  define LOCALRCFILE             RCBASENAME
#else
#  define LOCALRCFILE             "." RCBASENAME
#endif

TEXT   ConfigFile [BUFSIZ] = LOCALRCFILE;
STRPTR ReverseOn, ReverseOff;


static BOOL HasFile(STRPTR Dir, const STRPTR Filename, const STRPTR App);

#if USE_RECURSE
static BOOL SearchFile(STRPTR Dir, const STRPTR Filename, const STRPTR App);
#endif /* USE_RECURSE */

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */


/*
 * Modify this one to suit your needs. In any case, it should fill
 * the ConfigFile (sized BUFLEN) buffer above with full name & path
 * for the configuration file. The macro RCFILE will give you the
 * filename part of the file, if you need that.
 *
 * Note: This routine will be called several times. Your mission will
 * be to look in each location, and see whether a .chktexrc file exist
 * there.
 *
 * If you choose to do nothing, only the current directory will be
 * searched.
 *
 */

enum LookIn
{
  liMin,
  liSysDir,
  liUsrDir,
  liEnvir,
  liCurDir,
  liNFound,
  liMax
};


BOOL SetupVars(void)
{
    STRPTR Env;
#ifdef __MSDOS__
    STRPTR Ptr;
#endif
    static enum LookIn i = liMin;
    static BOOL FoundFile;

    while(++i < liMax)
    {
	switch(i)
	{
	case liCurDir:  /* Current directory */
	    strcpy(ConfigFile, LOCALRCFILE);
	    break;
	case liEnvir:  /* Environment defined */
#ifdef __MSDOS__
	    if((Env = getenv("CHKTEXRC")) ||
	       (Env = getenv("CHKTEX_HOME")))
#else
	    if((Env = getenv("CHKTEXRC")))
#endif
	    {
		strcpy(ConfigFile, Env);
		tackon(ConfigFile, LOCALRCFILE);
	    }
	    else
#ifdef __MSDOS__
            if((Env = getenv("EMTEXDIR")))
            {
                strcpy(ConfigFile, Env);
                tackon(ConfigFile, "data");
                tackon(ConfigFile, LOCALRCFILE);
            }
            else
#endif
		*ConfigFile = 0;
	    break;
	case liUsrDir: /* User dir for resource files */
#ifdef KPATHSEA
	  {
	    char *f = concat("~/", LOCALRCFILE);
	    char *p = kpse_path_expand(f);
	    if (p) {
	      strcpy(ConfigFile, p);
	      free(p);
	    }
	    else {
	      *ConfigFile = 0;
	    }
	    free(f);
	  }
#else /* ! KPATHSEA */
#ifdef AMIGA
            if(V37)
		strcpy(ConfigFile, "ENV:");
            else
		strcpy(ConfigFile, "S:");

	    tackon(ConfigFile, LOCALRCFILE);
#elif defined(__unix__)
	    if((Env = getenv("HOME")) ||
	       (Env = getenv("LOGDIR")))
	    {
		strcpy(ConfigFile, Env);
		tackon(ConfigFile, LOCALRCFILE);
	    }
	    else
		*ConfigFile = 0;
#elif defined(__MSDOS__)
            strcpy(ConfigFile, PrgName);
            if((Ptr = strrchr(ConfigFile, '\\')) ||
	       (Ptr = strchr(ConfigFile, ':')))
                strcpy(++Ptr, RCBASENAME);
            else
                *ConfigFile = 0;
#endif
#endif /* KPATHSEA */
	    break;
	case liSysDir: /* System dir for resource files */
#ifdef KPATHSEA
	  {
	    char *f = kpse_find_file(LOCALRCFILE, kpse_program_text_format, TRUE);
	    if (f) {
	      strcpy(ConfigFile, f);
	    }
	    else {
	      *ConfigFile = '\0';
	    }
	  }
#else /* ! KPATHSEA */
#if defined(__unix__) || defined(__MSDOS__)
	  strcpy(ConfigFile, DATADIR);
	  tackon(ConfigFile, RCBASENAME);
#else
	    *ConfigFile = 0;
#endif
#endif /* KPATHSEA */
	  break;
	case liNFound:
	case liMin:
	case liMax:
	  *ConfigFile = 0;
	  if(!FoundFile)
	    PrintPrgErr(pmNoRsrc);
	}

	if(*ConfigFile && fexists(ConfigFile))
	    break;
    }
    FoundFile |= *ConfigFile;

    return(*ConfigFile);
}

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */

/*
 * This function should initialize the global variables ReverseOn
 * and ReverseOff to magic cookies, which when printed, makes the
 * text in between stand out.
 */

void SetupTerm(void)
{
#ifdef USE_TERMCAP
    char *termtype = getenv("TERM");
    int success;
    char *buffer;

    ifn(termtype)
	PrintPrgErr(pmSpecifyTerm);

    success = tgetent(term_buffer, termtype);
    if(success < 0)
	PrintPrgErr(pmNoTermData);
    if(success == 0)
	PrintPrgErr(pmNoTermDefd);

    ifn((buffer = (char *) malloc(strlen(term_buffer))) &&
        (ReverseOn = tgetstr("so", &buffer)) &&
	(ReverseOff = tgetstr("se", &buffer)))
#endif
    {
	ReverseOn = PRE_ERROR_STR;
	ReverseOff = POST_ERROR_STR;
    }

}

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */

/*
 * Concatenates the `File' string to the `Dir' string, leaving the result
 * in the `Dir' buffer. Takes care of inserting `directory' characters;
 * if we've got the strings "/usr/foo" and "bar", we'll get
 * "/usr/foo/bar".
 *
 * Behaviour somewhat controlled by the macros SLASH and DIRCHARS in the
 * OpSys.h file.
 *
 */

void tackon(STRPTR Dir, const STRPTR File)
{
    int         EndC;
    ULONG       SLen;

    if(Dir && (SLen = strlen(Dir)))
    {
        EndC = Dir[SLen -1];
        ifn(strchr(DIRCHARS, EndC))
        {
            Dir[SLen++] = SLASH;
            Dir[SLen  ] = 0L;
        }
    }

    strcat(Dir, File);
}

/*
 * This function should add the appendix App to the filename Name.
 * If the resulting filename gets too long due to this, it may 
 * overwrite the old appendix.
 *
 * Name may be assumed to be a legal filename under your OS.
 *
 * The appendix should contain a leading dot.
 */

void AddAppendix(STRPTR Name, const STRPTR App)
{
#ifdef __MSDOS__
    STRPTR p;

    if((p = strrchr(Name, '.')))
        strcpy(p, App);
    else
        strcat(Name, App);
#else
    /*
     * NOTE! This may fail if your system has a claustrophobic file 
     * name length limit.
     */
    strcat(Name, App);
#endif

}

/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */


/*
 * Locates a file, given a wordlist containing paths. If a
 * dir ends in a double SLASH, we'll search it recursively.
 *
 * We assume that
 *   a) a deeper level in the dir. tree. has a longer path than
 *      one above.
 *   b) adding a level doesn't change any of the previous levels.
 *
 * If this function returns TRUE, Dest is guaranteed to contain
 * path & name of the found file.
 *
 * FALSE indicates that the file was not found; Dest is then
 * unspecified.
 */

#ifdef KPATHSEA
BOOL LocateFile(const STRPTR Filename,    /* File to search for */
                STRPTR Dest,              /* Where to put final file */
                const STRPTR App,         /* Extra optional appendix */
                struct WordList *wl)      /* Not used with kpathsea.
                                           */
{
  BOOL ret;
  char *fn, *p;

  fn = Filename;
  p = kpse_find_file(fn, kpse_tex_format, FALSE);

  if (! p || ! *p) {
      fn = concat(Filename, App);
      p = kpse_find_file(fn, kpse_tex_format, FALSE);
      free(fn);
  }

  if (p) {
    strcpy(Dest, p);
    free(p);
    ret = TRUE;
  }
  else {
    fprintf(stderr, "%s: warning, file %s%s not found.\n", kpse_program_name, Filename, App);
    ret = FALSE;
  }

  return ret;
}
#else
BOOL LocateFile(const STRPTR Filename,    /* File to search for */
                STRPTR Dest,              /* Where to put final file */
                const STRPTR App,         /* Extra optional appendix */
                struct WordList *wl)      /* List of paths, entries
                                           * ending in // will be recursed
                                           */
{
    ULONG i, Len;

    FORWL(i, *wl)
    {
        strcpy(Dest, wl->Stack.Data[i]);

#if USE_RECURSE
        Len = strlen(Dest);

        if(Len && (Dest[Len - 1] == SLASH) && (Dest[Len - 2] == SLASH))
        {
            Dest[Len - 1] = Dest[Len - 2] = 0;
            if(SearchFile(Dest, Filename, App))
                return(TRUE);
        }
        else
#endif /* USE_RECURSE */
        {
            if(HasFile(Dest, Filename, App))
                return(TRUE);
        }
    }
    return(FALSE);
}
#endif

static BOOL HasFile(STRPTR Dir, const STRPTR Filename, const STRPTR App)
{
    int DirLen = strlen(Dir);

    tackon(Dir, Filename);
    if(fexists(Dir))
        return(TRUE);

    if(App)
    {
        AddAppendix(Dir, App);
        if(fexists(Dir))
            return(TRUE);
    }

    Dir[DirLen] = 0;
    return(FALSE);

}


#if USE_RECURSE
static BOOL SearchFile(STRPTR Dir, const STRPTR Filename, const STRPTR App)
{
    struct stat *statbuf;
    struct dirent *de;
    DIR *dh;

    int DirLen = strlen(Dir);
    BOOL Found = FALSE;

    DEBUG(("Searching %s for %s\n", Dir, Filename));

    if(HasFile(Dir, Filename, App))
        return(TRUE);
    else
    {
        if((statbuf = malloc(sizeof(struct stat))))
        {
            if((dh = opendir(Dir)))
            {
                while(!Found && (de = readdir(dh)))
                {
                    Dir[DirLen] = 0;
		    if(strcmp(de->d_name, ".") && strcmp(de->d_name, ".."))
		    {
			tackon(Dir, de->d_name);

			if(!stat(Dir, statbuf))
			{
			    if((statbuf->st_mode & S_IFMT) == S_IFDIR)
				Found = SearchFile(Dir, Filename, App);
			}
		    }
                }
                closedir(dh);
            }
            else
                PrintPrgErr(pmNoOpenDir, Dir);
            free(statbuf);
        }
    }
    return(Found);
}
#endif /* USE_RECURSE */



/*  -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=- -=><=-  */

/*
 * This function should be called first with String being a file pattern.
 * It should then return the first filename matching that pattern.
 * Subsequent calls will be made with String = NULL; it should then return
 * any other filenames that matches the same pattern.
 * If the first call is made with NULL, you should return NULL.
 * It may be called more than once with String != NULL, however, you need
 * only to keep track of the last invocation.
 *
 * If a string doesn't match any pattern at all, return the string itself.
 *
 * Don't assume that the String passed will live on until the next
 * invocation, though. strdup() it if you need it after the first time.
 */

#ifdef AMIGA

STRPTR MatchFileName(STRPTR String)
{
    STRPTR	Retval = NULL;

    if(AnchorPath || String)	/* Is this the first invocation? */
    {
	if(V37)
	{
	    ifn(AnchorPath)
	    {
		if(AnchorPath = malloc(sizeof(struct AnchorPath) + FMSIZE))
		{
		    Retval = InitAnchorPath(String);
		    atexit(&KillAnchorPath);
		}
	    }
	    else
	    {
		if(String)
		{
		    MatchEnd(AnchorPath);
		    Retval = InitAnchorPath(String);
		}
		else
		{
		    ifn(MatchNext(AnchorPath))
			Retval = AnchorPath->ap_Buf;
		}
	    }
	}
	else		/* ARP support may be added in the future */
	    Retval = String;

	if(AnchorPath && AnchorPath->ap_FoundBreak)
	    raise(SIGINT);
    }

    if(!Retval && String)
	Retval = String;

    return(Retval);
}

static void KillAnchorPath(void)
{
    MatchEnd(AnchorPath);
}

static STRPTR InitAnchorPath(STRPTR String)
{
    STRPTR	Retval = NULL;

    AnchorPath->ap_BreakBits = SIGBREAKF_CTRL_C;
    AnchorPath->ap_Strlen = FMSIZE;
    AnchorPath->ap_FoundBreak = FALSE;
    AnchorPath->ap_Flags = 0L;

    ifn(MatchFirst(String, AnchorPath))
	Retval = AnchorPath->ap_Buf;
    else
	PrintPrgErr(pmNoFileMatch, String);

    AnchorPath->ap_Flags &= ~(APF_DODIR);
    return(Retval);
}

#else
STRPTR MatchFileName(STRPTR String)
{
    return(String);
}
#endif
