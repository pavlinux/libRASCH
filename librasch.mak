# Microsoft Developer Studio Generated NMAKE File, Based on librasch.dsp
!IF "$(CFG)" == ""
CFG=librasch - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. librasch - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "librasch - Win32 Release" && "$(CFG)" != "librasch - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "librasch.mak" CFG="librasch - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "librasch - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "librasch - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "librasch - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\librasch.dll"

!ELSE 

ALL : ".\librasch.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\estimate_ch_infos.obj"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\event_prop.obj"
	-@erase "$(INTDIR)\handle_plugin_win32.obj"
	-@erase "$(INTDIR)\i18n.obj"
	-@erase "$(INTDIR)\linked_list.obj"
	-@erase "$(INTDIR)\load_xml.obj"
	-@erase "$(INTDIR)\mime64.obj"
	-@erase "$(INTDIR)\pl_comm.obj"
	-@erase "$(INTDIR)\pl_general.obj"
	-@erase "$(INTDIR)\ra.obj"
	-@erase "$(INTDIR)\ra_file.obj"
	-@erase "$(INTDIR)\save_xml.obj"
	-@erase "$(INTDIR)\template.obj"
	-@erase "$(INTDIR)\value.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\librasch.dll"
	-@erase ".\librasch.exp"
	-@erase ".\librasch.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D LOCALEDIR=\"\" /D PACKAGE=\"librasch\" /Fp"$(INTDIR)\librasch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\librasch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libxml2.lib intl.lib iconv.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\librasch.pdb" /machine:I386 /out:"librasch.dll" /implib:"librasch.lib" 
LINK32_OBJS= \
	"$(INTDIR)\estimate_ch_infos.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\event_prop.obj" \
	"$(INTDIR)\handle_plugin_win32.obj" \
	"$(INTDIR)\i18n.obj" \
	"$(INTDIR)\linked_list.obj" \
	"$(INTDIR)\load_xml.obj" \
	"$(INTDIR)\mime64.obj" \
	"$(INTDIR)\pl_comm.obj" \
	"$(INTDIR)\pl_general.obj" \
	"$(INTDIR)\ra.obj" \
	"$(INTDIR)\ra_file.obj" \
	"$(INTDIR)\save_xml.obj" \
	"$(INTDIR)\template.obj" \
	"$(INTDIR)\value.obj"

".\librasch.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\libraschd.dll"

!ELSE 

ALL : ".\libraschd.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\estimate_ch_infos.obj"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\event_prop.obj"
	-@erase "$(INTDIR)\handle_plugin_win32.obj"
	-@erase "$(INTDIR)\i18n.obj"
	-@erase "$(INTDIR)\linked_list.obj"
	-@erase "$(INTDIR)\load_xml.obj"
	-@erase "$(INTDIR)\mime64.obj"
	-@erase "$(INTDIR)\pl_comm.obj"
	-@erase "$(INTDIR)\pl_general.obj"
	-@erase "$(INTDIR)\ra.obj"
	-@erase "$(INTDIR)\ra_file.obj"
	-@erase "$(INTDIR)\save_xml.obj"
	-@erase "$(INTDIR)\template.obj"
	-@erase "$(INTDIR)\value.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libraschd.pdb"
	-@erase ".\libraschd.dll"
	-@erase ".\libraschd.exp"
	-@erase ".\libraschd.ilk"
	-@erase ".\libraschd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D "TRACE_SIGNAL" /Fp"$(INTDIR)\librasch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\librasch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libxml2.lib intl.lib iconv.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libraschd.pdb" /debug /machine:I386 /out:"libraschd.dll" /implib:"libraschd.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\estimate_ch_infos.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\event_prop.obj" \
	"$(INTDIR)\handle_plugin_win32.obj" \
	"$(INTDIR)\i18n.obj" \
	"$(INTDIR)\linked_list.obj" \
	"$(INTDIR)\load_xml.obj" \
	"$(INTDIR)\mime64.obj" \
	"$(INTDIR)\pl_comm.obj" \
	"$(INTDIR)\pl_general.obj" \
	"$(INTDIR)\ra.obj" \
	"$(INTDIR)\ra_file.obj" \
	"$(INTDIR)\save_xml.obj" \
	"$(INTDIR)\template.obj" \
	"$(INTDIR)\value.obj"

".\libraschd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("librasch.dep")
!INCLUDE "librasch.dep"
!ELSE 
!MESSAGE Warning: cannot find "librasch.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "librasch - Win32 Release" || "$(CFG)" == "librasch - Win32 Debug"
SOURCE=.\src\estimate_ch_infos.c

"$(INTDIR)\estimate_ch_infos.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\eval.c

"$(INTDIR)\eval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\event.c

"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\event_prop.c

"$(INTDIR)\event_prop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\handle_plugin_win32.c

"$(INTDIR)\handle_plugin_win32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\shared\i18n.c

!IF  "$(CFG)" == "librasch - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D LOCALEDIR=\"\" /D PACKAGE=\"librasch\" /Fp"$(INTDIR)\librasch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\i18n.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D "TRACE_SIGNAL" /Fp"$(INTDIR)\librasch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\i18n.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\linked_list.c

"$(INTDIR)\linked_list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\load_xml.c

"$(INTDIR)\load_xml.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\mime64.c

"$(INTDIR)\mime64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\pl_comm.c

"$(INTDIR)\pl_comm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\shared\pl_general.c

"$(INTDIR)\pl_general.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\ra.c

!IF  "$(CFG)" == "librasch - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D LOCALEDIR=\"\" /D PACKAGE=\"librasch\" /Fp"$(INTDIR)\librasch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBRASCH_EXPORTS" /D LOCALEDIR=\"\" /D PACKAGE=\"librasch\" /Fp"$(INTDIR)\librasch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ra.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\shared\ra_file.c

"$(INTDIR)\ra_file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\save_xml.c

"$(INTDIR)\save_xml.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\eval\template.c

"$(INTDIR)\template.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\value.c

"$(INTDIR)\value.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "librasch - Win32 Release"

!ELSEIF  "$(CFG)" == "librasch - Win32 Debug"

!ENDIF 


!ENDIF 

