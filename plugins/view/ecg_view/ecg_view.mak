# Microsoft Developer Studio Generated NMAKE File, Based on ecg_view.dsp
!IF "$(CFG)" == ""
CFG=ecg_view - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. ecg_view - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "ecg_view - Win32 Release" && "$(CFG)" != "ecg_view - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ecg_view.mak" CFG="ecg_view - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ecg_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ecg_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ecg_view - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\ecg_view.dll"

!ELSE 

ALL : "librasch - Win32 Release" "ts_view - Win32 Release" "..\..\bin\Release\ecg_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ts_view - Win32 ReleaseCLEAN" "librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ecg_view.obj"
	-@erase "$(INTDIR)\ecg_view.res"
	-@erase "$(INTDIR)\ecg_view_general.obj"
	-@erase "$(INTDIR)\ecg_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ecg_view.exp"
	-@erase "$(OUTDIR)\ecg_view.lib"
	-@erase "..\..\bin\Release\ecg_view.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ecg_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ecg_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ecg_view.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ecg_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj ../ts_view/Release/gen_opt_dlg_mfc.obj ../ts_view/Release/ts_view_mfc.obj ../ts_view/Release/ts_view_general.obj ../ts_view/Release/ts_view_ch_opt_dlg_mfc.obj ../ts_view/Release/annot_dlg_mfc.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ecg_view.pdb" /machine:I386 /out:"../../bin/Release/ecg_view.dll" /implib:"$(OUTDIR)\ecg_view.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ecg_view.obj" \
	"$(INTDIR)\ecg_view_general.obj" \
	"$(INTDIR)\ecg_view_mfc.obj" \
	"$(INTDIR)\ecg_view.res" \
	"..\ts_view\Release\ts_view.lib" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\ecg_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ecg_view - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\ecg_view.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "ts_view - Win32 Debug" "..\..\bin\Debug\ecg_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ts_view - Win32 DebugCLEAN" "librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ecg_view.obj"
	-@erase "$(INTDIR)\ecg_view.res"
	-@erase "$(INTDIR)\ecg_view_general.obj"
	-@erase "$(INTDIR)\ecg_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ecg_view.exp"
	-@erase "$(OUTDIR)\ecg_view.lib"
	-@erase "$(OUTDIR)\ecg_view.pdb"
	-@erase "..\..\bin\Debug\ecg_view.dll"
	-@erase "..\..\bin\Debug\ecg_view.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ecg_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ecg_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ecg_view.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ecg_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj ../ts_view/Debug/gen_opt_dlg_mfc.obj ../ts_view/Debug/ts_view_mfc.obj ../ts_view/Debug/ts_view_general.obj ../ts_view/Debug/ts_view_ch_opt_dlg_mfc.obj ../ts_view/Debug/annot_dlg_mfc.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ecg_view.pdb" /debug /machine:I386 /out:"../../bin/Debug/ecg_view.dll" /implib:"$(OUTDIR)\ecg_view.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ecg_view.obj" \
	"$(INTDIR)\ecg_view_general.obj" \
	"$(INTDIR)\ecg_view_mfc.obj" \
	"$(INTDIR)\ecg_view.res" \
	"..\ts_view\Debug\ts_view.lib" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\ecg_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ecg_view.dep")
!INCLUDE "ecg_view.dep"
!ELSE 
!MESSAGE Warning: cannot find "ecg_view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ecg_view - Win32 Release" || "$(CFG)" == "ecg_view - Win32 Debug"
SOURCE=.\ecg_view.cpp

"$(INTDIR)\ecg_view.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ecg_view_general.cpp

"$(INTDIR)\ecg_view_general.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ecg_view_mfc.cpp

"$(INTDIR)\ecg_view_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ecg_view.rc

"$(INTDIR)\ecg_view.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ecg_view - Win32 Release"

"ts_view - Win32 Release" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Release" 
   cd "..\ecg_view"

"ts_view - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Release" RECURSE=1 CLEAN 
   cd "..\ecg_view"

!ELSEIF  "$(CFG)" == "ecg_view - Win32 Debug"

"ts_view - Win32 Debug" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Debug" 
   cd "..\ecg_view"

"ts_view - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\ecg_view"

!ENDIF 

!IF  "$(CFG)" == "ecg_view - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\ecg_view"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\ecg_view"

!ELSEIF  "$(CFG)" == "ecg_view - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\ecg_view"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\ecg_view"

!ENDIF 


!ENDIF 

