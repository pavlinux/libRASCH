# Microsoft Developer Studio Project File - Name="simple_calc_dlg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=simple_calc_dlg - Win32 Debug
!MESSAGE Dies ist kein g�ltiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und f�hren Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "simple_calc_dlg.mak".
!MESSAGE 
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "simple_calc_dlg.mak" CFG="simple_calc_dlg - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "simple_calc_dlg - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "simple_calc_dlg - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLE_CALC_DLG_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLE_CALCL_DLG_EXPORTS" /D "_WINDLL" /D "QT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 QtCore4.lib QtGui4.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /dll /machine:I386 /out:"../../bin/Release/simple_calc_dlg.dll"

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLE_CALCL_DLG_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLE_CALC_DLG_EXPORTS" /D "_WINDLL" /D "QT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 QtCored4.lib QtGuid4.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj ../ts_view/Debug/ts_view_optdlg_qt.obj ../ts_view/Debug/ts_view_qt.obj ../ts_view/Debug/ts_view_general.obj ../ts_view/Debug/ch_optdlg_qt.obj ../ts_view/Debug/moc_ts_view_optdlg_qt.obj ../ts_view/Debug/moc_ts_view_qt.obj ../ts_view/Debug/moc_ch_optdlg_qt.obj ../ts_view/Debug/moc_annot_dlg_qt.obj ../ts_view/Debug/annot_dlg_qt.obj intl.lib ../ts_view/Debug/view_frame_qt.obj ../ts_view/Debug/moc_view_frame_qt.obj ../ts_view/Debug/view_frame_general.obj /nologo /dll /debug /machine:I386 /out:"../../bin/Debug/simple_calc_dlg.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "simple_calc_dlg - Win32 Release"
# Name "simple_calc_dlg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\options_dlg_qt.cpp
# End Source File
# Begin Source File

SOURCE=.\simple_calc_dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\simple_calc_dlg_qt.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\options_dlg_qt.h

!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\options_dlg_qt.h

"moc_options_dlg_qt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_options_dlg_qt.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\options_dlg_qt.h

"moc_options_dlg_qt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_options_dlg_qt.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\simple_calc_dlg.h
# End Source File
# Begin Source File

SOURCE=.\simple_calc_dlg_qt.h

!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\simple_calc_dlg_qt.h

"c_simple_calc_dlg_qt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_simple_calc_dlg_qt.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - MOC $(InputPath)
InputPath=.\simple_calc_dlg_qt.h

"c_simple_calc_dlg_qt.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe  -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_THREAD_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -I"$(QTDIR)/include/QtCore" -I"$(QTDIR)/include/QtGui" -I"$(QTDIR)/include" -I"$(QTDIR)/include/ActiveQt" -I"win32\moc" -I"win32\ui" -I"$(QTDIR)/mkspecs/win32-msvc" -D_MSC_VER=1200 -DWIN32 $(InputPath) -o moc_simple_calc_dlg_qt.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\options_dlg.ui

!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UIC $(InputPath)
InputPath=.\options_dlg.ui

"options_dlg_ui.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic.exe $(InputPath) -o options_dlg_ui.h

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UIC $(InputPath)
InputPath=.\options_dlg.ui

"options_dlg_ui.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic.exe $(InputPath) -o options_dlg_ui.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\simple_calc_dlg.ui

!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UIC $(InputPath)
InputPath=.\simple_calc_dlg.ui

"simple_calc_dlg_ui.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic.exe $(InputPath) -o simple_calc_dlg_ui.h

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - UIC $(InputPath)
InputPath=.\simple_calc_dlg.ui

"simple_calc_dlg_ui.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\uic.exe $(InputPath) -o simple_calc_dlg_ui.h

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\moc_options_dlg_qt.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_simple_calc_dlg_qt.cpp
# End Source File
# Begin Source File

SOURCE=.\options_dlg_ui.h
# End Source File
# Begin Source File

SOURCE=.\simple_calc_dlg_ui.h
# End Source File
# End Group
# End Target
# End Project
