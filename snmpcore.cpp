#ifdef WIN32
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <string>

#include "snmpcore.h"
#include "asntypes.h"
#include "usermsg.h"
#include "globals.h"

static bool log_to_trace = true;

static netsnmp_log_handler *callback_logh, *file_logh;

char log_filename[FILENAME_MAX+1];
                
static bool stop_current_action;

bool snmpcore_get_stop_flag( void )
{
    return stop_current_action;
}

bool snmpcore_set_stop_flag( bool new_flag )
{
    bool old_flag;

    old_flag = stop_current_action;

    stop_current_action = new_flag;
    return old_flag;
}

int snmpcore_log_msg( int majorID, int minorID, void *serverarg, void *clientarg)
{
    struct snmp_log_message *slm;

    slm = (struct snmp_log_message *)serverarg;
//    TRACE0(slm->msg);
    return 0;
}

void snmpcore_session_log( std::string& s )
{
    int priority;

    priority = LOG_DEBUG;
//    priority = LOG_INFO;

    file_logh = netsnmp_register_loghandler(NETSNMP_LOGHANDLER_FILE, priority );
    assert( file_logh != NULL );

    cstring_strncpy( log_filename, s, FILENAME_MAX );
    file_logh->token = log_filename;
}

void snmpcore_stop_session_log( void )
{
    if( file_logh ) {
        netsnmp_remove_loghandler( file_logh );
        free( file_logh );
        file_logh = NULL;
    }
}

int snmpcore_init( void )
{
    int priority;
    int retcode;

    priority = LOG_DEBUG;
  //  priority = LOG_INFO;

    assert( get_globals() != NULL );

    /* enable debugging if requested */
    if( get_globals()->get_net_snmp_debug() ) {
        snmp_set_do_debugging( TRUE );
    }

    /* TODO enable a default log handler so we don't get stderr popping up (creates leaks) */

    /* log to Visual C++ Console */
    if( log_to_trace ) {
        callback_logh = netsnmp_register_loghandler( NETSNMP_LOGHANDLER_CALLBACK, priority );
        assert( callback_logh != NULL );

        netsnmp_register_callback( SNMP_CALLBACK_APPLICATION, SNMP_CALLBACK_LOGGING, 
            snmpcore_log_msg, NULL, priority );
        netsnmp_register_callback( SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_LOGGING, 
            snmpcore_log_msg, NULL, priority );
    }

    /* log to a file, too */
    if( 1 ) {
//    if( log_to_file ) {
        file_logh = netsnmp_register_loghandler(NETSNMP_LOGHANDLER_FILE, priority );
        assert( file_logh != NULL );
        file_logh->token = "c:\\snmptree.log";
    }

    std::string mibdir;
    const char *mibdir_cstr;
    mibdir = get_globals()->get_mibdir();
    mibdir_cstr = mibdir.c_str();
    if( mibdir.length() > 0 ) {
        DEBUGMSGTL( ("Set MIB dir to %s\n", mibdir_cstr ) );
        netsnmp_set_mib_directory( mibdir_cstr );
    }

    // get env vars for MIBS; add any registry settings to it
    std::string mibs_env;
    if( getenv( "MIBS" ) != NULL ) {
        mibs_env = getenv( "MIBS" );
    }
    
    
    std::string mibs;
    mibs = get_globals()->get_mibs();
    if( mibs.length() > 0 ) {

        // if no MIBS from environment, just set to registry setting
        if( mibs_env.length() == 0 ) {
            // NetSNMP wants a leading "+" on MIBS to append the value of MIBS
            // to the standard list of mibs. Since we don't want to replace the
            // list, add the "+".
            mibs.insert( 0, 1, '+' );

            const char *mibs_cstr = mibs.c_str();
            setenv( "MIBS", mibs_cstr, 1 );
        }
        else {
            // we have a MIBS from the environment; append our registry
            // settings to it
            mibs_env += ENV_SEPARATOR_CHAR;
            mibs_env += mibs;

            const char *mibs_env_cstr = mibs_env.c_str();
            setenv( "MIBS", mibs_env_cstr, 1 );
        }
    }

    /* disable printing of units on the front of variable values */
    retcode = netsnmp_ds_set_boolean( NETSNMP_DS_LIBRARY_ID,
                            NETSNMP_DS_LIB_QUICK_PRINT, 1 );
    assert( retcode == SNMPERR_SUCCESS );

    /* disable printing of MIB name prefix on instance names */
    retcode = netsnmp_ds_set_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT,
                                              NETSNMP_OID_OUTPUT_SUFFIX);
    assert( retcode == SNMPERR_SUCCESS );

#if 0
    netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "includeRequested",
                               NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_WALK_INCLUDE_REQUESTED);

    netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "printStatistics",
                               NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_WALK_PRINT_STATISTICS);

    netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "dontCheckOrdering",
                               NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_WALK_DONT_CHECK_LEXICOGRAPHIC);

    netsnmp_ds_register_config(ASN_BOOLEAN, "snmpwalk", "timeResults",
                               NETSNMP_DS_APPLICATION_ID,
                               NETSNMP_DS_WALK_TIME_RESULTS);
#endif

    // SOCK_STARTUP not necessary since WinSock already initalized by 
    //SOCK_STARTUP;

    /*
     * read in MIB database and initialize the snmp library
     */
    init_snmp( "snmptree" );

//    FILE *f;
//    f = fopen( "c:\\tmp\\out.dat", "w" );
//    print_mib( f );
//    fclose(f);

#ifdef ASN_TEST
    ASNTest *asn_test;
    asn_test = new ASNTest;
    delete asn_test;
#endif

    return SNMPCORE_SUCCESS;
}

int snmpcore_shutdown( void )
{
    int retcode;

    snmpcore_stop_session_log();

//    if( log_to_file ) {
//        retcode = netsnmp_remove_loghandler( file_logh );
//        ASSERT( retcode == 1 );
//    }

    if( log_to_trace ) {
        retcode = netsnmp_remove_loghandler( callback_logh );
        assert( retcode == 1 );
    }

//    if( log_to_file ) {
//        free( file_logh );
//    }

    if( log_to_trace ) {
        free( callback_logh );
    }

    snmp_shutdown( "snmptree" );

    return SNMPCORE_SUCCESS;
}

/**
 * Prints an oid to an MFC std::string.
 * Based on Net-SNMP fprint_objid() - mib.c
 *
 * @param cstr       The std::string to "print" to.
 * @param objid      The oid to print
 * @param objidlen   The length of oidid.
 */

void cstring_print_objid( std::string& str, const oid * objid, size_t objidlen)
{                               
    u_char         *buf = NULL;
    size_t          buf_len = 256, out_len = 0;
    int             buf_overflow = 0;

    if ((buf = (u_char *) calloc(buf_len, 1)) == NULL) {
        str = "[TRUNCATED]\n";
        return;
    } 
    else {
        netsnmp_sprint_realloc_objid_tree(&buf, &buf_len, &out_len, 1,
                                          &buf_overflow, objid, objidlen);
        if (buf_overflow) {
            str = (char *)buf;
            str += " [TRUNCATED]\n";
        } 
        else {
            str = (char *)buf;
            str += "\n";
        }
    }

    SNMP_FREE(buf);
}

char *cstring_strdup( std::string& s )
{
    const char *ptr;
    char *buf;

    /* +1 for NULL */
    ptr = s.c_str();
    if( ptr == NULL ) {
        return NULL;
    }

    buf = strdup( ptr );
    if( buf == NULL ) {
        /* will return NULL but we still have to release the std::string's buffer
         * before we bail.
         */
    }

    return buf;
}

void cstring_strncpy( char *dst, std::string& src, int dstmax )
{
    const char *ptr;

    /* +1 for NULL */
    ptr = src.c_str();
    if( ptr == NULL ) {
        return;
    }

    strncpy( dst, ptr, dstmax );
}

/* started with mib_to_asn_type() - mib.c */
const char *asn_type_to_str( u_char asn_type )
{
    switch( asn_type ) {
        case ASN_OBJECT_ID :
            return "OBJECT IDENTIFIER";
    
        case ASN_OCTET_STR :
            return "OCTET STRING";
    
        case ASN_IPADDRESS :
            return "IpAddress";
    
        case ASN_INTEGER :
            return "INTEGER";
    
        case ASN_INTEGER64 :
            return "Integer64";

        case ASN_COUNTER :
            return "Counter";
    
        case ASN_GAUGE :
            return "Gauge";

        case ASN_TIMETICKS :
            return "TimeTicks";
    
        case ASN_OPAQUE :
            return "Opaque";
    
        case ASN_NULL :
            return "NULL";
    
        case ASN_COUNTER64 :
            return "Counter64";
    
        case ASN_BIT_STR :
            return "BITS";
    }
    return "Unknown";
}

/* a slightly more picky wrapper around inet_addr() since WinSock doesn't 
 * support inet_ntoa() ...jerks
 */
int ipstr( const char *ipstr_in, u_long *ip )
{
//    int pos;
    const char *next;

    // make sure we have three '.'s
    next = strchr( ipstr_in, '.' );
    if( next == NULL ) {
        goto bad_ip;
    }
    next++;
    next = strchr( next, '.' );
    if( next == NULL ) {
        goto bad_ip;
    }
    next++;
    next = strchr( next, '.' );
    if( next == NULL ) {
        goto bad_ip;
    }

    *ip = inet_addr( ipstr_in );
    if( *ip == INADDR_NONE && strncmp( ipstr_in, "255.255.255.255", 13 )!=0  ) {
        goto bad_ip;
    }
    if( *ip == INADDR_ANY && strncmp( ipstr_in, "0.0.0.0", 7 )!=0 ) {
        goto bad_ip;
    }

    return 0;
bad_ip:
    *ip = 0;
    return -1;
}

