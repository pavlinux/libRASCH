# Microsoft Developer Studio Generated NMAKE File, Based on ctg_view.dsp
!IF "$(CFG)" == ""
CFG=ctg_view - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. ctg_view - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "ctg_view - Win32 Release" && "$(CFG)" != "ctg_view - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ctg_view.mak" CFG="ctg_view - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ctg_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ctg_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ctg_view - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\ctg_view.dll"

!ELSE 

ALL : "ts_view - Win32 Release" "librasch - Win32 Release" "..\..\bin\Release\ctg_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" "ts_view - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ctg_view.obj"
	-@erase "$(INTDIR)\ctg_view.res"
	-@erase "$(INTDIR)\ctg_view_general.obj"
	-@erase "$(INTDIR)\ctg_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ctg_view.exp"
	-@erase "$(OUTDIR)\ctg_view.lib"
	-@erase "..\..\bin\Release\ctg_view.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ctg_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ctg_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ctg_view.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ctg_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj ../ts_view/Release/gen_opt_dlg_mfc.obj ../ts_view/Release/ts_view_mfc.obj ../ts_view/Release/ts_view_general.obj ../ts_view/Release/ts_view_ch_opt_dlg_mfc.obj ../ts_view/Release/annot_dlg_mfc.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ctg_view.pdb" /machine:I386 /out:"../../bin/Release/ctg_view.dll" /implib:"$(OUTDIR)\ctg_view.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ctg_view.obj" \
	"$(INTDIR)\ctg_view_general.obj" \
	"$(INTDIR)\ctg_view_mfc.obj" \
	"$(INTDIR)\ctg_view.res" \
	"..\..\..\librasch.lib" \
	"..\ts_view\Release\ts_view.lib"

"..\..\bin\Release\ctg_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ctg_view - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\ctg_view.dll"

!ELSE 

ALL : "ts_view - Win32 Debug" "librasch - Win32 Debug" "..\..\bin\Debug\ctg_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" "ts_view - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ctg_view.obj"
	-@erase "$(INTDIR)\ctg_view.res"
	-@erase "$(INTDIR)\ctg_view_general.obj"
	-@erase "$(INTDIR)\ctg_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ctg_view.exp"
	-@erase "$(OUTDIR)\ctg_view.lib"
	-@erase "$(OUTDIR)\ctg_view.pdb"
	-@erase "..\..\bin\Debug\ctg_view.dll"
	-@erase "..\..\bin\Debug\ctg_view.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ctg_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ctg_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ctg_view.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ctg_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj ../ts_view/Debug/gen_opt_dlg_mfc.obj ../ts_view/Debug/ts_view_mfc.obj ../ts_view/Debug/ts_view_general.obj ../ts_view/Debug/ts_view_ch_opt_dlg_mfc.obj ../ts_view/Debug/annot_dlg_mfc.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ctg_view.pdb" /debug /machine:I386 /out:"../../bin/Debug/ctg_view.dll" /implib:"$(OUTDIR)\ctg_view.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ctg_view.obj" \
	"$(INTDIR)\ctg_view_general.obj" \
	"$(INTDIR)\ctg_view_mfc.obj" \
	"$(INTDIR)\ctg_view.res" \
	"..\..\..\libraschd.lib" \
	"..\ts_view\Debug\ts_view.lib"

"..\..\bin\Debug\ctg_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ctg_view.dep")
!INCLUDE "ctg_view.dep"
!ELSE 
!MESSAGE Warning: cannot find "ctg_view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ctg_view - Win32 Release" || "$(CFG)" == "ctg_view - Win32 Debug"
SOURCE=.\ctg_view.cpp

"$(INTDIR)\ctg_view.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ctg_view_general.cpp

"$(INTDIR)\ctg_view_general.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ctg_view_mfc.cpp

"$(INTDIR)\ctg_view_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ctg_view.rc

"$(INTDIR)\ctg_view.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ctg_view - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\ctg_view"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\ctg_view"

!ELSEIF  "$(CFG)" == "ctg_view - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\ctg_view"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\ctg_view"

!ENDIF 

!IF  "$(CFG)" == "ctg_view - Win32 Release"

"ts_view - Win32 Release" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Release" 
   cd "..\ctg_view"

"ts_view - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Release" RECURSE=1 CLEAN 
   cd "..\ctg_view"

!ELSEIF  "$(CFG)" == "ctg_view - Win32 Debug"

"ts_view - Win32 Debug" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Debug" 
   cd "..\ctg_view"

"ts_view - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\ctg_view"

!ENDIF 


!ENDIF 

