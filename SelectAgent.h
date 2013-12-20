#if !defined(AFX_SELECTAGENT_H__24BD6C21_713E_4B05_AA58_E44652D7EE6C__INCLUDED_)
#define AFX_SELECTAGENT_H__24BD6C21_713E_4B05_AA58_E44652D7EE6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectAgent.h : header file
//

// List view columns
#define SELAG_NAME_COLUMN  0
#define SELAG_AGENT_COLUMN 1

/////////////////////////////////////////////////////////////////////////////
// CSelectAgent dialog

class CSelectAgent : public CDialog
{
// Construction
public:
    CSelectAgent(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CSelectAgent)
    enum { IDD = IDD_SELECT_AGENT_DIALOG };
    CListCtrl    m_AgentList;
    //}}AFX_DATA


    /* the history list of previous connections */
    ConnectHistory *m_history;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSelectAgent)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CSelectAgent)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTAGENT_H__24BD6C21_713E_4B05_AA58_E44652D7EE6C__INCLUDED_)
