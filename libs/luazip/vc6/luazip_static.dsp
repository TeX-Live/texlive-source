# Microsoft Developer Studio Project File - Name="luazip_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=luazip_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "luazip_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "luazip_static.mak" CFG="luazip_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "luazip_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "luazip_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "luazip_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib/vc6"
# PROP Intermediate_Dir "luazip_static/Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../lua/include" /I "../../zlib/include" /I "../zziplib-0.12.83" /I "../../compat" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x416 /d "NDEBUG"
# ADD RSC /l 0x416 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/vc6/libzip.lib"

!ELSEIF  "$(CFG)" == "luazip_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib/vc6"
# PROP Intermediate_Dir "luazip_static/Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../lua/include" /I "../../zlib/include" /I "../zziplib-0.12.83" /I "../../compat" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x416 /d "_DEBUG"
# ADD RSC /l 0x416 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/vc6/libzipd.lib"

!ENDIF 

# Begin Target

# Name "luazip_static - Win32 Release"
# Name "luazip_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\compat\compat-5.1.c"
# End Source File
# Begin Source File

SOURCE=..\luazip.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\compat\compat-5.1.h"
# End Source File
# Begin Source File

SOURCE=..\luazip.h
# End Source File
# End Group
# Begin Group "zziplib Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\dir.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\err.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\file.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\info.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\plugin.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\stat.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\write.c"
# End Source File
# Begin Source File

SOURCE="..\zziplib-0.12.83\zzip\zip.c"
# End Source File
# End Group
# End Target
# End Project
