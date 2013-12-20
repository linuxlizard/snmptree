#ifndef WORKERS_H
#define WORKERS_H

#define MAX_DISPLAY_STR 512

struct snmpwalk_param {
    HWND parent;

    oid root[MAX_OID_LEN];
    size_t rootlen;

    long snmp_version;

    CString get_community;
    CString ip_address;
};

//struct snmpwalk_result {
//    oid oid[MAX_OID_LEN];
//    size_t oidlen;
//    u_char asn_type;
//
//    /* C-style NULL terminated strings, human readable data */
//    char value_str[MAX_DISPLAY_STR+1];
//    char oid_str[MAX_DISPLAY_STR+1];
//};

struct snmpset_param {
    HWND parent;

//    CArray<OidArray, int> names;
//    CArray<size_t, int> name_lengths;
//    CStringArray names;
//    CArray<u_char, int> types;
//    CStringArray values;

    CArray< ASN::Instance *, ASN::Instance *> values;

    long snmp_version;

    CString set_community;
    CString ip_address;
};


DWORD WINAPI snmpwalk_thread( LPVOID lpThreadParameter );
DWORD WINAPI snmpset_thread( LPVOID lpThreadParameter );

// these functions shared by the snmpXXX_thread()s 
void
thread_print_variable( HWND hWnd, const oid *objid,
                       size_t objidlen, const netsnmp_variable_list *vp );

void get_netsnmp_error( netsnmp_session *session, CString *errstr );

#endif

