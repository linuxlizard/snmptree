// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "snmptree.h"

#include <afxtempl.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/snmpv3_api.h>

#include "MainFrm.h"

#include "SListView.h"
#include "STreeView.h"
#include "connhist.h"
#include "ConnectDialog.h"
#include "SelectAgent.h"
#include "asntypes.h"
#include "SetDialog.h"
#include "usermsg.h"
#include "snmpcore.h"
#include "errstr.h"
#include "workers.h"
#include "PrefDialog.h"
#include "globals.h"
#include "PassPhrase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* compare_tree_nodes() used to sort OIDs in subtrees so I can add them into
 * the tree level in numerical order.
 */

int __cdecl compare_tree_nodes( const void *elem1, const void *elem2 )
{
    struct tree *node1, *node2;

    node1 = *(struct tree **)elem1;
    node2 = *(struct tree **)elem2;

    return node1->subid - node2->subid;
}

/* 6-Feb-05 ; copied FindMenuItem() from 
 * http://msdn.microsoft.com/library/default.asp?
 *      url=/library/en-us/dv_wcemfc4/html/aflrfCMenucolcolGetMenuString.asp
 */

// FindMenuItem() will find a menu item string from the specified
// popup menu and returns its position (0-based) in the specified 
// popup menu. It returns -1 if no such menu item string is found.
int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
   ASSERT(Menu);
   ASSERT(::IsMenu(Menu->GetSafeHmenu()));

   int count = Menu->GetMenuItemCount();
   for (int i = 0; i < count; i++)
   {
      CString str;
      if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
         (strcmp(str, MenuString) == 0))
         return i;
   }

   return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_COMMAND(ID_SNMP_GET, OnSnmpGet)
    ON_COMMAND(ID_SNMP_SET, OnSnmpSet)
    ON_COMMAND(ID_FILE_CONNECT, OnFileConnect)
    ON_MESSAGE(MSG_SNMP_RESULT, OnSnmpWalkResult)
    ON_MESSAGE(MSG_SET_AGENT, OnSetAgent)
    ON_MESSAGE(MSG_SNMP_WALK_START, OnSnmpWalkStart)
    ON_MESSAGE(MSG_SNMP_WALK_DONE, OnSnmpWalkDone)
    ON_MESSAGE(MSG_SNMP_SET_DONE, OnSnmpSetDone)
    ON_MESSAGE(MSG_SWAP_VIEW, OnSwapView)
    ON_MESSAGE(MSG_SNMP_SET_START, OnSnmpSetStart)
    ON_COMMAND(ID_SNMP_STOP, OnSnmpStop)
    ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
    ON_WM_CLOSE()
    ON_COMMAND(ID_FILE_LOGSESSION, OnFileLogsession)
    ON_COMMAND(ID_EDIT_PREFERENCES, OnEditPreferences)
	ON_COMMAND(ID_FILE_PASSPHRASE, OnFilePassphrase)
	ON_COMMAND(ID_FILE_FORGET_PASSPHRASE, OnFileForgetPassphrase)
	ON_COMMAND(ID_TEST_CONNHIST, OnTestConnHist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// status pane fields
#define PANE_CONNECT_FIELD  1

static UINT indicators[] =
{
    ID_SEPARATOR,
    IDS_NOT_CONNECT
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
    m_get_session = NULL;
    m_walkoid_len = 0;
    m_Stop = false;

    m_thd = NULL;
    m_thdid = 0;

//    m_AgentAddress = "192.168.42.1";
//    m_AgentAddress = "141.184.117.203";
    m_AgentAddress = "";
    m_GetCommunity = "public";
    m_SetCommunity = "private";
    m_SNMPVersion = SNMP_VERSION_2c;
}

CMainFrame::~CMainFrame()
{
    // release m_get_session
    if( m_get_session != NULL ) {
        snmp_sess_close( m_get_session );
        m_get_session = NULL;
    }


}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // create a view to occupy the client area of the frame
//    if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
//        CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
//    {
//        TRACE0("Failed to create view window\n");
//        return -1;
//    }
    
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    // widen out the connect-to status line
    UINT id, style;
    int width;
    m_wndStatusBar.GetPaneInfo( PANE_CONNECT_FIELD, id, style, width );
    m_wndStatusBar.SetPaneInfo( PANE_CONNECT_FIELD, id, style, width*3 );

    // give ourselves a nice icon instead of that crappy Windows default >:-P
    SetIcon( LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE );

    // TODO: Delete these three lines if you don't want the toolbar to
    //  be dockable
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass = AfxRegisterWndClass(0);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
//    // forward focus to the view window
//    m_wndSplitter.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // let the view have first crack at the command
    if (m_wndSplitter.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
    m_wndSplitter.CreateStatic( this, 1, 2 );
  
    m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS( STreeView ), CSize(500, 250), pContext );
    m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS( SListView ), CSize(0, 0), pContext );

    m_tree = dynamic_cast<STreeView*>(m_wndSplitter.GetPane(0,0));
    m_list = dynamic_cast<SListView*>(m_wndSplitter.GetPane(0,1));

    fill_tree_view();

    /* davep 22-Jan-2006 ; expand down to mib-2.system which is where most
     * stuff will live anyway
     * 1.3.6.1.2.1.1
     */
    m_tree->expand_oid_path( ".1.3.6.1.2.1.1" );

    m_list->GetListCtrl().InsertColumn( NAME_COLUMN, "Name", LVCFMT_LEFT, 200 );
    m_list->GetListCtrl().InsertColumn( VALUE_COLUMN, "Value", LVCFMT_LEFT, 300 );
    m_list->GetListCtrl().InsertColumn( OID_COLUMN, "OID", LVCFMT_LEFT, 200 );
    m_list->GetListCtrl().InsertColumn( TYPE_COLUMN, "Type", LVCFMT_LEFT, 150 );

    // activate the input view
    SetActiveView( dynamic_cast<CView *>(m_tree) );

    // set an extra bit of style
    ListView_SetExtendedListViewStyle( m_list->m_hWnd, LVS_EX_FULLROWSELECT );

//    return CFrameWnd::OnCreateClient(lpcs, pContext);
    return TRUE;
}

void CMainFrame::OnEditCopy() 
{
    POSITION pos;
    CListCtrl& list = m_list->GetListCtrl();

    pos = list.GetFirstSelectedItemPosition();
    if( pos == NULL ) {
        /* Nothing selected */
        return;
    }

    OpenClipboard();
    EmptyClipboard();

    CString to_clipboard;
    CString one_line;

    while( pos ) {
        int nItem = list.GetNextSelectedItem( pos );

        one_line.Format( "%s\t%s\t%s\r\n", 
            list.GetItemText( nItem, NAME_COLUMN ), 
            list.GetItemText( nItem, VALUE_COLUMN ), 
            list.GetItemText( nItem, OID_COLUMN ), 
            list.GetItemText( nItem, TYPE_COLUMN ) );

        to_clipboard += one_line;
    }

    // No clue if there's a better way to do this but, hey, this is how
    // Petzold says to do it.  p572 _Programming Windows, 5th ed_, Charles Petzold
    HGLOBAL hGlobal = GlobalAlloc( GHND|GMEM_SHARE, to_clipboard.GetLength()+1 );
    ASSERT( hGlobal != NULL );
    if( hGlobal == NULL ) {
        MessageBox( "Out of memory", NULL, MB_ICONERROR );
        return;
    }
    LPVOID pGlobal = GlobalLock( hGlobal );
    ASSERT( pGlobal != NULL );
    if( pGlobal == NULL ) {
        // GlobalLock() probably won't fail but I needed an excuse to fiddle
        // with error strings
        CString errstr;
        errstr.Format( "GlobalLock() failed : %s", DecodeError(GetLastError()));
        MessageBox( errstr, NULL, MB_ICONERROR );
        GlobalFree( hGlobal );
    }
    strncpy( (char *)pGlobal, (LPCSTR)to_clipboard, to_clipboard.GetLength() );
    GlobalUnlock( pGlobal );

    SetClipboardData( CF_TEXT, hGlobal );
    CloseClipboard();
    // end stuff from Petzold
}

void CMainFrame::OnEditClear() 
{
    BOOL bretcode;
    ASN::Instance *instance;
    int i, len;

    CListCtrl& list = m_list->GetListCtrl();

    // free all the instance variables we're holding in the item data
    len = list.GetItemCount();
    for( i=0 ; i<len ; i++ ) {
        instance = (ASN::Instance *)list.GetItemData( i );
        list.SetItemData( i, 0 );
        delete instance;
    }

    bretcode = list.DeleteAllItems();
    ASSERT( bretcode == TRUE );
    // shouldn't fail
}

void CMainFrame::OnSnmpGet() 
{
    oid tmpoid[MAX_OID_LEN];
    size_t tmpoid_len;
    int retcode;

    memset( tmpoid, 0, sizeof(tmpoid) );
    tmpoid_len = 0;

    retcode = m_tree->GetSelectedOID( tmpoid, &tmpoid_len, MAX_OID_LEN );
    if( retcode <= 0 ) {
        /* nothing selected, don't do anything */
        TRACE0( _T("Nothing selected") );
        return;
    }

    StartWalk( tmpoid, tmpoid_len );
}

void CMainFrame::OnSnmpSet() 
{
    StartSet();
}

long CMainFrame::gui_to_snmp_version( long gui_version )
{
    /* 22-Jan-06 ; current implementation is a set of radio buttons with only
     * v1 and v2c. MFC does radio buttons as an integer list starting from
     * zero.
     */
    switch( gui_version ) {
        case 0 :
            return SNMP_VERSION_1;

        case 1 :
            return SNMP_VERSION_2c;

        default :
            ASSERT(0);
            return SNMP_VERSION_1;
    }
}

long CMainFrame::snmp_to_gui_version( long snmp_version )
{
    switch( snmp_version ) {
        case SNMP_VERSION_1 :
            return 0;

        case SNMP_VERSION_2c :
            return 1;

        default:
            ASSERT(0);
            return 0;
    }
}

void CMainFrame::OnFileConnect() 
{
    int retcode;
    CString recent_agent;
    ConnectHistory conn_hist;
    ConnectHistoryEntry *conn_entry;
    BOOL history_loaded;
    BOOL history_exists;

    CString reg_section = _T("history");
    CString reg_entry = _T("connection");

    conn_entry = NULL;
    history_loaded = FALSE;

    /* If the history exists and we were able to load it, save it when we're
     * done.
     * If the history exists and we weren't able to load it (decryption
     * error?), don't try to save it.
     * 
     */
    history_exists = FALSE;

    /* if the user hasn't set an encrypt/decrypt password, ask for it now */ 
    if( m_ConnHistPassword.GetLength() == 0 ) {
        OnFilePassphrase();
    }

    /* The user still has the option to cancel the passphrase dialog and just
     * do one-time connections.
     */
    if( m_ConnHistPassword.GetLength() == 0 ) {
        conn_hist.set_password( m_ConnHistPassword );
        retcode = conn_hist.Load( reg_section, reg_entry );

        switch( retcode ) {
            case 0 :
                history_loaded = TRUE;
                history_exists = TRUE;
                break;

            case CONNHIST_ERR_REGISTRY_FAIL :
                /* the registry key didn't exist so feel free to create the key
                 * once we're done 
                 */
                history_loaded = FALSE;
                history_exists = FALSE;
                break;

            default :
                /* We weren't able to decrypt or parse or some other failure in
                 * loading the history. We'll load a set of defaults into the
                 * dialog but not save them when we're done.
                 */
                history_loaded = FALSE;
                history_exists = TRUE;
                break;
        }
    }

#if 0
    CConnectDialog connDlg;

    /* get an entry to load onto the first part of the dialog */
    if( history_loaded ) {
        conn_entry = conn_hist.GetAt( 0 );

    }

    /* if we didn't get history loaded, simply plug in some defaults */
    if( ! history_loaded || conn_entry==NULL ) {
    
        MessageBox( _T( "No Passphrase" ) );
        
        if( m_GetCommunity.GetLength() == 0 ) {
            connDlg.m_GetCommunity = _T("public");
        }
        else {
            connDlg.m_GetCommunity = m_GetCommunity;
        }
        if( m_SetCommunity.GetLength() == 0 ) {
            connDlg.m_SetCommunity = _T("public");
        }
        else {
            connDlg.m_SetCommunity = m_SetCommunity;
        }

        connDlg.m_AgentAddress = m_AgentAddress;
        connDlg.m_SNMPVersion = snmp_to_gui_version( m_SNMPVersion );
    }
    else { 
        connDlg.m_GetCommunity = conn_entry->get_community;
        connDlg.m_SetCommunity = conn_entry->set_community;
        connDlg.m_AgentAddress = conn_entry->agent;
        connDlg.m_SNMPVersion = snmp_to_gui_version( conn_entry->snmp_version);
        connDlg.m_history = &conn_hist;
    }

    retcode = connDlg.DoModal();

    if( retcode == -1 || retcode == IDABORT ) {
        TRACE0( _T("Failed to create connect dialog box") );
        return;
    }
    if( retcode != IDOK ) {
        return;
    }

    m_AgentAddress = connDlg.m_AgentAddress;
    m_GetCommunity = connDlg.m_GetCommunity;
    m_SetCommunity = connDlg.m_SetCommunity;
    m_SNMPVersion = gui_to_snmp_version( connDlg.m_SNMPVersion );
#endif

    CSelectAgent selectDlg;

    selectDlg.m_history = &conn_hist;

    retcode = selectDlg.DoModal();
    if( retcode == -1 || retcode == IDABORT ) {
        TRACE0( _T("Failed to create connect dialog box") );
        return;
    }
    if( retcode != IDOK ) {
        return;
    }

    SetWorkingState( state_connected );

    // clear all the previous results to avoid any possible confusion */
    OnEditClear();
    // shouldn't fail

    /* If we were able to successfully load a history, save it back. 
     * OR
     * If a history didn't exist before, save a new one.
     */
//    if( (history_loaded && history_exists) || !history_exists ) {
//        retcode = conn_hist.Save( reg_section, reg_entry );
//    }

#if 0
    AfxGetApp()->AddToRecentFileList( m_AgentAddress );

    HKEY agentGroupKey = AfxGetApp()->GetSectionKey( "Agents" );
    ASSERT( agentGroupKey != NULL );

    DWORD dw;
    HKEY agentKey;
    LONG lretcode;
    BYTE *data;
    int datalen;

    lretcode = RegCreateKeyEx( agentGroupKey, m_AgentAddress, 0, REG_NONE,
        REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
        &agentKey, &dw);
    ASSERT( lretcode == ERROR_SUCCESS );

    // save the Get community name
    datalen = m_GetCommunity.GetLength();
    data = (BYTE *)m_GetCommunity.GetBuffer( datalen );
    lretcode = RegSetValueEx( agentKey, "Get", 0, REG_SZ, data, datalen );
    ASSERT( lretcode == ERROR_SUCCESS );
    m_GetCommunity.ReleaseBuffer();

    // save the Set community name
    datalen = m_SetCommunity.GetLength();
    data = (BYTE *)m_SetCommunity.GetBuffer( datalen );
    lretcode = RegSetValueEx( agentKey, "Set", 0, REG_SZ, data, datalen );
    ASSERT( lretcode == ERROR_SUCCESS );
    m_SetCommunity.ReleaseBuffer();

    // TODO - save extra agent settings (timeout, etc)

    RegCloseKey( agentGroupKey );
    RegCloseKey( agentKey );
#endif
}

LRESULT CMainFrame::OnSnmpWalkResult( WPARAM wparam, LPARAM lparam)
{
//    struct snmpwalk_result *result;
    int index;
    BOOL bretcode;
    ASN::Instance *instance;

//    result = (struct snmpwalk_result *)wparam;
    instance = (ASN::Instance *)wparam;

    CListCtrl& list = m_list->GetListCtrl();

    /* check to see if our walk should end */
    if( !m_Stop && instance->oid_is_subtree(m_walkoid, m_walkoid_len)==0  ) {

        index = list.InsertItem( m_list->GetListCtrl().GetItemCount(), 
                instance->get_object_name_string() );
        ASSERT( index >= 0 );
        // shouldn't fail
    
        bretcode = list.SetItemText( index, VALUE_COLUMN, 
                instance->get_pretty_display_string() );
        ASSERT( bretcode == TRUE );
        // shouldn't fail

        bretcode = list.SetItemText( index, OID_COLUMN,
                instance->get_objectid_string() );
        ASSERT( bretcode == TRUE );

        bretcode = list.SetItemText( index, TYPE_COLUMN, 
                instance->get_type_name() );
        ASSERT( bretcode == TRUE );
        // shouldn't fail

        // Need to save the type in an internal format in case user wants
        // to do a set on the variable later. Easier than parsing the display
        // string.
        bretcode = list.SetItemData( index, (DWORD)instance );
        ASSERT( bretcode == TRUE );
        // shouldn't fail
    }
    else {
        /* walk is stopped; don't record any more data */
        DEBUGMSGTL(("MainFrm", "walk of "));
        DEBUGMSGOID(("MainFrm", m_walkoid, m_walkoid_len ));
        DEBUGMSG(("MainFrm"," is stopped\n"));
    }

    return (LRESULT)0;
}

LRESULT CMainFrame::OnSetAgent( WPARAM wparam, LPARAM lparam)
{
    CString *agent;

    agent = (CString *)wparam;

    m_AgentAddress = *agent;

    SetWorkingState( state_connected );

    TRACE1( _T("Set agent to %s\n"), *agent );

    delete agent;

    return (LRESULT)0;
}

void CMainFrame::add_subtree( struct tree *node, HTREEITEM parenth )
{
    CString str;
    HTREEITEM h1;
    struct tree **node_sort;
    struct tree *tmp_node;
    int i, node_count, node_max;
    BOOL bretcode;

    ASSERT( node != NULL );
    if( node==NULL ) {
        return;
    }

    /* if we only have one peer at this level, make our lives easier */
    if( node->next_peer == NULL ) {
        str.Format( "%s (%ld)", node->label, node->subid );
        h1 = m_tree->GetTreeCtrl().InsertItem( str, parenth );
        ASSERT( h1 != NULL );
        bretcode = m_tree->GetTreeCtrl().SetItemData( h1, node->subid );
        ASSERT( bretcode == TRUE );

        /* load its children */
        if( node->child_list ) {
            add_subtree( node->child_list, h1 );
        }
    }
    else {
    
        /* peers are stored unsorted so sort numerically by subid */
        node_max = 10;
        node_sort = (struct tree **)calloc( node_max, sizeof(struct tree *) );
        if( node_sort == NULL ) {
            MessageBox( "Out of memory", "Error", MB_ICONERROR );
            return;
        }
        tmp_node = node;
        node_count = 0;
        while( tmp_node != NULL ) {
            if( node_count >= node_max ) {
                node_max *= 2;
                node_sort = (struct tree **)realloc( node_sort, node_max*sizeof(struct tree *) );
                if( node_sort == NULL ) {
                    MessageBox( "Out of memory", "Error", MB_ICONERROR );
                    return;
                }
            }
            node_sort[node_count++] = tmp_node;
            tmp_node = tmp_node->next_peer;
        }

        if( node_count > 1 ) {
            qsort( node_sort, node_count, sizeof(struct tree *), compare_tree_nodes );
        }
    
        for( i=0 ; i<node_count ; i++ ) {
            tmp_node = node_sort[i];
    
            str.Format( "%s (%ld)", tmp_node->label, tmp_node->subid );
            h1 = m_tree->GetTreeCtrl().InsertItem( str, parenth );
            ASSERT( h1 != NULL );
            bretcode = m_tree->GetTreeCtrl().SetItemData( h1, tmp_node->subid );
            ASSERT( bretcode == TRUE );

            /* load its children */
            if( tmp_node->child_list ) {
                add_subtree( tmp_node->child_list, h1 );
            }
        }
    
        free( node_sort );
    }
}

void CMainFrame::fill_tree_view( void )
{
    CString str;
    HTREEITEM h1;
    struct tree *tree_head;
    BOOL bretcode;

    tree_head = get_tree_head();

    /* Net-SNMP includes a few non-ISO MIBs so load the root node then 
     * immediately go below ISO to load the rest. 
     */
    if( tree_head == NULL ) {
        // FIXME - need better error message here
        MessageBox( "No MIBs loaded", "Error", MB_ICONERROR );
        return;
    }

    str.Format( "%s (%ld)", tree_head->label, tree_head->subid );
    h1 = m_tree->GetTreeCtrl().InsertItem( str );
    ASSERT( h1 != NULL );
    bretcode = m_tree->GetTreeCtrl().SetItemData( h1, tree_head->subid );
    ASSERT( bretcode == TRUE );

    if( tree_head->child_list ) {
        add_subtree( tree_head->child_list, h1 );
    }
}

LRESULT CMainFrame::OnSnmpWalkStart( WPARAM wparam, LPARAM lparam)
{
    oid *walk_oid;
    size_t walk_oidlen;

    walk_oid = (oid *)wparam;
    walk_oidlen = (size_t)lparam;

    ASSERT( walk_oid != NULL );
    ASSERT( walk_oidlen > 0 && walk_oidlen <= MAX_OID_LEN );

    StartWalk( walk_oid, walk_oidlen );

    free( walk_oid );
    return (LRESULT)0;
}

LRESULT CMainFrame::ReapThread( WPARAM wparam, LPARAM lparam, LPCSTR parent_action )
{
    DWORD exitcode;

    ASSERT( m_thd != NULL );

    do {
        if( !GetExitCodeThread( m_thd, &exitcode ) ) {
            /* GetExitCodeThread() failed; shouldn't happen */
            MessageBox( DecodeError( WSAGetLastError() ), _T("Thread Exit Error"), MB_ICONERROR );

            /* note: orphaning current thread */
            break;
        }

        if( exitcode == STILL_ACTIVE ) {
            DEBUGMSGTL( ("MainFrm", "Waiting for thread to exit...\n") );
            Sleep( 1 );
        }

    } while( exitcode == STILL_ACTIVE );

    DEBUGMSGTL( ("MainFrm","thread exit code %d\n", exitcode) );

    CString *errstr;
    errstr = (CString *)wparam;

    // FIXME - want to do an ASSERT that wparam is indeed a CString *

    if( exitcode != 0 ) {
        if( errstr->GetLength() > 0 ) {
            /* snmpwalk/snmpset sent us an error message */
            MessageBox( *errstr, parent_action, MB_ICONERROR );
        }
    }

    delete errstr;

    m_thd = NULL;
    m_thdid = 0;

    /* go back to an idle state */
    SetWorkingState( state_connected );

    return (LRESULT)0;
}

LRESULT CMainFrame::OnSnmpWalkDone( WPARAM wparam, LPARAM lparam)
{
    return ReapThread( wparam, lparam, _T("Message from Walk") );
}

LRESULT CMainFrame::OnSnmpSetDone( WPARAM wparam, LPARAM lparam)
{
    return ReapThread( wparam, lparam, _T("Message from Set") );
}

LRESULT CMainFrame::OnSnmpSetStart( WPARAM wparam, LPARAM lparam)
{
    StartSet();
    return (LRESULT)0;
}

void CMainFrame::OnSnmpStop() 
{
    /* just set the flag to true; will check whenever a packet or a timer comes in */
    m_Stop = true;

    snmpcore_set_stop_flag( true );
    SetWorkingState( state_snmp_stopping );
}

void CMainFrame::StartWalk( oid *walk_oid, size_t walk_oidlen )
{
    DWORD err;
    CString errstr;

    if( m_thd != NULL ) {
        /* thread already running */
        return;
    }

    if( m_AgentAddress.GetLength()==0 || m_GetCommunity.GetLength()==0 ) {
        /* FIXME - report better error message */
        MessageBox( "Please set agent IP address and community name", "Missing Parameter", 
                MB_ICONWARNING );
        return;
    }

    memcpy( m_walkoid, walk_oid, sizeof(oid) * walk_oidlen );
    m_walkoid_len = walk_oidlen;

    /* Crank it up! */
    m_Stop = false;
    snmpcore_set_stop_flag( false );

    /* start up a worker thread */
    struct snmpwalk_param *param;
    param = new struct snmpwalk_param;
    if( param==NULL ) {
        MessageBox( "Out of Memory", "Error", MB_ICONERROR );
        return;
    }

    /* set status line so we know what we're doing if there isn't a response */
    SetWorkingState( state_snmp_walking );

    param->parent = m_hWnd;
    param->get_community = m_GetCommunity;
    memcpy( param->root, m_walkoid, sizeof(oid) * m_walkoid_len ); 
    param->rootlen = walk_oidlen;
    param->ip_address = m_AgentAddress;
    /* davep 22-Jan-06 ; add support for user specified snmp version */
//    param->snmp_version = SNMP_VERSION_2c;
    param->snmp_version = m_SNMPVersion;

    m_thd = ::CreateThread( 0, 0, snmpwalk_thread, param, 0, &m_thdid );
    ASSERT( m_thd != NULL );
    if( m_thd == NULL ) {
        err = GetLastError();
        errstr.FormatMessage( err );
        MessageBox( errstr );

        delete param;
    }

//    retcode = snmpcore_async_get( m_get_session, m_hWnd, m_walkoid, m_walkoid_len );
//    /* FIXME - report error to user */
//    ASSERT( retcode == SNMPCORE_SUCCESS );
//
//    /* start a timer to periodically poll the NetSNMP library for retransmits,
//     * response timeouts, etc.
//     */
//    int numfds, block;
//    FD_SET fds;
//    struct timeval timeout;
//
//    block = 0;
//    retcode = snmp_sess_select_info( m_get_session, &numfds, &fds, &timeout, &block );
//    ASSERT( retcode > 0 );
//
//    /* All I really want is the timeout.  'struct timeval' is in terms of seconds
//     * and microseconds (tv_sec, tv_usec). SetTimer() wants milliseconds.
//     */
//
//    /* make sure we don't already have our timer */
//    ASSERT( m_timer_id == 0 );
//
//    m_timer_id = SetTimer( RETRANSMIT_TIMER_ID, timeout.tv_sec * 1000 + timeout.tv_usec/1000, NULL);
//    if( m_timer_id == 0 ) {
//        AfxMessageBox( "Cannot get a system timer. Will be unable to retransmit packets!" );
//    }
}

void CMainFrame::StartSet( void )
{
    DWORD err;
    CString errstr;
    int retcode;

    if( m_thd != NULL ) {
        /* thread already running */
        return;
    }

//    MessageBox( _T("Set not implemented"), _T("Sorry."), MB_ICONINFORMATION );
//    return;

    if( m_AgentAddress.GetLength()==0 || m_SetCommunity.GetLength()==0 ) {
        /* FIXME - report better error message */
        MessageBox( "Please set agent IP address and Set community name", "Missing Parameter", 
                MB_ICONWARNING );
        return;
    }

    POSITION pos;
    CListCtrl& list = m_list->GetListCtrl();

    pos = list.GetFirstSelectedItemPosition();
    if( pos == NULL ) {
        /* Nothing selected */
        return;
    }

    // at this point we start the real work of collecting data and allocating
    // memory 

    struct snmpset_param *param;
//    CArray<ASN::Instance *, ASN::Instance *> instance_array;
    ASN::Instance *instance;
    ASN::Instance *new_inst;
    CSetDialog setDlg;

    // If we fail somewhere in this function, delete these.
    // Otherwise, the snmpset thread is responsible for freeing the memory.
    param = NULL;
    new_inst = NULL;

    // this pointer belongs in the CListView so don't delete it if something
    // goes wrong
    instance = NULL;

    while( pos ) {
        int nItem = list.GetNextSelectedItem( pos );

        instance = (ASN::Instance *)m_list->GetListCtrl().GetItemData( nItem );

//        instance_array.Add( instance );

        // make a copy of the instance to pass to the SetDialog
#if 0
        new_inst = new ASN::Instance( *instance );
#else
        new_inst = instance->Clone();
#endif
        ASSERT( new_inst != NULL );
        if( new_inst == NULL ) {
            MessageBox( "Out of Memory", "Error", MB_ICONERROR );
            goto leave;
        }

        setDlg.set_variables.Add( new_inst );
        new_inst = NULL;
        instance = NULL;
    }

    // bring up the set dialog for the user to twiddle the values

    retcode = setDlg.DoModal();

    if( retcode == -1 || retcode == IDABORT ) {
        TRACE0( _T("Failed to create Set dialog box") );
        goto leave;
    }

    if( retcode != IDOK ) {
        // user hit cancel; all that work for nothing!
        goto leave;
    }

    /* start up a worker thread */
    param = new struct snmpset_param;
    if( param==NULL ) {
        MessageBox( "Out of Memory", "Error", MB_ICONERROR );
        goto leave;
    }

    /* Crank it up! */
    m_Stop = false;
    snmpcore_set_stop_flag( false );

    /* set status line so we know what we're doing if there isn't a response */
    SetWorkingState( state_snmp_setting );

    param->parent = m_hWnd;
    param->set_community = m_SetCommunity;
    param->ip_address = m_AgentAddress;
    /* davep 22-Jan-2006 ; add support for user specified snmp version */
//    param->snmp_version = SNMP_VERSION_2c;
    param->snmp_version = m_SNMPVersion;

    // pass the values we want to set on to the thread (this is an array
    // of pointers so the copy is cheap)
    param->values.Copy( setDlg.set_variables );

    // The pointers to the new instances are now the responsibility of the
    // snmpset_thread() (the remove isn't necessary but it gets the point
    // across as to who is responsible for the memory).
    setDlg.set_variables.RemoveAll();

    m_thd = ::CreateThread( 0, 0, snmpset_thread, param, 0, &m_thdid );
    ASSERT( m_thd != NULL );
    if( m_thd == NULL ) {
        err = GetLastError();
        errstr.FormatMessage( err );
        MessageBox( errstr );

        goto leave;
    }

    // success!
    return;

leave:
    // failure or the user hit cancel
    // clean up after outselves
    if( param ) {
        delete param;
    }

    // delete an instance copies we might have made before everything went
    // kablooie
    int i;
    for( i=0 ; i<setDlg.set_variables.GetSize() ; i++ ) {
        new_inst = setDlg.set_variables[i];
        setDlg.set_variables.SetAt( i, NULL );
        delete new_inst;
        new_inst = NULL;
    }
}

void CMainFrame::OnEditSelectAll() 
{
    CListCtrl& list = m_list->GetListCtrl();
    
    int i;
    for( i=0 ; i<list.GetItemCount() ; i++ ) {
        list.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
    }
}

void CMainFrame::SetWorkingState( snmp_working_state new_state )
{
    CString str;
    snmp_working_state old_state;

    old_state = m_state;

    switch( new_state ) {
        case state_notconnected :
            str = _T("Idle");
            m_state = new_state;
            break;

        case state_connected :
            str = _T("Connected to ") + m_AgentAddress;
            m_state = new_state;
            break;

        case state_snmp_walking :
            str.Format( _T("Walking %s ..."), m_AgentAddress );
            m_state = new_state;
            break;

        case state_snmp_setting :
            str.Format( _T("Setting %s ..."), m_AgentAddress );
            m_state = new_state;
            break;

        case state_snmp_stopping :
            if( m_state == state_snmp_walking || m_state == state_snmp_setting ) {
                str = _T("Stopping");
                m_state = new_state;
            }
            break;
    }

    /* if our state changed, set display to new state */
    if( m_state != old_state ) {
        m_wndStatusBar.SetPaneText( PANE_CONNECT_FIELD, str, TRUE );    
    }
}

LRESULT CMainFrame::OnSwapView( WPARAM wparam, LPARAM lparam)
{
    CView *curr;

    curr = GetActiveView();

    CRuntimeClass *rtc = curr->GetRuntimeClass();

    if( curr->IsKindOf( RUNTIME_CLASS( STreeView ) ) ) {
        SetActiveView( dynamic_cast<CView *>(m_list) );
    }
    else {
        SetActiveView( dynamic_cast<CView *>(m_tree) );
    }

    return 0;
}

void CMainFrame::OnClose() 
{
    // empty out the members in the list
    int i, len;
    ASN::Instance *instance;
    CListCtrl& list = m_list->GetListCtrl();

    // free all the instance variables we're holding in the item data
    len = list.GetItemCount();
    for( i=0 ; i<len ; i++ ) {
        instance = (ASN::Instance *)list.GetItemData( i );
        list.SetItemData( i, 0 );
        delete instance;
    }    

    
    CFrameWnd::OnClose();
}

void CMainFrame::OnFileLogsession() 
{
    static netsnmp_log_handler *file_logh=NULL;

    CMenu *menu = GetMenu();
    ASSERT(menu);

    int pos = FindMenuItem( menu, _T("&File") );
    ASSERT( pos > -1 );

    CMenu *submenu = menu->GetSubMenu(pos);
    ASSERT(submenu);

    UINT state = submenu->GetMenuState( ID_FILE_LOGSESSION, MF_BYCOMMAND );
    ASSERT( state != 0xffffffff );

    if( state & MF_CHECKED ) {
        submenu->CheckMenuItem( ID_FILE_LOGSESSION, MF_UNCHECKED|MF_BYCOMMAND );
        snmpcore_stop_session_log();
    }
    else {

        CFileDialog dlg( TRUE, _T(".log"), _T("session.log"),
                OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
                _T("Log Files (*.log)|*.log||") );

        if( dlg.DoModal() != IDOK ) {
            return;
        }
        
        submenu->CheckMenuItem( ID_FILE_LOGSESSION, MF_CHECKED|MF_BYCOMMAND );
        snmpcore_session_log( dlg.GetPathName() );
    }
}


void CMainFrame::OnEditPreferences() 
{
    CPrefDialog dlg;
    Globals *globals;

    globals = get_globals();
    ASSERT( globals != NULL );

    dlg.m_Debug = globals->get_net_snmp_debug();
    dlg.m_MIBS = globals->get_mibs();
    dlg.m_MIBDIR = globals->get_mibdir();

    if( dlg.DoModal() != IDOK ) {
        return;
    }

    globals->set_net_snmp_debug( dlg.m_Debug );
    globals->set_mibs( dlg.m_MIBS );
    globals->set_mibdir( dlg.m_MIBDIR );
}


void CMainFrame::OnFilePassphrase() 
{
    PassPhrase dlg;

    if( dlg.DoModal() != IDOK ) {
        return;
    }

    poison_cstring( m_ConnHistPassword );
    m_ConnHistPassword = dlg.m_PassPhrase;

    /* kill the passphrase string in the dialog before it gets freed */
    poison_cstring( dlg.m_PassPhrase );
}


void CMainFrame::OnFileForgetPassphrase() 
{
    if( m_ConnHistPassword.GetLength() > 0 ) {
        poison_cstring( m_ConnHistPassword );
        m_ConnHistPassword.Empty();

    }
    MessageBox( _T("Passphrase Forgotten"), MB_OK );
}


void CMainFrame::OnTestConnHist() 
{
    ConnectHistory connhist;
    CString pwd;
    CString reg_section, reg_entry;
    ConnectHistoryEntry *entry;

    pwd = _T( "This is a test" );

    connhist.set_password( pwd );

    entry = new ConnectHistoryEntry;
    entry->name = "Entry 1";
    entry->agent_address = "172.31.1.42";
    entry->get_community = "public";
    entry->set_community = "private";
    entry->snmp_version = SNMP_VERSION_2c;
    connhist.Push( entry );
    entry = NULL;

    entry = new ConnectHistoryEntry;
    entry->name = "Entry 2";
    entry->agent_address = "172.31.1.43";
    entry->get_community = "foo";
    entry->set_community = "bar";
    entry->snmp_version = SNMP_VERSION_1;
    connhist.Push( entry );
    entry = NULL;    

    reg_section = _T("test_history");
    reg_entry = _T("test_connection");
    connhist.Save( reg_section, reg_entry );

    // now read it back
    ConnectHistory newhist;
    newhist.set_password( pwd );
    newhist.Load( reg_section, reg_entry );
}
