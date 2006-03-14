#include <stdio.h>

#include "comm.h"
#include "calls.h"

int main()
    {
    int ord;
    DapiConnection* conn = dapi_connect();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    if( !dapi_Init( conn ))
        {
        fprintf( stderr, "Initialization failed!\n" );
        return 2;
        }
    ord = dapi_ButtonOrder( conn );
    printf( "Order: %d (%s)\n", ord, ord == 0 ? "Failed" : ord == 1 ? "Ok/Cancel" : "Cancel/Ok" );
    dapi_close( conn );
    return 0;
    }
