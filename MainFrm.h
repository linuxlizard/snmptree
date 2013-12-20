// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__14405926_301E_4FA3_95CE_4BAAF7A8B193__INCLUDED_)
#define AFX_MAINFRM_H__14405926_301E_4FA3_95CE_4BAAF7A8B193__INCLUDED_

#include "SListView.h"    // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"
#include "STreeView.h"    // Added by ClassView

#define RETRANSMIT_TIMER_ID  1357

class CMainFrame : public CFrameWnd
{
    
public:
    CMainFrame();
protected: 
    DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
    public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    protected:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;
//    CChildView    m_wndView;

    void fill_tree_view(void);
    void add_subtree( struct tree *node, HTREEITEM parenth );
    void StartWalk( oid *walk_oid, size_t walk_oidlen );
    void StartSet( void );

    /* convert to/from display version radio button to SNMP_VERSION_xx */
    long gui_to_snmp_version( long gui_version );
    long snmp_to_gui_version( long snmp_version );

    LRESULT ReapThread( WPARAM wparam, LPARAM lparam, LPCSTR parent_action );

    typedef enum {
        state_notconnected,
        state_connected,
        state_snmp_walking,
        state_snmp_setting,
        state_snmp_stopping

    } snmp_working_state;

    void SetWorkingState( snmp_working_state new_state );

    snmp_working_state m_state;

    CString m_ConnHistPassword;
    CString m_AgentAddress;
    CString m_GetCommunity;
    CString m_SetCommunity;
    long m_SNMPVersion; /* SNMP_VERSION_xxx from net-snmp/library/snmp.h */

    // Net-SNMP session
    void *m_get_session;
    // save original OID when starting a walk so can stop when leave the tree
    oid m_walkoid[MAX_OID_LEN];
    size_t m_walkoid_len;
    bool m_Stop;

    HANDLE m_thd;
    DWORD m_thdid;

// Generated message map functions
protected:
    STreeView *m_tree;
    SListView *m_list;
    CSplitterWnd m_wndSplitter;
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnEditCopy();
    afx_msg void OnEditClear();
    afx_msg void OnSnmpGet();
    afx_msg void OnSnmpSet();
    afx_msg void OnFileConnect();
    afx_msg LRESULT OnSnmpWalkResult( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSetAgent( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSnmpWalkStart( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSnmpWalkDone( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSnmpSetDone( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSwapView( WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnSnmpSetStart( WPARAM wparam, LPARAM lparam);
    afx_msg void OnSnmpStop();
    afx_msg void OnEditSelectAll();
    afx_msg void OnClose();
    afx_msg void OnFileLogsession();
    afx_msg void OnEditPreferences();
	afx_msg void OnFilePassphrase();
	afx_msg void OnFileForgetPassphrase();
	afx_msg void OnTestConnHist();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__14405926_301E_4FA3_95CE_4BAAF7A8B193__INCLUDED_)
