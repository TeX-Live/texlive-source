/*******************************************************************
 *
 *  time.h   Replacement for buggy <time.h> of old Turbo C compilers
 *
 *  This file is a hack!  It replaces <time.h> when compiling with
 *  old versions of Borland Turbo C compilers that lack clock(),
 *  and provide its own version.
 *
 *  Written by Antoine Leca
 *  Copyright 1999-2000 Antoine Leca,
 *  David Turner, Robert Wilhelm and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT. By continuing to use, modify or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#if !defined __TURBOC__ || !defined __MSDOS__ || __TURBOC__>0x220
/*
 * We are not running on a Borland compiler, or either on
 * a recent version that does not need the hack.
 * Certainly the user does not clean up the directory.
 * Stop the compilation.
 */
#error Remove the file time.h in directory test

#endif

#ifndef _TIME_H_DEFINED
#define _TIME_H_DEFINED

#if defined _TM_DEFINED || defined _TIME_T
#error Another version of <time.h> seems to have been already included
#endif

#ifndef  __TIME_T
#define  __TIME_T
typedef long	time_t;
#endif

#ifndef  __CLOCK_T
#define  __CLOCK_T
typedef long clock_t;
#define CLK_TCK 18.2
#endif

#define CLOCKS_PER_SEC  CLK_TCK

struct	tm	{
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};


clock_t  clock   (void);
double   difftime(time_t time2, time_t time1);
time_t   time    (time_t *timer);

char      *asctime  (const struct tm *tblock);
char      *ctime    (const time_t *time);
struct tm *gmtime   (const time_t *timer);
struct tm *localtime(const time_t *timer);


#if __TURBOC__ <= 0x0150

/*******************************************************************
 *
 *  Function    : clock
 *
 *  Description : Turbo C v.1.x lacks the clock() function that is
 *                needed for at least fttimer.
 *                So this is a replacement that does more or less
 *                the functionnality of clock(), using the BIOS.
 *                Since we do not know exactly when the process
 *                started (as clock() is supposed to do), we cheat
 *                a little here.
 *
 *  Input  : None
 *
 *  Output : None
 *
 *  Notes  : Use two static objects.
 *           NEED_CLOCK_HERE is a macro that should be defined
 *           in only ONE module (otherwise, the linker will complain).
 *
 ******************************************************************/

extern long biostime(int cmd, long newtime);

static long   CountOfTicks;
static long   DateOfReference = 0;

clock_t  clock   (void)
{

    if (DateOfReference == 0)       /* this is the first call */
    {
        DateOfReference = time(NULL) / 86400L;
        CountOfTicks = biostime(0,0L) - CLOCKS_PER_SEC;
                                    /* pretend we start one second ago */
        return CLOCKS_PER_SEC;      /* to avoid returning 0            */
    }

    return (time(NULL) / 86400L - DateOfReference) * 0x1800B0L
             + biostime(0,0L) - CountOfTicks;
}

#endif /* Turbo C v.1.x */

#endif /* defined __TIME_H_DEFINED */


/* End */
