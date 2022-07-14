/*359:*/
#line 7180 "hint.w"

#ifndef _ERROR_H
#define _ERROR_H
#include <stdlib.h> 
#include <stdio.h> 
#include <setjmp.h> 
#define MAX_HERROR 1024
extern char herror_string[MAX_HERROR];
extern FILE*hlog;
extern void hint_end(void);
extern jmp_buf error_exit;

#ifdef _MSC_VER 
#define snprintf(S,N,F,...) _snprintf(S,N,F,__VA_ARGS__)
#ifndef _CONSOLE
#pragma warning(disable : 4996)
extern void hmessage(char*title,char*format,...);
#define MESSAGE(...)  hmessage("HINT",__VA_ARGS__)

extern int herror(char*title,char*msg);
#define ERROR_MESSAGE  herror("HINT ERROR",herror_string)
#endif
#endif

#ifdef __ANDROID__ 
#include <android/log.h> 

#define LOG(...)      __android_log_print(ANDROID_LOG_DEBUG,__FILE__,__VA_ARGS__)
#define MESSAGE(...)  __android_log_print(ANDROID_LOG_INFO,__FILE__, __VA_ARGS__)
#define ERROR_MESSAGE __android_log_print(ANDROID_LOG_ERROR,__FILE__,"ERROR: %s\n", herror_string)

#endif

#ifndef LOG
#define LOG(...) (fprintf(hlog,__VA_ARGS__),fflush(hlog))
#endif

#ifndef MESSAGE
#define MESSAGE(...)  (fprintf(stderr,__VA_ARGS__),fflush(stderr))
#endif

#ifndef ERROR_MESSAGE
#define ERROR_MESSAGE        fprintf(stderr,"ERROR: %s\n",herror_string)
#endif

#ifndef QUIT
#define QUIT(...)    (snprintf(herror_string,MAX_HERROR-1,__VA_ARGS__),\
                     ERROR_MESSAGE,hint_end(),longjmp(error_exit,1))
#endif


#ifndef HINT_TRY
#define HINT_TRY if ((herror_string[0]= 0,setjmp(error_exit)==0))
#endif

#endif
/*:359*/
