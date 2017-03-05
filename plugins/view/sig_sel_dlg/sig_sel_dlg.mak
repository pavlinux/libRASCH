# Microsoft Developer Studio Generated NMAKE File, Based on sig_sel_dlg.dsp
!IF "$(CFG)" == ""
CFG=sig_sel_dlg - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. sig_sel_dlg - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "sig_sel_dlg - Win32 Release" && "$(CFG)" != "sig_sel_dlg - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "sig_sel_dlg.mak" CFG="sig_sel_dlg - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "sig_sel_dlg - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sig_sel_dlg - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "sig_sel_dlg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\sig_sel_dlg.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\sig_sel_dlg.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\sig_sel_dlg.obj"
	-@erase "$(INTDIR)\sig_sel_dlg.res"
	-@erase "$(INTDIR)\sig_sel_dlg_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\sig_sel_dlg.exp"
	-@erase "$(OUTDIR)\sig_sel_dlg.lib"
	-@erase "..\..\bin\Release\sig_sel_dlg.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "sig_sel_dlg_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\sig_sel_dlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\sig_sel_dlg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sig_sel_dlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\sig_sel_dlg.pdb" /machine:I386 /out:"../../bin/Release/sig_sel_dlg.dll" /implib:"$(OUTDIR)\sig_sel_dlg.lib" 
LINK32_OBJS= \
	"$(INTDIR)\sig_sel_dlg.obj" \
	"$(INTDIR)\sig_sel_dlg_mfc.obj" \
	"$(INTDIR)\sig_sel_dlg.res" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\sig_sel_dlg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sig_sel_dlg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\sig_sel_dlg.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\sig_sel_dlg.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\sig_sel_dlg.obj"
	-@erase "$(INTDIR)\sig_sel_dlg.res"
	-@erase "$(INTDIR)\sig_sel_dlg_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sig_sel_dlg.exp"
	-@erase "$(OUTDIR)\sig_sel_dlg.lib"
	-@erase "$(OUTDIR)\sig_sel_dlg.pdb"
	-@erase "..\..\bin\Debug\sig_sel_dlg.dll"
	-@erase "..\..\bin\Debug\sig_sel_dlg.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "sig_sel_dlg_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\sig_sel_dlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\sig_sel_dlg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sig_sel_dlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\sig_sel_dlg.pdb" /debug /machine:I386 /out:"../../bin/Debug/sig_sel_dlg.dll" /implib:"$(OUTDIR)\sig_sel_dlg.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\sig_sel_dlg.obj" \
	"$(INTDIR)\sig_sel_dlg_mfc.obj" \
	"$(INTDIR)\sig_sel_dlg.res" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\sig_sel_dlg.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("sig_sel_dlg.dep")
!INCLUDE "sig_sel_dlg.dep"
!ELSE 
!MESSAGE Warning: cannot find "sig_sel_dlg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "sig_sel_dlg - Win32 Release" || "$(CFG)" == "sig_sel_dlg - Win32 Debug"
SOURCE=.\sig_sel_dlg.cpp

"$(INTDIR)\sig_sel_dlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sig_sel_dlg_mfc.cpp

"$(INTDIR)\sig_sel_dlg_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sig_sel_dlg.rc

"$(INTDIR)\sig_sel_dlg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "sig_sel_dlg - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\sig_sel_dlg"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\sig_sel_dlg"

!ELSEIF  "$(CFG)" == "sig_sel_dlg - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\sig_sel_dlg"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\sig_sel_dlg"

!ENDIF 


!ENDIF 

