/* Most of snmpwalk_thread() lifted from net-snmp-5.1.1/apps/snmpwalk.c 
 * 
 * main() -> snmpwalk_thread()
 *      Changed the formatting. Got rid of stdout/stderr. snmp_parse_args()
 *      changed to thread_parse_args() to set up initial session.
 *
 */

/*
 * snmpwalk.c - send snmp GETNEXT requests to a network entity, walking a
 * subtree.
 *
 */
/**********************************************************************
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

static int thread_parse_args( struct snmpwalk_param *param, netsnmp_session *session )
{
    snmp_sess_init( session );

    session->community = (u_char *)cstring_strdup( param->get_community );
    if( session->community == NULL ) {
        snmp_log( LOG_ERR, "out of memory" );
        return SNMPCORE_OUTOFMEM;
    }
    session->community_len = strlen( (const char *)session->community );

    session->peername = cstring_strdup( param->ip_address );
    if( session->peername == NULL ) {
        snmp_log( LOG_ERR, "out of memory" );
        SNMP_FREE( session->community );
        return SNMPCORE_OUTOFMEM;
    }

    session->version = param->snmp_version;

    return 0;
}

DWORD WINAPI snmpwalk_thread( LPVOID lpThreadParameter )
{
    netsnmp_session session, *ss;
    netsnmp_pdu    *pdu, *response;
    netsnmp_variable_list *vars;
    oid             name[MAX_OID_LEN];
    size_t          name_length;
    int             count;
    int             running;
    int             status;
    int             exitval = 0;
    struct snmpwalk_param *param;
    CString *errstr;

    param = (struct snmpwalk_param *)lpThreadParameter;
    ASSERT( param != NULL );

    ss = NULL;
    pdu = NULL;
    response = NULL;

    errstr = new CString;
    if( errstr==NULL ) {
        snmp_log( LOG_ERR, "out of memory" );
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
     * get first object to start walk 
     */
    memmove(name, param->root, param->rootlen * sizeof(oid));
    name_length = param->rootlen;

    running = 1;

    while(running) {
        /*
         * create PDU for GETNEXT request and add object name to request 
         */
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        if( pdu==NULL ) {
            snmp_log( LOG_ERR, "out of memory" );
            *errstr = _T("out of memory");
            exitval = SNMPCORE_OUTOFMEM;
            running = 0;
            continue;
        }

        if( snmp_add_null_var(pdu, name, name_length) == NULL ) {
            /* looks like there's a few ways this can fail; out of memory or
             * a bad type
             */
            DEBUGMSGTL( ("snmpwalk","snmp_add_null_var() failed") );
            *errstr = _T("Internal Error adding NULL variable to request");
            exitval = SNMPCORE_GENERROR;
            running = 0;
            continue;
        }

        /* did user want us to stop? */
        if( snmpcore_get_stop_flag() ) {
            running = 0;
            continue;
        }

        /*
         * do the request 
         */
        status = snmp_synch_response(ss, pdu, &response);

        /* did user want us to stop? */
        if( snmpcore_get_stop_flag() ) {
            running = 0;
            continue;
        }

        if(status == STAT_SUCCESS) {
            if(response->errstat == SNMP_ERR_NOERROR) {
                /*
                 * check resulting variables 
                 */
                for(vars = response->variables; vars;
                   vars = vars->next_variable) {
                    if((vars->name_length < param->rootlen)
                       || (memcmp( param->root, vars->name, param->rootlen * sizeof(oid)) != 0)) {
                        /*
                         * not part of this subtree 
                         */
                        running = 0;
                        continue;
                    }
                    thread_print_variable( param->parent, vars->name, vars->name_length, vars);

                    if((vars->type != SNMP_ENDOFMIBVIEW) &&
                       (vars->type != SNMP_NOSUCHOBJECT) &&
                       (vars->type != SNMP_NOSUCHINSTANCE)) {
                        /*
                         * not an exception value 
                         */
                        if( snmp_oid_compare(name, name_length,
                                             vars->name,
                                             vars->name_length) >= 0) {
                            CString oidstr;

                            *errstr = _T("Error: OID not increasing: ");
                            cstring_print_objid( oidstr, name, name_length );
                            *errstr += oidstr;
                            *errstr += _T(" >= ");
                            cstring_print_objid( oidstr, vars->name, vars->name_length);
                            *errstr += oidstr;
                            *errstr += _T("\n");

                            snmp_log( LOG_ERR, *errstr );

                            running = 0;
                            exitval = SNMPCORE_SNMPERROR;
                        }
                        memmove((char *) name, (char *) vars->name,
                                vars->name_length * sizeof(oid));
                        name_length = vars->name_length;
                    }
                    else {
                        /*
                         * an exception value, so stop 
                         */
                        running = 0;
                    }
                }
            }
            else {
                /*
                 * error in response, print it 
                 */
                running = 0;
                if(response->errstat == SNMP_ERR_NOSUCHNAME) {
                    errstr->Format("End of MIB\n");
                }
                else {

                    errstr->Format( "Error in packet.\nReason: %s\n", snmp_errstring(response->errstat) );

                    if(response->errindex != 0) {

                        *errstr += "Failed object: ";

                        for(count = 1, vars = response->variables;
                           vars && count != response->errindex;
                           vars = vars->next_variable, count++) {
                            /*EMPTY*/;
                        }

                        if(vars) {
                            CString oidstr;
                            cstring_print_objid( oidstr, vars->name, vars->name_length);
                            *errstr += oidstr;
                        }
                        *errstr += "\n";
                    }

                    snmp_log( LOG_ERR, *errstr );

                    exitval = SNMPCORE_SNMPERROR;
                }
            }
        }
        else if(status == STAT_TIMEOUT) {
            errstr->Format("Timeout: No Response from %s\n", session.peername);
            running = 0;
            snmp_log( LOG_INFO, *errstr );
            exitval = SNMPCORE_TIMEOUT;
        }
        else {  
            /* status == STAT_ERROR */
            get_netsnmp_error( ss, errstr );
            running = 0;
            exitval = SNMPCORE_GENERROR;
        }

        if(response) {
            snmp_free_pdu(response);
            response = NULL;
        }
    } /* end while(running) */

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
    ::PostMessage( param->parent, MSG_SNMP_WALK_DONE, (WPARAM)errstr, 0 );
    delete param;
    param = NULL;

    return exitval;
}

