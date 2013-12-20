#include <string>
#include <iostream>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void char_star( std::string& str ) 
{
    std::string internal;
    const char *ptr;
    u_char *buf;

    internal = str;
    
    std::cout << internal << internal.length() << std::endl;

    ptr = str.c_str();
    std::cout << ptr << std::endl;

    buf = (u_char *)malloc( 1024 );
    strcpy( (char *)buf, "This is a C string" );
    internal = (char *)buf;
    free( buf );

    std::cout << internal << " " << internal.length() << std::endl;

    std::string str2 = "Hello, world!";
    str2.insert( 0, 3, '+' );
    std::cout << str2 << std::endl;

    std::ostringstream os;
    os << str2 << "[TRUNCATED]" << 32 << "." << std::endl;
    std::cout << os.str() ;

    int nums[4] = { 192, 168, 0, 1 };
    std::ostringstream tmp;
    tmp << nums[0] << '.' << nums[1] << '.' << nums[2] <<
            '.' << nums[3];
    str2 = tmp.str();
    std::cout << str2 << std::endl;
}

int main( void )
{
    std::string foo;

    foo = "Hello, world";
    std::cout << foo << std::endl;

    std::string value = "1.3.6.1.2.1.1.1.0";
    char_star( value );

    return 0;
}

