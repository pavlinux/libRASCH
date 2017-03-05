# Microsoft Developer Studio Project File - Name="ev_summary_view" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ev_summary_view - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "ev_summary_view.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ev_summary_view.mak" CFG="ev_summary_view - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ev_summary_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ev_summary_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ev_summary_view - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ev_summary_view_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ev_summary_view_EXPORTS" /D "_WINDLL" /D "QT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 QtCore4.lib QtGui4.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj ../ts_view/Release/ts_view_optdlg_qt.obj ../ts_view/Release/ts_view_qt.obj ../ts_view/Release/ts_view_general.obj ../ts_view/Release/ch_optdlg_qt.obj ../ts_view/Release/moc_ts_view_optdlg_qt.obj ../ts_view/Release/moc_ts_view_qt.obj ../ts_view/Release/moc_ch_optdlg_qt.obj ../ts_view/Release/moc_annot_dlg_qt.obj ../ts_view/Release/annot_dlg_qt.obj intl.lib ../ts_view/Release/view_frame_qt.obj ../ts_view/Release/moc_view_frame_qt.obj ../ts_view/Release/view_frame_general.obj /nologo /dll /machine:I386 /out:"../../bin/Release/ev_summary_view.dll"

!ELSEIF  "$(CFG)" == "ev_summary_view - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ev_summary_view_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ev_summary_view_EXPORTS" /D "_WINDLL" /D "QT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 QtCored4.lib QtGuid4.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj ../ts_view/Debug/ts_view_optdlg_qt.obj ../ts_view/Debug/ts_view_qt.obj ../ts_view/Debug/ts_view_general.obj ../ts_view/Debug/ch_optdlg_qt.obj ../ts_view/Debug/moc_ts_view_optdlg_qt.obj ../ts_view/Debug/moc_ts_view_qt.obj ../ts_view/Debug/moc_ch_optdlg_qt.obj ../ts_view/Debug/moc_annot_dlg_qt.obj ../ts_view/Debug/annot_dlg_qt.obj intl.lib ../ts_view/Debug/view_frame_qt.obj ../ts_view/Debug/moc_view_frame_qt.obj ../ts_view/Debug/view_frame_general.obj /nologo /dll /debug /machine:I386 /out:"../../bin/Debug/ev_summary_view.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ev_summary_view - Win32 Release"
# Name "ev_summary_view - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ev_summary_view.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_summary_view_general.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_summary_view_qt.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_summary_view_scroll_area.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ev_summary_view.h
# End Source File
# Begin Source File

SOURCE=.\ev_summary_view_general.h
# End Source File
# Begin Source File

SOURCE=.\ev_summary_view_qt.h

!IF  "$(CFG)" == "ev_summary_view - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\ev_summary_view_qt.h

"moc_ev_summary_view_qt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_ev_summary_view_qt.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ev_summary_view - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\ev_summary_view_qt.h

"moc_ev_summary_view_qt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_ev_summary_view_qt.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ev_summary_view_scroll_area.h

!IF  "$(CFG)" == "ev_summary_view - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\ev_summary_view_scroll_area.h

"moc_ev_summary_view_scroll_area.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_ev_summary_view_scroll_area.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "ev_summary_view - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\ev_summary_view_scroll_area.h

"moc_ev_summary_view_scroll_area.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_ev_summary_view_scroll_area.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\moc_ev_summary_view_qt.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_ev_summary_view_scroll_area.cpp
# End Source File
# End Group
# End Target
# End Project
