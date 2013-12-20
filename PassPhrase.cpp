// PassPhrase.cpp : implementation file
//

#include "stdafx.h"
#include "snmptree.h"
#include "PassPhrase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PassPhrase dialog


PassPhrase::PassPhrase(CWnd* pParent /*=NULL*/)
	: CDialog(PassPhrase::IDD, pParent)
{
	//{{AFX_DATA_INIT(PassPhrase)
	m_PassPhrase = _T("");
	//}}AFX_DATA_INIT
}


void PassPhrase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PassPhrase)
	DDX_Control(pDX, IDC_PASSPHRASE_EDIT, m_PassPhraseEdit);
	DDX_Text(pDX, IDC_PASSPHRASE_EDIT, m_PassPhrase);
	DDV_MaxChars(pDX, m_PassPhrase, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PassPhrase, CDialog)
	//{{AFX_MSG_MAP(PassPhrase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PassPhrase message handlers

BOOL PassPhrase::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    m_PassPhraseEdit.SetFocus();
    
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
