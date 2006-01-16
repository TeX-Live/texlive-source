# Microsoft Developer Studio Project File - Name="freetype" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **
#
#
# Copyright 1996-2002 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=freetype - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "freetype.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "freetype.mak" CFG="freetype - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "freetype - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "freetype - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /Za /W3 /GX /O1 /I "." /I "../.." /I "../../extend" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"./libttf.lib"

!ELSEIF  "$(CFG)" == "freetype - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Za /W3 /GX /Zi /Od /I "." /I "../.." /I "../../extend" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug/libttf.lib"

!ENDIF 

# Begin Target

# Name "freetype - Win32 Release"
# Name "freetype - Win32 Debug"
# Begin Source File

SOURCE=.\freetype.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxcmap.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxerr18.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxgasp.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxkern.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxpost.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxsbit.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxwidth.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxopen.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxgdef.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxgsub.c
# End Source File
# Begin Source File

SOURCE=..\..\extend\ftxgpos.c
# End Source File
# End Target
# End Project
