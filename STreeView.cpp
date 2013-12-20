// STreeView.cpp : implementation file
//

#include "stdafx.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmptree.h"
#include "STreeView.h"
#include "usermsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// STreeView

IMPLEMENT_DYNCREATE(STreeView, CTreeView)

STreeView::STreeView()
{
    
}

STreeView::~STreeView()
{
}


BEGIN_MESSAGE_MAP(STreeView, CTreeView)
    //{{AFX_MSG_MAP(STreeView)
    ON_WM_CHAR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// STreeView drawing

void STreeView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// STreeView diagnostics

#ifdef _DEBUG
void STreeView::AssertValid() const
{
    CTreeView::AssertValid();
}

void STreeView::Dump(CDumpContext& dc) const
{
    CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// STreeView message handlers

void STreeView::OnInitialUpdate() 
{
    CTreeView::OnInitialUpdate();
}

BOOL STreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style |= TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS;

    return CTreeView::PreCreateWindow(cs);
}

int STreeView::GetSelectedOID( oid *sel_oid, size_t *sel_oidlen, size_t max_oidlen )
{
    HTREEITEM h1;
    CTreeCtrl& tree = GetTreeCtrl();
    oid tmpoid[MAX_OID_LEN];
    size_t tmpoid_len, i;
    DWORD value;

    h1 = tree.GetSelectedItem();
    if( h1 == NULL ) {
        /* nothing selected, don't do anything */
        TRACE0( _T("Nothing selected") );
        return 0;
    }

    /* walk up the tree, getting the oids as we go */
    tmpoid_len = 0;
    while( h1 != NULL  ) {
        value = tree.GetItemData( h1 );
        tmpoid[tmpoid_len++] = value;
        h1 = tree.GetParentItem( h1 );
        ASSERT( tmpoid_len < MAX_OID_LEN );
        if( tmpoid_len >= MAX_OID_LEN ) {
            TRACE0( _T("oid too big") );
            return 0;
        }
    }
    
    /* will it fit? */
    ASSERT( max_oidlen >= tmpoid_len );
    if( max_oidlen < tmpoid_len ) {
        TRACE0( _T("oid too big") );
        return 0;
    }

    /* now make another oid, the reverse of the first */
    tmpoid_len--; /* now the index of the highest used slot */
    for( i=0 ; i<=tmpoid_len ; i++ ) {
        sel_oid[i] = tmpoid[tmpoid_len-i];
    }
    *sel_oidlen = tmpoid_len+1; /* +1 to turn back into the count */

    return *sel_oidlen;
}

void STreeView::expand( HTREEITEM root, oid *oid_to_expand, int oidlen )
{
    HTREEITEM node;
    DWORD value;
    CTreeCtrl& tree = GetTreeCtrl();

    if( oidlen==0 ) {
        return;
    }

    node = root;
    while( node != NULL ) {
        value = tree.GetItemData( node );
        if( value==oid_to_expand[0] ) {
            tree.Expand( node, TVE_EXPAND );

            expand( tree.GetChildItem( node ), ++oid_to_expand, --oidlen );

            break;
        }
        
        node = tree.GetNextSiblingItem( node );
    }
}

int STreeView::expand_oid_path( const char *oid_str )
{
    int retcode;
    oid oid_to_expand[MAX_OID_LEN];
    size_t oidlen;

    memset( oid_to_expand, 0, sizeof(oid_to_expand) );
    oidlen = MAX_OID_LEN;

    /* convert the oid into an integer form */
    retcode = read_objid( oid_str, oid_to_expand, &oidlen );
    if( retcode != 1 ) {
        /* bad OID */
        return -1;
    }

    expand( GetTreeCtrl().GetRootItem(), oid_to_expand, oidlen );

    return 0;
}


#if 0
void STreeView::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
    TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
    CTreeCtrl& tree = GetTreeCtrl();
    BOOL bretcode;
    oid *walk_oid;
    size_t walk_oidlen;

    HTREEITEM h1;
    h1 = tree.GetSelectedItem();

    if( h1 == NULL ) {
        /* nothing selected, don't do anything */
        *pResult = 0;
        return;
    }

    UINT state = tree.GetItemState( h1, TVIF_STATE );

    switch( pTVKeyDown->wVKey ) {
        case VK_SPACE :
            if( state & TVIS_EXPANDED ) {
                tree.Expand( h1, TVE_COLLAPSE );
            }
            else {
                tree.Expand( h1, TVE_EXPAND );
            }
            break;

        case VK_RETURN :
            CWnd *wnd = AfxGetMainWnd();

            walk_oid = (oid *)malloc( MAX_OID_LEN * sizeof(oid) );
            if( walk_oid == NULL ) {
                AfxMessageBox( "Out of memory" );
            }
            else {
                memset( walk_oid, 0, MAX_OID_LEN * sizeof(oid) );
                walk_oidlen = MAX_OID_LEN;

                if( GetSelectedOID( walk_oid, &walk_oidlen, MAX_OID_LEN ) > 0 ) {
                    bretcode = wnd->PostMessage( MSG_SNMP_WALK_START, (WPARAM)walk_oid, (LPARAM)walk_oidlen );
                    ASSERT( bretcode == TRUE );
                }
            }
            break;
    }
    
    *pResult = 1;
}
#endif

void STreeView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CTreeCtrl& tree = GetTreeCtrl();
    BOOL bretcode;
    oid *walk_oid;
    size_t walk_oidlen;
    UINT state;
    bool handled = false;
    CWnd *wnd;

    HTREEITEM h1;
    h1 = tree.GetSelectedItem();

    if( h1 != NULL ) {
        /* if nothing selected, don't do anything */

        state = tree.GetItemState( h1, TVIF_STATE );

        switch( nChar ) {
            /* tell the top level window to swap views of the splitter window */
            case VK_TAB :
                wnd = AfxGetMainWnd();
                bretcode = wnd->PostMessage( MSG_SWAP_VIEW, 0, 0 );
                ASSERT( bretcode == TRUE );
                handled = true;
                break;

            /* I hate using the mouse so add <space> to the chars that can 
             * expand/contract a subtree 
             */
            case VK_SPACE :
                if( state & TVIS_EXPANDED ) {
                    tree.Expand( h1, TVE_COLLAPSE );
                }
                else {
                    tree.Expand( h1, TVE_EXPAND );
                }
                handled = true;
                break;

            /* user hit <CR> so start a walk on the selected object */
            case VK_RETURN :
                wnd = AfxGetMainWnd();

                walk_oid = (oid *)malloc( MAX_OID_LEN * sizeof(oid) );
                if( walk_oid == NULL ) {
                    AfxMessageBox( "Out of memory" );
                }
                else {
                    memset( walk_oid, 0, MAX_OID_LEN * sizeof(oid) );
                    walk_oidlen = MAX_OID_LEN;

                    if( GetSelectedOID( walk_oid, &walk_oidlen, MAX_OID_LEN ) > 0 ) {
                        bretcode = wnd->PostMessage( MSG_SNMP_WALK_START, (WPARAM)walk_oid, (LPARAM)walk_oidlen );
                        ASSERT( bretcode == TRUE );
                    }
                }
                handled = true;
                break;
        }
    }

    if( !handled ) {
        CTreeView::OnChar(nChar, nRepCnt, nFlags);
    }
}

