#include "stdafx.h"

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

class OIDString {

private:
    oid *m_oid;
    size_t m_oidlen;

    CString m_str;


public:
    OIDString();
    OIDString( const OIDString& stringSrc );
    OIDString( oid *oid, size_t oidlen );
    ~OIDString();

    // return pointer to const string
    operator LPCTSTR() const;

    CString& GetString( void );
    void SetOID( oid *oid, size_t oidlen );

    void Empty( void );
};


