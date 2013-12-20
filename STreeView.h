#if !defined(AFX_STREEVIEW_H__CEFB7A7D_918A_4141_8821_AC149399A4B1__INCLUDED_)
#define AFX_STREEVIEW_H__CEFB7A7D_918A_4141_8821_AC149399A4B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// STreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// STreeView view

#include "afxcview.h"

class STreeView : public CTreeView
{
protected:
    STreeView();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(STreeView)

// Attributes
public:

// Operations
public:

    int GetSelectedOID( oid *oid, size_t *oidlen, size_t max_oidlen );

    /* recursively expand the oid in the tree */
    void expand( HTREEITEM root, oid *oid_to_expand, int oidlen );

    /* expand a given oid path in the tree view */
    int expand_oid_path( const char *oid_str );
    

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(STreeView)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~STreeView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    //{{AFX_MSG(STreeView)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STREEVIEW_H__CEFB7A7D_918A_4141_8821_AC149399A4B1__INCLUDED_)
