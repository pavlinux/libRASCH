# Microsoft Developer Studio Generated NMAKE File, Based on simple_calc_dlg.dsp
!IF "$(CFG)" == ""
CFG=simple_calc_dlg - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. simple_calc_dlg - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "simple_calc_dlg - Win32 Release" && "$(CFG)" != "simple_calc_dlg - Win32 Debug"
!MESSAGE Ung�ltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie k�nnen beim Ausf�hren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "simple_calc_dlg.mak" CFG="simple_calc_dlg - Win32 Debug"
!MESSAGE 
!MESSAGE F�r die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "simple_calc_dlg - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "simple_calc_dlg - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\simple_calc_dlg.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\simple_calc_dlg.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\simple_calc_dlg.obj"
	-@erase "$(INTDIR)\simple_calc_dlg.res"
	-@erase "$(INTDIR)\simple_calc_dlg_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\simple_calc_dlg.exp"
	-@erase "$(OUTDIR)\simple_calc_dlg.lib"
	-@erase "..\..\bin\Release\simple_calc_dlg.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLE_CALCL_DLG_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\simple_calc_dlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\simple_calc_dlg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\simple_calc_dlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\simple_calc_dlg.pdb" /machine:I386 /out:"../../bin/Release/simple_calc_dlg.dll" /implib:"$(OUTDIR)\simple_calc_dlg.lib" 
LINK32_OBJS= \
	"$(INTDIR)\simple_calc_dlg.obj" \
	"$(INTDIR)\simple_calc_dlg_mfc.obj" \
	"$(INTDIR)\simple_calc_dlg.res" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\simple_calc_dlg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\simple_calc_dlg.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\simple_calc_dlg.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\simple_calc_dlg.obj"
	-@erase "$(INTDIR)\simple_calc_dlg.res"
	-@erase "$(INTDIR)\simple_calc_dlg_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\simple_calc_dlg.exp"
	-@erase "$(OUTDIR)\simple_calc_dlg.lib"
	-@erase "$(OUTDIR)\simple_calc_dlg.pdb"
	-@erase "..\..\bin\Debug\simple_calc_dlg.dll"
	-@erase "..\..\bin\Debug\simple_calc_dlg.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SIMPLE_CALC_DLG_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\simple_calc_dlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\simple_calc_dlg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\simple_calc_dlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\simple_calc_dlg.pdb" /debug /machine:I386 /out:"../../bin/Debug/simple_calc_dlg.dll" /implib:"$(OUTDIR)\simple_calc_dlg.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\simple_calc_dlg.obj" \
	"$(INTDIR)\simple_calc_dlg_mfc.obj" \
	"$(INTDIR)\simple_calc_dlg.res" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\simple_calc_dlg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("simple_calc_dlg.dep")
!INCLUDE "simple_calc_dlg.dep"
!ELSE 
!MESSAGE Warning: cannot find "simple_calc_dlg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "simple_calc_dlg - Win32 Release" || "$(CFG)" == "simple_calc_dlg - Win32 Debug"
SOURCE=.\simple_calc_dlg.cpp

"$(INTDIR)\simple_calc_dlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\simple_calc_dlg_mfc.cpp

"$(INTDIR)\simple_calc_dlg_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\simple_calc_dlg.rc

"$(INTDIR)\simple_calc_dlg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "simple_calc_dlg - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\simple_calc_dlg"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\simple_calc_dlg"

!ELSEIF  "$(CFG)" == "simple_calc_dlg - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\simple_calc_dlg"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\simple_calc_dlg"

!ENDIF 


!ENDIF 

