# Microsoft Developer Studio Generated NMAKE File, Based on ra_view.dsp
!IF "$(CFG)" == ""
CFG=ra_view - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. ra_view - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "ra_view - Win32 Release" && "$(CFG)" != "ra_view - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ra_view.mak" CFG="ra_view - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ra_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ra_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ra_view - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\ra_view.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\ra_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ra_splitter_mfc.obj"
	-@erase "$(INTDIR)\ra_view.obj"
	-@erase "$(INTDIR)\ra_view.res"
	-@erase "$(INTDIR)\ra_view_general.obj"
	-@erase "$(INTDIR)\ra_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ra_view.exp"
	-@erase "$(OUTDIR)\ra_view.lib"
	-@erase "..\..\bin\Release\ra_view.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ra_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ra_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ra_view.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ra_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ra_view.pdb" /machine:I386 /out:"../../bin/Release/ra_view.dll" /implib:"$(OUTDIR)\ra_view.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ra_splitter_mfc.obj" \
	"$(INTDIR)\ra_view.obj" \
	"$(INTDIR)\ra_view_general.obj" \
	"$(INTDIR)\ra_view_mfc.obj" \
	"$(INTDIR)\ra_view.res" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\ra_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ra_view - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\ra_view.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\ra_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ra_splitter_mfc.obj"
	-@erase "$(INTDIR)\ra_view.obj"
	-@erase "$(INTDIR)\ra_view.res"
	-@erase "$(INTDIR)\ra_view_general.obj"
	-@erase "$(INTDIR)\ra_view_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ra_view.exp"
	-@erase "$(OUTDIR)\ra_view.lib"
	-@erase "$(OUTDIR)\ra_view.pdb"
	-@erase "..\..\bin\Debug\ra_view.dll"
	-@erase "..\..\bin\Debug\ra_view.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ra_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\ra_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\ra_view.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ra_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ra_view.pdb" /debug /machine:I386 /out:"../../bin/Debug/ra_view.dll" /implib:"$(OUTDIR)\ra_view.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ra_splitter_mfc.obj" \
	"$(INTDIR)\ra_view.obj" \
	"$(INTDIR)\ra_view_general.obj" \
	"$(INTDIR)\ra_view_mfc.obj" \
	"$(INTDIR)\ra_view.res" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\ra_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ra_view.dep")
!INCLUDE "ra_view.dep"
!ELSE 
!MESSAGE Warning: cannot find "ra_view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ra_view - Win32 Release" || "$(CFG)" == "ra_view - Win32 Debug"
SOURCE=.\ra_splitter_mfc.cpp

"$(INTDIR)\ra_splitter_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_view.cpp

"$(INTDIR)\ra_view.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_view_general.cpp

"$(INTDIR)\ra_view_general.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_view_mfc.cpp

"$(INTDIR)\ra_view_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_view.rc

"$(INTDIR)\ra_view.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ra_view - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\ra_view"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\ra_view"

!ELSEIF  "$(CFG)" == "ra_view - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\ra_view"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\ra_view"

!ENDIF 


!ENDIF 

