// PrefDialog.cpp : implementation file
//

#include "stdafx.h"
#include "snmptree.h"
#include "PrefDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefDialog dialog


CPrefDialog::CPrefDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPrefDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrefDialog)
	m_Debug = FALSE;
	m_MIBDIR = _T("");
	m_MIBS = _T("");
	//}}AFX_DATA_INIT
}


void CPrefDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefDialog)
	DDX_Check(pDX, IDC_PREF_DEBUG_CHECK, m_Debug);
	DDX_Text(pDX, IDC_MIBDIR_EDIT, m_MIBDIR);
	DDX_Text(pDX, IDC_MIBS_EDIT, m_MIBS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefDialog, CDialog)
	//{{AFX_MSG_MAP(CPrefDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefDialog message handlers
