; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "snmptree.h"
LastPage=0

ClassCount=13
Class1=CSnmptreeApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=9
Resource1=IDD_PASSPHRASE_DIALOG
Class2=CChildView
Class5=STreeView
Class6=SListView
Resource2=IDD_ABOUTBOX
Class7=CConnectDialog
Class8=CGetDataDialog
Resource3=IDR_MAINFRAME
Class9=CSetDialog
Resource4=IDD_SELECT_AGENT_DIALOG
Class10=CPrefDialog
Resource5=IDD_SET_DIALOG
Class11=PassPhrase
Resource6=IDD_CONNECT_DIALOG
Class12=CSelectAgent
Resource7=IDD_PREFERENCES_DIALOG
Class13=CEditAgent
Resource8=IDD_EDIT_AGENT_DIALOG
Resource9=IDR_SELECT_TOOLBAR

[CLS:CSnmptreeApp]
Type=0
HeaderFile=snmptree.h
ImplementationFile=snmptree.cpp
Filter=N
LastObject=CSnmptreeApp
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CChildView]
Type=0
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=N
LastObject=CChildView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=snmptree.cpp
ImplementationFile=snmptree.cpp
Filter=D
LastObject=IDC_NETSNMP_STATIC
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_NETSNMP_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUILD_DATE_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_CONNECT
Command2=ID_FILE_PASSPHRASE
Command3=ID_FILE_FORGET_PASSPHRASE
Command4=ID_FILE_LOGSESSION
Command5=ID_APP_EXIT
Command6=ID_EDIT_CLEAR
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PREFERENCES
Command9=ID_VIEW_TOOLBAR
Command10=ID_VIEW_STATUS_BAR
Command11=ID_APP_ABOUT
Command12=ID_TEST_CONNHIST
CommandCount=12

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CLEAR
Command6=ID_EDIT_CUT
Command7=ID_NEXT_PANE
Command8=ID_PREV_PANE
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_EDIT_CUT
Command12=ID_EDIT_UNDO
CommandCount=12

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_EDIT_CLEAR
Command3=ID_EDIT_COPY
Command4=ID_SNMP_GET
Command5=ID_SNMP_SET
Command6=ID_SNMP_STOP
CommandCount=6

[CLS:STreeView]
Type=0
HeaderFile=STreeView.h
ImplementationFile=STreeView.cpp
BaseClass=CTreeView
Filter=C
LastObject=ID_APP_ABOUT
VirtualFilter=VWC

[CLS:SListView]
Type=0
HeaderFile=SListView.h
ImplementationFile=SListView.cpp
BaseClass=CListView
Filter=C
LastObject=SListView
VirtualFilter=VWC

[CLS:CConnectDialog]
Type=0
HeaderFile=ConnectDialog.h
ImplementationFile=ConnectDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CConnectDialog

[CLS:CGetDataDialog]
Type=0
HeaderFile=GetDataDialog.h
ImplementationFile=GetDataDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CGetDataDialog

[DLG:IDD_SET_DIALOG]
Type=1
Class=CSetDialog
ControlCount=10
Control1=IDC_VALUE_EDIT,edit,1350631552
Control2=IDCANCEL,button,1342242816
Control3=IDC_PREV_BUTTON,button,1342242816
Control4=IDC_NEXT_BUTTON,button,1342242816
Control5=IDOK,button,1342242817
Control6=IDC_NAME_EDIT,edit,1350568064
Control7=IDC_TYPE_EDIT,edit,1350568064
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[CLS:CSetDialog]
Type=0
HeaderFile=SetDialog.h
ImplementationFile=SetDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CSetDialog
VirtualFilter=dWC

[DLG:IDD_PREFERENCES_DIALOG]
Type=1
Class=CPrefDialog
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PREF_DEBUG_CHECK,button,1342242851
Control4=IDC_STATIC,static,1342308352
Control5=IDC_MIBDIR_EDIT,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_MIBS_EDIT,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[CLS:CPrefDialog]
Type=0
HeaderFile=PrefDialog.h
ImplementationFile=PrefDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CPrefDialog
VirtualFilter=dWC

[DLG:IDD_PASSPHRASE_DIALOG]
Type=1
Class=PassPhrase
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PASSPHRASE_EDIT,edit,1350631584
Control4=IDC_STATIC,static,1342308352

[CLS:PassPhrase]
Type=0
HeaderFile=PassPhrase.h
ImplementationFile=PassPhrase.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_APP_ABOUT

[DLG:IDD_EDIT_AGENT_DIALOG]
Type=1
Class=CEditAgent
ControlCount=12
Control1=IDC_AGENT_GET_COMMUNITY,edit,1350631456
Control2=IDC_AGENT_SET_COMMUNITY,edit,1350631456
Control3=IDC_SHOW_COMMUNITY_CHECK,button,1342242819
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,button,1342308359
Control10=IDC_RADIO_SNMPv1,button,1342308361
Control11=IDC_RADIO_SNMPv2c,button,1342177289
Control12=IDC_AGENT_EDIT,edit,1350631552

[DLG:IDD_SELECT_AGENT_DIALOG]
Type=1
Class=CSelectAgent
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_AGENT_LIST,SysListView32,1350631429

[CLS:CSelectAgent]
Type=0
HeaderFile=SelectAgent.h
ImplementationFile=SelectAgent.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSelectAgent

[DLG:IDD_CONNECT_DIALOG]
Type=1
Class=CConnectDialog
ControlCount=12
Control1=IDC_AGENT_GET_COMMUNITY,edit,1350631456
Control2=IDC_AGENT_SET_COMMUNITY,edit,1350631456
Control3=IDC_SHOW_COMMUNITY_CHECK,button,1342242819
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,button,1342308359
Control10=IDC_RADIO_SNMPv1,button,1342308361
Control11=IDC_RADIO_SNMPv2c,button,1342177289
Control12=IDC_AGENT_EDIT,edit,1350631552

[CLS:CEditAgent]
Type=0
HeaderFile=EditAgent.h
ImplementationFile=EditAgent.cpp
BaseClass=CDialog
Filter=D
LastObject=CEditAgent
VirtualFilter=dWC

[TB:IDR_SELECT_TOOLBAR]
Type=1
Command1=ID_BUTTON32783
Command2=ID_BUTTON32784
Command3=ID_BUTTON32785
CommandCount=3

