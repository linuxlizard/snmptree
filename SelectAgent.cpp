// SelectAgent.cpp : implementation file
//

#include "stdafx.h"
#include <afxtempl.h>

#include "snmptree.h"
#include "connhist.h"
#include "SelectAgent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectAgent dialog


CSelectAgent::CSelectAgent(CWnd* pParent /*=NULL*/)
    : CDialog(CSelectAgent::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSelectAgent)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CSelectAgent::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSelectAgent)
    DDX_Control(pDX, IDC_AGENT_LIST, m_AgentList);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectAgent, CDialog)
    //{{AFX_MSG_MAP(CSelectAgent)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectAgent message handlers

BOOL CSelectAgent::OnInitDialog() 
{
    CDialog::OnInitDialog();

    m_AgentList.InsertColumn( SELAG_NAME_COLUMN, _T("Name"), LVCFMT_LEFT, 200 ); 
    m_AgentList.InsertColumn( SELAG_AGENT_COLUMN, _T("Agent"), LVCFMT_LEFT, 200 ); 

    // populate the list with our history
    int i, index;
    ConnectHistoryEntry *entry;

    for( i=0 ;i<m_history->GetCount() ; i++ ) {
        entry = m_history->GetAt( i );

        index = m_AgentList.InsertItem( i, entry->name );
        m_AgentList.SetItemText( index, SELAG_AGENT_COLUMN, entry->agent_address );

    }
    
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
