# Microsoft Developer Studio Project File - Name="T1_Static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=T1_Static - Win32 Debug MD
!MESSAGE これは有効なメイクファイルではありません。 このプロジェクトをビルドするためには NMAKE を使用してください。
!MESSAGE [メイクファイルのエクスポート] コマンドを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "T1_Static.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE コマンド ライン上でマクロの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "T1_Static.mak" CFG="T1_Static - Win32 Debug MD"
!MESSAGE 
!MESSAGE 選択可能なビルド モード:
!MESSAGE 
!MESSAGE "T1_Static - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "T1_Static - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE "T1_Static - Win32 Release MD" ("Win32 (x86) Static Library" 用)
!MESSAGE "T1_Static - Win32 Debug MD" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "T1_Static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Static_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /Fd"Lib\T1_Static.pdb" /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "T1_Static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Static_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /Zi /Od /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /Fd"Lib/T1_Staticd.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\T1_Staticd.lib"

!ELSEIF  "$(CFG)" == "T1_Static - Win32 Release MD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "T1_Static___Win32_Release_MD"
# PROP BASE Intermediate_Dir "T1_Static___Win32_Release_MD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "StaticMD_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Zi /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /Fd"Lib\T1_StaticMD.pdb" /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\T1_StaticMD.lib"

!ELSEIF  "$(CFG)" == "T1_Static - Win32 Debug MD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "T1_Static___Win32_Debug_MD"
# PROP BASE Intermediate_Dir "T1_Static___Win32_Debug_MD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "StaticMD_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /Fd"Lib/T1_Staticd.pdb" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /Fd"Lib/T1_StaticMDd.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Lib\T1_Staticd.lib"
# ADD LIB32 /nologo /out:"Lib\T1_StaticMDd.lib"

!ENDIF 

# Begin Target

# Name "T1_Static - Win32 Release"
# Name "T1_Static - Win32 Debug"
# Name "T1_Static - Win32 Release MD"
# Name "T1_Static - Win32 Debug MD"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\lib\type1\arith.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\curves.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fontfcn.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\hints.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\lines.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\objects.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\parseAFM.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\paths.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\regions.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\scanfont.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\spaces.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1aaset.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1afmtool.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1base.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1delete.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1enc.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1env.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1finfo.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\t1io.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1load.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1outline.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1set.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\t1snap.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\t1stub.c
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1trans.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\token.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\type1.c
# End Source File
# Begin Source File

SOURCE=..\lib\type1\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\lib\type1\arith.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\blues.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\cluts.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\curves.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\digit.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\ffilest.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\font.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fontfcn.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fontfile.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fontmisc.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fonts.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fontstruct.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fontxlfd.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\fsmasks.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\hdigit.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\hints.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\lines.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\objects.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\parseAFM.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\paths.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\paths_rmz.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\pictures.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\regions.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\spaces.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\spaces_rmz.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\strokes.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\sysconf.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1aaset.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1afmtool.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1base.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1delete.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1enc.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1env.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1extern.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1finfo.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1global.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\t1hdigit.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\t1imager.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1load.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1misc.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1outline.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1set.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\t1stdio.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1trans.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1types.h
# End Source File
# Begin Source File

SOURCE=..\lib\t1lib\t1x11.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\token.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\tokst.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\trig.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\util.h
# End Source File
# Begin Source File

SOURCE=..\lib\type1\Xstuff.h
# End Source File
# End Group
# End Target
# End Project
