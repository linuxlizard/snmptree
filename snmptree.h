// snmptree.h : main header file for the SNMPTREE application
//

#if !defined(AFX_SNMPTREE_H__13A8A295_9FB5_4D75_9987_78BB8D295B31__INCLUDED_)
#define AFX_SNMPTREE_H__13A8A295_9FB5_4D75_9987_78BB8D295B31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSnmptreeApp:
// See snmptree.cpp for the implementation of this class
//

class CSnmptreeApp : public CWinApp
{
public:
	CSnmptreeApp();

    int GetRecentFileCount( void );
    CString GetRecentFile( int index );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSnmptreeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CSnmptreeApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNMPTREE_H__13A8A295_9FB5_4D75_9987_78BB8D295B31__INCLUDED_)
