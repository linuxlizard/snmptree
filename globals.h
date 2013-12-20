#ifndef GLOBALS_H
#define GLOBALS_H

/* 6-Feb-05 ; I wanted a simple way to contain all the global variables
 * snmptree might contain as well as a way to keep track of who might be
 * getting/setting them.  I wanted a way to hit a breakpoint at one and only
 * one location if two chunks of code start fighting over the meaning of a
 * global variable.  
 *
 * This is pretty simple so far. InitInstance() creates the one (and only one)
 * instance of Globals. Once initialized, Globals is then responsible for
 * saving the new settings back into the registry.
 */
class Globals {
    public:
        Globals();
        ~Globals();

        void load( void );
        void save( void );

        bool get_net_snmp_debug( void ) { return net_snmp_debug; };
        void set_net_snmp_debug( bool val );

        std::string get_mibs( void ) { return MIBS; };
        void set_mibs( std::string& val );

        std::string get_mibdir( void ) { return MIBDIR; };
        void set_mibdir( std::string &val );

    private:
        bool net_snmp_debug;
        std::string MIBS;
        std::string MIBDIR;

        /* registry keys for my settings */
        std::string net_snmp_debug_key;
        std::string MIBS_key;
        std::string MIBDIR_key;

        /* registry section where preferences are stored */
        std::string prefs_section;
};

Globals *get_globals( void );

int init_globals( void );
void cleanup_globals( void );

#endif

