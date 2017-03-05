# Microsoft Developer Studio Project File - Name="librasch" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=librasch - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "librasch.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "librasch.mak" CFG="librasch - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "librasch - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "librasch - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "librasch - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D LOCALEDIR=\"\" /D PACKAGE=\"librasch\" /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libxml2.lib intl.lib iconv.lib /nologo /dll /machine:I386 /out:"librasch.dll" /implib:"librasch.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D "TRACE_SIGNAL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libxml2.lib intl.lib iconv.lib /nologo /dll /debug /machine:I386 /out:"libraschd.dll" /implib:"libraschd.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "librasch - Win32 Release"
# Name "librasch - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\estimate_ch_infos.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\eval.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\eval_auto.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\eval_internal.c
# End Source File
# Begin Source File

SOURCE=.\src\handle_plugin_win32.c
# End Source File
# Begin Source File

SOURCE=.\src\shared\i18n.c

!IF  "$(CFG)" == "librasch - Win32 Release"

# SUBTRACT CPP /X

!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\linked_list.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\load_eval_v0.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\load_eval_v1.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\mime64.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\mime64.h
# End Source File
# Begin Source File

SOURCE=.\src\pl_comm.c
# End Source File
# Begin Source File

SOURCE=.\src\shared\pl_general.c
# End Source File
# Begin Source File

SOURCE=.\src\ra.c

!IF  "$(CFG)" == "librasch - Win32 Release"

!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

# ADD CPP /D LOCALEDIR=\"\" /D PACKAGE=\"librasch\"
# SUBTRACT CPP /D "TRACE_SIGNAL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\shared\ra_file.c
# End Source File
# Begin Source File

SOURCE=.\src\raw_process.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\read_evaluation.c
# End Source File
# Begin Source File

SOURCE=.\src\eval\save_eval_v1.c
# End Source File
# Begin Source File

SOURCE=.\src\value.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
