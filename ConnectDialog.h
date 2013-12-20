#if !defined(AFX_CONNECTDIALOG_H__99DAA0F3_A9A5_4BA9_A340_F4FCE6BA1084__INCLUDED_)
#define AFX_CONNECTDIALOG_H__99DAA0F3_A9A5_4BA9_A340_F4FCE6BA1084__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectDialog dialog

class CConnectDialog : public CDialog
{
// Construction
public:
    CConnectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CConnectDialog)
    enum { IDD = IDD_CONNECT_DIALOG };
    CEdit    m_AgentEdit;
    CButton    m_ShowCommunityCheck;
    CEdit    m_SetCommunityEdit;
    CEdit    m_GetCommunityEdit;
    CString    m_GetCommunity;
    CString    m_SetCommunity;
    int        m_SNMPVersion;
    CString    m_AgentAddress;
    //}}AFX_DATA

    /* the history list of previous connections */
    ConnectHistory *m_history;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CConnectDialog)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CConnectDialog)
    virtual BOOL OnInitDialog();
    afx_msg void OnShowCommunityCheck();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTDIALOG_H__99DAA0F3_A9A5_4BA9_A340_F4FCE6BA1084__INCLUDED_)
