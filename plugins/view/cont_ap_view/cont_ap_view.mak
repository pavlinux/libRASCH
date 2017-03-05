# Microsoft Developer Studio Generated NMAKE File, Based on cont_ap_view.dsp
!IF "$(CFG)" == ""
CFG=cont_ap_view - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. cont_ap_view - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "cont_ap_view - Win32 Release" && "$(CFG)" != "cont_ap_view - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "cont_ap_view.mak" CFG="cont_ap_view - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "cont_ap_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cont_ap_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "cont_ap_view - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\cont_ap_view.dll"

!ELSE 

ALL : "ts_view - Win32 Release" "librasch - Win32 Release" "..\..\bin\Release\cont_ap_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" "ts_view - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cont_ap_view.obj"
	-@erase "$(INTDIR)\cont_ap_view.res"
	-@erase "$(INTDIR)\cont_ap_view_general.obj"
	-@erase "$(INTDIR)\cont_ap_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cont_ap_view.exp"
	-@erase "$(OUTDIR)\cont_ap_view.lib"
	-@erase "..\..\bin\Release\cont_ap_view.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "cont_ap_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\cont_ap_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\cont_ap_view.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cont_ap_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj ../ts_view/Release/gen_opt_dlg_mfc.obj ../ts_view/Release/ts_view_mfc.obj ../ts_view/Release/ts_view_general.obj ../ts_view/Release/ts_view_ch_opt_dlg_mfc.obj ../ts_view/Release/annot_dlg_mfc.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\cont_ap_view.pdb" /machine:I386 /out:"../../bin/Release/cont_ap_view.dll" /implib:"$(OUTDIR)\cont_ap_view.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cont_ap_view.obj" \
	"$(INTDIR)\cont_ap_view_general.obj" \
	"$(INTDIR)\cont_ap_view_mfc.obj" \
	"$(INTDIR)\cont_ap_view.res" \
	"..\..\..\librasch.lib" \
	"..\ts_view\Release\ts_view.lib"

"..\..\bin\Release\cont_ap_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cont_ap_view - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\cont_ap_view.dll"

!ELSE 

ALL : "ts_view - Win32 Debug" "librasch - Win32 Debug" "..\..\bin\Debug\cont_ap_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" "ts_view - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cont_ap_view.obj"
	-@erase "$(INTDIR)\cont_ap_view.res"
	-@erase "$(INTDIR)\cont_ap_view_general.obj"
	-@erase "$(INTDIR)\cont_ap_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cont_ap_view.exp"
	-@erase "$(OUTDIR)\cont_ap_view.lib"
	-@erase "$(OUTDIR)\cont_ap_view.pdb"
	-@erase "..\..\bin\Debug\cont_ap_view.dll"
	-@erase "..\..\bin\Debug\cont_ap_view.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "cont_ap_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\cont_ap_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\cont_ap_view.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cont_ap_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj ../ts_view/Debug/gen_opt_dlg_mfc.obj ../ts_view/Debug/ts_view_mfc.obj ../ts_view/Debug/ts_view_general.obj ../ts_view/Debug/ts_view_ch_opt_dlg_mfc.obj ../ts_view/Debug/annot_dlg_mfc.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\cont_ap_view.pdb" /debug /machine:I386 /out:"../../bin/Debug/cont_ap_view.dll" /implib:"$(OUTDIR)\cont_ap_view.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cont_ap_view.obj" \
	"$(INTDIR)\cont_ap_view_general.obj" \
	"$(INTDIR)\cont_ap_view_mfc.obj" \
	"$(INTDIR)\cont_ap_view.res" \
	"..\..\..\libraschd.lib" \
	"..\ts_view\Debug\ts_view.lib"

"..\..\bin\Debug\cont_ap_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("cont_ap_view.dep")
!INCLUDE "cont_ap_view.dep"
!ELSE 
!MESSAGE Warning: cannot find "cont_ap_view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cont_ap_view - Win32 Release" || "$(CFG)" == "cont_ap_view - Win32 Debug"
SOURCE=.\cont_ap_view.cpp

"$(INTDIR)\cont_ap_view.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cont_ap_view_general.cpp

"$(INTDIR)\cont_ap_view_general.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cont_ap_view_mfc.cpp

"$(INTDIR)\cont_ap_view_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cont_ap_view.rc

"$(INTDIR)\cont_ap_view.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "cont_ap_view - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\cont_ap_view"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\cont_ap_view"

!ELSEIF  "$(CFG)" == "cont_ap_view - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\cont_ap_view"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\cont_ap_view"

!ENDIF 

!IF  "$(CFG)" == "cont_ap_view - Win32 Release"

"ts_view - Win32 Release" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Release" 
   cd "..\cont_ap_view"

"ts_view - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Release" RECURSE=1 CLEAN 
   cd "..\cont_ap_view"

!ELSEIF  "$(CFG)" == "cont_ap_view - Win32 Debug"

"ts_view - Win32 Debug" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Debug" 
   cd "..\cont_ap_view"

"ts_view - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch\plugins\view\ts_view"
   $(MAKE) /$(MAKEFLAGS) /F .\ts_view.mak CFG="ts_view - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\cont_ap_view"

!ENDIF 


!ENDIF 

