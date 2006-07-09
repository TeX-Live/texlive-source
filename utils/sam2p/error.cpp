/*
 * error.cpp
 * by pts@fazekas.hu at Fri Mar  1 11:46:27 CET 2002
 * added policies at Sat Sep  7 18:28:30 CEST 2002
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include "error.hpp"
#include "gensio.hpp"
#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <string.h> /* strlen() */
#if _MSC_VER > 1000
#  include "windows.h" /* ExitThread() */
#endif

char const* Error::argv0=(char const*)NULLP;
char const* Error::long_argv0=(char const*)NULLP;
char const* Error::banner0=(char const*)NULLP;
char const* Error::tmpargv0=(char const*)"pRg_tMp";

static Filter::NullE devNull;
GenBuffer::Writable *Error::serr=new Files::FILEW(stderr);
static Error::Policy defaultPolicy={
  (SimBuffer::B*)NULLP, /* record */
  (Error::level_t)-9999, /* topSecret */
  (Error::level_t)-9999, /* topRecorded */
  (Error::level_t)0, /* killer (level>=0) */
  (Error::level_t)-99, /* printed */
  (Error::level_t)-199, /* recorded */
  Error::serr, /* err */
  (Error::Policy*)NULLP, /* prev */
  (Error::Policy*)NULLP, /* next */
  (Error::level_t)-9999, /* curlev */
};
Error::Policy *Error::policy_top=&defaultPolicy, *Error::policy_bottom=&defaultPolicy;

char const*Error::level2str(level_t level) {
  return level==ASSERT ? "failed_assertion" : /* Imp: make assert() produce this */
         level==FATAL ? "Fatal Error" :
         level==ERROR_CONT ? "Error" :
         level==EERROR ? "Error" :
         level==WARNING_DEFER ? "Warning" :
         level==WARNING ? "Warning" :
         level==NOTICE ? "Notice" :
         level==NOTICE_DEFER ? "Notice" :
         level==INFO ? "Info" :
         level==DEBUG ? "DEBUG" :
         "??level" ;
}

GenBuffer::Writable& Error::sev(level_t level) {
  /* So they are trying to make an error? Let's see whether they can. */
  GenBuffer::Writable *err=policy_top->err;
  // printf("curlev=%d\n", policy_top->curlev);
  assert(policy_top->curlev==-9999 && "nested error/ unfinished prev err");
  policy_top->curlev=level;
  /* printf("level=%d\n", level); */
  if (level>=policy_top->printed) { /* printed or killer */
    #if 0 /* Disables printing of "using template: " */
      /* vvv Delay printing this message after all recorded messages */
      if (policy_top->record!=NULLP) err=policy_top->record;
    #endif
  } else if (level>=policy_top->recorded) { /* recorded */
    if (NULLP==(err=policy_top->record)) err=policy_top->record=new SimBuffer::B();
    if (level>policy_top->topRecorded) policy_top->topRecorded=level;
  } else { /* secret */
    if (level>policy_top->topSecret) policy_top->topSecret=level;
    return devNull;
  }
  return *err << (argv0==(char const*)NULLP?"??argv0":argv0) << ": " << level2str(level) << ": ";
  /* Processing will continue soon by GenBuffer::Writable& operator <<(GenBuffer::Writable& gb,Error*) */
}

GenBuffer::Writable& operator <<(GenBuffer::Writable& err,Error*) {
  err << '\n';
  Error::level_t level=Error::policy_top->curlev;
  if (level>=Error::policy_top->killer) { /* killer */
    /* Also print recorded messages throughout the the policy stack */
    Error::Policy *p=Error::policy_bottom;
    while (p!=NULLP) {
      if (NULLP!=p->record) Error::policy_top->err->vi_write(p->record->begin_(), p->record->getLength());
      p=p->next;
    }
    // if (level>=Error::policy_top->killer)
    Error::cexit(level);
  }
  /* Note that the order of error messages might be scrambled, i.e `printed'
   * is printed before `recorded'.
   */
  Error::policy_top->curlev=(Error::level_t)-9999; /* pedantic but useless because of nesting */
  return err;
}

void Error::pushPolicy(level_t killer_, level_t printed_, level_t recorded_, GenBuffer::Writable *err) {
  Policy *p=new Policy();
  p->record=(SimBuffer::B*)NULLP;
  p->topSecret=p->topRecorded=(Error::level_t)-9999;
  p->killer=killer_;
  p->printed=printed_;
  p->recorded=recorded_;
  p->err=(err==NULLP) ? policy_top->err : err;
  p->prev=policy_top;
  p->next=(Policy*)NULLP;
  policy_top=p->prev->next=p;
  p->curlev=(Error::level_t)-9999; /* pedantic but useless because of nesting */
}

SimBuffer::B *Error::getRecorded() {
  SimBuffer::B *ret=policy_top->record;
  policy_top->record=(SimBuffer::B*)NULLP;
  return ret;
}

void Error::setTopPrinted(level_t printed_) {
  policy_top->printed=printed_;
}

Error::level_t Error::getTopPrinted() { return policy_top->printed; }

void Error::popPolicy() {
  if (policy_top==policy_bottom) {
    Error::sev(Error::ASSERT) << "popPolicy: underflow" << (Error*)0;
  } else {
    if (NULLP!=policy_top->record) {
      (*Error::policy_top->err) << "-- recorded messages:\n";
      Error::policy_top->err->vi_write(policy_top->record->begin_(), policy_top->record->getLength());
      delete policy_top->record;
    }
    policy_top=policy_top->prev;
    delete policy_top->next;
    policy_top->next=(Policy*)NULLP;
  }
}

/* --- */

Error::Cleanup *Error::first_cleanup=(Error::Cleanup*)NULLP;

void Error::cexit(int exitCode) {
  Cleanup *next;
  int exit2;
  while (first_cleanup!=NULLP) {
    // fprintf(stderr, "hand %p\n", first_cleanup);
    if (exitCode<(exit2=first_cleanup->handler(first_cleanup))) exitCode=exit2;
    next=first_cleanup->next;
    delete [] (char*)first_cleanup; /* Allocated from as an array, but has no destructors. */
    first_cleanup=next;
  }
  #if _MSC_VER > 1000
    ExitThread(exitCode);
  #else
    exit(exitCode); /* <stdlib.h> */
  #endif
}

Error::Cleanup* Error::newCleanup(Error::Cleanup::handler_t handler, void *data, slen_t size) {
  param_assert(handler!=0);
  // slen_t num_packets=(size+sizeof(Cleanup)-1)/sizeof(Cleanup);
  Cleanup *new_=(Cleanup*)new char[size+sizeof(Cleanup)]; /* new Cleanup[1+num_packets]; */
  /* ^^^ should be a new Cleanup + new char[size]; now we can avoid alignment
   *     problems
   */
  new_->handler=handler;
  new_->size=size;
  new_->data=data;
  new_->next=first_cleanup;
  first_cleanup=new_;
  return new_;
}

Error::Cleanup* Error::newCleanup(Error::Cleanup::handler_t handler, void *data, char const*cstr) {
  slen_t slen1=strlen(cstr)+1;
  Cleanup *new_=newCleanup(handler, data, slen1+1);
  memcpy(new_->getBuf(), cstr, slen1);
  return new_;
}

/* __END__ */
