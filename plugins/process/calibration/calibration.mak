# Microsoft Developer Studio Generated NMAKE File, Based on calibration.dsp
!IF "$(CFG)" == ""
CFG=calibration - Win32 Release
!MESSAGE Keine Konfiguration angegeben. calibration - Win32 Release wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "calibration - Win32 Release" && "$(CFG)" != "calibration - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "calibration.mak" CFG="calibration - Win32 Release"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "calibration - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "calibration - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "calibration - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\bin\Release\calibration.dll"


CLEAN :
	-@erase "$(INTDIR)\calibration.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\calibration.exp"
	-@erase "$(OUTDIR)\calibration.lib"
	-@erase "..\..\bin\Release\calibration.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "respiration_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\calibration.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calibration.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\calibration.pdb" /machine:I386 /out:"../../bin/Release/calibration.dll" /implib:"$(OUTDIR)\calibration.lib" 
LINK32_OBJS= \
	"$(INTDIR)\calibration.obj"

"..\..\bin\Release\calibration.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "calibration - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\bin\Debug\calibration.dll"


CLEAN :
	-@erase "$(INTDIR)\calibration.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\calibration.exp"
	-@erase "$(OUTDIR)\calibration.lib"
	-@erase "$(OUTDIR)\calibration.pdb"
	-@erase "..\..\bin\Debug\calibration.dll"
	-@erase "..\..\bin\Debug\calibration.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "respiration_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\calibration.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calibration.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\calibration.pdb" /debug /machine:I386 /out:"../../bin/Debug/calibration.dll" /implib:"$(OUTDIR)\calibration.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\calibration.obj"

"..\..\bin\Debug\calibration.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("calibration.dep")
!INCLUDE "calibration.dep"
!ELSE 
!MESSAGE Warning: cannot find "calibration.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "calibration - Win32 Release" || "$(CFG)" == "calibration - Win32 Debug"
SOURCE=.\calibration.c

"$(INTDIR)\calibration.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

