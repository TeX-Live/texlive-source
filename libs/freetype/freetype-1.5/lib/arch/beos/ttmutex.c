/*******************************************************************
 *
 *  ttmutex.c                                                1.0
 *
 *    Mutual exclusion object, BeOS implementation
 *
 *  Copyright 1999-2001 by
 *  Catharon Productions, Inc.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#include "ttmutex.h"

#include <kernel/OS.h>
#include <support/SupportDefs.h>
#include <stdio.h>

/* required by the tracing mode */
#undef  TT_COMPONENT
#define TT_COMPONENT  trace_mutex


#ifdef TT_CONFIG_OPTION_THREAD_SAFE

/* New and improved BeOS mutex in the form of a "benaphore". */

typedef struct
{
  sem_id  _sem;
  int32   _atom;
} beos_mutex_t;

/* Counter so the semaphores have unique names; access this with */
/* atomic_*() functions only!                                    */

static int32  _mutex_counter = 0;

/* Allocate and initialize the mutex. */

FT_INTERNAL_FUNC( void )
TT_Mutex_Create( TMutex*  mutex )
{
  beos_mutex_t*  m = NULL;


  m = (beos_mutex_t *)malloc( sizeof( beos_mutex_t ) );
  if ( m )
  {
    char   mutex_name[B_OS_NAME_LENGTH];
    int32  prev = atomic_add( &_mutex_counter, 1 );


    sprintf( mutex_name, "FreeType mutex (%d)", (int)prev );

    m->_atom = 0;
    m->_sem  = create_sem( 0, mutex_name );
    
    if ( m->_sem < B_NO_ERROR )
    {
      /* Can't get a semaphore, so we're screwed. */
      free( m );
      m = NULL;
    }
  }

  /* Send back the new mutex. */
  *mutex = (TMutex)m;
}


/* Delete a mutex. */

FT_INTERNAL_FUNC( void )
TT_Mutex_Delete( TMutex*  mutex )
{
  if ( mutex )
  {
    beos_mutex_t*  m = (beos_mutex_t *)*mutex;

    
    if ( m->_sem >= B_NO_ERROR )
    {
      status_t  retval = delete_sem( m->_sem );

      
      /* Make the compiler shut up... */
      retval = retval;
    }

    free( m );
  }
}


/* Attempt to lock a mutex; blocks until we can lock it or the semaphore */
/* is destroyed.                                                         */

FT_INTERNAL_FUNC( void )
TT_Mutex_Lock( TMutex*  mutex )
{
  int32          prev  = 0;
  beos_mutex_t*  m     = (beos_mutex_t *)*mutex;


  if ( !mutex )
    return;

  prev = atomic_add( &(m->_atom), 1 );
  if ( prev > 0 )
  {
    status_t  retval = acquire_sem( m->_sem );

      
    /* Make the compiler shut up... */
    retval = retval;
  }
}


/* Release a mutex. */

FT_INTERNAL_FUNC( void )
TT_Mutex_Release( TMutex*  mutex )
{
  int32          prev = 0;
  beos_mutex_t*  m    = (beos_mutex_t *)*mutex;


  if ( !mutex )
    return;

  prev = atomic_add( &(m->_atom), -1 );
  if ( prev > 1 ) {
    status_t  retval = release_sem( m->_sem );
      
    /* Make the compiler shut up... */
    retval = retval;
  }
}

#endif /* TT_CONFIG_OPTION_THREAD_SAFE */


/* END */
