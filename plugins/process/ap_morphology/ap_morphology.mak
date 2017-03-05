# Microsoft Developer Studio Generated NMAKE File, Based on ap_morphology.dsp
!IF "$(CFG)" == ""
CFG=ap_morphology - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. ap_morphology - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "ap_morphology - Win32 Release" && "$(CFG)" != "ap_morphology - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ap_morphology.mak" CFG="ap_morphology - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ap_morphology - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ap_morphology - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ap_morphology - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\bin\Release\ap_morphology.dll"


CLEAN :
	-@erase "$(INTDIR)\ap_morphology.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ap_morphology.exp"
	-@erase "$(OUTDIR)\ap_morphology.lib"
	-@erase "..\..\bin\Release\ap_morphology.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ap_morphology_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\ap_morphology.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ap_morphology.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ap_morphology.pdb" /machine:I386 /out:"../../bin/Release/ap_morphology.dll" /implib:"$(OUTDIR)\ap_morphology.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ap_morphology.obj"

"..\..\bin\Release\ap_morphology.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ap_morphology - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\bin\Debug\ap_morphology.dll"


CLEAN :
	-@erase "$(INTDIR)\ap_morphology.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ap_morphology.exp"
	-@erase "$(OUTDIR)\ap_morphology.lib"
	-@erase "$(OUTDIR)\ap_morphology.pdb"
	-@erase "..\..\bin\Debug\ap_morphology.dll"
	-@erase "..\..\bin\Debug\ap_morphology.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ap_morphology_EXPORTS" /D "_LITTLE_ENDIAN_MACHINE" /Fp"$(INTDIR)\ap_morphology.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ap_morphology.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ap_morphology.pdb" /debug /machine:I386 /out:"../../bin/Debug/ap_morphology.dll" /implib:"$(OUTDIR)\ap_morphology.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ap_morphology.obj"

"..\..\bin\Debug\ap_morphology.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ap_morphology.dep")
!INCLUDE "ap_morphology.dep"
!ELSE 
!MESSAGE Warning: cannot find "ap_morphology.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ap_morphology - Win32 Release" || "$(CFG)" == "ap_morphology - Win32 Debug"
SOURCE=.\ap_morphology.c

"$(INTDIR)\ap_morphology.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

