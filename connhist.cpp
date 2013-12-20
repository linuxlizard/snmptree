#include "stdafx.h"
#include <afxtempl.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/snmpv3_api.h>

#include "blowfish/blowfish.h"

#include "connhist.h"

void poison_cstring( CString& str )
{
    char *pwd;
    int len;

    /* poison a password (I have no idea if this is the right thing to do or
     * not)
     */
    len = str.GetLength();
    pwd = str.GetBuffer( len );
    memset( pwd, 0xcc, len );
    str.ReleaseBuffer();
}

ConnectHistory::ConnectHistory()
{
    m_password_set = FALSE;

    m_max_history_entries = 32;
    m_num_history_entries = 0;
}

ConnectHistory::~ConnectHistory()
{
    ConnectHistoryEntry *entry;

    forget_password();

    /* poison all the community names */
    while( !m_history.IsEmpty() ) {
        entry = m_history.RemoveHead();
        poison_cstring( entry->get_community );
        poison_cstring( entry->set_community );

        /* XXX if I add any additional fields to ConnectHistoryEntry that can't
         * destroy themselves, free it here
         */

        /* now delete the entry itself */
        delete entry;
    }

    m_num_history_entries = 0;
    m_max_history_entries = 0;
}

void ConnectHistory::set_password( CString& password )
{
    if( m_password_set ) {
        forget_password();
    }
    m_password = password;
    m_password_set = TRUE;
}

void ConnectHistory::forget_password( void )
{
    poison_cstring( m_password );
    m_password.Empty();
    m_password_set = FALSE;
}

int ConnectHistory::Load( CString& reg_section, CString& reg_entry )
{
    int retcode;
    unsigned char *buf, *decryptbuf;
    unsigned int buflen;
    
    if( !m_password_set ) {
        return CONNHIST_ERR_NO_PASSWORD;
    }

    retcode = AfxGetApp()->GetProfileBinary( reg_section, reg_entry, &buf, &buflen);
    if( retcode == 0 ) {
        return CONNHIST_ERR_REGISTRY_FAIL;
    }

    retcode = decrypt( buf, &decryptbuf, buflen );
    if( retcode != 0 ) {
        TRACE0( _T("decrypt failed") );
        return retcode;
    }

    /* +1 for the NULL termination that decrypt() throws on there for me */
    retcode = parse( decryptbuf, buflen+1 );
    if( retcode != 0 ) {
        TRACE0( _T( "parse failed" ) );
        return retcode;
    }

    /* get rid of anything that might contain something we want to keep secure */
    memset( buf, 0, buflen );
    /* +1 for the NULL termination that decrypt() throws on there for me */
    memset( decryptbuf, 0, buflen+1 );

    delete[] buf;
    delete[] decryptbuf;

    return 0;
}

int ConnectHistory::Save( CString& reg_section, CString& reg_entry )
{
    struct ConnectHistoryEntry *entry;
    POSITION listpos;
    unsigned char *buf, *outbuf;
    int buflen;
    CString data, tmpstr;

    /* serialize the fields using \n between each field and \n\n between each
     * record 
     */

    listpos = m_history.GetHeadPosition();
//    entry = m_history.GetAt( listpos );
    while( listpos != NULL ) {

        entry = m_history.GetNext( listpos );

         /* TODO - find a faster way to do this */
        data += entry->name;
        data += "\n";
        data += entry->agent_address;
        data += "\n";
        data += entry->get_community;
        data += "\n";
        data += entry->set_community;
        data += "\n";
        tmpstr.Format( "%ld\n\n", entry->snmp_version );
        data += tmpstr;
 
//        entry = m_history.GetNext( listpos );
    }

    buflen = data.GetLength();
    buf = (unsigned char *)data.GetBuffer( buflen );

    encrypt( buf, &outbuf, buflen );

    data.ReleaseBuffer();
    poison_cstring( data );

    AfxGetApp()->WriteProfileBinary( reg_section, reg_entry, outbuf, buflen );

    memset( outbuf, 0, buflen );
    delete[] outbuf;

    return 0;
}

struct ConnectHistoryEntry * ConnectHistory::GetAt( int idx )
{
    return m_history.GetAt( m_history.FindIndex( idx ) );
}

int ConnectHistory::Push( struct ConnectHistoryEntry *entry )
{
    /* push all the existing entries down one */
    m_history.AddHead( entry );

    return 0;
}

int ConnectHistory::encrypt( unsigned char *inbuf, unsigned char **outbuf, int buflen)
{
    return crypt( inbuf, outbuf, buflen, BF_ENCRYPT );
}

int ConnectHistory::decrypt( unsigned char *inbuf, unsigned char **outbuf, int buflen)
{
    return crypt( inbuf, outbuf, buflen, BF_DECRYPT );
}

int ConnectHistory::crypt( unsigned char *inbuf, unsigned char **outbuf, 
                            int buflen, int encrypt )
{
    int retcode;
    unsigned char cbc_iv [8]={0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
    unsigned char iv[8];
    int n;
    u_char cbc_key[16];
    int pwdlen;
    u_char *pwd;

    /* +1 for NULL terminate */
    *outbuf = new unsigned char[buflen+1];
    if( *outbuf==NULL ) {
        TRACE0( _T("Out of Memory") );
        return CONNHIST_ERR_OUT_OF_MEMORY;
    }
    memset( *outbuf, 0, buflen+1 );

    pwdlen = m_password.GetLength();
    pwd = (u_char *)m_password.GetBuffer( pwdlen );

    /* hash the password using MD5 to get the key */
    memset( cbc_key, 0, sizeof(cbc_key) );
    retcode = MDchecksum( pwd, pwdlen, cbc_key, 16 );
    /* I'm not sure how MDchecksum() could fail so assert just in case */
    ASSERT( retcode==0 );

    m_password.ReleaseBuffer();

    /* now do the actual encrypt/decryption */
    BF_KEY key;
    BF_set_key(&key,16,cbc_key);
    memcpy(iv,cbc_iv,8);
    n=0;
//    BF_cfb64_encrypt( *outbuf, inbuf, buflen, &key, iv, &n, encrypt );
    BF_cfb64_encrypt( inbuf, *outbuf, buflen, &key, iv, &n, encrypt );

    return 0;
}

int ConnectHistory::parse( unsigned char *buf, int buflen )
{
    int retcode;
    struct ConnectHistoryEntry *entry;
    unsigned char *ptr, *endptr;
    CString cstr;

    /* My simple initial check for a potentially crap buffer (i.e., decryption
     * failed because of a bad key) is to check for \n\n as the last two chars
     * of the string.
     *
     * Note also I'm requiring a NULL terminated string. When decrypting, I
     * allocate one extra char for a NULL. 
     */
    if( buf[buflen-1]!=NULL || buf[buflen-2]!='\n' || buf[buflen-3]!='\n' ) {
        return CONNHIST_ERR_PARSE_FAIL;
    }

    entry = new ConnectHistoryEntry;
    if( entry==NULL ) {
        return CONNHIST_ERR_OUT_OF_MEMORY;
    }

    retcode = 0;
    ptr = buf;

    while( 1 ) {

        /* at this particular time we should have 5 fields in each record */
#define NAME_FIELD      0
#define ADDRESS_FIELD   1
#define GET_COMM_FIELD  2
#define SET_COMM_FIELD  3
#define VERSION_FIELD   4

        int i;
        for( i=0 ; i<4 ; i++ ) {
            endptr = (unsigned char *)strchr( (const char *)ptr, '\n' );
            if( endptr==NULL ) {
                /* this shouldn't have happened if we decrypted correctly */
                TRACE0( "missing field separator\n" );
                retcode = CONNHIST_ERR_PARSE_FAIL;
                break;
            }

            /* make \n into NULL so we can do simple string assignment */
            *endptr = NULL;

            cstr = ptr;
            
            /* store the string into our record */
            switch( i ) {
                case NAME_FIELD :
                    entry->name = ptr;
                    break;

                case ADDRESS_FIELD :
                    entry->agent_address = ptr;
                    break;

                case GET_COMM_FIELD :
                    entry->get_community = ptr;
                    break;

                case SET_COMM_FIELD :
                    entry->set_community = ptr;
                    break;

                case VERSION_FIELD :
                    entry->snmp_version = atoi( (const char *)ptr );

                    /* stupid check for parse problems */
                    if( entry->snmp_version==0 && ptr[0] != '0' ) {
                        TRACE0( "bad snmp version\n" );
                        retcode = CONNHIST_ERR_PARSE_FAIL;
                    }
                    break;
            }
            if( retcode != 0 ) {
                break;
            }

            /* skip past string and '\n' */
            ptr = endptr+1;
        }

#undef NAME_FIELD      
#undef ADDRESS_FIELD  
#undef GET_COMM_FIELD 
#undef SET_COMM_FIELD 
#undef VERSION_FIELD 

        /* we should have an additional \n between records */
        if( retcode==0 && *ptr != '\n' ) {
            TRACE0( "missing end of record marker\n" );
            retcode = CONNHIST_ERR_PARSE_FAIL;
        }
        /* skip end-of-record marker */
        ptr++;

        if( retcode != 0 ) {
            TRACE0( "parse failed\n" );
            break;
        }

        /* successful parse so store this record */
        Push( entry );
        entry = NULL;

        /* are we at the end of the string? -1 since we don't really care about
         * the NULL terminator 
         */
        if( ptr-buf >= (buflen-1) ) {
            break;
        }

        /* get a new record */
        entry = new ConnectHistoryEntry;        
        if( entry==NULL ) {
            TRACE0( "out of memory\n" );
            retcode = CONNHIST_ERR_OUT_OF_MEMORY;
            break;
        }
    }

    /* If we get here an our entry isn't null, make sure to free it. I set it
     * to NULL after calling Push() so if we have a non-null pointer here it's
     * because we have a parse fail or some other calamity and a partially
     * parsed record.
     */
    if( entry!=NULL ) {
        delete entry;
    }

    return retcode;
}

