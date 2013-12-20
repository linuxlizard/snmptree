#if !defined(AFX_EDITAGENT_H__F1E5B0AD_9339_4657_939E_936C8C9F4C7A__INCLUDED_)
#define AFX_EDITAGENT_H__F1E5B0AD_9339_4657_939E_936C8C9F4C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditAgent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditAgent dialog

class CEditAgent : public CDialog
{
// Construction
public:
	CEditAgent(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditAgent)
	enum { IDD = IDD_EDIT_AGENT_DIALOG };
	CEdit	m_SetCommunityEdit;
	CEdit	m_GetCommunityEdit;
	CButton	m_ShowCommunityCheck;
	CEdit	m_AgentEdit;
	CString	m_GetCommunity;
	CString	m_SetCommunity;
	int		m_SNMPVersion;
	CString	m_AgentAddress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditAgent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditAgent)
	afx_msg void OnShowCommunityCheck();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITAGENT_H__F1E5B0AD_9339_4657_939E_936C8C9F4C7A__INCLUDED_)
