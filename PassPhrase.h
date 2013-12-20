#if !defined(AFX_PASSPHRASE_H__0ABCA40A_E8C8_4712_AAB6_331EE0E5CB5E__INCLUDED_)
#define AFX_PASSPHRASE_H__0ABCA40A_E8C8_4712_AAB6_331EE0E5CB5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassPhrase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PassPhrase dialog

class PassPhrase : public CDialog
{
// Construction
public:
	PassPhrase(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PassPhrase)
	enum { IDD = IDD_PASSPHRASE_DIALOG };
	CEdit	m_PassPhraseEdit;
	CString	m_PassPhrase;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PassPhrase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PassPhrase)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSPHRASE_H__0ABCA40A_E8C8_4712_AAB6_331EE0E5CB5E__INCLUDED_)
