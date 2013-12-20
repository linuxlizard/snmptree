#include "stdafx.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "util.h"

OIDString::OIDString()
{
    m_str.Empty();
    m_oid = NULL;
    m_oidlen = 0;
}

OIDString::OIDString( const OIDString& stringSrc )
{
    m_str.Empty();
    m_oid = NULL;
    m_oidlen = 0;

    if( stringSrc.m_oidlen == 0 ) {
        // empty oid; no need to do anything else
        return;
    }

    // using malloc() in case I have to interoperate with Net-SNMP (????)
    m_oid = (oid *)malloc( stringSrc.m_oidlen * sizeof(oid) );
    // FIXME - check return value; how do I handle an error in a constructor?

    m_oidlen = stringSrc.m_oidlen;
    memcpy( m_oid, stringSrc.m_oid, m_oidlen * sizeof(oid) );


    int len, maxlen;

    // make a viewable string
    maxlen = 256;
    char *buf = new char[maxlen+1];
    // FIXME - check return value; how do I handle an error in a constructor?

    do {
        len = snprint_objid( buf, maxlen, m_oid, m_oidlen );
        if( len < 0 ) {
            DEBUGMSGTL( ("snmptree/util", "%d too small; realloc to %d\n", maxlen, maxlen*2) );
            maxlen *= 2;

            delete[] buf;
            buf= new char[maxlen+1];
        }
    } while( len<0 && maxlen<1024 );

    m_str.ReleaseBuffer();
}

OIDString::OIDString( oid *oid, size_t oidlen )
{
    m_str.Empty();
    m_oid = NULL;
    m_oidlen = 0;


}

OIDString::~OIDString()
{
    if( m_oid != NULL ) {
        free( m_oid );
        m_oid = NULL;
        m_oidlen = 0;
    }
}

OIDString::operator LPCTSTR() const 
{
    return (LPCTSTR)m_str;
}

CString& OIDString::GetString( void )
{
    return m_str;
}

void OIDString::SetOID( oid *oid, size_t oidlen )
{

}

void OIDString::Empty( void )
{
    m_str.Empty();

    if( m_oid != NULL ) {
        ASSERT( m_oidlen != 0 );
        free( m_oid );
        m_oid = NULL;
        m_oidlen = 0;
    }
    ASSERT( m_oidlen == 0 );
}


