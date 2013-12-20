// snmptree.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "snmptree.h"
#include <afxadv.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "usermsg.h"
#include "MainFrm.h"
#include "snmpcore.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSnmptreeApp

BEGIN_MESSAGE_MAP(CSnmptreeApp, CWinApp)
    //{{AFX_MSG_MAP(CSnmptreeApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSnmptreeApp construction

CSnmptreeApp::CSnmptreeApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSnmptreeApp object

CSnmptreeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSnmptreeApp initialization

BOOL CSnmptreeApp::InitInstance()
{
    if (!AfxSocketInit())
    {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();            // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();    // Call this when linking to MFC statically
#endif

    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    SetRegistryKey(_T("Mbuf"));

        // 6-Feb-05 ; load global variable settings
        int retcode = init_globals();
        ASSERT( retcode == 0 );

//    LoadStdProfileSettings(6);

    char *mibdir = netsnmp_get_mib_directory();

    /* Fire up the Net-SNMP library */
    if( snmpcore_init() != SNMPCORE_SUCCESS ) {
        // FIXME -- need better error message
        AfxMessageBox( "Net-SNMP failed to start" );
        return FALSE;
    }

    mibdir = netsnmp_get_mib_directory();

    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object.

    CMainFrame* pFrame = new CMainFrame;
    m_pMainWnd = pFrame;

    // create and load the frame with its resources

    pFrame->LoadFrame(IDR_MAINFRAME,
        WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
        NULL);
    
    // The one and only window has been initialized, so show and update it.
    pFrame->ShowWindow(SW_SHOW);
    pFrame->UpdateWindow();

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSnmptreeApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    CString    m_NetSNMPVersion;
    CString    m_BuildDate;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    m_NetSNMPVersion = _T("");
    m_BuildDate = _T("");
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    DDX_Text(pDX, IDC_NETSNMP_STATIC, m_NetSNMPVersion);
    DDV_MaxChars(pDX, m_NetSNMPVersion, 32);
    DDX_Text(pDX, IDC_BUILD_DATE_STATIC, m_BuildDate);
    DDV_MaxChars(pDX, m_BuildDate, 32);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSnmptreeApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSnmptreeApp message handlers


int CSnmptreeApp::ExitInstance() 
{
    // Add your specialized code here and/or call the base class
    snmpcore_shutdown();
    
    cleanup_globals();

    return CWinApp::ExitInstance();
}

CDocument* CSnmptreeApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
    CString agent_path, *agent;

    // create a non-stack string to pass to the MainFrm
    agent = new CString( lpszFileName );
    if( agent == NULL ) {
        AfxMessageBox( _T("out of memory") );

        // fool CWinApp::OnOpenRecentFile() into thinking we handled the document
        return (CDocument *)1;
    }

    // filenames have path repended on them so clean that off before sending
    // the string
    agent_path = lpszFileName;
    int pos = agent_path.ReverseFind( '\\' );
    if( pos > -1 ) {
        // -1 to cut '\' 
        *agent = agent_path.Right( agent_path.GetLength()-pos-1 );
    }
    else {
        *agent = agent_path;
    }

    LRESULT retcode = m_pMainWnd->SendMessage( MSG_SET_AGENT, (WPARAM)agent, NULL );

    if( retcode != 0 ) {
        // MainFrm thought IP address was crap so remove from display
        return NULL;
    }

    // fool CWinApp::OnOpenRecentFile() into thinking we handled the document
    return (CDocument *)1;
//    return CWinApp::OpenDocumentFile(lpszFileName);
}

int CSnmptreeApp::GetRecentFileCount( void )
{
    return m_pRecentFileList->GetSize();
}

CString CSnmptreeApp::GetRecentFile( int index )
{
    CString tmp, agent;
     
    tmp = (*m_pRecentFileList)[ index ];

    // filenames have path repended on them so clean that off before returning 
    // the string
    int pos = tmp.ReverseFind( '\\' );
    if( pos > -1 ) {
        // -1 to cut '\' 
        agent = tmp.Right( tmp.GetLength()-pos-1 );
    }
    else {
        agent = tmp;
    }

    return agent;
}


BOOL CAboutDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    // Add extra initialization here
    m_BuildDate = __DATE__;
    m_NetSNMPVersion.Format( _T("Net-SNMP Version %s"), netsnmp_get_version() );
    UpdateData( FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
