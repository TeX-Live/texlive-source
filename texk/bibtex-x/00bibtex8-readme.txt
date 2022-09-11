******************************************************************************
FILE: 00readme.txt
******************************************************************************


An 8-bit Implementation of BibTeX 0.99 with a Very Large Capacity
=================================================================


Contents
--------

    0.  Abstract

    1.  Introduction
        1.1 8-bit Character Set Support
        1.2 Big and Customisable Capacity

    2.  Distribution Kits and Where to Find Them
        2.1 bt371dos.zip
        2.2 bt371os2.zip
        2.3 bt371src.zip
        2.4 bt371csf.zip

    3.  Running 8-bit BibTeX
        3.1 Command line options
        3.2 Finding Files
        3.3 Environment variables

    4.  The Codepage and Sort Order (CS) File
        4.1 CS file syntax
        4.2 Testing a CS file
        4.3 Sharing your CS file

    5.  Building 8-bit BibTeX from Source Code
        5.1 DOS
        5.2 OS/2
        5.3 Unix
        5.4 VMS

    6.  Reporting Bugs and Requesting Improvements
        6.1 Contacting the authors
        6.2 Further enhancements

    7.  Frequently Asked Questions

    8.  Acknowledgements

    9.  BibTeX Use and Copying Conditions

   10.  Change Log



0. Abstract
-----------

This abstract is in a format suitable for inclusion in BBS description files
(file_id.diz):

    (BibTeX8) "big" BibTeX with full 8-bit support

    An enhanced, portable C version of BibTeX.  
    Enhanced by conversion to "big" (32-bit) 
    capacity, addition of run-time selectable 
    capacity and 8-bit support extensions.
    National character set and sorting order
    are controlled by an external configuration 
    file.  Various examples are included.
    Freeware / GNU Public Licence.
    
    Niel Kempson <kempson@snowyowl.co.uk>
    Alejandro Aguilar-Sierra <asierra@servidor.unam.mx>



1.  Introduction
----------------

8-bit BibTeX is an enhanced, portable C version of BibTeX 0.99.  It has been
enhanced in these areas:

  - conversion to "big" (32-bit) capacity
  - capacity selectable at run time
  - flexible support for non-English languages using 8-bit character sets
  - well matched to LateX2e and its "inputenc" package

Oren Patashnik, the creator of BibTeX, is working on a new BibTeX 1.0 that
will be a modern implementation supporting large capacities and non-English
languages (see TUGboat, pages 269--274, volume 15, number 3, September 1994).
He is content for this version to be released, but hopes that people will
eventually migrate to BibTeX 1.0 when it is released.  Its release date is
uncertain at the moment.


    1.1 Big and Customisable Capacity
    ---------------------------------

    The original 16-bit code has been rewritten to use 32-bit data types
    wherever possible.  The result is a very large potential capacity.  To
    increase flexibility and to avoid BibTeX allocating all available memory,
    the capacity of some key arrays can be set on the command line.
    
    For convenience, several standard capacities have been predefined:
    default, big and huge.  The key capacities are set as follows:

        Parameter       Standard       --big      --huge  --wolfgang
        ------------------------------------------------------------
        Hash_Prime         4,253       8,501      16,319      30,011
        Hash_Size          5,000      10,000      19,000      35,000
        Max_Cites            750       2,000       5,000       7,500
        Max_Ent_Ints       3,000       4,000       5,000       7,500
        Max_Ent_Strs       3,000       6,000      10,000      10,000
        Max_Fields        17,250      30,000      85,000     125,000
        Max_Strings        4,000      10,000      19,000      30,000
        Pool_Size         65,530     130,000     500,000     750,000
        Wiz_Fn_Space       3,000       6,000      10,000      10,000

    If these parameter names mean nothing to you, don't worry, you've
    probably never exceeded BibTeX's capacity.


    1.2 8-bit Character Set Support
    -------------------------------

    BibTeX now accepts 8-bit characters in its input files and writes 8-bit
    characters to its output files.  The character set is defined by an
    external configuration text file - the codepage and sort order ("CS")
    file.

    The sort order can be defined for the language and character set.  For
    example, in German, the control sequence \"o (o umlaut) should be sorted
    as if it were the letter "o", but after ordinary "o", leading to this
    order:

        Trofer, Tr\"ofer, Trufer

    However, in Swedish, \"o (o umlaut) is treated as the 29th letter of the
    alphabet and these entries would be sorted as:

        Trofer, Trufer, Tr\"ofer

    The sorting order is defined by an external configuration text file  -
    the codepage and sort order ("CS") file.

    This version of BibTeX, coupled with LaTeX2e and its "inputenc" package
    provide a robust means of handling 8-bit character sets.
    


2.  Distribution Kits and Where to Find Them
--------------------------------------------

Note by TeX Live team - This section is an old description by the original
author Niel Kempson. Currently (2022) BibTeX8 is shipped with TL.

8-bit BibTeX is distributed as a set of ZIP files created by the freely
available implementation of ZIP by the Info-ZIP project.  The files have all
been compressed using the new "deflation" algorithm and can only be
compressed using the Info-ZIP implementation of UNZIP, or PKUNZIP v2.04 or
later.  Ancient versions of PKUNZIP (e.g. v1.10) will not be able to unZIP
the files and will complain with a message like: "PKUNZIP: Warning! I don't
know how to handle:  xxxxxxxx.xxx".

The "official" version of 8-bit BibTeX will be available by anonymous FTP
from the Comprehensive TeX Archive Network (CTAN) sites:

        ftp.tex.ac.uk:/tex-archive/biblio/bibtex/8-bit
        ftp.uni-stuttgart.de:/tex-archive/biblio/bibtex/8-bit

There are four main ZIP files in the complete 8-bit BibTeX distribution:

    bt###dos.zip    bt###os2.zip    bt###src.zip    bt###csf.zip
    
where ### is the latest version, currently 371 (for v3.71).  Their contents
and purpose are summarised below.


    2.1 bt371dos.zip
    ----------------

    The binary + documentation kit for MS-DOS users.  It contains all you
    need to run 8-bit BibTeX under MS-DOS, but no source code.  The
    executable program uses a 32-bit virtual memory extender called EMX to
    run in 32-bit mode.  emTeX is probably the most comprehensive and capable
    implementation of TeX for DOS & OS/2 and it too uses EMX.
    
    The kit contains these files:

        00readme.txt        this file
        COPYING             GNU copyright notice
        HISTORY             summary of changes made so far
        csfile.txt          documentation for codepage and sort order
                            ("CS") files
        msdos/bibtex.exe    the 8-bit BibTeX program

    EMX uses the VCPI mechanism to run in 32-bit mode and will therefore not
    run in a DOS session under MS Windows.  It will run under native DOS and
    in a DOS session under OS/2.  The EMX extender (v0.9b) is bound into
    bibtex.exe, but you can always obtain the latest version of EMX by
    anaonymous FTP from:
    
        ftp.uni-stuttgart.de:/pub/systems/os2/emx-0.9b
        ftp.leo.org:/pub/comp/os/os2/gnu/emx+gcc

    If you want to run this version of BibTeX in a DOS session under MS
    Windows, you have two choices:
    
      - get and install the RSX extender.  RSX is a DPMI-compliant DOS
        extender which is more or less compatible with emx.  It is compatible
        with EMX, DPMI servers and DOS sessions under MS Windows.  The
        official site for the latest version of RSX is

            ftp.uni-bielefeld.de:/pub/systems/msdos/misc
        
        but you can also get it by anonymous FTP from the same CTAN sites as
        BibTeX:
            
            ftp.tex.ac.uk:/tex-archive/systems/msdos/dpmigcc
            ftp.uni-stuttgart.de:/tex-archive/systems/msdos

        The files to look for are dpmigcc5.zip and rsxwin3a.zip

        If you use the emTeX implementation of TeX and friends, it may be
        more convenient to get the "emxrsx" package - it's a minimal version
        of RSX to allow emTeX to run in DOS sessions under MS Windows.  It is
        available by anonymous FTP from CTAN sites in the emTeX directory:

            ftp.tex.ac.uk:/tex-archive/systems/msdos/emtex
            ftp.uni-stuttgart.de:/tex-archive/systems/msdos/emtex

        On the other hand, if you're using emTeX under MS Windows, you've
        probably solved the problem already :-)

      - get the djgpp GNU C compiler and DOS extender package and build
        BibTeX from the source code.  The master site for djgpp is
        ftp.delorie.com.


    2.2 bt371os2.zip
    ----------------

    The binary + documentation kit for OS/2 2.x and 3.x users.  It contains
    all you need to run 8-bit BibTeX under OS/2, but no source code.  The
    executable program was developed using the EMX development system and has
    its run-time library linked in - there is no need to install EMX just to
    run BibTeX.  If you already have EMX installed want a smaller executable,
    you'll need to rebuild from sources.
    
    The kit contains these files:

        00readme.txt        this file
        COPYING             GNU copyright notice
        HISTORY             summary of changes made so far
        csfile.txt          documentation for codepage and sort order
                            ("CS") files
        os2/bibtex.exe      the 8-bit BibTeX program


    2.3 bt371src.zip
    ----------------
    
    The complete source code kit to build 8-bit BibTeX on all supported
    systems.  The source code is known to build easily under MS-DOS and OS/2
    if you have the GNU C Compiler installed.  The source code is quite
    portable and contains almost no system-specific items - it should very
    easily port to Unix or VMS.  If you decide to port 8-bit BibTeX to
    another platform, please let me know so that I can include your efforts
    in the master distribution.
    
    The source kit contains these files:

        00readme.txt        this file
        COPYING             GNU copyright notice
        HISTORY             summary of changes made so far
        csfile.txt          documentation for codepage and sort order
                            ("CS") files
    
        bibtex.h            definition of global parameters and limits
        datatype.h          definition of custom data types
        gblprocs.h          forward declaration of all global functions
        gblvars.h           declaration of global variables
        sysdep.h            determine the compiler and environment
    
        bibtex.c            main source including system dependent code
        bibtex-1.c          part 1 of the system independent functions
        bibtex-2.c          part 2 of the system independent functions
        bibtex-3.c          part 3 of the system independent functions
        bibtex-4.c          part 4 of the system independent functions
    
        utils.c             nearly all of the enhancement code
        utils.h
    
        getopt.c            the GNU getopt package for command line parsing
        getopt1.c
        getopt.h
    
        dos-emx.mak         makefile for EMX GNU C under MS-DOS
        dos-dj.mak          makefile for DJGPP GNU C under MS-DOS
        os2.mak             makefile for EMX GNU C under OS/2
        unix.mak            makefile for Unix variants


    2.4 bt371csf.zip
    ----------------

    The character set and sorting order is now defined by an external
    configuration text file - the codepage and sort order ("CS") file.  A
    number of example files have been included with the master distribution,
    but I hope that others will be written by 8-bit BibTeX users in due
    course.  To avoid the need to reissue to the master ZIP files every time
    a new CS file is created, CS files will also be available as a separate
    kit.
    
    At the time of writing, the kit contains these CS files:

        File Name           Character Set               Sorting Order
        -------------------------------------------------------------
        88591lat.csf        ISO 8859-1                  Latin
        88591sca.csf        ISO 8859-1                  Scandinavean
        ascii.csf           US ASCII                    English
        cp437lat.csf        IBM codepage 437            Latin
        cp850lat.csf        IBM codepage 850            Latin
        cp850sca.csf        IBM codepage 850            Scandinavean
        cp866rus.csf        IBM codepage 866            Russian



3.  Running 8-bit BibTeX
------------------------

Running 8-bit BibTeX is superficially the same as running the original
BibTeX, but there are quite a number of command line options that may be
specified.


    3.1 Command line options
    ------------------------

    The command line syntax is:
    
        bibtex [options] aux-file

    where "aux-file" is the name of the TeX auxilliary output file to be
    processed by BibTeX.  The trailing ".aux" may be omitted.

    Valid options are:

        -?  --help              

                Display some brief help text and then exit.

        -7  --traditional

                Operate in the original 7-bit mode.  A CS file is not read:
                only 7-bit ASCII characters are supported and sorting is
                strictly by ASCII code value.
                
                BibTeX will not allow you to specify --traditional with
                either the --8bit or --csfile option.

        -8  --8bit
        
                Force 8-bit mode.  A CS file is not read.  All 8-bit
                characters (code > 127) are treated as letters and sorting is
                strictly by code page value.
                
                BibTeX will not allow you to specify --8bit with either the
                --csfile or --traditional option.

        -c  --csfile FILE
        
                Read FILE as the BibTeX codepage and sort definition (CS)
                file.  The CS file is used to define the 8-bit character set
                used by BibTeX and the order in which those characters should
                be sorted.

                BibTeX will not allow you to specify --csfile with either
                the --8bit or --traditional option.

        -d  --debug TYPE
        
                Report debugging information to the BibTeX log file and the
                standard error device.  The value TYPE controls the type of
                debugging information reported.  TYPE can be one or more of:

                    all     - all debugging categories
                    csf     - CS file processing
                    io      - file I/O
                    mem     - memory allocation and capacity
                    misc    - other debugging information
                    search  - path searching and file location

                It is possible that your version of BibTeX has been compiled
                with debugging support disabled.  If this is the case, BibTeX
                will issue a warning message when --debug is specified.

        -s  --statistics
        
                Report internal statistics to the BibTeX log file.

                It is possible that your version of BibTeX has been compiled
                with statistics support disabled.  If this is the case,
                BibTeX will issue a warning message when --statistics is
                specified.

        -t  --trace

                Report execution tracing to the BibTeX log file.

                It is possible that your version of BibTeX has been compiled
                with tracing support disabled.  If this is the case, BibTeX
                will issue a warning message when --trace is specified.

        -v  --version

                Report BibTeX's version and then exit.

        -B  --big

                Set BibTeX's capacity to "big".  The size of particular
                parameters will be set as follows (the default sizes are
                shown in brackets):

                    Hash_Prime            8,501   (4,253) 
                    Hash_Size            10,000   (5,000) 
                    Max_Cites             2,000     (750) 
                    Max_Ent_Ints          4,000   (3,000) 
                    Max_Ent_Strs          6,000   (3,000) 
                    Max_Fields           30,000  (17,250) 
                    Max_Strings          10,000   (4,000) 
                    Pool_Size           130,000  (65,530) 
                    Wiz_Fn_Space          6,000   (3,000) 

        -H  --huge

                Set BibTeX's capacity to "huge".  The size of particular
                parameters will be set as follows (the default sizes are
                shown in brackets):

                    Hash_Prime           16,319   (4,253) 
                    Hash_Size            19,000   (5,000) 
                    Max_Cites             5,000     (750) 
                    Max_Ent_Ints          5,000   (3,000) 
                    Max_Ent_Strs         10,000   (3,000) 
                    Max_Fields           85,000  (17,250) 
                    Max_Strings          19,000   (4,000) 
                    Pool_Size           500,000  (65,530) 
                    Wiz_Fn_Space         10,000   (3,000) 

        -W  --wolfgang

                Set BibTeX's capacity to "really huge" - required for
                Wolfgang's PhD thesis.  The size of particular parameters
                will be set as follows (the default sizes are shown in
                brackets):

                    Hash_Prime           30,011   (4,253) 
                    Hash_Size            35,000   (5,000) 
                    Max_Cites             7,500     (750) 
                    Max_Ent_Ints          7,500   (3,000) 
                    Max_Ent_Strs         10,000   (3,000) 
                    Max_Fields          125,000  (17,250) 
                    Max_Strings          30,000   (4,000) 
                    Pool_Size           750,000  (65,530) 
                    Wiz_Fn_Space         10,000   (3,000) 

        -M  --min_crossrefs ##

                Set min_crossrefs to ##.  If an item is cross-referenced at
                least ## times, it will be placed in the list of citations,
                even if it is not explicitly \cited as a reference.  The
                default value is 2.

        --mcites ##

                Allow a maximum of ## distinct \cites in the .aux files.
                This number must be less than the maximum number of strings
                (settable with --mstrings).

        --mentints ##

                Allow a maximum of ## integer entries in the .bib databases.

        --mentstrs ##

                Allow a maximum of ## string entries in the .bib databases.

        --mfields ##

                Allow a maximum of ## fields in the .bib databases.

        --mpool ##

                Set the string pool to ## bytes.

        --mstrings ##

                Allow a maximum of ## unique strings.  This number must be
                less than the hash size and greater than the maximum number
                of \cites (settable with --mcites).

        --mwizfuns ##
        
                Allow a maximum of ## wizard functions.


    3.2 Finding Files
    -----------------

    8-bit BibTeX looks for input files in three different steps as summarised
    below.  As soon as 8-bit BibTeX finds a matching file, it stops looking
    (i.e. only the first matching file is used).  In order, the three steps
    are:

      - look for the file in the current working directory.
      
      - if the appropriate environment variable has been set (e.g. BSTINPUT),
        treat its value as a list of directories to be searched.  Look for the
        file in each of the directories in the list.
        
      - if the appropriate environment variable (e.g. BSTINPUT) has not been
        set, use a predefined "fallback" path as a list of directories to be
        searched.

    A search list consists of a number of directories separated by a delimiter
    (semicolons for MS-DOS & OS/2, colons for Unix and commas for VMS).

    As distributed, the strategy adopted by 8-bit BibTeX for opening specific
    file types is

        .aux files

          - look in current working directory only

        .bib (BibTeX database) files

          - look in current working directory
          - search along path defined by environment variable BIBINPUT
          - search along fallback path (empty by default)
        
        .bst (BibTeX style) files

          - look in current working directory
          - search along path defined by environment variable BSTINPUT
          - search along fallback path (empty by default)

        .csf (CS) files

            - look in current working directory
            - search along path defined by environment variable CSFINPUT
            - search along fallback path (empty by default)

            The name of the CS file is determined using a number of steps
        
              - use value of --csfile command line option,
              - use value of the BIBTEX_CSFILE environment variable
              - use fallback CS file name (empty by default)

        Output files

            All of BibTeX's output (.bbl, .blg) files are created in the
            current working directory.


    The environment variables and fallback paths used by 8-bit BibTeX are
    defined in the Makefile and set at compile time, but you can determine
    what your version of 8-bit BibTeX is using by starting it with the command
    line
    
        bibtex --debug=search non-existent-file-name
        
    The debugging output (written to the standard error device) should look
    something like:

        D-SCH: Search strategy for .aux files:
        D-SCH:   search path environment variable: <undefined>
        D-SCH:   fallback search path: <undefined>
        D-SCH: Search strategy for .bib files:
        D-SCH:   search path environment variable: BIBINPUT
        D-SCH:   BIBINPUT value: e:\usr\c\bibtex
        D-SCH:   fallback search path: e:/usr/latex/bibtex;e:/emtex/bibtex/bib
        D-SCH: Search strategy for .bst files:
        D-SCH:   search path environment variable: BSTINPUT
        D-SCH:   BSTINPUT value: <undefined>
        D-SCH:   fallback search path: e:/usr/latex/bibtex;e:/emtex/bibtex/bst
        D-SCH: Search strategy for .csf files:
        D-SCH:   search path environment variable: CSFINPUT
        D-SCH:   CSFINPUT value: e:\usr\c\bibtex
        D-SCH:   fallback search path: e:/usr/latex/bibtex;e:/emtex/bibtex/csf
        D-SCH: Default .csf file:
        D-SCH:   file name environment variable: BIBTEX_CSFILE
        D-SCH:   BIBTEX_CSFILE value: e:/emtex/texinput/cp437lat.csf
        D-SCH:   fallback file name: cp850lat.csf
    
    The current working directory will always be searched, even if the
    environment variable and fallback paths have not been specified (e.g. as
    for .aux files in the above example).

    If the default behaviour is not to your liking, you will need to rebuild
    8-bit BibTeX from its source code (see section 5).

    
    3.3 Environment variables
    -------------------------

    As supplied, 8-bit BibTeX uses a number of environment variables:

        BIBINPUT        search path for database (.bib) files
        BSTINPUT        search path for style (.bst) files
        CSFINPUT        search path for CS (.csf) files
        BIBTEX_CSFILE   the default CS file
        TMP             directory for virtual memory files (DOS only)

    The name of the environment variables used may be changed (in the
    Makefile) when BibTeX is built.  See the appropriate Makefile for your
    environment and remember that you can use the "--debug=search" command
    line option to reveal the environment variables used by your version of
    8-bit BibTeX (see previous section).

    All environment variables used as a search list (BIBINPUT, BSTINPUT &
    CSFINPUT) can be set to a number of separate directories, separated
    by a delimiter (semicolons for MS-DOS & OS/2, colons for Unix and
    commas for VMS).

    Examples for MS-DOS and OS/2 are:
    
        SET BIBINPUT=e:\data\tex\bibtex;c:\emtex\bibtex\bib
        SET BSTINPUT=e:\data\tex\bibtex;c:\emtex\bibtex\bst
        SET CSFINPUT=e:\data\tex\bibtex
        SET BIBTEX_CSFILE=c:\data\tex\bibtex\cp850lat.csf

    Examples for Unix:

        setenv BIBINPUT /u/kempson/bibtex:/usr/local/lib/tex/bib-files
        setenv BSTINPUT /u/kempson/bibtex:/usr/local/lib/tex/bst-files
        setenv CSFINPUT /u/kempson/bibtex:/usr/local/lib/tex/csf-files
        setenv BIBTEX_CSFILE /usr/local/lib/tex/csf-files/88591lat.csf

    Examples for VMS:

        define BIBINPUT "sys$login:,disk$tex:[bibtex.bib-files]"
        define BSTINPUT "sys$login:,disk$tex:[bibtex.bst-files]"
        define CSFINPUT "sys$login:,disk$tex:[bibtex.csf-files]"
        define BIBTEX_CSFILE disk$tex:[bibtex.csf-files]88951lat.csf



4.  The Codepage and Sort Order (CS) File
-----------------------------------------

The Codepage and Sort definition (CS) file is used to define the 8-bit
character set used by BibTeX and the order in which those characters should
be sorted.  

Please see the associated csfile.txt for details of CS file syntax and
guidelines for testing new CS files.  NOTE: it contains many 8-bit characters
and may cause problems if you try to display or print it on 7-bit systems
(e.g. older versions of Unix).



5.  Building 8-bit BibTeX from Source Code
------------------------------------------

The 8-bit BibTeX source is fairly standard ANSI C with almost no system
specific code.  It should therefore be relatively straightforward to build it
in a different environment if you have GNU C or an ANSI C compiler.

A number of Makefiles have been provided to build 8-bit BibTeX from source
code:

    dos-emx.mak         makefile for EMX GNU C under MS-DOS
    dos-dj.mak          makefile for DJGPP GNU C under MS-DOS
    os2.mak             makefile for EMX GNU C under OS/2
    unix.mak            makefile for Unix variants

Whichever Makefile you use, you need to check that the "local definitions"
are appropriate for your system.  There are three small sections to
customise:

  BibTeX File Searching
    
    - specifies the names of environment variables and paths to be used when
      searching for input files

  Utility Programs

    - specifies the names of programs to be used for simple functions

  Compiler/Linker

    - specifies compiler/linker command lines

If your system is already supported it should not be necessary to modify any
part of the Makefile except these three sections.

Brief notes for specific environments follow.


    5.1 DOS
    -------

        8-bit BibTeX has been built and tested using the EMX and DJGPP
        development environments.  Both are based on GNU C with their own
        custom 32-bit extenders.


    5.2 OS/2
    --------

        The EMX development environment is supported.


    5.3 Unix
    --------

        Most variants of Unix supporting GNU C should be capable of building
        and running 8-bit BibTeX.


    5.4 VMS
    -------

        The authors have not built or tested this version of 8-bit BibTeX on
        VMS, but see no reason why it shouldn't compile and run successfully
        if GNU C is installed on the system.  (The only system-dependent code
        in 8-bit BibTeX concerns file opening and provision has been made for
        VMS file modes.)

        If you have GNU C installed, we recommend starting with a copy of the
        unix.mak Makefile and customising it for VMS.  If you successfully
        get 8-bit BibTeX running under VMS, *please* let the authors know how
        you did it.



6.  Reporting Bugs and Requesting Improvements
----------------------------------------------

Where possible, we will try to fix bugs and will consider requests for
improvements.  If you are reporting a bug, please provide as much information
as possible (e.g. operating environment, 8-bit BibTeX version and source,
exact error message and the offending files if possible).  
    
The most common message is of the form "BibTeX doesn't work on XXXX".  This
is generally of no help in debugging a problem so please provide as much
information as possible.


    6.1 Contacting the authors
    --------------------------
    
    The authors are

        Niel Kempson
        Snowy Owl Systems Limited, Cheltenham, England
        E-mail: kempson@snowyowl.co.uk

    and 

        Alejandro Aguilar-Sierra
        Centro de Ciencias de la Atm\'osfera, 
        Universidad Nacional Aut\'onoma de M\'exico, M\'exico
        E-mail: asierra@servidor.unam.mx

    Niel Kempson did the original manual translation from WEB to C,
    conversion to "big" (32-bit) capacity, addition of run-time selectable
    capacity and part of the 8-bit support extensions.  He intermittently
    maintains the master version of the source code.
    
    Alejandro Aguilar-Sierra should take the credit for most of the 8-bit
    function provided by this version of BibTeX.


    6.2 Further Enhancements
    ------------------------

    No program is perfect and this version of BibTeX is no exception to that
    rule.  Some known weaknesses are:
    
      - the 8-bit support is currently limited to single 8-bit characters.
        TeX control sequences (e.g. \'{A}) are not interpreted and treated in
        the same way as the equivalent 8-bit character.
            
      - it is not possible to redefine the lower 128 character codes.  This
        excludes support for character codes not built on ASCII (e.g. EBCDIC).

    Future enhancements will be considered, but it may be time to build a
    "proper" system built from the ground up to handle 8-bit character
    sets.  BibTeX 1.0 promises to be this system.

    Other items that ought to be on the "to do" list:
    
      - path searching using Karl Berry's kpathsea package
      - TeX format documentation
      - Unix man pages
      - OS/2 .inf format documentation
      - native 32-bit support for MS Windows 95/NT



7.  Frequently Asked Questions
------------------------------

When I run the DOS version 8-bit BibTeX in a DOS window under MS Windows 3.x,
95 or NT, I get the message "DPMI not supported".

    8-bit BibTeX has been built using the EMX development environment.  It
    uses the VCPI mechanism to run in 32-bit mode and will therefore not run
    in a DOS session under MS Windows.
    
    If you want to run this version of 8-bit BibTeX in a DOS session under
    MS Windows, you have two choices:
    
      - get and install the RSX extender
      - rebuild from sources using a suitable compiler

    See section 2.1 for more information.


8-bit BibTeX doesn't find my .bib/.bst files.  How can I find out where it
looks for them?

    See section 3.2


There isn't a CS file for my character set / language sorting order

    Creating a new CS file should be relatively straightforward.  If you'd
    like to try, please contact the authors for assistance.  If you don't
    need assistance, *please* let us have a copy of your finished CS file so
    we can include it in the distribution.



8.  Acknowledgement
-------------------

The original BibTeX was written by Oren Patashnik using Donald Knuth's WEB
system.  This format produces a PASCAL program for execution and a TeX
documented version of the source code. This program started as a (manual)
translation of the WEB source into C.



9.  BibTeX Use and Copying Conditions
-------------------------------------

The programs currently being distributed that relate to 8-bit BibTeX are
*free*;  this means that everyone may use them and redistribute them freely.
The 8-bit BibTeX-related programs are not in the public domain;  they are
copyrighted and there are restrictions on their distribution, but these
restrictions are designed to permit everything that a good cooperating
citizen would want to do.  What is not allowed is to try to prevent others
from further sharing any version of these programs that they might get from
you.

Specifically, we want to make sure that you have the right to give away
copies of the programs that relate to 8-bit BibTeX, that you receive source
code or else can get it if you want it, that you can change these programs or
use pieces of them in new free programs, and that you know you can do these
things.

To make sure that everyone has such rights, we have to forbid you to deprive
anyone else of these rights.  For example, if you distribute copies of the
8-bit BibTeX related programs, you must give the recipients all the rights
that you have.  You must make sure that they, too, receive or can get the
source code.  And you must tell them their rights.

Also, for our own protection, we must make certain that everyone finds out
that there is no warranty for the programs that relate to 8-bit BibTeX.  If
these programs are modified by someone else and passed on, we want their
recipients to know that what they have is not what we distributed, so that
any problems introduced by others will not reflect on our reputation.

The precise conditions of the licences for the programs currently being
distributed that relate to 8-bit BibTeX are found in the General Public
Licences that accompany them.



10.  Change Log
---------------

For later changes by TeX Live, please refer to HISTORY.

Revision 3.71  1996/08/18  20:38:55  kempson
Official release 3.71 (see HISTORY for details).

Revision 3.70  1996/04/29  20:17:53  kempson
Final documentation & cosmetic changes for official release 3.70.

Revision 1.2  1995/10/21  22:23:01  kempson
Updated for v3.60 beta.  Added description of --wolfgang option.
Added some more information on running BibTeX in a Windows DOS session.
Changed the example of CSF debugging output to something that is correct.

Revision 1.1  1995/09/24  20:50:00  kempson
Updated for the final beta test release.

Revision 1.0  1995/09/24  20:42:30  kempson
Placed under RCS control

******************************** END OF FILE *******************************
