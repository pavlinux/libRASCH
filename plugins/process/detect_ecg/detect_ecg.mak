# Microsoft Developer Studio Generated NMAKE File, Based on detect_simple.dsp
!IF "$(CFG)" == ""
CFG=detect_simple - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. detect_simple - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "detect_simple - Win32 Release" && "$(CFG)" != "detect_simple - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "detect_simple.mak" CFG="detect_simple - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "detect_simple - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "detect_simple - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "detect_simple - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\detect_simple.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\detect_simple.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\detect_simple.obj"
	-@erase "$(INTDIR)\simple.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\detect_simple.exp"
	-@erase "$(OUTDIR)\detect_simple.lib"
	-@erase "..\..\bin\Release\detect_simple.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "detect_simple_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\detect_simple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\detect_simple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\detect_simple.pdb" /machine:I386 /out:"../../bin/Release/detect_simple.dll" /implib:"$(OUTDIR)\detect_simple.lib" 
LINK32_OBJS= \
	"$(INTDIR)\detect_simple.obj" \
	"$(INTDIR)\simple.obj" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\detect_simple.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "detect_simple - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\detect_simple.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\detect_simple.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\detect_simple.obj"
	-@erase "$(INTDIR)\simple.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\detect_simple.exp"
	-@erase "$(OUTDIR)\detect_simple.lib"
	-@erase "$(OUTDIR)\detect_simple.pdb"
	-@erase "..\..\bin\Debug\detect_simple.dll"
	-@erase "..\..\bin\Debug\detect_simple.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "detect_simple_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\detect_simple.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\detect_simple.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\detect_simple.pdb" /debug /machine:I386 /out:"../../bin/Debug/detect_simple.dll" /implib:"$(OUTDIR)\detect_simple.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\detect_simple.obj" \
	"$(INTDIR)\simple.obj" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\detect_simple.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("detect_simple.dep")
!INCLUDE "detect_simple.dep"
!ELSE 
!MESSAGE Warning: cannot find "detect_simple.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "detect_simple - Win32 Release" || "$(CFG)" == "detect_simple - Win32 Debug"
SOURCE=.\detect_simple.c

"$(INTDIR)\detect_simple.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\simple.c

"$(INTDIR)\simple.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "detect_simple - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\process\detect_simple"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\process\detect_simple"

!ELSEIF  "$(CFG)" == "detect_simple - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\process\detect_simple"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\process\detect_simple"

!ENDIF 


!ENDIF 

