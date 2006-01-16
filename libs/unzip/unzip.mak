# Microsoft Developer Studio Generated NMAKE File, Based on unzip.dsp
!IF "$(CFG)" == ""
CFG=unzip - Win32 Debug
!MESSAGE No configuration specified. Defaulting to unzip - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "unzip - Win32 Release" && "$(CFG)" != "unzip - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "unzip.mak" CFG="unzip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "unzip - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "unzip - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "unzip - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\unzip.lib"


CLEAN :
	-@erase "$(INTDIR)\api.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crctab.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\extract.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\match.obj"
	-@erase "$(INTDIR)\nt.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\ttyio.obj"
	-@erase "$(INTDIR)\unreduce.obj"
	-@erase "$(INTDIR)\unshrink.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\zipinfo.obj"
	-@erase "$(OUTDIR)\unzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\unzip.lib" 
LIB32_OBJS= \
	"$(INTDIR)\api.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crctab.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\extract.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\match.obj" \
	"$(INTDIR)\nt.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\ttyio.obj" \
	"$(INTDIR)\unreduce.obj" \
	"$(INTDIR)\unshrink.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\zipinfo.obj"

"$(OUTDIR)\unzip.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "unzip - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\unzip.lib"


CLEAN :
	-@erase "$(INTDIR)\api.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crctab.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\extract.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\inflate.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\match.obj"
	-@erase "$(INTDIR)\nt.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\ttyio.obj"
	-@erase "$(INTDIR)\unreduce.obj"
	-@erase "$(INTDIR)\unshrink.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32.obj"
	-@erase "$(INTDIR)\windll.obj"
	-@erase "$(INTDIR)\zipinfo.obj"
	-@erase "$(OUTDIR)\unzip.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NDEBUG" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\unzip.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\unzip.lib" 
LIB32_OBJS= \
	"$(INTDIR)\api.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crctab.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\extract.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\inflate.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\match.obj" \
	"$(INTDIR)\nt.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\ttyio.obj" \
	"$(INTDIR)\unreduce.obj" \
	"$(INTDIR)\unshrink.obj" \
	"$(INTDIR)\win32.obj" \
	"$(INTDIR)\windll.obj" \
	"$(INTDIR)\zipinfo.obj"

"$(OUTDIR)\unzip.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("unzip.dep")
!INCLUDE "unzip.dep"
!ELSE 
!MESSAGE Warning: cannot find "unzip.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "unzip - Win32 Release" || "$(CFG)" == "unzip - Win32 Debug"
SOURCE=.\api.c

"$(INTDIR)\api.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\crc32.c

"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\crctab.c

"$(INTDIR)\crctab.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\crypt.c

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\explode.c

"$(INTDIR)\explode.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\extract.c

"$(INTDIR)\extract.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fileio.c

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\globals.c

"$(INTDIR)\globals.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\inflate.c

"$(INTDIR)\inflate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\list.c

"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\match.c

"$(INTDIR)\match.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\win32\nt.c

!IF  "$(CFG)" == "unzip - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\nt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "unzip - Win32 Debug"

CPP_SWITCHES=/MTd /W3 /GX /ZI /Od /I "../" /I "./" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NDEBUG" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\nt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\process.c

"$(INTDIR)\process.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ttyio.c

"$(INTDIR)\ttyio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unreduce.c

"$(INTDIR)\unreduce.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unshrink.c

"$(INTDIR)\unshrink.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\win32\win32.c

!IF  "$(CFG)" == "unzip - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "unzip - Win32 Debug"

CPP_SWITCHES=/MTd /W3 /GX /ZI /Od /I "../" /I "./" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NDEBUG" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\windll\windll.c

!IF  "$(CFG)" == "unzip - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "unzip - Win32 Debug"

CPP_SWITCHES=/MTd /W3 /GX /ZI /Od /I "../" /I "./" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "NDEBUG" /D "_WINDOWS" /D "WINDLL" /D "USE_EF_UT_TIME" /D "DLL" /D "UNZIPLIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\windll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\zipinfo.c

"$(INTDIR)\zipinfo.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

