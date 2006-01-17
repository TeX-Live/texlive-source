/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
/* This is file STAT.C */
/*
 *   Almost a 100% U**X-compatible stat() substitute.
 *
 * Usage:
 *
 *   That's easy: put this into libc.a, then just call stat() as usual.
 *
 * Rationale:
 *
 *   Many Unix-born programs make heavy use of stat() library
 *   function to make decisions on files' equality, size, access
 *   attributes etc.  In the MS-DOS environment, many implementations
 *   of stat() are crippled, because DOS makes it very hard to get to
 *   certain pieces of information about files and directories.  Thus
 *   porting a program to DOS is usually an exercise in #ifdef'ing.
 *   This implementation facilitates porting Unix programs to MS-DOS
 *   by providing stat() which is much more Unix-compatible than those
 *   of most DOS-based C compilers (e.g., Borland's).
 *   Specifically, the following issues are taken care of:
 *
 *      1. This stat() doesn't fail for root directories, returning
 *         valid information.
 *      2. Directory size is not reported zero; the number of used
 *         directory entries multiplied by entry size is returned instead.
 *      3. Mode bits are set for all 3 groups (user, group, other).
 *      4. Directories are NOT reported read-only, unless one of R, H or S
 *         attributes is set.
 *      5. Directories have their execute bit set, as they do under Unix.
 *      6. Device names (such as /dev/con, lpt1, aux etc.) are treated as
 *         if they were on a special drive called `@:' (st_dev = -1).
 *         The "character special" mode bit is set for these devices.
 *      7. The inode number (st_ino) is taken from the starting cluster
 *         number of the file.  If the cluster number is unavailable, it
 *         is invented using the file's name in a manner that minimizes
 *         the possibility of inventing an inode which already belongs
 *         to another file.  See below for details.
 *      8. Executable files are found based on files' extensions and
 *         magic numbers present at their beginning, and their execute
 *         bits are set.
 *
 *   Lossage:
 *
 *      Beautiful as the above sounds, this implementation does fail
 *      under certain circumstances.  The following is a list of known
 *      problems:
 *
 *      1. The time fields for a root directory cannot be obtained, so
 *         they are set to the beginning of the Epoch.
 *      2. For files which reside on networked drives, the inode number
 *         is invented, because network redirectors usually do not
 *         bring that info with them.  This is not a total lossage, but
 *         it could get us a different inode for each program run.
 *      3. Empty files do not have a starting cluster number, because
 *         DOS doesn't allocate one until you actually write something
 *         to a file.  For these the inode is also invented.
 *      4. If the st_ino field is a 16 bit number, the invented inode
 *         numbers are from 65535 and down, assuming that most disks have
 *         unused portions near their end.  Valid cluster numbers are 16-bit
 *         unsigned integers, so a possibility of a clash exists, although
 *         the last 80 or more cluster numbers are unused on all drives
 *         I've seen.  If the st_ino is 32 bit, then invented inodes are
 *         all greater than 64k, which totally eliminates a possibility
 *         of a clash with an actual cluster number.
 *      5. The method of computing directory size is an approximation:
 *         a directory might consume much more space, if it has many
 *         deleted entries.  Still, this is a close approximation, and
 *         it does follow the logic of reporting size for a regular file:
 *         only the actually used space is returned.
 *      6. As this implementation relies heavily on undocumented DOS
 *         features, it will fail to get actual file info in environments
 *         other than native DOS, such as DR-DOS, OS/2 etc.  For these,
 *         the function will return whatever info is available with
 *         conventional DOS calls, which is no less than any other
 *         implementation could do.  This stat() might also fail for
 *         future DOS versions, if the layout of internal DOS data
 *         area is changed; however, this seems unlikely.
 *
 * Copyright (c) 1994-96 Eli Zaretskii <eliz@is.elta.co.il>
 *
 * This software may be used freely so long as this copyright notice is
 * left intact.  There is no warranty on this software.
 *
 */

/*
 * Tested with DJGPP port of GNU C compiler, versions 1.11maint5 and 1.12,
 * under MS-DOS 3.3, 4.01, 5.0, 6.20 (with and without DoubleSpace) and
 * with networked drives under XFS 1.86, Novell Netware 3.22, and
 * TSoft NFS 0.24Beta.
 *
 */

#include <libc/stubs.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dos.h>
#include <dir.h>

#include <dpmi.h>
#include <go32.h>
#include <libc/farptrgs.h>
#include <libc/bss.h>
#include <libc/dosio.h>

#include "xstat.h"

int __getdisk(void);
int __findfirst(const char *, struct ffblk *, int);
int __findnext(struct ffblk *);

#define ALL_FILES   (FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_DIREC|FA_ARCH)

#define _STAT_INODE         1   /* should we bother getting inode numbers? */
#define _STAT_EXEC_EXT      2   /* get execute bits from file extension? */
#define _STAT_EXEC_MAGIC    4   /* get execute bits from magic signature? */
#define _STAT_DIRSIZE       8   /* compute directory size? */
#define _STAT_ROOT_TIME  0x10   /* try to get root dir time stamp? */
#define _STAT_WRITEBIT   0x20   /* fstat() needs write bit? */

/* Should we bother about executables at all? */
#define _STAT_EXECBIT       (_STAT_EXEC_EXT | _STAT_EXEC_MAGIC)

/* The structure of the full directory entry.  This is the 32-byte
   record present for each file/subdirectory in a DOS directory.
   Although the ``packed'' attribute seems to be unnecessary, I use
   it to be sure it will still work for future versions of GCC.  */

struct full_dirent {
  char           fname[8]      __attribute__ ((packed));
  char           fext[3]       __attribute__ ((packed));
  unsigned char  fattr         __attribute__ ((packed));
  unsigned char  freserved[10] __attribute__ ((packed));
  unsigned short ftime         __attribute__ ((packed));
  unsigned short fdate         __attribute__ ((packed));
  unsigned short fcluster      __attribute__ ((packed));
  unsigned int   fsize         __attribute__ ((packed));
};


/* Static variables to speed up SDA DOS Swappable Data Area access on
   subsequent calls.  */

/* The count of number of SDA's we have.  It is more than 1 for DOS
   4.x only.  If it has a value of 0, the function init_dirent_table()
   will be called to compute the addresses where we are to look for
   directory entry of our file.  A value of -1 means this method is
   unsupported for this version of DOS.  */
static int  dirent_count;

/* The table of places to look for our directory entry.
   Each entry in the table is a linear offset from the beginning of
   conventional memory which points to a particular location within
   one of the SDA's, where the entry of a file being stat()'ed could
   appear.  The offsets are computed once (when the routine is first
   called) and then reused for other calls.  The actual storage for
   the table is malloc()'ed when this function is first called.  */
static unsigned int * dirent_table;

/* When we have only one SDA, this is where its only place to look for
   directory entry is stored.  */
static unsigned int   dirent_place;

/* This holds the fail bits from the last call to init_dirent_table(),
   so we can return them every time get_inode_from_sda() is called.  */
static unsigned short init_dirent_table_bits;

/* Holds the last seen value of __bss_count, to be safe for
   restarted programs (emacs).  */
static int stat_count = -1;

/*
 * Parts of the following code is derived from file DOSSWAP.C,
 * which came with ``Undocumented DOS'', 1st edition.
 */

/* Compute table of pointers to look for directory entry of a file.  */
static int
init_dirent_table (void)
{
  short          get_sda_func;
  unsigned short dirent_offset;
  unsigned short true_dos_version;
  unsigned short dos_major, dos_minor;
  __dpmi_regs    regs;

  if (dirent_count == -1)     /* we already tried and found we can't */
    return 0;

  /* Compute INT 21h function number and offset of directory entry
     from start of SDA.  These depend on the DOS version.
     We need exact knowledge about DOS internals, so we need the
     TRUE DOS version (not the simulated one by SETVER), if that's
     available.  */
  true_dos_version = _get_dos_version(1);
  dos_major = true_dos_version >> 8;
  dos_minor = true_dos_version & 0xff;

  if ((dos_major == 3) && (dos_minor >= 10))
    {
      get_sda_func  = 0x5d06;
      dirent_offset = 0x1a7;
    }
  else if (dos_major == 4)
    {
      /* According to ``Undocumented DOS, 2nd edition'', I could have
         used 5d06 here, as for DOS 5 and above, but I like to be
         defensive.  In fact, the above book itself uses 5d0b, contrary
         to its own recommendation.  */
      get_sda_func  = 0x5d0b;
      dirent_offset = 0x1b3;
    }
  else if (dos_major >= 5)
    {
      get_sda_func  = 0x5d06;
      dirent_offset = 0x1b3;
    }
  else
    {
      _djstat_fail_bits |= _STFAIL_OSVER;
      dirent_count = -1;
      return 0;
    }

  _djstat_fail_bits &= ~_STFAIL_OSVER;  /* version is OK */

  /* Get the pointer to SDA by calling undocumented function 5dh of INT 21. */
  regs.x.ax = get_sda_func;
  __dpmi_int(0x21, &regs);
  if (regs.x.flags & 1)
    {
      _djstat_fail_bits |= _STFAIL_SDA;
      dirent_count = -1;      /* if the call failed, never try this later */
      return 0;
    }

  _djstat_fail_bits &= ~_STFAIL_SDA;    /* Get SDA succeeded */

  /* DOS 4.x might have several SDA's, which means we might have more
     than one place to look into.  (It is typical of DOS 4 to complicate
     things.)
     Compute all the possible addresses where we will have to look.  */
  if (dos_major == 4)
    {
      /* The pointer returned by INT 21h, AX=5D0b points to a header
         which holds a number of SDA's and then an array of that number
         of records each one of which includes address of an SDA (DWORD)
         and its length and type (encoded in a WORD).
         While walking this list of SDA's, we add to each pointer the
         offset of directory entry and stash the resulting address in
         our table for later use.  */

      int  sda_list_walker = MK_FOFF(regs.x.ds, regs.x.si);
      int  i;
      int *tbl;

      dirent_count = _farpeekw(_dos_ds, sda_list_walker); /* number of SDA's */

      /* Allocate storage for table.  */
      tbl = dirent_table = (int *)malloc(dirent_count*sizeof(int));
      if (!dirent_table)
        {
          /* If malloc() failed, maybe later it will succeed, so don't
             store -1 in dirent_count.  */
          dirent_count = 0;
          _djstat_fail_bits |= _STFAIL_DCOUNT;
          return 0;
        }

      memset(dirent_table, 0, dirent_count*sizeof(int));
      _djstat_fail_bits &= ~_STFAIL_DCOUNT; /* dirent_count seems OK */

      /* Walk the array of pointers, computing addresses of directory
         entries and stashing them in our table.  */
      _farsetsel(_dos_ds);
      for (i = dirent_count, sda_list_walker += 2; i--; sda_list_walker += 6)
        {
          int            sda_start = _farnspeekl(sda_list_walker);
          unsigned short sda_len   = _farnspeekw(sda_list_walker + 4) & 0x7fff;

          /* Let's be defensive here: if this SDA is too short to have
             place for directory entry, we won't use it.  */
          if (sda_len > dirent_offset)
            *tbl++ = sda_start + dirent_offset;
          else
            dirent_count--;
        }
    }

  /* DOS 3.1 and 5.0 or later.  We have only one SDA pointed to by
     whatever INT 21h, AH=5d returns.  */
  else
    {
      dirent_count = 1;
      dirent_place = MK_FOFF(regs.x.ds, regs.x.si) + dirent_offset;
      dirent_table = &dirent_place;
    }

  return 1;
}

/* Get inode number by searching DOS Swappable Data Area.
   The entire directory entry for a file found by FindFirst/FindNext
   appears at a certain (version-dependent) offset in the SDA after
   one of those function is called.
   Should be called immediately after calling DOS FindFirst function,
   before the info is overwritten by somebody who calls it again.  */
static unsigned int
get_inode_from_sda(const char *basename)
{
  int            count          = dirent_count;
  unsigned int * dirent_p       = dirent_table;
  unsigned short dos_mem_base   = _dos_ds;
  unsigned short our_mem_base   = _my_ds();
  char  * dot                   = strchr(basename, '.');
  size_t  total_len             = strlen(basename);
  int     name_len              = dot ? dot - basename : total_len;
  int     ext_len               = dot ? total_len - name_len - 1 : 0;
  int     cluster_offset        = offsetof(struct full_dirent, fcluster);

  /* Restore failure bits set by last call to init_dirent_table(), so
     they will be reported as if it were called now.  */
  _djstat_fail_bits |= init_dirent_table_bits;

  /* Force reinitialization in restarted programs (emacs).  */
  if (stat_count != __bss_count)
    {
      stat_count = __bss_count;
      dirent_count = 0;
    }

  /* Initialize the table of SDA entries where we are to look for
     our file.  */
  if (!dirent_count && !init_dirent_table())
    {
      /* Don't save the truename failure bit.  */
      init_dirent_table_bits = (_djstat_fail_bits & ~_STFAIL_TRUENAME);
      return 0;
    }
  init_dirent_table_bits = (_djstat_fail_bits & ~_STFAIL_TRUENAME);
  if (dirent_count == -1)
    return 0;

  count = dirent_count;
  dirent_p = dirent_table;

  _farsetsel(dos_mem_base);

  /* This is DOS 4.x lossage: this loop might execute many times.
     For other DOS versions it is executed exactly once.  */
  while (count--)
    {
      unsigned int  src_address = *dirent_p & 0x000fffff;
      char          cmp_buf[sizeof(struct full_dirent)];

      /* Copy the directory entry from the SDA to local storage.
         The filename is stored there in infamous DOS format: name and
         extension blank-padded to 8/3 characters, no dot between them.  */
      movedata(dos_mem_base, src_address, our_mem_base, (unsigned int)cmp_buf,
               sizeof(struct full_dirent));

      /* If this is the filename we are looking for, return
         its starting cluster. */
      if (!strncmp(cmp_buf, basename, name_len) &&
          (ext_len == 0 || !strncmp(cmp_buf + 8, dot + 1, ext_len)))
        return (unsigned int)_farnspeekw(*dirent_p + cluster_offset);

      /* This is not our file.  Search more, if more addresses left. */
      dirent_p++;
    }

  /* If not found, give up.  */
  _djstat_fail_bits |= _STFAIL_BADSDA;
  return 0;
}

int _ioctl_get_first_cluster(const char *);

/* Get the number of the first cluster of PATHNAME using
   the IOCTL call Int 21h/AX=440Dh/CX=0871h, if that call
   is supported by the OS.  Return the cluster number, or
   a negative number if this service isn't supported.  */

int
_ioctl_get_first_cluster(const char *pathname)
{
  __dpmi_regs r;

  /* See if the IOCTL GetFirstCluster call is supported.  */
  r.x.ax = 0x4411;	       /* query generic IOCTL capability by drive */
  r.h.bl = pathname[0] & 0x1f; /* drive number (1=A:) */
  r.x.cx = 0x871;
  __dpmi_int(0x21, &r);
  if ((r.x.flags & 1) == 0 && r.x.ax == 0)
    {
      r.x.ax = 0x440d;	       /* Generic IOCTL */
      r.x.cx = 0x0871;	       /* category code 08h, minor code 71h */
      r.x.bx = 1;	       /* pathname uses current OEM character set */
      r.x.ds = __tb >> 4;
      r.x.dx = __tb & 0x0f;
      _put_path(pathname);
      __dpmi_int(0x21, &r);
      if ((r.x.flags & 1) == 0)
	return ( ((int)r.x.dx << 16) + r.x.ax );
    }
  return -1;
}

static char blanks_8[] = "        ";

static int
stat_assist(const char *path, struct stat *statbuf)
{
  struct   ffblk ff_blk;
  char     canon_path[MAX_TRUE_NAME];
  char     pathname[MAX_TRUE_NAME];
  short    drv_no;
  unsigned dos_ftime;

  _djstat_fail_bits = 0;

  memset(statbuf, 0, sizeof(struct stat));
  memset(&dos_ftime, 0, sizeof(dos_ftime));

  /* Fields which are constant under DOS.  */
  statbuf->st_uid     = getuid();
  statbuf->st_gid     = getgid();
  statbuf->st_nlink   = 1;
#ifndef  NO_ST_BLKSIZE
  statbuf->st_blksize = _go32_info_block.size_of_transfer_buffer;
#endif

  /* Make the path explicit.  This makes the rest of our job much
     easier by getting rid of some constructs which, if present,
     confuse `_truename' and/or `findfirst'.  In particular, it
     deletes trailing slashes, makes "d:" explicit, and allows us
     to make an illusion of having a ".." entry in root directories.  */
  _fixpath (path, pathname);

  /* Get the drive number.  It is always explicit, since we
     called `_fixpath' on the original pathname.  */
  drv_no = toupper(pathname[0]) - 'A';

  /* Produce canonical pathname, with all the defaults resolved and
     all redundant parts removed.  This calls undocumented DOS
     function 60h.  */
  if (_truename(path, canon_path) || _truename(pathname, canon_path))
    {
      /* Detect character device names which must be treated specially.
         We could simply call FindFirst and test the 6th bit, but some
         versions of DOS have trouble with this (see Ralph Brown's
         Interrupt List, ``214E'', under `Bugs').  Instead we use
         truename() which calls INT 21/AX=6000H.  For character devices
         it returns X:/DEVNAME, where ``X'' is the current drive letter
         (note the FORWARD slash!).  E.g., for CON or \dev\con it will
         return C:/CON.
         We will pretend that devices all reside on a special drive
         called `@', which corresponds to st_dev = -1.  This is because
         these devices have no files, and we must invent inode numbers
         for them; this scheme allows to lower a risk of clash between
         invented inode and one which belongs to a real file.  This is
         also compatible with what our fstat() does.
      */
    char_dev:
      if (canon_path[2] == '/')
        {
          char dev_name[9];     /* devices are at most 8 characters long */

          strncpy(dev_name, canon_path + 3, 8); /* the name without `X:/' */
          dev_name[8] = '\0';
          strcpy(canon_path, "@:\\dev\\");
          strcat(canon_path, dev_name);
          strncat(canon_path, blanks_8, 8 - strlen(dev_name)); /* blank-pad */
          canon_path[15] = '\0';   /* ensure zero-termination */

          /* Invent inode */
          statbuf->st_ino = _invent_inode(canon_path, 0, 0);

          /* Device code. */
          statbuf->st_dev = -1;
#ifdef  HAVE_ST_RDEV
          statbuf->st_rdev = -1;
#endif

          /* Set mode bits, including character special bit.
             Should we treat printer devices as write-only?  */
          statbuf->st_mode |= (S_IFCHR | READ_ACCESS | WRITE_ACCESS);

          /* We will arrange things so that devices have current time in
             the access-time and modified-time fields of struct stat, and
             zero (the beginning of times) in creation-time field.  This
             is consistent with what DOS FindFirst function returns for
             character device names (if it succeeds--see above).  */
          statbuf->st_atime = statbuf->st_mtime = time(0);
          statbuf->st_ctime = _file_time_stamp(dos_ftime);

          return 0;
        }
      else if (canon_path[0] >= 'A' && canon_path[0] <= 'z' &&
               canon_path[1] == ':' && canon_path[2] == '\\')
        {
          /* _truename() returned a name with a drive letter.  (This is
             always so for local drives, but some network redirectors
             also do this.)  We will take this to be the TRUE drive
             letter, because _truename() knows about SUBST and JOIN.
             If the canonicalized path returns in the UNC form (which
             means the drive is remote), it cannot be SUBSTed or JOINed,
             because SUBST.EXE and JOIN.EXE won't let you do it; so, for
             these cases, there is no problem in believing the drive
             number we've got from the original path (or is there?).  */
          drv_no = toupper(canon_path[0]) - 'A';
        }
    }
  else
    {
      /* _truename() failed.  (This really shouldn't happen, but who knows?)
         At least uppercase all letters, convert forward slashes to backward
         ones, and pray... */
      register const char *src = pathname;
      register       char *dst = canon_path;

      while ( (*dst = (*src > 'a' && *src < 'z'
                       ? *src++ - ('a' - 'A')
                       : *src++)) != '\0')
        {
          if (*dst == '/')
            *dst = '\\';
          dst++;
        }

      _djstat_fail_bits |= _STFAIL_TRUENAME;
    }

  /* Call DOS FindFirst function, which will bring us most of the info.  */
  if (!__findfirst(pathname, &ff_blk, ALL_FILES))
    {
      /* Time fields. */
      dos_ftime =
        ( (unsigned short)ff_blk.ff_fdate << 16 ) +
          (unsigned short)ff_blk.ff_ftime;

      /* If the IOCTL GetFirstCluster call is available, try it first.  */
      if ( (_djstat_flags & _STAT_INODE) == 0
	   && (statbuf->st_ino = _ioctl_get_first_cluster(pathname)) <= 0)
        {

          /* For networked drives, don't believe the starting cluster
             that the network redirector feeds us; always invent inode.
             This is because network redirectors leave bogus values there,
             and we don't have enough info to decide if the starting
             cluster value is real or just a left-over from previous call.
             For local files, try first using DOS SDA to get the inode from
             the file's starting cluster number; if that fails, invent inode.
             Note that the if clause below tests for non-zero value returned
             by is_remote_drive(), which includes possible failure (-1).
             This is because findfirst() already succeeded for our pathname,
             and therefore the drive is a legal one; the only possibility that
             is_remote_drive() fails is that some network redirector takes
             over IOCTL functions in an incompatible way, which means the
             drive is remote.  QED.  */
          if (statbuf->st_ino == 0  /* don't try SDA if IOCTL call succeeded */
	      || _is_remote_drive(drv_no)
              || (statbuf->st_ino = get_inode_from_sda(ff_blk.ff_name)) == 0)
            {
              _djstat_fail_bits |= _STFAIL_HASH;
              statbuf->st_ino =
                _invent_inode(canon_path, dos_ftime, ff_blk.ff_fsize);
            }
	  else if (toupper (canon_path[0]) != toupper (pathname[0])
		   && canon_path[1] == ':'
		   && canon_path[2] == '\\'
		   && canon_path[3] == '\0')
	    /* The starting cluster in SDA for the root of JOINed drive
	       actually belongs to the directory where that drive is
	       ``mounted''.  This can potentially be the cluster of
	       another file on the JOINed drive.  We cannot allow this.  */
	    statbuf->st_ino = 1;
        }

      /* File size. */
      statbuf->st_size = ff_blk.ff_fsize;

      /* Mode bits. */
      statbuf->st_mode |= READ_ACCESS;
      if ( !(ff_blk.ff_attrib & 0x07) )  /* no R, H or S bits set */
        statbuf->st_mode |= WRITE_ACCESS;

      /* Sometimes `_truename' doesn't return X:/FOO for character
	 devices.  However, FindFirst returns attribute 40h for them.  */
      if (ff_blk.ff_attrib == 0x40)
	{
	  size_t cplen = strlen (canon_path);
	  char *pslash = canon_path + cplen - 1;

	  while (pslash > canon_path + 2 && *pslash != '\\')
	    pslash--;

	  /* Force it into X:/FOO form.  */
	  if (canon_path[1] == ':')
	    {
	      if (pslash > canon_path + 2)
		memmove (canon_path + 2, pslash,
			 cplen - (pslash - canon_path) + 1);
	      canon_path[2] = '/';
	      goto char_dev;
	    }
	}

      /* Directories should have Execute bits set. */
      if (ff_blk.ff_attrib & 0x10)
        statbuf->st_mode |= (S_IFDIR | EXEC_ACCESS);

      else
        {
          /* This is a regular file. */
          char *extension  = strrchr(ff_blk.ff_name, '.');

          /* Set regular file bit.  */
          statbuf->st_mode |= S_IFREG;

          if ((_djstat_flags & _STAT_EXECBIT) != _STAT_EXECBIT)
            {
              /* Set execute bits based on file's extension and
                 first 2 bytes. */
              if (extension)
                extension++;    /* get past the dot */
              if (_is_executable(pathname, -1, extension))
                statbuf->st_mode |= EXEC_ACCESS;
            }
        }
    }
  else if ((_djstat_fail_bits & _STFAIL_TRUENAME))
    {
      /* If both `findfirst' and `_truename' failed, this must
	 be a non-existent file or an illegal/inaccessible drive.  */
      if (errno == ENMFILE)
	errno = ENODEV;
      return -1;
    }
  else if (pathname[3] == '\0')
    {
      /* Detect root directories.  These are special because, unlike
	 subdirectories, FindFirst fails for them.  We look at PATHNAME
	 because a network redirector could tweak what `_truename'
	 returns to be utterly unrecognizable as root directory.  PATHNAME
	 always begins with "d:/", so it is root if PATHNAME[3] = 0.  */

      /* Mode bits. */
      statbuf->st_mode |= (S_IFDIR|READ_ACCESS|WRITE_ACCESS|EXEC_ACCESS);

      /* Root directory will have an inode = 1.  Valid cluster numbers
         for real files under DOS start with 2. */
      statbuf->st_ino = 1;

      /* Simulate zero size.  This is what FindFirst returns for every
         sub-directory.  Later we might compute a better approximation
         (see below).  */
      ff_blk.ff_fsize = 0L;

      /* The time fields are left to be zero, unless the user wants us
         to try harder.  In the latter case, we check if the root has
         a volume label entry, and use its time if it has. */

      if ( (_djstat_flags & _STAT_ROOT_TIME) == 0 )
	{
	  char buf[7];

	  strcpy(buf, pathname);
	  strcat(buf, "*.*");
	  if (!__findfirst(buf, &ff_blk, FA_LABEL))
	    dos_ftime = ( (unsigned)ff_blk.ff_fdate << 16 ) + ff_blk.ff_ftime;
	  else
	    _djstat_fail_bits |= _STFAIL_LABEL;
	}
    }
  else
    {
      int e = errno;	/* errno value from original FindFirst on PATHNAME */
      int i = 0;
      int j = strlen (pathname) - 1;

      /* Check for volume labels.  We did not mix FA_LABEL with
	 other attributes in the call to `__findfirst' above,
	 because some environments will return bogus info in
	 that case.  For instance, Win95 and WinNT seem to
	 ignore `pathname' and return the volume label even if it
	 doesn't fit the name in `pathname'.  This fools us to
	 think that a non-existent file exists and is a volume
	 label.  Hence we test the returned name to be PATHNAME.  */
      if (!__findfirst(pathname, &ff_blk, FA_LABEL))
	{
	  i = strlen (ff_blk.ff_name) - 1;

	  if (j >= i)
	    {
	      for ( ; i >= 0 && j >= 0; i--, j--)
		if (toupper (ff_blk.ff_name[i]) != toupper (pathname[j]))
		  break;
	    }
	}

      if (i < 0 && pathname[j] == '/')
	{
	  /* Indeed a label.  */
	  statbuf->st_mode = READ_ACCESS;
#ifdef  S_IFLABEL
	  statbuf->st_mode |= S_IFLABEL;
#endif
	  statbuf->st_ino = 1;
	  statbuf->st_size = 0;
	  dos_ftime = ( (unsigned)ff_blk.ff_fdate << 16 ) + ff_blk.ff_ftime;
	}
      else
	{
	  /* FindFirst on volume labels might set errno to ENMFILE or even
	     to something more strange like EINVAl; correct that.  */
	  errno = e;	/* restore errno from the original FindFirst */
	  if (errno == ENMFILE)
	    errno = ENOENT;
	  return -1;
	}
    }

  /* Device code. */
  statbuf->st_dev = drv_no;
#ifdef  HAVE_ST_RDEV
  statbuf->st_rdev = drv_no;
#endif

  /* Time fields. */
  statbuf->st_atime = statbuf->st_mtime = statbuf->st_ctime =
    _file_time_stamp(dos_ftime);

  if ( ! strcmp(ff_blk.lfn_magic,"LFN32") )
    {
      unsigned xtime;
      xtime = *(unsigned *)&ff_blk.lfn_ctime;
      if(xtime)			/* May be zero if file written w/o lfn active */
        statbuf->st_ctime = _file_time_stamp(xtime);
      xtime = *(unsigned *)&ff_blk.lfn_atime;
      if(xtime > dos_ftime)	/* Accessed time is date only, no time */
        statbuf->st_atime = _file_time_stamp(xtime);
    }

  if ( (statbuf->st_mode & S_IFMT) == S_IFDIR
       && (_djstat_flags & _STAT_DIRSIZE) == 0 )
    {
      /* Under DOS, directory entries for subdirectories have
         zero size.  Therefore, FindFirst brings us zero size
         when called on a directory.  (Some network redirectors
         might do a better job, thus below we also test for zero size
         actually being returned.)  If we have zero-size directory,
         we compute here the actual directory size by reading its
         entries, then multiply their number by 32 (the size of a
         directory entry under DOS).  This might lose in the case
         that many files were deleted from a once huge directory,
         because AFAIK, directories don't return unused clusters to
         the disk pool.  Still, it is a good approximation of the
         actual directory size.

         We also take this opportunity to return the number of links
         for directories as Unix programs expect it to be: the number
         of subdirectories, plus 2 (the directory itself and the ``.''
         entry).

         The (max) size of the root directory could also be taken from
         the disk BIOS Parameter Block (BPB) which can be obtained
         by calling IOCTL (INT 21/AH=44H), subfunction 0DH, minor
         function 60H.  But we will treat all directories the same,
         even at performance cost, because it's more robust for
         networked drives.  */

      size_t pathlen = strlen (pathname);
      char   lastc   = pathname[pathlen - 1];
      char  *search_spec = (char *)alloca (pathlen + 10); /* need only +5 */
      int nfiles = 0, nsubdirs = 0, done;
      size_t extra = 0;
      int add_extra = 0;

      strcpy(search_spec, pathname);
      if (lastc == '/')
        strcat(search_spec, "*.*");
      else
        strcat(search_spec, "/*.*");

      if (statbuf->st_size == 0 && _USE_LFN)
	{
	  /* VFAT filesystems use additional directory entries to
	     store the long filenames.  */
	  char fstype[40];

	  if ((_get_volume_info(pathname,0,0,fstype) & _FILESYS_LFN_SUPPORTED)
	      && strncmp(fstype, "FAT", 4) == 0)
	    add_extra = 1;
	}

      /* Count files and subdirectories.  */
      for (done = __findfirst(search_spec, &ff_blk, ALL_FILES);
	   !done;
	   done = __findnext(&ff_blk))
	{
	  register char *fname = ff_blk.ff_name;

	  /* Don't count "." and ".." entries.  This will show empty
	     directories as size 0.  */
	  if (! (fname[0] == '.'
		 && (fname[1] == '\0'
		     || (fname[1] == '.'
			 && fname[2] == '\0'))))
	    {
	      char fn[13];

	      nfiles++;
	      if (ff_blk.ff_attrib & 0x10)
		nsubdirs++;
	      /* For each 13 characters of the long filename, a
		 32-byte directory entry is used.  */
	      if (add_extra && strcmp(_lfn_gen_short_fname(fname, fn), fname))
		extra += (strlen(fname) + 12) / 13;
	    }
	}

      statbuf->st_nlink = nsubdirs + 2;
      if (statbuf->st_size == 0)
	statbuf->st_size  = (nfiles + extra) * sizeof(struct full_dirent);
    }

  return 0;
}

/* Main entry point.  This is library stat() function.
 */

int
stat(const char *path, struct stat *statbuf)
{
  int  e = errno;
  int  pathlen;

  if (!path || !statbuf)
    {
      errno = EFAULT;
      return -1;
    }

  if ((pathlen = strlen (path)) >= MAX_TRUE_NAME)
    {
      errno = ENAMETOOLONG;
      return -1;
    }

  /* Fail if PATH includes wildcard characters supported by FindFirst.  */
  if (memchr(path, '*', pathlen) || memchr(path, '?', pathlen))
    {
      errno = ENOENT;	/* since no such filename is possible */
      return -1;
    }

  if (stat_assist(path, statbuf) == -1)
    {
      return -1;      /* errno set by stat_assist() */
    }
  else
    {
      errno = e;
      return 0;
    }
}

#ifdef  TEST

unsigned short _djstat_flags = 0;

void
main(int argc, char *argv[])
{
  struct stat stat_buf;
  char *endp;

  if (argc < 2)
    {
      fprintf (stderr, "Usage: %s <_djstat_flags> <file...>\n", argv[0]);
      exit(0);
    }

  if (stat(*argv, &stat_buf) != 0)
    perror ("stat failed on argv[0]");
  else
    fprintf(stderr, "DOS %d.%d (%s)\n", _osmajor, _osminor, _os_flavor);
  argc--; argv++;

  _djstat_flags = (unsigned short)strtoul(*argv, &endp, 0);
  argc--; argv++;

  while (argc--)
    {
      if (!stat(*argv, &stat_buf))
        {
          fprintf(stderr, "%s: %d %6u %o %d %d %ld %lu %s", *argv,
                  stat_buf.st_dev,
                  (unsigned)stat_buf.st_ino,
                  stat_buf.st_mode,
                  stat_buf.st_nlink,
                  stat_buf.st_uid,
                  (long)stat_buf.st_size,
                  (unsigned long)stat_buf.st_mtime,
                  ctime(&stat_buf.st_mtime));
          _djstat_describe_lossage(stderr);
        }
      else
        {
          fprintf(stderr, "%s: lossage", *argv);
          perror(" ");
          _djstat_describe_lossage(stderr);
        }

      ++argv;
    }

    exit (0);
}

#endif

