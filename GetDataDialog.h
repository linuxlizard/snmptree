#if !defined(AFX_GETDATADIALOG_H__4B28BDCB_528C_4D7B_9C73_270E9B336B48__INCLUDED_)
#define AFX_GETDATADIALOG_H__4B28BDCB_528C_4D7B_9C73_270E9B336B48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetDataDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetDataDialog dialog

class CGetDataDialog : public CDialog
{
// Construction
public:
	CGetDataDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetDataDialog)
	enum { IDD = IDD_GET_DATA_DIALOG };
	CButton	m_CancelButton;
	//}}AFX_DATA

    CString m_AgentAddress;
    CString m_GetCommunity;

    void SetStartingOid( oid *startoid, size_t len );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetDataDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    // NetSNMP session
    void *m_get_session;
    // save original OID when starting a walk so can stop when leave the tree
    oid walkoid[MAX_OID_LEN];
    size_t walkoid_len;

    // set to TRUE when user hits cancel button
    BOOL stop;

	// Generated message map functions
	//{{AFX_MSG(CGetDataDialog)
	virtual void OnCancel();
    afx_msg LRESULT OnAsyncSelect( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSNMPWalkResult( WPARAM wparam, LPARAM lparam);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETDATADIALOG_H__4B28BDCB_528C_4D7B_9C73_270E9B336B48__INCLUDED_)
