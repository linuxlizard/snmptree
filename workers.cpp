#include "stdafx.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmpcore.h"
#include "usermsg.h"
#include "asntypes.h"
#include "workers.h"

void
thread_print_variable( HWND hWnd, const oid *objid,
                       size_t objidlen, const netsnmp_variable_list *vp )
{
    int retcode;
    BOOL bretcode;
    ASN::Instance *instance;

    ASSERT( vp != NULL );

    instance = new ASN::Instance;
    if( instance == NULL ) {
        snmp_log( LOG_ERR, "out of memory" );
        return;
    }

    retcode = instance->set_variable( vp );

    if( retcode != ASN::NoError ) {
        // FIXME -- handle error better
        snmp_log( LOG_ERR, "instance->set_variable() failed\n" );
        delete instance;
    }
    else {
        /* tell display part of snmptree about this new data */
        bretcode = ::PostMessage( hWnd, MSG_SNMP_RESULT, (WPARAM)instance, (LPARAM)0 );
        ASSERT( bretcode );
        if( !bretcode ) {
            delete instance;
        }
    }
}

void get_netsnmp_error( netsnmp_session *session, CString *errstr )
{
    int sys_errno, snmp_errno;
    int winsockerr;
    
    winsockerr = WSAGetLastError();

    /* Don't bother with error string to snmp_error() since it doesn't
     * return snmp_detail[]. errno is mostly useless with WinSock but get
     * it anyway.
     */
    snmp_error( session, &sys_errno, &snmp_errno, NULL );

    /* FIXME - snmp_api_errstring() is not threadsafe */
    *errstr = snmp_api_errstring( snmp_errno );
}

