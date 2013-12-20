/* globals.cpp  -- global variables for snmptree */

#ifdef WIN32
#define HAVE_SSIZE_T
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <string>

#include "asntypes.h"
#include "snmpcore.h"
#include "globals.h"

/* Hopefully the one and only one instance of this class. Yes, there are tricks I
 * can do in the class to make sure only one instance of Globals is ever
 * instantiated but I don't feel like looking them up right now.
 */
static Globals *globals;

Globals *get_globals( void )
{
    return globals;
}

int init_globals( void )
{
    if( globals != NULL ) {
        // already initialized
        return 0;
    }
    globals = new Globals;
    assert( globals != NULL );
    if( globals == NULL ) {
        return SNMPCORE_OUTOFMEM;
    }

    globals->load();

    return 0;
}

void cleanup_globals( void )
{
    if( globals != NULL ) {
        globals->save();

        delete globals;
        globals = NULL;
    }
}

/***********************************************************
 *  Global settings (preferences, etc) wrapped in a class to make more
 *  manageable.
 */

Globals::Globals()
{
    net_snmp_debug = FALSE;
    MIBS = "";
    MIBDIR="";

    /* lpszSection */
    prefs_section = "Preferences";

    /* lpszEntry paramters for my various settings */
    net_snmp_debug_key = "NetSnmpDebugFlag";
    MIBS_key = "MIBS";
    MIBDIR_key = "MIBDIR";
}

Globals::~Globals()
{
    /* nothing */
}

void Globals::set_net_snmp_debug( bool val )
{
    if( net_snmp_debug == val ) {
        return;
    }

    if( val ) {
        snmp_set_do_debugging( TRUE );
    }
    else {
        snmp_set_do_debugging( FALSE );
    }

    this->net_snmp_debug = val;    
}

void Globals::set_mibs( std::string& val )
{
    if( val == MIBS ) {
        return;
    }

    this->MIBS = val;
}

void Globals::set_mibdir( std::string& val )
{
    if( val == MIBDIR ) {
        return;
    }

    this->MIBDIR = val;
}

void Globals::load( void )
{
#if 0
    CWinApp *app;

    app = AfxGetApp();
    assert( app != NULL );

    net_snmp_debug = app->GetProfileInt( prefs_section, net_snmp_debug_key, FALSE );

    MIBS = app->GetProfileString( prefs_section, MIBS_key, _T("") );

    MIBDIR = app->GetProfileString( prefs_section, MIBDIR_key, _T("c:\\bin\\mibs") );
#endif

//    MIBDIR = wxT( "c:\\bin\\mibs" );
#ifdef WIN32
    MIBDIR =  "c:\\bin\\mibs";
#else
    MIBDIR =  "/usr/share/snmp/mibs";
#endif
}

void Globals::save( void )
{
#if 0
    bool retcode;
    CWinApp *app;

    app = AfxGetApp();
    assert( app != NULL );

    retcode = app->WriteProfileInt( prefs_section, net_snmp_debug_key, net_snmp_debug );
    assert( retcode==TRUE );

    retcode = app->WriteProfileString( prefs_section, MIBS_key, MIBS );
    assert( retcode==TRUE );

    retcode = app->WriteProfileString( prefs_section, MIBDIR_key, MIBDIR );
    assert( retcode==TRUE );
#endif
}

