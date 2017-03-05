; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mtklab_mfc.h"
LastPage=0

ClassCount=7
Class1=CMtklab_mfcApp
Class2=CMtklab_mfcDoc
Class3=CMtklab_mfcView
Class4=CMainFrame

ResourceCount=9
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDR_MTKLABTYPE
Class5=CChildFrame
Class6=CAboutDlg
Resource4=IDD_ABOUTBOX (Englisch (USA))
Resource5=IDR_MTKLABTYPE (Englisch (USA))
Resource6=IDR_MAINFRAME (Englisch (USA))
Class7=process_dlg
Resource7=IDD_PROCESS_DLG
Resource8=IDR_RASCHLABTYPE (Englisch (USA))
Resource9=IDD_PROGRESS_DLG

[CLS:CMtklab_mfcApp]
Type=0
HeaderFile=mtklab_mfc.h
ImplementationFile=mtklab_mfc.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CMtklab_mfcApp

[CLS:CMtklab_mfcDoc]
Type=0
HeaderFile=mtklab_mfcDoc.h
ImplementationFile=mtklab_mfcDoc.cpp
Filter=N

[CLS:CMtklab_mfcView]
Type=0
HeaderFile=mtklab_mfcView.h
ImplementationFile=mtklab_mfcView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame



[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M
LastObject=CChildFrame
BaseClass=CMDIChildWnd
VirtualFilter=mfWC


[CLS:CAboutDlg]
Type=0
HeaderFile=mtklab_mfc.cpp
ImplementationFile=mtklab_mfc.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_PROGNAME

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
CommandCount=8
Command8=ID_APP_ABOUT

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
CommandCount=8
Command8=ID_APP_ABOUT

[MNU:IDR_MTKLABTYPE]
Type=1
Class=CMtklab_mfcView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
CommandCount=21
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
CommandCount=14
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE


[TB:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_PRINT
Command4=ID_APP_ABOUT
CommandCount=4

[MNU:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_GENERAL_PLUGININFOS
Command7=ID_GENERAL_EVALUATIONINFOS
Command8=ID_GENERAL_SAVEASCIIEVALVALUES
Command9=ID_DETECT_DETECT
Command10=ID_DETECT_DETECTCTG
Command11=ID_DETECT_MAKECLASSIFICATIONS
Command12=ID_DETECT_MAKEBLOODPRESSURE
Command13=ID_DETECT_PERFORMCALCULATIONS
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_VIEW_PLOT
Command17=ID_APP_ABOUT
CommandCount=17

[MNU:IDR_MTKLABTYPE (Englisch (USA))]
Type=1
Class=CMtklab_mfcView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
CommandCount=21

[ACL:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (Englisch (USA))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_PROGNAME,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:process_dlg]
Type=0
HeaderFile=process_dlg.h
ImplementationFile=process_dlg.cpp
BaseClass=CDialog
Filter=D
LastObject=process_dlg
VirtualFilter=dWC

[DLG:IDD_PROCESS_DLG]
Type=1
Class=process_dlg
ControlCount=4
Control1=IDCANCEL,button,1342242816
Control2=IDC_TEXT,static,1342308352
Control3=IDC_PROGRESS,msctls_progress32,1350565888
Control4=IDC_PERCENT,static,1342308352

[DLG:IDD_PROGRESS_DLG]
Type=1
Class=?
ControlCount=4
Control1=IDCANCEL,button,1342242816
Control2=IDC_TEXT,static,1342308352
Control3=IDC_PROGRESS,msctls_progress32,1350565888
Control4=IDC_PERCENT,static,1342308352

[MNU:IDR_RASCHLABTYPE (Englisch (USA))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_VIEW_TOOLBAR
Command16=ID_VIEW_STATUS_BAR
Command17=ID_WINDOW_NEW
Command18=ID_WINDOW_CASCADE
Command19=ID_WINDOW_TILE_HORZ
Command20=ID_WINDOW_ARRANGE
Command21=ID_APP_ABOUT
CommandCount=21

