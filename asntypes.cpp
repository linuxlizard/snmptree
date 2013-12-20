#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <string>
#include <iostream>
#include <sstream>

#include "snmpcore.h"
#include "asntypes.h"

#ifndef MIN
#define MIN(x,y) ( (x)<(y)?x:y )
#endif

/* I'm using a lot of the NetSNMP library's ability to print into realloc-able
 * buffers. This function gets all the stuff necessary as a starting point.
 */
static void alloc_netsnmp_buffer( u_char **buf, size_t *buf_len, size_t *out_len )
{
    *buf_len = 64;
    *out_len = 0;

    // buf could potentially be passed to realloc() so use C-style malloc()
    // and not C++ 'new'.
    *buf = (u_char *)calloc( *buf_len, sizeof(u_char) );
    if( *buf == NULL ) {
        snmp_log( LOG_ERR, "out of memory" );
    }
}

namespace ASN {

    Value::Value() 
    {
        // should never see this
        type_name = "ASN Value";
    }

    /**************************************************************************/

    IpAddress::IpAddress()
    {
        type_name = "IpAddress";
        asn_type = ASN_IPADDRESS;
        pretty_value_string = "0.0.0.0";
        value_string = "0.0.0.0";
        value = 0;
    }

    IpAddress::IpAddress( const IpAddress& rhs ) 
    {
        type_name = "IpAddress";
        asn_type = ASN_IPADDRESS;

        value = rhs.value;
        pretty_value_string = rhs.pretty_value_string;
        value_string = rhs.value_string;
    }

    int IpAddress::parse( std::string& str_value )
    {
        int retcode;
        u_long new_ip;

        retcode = ipstr( str_value.c_str(), &new_ip );
        if( retcode != 0 ) {
            return ParseError;
        }

        this->value = new_ip;

        struct in_addr addr;
        addr.s_addr = new_ip;
        this->value_string = inet_ntoa( addr );

        return NoError;
    }

    int IpAddress::set_data( netsnmp_vardata val, size_t val_len )
    {
        // looking at sprint_realloc_ipaddress(), an IP address is in val.string
        assert( val.string != NULL );
        assert( val_len == 4 ); 

        std::ostringstream tmp;
        tmp << val.string[0] << '.' << val.string[1] << '.' << val.string[2] <<
            '.' << val.string[3];
        value_string = tmp.str();
#if 0
        value_string.Format( "%d.%d.%d.%d", val.string[0], val.string[1], 
            val.string[2], val.string[3] );
#endif

        return NoError;
    }


    /**************************************************************************/

    INTEGERValue::INTEGERValue()
    {
        type_name = "INTEGER";
        asn_type = ASN_INTEGER;
        value_string = "0";
        value = 0;

        // should only see value_string in plain INTEGERs
        pretty_value_string = "Shouldn't see me.";
    }

    INTEGERValue::INTEGERValue( const INTEGERValue& rhs )
    {
        type_name = "INTEGER";
        asn_type = ASN_INTEGER;

        value = rhs.value;
        value_string = rhs.value_string;
        pretty_value_string = rhs.pretty_value_string;
    }

    int INTEGERValue::parse( std::string& str_value ) 
    {
        u_long new_int; 
        char *errptr;

        new_int = strtol( str_value.c_str(), &errptr, 10 );
        if( *errptr ) {
            return ParseError;
        }

        value = new_int;

        // convert value to a nice value with extra formatting 
        pretty_print();

        // now make a nice simple formatted value 
        std::ostringstream tmp;
        tmp << value;
        value_string = tmp.str();
#if 0
        value_string.Format( "%ld", value );
#endif

        return NoError;
    }

    int INTEGERValue::set_data( netsnmp_vardata val, size_t val_len )
    {
        assert( val.integer != NULL );

        value = *val.integer;

        // convert value to a nice value with extra formatting 
        pretty_print();

        // now make a nice simple formatted value 
        std::ostringstream tmp;
        tmp << value;
        value_string = tmp.str();
#if 0
        value_string.Format( "%ld", value );
#endif

        return NoError;
    }

    /**************************************************************************/

    Counter::Counter() 
    {
        type_name = "Counter";
        asn_type = ASN_COUNTER;
        value_string = "0";
        value = 0;
    }

    Counter::Counter( const Counter& rhs ) 
    {
        type_name = "Counter";
        asn_type = ASN_COUNTER;

        value = rhs.value;
        value_string = rhs.value_string;
        pretty_value_string = rhs.pretty_value_string;
    }

    /**************************************************************************/

    Gauge::Gauge() 
    {
        type_name = "Gauge";
        asn_type = ASN_GAUGE;
        value_string = "0";
        value = 0;
    }

    Gauge::Gauge( const Gauge& rhs ) 
    {
        type_name = "Gauge";
        asn_type = ASN_GAUGE;
        value = rhs.value;

        value_string = rhs.value_string;
        pretty_value_string = rhs.pretty_value_string;
    }

    /**************************************************************************/

    TimeTicks::TimeTicks() 
    {
        type_name = "TimeTicks";
        asn_type = ASN_TIMETICKS;
        value = 0;
        value_string = "0";
        pretty_value_string = "0";
    }

    TimeTicks::TimeTicks( const TimeTicks& rhs ) 
    {
        type_name = "TimeTicks";
        asn_type = ASN_TIMETICKS;
        value = rhs.value;

        value_string = rhs.value_string;
        pretty_value_string = rhs.pretty_value_string;
    }

    int TimeTicks::pretty_print( void )
    {
        char buf[60];

        /* FIXME - NetSNMP's uptimeString() has comment saying buf "must be
         * at least 40 bytes" which I find a bit scary. uptimeString() doesn't
         * use snprintf() but sprintf() which is unsafe. On the other hand,
         * they do know exactly how big the output will be (integers with fixed
         * widths).
         *
         * Eh. Whatever.
         */

        uptime_string( this->value, buf );

        pretty_value_string = buf;

        return NoError;
    }

    /**************************************************************************/

    OctetString::OctetString() 
    {
        type_name = "Octet String";
        asn_type = ASN_OCTET_STR;
        
        value = NULL;
        value_len = 0;

        memset( objid, 0, sizeof(objid) );
        objidlen = 0;
    }

    OctetString::OctetString( const OctetString& rhs ) 
    {
        type_name = "Octet String";
        asn_type = ASN_OCTET_STR;

        // since the copy constructor can only be called from Clone(), value
        // should always be empty by the time we get here
        value = NULL;
        value_len = 0;

        memset( objid, 0, sizeof(objid) );
        objidlen = 0;

        Copy( rhs );
    }

    void OctetString::Copy( const OctetString& octetstr )
    {
        if( this->value ) {
            delete[] value;
            value = NULL;
            value_len = 0;
        }
        this->value = new u_char[ octetstr.value_len ];
        // FIXME - throw some sort of error if memory allocation fails
        assert( this->value != NULL );

        memcpy( this->value, octetstr.value, octetstr.value_len * sizeof(u_char) );
        this->value_len = octetstr.value_len;

        this->objidlen = octetstr.objidlen;
        memcpy( this->objid, octetstr.objid, this->objidlen * sizeof(oid) );

        pretty_value_string = octetstr.pretty_value_string;
        value_string = octetstr.value_string;
    }

    OctetString::~OctetString()
    {
        if( value ) {
            delete [] value;
            value = NULL;
            value_len = 0;
        }
    }

    int OctetString::parse( std::string& src ) 
    {
        return set_data( (const u_char *)src.c_str(), src.length() );
    }

    int OctetString::parse( const char * src ) 
    {
        return set_data( (const u_char *)src, strlen(src) );
    }

    int OctetString::copy_escaped_string( u_char *dst, size_t& dstsize, 
                                          const u_char *src, size_t srclen )
    {
        int retcode;

        retcode = NoError;

        // handle fancy formating (\n, \r, \t, \x00 hex digits, etc)
        size_t i, dstlen;
        dstlen = 0;

        for( i=0 ; i<srclen ; i++ ) {
            assert( dstlen <= dstsize );

            // handle standard normal character
            if( src[i] != '\\' ) {
                dst[dstlen] = src[i];
                dstlen++;
                continue;
            }

            // start parsing \ escaped character

            // ignored a lone trailing '\'
            if( i+1 >= srclen ) {
                break;
            }

            // parse the escaped character
            i++;

            switch( src[i] ) {
                case 'n' :
                    dst[dstlen++] = 0x0a;
                    break;

                case 'r' :
                    dst[dstlen++] = 0x0d;
                    break;

                case 't' :
                    dst[dstlen++] = 0x09;
                    break;

                case '\\' :
                    dst[dstlen++] = '\\';
                    break;

                case 'x' :
                    // do stuff like \x00 -> NULL 
                    char hex[2];

                    // possible to have a broken \x00 at the end of a
                    // string
                    if( i+2 >= srclen ) {
                        i += 2;
                        DEBUGMSGTL( ("snmptree", "hex string short; should be %d have %d",
                                    srclen, i ) );
                        retcode = ParseError;
                        break;
                    }

                    hex[0] = src[i+1];
                    hex[1] = src[i+2];

                    if( !isxdigit( hex[0] ) || !isxdigit( hex[1] ) ) {
                        DEBUGMSGTL( ("snmptree", "bad hex chars %c%c",
                                    hex[0], hex[1] ) );
                        retcode = ParseError;
                        dst[dstlen] = ' ';
                    }
                    else {
                        dst[dstlen] = HEX2VAL( hex[0] ) << 4;
                        dst[dstlen] += HEX2VAL( hex[1] );
                    }

                    dstlen++;
                    i += 2;
                    break;

                default :
                    DEBUGMSGTL( ("snmptree", "unknown escape char %c", src[i]) );
                    dst[dstlen++] = ' ';
                    break;
            }
        }

        // pass back the unescaped string size
        dstsize = dstlen;
        return retcode;
    }

    int OctetString::set_data( const u_char *src, size_t srclen )
    {
        int retcode;

        // get rid of previous values
        if( value ) {
            delete [] value;
            value = NULL;
            this->value_len = 0;
        }
        value_string.clear();
        pretty_value_string.clear();

        // start parsing
        retcode = NoError;

        if( srclen > 0 ) {

            assert( src != NULL );

            value = new u_char[ srclen ];
            if( value == NULL ) {
                snmp_log( LOG_ERR, "out of memory" );
                return OutOfMemory;
            }
                
            // size of buffer will be strlen but data could be shorter if
            // there are any escaped characters
            size_t dstlen;
            dstlen = srclen;
            retcode = copy_escaped_string( value, dstlen, src, srclen );

            this->value_len = dstlen;
        }

        // make a simple string; hex dump if non printable, simple printable
        // if, uh, printable.
        make_value_string();

        // 6-Feb-05; the NetSNMP print of an OctetString puts \n into a hex
        // dump which can confuse the Win32 display. Just use the brute force
        // hex dump for now.
        pretty_value_string = value_string; 
        // FIXME - come up with a good way to handle pretty OctetStrings
        // make a fancy string

//        retcode = pretty_print();
        // pretty_print() logs error 

        return retcode;
    }

    int OctetString::set_data( netsnmp_vardata val, size_t val_len )
    {
        return set_data( val.string, val_len );
    }

    void OctetString::make_value_string( void )
    {
        size_t i;

        // print a simple string to value_string 
        //  - hex dump if contains any nonprintable chars
        for( i=0 ; i<value_len ; i++ ) {
            if( !isprint( value[i] ) ) {
                break;
            }
        }

        char *buf;
        if( i==value_len ) {
            // it's all printable; make a simple copy 
#if 0
            buf = value_string.GetBufferSetLength( value_len+1 );
            memcpy( buf, value, value_len );
            value_string.ReleaseBuffer( value_len );            
#else
            value_string.reserve( value_len+1 );
            value_string = (char *)value;
#endif
        }
        else {
            // contains non-printable chars; make a hex dump

            binary_to_hex( value, value_len, &buf );
            // FIXME - NetSNMP binary_to_hex() doesn't check for memory alloc
            // failure in calloc()

            value_string = buf;
            SNMP_FREE( buf );
        }

    }

    int OctetString::set_objid( oid *new_objid, size_t new_objidlen )
    {
        assert( new_objidlen < MAX_OID_LEN );

        objidlen = MIN( new_objidlen, MAX_OID_LEN );
        memcpy( objid, new_objid, objidlen * sizeof(oid) );

        return NoError;
    }

    int OctetString::pretty_print( void )
    {
        netsnmp_variable_list tmpvar;

        // some sanity tests to make sure things stay sync'd
        if( objid == NULL ) {
            assert( objidlen == 0 );
        }
        // objidlen can be zero (zero length string)

        memset( &tmpvar, 0, sizeof(tmpvar) );

        // Use a call into NetSNMP to do the print; NetSNMP looks up the OID in
        // its tree to find any lurking display hints.
        tmpvar.type = ASN_OCTET_STR;
        tmpvar.val.string = value;
        tmpvar.val_len = value_len;
        tmpvar.name = objid;
        tmpvar.name_length = objidlen;

        int retcode;

        u_char *buf;
        size_t buf_len, out_len;

        alloc_netsnmp_buffer( &buf, &buf_len, &out_len );
        if( buf == NULL ) {
            // alloc_netsnmp_buffer() logs message 
            return OutOfMemory;
        }

        retcode = sprint_realloc_value( (u_char **)&buf, &buf_len, &out_len, 1,
                                        objid, objidlen, &tmpvar );
        if( retcode <= 0 ) {
            DEBUGMSGTL( ("snmptree", "sprint_realloc_objid failed") );
            SNMP_FREE( buf );
            return NetSNMPError;
        }

        pretty_value_string = (char *)buf;

        SNMP_FREE(buf);

        return NoError;
    }

    /**************************************************************************/

    const size_t ObjectID::objidmax = MAX_OID_LEN;

    ObjectID::ObjectID()
    {
        type_name = "ObjectID";
        asn_type = ASN_OBJECT_ID;
        objidlen = 0;
    }

    ObjectID::ObjectID( const ObjectID& rhs )
    {
        type_name = "ObjectID";
        asn_type = ASN_OBJECT_ID;

        // since our copy constructor can only be called from Clone() which
        // creates a brand new object, this field should always be unset when we
        // get here.
        objidlen = 0;

        Copy( rhs );
    }

#if 0
    ObjectID& ObjectID::operator=( const ObjectID& objectid ) 
    {
        Copy( objectid );
        return *this;
    }
#endif

    void ObjectID::Copy( const ObjectID& objectid )
    {
        this->value_string = objectid.value_string;
        this->pretty_value_string = objectid.pretty_value_string;

        if( objectid.objidlen > this->objidmax ) {
            DEBUGMSGTL( ("snmptree", "length too short: need %d have %d\n", 
                        objectid.objidlen, this->objidmax ) );
        }
        this->objidlen = MIN( objectid.objidlen, this->objidmax );
        memcpy( this->objid, objectid.objid, this->objidlen*sizeof(oid) );
    }

    int ObjectID::get_objectid( oid *objid, size_t *objidlen )
    {
        size_t i;
        size_t len;

        // make a copy of the local objectid

        if( *objidlen < this->objidmax ) {
            DEBUGMSGTL( ("snmptree", "length too short: need %d have %d\n", 
                        this->objidlen, *objidlen ) );
        }
        len = MIN( *objidlen, this->objidlen );

        for( i=0 ; i<len ; i++ ) {
            objid[i] = this->objid[i];
        }

        *objidlen = len;

        return NoError;
    }

    int ObjectID::parse( std::string& str_value )
    {
        int retcode;

        retcode = NoError;

        objidlen = MAX_OID_LEN;
        if( snmp_parse_oid( str_value.c_str(), objid, &objidlen ) == NULL ) {
            // FIXME - snmp_parse_oid() fails if the MIBs aren't loaded or 
            // it can't find the oid.
            objidlen = 0;
            DEBUGMSGTL( ("snmptree", "ObjectID::parse() failed" ) );
            return ParseError;
        }

//        retcode = set_data( objid, objidlen );
//        if( retcode != NoError ) {
//            DEBUGMSGTL( ("snmptree", "ObjectID::parse() failed" ) );
//            objidlen = 0;
//            return retcode;
//        }

        retcode = make_value_string();
        // make_value_string() logs error 

        return retcode ;
    }

    int ObjectID::set_data( netsnmp_vardata val, size_t val_len )
    {
        // val_len is the bytesize of the object; divide by size of 'oid'
        // to get oid count
        return set_data( val.objid, val_len/sizeof(oid) );
    }

    int ObjectID::set_data( oid *objid, size_t objidlen )
    {
        size_t i;
        int retcode;

        // start clean
        this->objidlen = 0;

        if( objidlen > this->objidmax ) {
            DEBUGMSGTL( ("snmptree", "length too short: need %d have %d\n", 
                        objidlen, this->objidmax ) );
        }

        this->objidlen = MIN( objidlen, this->objidmax );

        for( i=0 ; i<this->objidlen ; i++ ) {
            this->objid[i] = objid[i];   
        }

        // XXX - eventually split display_string and pretty_display_string
        // such that display_string contains just the numeric oids and
        // pretty_display_string contains the strings.
        //
        // For now, just use the nice display with the strings from the MIB.

        retcode = make_value_string();
        // make_value_string() logs error

        return retcode;
    }

    int ObjectID::pretty_print( void )
    {
        // make a printable string
        int retcode;
        u_char *buf;
        size_t buf_len, out_len;

        pretty_value_string = "";

        alloc_netsnmp_buffer( &buf, &buf_len, &out_len );
        if( buf==NULL ) {
            // alloc_netsnmp_buffer() logs error                      
            return OutOfMemory;
        }

        retcode = sprint_realloc_objid( (u_char **)&buf, &buf_len, &out_len, 1,
                                        objid, objidlen );
        if( retcode <= 0 ) {
//            DEBUGMSGTL( (_T("snmptree"), _T("sprint_realloc_objid failed")) );
            SNMP_FREE( buf );
            return NetSNMPError;
        }

        // FIXME - if the sprint_realloc_objid() failed, perhaps print into a 
        // simple %d.%d.%d.%d.%d format.

        pretty_value_string = (char *)buf;

        SNMP_FREE( buf );

        return NoError;
    }

    int ObjectID::oid_is_subtree( oid *objid, size_t objidlen )
    {
        if( objidlen > this->objidmax ) {
            return 1;
        }

        size_t i;

        for( i=0 ; i<objidlen ; i++ ) {

            if( objid[i] != this->objid[i] ) {
                return 1;
            }
        }

        return 0;
    }

    int ObjectID::make_value_string( void )
    {
        size_t i;
        char buf[32];

        if( objidlen == 0 ) {
            value_string = "";
            return NoError;
        }

        // simple 1.3.6.1.4.1 display string 
        sprintf( buf, "%d", this->objid[0] ); 
        value_string = buf;
        for( i=1 ; i<objidlen ; i++ ) {
            sprintf( buf, ".%d", this->objid[i] ); 
            value_string += buf;
        }

        // make a fancy display string, looking up objects in the OID table
        pretty_print();

        return NoError;
    }


    /**************************************************************************/

    Opaque::Opaque() : OctetString()
    {
        type_name = "Opaque";
        asn_type = ASN_OPAQUE;
    }

#if 0
    Opaque::Opaque( int specific_type ) : OctetString()
    {
        type_name = "Opaque";
        asn_type = specific_type;
    }
#endif
    Opaque::Opaque( const Opaque& rhs ) : OctetString( rhs )
    {
        // not yet implemented
        assert( 0 );
    }

    void Opaque::Copy( const Opaque& src )
    {
        /* TODO - implement Copy */
        assert( 0 );
    }

    int Opaque::parse( std::string& str_value )
    {
        /* TODO - parse input */
        return NoError;
    }

    /**************************************************************************/

    int new_opaque( int asn_type, Value *&new_value )
    {
        int retcode;

        /* Opaque covers a lot of stuff a regular type doesn't so need a 
         * fairly complex chunk of decode.
         *
         * see sprint_realloc_opaque() - mib.c
         */

        retcode = NoError;

        /* FIXME - eventually have multiple classes? */
        
        switch( asn_type ) {
            case ASN_OPAQUE :
                new_value = new Opaque;
                break;

            case ASN_OPAQUE_COUNTER64 :
            case ASN_OPAQUE_U64 :
            case ASN_OPAQUE_I64 :
                new_value = new Opaque;
                break;

            case ASN_OPAQUE_FLOAT :
                new_value = new Opaque;
                break;

            case ASN_OPAQUE_DOUBLE :
                new_value = new Opaque;
                break;
        }

        if( new_value == NULL ) {
            snmp_log( LOG_ERR, "out of memory" );
            return OutOfMemory;
        }

        return retcode;
    }

    int new_value( int asn_type, Value *&new_value )
    {
        int retcode;

        retcode = NoError;

        switch( asn_type ) {
            case ASN_INTEGER :
                new_value = new INTEGERValue;
                break;

            case ASN_TIMETICKS :
                new_value = new TimeTicks;
                break;

            case ASN_COUNTER :
                new_value = new Counter;
                break;

            case ASN_GAUGE :
                new_value = new Gauge;
                break;

            case ASN_IPADDRESS :
                new_value = new IpAddress;
                break;

            case ASN_OCTET_STR :
                new_value = new OctetString;
                break;

            case ASN_OBJECT_ID :
                new_value = new ObjectID;
                break;

            default:
                retcode = UnknownType;
                new_value = NULL;
                break;
        }

        /* check for weird stuff */
        if( retcode == UnknownType ) {
            if( new_opaque( asn_type, new_value ) == 0 ) {
                /* yup, it was one of multiple Opaque types */
                return NoError;
            }
        }

        if( new_value == NULL ) {
            snmp_log( LOG_ERR, "out of memory" );
            return OutOfMemory;
        }

        return retcode;
    }


    /**************************************************************************/

    Instance::Instance() 
    {
        value = NULL;
        objectid = NULL;
    }

    Instance::Instance( const Instance& rhs )
    {
        value = NULL;
        objectid = NULL;
        Copy( rhs );
    }

    void Instance::Copy( const Instance& rhs )
    {
//        int retcode;

        this->sanity_check();

        // we're taking on someone else's stuff so get rid of our own
        this->clean_self();

        // If the source is empty, we're done since we've already emptied 
        // ourselves.
        //
        // Otherwise make copies of RHS's stuff
        if( rhs.value != NULL ) {
#if 0
            retcode = new_value( inst.value->get_asn_type(), this->value );
            // FIXME - raise an error on memory allocation failure
            assert( retcode == NoError );
            assert( this->value != NULL );

            this->objectid = new ObjectID;

            *(this->value) = *inst.value;
            *(this->objectid) = *inst.objectid;
#else
            this->value = rhs.value->Clone();
            this->objectid = dynamic_cast<ObjectID *>(rhs.objectid->Clone());
#endif
        }
    }

    Instance::~Instance()
    {
        sanity_check();
        clean_self();
    }

    void Instance::clean_self( void )
    {
        sanity_check();

        // if we have anything, get rid of it
        if( value ) {
            delete value;
            value = NULL;
            delete objectid;
            objectid = NULL;
        }
    }

    void Instance::sanity_check( void )
    {
        if( value!=NULL ) {
            /* if we have a value, we better have an OID */
            assert( objectid != NULL );    
        }
        else {
            /* if we didn't have a value, we better NOT have an OID */
            assert( objectid == NULL );
        }
    }

    std::string Instance::get_display_string( void )
    {
        sanity_check();

        if( value ) {
            return value->get_display_string();
        }
        else {
            return empty;
        }
    }

    std::string Instance::get_pretty_display_string( void )
    {
        sanity_check();

        if( value ) {
            return value->get_pretty_display_string();
        }
        else {
            return empty;
        }
    }

    std::string Instance::get_type_name( void )
    {
        sanity_check();

        if( value ) {
            return value->get_type_name();
        }
        else {
            return empty;
        }
    }

    std::string Instance::get_object_name_string( void )
    {
        sanity_check();

        if( objectid ) {
            return objectid->get_pretty_display_string();
        }
        else {
            return empty;
        }
    }

    std::string Instance::get_objectid_string( void )
    {
        sanity_check();

        if( objectid ) {
            return objectid->get_display_string();
        }
        else {
            return empty;
        }
    }

    int Instance::get_asn_type( void )
    {
        sanity_check();

        if( value ) {
            return value->get_asn_type();
        }
        else {
            return 0;
        }
    }


    int Instance::oid_is_subtree( oid *objid, size_t objidlen )
    {
        sanity_check();

        if( objectid ) {
            return objectid->oid_is_subtree( objid, objidlen );
        }
        else {
            // nope, nothing there
            return 1;
        }
    }
    
    int Instance::parse( std::string& str_value )
    {
        sanity_check();

        if( value ) {
            return value->parse( str_value );
        }
        else {
            return InternalError;
        }
    }

    u_char * Instance::get_data_ptr( void )
    {
        sanity_check();

        if( value ) {
            return value->get_data_ptr();
        }
        else {
            return NULL;
        }
    }

    size_t Instance::get_data_len( void )
    {
        sanity_check();

        if( value ) {
            return value->get_data_len();
        }
        else {
            return InternalError;
        }
    }


    int Instance::get_objectid( oid *objid, size_t *objidlen )
    {
        sanity_check();

        if( objectid ) {
            return objectid->get_objectid( objid, objidlen );
        }
        else {
            return InternalError;
        }
    }

    int Instance::set_variable( const netsnmp_variable_list *var )
    {
        int retcode;

        sanity_check();

        clean_self();

        retcode = new_value( var->type, value );
        if( retcode != NoError ) {
            // new_value() logs error 
            return retcode;
        }

        objectid = new ObjectID;
        if( objectid == NULL ) {
            snmp_log( LOG_ERR, "out of memory" );
            delete value;
            value = NULL;
            return OutOfMemory;
        }

        /* This is a bit of a kludge; in order to properly pretty print Octet Strings,
         * the class needs to know the object ID to look up display hints, etc,
         * from the mib tree.
         */
        if( var->type == ASN_OCTET_STR ) {
            OctetString  *octsvalue;

            octsvalue = dynamic_cast<OctetString *>( value );

            retcode = octsvalue->set_objid( var->name, var->name_length );
            if( retcode != NoError ) {
                // set_objid() logs error
                goto failure;
            }
        }

        retcode = value->set_data( var->val, var->val_len );
        if( retcode != NoError ) {
            // set_data() logs error
            goto failure;
        }

        retcode = objectid->set_data( var->name, var->name_length );
        if( retcode != NoError ) {
            // set_data() logs error
            goto failure;
        }

        return NoError;

    failure:
        delete value;
        delete objectid;
        value = NULL;
        objectid = NULL;
        return retcode;
    }
}

#ifdef ASN_TEST

using namespace ASN;

// FIXME - should probably collapse these into separate classes but I'm in a
// hurry right now so, naturally I'll assume there's time to do it over later.

int ASNTest::integer_test()
{
    std::string value = "33";
    int retcode;

    INTEGERValue v1;
    retcode = v1.parse( value );
    assert( retcode == NoError );

    INTEGERValue *v3 = dynamic_cast<INTEGERValue*>(v1.Clone());

    assert( v1.get_asn_type() == ASN_INTEGER );
    assert( v3->get_asn_type() == ASN_INTEGER );

    std::string type_name = "INTEGER";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    assert( v1.get_display_string() == value );
    assert( v3->get_display_string() == value );

    assert( v1.get_pretty_display_string() == value );
    assert( v3->get_pretty_display_string() == value );

    assert( v1.get_value() == v3->get_value() );

    value = "should fail";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    delete v3;

    return 0;
}

int ASNTest::counter_test()
{
    std::string value = "33";
    int retcode;

    Counter v1;
    retcode = v1.parse( value );
    assert( retcode == NoError );

    Counter *v3 = dynamic_cast<Counter *>(v1.Clone());

    assert( v1.get_asn_type() == ASN_COUNTER );
    assert( v3->get_asn_type() == ASN_COUNTER );

    std::string type_name = "Counter";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    assert( v1.get_display_string() == value );
    assert( v3->get_display_string() == value );

    assert( v1.get_pretty_display_string() == value );
    assert( v3->get_pretty_display_string() == value );

    assert( v1.get_value() == v3->get_value() );

    value = "should fail";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    delete v3;

    return 0;
}

int ASNTest::gauge_test()
{
    std::string value = "33";
    int retcode;

    Gauge v1;
    retcode = v1.parse( value );
    assert( retcode == NoError );

    Gauge *v3 = dynamic_cast<Gauge*>(v1.Clone());

    assert( v1.get_asn_type() == ASN_GAUGE );
    assert( v3->get_asn_type() == ASN_GAUGE );

    std::string type_name = "Gauge";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    assert( v1.get_display_string() == value );
    assert( v3->get_display_string() == value );

    assert( v1.get_pretty_display_string() == value );
    assert( v3->get_pretty_display_string() == value );

    assert( v1.get_value() == v3->get_value() );

    value = "should fail";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    delete v3;

    return 0;
}

int ASNTest::timeticks_test()
{
    std::string value = "339087324";
    int retcode;

    TimeTicks v1;
    retcode = v1.parse( value );
    assert( retcode == NoError );

    std::string pretty_value = "39:5:54:33.24";

    TimeTicks *v3 = dynamic_cast<TimeTicks*>(v1.Clone());

    assert( v1.get_asn_type() == ASN_TIMETICKS );
    assert( v3->get_asn_type() == ASN_TIMETICKS );

    std::string type_name = "TimeTicks";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    assert( v1.get_display_string() == value );
    assert( v3->get_display_string() == value );

    assert( v1.get_pretty_display_string() == pretty_value );
    assert( v3->get_pretty_display_string() == pretty_value );

    assert( v1.get_value() == v3->get_value() );

    value = "should fail";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    delete v3;

    return 0;
}

int ASNTest::ipaddress_test()
{
    std::string value = "172.31.42.66";
    int retcode;

    IpAddress v1;
    retcode = v1.parse( value );
    assert( retcode == NoError );

    IpAddress *v3 = dynamic_cast<IpAddress*>(v1.Clone());

    assert( v1.get_asn_type() == ASN_IPADDRESS );
    assert( v3->get_asn_type() == ASN_IPADDRESS );

    std::string type_name = "IpAddress";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    assert( v1.get_display_string() == value );
    assert( v3->get_display_string() == value );

    assert( v1.get_pretty_display_string() == value );
    assert( v3->get_pretty_display_string() == value );

    assert( v1.get_value() == v3->get_value() );

    value = "should fail";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    value = "0";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    value = "0.0.0.0.0.0";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    value = "300.200.40.33";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    delete v3;

    return 0;
}

int ASNTest::objectid_test()
{
    std::string value = "1.3.6.1.2.1.1.1.0";
    std::string pretty_value = "sysDescr.0";
    int retcode;

    ObjectID v1;
    retcode = v1.parse( value );
    assert( retcode == NoError );

    ObjectID *v3 = dynamic_cast<ObjectID*>(v1.Clone());

    assert( v1.get_asn_type() == ASN_OBJECT_ID );
    assert( v3->get_asn_type() == ASN_OBJECT_ID );

    std::string type_name = "ObjectID";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    // FIXME - eventually need to diverge pretty_display_string and display_string
    assert( v1.get_display_string() == value );
    assert( v3->get_display_string() == value );

    assert( v1.get_pretty_display_string() == pretty_value );
    assert( v3->get_pretty_display_string() == pretty_value );

    value = "should fail";
    retcode = v1.parse( value );
    assert( retcode == ParseError );

    delete v3;

    return 0;
}

void ASNTest::octetstring_test_single( const u_char *srcdata, 
                const u_char *dstdata, size_t dstlen,
                std::string& test_result,
                std::string& pretty_test_result )
{
    int retcode;

    OctetString v1;
    retcode = v1.parse( (const char *)srcdata );
    assert( retcode == NoError );

    std::string pds = v1.get_pretty_display_string();
    std::string ds = v1.get_display_string();

    // test copy constructor
    OctetString *v3 = dynamic_cast<OctetString*>(v1.Clone());

    assert( v1.get_asn_type() == ASN_OCTET_STR );
    assert( v3->get_asn_type() == ASN_OCTET_STR );

    std::string type_name = "Octet String";
    assert( v1.get_type_name() == type_name );
    assert( v3->get_type_name() == type_name );

    assert( v1.get_data_len() == dstlen );
    assert( v3->get_data_len() == dstlen );

    assert( v1.get_pretty_display_string() == pretty_test_result );
    assert( v3->get_pretty_display_string() == pretty_test_result );

    assert( v1.get_display_string() == test_result );
    assert( v3->get_display_string() == test_result );

    // make sure the result buffers are exactly what we expect
    size_t i;
    u_char *buf;

    buf = v1.get_data_ptr();
    for( i=0 ; i<dstlen ; i++ ) {
        assert( buf[i] == dstdata[i] );
    }

    buf = v3->get_data_ptr();
    for( i=0 ; i<dstlen ; i++ ) {
        assert( buf[i] == dstdata[i] );
    }
    
    delete v3;
}

int ASNTest::octetstring_test()
{
    // note that these test strings have \\ which will be interpretted by the
    // compiler as a single \ so my parse() will re-interpret as an escaped
    // string.
    const u_char value1[] = "This is a test.";
    const u_char raw_result1[] = "This is a test.";

    const u_char value2[] = "This is a test\\n.";
    const u_char result2[] = "This is a test\n.";

    const u_char value3[] = "This is a\\x00 test\\n.";
    u_char raw_result3[] = "This is a? test\n."; 
    raw_result3[9] = 0;
    size_t len3 = 17; 
    std::string pretty_result3 = "5468697320697320610020746573740a2e";
    std::string result3 = "5468697320697320610020746573740a2e";

    octetstring_test_single( value3, raw_result3, len3, result3, pretty_result3 );

    const u_char value4[] = "\\r\\nThis is a\\x00 test\\n.";
    u_char raw_result4[] = "\r\nThis is a? test\n.";
    raw_result4[11] = 0;
    size_t len4 = 19;
    std::string pretty_result4 = "0d0a5468697320697320610020746573740a2e";
    std::string result4 = "0d0a5468697320697320610020746573740a2e";

    octetstring_test_single( value4, raw_result4, len4, result4, pretty_result4 );

    return 0;
}

int ASNTest::pointer_test()
{
    const u_char value1[] = "This is a test.";
    OctetString *octstr;

    octstr = new OctetString;
    assert( octstr!=NULL );
    octstr->parse( (const char *)value1 );    
    
    Value *value;
    value = octstr->Clone();

    OctetString *octstr2;
    octstr2 = dynamic_cast<OctetString *>(value);

    delete octstr;
    delete value;

    return 0;
}

int ASNTest::instance_test( void )
{
    /* TODO */
    return 0;
}

ASNTest::ASNTest()
{
    std::cout << "starting ASN tests" << std::endl;

    integer_test();
    counter_test();
    gauge_test();
    timeticks_test();

    ipaddress_test();

    objectid_test();

    octetstring_test();

    pointer_test();

    instance_test();
}
#endif

#ifdef ASN_STANDALONE
#include "globals.h"

int main( void )
{
    int retcode;

    std::cout << "hello, world!" << std::endl;

    retcode = init_globals();
    assert( retcode==0 );

    retcode = snmpcore_init();
    assert( retcode==0 );

    ASNTest();

    return 0;
}

#endif

