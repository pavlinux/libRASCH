# Microsoft Developer Studio Generated NMAKE File, Based on mit_bih.dsp
!IF "$(CFG)" == ""
CFG=mit_bih - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. mit_bih - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "mit_bih - Win32 Release" && "$(CFG)" != "mit_bih - Win32 Debug"
!MESSAGE Ung�ltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "mit_bih.mak" CFG="mit_bih - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "mit_bih - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mit_bih - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ung�ltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mit_bih - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\mit_bih.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\mit_bih.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\mit_bih.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mit_bih.exp"
	-@erase "$(OUTDIR)\mit_bih.lib"
	-@erase "..\..\bin\Release\mit_bih.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "mit_bih_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\mit_bih.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mit_bih.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\mit_bih.pdb" /machine:I386 /out:"../../bin/Release/mit_bih.dll" /implib:"$(OUTDIR)\mit_bih.lib" 
LINK32_OBJS= \
	"$(INTDIR)\mit_bih.obj" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\mit_bih.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mit_bih - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\mit_bih.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\mit_bih.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\mit_bih.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mit_bih.exp"
	-@erase "$(OUTDIR)\mit_bih.lib"
	-@erase "$(OUTDIR)\mit_bih.pdb"
	-@erase "..\..\bin\Debug\mit_bih.dll"
	-@erase "..\..\bin\Debug\mit_bih.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "mit_bih_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\mit_bih.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mit_bih.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\mit_bih.pdb" /debug /machine:I386 /out:"../../bin/Debug/mit_bih.dll" /implib:"$(OUTDIR)\mit_bih.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\mit_bih.obj" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\mit_bih.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("mit_bih.dep")
!INCLUDE "mit_bih.dep"
!ELSE 
!MESSAGE Warning: cannot find "mit_bih.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mit_bih - Win32 Release" || "$(CFG)" == "mit_bih - Win32 Debug"
SOURCE=.\mit_bih.c

"$(INTDIR)\mit_bih.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "mit_bih - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\access\mit_bih"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\access\mit_bih"

!ELSEIF  "$(CFG)" == "mit_bih - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\access\mit_bih"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\access\mit_bih"

!ENDIF 


!ENDIF 

