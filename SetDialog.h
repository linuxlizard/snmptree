#if !defined(AFX_SETDIALOG_H__32D111D5_395A_44D9_9540_F1B689FA5042__INCLUDED_)
#define AFX_SETDIALOG_H__32D111D5_395A_44D9_9540_F1B689FA5042__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetDialog dialog

class CSetDialog : public CDialog
{
// Construction
public:
	CSetDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetDialog)
	enum { IDD = IDD_SET_DIALOG };
	CEdit	m_ValueEdit;
	CButton	m_PrevButton;
	CButton	m_NextButton;
	CString	m_Name;
	CString	m_Type;
	CString	m_Value;
	//}}AFX_DATA

    CArray< ASN::Instance *, ASN::Instance *> set_variables;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    // index into set_variables array
    int m_CurrentInstanceIdx;
    CString m_Title;

    void UpdateVariableDisplay( void );
    BOOL ValidateData( void );

	// Generated message map functions
	//{{AFX_MSG(CSetDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnNextButton();
	afx_msg void OnPrevButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDIALOG_H__32D111D5_395A_44D9_9540_F1B689FA5042__INCLUDED_)
