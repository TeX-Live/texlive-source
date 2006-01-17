/* pdfdde.c -- control Adobe Acrobat under Windows.  Written by Fabrice
   Popineau many years ago, based on Adobe documentation.  Public
   domain.  */
   
// #include <afx.h>
#include <win32lib.h>
#include <ddeml.h>
#include <dde.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shellapi.h>

#include <getopt.h>

#define ACROBAT_EXE	"acrobat.exe"
#define ACROBAT		"acroview"
#define EVAL_TOPIC	"control"
#define RESULT_ITEM	"Result"
#define ERROR_ITEM	"Error"

/* Timeouts & delays */
#define CONNECT_DELAY		500		/* ms */
#define TRANSACTION_TIMEOUT	5000		/* ms */
#define MAX_INPUT_IDLE_WAIT     INFINITE	/* ms */

static DWORD DDEsession;
static HCONV DDEconversation;

/* The raw base name */
static char *argv0 = NULL;
static int nPage = 0;
static char *sFileName = NULL;
static int bAll = 0;
static int bServer = 0;
static int iDebug = 0;
static char *sGoto = NULL;
static char sLongFileName[MAXPATHLEN];

const char *pdfdde_version_string = "0.2";

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) (_stricmp(long_options[option_index].name, a) == 0)
#define PROGRAM_IS(a) (_stricmp(a, argv0) == 0)

static struct option long_options [] = {
    { "debug",               0, 0, 0},
    { "help",                0, 0, 0},
    { "version",             0, 0, 0},
    { "file",                1, 0, 0},
    { "page",                1, 0, 0},
    { "goto",                1, 0, 0},
    { "all",                 0, 0, 0},
    {0, 0, 0, 0}
};


char *FindAcrobatModule()
{
    char buf[260];
    char path[260];
    char *program;
    HANDLE hFile;
    HINSTANCE hInst;
 
    program = (char *)malloc(MAX_PATH*sizeof(char));
    if (program == NULL) goto exit_1;

    buf[0] = path[0] = program[0] = '\0';
    
    if (GetTempPath(sizeof(path), path) != 0) {
	strcpy(buf, path);
    }


    strcat(buf, "dummy.pdf");
    
    hFile = CreateFile(buf, 
		       GENERIC_READ|GENERIC_WRITE, 
		       FILE_SHARE_READ | FILE_SHARE_WRITE, 
		       NULL,
		       OPEN_ALWAYS, 
		       FILE_ATTRIBUTE_NORMAL, 
		       NULL);
    
    CloseHandle(hFile);
    
    hInst = FindExecutable("dummy.pdf", path, program);
    
    DeleteFile(buf);
    
#if 0
    switch ((int)hInst) {
    case SE_ERR_FNF:
	fprintf(stdout, "File not found: %s\n", argv[1]);
	break;
    case SE_ERR_NOASSOC:
	fprintf(stdout, "No association for %s\n", argv[1]);
	break;
    default:
	fprintf(stdout, "File .pdf is opened by %s\n", program);
	break;
    }
#endif

    if (*program == '\0' || (GetFileAttributes(program) == 0xFFFFFFFF)) {
	program = NULL;
    }

exit_1:
    return program;

}

void CloseConversation (void)
{
    if (DDEconversation)
	DdeDisconnect(DDEconversation);
    if (DDEsession)
	DdeUninitialize(DDEsession);
}

void error(LPCTSTR fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);

    /* Tidy up */
    CloseConversation();
    exit(1);
}

void *emalloc(size_t len)
{
    void *p = malloc(len);

    if (p == NULL)
	error("Out of memory - cannot allocate %d bytes", len);

    return p;
}

int outstanding_requests;

HDDEDATA CALLBACK
Callback (UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2,
	  HDDEDATA hdata, DWORD dwData1, DWORD dwData2)
{
    if (uType == XTYP_ADVDATA) {
	DWORD len = DdeGetData(hdata, NULL, 0, 0);
	char *buf = _alloca(len + 1);
	DdeGetData(hdata, (LPBYTE)buf, len + 1, 0);
	
	if (--outstanding_requests == 0)
	    PostQuitMessage(0);
	
	return (HDDEDATA) DDE_FACK;
    }
    
    return (HDDEDATA) NULL;
}

int StartServer(LPCTSTR prog)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    /* To be sure ... */
    if (! prog) 
	return 1;

    ZeroMemory (&si, sizeof (si));
    si.cb = sizeof (si);
    if (!CreateProcess (NULL, (LPTSTR)prog, NULL, NULL, FALSE, 0,
			NULL, NULL, &si, &pi))
    {
	error("Could not start process %s", prog);
	return 1;
    }

    /* Wait for the process to enter an idle state */
    WaitForInputIdle (pi.hProcess, MAX_INPUT_IDLE_WAIT);

    /* Close the handles */
    CloseHandle (pi.hThread);
    CloseHandle (pi.hProcess);
    return 0;
}

HSZ DDEString(LPCTSTR str)
{
    HSZ hsz = DdeCreateStringHandle(DDEsession, str, CP_WINANSI);
    if (hsz == 0)
	error("Cannot create string for %s", str);
    return hsz;
}

FreeString(HSZ hsz)
{
    DdeFreeStringHandle(DDEsession, hsz);
}

void OpenConversation(LPCTSTR topic_name)
{
    UINT ret;
    HSZ service;
    HSZ topic;
    int n;
    
    ret = DdeInitialize(&DDEsession, Callback, APPCMD_CLIENTONLY, 0);
    if (ret != DMLERR_NO_ERROR)
	error("Cannot start DDE");

    service = DDEString(ACROBAT);
    topic = DDEString(topic_name);
    DDEconversation = DdeConnect(DDEsession, service, topic, 0);

    if (DDEconversation == 0)
    {
	if (StartServer(FindAcrobatModule()) == 0)
	{
	    /* Try to connect */
	    for (n = 0; n < 5; n++)
	    {
		Sleep (CONNECT_DELAY);
		DDEconversation = DdeConnect(DDEsession, service, topic, 0);
		if (DDEconversation)
		    break;
	    }
	    if (DDEconversation == 0)
		error("Cannot contact server %s", ACROBAT);
	}
    }
    FreeString(service);
    FreeString(topic);
}

void __cdecl ExecuteCommand(LPCTSTR command, ...)
{
    HDDEDATA ret;
    va_list args;
    int len;
    char *buffer;
    
    va_start( args, command );
    len = _vscprintf(command, args) // _vscprintf doesn't count
	+ 1;			// terminating '\0'
    buffer = malloc( len * sizeof(char) );
    vsprintf( buffer, command, args );
    
    ret = DdeClientTransaction((LPBYTE)buffer, strlen(buffer) + 1,
			       DDEconversation, 0, 0, XTYP_EXECUTE,
			       TRANSACTION_TIMEOUT, 0);
    if (ret == 0)
	error("Cannot execute command \"%s\" (error %ld)", command, DdeGetLastError(DDEsession));
}

void StartHotLink(LPCTSTR item_name)
{
    HSZ item = DDEString(item_name);
    DdeClientTransaction(0, 0, DDEconversation, item, CF_TEXT, XTYP_ADVSTART,
			 TRANSACTION_TIMEOUT, 0);
    FreeString(item);
}

LPTSTR RequestData (LPCTSTR item_name)
{
    HSZ item = DDEString(item_name);
    DWORD len;
    char *buf;
    HDDEDATA result;
    result = DdeClientTransaction (NULL, 0, DDEconversation, item, CF_TEXT,
				   XTYP_REQUEST, TRANSACTION_TIMEOUT, 0);
    FreeString(item);
    if (result == 0)
	error("Cannot request item %s\n", item_name);

    len = DdeGetData(result, NULL, 0, 0);
    buf = emalloc(len + 1);
    DdeGetData(result, (LPBYTE)buf, len + 1, 0);
    return buf;
}

void usage()
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "\tpdfopen  --file <filename> [--page <page>]\n");
  fprintf(stderr, "\t\t       Opens <filename> (at page <page> if available)\n");
  fprintf(stderr, "\t\t             in Acrobat or Acrobat Reader.\n");
  fprintf(stderr, "\tpdfclose --file <filename>\n");
  fprintf(stderr, "\t         Closes <filename> in Acrobat or Acrobat Reader.\n");
  fprintf(stderr, "\tpdfclose --all\n");
  fprintf(stderr, "\t         Closes all documents in Acrobat or Acrobat Reader.\n");
  fprintf(stderr, "\tpdfdde\n");
  fprintf(stderr, "\t\t       Read commands on stdin\n");
  fprintf(stderr, "Beware: only documents opened by `pdfopen' can be closed\n");
  fprintf(stderr, "        by `pdfclose'.\n");
  exit(0);
}

char *CheckFileName(char *p)
{
  char sTmp[MAXPATHLEN];
  char *fp;

  sLongFileName[0] = '\0';

  if (GetFileAttributes(p) == -1) {
    fprintf(stderr, "%s: non existent file %s\n", argv0, p);
    return NULL;
  }
  if (GetFullPathName(p, sizeof(sTmp), sTmp, &fp) == 0) {
    fprintf(stderr, "%s: failed to get full path name for %s (Error %d)\n", argv0, p, GetLastError());
    return NULL;
  }
  if (GetLongPathName(sTmp, sLongFileName, sizeof(sLongFileName)) == 0) {
    fprintf(stderr, "%s: failed to get long path name for %s (Error %d)\n", argv0, sTmp, GetLastError());
    return NULL;
  }
  return sLongFileName;
}

int pdfopen()
{
  if (sLongFileName[0] == '\0') {
    fprintf(stderr, "%s: mandatory `--file' argument not found.\n", argv0);
    return EXIT_FAILURE;
  }

  OpenConversation(EVAL_TOPIC);
  
  ExecuteCommand("[DocOpen(\"%s\")]", sLongFileName); 
  ExecuteCommand("[FileOpen(\"%s\")]", sLongFileName); 

  if (nPage > 0) {
    ExecuteCommand("[DocGoTo(\"%s\", %d)]", sLongFileName, nPage); 
  }

  if (sGoto != NULL) {
    ExecuteCommand("[DocGoToNameDest(\"%s\", %s)]", sLongFileName, sGoto); 
  }

  CloseConversation();
#if 0
  /* Problem : it seems that these calls can put AR into a strange state.
     It would have been simpler if Adobe had simply iconified their application. */
  if (stricmp(argv0, "pdfclose") == 0) {
    sprintf(buf, "[AppHide()]");
  }
  else if (stricmp(argv0, "pdfopen") == 0) {
    sprintf(buf, "[AppShow()]");
  }
  bRetVal = DdeClientTransaction((unsigned char *)buf, (DWORD)strlen(buf),
				 (HCONV)hConversation, NULL,
				 (UINT)CF_TEXT, (UINT)XTYP_EXECUTE, (DWORD)1000, &dwResult);
#endif
  
  return EXIT_SUCCESS;
}

int pdfclose()
{
    OpenConversation(EVAL_TOPIC);

    if (bAll) {
	ExecuteCommand("[CloseAllDocs()]");
    }
    else if (sLongFileName[0] != '\0') {
	ExecuteCommand("[DocClose(\"%s\")]", sLongFileName);
    }
    
#if 0
    fprintf(stderr, "cmd : %s\n", buf);
#endif
    
    CloseConversation();
    
    return EXIT_SUCCESS;
}

int pdfserver()
{
  char line[1024];

  OpenConversation(EVAL_TOPIC);
  
  fprintf(stdout, "Type Ctrl-Z to quit.\n");

  while (fgets(line, sizeof(line), stdin)) {
#define WHITE_DELIM " \t\n"
    char *p = strtok(line, WHITE_DELIM);
    if (stricmp(p, "open") == 0) {
      p = strtok(NULL, WHITE_DELIM);
      if (CheckFileName(p) == NULL) goto next;
  
      ExecuteCommand("[DocOpen(\"%s\")]", sLongFileName); 
    }
    else if (stricmp(p, "close") == 0) {
      p = strtok(NULL, WHITE_DELIM);
      if (CheckFileName(p) == NULL) goto next;
      ExecuteCommand("[DocClose(\"%s\")]", sLongFileName);
    }
    else if (stricmp(p, "closeall") == 0) {
      ExecuteCommand("[CloseAllDocs()]");
    }
    else if (stricmp(p, "goto") == 0) {
      p = strtok(NULL, WHITE_DELIM);
      if (p == NULL) goto next;
      if (CheckFileName(p) == NULL) goto next;
      p = strtok(NULL, WHITE_DELIM);
      if (p == NULL) goto next;
      nPage = atoi(p) - 1;
      ExecuteCommand("[DocGoTo(\"%s\", %d)]", sLongFileName, nPage); 
    }
    else if (stricmp(p, "gotoname") == 0) {
      p = strtok(NULL, WHITE_DELIM);
      if (p == NULL) goto next;
      if (CheckFileName(p) == NULL) goto next;
      p = strtok(NULL, WHITE_DELIM);
      if (p == NULL) goto next;
      sGoto = p;
      ExecuteCommand("[DocGoToNameDest(\"%s\", %s)]", sLongFileName, sGoto); 
    }
    else if (stricmp(p, "show") == 0) {
      ExecuteCommand("[AppShow()]");

    }
    else if (stricmp(p, "hide") == 0) {
      ExecuteCommand("[AppHide()]");

    }
    else if (stricmp(p, "exit") == 0) {
      ExecuteCommand("[AppExit()]");
      goto exit;
    }
    else if (stricmp(p, "list") == 0) {
      fprintf(stdout, "open <file>		open the file\n");
      fprintf(stdout, "close <file>		close the file\n");
      fprintf(stdout, "closeall		close all files\n");
      fprintf(stdout, "goto <file> <page>	goto the given <page> in <file>\n");
      fprintf(stdout, "gotoname <file> <dest>	goto the <dest> named destination in <file>\n");
      fprintf(stdout, "show			show acroview\n");
      fprintf(stdout, "hide			hide acroview\n");
      fprintf(stdout, "exit			exit acroview and %s\n", argv0);
    }
  next: ;   
  }

#if 0
  fprintf(stderr, "cmd : %s\n", buf);
#endif
exit:
  CloseConversation();

  return EXIT_SUCCESS;
}

int main (int argc, char *argv[])
{
  BOOL bRet = FALSE;
  char *p;
  int argv0len = 0;
  int g; /* getopt return code */
  int option_index;

  /* Make argv0 to point to the raw base name */
  argv0 = argv[0];
  if ((p = strrchr(argv0, '/'))
      || (p = strrchr(argv0, '\\'))) {
    argv0 = _strdup(p+1);
  }
  if ((argv0len = strlen(argv0)) > 4 
      && _strnicmp(argv0 + argv0len - 4, ".exe", 4) == 0)
    argv0[argv0len - 4] = '\0';

  for(;;) {
    g = getopt_long_only (argc, argv, "", long_options, &option_index);
    
    if (g == EOF)
      break;

    if (g == '?') {
      usage();			/* Unknown option.  */
      exit(EXIT_FAILURE);
    }

    /* assert (g == 0); */ /* We have no short option names.  */
    /*
      FIXME : try 'mktexpk --mfmode --bdpi 600 ...'
    */

    if (ARGUMENT_IS ("debug")) {
#if 0
      if (! isalnum(*optarg)) {
	fprintf(stderr, "%s: warning, `%s' bad debug argument.\n", argv0, optarg);
      }
      iDebug |= atoi (optarg);
#endif
      iDebug = TRUE;
    }
    else if (ARGUMENT_IS ("help")) {
      usage();
      exit(EXIT_SUCCESS);
    }
    else if (ARGUMENT_IS ("version")) {
      fprintf(stderr, "%s of %s.\n", argv0, pdfdde_version_string);
      exit(EXIT_SUCCESS);
    }
    else if (PROGRAM_IS("pdfopen")) {
      if (ARGUMENT_IS("file")) {
	CheckFileName(optarg);
      }
      else if (ARGUMENT_IS("page")) {
	nPage = atoi(optarg) - 1;
      }
      else if (ARGUMENT_IS("goto")) {
	sGoto = optarg;
      }
    }
    else if (PROGRAM_IS("pdfclose")) {
      if (ARGUMENT_IS("file")) {
	CheckFileName(optarg);
      }
      else if (ARGUMENT_IS("all")) {
	bAll = TRUE;
      }
    }
  }

#if 0
  /* shifting options from argv[] list */
  for (i = 0; i < argc - optind; i++)
    argv[i] = argv[optind+i];
  argv[i] = NULL;

  argc = i;
#endif

  if (PROGRAM_IS("pdfopen")) {
    bRet = pdfopen();
  }
  else if (PROGRAM_IS("pdfclose")) {
    bRet = pdfclose();
  }
  else if (PROGRAM_IS("pdfdde")) {
    bRet = pdfserver();
  }
  else {
    fprintf(stderr, "This program has been incorrectly copied to the name %s.\n", argv[0]);
    bRet = EXIT_FAILURE;
  }
  return bRet;
}






