#include "stdafx.h"

#include "errstr.h"

LPTSTR DecodeError(int ErrorCode)
{
    static char Message[1024+1];  // +1 for NULL

    // If this program was multi-threaded, we'd want to use
    // FORMAT_MESSAGE_ALLOCATE_BUFFER instead of a static buffer here.
    // (And of course, free the buffer when we were done with it)

    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM 
//                   | FORMAT_MESSAGE_IGNORE_INSERTS 
                   | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
                    NULL, 
                    ErrorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    Message, 
                    1024, 
                    NULL );

    return Message;
}


