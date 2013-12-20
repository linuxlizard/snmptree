#ifndef ASNTYPES_H
#define ASNTYPES_H

#include <sys/types.h>
#include <assert.h>

#include <string>

//typedef CArray<oid,int> COID;
                           
/* I love namespaces! Woo! */

namespace ASN {

    enum Errors {
        NoError=0,
        InternalError,
        OutOfMemory,
        UnknownType,
        ParseError,
        NetSNMPError,  // a NetSNMP API call failed for some reason
        /* more? */
    };


    class Value {
        public:
            Value();
            virtual ~Value() {};

            virtual Value * Clone() =0;

            int get_asn_type( void ) { return asn_type; };

            std::string get_display_string( void ) { return value_string; };

            /* Most of the time the "pretty" display string will simply be the
             * display string (nothing much exciting to display with an INTEGER
             * for example). In descendants, overload to return something nicer.
             */
            virtual std::string get_pretty_display_string( void ) { return value_string; };

            std::string get_type_name( void ) { return type_name; };

            /* Use parse to validate and convert a human entered string to 
             * the internal data value.
             */
            virtual int parse( std::string& str_value )=0;

            /* set internal data value based on NetSNMP type */
            virtual int set_data( netsnmp_vardata val, size_t val_len )=0;

            /* get a raw pointer to the NetSNMP formatted data we're wrapping */
            virtual u_char *get_data_ptr( void )=0;
            virtual size_t get_data_len( void )=0;

        protected:
            Value( const Value& value ) {};
//            Value& operator=( const Value& rhs ) 
//                { assert(0); Value *v = new Value; return *v;};

            /* pretty_value_string is the value nicely formatted with extra
             * information; e.g., TimeTicks as hours:minutes:seconds.hsecs
             * 3:12:55.35
             *
             * value_string is a simply formatted value with no extra 
             * information; e.g., TimeTicks as 13571113
             *
             * pretty_value_string is used to display the results of an 
             * operation. value_string is used in the Set dialog box.
             *
             * In most cases value_string is the only one used since there isn't
             * much more detail that can be added to some types.
             */
            std::string pretty_value_string;
            std::string value_string;

            /* type_name is a printable version of the type; e.g., "INTEGER",
             * "TimeTicks", etc.
             */
            std::string type_name;

            /* ASN_xxx type from asn1.h */
            int asn_type;

            /* Called by set_data() and parse() to create the "pretty_value_string" 
             * Override in functions that actually have something pretty to 
             * show.
             */
            virtual int pretty_print( void ) { return NoError; };
    };

    class IpAddress : public Value {
        public:
            IpAddress();
//            IpAddress( const IpAddress& ipval );
//            IpAddress& operator=( const IpAddress& ipval );
            virtual ~IpAddress() {};

            virtual Value * Clone() { return new IpAddress(*this); };

            virtual int parse( std::string& str_value);
            virtual int set_data( netsnmp_vardata val, size_t val_len );

            virtual u_char *get_data_ptr( void ) { return (u_char *)&value; };
            virtual size_t get_data_len( void ) { return 4; };

            u_long get_value( void ) { return value; };

        protected:
            IpAddress( const IpAddress& rhs );
            IpAddress& operator=( const IpAddress& rhs ) 
                { assert(0); IpAddress *ip = new IpAddress; return *ip; };

            u_long value;
    };


    /* Unfortunately, Visual C++ 6.0 doesn't differentiate between 
     * the #define INTEGER in Net-SNMP and my (originally named) INTEGER class.
     * Thus the dumb name.
     */
    class INTEGERValue : public Value {
        public:
            INTEGERValue();
//            INTEGERValue( const INTEGERValue& integerval );
//            INTEGERValue& operator=( const INTEGERValue& integerval );

            virtual ~INTEGERValue() {};

            virtual Value* Clone() { return new INTEGERValue(*this); };

            virtual int parse( std::string& str_value );

            virtual int set_data( netsnmp_vardata val, size_t val_len );

            virtual u_char *get_data_ptr( void ) { return (u_char *)&value; };
            virtual size_t get_data_len( void ) { return sizeof(value); };

            u_long get_value( void ) { return value; };

        protected:
            INTEGERValue( const INTEGERValue& rhs );
            INTEGERValue& operator=( const INTEGERValue& rhs ) 
                { assert(0); INTEGERValue *v=new INTEGERValue; return *v; };

            u_long value;
    };

    class Counter : public INTEGERValue {
        public:
            Counter();
            virtual ~Counter() {};

            virtual Value* Clone() { return new Counter(*this); };

        protected:
            Counter( const Counter& rhs );
            Counter& operator=( const Counter rhs ) 
                { assert(0); Counter *cnt =new Counter(); return *cnt;};
    };

    class Gauge : public INTEGERValue {
        public:
            Gauge();
            virtual ~Gauge() {};

            virtual Value* Clone() { return new Gauge(*this); };

        protected:
            Gauge( const Gauge& rhs );
            Gauge& operator=( const Gauge rhs ) 
                { assert(0); Gauge *g = new Gauge; return *g;};
    };

    class TimeTicks : public INTEGERValue {
        public:
            TimeTicks();
            virtual ~TimeTicks() {};

            virtual Value* Clone() { return new TimeTicks(*this); };

            virtual std::string get_pretty_display_string( void ) { return pretty_value_string; };

        protected:
            TimeTicks( const TimeTicks& rhs );
            TimeTicks& operator=( const TimeTicks& rhs ) 
                { assert(0); TimeTicks *t = new TimeTicks; return *t;};

            virtual int pretty_print( void );
    };


    class OctetString : public Value {
        public:
            OctetString();
//            OctetString( const OctetString& octetstr );
//            OctetString& operator=( const OctetString& octetstr );

            virtual ~OctetString();

            virtual Value* Clone() { return new OctetString(*this); };

            virtual int parse( std::string& src );
            virtual int parse( const char *src );
            virtual int set_data( const u_char *src, size_t strlen );
            virtual int set_data( netsnmp_vardata val, size_t val_len );

            /* The display of an OCTET STRING can depend on DISPLAY HINTs and 
             * other stuff from the mib file. In order to take advantage of the
             * extra formatting, need to know what the OBJECT ID of the OCTET
             * STRING. A bit of a kludge.
             */
            int set_objid( oid *objid, size_t objidlen );

            virtual std::string get_pretty_display_string( void ) { return pretty_value_string; };

            virtual void Copy( const OctetString& octetstr );

            virtual u_char *get_data_ptr( void ) { return value; };
            virtual size_t get_data_len( void ) { return value_len; };

        protected:
            OctetString( const OctetString& rhs );
            OctetString& operator=( const OctetString& rhs ) 
                { assert(0); OctetString *o = new OctetString; return *o; };

            u_char *value;
            size_t value_len;

            // Not using a ObjectID or COID because this has to be passed into
            // the NetSNMP library functions. The ObjectID and COID types 
            // are used inside snmptree itself.
            oid objid[MAX_OID_LEN];
            size_t objidlen;

            virtual int pretty_print( void );

        private:
            // if value contains unprintable chars, make value_string into a 
            // hex dump; otherwise, put value into a proper std::string
            void make_value_string( void );

            int copy_escaped_string( u_char *dst, size_t& dstlen, 
                    const u_char *src, size_t srclen );
    };


    class ObjectID : public Value {
        public:
            ObjectID();
            virtual ~ObjectID() {}

            virtual Value* Clone() { return new ObjectID(*this); };

            virtual int parse( std::string& str_value);
            virtual int set_data( netsnmp_vardata val, size_t val_len );

            virtual void Copy( const ObjectID& objectid );

            int set_data( oid *objid, size_t objidlen );
            int oid_is_subtree( oid *objid, size_t objidlen );
            int get_objectid( oid *objid, size_t *objidlen );

            virtual u_char *get_data_ptr( void ) { return (u_char *)objid; };
            virtual size_t get_data_len( void ) { return objidlen; };

            virtual std::string get_pretty_display_string( void ) { return pretty_value_string; };

        protected:
            ObjectID( const ObjectID& rhs );
            ObjectID& operator=( const ObjectID& rhs ) 
                { assert(0); ObjectID *o = new ObjectID; return *o; };

            oid objid[MAX_OID_LEN];
            size_t objidlen;
            static const size_t objidmax;

        private:
            virtual int pretty_print( void );
            int make_value_string( void );
    };


    /* FIXME - Opaque implementation is incomplete */
    class Opaque : public OctetString {
        public:
            Opaque();
//            Opaque( int specific_type );
            virtual ~Opaque() {};

            virtual Value* Clone() { return new Opaque(*this); };

            virtual void Copy( const Opaque& src );

            virtual int parse( std::string& str_value );

            virtual u_char *get_data_ptr( void ) { assert(0); return NULL; };
            virtual size_t get_data_len( void ) { assert(0); return 0; };

        protected:
            Opaque( const Opaque& rhs );
    };

    /* sort of "factory" function; decodes a type and returns a new parent
     * class pointer for the appropriate type.
     */
    int new_value( int asn_type, Value *&new_value );


    /* An instance of a variable. Contains the ObjectID of the variable and the
     * value of the instance.
     */
    class Instance {

        public: 
            Instance();
            ~Instance();

            Instance *Clone() { return new Instance(*this); };
    
            int set_variable( const netsnmp_variable_list *var );

            std::string get_display_string( void );
            std::string get_pretty_display_string( void );
            std::string get_object_name_string( void );
            std::string get_objectid_string( void );
            std::string get_type_name( void );
            int get_asn_type( void );

            int get_objectid( oid *objid, size_t *objidlen );
            int oid_is_subtree( oid *objid, size_t objidlen );

            void Copy( const Instance& inst );

            int parse( std::string& str_value );

            u_char *get_data_ptr( void );
            size_t get_data_len( void );

        protected:
            Instance( const Instance& rhs );
            Instance& operator=( const Instance& rhs ) 
                { assert(0); Instance *i = new Instance; return *i; };

            Value *value;
            ObjectID *objectid;

            std::string empty;

            void clean_self( void );
            void sanity_check( void );
    };

}

#define ASN_TEST

#ifdef ASN_TEST
class ASNTest {
    public:
        ASNTest();

        int integer_test();
        int counter_test();
        int gauge_test();
        int timeticks_test();

        int ipaddress_test();

        /* MIBs must be loaded for this to work */
        int objectid_test();

        void octetstring_test_single( const u_char *srcdata, 
                const u_char *dstdata, size_t dstlen,
                std::string& test_result,
                std::string& pretty_test_result );

        int octetstring_test();

        int pointer_test();

        int instance_test();
};
#endif

#endif

