# Microsoft Developer Studio Generated NMAKE File, Based on plot_view.dsp
!IF "$(CFG)" == ""
CFG=plot_view - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. plot_view - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "plot_view - Win32 Release" && "$(CFG)" != "plot_view - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "plot_view.mak" CFG="plot_view - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "plot_view - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plot_view - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "plot_view - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Release\plot_view.dll"

!ELSE 

ALL : "librasch - Win32 Release" "..\..\bin\Release\plot_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\color_button.obj"
	-@erase "$(INTDIR)\pair_opt_dlg_mfc.obj"
	-@erase "$(INTDIR)\plot_view.res"
	-@erase "$(INTDIR)\ra_plot.obj"
	-@erase "$(INTDIR)\ra_plot_general.obj"
	-@erase "$(INTDIR)\ra_plot_mfc.obj"
	-@erase "$(INTDIR)\sel_data_dlg_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\plot_view.exp"
	-@erase "$(OUTDIR)\plot_view.lib"
	-@erase "..\..\bin\Release\plot_view.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "plot_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\plot_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\plot_view.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\plot_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42.lib librasch.lib ../../../Release/pl_general.obj ../../../Release/ra_file.obj intl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\plot_view.pdb" /machine:I386 /out:"../../bin/Release/plot_view.dll" /implib:"$(OUTDIR)\plot_view.lib" 
LINK32_OBJS= \
	"$(INTDIR)\color_button.obj" \
	"$(INTDIR)\pair_opt_dlg_mfc.obj" \
	"$(INTDIR)\ra_plot.obj" \
	"$(INTDIR)\ra_plot_general.obj" \
	"$(INTDIR)\ra_plot_mfc.obj" \
	"$(INTDIR)\sel_data_dlg_mfc.obj" \
	"$(INTDIR)\plot_view.res" \
	"..\..\..\librasch.lib"

"..\..\bin\Release\plot_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "plot_view - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\bin\Debug\plot_view.dll"

!ELSE 

ALL : "librasch - Win32 Debug" "..\..\bin\Debug\plot_view.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"librasch - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\color_button.obj"
	-@erase "$(INTDIR)\pair_opt_dlg_mfc.obj"
	-@erase "$(INTDIR)\plot_view.res"
	-@erase "$(INTDIR)\ra_plot.obj"
	-@erase "$(INTDIR)\ra_plot_general.obj"
	-@erase "$(INTDIR)\ra_plot_mfc.obj"
	-@erase "$(INTDIR)\sel_data_dlg_mfc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\plot_view.exp"
	-@erase "$(OUTDIR)\plot_view.lib"
	-@erase "$(OUTDIR)\plot_view.pdb"
	-@erase "..\..\bin\Debug\plot_view.dll"
	-@erase "..\..\bin\Debug\plot_view.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "plot_view_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /Fp"$(INTDIR)\plot_view.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\plot_view.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\plot_view.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mfcs42d.lib libraschd.lib ../../../Debug/pl_general.obj ../../../Debug/ra_file.obj intl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\plot_view.pdb" /debug /machine:I386 /out:"../../bin/Debug/plot_view.dll" /implib:"$(OUTDIR)\plot_view.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\color_button.obj" \
	"$(INTDIR)\pair_opt_dlg_mfc.obj" \
	"$(INTDIR)\ra_plot.obj" \
	"$(INTDIR)\ra_plot_general.obj" \
	"$(INTDIR)\ra_plot_mfc.obj" \
	"$(INTDIR)\sel_data_dlg_mfc.obj" \
	"$(INTDIR)\plot_view.res" \
	"..\..\..\libraschd.lib"

"..\..\bin\Debug\plot_view.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("plot_view.dep")
!INCLUDE "plot_view.dep"
!ELSE 
!MESSAGE Warning: cannot find "plot_view.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "plot_view - Win32 Release" || "$(CFG)" == "plot_view - Win32 Debug"
SOURCE=.\color_button.cpp

"$(INTDIR)\color_button.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pair_opt_dlg_mfc.cpp

"$(INTDIR)\pair_opt_dlg_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_plot.cpp

"$(INTDIR)\ra_plot.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_plot_general.cpp

"$(INTDIR)\ra_plot_general.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_plot_mfc.cpp

"$(INTDIR)\ra_plot_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sel_data_dlg_mfc.cpp

"$(INTDIR)\sel_data_dlg_mfc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ra_plot.rc

"$(INTDIR)\plot_view.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "plot_view - Win32 Release"

"librasch - Win32 Release" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" 
   cd ".\plugins\view\plot_view"

"librasch - Win32 ReleaseCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Release" RECURSE=1 CLEAN 
   cd ".\plugins\view\plot_view"

!ELSEIF  "$(CFG)" == "plot_view - Win32 Debug"

"librasch - Win32 Debug" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" 
   cd ".\plugins\view\plot_view"

"librasch - Win32 DebugCLEAN" : 
   cd "\user\rasch\source\librasch"
   $(MAKE) /$(MAKEFLAGS) /F .\librasch.mak CFG="librasch - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\plugins\view\plot_view"

!ENDIF 


!ENDIF 

