# Microsoft Developer Studio Project File - Name="ABrowser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** �ҏW���Ȃ��ł������� **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ABrowser - Win32 Debug
!MESSAGE ����͗L����Ҳ�̧�قł͂���܂���B ������ۼު�Ă�����ނ��邽�߂ɂ� NMAKE ���g�p���Ă��������B
!MESSAGE [Ҳ�̧�ق̴���߰�] ����ނ��g�p���Ď��s���Ă�������
!MESSAGE 
!MESSAGE NMAKE /f "ABrowser.mak".
!MESSAGE 
!MESSAGE NMAKE �̎��s���ɍ\�����w��ł��܂�
!MESSAGE ����� ײݏ��ϸۂ̐ݒ���`���܂��B��:
!MESSAGE 
!MESSAGE NMAKE /f "ABrowser.mak" CFG="ABrowser - Win32 Debug"
!MESSAGE 
!MESSAGE �I���\������� Ӱ��:
!MESSAGE 
!MESSAGE "ABrowser - Win32 Release" ("Win32 (x86) Application" �p)
!MESSAGE "ABrowser - Win32 Debug" ("Win32 (x86) Application" �p)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ABrowser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /Zd /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "ABrowser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ABrowser - Win32 Release"
# Name "ABrowser - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AB_main.cpp
# End Source File
# Begin Source File

SOURCE=.\ABrowser.rc
# End Source File
# Begin Source File

SOURCE=.\CABr.cpp
# End Source File
# Begin Source File

SOURCE=.\CABrView.cpp
# End Source File
# Begin Source File

SOURCE=.\CCmdLine.cpp
# End Source File
# Begin Source File

SOURCE=.\CFindWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CMem.cpp
# End Source File
# Begin Source File

SOURCE=.\CReg.cpp
# End Source File
# Begin Source File

SOURCE=.\CStatusW.cpp
# End Source File
# Begin Source File

SOURCE=.\CTxtFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CTxtQry.cpp
# End Source File
# Begin Source File

SOURCE=.\CWhlMous.cpp
# End Source File
# Begin Source File

SOURCE=.\EdCmdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\str_repl.c
# End Source File
# Begin Source File

SOURCE=.\winapiex.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ABrowser.h
# End Source File
# Begin Source File

SOURCE=.\CABr.h
# End Source File
# Begin Source File

SOURCE=.\CABrView.h
# End Source File
# Begin Source File

SOURCE=.\CCmdLine.h
# End Source File
# Begin Source File

SOURCE=.\CFindWnd.h
# End Source File
# Begin Source File

SOURCE=.\CMem.h
# End Source File
# Begin Source File

SOURCE=.\CReg.h
# End Source File
# Begin Source File

SOURCE=.\CStatusW.h
# End Source File
# Begin Source File

SOURCE=.\CTxtFile.h
# End Source File
# Begin Source File

SOURCE=.\CTxtQry.h
# End Source File
# Begin Source File

SOURCE=.\CWhlMous.h
# End Source File
# Begin Source File

SOURCE=.\EdCmdDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\str_repl.h
# End Source File
# Begin Source File

SOURCE=.\winapiex.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Resource\ABlogo.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\ABpaper.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\ABrowser.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\ABwdrag.cur
# End Source File
# End Group
# End Target
# End Project
