# Microsoft Developer Studio Project File - Name="GNU FreeDinkedit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GNU FreeDinkedit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "freedinkedit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "freedinkedit.mak" CFG="GNU FreeDinkedit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GNU FreeDinkedit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GNU FreeDinkedit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GNU FreeDinkedit - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"ERelease/" /Fp"ERelease/FreeDinkedit.pch" /YX /Fo"ERelease/" /Fd"ERelease/" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"ERelease/FreeDinkedit.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib ddraw.lib dinput.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib SDL_mixer.lib SDL.lib /nologo /subsystem:windows /machine:I386 /out:"freedink\freedinkedit.exe"

!ELSEIF  "$(CFG)" == "GNU FreeDinkedit - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"EDebug/" /Fp"EDebug/FreeDinkedit.pch" /YX /Fo"EDebug/" /Fd"EDebug/" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dsound.lib ddraw.lib dinput.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib SDL_mixer.lib SDL.lib /nologo /subsystem:windows /debug /machine:I386 /out:"freedink\freedinkedit_debug.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GNU FreeDinkedit - Win32 Release"
# Name "GNU FreeDinkedit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\bgm.cpp
# End Source File
# Begin Source File

SOURCE=..\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=..\dinkvar.cpp
# End Source File
# Begin Source File

SOURCE=..\fastfile.c
# End Source File
# Begin Source File

SOURCE=..\freedinkedit.cpp
# End Source File
# Begin Source File

SOURCE=..\sfx.cpp
# End Source File
# Begin Source File

SOURCE=..\string_util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\bgm.h
# End Source File
# Begin Source File

SOURCE=..\ddutil.h
# End Source File
# Begin Source File

SOURCE=..\dinkvar.h
# End Source File
# Begin Source File

SOURCE=..\fastfile.h
# End Source File
# Begin Source File

SOURCE=..\ffent.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\string_util.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dink.ico
# End Source File
# Begin Source File

SOURCE=..\freedink.rc
# End Source File
# End Group
# End Target
# End Project
