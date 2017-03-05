# Microsoft Developer Studio Generated NMAKE File, Based on dawes_redman.dsp
!IF "$(CFG)" == ""
CFG=dawes_redman - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. dawes_redman - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "dawes_redman - Win32 Release" && "$(CFG)" != "dawes_redman - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "dawes_redman.mak" CFG="dawes_redman - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "dawes_redman - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dawes_redman - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dawes_redman - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\dawes_redman.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\dawes_redman.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dawes_redman.obj"
	-@erase "$(INTDIR)\frequency.obj"
	-@erase "$(INTDIR)\statistics.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dawes_redman.exp"
	-@erase "$(OUTDIR)\dawes_redman.lib"
	-@erase "..\..\bin\Release\dawes_redman.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "dawes_redman_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\dawes_redman.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dawes_redman.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\dawes_redman.pdb" /machine:I386 /out:"../../bin/Release/dawes_redman.dll" /implib:"$(OUTDIR)\dawes_redman.lib" 
LINK32_OBJS= \
	"$(INTDIR)\dawes_redman.obj" \
	"$(INTDIR)\frequency.obj" \
	"$(INTDIR)\statistics.obj" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\dawes_redman.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dawes_redman - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\dawes_redman.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\dawes_redman.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dawes_redman.obj"
	-@erase "$(INTDIR)\frequency.obj"
	-@erase "$(INTDIR)\statistics.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dawes_redman.exp"
	-@erase "$(OUTDIR)\dawes_redman.lib"
	-@erase "$(OUTDIR)\dawes_redman.pdb"
	-@erase "..\..\bin\Debug\dawes_redman.dll"
	-@erase "..\..\bin\Debug\dawes_redman.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "dawes_redman_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\dawes_redman.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dawes_redman.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\dawes_redman.pdb" /debug /machine:I386 /out:"../../bin/Debug/dawes_redman.dll" /implib:"$(OUTDIR)\dawes_redman.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dawes_redman.obj" \
	"$(INTDIR)\frequency.obj" \
	"$(INTDIR)\statistics.obj" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\dawes_redman.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dawes_redman.dep")
!INCLUDE "dawes_redman.dep"
!ELSE 
!MESSAGE Warning: cannot find "dawes_redman.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dawes_redman - Win32 Release" || "$(CFG)" == "dawes_redman - Win32 Debug"
SOURCE=.\dawes_redman.c

"$(INTDIR)\dawes_redman.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\hrv\frequency.c

"$(INTDIR)\frequency.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\hrv\statistics.c

"$(INTDIR)\statistics.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "dawes_redman - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\process\dawes_redman"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\process\dawes_redman"

!ELSEIF  "$(CFG)" == "dawes_redman - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\process\dawes_redman"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\process\dawes_redman"

!ENDIF 


!ENDIF 

