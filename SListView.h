#if !defined(AFX_SLISTVIEW_H__1F568E48_FAAE_45B8_83F2_72EFB6E7A0D1__INCLUDED_)
#define AFX_SLISTVIEW_H__1F568E48_FAAE_45B8_83F2_72EFB6E7A0D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SListView.h : header file
//

/* CListView columns */
#define NAME_COLUMN     0
#define VALUE_COLUMN    1
#define OID_COLUMN      2
#define TYPE_COLUMN     3


/////////////////////////////////////////////////////////////////////////////
// SListView view

#include "afxcview.h"

class SListView : public CListView
{
protected:
	SListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(SListView)

// Attributes
public:

// Operations
public:

    int GetSelectedVar( CString &var_name, u_char &asn_type, CString &var_value );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SListView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~SListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(SListView)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLISTVIEW_H__1F568E48_FAAE_45B8_83F2_72EFB6E7A0D1__INCLUDED_)
