
/**********************************************************/
/* htcmd.c                               2004-12-16-16:20 */
/* Copyright (C) 1996--2004    Eitan M. Gurari            */
/*                                                        */
/* This work may be distributed and/or modified under the */
/* conditions of the LaTeX Project Public License, either */
/* version 1.3 of this license or (at your option) any    */
/* later version. The latest version of this license is   */
/* in                                                     */
/*   http://www.latex-project.org/lppl.txt                */
/* and version 1.3 or later is part of all distributions  */
/* of LaTeX version 2003/12/01 or later.                  */
/*                                                        */
/* This work has the LPPL maintenance status "maintained".*/
/*                                                        */
/* This Current Maintainer of this work                   */
/* is Eitan M. Gurari.                                    */
/*                                                        */
/* However, you are allowed to modify this program        */
/* without changing its name, if you modify its           */
/* signature. Changes to the signature can be             */
/* introduced with a directive of the form                */
/*      #define PLATFORM "signature"                      */
/*                                                        */
/*                             gurari@cse.ohio-state.edu  */
/*                 http://www.cse.ohio-state.edu/~gurari  */
/**********************************************************/

/* **********************************************
    Compiler options                            *
    (uncommented | command line)                *
------------------------------------------------*
        Clasic C (CC)             default
#define ANSI                      ansi-c, c++
#define DOS_C
#define HAVE_STRING_H             <string.h>
#define HAVE_DIRENT_H             <dirent.h>
#define HAVE_SYS_NDIR_H           <sys/ndir.h>
#define HAVE_SYS_DIR_H            <sys/dir.h>
#define HAVE_NDIR_H               <ndir.h>
#define HAVE_IO_H                 <io.h>
#define HAVE_UNISTD_H             <unistd.h>
#define WIN32
#define KPATHSEA
#define SYSTEM_FUNCTION_OK
#define CDECL                    ..........
#define BCC32                    bordland c++

*************************************************
    Tex4ht variables                            *
    (uncommented | command line)                *
----------------------------------------------- */

/* ******************************************** */


#ifdef BCC32
#define WIN32
#define ANSI
#define HAVE_IO_H
#define HAVE_STRING_H
#define PLATFORM "ms-win32"
#endif



#ifdef BCC
#define DOS_C
#define ANSI
#define HAVE_IO_H
#define PLATFORM "ms-dos"
#endif



#ifdef __DJGPP__
#define DOS_WIN32
#define ANSI
#endif


#ifdef DOS_C
#define DOS
#endif
#ifdef DOS
#define DOS_WIN32
#ifndef HAVE_STRING_H
#define HAVE_STRING_H
#endif
#endif
#ifdef WIN32
#define DOS_WIN32
#endif

#ifdef DOS_WIN32
#define STRUCT_DIRENT
#endif



#ifdef KPATHSEA
#ifdef WIN32
#define KWIN32
#endif
#endif



#include <signal.h>


#include <stdio.h>   
#include <stdlib.h>  


#ifdef HAVE_STRING_H
#include <string.h>
#endif


#ifdef HAVE_DIRENT_H

#include <dirent.h>


#else
#ifndef STRUCT_DIRENT
#define STRUCT_DIRECT
#endif

#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif


#endif




#ifndef __DJGPP__

#ifndef F_OK
#ifdef DOS_WIN32
#define  F_OK 0               
#endif
#ifndef KPATHSEA
#ifndef DOS_WIN32
#define HAVE_UNISTD_H
#endif
#endif
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifndef KPATHSEA
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif


#endif


#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/c-errno.h>
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/proginit.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-make.h>
#include <signal.h>
#if !defined(_AMIGA) && !defined(WIN32)
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <setjmp.h>
#endif 

#ifdef __DJGPP__

#ifndef F_OK
#ifdef DOS_WIN32
#define  F_OK 0               
#endif
#ifndef KPATHSEA
#ifndef DOS_WIN32
#define HAVE_UNISTD_H
#endif
#endif
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifndef KPATHSEA
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif


#endif






#ifdef DOS
#define PROTOTYP
#endif
#ifdef ANSI
#define PROTOTYP
#endif
#ifdef KWIN32
#define PROTOTYP
#endif


#ifdef KPATHSEA
#ifdef WIN32
#undef CDECL
#define CDECL                     __cdecl
#else
#define CDECL
#endif
#endif


#if defined(DOS_WIN32) || defined(__MSDOS__)
#define READ_BIN_FLAGS "rb"
#define READ_TEXT_FLAGS "r"
#define WRITE_BIN_FLAGS "wb"
#define WRITE_TEXT_FLAGS "w"
#else
#define READ_BIN_FLAGS "r"
#define READ_TEXT_FLAGS "r"
#define WRITE_BIN_FLAGS "w"
#define WRITE_TEXT_FLAGS "w"
#endif



#define Q_CHAR char
#define U_CHAR char
#define C_CHAR char
#define Q_NULL (Q_CHAR *) 0
#define U_NULL (U_CHAR *) 0
#define C_NULL (C_CHAR *) 0


#define IGNORED void


#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif


#ifdef PROTOTYP
#define VOID void
#define ARG_I(x) x
#define ARG_II(x,y) x,y
#define ARG_III(x,y,z) x,y,z
#define ARG_IV(x,y,z,w) x,y,z,w
#define ARG_V(x,y,z,w,u) x,y,z,w,u
#else
#define VOID
#define ARG_I(x)
#define ARG_II(x,y)
#define ARG_III(x,y,z)
#define ARG_IV(x,y,z,w)
#define ARG_V(x,y,z,w,u)
#endif


struct sys_call_rec{
  Q_CHAR * filter;
  struct sys_call_rec *next;
};


struct script_struct{
  Q_CHAR *command;
  struct script_struct *next;
};


#define NULL_SCRIPT (struct script_struct *) 0


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BOOL
#define BOOL int
#endif


#define bad_arg            err_arg(0)
#define bad_mem             err_i(1)


#define eq_str(x,y) (!strcmp(x,y))



static BOOL system_yes;


static int system_return;


static C_CHAR *warn_err_mssg[]={ 
"\n---------------------------------------------------------------\n\
htcmd optional-flag command-line\n\n\
   optional-flag:\n\
        -slash     translate '/' into '\\'\n\
        -dslash    translate '/' into '\\\\'\n\n\
Example: \n\
   htcmd t4ht name -d/WWW/temp/ -etex4ht-32.env -m644\n\
---------------------------------------------------------------\n"

 "" };



static void

#ifdef CDECL
CDECL
#endif


sig_err(ARG_I(int));


static void err_i( ARG_I(int) );


static void err_arg( ARG_I(int) );


static void call_sys(ARG_I(Q_CHAR *));


static void strct( ARG_II(C_CHAR *, C_CHAR *) );




static void

#ifdef CDECL
CDECL
#endif


sig_err
#ifdef ANSI
#define SEP ,
(  int s
)
#undef SEP
#else
#define SEP ;
(s)  int s
;
#undef SEP
#endif
{
  (void) signal(s,SIG_IGN);  
  switch( s ){
#ifdef SIGSEGV
    case SIGSEGV: err_i(2);
#endif
    case SIGFPE : err_i(3);
#if defined(SIGINT) && !defined(WIN32)
    case SIGINT : err_i(4);
#endif
  }
  
#ifdef __DJGPP__
  if (s != SIGINT && s != SIGQUIT)
    exit(EXIT_FAILURE);
#endif


}



static void err_i
#ifdef ANSI
#define SEP ,
(      int  n

)
#undef SEP
#else
#define SEP ;
(n)      int  n

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- error --- ");
   (IGNORED) fprintf(stderr, warn_err_mssg[n]);
   exit(EXIT_FAILURE);
}



static void err_arg
#ifdef ANSI
#define SEP ,
(      int  n

)
#undef SEP
#else
#define SEP ;
(n)      int  n

;
#undef SEP
#endif
{  (IGNORED) fprintf(stderr,"--- error --- ");
   (IGNORED) fprintf(stderr, warn_err_mssg[n]);
   exit(EXIT_FAILURE);
}



static void call_sys
#ifdef ANSI
#define SEP ,
(  Q_CHAR * command
)
#undef SEP
#else
#define SEP ;
(command)  Q_CHAR * command
;
#undef SEP
#endif
{
   if( *command ){
      (IGNORED) printf("htcmd calls: %s\n", command);
      system_return = system_yes?  (int) system(command) : -1;
      (IGNORED) printf("%shtcmd returns: %d\n",
            system_return? "--- Warning --- " : "", system_return );
   }
}



static void strct
#ifdef ANSI
#define SEP ,
(
     Q_CHAR * str1 SEP 
     Q_CHAR * str2

)
#undef SEP
#else
#define SEP ;
( str1, str2 )
     Q_CHAR * str1 SEP 
     Q_CHAR * str2

;
#undef SEP
#endif
{   Q_CHAR * ch;
   ch = str1 + (int) strlen(str1);
   (IGNORED) strcpy( ch, str2 );
}



int 
#ifdef CDECL
CDECL
#endif

 main
#ifdef ANSI
#define SEP ,
(
       int  argc SEP 
       Q_CHAR **argv
)
#undef SEP
#else
#define SEP ;
(argc, argv)
       int  argc SEP 
       Q_CHAR **argv
;
#undef SEP
#endif
{ 
int i;
BOOL slash, dslash;
char *in_command, *command, *p, *q;


   

#ifdef SIGSEGV
  (void) signal(SIGSEGV,sig_err);
#endif
  (void) signal(SIGFPE,sig_err);
#ifdef KWIN32
  
SetConsoleCtrlHandler((PHANDLER_ROUTINE)sigint_handler, TRUE);


#else
#ifdef SIGINT
  (void) signal(SIGINT,sig_err);    
#endif
#endif


#ifdef PLATFORM
   (IGNORED) printf("htcmd.c (2004-12-16-16:20 %s)\n",PLATFORM);
#else
   (IGNORED) printf("htcmd.c (2004-12-16-16:20)\n");
#endif
  
{                   C_CHAR   *yes = NULL;
  system_yes = (
#ifdef SYSTEM_FUNCTION_OK
0
#else
system( yes ) != 0
#endif

);
}


  
  in_command = (char *) malloc(1024);
  command = (char *) malloc(1024);
  in_command[0] = '\0';
  
  if( argc == 1 ){ bad_arg; }
  slash = eq_str(argv[1],"-slash") ;
  dslash = eq_str(argv[1],"-dslash") ;
  for(i=1+slash+dslash; i<argc; i++){
     (IGNORED) printf("%s ", argv[i]);
     (IGNORED) strct(in_command,argv[i]);
     (IGNORED) strct(in_command," ");
  }
  (IGNORED) printf("\n");


  
p = in_command;
q = command;
do{
  if( (*p != '"' ) && (*p != '\'' ) && (*p != '`' ) ){
    if(slash && (*p == '/')) {
      *(q++) = '\\';
    }
    else if(dslash && (*p == '/')) {
      *(q++) = '\\';
      *(q++) = '\\';
    }
    else { *(q++) =  *p;  }
  }
} while (*(p++) != '\0');
(IGNORED) call_sys(command);


   return 0;
}


