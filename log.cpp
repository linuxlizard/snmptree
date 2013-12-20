#include "stdafx.h"

#include "log.h"

#define LOG_MAX 64

static bool log_initialized;

CStringArray log;
int log_index;

static void log_init( void )
{
    log.SetSize( LOG_MAX );    
    log_index = 0;
}


void log_message( const CString &msg )
{
    if( ! log_initialized ) {
        log_init();
    }

    log.SetAt( log_index, msg );
    log_index++;
    if( log_index >= LOG_MAX ) {
        log_index = 0;
    }
}

void log_message( const char *msg )
{
    CString s( msg );

    log_message( s );
}


