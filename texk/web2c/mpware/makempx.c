/* $Id$

 Make an MPX file from the labels in a MetaPost source file,
 using mpto and either dvitomp (TeX) or dmp (troff).

 Started from a shell script initially based on John Hobby's original
 version, that was then translated to C by Akira Kakuto (Aug 1997, 
 Aug 2001), and updated and largely rewritten by Taco Hoekwater (Nov 2006).

 Public Domain.
 
 indent -kr -psl
*/

/* Differences between the script and this C version:

 * The script trapped HUP, INT, QUIT and TERM for cleaning up 
   temporary files. This is a refinement, and not portable.

 * The command 'newer' became a function.

 * The script put its own directory in front of the
   executable search PATH. This is not portable either, and
   it seems a safe bet that normal users do not have 'mpto', 
   'dvitomp', or 'dmp' commands in their path.  

 * The command-line '-troff' now also accepts an optional argument.

 * The troff infile for error diagnostics is renamed "mpxerr.i", 
   not plain "mpxerr".

 * The original script deleted mpx*.* in the cleanup process. 

   That is a bit harder in C, because it requires reading the contents 
   of the current directory.  The current program assumes that 
   opendir(), readdir() and closedir() are known everywhere where 
   the function getcwd() exists (except on WIN32, where it uses
   _findfirst & co).

   If this assumption is false, you can define NO_GETCWD, and makempx
   will revert to trying to delete only a few known extensions

 * There is a -debug switch, preventing the removal of tmp files
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#include <process.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#if !defined(HAVE_MKSTEMP) && ! defined (HAVE_MKTEMP)
#include "time.h"
#endif

#include <kpathsea/kpathsea.h>
#include "c-auto.h"

#define INLINE_NEWER 1

/* We may have high-res timers in struct stat.  If we do, use them.  */
#ifdef HAVE_STRUCT_STAT_ST_MTIM
#define ISNEWER(S,T) (S.st_mtim.tv_sec > T.st_mtim.tv_sec ||              \
                    (S.st_mtim.tv_sec  == T.st_mtim.tv_sec &&           \
                     S.st_mtim.tv_nsec >= T.st_mtim.tv_nsec))
#else
#define ISNEWER(S,T) (S.st_mtime >= T.st_mtime)
#endif

#ifdef WIN32
#define nuldev "nul"
#define ACCESS_MODE 4
#define DUP _dup
#define DUPP _dup2
#define uexit exit
#define GETCWD _getcwd
#else
#define nuldev "/dev/null"
#define ACCESS_MODE R_OK
#define DUP dup
#define DUPP dup2
#define uexit _exit
#define GETCWD getcwd
#endif

#define version "1.002"

#define ERRLOG "mpxerr.log"
#define TEXERR "mpxerr.tex"
#define DVIERR "mpxerr.dvi"
#define TROFF_INERR "mpxerr.i"
#define TROFF_OUTERR "mpxerr.t"
#define DMP "dmp"
#define DVITOMP "dvitomp"
#define NEWER "newer"
#define MPTO "mpto"
#define MPTOTEXOPT "-tex"
#define MPTOTROPT  "-troff"
#define TROFF   "soelim | eqn -Tps -d$$ | troff -Tps"
#define TEX     "tex"

#define LNAM 255
#define SNAM 128
#define COMMAND_PARTS 10

#define ARGUMENT_IS(a) (!strncmp(av[curarg],(a),strlen((a))))

#define TMPNAME_EXT(a,b) { strcpy(a,tmpname); strcat(a,b); }

#define RENAME(a,b) { if (debug) fprintf(stderr,"%s: renaming %s to %s\n",progname,a,b); \
                      rename(a,b); }

#define split_command(a,b) do_split_command(a,b,' ')

#define split_pipes(a,b) do_split_command(a,b,'|')

/* Define 'getcwd' for systems that do not implement it */

#ifdef NO_GETCWD
#define GETCWD(p,s)	NULL
#endif


/* Temporary files */
char mp_i[SNAM], mp_t[SNAM], mp_tmp[SNAM], mp_log[SNAM];

char *mp_tex = NULL;

int debug = 0;

char *progname;


void
mess(char *progname)
{
    fprintf(stderr, "Try `%s --help' for more information.\n", progname);
    uexit(1);
}

void 
default_erasetmp(void) 
{
    char wrk[SNAM];
    char *p;
    strcpy(wrk, mp_tex);
    p = strrchr(wrk, '.');
    *p = '\0';  strcat(wrk, ".aux");   remove(wrk);
    *p = '\0';  strcat(wrk, ".pdf");   remove(wrk);
    *p = '\0';  strcat(wrk, ".toc");   remove(wrk);
    *p = '\0';  strcat(wrk, ".idx");   remove(wrk);
    *p = '\0';  strcat(wrk, ".ent");   remove(wrk);
    *p = '\0';  strcat(wrk, ".out");   remove(wrk);
    *p = '\0';  strcat(wrk, ".nav");   remove(wrk);
    *p = '\0';  strcat(wrk, ".snm");   remove(wrk);
    *p = '\0';  strcat(wrk, ".tui");   remove(wrk);
}

void
erasetmp(void)
{
    char wrk[SNAM];
    char *p;
    char cur_path[1024];
#ifdef _WIN32
    struct _finddata_t c_file;
    long hFile;
#else
    struct dirent *entry;
    DIR *d;
#endif
    if (debug)
	return;

    if (mp_tex != NULL) {
	remove(mp_tex);
	strcpy(wrk, mp_tex);
	if ((p = strrchr(wrk, '.'))) {
	    *p = '\0';
	    /* now wrk is identical to tmpname */
	    if(GETCWD(cur_path,1020) == NULL) {
	      /* don't know where we are, back to old behaviour */
	      default_erasetmp();
	    } else {
#ifdef _WIN32
	      strcat(cur_path,"/*");
	      if ((hFile = _findfirst (cur_path, &c_file)) == -1L) {
		default_erasetmp();
	      } else {
		if (strstr(c_file.name,wrk)==c_file.name) 
		  remove(c_file.name);
		while (_findnext (hFile, &c_file) != -1L) {
		  if (strstr(c_file.name,wrk)==c_file.name) 
		    remove(c_file.name);
		}
		_findclose (hFile); /* no more entries => close directory */
	      }
#else
	      if ((d = opendir(cur_path)) == NULL) {
		default_erasetmp();
	      } else {
		while ((entry = readdir (d)) != NULL) {
		  if (strstr(entry->d_name,wrk)==entry->d_name) 
		    remove(entry->d_name);
		}
		closedir(d);
	      }
#endif	    
	    }
	}
    }
    if (mp_i[0]) {
	remove(mp_i);
    }
    if (mp_t[0]) {
	remove(mp_t);
    }
    if (mp_tmp[0]) {
	remove(mp_tmp);
    }
    if (mp_log[0]) {
	remove(mp_log);
    }
}

FILE *
makempx_xfopen(char *name, char *mode)
{
    FILE *ret;
    if (!(ret = fopen(name, mode))) {
	if (*mode == 'r') {
	    fprintf(stderr, "Cannot open %s for reading.\n", name);
	} else {
	    fprintf(stderr, "Cannot open %s for writing.\n", name);
	}
	erasetmp();
	uexit(1);
    }
    return ret;
}


int
run_command(int count, char **cmdl)
{
    int i;
    char *cmd;
    int retcode;
#ifndef WIN32
    pid_t child;
#endif
    char **options = NULL;
    /* return non-zero by default, signalling an error */
    retcode = -1;

    if (count < 1 || cmdl == NULL || cmdl[0] == NULL)
	return retcode;

    options = xmalloc(sizeof(char *) * (count + 1));
    cmd = xstrdup(cmdl[0]);

    for (i = 0; i < count; i++)
	options[i] = cmdl[i];
    options[count] = NULL;
    if (debug) {
	fprintf(stderr, "%s: running command", progname);
	for (i = 0; i < count; i++)
	    fprintf(stderr, " %s", options[i]);
	fprintf(stderr, "\n");
    }
#ifndef WIN32
    {
	child = fork();
	if (child >= 0) {
	    if (child == 0) {
		execvp(cmd, options);
	    } else
		wait(&retcode);
	}
	free(cmd);
	free(options);
    }
    return WEXITSTATUS(retcode);
#else
    retcode = spawnvp(P_WAIT, cmd, options);
    free(cmd);
    free(options);
    return retcode;
#endif
}

int
do_split_command(char *maincmd, char **cmdline, char target)
{
    char *piece;
    char *cmd;
    unsigned int i;
    int ret = 0;
    int in_string = 0;
    if (strlen(maincmd) == 0)
	return 0;
    cmd = xstrdup(maincmd);
    i = 0;
    while (cmd[i] == ' ')
	i++;
    piece = cmd;
    for (; i <= strlen(maincmd); i++) {
	if (in_string == 1) {
	    if (cmd[i] == '"') {
		in_string = 0;
	    }
	} else if (in_string == 2) {
	    if (cmd[i] == '\'') {
		in_string = 0;
	    }
	} else {
	    if (cmd[i] == '"') {
		in_string = 1;
	    } else if (cmd[i] == '\'') {
		in_string = 2;
	    } else if (cmd[i] == target) {
		cmd[i] = 0;
		if (ret == COMMAND_PARTS) {
		    fprintf(stderr,
			    "%s: Executable command too complex.\n",
			    progname);
		    mess(progname);
		}
		cmdline[ret++] = xstrdup(piece);
		while (i < strlen(maincmd) && cmd[(i + 1)] == ' ')
		    i++;
		piece = cmd + i + 1;
	    }
	}
    }
    if (*piece) {
	if (ret == COMMAND_PARTS) {
	    fprintf(stderr, "%s: Executable command too complex.\n",
		    progname);
	    mess(progname);
	}
	cmdline[ret++] = xstrdup(piece);
    }
    return ret;
}


int
do_newer(char *source, char *target)
{
#ifndef INLINE_NEWER
    char *cmdline[] = { NEWER, source, target };
    return run_command(3, cmdline);
#else
    struct stat source_stat, target_stat;

    /* check the target file */
    if (stat(target, &target_stat) < 0)
	return 0;		/* true */

    /* check the source file */
    if (stat(source, &source_stat) < 0)
	return 1;		/* false */

    if (ISNEWER(source_stat, target_stat))
	return 0;

    return 1;
#endif
}

#define MBUF 512

void
prepare_tex(char *texname, char *tmpname)
{
    int i;
    FILE *fr, *fw;
    char buffer[MBUF];
    char *tmpfile = NULL;
    char *texkeep = NULL;	/* for debugging */
    char *mptexpre = NULL;

    mptexpre = kpse_var_value("MPTEXPRE");

    if (!mptexpre)
	mptexpre = xstrdup("mptexpre.tex");

    if (!access(mptexpre, ACCESS_MODE)) {
	tmpfile = concat(tmpname, ".tmp");
	fr = makempx_xfopen(mptexpre, "r");
	fw = makempx_xfopen(tmpfile, "wb");

	while ((i = fread(buffer, 1, MBUF, fr)))
	    fwrite(buffer, 1, i, fw);
	fclose(fr);
	fr = makempx_xfopen(texname, "r");
	while ((i = fread(buffer, 1, MBUF, fr)))
	    fwrite(buffer, 1, i, fw);
	fclose(fr);
	fclose(fw);
	if (debug) {
	    texkeep = concat(texname, ".keep");
	    RENAME(texname, texkeep);
	} else {
	    remove(texname);
	}
	RENAME(tmpfile, texname);
    }
}

int
main(int ac, char **av)
{
    char **cmdline, **cmdbits;
    char whatever_to_mpx[SNAM], infile[SNAM], inerror[SNAM];
    int retcode, i, sav_o, sav_e, sav_i;
    FILE *fr, *fw, *few, *fnulr, *fnulw;

    /* TeX command name */
    char maincmd[(LNAM + 1)];

    int mpmode = 0;
    char tmpname[] = "mpXXXXXX";

    char *mpfile = NULL;
    char *mpxfile = NULL;
    char *tmpstring = NULL;

    int cmdlength = 1;
    int cmdbitlength = 1;

    int curarg = 0;

    cmdline = xmalloc(sizeof(char *) * COMMAND_PARTS);
    cmdbits = xmalloc(sizeof(char *) * COMMAND_PARTS);
    for (i = 0; i < COMMAND_PARTS; i++) {
	cmdline[i] = NULL;
	cmdbits[i] = NULL;
    }

    progname = av[0];

    /* Initialize buffer for temporary file names */
    for (i = 0; i < SNAM; i++) {
	mp_i[i] = '\0';
	mp_t[i] = '\0';
	mp_tmp[i] = '\0';
	mp_log[i] = '\0';
    }

    kpse_set_progname(progname);

    /* Set TeX command */
    if ((tmpstring = kpse_var_value("TEX")))
	strncpy(maincmd, tmpstring, LNAM);
    else if ((tmpstring = kpse_var_value("MPXMAINCMD")))
	strncpy(maincmd, tmpstring, LNAM);
    else
	strncpy(maincmd, TEX, LNAM);
    if (tmpstring)
	free(tmpstring);

    strncat(maincmd, " --parse-first-line --interaction=nonstopmode",
	    (LNAM - strlen(maincmd)));

    while (curarg < (ac - 1)) {

	curarg++;

	if (ARGUMENT_IS("-help") || ARGUMENT_IS("--help")) {

	    fprintf(stderr,
		    "Usage: %s [-tex[=<program>]|-troff[=<program>]] MPFILE MPXFILE.\n",
		    av[0]);
	    fputs
		("  If MPXFILE is older than MPFILE, translate the labels from the MetaPost\n"
		 "  input file MPFILE to low-level commands in MPXFILE, by running\n"
		 "    " MPTO " " MPTOTEXOPT ", " TEX ", and " DVITOMP "\n"
		 "  by default; or, if -troff is specified,\n" "    " MPTO
		 " " MPTOTROPT ", " TROFF ", and " DMP ".\n\n"
		 "  The current directory is used for writing temporary files.  Errors are\n"
		 "  left in mpxerr.{tex,log,dvi}.\n\n"
		 "  If the file named in $MPTEXPRE (mptexpre.tex by default) exists, it is\n"
		 "  prepended to the output in tex mode.\n\n"
		 "Email bug reports to metapost@tug.org.\n", stderr);
	    uexit(0);

	} else if (ARGUMENT_IS("-version") || ARGUMENT_IS("--version")) {

	    fprintf(stdout, "%s %s\n", progname, version);
	    fputs
		("There is NO warranty. This program is in the public domain.\n"
		 "Original author: John Hobby.\n"
		 "Current maintainer: Taco Hoekwater.\n", stdout);
	    uexit(0);

	} else if (ARGUMENT_IS("-debug") || ARGUMENT_IS("--debug")) {

	    debug = 1;

	} else if (ARGUMENT_IS("-troff") || ARGUMENT_IS("--troff")) {

	    mpmode = 1;

	    if (ARGUMENT_IS("-troff=") || ARGUMENT_IS("--troff=")) {
		i = 7;
		if (*(av[curarg] + i) == '=')
		    i++;
		if (*(av[curarg] + i) == '\'' || *(av[curarg] + i) == '\"') {
		    strncpy(maincmd, av[curarg] + i + 1, LNAM);
		    *(maincmd + strlen(maincmd)) = 0;
		} else {
		    strncpy(maincmd, av[curarg] + i, LNAM);
		}
	    } else {
		strncpy(maincmd, TROFF, LNAM);
	    }

	} else if (ARGUMENT_IS("-tex") || ARGUMENT_IS("--tex")) {

	    mpmode = 0;

	    if (ARGUMENT_IS("-tex=") || ARGUMENT_IS("--tex=")) {
		i = 5;
		if (*(av[curarg] + i) == '=')
		    i++;
		if (*(av[curarg] + i) == '\'' || *(av[curarg] + i) == '\"') {
		    strncpy(maincmd, av[curarg] + i + 1, LNAM);
		    *(maincmd + strlen(maincmd)) = 0;
		} else {
		    strncpy(maincmd, av[curarg] + i, LNAM);
		}
	    }

	} else if (ARGUMENT_IS("-")) {
	    fprintf(stderr, "%s: Invalid option: %s.\n", progname,
		    av[curarg]);
	    mess(progname);
	} else {
	    if (mpfile == NULL) {

		if (strchr(av[curarg], ' ') && (av[curarg][0] != '\"'))
		    mpfile = concat3("\"", av[curarg], "\"");
		else
		    mpfile = strdup(av[curarg]);

	    } else if (mpxfile == NULL) {

		if (strchr(av[curarg], ' ') && (av[curarg][0] != '\"'))
		    mpxfile = concat3("\"", av[curarg], "\"");
		else
		    mpxfile = strdup(av[curarg]);

	    } else {
		fprintf(stderr, "%s: Extra argument %s.\n", progname,
			av[curarg]);
		mess(progname);
	    }
	}
    }

    if (mpfile == NULL || mpxfile == NULL) {
	fprintf(stderr, "%s: Need exactly two file arguments.\n",
		progname);
	mess(progname);
    }

    /* Check if mpfile is newer than mpxfile */

    retcode = do_newer(mpfile, mpxfile);

    /* If MPX file is up-to-date or if MP file does not exist, do nothing. */
    if (retcode)
	uexit(0);

#ifdef HAVE_MKSTEMP
    i = mkstemp(tmpname);
    if (i == -1)
	uexit(1);
    close(i);
    remove(tmpname);
#else
#ifdef HAVE_MKTEMP
    tmpstring = mktemp(tmpname);
    if ((tmpstring == NULL) || strlen(tmpname) == 0)
	uexit(1);
    /* this should not really be needed, but better
       safe than sorry. */
    if (tmpstring != tmpname) {
	i = strlen(tmpstring);
	if (i > 8) i = 8;
	strncpy(tmpname, tmpstring, i);
    }
#else
    sprintf(tmpname, "mp%06d", (time(NULL) % 1000000));
#endif
#endif

    mp_tex = concat(tmpname, ".tex");

    /* step 1: */

    fw = makempx_xfopen(mp_tex, "wb");
    few = makempx_xfopen(ERRLOG, "wb");

    cmdline[0] = MPTO;
    cmdline[1] = (mpmode == 0 ? MPTOTEXOPT : MPTOTROPT);
    cmdline[2] = mpfile;

    sav_o = DUP(fileno(stdout));
    DUPP(fileno(fw), fileno(stdout));
    sav_e = DUP(fileno(stderr));
    DUPP(fileno(few), fileno(stderr));

    retcode = run_command(3, cmdline);

    DUPP(sav_o, fileno(stdout));
    close(sav_o);
    fclose(fw);

    DUPP(sav_e, fileno(stderr));
    close(sav_e);
    fclose(few);

    if (retcode) {
	fprintf(stderr, "%s: Command failed: %s %s %s\n",
		progname, cmdline[0], cmdline[1], cmdline[2]);
	erasetmp();
	uexit(1);
    }

    /* step 2: */

    if (mpmode == 0) {		/* TeX mode */

	prepare_tex(mp_tex, tmpname);	/* check for mptexpre.tex */

	if (strlen(maincmd) > (LNAM - strlen(mp_tex) - 1)) {
	    fprintf(stderr, "%s: Command too complex: %s\n", progname,
		    maincmd);
	    erasetmp();
	    uexit(1);
	}
	strcat(maincmd, " ");
	strcat(maincmd, mp_tex);
	cmdlength = split_command(maincmd, cmdline);

	fnulr = makempx_xfopen(nuldev, "r");
	sav_i = DUP(fileno(stdin));
	DUPP(fileno(fnulr), fileno(stdin));

	fnulw = makempx_xfopen(nuldev, "w");
	sav_o = DUP(fileno(stdout));
	DUPP(fileno(fnulw), fileno(stdout));

	retcode = run_command(cmdlength, cmdline);

	DUPP(sav_i, fileno(stdin));
	close(sav_i);
	fclose(fnulr);
	DUPP(sav_o, fileno(stdout));
	close(sav_o);
	fclose(fnulw);

	TMPNAME_EXT(mp_log, ".log");

	if (!retcode) {
	    strcpy(whatever_to_mpx, DVITOMP);
	    TMPNAME_EXT(infile, ".dvi");
	    strcpy(inerror, DVIERR);
	} else {
	    RENAME(mp_tex, TEXERR);
	    RENAME(mp_log, ERRLOG);
	    fprintf(stderr, "%s: Command failed:", progname);
	    for (i = 0; i < (cmdlength - 1); i++)
		fprintf(stderr, " %s", cmdline[i]);
	    fprintf(stderr, " %s", TEXERR);
	    fprintf(stderr, "; see mpxerr.log\n");
	    erasetmp();
	    uexit(2);
	}
    } else if (mpmode == 1) {	/* troff mode */

	TMPNAME_EXT(mp_i, ".i");
	RENAME(mp_tex, mp_i);
	TMPNAME_EXT(mp_t, ".t");
	fr = makempx_xfopen(mp_i, "r");
	TMPNAME_EXT(mp_tmp, ".tmp");
	fw = makempx_xfopen(mp_tmp, "wb");

	/* split the command in bits */
	cmdbitlength = split_pipes(maincmd, cmdbits);

	for (i = 0; i < cmdbitlength; i++) {
	    cmdlength = split_command(cmdbits[i], cmdline);

	    sav_i = DUP(fileno(stdin));
	    sav_o = DUP(fileno(stdout));
	    DUPP(fileno(fr), fileno(stdin));
	    DUPP(fileno(fw), fileno(stdout));

	    retcode = run_command(cmdlength, cmdline);

	    DUPP(sav_i, fileno(stdin));
	    close(sav_i);
	    DUPP(sav_o, fileno(stdout));
	    close(sav_o);
	    fclose(fr);
	    fclose(fw);
	    if (retcode) {
		RENAME(mp_i, TROFF_INERR);

		fprintf(stderr, "%s: Command failed:", progname);
		for (i = 0; i < cmdlength; i++) {
		    if (strcmp(cmdline[i], mp_i) == 0)
			fprintf(stderr, " %s", TROFF_INERR);
		    else
			fprintf(stderr, " %s", cmdline[i]);
		}
		fprintf(stderr, "\n");
		erasetmp();
		uexit(2);
	    }
	    if (i < cmdbitlength - 1) {
		if (i % 2 == 0) {
		    fr = makempx_xfopen(mp_tmp, "r");
		    fw = makempx_xfopen(mp_t, "wb");
		    strcpy(infile, mp_t);
		} else {
		    fr = makempx_xfopen(mp_t, "r");
		    fw = makempx_xfopen(mp_tmp, "wb");
		    strcpy(infile, mp_tmp);
		}
	    }
	}
	strcpy(whatever_to_mpx, DMP);
	strcpy(inerror, TROFF_OUTERR);
    }

    /* Step 3: */

    fw = makempx_xfopen(ERRLOG, "wb");

    sav_o = DUP(fileno(stdout));
    DUPP(fileno(fw), fileno(stdout));

    cmdline[0] = whatever_to_mpx;
    cmdline[1] = infile;
    cmdline[2] = mpxfile;
    retcode = run_command(3, cmdline);

    DUPP(sav_o, fileno(stdout));
    close(sav_o);
    fclose(fw);

    if (retcode) {
	RENAME(infile, inerror);
	if (mpmode == 1)
	    RENAME(mp_i, TROFF_INERR);
	if (!debug)
	    remove(mpxfile);
	fprintf(stderr, "%s: Command failed: %s %s %s\n",
		progname, whatever_to_mpx, inerror, mpxfile);
	erasetmp();
	uexit(3);
    }
    if (!debug) {
	remove(ERRLOG);
	remove(infile);
    }
    erasetmp();
    uexit(0);
}
