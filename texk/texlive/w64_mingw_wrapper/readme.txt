
  Script wrappers in TeX Live on Windows
  
  Rationale
  
    Wrappers enable use of scripts on Windows as regular programs.  
    They are also required for some binary programs to set up the 
    right environment for them. 
    
    Batch scripts can be used for wrapping but they are not as universal 
    as binaries (there are some odd cases where they don't work) and 
    it is hard to make them robust and secure.  Compiled binary wrappers 
    don't suffer from these problems but they are harder to write, debug
    and maintain in comparison to scripts.  For these reasons a hybrid 
    approach is taken that combines a binary stub with a launcher script.
  
  Adding wrappers for user scripts
  
    The script wrapping machinery is not limited to scripts shipped with 
    TeX Live.  You can also use it for script programs from manually 
    installed packages.  This should minimize problems when using them 
    with TeX Live. 
    
    First, make sure that there is an interpreter program available on 
    your system for the script you want to use.  Interpreters for Perl 
    and Lua are bundled with TeX Live, all others have to be installed 
    independently.  Lua scripts are the most efficient to run, so if you
    consider writing a new script, that would be the recommended choice.
    
    The following script types and their file extensions are currently 
    supported and searched in that order:
    
      Lua      (.tlu;.texlua;.lua) --  included
      Perl     (.pl)               --  included
      Ruby     (.rb)               --  requires installation
      Python   (.py)               --  requires installation
      Tcl      (.tcl)              --  requires installation
      Java     (.jar)              --  requires installation
      VBScript (.vbs)              --  part of Windows
      JScript  (.js)               --  part of Windows
      Batch    (.bat;.cmd)         --  part of Windows
    
    Finally, Unix-style extensionless scripts are searched as last and 
    the interpreter program is established based on the she-bang (#!) 
    specification on the very first line of the script.  This can be 
    an arbitrary program but it must be present on the search path.
    
    Next, the script program needs to be installed somewhere below the 
    'scripts' directory under one of the TEXMF trees (consult the 
    documentation or texmf/web2c/texmf.cnf file for a list).  You may 
    need to update the file search database afterwards with:
    
      mktexlsr [TEXMFDIR]
      
    It is also possible to use scripts that are outside of TEXMF hierarchy 
    by adjusting TEXMFSCRIPTS environment or kpathsea variable, see 
    kpathsea documentation for more information on setting its variables.
    
    Test if the script can be located with:
    
      kpsewhich --format=texmfscripts <script-name>.<ext>
    
    This should output the full path to the script if everything is 
    properly installed and configured.  If this test is successful, 
    the script can be run immediately with:
    
      runscript <script-name> [script arguments]
    
    If you prefer to call the script program simply by its name, copy 
    and rename bin/win32/runscript.exe to <script-name>.exe and put it 
    somewhere on the search path.

  Wrapper structure
  
    Wrappers consist of small binary stubs and a common texlua script.  
    The binary stubs are all the same, just different names (but CLI 
    and GUI stubs differ, see below, and GUI stubs are actually all 
    different due to different embedded icons).
    
    The job of the binary stub is twofold: (a) call the texlua launcher
    script 'runscript.tlu' from the same directory (or more precisely 
    from the directory containing 'runscript.dll') and (b) pass to it 
    argv[0] and the unparsed argument string as the last two arguments 
    (after adding a sentinel argument, which ends with a new line 
    character).  Arbitrary C strings can be passed, because the script 
    is executed by linking with luatex.dll and calling the lua 
    interpreter internally rather than by spawning a new process.
    
    There are two flavours of the binary stub: one for CLI programs 
    and another one for GUI programs.  The GUI variant does not open 
    a console window nor does it block the command prompt if started 
    from there.  It also uses a dialog box to display an error message 
    in addition to outputting to stderr.
    
    The stubs are further split into a common DLL and EXE proxies 
    to it.  This is for maintenance reasons - updates can be done by 
    replacement of a single DLL rather than all binary stubs.
    
    The launcher script knows, which variant has been used to invoke it 
    based on the sentinel argument.  The lack of this argument means 
    that it was invoked in a standard way, i.e., through texlua.exe. 
    
    All the hard work of locating a script/program to execute happens 
    in the launcher script.  The located script/program is always 
    executed directly by spawning its interpreter (or binary) in a new 
    process.  The system shell (cmd.exe) is never called (except for 
    batch scripts, of course).  If the located script happens to be 
    a (tex)lua script, it is loaded and called internally from within 
    this script, i.e. no new process is spawned.  Execution is done 
    using a protected call, so any compile or runtime errors are catched.
    
  Source files
  
    runscript.tlu     launcher script for locating and dispatching 
                      target scripts/programs
    runscript_dll.c   common DLL part of the binary stubs; locates and
                      calls the launcher script
    runscript_exe.c   EXE proxy to the common DLL for CLI mode stubs
    wrunscript_exe.c  EXE proxy to the common DLL for GUI mode stubs
  
  Compilation of binaries (requires luatex.dll in the same directory)

    with gcc (size optimized):
    
    x86_64-w64-mingw32-gcc -Os -s -shared -o runscript.dll runscript_dll.c -L./ -ltexlua
    x86_64-w64-mingw32-gcc -Os -s -o runscript.exe runscript_exe.c -L./ -lrunscript
Untested (todo):
    x86_64-w64-mingw32-gcc -mwindows -Os -s -o wrunscript.exe wrunscript_exe.c -L./ -lrunscript

Untested (todo):
    with tcc (extra small size):
    
    tiny_impdef texlua.dll
    tcc -shared -o runscript.dll runscript_dll.c luatex.def
    tcc -o runscript.exe runscript_exe.c runscript.def
    tcc -o wrunscript.exe wrunscript_exe.c runscript.def

  License
  
    Originally written in 2009 by Tomasz M. Trzeciak, Public Domain.
    
    Prior work:
    'tl-w32-wrapper.texlua' by Reinhard Kotucha and Norbert Preining.
    'tl-w32-wrapper.cmd' by Tomasz M. Trzeciak.
  
  Changelog
  
    2009/12/04 
        - initial version
    2009/12/15 
        - minor fixes for path & extension list parsing
    2010/01/09 
        - added support for GUI mode stubs
    2010/02/28 
        - enable GUI mode stubs for dviout, psv and texworks;
        - added generic handling of sys programs
        - added restricted repstopdf to alias_table
    2010/03/13 
        - added 'readme.txt' and changelog
        - added support and docs for calling user added scripts; 
          (use path of 'runscript.dll' instead of .exe stub to 
          locate 'runscript.tlu' script)
        - limit search for shell_escape_commands to system trees
        - added function for creating directory hierarchy 
        - fixed directory creation for dviout & texworks aliases
        - fixed arg[0] of repstopdf & rpdfcrop
    2010/03/28
        - restructured docs, added --help and --version options 
          (available only when invoked under 'runscript' name)
        - use TEXMF_RESTRICTED_SCRIPTS kpse var for searching
          shell_escape_commands
        - changed command validation to handle a list of commands
        - prepend GUI mode command(s) to the command list
        - added support for .tcl scripts
    2010/03/31
        - fixed fatal bug in extention_map definition for GUI mode
    2010/04/15
        - encapsulated main chunk in a function to execute with 
          pcall for more robustness and better error catching
        - added texdoctk to scripts4tlperl table
        - added tlgs and tlperl to alias_table; callable as e.g.: 
          runscript tlperl ...
        - doc tweaks
    2010/04/22
        - ensure only backslash is used in USERPROFILE variable 
          (Adobe Reader crash case)
        - fixed argument processing for direct execution under texlua
        - more doc tweaks
    2010/05/30
        - Windows XP or newer required to run TeXworks
    2010/06/04
        - added support for Perl scripts starting with eval-exec-perl 
          construct in place of she-bang (#!)
    2010/06/25
        - run internal tlperl only with our Perl
        - added fontinst to alias_table
        - added support for all tex4ht commands from mk4ht.pl
        - removed some unsued aliases
        - some code refactoring and cleanup
    2010/12/28
        - use of external Perl now requires kpathsea variable
          TEXLIVE_WINDOWS_TRY_EXTERNAL_PERL to be explicitly set to 1
        - alias_table replaced with if-elseif-end tests to streamline 
          special cases and to avoid hardcoding of texmf* file paths
        - added a2ping to special cases (requires -x switch to Perl)
        - set ASYMPTOTE_GS (for asy) to full path to tlgs
    2011/01/09
        - removed tex4ht commands starting with ht from mk4ht aliases; 
          they have their own scripts and mk4ht calls them internally, 
          so aliasing results in an infinite recursion
        - removed alias for fontinst (no fontinst.exe any more)
        - fixed GUI-mode interpreter for Ruby
    2011/09/10
        - added -dDisableFAPI=true to psview argument list. Needed by
          gs-9.xx
    2012/03/12
        - added '-i', '.' to psview argument list (author's request)
        - added environment clean up from Perl specific variables 
          (when not using external Perl)
    2012/08/05
        - added alias for fmtutil
    2013/05/09
        - added alias mkluatexfontdb -> luaotfload-tool
    2013/07/03
        - fix for psview and UNC paths in unix-style
        - remove not needed is_abs_path function
    2019/02/19
        - patches for cross-compiled texlive with Mingw x86_64 from Ubuntu

