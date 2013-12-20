#if !defined(AFX_PREFDIALOG_H__B4E19B2D_2674_48AF_8687_F132FEECDBC5__INCLUDED_)
#define AFX_PREFDIALOG_H__B4E19B2D_2674_48AF_8687_F132FEECDBC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrefDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefDialog dialog

class CPrefDialog : public CDialog
{
// Construction
public:
	CPrefDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrefDialog)
	enum { IDD = IDD_PREFERENCES_DIALOG };
	BOOL	m_Debug;
	CString	m_MIBDIR;
	CString	m_MIBS;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrefDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFDIALOG_H__B4E19B2D_2674_48AF_8687_F132FEECDBC5__INCLUDED_)
