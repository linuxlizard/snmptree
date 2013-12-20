// EditAgent.cpp : implementation file
//

#include "stdafx.h"
#include "snmptree.h"
#include "EditAgent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditAgent dialog


CEditAgent::CEditAgent(CWnd* pParent /*=NULL*/)
	: CDialog(CEditAgent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditAgent)
	m_GetCommunity = _T("");
	m_SetCommunity = _T("");
	m_SNMPVersion = -1;
	m_AgentAddress = _T("");
	//}}AFX_DATA_INIT
}


void CEditAgent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditAgent)
	DDX_Control(pDX, IDC_AGENT_SET_COMMUNITY, m_SetCommunityEdit);
	DDX_Control(pDX, IDC_AGENT_GET_COMMUNITY, m_GetCommunityEdit);
	DDX_Control(pDX, IDC_SHOW_COMMUNITY_CHECK, m_ShowCommunityCheck);
	DDX_Control(pDX, IDC_AGENT_EDIT, m_AgentEdit);
	DDX_Text(pDX, IDC_AGENT_GET_COMMUNITY, m_GetCommunity);
	DDV_MaxChars(pDX, m_GetCommunity, 64);
	DDX_Text(pDX, IDC_AGENT_SET_COMMUNITY, m_SetCommunity);
	DDV_MaxChars(pDX, m_SetCommunity, 64);
	DDX_Radio(pDX, IDC_RADIO_SNMPv1, m_SNMPVersion);
	DDX_Text(pDX, IDC_AGENT_EDIT, m_AgentAddress);
	DDV_MaxChars(pDX, m_AgentAddress, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditAgent, CDialog)
	//{{AFX_MSG_MAP(CEditAgent)
	ON_BN_CLICKED(IDC_SHOW_COMMUNITY_CHECK, OnShowCommunityCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditAgent message handlers

void CEditAgent::OnShowCommunityCheck() 
{
    // m_ShowCommunity is currently the previous state; OnShowCommunityCheck()
    // called before DDE/DDX handled so go directly to the source
    if( m_ShowCommunityCheck.GetCheck() ) {
        // disable Password style
        m_GetCommunityEdit.SetPasswordChar( 0 );
        m_SetCommunityEdit.SetPasswordChar( 0 );
    }
    else {
        // enable password style
        m_GetCommunityEdit.SetPasswordChar( '*' );
        m_SetCommunityEdit.SetPasswordChar( '*' );
    }

    m_GetCommunityEdit.Invalidate();
    m_SetCommunityEdit.Invalidate();	
}

void CEditAgent::OnOK() 
{
    int pos;
    u_long ip;

    UpdateData( TRUE );

    // inet_addr() is entirely too forgiving so make the check harder.

    // make sure we have three '.'s
    pos = m_AgentAddress.Find( '.' );
    if( pos == -1 ) {
        goto bad_ip;
    }
    pos = m_AgentAddress.Find( '.', pos );
    if( pos == -1 ) {
        goto bad_ip;
    }
    pos = m_AgentAddress.Find( '.', pos );
    if( pos == -1 ) {
        goto bad_ip;
    }

    ip = inet_addr( m_AgentAddress );
    if( ip == INADDR_NONE || ip == INADDR_ANY ) {
        goto bad_ip;
    }
    
    CDialog::OnOK();
    return;

bad_ip:
    MessageBox( _T("Invalid IP Address.\nIP Address should be in format like nnn.nnn.nnn.nnn"), 
            _T("Error"), MB_ICONERROR );
    return;
}

BOOL CEditAgent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    m_AgentEdit.SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
