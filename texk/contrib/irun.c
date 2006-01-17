/* link.c

Copyright (C) 1997 - 2003
Fabrice Popineau  <Fabrice.Popineau@supelec.fr>

This file is part of Web2c for Win32.

Web2c For Win32 is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation; either version 2, or 
(at your option) any later version. 
   
Web2c For Win32 is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
   
You should have received a copy of the GNU General Public License
along with Web2c For Win32; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.  */

/*
  the goal of this file is to mimic Unix's ln command. This is a quick
  hack, in that it will only allow to remap to predefined .exe files.
*/

#include <kpathsea/config.h>
#include <kpathsea/lib.h>
#include <kpathsea/proginit.h>
#include <kpathsea/progname.h>
#include <kpathsea/tex-file.h>

#define IS_DIRSEP(c) ((c) == '/' || (c) == '\\')

/* To be determined by 'dumpbin /symbols irun.exe' 
   web2c/Makefile automagically determines this value by making two passes
   when compiling irun.exe
*/

typedef void (* PFN_kpse_set_program_name)(const_string, const_string);
typedef void (* PFN_kpse_init_prog)(const_string, unsigned, const_string, const_string);
typedef string (* PFN_kpse_find_file)(const_string, kpse_file_format_type, boolean);

#ifndef RDATA
#include "irun.h"
#endif

#pragma section("FOOBAR", read)

#define marker "foobar"

#undef DEBUG

/* Initialized with marker string */
    __declspec(allocate("FOOBAR")) const char newargv0[MAX_PATH] = marker;

/*
  This programme should emulate Unix's ln for .exe files under Win32.

  Usage: irun <src> <dest>

  Method used: this .exe file is located, copied to the destination
  using the new name for the destination, while changing the newargv0
  constant to the src file name. 

  Caveat: to be portable, the src file name should not be absolute. 
  It should be either a pathname relative to the current executable file,
  either a simple name which may be looked for along the PATH, or only 
  in the same directory as the current executable file.

  Adopted strategy: relative pathes are ok, absolute one will give a 
  warning when doing the link.

*/

#define DIR_SEP '/'
#define DIR_SEP_STRING "/"
#define IS_DEVICE_SEP(ch) ((ch) == ':')
#define NAME_BEGINS_WITH_DEVICE(name) (*(name) && IS_DEVICE_SEP((name)[1]))
/* On DOS, it's good to allow both \ and / between directories.  */
#define IS_DIR_SEP(ch) ((ch) == '/' || (ch) == '\\')
/* On win32, UNC names are authorized */
#define IS_UNC_NAME(name) (strlen(name)>=3 && IS_DIR_SEP(*name)  \
                            && IS_DIR_SEP(*(name+1)) && isalnum(*(name+2)))

char cmdline[MAX_PATH];
char autoloc[MAX_PATH];
char *progname;

BOOL WINAPI catch_interrupt(DWORD arg)
{
    switch (arg) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
	return TRUE;
    default:
	/* No need to set interrupt as we are exiting anyway */
	return FALSE;
    }
}

#if 0
static BOOL
is_absolute(const char *filename)
{
    BOOL absolute = IS_DIR_SEP (*filename)
	/* Novell allows non-alphanumeric drive letters. */
	|| (*filename && IS_DEVICE_SEP (filename[1]))
	/* UNC names */
	|| IS_UNC_NAME(filename)
	;
    BOOL explicit_relative = (*filename == '.' && (IS_DIR_SEP (filename[1])
						   || (filename[1] == '.' && IS_DIR_SEP (filename[2]))));
    /* FIXME: On UNIX an IS_DIR_SEP of any but the last character in the name
       implies relative.  */
    return absolute || explicit_relative;
}
#else
is_absolute(const char *path)
{
    return IS_DIRSEP(*path)
	|| ( isalpha(*path) && *(path+1) == ':' && IS_DIRSEP(*path) );
}
#endif

is_relative(const char *path)
{
    return (*path == '.' && 
	    (IS_DIRSEP(*(path+1))
	     || (*(path+1) == '.' && IS_DIRSEP(*(path+2)))));
}

char *look_for_target(char *name, char *cmdline)
{
    char name_drive[_MAX_DRIVE], name_dir[_MAX_DIR], name_fname[_MAX_FNAME], name_ext[_MAX_EXT];
    char *cmd = NULL, *script = NULL, *exepath;
    char *cmdarg = NULL;

    _splitpath(name, 0, 0, name_fname, name_ext);
#ifdef DEBUG
    fprintf(stderr, "name = %s\n", name);
#endif
    if (_stricmp(name_ext, ".exe") == 0) {
	if (is_relative(name)) {
	    /* In this case, catenate the directory of the invoked program
	       with the relative name stored in the link */
	    char *unquoted_cmd;
	    _splitpath(_pgmptr, name_drive, name_dir, 0, 0); 
	    unquoted_cmd = concatn(name_drive, name_dir, name, NULL);
	    cmd = quote_filename(unquoted_cmd);
	    free(unquoted_cmd);
	    /* Do not forget to se the new name to the result, used below */
	    name = xdirsimplify(cmd);
	}
	else {
	    cmd = name;
	}
    }
    else {
	if (_stricmp(name_ext, ".pl") == 0) {
	    cmd = "perl.exe";
	}
	else if (_stricmp(name_ext, ".py") == 0) {
	    cmd = "python.exe";
	}
	else if (_stricmp(name_ext, ".rb") == 0) {
	    cmd = "ruby.exe";
	}
	else if (_stricmp(name_ext, ".java") == 0) {
	    cmd = "java.exe";
	}
	else if (_stricmp(name_ext, ".jar") == 0) {
	    cmd = "java.exe";
	    cmdarg = " -cp ";
	}
	else {
	    fprintf (stderr, "Unknown file type: %s%s\n", name_fname, name_ext);
	    return NULL;
	}
    }

    if (look_for_cmd(cmd, &exepath) == FALSE) {
	fprintf (stderr, "File not found: %s\n", cmd);
	return NULL;
    }

    {
	char *quoted_exepath = quote_filename(exepath);
	strcpy(cmdline, quoted_exepath);
#ifdef DEBUG
	fprintf(stderr, "quoted_exepath = %s\n", quoted_exepath);
#endif
	free(exepath);
	free(quoted_exepath);
    }

    if (cmd != name) {
	/* This is a script, need to find it */
	if (is_absolute(name)) {
	    script = name;
	}
	else {
	    /* Let's try kpathsea if available ! */
	    HMODULE hKPSEDll = 0;
	    PFN_kpse_set_program_name pkpsedll_set_program_name = NULL;
	    PFN_kpse_init_prog pkpsedll_init_prog = NULL;
	    PFN_kpse_find_file pkpsedll_find_file = NULL;
	    char *progname = xstrdup(cmd);
	    unsigned int dpi = 600;
	    char *p = strrchr(progname, '.');
	    if (p) *p = '\0';
#ifdef DEBUG
	    fprintf(stderr, "progname = %s\n", progname);
#endif
	    if ((hKPSEDll = LoadLibrary(KPATHSEA_DLL)) != NULL) {
		
		pkpsedll_set_program_name = (PFN_kpse_set_program_name)GetProcAddress(hKPSEDll, "@kpse_set_program_name@8");
		pkpsedll_init_prog = (PFN_kpse_init_prog)GetProcAddress(hKPSEDll, "@kpse_init_prog@16");
		pkpsedll_find_file = (PFN_kpse_find_file)GetProcAddress(hKPSEDll, "@kpse_find_file@12");
		if (pkpsedll_set_program_name && pkpsedll_init_prog && pkpsedll_find_file) {
		    (*pkpsedll_set_program_name)("irun", progname);
		    (*pkpsedll_init_prog)(uppercasify (kpse_program_name), dpi, NULL, NULL);
		    script = (*pkpsedll_find_file)(name, kpse_texmfscripts_format, TRUE);
		}
		FreeLibrary(hKPSEDll);
	    }
	    if (! script) {
		fprintf(stderr, "irun: no %s script\nof name %s can be found.\nAborting.\n",
			cmd, name);
		*cmdline = '\0';
	    }
	    free(progname);
	}
    }

    if (script) {
	char *quoted_script = quote_filename(script);
	if (cmdarg)
	    strcat(cmdline, cmdarg);
	else
	    strcat(cmdline, " ");
	strcat(cmdline, quoted_script);
	strcat(cmdline, " ");
	free (quoted_script);
	free (script);
    }

#ifdef DEBUG
    fprintf(stderr, "cmdline = %s\n", cmdline);
#endif
    return cmdline;
}

int link(char *src, char *dest, int force, int verbose, char *args[], int argc)
{
    int fdest;
    int cmdlen;
    
#ifdef DEBUG
    if (1)
#else
	if (verbose)
#endif
	    fprintf(stdout, "Linking %s to %s.\n", src, dest);

    if (is_absolute(src))
	fprintf(stderr, "Warning: source file %s is absolute!\n", src);

    /* Looking for the source. */
    if (SearchPath(NULL, "irun", ".exe", MAX_PATH, autoloc,
		   NULL) == 0) {
	fprintf(stderr, "irun: can't find the program.\n");
	exit(1);
    }

#ifdef DEBUG
    fprintf(stderr, "Autoloc = %s\n", autoloc);
#endif

    if (CopyFile(autoloc, dest, !force) == 0) {
	fprintf(stderr, "Can't copy to %s (file may already exist).\n",
		dest);
	exit(1);
    }

    /* Don't look for target now: it will be looked for at run time */
    strcpy(cmdline, src);
    cmdlen = strlen(cmdline);

    /* if there are arguments left on the command line, catenate them */
    while (argc--) {
	strcpy(cmdline+cmdlen+1, *args);
	cmdlen += strlen(*args) + 1;
#ifdef DEBUG
	fprintf(stderr, "argc = %d, cmdlen = %d, args = %s\n", argc, cmdlen, *args);
#endif
	args++;
    }
    cmdline[cmdlen+1] = '\0';

#ifdef DEBUG
    fprintf(stderr, "Cmdline = %s\n", cmdline);
#endif

    /* Now we need to put cmdline into the newargv0 variable in dest
       file.
       We could use the PE file format specfication to locate the .rdata
       section , but is it actually needed :-)
       In fact, we need to find the marker string "foobarbaz" and store
       the name here.
    */
    /* Open the file */
    if ((fdest = _open(dest, _O_RDWR | _O_BINARY)) == 0) {
	perror("Can't open target file");
	exit(1);
    }
    /* Look for the marker string */
    if (_lseek(fdest, RDATA, SEEK_SET) < 0) {
	perror("Can't seek to .rdata");
	exit(1);
    }

    if (_read(fdest, autoloc, MAX_PATH) < 0) {
	perror("Can't read into dest file");
	exit(1);
    }
    if (strcmp(autoloc, newargv0) != 0) {
	fprintf(stderr, "irun: marker string not found in %s.\n", dest);
	exit(1);
    }
    
    if (_lseek(fdest, RDATA, SEEK_SET) < 0) {
	perror("Can't seek (2) to .rdata");
	exit(1);
    }
    
    if (_write(fdest, cmdline, cmdlen+1) < 1) {
	perror("Can't write into dest file");
	exit(1);
    }

    /* Close the file */
    _close(fdest);

    return 0;
}

char *usage_msg = 
	"Usage: %s [-f] [-v] <src> <dest>\nwill link <src> to <dest>\n"
	"Option -f will force the link, -v is verbose.\n"
	"	If src is absolute, a warning is emitted because the hard link\n"
	"	is not portable.\n"
	"	If src is a relative path, it is relative to the dest path.\n"
	"	If src has one of the .java, .jar, .pl, .py or .rb extension,\n"
	"	it is assumed to be a script, and the executable linked to is\n"
	"	java.exe (for .java and .jar), perl.exe, python.exe and ruby.exe.\n"
	"	Those programs must be in accessible through your PATH.\n"
	"	If the script is not found, kpathsea is used to locate it through\n"
	"	the $JAVAINPUTS, $PERLINPUTS, $PYTHONINPUTS and $RUBYINPUTS variables.\n";

void usage()
{
    fprintf(stderr, usage_msg, progname);
    exit(1);
}

int
main (int argc, char *argv[])

{
    int rc;
    size_t j = 0;
    int force = 0, verbose = 0;

    for (progname = argv[0] + strlen(argv[0]) -1;
	 progname > argv[0] && *(progname-1) != '/' && *(progname-1) != '\\';
	 progname--);
#ifdef DEBUG
    fprintf(stderr, "progname = %s\n", progname);
#endif

#ifdef DEBUG
    fprintf(stderr, "argc = %d, argv[0] = %s\n", argc, argv[0]);
#endif

    if (strcasecmp(progname, "irun.exe") == 0
	|| strcasecmp(progname, "irun") == 0) {
	/* Doing the link */
    
	if (argc == 1)
	    usage();

	/* First, parse arguments */
	while (--argc > 0 && *(++argv)[0] == '-') {
#ifdef DEBUG
	    fprintf(stderr, "arg: %s\n", argv[0]);
#endif
	    switch (*(argv[0]+1) == '-' ? *(argv[0]+2) : *(argv[0]+1)) {
	    case 'f':
		force = 1;
		break;
	    case 'v':
		verbose = 1;
		break;
	    case 'h':
		usage();
		exit(0);
	    }
	}

	if (argc < 2)
	    usage();

	link(argv[0], argv[1], force, verbose, argv+2, argc - 2);
    }
    else {
	/* Calling the target program. Unfortunately, exec does not seem
	   to work under NT as expected. */

	char *newargv[256];
	boolean quoted = false;
	char *p, *q, *newcmd, *new_line, *new_argv_i;
	int i, newargc = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	SetConsoleCtrlHandler(catch_interrupt, TRUE);

	/* Looking for the target. */
	/* Either it is an executable or a (Perl|Python|Ruby|Java) script */
	/* If it is a script and kpathsea is available, then it should be 
	   searched for */
	if (look_for_target(newargv0, autoloc) == NULL) {
	    fprintf(stderr, "%s: can't find the target %s.\n",
		    progname, newargv0);
	    exit(1);
	}

	p = q = autoloc;
#ifdef DEBUG
	fprintf(stderr, "autoloc = %s\n", autoloc);
#endif
	/* Fix up in the case of a script */
	do {
	    if (*p == '"') {
		quoted = !quoted;
		p++;
	    }
	    
	    if (*p == ' ' && !quoted) {
		*p = '\0';
		newargv[newargc++] = unquote_filename(q);
		q = ++p;
	    }
	    else if (*p == '\0') {
		newargv[newargc++] = unquote_filename(q);
		break;
	    }
	    else {
		p++;
	    }
	} while (1);
#ifdef DEBUG
	fprintf(stderr, "stage1\n");
	for (i = 0; i < newargc; i++) {
	    fprintf(stderr, "newargv[%d] = %s\n", i, newargv[i]);
	}
#endif
	
	/* Add any argument */
	p = newargv0 + strlen(newargv0) + 1;
	while (*p) {
	    newargv[newargc++] = p;
	    p += strlen(p) + 1;
	}
#ifdef DEBUG
	fprintf(stderr, "stage2\n");
	for (i = 0; i < newargc; i++) {
	    fprintf(stderr, "newargv[%d] = %s\n", i, newargv[i]);
	}
#endif
	
	if (*p) {
	    newargv[newargc++] = unquote_filename(p);
	}
#ifdef DEBUG
	fprintf(stderr, "stage3\n");
	for (i = 0; i < newargc; i++) {
	    fprintf(stderr, "newargv[%d] = %s\n", i, newargv[i]);
	}
#endif
	
	for (i = 1; i < argc; i++) {
	    newargv[newargc++] = argv[i];
	}
	newargv[newargc] = NULL;
#ifdef DEBUG
	fprintf(stderr, "stage4\n");
	for (i = 0; i < newargc; i++) {
	    fprintf(stderr, "newargv[%d] = %s\n", i, newargv[i]);
	}
#endif
	
#if 1
	newcmd = quote_filename(_pgmptr);
	for (i = 1; newargv[i]; i++) {
	    new_argv_i = quote_filename(newargv[i]);
	    new_line = concat3(newcmd, " ", new_argv_i);
	    free(newcmd);
	    free(new_argv_i);
	    newcmd = new_line;
	}
	
	ZeroMemory( &si, sizeof(STARTUPINFO) );
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW ;
	si.wShowWindow = 0 ;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	
#ifdef DEBUG
	fprintf(stderr, "argv0 = %s, newargv0 = %s\n", _pgmptr, newargv[0]);
#endif
	/* creating child process */
	if (CreateProcess(newargv[0],	/* pointer to name of executable module */
			  newcmd,	/* pointer to command line string */
			  NULL,	/* pointer to process security attributes */
			  NULL,	/* pointer to thread security attributes */
			  TRUE,	/* handle inheritance flag */
			  0,		/* creation flags */
			  NULL,	/* pointer to environment */
			  NULL,	/* pointer to current directory */
			  &si,	/* pointer to STARTUPINFO */
			  &pi		/* pointer to PROCESS_INFORMATION */
		) == FALSE) {
	    fprintf(stderr, "irun: CreateProcess %x\n", GetLastError());
	    return NULL;
	}
	
	/* Only the process handle is needed, ignore errors */
	CloseHandle(pi.hThread);
	
	/* Waiting for the process to terminate */
	if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0) {
	    fprintf(stderr, "irun: error, process still active\n");
	    rc = -1;
	}
	
	/* retrieving the exit code */
	if (GetExitCodeProcess(pi.hProcess, &rc) == 0) {
	    fprintf(stderr, "irun: can't get process exit code\n");
	    rc = -1;
	}
#else
	
	rc = _spawnvp (_P_WAIT, newargv[0], newargv);
#endif      
	if (rc < 0) {
	    perror (cmdline);
	    return (1);
	}
	return (rc);
    }
    return 0;
}

/* link.c ends here */
