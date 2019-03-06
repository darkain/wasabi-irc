# Microsoft Developer Studio Project File - Name="IRC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IRC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IRC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IRC.mak" CFG="IRC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IRC - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IRC - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/wIRC"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IRC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\studio\wacs"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IRC_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /w /W0 /GX /O1 /I "../../studio/wa3cfg" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IRC_EXPORTS" /D "_USE_VBASE_" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\WasabiIRC\Wacs\IRC.wac"

!ELSEIF  "$(CFG)" == "IRC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\studio\wacs"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IRC_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "IRC_EXPORTS" /D "_USE_VBASE_" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\WasabiIRC\Wacs\IRC.wac" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "IRC - Win32 Release"
# Name "IRC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChatWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\IRC.cpp
# End Source File
# Begin Source File

SOURCE=.\IrcClient.cpp
# End Source File
# Begin Source File

SOURCE=.\IRCwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptObject.cpp
# End Source File
# Begin Source File

SOURCE=.\WndManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChatWnd.h
# End Source File
# Begin Source File

SOURCE=.\IRC.h
# End Source File
# Begin Source File

SOURCE=.\IrcClient.h
# End Source File
# Begin Source File

SOURCE=.\IRCwnd.h
# End Source File
# Begin Source File

SOURCE=.\ScriptObject.h
# End Source File
# Begin Source File

SOURCE=.\WndManager.h
# End Source File
# End Group
# Begin Group "Wnds"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\About.cpp
# End Source File
# Begin Source File

SOURCE=.\About.h
# End Source File
# Begin Source File

SOURCE=.\SendBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SendBar.h
# End Source File
# Begin Source File

SOURCE=.\TextLog.cpp
# End Source File
# Begin Source File

SOURCE=.\TextLog.h
# End Source File
# Begin Source File

SOURCE=.\TextLogWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TextLogWnd.h
# End Source File
# Begin Source File

SOURCE=.\Toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Toolbar.h
# End Source File
# End Group
# Begin Group "Logs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ChannelLog.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelLog.h
# End Source File
# Begin Source File

SOURCE=.\ChannelLogViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\ChannelLogViewer.h
# End Source File
# Begin Source File

SOURCE=.\ChatLog.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatLog.h
# End Source File
# Begin Source File

SOURCE=.\ChatLogViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatLogViewer.h
# End Source File
# Begin Source File

SOURCE=.\DccChatLog.cpp
# End Source File
# Begin Source File

SOURCE=.\DccChatLog.h
# End Source File
# Begin Source File

SOURCE=.\PrivMsgLog.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivMsgLog.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\IRC.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\wIRC_Icon_Active.png
# End Source File
# Begin Source File

SOURCE=.\wIRC_Icon_Hover.png
# End Source File
# Begin Source File

SOURCE=.\wIRC_Icon_Unactive.png
# End Source File
# End Group
# End Target
# End Project
