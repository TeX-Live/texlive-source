/* Utility and Unix shadow routines for XEmacs on Windows NT.
   Copyright (C) 1994, 1995 Free Software Foundation, Inc.

This file is part of XEmacs.

XEmacs is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

XEmacs is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with XEmacs; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.


   Geoff Voelker (voelker@cs.washington.edu) 7-29-94 */

/* Adapted for XEmacs by David Hobley <david@spook-le0.cia.com.au> */
/* Sync'ed with Emacs 19.34.6 by Marc Paquette <marcpa@cam.org> */
/* Adapted to fpTeX 0.4 by Fabrice Popineau <Fabrice.Popineau@supelec.fr> */

/* Emulate getpwuid, getpwnam and others.  */

#include <win32lib.h>

#define PASSWD_FIELD_SIZE 256

static char the_passwd_name[PASSWD_FIELD_SIZE];
static char the_passwd_passwd[PASSWD_FIELD_SIZE];
static char the_passwd_gecos[PASSWD_FIELD_SIZE];
static char the_passwd_dir[PASSWD_FIELD_SIZE];
static char the_passwd_shell[PASSWD_FIELD_SIZE];

struct passwd the_passwd = 
{
  the_passwd_name,
  the_passwd_passwd,
  0,
  0,
  0,
  the_passwd_gecos,
  the_passwd_dir,
  the_passwd_shell,
};

int 
getuid () 
{ 
  return the_passwd.pw_uid;
}

int 
geteuid () 
{ 
  /* I could imagine arguing for checking to see whether the user is
     in the Administrators group and returning a UID of 0 for that
     case, but I don't know how wise that would be in the long run.  */
  return getuid (); 
}

int 
getgid () 
{ 
  return the_passwd.pw_gid;
}

int 
getegid () 
{ 
  return getgid ();
}

struct passwd *
getpwuid (int uid)
{
  if (uid == the_passwd.pw_uid)
    return &the_passwd;
  return NULL;
}

struct passwd *
getpwnam (const char *name)
{
  struct passwd *pw;
  
  pw = getpwuid (getuid ());
  if (!pw)
    return pw;

  if (stricmp (name, pw->pw_name))
    return NULL;

  return pw;
}

void
init_user_info ()
{
  /* Find the user's real name by opening the process token and
     looking up the name associated with the user-sid in that token.

     Use the relative portion of the identifier authority value from
     the user-sid as the user id value (same for group id using the
     primary group sid from the process token). */
#if 0
  char            user_sid[256], name[256], domain[256];
  DWORD           length = sizeof (name), dlength = sizeof (domain), trash;
  HANDLE          token = NULL;
  SID_NAME_USE    user_type;

  if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &token)
      && GetTokenInformation (token, TokenUser,
			      (PVOID) user_sid, sizeof (user_sid), &trash)
      && LookupAccountSid (NULL, *((PSID *) user_sid), name, &length,
			   domain, &dlength, &user_type))
    {
      strcpy (the_passwd.pw_name, name);
      /* Determine a reasonable uid value. */
      if (stricmp ("administrator", name) == 0)
	{
	  the_passwd.pw_uid = 0;
	  the_passwd.pw_gid = 0;
	}
      else
	{
	  SID_IDENTIFIER_AUTHORITY * pSIA;

	  pSIA = GetSidIdentifierAuthority (*((PSID *) user_sid));
	  /* I believe the relative portion is the last 4 bytes (of 6)
	     with msb first. */
	  the_passwd.pw_uid = ((pSIA->Value[2] << 24) +
			       (pSIA->Value[3] << 16) +
			       (pSIA->Value[4] << 8)  +
			       (pSIA->Value[5] << 0));
	  /* restrict to conventional uid range for normal users */
	  the_passwd.pw_uid = the_passwd.pw_uid % 60001;

	  /* Get group id */
	  if (GetTokenInformation (token, TokenPrimaryGroup,
				   (PVOID) user_sid, sizeof (user_sid), &trash))
	    {
	      SID_IDENTIFIER_AUTHORITY * pSIA;

	      pSIA = GetSidIdentifierAuthority (*((PSID *) user_sid));
	      the_passwd.pw_gid = ((pSIA->Value[2] << 24) +
				   (pSIA->Value[3] << 16) +
				   (pSIA->Value[4] << 8)  +
				   (pSIA->Value[5] << 0));
	      /* I don't know if this is necessary, but for safety... */
	      the_passwd.pw_gid = the_passwd.pw_gid % 60001;
	    }
	  else
	    the_passwd.pw_gid = the_passwd.pw_uid;
	}
    }
  /* If security calls are not supported (presumably because we
       are running under Windows 95), fallback to this. */
  else 
    if (GetUserName (name, &length))
    {
      strcpy (the_passwd.pw_name, name);
      if (stricmp ("administrator", name) == 0)
	the_passwd.pw_uid = 0;
      else
	the_passwd.pw_uid = 123;
      the_passwd.pw_gid = the_passwd.pw_uid;
    }
  else
#else
    {
      strcpy (the_passwd.pw_name, "unknown");
      the_passwd.pw_uid = 123;
      the_passwd.pw_gid = 123;
    }
#endif

  /* Ensure HOME and SHELL are defined. */
#if 1
  /*
   * With XEmacs, setting $HOME is deprecated.
   * But with fpTeX, it is safer to assume $HOME defined.
   */
  {
    char *home = get_home_directory();
    if (home) {
      putenv(concat("HOME=", home));
    }
    else {
      putenv ("HOME=c:/");
    }
  }
#endif
  if (getenv ("SHELL") == NULL)
    putenv ((GetVersion () & 0x80000000) ? "SHELL=command" : "SHELL=cmd");

  {
    /* Win2K problem : we need a specific TEMP directory with 
       full access rights so that any user building a format file
       or a font file will build it with full access rights. The installer 
       takes care of defining TEXMFTEMP=$SELFAUTOPARENT/tmp in the environment.
       If it is defined, then use it as the TEMP and TMP variables.
    */
    char *p;
    if ((p = getenv("TEXMFTEMP")) != NULL) {
      putenv(concat("TEMP=", p));
      putenv(concat("TMP=", p));
    }
  }

  /* Set dir and shell from environment variables. */
  strcpy (the_passwd.pw_dir, get_home_directory());
  strcpy (the_passwd.pw_shell, getenv ("SHELL"));

#if 0
  if (token)
    CloseHandle (token);
#endif
}
