#include <stdio.h>

#include "comm.h"
#include "calls.h"

static void callback( DapiConnection* a1, int a2, int a3 )
    {
    (void) a1;
    (void) a2;
    (void) a3;
    fprintf( stderr, "Unexpected async reply, ignoring.\n" );
    }

int main()
    {
    int ord;
    DapiConnection* conn = dapi_connect();
    if( conn == NULL )
        {
        fprintf( stderr, "Cannot connect!\n" );
        return 1;
        }
    dapi_setSyncCallback( conn, callback );
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
