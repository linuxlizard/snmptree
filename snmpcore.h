#ifndef SNMPCORE_H
#define SNMPCORE_H

#define SNMPCORE_SUCCESS     0
#define SNMPCORE_GENERROR    1
#define SNMPCORE_OUTOFMEM    2
#define SNMPCORE_TIMEOUT     3
#define SNMPCORE_SNMPERROR   4
#define SNMPCORE_SETFAILED   5
                        
int snmpcore_init( void );
int snmpcore_shutdown( void );

bool snmpcore_get_stop_flag( void );
bool snmpcore_set_stop_flag( bool new_flag );

void snmpcore_session_log( std::string& s );
void snmpcore_stop_session_log( void );

void cstring_print_objid( std::string& str, const oid * objid, size_t objidlen);
char *cstring_strdup( std::string& s );
void cstring_strncpy( char *dst, std::string& src, int dstmax );

const char *asn_type_to_str( u_char asn_type );

int ipstr( const char *str, u_long *ip );

#endif

