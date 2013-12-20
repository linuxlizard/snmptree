#include "stdafx.h"

struct ConnectHistoryEntry {
    CString name;
    CString agent_address;
    CString get_community;
    CString set_community;
    long snmp_version;
};

class ConnectHistory {
    public: 
        ConnectHistory();
        ~ConnectHistory();

        int Load( CString& reg_section, CString& reg_entry );
        int Save( CString& reg_section, CString& reg_entry );

        int GetCount( void ) { return m_history.GetCount(); };
        struct ConnectHistoryEntry * GetAt( int idx );

        int Push( struct ConnectHistoryEntry *conn_entry );

        void set_password( CString& password );
        void forget_password( void ) ;

    private:
        CList< ConnectHistoryEntry *, ConnectHistoryEntry *> m_history;
        int m_num_history_entries;
        int m_max_history_entries;

        CString m_password;
        BOOL m_password_set;

        int encrypt( unsigned char *inbuf, unsigned char **outbuf, int buflen);
        int decrypt( unsigned char *inbuf, unsigned char **outbuf, int buflen);
        int crypt( unsigned char *inbuf, unsigned char **outbuf, int buflen, int encrypt );

        int parse( unsigned char *buf, int buflen );
};

/* values returned by get/set load/save etc */
#define CONNHIST_SUCCESS            0
#define CONNHIST_ERR_NO_PASSWORD    -1
#define CONNHIST_ERR_REGISTRY_FAIL  -2
#define CONNHIST_ERR_NO_SUCH_ENTRY      -3
#define CONNHIST_ERR_OUT_OF_MEMORY      -4
#define CONNHIST_ERR_PARSE_FAIL     -5

void poison_cstring( CString& str );

