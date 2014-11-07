# Microsoft Developer Studio Project File - Name="fftw" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=fftw - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fftw.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fftw.mak" CFG="fftw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fftw - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fftw - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fftw - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "fftw - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "fftw - Win32 Release"
# Name "fftw - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\config.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\executor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fftwf77.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fftwnd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_11.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_12.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_13.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_14.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_15.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_16.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fn_9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_11.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_12.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_13.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_14.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_15.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_16.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\fni_9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_16.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftw_9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_16.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_32.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\ftwi_9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\generic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\malloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\planner.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\putils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\rader.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\twiddle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\wisdom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Library\fftw\fftw\wisdomio.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
