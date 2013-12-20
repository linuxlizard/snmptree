/*
 * snmpset.c - send snmp SET requests to a network entity.
 *
 */
/***********************************************************************
    Copyright 1988, 1989, 1991, 1992 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#include "stdafx.h"
#include <afxtempl.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "snmpcore.h"
#include "usermsg.h"
#include "asntypes.h"
#include "workers.h"

static int thread_parse_args( struct snmpset_param *param, netsnmp_session *session )
{
    snmp_sess_init( session );

//    ASSERT( param->names.GetSize() == param->types.GetSize() );
//    ASSERT( param->types.GetSize() == param->values.GetSize() );

    session->community = (u_char *)cstring_strdup( param->set_community );
    if( session->community == NULL ) {
        DEBUGMSGTL( ("snmpset","out of memory") );
        return SNMPCORE_OUTOFMEM;
    }
    session->community_len = strlen( (const char *)session->community );

    session->peername = cstring_strdup( param->ip_address );
    if( session->peername == NULL ) {
        SNMP_FREE( session->community );
        DEBUGMSGTL( ("snmpset","out of memory") );
        return SNMPCORE_OUTOFMEM;
    }

    session->version = param->snmp_version;

    return 0;
}

DWORD WINAPI snmpset_thread( LPVOID lpThreadParameter )
{
    netsnmp_session session, *ss;
    netsnmp_pdu    *pdu, *response = NULL;
    netsnmp_variable_list *vars;
    int             count;
    int             current_name = 0;
    int             current_type = 0;
    int             current_value = 0;
    oid             name[MAX_OID_LEN];
    size_t          name_length;
    int             status;
    int             exitval = 0;
    struct snmpset_param *param;
    CString *errstr;
    int failures;

    param = (struct snmpset_param *)lpThreadParameter;
    ASSERT( param != NULL );
    ASSERT( AfxIsValidAddress( param, sizeof(snmpset_param), TRUE ) );


    ss = NULL;
    pdu = NULL;
    response = NULL;
    failures = 0;

    errstr = new CString;
    if( errstr==NULL ) {
        DEBUGMSGTL( ("snmpset","out of memory") );
        *errstr = _T("out of memory");
        exitval = SNMPCORE_OUTOFMEM;
        goto leave;
    }

    /*
     * get the common command line arguments 
     */
    if( thread_parse_args( param, &session ) != 0 ) {
        /* thread_parse_args() logs error */
        exitval = SNMPCORE_OUTOFMEM;
        goto leave;
    }

    /* start with a clean slate */
    WSASetLastError( 0 );


    /*
     * open an SNMP session 
     */
    ss = snmp_open(&session);
    if(ss == NULL) {
        /*
         * diagnose snmp_open errors with the input netsnmp_session pointer 
         */
        get_netsnmp_error( &session, errstr );

        exitval = SNMPCORE_GENERROR;
        goto leave;
    }

    /*
     * create PDU for SET request and add object names and values to request 
     */
    pdu = snmp_pdu_create(SNMP_MSG_SET);
    if( pdu==NULL ) {
        DEBUGMSGTL( ("snmpset","out of memory") );
        *errstr = _T("out of memory");
        exitval = SNMPCORE_OUTOFMEM;
        goto leave;
    }

    ASN::Instance *instance;
    netsnmp_variable_list *var;

    for( count=0 ; count < param->values.GetSize() ; count++ ) {

        instance = param->values[count];

        name_length = MAX_OID_LEN;
        instance->get_objectid( name, &name_length );

        // believe it or not it's been a couple weeks' work to get to the 
        // point I can call this damn function.
        var = snmp_pdu_add_variable( pdu, 
                                     name, 
                                     name_length, 
                                     instance->get_asn_type(),
                                     instance->get_data_ptr(), 
                                     instance->get_data_len() );

        // FIXME - need better error logging here
        if( var==NULL ) {
            failures++;
        }

        // once it's loaded into the variable we can get rid of the instance
        delete instance;
        param->values[count] = NULL;
        instance = NULL;
    }

    if( failures ) {
        DEBUGMSGTL( ("snmpset", "snmp_pdu_add_var() failed %d times\n", failures ));
        *errstr = _T("Bad Value. Internal Error.");
        exitval = SNMPCORE_GENERROR;

        // This is a little tricky. On an error condition I free memory at leave:
        // but the pdu is a special case. After snmp_sync_response() is called,
        // pdu becomes part of response or is freed by snmp_syn_response() if
        // the fct fails. I don't have any way of telling what happened once
        // snmp_sync_response() gets hold of it so if my parsing fails, free it
        // now instead of at leave.
        snmp_free_pdu( pdu );

        goto leave;
    }

    /*
     * do the request 
     */
    status = snmp_synch_response(ss, pdu, &response);
    if(status == STAT_SUCCESS) {
        if(response->errstat == SNMP_ERR_NOERROR) {
            for(vars = response->variables; vars; vars = vars->next_variable) {
                thread_print_variable( param->parent, vars->name, vars->name_length, vars);
            }
        }
        else {
            CString oidstr;

            errstr->Format( _T("Error in packet.\nReason: %s\n" ),
                            snmp_errstring(response->errstat));
            if(response->errindex != 0) {
                *errstr += _T("Failed object: ");

                for(count = 1, vars = response->variables;
                   vars && (count != response->errindex);
                   vars = vars->next_variable, count++);

                if(vars) {
                    cstring_print_objid( oidstr, vars->name, vars->name_length);
                    *errstr += oidstr;
                }

                *errstr += _T("\n");
            }

            snmp_log( LOG_ERR, *errstr );

            exitval = SNMPCORE_SETFAILED;
        }
    }
    else if(status == STAT_TIMEOUT) {
        errstr->Format("Timeout: No Response from %s\n", session.peername);
        snmp_log( LOG_INFO, *errstr );
        exitval = SNMPCORE_TIMEOUT;
    }
    else {  
        /* status == STAT_ERROR */
        get_netsnmp_error( &session, errstr );
        snmp_log( LOG_ERR, *errstr );
        exitval = SNMPCORE_GENERROR;
    }

leave:
    if( ss != NULL ) {
        snmp_close(ss);
    }
    if(response) {
        snmp_free_pdu(response);
    }

    SNMP_FREE( session.community );
    SNMP_FREE( session.peername );

    /* tell creater we're done 
     *
     * thanks to http://www.ai.mit.edu/people/irie/hacks/irie/notebook/mfctips.html
     */
    ::PostMessage( param->parent, MSG_SNMP_SET_DONE, (WPARAM)errstr, 0 );
    delete param;
    param = NULL;

    return exitval;
}

