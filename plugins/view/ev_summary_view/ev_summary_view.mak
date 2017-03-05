# Microsoft Developer Studio Generated NMAKE File, Based on ev_summary_view.dsp
!IF "$(CFG)" == ""
CFG=ev_summary_view - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. ev_summary_view - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "ev_summary_view - Win32 Release" && "$(CFG)" != "ev_summary_view - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ev_summary_view.mak" CFG="ev_summary_view - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ev_summary_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ev_summary_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ev_summary_view - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\ev_summary_view.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\ev_summary_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ev_summary_view.obj"
	-@erase "$(INTDIR)\ev_summary_view.res"
	-@erase "$(INTDIR)\ev_summary_view_general.obj"
	-@erase "$(INTDIR)\ev_summary_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ev_summary_view.exp"
	-@erase "$(OUTDIR)\ev_summary_view.lib"
	-@erase "..\..\bin\Release\ev_summary_view.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ev_summary_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ev_summary_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ev_summary_view.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ev_summary_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ev_summary_view.pdb" /machine:I386 /out:"../../bin/Release/ev_summary_view.dll" /implib:"$(OUTDIR)\ev_summary_view.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ev_summary_view.obj" \
	"$(INTDIR)\ev_summary_view_general.obj" \
	"$(INTDIR)\ev_summary_view_mfc.obj" \
	"$(INTDIR)\ev_summary_view.res" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\ev_summary_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ev_summary_view - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\ev_summary_view.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\ev_summary_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ev_summary_view.obj"
	-@erase "$(INTDIR)\ev_summary_view.res"
	-@erase "$(INTDIR)\ev_summary_view_general.obj"
	-@erase "$(INTDIR)\ev_summary_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ev_summary_view.exp"
	-@erase "$(OUTDIR)\ev_summary_view.lib"
	-@erase "$(OUTDIR)\ev_summary_view.pdb"
	-@erase "..\..\bin\Debug\ev_summary_view.dll"
	-@erase "..\..\bin\Debug\ev_summary_view.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ev_summary_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ev_summary_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ev_summary_view.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ev_summary_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ev_summary_view.pdb" /debug /machine:I386 /out:"../../bin/Debug/ev_summary_view.dll" /implib:"$(OUTDIR)\ev_summary_view.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ev_summary_view.obj" \
	"$(INTDIR)\ev_summary_view_general.obj" \
	"$(INTDIR)\ev_summary_view_mfc.obj" \
	"$(INTDIR)\ev_summary_view.res" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\ev_summary_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ev_summary_view.dep")
!INCLUDE "ev_summary_view.dep"
!ELSE 
!MESSAGE Warning: cannot find "ev_summary_view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ev_summary_view - Win32 Release" || "$(CFG)" == "ev_summary_view - Win32 Debug"
SOURCE=.\ev_summary_view.cpp

"$(INTDIR)\ev_summary_view.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ev_summary_view_general.cpp

"$(INTDIR)\ev_summary_view_general.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ev_summary_view_mfc.cpp

"$(INTDIR)\ev_summary_view_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ev_summary_view.rc

"$(INTDIR)\ev_summary_view.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ev_summary_view - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\ev_summary_view"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\ev_summary_view"

!ELSEIF  "$(CFG)" == "ev_summary_view - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\ev_summary_view"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\ev_summary_view"

!ENDIF 


!ENDIF 

