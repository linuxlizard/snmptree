// GetDataDialog.cpp : implementation file
//

#include "stdafx.h"
#include "snmptree.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "GetDataDialog.h"
#include "usermsg.h"
#include "snmpcore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetDataDialog dialog


CGetDataDialog::CGetDataDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGetDataDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetDataDialog)
	//}}AFX_DATA_INIT
}


void CGetDataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetDataDialog)
	DDX_Control(pDX, IDCANCEL, m_CancelButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetDataDialog, CDialog)
	//{{AFX_MSG_MAP(CGetDataDialog)
    ON_MESSAGE(MSG_ASYNC_SELECT, OnAsyncSelect)
    ON_MESSAGE(MSG_SNMP_RESULT, OnSNMPWalkResult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetDataDialog message handlers

void CGetDataDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

//BOOL CGetDataDialog::PreCreateWindow(CREATESTRUCT& cs) 
//{
//    int retcode;
//
//    // fire off the SNMP Get
//    retcode = snmpcore_async_get( m_get_session, m_hWnd, walkoid, walkoid_len );
//    /* FIXME - report error to user */
//    ASSERT( retcode == SNMPCORE_SUCCESS );
//	
//	return CDialog::PreCreateWindow(cs);
//}

LRESULT CGetDataDialog::OnAsyncSelect( WPARAM wparam, LPARAM lparam)
{
    int selerror, selevent;
    SOCKET s;
    int retcode;

    selerror = WSAGETSELECTERROR(lparam);
    selevent = WSAGETSELECTEVENT(lparam);
    // FIXME - check error

    s = (SOCKET)wparam;

    ASSERT( m_get_session != NULL );

    int fds = 0, block = 1;
    fd_set fdset;
    struct timeval timeout;

    FD_ZERO(&fdset);
    retcode = snmp_sess_select_info( m_get_session, &fds, &fdset, &timeout, &block);
    ASSERT( retcode > 0 );
    // FIXME - report error

    retcode = snmp_sess_read( m_get_session, &fdset );
    ASSERT( retcode == 0 );
    // FIXME -- report error

    return (LRESULT)0;
}

LRESULT CGetDataDialog::OnSNMPWalkResult( WPARAM wparam, LPARAM lparam)
{
    struct snmpcore_result *result;
    int retcode;
    BOOL bretcode;

    result = (struct snmpcore_result *)wparam;

    /* check to see if our walk should end */
//    if( result->oidlen < walkoid_len 
//       || (memcmp( walkoid, result->oid, walkoid_len*sizeof(oid)) != 0) ) {
//        /* Stop */
//    }
    if( netsnmp_oid_is_subtree( walkoid, walkoid_len, result->oid, result->oidlen ) ) {
        // pass a duplicate message on to our parent window for display
        bretcode = AfxGetMainWnd()->PostMessage( MSG_SNMP_RESULT, (WPARAM)result, 0 );
        ASSERT( bretcode );
        // shouldn't fail

        if( !stop ) {
            /* launch next GetNext */
            retcode = snmpcore_async_get( m_get_session, m_hWnd, result->oid, result->oidlen );
            /* FIXME - report error to user */
            ASSERT( retcode == SNMPCORE_SUCCESS );
        }
    }
    else {
        // stop.
        CDialog::OnOK();
    }

    return (LRESULT)1;
}

void CGetDataDialog::SetStartingOid( oid *startoid, size_t len )
{
    memcpy( walkoid, startoid, len*sizeof(oid) );
    walkoid_len = len;
}


BOOL CGetDataDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    stop = FALSE;

    // not sure the window handle is valid at this point (MFC ignorance)
    ASSERT( IsWindow( m_hWnd ) );

    ASSERT( m_GetCommunity.GetLength() > 0 );
    ASSERT( m_AgentAddress.GetLength() > 0 );

    int retcode;

    TRACE0( _T("Open session" ) );
    retcode = snmpcore_open_session( &m_get_session, m_GetCommunity, m_AgentAddress );
    ASSERT( retcode == SNMPCORE_SUCCESS );
    ASSERT( m_get_session != NULL );
    // FIXME - report error and return

    // fire off the SNMP Get
    retcode = snmpcore_async_get( m_get_session, m_hWnd, walkoid, walkoid_len );
    /* FIXME - report error to user */
    ASSERT( retcode == SNMPCORE_SUCCESS );
	
    m_CancelButton.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

