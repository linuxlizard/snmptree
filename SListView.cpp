// SListView.cpp : implementation file
//

#include "stdafx.h"
#include "snmptree.h"
#include "SListView.h"

#include "usermsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SListView

IMPLEMENT_DYNCREATE(SListView, CListView)

SListView::SListView()
{
}

SListView::~SListView()
{
}


BEGIN_MESSAGE_MAP(SListView, CListView)
    //{{AFX_MSG_MAP(SListView)
    ON_WM_CHAR()
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SListView drawing

void SListView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// SListView diagnostics

#ifdef _DEBUG
void SListView::AssertValid() const
{
    CListView::AssertValid();
}

void SListView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SListView message handlers

BOOL SListView::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style |= LVS_REPORT;
//    cs.dwExStyle |= LVS_EX_FULLROWSELECT;
    
    return CListView::PreCreateWindow(cs);
}

void SListView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    bool handled;
    CWnd *wnd;
    BOOL bretcode;

    switch( nChar ) {
        /* tell the top level window to swap views of the splitter window */
        case VK_TAB :
            wnd = AfxGetMainWnd();
            bretcode = wnd->PostMessage( MSG_SWAP_VIEW, 0, 0 );
            ASSERT( bretcode == TRUE );
            handled = true;
            break;

        /* user hit <CR> so tell the main frame to start a set on the selected object(s) */
        case VK_RETURN :
            wnd = AfxGetMainWnd();
            bretcode = wnd->PostMessage( MSG_SNMP_SET_START, 0, 0 );
            ASSERT( bretcode == TRUE );
            handled = true;
            break;
    }

    if( !handled ) {
        CListView::OnChar(nChar, nRepCnt, nFlags);
    }
}


void SListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // user double clicked on something so tell the main fram to start a set on the selectec object(s) 
    CWnd *wnd = AfxGetMainWnd();
    BOOL bretcode;

    bretcode = wnd->PostMessage( MSG_SNMP_SET_START, 0, 0 );
    ASSERT( bretcode == TRUE );
    
    *pResult = 0;
}

int SListView::GetSelectedVar( CString &var_name, u_char &asn_type, CString &var_value )
{
    POSITION pos;
    CListCtrl& list = GetListCtrl();

    pos = list.GetFirstSelectedItemPosition();
    if( pos == NULL ) {
        /* Nothing selected */
        return 0;
    }

    int nItem = list.GetNextSelectedItem( pos );

    var_name = list.GetItemText( nItem, OID_COLUMN );
    var_value = list.GetItemText( nItem, VALUE_COLUMN );

    asn_type = (u_char)list.GetItemData( nItem );

    return 1;
}

