#############################################################################*
##############################################################################
##
##  MODULE
##
##      $RCSfile: unix.mak,v $
##      $Revision: 3.71 $
##      $Date: 1996/08/18 20:37:06 $
##
##  DESCRIPTION
##
##      A 32-bit implementation of BibTeX v0.99c for MS-DOS, OS/2 2.x, 
##      Unix and VMS.  This C language implementation is based on the 
##      original WEB source but it has been enhanced to support 8-bit input
##      characters and a very large processing capacity.
##
##      For documentation describing how to use and build this program, 
##      see the 00README.TXT file that accompanies this distribution.
##
##  MODULE CONTENTS
##
##      This is the BibTeX makefile to build a Unix implementation.  This 
##      file should be called "unix.mak" and used with a command like:
##
##          % make -f unix.mak bibtex
##
##      If you omit the "bibtex" argument, the Makefile will display list 
##      of the supported targets.
##
##      Below is a list of the Unix environments on which BibTeX has been
##      built and tested.  The appropriate Make command to build BibTeX 
##      is also given for each of the environments.  Each entry is preceded 
##      by a status character:
##
##          +   tested and believed to work
##          ?   not tested but should work
##	    X   believed not to work.
##
##      The environments and Make targets are:
##	
##	    + Linux, GNU cc: "linux-gcc"
##	    + SUN, SunOS 3.x & 4.1.x, GNU cc: "sunos-gcc"
##	    + SUN, Solaris 2.x, GNU cc: "sysv-gcc"
##
##	    ? BSD generic, GNU cc: "bsd-gcc"
##	    ? System V generic, GNU cc: "sysv-gcc"
##
##  	For example, if you want to build BibTeX for a SunOS system running 
##      SunOS 4.1.3 and using the GNU compiler, you should issue the
##  	command:
##
##  	    make -f unix.mak sunos-gcc
##
##      If you port BibTeX to a new environment or find that it works 
##      unchanged in a new environment, please report back to the author so 
##      that this file can be updated for everyone else.
##
##  AUTHORS
##
##      Original WEB translation and porting by:
##
##          Niel Kempson, 
##          Snowy Owl Systems Limited, Cheltenham, England
##          E-mail: kempson@snowyowl.co.uk
##      
##      8 bit support extensions by:
##
##          Alejandro Aguilar-Sierra
##          E-mail: asierra@servidor.unam.mx
##
##  COPYRIGHT
##
##      This implementation copyright (c) 1991-1995 by Niel Kempson
##           and copyright (c) 1995 by Alejandro Aguilar-Sierra.
##
##      This program is free software; you can redistribute it and/or
##      modify it under the terms of the GNU General Public License as
##      published by the Free Software Foundation; either version 1, or
##      (at your option) any later version.
##
##      This program is distributed in the hope that it will be useful,
##      but WITHOUT ANY WARRANTY; without even the implied warranty of
##      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##      General Public License for more details.
##
##      You should have received a copy of the GNU General Public License
##      along with this program; if not, write to the Free Software
##      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##      In other words, you are welcome to use, share and improve this
##      program.  You are forbidden to forbid anyone else to use, share
##      and improve what you give them.  Help stamp out software-hoarding!
##
##  ACKNOWLEDGEMENT
##      
##      The original BibTeX was written by Oren Patashnik using Donald 
##      Knuth's WEB system.  This format produces a PASCAL program for 
##      execution and a TeX documented version of the source code. This 
##      program is a (hand) translation of the WEB source into C.
##  
##  CHANGE LOG
##
##      $Log: unix.mak,v $
##      Revision 3.71  1996/08/18  20:37:06  kempson
##      Official release 3.71 (see HISTORY file for details).
##
##      Revision 3.70  1996/04/08  10:17:58  kempson
##      Final documentation & cosmetic changes for official release 3.70.
##
##      Revision 1.0  1995/10/21  21:52:44  kempson
##      Placed under RCS control
##
##############################################################################
##############################################################################



##############################################################################
##############################################################################
#
# Start of local definitions for file types and compilation / linking.  There
# are three small sections to customise:
#
#   BibTeX File Searching
#     - specifies the names of environment variables and paths to be used when
#       searching for input files
#
#   Utility Programs
#     - specifies the names of programs to be used for simple functions
#
#   Compiler/Linker
#     - specifies compiler/linker command lines
#
# If your system is already supported it should not be necessary to modify
# any part of the Makefile except these three sections.
#
##############################################################################
##############################################################################

#-----------------------------------------------------------------------------
# BIBTEX FILE SEARCHING
#
#   The macros in this section define the environment variables and default
#   search paths that will be used by BibTeX.  8-bit BibTeX looks for input
#   files in three different steps as summarised below.  As soon as 8-bit
#   BibTeX finds a matching file, it stops looking (i.e. only the first
#   matching file is used).  In order, the three steps are:
#
#     - look for the file in the current working directory.
#      
#     - if the appropriate environment variable has been set (e.g. BSTINPUT),
#       treat its value as a list of directories to be searched.  Look for the
#       file in each of the directories in the list.
#        
#     - if the appropriate environment variable (e.g. BSTINPUT) has not been
#       set, use a predefined "fallback" path as a list of directories to be
#       searched.
#
#   A search list consists of a number of directories separated by colons.
#
#   The following entries must be defined:
#
#     AUX_ENVVAR        the name of the environment variable defining the path
#                       to be searched for .aux files.
#
#     AUX_INPUT_PATH    the fallback path to be searched for .aux files.
#
#     BIB_ENVVAR        the name of the environment variable defining the path
#                       to be searched for .bib files (e.g. BIBINPUT).
#
#     BIB_INPUT_PATH    the fallback path to be searched for .bib files
#                       (e.g. /u/kempson/bibtex:/usr/local/lib/bibtex/bib).
#
#     BST_ENVVAR        the name of the environment variable defining the path
#                       to be searched for .bst files (e.g. BSTINPUT).
#
#     BST_INPUT_PATH    the fallback path to be searched for .bst files
#                       (e.g. /u/kempson/bibtex:/usr/local/lib/bibtex/bst).
#
#     CSF_ENVVAR        the name of the environment variable defining the path
#                       to be searched for .csf files (e.g. CSFINPUT).
#
#     CSF_INPUT_PATH    the fallback path to be searched for .csf files
#                       (e.g. /u/kempson/bibtex:/usr/local/lib/bibtex/csf).
#
#     CSF_FILE_ENVVAR   the name of the environment variable defining the
#                       fallback CS file (e.g. BIBTEX_CSFILE).
#
#     CSF_FILE_NAME     the name of the fallback CS file to be used if one is
#                       not specified on the command line and the above-named
#                       environment variable is not defined (e.g. cp850lat.csf).
#-----------------------------------------------------------------------------
AUX_INPUT_ENVVAR    = 
AUX_INPUT_PATH      = 

BIB_INPUT_ENVVAR    = BIBINPUT
BIB_INPUT_PATH      = /usr/local/lib/bibtex/bib

BST_INPUT_ENVVAR    = BSTINPUT
BST_INPUT_PATH      = /usr/local/lib/bibtex/bst

CSF_INPUT_ENVVAR    = CSFINPUT
CSF_INPUT_PATH      = /usr/local/lib/bibtex/csf

CSF_FILE_ENVVAR     = BIBTEX_CSFILE
CSF_FILE_NAME       = cp850lat.csf


#-----------------------------------------------------------------------------
# UTILITY PROGRAMS
#
#   MAKE                the command used to invoke Make to process this file.
#                       As distributed, it is defined as "make -f os2.mak"
#
#   RM	                rm program that won't complain (e.g. /bin/rm -f) if a
#                       file is missing
#-----------------------------------------------------------------------------
MAKE                = make -f unix.mak
RM                  = rm -f


#-----------------------------------------------------------------------------
#
# COMPILER/LINKER
#
#   CC		        the command used to invoke the C compiler
#   CFLAGS	        compiler command line flags to be used
#
#   LD		        the command used to invoke the linker
#   LDFLAGS	        linker command line flags to be used
#   LDLIBS	        linker command line option to search the specified
#		        library files
#
#   The definitions are used to compile and link as shown below:
#
#       Compiling C programs (e.g. xx.c)
#
#	    $(CC)  -c  $(CFLAGS)  xx.c
#
#       Linking object files (e.g. xx.o, yy.o & zz.o)
#
#	    $(LD)  -o $@ $(LDFLAGS)  xx.o  yy.o  zz.o  $(LDLIBS)
#
#       The 'vanilla' settings should usually be:
#
#	    CC	    = cc
#	    CFLAGS  = -O -DUNIX
#	    LD	    = cc
#	    LDFLAGS = 
#           LDLIBS  =
#
#       For GNU C, the following settings are recommended:
#
#	    CC	    = gcc
#	    CFLAGS  = -O -DUNIX -O -Wall -Wno-char-subscripts -funsigned-char
#	    LD	    = gcc
#	    LDFLAGS = 
#           LDLIBS  =
#-----------------------------------------------------------------------------
CC		= cc
CFLAGS		= 

LD		= cc
LDFLAGS		= 
LDLIBS          = 

##############################################################################
##############################################################################
#
# End of local definitions.  You shouldn't need to change anything below.
#
##############################################################################
##############################################################################


##############################################################################
# File types and default rule for compilation
##############################################################################
BIBTEX_DEFINES  = -DSUPPORT_8BIT \
                  -DAUX_INPUT_ENVVAR=\"$(AUX_INPUT_ENVVAR)\" \
                  -DAUX_INPUT_PATH=\"$(AUX_INPUT_PATH)\" \
                  -DBIB_INPUT_ENVVAR=\"$(BIB_INPUT_ENVVAR)\" \
                  -DBIB_INPUT_PATH=\"$(BIB_INPUT_PATH)\" \
                  -DBST_INPUT_ENVVAR=\"$(BST_INPUT_ENVVAR)\" \
                  -DBST_INPUT_PATH=\"$(BST_INPUT_PATH)\" \
                  -DCSF_INPUT_ENVVAR=\"$(CSF_INPUT_ENVVAR)\" \
                  -DCSF_INPUT_PATH=\"$(CSF_INPUT_PATH)\" \
                  -DCSF_FILE_ENVVAR=\"$(CSF_FILE_ENVVAR)\" \
                  -DCSF_FILE_NAME=\"$(CSF_FILE_NAME)\"

EXE		=
O		= .o

.SUFFIXES:      .c .h $(O)

.c.o:
	$(CC) -c $(BIBTEX_DEFINES) -DUNIX $(CFLAGS) $*.c



##############################################################################
# Target Groupings
##############################################################################
HDRFILES	= bibtex.h datatype.h gblprocs.h gblvars.h sysdep.h \
                  getopt.h utils.h version.h

OBJFILES	= bibtex$(O) \
                  bibtex-1$(O) bibtex-2$(O) bibtex-3$(O) bibtex-4$(O) \
                  getopt$(O) getopt1$(O) utils$(O)

EXEFILES	= bibtex$(EXE)



##############################################################################
# Top level targets
##############################################################################
make:
	@ echo 'You must specify the system for which BibTeX should be made.'
	@ echo ''
	@ echo 'For example: $(MAKE) sunos-gcc'
	@ echo ''
	@ echo 'Valid system targets are:'
	@ echo ''
	@ echo '    clean, veryclean'
	@ echo ''
	@ echo '    bsd-gcc, linux-gcc, sunos-gcc, sysv-gcc'    

bibtex-exe: $(EXEFILES)

clean:
	@ echo 'Cleaning up ... '
	$(RM) $(OBJFILES)

veryclean:
	@ echo 'Really cleaning up ... '
	$(RM) $(OBJFILES)
	$(RM) $(EXEFILES)



#-----------------------------------------------------------------------------
# Dependencies for executables
#-----------------------------------------------------------------------------
bibtex$(EXE):	$(OBJFILES)
	$(LD)  -o $@  $(LDFLAGS) $(OBJFILES) $(LDLIBS)


#-----------------------------------------------------------------------------
# Dependencies for object files
#-----------------------------------------------------------------------------
bibtex$(O):   bibtex.c $(HDRFILES)

bibtex-1$(O): bibtex-1.c $(HDRFILES)

bibtex-2$(O): bibtex-2.c $(HDRFILES)

bibtex-3$(O): bibtex-3.c $(HDRFILES)

bibtex-4$(O): bibtex-4.c $(HDRFILES)

getopt$(O):   getopt.c getopt.h

getopt1$(O):  getopt1.c getopt.h

utils$(O):    utils.c $(HDRFILES)



##############################################################################
##############################################################################
##
## OPERATING SYSTEM ENVIRONMENT SPECIFIC TARGETS
##
##  The following symbols may be defined using "-D" to affect the 
##  compilation:
##
##  HAS_STRINGS_H	-   The header file for string functions is 
##			    <strings.h> rather than <string.h>
##############################################################################
##############################################################################


#-----------------------------------------------------------------------------
# BSD generic environment, GNU cc
#-----------------------------------------------------------------------------
bsd-gcc:
	@ echo 'Making BibTeX for BSD using GNU cc ... '
	$(MAKE) bibtex-exe CC=gcc \
		CFLAGS="-Wall -Wno-char-subscripts -funsigned-char" \
		LD=gcc

#-----------------------------------------------------------------------------
# Linux, GNU cc
#-----------------------------------------------------------------------------
linux-gcc:
	@ echo 'Making BibTeX for Linux using GNU cc ... '
	$(MAKE) bibtex-exe CC=gcc \
		CFLAGS="-Wall -Wno-char-subscripts -funsigned-char" \
		LD=gcc

#-----------------------------------------------------------------------------
# SunOS 4.1.x Unix BSD environment, GNU cc
#-----------------------------------------------------------------------------
sunos-gcc:
	@ echo 'Making BibTeX for SunOS 4.1.x using GNU cc ... '
	$(MAKE) bibtex-exe CC=gcc \
		CFLAGS="-Wall -Wno-char-subscripts -funsigned-char" \
		LD=gcc

#-----------------------------------------------------------------------------
# System V generic environment, GNU cc
#-----------------------------------------------------------------------------
sysv-gcc:
	@ echo 'Making BibTeX for System V using GNU cc ... '
	$(MAKE) bibtex-exe CC=gcc \
		CFLAGS="-Wall -Wno-char-subscripts -funsigned-char" \
		LD=gcc
