# Microsoft Developer Studio Generated NMAKE File, Based on plugin_info_dlg.dsp
!IF "$(CFG)" == ""
CFG=plugin_info_dlg - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. plugin_info_dlg - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "plugin_info_dlg - Win32 Release" && "$(CFG)" != "plugin_info_dlg - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "plugin_info_dlg.mak" CFG="plugin_info_dlg - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "plugin_info_dlg - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plugin_info_dlg - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plugin_info_dlg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\plugin_info_dlg.dll" "$(OUTDIR)\plugin_info_dlg.pch"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\plugin_info_dlg.dll" "$(OUTDIR)\plugin_info_dlg.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\plugin_info_dlg.obj"
	-@erase "$(INTDIR)\plugin_info_dlg.pch"
	-@erase "$(INTDIR)\plugin_info_dlg.res"
	-@erase "$(INTDIR)\plugin_info_dlg_mfc.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\plugin_info_dlg.exp"
	-@erase "$(OUTDIR)\plugin_info_dlg.lib"
	-@erase "..\..\bin\Release\plugin_info_dlg.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\plugin_info_dlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\plugin_info_dlg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\plugin_info_dlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\plugin_info_dlg.pdb" /machine:I386 /def:".\plugin_info_dlg.def" /out:"../../bin/Release/plugin_info_dlg.dll" /implib:"$(OUTDIR)\plugin_info_dlg.lib" 
DEF_FILE= \
	".\plugin_info_dlg.def"
LINK32_OBJS= \
	"$(INTDIR)\plugin_info_dlg.obj" \
	"$(INTDIR)\plugin_info_dlg_mfc.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\plugin_info_dlg.res" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\plugin_info_dlg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "plugin_info_dlg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\plugin_info_dlg.dll" "$(OUTDIR)\plugin_info_dlg.pch"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\plugin_info_dlg.dll" "$(OUTDIR)\plugin_info_dlg.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\plugin_info_dlg.obj"
	-@erase "$(INTDIR)\plugin_info_dlg.pch"
	-@erase "$(INTDIR)\plugin_info_dlg.res"
	-@erase "$(INTDIR)\plugin_info_dlg_mfc.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\plugin_info_dlg.exp"
	-@erase "$(OUTDIR)\plugin_info_dlg.lib"
	-@erase "$(OUTDIR)\plugin_info_dlg.pdb"
	-@erase "..\..\bin\Debug\plugin_info_dlg.dll"
	-@erase "..\..\bin\Debug\plugin_info_dlg.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\plugin_info_dlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\plugin_info_dlg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\plugin_info_dlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj intl.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\plugin_info_dlg.pdb" /debug /machine:I386 /def:".\plugin_info_dlg.def" /out:"../../bin/Debug/plugin_info_dlg.dll" /implib:"$(OUTDIR)\plugin_info_dlg.lib" /pdbtype:sept 
DEF_FILE= \
	".\plugin_info_dlg.def"
LINK32_OBJS= \
	"$(INTDIR)\plugin_info_dlg.obj" \
	"$(INTDIR)\plugin_info_dlg_mfc.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\plugin_info_dlg.res" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\plugin_info_dlg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("plugin_info_dlg.dep")
!INCLUDE "plugin_info_dlg.dep"
!ELSE 
!MESSAGE Warning: cannot find "plugin_info_dlg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "plugin_info_dlg - Win32 Release" || "$(CFG)" == "plugin_info_dlg - Win32 Debug"
SOURCE=.\plugin_info_dlg.cpp

"$(INTDIR)\plugin_info_dlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_info_dlg.rc

"$(INTDIR)\plugin_info_dlg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\plugin_info_dlg_mfc.cpp

"$(INTDIR)\plugin_info_dlg_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "plugin_info_dlg - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\plugin_info_dlg.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\plugin_info_dlg.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "plugin_info_dlg - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\plugin_info_dlg.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\plugin_info_dlg.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "plugin_info_dlg - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\PLUGINS\VIEW\plugin_info_dlg"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\PLUGINS\VIEW\plugin_info_dlg"

!ELSEIF  "$(CFG)" == "plugin_info_dlg - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\PLUGINS\VIEW\plugin_info_dlg"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\PLUGINS\VIEW\plugin_info_dlg"

!ENDIF 


!ENDIF 

